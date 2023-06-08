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
 * @date 06/08/2023
 * @copyright Copyright (c) 2023 Dr. Richard C. Gilmore
 * 
 * @brief Definitions for Ansys Fluent User-Defined Functions used to calculate 
 * volumetric strain increment over various sections and types of gob panels.
 */

#ifndef GOB_UDF_VSI_H
#define GOB_UDF_VSI_H

#include "udf.h" // Fluent Macros, real typedef

/**
 * @brief 
 * 
 * @param [in] single_part_mesh is the mesh partitioned?
 * @param [in] panel_x_offset displacement to center of old panel
 * @param [in] panel_y_offset displacement to recovery room of old panel
 */
void vsi_trona_stepped(const bool single_part_mesh, const real panel_x_offset, const real panel_y_offset);

/**
 * @brief 
 * 
 * @param single_part_mesh 
 * @param panel_x_offset 
 * @param panel_y_offset 
 */
void vsi_mine_C_stepped(const bool single_part_mesh, const real panel_x_offset, const real panel_y_offset);

/**
 * @brief 
 * 
 * @param single_part_mesh 
 * @param panel_x_offset 
 * @param panel_y_offset 
 */
void vsi_mine_E_stepped(const bool single_part_mesh, const real panel_x_offset, const real panel_y_offset);

#endif // GOB_UDF_VSI_H
