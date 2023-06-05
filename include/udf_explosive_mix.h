#ifndef GOB_UDF_EXPLOSIVE_MIX_H
#define GOB_UDF_EXPLOSIVE_MIX_H

#include "udf.h" // Fluent macros

/*
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	!!!!!!!!!!!!!!! EXPLOSIVE METHANE-AIR MIXTURES !!!!!!!!!!!!!
	!!!!!!!EGZ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!EGZ!!!!!!
	!!!!!!!!!!!! STORES in (user-define-memory 2)   !!!!!!!!!!!!
	!!!!!!!!!!!!!!!!!!!!!!          udm-2           !!!!!!!!!!!!
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

DEFINE_ON_DEMAND(calc_explosive_mix_NEWER)
{
	Domain *d;
	Thread *t;
	cell_t c;

	real px;
	real py;
	real u;
	real v;
	real u1;
	real v1;
	real w;
	real Y_CH4, Y_O2, Y_N2, MW_CH4, MW_O2, MW_N2, MW_Mix, X_CH4, X_O2;
	real explode;
	d = Get_Domain(1);
	thread_loop_c(t, d)
	{
		begin_c_loop(c, t)
		{
			/* Y_X = Mass Fraction of Species X  || X_X = Mole Fraction of Species X */
			Y_CH4 = C_YI(c, t, 0);
			Y_O2 = C_YI(c, t, 1);
			Y_N2 = 1.0 - Y_CH4 - Y_O2;
			MW_CH4 = 16.043;
			MW_O2 = 31.9988;
			MW_N2 = 28.0134;
			MW_Mix = 1 / (Y_CH4 / MW_CH4 + Y_O2 / MW_O2 + Y_N2 / MW_N2);
			X_CH4 = (Y_CH4 * MW_Mix) / MW_CH4; /* X = Mole Fraction of X */
			X_O2 = (Y_O2 * MW_Mix) / MW_O2;
			px = X_CH4;
			py = X_O2;
			u = 0.8529 * px + 0.0606; /* Near Explosive to Explosive Slope */
			v = -0.21 * px + 0.21; /* Upper Explosive Limit  */
			u1 = 0.8864 * px + 0.0445; /* Near Explosive to Requires Air Slope */
			v1 = -1.3929 * px + 0.195;
			w = v1;
			/*v1=-1.2647*px+0.1771; Cyan to Yellow Slope Transition */

			/*w=-1.8545*px+0.2095; Continuation of Slope Oxygen Rich to Oxygen Poor */

			/* Explosive Zone - RED */
			if (py > u && px > 0.055 && py < v) {
				explode = 1.0E0;
				C_UDMI(c, t, 2) = explode;
			}
			/* Near Explosive Zone - ORANGE */
			else if (py > u1 && px > 0.04 && py < v) {
				explode = 0.81E0;
				C_UDMI(c, t, 2) = explode;
			}
			/* Fuel Rich Inert - YELLOW */
			else if (py < u1 && py > v1 && px > 0.055) {
				explode = 0.66E0;
				C_UDMI(c, t, 2) = explode;
			}
			/* Oxygen Lean Inert - Green A  */
			else if (py < v1 && px > 0.04) {
				explode = 0.48E0;
				C_UDMI(c, t, 2) = explode;
			}
			/* Oxygen Lean Inert - DARK  GREEN  */
			else if (py < 0.08 && px < 0.04) {
				explode = 0.0E0;
				C_UDMI(c, t, 2) = explode;
			}
			/* Oxygen Lean Inert - Green B  */
			else if (py < w && px < 0.04) {
				explode = 0.48E0;
				C_UDMI(c, t, 2) = explode;
			}
			/* Oxygen Rich Inert - CYAN */
			else if (py > w) {
				explode = 0.27E0;
				C_UDMI(c, t, 2) = explode;
			}
			/* Explosive Zone - DARK BLUE */
			else {
				explode = 2.66E0;
				C_UDMI(c, t, 2) = explode;
			}
		}
		end_c_loop(c, t)
	}
}

/*
	_________________________________________
	|                                       |
	|   Explosive Intergral                 |
	|   Must use Volume-Integral-report     |
	|                                       |
	|   STORES in (user-define-memory 3)    |
	|            udm-3                      |
	-----------------------------------------
*/

DEFINE_ON_DEMAND(calc_explosive_integral_gob)
/* For use in the gob - currently porosity in strata is user-defined-variable and this will return a value of zero in the strata
because the value of the porosity stored in C_UDMI(c,t,1) is zero. This could be changed by patching a value into C_UDMI for the strata.
For strata use: calc_explosive_integral*/
{
	Domain *d;
	Thread *t;
	cell_t c;
	d = Get_Domain(1);
	thread_loop_c(t, d)
	{
		begin_c_loop(c, t)
		{
			if (fabs(C_UDMI(c, t, 2) - 1) < 1e-6) {
				/* Assign marker value for cell volume that is explosive */

				C_UDMI(c, t, 3) = 1 - C_UDMI(c, t, 1);
			} /* Report Volume-Volume-Integral udm-3 */

			/* Cell_Volume*Cell_Porosity*1.000e0 = The explosive volume reported */
		}
		end_c_loop(c, t)
	}
}

DEFINE_ON_DEMAND(reset_explosive_integral)
{
	/* Required to execute on transient runs - otherwise explosive volume is additive */
	Domain *d;
	Thread *t;
	cell_t c;
	d = Get_Domain(1);
	thread_loop_c(t, d)
	{
		begin_c_loop(c, t)
		{
			C_UDMI(c, t, 3) = 0.00E0;
		}
		end_c_loop(c, t)
	}
}

#endif // GOB_UDF_EXPLOSIVE_MIX_H
