#pragma once

#include "graph_curve_traits_2.h"
#include <CGAL/Point_2.h>
#include <CGAL/Segment_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

namespace cartocrow {
template <class K>
struct Graph_segment_curve_traits_2 {
    using Kernel = K;
    using Curve_2 = CGAL::Segment_2<K>;
    using Point_2 = CGAL::Point_2<K>;

    static Curve_2 reversed(const Curve_2& curve) {
        return {curve.target(), curve.source()};
    }
    static Curve_2 transform(const Curve_2& curve, const CGAL::Aff_transformation_2<Kernel>& trans) {
        return curve.transform(trans);
    }
    static CGAL::Bbox_2 bbox(const Curve_2& curve) {
        return curve.bbox();
    }
};
static_assert(GraphCurveTraits_2<Graph_segment_curve_traits_2<CGAL::Epick>>);
static_assert(GraphCurveTraits_2<Graph_segment_curve_traits_2<CGAL::Epeck>>);
}
