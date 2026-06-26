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
	{ Traits::template convert(o, out) };
};

// todo: make a RenderPath reader traits that parses everything to a render path?

using IntermediateIpeGeometry = std::variant<
    PolygonSetRaw<Inexact>, PolygonWithHoles<Inexact>, Polygon<Inexact>, PolylineSet<Inexact>, Polyline<Inexact>, Point<Inexact>, PointSet<Inexact>, 
	CubicBezierCurve, CubicBezierSpline, Ellipse>;

template <class Geometry, class OutputIterator, class Traits>
concept IpeReaderIntermediateGeometryConverter = requires(const IntermediateIpeGeometry& g, OutputIterator out) {
	{ Traits::template convert(g, out) };
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
			}

			const ipe::Curve* curve = ssp->asCurve();
			for (int j = 0; j < curve->countSegments() && allStraightSegments; ++j) {
				ipe::CurveSegment segment = curve->segment(j);
				if (segment.type() != ipe::CurveSegment::ESegment) {
					allStraightSegments = false;
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
					*out++ = polyline;
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
					spline.appendCurve(Point<Inexact>(bezier.iV[0].x, bezier.iV[0].y),
						                Point<Inexact>(bezier.iV[1].x, bezier.iV[1].y),
						                Point<Inexact>(bezier.iV[2].x, bezier.iV[2].y),
						                Point<Inexact>(bezier.iV[3].x, bezier.iV[3].y));
				}
				*out++ = spline;
			} else if (ssp->type() == ipe::SubPath::EEllipse) {
				auto matrix = ssp->asEllipse()->matrix();
				
				Ellipse ellipse(matrix.a[0], matrix.a[1], matrix.a[2], matrix.a[3], matrix.a[4],
				                matrix.a[5]);
				*out++ = ellipse;
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
							spline.appendCurve(Point<Inexact>(bezier.iV[0].x, bezier.iV[0].y),
							                   Point<Inexact>(bezier.iV[1].x, bezier.iV[1].y),
							                   Point<Inexact>(bezier.iV[2].x, bezier.iV[2].y),
							                   Point<Inexact>(bezier.iV[3].x, bezier.iV[3].y));
						}
						*out++ = spline;
					}
				}
			}
		}
		return;
	}

	template <class OutputIterator>
	static void convert(ipe::Object& o, OutputIterator out) {
		std::vector<IntermediateIpeGeometry> intermediates;
		convertToIntermediate(o, std::back_inserter(intermediates));

		for (const auto& intermediate : intermediates) {
			Converter::convert(intermediate, out);
		}
	}
};

// is a model of IpeReaderIntermediateGeometryConverter
template <class Geometry>
struct BasicIpeReaderTraitsConverter {
	template <class OutputIterator>
	static void convert(const IntermediateIpeGeometry& g, OutputIterator out) {
		std::visit(
		    [&](auto&& g) {
			    using T = std::decay_t<decltype(g)>;

			    if constexpr (std::is_convertible_v<T, Geometry>) {
				    *out++ = Geometry{g};
			    }
		    },
		g);
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
	int m_pageNumber = 0;

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

	Color convertIpeColor(ipe::Color color) {
		return Color{static_cast<int>(color.iRed.toDouble() * 255),
		             static_cast<int>(color.iGreen.toDouble() * 255),
		             static_cast<int>(color.iBlue.toDouble() * 255)};
	}

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

	template <
		class Geometry,
		class OutputIterator,
		class Traits = BasicIpeReaderTraits<Geometry>
	>
	requires IpeReaderTraits<Geometry, OutputIterator, Traits>
	void read(std::filesystem::path path, OutputIterator out) {
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
			ipe::Object* object = page->object(i);
			Traits::convert(*object, out);
		}
	}

	template <class Geometry, class Traits = BasicIpeReaderTraits<Geometry>>
	requires IpeReaderTraits<Geometry, std::back_insert_iterator<std::vector<Geometry>>, Traits>
	std::optional<Geometry> readSingle(std::filesystem::path path) {
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

		std::vector<Geometry> gs;
		for (int i = 0; i < page->count(); ++i) {
			ipe::Object* object = page->object(i);
			Traits::convert(*object, std::back_inserter(gs));
			if (!gs.empty())
				return gs[0];
		}

		return std::nullopt;
	}
};
}