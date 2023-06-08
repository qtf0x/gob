/**
 * @file fits.h
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
 * @brief Function declarations for equation fits of all three mine models. 
 * Each mesh zone is fit using a separate function. Mines modeled in FLAC3D, 
 * equations fit in MATLAB.
 */

#ifndef GOB_FITS_H
#define GOB_FITS_H

/*******************************************************************************
 * TRONA MINE
 * SUB CRITICAL PANEL
 * 3 REGIONS - REFLECTED FOR 6 TOTAL
*******************************************************************************/

double sub_critical_trona_working_face_corner(const double x, const double y);
double sub_critical_trona_mid_panel_gateroad(const double x);
double sub_critical_trona_startup_room_corner(const double x, const double y);

/*******************************************************************************
 * MINE E
 * SUPER CRITICAL PANEL
 * 6 REGIONS - REFLECTED FOR 9 TOTAL
*******************************************************************************/

double super_critical_mine_E_startup_room_center(const double x,
						 const double y);
double super_critical_mine_E_mid_panel_center(const double x, const double y);
double super_critical_mine_E_working_face_center(const double x,
						 const double y);
double super_critical_mine_E_working_face_corner(const double x,
						 const double y);
double super_critical_mine_E_startup_room_corner(const double x,
						 const double y);
double super_critical_mine_E_mid_panel_gateroad(const double x, const double y);

/*******************************************************************************
 * MINE C
 * SUPER CRITICAL PANEL
 * 6 REGIONS - REFLECTED FOR 9 TOTAL
*******************************************************************************/

double super_critical_mine_C_startup_room_center(const double x,
						 const double y);
double super_critical_mine_C_mid_panel_center(const double x, const double y);
double super_critical_mine_C_working_face_center(const double x,
						 const double y);
double super_critical_mine_C_working_face_corner(const double x,
						 const double y);
double super_critical_mine_C_startup_room_corner(const double x,
						 const double y);
double super_critical_mine_C_mid_panel_gateroad(const double x, const double y);

#endif // GOB_FITS_H
