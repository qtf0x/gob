#include <fstream>
#include <vector>
#include <string>
#include <iostream>

int main()
{
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

	std::ofstream output{ "set_dimensions.scm" };

	for (const auto zone : ZONES) {
		std::ifstream report{ zone + ".txt" };
		if (report.fail())
			continue;

		std::string temp;
		for (std::size_t i{ 0 }; i < 12; ++i)
			report >> temp;

		double num{ 0 };
		report >> num;

		output << "(rpsetvar 'longwallgobs/" << zone << ' ' << num << ")\n";
	}
}
