#include <stdbool.h>

#include "udf.h" // Fluent macros

#include "udf_vsi.h"

#define domain_ID 2 // using primary phase domain

int ite = 0; // number of iterations elapsed; for global use in UDF definitions

DEFINE_EXECUTE_FROM_GUI(udf_main, longwallgobs, mode)
{
	bool mine_C = RP_Get_Boolean("longwallgobs/mine_C_radio_button");
	bool mine_E = RP_Get_Boolean("longwallgobs/mine_E_radio_button");
	bool mine_T = RP_Get_Boolean("longwallgobs/mine_T_radio_button");

	// calculate vsi
	if (mine_C)
		VSI_MINE_C_Stepped();
	else if (mine_E)
		VSI_MINE_E_Stepped();
	else if (mine_T)
		VSI_MINE_Trona_Stepped();
}
