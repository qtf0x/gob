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
#include "utils.h" // for clamp

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
	   -152.5      -52.5      0       92.5     +152.5  My Panel

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
			if (x_loc > panel_half_width || y_loc < 0 || y_loc > BOX[5]) {
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
			}
		}

		// clamp and assign vsi to user-defined-memory location
		C_UDMI(c, t, 4) = clamp(vsi, 0, max_vsi);
	}
	end_c_loop(c, t)
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
	   -152.5      -52.5           0            92.5      +152.5  My Panel

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
		{ begin_c_loop(c, t) // loop over all cells in thread
		  { // get mesh cell location
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
			const real BLEND_MIX = -(y_loc - BOX[5] - BLEND_RANGE_Y - 15) / (2 * BLEND_RANGE_Y + 30);

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
		const real BLEND_MIX = (x_loc - BOX[1] + BLEND_RANGE) / (2 * BLEND_RANGE);

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
C_UDMI(c, t, 4) = clamp(vsi, 0, max_vsi)
}
end_c_loop(c, t)
}

/*******************************************************************************
 * MINE E
 * SUPER CRITIAL PANEL
*******************************************************************************/
void vsi_mine_E_stepped()
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
		max_vsi = 0.179;

	// variables used in calculation

	real vsi = 0; // volumetric strain increment
	const real BLEND_RANGE = 20;
	const real BLEND_RANGE_Y = 20; // (half) width of the blend zone

	const real BOX[7] = { 0, panel_half_width - 100, panel_half_width, 0, 190, panel_length - 300, panel_length };

	/*     Scale each section of the model to the curve fits.
	UNITS in METERS

	   TG           RECOVERY ROOM / ACTIVE FACE        HG
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
	   -152.5      -52.5           0            92.5      +152.5  My Panel

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

	thread_loop_c(t, d)
	{
		begin_c_loop(c, t)
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
					y_loc /= BOX[4];

					vsi = super_critical_mine_E_startup_room_center(x_loc, y_loc);
				} else if (y_loc < BOX[4] + BLEND_RANGE_Y - 15) {
					// normalize to equation
					const real X_LOC_1 = -(x_loc - BOX[1] + 20) / BOX[1];
					const real X_LOC_2 = -(x_loc - BOX[1] + 10) / BOX[1];
					const real Y_LOC_1 = y_loc / BOX[4];
					const real Y_LOC_2 = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_E_startup_room_center(X_LOC_1, Y_LOC_1);
					const real FUN2 = super_critical_mine_E_mid_panel_center(X_LOC_2, Y_LOC_2);

					// calculate blending factor
					const real BLEND_MIX =
						(y_loc - (BOX[4] + BLEND_RANGE_Y - 15)) / (2 * BLEND_RANGE_Y);

					// linearlly interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else if (y_loc < BOX[5] - BLEND_RANGE_Y - 15) {
					// normalize to equation
					x_loc = -(x_loc - BOX[1] + 10) / BOX[1];
					y_loc = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);

					vsi = super_critical_mine_E_mid_panel_center(x_loc, y_loc);
				} else if (y_loc < BOX[5] + BLEND_RANGE_Y - 15) {
					// normalize to equation
					const real X_LOC_1 = -(x_loc - BOX[1]) / BOX[1];
					const real X_LOC_2 = (x_loc - BOX[1] + BLEND_RANGE + 15) / BOX[1];
					const real Y_LOC_1 = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);
					const real Y_LOC_2 = 1 - (y_loc - BOX[5]) / (BOX[6] - BOX[5]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_E_mid_panel_center(X_LOC_1, Y_LOC_1);
					const real FUN2 = super_critical_mine_E_working_face_center(X_LOC_2, Y_LOC_2);

					// calculate blending factor
					const real BLEND_MIX =
						-((y_loc - (BOX[5] + BLEND_RANGE_Y - 15)) / (2 * BLEND_RANGE_Y));

					// linearlly interpolate
					vsi = FUN1 * BLEND_MIX + FUN2 * (1 - BLEND_MIX);
				} else if (y_loc < BOX[6]) {
					// normalize to equation
					x_loc = (x_loc - BOX[1] + BLEND_RANGE + 15) / BOX[1];
					y_loc = 1 - (y_loc - BOX[5]) / (BOX[6] - BOX[5]);

					vsi = super_critical_mine_E_working_face_center(x_loc, y_loc);
				} else {
					vsi = 0;
				}
			} else if (x_loc <= BOX[1] + BLEND_RANGE) {
				// calculate blending factor
				const real BLEND_MIX = (x_loc - BOX[1] + BLEND_RANGE) / (2 * BLEND_RANGE);

				if (y_loc < 0) {
					vsi = 0;
				} else if (y_loc < BOX[4]) {
					// normalize to equation
					const real X_LOC_1 = -(x_loc - BOX[1]) / BOX[1];
					const real X_LOC_2 = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					y_loc /= BOX[4];

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_E_startup_room_center(X_LOC_1, y_loc);
					const real FUN2 = super_critical_mine_E_startup_room_corner(X_LOC_2, y_loc);

					// linerally interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else if (y_loc <= BOX[5]) {
					// normalize to equation
					const real X_LOC_1 = -(x_loc - BOX[1]) / (BOX[1]);
					const real X_LOC_2 = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					y_loc = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_E_mid_panel_center(X_LOC_1, y_loc);
					const real FUN2 = super_critical_mine_E_mid_panel_gateroad(X_LOC_2, y_loc);

					// linerally interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else if (y_loc < BOX[6]) {
					// normalize to equation
					const real X_LOC_1 = (x_loc - (BOX[1])) / (BOX[1]);
					const real X_LOC_2 = 1 - (x_loc - (BOX[1])) / (BOX[2] - BOX[1]);
					y_loc = 1 - (y_loc - BOX[5]) / (BOX[6] - BOX[5]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_E_working_face_center(X_LOC_1, y_loc);
					const real FUN2 = super_critical_mine_E_working_face_corner(X_LOC_2, y_loc);

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

					vsi = super_critical_mine_E_startup_room_corner(x_loc, y_loc);
				} else if (y_loc < BOX[4] + BLEND_RANGE_Y) {
					// normalize to equation
					x_loc = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					const real Y_LOC_1 = y_loc / BOX[4];
					const real Y_LOC_2 = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);

					// calculate fits for both equations
					const real FUN1 = super_critical_mine_E_startup_room_corner(x_loc, Y_LOC_1);
					const real FUN2 = super_critical_mine_E_mid_panel_gateroad(x_loc, Y_LOC_2);

					// calculate blending factor
					const real BLEND_MIX = (y_loc - BOX[4] + BLEND_RANGE_Y) / (2 * BLEND_RANGE_Y);

					// linearlly interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else if (y_loc < BOX[5] - BLEND_RANGE_Y - 20) {
					// normalize to equation
					x_loc = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					y_loc = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);

					vsi = super_critical_mine_E_mid_panel_gateroad(x_loc, y_loc);
				} else if (y_loc < BOX[5] + BLEND_RANGE_Y + 20) {
					// normalize to equation
					x_loc = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					const real Y_LOC_1 = (y_loc - BOX[4]) / (BOX[5] - BOX[4]);
					const real Y_LOC_2 = 1 - (y_loc - BOX[5]) / (BOX[6] - BOX[5]);

					// calculate fits for both zones
					const real FUN1 = super_critical_mine_E_mid_panel_gateroad(x_loc, Y_LOC_1);
					const real FUN2 = super_critical_mine_E_working_face_corner(x_loc, Y_LOC_2);

					// calculate blending factor
					const real BLEND_MIX = (y_loc - BOX[5] + BLEND_RANGE_Y) / (2 * BLEND_RANGE_Y);

					// linearlly interpolate
					vsi = FUN2 * BLEND_MIX + FUN1 * (1 - BLEND_MIX);
				} else if (y_loc < panelength) {
					// normalize to equation
					x_loc = 1 - (x_loc - BOX[1]) / (BOX[2] - BOX[1]);
					y_loc = 1 - (y_loc - BOX[5]) / (BOX[6] - BOX[5]);

					vsi = super_critical_mine_E_working_face_corner(x_loc, y_loc);
				} else {
					vsi = 0;
				}
			}
		}

		// clamp and assign vsi to user-defined-memory location
		C_UDMI(c, t, 4) = clamp(vsi, 0, max_vsi);
	}
	end_c_loop(c, t)
}
