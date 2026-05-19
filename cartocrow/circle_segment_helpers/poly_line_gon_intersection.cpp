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

#include "poly_line_gon_intersection.h"

namespace cartocrow {
std::vector<CSPolyline> intersection(const CSPolyline& line, const CSPolygon& gon, bool keepOverlap) {
	CSPolygonWithHoles withHoles(gon);
	return intersection(line, withHoles, keepOverlap);
}

std::vector<CSPolyline> difference(const CSPolyline& line, const CSPolygon& gon, bool keepOverlap) {
	CSPolygonWithHoles withHoles(gon);
	return difference(line, withHoles, keepOverlap);
}

std::vector<CSPolyline> intersection(const CSPolyline& line, const CSPolygonWithHoles& gon, bool keepOverlap) {
	std::vector<CSPolyline> polylines;
	intersection(line, gon, std::back_inserter(polylines), false, keepOverlap);
	return polylines;
}

std::vector<CSPolyline> difference(const CSPolyline& line, const CSPolygonWithHoles& gon, bool keepOverlap) {
	std::vector<CSPolyline> polylines;
	intersection(line, gon, std::back_inserter(polylines), true, keepOverlap);
	return polylines;
}
}
