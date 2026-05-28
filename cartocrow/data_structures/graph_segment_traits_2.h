#pragma once

#include "graph_curve_traits_2.h"
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Point_2.h>
#include <CGAL/Segment_2.h>

namespace cartocrow {
template <class K> struct Graph_segment_curve_traits_2 {
	using Kernel = K;
	using CurveRepresentation_2 = std::monostate;
	using Curve_2 = CGAL::Segment_2<K>;
	using Point_2 = CGAL::Point_2<K>;

	static CurveRepresentation_2 representation(const Curve_2& curve) {
		return std::monostate();
	}
	static Curve_2 curve(const Point_2& start, const Point_2& end, const CurveRepresentation_2& rep) {
		return Curve_2(start, end);
	}
	static Curve_2 curve_reversed(const Point_2& start, const Point_2& end,
	                              const CurveRepresentation_2& rep) {
		return Curve_2(start, end);
	}
	static CGAL::Bbox_2 bbox(const Point_2& start, const Point_2& end,
	                         const CurveRepresentation_2& rep) {
		return curve(start, end, rep).bbox();
	}

	static void reverse_representation(const Point_2& start, const Point_2& end,
	                                   CurveRepresentation_2& rep) {
		// skip
	}
	static void move_start(const Point_2& start, const Point_2& end,
	                                   CurveRepresentation_2& rep) {
		// skip
	}
	static void move_end(const Point_2& start, const Point_2& end, CurveRepresentation_2& rep) {
		// skip
	}
	static void transform(const CGAL::Aff_transformation_2<Kernel>& trans, const Point_2& start,
	                      const Point_2& end, CurveRepresentation_2& rep) {
		// skip
	}
};
static_assert(GraphCurveTraits_2<Graph_segment_curve_traits_2<CGAL::Epick>>);
static_assert(GraphCurveTraits_2<Graph_segment_curve_traits_2<CGAL::Epeck>>);
} // namespace cartocrow
