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

#include "cs_types.h"

namespace cartocrow {
std::vector<Point<Exact>> makeExact(const std::vector<Point<Inexact>>& points) {
	std::vector<Point<Exact>> exact_points;
	std::transform(points.begin(), points.end(), std::back_inserter(exact_points),
	               [](const Point<Inexact>& pt) { return pretendExact(pt); });
	return exact_points;
}

Point<Inexact> approximateOneRootPoint(const OneRootPoint &algebraic_point) {
	return {CGAL::to_double(algebraic_point.x()), CGAL::to_double(algebraic_point.y())};
}

OneRootPoint pretendOneRootPoint(const Point<Inexact>& point) {
    OneRootNumber x = point.x();
    OneRootNumber y = point.y();
    return {x, y};
}

OneRootPoint pretendOneRootPoint(const Point<Exact>& point) {
    OneRootNumber x = point.x();
    OneRootNumber y = point.y();
    return {x, y};
}
}