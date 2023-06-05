#ifndef GOB_UTILS_H
#define GOB_UTILS_H

// double Cell_Inertia_Resistance(double cellporo,
// 			       double initial_inertia_resistance);

// double Initial_Inertia_Resistance();

// double Initial_Perm();

// double Cell_Resistance(double cellporo, double initial_permeability);

// /* 
// !!!!!!!!!!!!!!!!!!!	SCHEME MESSAGES     !!!!!!!!!!!!!!!!!!!
// */
// void Print_Scheme_Variable_Settings();

/**
 * @brief Clamps a floating-point value to be positive (>= 0).
 * 
 * @param [in] num value to be clamped
 * @return [double] value >= 0
 */
double clamp_positive(const double num);

#endif // GOB_UTILS_H
