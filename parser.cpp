#include <fstream>
#include <vector>
#include <string>

int main()
{
	// mesh zone names
	const std::vector<std::string> ZONES{ "startup_room_center_min_x", "startup_room_center_max_x",
					      "startup_room_center_min_y", "startup_room_center_max_y",

					      "startup_room_corner_min_x", "startup_room_corner_max_x",
					      "startup_room_corner_min_y", "startup_room_corner_max_y",

					      "mid_panel_center_min_x",	   "mid_panel_center_max_x",
					      "mid_panel_center_min_y",	   "mid_panel_center_max_y",

					      "mid_panel_gateroad_min_x",  "mid_panel_gateroad_max_x",
					      "mid_panel_gateroad_min_y",  "mid_panel_gateroad_max_y",

					      "working_face_center_min_x", "working_face_center_max_x",
					      "working_face_center_min_y", "working_face_center_max_y",

					      "working_face_corner_min_x", "working_face_corner_max_x",
					      "working_face_corner_min_y", "working_face_corner_max_y",

					      "single_part_mesh_min_x",	   "single_part_mesh_max_x",
					      "single_part_mesh_min_y",	   "single_part_mesh_max_y" };

	// Scheme script to set rp variables when loaded by Fluent
	std::ofstream output{ "set_dimensions.scm" };

	std::string temp;
	double num{ 0 };

	for (const auto zone : ZONES) {
		std::ifstream report{ zone + ".txt" };
		if (report.fail())
			continue; // zone missing, skip it

		// skip exactly 12 strings before the first value in the report
		for (std::size_t i{ 0 }; i < 12u; ++i)
			report >> temp;

		report >> num;

		// the actual Scheme command to set this RP variable
		output << "(rpsetvar 'longwallgobs/" << zone << ' ' << num << ")\n";
	}
}
