#pragma once

#include <ogrsf_frmts.h>
#include "cartocrow/core/core.h"
#include "cartocrow/core/polyline.h"

namespace cartocrow {
PolygonSet<Exact> ogrMultiPolygonToPolygonSet(const OGRMultiPolygon& multiPolygon);
PolygonSet<Exact> ogrPolygonToPolygonSet(const OGRPolygon& ogrPolygon);
Polygon<Exact> ogrLinearRingToPolygon(const OGRLinearRing& ogrLinearRing);
std::vector<Polyline<Exact>> ogrMultiLineStringToMultiPolyline(const OGRMultiLineString& ogrMultiLineString);
Polyline<Exact> ogrLineStringToPolyline(const OGRLineString& ogrLineString);
PolygonWithHoles<Exact> ogrPolygonToPolygonWithHoles(const OGRPolygon& ogrPolygon);
OGRLinearRing polygonToOGRLinearRing(const Polygon<Inexact>& polygon);
OGRPolygon polygonWithHolesToOGRPolygon(const PolygonWithHoles<Inexact>& polygon);
OGRMultiPolygon polygonSetToOGRMultiPolygon(const PolygonSet<Inexact>& polygonSet);
OGRLinearRing polygonToOGRLinearRing(const Polygon<Exact>& polygon);
OGRPolygon polygonWithHolesToOGRPolygon(const PolygonWithHoles<Exact>& polygon);
OGRMultiPolygon polygonSetToOGRMultiPolygon(const PolygonSet<Exact>& polygonSet);
}
