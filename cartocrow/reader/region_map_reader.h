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

#include "ipe_reader.h"
#include "cartocrow/core/region_map.h"

namespace cartocrow {
namespace {
struct RegionLabelReaderTraits {
	template <class OutputIterator>
	static bool convert(const IpeObject& ipeObject, OutputIterator out) {
		auto [page, index] = ipeObject;
		auto& object = *page->object(index);
		ipe::Object::Type type = object.type();
		if (type != ipe::Object::Type::EText) {
			return false;
		}
		ipe::Matrix matrix = object.matrix();
		ipe::Vector translation = matrix * object.asText()->position();
		Point<Exact> position(translation.x, translation.y);
		ipe::String ipeString = object.asText()->text();
		std::string text(ipeString.data(), ipeString.size());
		*out++ = detail::RegionLabel{position, text, false};
		return true;
	}
};
}

/// Creates a \ref RegionMap from a region map in Ipe format.
///
/// The Ipe figure to be read needs to contain a single page. This page
/// has polygonal shapes (possibly containing holes or separate connected
/// components), each representing a region. Each region then needs to contain
/// exactly one label in its interior, indicating the name of the region.
///
/// Throws if the file could not be read, if the file is not a valid Ipe file,
/// or if the file does not contain regions like specified above.
RegionMap ipeToRegionMap(const std::filesystem::path& file, bool labelAtCentroid = false);

/// (temp) Should be removed in favor of an algorithm that computes salient
/// points from a \ref RegionMap.
std::vector<Point<Exact>> ipeToSalientPoints(const std::filesystem::path& file);
}
