#pragma once

#include "core.h"

namespace cartocrow {
template <class K> 
struct PointSet {
	std::vector<Point<K>> points;

	PointSet<K> transform(const CGAL::Aff_transformation_2<K>& trans) const {
		PointSet<K> transformed;
		for (const auto& p : points) {
			transformed.points.push_back(p.transform(trans));
		}
		return transformed;
	}

	Box bbox() const {
		return CGAL::bbox_2(points.begin(), points.end());
	}
};


template <typename KernelOut, typename KernelIn>
PointSet<KernelOut> convert_kernel(const PointSet<KernelIn>& v) {
	std::vector<Point<KernelOut>> result;

	for (const auto& p : v.points) {
		result.push_back(convert_kernel<KernelOut>(p));
	}

	return {result};
}
} // namespace cartocrow
