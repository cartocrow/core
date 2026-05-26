#include "../catch.hpp"

#include <cartocrow/core/core.h>
#include <cartocrow/data_structures/straight_graph_2.h>

using namespace cartocrow;

TEST_CASE("Graph static vertex map") {

	using G = Straight_graph_2<std::monostate, std::monostate, Exact>;
	using V = G::Vertex_handle;
	using P = Point<Exact>;

	G g;
	V u = g.add_vertex(P(0, 0));
	V v = g.add_vertex(P(0, 0));

	Graph_static_vertex_map<G,int> map(g, 1);

	CHECK(map[u] == 1);
	CHECK(map[v] == 1);
		
	//for (auto& v : g.vertices()) {
	//	map[&v] = 7;
	//}

	map[u] = 3;
	map[v] = 2;

	CHECK(map[u] == 3);
	CHECK(map[v] == 2);
}

TEST_CASE("Graph vertex map") {

	using G = Straight_graph_2<std::monostate, std::monostate, Exact>;
	using V = G::Vertex_handle;
	using P = Point<Exact>;

	G g;
	V u = g.add_vertex(P(0, 0));
	V v = g.add_vertex(P(0, 0));

	Graph_vertex_map<G, int> map(g, 1);

	CHECK(map[u] == 1);
	CHECK(map[v] == 1);

	map[u] = 3;
	map[v] = 2;

	CHECK(map[u] == 3);
	CHECK(map[v] == 2);

	g.remove_vertex(u);

	CHECK(map[v] == 2);

	map[v] = 5;

	CHECK(map[v] == 5);
}

TEST_CASE("Graph static edge map") {

	using G = Straight_graph_2<std::monostate, std::monostate, Exact>;
	using V = G::Vertex_handle;
	using E = G::Edge_handle;
	using P = Point<Exact>;

	G g;
	V u = g.add_vertex(P(0, 0));
	V v = g.add_vertex(P(0, 0));
	V w = g.add_vertex(P(0, 0));

	E e = g.add_edge(u, v, Segment<Exact>(P(0, 0), P(0, 0)));
	E f = g.add_edge(v, w, Segment<Exact>(P(0, 0), P(0, 0)));

	Graph_static_edge_map<G, int> map(g, 1);

	CHECK(map[e] == 1);
	CHECK(map[f] == 1);

	map[e] = 3;
	map[f] = 2;

	CHECK(map[e] == 3);
	CHECK(map[f] == 2);
}

TEST_CASE("Graph edge map") {

	using G = Straight_graph_2<std::monostate, std::monostate, Exact>;
	using V = G::Vertex_handle;
	using E = G::Edge_handle;
	using P = Point<Exact>;

	G g;
	V u = g.add_vertex(P(0, 0));
	V v = g.add_vertex(P(0, 0));
	V w = g.add_vertex(P(0, 0));

	E e = g.add_edge(u, v, Segment<Exact>(P(0, 0), P(0, 0)));
	E f = g.add_edge(v, w, Segment<Exact>(P(0, 0), P(0, 0)));

	Graph_edge_map<G, int> map(g, 1);

	CHECK(map[e] == 1);
	CHECK(map[f] == 1);

	map[e] = 3;
	map[f] = 2;

	CHECK(map[e] == 3);
	CHECK(map[f] == 2);

	g.remove_edge(e);

	CHECK(map[f] == 2);

	map[f] = 5;

	CHECK(map[f] == 5);
}