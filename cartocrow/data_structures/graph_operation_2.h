#pragma once

namespace cartocrow {

class Operation {

  protected:
	bool m_past = true;
	virtual void undo_internal() = 0;
	virtual void redo_internal() = 0;

  public:
	void undo() {
		m_past = false;
		undo_internal();
	}
	void redo() {
		m_past = true;
		redo_internal();
	}
};

class OperationGroup {
  private:
	std::vector<Operation*> ops;
  public:
	~OperationGroup() {
		for (Operation* op : ops) {
			delete op;
		}
	}

	void add_operation(Operation* op) {
		ops.push_back(op);
	}

	void undo() {
		for (auto it = ops.rbegin(); it != ops.rend(); ++it) {			
			(*it)->undo();
		}
	}

	void redo() {
		for (Operation* op : ops) {
			op->redo();
		}
	}
};

template <class G> class AddVertex : public Operation {
  private:
	G& m_graph;
	G::Vertex_handle m_vertex;

  public:
	AddVertex(G& graph, G::Vertex_handle v) : m_graph(graph), m_vertex(v) {}
	~AddVertex() {
		if (!m_past) {
			// this vertex is added in the future, delete it
			delete m_vertex;
		}
	}

	void undo_internal() override {
		m_graph.m_vertices.pop_back();
	}
	void redo_internal() override {
		m_graph.m_vertices.push_back(m_vertex);
	}
};

template <class G> class RemoveVertex : public Operation {
  private:
	G& m_graph;
	G::Vertex_handle m_vertex;

  public:
	RemoveVertex(G& graph, G::Vertex_handle v) : m_graph(graph), m_vertex(v) {}

	~RemoveVertex() {
		if (m_past) {
			// this vertex is removed in the past, delete it
			delete m_vertex;
		}
	}

	void undo_internal() override {
		const size_t index = m_vertex->graph_index();
		if (index == m_graph.m_vertices.size()) {
			m_graph.m_vertices.push_back(m_vertex);
		} else {
			m_graph.m_vertices.push_back(m_graph.m_vertices[index]);
			m_graph.m_vertices[index] = m_vertex;
		}
	}
	void redo_internal() override {
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
	~AddEdge() {
		if (!m_past) {
			// this vertex is added in the future, delete it
			delete m_edge;
		}
	}

	void undo_internal() override {
		m_graph.m_edges.pop_back();

		m_edge->m_source->remove_incident(m_edge);
		m_edge->m_target->remove_incident(m_edge);

		m_graph.ensure_traits(m_edge->m_source);
		m_graph.ensure_traits(m_edge->m_target);
	}
	void redo_internal() override {
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

	~RemoveEdge() {
		if (m_past) {
			// this vertex is removed in the past, delete it
			delete m_edge;
		}
	}

	void undo_internal() override {
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
	void redo_internal() override {
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