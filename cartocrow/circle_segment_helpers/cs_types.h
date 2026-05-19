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

#include "cartocrow/core/core.h"
#include "cartocrow/core/general_polyline.h"
#include <CGAL/Arr_circle_segment_traits_2.h>
#include <CGAL/Arr_polycurve_traits_2.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Gps_circle_segment_traits_2.h>
#include <CGAL/Gps_traits_2.h>

namespace cartocrow {
typedef CGAL::Arr_circle_segment_traits_2<Exact> ArrCSTraits;
typedef CGAL::Gps_circle_segment_traits_2<Exact> GpsCSTraits;
typedef CGAL::Arr_polycurve_traits_2<ArrCSTraits> PolycurveCSTraits;
typedef GpsCSTraits::Polygon_2 CSPolygon;
typedef GpsCSTraits::Polygon_with_holes_2 CSPolygonWithHoles;
typedef CGAL::General_polygon_set_2<GpsCSTraits> CSPolygonSet;
typedef General_polyline_2<ArrCSTraits> CSPolyline;
typedef PolycurveCSTraits::Curve_2 CSPolycurve;
typedef PolycurveCSTraits::X_monotone_curve_2 CSXMPolycurve;
typedef CGAL::Arrangement_2<ArrCSTraits> CSArrangement;

typedef ArrCSTraits::X_monotone_curve_2 CSXMCurve;
typedef ArrCSTraits::Curve_2 CSCurve;
typedef ArrCSTraits::CoordNT OneRootNumber;
typedef ArrCSTraits::Point_2 OneRootPoint;

Point<Inexact> approximateOneRootPoint(const OneRootPoint &algebraic_point);
OneRootPoint pretendOneRootPoint(const Point<Inexact>& point);
OneRootPoint pretendOneRootPoint(const Point<Exact>& point);
}
