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

namespace cartocrow {
using GeometryAttribute = std::variant<int, std::vector<int>, double, std::vector<double>, 
										std::string, std::vector<std::string>, int64_t>;

using GeometryAttributes = std::unordered_map<std::string, GeometryAttribute>;

template <class Geometry> struct GeometricFeature {
	Geometry geometry;
	GeometryAttributes attributes;
};
}