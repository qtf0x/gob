#ifndef GOB_UDF_POROSITY_H
#define GOB_UDF_POROSITY_H

#include "udf.h" // Fluent macros

extern int ite;

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

#endif // GOB_UDF_POROSITY_H
