#ifndef GOB_UTILS_H
#define GOB_UTILS_H

#include <stdbool.h>

#include "udf.h"

double Cell_Inertia_Resistance(double cellporo, double initial_inertia_resistance);

double Initial_Inertia_Resistance();

double Initial_Perm();

double Cell_Resistance(double cellporo, double initial_permeability);

/* 
!!!!!!!!!!!!!!!!!!!	SCHEME MESSAGES     !!!!!!!!!!!!!!!!!!!
*/
void Print_Scheme_Variable_Settings();

/**
 * @brief Calculate the dimensions of the thread with the given ID.
 * 
 * @param [in] t_id Fluent identifier of the thread to be queried
 * @param [out] width total width of the thread (x dimension)
 * @param [out] length total length of the thread (y dimension)
 * @param [out] min_x_out minimum x value of any cell in the thread
 * @param [out] max_x_out maximum x value of any cell in the thread
 * @param [out] min_y_out minimum y value of any cell in the thread
 * @param [out] max_y_out maximum y value of any cell in the thread
 */
void get_thread_dimensions(const int t_id, real *width, real *length, real *min_x_out, real *max_x_out, real *min_y_out,
			   real *max_y_out);

/**
 * @brief Determines approximate equality between floating point numbers. Use
 * this instead of native equality operator to avoid round-off errors related
 * to IEEE-754 floating point representation.
 * 
 * @param [in] num1 first value to compare
 * @param [in] num2 secound value to compare
 * @return [true] values are approximately equal
 * @return [false] values are not equal
 */
bool fequal(const double num1, const double num2);

/**
 * @brief Clamps a floating-point value to be positive (>= 0).
 * 
 * @param [in] num value to be clamped
 * @return [double] value >= 0
 */
double clamp_positive(const double num);

/**
 * @brief Clamps a floating-point value between some lower and upper bounds.
 * 
 * @param [in] num value to be clamped
 * @param [in] min lower bound (inclusive)
 * @param [in] max upper bound (inclusive)
 * @return [double] lower <= value <= upper
 */
double clamp(const double num, const double min, const double max);

#endif // GOB_UTILS_H
