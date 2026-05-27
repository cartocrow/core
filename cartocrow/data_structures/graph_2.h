#pragma once

#include "graph_curve_traits_2.h"
#include "graph_map_2.h"
#include "graph_operation_2.h"
#include "graph_traits_2.h"

namespace utils {
template <typename T> bool vectorRemove(T elt, std::vector<T>& vec) {
	auto pos = std::find(vec.begin(), vec.end(), elt);
	if (pos != vec.end()) {
		vec.erase(pos);
		return true;
	} else {
		return false;
	}
}
} // namespace utils

namespace cartocrow {

template <class VertexData, class EdgeData, GraphCurveTraits_2 CurveTraits, GraphTraits_2 GraphTraits>
class Graph_2_vertex;
template <class VertexData, class EdgeData, GraphCurveTraits_2 CurveTraits, GraphTraits_2 GraphTraits>
class Graph_2_edge;

template <class VertexData, class EdgeData, GraphCurveTraits_2 CurveTraits, GraphTraits_2 GraphTraits>
class Graph_2 {
	friend class Graph_2_vertex<VertexData, EdgeData, CurveTraits, GraphTraits>;
	friend class Graph_2_edge<VertexData, EdgeData, CurveTraits, GraphTraits>;
	template <typename G, typename T> friend class Graph_vertex_map;
	template <typename G, typename T> friend class Graph_edge_map;

	template <class G> friend class AddVertex;
	template <class G> friend class RemoveVertex;
	template <class G> friend class AddEdge;
	template <class G> friend class RemoveEdge;

  public:
	using Vertex = Graph_2_vertex<VertexData, EdgeData, CurveTraits, GraphTraits>;
	using Vertex_handle = Vertex*;
	using Vertex_const_handle = const Vertex*;

	using Edge = Graph_2_edge<VertexData, EdgeData, CurveTraits, GraphTraits>;
	using Edge_handle = Edge*;
	using Edge_const_handle = const Edge*;

  private:
	using Vertex_container = std::vector<Vertex_handle>;
	using Edge_container = std::vector<Edge_handle>;

  public:
	using Vertex_iterator = Vertex_container::iterator;
	using Vertex_const_iterator = Vertex_container::const_iterator;
	using Edge_iterator = Edge_container::iterator;
	using Edge_const_iterator = Edge_container::const_iterator;

	using Point_2 = CurveTraits::Point_2;
	using Curve_2 = CurveTraits::Curve_2;
	using Vertex_data = VertexData;
	using Edge_data = EdgeData;
	using Curve_traits = CurveTraits;
	using Kernel = Curve_traits::Kernel;

  private:
	Vertex_container m_vertices;
	Edge_container m_edges;
	bool m_oriented = false;
	bool m_sorted = false;

	std::vector<Graph_map_base*> m_vertex_maps;
	std::vector<Graph_map_base*> m_edge_maps;

	using HistoryContainer =
	    std::conditional<GraphTraits::historic, std::vector<Operation*>, std::monostate>::type;

	HistoryContainer m_past;
	HistoryContainer m_future;

	void add_vertex_map(Graph_map_base* map) {
		m_vertex_maps.push_back(map);
	}

	void remove_vertex_map(Graph_map_base* map) {
		auto pos = std::find(m_vertex_maps.begin(), m_vertex_maps.end(), map);
		if (pos != m_vertex_maps.end()) {
			m_vertex_maps.erase(pos);
		}
	}

	void add_edge_map(Graph_map_base* map) {
		m_edge_maps.push_back(map);
	}

	void remove_edge_map(Graph_map_base* map) {
		auto pos = std::find(m_edge_maps.begin(), m_edge_maps.end(), map);
		if (pos != m_vertex_maps.end()) {
			m_edge_maps.erase(pos);
		}
	}

  public:
	class Vertex_range {
		friend class Graph_2<VertexData, EdgeData, CurveTraits, GraphTraits>;

	  private:
		Vertex_container& m_container;

		Vertex_range(Vertex_container& container) : m_container(container) {}

	  public:
		Vertex_iterator begin() {
			return m_container.begin();
		}

		Vertex_iterator end() {
			return m_container.end();
		}
	};

	class Edge_range {
		friend class Graph_2<VertexData, EdgeData, CurveTraits, GraphTraits>;

	  private:
		Edge_container& m_container;

		Edge_range(Edge_container& container) : m_container(container) {}

	  public:
		Edge_iterator begin() {
			return m_container.begin();
		}

		Edge_iterator end() {
			return m_container.end();
		}
	};

	Graph_2& operator=(const Graph_2& other) {
		if (this == &other)
			return *this;

		clear();

		const size_t num_v = other.number_of_vertices();
		m_vertices.resize(num_v);
		for (Graph_map_base* m : m_vertex_maps) {
			m->resize(num_v);
		}

		const size_t num_e = other.number_of_edges();
		m_edges.resize(num_e);
		for (Graph_map_base* m : m_edge_maps) {
			m->resize(num_e);
		}

		m_oriented = other.m_oriented;
		m_sorted = other.m_sorted;

		Graph_static_vertex_map<Graph_2, Vertex_handle> vmap(other, nullptr);

		for (auto vit : other.vertices()) {
			Vertex_handle new_v = new Vertex(vit->m_point, vit->m_data);
			new_v->m_index = vit->m_index;
			m_vertices[vit->m_index] = new_v;

			vmap[vit] = new_v;
		}

		Graph_static_edge_map<Graph_2, Edge_handle> emap(other, nullptr);

		for (auto eit : other.edges()) {
			Vertex_handle new_source = vmap[eit->m_source];
			Vertex_handle new_target = vmap[eit->m_target];

			Edge_handle new_e = new Edge(new_source, new_target, eit->m_curve, eit->m_data);
			new_e->m_index = eit->m_index;
			m_edges[eit->m_index] = new_e;

			emap[eit] = new_e;
		}

		for (auto vit : other.vertices()) {
			Vertex_handle new_v = vmap[vit];
			for (auto old_e : vit->m_incident) {
				new_v->m_incident.push_back(emap[old_e]);
			}
		}

		return *this;
	}

	Graph_2() = default;

	Graph_2(const Graph_2& other) {
		*this = other;
	}

	~Graph_2() {
		for (Vertex_handle v : m_vertices) {
			delete v;
		}
		for (Edge_handle e : m_edges) {
			delete e;
		}
		if constexpr (GraphTraits::historic) {
			for (Operation* op : m_past) {
				delete op;
			}
			for (Operation* op : m_future) {
				delete op;
			}
		}
	}

	Graph_2 transform(CGAL::Aff_transformation_2<Kernel> trans) const {
		Graph_2 transformed;

		transformed.m_vertices.resize(number_of_vertices());
		transformed.m_edges.resize(number_of_edges());

		transformed.m_oriented = m_oriented;
		transformed.m_sorted = m_sorted;

		Graph_static_vertex_map<Graph_2, Vertex_handle> vmap(this, nullptr);

		for (auto vit : vertices()) {
			Vertex_handle new_v = new Vertex(vit->m_point.transform(trans), vit->m_data);
			new_v->m_index = vit->m_index;
			transformed.m_vertices[vit->m_index] = new_v;

			vmap[vit] = new_v;
		}

		Graph_static_edge_map<Graph_2, Edge_handle> emap(this, nullptr);

		for (auto eit : edges()) {
			Vertex_handle new_source = vmap[eit->m_source];
			Vertex_handle new_target = vmap[eit->m_target];

			Edge_handle new_e = new Edge(new_source, new_target,
			                             Curve_traits::transform(eit->m_curve, trans), eit->m_data);
			new_e->m_index = eit->m_index;
			transformed.m_edges[eit->m_index] = new_e;

			emap[eit] = new_e;
		}

		for (auto vit : vertices()) {
			Vertex_handle new_v = vmap[vit];
			for (auto old_e : vit->m_incident) {
				new_v->m_incident.push_back(emap[old_e]);
			}
		}

		return transformed;
	}

	Vertex_range vertices() const {
		return Vertex_range(m_vertices);
	}
	Vertex_iterator vertices_begin() {
		return m_vertices.begin();
	}
	Vertex_iterator vertices_end() {
		return m_vertices.end();
	}

	Edge_range edges() const {
		return Edge_range(m_edges);
	}
	Edge_iterator edges_begin() {
		return m_edges.begin();
	}
	Edge_iterator edges_end() {
		return m_edges.end();
	}

	Vertex_const_iterator vertices_begin() const {
		return m_vertices.cbegin();
	}
	Vertex_const_iterator vertices_end() const {
		return m_vertices.cend();
	}
	Edge_const_iterator edges_begin() const {
		return m_edges.cbegin();
	}
	Edge_const_iterator edges_end() const {
		return m_edges.cend();
	}

	size_t number_of_vertices() const {
		return m_vertices.size();
	}
	size_t number_of_edges() const {
		return m_edges.size();
	}

	void clear() {
		for (Vertex_handle v : m_vertices) {
			delete v;
		}
		for (Edge_handle e : m_edges) {
			delete e;
		}
		if constexpr (GraphTraits::historic) {
			for (Operation* op : m_past) {
				delete op;
			}
			m_past.clear();
			for (Operation* op : m_future) {
				delete op;
			}
			m_future.clear();
		}
		for (Graph_map_base* m : m_vertex_maps) {
			m->clear();
		}
		for (Graph_map_base* m : m_edge_maps) {
			m->clear();
		}
	}

	Vertex_handle add_vertex(Point_2 p) {
		Vertex_handle v = new Vertex(p);
		const int index = v->m_index = m_vertices.size();
		m_vertices.push_back(v);
		for (Graph_map_base* m : m_vertex_maps) {
			m->add_index();
		}

		if constexpr (GraphTraits::historic) {
			m_past.push_back(new AddVertex(*this, v));
		}

		return v;
	}
	void remove_vertex(Vertex_handle v) {
		const int index = v->m_index;
		for (Edge_handle e : v->m_incident) {
			remove_edge(e);
		}
		const int last = m_vertices.size() - 1;
		if (index != last) {
			m_vertices[index] = m_vertices[last];
			m_vertices[index]->m_index = index;
			for (Graph_map_base* m : m_vertex_maps) {
				m->remove_index(index);
			}
		} else {
			for (Graph_map_base* m : m_vertex_maps) {
				m->remove_last_index();
			}
		}

		m_vertices.pop_back();

		if constexpr (GraphTraits::historic) {
			m_past.push_back(new RemoveVertex(*this, v));
		} else {
			delete v;
		}
	}
	Edge_handle add_edge(Vertex_handle source,
	                     Vertex_handle target) requires std::same_as<Curve_2, Segment<Kernel>> {
		return add_edge(source, target, Curve_2(source->m_point, target->m_point));
	}

	Edge_handle add_edge(Vertex_handle source, Vertex_handle target, const Curve_2& curve) {
		Edge_handle e = new Edge(source, target, curve);
		const int index = e->m_index = m_edges.size();
		m_edges.push_back(e);

		m_sorted = false;

		std::vector<Edge_handle>& source_inc = source->m_incident;
		std::vector<Edge_handle>& target_inc = target->m_incident;

		// Make sure orientation is preserved
		source_inc.push_back(e);
		if (target_inc.empty()) {
			target_inc.push_back(e);
		} else {
			target_inc.push_back(target_inc.front());
			target_inc[0] = e;
		}

		for (Graph_map_base* m : m_edge_maps) {
			m->add_index();
		}

		return e;
	}
	void remove_edge(Edge_handle e) {

		const int index = e->m_index;

		auto& sInc = e->m_source->m_incident;
		auto& tInc = e->m_target->m_incident;
		utils::vectorRemove(e, sInc);
		utils::vectorRemove(e, tInc);

		const int last = m_edges.size() - 1;
		if (index != last) {
			m_edges[index] = m_edges[last];
			m_edges[index]->m_index = index;

			for (Graph_map_base* m : m_edge_maps) {
				m->remove_index(index);
			}
		} else {
			for (Graph_map_base* m : m_edge_maps) {
				m->remove_last_index();
			}
		}
		m_edges.pop_back();
		delete e;
	}

	bool can_undo() const requires GraphTraits::historic {
		return !m_past.empty();
	}
	void undo() requires GraphTraits::historic {
		Operation* op = m_past.back();
		m_past.pop_back();
		op->undo();
		m_future.push_back(op);
	}
	void undo_all() requires GraphTraits::historic {
		while (!m_past.empty()) {
			undo();
		}
	}
	bool can_redo() const requires GraphTraits::historic {
		return !m_future.empty();
	}
	void redo() requires GraphTraits::historic {
		Operation* op = m_future.back();
		m_future.pop_back();
		op->redo();
		m_past.push_back(op);
	}
	void redo_all() requires GraphTraits::historic {
		while (!m_future.empty()) {
			redo();
		}
	}
	void forget_past() requires GraphTraits::historic {
		while (!m_past.empty()) {
			Operation* op = m_past.back();
			m_past.pop_back();
			delete op;
		}
	}
	void forget_future() requires GraphTraits::historic {
		while (!m_future.empty()) {
			Operation* op = m_future.back();
			m_future.pop_back();
			delete op;
		}
	}

	/// Split a vertex into two. Or equivalently, replace two curves by three new ones.
	/// Connect the two new vertices with three new curves.
	Edge_handle split_vertex(Vertex_handle v, Curve_2 c0, Curve_2 c1, Curve_2 c2) {
		assert(c0.target() == c1.source());
		assert(c1.target() == c2.source());

		auto e0 = v->incoming();
		auto v0 = e0->source();
		auto e1 = v->outgoing();
		auto v3 = e1->target();
		remove_edge(e0);
		remove_edge(e1);
		remove_vertex(v);
		auto v1 = add_vertex(c0.target());
		auto v2 = add_vertex(c1.target());
		add_edge(v0, v1, c0);
		auto eh = add_edge(v1, v2, c1);
		add_edge(v2, v3, c2);

		return eh;
	}
	/// Collapse an edge, removing it together with the edge that comes before and after it.
	/// Replace it with a vertex connected by two new curves.
	/// This is equivalent to replacing three curves by two new ones.
	Vertex_handle collapse_edge(Edge_handle e, Curve_2 toNewPoint, Curve_2 fromNewPoint) {
		assert(toNewPoint.target() == fromNewPoint.source());
		Edge_handle prev = e->prev();
		Edge_handle next = e->next();
		Vertex_handle eSource = e->source();
		Vertex_handle eTarget = e->target();
		Vertex_handle prevSource = prev->source();
		Vertex_handle nextTarget = next->target();

		// todo: reuse edge/vertex objects and move them

		// Remove old edges and vertices
		remove_edge(e);
		remove_edge(prev);
		remove_edge(next);
		remove_vertex(eSource);
		remove_vertex(eTarget);

		// Add new vertex
		auto v = add_vertex(toNewPoint.target());
		// Add the two new edges
		auto eh1 = add_edge(prevSource, v, toNewPoint);
		auto eh2 = add_edge(v, nextTarget, fromNewPoint);

		return v;
	}

	/// Merge an edge with the one that precedes it and replace them with newCurve.
	/// Returns the handle of the new edge.
	/// \pre Source vertex of edge e has degree 2.
	Edge_handle merge_edge_with_prev(Edge_handle e, Curve_2 newCurve) {
		assert(e->source()->degree() != 2);
		Edge_handle prev = e->prev();

		Vertex_handle u = prev->source();
		Vertex_handle v = e->target();

		// todo: reuse edge/vertex objects and move them

		// Remove old edges and vertices
		remove_edge(e);
		remove_edge(prev);
		remove_vertex(e->source());

		// Add new edge
		auto eh = add_edge(u, v, newCurve);

		return eh;
	}

	/// Replace an edge by two edges
	/// Returns the handle of the newly created vertex.
	Vertex_handle subdivide_edge(Edge_handle e, Curve_2 toNewPoint, Curve_2 fromNewPoint) {
		assert(toNewPoint.target() == fromNewPoint.source());

		auto s = e->source();
		auto t = e->target();
		remove_edge(e);
		auto vh = add_vertex(toNewPoint.target());
		add_edge(s, vh, toNewPoint);
		add_edge(vh, t, fromNewPoint);

		return vh;
	}

	/// <summary>
	/// Ensures that all degree-2 vertices v have edge(0) = (u,v) and edge(1) = (v,w).
	/// </summary>
	void orient() {
		for (Vertex_handle v = m_vertices.begin(); v != m_vertices.end(); ++v) {
			if (v->degree() != 2)
				continue; // irrelevant for orientation

			Edge_handle bwd = v->m_incident[0];
			Edge_handle fwd = v->m_incident[1];

			if (bwd->m_target == v && fwd->m_source == v)
				continue; // already satisfies orientation

			if (bwd->m_target != v) {
				bwd->reverse();
			}

			if (fwd->m_source != v) {
				fwd->reverse();
			}

			Vertex_handle fv = fwd->m_target;
			while (fv->degree() == 2 && fv != v) {
				if (fv->m_incident[0] != fwd) {
					fv->m_incident[1] = fv->m_incident[0];
					fv->m_incident[0] = fwd;
				}

				fwd = fv->m_incident[1];
				if (fwd->m_source != fv) {
					fwd->reverse();
				}
				fv = fwd->m_target;
			}

			if (fv != v) {
				Vertex_handle bv = bwd->m_source;
				while (bv->degree() == 2) {
					if (bv->m_incident[1] != bwd) {
						bv->m_incident[0] = bv->m_incident[1];
						bv->m_incident[1] = bwd;
					}

					bwd = bv->m_incident[0];
					if (bwd->m_target != bv) {
						bwd->reverse();
					}
					bv = bwd->m_source;
				}
			}
		}

		m_oriented = true;
	}
	bool verify_oriented() const {
		for (Vertex_handle v = m_vertices.begin(); v != m_vertices.end(); ++v) {
			if (v->degree() != 2)
				continue; // irrelevant for orientation

			auto bwd = v->m_incident[0];
			auto fwd = v->m_incident[1];

			if (bwd->m_target == v && fwd->m_source == v)
				continue;

			return false;
		}

		return true;
	}
	bool is_oriented() const {
		assert(!m_oriented || verify_oriented());
		return m_oriented;
	}
	void sort_incident_edges() {
		for (Vertex_handle& v : m_vertices) {
			if (v->degree() > 2) {
				std::ranges::sort(v->incident, [&v](Edge* e, Edge* f) {
					CGAL::Direction_2<Kernel> dir_e =
					    CGAL::Direction_2<Kernel>(e->other(v)->getPoint() - v->getPoint());
					CGAL::Direction_2<Kernel> dir_f =
					    CGAL::Direction_2<Kernel>(f->other(v)->getPoint() - v->getPoint());
					return dir_e < dir_f;
				});
			}
		}
		m_sorted = true;
	}
	bool verify_sorted() const {
		for (const Vertex_const_handle v : m_vertices) {
			if (v->degree() > 2) {
				CGAL::Direction_2<Kernel> dir_prev =
				    CGAL::Direction_2<Kernel>(v->neighbor(0)->getPoint() - v->getPoint());
				for (int i = 1; i < v->degree(); i++) {
					CGAL::Direction_2<Kernel> dir =
					    CGAL::Direction_2<Kernel>(v->neighbor(i)->getPoint() - v->getPoint());
					if (dir < dir_prev) {
						return false;
					}
					dir_prev = dir;
				}
			}
		}
		return true;
	}
	bool is_sorted() const {
		assert(!m_sorted || verify_sorted());
		return m_sorted;
	}

	CGAL::Bbox_2 bbox() const {
		if (m_vertices.empty()) {
			return {0, 0, 0, 0};
		}
		std::vector<Point<Inexact>> points;
		for (Vertex_handle v : m_vertices) {
			points.push_back(v.point());
		}
		auto box = CGAL::bbox_2(points.begin(), points.end());
		for (const auto& e : m_edges) {
			box = box + Curve_traits::bbox(e.curve());
		}
		return box;
	}
};

template <class VertexData, class EdgeData, GraphCurveTraits_2 CurveTraits, GraphTraits_2 GraphTraits>
class Graph_2_vertex {
	friend class Graph_2<VertexData, EdgeData, CurveTraits, GraphTraits>;
	friend class Graph_2_edge<VertexData, EdgeData, CurveTraits, GraphTraits>;

  public:
	using Vertex = Graph_2_vertex<VertexData, EdgeData, CurveTraits, GraphTraits>;
	using Edge = Graph_2_edge<VertexData, EdgeData, CurveTraits, GraphTraits>;
	using Graph = Graph_2<VertexData, EdgeData, CurveTraits, GraphTraits>;
	using Vertex_handle = Graph::Vertex_handle;
	using Vertex_const_handle = Graph::Vertex_const_handle;
	using Edge_handle = Graph::Edge_handle;
	using Edge_const_handle = Graph::Edge_const_handle;
	using Point_2 = CurveTraits::Point_2;
	using Vertex_data = VertexData;
	using Edge_data = EdgeData;
	using Curve_traits = CurveTraits;

  private:
	using Edge_container = std::vector<Edge_handle>;
	Point_2 m_point;
	Edge_container m_incident = Edge_container();
	Vertex_data m_data;
	size_t m_index;

	Graph_2_vertex(const Point_2 point) : m_point(std::move(point)) {}
	Graph_2_vertex(const Point_2 point, const Vertex_data data)
	    : m_point(std::move(point)), m_data(std::move(data)) {}

  public:
	size_t graph_index() const {
		return m_index;
	}
	const Point_2& point() const {
		return m_point;
	}
	Point_2& point() {
		return m_point;
	}
	int degree() const {
		return m_incident.size();
	}
	Edge_handle incoming() {
		assert(degree() == 2);
		return m_incident[0];
	}
	Edge_const_handle incoming() const {
		assert(degree() == 2);
		return m_incident[0];
	}
	Edge_handle outgoing() {
		assert(degree() == 2);
		return m_incident[1];
	}
	Edge_const_handle outgoing() const {
		assert(degree() == 2);
		return m_incident[1];
	}
	Vertex_handle prev() {
		return incoming()->source();
	}
	Vertex_const_handle prev() const {
		return incoming()->source();
	}
	Vertex_handle next() {
		return outgoing()->target();
	}
	Vertex_const_handle next() const {
		return outgoing()->target();
	}
	Edge_container::iterator incident_edges_begin() {
		return m_incident.begin();
	}
	Edge_container::iterator incident_edges_end() {
		return m_incident.end();
	}
	Edge_container::const_iterator incident_edges_begin() const {
		return m_incident.cbegin();
	}
	Edge_container::const_iterator incident_edges_end() const {
		return m_incident.cend();
	}
	Vertex_data& data() {
		return m_data;
	}
	const Vertex_data& data() const {
		return m_data;
	}
};

template <class VertexData, class EdgeData, GraphCurveTraits_2 CurveTraits, GraphTraits_2 GraphTraits>
class Graph_2_edge {
	friend class Graph_2<VertexData, EdgeData, CurveTraits, GraphTraits>;
	friend class Graph_2_vertex<VertexData, EdgeData, CurveTraits, GraphTraits>;

  public:
	using Vertex = Graph_2_vertex<VertexData, EdgeData, CurveTraits, GraphTraits>;
	using Edge = Graph_2_edge<VertexData, EdgeData, CurveTraits, GraphTraits>;
	using Graph = Graph_2<VertexData, EdgeData, CurveTraits, GraphTraits>;
	using Vertex_handle = Graph::Vertex_handle;
	using Vertex_const_handle = Graph::Vertex_const_handle;
	using Edge_handle = Graph::Edge_handle;
	using Edge_const_handle = Graph::Edge_const_handle;
	using Point_2 = CurveTraits::Point_2;
	using Curve_2 = CurveTraits::Curve_2;
	using Vertex_data = VertexData;
	using Edge_data = EdgeData;
	using Curve_traits = CurveTraits;

  private:
	Vertex_handle m_source;
	Vertex_handle m_target;
	Curve_2 m_curve;
	EdgeData m_data;
	size_t m_index;

	Graph_2_edge(Vertex_handle source, Vertex_handle target, const Curve_2 curve)
	    : m_source(std::move(source)), m_target(std::move(target)), m_curve(std::move(curve)) {
		assert(m_curve.source() == m_source->m_point);
		assert(m_curve.target() == m_target->m_point);
	}

	Graph_2_edge(Vertex_handle source, Vertex_handle target, const Curve_2 curve, const Edge_data d)
	    : m_source(std::move(source)), m_target(std::move(target)), m_curve(std::move(curve)),
	      m_data(std::move(d)) {
		assert(m_curve.source() == m_source->m_point);
		assert(m_curve.target() == m_target->m_point);
	}

  public:
	size_t graph_index() const {
		return m_index;
	}

	Vertex_handle source() {
		return m_source;
	}
	Vertex_handle target() {
		return m_target;
	}
	Vertex_const_handle source() const {
		return m_source;
	}
	Vertex_const_handle target() const {
		return m_target;
	}
	Vertex_handle other(Vertex_handle v) {
		assert(v == m_source || v == m_target);
		return v == m_source ? m_target : m_source;
	}
	Curve_2& curve() {
		return m_curve;
	}
	const Curve_2& curve() const {
		return m_curve;
	}
	void reverse() {
		std::swap(m_source, m_target);
		m_curve = Curve_traits::reversed(m_curve);
	}
	Edge_handle prev() {
		return m_source->incoming();
	}
	Edge_const_handle prev() const {
		return m_source->incoming();
	}
	Edge_handle next() {
		return m_target->outgoing();
	}
	Edge_const_handle next() const {
		return m_target->outgoing();
	}
	Edge_data& data() {
		return m_data;
	}
	const Edge_data& data() const {
		return m_data;
	}
};

} // namespace cartocrow
