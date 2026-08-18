#pragma once

#include <cartocrow/core/core.h>
#include <cartocrow/core/transform_helpers.h>

namespace cartocrow {
template <class K>
struct PolygonSetRaw {
	std::vector<PolygonWithHoles<K>> polygons_with_holes;

	PolygonSetRaw<K> transform(const CGAL::Aff_transformation_2<K>& trans) const {
		PolygonSetRaw<K> transformed;
		for (const auto& pgn : polygons_with_holes) {
			transformed.polygons_with_holes.push_back(cartocrow::transform(trans, pgn));
		}
		return transformed;
	}

	Box bbox() const {
		return CGAL::bbox_2(polygons_with_holes.begin(), polygons_with_holes.end());
	}

	PolygonSet<K> polygonSet() const {
		PolygonSet<K> polygonSet;
		for (auto pgn : polygons_with_holes) {
			if (!pgn.outer_boundary().is_simple()) {
				throw std::runtime_error("Encountered non-simple polygon");
			}
			if (pgn.outer_boundary().is_clockwise_oriented()) {
				pgn.outer_boundary().reverse_orientation();
			}
			for (auto& hole : pgn.holes()) {
				if (!hole.is_simple()) {
					throw std::runtime_error("Encountered non-simple polygon");
				}
				if (hole.is_counterclockwise_oriented()) {
					hole.reverse_orientation();
				}
			}
			polygonSet.symmetric_difference(pgn);
		}
		return polygonSet;
	}

	PolygonSetRaw() = default;

	PolygonSetRaw(Polygon<K> polygon) {
		polygons_with_holes.emplace_back(std::move(polygon));
	}

	PolygonSetRaw(PolygonWithHoles<K> polygon) {
		polygons_with_holes.push_back(std::move(polygon));
	}
};

PolygonSetRaw<Inexact> approximate(const PolygonSetRaw<Exact>& pgs);
PolygonSetRaw<Exact> pretendExact(const PolygonSetRaw<Inexact>& pgs);
}