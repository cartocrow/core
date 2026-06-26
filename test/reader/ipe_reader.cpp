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

#include "cartocrow/reader/ipe_reader.h"

using namespace cartocrow;

TEST_CASE("Reading points") {
	IpeReader ipeReader;

	std::vector<Point<Inexact>> points;
	ipeReader.read<Point<Inexact>>("data/test_ipe_reader.ipe", std::back_inserter(points));
	CHECK(points.size() == 4);

	auto exists = [&](Point<Inexact> point) {
		return std::find(points.begin(), points.end(), point) != points.end();
	};

	CHECK(exists({0, 0}));
	CHECK(exists({64, 64}));
	CHECK(exists({64, 0}));
	CHECK(exists({0, 64}));
}

TEST_CASE("Reading a polygon") {
	std::vector<Point<Inexact>> points({{144.544, 155.39}, {123.907, 113.135}, {178.446, 67.44}, {249.199, 124.927}});
	Polygon<Inexact> expectedPolygon(points.begin(), points.end());

	IpeReader ipeReader;
	auto parsedPolygon = ipeReader.readSingle<Polygon<Inexact>>("data/test_ipe_reader.ipe");
	CHECK(parsedPolygon == expectedPolygon);
}