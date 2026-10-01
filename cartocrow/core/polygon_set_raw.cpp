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

#include "polygon_set_raw.h"

namespace cartocrow {
PolygonSetRaw<Inexact> approximate(const PolygonSetRaw<Exact>& pgs) {
	PolygonSetRaw<Inexact> approximated;
	for (const PolygonWithHoles<Exact>& pgn : pgs.polygons_with_holes) {
		approximated.polygons_with_holes.push_back(cartocrow::approximate(pgn));
	}
	return approximated;
}

PolygonSetRaw<Exact> pretendExact(const PolygonSetRaw<Inexact>& pgs) {
	PolygonSetRaw<Exact> exact;
	for (const auto& pgn : pgs.polygons_with_holes) {
		exact.polygons_with_holes.push_back(cartocrow::pretendExact(pgn));
	}
	return exact;
}
}