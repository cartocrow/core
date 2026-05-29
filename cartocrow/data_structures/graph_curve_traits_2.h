#pragma once

#include <CGAL/Aff_transformation_2.h>
#include <CGAL/Bbox_2.h>
#include <concepts>

namespace cartocrow {
template <class CST>
concept GraphCurveTraits_2 =
    requires(const typename CST::Point_2& const_start, const typename CST::Point_2& const_new_start,
             const typename CST::Point_2& const_end, const typename CST::Point_2& const_new_end,
             typename CST::Curve_representation_2& rep,
             const typename CST::Curve_representation_2& const_rep,
             const typename CST::Curve_2& const_curve,
             const CGAL::Aff_transformation_2<typename CST::Kernel>& const_trans) {

	// the type stored with an edge
	typename CST::Curve_representation_2;
	// the actual geometry (may or may not be the same as the representation)
	typename CST::Curve_2;
	typename CST::Point_2;
	typename CST::Kernel;

	// converts the curve into the representation
	{CST::representation(const_curve)}->std::same_as<typename CST::Curve_representation_2>;
	// converts the representation into the curve
	{CST::curve(const_start, const_end, const_rep)}->std::same_as<typename CST::Curve_2>;
	// converts the representation into the curve, but for the reversed direction
	{CST::curve_reversed(const_start, const_end, const_rep)}->std::same_as<typename CST::Curve_2>;
	// determine the bounding box of the curve, from its representation
	{CST::bbox(const_start, const_end, const_rep)}->std::same_as<CGAL::Bbox_2>;

	// reverse the representation of an edge (NB: given position are the new positions)
	{CST::reverse_representation(const_new_start, const_new_end, rep)};
	// update the representation, given that its startpoint moved (NB: given position are the new positions)
	{CST::move_start(const_new_start, const_end, rep)};
	// update the representation, given that its endpoint moved (NB: given position are the new positions)
	{CST::move_end(const_start, const_new_end, rep)};
	// apply the transformation to the curve representation (NB: given position are the new positions)
	{CST::transform(const_trans, const_new_start, const_new_end, rep)};
};
} // namespace cartocrow