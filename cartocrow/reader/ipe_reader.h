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

#include "geometry_reader.h"
#include "../core/core.h"
#include "../core/ellipse.h"
#include "../core/polyline.h"
#include "../core/polyline_set.h"
#include "../core/point_set.h"
#include "../core/polygon_set_raw.h"
#include "../core/cubic_bezier.h"
#include "../core/geometric_feature.h"

#include <regex>

#include <ipeattributes.h>
#include <ipedoc.h>
#include <ipereference.h>
#include <ipeshape.h>
#include <ipepath.h>
#include <ipegroup.h>
#include <ipebase.h>
#include <ipegeo.h>

namespace cartocrow {
template <class Geometry, class OutputIterator, class Traits>
concept IpeReaderTraits = requires(ipe::Object& o, OutputIterator out) {
	{ Traits::template convert(o, out) }->std::same_as<bool>;
};

// todo: make a RenderPath reader traits that parses everything to a render path?

using IntermediateIpeGeometry = std::variant<
    PolygonSetRaw<Inexact>, PolygonWithHoles<Inexact>, Polygon<Inexact>, PolylineSet<Inexact>, Polyline<Inexact>, Segment<Inexact>, 
	Point<Inexact>, PointSet<Inexact>, CubicBezierCurve, CubicBezierSpline, Circle<Inexact>, Ellipse>;

template <class Geometry, class OutputIterator, class Traits>
concept IpeReaderIntermediateGeometryConverter = requires(const IntermediateIpeGeometry& g, OutputIterator out) {
	{ Traits::template convert(g, out) }->std::same_as<bool>;
};

/// is a model of IpeReaderTraits 
template<class Geometry, class Converter>
requires IpeReaderIntermediateGeometryConverter<Geometry, std::back_insert_iterator<std::vector<Geometry>>, Converter>
struct IpeReaderIntermediateGeometryTraits {
	template <class OutputIterator>
	static void convertToIntermediate(ipe::Object& o, OutputIterator out) {
		// We (currently) don't have a proper intermediate type for a group, so instead just ungroup everything.
		if (o.type() == ipe::Object::EGroup) {
			auto* group = o.asGroup();
			for (auto* obj : *group) {
				convertToIntermediate(*obj, out);
			}
			return;
		}

		if (o.type() == ipe::Object::EReference) {
			// Convert to point if it is a mark
			auto* ref = o.asReference();
			auto type = ref->type();
			if (ref->flags() & ipe::Reference::EIsMark) {
				auto matrix = o.matrix();
				auto realPos = matrix * ref->position();
				*out++ = Point<Inexact>(realPos.x, realPos.y);
			}
			return;
		}

		// Only the path object type remains to be parsed.
		if (o.type() != ipe::Object::Type::EPath) {
			return;
		}

		const ipe::Path* path = o.asPath();
		ipe::Matrix matrix = path->matrix();
		ipe::Shape shape = path->shape();

		// If all subpaths are closed curves with straight segments then make PolygonSetRaw (or Polygon).
		// If all subpaths are open curves with straight segments then make PolylineSet (or Polyline)
		// Otherwise, parse and output the subpaths separately
		bool allStraightSegments = true;
		bool allClosed = true;
		bool allOpen = true;
		for (int i = 0; i < shape.countSubPaths(); ++i) {
			auto* ssp = shape.subPath(i);
			if (!ssp->closed()) {
				allClosed = false;
			} else {
				allOpen = false;
			}
			if (ssp->type() != ipe::SubPath::ECurve) {
				allStraightSegments = false;
			} else {
				const ipe::Curve* curve = ssp->asCurve();
				for (int j = 0; j < curve->countSegments() && allStraightSegments; ++j) {
					ipe::CurveSegment segment = curve->segment(j);
					if (segment.type() != ipe::CurveSegment::ESegment) {
						allStraightSegments = false;
					}
				}
			}
		}

		if (allStraightSegments && allClosed) {
			PolygonSetRaw<Inexact> ps;
			for (int i = 0; i < shape.countSubPaths(); ++i) {
				Polygon<Inexact> polygon;
				const ipe::Curve* curve = shape.subPath(i)->asCurve();
				for (int j = 0; j < curve->countSegments(); ++j) {
					ipe::CurveSegment segment = curve->segment(j);
					if (j == 0) {
						ipe::Vector v = matrix * segment.cp(0);
						polygon.push_back(Point<Inexact>(v.x, v.y));
					}
					ipe::Vector v = matrix * segment.last();
					Point<Inexact> p(v.x, v.y);
					if (p != polygon.container().back()) {
						polygon.push_back(Point<Inexact>(v.x, v.y));
					}
				}
				// if the begin and end vertices are equal, remove one of them
				if (polygon.container().front() == polygon.container().back()) {
					polygon.container().pop_back();
				}
				if (shape.countSubPaths() == 1) {
					*out++ = polygon;
				} else {
					ps.polygons_with_holes.emplace_back(polygon);
				}
			}
			if (shape.countSubPaths() > 1) {
				*out++ = ps;
			}

			return;
		} else if (allStraightSegments && allOpen) {
			PolylineSet<Inexact> ps;
			for (int i = 0; i < shape.countSubPaths(); ++i) {
				Polyline<Inexact> polyline;
				const ipe::Curve* curve = shape.subPath(i)->asCurve();
				for (int j = 0; j < curve->countSegments(); ++j) {
					ipe::CurveSegment segment = curve->segment(j);
					if (j == 0) {
						ipe::Vector v = matrix * segment.cp(0);
						polyline.push_back(Point<Inexact>(v.x, v.y));
					}
					ipe::Vector v = matrix * segment.last();
					Point<Inexact> p(v.x, v.y);
					polyline.push_back(Point<Inexact>(v.x, v.y));
				}
				if (shape.countSubPaths() == 1) {
					if (polyline.num_edges() == 1) {
						*out++ = polyline.edge(0);
					} else {
						*out++ = polyline;
					}
				} else {
					ps.polylines.push_back(polyline);
				}
			}
			if (shape.countSubPaths() > 1) {
				*out++ = ps;
			}

			return;
		}

		// not all straight or not all closed or not all open, so parse separately
		for (int i = 0; i < shape.countSubPaths(); ++i) {
			auto* ssp = shape.subPath(i);
			if (ssp->type() == ipe::SubPath::EClosedSpline) {
				CubicBezierSpline spline;
				std::vector<ipe::Bezier> beziers;
				ssp->asClosedSpline()->beziers(beziers);
				for (auto bezier : beziers) {
					auto c0T = matrix * bezier.iV[0];
					auto c1T = matrix * bezier.iV[1];
					auto c2T = matrix * bezier.iV[2];
					auto c3T = matrix * bezier.iV[3];
					spline.appendCurve(Point<Inexact>(c0T.x, c0T.y),
						               Point<Inexact>(c1T.x, c1T.y),
						               Point<Inexact>(c2T.x, c2T.y),
						               Point<Inexact>(c3T.x, c3T.y));
				}
				*out++ = spline;
			} else if (ssp->type() == ipe::SubPath::EEllipse) {
				auto ellipseMatrix = ssp->asEllipse()->matrix();
				auto finalMatrix = matrix * ellipseMatrix;
				// Transposed linear map
				auto a = finalMatrix.a[0];
				auto b = finalMatrix.a[2];
				auto c = finalMatrix.a[1];
				auto d = finalMatrix.a[3];
				// Translation
				auto e = finalMatrix.a[4];
				auto f = finalMatrix.a[5];
				double det = a * d - b * c;
				double ia = d / det;
				double ib = -b / det;
				double ic = -c / det;
				double id = a / det;
				double q11 = ia * ia + ic * ic;
				double q12 = ia * ib + ic * id;
				double q22 = ib * ib + id * id;
				double tx = e;
				double ty = f;
				double A = q11;
				double B = 2 * q12;
				double C = q22;
				double D = -2 * (q11 * tx + q12 * ty);
				double E = -2 * (q12 * tx + q22 * ty);
				double F = q11 * tx * tx + 2 * q12 * tx * ty + q22 * ty * ty - 1;

				if (std::abs(B) < M_EPSILON && std::abs(A - C) < M_EPSILON) {
					Point<Inexact> center(-D / (2 * A), -E / (2 * A));
					Circle<Inexact> circle(center, center.x() * center.x() + center.y() * center.y() - F / A);
					*out++ = circle;
				} else {
					Ellipse ellipse(A, B, C, D, E, F);
					*out++ = ellipse;
				}
			} else if (ssp->type() == ipe::SubPath::ECurve) {
				auto* curve = ssp->asCurve();

				bool allSegments = true;
				for (int j = 0; j < curve->countSegments(); ++j) {
					ipe::CurveSegment segment = curve->segment(j);
					if (segment.type() != ipe::CurveSegment::ESegment) {
						allSegments = false;
						break;
					}
				}
				if (allSegments) {
					if (ssp->closed()) {
						// make polygon
						Polygon<Inexact> polygon;
						for (int j = 0; j < curve->countSegments(); ++j) {
							ipe::CurveSegment segment = curve->segment(j);
							if (j == 0) {
								ipe::Vector v = matrix * segment.cp(0);
								polygon.push_back(Point<Inexact>(v.x, v.y));
							}
							ipe::Vector v = matrix * segment.last();
							Point<Inexact> p(v.x, v.y);
							if (p != polygon.container().back()) {
								polygon.push_back(Point<Inexact>(v.x, v.y));
							}
						}
						// if the begin and end vertices are equal, remove one of them
						if (polygon.container().front() == polygon.container().back()) {
							polygon.container().pop_back();
						}
						*out++ = polygon;
					} else {
						// make polyline
						Polyline<Inexact> polyline;
						for (int j = 0; j < curve->countSegments(); ++j) {
							ipe::CurveSegment segment = curve->segment(j);
							if (j == 0) {
								ipe::Vector v = matrix * segment.cp(0);
								polyline.push_back(Point<Inexact>(v.x, v.y));
							}
							ipe::Vector v = matrix * segment.last();
							Point<Inexact> p(v.x, v.y);
							polyline.push_back(Point<Inexact>(v.x, v.y));
						}
						*out++ = polyline;
					}
				} else {
					// make spline
					for (int j = 0; j < curve->countSegments(); ++j) {
						ipe::CurveSegment segment = curve->segment(j);
						std::vector<ipe::Bezier> beziers;
						segment.beziers(beziers);

						// todo test if .beziers also converts circular arcs
						CubicBezierSpline spline;
						for (auto bezier : beziers) {
							auto c0T = matrix * bezier.iV[0];
							auto c1T = matrix * bezier.iV[1];
							auto c2T = matrix * bezier.iV[2];
							auto c3T = matrix * bezier.iV[3];
							spline.appendCurve(Point<Inexact>(c0T.x, c0T.y),
											   Point<Inexact>(c1T.x, c1T.y),
											   Point<Inexact>(c2T.x, c2T.y),
											   Point<Inexact>(c3T.x, c3T.y));
						}

						if (spline.numCurves() == 1) {
							*out++ = spline.curve(0);
						} else {
							*out++ = spline;
						}
					}
				}
			}
		}
		return;
	}

	template <class OutputIterator>
	static bool convert(ipe::Object& o, OutputIterator out) {
		std::vector<IntermediateIpeGeometry> intermediates;
		convertToIntermediate(o, std::back_inserter(intermediates));

		for (const auto& intermediate : intermediates) {
			Converter::convert(intermediate, out);
		}

		return !intermediates.empty();
	}
};

// is a model of IpeReaderIntermediateGeometryConverter
template <class Geometry>
struct BasicIpeReaderTraitsConverter {
	template <class OutputIterator>
	static bool convert(const IntermediateIpeGeometry& g, OutputIterator out) {
		bool convertedSomething = false;
		std::visit(
		    [&](auto&& g) {
			    using T = std::decay_t<decltype(g)>;

			    if constexpr (std::is_convertible_v<T, Geometry>) {
				    *out++ = Geometry{g};
					convertedSomething = true;
			    }
		    },
		g);
		return convertedSomething;
	}
};

template <class Geometry>
using BasicIpeReaderTraits = IpeReaderIntermediateGeometryTraits<Geometry, BasicIpeReaderTraitsConverter<Geometry>>;

namespace {
	using Out = std::back_insert_iterator<std::vector<Point<Inexact>>>;

	static_assert(IpeReaderTraits<Point<Inexact>, Out, BasicIpeReaderTraits<Point<Inexact>>>);
}

// models GeometryReader and GeometryReaderFor every Geometry
class IpeReader {
  private:
	/// The page to read from
	int m_pageNumber = 0;
	/// The layer to read from. If std::nullopt then it reads from all layers.
	std::optional<std::variant<int, std::string>> m_layer = std::nullopt;

  public:
	// ===== Static Ipe helpers =====
	static std::shared_ptr<ipe::Document> loadIpeFile(const std::filesystem::path& filename) {
		std::fstream fin(filename);
		std::string input;
		if (fin) {
			using Iterator = std::istreambuf_iterator<char>;
			input.assign(Iterator(fin), Iterator());
		} else {
			 std::stringstream ss;
			 ss << "Cannot open file " << filename;
			 throw std::runtime_error(ss.str());
		}

		ipe::Platform::initLib(ipe::IPELIB_VERSION);
		int load_reason = 0;
		ipe::Buffer buffer(input.c_str(), input.size());
		ipe::BufferSource bufferSource(buffer);
		ipe::FileFormat format = ipe::Document::fileFormat(bufferSource);
		ipe::Document* document = ipe::Document::load(bufferSource, format, load_reason);

		if (load_reason > 0) {
			throw std::runtime_error("Unable to load Ipe file: parse error at position " +
				std::to_string(load_reason));
		} else if (load_reason == ipe::Document::EVersionTooOld) {
			throw std::runtime_error("Unable to load Ipe file: the version of the file is too old");
		} else if (load_reason == ipe::Document::EVersionTooRecent) {
			throw std::runtime_error(
				"Unable to load Ipe file: the file version is newer than Ipelib");
		} else if (load_reason == ipe::Document::EFileOpenError) {
			throw std::runtime_error("Unable to load Ipe file: error opening the file");
		} else if (load_reason == ipe::Document::ENotAnIpeFile) {
			throw std::runtime_error(
				"Unable to load Ipe file: the file does not exist or was not created by Ipe");
		}

		return std::shared_ptr<ipe::Document>(document);
	}

	static Color convertIpeColor(ipe::Color color) {
		return Color{ static_cast<int>(color.iRed.toDouble() * 255),
					 static_cast<int>(color.iGreen.toDouble() * 255),
					 static_cast<int>(color.iBlue.toDouble() * 255) };
	}

	static Color convertStringToColor(std::string s, std::filesystem::path path) {
		std::istringstream iss(s);
		double r, g, b;
		if (!(iss >> r >> g >> b)) {
			auto doc = loadIpeFile(path);
			ipe::Attribute attr(true, ipe::String(s.data()));
			return convertIpeColor(doc->cascade()->find(ipe::Kind::EColor, attr).color());
		}

		return {static_cast<int>(std::lround(r * 255)), static_cast<int>(std::lround(g * 255)),
		        static_cast<int>(std::lround(b * 255))};
	}

	// ===== Ipe reader specific functions =====
	/// Set the page to read from.
	/// Note! Page indices start at zero (so pass one integer smaller than the one the ipe GUI shows).
	void setPage(int pageNumber) {
		m_pageNumber = pageNumber;
	}

	/// Removes the layer filter so that objects are read from all layers.
	void removeLayerFilter() {
		m_layer = std::nullopt;
	}

	/// Read objects only from the specified layer
	void setLayerFilter(int layerNumber) {
		m_layer = layerNumber;
	}

	/// Read objects only from the specified layer
	void setLayerFilter(std::string layerName) {
		m_layer = layerName;
	}

	/// Return the number of pages in the ipe document
	int numberOfPages(std::filesystem::path path) {
		auto doc = loadIpeFile(path);
		return doc->countPages();
	}

	/// Number of layers
	int numberOfLayer(std::filesystem::path path, int pageIndex) {
		auto doc = loadIpeFile(path);
		auto page = doc->page(pageIndex);
		return page->countLayers();
	}

  private:
	/// Whether to skip the ipe object with index i in the given page.
	bool skipObject(ipe::Page* page, int i) const {
		if (m_layer.has_value()) {
			auto layerIndex = page->layerOf(i);
			if (auto* layerIndexP = std::get_if<int>(&*m_layer)) {
				if (layerIndex != *layerIndexP)
					return true; // object is not on layer so we skip
			} else if (auto* layerNameP = std::get_if<std::string>(&*m_layer)) {
				if (*page->layer(layerIndex).data() != *layerNameP->c_str()) {
					return true;
				}
			}
		}
		return false;
	}

	GeometryAttributes getAttributes(ipe::Page* page, int i) const {
		// There is no nice way to get all attributes that are relevant for an object, the logic is all in the saveAsXml function.
		// So for now we convert to xml and parse that. This is not so efficient because the entire geometry is also exported to xml.

		ipe::String ipeString;
		ipe::StringStream ipeSS(ipeString);
		page->object(i)->saveAsXml(ipeSS, page->layer(page->layerOf(i)));
		std::string input = ipeString.data();

		GeometryAttributes result;

		// Regex for key="value"
		std::regex attr_regex("(\\w+)\\s*=\\s*\"([^\"]*)\"");

		for (auto it = std::sregex_iterator(input.begin(), input.end(), attr_regex);
			        it != std::sregex_iterator(); ++it) {
			std::string key = (*it)[1].str();
			std::string value = (*it)[2].str();

			// Ignore matrix attributes for paths and position attributes for references
			if (key == "matrix" || key == "pos")
				continue;

			result[key] = value;
		}

		return result;
	}

	/// If handle returns true the parsing stops.
	void readHelper(std::filesystem::path path, std::function<bool(ipe::Page*, int)> handle) {
		std::shared_ptr<ipe::Document> document = IpeReader::loadIpeFile(path);

		if (m_pageNumber >= document->countPages()) {
			std::cerr << "Current page number exceeds document page count." << std::endl;
			std::cerr << "Setting page number to last page." << std::endl;
			m_pageNumber = document->countPages() - 1;
		} else if (m_pageNumber < 0) {
			std::cerr << "Current page number is negative." << std::endl;
			std::cerr << "Setting page number to first page." << std::endl;
			m_pageNumber = 0;
		}

		ipe::Page* page = document->page(m_pageNumber);

		for (int i = 0; i < page->count(); ++i) {
			if (skipObject(page, i))
				continue;
			if (handle(page, i))
				break;
		}
	}

  public:

	// ===== Reader methods =====

	/// If it exists, return the well-known text representation (WKT) of the coordinate reference system
	std::optional<std::string> readSpatialReference(std::filesystem::path path) {
		return std::nullopt;
	}

	/// Returns whether the reader can parse the given file.
	/// Currently only checks the file extension.
	// todo: actually try to parse to ipe document?
	bool canRead(std::filesystem::path path) {
		return path.extension() == ".ipe";
	}

	/// Returns geometries in the provided file that are convertible to Geometry.
	/// \pre canRead(path)
	template <
		class Cardinality,
		class Geometry,
		class AttrMode,
		class Traits = BasicIpeReaderTraits<Geometry>
	>
		requires IpeReaderTraits<Geometry, std::back_insert_iterator<std::vector<Geometry>>, Traits>
	ReadResultT<Geometry, AttrMode, Cardinality> read(std::filesystem::path path) {
		std::vector<ElementTypeT<Geometry, AttrMode>> gs;

		readHelper(path, [&](ipe::Page* page, int i) {
			ipe::Object* object = page->object(i);
			if constexpr (std::same_as<AttrMode, WithoutAttributes>) {
				Traits::convert(*object, std::back_inserter(gs));
				if constexpr (std::same_as<Cardinality, Single>) {
					return !gs.empty(); // stop if a geometry is found
				} else {
					return false;
				}
			} else {
				auto attributes = getAttributes(page, i);

				std::vector<Geometry> temps;
				Traits::convert(*object, std::back_inserter(temps));
				for (auto& t : temps) {
					gs.emplace_back(std::move(t), attributes);
				}
				if constexpr (std::same_as<Cardinality, Single>) {
					return !gs.empty(); // stop if a geometry is found
				} else {
					return false;
				}
			}
		});

		if constexpr (std::same_as<Cardinality, Single>) {
			return gs.empty() ? std::nullopt : std::optional<ElementTypeT<Geometry, AttrMode>>(gs.front());
		} else {
			return gs;
		}
	}

	/// Returns geometries in the provided file that are convertible to Geometry.
	/// \pre canRead(path)
	template <
		class Cardinality,
		class Geometry,
		class AttrMode,
		class OutputIterator,
		class Traits = BasicIpeReaderTraits<Geometry>
	>
		requires IpeReaderTraits<Geometry, std::back_insert_iterator<std::vector<Geometry>>, Traits>
	void read(std::filesystem::path path, OutputIterator out) {
		readHelper(path, [&](ipe::Page* page, int i) {
			ipe::Object* object = page->object(i);
			if constexpr (std::same_as<AttrMode, WithoutAttributes>) {
				auto convertedSomething = Traits::convert(*object, out);
				if constexpr (std::same_as<Cardinality, Single>) {
					return convertedSomething; // stop if a geometry is found
				} else {
					return false;
				}
			} else {
				auto attributes = getAttributes(page, i);

				std::vector<Geometry> temps;
				Traits::convert(*object, std::back_inserter(temps));
				for (auto& t : temps) {
					*out++ = GeometricFeature<Geometry>(std::move(t), attributes);
				}
				if constexpr (std::same_as<Cardinality, Single>) {
					return !temps.empty(); // stop if a geometry is found
				} else {
					return false;
				}
			}
		});
	}
};

namespace {
static_assert(GeometryReader<IpeReader>);
static_assert(GeometryReaderFor<IpeReader, Point<Inexact>>);
}
}