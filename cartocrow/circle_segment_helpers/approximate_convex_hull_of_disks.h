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

#include "cs_types.h"
#include "circle_tangents.h"

namespace cartocrow {
/// Given circles with rational radii, returns the circles that are part of their convex hull.
/// \pre the circle centers are distinct.
std::vector<RationalRadiusCircle> circlesOnConvexHull(const std::vector<RationalRadiusCircle>& circles);
/// Return the approximate convex hull, oriented counter-clockwise, of the provided circles.
/// The convex hull is approximate in the same sense that bitangents between rational radius circles
/// are approximate. That is, the returned convex hull is a superset of the exact convex hull;
/// bitangents may consist of two line segments each tangent to one circle, which meet at a point
/// outside the exact convex hull.
/// \pre the circle centers are distinct.
/// \sa rationalBitangents
CSPolygon approximateConvexHull(const std::vector<RationalRadiusCircle>& rrCircles);
/// Return the approximate convex hull, oriented counter-clockwise, of the provided circles.
/// The radii are first approximated by rational numbers; then approximate bitangents are computed.
/// If the circles have rational radius use the overloaded function that takes objects of type RationalRadiusCircle instead.
/// \pre the circle centers are distinct.
/// \sa rationalBitangents
CSPolygon approximateConvexHull(const std::vector<Circle<Exact>>& circles);
}
