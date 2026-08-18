#pragma once

#include <ogrsf_frmts.h>
#include "geometry_reader.h"
#include "linear_object_reader.h"
#include "gdal_conversion.h"
#include "../core/straight_geometry.h"

namespace cartocrow {
namespace {
using GDALObject = OGRFeature;
}

template <class Geometry, class OutputIterator, class Traits>
concept GDALReaderTraits =
    LinearObjectReaderTraits<
        GDALObject,
        Geometry,
        OutputIterator,
        Traits>;

using IntermediateGDALGeometry = StraightGeometry<Inexact>;

template <class Geometry, class OutputIterator, class Traits>
concept GDALReaderIntermediateGeometryConverter = requires(const IntermediateGDALGeometry& g, OutputIterator out) {
	{ Traits::template convert(g, out) }->std::same_as<bool>;
};

/// is a model of GDALReaderTraits 
template<class Geometry, class Converter>
requires GDALReaderIntermediateGeometryConverter<Geometry, std::back_insert_iterator<std::vector<Geometry>>, Converter>
struct GDALReaderIntermediateGeometryTraits {
	template <class OutputIterator>
	static void convertToIntermediate(const GDALObject& o, OutputIterator out) {
		const OGRGeometry *poGeometry;
        poGeometry = o.GetGeometryRef();
		*out++ = ogrGeometryToStraightGeometry(*poGeometry);
		return;
	}

	template <class OutputIterator>
	static bool convert(const GDALObject& o, OutputIterator out) {
		std::vector<IntermediateGDALGeometry> intermediates;
		convertToIntermediate(o, std::back_inserter(intermediates));

		for (const auto& intermediate : intermediates) {
			Converter::convert(intermediate, out);
		}

		return !intermediates.empty();
	}
};

// is a model of GDALReaderIntermediateGeometryConverter
template <class Geometry>
struct BasicGDALReaderTraitsConverter {
	template <class OutputIterator>
	static bool convert(const IntermediateGDALGeometry& g, OutputIterator out) {
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
using BasicGDALReaderTraits = GDALReaderIntermediateGeometryTraits<Geometry, BasicGDALReaderTraitsConverter<Geometry>>;

namespace {
	using Out = std::back_insert_iterator<std::vector<Point<Inexact>>>;

	static_assert(GDALReaderTraits<Point<Inexact>, Out, BasicGDALReaderTraits<Point<Inexact>>>);
}

class GDALReader : public LinearObjectReader<GDALObject, BasicGDALReaderTraits> {
  private:
	// Layer index or name.
	std::variant<int, std::string> m_layer = 0;
	GDALDataset* m_dataset;

  public:
	GDALReader(const std::filesystem::path& path) {
		GDALAllRegister();

		m_dataset = (GDALDataset*) GDALOpenEx( path.string().c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr );
		if( m_dataset == nullptr ) {
			printf( "GDAL open failed.\n" );
			exit( 1 );
		}
	}

	/// Set layer to read from
	void setLayer(int layerNumber) {
		if (m_dataset->GetLayer(layerNumber) != nullptr) {
			m_layer = layerNumber;
		} else {
			std::cerr << "No layer with index: " << layerNumber << std::endl;
		}
	}

	/// Set layer to read from.
	void setLayer(std::string layerName) {
		if (m_dataset->GetLayerByName(layerName.c_str()) != nullptr) {
			m_layer = layerName;
		} else {
			std::cerr << "No layer named: " << layerName << std::endl;
		}
	}

	/// Number of layers.
	int numberOfLayers() const {
		return m_dataset->GetLayerCount();
	}

	/// Returns the name of a layer.
	std::string layerName(int layerIndex) const {
		return m_dataset->GetLayer(layerIndex)->GetName();
	}

	/// Outputs the names of the layers of a page.
	template <class OutputIterator>
	void layerNames(OutputIterator out) const {
		for (int i = 0; i < numberOfLayers(); ++i) {
			*out++ = layerName(i);
		}
	}

	/// Returns the names of the layers of a page.
	std::vector<std::string> layerNames() const {
		std::vector<std::string> names;
		layerNames(std::back_inserter(names));
		return names;
	}
  private:
	OGRLayer* getLayer() {
		OGRLayer* poLayer;

		if (auto* layerIndexP = std::get_if<int>(&m_layer)) {
			poLayer = m_dataset->GetLayer(*layerIndexP);
		} else if (auto* layerNameP = std::get_if<std::string>(&m_layer)) {
			poLayer = m_dataset->GetLayerByName(layerNameP->c_str());
		}

		return poLayer;
	}

  private:
	/// If handle returns true the parsing stops.
	void readHelper(std::function<bool(const GDALObject&)> handle) override {
		auto* poLayer = getLayer();

		for (const auto& poFeature : *poLayer) {
			if (handle(*poFeature)) {
				break;
			}
		}
	}

	GeometryAttributes getAttributes(const GDALObject& obj) const override {
		GeometryAttributes attributes;

		int i = 0;
        for( auto&& oField : obj ) {
            std::string name = obj.GetDefnRef()->GetFieldDefn(i)->GetNameRef();
            if (oField.IsNull()) { continue; }
            switch (oField.GetType()) {
                case OFTInteger:
                    attributes[name] = static_cast<int>(oField.GetInteger());
                    break;
                case OFTReal:
                    attributes[name] = oField.GetDouble();
                    break;
                case OFTInteger64:
                    attributes[name] = static_cast<int64_t>(oField.GetInteger64());
                    break;
                case OFTString: {
                    attributes[name] = static_cast<std::string>(oField.GetString());
                    break;
                }
                default:
                    std::cout << "Did not handle this type of attribute: " << oField.GetType() << std::endl;
                    break;
                }
            ++i;
        }

		return attributes;
	}

	bool skipObject(const GDALObject& obj) const override {
		return false;
	}
  public:
	// ===== Reader methods =====
	/// Load a different file. 
	/// Resets the page number and layer focus.
	void load(const std::filesystem::path& path) {
		m_dataset = (GDALDataset*) GDALOpenEx( path.string().c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr );
		if( m_dataset == nullptr ) {
			printf( "GDAL open failed.\n" );
			exit( 1 );
		}
	}

	/// If it exists, return the well-known text representation (WKT) of the coordinate reference system
	std::optional<std::string> readSpatialReference() {
		auto* poLayer = getLayer();
		poLayer->ResetReading();
		if (poLayer->GetSpatialRef() == nullptr) return std::nullopt;
		return poLayer->GetSpatialRef()->exportToWkt();
	}

	/// Returns whether the reader can parse the given file.
	static bool canRead(std::filesystem::path path) {
		GDALDataset *poDS;

		poDS = (GDALDataset*) GDALOpenEx( path.string().c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr );
		return poDS != nullptr;
	}
};

namespace {
static_assert(GeometryReader<GDALReader>);
static_assert(GeometryReaderFor<GDALReader, Point<Inexact>>);
}
}