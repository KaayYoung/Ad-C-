#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
#include <sstream>

// Testing rationale comment //
// graph_test6 is used to test 2.6 Comparisons and 2.7 Extractor
// The method I test Comparisons is check equality when
// 1. start with only few nodes, no edges;
// 2. few nodes, add some edges;
// 3. do some other operations(erase, insert, replace, merge_replace)
// The method I test 2.7 Extractor is
// 1. no node
// 2. only nodes
// 3. nodes with edges, then add/erase nodes or edges and test

TEST_CASE("2.6 Comparions && 2.7 Extractor") {
	auto g1 = gdwg::graph<int, std::string>{};
	auto g2 = gdwg::graph<std::string, int>{};
	auto g3 = gdwg::graph<std::string, int>{};
	SECTION("2.6 Comparisons") {
		// Compare type
		{
			g1.insert_node(1);
			g2.insert_node("a");
			// CHECK((g1 == g2) == false);
		}

		// Only nodes in the two graphs
		{
			g2.insert_node("b");
			g2.insert_node("c");
			g2.insert_node("d");

			g3.insert_node("a");
			g3.insert_node("b");
			g3.insert_node("c");
			g3.insert_node("d");

			CHECK((g2 == g3) == true);
		}

		// add edges
		{
			g2.insert_edge("a", "b", 1);
			g2.insert_edge("a", "c", 2);
			g2.insert_edge("b", "c", 3);

			g3.insert_edge("a", "b", 1);
			g3.insert_edge("a", "c", 2);
			g3.insert_edge("b", "c", 3);
			CHECK((g2 == g3) == true);
		}

		// return comparison result is false
		{
			g2.insert_edge("a", "d", 5);
			CHECK((g2 == g3) == false);
		}

		// Harder test
		{
			g3.insert_edge("a", "d", 5);
			CHECK((g2 == g3) == true);
			g2.erase_edge("a", "b", 1);
			CHECK((g2 == g3) == false);
			g3.erase_edge("a", "b", 1);
			CHECK((g2 == g3) == true);
			g2.insert_edge("c", "c", 5);
			CHECK((g2 == g3) == false);
			g3.insert_edge("c", "c", 4);
			CHECK((g2 == g3) == false);
			g3.erase_edge("c", "c", 4);
			g3.insert_edge("c", "c", 5);
			CHECK((g2 == g3) == true);
			g2.replace_node("a", "f");
			CHECK((g2 == g3) == false);
			g3.replace_node("a", "f");
			CHECK((g2 == g3) == true);
			g2.merge_replace_node("f", "b");
			g3.merge_replace_node("f", "b");
			CHECK((g2 == g3) == true);
		}
	}
	SECTION("2.7 Extractor") {
		auto out = std::ostringstream{};
		out << g3;
		auto expected_output = std::string_view(R"()");
		CHECK(out.str() == expected_output);

		// Only nodes
		{
			g3.insert_node("a");
			g3.insert_node("b");
			g3.insert_node("c");
			g3.insert_node("d");
			expected_output = std::string_view(R"(a (
)
b (
)
c (
)
d (
)
)");
			out = std::ostringstream{};
			out << g3;
			CHECK(out.str() == expected_output);
		}

		// add edges
		{
			g3.insert_edge("a", "b", 1);
			g3.insert_edge("a", "a", 3);
			g3.insert_edge("a", "c", 3);
			g3.insert_edge("b", "a", 5);
			g3.insert_edge("c", "d", 2);
			g3.insert_edge("c", "a", 7);
			g3.insert_edge("d", "a", 3);
			expected_output = std::string_view(R"(a (
  a | 3
  b | 1
  c | 3
)
b (
  a | 5
)
c (
  a | 7
  d | 2
)
d (
  a | 3
)
)");
			out = std::ostringstream{};
			out << g3;
			CHECK(out.str() == expected_output);
		}

		// insert one node
		{
			g3.insert_node("f");
			expected_output = std::string_view(R"(a (
  a | 3
  b | 1
  c | 3
)
b (
  a | 5
)
c (
  a | 7
  d | 2
)
d (
  a | 3
)
f (
)
)");
			out = std::ostringstream{};
			out << g3;
			CHECK(out.str() == expected_output);
		}

		// erase node and insert one edge
		{
			g3.erase_node("a");
			g3.insert_edge("d", "f", 7);
			expected_output = std::string_view(R"(b (
)
c (
  d | 2
)
d (
  f | 7
)
f (
)
)");
			out = std::ostringstream{};
			out << g3;
			CHECK(out.str() == expected_output);
		}
	}
}