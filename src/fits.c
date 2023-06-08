/**
 * @file fits.c
 * @author Dr. Richard C. Gilmore
 * @author Dr. Jonathan A. Marts
 * @author Dr. Dan Worrall Jr.
 * @author Emmaleigh Hawkins
 * @author Vincent Marias
 * @author Will Meinert
 * @author Daniel Nau
 *
 * @date 06/03/2023
 * @copyright Copyright (c) 2023 Dr. Richard C. Gilmore
 * 
 * @brief Function definitions for equation fits of all three mine models.
 */

#include <math.h> // for pow, exp

#include "fits.h"
#include "utils.h" // for clamp_positive

/* TRONA MINE FITS ************************************************************/

double sub_critical_trona_working_face_corner(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_2 = x * x; // x squared
	const double Y_2 = y * y; // y squared
	const double X_Y = x * y; // x * y

	// calculate change using coefficients from MATLAB
	const double VSI =
		pow(X_Y, 0.107302089705487) *
		(0.1477 - 0.812278751377339 * exp(-9.96978304250904 * y) * X_Y +
		 0.103507270969929 * exp(-688.057090793680 * X_Y) -
		 0.1738 * exp(-6.368 * y) + 0.1971 * X_2 * exp(-1.38 * X_2) +
		 13.6 * Y_2 * exp(-2890 * y) - 14.56 * x * exp(-47.01 * x) +
		 11.19 * x * exp(-7883 * X_2) + 0.07992 * exp(-2.155 * x) -
		 6.274 * y * exp(-99.58 * y) + 0.03141 * y * exp(-8.748 * Y_2));

	// expect only positive changes
	return clamp_positive(VSI);
}

double sub_critical_trona_mid_panel_gateroad(const double x)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_2 = x * x; // x squared

	// calculate change using coefficients from MATLAB
	const double VSI = 0.2031 + 0.007304 * x + 1.495 * x * exp(-19.69 * x) -
			   0.1661 * exp(-162.6 * X_2) -
			   0.1315 * x * exp(-7.204 * X_2) -
			   3.298 * X_2 * exp(-44.01 * X_2);

	// expect only positive changes
	return clamp_positive(VSI);
}

double sub_critical_trona_startup_room_corner(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_Y = x * y; // x * y

	// calculate change using coefficients from MATLAB
	const double VSI =
		pow(X_Y, 0.1007) *
		(0.1796 + 0.2762 * exp(-4.552 * y) * X_Y +
		 0.04375 * exp(-5.354 * X_Y) - 0.3093 * exp(-60.54 * x) -
		 0.2702 * exp(-50.36 * y) + 0.437 * x * x * exp(-2.728 * x));

	// expect only positive changes
	return clamp_positive(VSI);
}

/* MINE E FITS ****************************************************************/

double super_critical_mine_E_startup_room_center(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double Y_2 = y * y; // y squared

	// calculate change using coefficients from MATLAB
	const double VSI = 0.155394214 + x * x * (-0.004966014) +
			   0.142894504 * y * exp(-1.11507158 * Y_2) -
			   0.154156852 * exp(-994.6190264 * Y_2) -
			   0.165429282 * Y_2 * exp(-2.119029131 * Y_2);

	// expect only positive changes
	return clamp_positive(VSI);
}

double super_critical_mine_E_mid_panel_center(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_2 = x * x; // x squared

	// calculate change using coefficients from MATLAB
	const double VSI = 0.182881808 + 0.000219076 * y - 0.001701901 * X_2 -
			   0.003415753 * X_2 * x;

	// expect only positive changes
	return clamp_positive(VSI);
}

double super_critical_mine_E_working_face_center(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double Y_2 = y * y; // y squared

	// calculate change using coefficients from MATLAB
	const double VSI = 0.034705045 + x * x * (-0.007156676) +
			   0.392853454 * y * exp(-2.690847002 * Y_2) -
			   0.016570035 * exp(-290 * Y_2) +
			   0.206091545 * Y_2 * exp(-0.513740978 * Y_2);

	// expect only positive changes
	return clamp_positive(VSI);
}

double super_critical_mine_E_startup_room_corner(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_Y = x * y; // x * y

	// calculate change using coefficients from MATLAB
	const double VSI =
		pow(X_Y, 0.070680995) *
		(0.162003881 + 0.114056257 * exp(-2.060750448 * y) * X_Y +
		 0.027309527 * exp(-2.32002878 * X_Y) -
		 0.134663756 * exp(-8.323851432 * x) -
		 0.263467643 * exp(-50.02086538 * y) +
		 51.01309648 * x * x * exp(-24.66420708 * x));

	// expect only positive changes
	return clamp_positive(VSI);
}

double super_critical_mine_E_mid_panel_gateroad(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_2 = x * x; // x squared

	// calculate change using coefficients from MATLAB
	const double VSI = 0.10083973 + 0.05329973 * x + 0.000111875 * y +
			   0.715710581 * x * exp(-3.193027724 * x) -
			   0.100070375 * exp(-1200.384929 * X_2) -
			   0.151653961 * x * exp(-3.716593738 * X_2) -
			   0.378856069 * X_2 * exp(-16.20732696 * X_2);

	// expect only positive changes
	return clamp_positive(VSI);
}

double super_critical_mine_E_working_face_corner(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_2 = x * x; // x squared
	const double Y_2 = y * y; // y squared
	const double X_Y = x * y; // x * y

	// calculate change using coefficients from MATLAB
	const double VSI =
		pow(X_Y, 0.251307505) *
		(0.197539477 - 0.258183405 * exp(-2.062155525 * y) * X_Y +
		 0.02301539 * exp(-21.41498958 * X_Y) -
		 0.15928258 * exp(-10.01527015 * y) +
		 0.445654501 * X_2 * exp(-17.13983263 * X_2) +
		 4.68818221 * Y_2 * exp(-5.633256844 * y) -
		 13.90840849 * x * exp(-65.9273908 * x) +
		 0.772026679 * x * exp(-68.99785585 * X_2) +
		 34.5 * exp(-3200.000001 * x) +
		 0.263621861 * y * exp(-27.16257242 * y) -
		 0.255066042 * y * exp(-9.010678902 * Y_2));

	// expect only positive changes
	return clamp_positive(VSI);
}

/* MINE C FITS ****************************************************************/

double super_critical_mine_C_startup_room_center(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double Y_2 = y * y; // y squared

	// calculate change using coefficients from MATLAB
	const double VSI = 0.23446547 + x * x * (-0.007274502) +
			   0.21112871 * y * exp(-1.254341353 * Y_2) -
			   0.232563013 * exp(-986.7723584 * Y_2) -
			   0.288213205 * Y_2 * exp(-2.41029839 * Y_2);

	// expect only positive changes
	return clamp_positive(VSI);
}

double super_critical_mine_C_mid_panel_center(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_2 = x * x; // x squared

	// calculate change using coefficients from MATLAB
	const double VSI = 0.26928324 + 0.000607666 * y - 0.001387445 * X_2 -
			   0.005923021406 * X_2 * x;

	// expect only positive changes
	return clamp_positive(VSI);
}

double super_critical_mine_C_working_face_center(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double Y_2 = y * y; // y squared

	// calculate change using coefficients from MATLAB
	const double VSI = 0.054623275 + x * x * (-0.007156676) +
			   0.537305093 * y * exp(-3.725760322 * Y_2) -
			   0.028010127 * exp(-290 * Y_2) +
			   0.501978887 * Y_2 * exp(-0.911642577 * Y_2);

	// expect only positive changes
	return clamp_positive(VSI);
}

double super_critical_mine_C_working_face_corner(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_2 = x * x; // x squared
	const double Y_2 = y * y; // y squared
	const double X_Y = x * y; // x * y

	// calculate change using coefficients from MATLAB
	const double VSI =
		pow(X_Y, 0.162024335) *
		(0.262664371 - 0.253166473007042 * exp(-3.203358282 * y) * X_Y +
		 0.065491826 * exp(-228.3897538 * X_Y) -
		 0.243491669 * exp(-8.890275525 * y) -
		 0.01 * X_2 * exp(-47.29743004 * X_2) +
		 5.007983398 * Y_2 * exp(-5.70033048 * y) -
		 27.10582475 * x * exp(-73.60496053 * x) +
		 0.315580701 * x * exp(-51.89579568 * X_2) +
		 38.98661392 * exp(-3169.255209 * x) +
		 2.089424053 * y * exp(-30.00980383 * y) -
		 0.303675247 * y * exp(-7.38256233 * Y_2));

	// expect only positive changes
	return clamp_positive(VSI);
}

double super_critical_mine_C_startup_room_corner(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_Y = x * y; // x * y

	// calculate change using coefficients from MATLAB
	const double VSI =
		pow(X_Y, 0.072583782) *
		(0.222803703 + 0.217897624 * exp(-3.019723703 * y) * X_Y +
		 0.035000529 * exp(-4.519932999 * X_Y) -
		 0.213737696 * exp(-33.3990023 * x) -
		 0.399728137 * exp(-49.33761923 * y) +
		 0.36279155 * x * x * exp(-2.519206805 * x));

	// expect only positive changes
	return clamp_positive(VSI);
}

double super_critical_mine_C_mid_panel_gateroad(const double x, const double y)
{
	// factor expression in case compiler doesn't feel like doing it
	const double X_2 = x * x; // x squared

	// calculate change using coefficients from MATLAB
	const double VSI = 0.10083973 + 0.128284224 * x + 0.000603995 * y +
			   2.171935712 * x * exp(-4.062177714 * x) -
			   0.101220528 * exp(-1464.235434 * X_2) -
			   0.474820214 * x * exp(-5.389192365 * X_2) -
			   1.477162806 * X_2 * exp(-23.91528913 * X_2);

	// expect only positive changes
	return clamp_positive(VSI);
}
