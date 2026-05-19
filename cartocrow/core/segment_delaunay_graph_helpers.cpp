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

#include "segment_delaunay_graph_helpers.h"

namespace cartocrow {
CGAL::Parabola_segment_2<CGAL::Segment_Delaunay_graph_traits_2<Inexact>>
approximate(const CGAL::Parabola_segment_2<CGAL::Segment_Delaunay_graph_traits_2<Exact>>& ps) {
	return CGAL::Parabola_segment_2<CGAL::Segment_Delaunay_graph_traits_2<Inexact>>(approximate(ps.center()), approximate(ps.line()), approximate(ps.p1), approximate(ps.p2));
}
}