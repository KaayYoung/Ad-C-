#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <__string>
#include <absl/container/flat_hash_set.h>
#include <algorithm>
#include <concepts/concepts.hpp>
#include <cstdarg>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <range/v3/algorithm.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/iterator.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/range.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/utility.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/common_tuple.hpp>
#include <range/v3/view.hpp>
#include <stdexcept>
#include <tuple>

namespace gdwg {
	template<concepts::regular N, concepts::regular E>
	requires concepts::totally_ordered<N> //
	   and concepts::totally_ordered<E> //
	   class graph {
	public:
		struct edge {
			std::shared_ptr<N> from;
			std::shared_ptr<N> to;
			std::unique_ptr<E> weight;
		};

		struct node_comparator {
			using is_transparent = void;
			auto operator()(std::shared_ptr<N> const& a, std::shared_ptr<N> const& b) const noexcept
			   -> bool {
				return *a < *b;
			}
			auto operator()(std::shared_ptr<N> const& a, N const& b) const noexcept -> bool {
				return *a < b;
			}
			auto operator()(N const& a, std::shared_ptr<N> const& b) const noexcept -> bool {
				return a < *b;
			}
		};
		// Compare different types of elements
		struct edge_comparator {
			using is_transparent = void;
			auto operator()(edge const& a, edge const& b) const noexcept -> bool {
				if (*a.from != *b.from) {
					return *a.from < *b.from;
				}
				if (*a.to != *b.to) {
					return *a.to < *b.to;
				}
				return *a.weight < *b.weight;
			}
			// Used in set.find(dst, src, weight)
			auto operator()(edge const& a, ranges::common_tuple<N, N, E> const& b) const noexcept
			   -> bool {
				if (*a.from != ranges::get<0>(b)) {
					return *a.from < ranges::get<0>(b);
				}
				if (*a.to != ranges::get<1>(b)) {
					return *a.to < ranges::get<1>(b);
				}
				return *a.weight < ranges::get<2>(b);
			}
			auto operator()(ranges::common_tuple<N, N, E> const& a, edge const& b) const noexcept
			   -> bool {
				if (ranges::get<0>(a) != *b.from) {
					return ranges::get<0>(a) < *b.from;
				}
				if (ranges::get<1>(a) != *b.to) {
					return ranges::get<1>(a) < *b.to;
				}
				return ranges::get<2>(a) < *b.weight;
			}
			// Used in set.find(src, dst) pair
			auto operator()(edge const& a, ranges::common_tuple<N, N> const& b) const noexcept -> bool {
				if (*a.from != ranges::get<0>(b)) {
					return *a.from < ranges::get<0>(b);
				}
				return *a.to < ranges::get<1>(b);
			}
			auto operator()(ranges::common_tuple<N, N> const& a, edge const& b) const noexcept -> bool {
				if (ranges::get<0>(a) != *b.from) {
					return ranges::get<0>(a) < *b.from;
				}
				return ranges::get<1>(a) < *b.to;
			}
			// Used in set.find(src)
			auto operator()(edge const& a, N const& b) const noexcept -> bool {
				return *a.from < b;
			}
			auto operator()(N const& a, edge const& b) const noexcept -> bool {
				return a < *b.from;
			}
		};
		class iterator {
			using nodeset = std::set<std::shared_ptr<N>, node_comparator>;
			using edgeset = std::set<edge, edge_comparator>;

			// Iterator constructor
		public:
			using value_type = ranges::common_tuple<N, N, E>;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			iterator() = default;
			auto operator*() const -> ranges::common_tuple<N const&, N const&, E const&> {
				return std::tie(*((*pointee_).from), *((*pointee_).to), *((*pointee_).weight));
			}

			auto operator*() -> ranges::common_tuple<N const&, N const&, E const&> {
				return std::tie(*((*pointee_).from), *((*pointee_).to), *((*pointee_).weight));
			}

			// Iterator traversal
			// return itself
			auto operator++() -> iterator& {
				++pointee_;
				return *this;
			}
			// make a copy, increment and return the copy
			auto operator++(int) -> iterator {
				auto copy = *this;
				++*this;
				return copy;
			}
			auto operator--() -> iterator& {
				--pointee_;
				return *this;
			}
			auto operator--(int) -> iterator {
				auto copy = *this;
				--*this;
				return copy;
			}

			// Iterator comparison
			auto operator==(iterator const& other) const -> bool = default;

		private:
			typename edgeset::iterator pointee_;
			friend class graph<N, E>;
			explicit iterator(typename edgeset::iterator ptr)
			: pointee_(ptr){};
		};

		struct value_type {
			N from;
			N to;
			E weight;
		};

		// 2.2 Constructors
		graph() = default;

		graph(std::initializer_list<N> il) noexcept
		: graph(il.begin(), il.end()){};

		template<ranges::forward_iterator I, ranges::sentinel_for<I> S>
		requires ranges::indirectly_copyable<I, N*> graph(I first, S last) {
			for (auto it = first; it != last; ++it) {
				nodes_.insert(std::make_shared<N>(*it));
			}
		}

		template<ranges::forward_iterator I, ranges::sentinel_for<I> S>
		requires ranges::indirectly_copyable<I, value_type*> graph(I first, S last) noexcept {
			ranges::for_each(first, last, [this](auto const& x) {
				insert_node(x.from);
				insert_node(x.to);
				edges_.insert({std::make_shared<N>(x.from),
				               std::make_shared<N>(x.to),
				               std::make_unique<E>(x.weight)});
			});
		}

		graph(graph&& other) noexcept
		: nodes_{std::exchange(other.nodes_, std::set<std::shared_ptr<N>, node_comparator>{})}
		, edges_{std::exchange(other.edges_, std::set<edge, edge_comparator>{})} {};

		auto operator=(graph&& other) noexcept -> graph& {
			if (this == &other) {
				return *this;
			}
			nodes_ = std::move(other.nodes_);
			edges_ = std::move(other.edges_);
			return *this;
		}

		graph(graph const& other) noexcept
		: nodes_(std::set<std::shared_ptr<N>, node_comparator>())
		, edges_(std::set<edge, edge_comparator>()) {
			ranges::for_each(ranges::begin(other.nodes_),
			                 ranges::end(other.nodes_),
			                 [this](auto const& x) { nodes_.emplace(std::make_shared<N>(*x)); });
			ranges::for_each(ranges::begin(other.edges_),
			                 ranges::end(other.edges_),
			                 [this](auto const& x) {
				                 edges_.insert({std::make_shared<N>(*x.from),
				                                std::make_shared<N>(*x.to),
				                                std::make_unique<E>(*x.weight)});
			                 });
		}

		// Postconditions: *this == other is true;
		auto operator=(graph const& other) noexcept -> graph& {
			graph(other).swap(*this);
			return *this;
		} // Postconditions: *this == other is true

		// 2.3 Modifiers
		auto insert_node(N const& value) noexcept -> bool {
			if (!is_node(value)) {
				auto cur = std::make_shared<N>(value);
				nodes_.emplace(cur);
				return true;
			}
			return false;
		}
		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either "
				                         "src "
				                         "or dst node does not exist");
			}
			auto found =
			   ranges::find_if(edges_.begin(), ranges::end(edges_), [&src, &dst, &weight](const edge& x) {
				   return *x.from == src && *x.to == dst && *x.weight == weight;
			   });
			if (found == edges_.end()) {
				edges_.insert(
				   {std::make_shared<N>(src), std::make_shared<N>(dst), std::make_unique<E>(weight)});
				return true;
			}
			return false;
		}
		auto replace_node(N const& old_data, N const& new_data) -> bool {
			if (!is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node "
				                         "that "
				                         "doesn't exist");
			}
			if (is_node(new_data)) {
				return false;
			}

			// Add new node first
			nodes_.emplace(std::make_shared<N>(new_data));

			// Iterate over edge set and each time, add the new edge, delete the old edge
			auto l = edges_.begin();
			for (; l != edges_.end();) {
				if (*l->from == old_data) {
					edges_.insert({std::make_shared<N>(new_data),
					               std::make_shared<N>(*l->to),
					               std::make_unique<E>(*l->weight)});
					l = edges_.erase(l);
				}
				else if (*l->to == old_data) {
					edges_.insert({std::make_shared<N>(*l->from),
					               std::make_shared<N>(new_data),
					               std::make_unique<E>(*l->weight)});
					l = edges_.erase(l);
				}
				else if (*l->from == old_data && *l->to == old_data) {
					edges_.insert({std::make_shared<N>(new_data),
					               std::make_shared<N>(new_data),
					               std::make_unique<E>(*l->weight)});
					l = edges_.erase(l);
				}
				else {
					++l;
				}
			}

			auto found = nodes_.find(old_data);
			nodes_.erase(found);

			return true;
		}
		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (!is_node(old_data) || !is_node(new_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old "
				                         "or "
				                         "new data if they don't exist in the graph");
			}

			for (auto l = edges_.begin(); l != edges_.end();) {
				if (*l->from == old_data) {
					edges_.insert({std::make_shared<N>(new_data),
					               std::make_shared<N>(*l->to),
					               std::make_unique<E>(*l->weight)});
					l = edges_.erase(l);
				}
				else if (*l->to == old_data) {
					edges_.insert({std::make_shared<N>(*l->from),
					               std::make_shared<N>(new_data),
					               std::make_unique<E>(*l->weight)});
					l = edges_.erase(l);
				}
				else if (*l->from == old_data && *l->to == old_data) {
					edges_.insert({std::make_shared<N>(new_data),
					               std::make_shared<N>(new_data),
					               std::make_unique<E>(*l->weight)});
					l = edges_.erase(l);
				}
				else {
					++l;
				}
			}
			auto found = nodes_.find(old_data);
			nodes_.erase(found);
		}
		auto erase_node(N const& value) -> bool {
			if (!is_node(value)) {
				return false;
			}

			// Remove all related edges
			for (auto l = edges_.begin(); l != edges_.end();) {
				if (*l->from == value || *l->to == value) {
					l = edges_.erase(l);
				}
				else {
					l++;
				}
			}
			auto found = nodes_.find(value);
			nodes_.erase(found);
			return true;
		}

		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst "
				                         "if "
				                         "they don't exist in the graph");
			}
			// O(log(n) + e)
			auto found = edges_.find(ranges::common_tuple<N, N, E>(src, dst, weight));
			if (found != edges_.end()) {
				edges_.erase(found);
				return true;
			}
			return false;
		}

		auto erase_edge(iterator i) -> iterator {
			auto it1 = i.pointee_;
			it1 = edges_.erase(it1);
			return iterator(it1);
		}

		// Complexity O(d) where d = ranges::distance(i, s)
		auto erase_edge(iterator i, iterator s) -> iterator {
			auto it1 = i.pointee_;
			auto it2 = s.pointee_;
			for (; it1 != it2;) {
				it1 = edges_.erase(it1);
			}
			return iterator(it1);
		}

		// Clear nodes and edges
		auto clear() noexcept -> void {
			nodes_.clear();
			edges_.clear();
		}

		// 2.4 Accessors
		[[nodiscard]] auto is_node(N const& value) const noexcept -> bool {
			auto found = nodes_.find(value);
			return found != nodes_.end();
		}
		[[nodiscard]] auto empty() const noexcept -> bool {
			return nodes_.empty();
		}
		[[nodiscard]] auto is_connected(N const& src, N const& dst) const -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst "
				                         "node don't exist in the graph");
			}

			auto found =
			   ranges::find_if(ranges::begin(edges_), ranges::end(edges_), [&src, &dst](const edge& x) {
				   return *x.from == src && *x.to == dst;
			   });
			return found != ranges::end(edges_);
		}
		[[nodiscard]] auto nodes() const noexcept -> std::vector<N> {
			auto all_nodes = std::vector<N>();
			ranges::for_each(ranges::begin(nodes_), ranges::end(nodes_), [&all_nodes](auto const& x) {
				all_nodes.push_back(*x);
			});
			return all_nodes;
		}

		// Complexity: log(n) + e
		[[nodiscard]] auto weights(N const& src, N const& dst) const -> std::vector<E> {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node "
				                         "don't exist in the graph");
			}
			auto result = std::vector<E>();
			auto found = edges_.find(ranges::common_tuple<N, N>(src, dst)); // O(log(n))

			// O(e)
			for (auto it = found; it != edges_.end() && *it->from == src && *it->to == dst; ++it) {
				result.push_back(*it->weight);
			}
			return result;
		};

		// Complexity: log(n) + log(e)
		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) const -> iterator {
			auto it = edges_.find(ranges::common_tuple<N, N, E>(src, dst, weight));
			return iterator(it);
		}

		// Complexity: log(n) + e
		[[nodiscard]] auto connections(N const& src) const -> std::vector<N> {
			auto result = std::vector<N>();

			// O(log(n))
			auto found = edges_.find(src);
			// O(e), e is the number of edges associated with src
			for (auto it = found; it != edges_.end() && *it->from == src; ++it) {
				auto f = ranges::find(result.begin(), result.end(), *it->to);
				if (f == result.end()) {
					result.push_back(*it->to);
				}
			}
			return result;
		}

		// 2.5 Range access
		[[nodiscard]] auto begin() const -> iterator {
			return iterator(edges_.begin());
		}
		[[nodiscard]] auto end() const -> iterator {
			return iterator(edges_.end());
		}

		// 2.6 Comparisons
		[[nodiscard]] auto operator==(graph const& other) const -> bool {
			auto nodes_equal = ranges::equal(nodes_, other.nodes_, [](auto const& a, auto const& b) {
				return *a == *b;
			});
			if (nodes_equal == true) {
				return ranges::equal(edges_, other.edges_, [](auto const& a, auto const& b) {
					return *a.from == *b.from && *a.to == *b.to && *a.weight == *b.weight;
				});
			}
			return false;
		}

		// 2.7 Extractor
		friend auto operator<<(std::ostream& os, graph const& g) -> std::ostream& {
			for (auto const& x : g.nodes_) {
				os << *x << " (\n";
				auto all_target =
				   g.edges_ | ranges::views::filter([&x](edge const& e) { return *e.from == *x; });
				for (auto const& e : all_target) {
					os << "  " << *e.to << " | " << *e.weight << "\n";
				}
				os << ")\n";
			}
			return os;
		}

		auto swap(graph& g) {
			ranges::swap(nodes_, g.nodes_);
			ranges::swap(edges_, g.edges_);
		}

	private:
		std::set<std::shared_ptr<N>, node_comparator> nodes_{};
		std::set<edge, edge_comparator> edges_{};
	};

} // namespace gdwg

#endif // GDWG_GRAPH_HPP
