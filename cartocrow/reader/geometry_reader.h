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

#include "../core/geometric_feature.h"

#include <optional>
#include <string>
#include <filesystem>

namespace cartocrow {
template <class R> concept GeometryReader = requires(R reader, std::filesystem::path path) {
	/// If it exists, return the well-known text representation (WKT) of the coordinate reference system
	{reader.readSpatialReference(path)}->std::same_as<std::optional<std::string>>;

	/// Returns whether the reader can parse the given file.
	{reader.canRead(path)}->std::same_as<bool>;
};

//GeometryReader R
template <class  R, class Geometry, class OutputIterator>
concept GeometryReaderFor =
    GeometryReader<R> && requires(R reader, std::filesystem::path path, OutputIterator out) {

	/// Returns all geometries in the provided file that are convertible to Geometry.
	/// \pre canRead(path)
	reader.template read<Geometry>(path, out);

	/// Returns a vector with all geometries in the provided file that are convertible to Geometry.
	/// \pre canRead(path)
	{reader.template readV<Geometry>(path)}->std::same_as<std::vector<Geometry>>;

	/// Returns the first geometry in the provided file that is convertible to Geometry.
	/// \pre canRead(path)
	{reader.template readSingle<Geometry>(path)}->std::same_as<std::optional<Geometry>>;

	/// Returns geometries in the provided file that are convertible to Geometry including their attributes.
	/// Outputs Feature<Geometry>.
	/// \pre canRead(path)
	reader.template readWithAttributes<Geometry>(path, out);

	/// Returns a vector with all geometries in the provided file that are convertible to Geometry including their attributes.
	/// \pre canRead(path)
	{reader.template readWithAttributesV<Geometry>(path)}->std::same_as<std::vector<GeometricFeature<Geometry>>>;

	/// Returns the first feature in the provided file that is convertible to Geometry.
	/// \pre canRead(path)
	{reader.template readSingleWithAttributes<Geometry>(path)}->std::same_as<std::optional<GeometricFeature<Geometry>>>;
};
}