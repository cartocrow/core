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
#include "cartocrow/renderer/ipe_renderer.h"

using namespace cartocrow;
using namespace renderer;

TEST_CASE("Reading points") {
	IpeReader ipeReader;

	auto points = ipeReader.readV<Point<Inexact>>("data/test_ipe_reader.ipe");
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

TEST_CASE("Reading points and a polygon") {
	std::vector<Point<Inexact>> points(
	    {{144.544, 155.39}, {123.907, 113.135}, {178.446, 67.44}, {249.199, 124.927}});
	Polygon<Inexact> expectedPolygon(points.begin(), points.end());

	using PointOrPoly = std::variant<Point<Inexact>, Polygon<Inexact>>;

	IpeReader ipeReader;
	auto pointOrPolys = ipeReader.readV<PointOrPoly>("data/test_ipe_reader.ipe");

	CHECK(pointOrPolys.size() == 5);

	auto exists = [&](PointOrPoly&& p) {
		return std::find(pointOrPolys.begin(), pointOrPolys.end(), p) != pointOrPolys.end();
	};
	
	CHECK(exists(expectedPolygon));
	CHECK(exists(Point<Inexact>{0, 0}));
	CHECK(exists(Point<Inexact>{64, 64}));
	CHECK(exists(Point<Inexact>{64, 0}));
	CHECK(exists(Point<Inexact>{0, 64}));
}

TEST_CASE("Reading polygon sets") {
	// Test whether polygon is automatically converted to PolygonSetRaw.
	// The file contains 2 polygon sets and 1 polygon.
	IpeReader ipeReader;
	ipeReader.setPage(1);

	auto psrs = ipeReader.readV<PolygonSetRaw<Inexact>>("data/test_ipe_reader.ipe");
	auto pgns = ipeReader.readV<Polygon<Inexact>>("data/test_ipe_reader.ipe");

	CHECK(psrs.size() == 3);
	CHECK(pgns.size() == 1);
}

TEST_CASE("Reading polylines, segments, Bézier curves and splines") {
	std::filesystem::path fn("data/test_ipe_reader.ipe");

	// Test open geometries.
	// The file contains a line segment, a polyline, a cubic Bézier curve, a cubic Bézier spline.
	IpeReader ipeReader;
	ipeReader.setPage(2);

	auto ls = ipeReader.readV<Segment<Inexact>>(fn);
	CHECK(ls.size() == 1); // should only return the line segment

	auto pls = ipeReader.readV<Polyline<Inexact>>(fn);
	CHECK(pls.size() == 2); // should return the polyline and the line segment

	auto cbcs = ipeReader.readV<CubicBezierCurve>(fn);
	CHECK(cbcs.size() == 2); // should return the cubic Bézier curve and the line segment

	auto cbss = ipeReader.readV<CubicBezierSpline>(fn);
	CHECK(cbss.size() == 4); // should return all
}

TEST_CASE("Read points from specific layer") {
	std::filesystem::path fn("data/test_ipe_reader.ipe");

	IpeReader ipeReader;
	ipeReader.setPage(3);

	ipeReader.setLayerFilter("red");
	auto redPoints = ipeReader.readV<Point<Inexact>>(fn);
	ipeReader.setLayerFilter(0);
	auto bluePoints = ipeReader.readV<Point<Inexact>>(fn);
	ipeReader.setLayerFilter("green");
	auto greenPoints = ipeReader.readV<Point<Inexact>>(fn);

	CHECK(bluePoints.size() == 7);
	CHECK(redPoints.size() == 11);
	CHECK(greenPoints.size() == 8);
}

TEST_CASE("Read attributes") {
	std::filesystem::path fn("data/test_ipe_reader.ipe");

	IpeReader ipeReader;
	ipeReader.setPage(3);

	ipeReader.setLayerFilter("red");
	auto redPoints = ipeReader.readWithAttributesV<Point<Inexact>>(fn);
	ipeReader.setLayerFilter(0);
	auto bluePoints = ipeReader.readWithAttributesV<Point<Inexact>>(fn);
	ipeReader.setLayerFilter("green");
	auto greenPoints = ipeReader.readWithAttributesV<Point<Inexact>>(fn);

	CHECK(bluePoints.size() == 7);
	for (const auto& bp : bluePoints) {
		CHECK(bp.attributes.contains("fill"));
		CHECK(std::holds_alternative<std::string>(bp.attributes.at("fill")));
		CHECK(std::get<std::string>(bp.attributes.at("fill")) == "CB light blue");
	}

	CHECK(redPoints.size() == 11);
	for (const auto& rp : redPoints) {
		CHECK(rp.attributes.contains("fill"));
		CHECK(std::holds_alternative<std::string>(rp.attributes.at("fill")));
		CHECK(std::get<std::string>(rp.attributes.at("fill")) == "CB light red");
	}

	CHECK(greenPoints.size() == 8);
	for (const auto& gp : greenPoints) {
		CHECK(gp.attributes.contains("fill"));
		CHECK(std::holds_alternative<std::string>(gp.attributes.at("fill")));
		CHECK(std::get<std::string>(gp.attributes.at("fill")) == "CB light green");
	}
}

//TEST_CASE("Manual check: load and save test_ipe_reader.ipe; geometries should be equivalent") {
//	IpeReader ipeReader;
//	IpeRenderer ipeRenderer;
//	
//	auto fn = "data/test_ipe_reader.ipe";
//
//	for (int pageIndex = 0; pageIndex < ipeReader.numberOfPages(fn); ++pageIndex) {
//		ipeReader.setPage(pageIndex);
//		auto geoms = ipeReader.readV<IntermediateIpeGeometry>(fn);
//		ipeRenderer.addPainting([geoms](GeometryRenderer& r) {
//			for (const auto& g : geoms) {
//				std::visit([&](auto& someG) { r.draw(someG); }, g);
//			}
//		});
//		ipeRenderer.nextPage();
//	}
//
//	ipeRenderer.save("test_ipe_reader_saved.ipe");
//}