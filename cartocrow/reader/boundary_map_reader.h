#pragma once

#include "cartocrow/core/boundary_map.h"

namespace cartocrow {
/// Creates a \ref BoundaryMap from a map in Ipe format.
///
/// The Ipe figure to be read needs to contain a single page. This page
/// has polygons and polylines describing the various boundaries.
///
/// Throws if the file could not be read, if the file is not a valid Ipe file,
/// or if the file does not contain boundaries like specified above.
BoundaryMap ipeToBoundaryMap(const std::filesystem::path &file);
}
