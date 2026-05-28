#pragma once

namespace cartocrow {

class Operation {

  public:
	virtual void undo() = 0;
	virtual void redo() = 0;
	virtual void forget_past() {}
	virtual void forget_future() {}
};

class OperationGroup {
  private:
	std::list<std::unique_ptr<Operation>> ops;

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
		while (m_curr != m_groups.end()) {
			m_groups.front().forget_future();
			m_groups.pop_back();
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

	void undo() override {
		m_graph.m_vertices.pop_back();
	}
	void redo() override {
		m_graph.m_vertices.push_back(m_vertex);
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

	void undo() override {
		const size_t index = m_vertex->graph_index();
		if (index == m_graph.m_vertices.size()) {
			m_graph.m_vertices.push_back(m_vertex);
		} else {
			m_graph.m_vertices.push_back(m_graph.m_vertices[index]);
			m_graph.m_vertices[index] = m_vertex;
		}
	}
	void redo() override {
		const size_t index = m_vertex->graph_index();
		const size_t last = m_graph.m_vertices.size() - 1;
		if (index != last) {
			m_graph.m_vertices[index] = m_graph.m_vertices[last];
		}
		m_graph.m_vertices.pop_back();
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

	void undo() override {
		m_graph.m_edges.pop_back();

		m_edge->m_source->remove_incident(m_edge);
		m_edge->m_target->remove_incident(m_edge);

		m_graph.ensure_traits(m_edge->m_source);
		m_graph.ensure_traits(m_edge->m_target);
	}
	void redo() override {
		m_graph.m_edges.push_back(m_edge);

		m_edge->m_source->m_incident.push_back(m_edge);
		m_edge->m_target->m_incident.push_back(m_edge);

		m_graph.ensure_traits(m_edge->source());
		m_graph.ensure_traits(m_edge->target());
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

	void undo() override {
		const size_t index = m_edge->graph_index();
		if (index == m_graph.m_edges.size()) {
			m_graph.m_edges.push_back(m_edge);
		} else {
			m_graph.m_edges.push_back(m_graph.m_edges[index]);
			m_graph.m_edges[index] = m_edge;
		}

		m_edge->m_source->m_incident.push_back(m_edge);
		m_edge->m_target->m_incident.push_back(m_edge);

		m_graph.ensure_traits(m_edge->source());
		m_graph.ensure_traits(m_edge->target());
	}
	void redo() override {
		const size_t index = m_edge->graph_index();
		const size_t last = m_graph.m_edges.size() - 1;
		if (index != last) {
			m_graph.m_edges[index] = m_graph.m_edges[last];
		}
		m_graph.m_edges.pop_back();

		m_edge->m_source->remove_incident(m_edge);
		m_edge->m_target->remove_incident(m_edge);

		m_graph.ensure_traits(m_edge->m_source);
		m_graph.ensure_traits(m_edge->m_target);
	}
};

} // namespace cartocrow