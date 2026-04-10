#include "transform_helpers.h"

namespace cartocrow {
Box transform(const Box& box, const CGAL::Aff_transformation_2<Inexact>& t) {
	Point<Inexact> bl(box.xmin(), box.ymin());
	Point<Inexact> tr(box.xmax(), box.ymax());
	auto blT = bl.transform(t);
	auto trT = tr.transform(t);
	return Box(blT.x(), blT.y(), trT.x(), trT.y());
}

CGAL::Aff_transformation_2<Exact> pretendExact(const CGAL::Aff_transformation_2<Inexact>& t) {
	return CGAL::Aff_transformation_2<Exact>(t.m(0, 0), t.m(0, 1), t.m(0, 2), t.m(1, 0), t.m(1, 1),
	                                         t.m(1, 2));
}

CGAL::Aff_transformation_2<Inexact> fitInto(const Box& toFit, const Box& into) {
	return fitInto(Rectangle<Inexact>(toFit), Rectangle<Inexact>(into));
}
CGAL::Aff_transformation_2<Inexact> fitInto(const Rectangle<Inexact>& toFit, const Box& into) {
	return fitInto(toFit, Rectangle<Inexact>(into));
}
CGAL::Aff_transformation_2<Inexact> fitInto(const Box& toFit, const Rectangle<Inexact>& into) {
	return fitInto(Rectangle<Inexact>(toFit), into);
}
}