#pragma once

#include "geometry_reader.h"

namespace cartocrow {
template <class Object, class Geometry, class OutputIterator, class Traits>
concept LinearObjectReaderTraits = requires(const Object& o, OutputIterator out) {
	{ Traits::template convert(o, out) }->std::same_as<bool>;
};

/// A GeometryReader that iterates over objects and converts them.
/// This is a helper class for implementing e.g. the Ipe and GDAL readers.
template <class Object, template<class> class DefaultReaderTraits>
class LinearObjectReader {
	virtual bool skipObject(const Object& obj) const = 0;
	virtual void readHelper(std::function<bool(const Object&)> handle) = 0;
	virtual GeometryAttributes getAttributes(const Object& obj) const = 0;

  public:
	/// Returns geometries in the provided file that are convertible to Geometry.
	/// \pre canRead(path)
	template <
		class Cardinality,
		class Geometry,
		class AttrMode,
		class Traits = DefaultReaderTraits<Geometry>
	>
		requires LinearObjectReaderTraits<Object, Geometry, std::back_insert_iterator<std::vector<Geometry>>, Traits>
	ReadResultT<Geometry, AttrMode, Cardinality> read() {
		std::vector<ElementTypeT<Geometry, AttrMode>> gs;

		readHelper([&](const Object& object) {
			if constexpr (std::same_as<AttrMode, WithoutAttributes>) {
				Traits::convert(object, std::back_inserter(gs));
				if constexpr (std::same_as<Cardinality, Single>) {
					return !gs.empty(); // stop if a geometry is found
				} else {
					return false;
				}
			} else {
				auto attributes = getAttributes(object);

				std::vector<Geometry> temps;
				Traits::convert(object, std::back_inserter(temps));
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
		class Traits = DefaultReaderTraits<Geometry>
	>
		requires LinearObjectReaderTraits<Object, Geometry, std::back_insert_iterator<std::vector<Geometry>>, Traits>
	void read(OutputIterator out) {
		readHelper([&](const Object& object) {
			if constexpr (std::same_as<AttrMode, WithoutAttributes>) {
				auto convertedSomething = Traits::convert(object, out);
				if constexpr (std::same_as<Cardinality, Single>) {
					return convertedSomething; // stop if a geometry is found
				} else {
					return false;
				}
			} else {
				auto attributes = getAttributes(object);

				std::vector<Geometry> temps;
				Traits::convert(object, std::back_inserter(temps));
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
}