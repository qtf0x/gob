#ifndef GOB_UDF_PERMEABILITY_H
#define GOB_UDF_PERMEABILITY_H

#include "udf.h" // Fluent Macros

#include "utils.h"

extern int ite;

/*
	-------------------------------------------------
	!   Permeability or in FLUENT the inverse of    !
	!   makes RESISTANCE profiles                   !
	!   One for each direction 1,2,3 or x,y,x       !
	!   and one for each fluid zone                 !
	!                                               !
	!   STORES in (user-define-memory 0)            !
	!				udm-0           !
	!-----------------------------------------------!
*/

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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
DEFINE_PROFILE(set_1perm_1_VSI, t, nv)
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_1perm_2_VSI, t, nv)
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_1perm_3_VSI, t, nv)
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
DEFINE_PROFILE(set_2perm_1_VSI, t, nv)
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_2perm_2_VSI, t, nv)
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_2perm_3_VSI, t, nv)
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
DEFINE_PROFILE(set_3perm_1_VSI, t, nv)
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_3perm_2_VSI, t, nv)
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}

DEFINE_PROFILE(set_3perm_3_VSI, t, nv)
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
	real minimum_resist =
		1.45000E5; /* equals 6.91e-6 1/m2 permeability */
	if (RP_Variable_Exists_P(
		    "vsi/maximum-porosity")) { /* Get scheme variable and assign it if it exists */
		V_v = (RP_Get_Real("vsi/maximum-porosity"));
	}
	if (RP_Variable_Exists_P("vsi/porosity-scaler")) {
		a = (RP_Get_Real("vsi/porosity-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/resist-scaler")) {
		resist_scaler = (RP_Get_Real("vsi/resist-scaler"));
	}
	if (RP_Variable_Exists_P("vsi/maximum-resist")) {
		maximum_resist = (RP_Get_Real("vsi/maximum-resist"));
	}
	if (RP_Variable_Exists_P("vsi/minimum-resist")) {
		minimum_resist = (RP_Get_Real("vsi/minimum-resist"));
	}

	initial_permeability = Initial_Perm();

	begin_c_loop(c, t)
	{
		C_CENTROID(x, c, t);
		if (ite <= 1) {
			cellporo =
				((V_v - C_UDMI(c, t, 4)) * a < 0) ?
					0 :
					(V_v - C_UDMI(c, t, 4)) *
						a; /* Limit lowest value of porosity to zero */
			cellresist = Cell_Resistance(
				cellporo,
				initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
			if (cellresist < maximum_resist) {
				if (cellresist < minimum_resist) {
					cellresist = minimum_resist;
				}
			} else {
				cellresist = maximum_resist;
			}
			C_PROFILE(c, t, nv) =
				cellresist *
				resist_scaler; /* Scaler applied to cell resistance */
			C_UDMI(c, t, 0) = cellresist * resist_scaler;
		}
		if (ite > 1) {
			C_PROFILE(c, t, nv) = C_UDMI(c, t, 0);
		}
	}
	end_c_loop(c, t)
}


#endif // GOB_UDF_PERMEABILITY_H
