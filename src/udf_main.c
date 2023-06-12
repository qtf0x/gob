#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "udf.h" // Fluent macros

#include "udf_vsi.h"
// #include "udf_adjust.h"
#include "udf_explosive_mix.h"
// #include "udf_inertia.h"
// #include "udf_permeability.h"
// #include "udf_porosity.h"
#include "utils.h"

#define domain_ID 2 // using primary phase domain

int ite = 0; // number of iterations elapsed; for global use in UDF definitions

DEFINE_ADJUST(demo_calc, d)
{
	++ite;
}

DEFINE_PROFILE(set_poro_VSI, t, nv)
{
	/* n = (V_v - VSI) /V_t
where n is porosity (%), V_v is volume of voids (cubic meters), 
vsi is volumetric strain (%), and V_t is total volume (cubic meters). */
	real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
	/*  real V_t=10.0000 * 10.0000 * 10.0000;  10 meter cubed grid cell in FLAC3D */
	cell_t c;
	real cellpor;
	real V_v = 0.40000;
	real a = 1; /* for a scalor */
	if (RP_Variable_Exists_P("longwallgobs/initial_porosity")) { /* Returns true if the variable exists */
		a = (RP_Get_Real("longwallgobs/initial_porosity"));
	}
	if (RP_Variable_Exists_P("longwallgobs/max_porosity")) {
		V_v = (RP_Get_Real("longwallgobs/max_porosity"));
	}

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellpor = ((V_v - C_UDMI(c, t, 4)) *
				   a); /* Initial Maximum gob porosity minus the change in porosity (VSI). */

			C_PROFILE(c, t, nv) = (cellpor < 0) ? 0 : cellpor; /* 'a' scaler for later use */
			C_UDMI(c, t, 1) = (cellpor < 0) ? 0 : cellpor;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 1);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_inertia_1_VSI, t, nv)
{
	/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

	real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
	cell_t c;
	real cellporo;
	real initial_inertia_resistance;
	real cellinertiaresist;

	real V_v = 0.40000000;
	real a = 1;
	real resist_scaler = 1;
	real maximum_inertia_resist = 1.3E5;
	real minimum_inertia_resist = 0.000;
	if (RP_Variable_Exists_P("longwallgobs/max_porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("longwallgobs/max_porosity"));
	}
	if (RP_Variable_Exists_P("longwallgobs/initial_porosity")) {
		a = (RP_Get_Real("longwallgobs/initial_porosity"));
	}
	if (RP_Variable_Exists_P("vsi/resist-inertia-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-inertia-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-inertia-resist")) {
		maximum_inertia_resist = (RP_Get_Real("vsi/maximum-inertia-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-inertia-resist")) {
		minimum_inertia_resist = (RP_Get_Real("vsi/minimum-inertia-resist"));
	}

	initial_inertia_resistance = Initial_Inertia_Resistance();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo = ((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					   0 :
					   (V_v - C_UDMI(c, t, 4)) * a; /* Limit lowest value of porosity to zero */
			cellinertiaresist = Cell_Inertia_Resistance(
				cellporo, initial_inertia_resistance); /* Blake-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellinertiaresist < maximum_inertia_resist) {
				if (cellinertiaresist < minimum_inertia_resist) {
					cellinertiaresist = minimum_inertia_resist;
				}
			} else {
				cellinertiaresist = maximum_inertia_resist;
			}
			C_PROFILE(c, t, nv) = cellinertiaresist * resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 5) = cellinertiaresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 5);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_inertia_2_VSI, t, nv)
{
	/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

	real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
	cell_t c;
	real cellporo;
	real initial_inertia_resistance;
	real cellinertiaresist;

	real V_v = 0.40000000;
	real a = 1;
	real resist_scaler = 1;
	real maximum_inertia_resist = 1.3E5;
	real minimum_inertia_resist = 0.000;
	if (RP_Variable_Exists_P("longwallgobs/max_porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("longwallgobs/max_porosity"));
	}
	if (RP_Variable_Exists_P("longwallgobs/initial_porosity")) {
		a = (RP_Get_Real("longwallgobs/initial_porosity"));
	}
	if (RP_Variable_Exists_P("vsi/resist-inertia-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-inertia-scaler"));
	}
	if (RP_Variable_Exists_P("longwallgobs/maximum_inertia_resist")) {
		maximum_inertia_resist = (RP_Get_Real("longwallgobs/maximum_inertia_resist"));
	}
	if (RP_Variable_Exists_P("longwallgobs/minimum_inertia_resist")) {
		minimum_inertia_resist = (RP_Get_Real("longwallgobs/minimum_inertia_resist"));
	}

	initial_inertia_resistance = Initial_Inertia_Resistance();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo = ((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					   0 :
					   (V_v - C_UDMI(c, t, 4)) * a; /* Limit lowest value of porosity to zero */
			cellinertiaresist = Cell_Inertia_Resistance(
				cellporo, initial_inertia_resistance); /* Blake-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellinertiaresist < maximum_inertia_resist) {
				if (cellinertiaresist < minimum_inertia_resist) {
					cellinertiaresist = minimum_inertia_resist;
				}
			} else {
				cellinertiaresist = maximum_inertia_resist;
			}
			C_PROFILE(c, t, nv) = cellinertiaresist * resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 5) = cellinertiaresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 5);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_inertia_3_VSI, t, nv)
{
	/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

	real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
	cell_t c;
	real cellporo;
	real initial_inertia_resistance;
	real cellinertiaresist;

	real V_v = 0.40000000;
	real a = 1;
	real resist_scaler = 1;
	real maximum_inertia_resist = 1.3E5;
	real minimum_inertia_resist = 0.000;
	if (RP_Variable_Exists_P("longwallgobs/max_porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("longwallgobs/max_porosity"));
	}
	if (RP_Variable_Exists_P("longwallgobs/initial_porosity")) {
		a = (RP_Get_Real("longwallgobs/initial_porosity"));
	}
	if (RP_Variable_Exists_P("vsi/resist-inertia-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-inertia-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-inertia-resist")) {
		maximum_inertia_resist = (RP_Get_Real("vsi/maximum-inertia-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-inertia-resist")) {
		minimum_inertia_resist = (RP_Get_Real("vsi/minimum-inertia-resist"));
	}

	initial_inertia_resistance = Initial_Inertia_Resistance();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo = ((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					   0 :
					   (V_v - C_UDMI(c, t, 4)) * a; /* Limit lowest value of porosity to zero */
			cellinertiaresist = Cell_Inertia_Resistance(
				cellporo, initial_inertia_resistance); /* Blake-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellinertiaresist < maximum_inertia_resist) {
				if (cellinertiaresist < minimum_inertia_resist) {
					cellinertiaresist = minimum_inertia_resist;
				}
			} else {
				cellinertiaresist = maximum_inertia_resist;
			}
			C_PROFILE(c, t, nv) = cellinertiaresist * resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 5) = cellinertiaresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 5);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_perm_1_VSI, t, nv)
{
	/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

	real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
	cell_t c;
	real cellporo;
	real initial_permeability;
	real cellresist;

	real V_v = 0.40000000;
	real a = 1;
	real resist_scaler = 1;
	real maximum_resist = 5.000000E6;
	real minimum_resist = 1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P("longwallgobs/max_porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("longwallgobs/max_porosity"));
	}
	if (RP_Variable_Exists_P("longwallgobs/initial_porosity")) {
		a = (RP_Get_Real("longwallgobs/initial_porosity"));
	}
	if (RP_Variable_Exists_P("longwallgobs/resist_scaler")) {
		resist_scaler = (RP_Get_Real("longwallgobs/resist_scaler"));
	}
	if (RP_Variable_Exists_P("longwallgobs/max_resistance")) {
		maximum_resist = (RP_Get_Real("longwallgobs/max_resistance"));
	}
	if (RP_Variable_Exists_P("longwallgobs/min_resistance")) {
		minimum_resist = (RP_Get_Real("longwallgobs/min_resistance"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo = ((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					   0 :
					   (V_v - C_UDMI(c, t, 4)) * a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_perm_2_VSI, t, nv)
{
	/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

	real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
	cell_t c;
	real cellporo;
	real initial_permeability;
	real cellresist;

	real V_v = 0.40000000;
	real a = 1;
	real resist_scaler = 1;
	real maximum_resist = 5.000000E6;
	real minimum_resist = 1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P("longwallgobs/max_porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("longwallgobs/max_porosity"));
	}
	if (RP_Variable_Exists_P("longwallgobs/initial_porosity")) {
		a = (RP_Get_Real("longwallgobs/initial_porosity"));
	}
	if (RP_Variable_Exists_P("longwallgobs/resist_scaler")) {
		resist_scaler = (RP_Get_Real("longwallgobs/resist_scaler"));
	}
	if (RP_Variable_Exists_P("longwallgobs/max_resistance")) {
		maximum_resist = (RP_Get_Real("longwallgobs/max_resistance"));
	}
	if (RP_Variable_Exists_P("longwallgobs/min_resistance")) {
		minimum_resist = (RP_Get_Real("longwallgobs/min_resistance"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo = ((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					   0 :
					   (V_v - C_UDMI(c, t, 4)) * a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_perm_3_VSI, t, nv)
{
	/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

	real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
	cell_t c;
	real cellporo;
	real initial_permeability;
	real cellresist;

	real V_v = 0.40000000;
	real a = 1;
	real resist_scaler = 1;
	real maximum_resist = 5.000000E6;
	real minimum_resist = 1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P("longwallgobs/max_porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("longwallgobs/max_porosity"));
	}
	if (RP_Variable_Exists_P("longwallgobs/initial_porosity")) {
		a = (RP_Get_Real("longwallgobs/initial_porosity"));
	}
	if (RP_Variable_Exists_P("longwallgobs/resist_scaler")) {
		resist_scaler = (RP_Get_Real("longwallgobs/resist_scaler"));
	}
	if (RP_Variable_Exists_P("longwallgobs/max_resistance")) {
		maximum_resist = (RP_Get_Real("longwallgobs/max_resistance"));
	}
	if (RP_Variable_Exists_P("longwallgobs/min_resistance")) {
		minimum_resist = (RP_Get_Real("longwallgobs/min_resistance"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo = ((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					   0 :
					   (V_v - C_UDMI(c, t, 4)) * a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

DEFINE_EXECUTE_FROM_GUI(udf_main, longwallgobs, mode)
{
	// if not set to default -1, we are using a single part mesh
	const bool SINGLE_PART_MESH = RP_Get_Integer("longwallgobs/single_part_mesh_id") >= 0;

	// get FLAC3D offsets
	real panel_x_offset = 0;
	real panel_y_offset = 0;

	if (SINGLE_PART_MESH) {
		// midpoint of working face center
		panel_x_offset = (RP_Get_Real("longwallgobs/single_part_mesh_max_x") +
				  RP_Get_Real("longwallgobs/single_part_mesh_min_x")) /
				 2;

		// assumption: startup room MORE POSITIVE than working face
		panel_y_offset = RP_Get_Real("longwallgobs/single_part_mesh_max_y");
	} else if (RP_Get_Boolean("mine_t")) {
		if (RP_Get_Real("longwallgobs/startup_room_corner_max_y") >
		    RP_Get_Real("longwallgobs/working_face_corner_max_y")) {
			panel_x_offset = RP_Get_Real("longwallgobs/working_face_corner_min_x");
			panel_y_offset = RP_Get_Real("longwallgobs/startup_room_corner_max_y");
		} else {
			panel_x_offset = RP_Get_Real("longwallgobs/working_face_corner_max_x");
			panel_y_offset = RP_Get_Real("longwallgobs/startup_room_corner_min_y");
		}
	} else {
		// midpoint of working face center
		panel_x_offset = (RP_Get_Real("longwallgobs/working_face_center_max_x") +
				  RP_Get_Real("longwallgobs/working_face_center_min_x")) /
				 2;

		// working face->startup room along positive y-axis
		if (RP_Get_Real("longwallgobs/startup_room_center_max_y") >
		    RP_Get_Real("longwallgobs/working_face_center_max_y"))
			panel_y_offset = RP_Get_Real("longwallgobs/startup_room_center_max_y");

		// working face->startup room along negative y-axis
		else
			panel_y_offset = RP_Get_Real("longwallgobs/startup_room_center_min_y");
	}

	printf("panel_x_offset: %f\npanel_y_offset: %f\n", panel_x_offset, panel_y_offset);

	printf("Calculating VSI...\n");

	// calculate vsi
	if (RP_Get_Boolean("mine_c"))
		vsi_mine_C_stepped(SINGLE_PART_MESH, panel_x_offset, panel_y_offset);

	if (RP_Get_Boolean("mine_e"))
		vsi_mine_E_stepped(SINGLE_PART_MESH, panel_x_offset, panel_y_offset);

	if (RP_Get_Boolean("mine_t"))
		vsi_trona_stepped(SINGLE_PART_MESH, panel_x_offset, panel_y_offset);

	// calculate explosive gas mix + integral
	if (RP_Get_Boolean("longwallgobs/egz_radio_buttons")) {
		calc_explosive_mix(void);
		calc_explosive_integral_gob(void);
	}
}
