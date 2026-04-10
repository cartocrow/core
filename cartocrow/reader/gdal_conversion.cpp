#include "gdal_conversion.h"

namespace cartocrow {
PolygonSetRaw<Inexact> ogrMultiPolygonToPolygonSetRaw(const OGRMultiPolygon& multiPolygon) {
	PolygonSetRaw<Inexact> polygonSet;
    for (const auto& poly : multiPolygon) {
		auto pgnWH = ogrPolygonToPolygonWithHoles(*poly);
		polygonSet.polygons_with_holes.push_back(pgnWH);
    }
    return polygonSet;
}

Polygon<Inexact> ogrLinearRingToPolygon(const OGRLinearRing& ogrLinearRing) {
    Polygon<Inexact> polygon;
    for (auto &pt: ogrLinearRing) {
        polygon.push_back({pt.getX(), pt.getY()});
    }
    // if the begin and end vertices are equal, remove one of them
    if (polygon.container().front() == polygon.container().back()) {
        polygon.container().pop_back();
    }
    return polygon;
}

PolygonSetRaw<Inexact> ogrPolygonToPolygonSetRaw(const OGRPolygon& ogrPolygon) {
	PolygonSetRaw<Inexact> polygonSet;
	auto polygon = ogrPolygonToPolygonWithHoles(ogrPolygon);
	polygonSet.polygons_with_holes.emplace_back(polygon);
    return polygonSet;
}

PolygonWithHoles<Inexact> ogrPolygonToPolygonWithHoles(const OGRPolygon& ogrPolygon) {
	auto outer = ogrLinearRingToPolygon(*ogrPolygon.getExteriorRing());
	std::vector<Polygon<Inexact>> holes;
	for (int i = 0; i < ogrPolygon.getNumInteriorRings(); ++i) {
		holes.push_back(ogrLinearRingToPolygon(*ogrPolygon.getInteriorRing(i)));
    }
	return {outer, holes.begin(), holes.end()};
}

PolylineSet<Inexact> ogrMultiLineStringToPolylineSet(const OGRMultiLineString& ogrMultiLineString) {
	PolylineSet<Inexact> polylineSet;

	for (const auto& lineString : ogrMultiLineString) {
		polylineSet.polylines.push_back(ogrLineStringToPolyline(*lineString));
	}

	return polylineSet;
}

Polyline<Inexact> ogrLineStringToPolyline(const OGRLineString& ogrLineString) {
	Polyline<Inexact> pl;

	for (const auto& pt : ogrLineString) {
		pl.push_back({pt.getX(), pt.getY()});
	}

	return pl;
}

OGRLinearRing polygonToOGRLinearRing(const Polygon<Inexact>& polygon) {
    OGRLinearRing ring;
    for (const auto& v : polygon.vertices()) {
        auto v_ = v;
        ring.addPoint(v_.x(), v_.y());
    }
    auto v_ = polygon.vertices().front();
    ring.addPoint(v_.x(), v_.y());

    return ring;
}

OGRPolygon polygonWithHolesToOGRPolygon(const PolygonWithHoles<Inexact>& polygon) {
    assert(!polygon.is_unbounded());
    auto outerRing = polygonToOGRLinearRing(polygon.outer_boundary());
    std::vector<OGRLinearRing> holeRings;
    for (const auto& h : polygon.holes()) {
        holeRings.push_back(polygonToOGRLinearRing(h));
    }
    OGRPolygon ogrPolygon;
    ogrPolygon.addRing(&outerRing);
    for (auto& hr : holeRings) {
        ogrPolygon.addRing(&hr);
    }
    return ogrPolygon;
}

OGRMultiPolygon polygonSetToOGRMultiPolygon(const PolygonSet<Inexact>& polygonSet) {
    std::vector<PolygonWithHoles<Inexact>> pgns;
    polygonSet.polygons_with_holes(std::back_inserter(pgns));

    OGRMultiPolygon ogrMultiPolygon;
    for (const auto& pgn : pgns) {
        auto ogrPolygon = polygonWithHolesToOGRPolygon(pgn);
        ogrMultiPolygon.addGeometry(&ogrPolygon);
    }

    return ogrMultiPolygon;
}

OGRLinearRing polygonToOGRLinearRing(const Polygon<Exact>& polygon) {
	return polygonToOGRLinearRing(approximate(polygon));
}

OGRPolygon polygonWithHolesToOGRPolygon(const PolygonWithHoles<Exact>& polygon) {
	return polygonWithHolesToOGRPolygon(approximate(polygon));
}

OGRMultiPolygon polygonSetToOGRMultiPolygon(const PolygonSet<Exact>& polygonSet) {
	return polygonSetToOGRMultiPolygon(approximate(polygonSet));
}
}