#include <math.h> // for fabs

#include "utils.h" // Fluent macros

double Cell_Inertia_Resistance(double cellporo, double initial_inertia_resistance)
{
	/* The Blake-Kozeny equation for flow through porous media was
used to estimate the inertia resistance or C2 value in FLUENT of the gob as follows:
C2 = 3.5/Dp * (1-n)/n^3
where the initial inertia resistance is base on the intial porosity of the rock, and 
the change is calcuated as:
C2 = C2_inital * (1-n)/n^3
*/

	return (initial_inertia_resistance * (1.0000000 - cellporo) / (cellporo * cellporo * cellporo));
}

double Initial_Inertia_Resistance()
{
	/* C2_initial=3.5/Dp * (1-n)/n^3 
	Dp is the mean particle diameter, and n is porosity (%) 
	mean particle diameter from Pappas & Mark 1993 = 0.2 meters, Kozeny constant 180.  */

	double initial_porosity = 0.2577800000000;
	if (RP_Variable_Exists_P("vsi/initial-poroisty")) { /* Returns true if the variable exists */
		initial_porosity = (RP_Get_double("vsi/initial-porosity"));
	}

	return (3.5 / 0.2000000 * (1.000000000000 - initial_porosity) /
		(initial_porosity * initial_porosity * initial_porosity));
}

double Initial_Perm()
{
	/* K_o=n^3 /(180 * (1-n)^2) * d^2
	where K_o is permeability (miliDarcies), n is porosity (%) 
	and d is the mean particle diameter (meters).
	mean particle diameter from Pappas & Mark 1993 = 0.2 meters, Kozeny constant 180.  */

	double initial_porosity = 0.2577800000000;
	if (RP_Variable_Exists_P("vsi/initial-poroisty")) { /* Returns true if the variable exists */
		initial_porosity = (RP_Get_double("vsi/initial-porosity"));
	}

	return (initial_porosity * initial_porosity * initial_porosity /
		(180.0000000000 * (1.0000000000 - initial_porosity) * (1.000000000 - initial_porosity)) *
		0.20000000000 * 0.20000000000);
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
	cellperm = initial_permeability / 0.24100000000 * (cellporo * cellporo * cellporo) /
		   ((1.00000000 - cellporo) * (1.0000000 - cellporo));
	return (1.0 / cellperm);
}

bool fequal(const double num1, const double num2)
{
	return fabs(num2 - num1) < 1e-6;
}

double clamp_positive(const double num)
{
	return (num < 0) ? 0 : num;
}

double clamp(const double num, const double min, const double max)
{
	if (num < min)
		return min;
	if (num > max)
		return max;

	return num;
}

void get_thread_dimensions(const int thread_id, real *const width, real *const length)
{
	// retrieve a pointer to the selected thread
	Thread *t = Lookup_Thread(domain, thread_id);

	// ND_ND is just 2 for 2D, 3 for 3D
	real loc[ND_ND]; // mesh cell location "vector"

	bool first_iteration = true;

	real min_x;
	real max_x;
	real min_y;
	real max_y;

	cell_t c; // current cell index w/in thread

	begin_c_loop(c, t) // loop over all cells in the thread
	{
		// get mesh cell location
		C_CENTROID(loc, c, t);

		// get min and max x and y locations
		if (loc[0] < min_x || first_iteration)
			min_x = loc[0];
		if (loc[0] > max_x || first_iteration)
			max_x = loc[0];
		if (loc[1] < min_y || first_iteration)
			min_y = loc[1];
		if (loc[1] > max_y || first_iteration)
			max_y = loc[1];

		first_iteration = false;
	}
	end_c_loop(c, t);

	// output thread dimensions
	if (width)
		&width = max_x - min_x;
	if (length)
		&length = max_y - min_y;
}

void get_offsets_sub_critical(real *const x_offset, real *const y_offset)
{
	// ND_ND is just 2 for 2D, 3 for 3D
	real loc[ND_ND]; // mesh cell location "vector"

	bool first_iteration = true;

	real max_x;
	real max_y;

	cell_t c; // current cell index w/in thread

	// retrieve the ID for the working face corner thread
	const int THREAD_ID = RP_Get_Integer("longwallgobs/working_face_corner_id");

	// retrieve a pointer to the selected thread
	Thread *t = Lookup_Thread(domain, THREAD_ID);

	begin_c_loop(c, t) // loop over all cells in thread
	{
		// get mesh cell location
		C_CENTROID(loc, c, t);

		if (loc[0] > max_x || first_iteration)
			max_x = loc[0];
		if (loc[1] > max_y || first_iteration)
			max_y = loc[1];

		first_iteration = false;
	}
	end_c_loop(c, t);

	if (x_offset)
		&x_offset = max_x;
	if (y_offset)
		&y_offset = max_y;
}

void get_offsets_super_critical(real *const x_offset, real *const y_offset)
{
	// ND_ND is just 2 for 2D, 3 for 3D
	real loc[ND_ND]; // mesh cell location "vector"

	bool first_iteration = true;

	real min_x;
	real max_x;
	real max_y;

	cell_t c; // current cell index w/in thread

	// retrieve the ID for the working face corner thread
	const int THREAD_ID = RP_Get_Integer("longwallgobs/working_face_center_id");

	// retrieve a pointer to the selected thread
	Thread *t = Lookup_Thread(domain, THREAD_ID);

	begin_c_loop(c, t) // loop over all cells in threads
	{
		// get mesh cell location
		C_CENTROID(loc, c, t);

		if (loc[0] < min_x || first_iteration)
			min_x = loc[0];
		if (loc[0] > max_x || first_iteration)
			max_x = loc[0];
		if (loc[1] > max_y || first_iteration)
			max_y = loc[1];

		first_iteration = false;
	}
	end_c_loop(c, t);

	if (x_offset)
		&x_offset = (max_x - min_x) / 2;
	if (y_offset)
		&y_offset = max_y;
}

void get_offsets_single_part(real *const x_offset, real *const y_offset)
{
	// ND_ND is just 2 for 2D, 3 for 3D
	real loc[ND_ND]; // mesh cell location "vector"

	bool first_iteration = true;

	real min_x;
	real max_x;
	real max_y;

	cell_t c; // current cell index w/in thread

	// expect all zones/threads to be in a single domain
	Domain *d = Get_Domain(1);

	thread_loop_c(t, d) // loop over all threads in domain
	{
		begin_c_loop(c, t)
		{
			// get mesh cell location
			C_CENTROID(loc, c, t);

			if (loc[0] < min_x || first_iteration)
				min_x = loc[0];
			if (loc[0] > max_x || first_iteration)
				max_x = loc[0];
			if (loc[1] > max_y || first_iteration)
				max_y = loc[1];

			first_iteration = false;
		}
	}
	end_c_loop(c, t);

	if (x_offset)
		&x_offset = (max_x - min_x) / 2;
	if (y_offset)
		&y_offset = max_y;
}
