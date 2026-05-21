#include "point_set.h"

namespace cartocrow {
PointSet<Inexact> approximate(const PointSet<Exact>& ps) {
    std::vector<Point<Inexact>> psInexact;

    for (const auto& p : ps.points) {
		psInexact.push_back(approximate(p));
    }

    return {psInexact};
}
}
