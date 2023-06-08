#include <stdbool.h>

#include "udf.h" // Fluent macros

#include "udf_vsi.h"

#include "utils.h"

#define domain_ID 2 // using primary phase domain

int ite = 0; // number of iterations elapsed; for global use in UDF definitions

DEFINE_EXECUTE_FROM_GUI(udf_main, longwallgobs, mode)
{
	// if not set to default -1, we are using a single part mesh
	const bool SINGLE_PART_MESH = RP_Get_Integer("longwallgobs/single_part_mesh_id") >= 0;

	// get FLAC3D offsets
	real panel_x_offset, panel_y_offset;

	if (SINGLE_PART_MESH)
		get_offsets_single_part(&panel_x_offset, &panel_y_offset);
	else if (RP_Get_Boolean("longwallgobs/mine_T_radio_button"))
		get_offsets_sub_critical(&panel_x_offset, &panel_y_offset);
	else
		get_offsets_super_critical(&panel_x_offset, &panel_y_offset);

	// calculate vsi
	if (RP_Get_Boolean("longwallgobs/mine_C_radio_button"))
		vsi_mine_C_stepped(SINGLE_PART_MESH, panel_x_offset, panel_y_offset);
	else if (RP_Get_Boolean("longwallgobs/mine_E_radio_button"))
		vsi_mine_E_stepped(SINGLE_PART_MESH, panel_x_offset, panel_y_offset);
	else if (RP_Get_Boolean("longwallgobs/mine_T_radio_button"))
		vsi_trona_stepped(SINGLE_PART_MESH, panel_x_offset, panel_y_offset);
}
