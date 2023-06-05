/**
 * @file udf_adjust.h
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
 * @brief Definitions for Ansys Fluent "adjust" User-Defined Functions.
 */

#ifndef GOB_UDF_ADJUST_H
#define GOB_UDF_ADJUST_H

#include "udf.h" // Fluent macros

extern int ite; // number of iterations elapsed

/**
 * @brief Updates iteration count. Executes at every iteration and is called at 
 * the beginning of every iteration before transport equations are solved.
 * 
 * @param [in] Domain *d pointer to the domain over which the adjust function 
 * is to be applied (passed by the Ansys Fluent solver)
 */
DEFINE_ADJUST(demo_calc, d)
{
	++ite;
}

#endif // GOB_UDF_ADJUST_H
