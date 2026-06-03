#pragma once

#include "graph_traits_2.h"
#include "graph_curve_traits_2.h"
#include "graph_operation_2.h"

namespace cartocrow {

class Graph_map_base {

	template <class VertexData, class EdgeData, GraphCurveTraits_2 CurveTraits, GraphTraits_2 GraphTraits>
	friend class Graph_2;

	template <class G>
	friend class AddVertex;
	template <class G>
	friend class RemoveVertex;
	template <class G>
	friend class AddEdge;
	template <class G>
	friend class RemoveEdge;

  protected:
	void clear() {
		resize(0);
	}

	virtual void resize(const size_t size) = 0;
	virtual void add_index() = 0;
	virtual void add_index(const size_t index) = 0;
	virtual void remove_index(const size_t index) = 0;
	virtual void remove_last_index() = 0;
};

template <class G, class E, typename T> class Graph_map : public Graph_map_base {

	friend G;

  protected:
	std::vector<T> m_vec;
	const T m_init;

	void resize(const size_t size) override {
		m_vec.resize(size, m_init);
	}

	void add_index() override {
		m_vec.push_back(m_init);
	}

	void add_index(const size_t index) override {
		m_vec.push_back(m_vec[index]);
		m_vec[index] = m_init;
	}

	void remove_index(const size_t index) override {
		m_vec[index] = m_vec[m_vec.size() - 1];
		m_vec.pop_back();
	}

	void remove_last_index() override {
		m_vec.pop_back();
	}

  public:
	Graph_map(const T init, int cnt) : m_init(init) {
		m_vec.resize(cnt, m_init);
	}

	T& operator[](const E elt) {
		return m_vec[elt->graph_index()];
	}

	void assign(const T v) {
		m_vec.assign(m_vec.size(), v);
	}
};

template <class G, typename T>
class Graph_vertex_map : public Graph_map<G, typename G::Vertex_handle, T> {
	G& m_graph;

  public:
	Graph_vertex_map(G& graph, const T init)
	    : Graph_map<G, G::Vertex_handle, T>(init, graph.number_of_vertices()), m_graph(graph) {
		this->m_graph.add_vertex_map(this);
	}

	~Graph_vertex_map() {
		this->m_graph.remove_vertex_map(this);
	}
};

template <class G, typename T> class Graph_static_vertex_map {

  public:
	using Vertex_handle = G::Vertex_handle;

  private:
	std::vector<T> m_vec;

  public:
	Graph_static_vertex_map(const G& graph, const T init) {
		m_vec.resize(graph.number_of_vertices(), init);
	}

	T& operator[](const Vertex_handle vtx) {
		return m_vec[vtx->graph_index()];
	}
};

template <class G> class Graph_vertex_index_map {
  public:
	using Vertex_handle = G::Vertex_handle;

	size_t operator[](const Vertex_handle vtx) {
		return vtx->graph_index();
	}
};

template <class G, typename T>
class Graph_edge_map : public Graph_map<G, typename G::Edge_handle, T> {
	G& m_graph;

  public:
	Graph_edge_map(G& graph, const T init)
	    : Graph_map<G, typename G::Edge_handle, T>(init, graph.number_of_edges()), m_graph(graph) {
		this->m_graph.add_edge_map(this);
	}

	~Graph_edge_map() {
		this->m_graph.remove_edge_map(this);
	}
};

template <class G, typename T> class Graph_static_edge_map {

  public:
	using Edge_handle = G::Edge_handle;

  private:
	std::vector<T> m_vec;

  public:
	Graph_static_edge_map(const G& graph, const T init) {
		m_vec.resize(graph.number_of_edges(), init);
	}

	T& operator[](const Edge_handle edge) {
		return m_vec[edge->graph_index()];
	}
};

template <class G> class Graph_edge_index_map {
  public:
	using Edge_handle = G::Edge_handle;

	size_t operator[](const Edge_handle edge) {
		return edge->graph_index();
	}
};

} // namespace cartocrow