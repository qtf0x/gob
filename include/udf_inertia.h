#ifndef GOB_UDF_INERTIA_H
#define GOB_UDF_INERTIA_H

#include "udf.h" // Fluent macros

#include "utils.h"

extern int ite;

/* 
#################################
# C2 Inertia Resistance		#
#################################
*/

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
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-inertia-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-inertia-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-inertia-resist")) {
		maximum_inertia_resist =
			(RP_Get_Real("vsi/maximum-inertia-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-inertia-resist")) {
		minimum_inertia_resist =
			(RP_Get_Real("vsi/minimum-inertia-resist"));
	}

	initial_inertia_resistance = Initial_Inertia_Resistance();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellinertiaresist = Cell_Inertia_Resistance(
				cellporo,
				initial_inertia_resistance); /* Blake-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellinertiaresist < maximum_inertia_resist) {
				if (cellinertiaresist <
				    minimum_inertia_resist) {
					cellinertiaresist =
						minimum_inertia_resist;
				}
			} else {
				cellinertiaresist = maximum_inertia_resist;
			}
			C_PROFILE(c, t, nv) =
				cellinertiaresist *
				resist_scaler; /* Scaler applied to cell resistance */
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
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-inertia-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-inertia-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-inertia-resist")) {
		maximum_inertia_resist =
			(RP_Get_Real("vsi/maximum-inertia-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-inertia-resist")) {
		minimum_inertia_resist =
			(RP_Get_Real("vsi/minimum-inertia-resist"));
	}

	initial_inertia_resistance = Initial_Inertia_Resistance();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellinertiaresist = Cell_Inertia_Resistance(
				cellporo,
				initial_inertia_resistance); /* Blake-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellinertiaresist < maximum_inertia_resist) {
				if (cellinertiaresist <
				    minimum_inertia_resist) {
					cellinertiaresist =
						minimum_inertia_resist;
				}
			} else {
				cellinertiaresist = maximum_inertia_resist;
			}
			C_PROFILE(c, t, nv) =
				cellinertiaresist *
				resist_scaler; /* Scaler applied to cell resistance */
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
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-inertia-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-inertia-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-inertia-resist")) {
		maximum_inertia_resist =
			(RP_Get_Real("vsi/maximum-inertia-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-inertia-resist")) {
		minimum_inertia_resist =
			(RP_Get_Real("vsi/minimum-inertia-resist"));
	}

	initial_inertia_resistance = Initial_Inertia_Resistance();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellinertiaresist = Cell_Inertia_Resistance(
				cellporo,
				initial_inertia_resistance); /* Blake-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellinertiaresist < maximum_inertia_resist) {
				if (cellinertiaresist <
				    minimum_inertia_resist) {
					cellinertiaresist =
						minimum_inertia_resist;
				}
			} else {
				cellinertiaresist = maximum_inertia_resist;
			}
			C_PROFILE(c, t, nv) =
				cellinertiaresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 5) = cellinertiaresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 5);
		}
	}
	end_c_loop(c, t)
}


#endif // GOB_UDF_INERTIA_H
