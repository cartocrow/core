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

#include <vector>

template <class ArrTraits>
class General_polyline_2 {
  public:
	typedef typename std::vector<typename ArrTraits::X_monotone_curve_2>::iterator Curve_iterator;
	typedef typename std::vector<typename ArrTraits::X_monotone_curve_2>::const_iterator Curve_const_iterator;

	template <class InputIterator>
	General_polyline_2(InputIterator begin, InputIterator end) {
		m_xm_curves = std::vector(begin, end);
	}

	Curve_iterator curves_begin() {
		return m_xm_curves.begin();
	}
	Curve_iterator curves_end() {
		return m_xm_curves.end();
	}
	Curve_const_iterator curves_begin() const {
		return m_xm_curves.cbegin();
	}
	Curve_const_iterator curves_end() const {
		return m_xm_curves.cend();
	}
	typename ArrTraits::X_monotone_curve_2::Point_2 source() const {
		return curves_begin()->source();
	}
	typename ArrTraits::X_monotone_curve_2::Point_2 target() const {
		return (--curves_end())->target();
	}
	[[nodiscard]] unsigned int size() const
	{
		return static_cast<unsigned int>(m_xm_curves.size());
	}
  private:
	std::vector<typename ArrTraits::X_monotone_curve_2> m_xm_curves;
};
