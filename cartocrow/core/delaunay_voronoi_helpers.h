#pragma once

#include "core.h"
#include "segment_delaunay_graph_helpers.h"

namespace cartocrow {
/// Return the sites that define an edge of a (segment) Delaunay graph / triangulation
template <class DG>
std::pair<typename DG::Site_2, typename DG::Site_2> defining_sites(const typename DG::Edge& edge) {
	return {edge.first->vertex(DG::cw(edge.second))->site(),
	        edge.first->vertex(DG::ccw(edge.second))->site()};
}

/// Return the Voronoi edges that share an endpoint with the given edge.
/// Equivalently: these are the Delaunay edges that share a face with the given edge.
template <class DG>
std::pair<std::pair<typename DG::Edge, typename DG::Edge>, std::pair<typename DG::Edge, typename DG::Edge>>
adjacent_edges(const DG& dg, const typename DG::Edge& edge) {
	auto& [f1, i1] = edge;
	auto [f2, i2] = mirror_edge(dg, edge);
	return {
		{
		    {f1, (i1+1) % 3},
			{f1, (i1+2) % 3},
		},
		{
			{f2, (i2+1) % 3},
			{f2, (i2+2) % 3},
		}
	};
}

/// Output the connected components of Voronoi edges that satisfy the given predicate.
template <class SDG, class OutputIterator>
void bfsOnVoronoiEdges(const SDG& delaunay, const std::function<bool(const typename SDG::Edge&)>& predicate, OutputIterator out) {
	using Edge = typename SDG::Edge;
	std::vector<Edge> handled;

	for (auto eit = delaunay.finite_edges_begin(); eit != delaunay.finite_edges_end(); ++eit) {
		if (!predicate(*eit)) continue;
		auto& e = *eit;

		if (std::find(handled.begin(), handled.end(), e) != handled.end()) continue;
		if (std::find(handled.begin(), handled.end(), mirror_edge(delaunay, e)) != handled.end()) continue;

		// Construct the component that has edge e.
		std::vector<Edge> component;
		std::deque<Edge> q({e});

		while (!q.empty()) {
			auto current = q.front();
			q.pop_front();
			if (std::find(component.begin(), component.end(), current) != component.end()) continue;
			if (std::find(component.begin(), component.end(), mirror_edge(delaunay, current)) != component.end()) continue;
			handled.push_back(current);
			if (!predicate(current)) continue;
			component.push_back(current);

			auto [adj1, adj2] = adjacent_edges(delaunay, current);
			q.push_back(adj1.first);
			q.push_back(adj1.second);
			q.push_back(adj2.first);
			q.push_back(adj2.second);
		}

		*out++ = component;
	}
}
}