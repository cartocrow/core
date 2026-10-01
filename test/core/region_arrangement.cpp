/*
Copyright (C) 2026  TU Eindhoven

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "../catch.hpp"

#include <filesystem>

#include "cartocrow/core/region_arrangement.h"
#include "cartocrow/reader/region_map_reader.h"

using namespace cartocrow;

TEST_CASE("Converting a region map to an arrangement") {
	RegionMap map = ipeToRegionMap(std::filesystem::path("data/test_region_map.ipe"));
	CHECK(map.size() == 2);
	CHECK(map.contains("R1"));
	CHECK(map.contains("R2"));

	RegionArrangement arrangement = regionMapToArrangement(map);
	CHECK(arrangement.number_of_faces() == 4); // R1, R2 (2 pieces), outer face
	int num_r1 = 0;
	int num_r2 = 0;
	int num_no_id = 0;
	for (auto face_iterator = arrangement.faces_begin(); face_iterator != arrangement.faces_end();
	     ++face_iterator) {
		if (face_iterator->data() == "R1") {
			num_r1++;
		} else if (face_iterator->data() == "R2") {
			num_r2++;
		} else if (face_iterator->data() == "") {
			num_no_id++;
		} else {
			FAIL_CHECK();
		}
	}
	CHECK(num_r1 == 1);
	CHECK(num_r2 == 2);
	CHECK(num_no_id == 1);
}

//TEST_CASE("Converting overlapping regions to an arrangement (should throw)") {
//	RegionMap map = ipeToRegionMap(std::filesystem::path("data/test_region_map_overlap.ipe"));
//	CHECK(map.size() == 2);
//	CHECK(map.contains("R1"));
//	CHECK(map.contains("R2"));
//	CHECK_THROWS_WITH(regionMapToArrangement(map), Catch::StartsWith("Found overlapping regions "));
//}
