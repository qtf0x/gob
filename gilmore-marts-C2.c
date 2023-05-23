/*
Underground Longwall Mine Equation Fits for the change in porosity (volumetric strain increment = VSI)
output from FLAC3D Itasca, Consulting Group, Inc. Model development and methods published by 
Jonathan A. Marts, Jürgen F. Brune, Richard C. Gilmore, Gregory E. Bogin Jr., and John W. Grubb.
February 2014. Dynamic Gob Response and Reservoir Properties for Active Longwall Coal Mines. 
Society of Mining, Metallurgy, and Exploration Symposium Annual Meeting and Exhibit Pre-print, Salt Lake City, UT. (submitted for peer review)

-- Colorado School of Mines -- 
Author implementation
Richard C. Gilmore & Dan Worrall Jr. & Jonathan A. Marts 

-- Uses with ANSYS FLUENT 15.0 -- 
Text User Interface Control Commands: see FLUENT UDF manual Section: 3.6 Scheme Macros
REQUIRED settings:
To set the full panel width and length for the equation to fit 
(rp-var-define 'vsi/panel-width 200 'real  #f) 
(rp-var-define 'vsi/panel-length 3078.48 'real  #f)
To set the offset from (x=0, y=0) at the center panel (x-directions) of the recovery room (y-direction) or active face.
For example the center of the panel is located at a negative x-direction of 341 meters.
(rp-var-define 'vsi/panel-xoffset -341 'real  #f)
and the recovery room of the panel is located at a positive y-direction of 940 meters.
(rp-var-define 'vsi/panel-yoffset 940 'real  #f)
To use multiple panels compile to libudf libraries and set the displacement variables to the correct value before excuting
Also, must assign each panel past the first to a new user-define-memory location
search and replace this code assignment "C_UDMI(c,t,4)" and increment to 5

OPTIONAL settings:
To set a Carman-Kozeny Equation Scaler relationship multiplier
(rp-var-define 'vsi/resist-scaler 10 'real  #f)
To set a Maximum resistance in the gob center that will crop the equation fit to this value
(rp-set-define 'vsi/maximum-resist 5.0e6 'real #f)
To set a Minimum resistance in the on outer edge of the gob that will crop the equation fit to this value
(rp-set-define 'vsi/minimum-resist 1.45e5 'real #f)

To set the Maximum Porosity in the Gob (0.4 to 0.5) 
(rp-var-define 'vsi/maximum-porosity 0.40 'real  #f)
To set the Initial Porosity of the Host Rock (Undisturbed rock)
(rp-var-define 'vsi/initial-porosity 0.2577800000000 'real  #f)
To set the Maximum Value of VSI change at the center of the panel
(rp-var-define 'vsi/maximum-vsi 0.40 'real  #f)

To display current values of a variable  
(%rpgetvar 'vsi/var-name)
To change value
(rpsetvar 'vsi/var-name VALUE)
*/

#include "udf.h"
# define domain_ID 2
#include "math.h"


/* PROTOTYPES DECLARED */
/* ==================== Mine E fits ======================*/
double SUPER_CRITICAL_MINE_E_STARTUP_CENTER(double x_loc, double y_loc);
double SUPER_CRITICAL_MINE_E_CENTER_PANEL(double x_loc, double y_loc);
double SUPER_CRITICAL_MINE_E_RECOVERY_CENTER(double x_loc, double y_loc);
double SUPER_CRITICAL_MINE_E_RECOVERY_GATEROADS(double x_loc, double y_loc);
double SUPER_CRITICAL_MINE_E_STARTUP_GATEROADS(double x_loc, double y_loc);
double SUPER_CRITICAL_MINE_E_CENTER_GATEROADS(double x_loc, double y_loc);
/*===================== Mine C fits ======================*/
double SUPER_CRITICAL_MINE_C_STARTUP_CENTER(double x_loc, double y_loc);
double SUPER_CRITICAL_MINE_C_CENTER_PANEL(double x_loc, double y_loc);
double SUPER_CRITICAL_MINE_C_RECOVERY_CENTER(double x_loc, double y_loc);
double SUPER_CRITICAL_MINE_C_RECOVERY_GATEROADS(double x_loc, double y_loc);
double SUPER_CRITICAL_MINE_C_STARTUP_GATEROADS(double x_loc, double y_loc);
double SUPER_CRITICAL_MINE_C_CENTER_GATEROADS(double x_loc, double y_loc);
/*---------------------- Trona fits ----------------------*/
double SUB_CRITICAL_Trona_RECOVERY_CORNER(double x_loc, double y_loc);
double SUB_CRITICAL_Trona_CENTER_GATEROAD(double x_loc);
double SUB_CRITICAL_Trona_STARTUP_CORNER (double x_loc, double y_loc);


double Cell_Inertia_Resistance(double cellporo, double initial_inertia_resistance);
double Initial_Inertia_Resistance();
double Initial_Perm();
double Cell_Resistance(double cellporo, double initial_permeability);
void Print_Scheme_Variable_Settings();

int ite=0;



double Cell_Inertia_Resistance(double cellporo, double initial_inertia_resistance)
{

/* The Blake-Kozeny equation for flow through porous media was
used to estimate the inertia resistance or C2 value in FLUENT of the gob as follows:
C2 = 3.5/Dp * (1-n)/n^3
where the initial inertia resistance is base on the intial porosity of the rock, and 
the change is calcuated as:
C2 = C2_inital * (1-n)/n^3
*/

return  ( initial_inertia_resistance*(1.0000000-cellporo)/(cellporo*cellporo*cellporo) );
}

double Initial_Inertia_Resistance()
{
	/* C2_initial=3.5/Dp * (1-n)/n^3 
	Dp is the mean particle diameter, and n is porosity (%) 
	mean particle diameter from Pappas & Mark 1993 = 0.2 meters, Kozeny constant 180.  */  

	double initial_porosity=0.2577800000000;
	if (RP_Variable_Exists_P("vsi/initial-poroisty") ){        /* Returns true if the variable exists */
		 initial_porosity=( RP_Get_Real("vsi/initial-porosity") );}

	return  (3.5/0.2000000 *(1.000000000000-initial_porosity) / (initial_porosity*initial_porosity*initial_porosity) );  
}


double Cell_Resistance(double cellporo, double initial_permeability)
{

/* The Carman-Kozeny equation for flow through porous media was
used to estimate the permeability of the gob (K) as follows: K ? n3
K = K_o / 0.241 *(n^3/(1-n)^2)
Where K_o is the base permeability of the broken rock at the
maximum porosity, and n is the porosity. The value of K_o was taken as 1x10^6 miliDarcies 
which places it the as "open jointed rock" range according to
Hoek and Bray (1981). */
double cellperm;
cellperm = initial_permeability /0.24100000000 *(cellporo*cellporo*cellporo)/((1.00000000-cellporo)*(1.0000000-cellporo));
return (1.0/cellperm);
}

double Initial_Perm()
{
	/* K_o=n^3 /(180 * (1-n)^2) * d^2
	where K_o is permeability (miliDarcies), n is porosity (%) 
	and d is the mean particle diameter (meters).
	mean particle diameter from Pappas & Mark 1993 = 0.2 meters, Kozeny constant 180.  */  

	double initial_porosity=0.2577800000000;
	if (RP_Variable_Exists_P("vsi/initial-poroisty") ){        /* Returns true if the variable exists */
		 initial_porosity=( RP_Get_Real("vsi/initial-porosity") );}

	return  (initial_porosity*initial_porosity*initial_porosity/(180.0000000000*(1.0000000000-initial_porosity)*(1.000000000-initial_porosity))*0.20000000000*0.20000000000);  }


/******************************************************************/
	
DEFINE_ADJUST(demo_calc,d)
{
	ite=ite+1;
}
/*******************************************************************/
/*
				Position-Dependent Porous Media  				   
				     USING VSI                     
                                                    SUB-CRITICAL   */
/*******************************************************************/


DEFINE_ON_DEMAND(VSI_MINE_Trona_Stepped)
{
  Domain *d;  Thread *t;  cell_t c;   real x[ND_ND]; /* Fluent location vectors */
   
  double x_loc_norm, y_loc_norm, x_loc, y_loc;
  double panelwidth=100; /* specified here as half-width */
  double panelength=3078.48;
  double panelxoffset=0.0;
  double panelyoffset=0.0;
  double VSI=0;
  double FUN1, FUN2;
  double maximum_vsi=0.22;
  double blendrangey=25, mix=0.5;
  double box[6]={0, 100, 0, 300, 600, 1000}; /* default panel sizing */
  d = Get_Domain(1);
/* Assign new panel size from scheme variable define with in FLUENT or use the default sizing above */
  if (RP_Variable_Exists_P("vsi/panel-width")){       /* Returns true if the variable exists */
	 panelwidth=( RP_Get_Real("vsi/panel-width") / 2 );
	 Message ("Panel width is: %g\n",panelwidth*2); }/* else default or manual set above is used */
  else{ Message("Panel Width not set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/panel-width VALUE 'real #f)\n" , panelwidth*2);	 }
  box[1]=panelwidth;
  
  if (RP_Variable_Exists_P("vsi/panel-length")){        
	 panelength=( RP_Get_Real("vsi/panel-length") );
	 Message ("Panel length is set to: %g\n reset value using (rpsetvar 'vsi/panel-length VALUE)\n",panelength); } 
  else{ Message("Panel Length not set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/panel-length VALUE'real #f)\n", panelength); }
  box[4]=panelength-400;
  box[5]=panelength;
/* Specify a maximum value of VSI for the change porosity from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/maximum-vsi")){        
	 maximum_vsi=( RP_Get_Real("vsi/maximum-vsi") );
	 Message ("The Maximum change in porosity from the maximum-porosity behind the face occurs \nat the center of the panel and is set to: %g\n",maximum_vsi); } /* else default or manual set above is used */
  else{ Message("Maximum change in porosity from the maximum-porosity behind the face occurs \nat the center of the panel and is NOT set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/maximum-vsi VALUE 'real #f)\n", maximum_vsi); }

/* Specify the displacement to the center of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-xoffset")){        
	 panelxoffset=( RP_Get_Real("vsi/panel-xoffset") );
	 Message ("The x-direction displacement to the center of the panel is set to: %g\n",panelxoffset); } /* else default or manual set above is used */
  else{ Message("The x-direction displacement to the center of the panel is to zero.\n You may set it with TUI Command: (rp-var-define 'vsi/panel-xoffset VALUE 'real #f)\n"); }
  /* Specify the displacement to the recovery room of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-yoffset")){        
	 panelyoffset=( RP_Get_Real("vsi/panel-yoffset") );
	 Message ("The y-direction displacement to the recovery room of the panel is set to: %g\n",panelyoffset); } /* else default or manual set above is used */
  else{ Message("The y-direction displacement to the recovery room of the panel is to zero.\n You may set it with TUI Command: (rp-var-define 'vsi/panel-yoffset VALUE 'real #f)\n"); }
  
  thread_loop_c(t,d){
  begin_c_loop(c,t){ 
    C_CENTROID(x,c,t) /* Get mesh cell location */
	/*     Scale each section of the model to the curve fits.
	UNITS in METERS                                                
	                                                               
	           TG  RECOVERY ROOM / ACTIVE FACE        HG           
	       Fit 100                  0                 100          
	       1000|---------------------------------------| 0  	 
	           |0                  1|1                0|        
	           |    3               |                  |	      
	           | Recovery           |                  |		 
	           | Gateroad           |                  |		 
	           |Exact-Size     Sub-Critial no-Expansion|   	 
	           |1                  1|1                1|         	 
	        600|---------------------------------------| 300       
	           |                    |                  |           
	           |                    |                  |           
	           |    2               |                  |           
	           |  Center            |                  |           
	           | Gateroad           |                  |           
	           |                    |      	           |           
	           | Expansion          |    		   |           
	           | Equation           |                  |        
	           |                    |                  |           
	           |                    |                  |           
	        190|---------------------------------------| 810       -
	           |1                  1|1                1|           
	           |    1               |                  |           
	           | Startup            |                  |           
	           | Gateroad           |                  |           
	           |Exact-Size          |   		   |	 	 
	           |0                  1|1    	          0|           
	          0|---------------------------------------| 1000      
	         -152.5      -52.5               0        92.5      +152.5  My Panel
	          box=[0 92.5 160 0 190 1010 1200]   [0 100 0 190 600 1000]
	MIN=144871.4  1/m^2	MAX=492170 1/m^2	*/
/* UPDATE VALUES */
	x_loc=fabs(x[0]-panelxoffset); /* Center of Panel is Zero and Mirrored */
	y_loc=(panelength+x[1])-panelyoffset; /* Shift FLUENT MESH to FLAC3D data Zero point at startup room for equations */
	if( x_loc>panelwidth ){ VSI=0;} /* limit VSI function to only within panel domain sizing */
	else{
		if (y_loc<0) {VSI=0;}
		else if(y_loc < box[3]-blendrangey){
			x_loc_norm=( -(x_loc-box[1] )/( box[1] ) ); /* NORMALIZE to equation */
			y_loc_norm=(y_loc/box[5]);
			VSI = SUB_CRITICAL_Trona_STARTUP_CORNER(x_loc_norm, y_loc_norm);	}		
		else if( (y_loc < ( box[3]+blendrangey) ) && (y_loc > (box[3]-blendrangey)) ){
			mix= -( (y_loc-box[3]-blendrangey)/ (2*blendrangey) );
			x_loc_norm=( -(x_loc-box[1] )/( box[1] ) );
			y_loc_norm=(y_loc/box[5]);
			FUN2 = SUB_CRITICAL_Trona_CENTER_GATEROAD(x_loc_norm);
			FUN1 = SUB_CRITICAL_Trona_STARTUP_CORNER(x_loc_norm, y_loc_norm);
			 VSI=(FUN1*(mix)+FUN2*(1-mix)); }
		else if ( (y_loc < (box[4]-blendrangey-20)) && ( y_loc > (box[3]+blendrangey)) ){
			x_loc_norm=( -(x_loc-box[1] )/( box[1] ) );
			VSI = SUB_CRITICAL_Trona_CENTER_GATEROAD(x_loc_norm); }
		else if ( (y_loc < (box[4]+blendrangey+20)) && (y_loc > (box[4]-blendrangey-20) )){
			x_loc_norm=( -(x_loc-box[1] )/( box[1] ) );
			FUN1 = SUB_CRITICAL_Trona_CENTER_GATEROAD(x_loc_norm);
			x_loc_norm=( -(x_loc-box[1])/( box[1]+40 ))+0.02;
			y_loc_norm=(-(y_loc-box[5])/(box[5]-box[4]))+.012;
			FUN2 = SUB_CRITICAL_Trona_RECOVERY_CORNER(x_loc_norm, y_loc_norm);
			mix= -( (y_loc-box[4]-blendrangey-20)/(2*blendrangey+40));
			VSI=(FUN1*(mix)+FUN2*(1-mix)); }
		else if( (y_loc < box[5]) && (y_loc > (box[4]+blendrangey+20)) ){ /* Remained of data points are in the recovery room 600-1000m */
			x_loc_norm=( -(x_loc-box[1])/( box[1]+40))+0.02;
			y_loc_norm=(-(y_loc-box[5])/(box[5]-box[4]))+.012;
			VSI = SUB_CRITICAL_Trona_RECOVERY_CORNER(x_loc_norm, y_loc_norm);	}
		else{ VSI=0; }
		}
	C_UDMI(c,t,4) = (VSI>maximum_vsi)?maximum_vsi:VSI; /* Assign VSI to user-defined-memory location after cropping to maximum change */
	}
  end_c_loop(c,t) }
Print_Scheme_Variable_Settings();
}

/*
	%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-
	%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-
									MINE C
	!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$
	!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$!$
*/


DEFINE_ON_DEMAND(VSI_MINE_C_Stepped)
{
  Domain *d;  Thread *t;  cell_t c;   real x[ND_ND]; /* Fluent location vectors */
   
  double x_loc_norm, y_loc_norm, x_loc, y_loc;
  double panelwidth=151.4856; /* specified here as half-width */
  double panelength=1000;
  double panelxoffset=0.0;
  double panelyoffset=0.0;
  double VSI=0;
  double FUN1, FUN2;
  double maximum_vsi=0.2623;
  double blendrange=15, blendrangey=25, mix=0.5;
  double box[7]={0, 100, 200, 0, 190, 700, 1000};
  d = Get_Domain(1);
/* Assign new panel size from scheme variable define with in FLUENT or use the default sizing above */
  if (RP_Variable_Exists_P("vsi/panel-width")){       /* Returns true if the variable exists */
	 panelwidth=( RP_Get_Real("vsi/panel-width") / 2 );
	 Message ("Panel width is: %g\n",panelwidth*2); }/* else default or manual set above is used */
  else{ Message("Panel Width not set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/panel-width VALUE 'real #f)\n" , panelwidth*2);	 }
  box[1]=panelwidth-100.0;
  box[2]=panelwidth;
  
  if (RP_Variable_Exists_P("vsi/panel-length")){        
	 panelength=( RP_Get_Real("vsi/panel-length") );
	 Message ("Panel length is set to: %g\n reset value using (rpsetvar 'vsi/panel-length VALUE)\n",panelength); } 
  else{ Message("Panel Length not set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/panel-length VALUE'real #f)\n", panelength); }
  box[5]=panelength-300;
  box[6]=panelength;

/* Specify a maximum value of VSI for the change porosity from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/maximum-vsi")){        
	 maximum_vsi=( RP_Get_Real("vsi/maximum-vsi") );
	 Message ("The Maximum change in porosity from the maximum-porosity behind the face occurs \nat the center of the panel and is set to: %g\n",maximum_vsi); } /* else default or manual set above is used */
  else{ Message("Maximum change in porosity from the maximum-porosity behind the face occurs at the center of the panel and is NOT set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/maxiumum-vsi VALUE 'real #f)\n", maximum_vsi); }

/* Specify the displacement to the center of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-xoffset")){        
	 panelxoffset=( RP_Get_Real("vsi/panel-xoffset") );
	 Message ("The x-direction displacement to the center of the panel is set to: %g\n",panelxoffset); } /* else default or manual set above is used */
  else{ Message("The x-direction displacement to the center of the panel is to zero.\n You may set it with TUI Command: (rp-var-define 'vsi/panel-xoffset VALUE 'real #f)\n"); }
  /* Specify the displacement to the recovery room of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-yoffset")){        
	 panelyoffset=( RP_Get_Real("vsi/panel-yoffset") );
	 Message ("The y-direction displacement to the recovery room of the panel is set to: %g\n",panelyoffset); } /* else default or manual set above is used */
  else{ Message("The y-direction displacement to the recovery room of the panel is to zero.\n You may set it with TUI Command: (rp-var-define 'vsi/panel-yoffset VALUE 'real #f)\n"); }
  
thread_loop_c(t,d)  {
  begin_c_loop(c,t)   { 
    C_CENTROID(x,c,t);
	
	/*     Scale each section of the model to the curve fits.             
									
	UNITS in METERS                                                            
	                                                                           
	           TG           RECOVERY ROOM / ACTIVE FACE        HG              
	       Fit 160        100             0            100        160          
	       1200|--------------------------------------------------| 0          
	           |0        1|                            |1        0|            
	           |    5     |    6                       |          |            
	           | Recovery |  Recovery                  |          |            
	           | Gateroad |  Center                    |          |            
	           |Exact-Size|   Super Critial Expansion  |          |            
	           |1         |                            |         1|            
	       1010|--------------------------------------------------| 190        
	           |          |                            |          |            
	           |          |                            |          |            
	           |    3     |    4                       |          |            
	           |  Center  | Center                     |          |            
	           | Gateroad | Panel                      |          |            
	           |          |                            |          |            
	           | Expansion| Expansion                  |          |            
	           | Equation | Equation                   |          |            
	           |          |                            |          |            
	           |          |                            |          |            
	        190|--------------------------------------------------| 1010       
	           |1         |                            |         1|            
	           |    1     |     2                      |          |            
	           | Startup  |  Startup                   |          |            
	           | Gateroad |  Center                    |          |            
	           |Exact-Size|   Super Critial Expansion  |          |            
	           |0        1|                            |1        0|            
	          0|--------------------------------------------------| 1200       
	         -152.5      -52.5               0        92.5      +152.5  My Panel
	          box=[0 92.5 160 0 190 1010 1200]   
	MIN=144871.4  1/m^2	MAX=492170 1/m^2	*/
	x_loc=fabs(x[0]-panelxoffset); /* Center of Panel is Zero and Mirrored */
	y_loc=(panelength+x[1])-panelyoffset; /* Shift FLUENT MESH to FLAC3D data Zero point at startup room for equations */
	if( x_loc>panelwidth ){ VSI=0;} /* limit VSI function to only within panel domain sizing */
	else{
		if(x_loc < (box[1]-blendrange) ) {
			if (y_loc <0) {VSI=0;}
			else if (y_loc < box[4]-blendrangey-15){
				x_loc_norm=( -(x_loc-box[1]+20 )/( box[1] ) ); /* NORMALIZE to equation */
				y_loc_norm=(y_loc/box[4]);
				VSI =SUPER_CRITICAL_MINE_C_STARTUP_CENTER(x_loc_norm, y_loc_norm);	}
			else if ((y_loc > (box[4]-blendrangey-15) ) && (y_loc < box[4]+blendrangey-15)) {
				mix= ( (y_loc-blendrangey-15)/ (2*blendrangey+30) ) ;
				x_loc_norm=( -(x_loc-box[1] )/( box[1] ) ); /* NORMALIZE to equation */
				y_loc_norm=(box[4]);
				FUN1 =SUPER_CRITICAL_MINE_C_STARTUP_CENTER(x_loc_norm, y_loc_norm);
				x_loc_norm=( -(x_loc-box[1])/( box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN2 = SUPER_CRITICAL_MINE_C_CENTER_PANEL(x_loc_norm, y_loc_norm);	
				VSI=(FUN2*(mix)+FUN1*(1-mix)); }
			else if(( y_loc <  (box[5]-blendrangey-15 )) && ( y_loc > (box[4]+blendrangey-15) ) ){
				x_loc_norm=( -(x_loc-box[1]+10)/( box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				VSI = SUPER_CRITICAL_MINE_C_CENTER_PANEL(x_loc_norm, y_loc_norm);	}
			else if ((y_loc > (box[5]-blendrangey-15) ) && (y_loc < box[5]+blendrangey+15)) {
				mix = -((y_loc-box[5]-blendrangey-15)/(2*blendrangey+30));
				x_loc_norm=( -(x_loc-box[1])/( box[1] ));
				y_loc_norm=( (y_loc-box[4]-100)/( box[5]-box[4]) );
				FUN1 = SUPER_CRITICAL_MINE_C_CENTER_PANEL(x_loc_norm, y_loc_norm);	
				x_loc_norm=( (x_loc-box[1]+blendrange+15)/( box[1] ));
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				FUN2 = SUPER_CRITICAL_MINE_C_RECOVERY_CENTER(x_loc_norm, y_loc_norm);	
				VSI=(FUN1*(mix)+FUN2*(1-mix));	}
			else if( (y_loc< box[6]) && ( y_loc > (box[5]+blendrangey-15)) ){  /* Remained of data points are in the recovery room 600-1000m */
				x_loc_norm=( (x_loc-box[1]+blendrange+15)/( box[1] ));
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				VSI = SUPER_CRITICAL_MINE_C_RECOVERY_CENTER(x_loc_norm, y_loc_norm);	}
			else {VSI=0;}
		}
		else if ( x_loc <= (box[1]+blendrange)){
			mix = ( (x_loc-box[1]+blendrange)/(2*blendrange));
			if (y_loc <0) {VSI=0;}
			else if(y_loc < box[4]){
				x_loc_norm=( -(x_loc-box[1] )/( box[1] ) ); /* NORMALIZE to equation */
				y_loc_norm=(y_loc/box[4]);
				FUN1 =SUPER_CRITICAL_MINE_C_STARTUP_CENTER(x_loc_norm, y_loc_norm);
				x_loc_norm=( 1-(x_loc-box[1] )/( box[2]-box[1] ) );
				y_loc_norm=(y_loc/box[4]);
				FUN2 = SUPER_CRITICAL_MINE_C_STARTUP_GATEROADS(x_loc_norm, y_loc_norm);
				VSI=( FUN2*(mix)+FUN1*(1-mix) ); }
			else if(y_loc <= box[5] ){
				x_loc_norm=( -(x_loc-box[1] )/( box[1] ) );
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN1 = SUPER_CRITICAL_MINE_C_CENTER_PANEL(x_loc_norm, y_loc_norm);
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN2 = SUPER_CRITICAL_MINE_C_CENTER_GATEROADS(x_loc_norm, y_loc_norm);
				VSI=(FUN2*(mix)+FUN1*(1-mix)); }
			else if (y_loc < box[6]){
				x_loc_norm=( (x_loc-(box[1]-blendrange) )/( box[1]+blendrange ));
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				FUN1 = SUPER_CRITICAL_MINE_C_RECOVERY_CENTER(x_loc_norm, y_loc_norm);	
				x_loc_norm=( 1-(x_loc-(box[1]))/( box[2]-box[1] ) );
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				FUN2 = SUPER_CRITICAL_MINE_C_RECOVERY_GATEROADS(x_loc_norm, y_loc_norm);	
				VSI=( FUN2*(mix)+FUN1*(1-mix) ); 	} 
			else {VSI=0;}	
		}
			
		else{
			if (y_loc <0) {VSI=0;}
			else if( y_loc < box[4]-blendrangey){
				x_loc_norm= ( 1-(x_loc-box[1] )/( box[2]-box[1] ) );
				y_loc_norm=(y_loc/box[4]);
				VSI = SUPER_CRITICAL_MINE_C_STARTUP_GATEROADS(x_loc_norm, y_loc_norm); }
			else if ((y_loc > (box[4]-blendrangey) ) && (y_loc < box[4]+blendrangey)) {
				mix = ((y_loc-box[4]+blendrangey)/(2*blendrangey));
				x_loc_norm= ( 1-(x_loc-box[1] )/( box[2]-box[1] ) );
				y_loc_norm=(y_loc/box[4]);
				FUN1 = SUPER_CRITICAL_MINE_C_STARTUP_GATEROADS(x_loc_norm, y_loc_norm);
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN2 = SUPER_CRITICAL_MINE_C_CENTER_GATEROADS(x_loc_norm, y_loc_norm);
				VSI=(FUN2*(mix)+FUN1*(1-mix)); 	}
			else if(( y_loc <  (box[5]-blendrangey-20 )) && ( y_loc > (box[4]+blendrangey) ) ){
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				VSI = SUPER_CRITICAL_MINE_C_CENTER_GATEROADS(x_loc_norm, y_loc_norm); }	
			else if ((y_loc > (box[5]-blendrangey-20) ) && (y_loc < box[5]+blendrangey+20)) {
				mix = ((y_loc-box[5]+blendrangey)/(2*blendrangey));
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN1 = SUPER_CRITICAL_MINE_C_CENTER_GATEROADS(x_loc_norm, y_loc_norm);
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ) );
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				FUN2 = SUPER_CRITICAL_MINE_C_RECOVERY_GATEROADS(x_loc_norm, y_loc_norm);
				VSI= (FUN2*(mix)+FUN1*(1-mix)); 	} 
			else if (y_loc<panelength){
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ) );
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				VSI = SUPER_CRITICAL_MINE_C_RECOVERY_GATEROADS(x_loc_norm, y_loc_norm); }
			else {VSI=0;}
		} 
	}
  VSI=(VSI<0.0)?0:VSI;
  VSI=(VSI>maximum_vsi)?maximum_vsi:VSI;
 C_UDMI(c,t,4) = VSI;
} 
end_c_loop(c,t) }
Print_Scheme_Variable_Settings();
}


/*
	+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_
	----------------------------------------------
			MINE E
	----------------------------------------------
	+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_
*/


DEFINE_ON_DEMAND(VSI_MINE_E_Stepped)
{
  Domain *d;  Thread *t;  cell_t c;   real x[ND_ND]; /* Fluent location vectors */
   
  double x_loc_norm, y_loc_norm, x_loc, y_loc;
  double panelwidth=151.4856; /* specified here as half-width */
  double panelength=1000;
  double panelxoffset=0.0;
  double panelyoffset=0.0;
  double VSI=0;
  double FUN1, FUN2;
  double maximum_vsi=0.179;
  double blendrange=20, blendrangey=20, mix=0.5;
  double box[7]={0, 100, 200, 0, 190, 700, 1000};
  d = Get_Domain(1);
/* Assign new panel size from scheme variable define with in FLUENT or use the default sizing above */
  if (RP_Variable_Exists_P("vsi/panel-width")){       /* Returns true if the variable exists */
	 panelwidth=( RP_Get_Real("vsi/panel-width") / 2 );
	 Message ("Panel width is: %g\n",panelwidth*2); }/* else default or manual set above is used */
  else{ Message("Panel Width not set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/panel-width VALUE 'real #f)\n" , panelwidth*2);	 }
  box[1]=panelwidth-100.0;
  box[2]=panelwidth;
  
  if (RP_Variable_Exists_P("vsi/panel-length")){        
	 panelength=( RP_Get_Real("vsi/panel-length") );
	 Message ("Panel length is set to: %g\n reset value using (rpsetvar 'vsi/panel-length VALUE)\n",panelength); } 
  else{ Message("Panel Length not set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/panel-length VALUE'real #f)\n", panelength); }
  box[5]=panelength-300;
  box[6]=panelength;

/* Specify a maximum value of VSI for the change porosity from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/maximum-vsi")){        
	 maximum_vsi=( RP_Get_Real("vsi/maximum-vsi") );
	 Message ("The Maximum change in porosity from the maximum-porosity behind the face occurs \nat the center of the panel and is set to: %g\n",maximum_vsi); } /* else default or manual set above is used */
  else{ Message("Maximum change in porosity from the maximum-porosity behind the face occurs at the center of the panel and is NOT set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/maxiumum-vsi VALUE 'real #f)\n", maximum_vsi); }

/* Specify the displacement to the center of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-xoffset")){        
	 panelxoffset=( RP_Get_Real("vsi/panel-xoffset") );
	 Message ("The x-direction displacement to the center of the panel is set to: %g\n",panelxoffset); } /* else default or manual set above is used */
  else{ Message("The x-direction displacement to the center of the panel is to zero.\n You may set it with TUI Command: (rp-var-define 'vsi/panel-xoffset VALUE 'real #f)\n"); }
  /* Specify the displacement to the recovery room of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-yoffset")){        
	 panelyoffset=( RP_Get_Real("vsi/panel-yoffset") );
	 Message ("The y-direction displacement to the recovery room of the panel is set to: %g\n",panelyoffset); } /* else default or manual set above is used */
  else{ Message("The y-direction displacement to the recovery room of the panel is to zero.\n You may set it with TUI Command: (rp-var-define 'vsi/panel-yoffset VALUE 'real #f)\n"); }  
thread_loop_c(t,d)  {
  begin_c_loop(c,t)   { 
    C_CENTROID(x,c,t);
	
	/*     Scale each section of the model to the curve fits.             
									
	UNITS in METERS                                                            
	                                                                           
	           TG           RECOVERY ROOM / ACTIVE FACE        HG              
	       Fit 160        100             0            100        160          
	       1200|--------------------------------------------------| 0          
	           |0        1|                            |1        0|            
	           |    5     |    6                       |          |            
	           | Recovery |  Recovery                  |          |            
	           | Gateroad |  Center                    |          |            
	           |Exact-Size|   Super Critial Expansion  |          |            
	           |1         |                            |         1|            
	       1010|--------------------------------------------------| 190        
	           |          |                            |          |            
	           |          |                            |          |            
	           |    3     |    4                       |          |            
	           |  Center  | Center                     |          |            
	           | Gateroad | Panel                      |          |            
	           |          |                            |          |            
	           | Expansion| Expansion                  |          |            
	           | Equation | Equation                   |          |            
	           |          |                            |          |            
	           |          |                            |          |            
	        190|--------------------------------------------------| 1010       
	           |1         |                            |         1|            
	           |    1     |     2                      |          |            
	           | Startup  |  Startup                   |          |            
	           | Gateroad |  Center                    |          |            
	           |Exact-Size|   Super Critial Expansion  |          |            
	           |0        1|                            |1        0|            
	          0|--------------------------------------------------| 1200       
	         -152.5      -52.5               0        92.5      +152.5  My Panel
	          box=[0 92.5 160 0 190 1010 1200]   
	MIN=144871.4  1/m^2	MAX=492170 1/m^2	*/
	x_loc=fabs(x[0]-panelxoffset); /* Center of Panel is Zero and Mirrored */
	y_loc=(panelength+x[1])-panelyoffset; /* Shift FLUENT MESH to FLAC3D data Zero point at startup room for equations */
	if( x_loc>panelwidth ){ VSI=0;} /* limit VSI function to only within panel domain sizing */
	 
	else{
		if(x_loc < (box[1]-blendrange) ) {
			if (y_loc <0) {VSI=0;}
			else if (y_loc < box[4]-blendrangey-15){
				x_loc_norm=( -(x_loc-box[1]+20 )/( box[1] ) ); /* NORMALIZE to equation */
				y_loc_norm=(y_loc/box[4]);
				VSI =SUPER_CRITICAL_MINE_E_STARTUP_CENTER(x_loc_norm, y_loc_norm);	}
			else if ((y_loc > (box[4]-blendrangey-15) ) && (y_loc < box[4]+blendrangey-15)) {
				mix=( (y_loc-(box[4]+blendrangey-15))/ (2*blendrangey) );
				x_loc_norm=( -(x_loc-box[1]+20 )/( box[1] ) ); /* NORMALIZE to equation */
				y_loc_norm=(y_loc/box[4]);
				FUN1 =SUPER_CRITICAL_MINE_E_STARTUP_CENTER(x_loc_norm, y_loc_norm);
				x_loc_norm=( -(x_loc-box[1]+10)/( box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN2 = SUPER_CRITICAL_MINE_E_CENTER_PANEL(x_loc_norm, y_loc_norm);	
				VSI=(FUN2*(mix)+FUN1*(1-mix)); }
			else if(( y_loc <  (box[5]-blendrangey-15 )) && ( y_loc > (box[4]+blendrangey-15) ) ){
				x_loc_norm=( -(x_loc-box[1]+10)/( box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				VSI = SUPER_CRITICAL_MINE_E_CENTER_PANEL(x_loc_norm, y_loc_norm);	}
			else if ((y_loc > (box[5]-blendrangey-15) ) && (y_loc < box[5]+blendrangey-15)) {
				mix = -((y_loc-(box[5]+blendrangey-15))/(2*blendrangey));
				x_loc_norm=(-(x_loc-box[1])/( box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN1 = SUPER_CRITICAL_MINE_E_CENTER_PANEL(x_loc_norm, y_loc_norm);	
				x_loc_norm=( (x_loc-box[1]+blendrange+15)/( box[1] ));
				y_loc_norm=( 1-(y_loc-box[5])/(box[6]-box[5]));
				FUN2 = SUPER_CRITICAL_MINE_E_RECOVERY_CENTER(x_loc_norm, y_loc_norm);	
				VSI=(FUN1*(mix)+FUN2*(1-mix)); 	}
			else if( (y_loc< box[6]) && ( y_loc > (box[5]+blendrangey-15)) ){  /* Remained of data points are in the recovery room 600-1000m */
				x_loc_norm=( (x_loc-box[1]+blendrange+15)/( box[1] ));
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				VSI = SUPER_CRITICAL_MINE_E_RECOVERY_CENTER(x_loc_norm, y_loc_norm);	}
			else {VSI=0;}
		}
		else if ( x_loc <= (box[1]+blendrange)){
			mix = ( (x_loc-box[1]+blendrange)/(2*blendrange));
			if (y_loc <0) {VSI=0;}
			else if(y_loc < box[4]){
				x_loc_norm=( -(x_loc-box[1] )/( box[1] ) ); /* NORMALIZE to equation */
				y_loc_norm=(y_loc/box[4]);
				FUN1 =SUPER_CRITICAL_MINE_E_STARTUP_CENTER(x_loc_norm, y_loc_norm);
				x_loc_norm=( 1-(x_loc-box[1] )/( box[2]-box[1] ) );
				y_loc_norm=(y_loc/box[4]);
				FUN2 = SUPER_CRITICAL_MINE_E_STARTUP_GATEROADS(x_loc_norm, y_loc_norm);
				VSI=( FUN2*(mix)+FUN1*(1-mix) ); }
			else if(y_loc <= box[5] ){
				x_loc_norm=( -(x_loc-box[1] )/( box[1] ) );
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN1 = SUPER_CRITICAL_MINE_E_CENTER_PANEL(x_loc_norm, y_loc_norm);
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN2 = SUPER_CRITICAL_MINE_E_CENTER_GATEROADS(x_loc_norm, y_loc_norm);
				VSI=(FUN2*(mix)+FUN1*(1-mix)); }
			else if (y_loc < box[6]){
				x_loc_norm=( (x_loc-(box[1]) )/( box[1] ));
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				FUN1 = SUPER_CRITICAL_MINE_E_RECOVERY_CENTER(x_loc_norm, y_loc_norm);	
				x_loc_norm=( 1-(x_loc-(box[1]))/( box[2]-box[1] ) );
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				FUN2 = SUPER_CRITICAL_MINE_E_RECOVERY_GATEROADS(x_loc_norm, y_loc_norm);	
				VSI=( FUN2*(mix)+FUN1*(1-mix) ); 	} 
			else {VSI=0;}	
		}
			
		else{
			if (y_loc <0) {VSI=0;}
			else if( y_loc < box[4]-blendrangey){
				x_loc_norm= ( 1-(x_loc-box[1] )/( box[2]-box[1] ) );
				y_loc_norm=(y_loc/box[4]);
				VSI = SUPER_CRITICAL_MINE_E_STARTUP_GATEROADS(x_loc_norm, y_loc_norm); }
			else if ((y_loc > (box[4]-blendrangey) ) && (y_loc < (box[4]+blendrangey)) ) {
				mix = ((y_loc-box[4]+blendrangey)/(2*blendrangey));
				x_loc_norm= ( 1-(x_loc-box[1] )/( box[2]-box[1] ) );
				y_loc_norm=(y_loc/box[4]);
				FUN1 = SUPER_CRITICAL_MINE_E_STARTUP_GATEROADS(x_loc_norm, y_loc_norm);
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN2 = SUPER_CRITICAL_MINE_E_CENTER_GATEROADS(x_loc_norm, y_loc_norm);
				VSI=(FUN2*(mix)+FUN1*(1-mix)); 	}
			else if(( y_loc <  (box[5]-blendrangey-20 )) && ( y_loc > (box[4]+blendrangey) ) ){
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				VSI = SUPER_CRITICAL_MINE_E_CENTER_GATEROADS(x_loc_norm, y_loc_norm); }	
			else if ((y_loc > (box[5]-blendrangey-20) ) && (y_loc < box[5]+blendrangey+20)) {
				mix = ((y_loc-box[5]+blendrangey)/(2*blendrangey));
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ));
				y_loc_norm=( (y_loc-box[4])/( box[5]-box[4]) );
				FUN1 = SUPER_CRITICAL_MINE_E_CENTER_GATEROADS(x_loc_norm, y_loc_norm);
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ) );
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				FUN2 = SUPER_CRITICAL_MINE_E_RECOVERY_GATEROADS(x_loc_norm, y_loc_norm);
				VSI= (FUN2*(mix)+FUN1*(1-mix)); 	} 
			else if (y_loc<panelength){
				x_loc_norm=( 1-(x_loc-box[1])/( box[2]-box[1] ) );
				y_loc_norm=(1-(y_loc-box[5])/(box[6]-box[5]));
				VSI = SUPER_CRITICAL_MINE_E_RECOVERY_GATEROADS(x_loc_norm, y_loc_norm); }
			else {VSI=0;}
		} 
	}
	VSI=(VSI<0.0)?0:VSI;
	VSI=(VSI>maximum_vsi)?maximum_vsi:VSI;
	C_UDMI(c,t,4) = VSI;
} 
end_c_loop(c,t) }
Print_Scheme_Variable_Settings();
}
/* 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	!!!!!!!!!!!  POROSITY CALCULATION profiles!!!!!!!!!!!!
	!!!!!!!!!!!	FROM VSI or Wachel (2012) Fit       !!!!!!
	!!!!!!!!!!!                                     !!!!!!
	!!!!!!!!!!! STORES in (user-define-memory 1)    !!!!!!
	!!!!!!!!!!!             udm-5                   !!!!!!
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
DEFINE_ON_DEMAND(VSI_MINE_Worrall_C)
{ 
  Domain *d; Thread *t; cell_t c; real x[ND_ND];   
  
  real cellpor;  real x_loc;  real y_loc;
  double panelwidth=157.255;
  /* 151.4856;  specified here as half-width */
  double panelength=2941.3024;
  double panelxoffset=0.0;
  double panelyoffset=0.0;
  double maximum_vsi=0.5;
    if (RP_Variable_Exists_P("vsi/panel-width")){       /* Returns true if the variable exists */
	 panelwidth=( RP_Get_Real("vsi/panel-width") / 2 );
	 Message ("Panel width is: %g\n",panelwidth*2); }/* else default or manual set above is used */
  else{ Message("Panel Width not set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/panel-width VALUE 'real #f)\n" , panelwidth*2);	 }
   
  if (RP_Variable_Exists_P("vsi/panel-length")){        
	 panelength=( RP_Get_Real("vsi/panel-length") );
	 Message ("Panel length is set to: %g\n reset value using (rpsetvar 'vsi/panel-length VALUE)\n",panelength); } 
  else{ Message("Panel Length not set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/panel-length VALUE'real #f)\n", panelength); }
  
  /* Specify a maximum value of VSI for the change porosity from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/maximum-vsi")){        
	 maximum_vsi=( RP_Get_Real("vsi/maximum-vsi") );
	 Message ("The Maximum change in porosity from the maximum-porosity behind the face occurs \nat the center of the panel and is set to: %g\n",maximum_vsi); } /* else default or manual set above is used */
	else{ Message("Maximum change in porosity from the maximum-porosity behind the face occurs at the center of the panel and is NOT set. Using default value: %g\n You may set it with TUI Command: (rp-var-define 'vsi/maxiumum-vsi VALUE 'real #f)\n", maximum_vsi); }
/* Specify the displacement to the center of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-xoffset")){        
	 panelxoffset=( RP_Get_Real("vsi/panel-xoffset") );
	 Message ("The x-direction displacement to the center of the panel is set to: %g\n",panelxoffset); } /* else default or manual set above is used */
  else{ Message("The x-direction displacement to the center of the panel is to zero.\n You may set it with TUI Command: (rp-var-define 'vsi/panel-xoffset VALUE 'real #f)\n"); }
  /* Specify the displacement to the recovery room of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-yoffset")){        
	 panelyoffset=( RP_Get_Real("vsi/panel-yoffset") );
	 Message ("The y-direction displacement to the recovery room of the panel is set to: %g\n",panelyoffset); } /* else default or manual set above is used */
  else{ Message("The y-direction displacement to the recovery room of the panel is to zero.\n You may set it with TUI Command: (rp-var-define 'vsi/panel-yoffset VALUE 'real #f)\n"); }
  
  d = Get_Domain(1);
  thread_loop_c(t,d) {
  begin_c_loop(c,t)   { 
    C_CENTROID(x,c,t);
	x_loc=(fabs(x[0]-panelxoffset)/panelwidth*166.88)-9.625; /* headgate 147.63, tailgate at 166.88  */
	y_loc=((x[1]-panelyoffset)/(panelength/2)*1474.3)+1465.65; /* recovery 1474.3, startup at -1457 */
	if( ( fabs(x[0]-panelxoffset) < panelwidth) && (fabs(y_loc) < 1474.3) ){
	/*WORRALL: Scales correctly */
	cellpor = pow(x_loc,1)*pow(y_loc,9)*(-4.11E-36)+pow(x_loc,0)*pow(y_loc,10)*(6.35E-32)+pow(x_loc,0)*pow(y_loc,9)*(-3.66E-29)+pow(x_loc,0)*pow(y_loc,8)*(-2.57E-25)+pow(x_loc,1)*pow(y_loc,8)*(-3.02E-29)+pow(x_loc,2)*pow(y_loc,8)*(-1.57E-30)+pow(x_loc,0)*pow(y_loc,7)*(1.32E-22)+pow(x_loc,1)*pow(y_loc,7)*(8.23E-27)+pow(x_loc,2)*pow(y_loc,7)*(4.25E-28)+pow(x_loc,3)*pow(y_loc,7)*(-1.54E-33)+pow(x_loc,0)*pow(y_loc,6)*(3.95E-19)+pow(x_loc,1)*pow(y_loc,6)*(1.16E-22)+pow(x_loc,2)*pow(y_loc,6)*(6.00E-24)+pow(x_loc,3)*pow(y_loc,6)*(-3.85E-27)+pow(x_loc,4)*pow(y_loc,6)*(-9.98E-29)+pow(x_loc,0)*pow(y_loc,5)*(-1.58E-16)+pow(x_loc,1)*pow(y_loc,5)*(-4.06E-20)+pow(x_loc,2)*pow(y_loc,5)*(-2.08E-21)+pow(x_loc,3)*pow(y_loc,5)*(1.95E-24)+pow(x_loc,4)*pow(y_loc,5)*(5.06E-26)+pow(x_loc,5)*pow(y_loc,5)*(2.13E-31)+pow(x_loc,0)*pow(y_loc,4)*(-2.79E-13)+pow(x_loc,1)*pow(y_loc,4)*(-1.51E-16)+pow(x_loc,2)*pow(y_loc,4)*(-7.75E-18)+pow(x_loc,3)*pow(y_loc,4)*(8.15E-21)+pow(x_loc,4)*pow(y_loc,4)*(2.12E-22)+pow(x_loc,5)*pow(y_loc,4)*(6.35E-26)+pow(x_loc,6)*pow(y_loc,4)*(1.11E-27)+pow(x_loc,0)*pow(y_loc,3)*(6.73E-11)+pow(x_loc,1)*pow(y_loc,3)*(6.45E-14)+pow(x_loc,2)*pow(y_loc,3)*(3.30E-15)+pow(x_loc,3)*pow(y_loc,3)*(-5.12E-18)+pow(x_loc,4)*pow(y_loc,3)*(-1.32E-19)+pow(x_loc,5)*pow(y_loc,3)*(5.34E-23)+pow(x_loc,6)*pow(y_loc,3)*(9.36E-25)+pow(x_loc,7)*pow(y_loc,3)*(9.94E-30)+pow(x_loc,0)*pow(y_loc,2)*(1.38E-07)+pow(x_loc,1)*pow(y_loc,2)*(7.30E-11)+pow(x_loc,2)*pow(y_loc,2)*(3.70E-12)+pow(x_loc,3)*pow(y_loc,2)*(-8.19E-15)+pow(x_loc,4)*pow(y_loc,2)*(-2.11E-16)+pow(x_loc,5)*pow(y_loc,2)*(-1.88E-20)+pow(x_loc,6)*pow(y_loc,2)*(-4.17E-22)+pow(x_loc,7)*pow(y_loc,2)*(-7.99E-25)+pow(x_loc,8)*pow(y_loc,2)*(-9.08E-27)+pow(x_loc,0)*pow(y_loc,1)*(7.76E-05)+pow(x_loc,1)*pow(y_loc,1)*(-3.91E-08)+pow(x_loc,2)*pow(y_loc,1)*(-2.04E-09)+pow(x_loc,3)*pow(y_loc,1)*(-1.42E-12)+pow(x_loc,4)*pow(y_loc,1)*(-3.55E-14)+pow(x_loc,5)*pow(y_loc,1)*(1.14E-16)+pow(x_loc,6)*pow(y_loc,1)*(1.92E-18)+pow(x_loc,7)*pow(y_loc,1)*(-6.16E-21)+pow(x_loc,8)*pow(y_loc,1)*(-8.47E-23)+pow(x_loc,9)*pow(y_loc,1)*(-8.37E-27)+pow(x_loc,0)*pow(y_loc,0)*(0.079182292)+pow(x_loc,1)*pow(y_loc,0)*(8.15E-05)+pow(x_loc,2)*pow(y_loc,0)*(4.30E-06)+pow(x_loc,3)*pow(y_loc,0)*(7.06E-09)+pow(x_loc,4)*pow(y_loc,0)*(2.05E-10)+pow(x_loc,5)*pow(y_loc,0)*(1.22E-12)+pow(x_loc,6)*pow(y_loc,0)*(1.99E-14)+pow(x_loc,7)*pow(y_loc,0)*(-4.46E-17)+pow(x_loc,8)*pow(y_loc,0)*(-5.61E-19)+pow(x_loc,9)*pow(y_loc,0)*(1.08E-22)+pow(x_loc,10)*pow(y_loc,0)*(8.35E-25); 
		
		/* WACHEL 
		cellpor = pow(x_loc,1)*pow(y_loc,9)*(1.11801E-44)+pow(x_loc,0)*pow(y_loc,10)*(6.82922E-32)+pow(x_loc,0)*pow(y_loc,9)*(-3.71619E-29)+pow(x_loc,0)*pow(y_loc,8)*(-2.78057E-25)+pow(x_loc,1)*pow(y_loc,8)*(-3.11556E-29)+pow(x_loc,2)*pow(y_loc,8)*(-1.61847E-30)+pow(x_loc,0)*pow(y_loc,7)*(1.333643E-22)+pow(x_loc,1)*pow(y_loc,7)*(7.57979E-27)+pow(x_loc,2)*pow(y_loc,7)*(3.93755E-28)+pow(x_loc,3)*pow(y_loc,7)*(-7.69046E-43)+pow(x_loc,0)*pow(y_loc,6)*(4.27461E-19)+pow(x_loc,1)*pow(y_loc,6)*(1.27786E-22)+pow(x_loc,2)*pow(y_loc,6)*(6.59117E-24)+pow(x_loc,3)*pow(y_loc,6)*(-4.88866E-27)+pow(x_loc,4)*pow(y_loc,6)*(-1.26978E-28)+pow(x_loc,0)*pow(y_loc,5)*(-1.60385E-16)+pow(x_loc,1)*pow(y_loc,5)*(-4.02033E-20)+pow(x_loc,2)*pow(y_loc,5)*(-2.06722E-21)+pow(x_loc,3)*pow(y_loc,5)*(2.20899E-24)+pow(x_loc,4)*pow(y_loc,5)*(5.73764E-26)+pow(x_loc,5)*pow(y_loc,5)*(8.12592E-41)+pow(x_loc,0)*pow(y_loc,4)*(-2.97114E-13)+pow(x_loc,1)*pow(y_loc,4)*(-1.83451E-16)+pow(x_loc,2)*pow(y_loc,4)*(-9.40516E-18)+pow(x_loc,3)*pow(y_loc,4)*(1.29569E-20)+pow(x_loc,4)*pow(y_loc,4)*(3.35872E-22)+pow(x_loc,5)*pow(y_loc,4)*(-4.17688E-26)+pow(x_loc,6)*pow(y_loc,4)*(-7.23269E-28)+pow(x_loc,0)*pow(y_loc,3)*(6.86026E-11)+pow(x_loc,1)*pow(y_loc,3)*(6.76388E-14)+pow(x_loc,2)*pow(y_loc,3)*(3.45435E-15)+pow(x_loc,3)*pow(y_loc,3)*(-6.15568E-18)+pow(x_loc,4)*pow(y_loc,3)*(-1.58543E-19)+pow(x_loc,5)*pow(y_loc,3)*(8.38502E-23)+pow(x_loc,6)*pow(y_loc,3)*(1.45195E-24)+pow(x_loc,7)*pow(y_loc,3)*(-2.64061E-40)+pow(x_loc,0)*pow(y_loc,2)*(1.40161E-07)+pow(x_loc,1)*pow(y_loc,2)*(1.10104E-10)+pow(x_loc,2)*pow(y_loc,2)*(5.53326E-12)+pow(x_loc,3)*pow(y_loc,2)*(-1.92511E-14)+pow(x_loc,4)*pow(y_loc,2)*(-4.84547E-16)+pow(x_loc,5)*pow(y_loc,2)*(9.5722E-19)+pow(x_loc,6)*pow(y_loc,2)*(1.58960E-20)+pow(x_loc,7)*pow(y_loc,2)*(-3.02429E-23)+pow(x_loc,8)*pow(y_loc,2)*(-3.92765E-25)+pow(x_loc,0)*pow(y_loc,1)*(7.73016E-05)+pow(x_loc,1)*pow(y_loc,1)*(-3.23049E-08)+pow(x_loc,2)*pow(y_loc,1)*(-1.73944E-09)+pow(x_loc,3)*pow(y_loc,1)*(-6.25735E-12)+pow(x_loc,4)*pow(y_loc,1)*(-1.48646E-13)+pow(x_loc,5)*pow(y_loc,1)*(8.56237E-16)+pow(x_loc,6)*pow(y_loc,1)*(1.40316E-17)+pow(x_loc,7)*pow(y_loc,1)*(-3.54006E-20)+pow(x_loc,8)*pow(y_loc,1)*(-4.59748E-22)+pow(x_loc,9)*pow(y_loc,1)*(-5.46536E-37)+pow(x_loc,0)*pow(y_loc,0)*(0.0790637)+pow(x_loc,1)*pow(y_loc,0)*(9.65660E-05)+pow(x_loc,2)*pow(y_loc,0)*(4.89570E-06)+pow(x_loc,3)*pow(y_loc,0)*(-1.17447E-08)+pow(x_loc,4)*pow(y_loc,0)*(-2.02395E-10)+pow(x_loc,5)*pow(y_loc,0)*(6.26721E-12)+pow(x_loc,6)*pow(y_loc,0)*(9.71115E-14)+pow(x_loc,7)*pow(y_loc,0)*(-5.02233E-16)+pow(x_loc,8)*pow(y_loc,0)*(-6.13986E-18)+pow(x_loc,9)*pow(y_loc,0)*(1.32517E-20)+pow(x_loc,10)*pow(y_loc,0)*(1.37680E-22);
		Scales wrong*/
		C_UDMI(c,t,4) = ((0.5 - cellpor)<0)?0:(0.5 - cellpor); }
	else {C_UDMI(c,t,4) = 0;}
	if (C_UDMI(c,t,4) > maximum_vsi) {C_UDMI(c,t,4)=maximum_vsi;}
  } 
  end_c_loop(c,t) }
}

DEFINE_PROFILE(set_poro_VSI,t,nv) 
{ 
/* n = (V_v - VSI) /V_t
where n is porosity (%), V_v is volume of voids (cubic meters), 
vsi is volumetric strain (%), and V_t is total volume (cubic meters). */
  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
/*  double V_t=10.0000 * 10.0000 * 10.0000;  10 meter cubed grid cell in FLAC3D */
	cell_t c;  real cellpor;
	double V_v = 0.40000;
	real a=1; 		/* for a scalor */
if (RP_Variable_Exists_P("vsi/porosity-scaler")) {        /* Returns true if the variable exists */
	 a=( RP_Get_Real("vsi/porosity-scaler") );	 }
if (RP_Variable_Exists_P("vsi/maximum-porosity")){       
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}

    begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

			cellpor = ( (V_v - C_UDMI(c,t,4))*a ); /* Initial Maximum gob porosity minus the change in porosity (VSI). */
 
		C_PROFILE(c,t,nv) = (cellpor<0)?0:cellpor; 		       /* 'a' scaler for later use */
		C_UDMI(c,t,1) = (cellpor<0)?0:cellpor; }
	if(ite>1){ C_PROFILE(c,t,nv) = C_UDMI(c,t,1); }
  }  end_c_loop(c,t) }

DEFINE_PROFILE(set_1poro_VSI,t,nv) 
{ 
/* n = (V_v - VSI) /V_t
where n is porosity (%), V_v is volume of voids (cubic meters), 
vsi is volumetric strain (%), and V_t is total volume (cubic meters). */
  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
/*  double V_t=10.0000 * 10.0000 * 10.0000;  10 meter cubed grid cell in FLAC3D */
  cell_t c;  real cellpor;
double V_v = 0.40000;
	real a=1; 		/* for a scalor */
if (RP_Variable_Exists_P("vsi/porosity-scaler")) {        /* Returns true if the variable exists */
	 a=( RP_Get_Real("vsi/porosity-scaler") );	 }
if (RP_Variable_Exists_P("vsi/maximum-porosity")){       
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}

    begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

			cellpor = ( (V_v - C_UDMI(c,t,4))*a ); /* Inital Maximum gob porosity minus the change in porosity (VSI). */
 
		C_PROFILE(c,t,nv) = (cellpor<0)?0:cellpor; 		       /* 'a' scaler for later use */
		C_UDMI(c,t,1) = (cellpor<0)?0:cellpor; }
	if(ite>1){ C_PROFILE(c,t,nv) = C_UDMI(c,t,1); }
  }  end_c_loop(c,t) }

DEFINE_PROFILE(set_2poro_VSI,t,nv) 
{ 
/* n = (V_v - VSI) /V_t
where n is porosity (%), V_v is volume of voids (cubic meters), 
vsi is volumetric strain (%), and V_t is total volume (cubic meters). */
  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
/*  double V_t=10.0000 * 10.0000 * 10.0000;  10 meter cubed grid cell in FLAC3D */
  cell_t c;  real cellpor;
double V_v = 0.40000;
	real a=1; 		/* for a scalor */
if (RP_Variable_Exists_P("vsi/porosity-scaler")) {        /* Returns true if the variable exists */
	 a=( RP_Get_Real("vsi/porosity-scaler") );	 }
if (RP_Variable_Exists_P("vsi/maximum-porosity")){       
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}

    begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

			cellpor = ( (V_v - C_UDMI(c,t,4))*a ); /* Inital Maximum gob porosity minus the change in porosity (VSI). */
 
		C_PROFILE(c,t,nv) = (cellpor<0)?0:cellpor; 		       /* 'a' scaler for later use */
		C_UDMI(c,t,1) = (cellpor<0)?0:cellpor; }
	if(ite>1){ C_PROFILE(c,t,nv) = C_UDMI(c,t,1); }
  }  end_c_loop(c,t) }

DEFINE_PROFILE(set_3poro_VSI,t,nv) 
{ 
/* n = (V_v - VSI) /V_t
where n is porosity (%), V_v is volume of voids (cubic meters), 
vsi is volumetric strain (%), and V_t is total volume (cubic meters). */
  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
/*  double V_t=10.0000 * 10.0000 * 10.0000;  10 meter cubed grid cell in FLAC3D */
  cell_t c;  real cellpor;
double V_v = 0.40000;
	real a=1; 		/* for a scalor */
if (RP_Variable_Exists_P("vsi/porosity-scaler")) {        /* Returns true if the variable exists */
	 a=( RP_Get_Real("vsi/porosity-scaler") );	 }
if (RP_Variable_Exists_P("vsi/maximum-porosity")){       
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}

    begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

			cellpor = ( (V_v - C_UDMI(c,t,4))*a ); /* Inital Maximum gob porosity minus the change in porosity (VSI). */
 
		C_PROFILE(c,t,nv) = (cellpor<0)?0:cellpor; 		       /* 'a' scaler for later use */
		C_UDMI(c,t,1) = (cellpor<0)?0:cellpor; }
	if(ite>1){ C_PROFILE(c,t,nv) = C_UDMI(c,t,1); }
  }  end_c_loop(c,t) }


/*:::::::::::::::::::::::::::::::::::::::::::  WACHEL (2012) ::::::::::::::::::::::::::::::::::::*/
DEFINE_PROFILE(set_legacy_porosity_wachel,t,nv) 
{ 
  real x[ND_ND];   real a;   cell_t c;   real cellpor;  real x_loc;  real y_loc;  real z_loc;
  
  double panelwidth=157.13;
  /* 151.4856;  specified here as half-width */
  double panelength=2921.3024;
  double panelxoffset=0.0;
  double panelyoffset=0.0;
if (RP_Variable_Exists_P("vsi/panel-width")){       /* Returns true if the variable exists */
	 panelwidth=( RP_Get_Real("vsi/panel-width") / 2 );}
if (RP_Variable_Exists_P("vsi/panel-length")){        
	 panelength=( RP_Get_Real("vsi/panel-length") );}
 /* Specify the displacement to the center of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-xoffset")){        
	 panelxoffset=( RP_Get_Real("vsi/panel-xoffset") );}
  /* Specify the displacement to the recovery room of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-yoffset")){        
	 panelyoffset=( RP_Get_Real("vsi/panel-yoffset") );	 }
  begin_c_loop(c,t)   { 
    C_CENTROID(x,c,t);
	x_loc=(fabs(x[0]-panelxoffset)/panelwidth*166.88)-9.625; /* headgate 147.63, tailgate at 166.88  */
	y_loc=((x[1]-panelyoffset)/(panelength/2)*1474.3)+1465.65; /* recovery 1474.3, startup at -1457 */	
	if(ite<=1){
		cellpor = pow(x_loc,1)*pow(y_loc,9)*(-4.11E-36)+pow(x_loc,0)*pow(y_loc,10)*(6.35E-32)+pow(x_loc,0)*pow(y_loc,9)*(-3.66E-29)+pow(x_loc,0)*pow(y_loc,8)*(-2.57E-25)+pow(x_loc,1)*pow(y_loc,8)*(-3.02E-29)+pow(x_loc,2)*pow(y_loc,8)*(-1.57E-30)+pow(x_loc,0)*pow(y_loc,7)*(1.32E-22)+pow(x_loc,1)*pow(y_loc,7)*(8.23E-27)+pow(x_loc,2)*pow(y_loc,7)*(4.25E-28)+pow(x_loc,3)*pow(y_loc,7)*(-1.54E-33)+pow(x_loc,0)*pow(y_loc,6)*(3.95E-19)+pow(x_loc,1)*pow(y_loc,6)*(1.16E-22)+pow(x_loc,2)*pow(y_loc,6)*(6.00E-24)+pow(x_loc,3)*pow(y_loc,6)*(-3.85E-27)+pow(x_loc,4)*pow(y_loc,6)*(-9.98E-29)+pow(x_loc,0)*pow(y_loc,5)*(-1.58E-16)+pow(x_loc,1)*pow(y_loc,5)*(-4.06E-20)+pow(x_loc,2)*pow(y_loc,5)*(-2.08E-21)+pow(x_loc,3)*pow(y_loc,5)*(1.95E-24)+pow(x_loc,4)*pow(y_loc,5)*(5.06E-26)+pow(x_loc,5)*pow(y_loc,5)*(2.13E-31)+pow(x_loc,0)*pow(y_loc,4)*(-2.79E-13)+pow(x_loc,1)*pow(y_loc,4)*(-1.51E-16)+pow(x_loc,2)*pow(y_loc,4)*(-7.75E-18)+pow(x_loc,3)*pow(y_loc,4)*(8.15E-21)+pow(x_loc,4)*pow(y_loc,4)*(2.12E-22)+pow(x_loc,5)*pow(y_loc,4)*(6.35E-26)+pow(x_loc,6)*pow(y_loc,4)*(1.11E-27)+pow(x_loc,0)*pow(y_loc,3)*(6.73E-11)+pow(x_loc,1)*pow(y_loc,3)*(6.45E-14)+pow(x_loc,2)*pow(y_loc,3)*(3.30E-15)+pow(x_loc,3)*pow(y_loc,3)*(-5.12E-18)+pow(x_loc,4)*pow(y_loc,3)*(-1.32E-19)+pow(x_loc,5)*pow(y_loc,3)*(5.34E-23)+pow(x_loc,6)*pow(y_loc,3)*(9.36E-25)+pow(x_loc,7)*pow(y_loc,3)*(9.94E-30)+pow(x_loc,0)*pow(y_loc,2)*(1.38E-07)+pow(x_loc,1)*pow(y_loc,2)*(7.30E-11)+pow(x_loc,2)*pow(y_loc,2)*(3.70E-12)+pow(x_loc,3)*pow(y_loc,2)*(-8.19E-15)+pow(x_loc,4)*pow(y_loc,2)*(-2.11E-16)+pow(x_loc,5)*pow(y_loc,2)*(-1.88E-20)+pow(x_loc,6)*pow(y_loc,2)*(-4.17E-22)+pow(x_loc,7)*pow(y_loc,2)*(-7.99E-25)+pow(x_loc,8)*pow(y_loc,2)*(-9.08E-27)+pow(x_loc,0)*pow(y_loc,1)*(7.76E-05)+pow(x_loc,1)*pow(y_loc,1)*(-3.91E-08)+pow(x_loc,2)*pow(y_loc,1)*(-2.04E-09)+pow(x_loc,3)*pow(y_loc,1)*(-1.42E-12)+pow(x_loc,4)*pow(y_loc,1)*(-3.55E-14)+pow(x_loc,5)*pow(y_loc,1)*(1.14E-16)+pow(x_loc,6)*pow(y_loc,1)*(1.92E-18)+pow(x_loc,7)*pow(y_loc,1)*(-6.16E-21)+pow(x_loc,8)*pow(y_loc,1)*(-8.47E-23)+pow(x_loc,9)*pow(y_loc,1)*(-8.37E-27)+pow(x_loc,0)*pow(y_loc,0)*(0.079182292)+pow(x_loc,1)*pow(y_loc,0)*(8.15E-05)+pow(x_loc,2)*pow(y_loc,0)*(4.30E-06)+pow(x_loc,3)*pow(y_loc,0)*(7.06E-09)+pow(x_loc,4)*pow(y_loc,0)*(2.05E-10)+pow(x_loc,5)*pow(y_loc,0)*(1.22E-12)+pow(x_loc,6)*pow(y_loc,0)*(1.99E-14)+pow(x_loc,7)*pow(y_loc,0)*(-4.46E-17)+pow(x_loc,8)*pow(y_loc,0)*(-5.61E-19)+pow(x_loc,9)*pow(y_loc,0)*(1.08E-22)+pow(x_loc,10)*pow(y_loc,0)*(8.35E-25);
		C_PROFILE(c,t,nv)=cellpor;
		C_UDMI(c,t,1) = cellpor; }
	if(ite>1){
		C_PROFILE(c,t,nv) = C_UDMI(c,t,1); }
  }   end_c_loop(c,t) 
}

/* 
	#################################
	# C2 Inertia Resistance		#
	#################################
*/

DEFINE_PROFILE(set_inertia_1_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_inertia_resistance; double cellinertiaresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_inertia_resist=1.3E5; double minimum_inertia_resist=0.000;  
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-inertia-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-inertia-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-inertia-resist") ){       
	 maximum_inertia_resist=( RP_Get_Real("vsi/maximum-inertia-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-inertia-resist") ){        
	 minimum_inertia_resist=( RP_Get_Real("vsi/minimum-inertia-resist") );}

initial_inertia_resistance = Initial_Inertia_Resistance();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellinertiaresist = Cell_Inertia_Resistance(cellporo, initial_inertia_resistance); /* Blake-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellinertiaresist < maximum_inertia_resist){
			if(cellinertiaresist < minimum_inertia_resist){ cellinertiaresist=minimum_inertia_resist;	}
			}
		else{				cellinertiaresist=maximum_inertia_resist;	}  
					C_PROFILE(c,t,nv) = cellinertiaresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,5) = cellinertiaresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,5); }
  } end_c_loop(c,t) 
}

DEFINE_PROFILE(set_inertia_2_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_inertia_resistance; double cellinertiaresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_inertia_resist=1.3E5; double minimum_inertia_resist=0.000;  
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-inertia-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-inertia-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-inertia-resist") ){       
	 maximum_inertia_resist=( RP_Get_Real("vsi/maximum-inertia-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-inertia-resist") ){        
	 minimum_inertia_resist=( RP_Get_Real("vsi/minimum-inertia-resist") );}

initial_inertia_resistance = Initial_Inertia_Resistance();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellinertiaresist = Cell_Inertia_Resistance(cellporo, initial_inertia_resistance); /* Blake-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellinertiaresist < maximum_inertia_resist){
			if(cellinertiaresist < minimum_inertia_resist){ cellinertiaresist=minimum_inertia_resist;	}
			}
		else{				cellinertiaresist=maximum_inertia_resist;	}  
					C_PROFILE(c,t,nv) = cellinertiaresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,5) = cellinertiaresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,5); }
  } end_c_loop(c,t) 
}

DEFINE_PROFILE(set_inertia_3_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_inertia_resistance; double cellinertiaresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_inertia_resist=1.3E5; double minimum_inertia_resist=0.000;  
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-inertia-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-inertia-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-inertia-resist") ){       
	 maximum_inertia_resist=( RP_Get_Real("vsi/maximum-inertia-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-inertia-resist") ){        
	 minimum_inertia_resist=( RP_Get_Real("vsi/minimum-inertia-resist") );}

initial_inertia_resistance = Initial_Inertia_Resistance();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellinertiaresist = Cell_Inertia_Resistance(cellporo, initial_inertia_resistance); /* Blake-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellinertiaresist < maximum_inertia_resist){
			if(cellinertiaresist < minimum_inertia_resist){ cellinertiaresist=minimum_inertia_resist;	}
			}
		else{				cellinertiaresist=maximum_inertia_resist;	}  
					C_PROFILE(c,t,nv) = cellinertiaresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,5) = cellinertiaresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,5); }
  } end_c_loop(c,t) 
}

/*
	-------------------------------------------------
	!   Permeability or in FLUENT the inverse of    !
	!   makes RESISTANCE profiles                   !
	!   One for each direction 1,2,3 or x,y,x       !
	!   and one for each fluid zone                 !
	!                                               !
	!   STORES in (user-define-memory 0)            !
	!				udm-0                           !
	!-----------------------------------------------!
*/

DEFINE_PROFILE(set_perm_1_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}


DEFINE_PROFILE(set_perm_2_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}

DEFINE_PROFILE(set_perm_3_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
DEFINE_PROFILE(set_1perm_1_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}

DEFINE_PROFILE(set_1perm_2_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}

DEFINE_PROFILE(set_1perm_3_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
DEFINE_PROFILE(set_2perm_1_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}

DEFINE_PROFILE(set_2perm_2_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}

DEFINE_PROFILE(set_2perm_3_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
DEFINE_PROFILE(set_3perm_1_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}

DEFINE_PROFILE(set_3perm_2_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}

DEFINE_PROFILE(set_3perm_3_VSI,t,nv) 
{ 

/* FLUENT allows for an inertial resistance parameter to account for turbulent 
and transitional flow. Inertial resistance can be found using the following 
equation ANSYS (2010):
C2 = 3.5 / d * (1-n)/ n^3
where d is the mean particle diameter (meters) and n is the porosity (%) of the medium.
This equation is valid for use in the momentum conservation equation used by Ansys, Inc. in FLUENT. */

  real x[ND_ND]; /* position vector x[0]=x, x[1]=y, x[2]=z */
  cell_t c;  double cellporo;  double initial_permeability; double cellresist;

  double V_v=0.40000000; double a=1;  double resist_scaler=1; double maximum_resist=5.000000E6; double minimum_resist=1.45000E5;  /* equals 6.91e-6 1/m2 permeability */
if (RP_Variable_Exists_P("vsi/maximum-porosity") ){        /* Get scheme variable and assign it if it exists */
	 V_v=( RP_Get_Real("vsi/maximum-porosity") );}
if (RP_Variable_Exists_P("vsi/porosity-scaler") ){        
	 a=( RP_Get_Real("vsi/porosity-scaler") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
if (RP_Variable_Exists_P("vsi/maximum-resist") ){       
	 maximum_resist=( RP_Get_Real("vsi/maximum-resist") );}
if (RP_Variable_Exists_P("vsi/minimum-resist") ){        
	 minimum_resist=( RP_Get_Real("vsi/minimum-resist") );}

initial_permeability = Initial_Perm();

  begin_c_loop(c,t) { 
    C_CENTROID(x,c,t);
	if(ite<=1){

		cellporo =( (V_v - C_UDMI(c,t,4) )*a<0 )?0:(V_v - C_UDMI(c,t,4) )*a; /* Limit lowest value of porosity to zero */
		cellresist = Cell_Resistance(cellporo, initial_permeability); /* Carmen-Kozeny Relationship */
			/* Limit MAX and MIN resistance */
		if(cellresist < maximum_resist){
			if(cellresist < minimum_resist){ cellresist=minimum_resist;	}
			}
		else{				cellresist=maximum_resist;	}  
					C_PROFILE(c,t,nv) = cellresist * resist_scaler; /* Scaler applied to cell resistance */
					C_UDMI(c,t,0) = cellresist * resist_scaler; 
	}
	if(ite>1) {
		C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  } end_c_loop(c,t) 
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/*::::::::::::::::::::: WACHEL PERM :::::::::::::::::::::::::::::::::::::*/
DEFINE_PROFILE(set_legacy_perm_1_worrall,t,nv) 
{ 
  real x[ND_ND];   cell_t c;   double cellperm;  double x_loc;  double y_loc;  double cellresist;
  double resist_scaler=1;
  double panelwidth=157.3;
  /* 151.4856;  specified here as half-width */
  double panelength=2921.3024;
	double panelxoffset=0.0;
	double panelyoffset=0.0;
if (RP_Variable_Exists_P("vsi/panel-width")){       /* Returns true if the variable exists */
	 panelwidth=( RP_Get_Real("vsi/panel-width") / 2 ); }
if (RP_Variable_Exists_P("vsi/panel-length")){        
	 panelength=( RP_Get_Real("vsi/panel-length") ); }
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
/* Specify the displacement to the center of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-xoffset")){        
	 panelxoffset=( RP_Get_Real("vsi/panel-xoffset") );}
  /* Specify the displacement to the recovery room of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-yoffset")){        
	 panelyoffset=( RP_Get_Real("vsi/panel-yoffset") );}	 
  begin_c_loop(c,t)   { 
    C_CENTROID(x,c,t);
	x_loc=(fabs(x[0]-panelxoffset)/panelwidth*166.88)-9.625; /* headgate 147.63, tailgate at 166.88  */
	y_loc=((x[1]-panelyoffset)/(panelength/2)*1474.3)+1465.65; /* recovery 1474.3, startup at -1457 */	
	if(ite<=1){	
		cellperm = (x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.144063718331E-63))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.667803992896E-60))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-6.751892520015E-57))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.474252626400E-54))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.978169607429E-57))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.065804148485E-58))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.325998219031E-50))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.778848307692E-54))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.957503485522E-55))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.313487364221E-62))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.557128231701E-47))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.336557503800E-50))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.729578958791E-51))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.804899223699E-55))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-7.269395530980E-57))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.700052175339E-43))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.199498959994E-47))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.656895162522E-48))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.373054032216E-52))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.801298429234E-54))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.934273626696E-59))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(7.561292185998E-42))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.128508835069E-43))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-5.839327957889E-45))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.869239188620E-48))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.831470006198E-50))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-5.038332937139E-54))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-8.607065333527E-56))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.816664532243E-37))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.076974643570E-40))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.568594890340E-42))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.402361583225E-45))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-6.242748009406E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.328203179946E-50))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.375199599323E-52))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.055292528076E-57))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-5.349648763316E-35))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.960792115639E-37))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.013098663769E-38))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.340939184349E-42))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.125103504664E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.200092681492E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.624425005657E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.224375832281E-51))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.593980895738E-53))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.578725192355E-31))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.838517841395E-34))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.486709878069E-36))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.529959204504E-39))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.690234452480E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-7.778380121475E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.364002690849E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.836259859384E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.276462052572E-51))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.231368490836E-55))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.688830873852E-29))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.832011928186E-31))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.467410074332E-33))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.203361298476E-36))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.685529185898E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.957834678024E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.831892444390E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.398510867623E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.767802386314E-46))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.754071728797E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.822822081644E-51))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.281170237010E-25))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.678929156272E-28))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.641048365500E-30))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-8.577019706256E-33))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.207313156738E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.850391876744E-37))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.163582495995E-39))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.772678383860E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.810059899195E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.122607696400E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.469800241881E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.687034882690E-54))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.709132151356E-23))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.613845618885E-26))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.482571423358E-27))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.282138723322E-30))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.697116932588E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-8.272164988532E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.331045009636E-35))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.630781762223E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.748458488688E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.343654020395E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.562876517466E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.230171755377E-48))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.666910960522E-50))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.139384967068E-20))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-7.912082447522E-23))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.056561791628E-24))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.802557954940E-27))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.470918203378E-28))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.648692282812E-31))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.303339671597E-33))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.426813326956E-35))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.727290781194E-37))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.805345611340E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.824682684994E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.809353021430E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.100139916202E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(7.464492412548E-52))+(1.000000000*y_loc*y_loc*y_loc*y_loc*(9.268964018621E-18))+(x_loc*y_loc*y_loc*y_loc*y_loc*(-1.531377997083E-20))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-8.320722489547E-22))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-4.314812461407E-24))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-9.847091988918E-26))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(8.689416045690E-28))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(1.378314356577E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-5.665796183845E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-6.932531334446E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(1.484092300195E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(1.492298353485E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-1.428787551742E-41))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-1.200650172329E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(9.119809086077E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(6.773231751144E-49))+(1.000000000*y_loc*y_loc*y_loc*(3.190190706242E-15))+(x_loc*y_loc*y_loc*y_loc*(1.702022673981E-17))+(x_loc*x_loc*y_loc*y_loc*y_loc*(8.655054667145E-19))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.011063985918E-21))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-4.892252477842E-23))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(2.162513313402E-25))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(3.307183816977E-27))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-1.996125697995E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.297338132691E-31))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(1.049039993500E-33))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(9.943195129374E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.866641744039E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.367856259851E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(3.257557479945E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(2.458998006781E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(4.538294541393E-50))+(1.000000000*y_loc*y_loc*(-6.407634112118E-13))+(x_loc*y_loc*y_loc*(-2.332834112086E-17))+(x_loc*x_loc*y_loc*y_loc*(1.417237426917E-17))+(x_loc*x_loc*x_loc*y_loc*y_loc*(1.652990830539E-18))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(3.725381796742E-20))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-3.511945028423E-22))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-5.216876752286E-24))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(3.749625814717E-26))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(4.035467912727E-28))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-2.789497532488E-30))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-2.328256033567E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(1.568770944843E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(1.139941145853E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-5.162933096051E-39))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-3.511130071062E-41))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(6.689474139203E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(4.345574550180E-46))+(1.000000000*y_loc*(4.403281264204E-12))+(x_loc*y_loc*(-9.848089396826E-13))+(x_loc*x_loc*y_loc*(-4.794835509428E-14))+(x_loc*x_loc*x_loc*y_loc*(3.326085117662E-16))+(x_loc*x_loc*x_loc*x_loc*y_loc*(7.172899785535E-18))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-8.875294141709E-20))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.146771046747E-21))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(1.679233473355E-23))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(1.680714341851E-25))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.683437566112E-27))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.417124039012E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(9.238641306722E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(6.943142790520E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-2.516221184873E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.745325683125E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(2.517703029197E-41))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(1.624876083821E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-2.341755164527E-48))+(1.000000000*1*(2.625441308571E-08))+(x_loc*1.000000000*(1.750644880639E-10))+(x_loc*x_loc*1.000000000*(7.639753260973E-12))+(x_loc*x_loc*x_loc*1.000000000*(-1.449441737249E-13))+(x_loc*x_loc*x_loc*x_loc*1.000000000*(-2.605617501914E-15))+(x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(6.867123803478E-17))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(8.506554922799E-19))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-1.442922691883E-20))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-1.419604613451E-22))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(1.558241120891E-24))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(1.482246843221E-26))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-4.454723463264E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-5.835270876962E-31))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-4.199071462702E-33))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-1.384323250043E-35))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(3.450868465845E-37))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(1.855461318215E-39))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-6.998238559677E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-4.035548380822E-44));		
		if(cellperm > 2.000E-7){
					if(cellperm > 6.91E-6){
						cellresist=1.45E5; }
					else{ cellresist=1.0/cellperm; }
		}
		else{ cellresist=5.000000E6; }
		C_PROFILE(c,t,nv)=cellresist *resist_scaler;
		C_UDMI(c,t,0) = cellresist *resist_scaler;
	}
	if(ite>1) {	C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  }  end_c_loop(c,t) 
}

DEFINE_PROFILE(set_legacy_perm_2_worrall,t,nv) 
{ 
  real x[ND_ND];   cell_t c;   double cellperm;  double x_loc;  double y_loc;  double cellresist;
  double resist_scaler=1;
  double panelwidth=157.13;
  /* 151.4856;  specified here as half-width */
  double panelength=2921.3024;
	double panelxoffset=0.0;
	double panelyoffset=0.0;
if (RP_Variable_Exists_P("vsi/panel-width")){       /* Returns true if the variable exists */
	 panelwidth=( RP_Get_Real("vsi/panel-width") / 2 );}  
if (RP_Variable_Exists_P("vsi/panel-length")){        
	 panelength=( RP_Get_Real("vsi/panel-length") ); }
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
	 /* Specify the displacement to the center of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-xoffset")){        
	 panelxoffset=( RP_Get_Real("vsi/panel-xoffset") );}
  /* Specify the displacement to the recovery room of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-yoffset")){        
	 panelyoffset=( RP_Get_Real("vsi/panel-yoffset") );	} 
  begin_c_loop(c,t)   { 
    C_CENTROID(x,c,t);
	x_loc=(fabs(x[0]-panelxoffset)/panelwidth*166.88)-9.625; /* headgate 147.63, tailgate at 166.88  */
	y_loc=((x[1]-panelyoffset)/(panelength/2)*1474.3)+1465.65; /* recovery 1474.3, startup at -1457 */	
	if(ite<=1){	
		cellperm = (x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.144063718331E-63))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.667803992896E-60))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-6.751892520015E-57))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.474252626400E-54))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.978169607429E-57))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.065804148485E-58))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.325998219031E-50))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.778848307692E-54))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.957503485522E-55))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.313487364221E-62))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.557128231701E-47))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.336557503800E-50))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.729578958791E-51))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.804899223699E-55))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-7.269395530980E-57))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.700052175339E-43))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.199498959994E-47))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.656895162522E-48))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.373054032216E-52))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.801298429234E-54))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.934273626696E-59))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(7.561292185998E-42))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.128508835069E-43))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-5.839327957889E-45))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.869239188620E-48))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.831470006198E-50))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-5.038332937139E-54))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-8.607065333527E-56))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.816664532243E-37))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.076974643570E-40))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.568594890340E-42))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.402361583225E-45))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-6.242748009406E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.328203179946E-50))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.375199599323E-52))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.055292528076E-57))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-5.349648763316E-35))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.960792115639E-37))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.013098663769E-38))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.340939184349E-42))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.125103504664E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.200092681492E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.624425005657E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.224375832281E-51))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.593980895738E-53))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.578725192355E-31))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.838517841395E-34))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.486709878069E-36))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.529959204504E-39))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.690234452480E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-7.778380121475E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.364002690849E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.836259859384E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.276462052572E-51))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.231368490836E-55))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.688830873852E-29))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.832011928186E-31))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.467410074332E-33))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.203361298476E-36))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.685529185898E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.957834678024E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.831892444390E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.398510867623E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.767802386314E-46))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.754071728797E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.822822081644E-51))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.281170237010E-25))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.678929156272E-28))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.641048365500E-30))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-8.577019706256E-33))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.207313156738E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.850391876744E-37))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.163582495995E-39))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.772678383860E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.810059899195E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.122607696400E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.469800241881E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.687034882690E-54))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.709132151356E-23))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.613845618885E-26))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.482571423358E-27))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.282138723322E-30))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.697116932588E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-8.272164988532E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.331045009636E-35))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.630781762223E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.748458488688E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.343654020395E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.562876517466E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.230171755377E-48))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.666910960522E-50))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.139384967068E-20))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-7.912082447522E-23))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.056561791628E-24))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.802557954940E-27))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.470918203378E-28))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.648692282812E-31))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.303339671597E-33))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.426813326956E-35))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.727290781194E-37))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.805345611340E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.824682684994E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.809353021430E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.100139916202E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(7.464492412548E-52))+(1.000000000*y_loc*y_loc*y_loc*y_loc*(9.268964018621E-18))+(x_loc*y_loc*y_loc*y_loc*y_loc*(-1.531377997083E-20))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-8.320722489547E-22))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-4.314812461407E-24))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-9.847091988918E-26))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(8.689416045690E-28))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(1.378314356577E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-5.665796183845E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-6.932531334446E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(1.484092300195E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(1.492298353485E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-1.428787551742E-41))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-1.200650172329E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(9.119809086077E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(6.773231751144E-49))+(1.000000000*y_loc*y_loc*y_loc*(3.190190706242E-15))+(x_loc*y_loc*y_loc*y_loc*(1.702022673981E-17))+(x_loc*x_loc*y_loc*y_loc*y_loc*(8.655054667145E-19))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.011063985918E-21))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-4.892252477842E-23))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(2.162513313402E-25))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(3.307183816977E-27))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-1.996125697995E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.297338132691E-31))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(1.049039993500E-33))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(9.943195129374E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.866641744039E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.367856259851E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(3.257557479945E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(2.458998006781E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(4.538294541393E-50))+(1.000000000*y_loc*y_loc*(-6.407634112118E-13))+(x_loc*y_loc*y_loc*(-2.332834112086E-17))+(x_loc*x_loc*y_loc*y_loc*(1.417237426917E-17))+(x_loc*x_loc*x_loc*y_loc*y_loc*(1.652990830539E-18))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(3.725381796742E-20))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-3.511945028423E-22))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-5.216876752286E-24))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(3.749625814717E-26))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(4.035467912727E-28))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-2.789497532488E-30))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-2.328256033567E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(1.568770944843E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(1.139941145853E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-5.162933096051E-39))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-3.511130071062E-41))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(6.689474139203E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(4.345574550180E-46))+(1.000000000*y_loc*(4.403281264204E-12))+(x_loc*y_loc*(-9.848089396826E-13))+(x_loc*x_loc*y_loc*(-4.794835509428E-14))+(x_loc*x_loc*x_loc*y_loc*(3.326085117662E-16))+(x_loc*x_loc*x_loc*x_loc*y_loc*(7.172899785535E-18))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-8.875294141709E-20))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.146771046747E-21))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(1.679233473355E-23))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(1.680714341851E-25))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.683437566112E-27))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.417124039012E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(9.238641306722E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(6.943142790520E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-2.516221184873E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.745325683125E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(2.517703029197E-41))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(1.624876083821E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-2.341755164527E-48))+(1.000000000*1*(2.625441308571E-08))+(x_loc*1.000000000*(1.750644880639E-10))+(x_loc*x_loc*1.000000000*(7.639753260973E-12))+(x_loc*x_loc*x_loc*1.000000000*(-1.449441737249E-13))+(x_loc*x_loc*x_loc*x_loc*1.000000000*(-2.605617501914E-15))+(x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(6.867123803478E-17))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(8.506554922799E-19))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-1.442922691883E-20))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-1.419604613451E-22))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(1.558241120891E-24))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(1.482246843221E-26))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-4.454723463264E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-5.835270876962E-31))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-4.199071462702E-33))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-1.384323250043E-35))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(3.450868465845E-37))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(1.855461318215E-39))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-6.998238559677E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-4.035548380822E-44));		
		if(cellperm > 2.000E-7){
					if(cellperm > 6.91E-6){
						cellresist=1.45E5; }
					else{ cellresist=1.0/cellperm; }
		}
		else{ cellresist=5.000000E6; }
		C_PROFILE(c,t,nv)=cellresist *resist_scaler;
		C_UDMI(c,t,0) = cellresist *resist_scaler;
	}
	if(ite>1) {	C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  }  end_c_loop(c,t) 
}
DEFINE_PROFILE(set_legacy_perm_3_worrall,t,nv) 
{ 
  real x[ND_ND];   cell_t c;   double cellperm;  double x_loc;  double y_loc;  double cellresist;
  double resist_scaler=1;
  double panelwidth=157.13;
  /* 151.4856;  specified here as half-width */
  double panelength=2921.3024;
  double panelxoffset=0.0;
  double panelyoffset=0.0;
if (RP_Variable_Exists_P("vsi/panel-width")){       /* Returns true if the variable exists */
	 panelwidth=( RP_Get_Real("vsi/panel-width") / 2 );}
if (RP_Variable_Exists_P("vsi/panel-length")){        
	 panelength=( RP_Get_Real("vsi/panel-length") );}
if (RP_Variable_Exists_P("vsi/resist-scaler") ){        
	 resist_scaler=( RP_Get_Real("vsi/resist-scaler") );}
/* Specify the displacement to the center of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-xoffset")){        
	 panelxoffset=( RP_Get_Real("vsi/panel-xoffset") ); }
  /* Specify the displacement to the recovery room of the old panel from the scheme variable defined in FLUENT */
  if (RP_Variable_Exists_P("vsi/panel-yoffset")){        
	 panelyoffset=( RP_Get_Real("vsi/panel-yoffset") );	 }
  begin_c_loop(c,t)   { 
    C_CENTROID(x,c,t);
	x_loc=(fabs(x[0]-panelxoffset)/panelwidth*166.88)-9.625; /* headgate 147.63, tailgate at 166.88  */
	y_loc=((x[1]-panelyoffset)/(panelength/2)*1474.3)+1465.65; /* recovery 1474.3, startup at -1457 */	
	if(ite<=1){	
		cellperm = (x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.144063718331E-63))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.667803992896E-60))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-6.751892520015E-57))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.474252626400E-54))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.978169607429E-57))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.065804148485E-58))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.325998219031E-50))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.778848307692E-54))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.957503485522E-55))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.313487364221E-62))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.557128231701E-47))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.336557503800E-50))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.729578958791E-51))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.804899223699E-55))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-7.269395530980E-57))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.700052175339E-43))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.199498959994E-47))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.656895162522E-48))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.373054032216E-52))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.801298429234E-54))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.934273626696E-59))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(7.561292185998E-42))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.128508835069E-43))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-5.839327957889E-45))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.869239188620E-48))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.831470006198E-50))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-5.038332937139E-54))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-8.607065333527E-56))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.816664532243E-37))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.076974643570E-40))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.568594890340E-42))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.402361583225E-45))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-6.242748009406E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.328203179946E-50))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.375199599323E-52))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.055292528076E-57))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-5.349648763316E-35))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.960792115639E-37))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.013098663769E-38))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.340939184349E-42))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.125103504664E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.200092681492E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.624425005657E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.224375832281E-51))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.593980895738E-53))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.578725192355E-31))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.838517841395E-34))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.486709878069E-36))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.529959204504E-39))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.690234452480E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-7.778380121475E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.364002690849E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.836259859384E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.276462052572E-51))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.231368490836E-55))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.688830873852E-29))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.832011928186E-31))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.467410074332E-33))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.203361298476E-36))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.685529185898E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.957834678024E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.831892444390E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.398510867623E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.767802386314E-46))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.754071728797E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.822822081644E-51))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.281170237010E-25))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.678929156272E-28))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.641048365500E-30))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-8.577019706256E-33))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.207313156738E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.850391876744E-37))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.163582495995E-39))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.772678383860E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.810059899195E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.122607696400E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(6.469800241881E-49))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.687034882690E-54))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.709132151356E-23))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(8.613845618885E-26))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.482571423358E-27))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(2.282138723322E-30))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.697116932588E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-8.272164988532E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-1.331045009636E-35))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.630781762223E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.748458488688E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.343654020395E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-9.562876517466E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(4.230171755377E-48))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(3.666910960522E-50))+(1.000000000*y_loc*y_loc*y_loc*y_loc*y_loc*(-3.139384967068E-20))+(x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-7.912082447522E-23))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.056561791628E-24))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.802557954940E-27))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.470918203378E-28))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-2.648692282812E-31))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.303339671597E-33))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.426813326956E-35))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(1.727290781194E-37))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.805345611340E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(-4.824682684994E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.809353021430E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(5.100139916202E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*y_loc*(7.464492412548E-52))+(1.000000000*y_loc*y_loc*y_loc*y_loc*(9.268964018621E-18))+(x_loc*y_loc*y_loc*y_loc*y_loc*(-1.531377997083E-20))+(x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-8.320722489547E-22))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-4.314812461407E-24))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-9.847091988918E-26))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(8.689416045690E-28))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(1.378314356577E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-5.665796183845E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-6.932531334446E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(1.484092300195E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(1.492298353485E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-1.428787551742E-41))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(-1.200650172329E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(9.119809086077E-47))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*y_loc*(6.773231751144E-49))+(1.000000000*y_loc*y_loc*y_loc*(3.190190706242E-15))+(x_loc*y_loc*y_loc*y_loc*(1.702022673981E-17))+(x_loc*x_loc*y_loc*y_loc*y_loc*(8.655054667145E-19))+(x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.011063985918E-21))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-4.892252477842E-23))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(2.162513313402E-25))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(3.307183816977E-27))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-1.996125697995E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.297338132691E-31))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(1.049039993500E-33))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(9.943195129374E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.866641744039E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(-2.367856259851E-40))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(3.257557479945E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(2.458998006781E-45))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*y_loc*(4.538294541393E-50))+(1.000000000*y_loc*y_loc*(-6.407634112118E-13))+(x_loc*y_loc*y_loc*(-2.332834112086E-17))+(x_loc*x_loc*y_loc*y_loc*(1.417237426917E-17))+(x_loc*x_loc*x_loc*y_loc*y_loc*(1.652990830539E-18))+(x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(3.725381796742E-20))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-3.511945028423E-22))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-5.216876752286E-24))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(3.749625814717E-26))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(4.035467912727E-28))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-2.789497532488E-30))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-2.328256033567E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(1.568770944843E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(1.139941145853E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-5.162933096051E-39))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(-3.511130071062E-41))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(6.689474139203E-44))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*y_loc*(4.345574550180E-46))+(1.000000000*y_loc*(4.403281264204E-12))+(x_loc*y_loc*(-9.848089396826E-13))+(x_loc*x_loc*y_loc*(-4.794835509428E-14))+(x_loc*x_loc*x_loc*y_loc*(3.326085117662E-16))+(x_loc*x_loc*x_loc*x_loc*y_loc*(7.172899785535E-18))+(x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-8.875294141709E-20))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.146771046747E-21))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(1.679233473355E-23))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(1.680714341851E-25))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.683437566112E-27))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.417124039012E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(9.238641306722E-32))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(6.943142790520E-34))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-2.516221184873E-36))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-1.745325683125E-38))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(2.517703029197E-41))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(1.624876083821E-43))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*y_loc*(-2.341755164527E-48))+(1.000000000*1*(2.625441308571E-08))+(x_loc*1.000000000*(1.750644880639E-10))+(x_loc*x_loc*1.000000000*(7.639753260973E-12))+(x_loc*x_loc*x_loc*1.000000000*(-1.449441737249E-13))+(x_loc*x_loc*x_loc*x_loc*1.000000000*(-2.605617501914E-15))+(x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(6.867123803478E-17))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(8.506554922799E-19))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-1.442922691883E-20))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-1.419604613451E-22))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(1.558241120891E-24))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(1.482246843221E-26))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-4.454723463264E-29))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-5.835270876962E-31))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-4.199071462702E-33))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-1.384323250043E-35))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(3.450868465845E-37))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(1.855461318215E-39))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-6.998238559677E-42))+(x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*x_loc*1.000000000*(-4.035548380822E-44));		
		if(cellperm > 2.000E-7){
					if(cellperm > 6.91E-6){
						cellresist=1.45E5; }
					else{ cellresist=1.0/cellperm; }
		}
		else{ cellresist=5.000000E6; }
		C_PROFILE(c,t,nv)=cellresist *resist_scaler;
		C_UDMI(c,t,0) = cellresist *resist_scaler;
	}
	if(ite>1) {	C_PROFILE(c,t,nv) = C_UDMI(c,t,0); }
  }  end_c_loop(c,t) 
}


/*
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	!!!!!!!!!!!!!!! EXPLOSIVE METHANE-AIR MIXTURES !!!!!!!!!!!!!
	!!!!!!!EGZ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!EGZ!!!!!!
	!!!!!!!!!!!! STORES in (user-define-memory 2)   !!!!!!!!!!!!
	!!!!!!!!!!!!!!!!!!!!!!          udm-2           !!!!!!!!!!!!
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

DEFINE_ON_DEMAND(calc_explosive_mix_2014)
{ 
		/* Currently not being used ..... corrects the blue zone in Coward's Triangle to Extend from 100% CH4 vs. ~ 15% - no significant changes noticed when nitrogen injection is used */
  Domain *d;  Thread *t;  cell_t c;  
  
  real px;  real py;  real u;
  real v;   real u1;  real v1;  real w;
  real Y_CH4, Y_O2, Y_N2, MW_CH4, MW_O2, MW_N2, MW_Mix, X_CH4, X_O2;
  real explode;
  d = Get_Domain(1);
  thread_loop_c(t,d) {
	begin_c_loop(c,t){ 
		/* Y_X = Mass Fraction of Species X  || X_X = Mole Fraction of Species X */
		Y_CH4=C_YI(c,t,0); 		
		Y_O2=C_YI(c,t,1); 		
		Y_N2=1.0-Y_CH4-Y_O2; 	
		MW_CH4= 16.043;
		MW_O2= 31.9988;
		MW_N2= 28.0134;
		MW_Mix= 1/(Y_CH4/MW_CH4+Y_O2/MW_O2+Y_N2/MW_N2);
		X_CH4=(Y_CH4*MW_Mix)/MW_CH4;	/* X = Mole Fraction of X */ 
		X_O2=(Y_O2*MW_Mix)/MW_O2;
		px= X_CH4;
		py=X_O2;
		u = 0.8529*px+0.0606; /* Near Explosive to Explosive Slope */
		v=-0.21*px+0.21;  /* Upper Explosive Limit  */
		u1=0.8864*px+0.0445;  /* Near Explosive to Requires Air Slope */
		v1=-1.3929*px+0.195;
		w=-0.113766666666667*px+0.113766666666667;
		/*v1=-1.2647*px+0.1771; Cyan to Yellow Slope Transition */
		
		/*w=-1.8545*px+0.2095; Continuation of Slope Oxygen Rich to Oxygen Poor */
		
		/* Explosive Zone - RED */
		if (py>u && px>0.055)  {
			explode=1.0E0;
			C_UDMI(c,t,2) = explode; 
		}
		/* Near Explosive Zone - ORANGE */
		else if (py>u1 && px>0.04)  {
			explode=0.81E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Fuel Rich Inert - YELLOW */
		else if (py<u1 && py>v1 && px>0.055)  {
			explode=0.66E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Oxygen Lean Inert - Green A  */
		else if (py<v1 && px>0.04) {
			explode = 0.48E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Oxygen Lean Inert - DARK  GREEN  */
		else if (py<0.08 && px<0.04) {
			explode = 0.0E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Oxygen Lean Inert - Green B  */
		else if (py<w && px<0.04) {
			explode = 0.48E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Oxygen Rich Inert - CYAN */
		else if (py>w) {
			explode = 0.27E0;
			C_UDMI(c,t,2) = explode;
		}
		/* ERROR - Not categorized by above values OR numerical error values of Oxygen above 21% */		
		else{
			explode = 2.66E0;
			C_UDMI(c,t,2) = explode;
		}		
	}   end_c_loop(c,t) }
}



DEFINE_ON_DEMAND(calc_explosive_mix_NEWER)
{ 
  Domain *d;  Thread *t;  cell_t c;
  
  real px;  real py;  real u;  real v;
  real u1;  real v1;  real w;
  real Y_CH4, Y_O2, Y_N2, MW_CH4, MW_O2, MW_N2, MW_Mix, X_CH4, X_O2;
  real explode;
  d = Get_Domain(1);
  thread_loop_c(t,d) {
	begin_c_loop(c,t) { 
		/* Y_X = Mass Fraction of Species X  || X_X = Mole Fraction of Species X */
		Y_CH4=C_YI(c,t,0); 		
		Y_O2=C_YI(c,t,1); 		
		Y_N2=1.0-Y_CH4-Y_O2; 	
		MW_CH4= 16.043;
		MW_O2= 31.9988;
		MW_N2= 28.0134;
		MW_Mix= 1/(Y_CH4/MW_CH4+Y_O2/MW_O2+Y_N2/MW_N2);
		X_CH4=(Y_CH4*MW_Mix)/MW_CH4;	/* X = Mole Fraction of X */ 
		X_O2=(Y_O2*MW_Mix)/MW_O2;
		px= X_CH4;
		py=X_O2;
		u = 0.8529*px+0.0606; /* Near Explosive to Explosive Slope */
		v=-0.21*px+0.21;  /* Upper Explosive Limit  */
		u1=0.8864*px+0.0445;  /* Near Explosive to Requires Air Slope */
		v1=-1.3929*px+0.195;
		w=v1;
		/*v1=-1.2647*px+0.1771; Cyan to Yellow Slope Transition */
		
		/*w=-1.8545*px+0.2095; Continuation of Slope Oxygen Rich to Oxygen Poor */
		
		/* Explosive Zone - RED */
		if (py>u && px>0.055 && py<v)  {
			explode=1.0E0;
			C_UDMI(c,t,2) = explode; 
		}
		/* Near Explosive Zone - ORANGE */
		else if (py>u1 && px>0.04 && py<v)  {
			explode=0.81E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Fuel Rich Inert - YELLOW */
		else if (py<u1 && py>v1 && px>0.055)  {
			explode=0.66E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Oxygen Lean Inert - Green A  */
		else if (py<v1 && px>0.04) {
			explode = 0.48E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Oxygen Lean Inert - DARK  GREEN  */
		else if (py<0.08 && px<0.04) {
			explode = 0.0E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Oxygen Lean Inert - Green B  */
		else if (py<w && px<0.04) {
			explode = 0.48E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Oxygen Rich Inert - CYAN */
		else if (py>w) {
			explode = 0.27E0;
			C_UDMI(c,t,2) = explode;
		}
		/* Explosive Zone - DARK BLUE */		
		else{
			explode = 2.66E0;
			C_UDMI(c,t,2) = explode;
		}		
	}  end_c_loop(c,t) }
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
  Domain *d;  Thread *t;  cell_t c;
  d = Get_Domain(1);
  thread_loop_c(t,d) {
	begin_c_loop(c,t) { 
		if (C_UDMI(c,t,2)>0.99e0 && C_UDMI(c,t,2)<1.01){
			/* Assign marker value for cell volume that is explosive */

			C_UDMI(c,t,3)=1.00E0*C_UDMI(c,t,1); } /* Report Volume-Volume-Integral udm-3 */

			/* Cell_Volume*Cell_Porosity*1.000e0 = The explosive volume reported */
	} end_c_loop(c,t) }
}

DEFINE_ON_DEMAND(calc_explosive_integral)
/* For use in the strata - requires manual manipulation of user defined strata porosity*/
{ 
  Domain *d;  Thread *t;  cell_t c;
  d = Get_Domain(1);
  thread_loop_c(t,d){
	begin_c_loop(c,t) { 
		if (C_UDMI(c,t,2)>0.99e0 && C_UDMI(c,t,2)<1.01){ 
			/* Assign marker value for cell volume that is explosive */
			C_UDMI(c,t,3)=1.00E0; } /* Report Volume-Volume-Integral udm-3 */
			/* Cell_Volume*1.000e0= The explosive volume reported */
	} end_c_loop(c,t) }
}

DEFINE_ON_DEMAND(reset_explosive_integral)
{ 
		/* Required to execute on transient runs - otherwise explosive volume is additive */
  Domain *d;  Thread *t;  cell_t c;
  d = Get_Domain(1);
  thread_loop_c(t,d) {
	begin_c_loop(c,t) { 
	C_UDMI(c,t,3)=0.00E0; 
	} end_c_loop(c,t) }
}


/*	
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	!!!!!!!!!!!!!!!!!!!!!!!!!!!! SUB CRITICAL PANEL ---- TRONA FIT !!!!!!!!!!!
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/


double SUB_CRITICAL_Trona_RECOVERY_CORNER(double x_loc, double y_loc)
{
	double FUN=0.0;

	FUN = pow((x_loc*y_loc),0.107302089705487)*( 0.1477 - 0.812278751377339*exp(-9.96978304250904*y_loc)*y_loc*x_loc + 0.103507270969929*exp(-688.057090793680*y_loc*x_loc) - 0.1738*exp(-6.368*y_loc) + 0.1971*x_loc*x_loc*exp(-1.38*x_loc*x_loc) + 13.6*y_loc*y_loc*exp(-2890*y_loc) - 14.56*x_loc*exp(-47.01*x_loc) + 11.19*x_loc*exp(-7883*x_loc*x_loc) + 0.07992*exp(-2.155*x_loc) - 6.274*y_loc*exp(-99.58*y_loc) + 0.03141*y_loc*exp(-8.748*y_loc*y_loc));

/* SUB-CRITICAL(Trona) RECOVERY CORNER
f(x,y) = ((x)*(y))^f * (0.1477 + c*exp(-d*y)*x*y   + d11*exp(-b11*x*y) - 0.1738*exp(-6.368*y)  +0.1971*x^2*exp(-1.38*x^2) + 13.6*y^2*exp(-2890*y) +-14.56*x*exp(-47.01*x)+ -11.19*x*exp(-7883*x^2) + 0.07992*exp(-2.155*x)+ -6.274*y*exp(-99.58*y) + 0.03141*y*exp(-8.748*y^2))'
688.057090793680	-0.812278751377339	9.96978304250904	0.103507270969929	0.107302089705487
ans = 'b11'			'c' 				'd'					'd11'				'f' */
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUB_CRITICAL_Trona_CENTER_GATEROAD(double x_loc)
{
	double FUN=0.0;

	FUN =  0.2031+0.007304*x_loc + 1.495*x_loc*exp(-19.69*x_loc) - 0.1661*exp(-162.6*x_loc*x_loc) - 0.1315*x_loc*exp(-7.204*x_loc*x_loc) - 3.298*x_loc*x_loc*exp(-44.01*x_loc*x_loc);

/* SUB-CRITICAL(Trona) CENTER GATEROAD
f(x,y) = 0.2031 + g*x +y*(0) + 1.495*x*exp(-b1*x)+ -0.1661*exp(-c1*(x)^2) + -0.1315*x*exp(-d1*(x)^2) + -3.298*x^2*exp(-e1*x^2)
Coefficients (with 95% confidence bounds):
       b1 =       19.69  (19.62, 19.76)
       c1 =       162.6  (162.2, 163)
       d1 =       7.204  (7.193, 7.214)
       e1 =       44.01  (43.92, 44.11)
       g =    0.007304  (0.007275, 0.007332)
  Mine TRONA  Center GATEROAD */
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUB_CRITICAL_Trona_STARTUP_CORNER(double x_loc, double y_loc)
{
	double FUN=0.0;

	FUN = pow((x_loc*y_loc),0.1007)*(0.1796 + 0.2762*exp(-4.552*y_loc)*y_loc*x_loc   + 0.04375*exp(-5.354*x_loc*y_loc) - 0.3093*exp(-60.54*x_loc) - 0.2702*exp(-50.36*y_loc) + 0.437*x_loc*x_loc*exp(-2.728*x_loc));

/* SUB-CRITICAL(Trona) STARTUP CORNER
General model:
     f(x,y) = (x*y)^0.1227 * (.1735 + .2789*exp(-d*y)*x*y   +( .04769*exp(-b11*x*y) - 0.2057*exp(-31.53*x) - 0.2646*exp(-40.73*y) ) +g*x^2*exp(-h*x))
Coefficients (with 95% confidence bounds):
       b11 =       4.045  (4.037, 4.053)
       d =       3.858  (3.853, 3.864)
       g =      0.5089  (0.508, 0.5099)
       h =       2.844  (2.841, 2.846)
		Mine TRONA Startup Gateroad */
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}



/*
	================================================
	=======                                 ========
	=======         MINE E Equation Fits    ========
	=======               6 regions         ========
	=======                                 ========
	================================================
*/

double SUPER_CRITICAL_MINE_E_STARTUP_CENTER(double x_loc, double y_loc)
{
	double FUN=0.0;
FUN = 0.155394214+x_loc*x_loc*(-0.004966014)	+0.142894504*y_loc*exp(-1.11507158*y_loc*y_loc)	-0.154156852*exp(-994.6190264*y_loc*y_loc)	-0.165429282*y_loc*y_loc*exp(-2.119029131*y_loc*y_loc);
			/* Mine E CHECK Startup Center */
			/* a 			+ x^2*(a1) 					+ b*y*exp(-b1*y^2) 								+ c*exp(-c1*y^2) 							+ d*y^2*exp(-d1*y^2)  			*/
			/* a			a1				b			b1			c				c1			d				d1  */
			/* 0.155394214	-0.004966014	0.142894504	1.11507158	-0.154156852	994.6190264	-0.165429282	2.119029131
			   0.155394214	-0.004966014	0.142894504	1.11507158	-0.154156852	994.6190264	-0.165429282	2.119029131
			   0.23446547	-0.007274502	0.21112871	1.254341353	-0.232563013	986.7723584	-0.288213205	2.41029839 */
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUPER_CRITICAL_MINE_E_CENTER_PANEL(double x_loc, double y_loc)
{
	double FUN=0.0;
	FUN =  0.182881808+0.000219076*y_loc-0.001701901*x_loc*x_loc-0.003415753*x_loc*x_loc*x_loc;
			/*  Mine E CHECK  Center Panel */
			/* p00  + p01*y  +  p20*x^2 + p30*x^3					*/
			/* 0.182881808	0.000219076	-0.001701901	-0.003415753
				0.182881808	0.000219076	-0.001701901	-0.003415753 */
			   /* 0.26928324	0.000607666	-0.001387445		*/
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUPER_CRITICAL_MINE_E_RECOVERY_CENTER(double x_loc, double y_loc)
{
	double FUN=0.0;
FUN = 0.034705045+x_loc*x_loc*(-0.007156676)+0.392853454*y_loc*exp(-2.690847002*y_loc*y_loc)-0.016570035*exp(-290*y_loc*y_loc)+0.206091545*y_loc*y_loc*exp(-0.513740978*y_loc*y_loc);
			/*  Mine E CHECK Recovery Center  */
			/*  a + (x)^2*(-0.007156676) + b*y*exp(-b1*y^2) + c*exp(-c1*y^2) + d*y^2*exp(-d1*y^2) 				*/
			/* a			a1				b			b1				c				c1			d				d1  */
			/* 0.034705045				0.392853454		2.690847002	-0.016570035	290			0.206091545		0.513740978
			0.034705045					0.392853454		2.690847002	-0.016570035	290			0.206091545		0.513740978
			   0.054623275					0.537305093		3.725760322	-0.028010127	290			0.501978887		0.911642577 */

	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUPER_CRITICAL_MINE_E_STARTUP_GATEROADS(double x_loc, double y_loc)
{
	double FUN=0.0;
FUN = pow((x_loc*y_loc),0.070680995)*(0.162003881+0.114056257*exp(-2.060750448*y_loc)*y_loc*x_loc   +0.027309527*exp(-2.32002878*x_loc*y_loc)-		0.134663756*exp(-8.323851432*x_loc)-0.263467643*exp(-50.02086538*y_loc)+51.01309648*x_loc*x_loc*exp(-24.66420708*x_loc));
			/* Mine E CHECK Startup Gateroads */
			/*			(x*y)^f * (				a 			+ c*exp(-d*y)*x*y   								+( d11*exp(-b11*x*y)							- d10*exp(-b10*x) 					- d01*exp(-b01*y) ) 				+g*x^2*exp(-h*x) ) */
											/*a				b01			b10			b11			c			d			d01			d10			d11			f			g			h            */
											/*0.162003881	50.02086538	8.323851432	2.32002878	0.114056257	2.060750448	0.263467643	0.134663756	0.027309527	0.070680995	51.01309648	24.66420708
											  0.173084569	44.62682947	28.23478556	8.254652434	0.248101616	3.582153601	0.240842547	0.148059341	0.009112079	0.075176133	0.5			32.27569134
											  0.222803703	49.41202954	33.3990023	4.519932999	0.217897624	3.019723703	0.399728137	0.213737696	0.035000529	0.072583782	0.36279155	2.519206805 */
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUPER_CRITICAL_MINE_E_CENTER_GATEROADS(double x_loc, double y_loc)
{
	double FUN=0.0;
FUN =  0.10083973+0.05329973*x_loc+0.000111875*y_loc+0.715710581*x_loc*exp(-3.193027724*x_loc)-0.100070375*exp(-1200.384929*x_loc*x_loc)-0.151653961*x_loc*exp(-3.716593738*x_loc*x_loc)-0.378856069*x_loc*x_loc*exp(-16.20732696*x_loc*x_loc);
			/* Mine E CHECK Center Gateroads  */
			/*			a + g*x +y*(a1) 				+ b*x*exp(-b1*x)						+ c*exp(-c1*(x)^2) 						+ d*x*exp(-d1*(x)^2) 							+ e*x^2*exp(-e1*x^2)*/
			/*'a'			'a1'		'b'			'b1'		'c'				'c1'		'd'				'd1'		'e'				'e1'		'g' */
			/*0.1			0.000111875	0.715710581	3.193027724	-0.100070375	1200.384929	-0.151653961	3.716593738	-0.378856069	16.20732696	0.05329973
  			  0.10083973	0.000603995	2.171935712	4.062177714	-0.101220528	1464.235434	-0.474820214	5.389192365	-1.477162806	23.91528913	0.128284224*/
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUPER_CRITICAL_MINE_E_RECOVERY_GATEROADS(double x_loc, double y_loc)
{
	double FUN=0.0;
FUN = pow((x_loc*y_loc),0.251307505)*( 0.197539477	-0.258183405*exp(-2.062155525*y_loc)*y_loc*x_loc +0.02301539*exp(-21.41498958*y_loc*x_loc)- 0.15928258*exp(-10.01527015*y_loc) +0.445654501*x_loc*x_loc*exp(-17.13983263*x_loc*x_loc)+4.68818221*y_loc*y_loc*exp(-5.633256844*y_loc)-13.90840849*x_loc*exp(-65.9273908*x_loc)+0.772026679*x_loc*exp(-68.99785585*x_loc*x_loc)+34.5*exp(-3200.000001*x_loc)+0.263621861*y_loc*exp(-27.16257242*y_loc)-0.255066042*y_loc*exp(-9.010678902*y_loc*y_loc));
			/*  Mine E CHECK Recovery Gateroads   */
			/* ((x)*(y))^f * (					a + 		c*exp(-d*y)*x*y   									+ d11*exp(-b11*x*y) 						- d01*exp(-b01*y)  					+g*x^2*exp(-h*x^2) 								+ k*y^2*exp(-l*y) 								+m*x*exp(-n*x)							+ o*x*exp(-p*x^2) 							+ q*exp(-r*x)						+ s*y*exp(-t*y) 						+ u*y*exp(-v*y^2))*/
			/* 												'a'					'b01'				'b11'				'c'					'd'					'd01'				'd11'				'f'					'g'					'h'					'k'					'l'					'm'					'n'					'o'					'p'					'q'					'r'					's'					't'					'u'					'v'*/
			/*												0.197539477			10.01527015			21.41498958			-0.258183405		2.062155525			0.15928258			0.02301539			0.251307505			0.445654501			17.13983263			4.68818221			5.633256844			-13.90840849		65.9273908			0.772026679			68.99785585			34.5				3200.000001			0.263621861			27.16257242			-0.255066042		9.010678902
															0.183702387			9.632781899			372.4030661			-0.176739562		2.683826404			0.169037785			0.054614777			0.167106788			-1					44.61606272			3.35898269			5.785522066			-17.23680573		69.20628701			0.339264617			39.86386103			34.5				3199.999994			1.65966029			38.95936604			-0.258196679		9.982806144*/
			/* 												0.262664370836130	8.89027552481122	228.389753817175	-0.253166473007042	3.20335828246872	0.243491669292467	0.0654918256996825	0.162024334530162	-0.0100000000000426	47.2974300364826	5.00798339750128	5.70033048005178	-27.1058247481921	73.6049605283396	0.315580700702219	51.8957956767753	38.9866139176046	3169.25520871711	2.08942405255152	30.0098038288443	-0.303675246839708	7.38256233004023*/
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


/*
	+++++++++++++++++++++++++++++++++++++
	++++                              +++
	++++       MINE C Equation fit    +++
	++++           6 regions          +++
	++++       Data fit of FLAC3D     +++
	++++                              +++
	+++++++++++++++++++++++++++++++++++++
*/

double SUPER_CRITICAL_MINE_C_STARTUP_CENTER(double x_loc, double y_loc)
{
	double FUN=0.0;
	FUN = 0.23446547+x_loc*x_loc*(-0.007274502)	+0.21112871*y_loc*exp(-1.254341353*y_loc*y_loc)	-0.232563013*exp(-986.7723584*y_loc*y_loc)	-0.288213205*y_loc*y_loc*exp(-2.41029839*y_loc*y_loc);
			/* CHECK Startup Center */
			/* a 			+ x^2*(a1) 					+ b*y*exp(-b1*y^2) 								+ c*exp(-c1*y^2) 							+ d*y^2*exp(-d1*y^2)  			*/
			/* a			a1				b			b1			c				c1			d				d1  */
			/* 0.23446547	-0.007274502	0.21112871	1.254341353	-0.232563013	986.7723584	-0.288213205	2.41029839 */
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUPER_CRITICAL_MINE_C_CENTER_PANEL(double x_loc, double y_loc)
{
	double FUN=0.0;
	FUN = 0.26928324+0.000607666*y_loc-0.001387445*x_loc*x_loc-0.005923021406*x_loc*x_loc*x_loc;
			/*  CHECK  Center Panel */
			/* p00  + p01*y  +  p20*x^2 + p30*x^3					*/
			/* 0.26928324	0.000607666	-0.001387445		*/
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUPER_CRITICAL_MINE_C_RECOVERY_CENTER(double x_loc, double y_loc)
{
	double FUN=0.0;
FUN = 0.054623275+x_loc*x_loc*(-0.007156676)+0.537305093*y_loc*exp(-3.725760322*y_loc*y_loc)-0.028010127*exp(-290*y_loc*y_loc)+0.501978887*y_loc*y_loc*exp(-0.911642577*y_loc*y_loc);
			/*  CHECK Recovery Center  */
			/*  a + (x)^2*(-0.007156676) + b*y*exp(-b1*y^2) + c*exp(-c1*y^2) + d*y^2*exp(-d1*y^2) 				*/
			/* a			a1				b			b1			c				c1			d				d1  */
			/* 0.054623275					0.537305093	3.725760322	-0.028010127	290	0.501978887	0.911642577 */
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUPER_CRITICAL_MINE_C_RECOVERY_GATEROADS(double x_loc, double y_loc)
{
	double FUN=0.0;
FUN = pow((x_loc*y_loc),0.162024335)*( 0.262664371-0.253166473007042*exp(-3.203358282*y_loc)*y_loc*x_loc +0.065491826*exp(-228.3897538*y_loc*x_loc)- 0.243491669*exp(-8.890275525*y_loc) -0.01*x_loc*x_loc*exp(-47.29743004*x_loc*x_loc)+5.007983398*y_loc*y_loc*exp(-5.70033048*y_loc)-27.10582475*x_loc*exp(-73.60496053*x_loc)+0.315580701*x_loc*exp(-51.89579568*x_loc*x_loc)+38.98661392*exp(-3169.255209*x_loc)+2.089424053*y_loc*exp(-30.00980383*y_loc)-0.303675247*y_loc*exp(-7.38256233*y_loc*y_loc));
			/*  CHECK Recovery Gateroads   */
			/* ((x)*(y))^f * (					a + 		c*exp(-d*y)*x*y   									+ d11*exp(-b11*x*y) 						- d01*exp(-b01*y)  					+g*x^2*exp(-h*x^2) 								+ k*y^2*exp(-l*y) 								+m*x*exp(-n*x)							+ o*x*exp(-p*x^2) 							+ q*exp(-r*x)						+ s*y*exp(-t*y) 						+ u*y*exp(-v*y^2))*/
			/* 												'a'					'b01'				'b11'				'c'					'd'					'd01'				'd11'				'f'					'g'					'h'					'k'					'l'					'm'					'n'					'o'					'p'					'q'					'r'					's'					't'					'u'					'v'*/
			/*												0.262664371			8.890275525			228.3897538			-0.253166473		3.203358282			0.243491669			0.065491826			0.162024335			-0.01				47.29743004			5.007983398			5.70033048			-27.10582475		73.60496053			0.315580701			51.89579568			38.98661392			3169.255209			2.089424053			30.00980383			-0.303675247		7.38256233*/
			/* 												0.262664370836130	8.89027552481122	228.389753817175	-0.253166473007042	3.20335828246872	0.243491669292467	0.0654918256996825	0.162024334530162	-0.0100000000000426	47.2974300364826	5.00798339750128	5.70033048005178	-27.1058247481921	73.6049605283396	0.315580700702219	51.8957956767753	38.9866139176046	3169.25520871711	2.08942405255152	30.0098038288443	-0.303675246839708	7.38256233004023*/
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}


double SUPER_CRITICAL_MINE_C_STARTUP_GATEROADS(double x_loc, double y_loc)
{
	double FUN=0.0;
FUN = pow((x_loc*y_loc),0.072583782)*(0.222803703+0.217897624*exp(-3.019723703*y_loc)*y_loc*x_loc   +0.035000529*exp(-4.519932999*x_loc*y_loc)-		0.213737696*exp(-33.3990023*x_loc)-0.399728137*exp(-49.33761923*y_loc)+0.36279155*x_loc*x_loc*exp(-2.519206805*x_loc));
			/* CHECK Startup Gateroads */
			/*			(x*y)^f * (				a 			+ c*exp(-d*y)*x*y   								+( d11*exp(-b11*x*y)							- d10*exp(-b10*x) 					- d01*exp(-b01*y) ) 				+g*x^2*exp(-h*x) ) */
											/*a				b01			b10			b11			c			d			d01			d10			d11			f			g			h            */
											/*0.222803703	49.41202954	33.3990023	4.519932999	0.217897624	3.019723703	0.399728137	0.213737696	0.035000529	0.072583782	0.36279155	2.519206805 */
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}

double SUPER_CRITICAL_MINE_C_CENTER_GATEROADS(double x_loc, double y_loc)
{
	double FUN=0.0;
FUN =  0.10083973+0.128284224*x_loc+0.000603995*y_loc+2.171935712*x_loc*exp(-4.062177714*x_loc)-0.101220528*exp(-1464.235434*x_loc*x_loc)-0.474820214*x_loc*exp(-5.389192365*x_loc*x_loc)-1.477162806*x_loc*x_loc*exp(-23.91528913*x_loc*x_loc);
			/* CHECK Center Gateroads  */
			/*			a + g*x +y*(a1) + b*x*exp(-b1*x)+ c*exp(-c1*(x)^2) + d*x*exp(-d1*(x)^2) + e*x^2*exp(-e1*x^2)*/
			/*'a'			'a1'		'b'			'b1'		'c'				'c1'		'd'				'd1'		'e'				'e1'		'g' */
			/*0.10083973	0.000603995	2.171935712	4.062177714	-0.101220528	1464.235434	-0.474820214	5.389192365	-1.477162806	23.91528913	0.128284224*/
	FUN = (FUN<0)?0:FUN; 
	return FUN; /* Limit the change to only positive*/}



/* 
		!!!!!!!!!!!!!!!!!!!	SCHEME MESSAGES     !!!!!!!!!!!!!!!!!!!
*/
void Print_Scheme_Variable_Settings()
{
if (RP_Variable_Exists_P("vsi/porosity-scaler")){        /* Returns true if the variable exists */
	 Message ("Porosity scaler: %g\n", RP_Get_Real("vsi/porosity-scaler")); } /* else default or manual set above is used */
  else{ Message("Porosity scaler at default value: 1\n You may define it with TUI Command: (rp-var-define 'vsi/porosity-scaler VALUE 'real #f)\n"); }

if (RP_Variable_Exists_P("vsi/maximum-porosity")){        /* Returns true if the variable exists */
	 Message ("The starting maximum porosity behind shields starts at: %g\n", RP_Get_Real("vsi/maximum-porosity")); } /* else default or manual set above is used */
  else{ Message("The starting maximum porostiy is not set. Using default value: 0.40\n You may set it with TUI Command: (rp-var-define 'vsi/maximum-porosity VALUE 'real #f)\n"); }

if (RP_Variable_Exists_P("vsi/initial-porosity")){        /* Returns true if the variable exists */
	 Message ("Initial Porosity of host rock is set to: %g\n", RP_Get_Real("vsi/initial-porosity")); } /* else default or manual set above is used */
  else{ Message("Initial Porosity of host rock is set to default value: 0.257780\n You may set it with TUI Command: (rp-var-define 'vsi/initial-porosity VALUE 'real #f)\n"); }

if (RP_Variable_Exists_P("vsi/resist-scaler")){        /* Returns true if the variable exists */
	 Message ("Resistance Scaler of Carmen-Kozeny relationship is set to: %g\n", RP_Get_Real("vsi/resist-scaler")); } /* else default or manual set above is used */
  else{ Message("Resistance Scaler of Carmen-Kozeny relationship is set to default value: 1\n You may set it with TUI Command: (rp-var-define 'vsi/resist-scaler VALUE 'real #f)\n"); }

if (RP_Variable_Exists_P("vsi/maximum-resist")){        /* Returns true if the variable exists */
	 Message ("Maximum cropped resistance (before scaler) in the gob center is set to: %g\n", RP_Get_Real("vsi/maximum-resist")); } /* else default or manual set above is used */
  else{ Message("Maximum cropped resistane (before scaler) in the gob center is set to default value: 5.00000E6\n You may set it with TUI Command: (rp-var-define 'vsi/maximum-resist VALUE 'real #f)\n"); }

if (RP_Variable_Exists_P("vsi/minimum-resist")){        /* Returns true if the variable exists */
	 Message ("Minimum cropped resistance (before scaler) in the edges of the gob is set to: %g\n", RP_Get_Real("vsi/minimum-resist")); } /* else default or manual set above is used */
  else{ Message("Minimum cropped resistance (before scaler) in the edges of the gob is set to default value: 1.45000E6\n You may set it with TUI Command: (rp-var-define 'vsi/minimum-resist VALUE 'real #f)\n"); }
}
