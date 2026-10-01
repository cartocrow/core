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

#include "cartocrow/core/cubic_bezier.h"

using namespace cartocrow;

TEST_CASE("Cubic Bézier curve area") {
	CubicBezierCurve curve({0, 0}, {1, 0}, {1, 1}, {0, 0});
	auto pl = curve.polyline(10000);
	Polygon<Inexact> polygon(pl.vertices_begin(), pl.vertices_end());
	CHECK(abs(curve.signedArea() - polygon.area()) < M_EPSILON);
}

TEST_CASE("Cubic Bézier spline area") {
	std::vector<Point<Inexact>> pts({{0, 0}, {1, 2}, {2, 4}, {1, 5}, {-1, 4}, {-3, 4}, {-4, 3}, {-4, 1}, {-3, 0}, {0, 0}});
	CubicBezierSpline spline(pts.begin(), pts.end());
	auto pl = spline.polyline(10000);
	Polygon<Inexact> polygon(pl.vertices_begin(), pl.vertices_end());
	CHECK(abs(spline.signedArea() - polygon.area()) < M_EPSILON);
}
