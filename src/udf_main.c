#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "udf.h" // Fluent macros

#include "udf_vsi.h"
#include "udf_adjust.h"
#include "udf_explosive_mix.h"
#include "udf_inertia.h"
#include "udf_permeability.h"
#include "udf_porosity.h"
#include "utils.h"

#define domain_ID 2 // using primary phase domain

int ite = 0; // number of iterations elapsed; for global use in UDF definitions

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
