#pragma once

#include <vector>

namespace cartocrow::datastructures {

	template <class QT> concept QueueTraits = requires(typename QT::Element* elt, int i) {
		typename QT::Element;

		{ QT::setIndex(elt, i) };

		{
			QT::getIndex(elt)
		} -> std::same_as<int>;

		{
			QT::compare(elt, elt)
		} -> std::same_as<int>; // negative if elt < elt2, positive if elt > elt2, zero if elt = elt2. Smallest value == highest priority (top of queue)
	};

	template <QueueTraits QT> class IndexedPriorityQueue {
	public:
		using Element = QT::Element;

	private:
		std::vector<Element*> queue;

		void siftUp(int k, Element* elt);
		void siftDown(int k, Element* elt);

	public:
		bool empty();

		void push(Element* elt);
		Element* pop();
		Element* peek();

		bool remove(Element* elt);
		bool contains(Element* elt);
		void update(Element* elt);

		void clear();
	};

} // namespace cartocrow::datastructures

#include "indexed_priority_queue.hpp"