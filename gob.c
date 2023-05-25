/* Test file that only does Trona fitting + VSI */

#include "udf.h"


/*---------------------- Trona fits ----------------------*/
double SUB_CRITICAL_Trona_RECOVERY_CORNER(double x_loc, double y_loc);
double SUB_CRITICAL_Trona_CENTER_GATEROAD(double x_loc);
double SUB_CRITICAL_Trona_STARTUP_CORNER (double x_loc, double y_loc);

void Print_Scheme_Variable_Settings();


DEFINE_EXECUTE_FROM_GUI(VSI_MINE_Trona_Stepped, libudf, mode)
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
  if (RP_Variable_Exists_P("gob/max-vsi")){        
	 maximum_vsi=( RP_Get_Real("gob/max-vsi") );
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
    C_CENTROID(x,c,t); /* Get mesh cell location */
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
// Print_Scheme_Variable_Settings();
}


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

