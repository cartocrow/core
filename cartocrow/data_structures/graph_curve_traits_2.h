#pragma once

#include <concepts>
#include <CGAL/Aff_transformation_2.h>
#include <CGAL/Bbox_2.h>

namespace cartocrow {
template <class CST>
concept GraphCurveTraits_2 =
	requires(CST cst, typename CST::Curve_2 curve, const CGAL::Aff_transformation_2<typename CST::Kernel>& trans) {
        typename CST::Curve_2;
	    typename CST::Point_2;
        typename CST::Kernel;

	    {
		    curve.source()
	    } -> std::convertible_to<typename CST::Point_2>;

		{
			curve.target()
		} -> std::convertible_to<typename CST::Point_2>;

	    {
		    CST::reversed(curve)
	    } -> std::convertible_to<typename CST::Curve_2>;

        {
            CST::transform(curve, trans)
        } -> std::convertible_to<typename CST::Curve_2>;

        {
            CST::bbox(curve)
        } -> std::convertible_to<CGAL::Bbox_2>;
    };
}