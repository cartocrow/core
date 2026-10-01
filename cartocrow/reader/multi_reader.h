#include "geometry_reader.h"

namespace cartocrow {
template <class Geometry>
struct DefaultMultiReaderTraits
{
    template<class Reader>
    using ReaderTraits = typename Reader::template DefaultTraits<Geometry>;
};

template <class... Readers>
class MultiReader;

template <>
class MultiReader<> {
  public:
    template<class Geometry>
    using DefaultTraits = DefaultMultiReaderTraits<Geometry>;

    MultiReader(std::filesystem::path path) {}

    void load(std::filesystem::path path) {}

    static bool canRead(std::filesystem::path path) {
        return false;
    }

    template <
		class Cardinality,
		class Geometry,
		class AttrMode,
		class Traits = DefaultMultiReaderTraits<Geometry>
	>
	ReadResultT<Geometry, AttrMode, Cardinality> read() {
        throw std::runtime_error("Cannot read this file!");
	}

	/// Returns geometries in the provided file that are convertible to Geometry.
	/// \pre canRead(path)
	template <
		class Cardinality,
		class Geometry,
		class AttrMode,
		class OutputIterator,
		class Traits = DefaultMultiReaderTraits<Geometry>
	>
	void read(OutputIterator out) {
        throw std::runtime_error("Cannot read this file!");
	}

    std::optional<std::string> readSpatialReference() { return std::nullopt; }
};

template <class FirstReader, class... OtherReaders>
class MultiReader<FirstReader, OtherReaders...> {
private:
    template<class...>
    friend class MultiReader;

	using NextReader = MultiReader<OtherReaders...>;

    using ReaderVariant =
    std::variant<FirstReader, OtherReaders...>;

    ReaderVariant m_reader;

protected:
    template <class Variant, class Reader, class... Rest>
    static Variant makeReaderImpl(std::filesystem::path path)
    {
        if (Reader::canRead(path))
            return Variant{Reader(path)};

        if constexpr (sizeof...(Rest) > 0)
            return makeReaderImpl<Variant, Rest...>(path);
        else
            throw std::runtime_error("Cannot read this file!");
    }

    static ReaderVariant makeReader(std::filesystem::path path)
    {
        return makeReaderImpl<
            ReaderVariant,
            FirstReader,
            OtherReaders...
        >(path);
    }
public:
    template<class Geometry>
    using DefaultTraits = DefaultMultiReaderTraits<Geometry>;

    MultiReader(std::filesystem::path path)
        : m_reader(makeReader(path)) {}

    void load(std::filesystem::path path) {
        m_reader = makeReader(path);
    }

    static bool canRead(std::filesystem::path path) {
        return FirstReader::canRead(path) || NextReader::canRead(path);
    }

    ReaderVariant& getReader() {
        return m_reader;
    }

    std::optional<std::string> readSpatialReference() { 
        return std::visit([](auto& reader) {
            return reader.readSpatialReference();
        }, m_reader);
    }

    template <
		class Cardinality,
		class Geometry,
		class AttrMode,
		class Traits = DefaultMultiReaderTraits<Geometry>
	>
	ReadResultT<Geometry, AttrMode, Cardinality> read() {
        return std::visit([](auto& reader) {
            using Reader = std::decay_t<decltype(reader)>;
            return reader.template read<Cardinality, Geometry, AttrMode, typename Traits::template ReaderTraits<Reader>>();
        }, m_reader);
	}

	/// Returns geometries in the provided file that are convertible to Geometry.
	/// \pre canRead(path)
	template <
		class Cardinality,
		class Geometry,
		class AttrMode,
		class OutputIterator,
		class Traits = DefaultMultiReaderTraits<Geometry>
	>
	void read(OutputIterator out) {
		return std::visit([&out](auto& reader) {
            using Reader = std::decay_t<decltype(reader)>;
            return reader.template read<Cardinality, Geometry, AttrMode, OutputIterator, typename Traits::template ReaderTraits<Reader>>(out);
        }, m_reader);
	}
};
}

// For testing purposes (not ideal that ipe and gdal readers are included here)
#include "ipe_reader.h"
#include "gdal_reader.h"

namespace cartocrow {
static_assert(GeometryReader<MultiReader<>>);
static_assert(GeometryReader<MultiReader<IpeReader>>);
static_assert(GeometryReader<MultiReader<IpeReader, GDALReader>>);
static_assert(GeometryReaderFor<MultiReader<>, Point<Inexact>>);
static_assert(GeometryReaderFor<MultiReader<IpeReader>, Point<Inexact>>);
static_assert(GeometryReaderFor<MultiReader<IpeReader, GDALReader>, Point<Inexact>>);
}