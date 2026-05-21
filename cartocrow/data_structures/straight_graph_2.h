#pragma once

#include "graph_2.h"
#include "graph_segment_traits_2.h"

namespace cartocrow {
template <class VD, class ED, class K>
using Straight_graph_2 = Graph_2<VD, ED, Graph_segment_curve_traits_2<K>>;
}
