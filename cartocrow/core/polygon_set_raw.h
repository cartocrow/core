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