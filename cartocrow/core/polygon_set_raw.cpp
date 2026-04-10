#include "polygon_set_raw.h"

namespace cartocrow {
PolygonSetRaw<Inexact> approximate(const PolygonSetRaw<Exact>& pgs) {
	PolygonSetRaw<Inexact> approximated;
	for (const PolygonWithHoles<Exact>& pgn : pgs.polygons_with_holes) {
		approximated.polygons_with_holes.push_back(cartocrow::approximate(pgn));
	}
	return approximated;
}

PolygonSetRaw<Exact> pretendExact(const PolygonSetRaw<Inexact>& pgs) {
	PolygonSetRaw<Exact> exact;
	for (const auto& pgn : pgs.polygons_with_holes) {
		exact.polygons_with_holes.push_back(cartocrow::pretendExact(pgn));
	}
	return exact;
}
}