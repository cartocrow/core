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