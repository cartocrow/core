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

#include "polyline.h"
#include "polyline_set.h"
#include "point_set.h"
#include "polygon_set_raw.h"

namespace cartocrow {
/// Type that serves as an internal representation of the straight (i.e. linear) features of the OGC Simple Feature Access.
/// OGC name:                         MultiPolygon      Polygon              LinearRing  MultiLineString LineString   Point     MultiPoint
template <class K>
using StraightGeometry = std::variant<PolygonSetRaw<K>, PolygonWithHoles<K>, Polygon<K>,
                                      PolylineSet<K>, Polyline<K>, Point<K>, PointSet<K>>;
}