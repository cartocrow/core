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

#pragma once

#include "core.h"
#include "rectangle_helpers.h"

#include <CGAL/Parabola_segment_2.h>

namespace cartocrow {
template <class K>
PolygonWithHoles<K> transform(const CGAL::Aff_transformation_2<K> &t, const PolygonWithHoles<K> &pwh) {
	Polygon<K> outerT;
	if (!pwh.is_unbounded()) {
		outerT = transform(t, pwh.outer_boundary());
	}
	std::vector <Polygon<K>> holesT;
	for (const auto &h: pwh.holes()) {
		holesT.push_back(transform(t, h));
	}
	return {outerT, holesT.begin(), holesT.end()};
}

template <class K>
PolygonSet<K> transform(const CGAL::Aff_transformation_2<K> &t, const PolygonSet<K> &ps) {
	PolygonSet<K> transformed;
	std::vector<PolygonWithHoles<K>> pwhs;
	ps.polygons_with_holes(std::back_inserter(pwhs));
	for (const auto& pwh : pwhs) {
		transformed.insert(transform(t, pwh));
	}
	return transformed;
}

Box transform(const Box& box, const CGAL::Aff_transformation_2<Inexact>& t);

template <class Gt, class K>
CGAL::Parabola_segment_2<Gt> transform(const CGAL::Parabola_segment_2<Gt>& ps,
	const CGAL::Aff_transformation_2<K>& t) {
	return {ps.center().transform(t), ps.line().transform(t), ps.p1.transform(t), ps.p2.transform(t)};
}

CGAL::Aff_transformation_2<Exact> pretendExact(const CGAL::Aff_transformation_2<Inexact>& t);

template <class K>
CGAL::Aff_transformation_2<K> fitInto(const Rectangle<K>& toFit, const Rectangle<K>& into) {
	CGAL::Aff_transformation_2<K> move1(CGAL::TRANSLATION, CGAL::ORIGIN - centroid(toFit));
	CGAL::Aff_transformation_2<K> move2(CGAL::TRANSLATION, centroid(into) - CGAL::ORIGIN);
	CGAL::Aff_transformation_2<K> scale(CGAL::SCALING, std::min(width(into) / width(toFit), height(into) / height(toFit)));
	return move2 * scale * move1;
}

CGAL::Aff_transformation_2<Inexact> fitInto(const Box& toFit, const Box& into);
CGAL::Aff_transformation_2<Inexact> fitInto(const Rectangle<Inexact>& toFit, const Box& into);
CGAL::Aff_transformation_2<Inexact> fitInto(const Box& toFit, const Rectangle<Inexact>& into);
}
