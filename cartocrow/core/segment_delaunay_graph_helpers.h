#pragma once

#include "core.h"
#include "cubic_bezier.h"
#include "segment_delaunay_graph_helpers_cgal_adapted.h"

namespace cartocrow {
/// Exposes the p1 and p2 member variables.
/// Not needed anymore in CGAL 6.0 and up (https://github.com/CGAL/cgal/pull/7976)
template <class Gt>
class Open_Parabola_segment_2 : public CGAL::Parabola_segment_2<Gt> {
  public:
	CGAL::Parabola_segment_2<Gt>::Point_2 get_p1() {
		return this->p1;
	}
	CGAL::Parabola_segment_2<Gt>::Point_2 get_p2() {
		return this->p2;
	}
};

// These CGAL functions are available for triangulations, but not for delaunay graphs, so we create them ourselves.
template <class SDG>
int
mirror_index(const SDG& sdg, typename SDG::Face_handle f, int i)
{
	// return the index of opposite vertex in neighbor(i);
	CGAL_precondition (f->neighbor(i) != typename SDG::Face_handle() &&
	                  f->dimension() >= 1);
	if (f->dimension() == 1) {
		CGAL_assertion(i<=1);
		const int j = f->neighbor(i)->index(f->vertex((i==0) ? 1 : 0));
		CGAL_assertion(j<=1);
		return (j==0) ? 1 : 0;
	}
	return sdg.ccw( f->neighbor(i)->index(f->vertex(sdg.ccw(i))));
}

template <class SDG>
typename SDG::Vertex_handle
mirror_vertex(const SDG& sdg, typename SDG::Face_handle f, int i)
{
	CGAL_precondition ( f->neighbor(i) != typename SDG::Face_handle()
						&& f->dimension() >= 1);
	return f->neighbor(i)->vertex(mirror_index(sdg, f,i));
}

template <class SDG>
typename SDG::Edge
mirror_edge(const SDG& sdg, const typename SDG::Edge e)
{
	CGAL_precondition(e.first->neighbor(e.second) != typename SDG::Face_handle()
	                  && e.first->dimension() >= 1);
	return typename SDG::Edge(e.first->neighbor(e.second),
	            mirror_index(sdg, e.first, e.second));
}

template <class SDG>
std::variant<typename SDG::Geom_traits::Point_2, typename SDG::Geom_traits::Segment_2>
site_projection(const SDG& delaunay, const typename SDG::Edge& edge, const typename SDG::Site_2& site) {
	if (site.is_point()) {
		return { site.point() };
	} else {
		using Gt = SDG::Geom_traits;
		typename Gt::Segment_2 s;
		CGAL::Parabola_segment_2<Gt> ps;
		CGAL::Object o = delaunay.primal(edge);

		// Ray and line cases cannot occur because they require both sites to be a point
		if (CGAL::assign(s, o)) {
			auto start = site.segment().supporting_line().projection(s.source());
			auto end = site.segment().supporting_line().projection(s.end());
			return { Segment<Inexact>(start, end) };
		}
		else if (CGAL::assign(ps, o)) {
			// Roundabout way to obtain start and end of parabolic segment because they are protected -_-
			Open_Parabola_segment_2 ops{ps};
			auto p1 = ops.get_p1();
			auto p2 = ops.get_p2();

			if (std::isnan(p1.x()) || std::isnan(p2.x())) {
				return {typename Gt::Segment_2(typename Gt::Point_2(0.0, 0.0), typename Gt::Point_2(1.0, 0.0))};
			}

			auto start = site.segment().supporting_line().projection(p1);
			auto end = site.segment().supporting_line().projection(p2);

			return {Segment<Inexact>(start, end)};
		}
		else {
			throw std::runtime_error("Impossible: a segment Voronoi edge is neither a line segment nor a parabolic "
			                         "segment, but at least one of its sites is a line segment.");
		}
	}
}

template <class Gt>
CubicBezierCurve parabolaSegmentToBezier(const CGAL::Parabola_segment_2<Gt>& p) {
	// Directrix
	auto dir = p.line();
	// Focus
	auto focus = p.center();

	// Roundabout way to obtain start and end of parabolic segment because they are protected -_-
	Open_Parabola_segment_2<Gt> op{p};
	auto start = op.get_p1();
	auto end = op.get_p2();

	// Geometric magic: the intersection of the tangents at points p and q of the parabola is
	// the circumcenter of the focus and the projections of p and q on the directrix.
	auto start_p = dir.projection(start);
	auto end_p = dir.projection(end);

	// If the points are collinear CGAL::circumcenter throws an error; draw a segment instead.
	if (CGAL::collinear(focus, start_p, end_p)) return CubicBezierCurve(approximate(start), approximate(end));

	auto control = CGAL::circumcenter(focus, start_p, end_p);
	return CubicBezierCurve(approximate(start), approximate(control), approximate(end));
}

CGAL::Parabola_segment_2<CGAL::Segment_Delaunay_graph_traits_2<Inexact>>
approximate(const CGAL::Parabola_segment_2<CGAL::Segment_Delaunay_graph_traits_2<Exact>>& ps);

// The functions below are adapted from a CGAL example. https://github.com/CGAL/cgal/blob/33a2a257bde0e33af8ff083c28ea12050b28e1b5/Segment_Delaunay_graph_2/examples/Segment_Delaunay_graph_2/sdg-advanced-draw.cpp
template < typename ExactSite, typename SDGSite >
ExactSite convert_site_to_exact(const SDGSite &site)
{
	// Note: in theory, a site can be constructed from more than just one or two points
	// (e.g. 4 points for the segment defined by the intersection of two segments). Thus, it
	// would be better to convert the input points at the very beginning and just maintain
	// a type of map between the base and exact sites.
	ExactSite es;
	if(site.is_point())
		es = ExactSite::construct_site_2(pretendExact(site.point()));
	else
		es = ExactSite::construct_site_2(pretendExact(site.segment().source()), pretendExact(site.segment().target()));

	return es;
}

// Dual (Voronoi site) of an SDG face
template < typename FiniteFacesIterator >
Point<Exact> exact_primal(const FiniteFacesIterator sdg_f)
{
	using Exact_SDG_traits = CGAL::Segment_Delaunay_graph_traits_2<Exact>;
	using Exact_site_2 = typename Exact_SDG_traits::Site_2;

	static Exact_SDG_traits e_sdg_gt;
	const Exact_site_2 es0 = convert_site_to_exact<Exact_site_2>(sdg_f->vertex(0)->site());
	const Exact_site_2 es1 = convert_site_to_exact<Exact_site_2>(sdg_f->vertex(1)->site());
	const Exact_site_2 es2 = convert_site_to_exact<Exact_site_2>(sdg_f->vertex(2)->site());

	return e_sdg_gt.construct_svd_vertex_2_object()(es0, es1, es2);
}

// Dual (Voronoi edge) of an SDG edge
// this function is identical 'SDG::primal()', but with a conversion to exact sites
template < typename Edge, class SDG >
CGAL::Object exact_primal(const Edge& e,
                          const SDG& sdg)
{
	using Exact_SDG_traits = CGAL::Segment_Delaunay_graph_traits_2<Exact>;
	using Exact_site_2 = typename Exact_SDG_traits::Site_2;

	using DT = CGAL::Field_with_sqrt_tag;
	using Construct_sdg_bisector_2 = CGAL::SegmentDelaunayGraph_2::Construct_sdg_bisector_2<Exact_SDG_traits, DT>;
	using Construct_sdg_bisector_ray_2 = CGAL::SegmentDelaunayGraph_2::Construct_sdg_bisector_ray_2<Exact_SDG_traits, DT>;
	using Construct_sdg_bisector_segment_2 = CGAL::SegmentDelaunayGraph_2::Construct_sdg_bisector_segment_2<Exact_SDG_traits, DT>;

	CGAL_precondition(!sdg.is_infinite(e));

	if(sdg.dimension() == 1)
	{
		Exact_site_2 p = convert_site_to_exact<Exact_site_2>((e.first)->vertex(sdg.cw(e.second))->site());
		Exact_site_2 q = convert_site_to_exact<Exact_site_2>((e.first)->vertex(sdg.ccw(e.second))->site());

		return make_object(Construct_sdg_bisector_2()(p, q));
	}

	// dimension == 2
	// neither of the two adjacent faces is infinite
	if((!sdg.is_infinite(e.first)) && (!sdg.is_infinite(e.first->neighbor(e.second))))
	{
		Exact_site_2 p = convert_site_to_exact<Exact_site_2>((e.first)->vertex(sdg.ccw(e.second))->site());
		Exact_site_2 q = convert_site_to_exact<Exact_site_2>((e.first)->vertex(sdg.cw(e.second))->site());
		Exact_site_2 r = convert_site_to_exact<Exact_site_2>((e.first)->vertex(e.second)->site());
		Exact_site_2 s = convert_site_to_exact<Exact_site_2>(sdg.tds().mirror_vertex(e.first, e.second)->site());

		return Construct_sdg_bisector_segment_2()(p, q, r, s);
	}

	// both of the adjacent faces are infinite
	if(sdg.is_infinite(e.first) && sdg.is_infinite(e.first->neighbor(e.second)))
	{
		Exact_site_2 p = convert_site_to_exact<Exact_site_2>((e.first)->vertex(sdg.cw(e.second))->site());
		Exact_site_2 q = convert_site_to_exact<Exact_site_2>((e.first)->vertex(sdg.ccw(e.second))->site());

		return make_object(Construct_sdg_bisector_2()(p, q));
	}

	// only one of the adjacent faces is infinite
	CGAL_assertion(sdg.is_infinite(e.first) || sdg.is_infinite(e.first->neighbor(e.second)));
	CGAL_assertion(!(sdg.is_infinite(e.first) && sdg.is_infinite(e.first->neighbor(e.second))));
	CGAL_assertion(sdg.is_infinite(e.first->vertex(e.second)) || sdg.is_infinite(sdg.tds().mirror_vertex(e.first, e.second)));

	Edge ee = e;
	if(sdg.is_infinite(e.first->vertex(e.second)))
	{
		ee = Edge(e.first->neighbor(e.second),
		          e.first->neighbor(e.second)->index(sdg.tds().mirror_vertex(e.first, e.second)));
	}

	Exact_site_2 p = convert_site_to_exact<Exact_site_2>(ee.first->vertex(sdg.ccw(ee.second))->site());
	Exact_site_2 q = convert_site_to_exact<Exact_site_2>(ee.first->vertex(sdg.cw(ee.second))->site());
	Exact_site_2 r = convert_site_to_exact<Exact_site_2>(ee.first->vertex(ee.second)->site());

	return make_object(Construct_sdg_bisector_ray_2()(p, q, r));
}
}