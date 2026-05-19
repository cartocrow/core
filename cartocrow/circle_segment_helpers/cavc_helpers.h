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
#include <cavc/polylineoffset.hpp>

namespace cartocrow {
/// Approximately dilate a CSPolygonSet.
/// That is, return the approximate Minkowski sum of the provided CSPolygonSet with a disk of the provided radius.
CSPolygonSet approximateDilate(const CSPolygonSet& csPolygonSet, double radius);
/// Erode a CSPolygonSet.
/// That is, return the approximate Minkowski difference of the provided CSPolygonSet with a disk of the provided radius.
CSPolygonSet approximateErode(const CSPolygonSet& csPolygonSet, double radius);
/// Perform the opening operator on the CSPolygonSet.
/// That is, first erode then dilate with a disk of the provided radius.
CSPolygonSet approximateOpening(const CSPolygonSet& csPolygonSet, Number<Exact> radius);
/// Perform the closing operator on the CSPolygonSet.
/// That is, first dilate then erode with a disk of the provided radius.
CSPolygonSet approximateClosing(const CSPolygonSet& csPolygonSet, Number<Exact> radius);
/// Smooth a CSPolygonSet by first applying the closing operator and then the opening operator.
CSPolygonSet approximateSmoothCO(const CSPolygonSet& csPolygonSet, double radius);
/// Smooth a CSPolygonSet by first applying the opening operator and then the closing operator.
CSPolygonSet approximateSmoothOC(const CSPolygonSet& csPolygonSet, double radius);
/// Approximately dilate a CSPolygonSet.
/// That is, return the approximate Minkowski sum of the provided CSPolygonSet with a disk of the provided radius.
CSPolygonSet approximateDilate(const CSPolygonSet& csPolygonSet, Number<Exact> radius);
/// Erode a CSPolygonSet.
/// That is, return the approximate Minkowski difference of the provided CSPolygonSet with a disk of the provided radius.
CSPolygonSet approximateErode(const CSPolygonSet& csPolygonSet, Number<Exact> radius);
/// Perform the opening operator on the CSPolygonSet.
/// That is, first erode then dilate with a disk of the provided radius.
CSPolygonSet approximateOpening(const CSPolygonSet& csPolygonSet, Number<Exact> radius);
/// Perform the closing operator on the CSPolygonSet.
/// That is, first dilate then erode with a disk of the provided radius.
CSPolygonSet approximateClosing(const CSPolygonSet& csPolygonSet, Number<Exact> radius);
/// Smooth a CSPolygonSet by first applying the closing operator and then the opening operator.
CSPolygonSet approximateSmoothCO(const CSPolygonSet& csPolygonSet, Number<Exact> radius);
/// Smooth a CSPolygonSet by first applying the opening operator and then the closing operator.
CSPolygonSet approximateSmoothOC(const CSPolygonSet& csPolygonSet, Number<Exact> radius);

cavc::Polyline<double> cavcPolyline(const CSPolygon& polygon);
cavc::Polyline<double> cavcPolyline(const CSPolyline& polyline);
}
