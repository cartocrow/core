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

PointSet<Inexact> approximate(const PointSet<Exact>& ps);
} // namespace cartocrow
