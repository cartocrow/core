#pragma once

#include <cartocrow/core/core.h>

namespace cartocrow::datastructures {

	template <typename QT> concept QuadTreeTraits = requires(typename QT::Element& elt, Rectangle<typename QT::Kernel>& rect) {

		typename QT::Element;
		typename QT::Kernel;

		{
			QT::get_bounding_box(elt)
		} -> std::convertible_to<Rectangle<typename QT::Kernel>>;

		{
			QT::element_overlaps_rectangle(elt,rect)
		} -> std::convertible_to<bool>;
	};

	namespace detail {
		template <QuadTreeTraits QT> 
		class QTNode;
	}

	template <QuadTreeTraits QT> class QuadTree {
	public:
		using Kernel = QT::Kernel;
		using Element = QT::Element;
		using ElementCallback = std::function<void(Element&)>;

		QuadTree(Rectangle<Kernel>& box, int depth, Number<Kernel> fuzz);
		~QuadTree();

		void clear();
		void insert(Element& elt);
		bool remove(Element& elt);

		void findOverlapped(Rectangle<Kernel>& query, ElementCallback act);

	private:
		using Node = detail::QTNode<QT>;

		Node* root;
		int maxdepth;
		Number<Kernel> fuzziness;

		// Does the rectangle enclose the (possibly infinite) node?
		bool encloses(Rectangle<Kernel>& rect, Node* node);

		// Is the (possibly infinite) node disjoint from the rectangle
		bool disjoint(Node* node, Rectangle<Kernel>& rect);

		void findOverlappedRecursive(Node* n, Rectangle<Kernel>& query, ElementCallback act);

		template <bool extend> Node* find(Element& elt);
	};

} // namespace cartocrow::datastructures

#include "quad_tree.hpp"