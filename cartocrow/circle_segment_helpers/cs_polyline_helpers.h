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

#pragma once

#include "cartocrow/core/polyline.h"
#include "cs_types.h"
#include <CGAL/Arr_polycurve_traits_2.h>

namespace cartocrow {
/// Return the point on the polyline nearest to the provided point.
OneRootPoint nearest(const CSPolyline& polyline, const Point<Exact>& point);
/// Return the curve in the CSPolyline that point p lies on (if any).
std::optional<CSPolyline::Curve_const_iterator> liesOn(const Point<Exact>& p, const CSPolyline& polyline);
/// Return the curve in the CSPolyline that point p lies on (if any).
std::optional<CSPolyline::Curve_const_iterator> liesOn(const OneRootPoint& p, const CSPolyline& polyline);
/// Return whether the curve overlaps the polyline and it is not a superset of the polyline.
bool liesOn(const CSXMCurve& c, const CSPolyline& polyline);
/// Convert the polyline to a CSPolycurve.
CSPolycurve arrPolycurveFromCSPolyline(const CSPolyline& polyline);
/// Convert the polyline consisting of line segments to a CSPolyline.
CSPolyline polylineToCSPolyline(const Polyline<Exact>& polyline);
/// Approximately extend the polyline on both ends along the respective tangents.
/// We do this approximately as tangents of circles may not be supported by lines with rational coefficients.
/// Endpoints that lie on circles and are extended remain on their respective circle, but may be moved to a nearby point that has rational coordinates.
/// This requires that circles part of the polyline have rational radius, which needs to be passed to the function as an argument.
/// The end points of the extended polyline have rational coordinates and are returned together with the extended polyline.
std::tuple<CSPolyline, Point<Exact>, Point<Exact>> approximateExtend(const CSPolyline& polyline, Number<Inexact> amount, Number<Exact> circleRadius);
/// Convert a polyline to a polygon by closing it around its bounding box.
/// The user can specify whether the distance kept to the bounding box, and whether closing is done clockwise or counter-clockwise.
/// The source and target of the polyline are required to have rational coordinates and passed as argument.
CSPolygon closeAroundBB(CSPolyline polyline, CGAL::Orientation orientation, Number<Inexact> offset, const Point<Exact>& source, const Point<Exact>& target);
/// An approximation of the absolute turning angle of the polyline.
double approximateAbsoluteTurningAngle(const CSPolyline& polyline);
/// Return the approximate length of the polyline.
Number<Inexact> approximateLength(const CSPolyline& polyline);
}
