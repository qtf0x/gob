/**
 * @file udf_vsi.h
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
 * @brief Definitions for Ansys Fluent User-Defined Functions used to calculate 
 * volumetric strain increment over various sections and types of gob panels.
 */

#ifndef GOB_UDF_VSI_H
#define GOB_UDF_VSI_H

/*******************************************************************************
 * TRONA MINE
 * SUB CRITIAL PANEL
*******************************************************************************/
void VSI_MINE_Trona_Stepped();

/*******************************************************************************
 * MINE C
 * SUPER CRITIAL PANEL
*******************************************************************************/
void VSI_MINE_C_Stepped();

/*******************************************************************************
 * MINE E
 * SUPER CRITIAL PANEL
*******************************************************************************/
void VSI_MINE_E_Stepped();

#endif // GOB_UDF_VSI_H
