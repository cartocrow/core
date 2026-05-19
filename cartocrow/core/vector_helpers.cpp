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

#include "vector_helpers.h"

namespace cartocrow {
double orientedAngleBetween(Vector<Inexact> v, Vector<Inexact> w, CGAL::Orientation orientation) {
	if (orientation == CGAL::CLOCKWISE) return orientedAngleBetween(w, v, CGAL::COUNTERCLOCKWISE);
	return atan2(v.x() * w.y() - v.y() * w.x(), v.x() * w.x() + v.y() * w.y());
}

Number<Inexact> smallestAngleBetween(const Vector<Inexact>& v, const Vector<Inexact>& w) {
	auto x = (v * w) / (sqrt(v.squared_length()) * sqrt(w.squared_length()));
	return abs(x - 1.0) < M_EPSILON ? 0.0 : acos((v * w) / (sqrt(v.squared_length()) * sqrt(w.squared_length())));
}
}