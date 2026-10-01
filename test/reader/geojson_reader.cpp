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

#include "cartocrow/reader/geojson_reader.h"

using namespace cartocrow;

// Test case names need to be unique, so we add a prefix.
#define TEST_CASE_(name) TEST_CASE("[GeoJSONReader] " name, "[GeoJSONReader]")

TEST_CASE_("Reading points") {
	using nlohmann::json;
	json j;
	j["type"] = "FeatureCollection";
	j["name"] = "point_test";
	j["features"] = {
	    {
			{"type", "Feature"},
	    	{"properties", {
	    	    { "weight", 0.4 },
				{ "name", "A" },
			}},
	    	{"geometry", {
	    	    { "type", "Point" },
	    	    { "coordinates", {0, 0}}
	    	}}
		},
	    {
			{"type", "Feature"},
	    	{"properties", {
	    	    { "weight", 0.1 },
				{ "name", "B" },
			}},
	    	{"geometry", {
	    	    { "type", "Point" },
	    	    { "coordinates", {1, 0}}
	    	}}
		},
	};
	GeoJSONReader reader(j);

	auto points = reader.read<Multiple, Point<Inexact>, WithAttributes>();
	CHECK(points.size() == 2);

	auto exists = [&](const Point<Inexact>& point, std::string name, double weight) {
		return std::find_if(points.begin(), points.end(), [&](const auto& f) {
			const auto& attrs = f.attributes;
			const auto& pt = f.geometry;

			return CGAL::squared_distance(pt, point) < M_EPSILON &&
				   attrs.contains("name") && std::holds_alternative<std::string>(attrs.at("name")) && std::get<std::string>(attrs.at("name")) == name && 
			       attrs.contains("weight") && std::holds_alternative<double>(attrs.at("weight")) && std::get<double>(attrs.at("weight")) == weight;
		}) != points.end();
	};

	CHECK(exists({0, 0}, "A", 0.4));
	CHECK(exists({1, 0}, "B", 0.1));
}