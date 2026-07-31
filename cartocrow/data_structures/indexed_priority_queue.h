#pragma once

#include <vector>

namespace cartocrow::data_structures {

	template <class QT> concept QueueTraits = requires(typename QT::Element_handle elt, int i) {
		typename QT::Element_handle;

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
		using Element_handle = QT::Element_handle;

	private:
		std::vector<Element_handle> queue;

		void siftUp(int k, Element_handle elt);
		void siftDown(int k, Element_handle elt);

	public:
		bool empty() const;

		void push(Element_handle elt);
		Element_handle pop();
		Element_handle peek() const;

		bool remove(Element_handle elt);
		bool contains(Element_handle elt) const;
		void update(Element_handle elt);

		void clear();

		const std::vector<Element_handle>& content() const;
	};

} // namespace cartocrow::data_structures

#include "indexed_priority_queue.hpp"