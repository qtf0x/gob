/**
 * @file udf_vsi.h
 * @author Dr. Richard C. Gilmore
 * @author Dr. Jonathan A. Marts
 * @author Dr. Dan Worrall Jr.
 * @author Emmaleigh Hawkins
 * @author Vincent Marias
 * @author Will Meinert
 * @author Daniel Nau
 *
 * @date 06/03/2023
 * @copyright Copyright (c) 2023 Dr. Richard C. Gilmore
 * 
 * @brief Definitions for Ansys Fluent User-Defined Functions used to calculate 
 * volumetric strain increment over various sections and types of gob panels.
 */

#include "udf.h" // Fluent Macros, real typedef

#include "udf_vsi.h"

#include "fits.h" // for equation fits
#include "utils.h" // for clamp_positive, clamp_upper

/*******************************************************************************
 * TRONA MINE
 * SUB CRITIAL PANEL
*******************************************************************************/
void vsi_trona_stepped()
{
	// RP variables (set by user)

	real panel_half_width; // half the total panel width
	real panel_length; // total length of panel
	real panel_x_offset; // displacement to center of old panel
	real panel_y_offset; // displacement to recovery room of old panel
	real max_vsi; // maximum VSI to clamp output to

	// retrieve RP variables from Fluent (or set default values)

	if (RP_Variable_Exists_P("longwallgobs/panel_half_width"))
		panel_half_width = RP_Get_Real("longwallgobs/panel_half_width");
	else
		panel_half_width = 100;

	if (RP_Variable_Exists_P("longwallgobs/panel_length"))
		panel_length = RP_Get_Real("longwallgobs/panel_length");
	else
		panel_length = 3078.48;

	if (RP_Variable_Exists_P("longwallgobs/panel_x_offset"))
		panel_x_offset = RP_Get_Real("longwallgobs/panel_x_offset");
	else
		panel_x_offset = 0;

	if (RP_Variable_Exists_P("longwallgobs/panel_y_offset"))
		panel_y_offset = RP_Get_Real("longwallgobs/panel_y_offset");
	else
		panel_y_offset = 0;

	if (RP_Variable_Exists_P("longwallgobs/max_vsi"))
		max_vsi = RP_Get_Real("longwallgobs/max_vsi");
	else
		max_vsi = 0.22;

	// variables used in calculation

	real vsi = 0; // volumetric strain increment
	const real BLEND_RANGE_Y = 25; // (half) width of the blend zone

	const real BOX[6] = { 0, panel_half_width, 0, 300, panel_length - 400, panel_length };

	/* Scale each section of the model to the curve fits.
	UNITS in METERS

	    TG     RECOVERY ROOM / ACTIVE FACE     HG
	Fit 100                  0                 100
	1000 |---------------------------------------| 0
	     |0                  1|1                0|
	     |    3               |                  |
	     | Recovery           |                  |
	     | Gateroad           |                  |
	     | Exact-Size    Sub-Critial no-Expansion|
	     |1                  1|1                1|
	 600 |---------------------------------------| 300
	     |                    |                  |
	     |                    |                  |
	     |    2               |                  |
	     |  Center            |                  |
	     | Gateroad           |                  |
	     |                    |                  |
	     | Expansion          |                  |
	     | Equation           |                  |
	     |                    |                  |
	     |                    |                  |
	 190 |---------------------------------------| 810
	     |1                  1|1                1|
	     |    1               |                  |
	     | Startup            |                  |
	     | Gateroad           |                  |
	     |Exact-Size          |                  |
	     |0                  1|1                0|
	   0 |---------------------------------------| 1000
	   -152.5      -52.5               0        92.5      +152.5 My Panel

	box = [0 92.5 160 0 190 1010 1200] [0 100 0 190 600 1000]

	MIN = 144871.4 1/m^2	MAX=492170 1/m^2
	*/

	// Fluent data structures used in calculation

	// expect all zones/threads to be in a single domain
	Domain *d = Get_Domain(1);

	Thread *t; // current cell thread (mesh zone)
	cell_t c; // current cell index w/in the current thread

	// ND_ND is just 2 for 2D, 3 for 3D
	real loc[ND_ND]; // mesh cell location "vector"

	thread_loop_c(t, d) // loop over all threads in domain
	{
		begin_c_loop(c, t) // loop over all cells in thread
		{
			// get mesh cell location
			C_CENTROID(loc, c, t);

			// center of panel is zero and mirrored
			real x_loc = fabs(loc[0] - panel_x_offset);

			// shift Fluent mesh to FLAC3D data zero point at startup room for equations
			real y_loc = panel_length + loc[1] - panel_y_offset;

			// limit vsi function to only within panel domain sizing
			if (x_loc > panel_half_width) {
				vsi = 0;
			} else {
				if (y_loc < 0) {
					vsi = 0;
				} else if (y_loc < BOX[3] - BLEND_RANGE_Y) {
					// normalize to equation
					x_loc = -(x_loc - BOX[1]) / BOX[1];
					y_loc = y_loc / BOX[5];

					vsi = sub_critical_trona_startup_room_corner(x_loc, y_loc);
				} else if (y_loc < BOX[3] + BLEND_RANGE_Y) {
					// normalize to equation
					x_loc = -(x_loc - BOX[1]) / BOX[1];
					y_loc = y_loc / BOX[5];

					// calculate fits for both zones
					const real FUN1 = sub_critical_trona_startup_room_corner(x_loc, y_loc);
					const real FUN2 = sub_critial_trona_mid_panel_gateroad(x_loc);

					// calculate blending factor
					const real BLEND_MIX = -(y_loc - BOX[3] - BLEND_RANGE_Y) / (2 * BLEND_RANGE_Y);

					// linearlly interpolate
					vsi = FUN1 * BLEND_MIX + FUN2 * (1 - BLEND_MIX);
				} else if (y_loc < (BOX[4] - BLEND_RANGE_Y - 20)) {
					// normalize to equation
					x_loc = -(x_loc - BOX[1]) / BOX[1];

					vsi = sub_critial_trona_mid_panel_gateroad(x_loc);
				} else if (y_loc < BOX[4] + BLEND_RANGE_Y + 20) {
					// normalize to equation
					const real X_LOC_1 = -(x_loc - BOX[1]) / BOX[1];
					const real X_LOC_2 = -(x_loc - BOX[1]) / (BOX[1] + 40) + 0.02;
					y_loc = -(y_loc - BOX[5]) / (BOX[5] - BOX[4]) + 0.012;

					// calculate fits for both zones
					const real FUN1 = sub_critial_trona_mid_panel_gateroad(X_LOC_1);
					const real FUN2 = sub_critical_trona_working_face_corner(X_LOC_2, y_loc);

					// calculate blending factor
					const real BLEND_MIX =
						-(y_loc - BOX[4] - BLEND_RANGE_Y - 20) / (2 * BLEND_RANGE_Y + 40);

					// linearlly interpolate
					vsi = FUN1 * BLEND_MIX + FUN2 * (1 - BLEND_MIX);
				} else if (y_loc < BOX[5]) {
					// normalize to equation
					x_loc = -(x_loc - BOX[1]) / (BOX[1] + 40) + 0.02;
					y_loc = -(y_loc - BOX[5]) / (BOX[5] - BOX[4]) + 0.012;

					vsi = sub_critical_trona_working_face_corner(x_loc, y_loc);
				} else {
					vsi = 0;
				}
			}

			// clamp and assign vsi to user-defined-memory location
			C_UDMI(c, t, 4) = clamp_upper(vsi, max_vsi);
		}
		end_c_loop(c, t)
	}
}

/*******************************************************************************
 * MINE C
 * SUPER CRITIAL PANEL
*******************************************************************************/
void vsi_mine_C_stepped()
{
	// RP variables (set by user)

	real panel_half_width; // half the total panel width
	real panel_length; // total length of panel
	real panel_x_offset; // displacement to center of old panel
	real panel_y_offset; // displacement to recovery room of old panel
	real max_vsi; // maximum VSI to clamp output to

	// retrieve RP variables from Fluent (or set default values)

	if (RP_Variable_Exists_P("longwallgobs/panel_half_width"))
		panel_half_width = RP_Get_Real("longwallgobs/panel_half_width");
	else
		panel_half_width = 151.4856;

	if (RP_Variable_Exists_P("longwallgobs/panel_length"))
		panel_length = RP_Get_Real("longwallgobs/panel_length");
	else
		panel_length = 1000;

	if (RP_Variable_Exists_P("longwallgobs/panel_x_offset"))
		panel_x_offset = RP_Get_Real("longwallgobs/panel_x_offset");
	else
		panel_x_offset = 0;

	if (RP_Variable_Exists_P("longwallgobs/panel_y_offset"))
		panel_y_offset = RP_Get_Real("longwallgobs/panel_y_offset");
	else
		panel_y_offset = 0;

	if (RP_Variable_Exists_P("longwallgobs/max_vsi"))
		max_vsi = RP_Get_Real("longwallgobs/max_vsi");
	else
		max_vsi = 0.2623;

	// variables used in calculation

	real vsi = 0; // volumetric strain increment
	const real BLEND_RANGE = 15;
	const real BLEND_RANGE_Y = 25; // (half) width of the blend zone

	const real BOX[7] = { 0, panel_half_width - 100, panel_half_width, 0, 190, panel_length - 300, panel_length };

	/* Scale each section of the model to the curve fits.
	UNITS in METERS

	    TG           RECOVERY ROOM / ACTIVE FACE           HG
	Fit 160        100             0            100        160
	1200 |--------------------------------------------------| 0
	     |0        1|                            |1        0|
	     |    5     |    6                       |          |
	     | Recovery |  Recovery                  |          |
	     | Gateroad |  Center                    |          |
	     |Exact-Size|   Super Critial Expansion  |          |
	     |1         |                            |         1|
	1010 |--------------------------------------------------| 190
	     |          |                            |          |
	     |          |                            |          |
	     |    3     |    4                       |          |
	     |  Center  | Center                     |          |
	     | Gateroad | Panel                      |          |
	     |          |                            |          |
	     | Expansion| Expansion                  |          |
	     | Equation | Equation                   |          |
	     |          |                            |          |
	     |          |                            |          |
	 190 |--------------------------------------------------| 1010
	     |1         |                            |         1|
	     |    1     |     2                      |          |
	     | Startup  |  Startup                   |          |
	     | Gateroad |  Center                    |          |
	     |Exact-Size|   Super Critial Expansion  |          |
	     |0        1|                            |1        0|
	   0 |--------------------------------------------------| 1200
	   -152.5      -52.5               0        92.5      +152.5  My Panel

	box = [0 92.5 160 0 190 1010 1200]

	MIN = 144871.4 1/m^2	MAX=492170 1/m^2
	*/

	// Fluent data structures used in calculation

	// expect all zones/threads to be in a single domain
	Domain *d = Get_Domain(1);

	Thread *t; // current cell thread (mesh zone)
	cell_t c; // current cell index w/in the current thread

	// ND_ND is just 2 for 2D, 3 for 3D
	real loc[ND_ND]; // mesh cell location "vector"

	thread_loop_c(t, d) // loop over all threads in domain
	{
		begin_c_loop(c, t) // loop over all cells in thread
		{
			// get mesh cell location
			C_CENTROID(loc, c, t);

			// center of panel is zero and mirrored
			real x_loc = fabs(loc[0] - panel_x_offset);

			// shift Fluent mesh to FLAC3D data zero point at startup room for equations
			real y_loc = panel_length + loc[1] - panel_y_offset;

			// limit vsi function to only within panel domain sizing
			if (x_loc > panelwidth) {
				vsi = 0;
			} else if (x_loc < BOX[1] - BLEND_RANGE) {
				if (y_loc < 0) {
					vsi = 0;
				} else if (y_loc < BOX[4] - BLEND_RANGE_Y - 15) {
					// normalize to equation
					x_loc = -(x_loc - BOX[1] + 20) / BOX[1];
					y_loc = y_loc / BOX[4];

					vsi = super_critical_mine_C_startup_room_center(x_loc, y_loc);
				} else if (y_loc < BOX[4] + BLEND_RANGE_Y - 15) {
					// normalize to equation
					const real X_LOC_1 = -(x_loc - BOX[1]) / (BOX[1]);
					const real X_LOC_2 = -(x_loc - BOX[1]) / BOX[1];
					const real Y_LOC_1 = BOX[4];
					const real Y_LOC_2 = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_C_startup_room_center(X_LOC_1, Y_LOC_1);
					const real FUN2 = super_critical_mine_C_mid_panel_center(X_LOC_2, Y_LOC_2);

					// calculate blending factor
					const real BLEND_MIX = (y_loc - BLEND_RANGE_Y - 15) / (2 * BLEND_RANGE_Y + 30);

					// linearlly interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else if (y_loc < BOX[5] - BLEND_RANGE_Y - 15) {
					// normalize to equation
					x_loc = (-(x_loc - BOX[1] + 10) / (BOX[1]));
					y_loc = ((y_loc - BOX[4]) / (BOX[5] - BOX[4]));

					vsi = super_critical_mine_C_mid_panel_center(x_loc, y_loc);
				} else if (y_loc < BOX[5] + BLEND_RANGE_Y + 15) {
					// normalize to equation
					const real X_LOC_1 = -(x_loc - BOX[1]) / BOX[1];
					const real X_LOC_2 = (x_loc - BOX[1] + BLEND_RANGE + 15) / BOX[1];
					const real Y_LOC_1 = (y_loc - BOX[4] - 100) / (BOX[5] - BOX[4]);
					const real Y_LOC_2 = 1 - (y_loc - BOX[5]) / (BOX[6] - BOX[5]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_C_mid_panel_center(X_LOC_1, Y_LOC_1);
					const real FUN2 = super_critical_mine_C_working_face_center(X_LOC_2, Y_LOC_2);

					// calculate blending factor
					const real BLEND_MIX =
						-(y_loc - BOX[5] - BLEND_RANGE_Y - 15) / (2 * BLEND_RANGE_Y + 30);

					// linerally interpolate
					vsi = FUN1 * BLEND_MIX + FUN2 * (1 - BLEND_MIX);
				} else if (y_loc < BOX[6]) {
					// normalize to equation
					x_loc = (x_loc - BOX[1] + BLEND_RANGE + 15) / BOX[1];
					y_loc = 1 - (y_loc - BOX[5]) / (BOX[6] - BOX[5]);

					vsi = super_critical_mine_C_working_face_center(x_loc, y_loc);
				} else {
					vsi = 0;
				}
			} else if (x_loc <= BOX[1] + BLEND_RANGE) {
				// calculate blending factor
				const real BLEND_MIX = ((x_loc - BOX[1] + BLEND_RANGE) / (2 * BLEND_RANGE));

				if (y_loc < 0) {
					vsi = 0;
				} else if (y_loc < BOX[4]) {
					// normalize to equation
					const real X_LOC_1 = -(x_loc - BOX[1]) / BOX[1];
					const real X_LOC_2 = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					y_loc /= BOX[4];

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_C_startup_room_center(X_LOC_1, y_loc);
					const real FUN2 = super_critical_mine_C_startup_room_corner(X_LOC_2, y_loc);

					// linearlly interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else if (y_loc <= BOX[5]) {
					// normalize to equation
					const real X_LOC_1 = -(x_loc - BOX[1]) / BOX[1];
					const real X_LOC_2 = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					y_loc = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_C_mid_panel_center(X_LOC_1, y_loc);
					const real FUN2 = super_critical_mine_C_mid_panel_gateroad(X_LOC_2, y_loc);

					// linearlly interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else if (y_loc < BOX[6]) {
					// normalize to equation
					const real X_LOC_1 = (x_loc - (BOX[1] - BLEND_RANGE)) / (BOX[1] + BLEND_RANGE);
					const real X_LOC_2 = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					y_loc = 1 - (y_loc - BOX[5]) / (BOX[6] - BOX[5]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_C_working_face_center(X_LOC_1, y_loc);
					const real FUN2 = super_critical_mine_C_working_face_corner(X_LOC_2, y_loc);

					// linearlly interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else {
					vsi = 0;
				}
			} else {
				if (y_loc < 0) {
					vsi = 0;
				} else if (y_loc < BOX[4] - BLEND_RANGE_Y) {
					// normalize to equation
					x_loc = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					y_loc /= BOX[4];

					vsi = super_critical_mine_C_startup_room_corner(x_loc, y_loc);
				} else if (y_loc < BOX[4] + BLEND_RANGE_Y) {
					// normalize to equation
					x_loc = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					const real Y_LOC_1 = y_loc / BOX[4];
					const real Y_LOC_2 = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_C_startup_room_corner(x_loc, Y_LOC_1);
					const real FUN2 = super_critical_mine_C_mid_panel_gateroad(x_loc, Y_LOC_2);

					// calculate blending factor
					const real BLEND_MIX = (y_loc - BOX[4] + BLEND_RANGE_Y) / (2 * BLEND_RANGE_Y);

					// linearlly interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else if (y_loc < BOX[5] - BLEND_RANGE_Y - 20) {
					// normalize to equation
					x_loc = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					y_loc = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);

					vsi = super_critical_mine_C_mid_panel_gateroad(x_loc, y_loc);
				} else if (y_loc < BOX[5] + BLEND_RANGE_Y + 20) {
					// normalize to equation
					x_loc = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					const real Y_LOC_1 = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);
					const real Y_LOC_2 = 1 - (y_loc - BOX[5]) / (BOX[6] - BOX[5]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_C_mid_panel_gateroad(x_loc, Y_LOC_1);
					const real FUN2 = super_critical_mine_C_working_face_corner(x_loc, Y_LOC_2);

					// calculate blending factor
					const real BLEND_MIX = ((y_loc - BOX[5] + BLEND_RANGE_Y) / (2 * BLEND_RANGE_Y));

					// linearlly interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else if (y_loc < panelength) {
					// normalize to equation
					x_loc = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					y_loc = 1 - (y_loc - BOX[5]) / (BOX[6] - BOX[5]);

					vsi = super_critical_mine_C_working_face_corner(x_loc, y_loc);
				} else {
					vsi = 0;
				}
			}
		}

		// clamp and assign vsi to user-defined-memory location
		C_UDMI(c, t, 4) = clamp_upper(clamp_positive(vsi), max_vsi);
	}
	end_c_loop(c, t)
}

/*******************************************************************************
 * MINE E
 * SUPER CRITIAL PANEL
*******************************************************************************/
void VSI_MINE_E_Stepped()
{
	Domain *d;
	Thread *t;
	cell_t c;
	real x[ND_ND]; /* Fluent location vectors */

	real x_loc_norm, y_loc_norm, x_loc, y_loc;
	real panelwidth = 151.4856; /* specified here as half-width */
	real panelength = 1000;
	real panelxoffset = 0.0;
	real panelyoffset = 0.0;
	real VSI = 0;
	real FUN1, FUN2;
	real maximum_vsi = 0.179;
	real blendrange = 20, blendrangey = 20, mix = 0.5;
	real box[7] = { 0, 100, 200, 0, 190, 700, 1000 };
	d = Get_Domain(1);
	/* Assign new panel size from scheme variable define with in FLUENT or use the default sizing above */
	if (RP_Variable_Exists_P("vsi/panel-width")) { /* Returns true if the variable exists */
		panelwidth = (RP_Get_Real("vsi/panel-width") / 2);
		Message("Panel width is: %g\n", panelwidth * 2);
	} /* else default or manual set above is used */
	else {
		Message("Panel Width not set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/panel-width VALUE 'real #f)\n",
			panelwidth * 2);
	}
	box[1] = panelwidth - 100.0;
	box[2] = panelwidth;

	if (RP_Variable_Exists_P("vsi/panel-length")) {
		panelength = (RP_Get_Real("vsi/panel-length"));
		Message("Panel length is set to: %g\n reset value using (rpsetvar 'vsi/panel-length VALUE)\n",
			panelength);
	} else {
		Message("Panel Length not set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/panel-length VALUE'real #f)\n",
			panelength);
	}
	box[5] = panelength - 300;
	box[6] = panelength;

	/* Specify a maximum value of VSI for the change porosity from the scheme variable defined in FLUENT */
	if (RP_Variable_Exists_P("vsi/maximum-vsi")) {
		maximum_vsi = (RP_Get_Real("vsi/maximum-vsi"));
		Message("The Maximum change in porosity from the maximum-porosity behind the face occurs \nat the center of the panel and is set to: %g\n",
			maximum_vsi);
	} /* else default or manual set above is used */
	else {
		Message("Maximum change in porosity from the maximum-porosity behind the face occurs at the center of the panel and is NOT set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/maxiumum-vsi VALUE 'real #f)\n",
			maximum_vsi);
	}

	/* Specify the displacement to the center of the old panel from the scheme variable defined in FLUENT */
	if (RP_Variable_Exists_P("vsi/panel-xoffset")) {
		panelxoffset = (RP_Get_Real("vsi/panel-xoffset"));
		Message("The x-direction displacement to the center of the panel is set to: %g\n", panelxoffset);
	} /* else default or manual set above is used */
	else {
		Message("The x-direction displacement to the center of the panel is to zero.\n You may set it with TUI Command: (rp-var-define 'vsi/panel-xoffset VALUE 'real #f)\n");
	}
	/* Specify the displacement to the recovery room of the old panel from the scheme variable defined in FLUENT */
	if (RP_Variable_Exists_P("vsi/panel-yoffset")) {
		panelyoffset = (RP_Get_Real("vsi/panel-yoffset"));
		Message("The y-direction displacement to the recovery room of the panel is set to: %g\n", panelyoffset);
	} /* else default or manual set above is used */
	else {
		Message("The y-direction displacement to the recovery room of the panel is to zero.\n You may set it with TUI Command: (rp-var-define 'vsi/panel-yoffset VALUE 'real #f)\n");
	}
	thread_loop_c(t, d)
	{
		begin_c_loop(c, t)
		{
			C_CENTROID(x, c, t);

			/*     Scale each section of the model to the curve fits.             
									
	UNITS in METERS                                                            
	                                                                           
	           TG           RECOVERY ROOM / ACTIVE FACE        HG              
	       Fit 160        100             0            100        160          
	       1200|--------------------------------------------------| 0          
	           |0        1|                            |1        0|            
	           |    5     |    6                       |          |            
	           | Recovery |  Recovery                  |          |            
	           | Gateroad |  Center                    |          |            
	           |Exact-Size|   Super Critial Expansion  |          |            
	           |1         |                            |         1|            
	       1010|--------------------------------------------------| 190        
	           |          |                            |          |            
	           |          |                            |          |            
	           |    3     |    4                       |          |            
	           |  Center  | Center                     |          |            
	           | Gateroad | Panel                      |          |            
	           |          |                            |          |            
	           | Expansion| Expansion                  |          |            
	           | Equation | Equation                   |          |            
	           |          |                            |          |            
	           |          |                            |          |            
	        190|--------------------------------------------------| 1010       
	           |1         |                            |         1|            
	           |    1     |     2                      |          |            
	           | Startup  |  Startup                   |          |            
	           | Gateroad |  Center                    |          |            
	           |Exact-Size|   Super Critial Expansion  |          |            
	           |0        1|                            |1        0|            
	          0|--------------------------------------------------| 1200       
	         -152.5      -52.5               0        92.5      +152.5  My Panel
	          box=[0 92.5 160 0 190 1010 1200]   
	MIN=144871.4  1/m^2	MAX=492170 1/m^2	*/
			x_loc = fabs(x[0] - panelxoffset); /* Center of Panel is Zero and Mirrored */
			y_loc = (panelength + x[1]) -
				panelyoffset; /* Shift FLUENT MESH to FLAC3D data Zero point at startup room for equations */
			if (x_loc > panelwidth) {
				VSI = 0;
			} /* limit VSI function to only within panel domain sizing */

			else {
				if (x_loc < (box[1] - blendrange)) {
					if (y_loc < 0) {
						VSI = 0;
					} else if (y_loc < box[4] - blendrangey - 15) {
						x_loc_norm =
							(-(x_loc - box[1] + 20) / (box[1])); /* NORMALIZE to equation */
						y_loc_norm = (y_loc / box[4]);
						VSI = SUPER_CRITICAL_MINE_E_STARTUP_CENTER(x_loc_norm, y_loc_norm);
					} else if ((y_loc > (box[4] - blendrangey - 15)) &&
						   (y_loc < box[4] + blendrangey - 15)) {
						mix = ((y_loc - (box[4] + blendrangey - 15)) / (2 * blendrangey));
						x_loc_norm =
							(-(x_loc - box[1] + 20) / (box[1])); /* NORMALIZE to equation */
						y_loc_norm = (y_loc / box[4]);
						FUN1 = SUPER_CRITICAL_MINE_E_STARTUP_CENTER(x_loc_norm, y_loc_norm);
						x_loc_norm = (-(x_loc - box[1] + 10) / (box[1]));
						y_loc_norm = ((y_loc - box[4]) / (box[5] - box[4]));
						FUN2 = SUPER_CRITICAL_MINE_E_CENTER_PANEL(x_loc_norm, y_loc_norm);
						VSI = (FUN2 * (mix) + FUN1 * (1 - mix));
					} else if ((y_loc < (box[5] - blendrangey - 15)) &&
						   (y_loc > (box[4] + blendrangey - 15))) {
						x_loc_norm = (-(x_loc - box[1] + 10) / (box[1]));
						y_loc_norm = ((y_loc - box[4]) / (box[5] - box[4]));
						VSI = SUPER_CRITICAL_MINE_E_CENTER_PANEL(x_loc_norm, y_loc_norm);
					} else if ((y_loc > (box[5] - blendrangey - 15)) &&
						   (y_loc < box[5] + blendrangey - 15)) {
						mix = -((y_loc - (box[5] + blendrangey - 15)) / (2 * blendrangey));
						x_loc_norm = (-(x_loc - box[1]) / (box[1]));
						y_loc_norm = ((y_loc - box[4]) / (box[5] - box[4]));
						FUN1 = SUPER_CRITICAL_MINE_E_CENTER_PANEL(x_loc_norm, y_loc_norm);
						x_loc_norm = ((x_loc - box[1] + blendrange + 15) / (box[1]));
						y_loc_norm = (1 - (y_loc - box[5]) / (box[6] - box[5]));
						FUN2 = SUPER_CRITICAL_MINE_E_RECOVERY_CENTER(x_loc_norm, y_loc_norm);
						VSI = (FUN1 * (mix) + FUN2 * (1 - mix));
					} else if ((y_loc < box[6]) &&
						   (y_loc >
						    (box[5] + blendrangey -
						     15))) { /* Remained of data points are in the recovery room 600-1000m */
						x_loc_norm = ((x_loc - box[1] + blendrange + 15) / (box[1]));
						y_loc_norm = (1 - (y_loc - box[5]) / (box[6] - box[5]));
						VSI = SUPER_CRITICAL_MINE_E_RECOVERY_CENTER(x_loc_norm, y_loc_norm);
					} else {
						VSI = 0;
					}
				} else if (x_loc <= (box[1] + blendrange)) {
					mix = ((x_loc - box[1] + blendrange) / (2 * blendrange));
					if (y_loc < 0) {
						VSI = 0;
					} else if (y_loc < box[4]) {
						x_loc_norm = (-(x_loc - box[1]) / (box[1])); /* NORMALIZE to equation */
						y_loc_norm = (y_loc / box[4]);
						FUN1 = SUPER_CRITICAL_MINE_E_STARTUP_CENTER(x_loc_norm, y_loc_norm);
						x_loc_norm = (1 - (x_loc - box[1]) / (box[2] - box[1]));
						y_loc_norm = (y_loc / box[4]);
						FUN2 = SUPER_CRITICAL_MINE_E_STARTUP_GATEROADS(x_loc_norm, y_loc_norm);
						VSI = (FUN2 * (mix) + FUN1 * (1 - mix));
					} else if (y_loc <= box[5]) {
						x_loc_norm = (-(x_loc - box[1]) / (box[1]));
						y_loc_norm = ((y_loc - box[4]) / (box[5] - box[4]));
						FUN1 = SUPER_CRITICAL_MINE_E_CENTER_PANEL(x_loc_norm, y_loc_norm);
						x_loc_norm = (1 - (x_loc - box[1]) / (box[2] - box[1]));
						y_loc_norm = ((y_loc - box[4]) / (box[5] - box[4]));
						FUN2 = SUPER_CRITICAL_MINE_E_CENTER_GATEROADS(x_loc_norm, y_loc_norm);
						VSI = (FUN2 * (mix) + FUN1 * (1 - mix));
					} else if (y_loc < box[6]) {
						x_loc_norm = ((x_loc - (box[1])) / (box[1]));
						y_loc_norm = (1 - (y_loc - box[5]) / (box[6] - box[5]));
						FUN1 = SUPER_CRITICAL_MINE_E_RECOVERY_CENTER(x_loc_norm, y_loc_norm);
						x_loc_norm = (1 - (x_loc - (box[1])) / (box[2] - box[1]));
						y_loc_norm = (1 - (y_loc - box[5]) / (box[6] - box[5]));
						FUN2 = SUPER_CRITICAL_MINE_E_RECOVERY_GATEROADS(x_loc_norm, y_loc_norm);
						VSI = (FUN2 * (mix) + FUN1 * (1 - mix));
					} else {
						VSI = 0;
					}
				}

				else {
					if (y_loc < 0) {
						VSI = 0;
					} else if (y_loc < box[4] - blendrangey) {
						x_loc_norm = (1 - (x_loc - box[1]) / (box[2] - box[1]));
						y_loc_norm = (y_loc / box[4]);
						VSI = SUPER_CRITICAL_MINE_E_STARTUP_GATEROADS(x_loc_norm, y_loc_norm);
					} else if ((y_loc > (box[4] - blendrangey)) &&
						   (y_loc < (box[4] + blendrangey))) {
						mix = ((y_loc - box[4] + blendrangey) / (2 * blendrangey));
						x_loc_norm = (1 - (x_loc - box[1]) / (box[2] - box[1]));
						y_loc_norm = (y_loc / box[4]);
						FUN1 = SUPER_CRITICAL_MINE_E_STARTUP_GATEROADS(x_loc_norm, y_loc_norm);
						x_loc_norm = (1 - (x_loc - box[1]) / (box[2] - box[1]));
						y_loc_norm = ((y_loc - box[4]) / (box[5] - box[4]));
						FUN2 = SUPER_CRITICAL_MINE_E_CENTER_GATEROADS(x_loc_norm, y_loc_norm);
						VSI = (FUN2 * (mix) + FUN1 * (1 - mix));
					} else if ((y_loc < (box[5] - blendrangey - 20)) &&
						   (y_loc > (box[4] + blendrangey))) {
						x_loc_norm = (1 - (x_loc - box[1]) / (box[2] - box[1]));
						y_loc_norm = ((y_loc - box[4]) / (box[5] - box[4]));
						VSI = SUPER_CRITICAL_MINE_E_CENTER_GATEROADS(x_loc_norm, y_loc_norm);
					} else if ((y_loc > (box[5] - blendrangey - 20)) &&
						   (y_loc < box[5] + blendrangey + 20)) {
						mix = ((y_loc - box[5] + blendrangey) / (2 * blendrangey));
						x_loc_norm = (1 - (x_loc - box[1]) / (box[2] - box[1]));
						y_loc_norm = ((y_loc - box[4]) / (box[5] - box[4]));
						FUN1 = SUPER_CRITICAL_MINE_E_CENTER_GATEROADS(x_loc_norm, y_loc_norm);
						x_loc_norm = (1 - (x_loc - box[1]) / (box[2] - box[1]));
						y_loc_norm = (1 - (y_loc - box[5]) / (box[6] - box[5]));
						FUN2 = SUPER_CRITICAL_MINE_E_RECOVERY_GATEROADS(x_loc_norm, y_loc_norm);
						VSI = (FUN2 * (mix) + FUN1 * (1 - mix));
					} else if (y_loc < panelength) {
						x_loc_norm = (1 - (x_loc - box[1]) / (box[2] - box[1]));
						y_loc_norm = (1 - (y_loc - box[5]) / (box[6] - box[5]));
						VSI = SUPER_CRITICAL_MINE_E_RECOVERY_GATEROADS(x_loc_norm, y_loc_norm);
					} else {
						VSI = 0;
					}
				}
			}
			VSI = (VSI < 0.0) ? 0 : VSI;
			VSI = (VSI > maximum_vsi) ? maximum_vsi : VSI;
			C_UDMI(c, t, 4) = VSI;
		}
		end_c_loop(c, t)
	}
}
