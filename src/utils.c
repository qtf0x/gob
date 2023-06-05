#include "utils.h" // Fluent macros

// double Cell_Inertia_Resistance(double cellporo,
// 			       double initial_inertia_resistance)
// {
// 	/* The Blake-Kozeny equation for flow through porous media was
// used to estimate the inertia resistance or C2 value in FLUENT of the gob as follows:
// C2 = 3.5/Dp * (1-n)/n^3
// where the initial inertia resistance is base on the intial porosity of the rock, and 
// the change is calcuated as:
// C2 = C2_inital * (1-n)/n^3
// */

// 	return (initial_inertia_resistance * (1.0000000 - cellporo) /
// 		(cellporo * cellporo * cellporo));
// }

// double Initial_Inertia_Resistance()
// {
// 	/* C2_initial=3.5/Dp * (1-n)/n^3 
// 	Dp is the mean particle diameter, and n is porosity (%) 
// 	mean particle diameter from Pappas & Mark 1993 = 0.2 meters, Kozeny constant 180.  */

// 	double initial_porosity = 0.2577800000000;
// 	if (RP_Variable_Exists_P(
// 		    "vsi/initial-poroisty")) { /* Returns true if the variable exists */
// 		initial_porosity = (RP_Get_double("vsi/initial-porosity"));
// 	}

// 	return (3.5 / 0.2000000 * (1.000000000000 - initial_porosity) /
// 		(initial_porosity * initial_porosity * initial_porosity));
// }

// double Initial_Perm()
// {
// 	/* K_o=n^3 /(180 * (1-n)^2) * d^2
// 	where K_o is permeability (miliDarcies), n is porosity (%) 
// 	and d is the mean particle diameter (meters).
// 	mean particle diameter from Pappas & Mark 1993 = 0.2 meters, Kozeny constant 180.  */

// 	double initial_porosity = 0.2577800000000;
// 	if (RP_Variable_Exists_P(
// 		    "vsi/initial-poroisty")) { /* Returns true if the variable exists */
// 		initial_porosity = (RP_Get_double("vsi/initial-porosity"));
// 	}

// 	return (initial_porosity * initial_porosity * initial_porosity /
// 		(180.0000000000 * (1.0000000000 - initial_porosity) *
// 		 (1.000000000 - initial_porosity)) *
// 		0.20000000000 * 0.20000000000);
// }

// double Cell_Resistance(double cellporo, double initial_permeability)
// {
// 	/* The Carman-Kozeny equation for flow through porous media was
// used to estimate the permeability of the gob (K) as follows: K ? n3
// K = K_o / 0.241 *(n^3/(1-n)^2)
// Where K_o is the base permeability of the broken rock at the
// maximum porosity, and n is the porosity. The value of K_o was taken as 1x10^6 miliDarcies 
// which places it the as "open jointed rock" range according to
// Hoek and Bray (1981). */
// 	double cellperm;
// 	cellperm = initial_permeability / 0.24100000000 *
// 		   (cellporo * cellporo * cellporo) /
// 		   ((1.00000000 - cellporo) * (1.0000000 - cellporo));
// 	return (1.0 / cellperm);
// }

// void Print_Scheme_Variable_Settings()
// {
// 	if (RP_Variable_Exists_P(
// 		    "vsi/porosity-scaler")) { /* Returns true if the variable exists */
// 		Message("Porosity scaler: %g\n",
// 			RP_Get_double("vsi/porosity-scaler"));
// 	} /* else default or manual set above is used */
// 	else {
// 		Message("Porosity scaler at default value: 1\n You may define it with TUI Command: (rp-var-define 'vsi/porosity-scaler VALUE 'double #f)\n");
// 	}

// 	if (RP_Variable_Exists_P(
// 		    "vsi/maximum-porosity")) { /* Returns true if the variable exists */
// 		Message("The starting maximum porosity behind shields starts at: %g\n",
// 			RP_Get_double("vsi/maximum-porosity"));
// 	} /* else default or manual set above is used */
// 	else {
// 		Message("The starting maximum porostiy is not set. Using default value: 0.40\n You may set it with TUI Command: (rp-var-define 'vsi/maximum-porosity VALUE 'double #f)\n");
// 	}

// 	if (RP_Variable_Exists_P(
// 		    "vsi/initial-porosity")) { /* Returns true if the variable exists */
// 		Message("Initial Porosity of host rock is set to: %g\n",
// 			RP_Get_double("vsi/initial-porosity"));
// 	} /* else default or manual set above is used */
// 	else {
// 		Message("Initial Porosity of host rock is set to default value: 0.257780\n You may set it with TUI Command: (rp-var-define 'vsi/initial-porosity VALUE 'double #f)\n");
// 	}

// 	if (RP_Variable_Exists_P(
// 		    "vsi/resist-scaler")) { /* Returns true if the variable exists */
// 		Message("Resistance Scaler of Carmen-Kozeny relationship is set to: %g\n",
// 			RP_Get_double("vsi/resist-scaler"));
// 	} /* else default or manual set above is used */
// 	else {
// 		Message("Resistance Scaler of Carmen-Kozeny relationship is set to default value: 1\n You may set it with TUI Command: (rp-var-define 'vsi/resist-scaler VALUE 'double #f)\n");
// 	}

// 	if (RP_Variable_Exists_P(
// 		    "vsi/maximum-resist")) { /* Returns true if the variable exists */
// 		Message("Maximum cropped resistance (before scaler) in the gob center is set to: %g\n",
// 			RP_Get_double("vsi/maximum-resist"));
// 	} /* else default or manual set above is used */
// 	else {
// 		Message("Maximum cropped resistane (before scaler) in the gob center is set to default value: 5.00000E6\n You may set it with TUI Command: (rp-var-define 'vsi/maximum-resist VALUE 'double #f)\n");
// 	}

// 	if (RP_Variable_Exists_P(
// 		    "vsi/minimum-resist")) { /* Returns true if the variable exists */
// 		Message("Minimum cropped resistance (before scaler) in the edges of the gob is set to: %g\n",
// 			RP_Get_double("vsi/minimum-resist"));
// 	} /* else default or manual set above is used */
// 	else {
// 		Message("Minimum cropped resistance (before scaler) in the edges of the gob is set to default value: 1.45000E6\n You may set it with TUI Command: (rp-var-define 'vsi/minimum-resist VALUE 'double #f)\n");
// 	}
// }

double clamp_positive(const double num)
{
	return (num < 0) ? 0 : num;
}
