// -----------------------------------------------------------------------------
// IMPLEMENTATION OF TEMPLATE FUNCTIONS
// Do not include this file, but the .h file instead
// -----------------------------------------------------------------------------
 
#include <cassert>

namespace cartocrow::datastructures {

	template <QueueTraits QT>
	void IndexedPriorityQueue<QT>::siftUp(int k, Element* elt) {
		while (k > 0) {
			int parent = (k - 1) >> 1;
			Element* e = queue[parent];
			if (QT::compare(elt, e) >= 0) {
				break;
			}
			queue[k] = e;
			QT::setIndex(e, k);
			k = parent;
		}
		queue[k] = elt;
		QT::setIndex(elt, k);
	}

	template <QueueTraits QT>
	void IndexedPriorityQueue<QT>::siftDown(int k, Element* elt) {
		int half = queue.size() >> 1;
		while (k < half) {
			int child = (k << 1) + 1;
			Element* c = queue[child];
			int right = child + 1;
			if (right < queue.size() && QT::compare(c, queue[right]) > 0) {
				c = queue[child = right];
			}
			if (QT::compare(elt, c) <= 0) {
				break;
			}
			queue[k] = c;
			QT::setIndex(c, k);
			k = child;
		}
		queue[k] = elt;
		QT::setIndex(elt, k);
	}

	template <QueueTraits QT>
	bool IndexedPriorityQueue<QT>::empty() {
		return queue.empty();
	}

	template <QueueTraits QT>
	void IndexedPriorityQueue<QT>::push(Element* elt) {
		assert(!contains(elt));
		assert(std::find(queue.begin(), queue.end(), elt) == queue.end());

		queue.push_back(elt);
		siftUp(queue.size() - 1, elt);
	}

	template <QueueTraits QT>
	QT::Element* IndexedPriorityQueue<QT>::pop() {
		if (queue.empty()) {
			return nullptr;
		}
		Element* result = queue[0];
		QT::setIndex(result, -1);

		Element* last = queue[queue.size() - 1];
		queue.pop_back();
		if (!queue.empty()) {
			siftDown(0, last);
		}

		assert(!contains(result));
		assert(std::find(queue.begin(), queue.end(), result) == queue.end());

		return result;
	}

	template <QueueTraits QT>
	QT::Element* IndexedPriorityQueue<QT>::peek() {
		if (queue.empty()) {
			return nullptr;
		}
		return queue[0];
	}

	template <QueueTraits QT>
	bool IndexedPriorityQueue<QT>::remove(Element* elt) {
		int id = QT::getIndex(elt);
		if (id < 0 || id >= queue.size() || queue[id] != elt) {
			assert(!contains(elt));
			assert(std::find(queue.begin(), queue.end(), elt) == queue.end());
			return false;
		}
		else {
			QT::setIndex(elt, -1);
			if (id == queue.size() - 1) {
				queue.pop_back();
			}
			else {
				Element* moved = queue[queue.size() - 1];
				queue.pop_back();
				siftDown(id, moved);
				if (queue[id] == moved) {
					siftDown(id, moved);
				}
			}

			assert(!contains(elt));
			assert(std::find(queue.begin(), queue.end(), elt) == queue.end());
			return true;
		}
	}

	template <QueueTraits QT>
	bool IndexedPriorityQueue<QT>::contains(Element* elt) {
		int id = QT::getIndex(elt);
		if (id < 0 || id >= queue.size()) {
			return false;
		}
		else {
			return queue[id] == elt;
		}
	}

	template <QueueTraits QT>
	void IndexedPriorityQueue<QT>::update(Element* elt) {
		assert(contains(elt));
		assert(std::find(queue.begin(), queue.end(), elt) != queue.end());

		siftUp(QT::getIndex(elt), elt);
		siftDown(QT::getIndex(elt), elt);
	}

	template <QueueTraits QT>
	void IndexedPriorityQueue<QT>::clear() {
		for (Element* elt : queue) {
			QT::setIndex(elt, -1);
		}
		queue.clear();
	}
} // namespace cartocrow::datastructures