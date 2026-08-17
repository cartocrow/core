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

#include "cartocrow/reader/gdal_reader.h"

using namespace cartocrow;

// Test case names need to be unique, so we prefix with GDALReader.
#define TEST_CASE_(name) TEST_CASE("[GDALReader] " name, "[GDALReader]")

TEST_CASE_("Reading points") {
	GDALReader gdalReader("data/test_gdal_reader.gpkg");
	gdalReader.setLayer("test_points");

	auto points = gdalReader.read<Multiple, Point<Inexact>, WithAttributes>();
	CHECK(points.size() == 4);

	auto exists = [&](const Point<Inexact>& point, std::string name, double weight) {
		return std::find_if(points.begin(), points.end(), [&](const auto& f) {
			const auto& attrs = f.attributes;
			const auto& pt = f.geometry;

			return CGAL::squared_distance(pt, point) < M_EPSILON &&
				   attrs.contains("name") && std::holds_alternative<std::string>(attrs.at("name")) && std::get<std::string>(attrs.at("name")) == name && 
			       attrs.contains("weight") && std::holds_alternative<double>(attrs.at("weight")) && std::get<double>(attrs.at("weight")) == weight;
		}) != points.end();
	};

	CHECK(exists({-0.450809, 0.212951}, "A", 0.4));
	CHECK(exists({0.0124533, -0.0261519}, "B", 2.3));
	CHECK(exists({-0.0772105, 0.235367}, "C", 8));
	CHECK(exists({-0.102117, 0.0709838}, "D", -1.1111));
}

TEST_CASE_("Reading polygons") {
	GDALReader gdalReader("data/test_gdal_reader.gpkg");
	gdalReader.setLayer("test_polygons");

	auto polygons = gdalReader.read<Multiple, PolygonWithHoles<Inexact>, WithAttributes>();
	CHECK(polygons.size() == 3);

	auto exists = [&](std::string name) {
		return std::find_if(polygons.begin(), polygons.end(), [&](const auto& f) {
			const auto& attrs = f.attributes;

			return attrs.contains("name") && std::holds_alternative<std::string>(attrs.at("name")) && std::get<std::string>(attrs.at("name")) == name;
		}) != polygons.end();
	};

	CHECK(exists("Alice"));
	CHECK(exists("Bob"));
	CHECK(exists("Eve"));
}

TEST_CASE_("Reading points and polygons as StraightGeometry") {
	GDALReader gdalReader("data/test_gdal_reader.gpkg");

	std::vector<GeometricFeature<StraightGeometry<Inexact>>> features;

	gdalReader.setLayer("test_points");
	gdalReader.read<Multiple, StraightGeometry<Inexact>, WithAttributes>(std::back_inserter(features));

	gdalReader.setLayer("test_polygons");
	gdalReader.read<Multiple, StraightGeometry<Inexact>, WithAttributes>(std::back_inserter(features));

	CHECK(features.size() == 7);
}

TEST_CASE_("Read layer names") {
	GDALReader gdalReader("data/test_gdal_reader.gpkg");
	auto names = gdalReader.layerNames();
	std::sort(names.begin(), names.end());
	std::vector<std::string> expected({"test_points", "test_polygons"});
	std::sort(expected.begin(), expected.end());
	CHECK(names == expected);
}

TEST_CASE_("Read spatial reference") {
	GDALReader gdalReader("data/test_gdal_reader.gpkg");
	auto sRef = gdalReader.readSpatialReference();
	CHECK(sRef.has_value());
	CHECK(*sRef == R"(GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.0174532925199433,AUTHORITY["EPSG","9122"]],AXIS["Latitude",NORTH],AXIS["Longitude",EAST],AUTHORITY["EPSG","4326"]])");
}