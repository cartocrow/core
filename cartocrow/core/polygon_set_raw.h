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
		for (const auto& pgn : polygons_with_holes) {
			polygonSet.join(pgn);
		}
		return polygonSet;
	}
};

template <typename KernelOut, typename KernelIn>
PolygonSetRaw<KernelOut> convert_kernel(const PolygonSetRaw<KernelIn>& v) {
	PolygonSetRaw<KernelOut> result;
	for (const auto& p : v.polygons_with_holes) {
		result.polygons_with_holes.push_back(convert_kernel<KernelOut>(p));
	}
	return result;
}
}