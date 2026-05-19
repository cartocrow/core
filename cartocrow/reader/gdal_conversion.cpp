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

#include "gdal_conversion.h"

namespace cartocrow {
PolygonSet<Exact> ogrMultiPolygonToPolygonSet(const OGRMultiPolygon& multiPolygon) {
    PolygonSet<Exact> polygonSet;
    for (auto& poly: multiPolygon) {
        for (auto& linearRing: *poly) {
            auto polygon = ogrLinearRingToPolygon(*linearRing);
            if (polygon.is_clockwise_oriented()) {
                polygon.reverse_orientation();
            }
            polygonSet.symmetric_difference(polygon);
        }
    }
    return polygonSet;
}

Polygon<Exact> ogrLinearRingToPolygon(const OGRLinearRing& ogrLinearRing) {
    Polygon<Exact> polygon;
    for (auto &pt: ogrLinearRing) {
        polygon.push_back({pt.getX(), pt.getY()});
    }
    // if the begin and end vertices are equal, remove one of them
    if (polygon.container().front() == polygon.container().back()) {
        polygon.container().pop_back();
    }
    return polygon;
}

PolygonSet<Exact> ogrPolygonToPolygonSet(const OGRPolygon& ogrPolygon) {
    PolygonSet<Exact> polygonSet;
    for (auto& linearRing : ogrPolygon) {
        Polygon<Exact> polygon;
        for (auto& pt : *linearRing) {
            polygon.push_back({pt.getX(), pt.getY()});
        }
        // if the begin and end vertices are equal, remove one of them
        if (polygon.container().front() == polygon.container().back()) {
            polygon.container().pop_back();
        }
        if (polygon.is_clockwise_oriented()) {
            polygon.reverse_orientation();
        }
        polygonSet.symmetric_difference(polygon);
    }
    return polygonSet;
}

PolygonWithHoles<Exact> ogrPolygonToPolygonWithHoles(const OGRPolygon& ogrPolygon) {
    std::vector<PolygonWithHoles<Exact>> pgns;
    ogrPolygonToPolygonSet(ogrPolygon).polygons_with_holes(std::back_inserter(pgns));
    assert(pgns.size() == 1);
    return pgns.front();
}

std::vector<Polyline<Exact>> ogrMultiLineStringToMultiPolyline(const OGRMultiLineString& ogrMultiLineString) {
	std::vector<Polyline<Exact>> pls;

	for (const auto& lineString : ogrMultiLineString) {
		pls.push_back(ogrLineStringToPolyline(*lineString));
	}

	return pls;
}

Polyline<Exact> ogrLineStringToPolyline(const OGRLineString& ogrLineString) {
	Polyline<Exact> pl;

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