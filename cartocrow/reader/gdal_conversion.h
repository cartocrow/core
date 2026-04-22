#ifndef CARTOCROW_GDAL_CONVERSION_H
#define CARTOCROW_GDAL_CONVERSION_H

#include <ogrsf_frmts.h>
#include "cartocrow/core/core.h"
#include "cartocrow/core/polyline.h"
#include "cartocrow/core/polyline_set.h"
#include "cartocrow/core/polygon_set_raw.h"
#include "cartocrow/core/point_set.h"

namespace cartocrow {
PolygonSetRaw<Inexact> ogrMultiPolygonToPolygonSetRaw(const OGRMultiPolygon& multiPolygon);
PolygonSetRaw<Inexact> ogrPolygonToPolygonSetRaw(const OGRPolygon& ogrPolygon);
Polygon<Inexact> ogrLinearRingToPolygon(const OGRLinearRing& ogrLinearRing);
PolylineSet<Inexact> ogrMultiLineStringToPolylineSet(const OGRMultiLineString& ogrMultiLineString);
Polyline<Inexact> ogrLineStringToPolyline(const OGRLineString& ogrLineString);
PolygonWithHoles<Inexact> ogrPolygonToPolygonWithHoles(const OGRPolygon& ogrPolygon);
PointSet<Inexact> ogrMultiPointToPointSet(const OGRMultiPoint& ogrMultiPoint);
Point<Inexact> ogrPointToPoint(const OGRPoint& ogrPoint);
OGRLinearRing polygonToOGRLinearRing(const Polygon<Inexact>& polygon);
OGRPolygon polygonWithHolesToOGRPolygon(const PolygonWithHoles<Inexact>& polygon);
OGRMultiPolygon polygonSetToOGRMultiPolygon(const PolygonSet<Inexact>& polygonSet);
OGRLinearRing polygonToOGRLinearRing(const Polygon<Exact>& polygon);
OGRPolygon polygonWithHolesToOGRPolygon(const PolygonWithHoles<Exact>& polygon);
OGRMultiPolygon polygonSetToOGRMultiPolygon(const PolygonSet<Exact>& polygonSet);
}

#endif //CARTOCROW_GDAL_CONVERSION_H
