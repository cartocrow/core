#pragma once

namespace cartocrow {
class Graph_map_base;

class Operation {

  public:
	virtual void undo() = 0;
	virtual void redo() = 0;
	virtual void forget_past() {}
	virtual void forget_future() {}
};

class OperationGroup {
  private:
	std::vector<std::unique_ptr<Operation>> ops;

  public:
	void add_operation(std::unique_ptr<Operation> op) {
		ops.push_back(std::move(op));
	}

	void undo() {
		for (auto it = ops.rbegin(); it != ops.rend(); ++it) {
			(*it)->undo();
		}
	}

	void redo() {
		for (auto& op : ops) {
			op->redo();
		}
	}
	
	void forget_past() {
		for (auto& op : ops) {
			op->forget_past();
		}
	}
	void forget_future() {
		for (auto& op : ops) {
			op->forget_future();
		}
	}
};

class History {
  private:
	using GroupContainer = std::list<OperationGroup>;
	using GroupHandle = GroupContainer::iterator;

	GroupContainer m_groups;
	GroupHandle m_curr = m_groups.end(); // points to the next redo-operation
	int m_build = 0;

  public:
	~History() {
		clear();
	}

	void start_group() {
		if (m_build == 0) {
			assert(m_curr == m_groups.end());
			m_groups.emplace_back();
			--m_curr;
		}
		++m_build;
	}
	void end_group() {
		assert(m_build > 0);
		--m_build;

		if (m_build == 0) {
			++m_curr;
		}
	}
	void add_operation(std::unique_ptr<Operation> op) {
		if (m_build == 0) {
			start_group();
			m_curr->add_operation(std::move(op));
			end_group();
		} else {
			m_curr->add_operation(std::move(op));
		}
	}

	inline bool can_undo() const {
		assert(m_build == 0);
		return m_curr != m_groups.begin();
	}
	inline void undo() {
		--m_curr;
		m_curr->undo();
	}
	void undo_all() {
		while (can_undo()) {
			undo();
		}
	}
	inline bool can_redo() const {
		return m_curr != m_groups.end();
	}
	inline void redo() {
		m_curr->redo();
		++m_curr;
	}
	void redo_all() {
		while (can_redo()) {
			redo();
		}
	}
	void clear() {
		forget_past();
		forget_future();
	}
	void forget_past() {
		while (m_curr != m_groups.begin()) {
			m_groups.front().forget_past();
			m_groups.pop_front();
		}
	}
	void forget_future() {
		if (m_curr != m_groups.end()) {
			while (true) {
				bool deleting_curr = m_curr == (--m_groups.end());
				m_groups.back().forget_future();
				m_groups.pop_back();
				if (deleting_curr)
					break;
			}

			m_curr = m_groups.end();
		}
	}
};

template <class G> class AddVertex : public Operation {
  private:
	G& m_graph;
	G::Vertex_handle m_vertex;

  public:
	AddVertex(G& graph, G::Vertex_handle v) : m_graph(graph), m_vertex(v) {}

	void forget_future() override {
		delete m_vertex;
	}
	
	static void add_vertex(G& g, typename G::Vertex_handle v) {
		const int index = v->m_index = g.m_vertices.size();
		g.m_vertices.push_back(v);
		for (Graph_map_base* m : g.m_vertex_maps) {
			m->add_index();
		}
	}

	void undo() override {
		m_graph.m_vertices.pop_back();

		for (Graph_map_base* m : m_graph.m_vertex_maps) {
			m->remove_last_index();
		}
	}
	void redo() override {
		add_vertex(m_graph, m_vertex);
	}
};

template <class G> class RemoveVertex : public Operation {
  private:
	G& m_graph;
	G::Vertex_handle m_vertex;

  public:
	RemoveVertex(G& graph, G::Vertex_handle v) : m_graph(graph), m_vertex(v) {}

	void forget_past() {
		delete m_vertex;
	}

	static void remove_vertex(G& g, typename G::Vertex_handle v) {
		const int index = v->m_index;
		const int last = g.m_vertices.size() - 1;
		if (index != last) {
			g.m_vertices[index] = g.m_vertices[last];
			g.m_vertices[index]->m_index = index;
			for (Graph_map_base* m : g.m_vertex_maps) {
				m->remove_index(index);
			}
		} else {
			for (Graph_map_base* m : g.m_vertex_maps) {
				m->remove_last_index();
			}
		}

		g.m_vertices.pop_back();
	}

	void undo() override {
		const size_t index = m_vertex->graph_index();
		if (index == m_graph.m_vertices.size()) {
			m_graph.m_vertices.push_back(m_vertex);
			for (Graph_map_base* m : m_graph.m_vertex_maps) {
				m->add_index();
			}
		} else {
			m_graph.m_vertices.push_back(m_graph.m_vertices[index]);
			m_graph.m_vertices.back()->m_index = m_graph.m_vertices.size()  - 1;
			m_graph.m_vertices[index] = m_vertex;
			for (Graph_map_base* m : m_graph.m_vertex_maps) {
				m->add_index(index);
			}
		}
	}
	void redo() override {
		remove_vertex(m_graph, m_vertex);
	}
};

template <class G> class AddEdge : public Operation {
  private:
	G& m_graph;
	G::Edge_handle m_edge;

  public:
	AddEdge(G& graph, G::Edge_handle e) : m_graph(graph), m_edge(e) {}

	void forget_future() override {
		delete m_edge;
	}

	static void add_edge(G& graph, G::Edge_handle e) {
		graph.m_edges.push_back(e);

		std::vector<G::Edge_handle>& source_inc = e->m_source->m_incident;
		std::vector<G::Edge_handle>& target_inc = e->m_target->m_incident;

		source_inc.push_back(e);
		target_inc.push_back(e);

		// TODO: we can be smarter about this
		graph.ensure_traits(e->m_source);
		graph.ensure_traits(e->m_target);

		for (Graph_map_base* m : graph.m_edge_maps) {
			m->add_index();
		}
	}

	void undo() override {
		m_graph.m_edges.pop_back();

		m_edge->m_source->remove_incident(m_edge);
		m_edge->m_target->remove_incident(m_edge);

		m_graph.ensure_traits(m_edge->m_source);
		m_graph.ensure_traits(m_edge->m_target);

		for (Graph_map_base* m : m_graph.m_edge_maps) {
			m->remove_last_index();
		}
	}
	void redo() override {
		add_edge(m_graph, m_edge);
	}
};

template <class G> class RemoveEdge : public Operation {
  private:
	G& m_graph;
	G::Edge_handle m_edge;

  public:
	RemoveEdge(G& graph, G::Edge_handle e) : m_graph(graph), m_edge(e) {}

	void forget_past() override {
		delete m_edge;
	}

	static void remove_edge(G& g, typename G::Edge_handle e) {
		const int index = e->m_index;

		e->m_source->remove_incident(e);
		e->m_target->remove_incident(e);

		g.ensure_traits(e->m_source);
		g.ensure_traits(e->m_target);

		const int last = g.m_edges.size() - 1;
		if (index != last) {
			g.m_edges[index] = g.m_edges[last];
			g.m_edges[index]->m_index = index;

			for (Graph_map_base* m : g.m_edge_maps) {
				m->remove_index(index);
			}
		} else {
			for (Graph_map_base* m : g.m_edge_maps) {
				m->remove_last_index();
			}
		}
		g.m_edges.pop_back();
	}

	void undo() override {
		const size_t index = m_edge->graph_index();
		if (index == m_graph.m_edges.size()) {
			m_graph.m_edges.push_back(m_edge);
			for (Graph_map_base* m : m_graph.m_edge_maps) {
				m->add_index();
			}
		} else {
			m_graph.m_edges.push_back(m_graph.m_edges[index]);
			m_graph.m_edges.back()->m_index = m_graph.m_edges.size() - 1;
			m_graph.m_edges[index] = m_edge;
			for (Graph_map_base* m : m_graph.m_edge_maps) {
				m->add_index(index);
			}
		}

		m_edge->m_source->m_incident.push_back(m_edge);
		m_edge->m_target->m_incident.push_back(m_edge);

		m_graph.ensure_traits(m_edge->source());
		m_graph.ensure_traits(m_edge->target());
	}
	void redo() override {
		remove_edge(m_graph, m_edge);
	}
};

template <class G> class ChangeCurve : public Operation {
  private:
	using Curve_traits = typename G::Curve_traits;
	using Curve_representation = Curve_traits::Curve_representation_2;
	using Curve = Curve_traits::Curve_2;

	G::Edge_handle m_edge;
	Curve_representation m_representation;

  public:
	ChangeCurve(G::Edge_handle e, Curve_representation old_representation)
	    : m_edge(e), m_representation(std::move(old_representation)) {}

	~ChangeCurve() {}

	void undo() override {
		std::swap(m_edge->m_representation, m_representation);
	}

	void redo() override {
		std::swap(m_edge->m_representation, m_representation);
	}
};

template <class G> class MoveVertex : public Operation {
  private:
	using Point = typename G::Point_2;

	G::Vertex_handle m_vertex;
	Point m_point;

  public:
	MoveVertex(G::Vertex_handle v, Point old_point) : 
		m_vertex(v), m_point(std::move(old_point)) {}
	~MoveVertex() {}

	void undo() override {
		std::swap(m_point, m_vertex->m_point);
	}
	void redo() override {
		std::swap(m_point, m_vertex->m_point);
	}
};
} // namespace cartocrow