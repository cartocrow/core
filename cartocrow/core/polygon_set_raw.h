#include <cartocrow/core/core.h>
#include <cartocrow/core/transform_helpers.h>

namespace cartocrow {
template <class K>
struct PolygonSetRaw {
	std::vector<PolygonWithHoles<K>> polygons_with_holes;

	PolygonSetRaw<K> transform(const CGAL::Aff_transformation_2<K>& trans) const {
		PolygonSetRaw<K> transformed;
		for (const auto& pgn : polygons_with_holes) {
			transformed.polygons_with_holes.push_back(transform(trans, pgn));
		}
		return transformed;
	}

	Box bbox() const {
		return CGAL::bbox_2(polygons_with_holes.begin(), polygons_with_holes.end());
	}
};

PolygonSetRaw<Inexact> approximate(const PolygonSetRaw<Exact>& pgs);
PolygonSetRaw<Exact> pretendExact(const PolygonSetRaw<Inexact>& pgs);
}