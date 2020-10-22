#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
#include <range/v3/algorithm/permutation.hpp>

TEST_CASE("Revised") {
	auto g4 = gdwg::graph<std::string, int>{};
	g4.insert_node("A");
	g4.insert_node("B");
	g4.insert_node("C");
	g4.insert_node("D");
	SECTION("const graph") {
		static_assert(ranges::weakly_incrementable<gdwg::graph<std::string, int>::iterator>);
		static_assert(ranges::indirectly_readable<gdwg::graph<std::string, int>::iterator>);
		static_assert(ranges::input_iterator<gdwg::graph<std::string, int>::iterator>);
		static_assert(ranges::forward_iterator<gdwg::graph<std::string, int>::iterator>);
		static_assert(ranges::forward_range<gdwg::graph<std::string, int>>);
		static_assert(ranges::forward_range<gdwg::graph<std::string, int> const>);
		static_assert(ranges::bidirectional_iterator<gdwg::graph<std::string, int>::iterator>);
		static_assert(ranges::bidirectional_range<gdwg::graph<std::string, int>>);

		REQUIRE(ranges::is_permutation(g4.nodes(), std::array{"A", "B", "C", "D"}));

		g4.insert_edge("A", "C", 15);
		g4.insert_edge("B", "C", 12);
		g4.insert_edge("B", "C", 13);
		g4.insert_edge("B", "C", 11);
		g4.erase_edge("A", "C", 15);
		CHECK(g4.erase_edge("B", "C", 12) == true);
		CHECK(g4.erase_edge("B", "C", 12) == false);

		REQUIRE(ranges::is_permutation(g4.nodes(), std::array{"A", "B", "C", "D"}));
	}

	SECTION("Move Constructor/Assignment") {
		auto edge_vector = std::vector<gdwg::graph<std::string, int>::value_type>{
		   gdwg::graph<std::string, int>::value_type{"A", "B", 30},
		   gdwg::graph<std::string, int>::value_type{"A", "B", 40},
		   gdwg::graph<std::string, int>::value_type{"B", "C", 15},
		   gdwg::graph<std::string, int>::value_type{"C", "D", 20}};
		auto graph = gdwg::graph<std::string, int>(edge_vector.begin(), edge_vector.end());
		auto graph2 = graph;
		auto graph3 = gdwg::graph(std::move(graph2));

		CHECK(graph == graph3);
		REQUIRE(ranges::is_permutation(graph3.nodes(), std::array{"A", "B", "C", "D"}));
		CHECK(graph3.is_connected("A", "B") == true);
		CHECK(graph3.is_connected("B", "C") == true);
		CHECK(graph3.is_connected("C", "D") == true);
		CHECK(graph3.is_connected("A", "D") == false);
		graph3.insert_edge("A", "D", 60);
		CHECK(std::as_const(graph3).connections("A") == std::vector<std::string>{"B", "D"});
	}
}