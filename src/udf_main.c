#include <stdbool.h>

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
	// TODO: 28 new RP variables yay!

	printf("panel_x_offset: %f\npanel_y_offset: %f\n", panel_x_offset, panel_y_offset);

	// calculate vsi
	if (RP_Get_Boolean("longwallgobs/mine_C_radio_button"))
		vsi_mine_C_stepped(SINGLE_PART_MESH, panel_x_offset, panel_y_offset);
	else if (RP_Get_Boolean("longwallgobs/mine_E_radio_button"))
		vsi_mine_E_stepped(SINGLE_PART_MESH, panel_x_offset, panel_y_offset);
	else if (RP_Get_Boolean("longwallgobs/mine_T_radio_button"))
		vsi_trona_stepped(SINGLE_PART_MESH, panel_x_offset, panel_y_offset);
}
