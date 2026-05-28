#pragma once

#include <concepts>

namespace cartocrow {

template <class T> concept GraphTraits_2 = requires {
	T::historic;
	requires std::is_same_v<bool const, decltype(T::historic)>;

	T::oriented;
	requires std::is_same_v<bool const, decltype(T::oriented)>;

	T::sorted;
	requires std::is_same_v<bool const, decltype(T::sorted)>;
};

struct SimpleGraph {
	static constexpr bool historic = false;
	static constexpr bool oriented = true;
	static constexpr bool sorted = true;
};
static_assert(GraphTraits_2<SimpleGraph>);

struct HistoricSimpleGraph {
	static constexpr bool historic = true;
	static constexpr bool oriented = true;
	static constexpr bool sorted = true;
};
static_assert(GraphTraits_2<HistoricSimpleGraph>);

} // namespace cartocrow
