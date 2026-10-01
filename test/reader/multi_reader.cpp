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

#include "cartocrow/reader/multi_reader.h"

using namespace cartocrow;

// Test case names need to be unique, so we add a prefix.
#define TEST_CASE_(name) TEST_CASE("[MultiReader] " name, "[MultiReader]")

TEST_CASE_("Reading points from .ipe and .gpkg") {
	using Reader = MultiReader<IpeReader, GDALReader>;

	std::vector<Point<Inexact>> points;

	CHECK(Reader::canRead("data/test_gdal_reader.gpkg"));

	Reader reader("data/test_gdal_reader.gpkg");
	std::get<GDALReader>(reader.getReader()).setLayer("test_points");

	reader.read<Multiple, Point<Inexact>, WithoutAttributes>(std::back_inserter(points));
	CHECK(points.size() == 4);

	CHECK(Reader::canRead("data/test_ipe_reader.ipe"));

	reader.load("data/test_ipe_reader.ipe");
	reader.read<Multiple, Point<Inexact>, WithoutAttributes>(std::back_inserter(points));
	CHECK(points.size() == 8);
}