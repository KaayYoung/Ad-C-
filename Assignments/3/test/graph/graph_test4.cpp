#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>

// Testing rationale comment //
// graph_test4.cpp is used to test the section "modifiers"
// where for each section, I will test whether my program can
// correctly handle one basic case, the normal case and
// the case which may occur exception message, and
// combining with "Accessors" to test more harder cases
// No test for complexity here, I write comments for functions' complexity in .hpp

TEST_CASE("2.3 Modifiers") {
	auto g1 = gdwg::graph<std::string, int>{};
	auto g2 = gdwg::graph<std::string, int>{};
	g2.insert_node("a");
	g2.insert_node("b");
	g2.insert_node("c");
	{
		auto w1 = std::string{"hello"};
		g1.insert_node(w1);
		g1.insert_node("how");
		g1.insert_node("are");
		g1.insert_node("you?");
	}

	// g4 is used to test erase with iterator
	auto g4 = gdwg::graph<std::string, int>{};
	g4.insert_node("a");
	g4.insert_node("b");
	g4.insert_node("c");
	g4.insert_edge("a", "c", 1);
	g4.insert_edge("a", "b", 10);

	SECTION("insert_node") {
		CHECK(g1.insert_node("z") == true);
		CHECK(g1.insert_node("x") == true);
		CHECK(g1.insert_node("!") == true);
	}
	SECTION("insert_edge") {
		g1.insert_edge("hello", "how", 5);

		// Test inserting one same edge
		CHECK(g1.insert_edge("hello", "how", 5) == false);

		// Test insertion exception
		CHECK_THROWS_WITH(g1.insert_edge("edf", "qqq", 1),
		                  "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node "
		                  "does not exist");

		// Insert a new edge and one more
		g1.insert_edge("hello", "are", 2);
		auto same_src_dst = g1.insert_edge("hello", "are", 8); // Insert edge with same src and dst
		CHECK(same_src_dst == true);
	}
	SECTION("replace node") {
		// Test basic case
		g2.replace_node("a", "e");
		CHECK(g2.is_node("a") == false);
		CHECK(g2.is_node("e") == true);

		// Test the incoming edge
		g2.insert_edge("b", "e", 2);

		g2.insert_edge("b", "e", 3);
		g2.insert_edge("c", "e", 5);
		g2.replace_node("e", "a");
		CHECK(g2.is_node("a") == true);
		CHECK(g2.is_node("e") == false);
		CHECK(g2.is_connected("b", "a") == true);
		CHECK(g2.is_connected("c", "a") == true);
		CHECK_THROWS_WITH(g2.is_connected("c", "e"),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");

		// Test exception
		CHECK_THROWS_WITH(g2.replace_node("abc", "edf"),
		                  "Cannot call gdwg::graph<N, E>::replace_node on a node "
		                  "that doesn't exist");

		// Test the new data already exists as a new node
		CHECK(g2.replace_node("b", "a") == false);
	}
	SECTION("merge and replacenode") {
		auto g3 = gdwg::graph<std::string, int>{};
		g3.insert_node("A");
		g3.insert_node("B");
		g3.insert_node("C");
		g3.insert_node("D");
		// CHECK the node is not in nodes set
		CHECK_THROWS_WITH(g3.merge_replace_node("p", "z"),
		                  "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if "
		                  "they don't exist in the graph");

		// Basic case
		{
			g3.insert_edge("A", "B", 1);
			g3.insert_edge("A", "C", 2);
			g3.insert_edge("A", "D", 3);
			g3.merge_replace_node("A", "B");
			CHECK(g3.nodes() == std::vector<std::string>{"B", "C", "D"});
			CHECK(g3.weights("B", "B") == std::vector<int>{1});
			CHECK(g3.weights("B", "C") == std::vector<int>{2});
			CHECK(g3.weights("B", "D") == std::vector<int>{3});
		}

		// Case with duplicate
		{
			g3.insert_edge("C", "C", 2);
			g3.merge_replace_node("B", "C");
			CHECK(g3.nodes() == std::vector<std::string>{"C", "D"});
			CHECK(g3.weights("C", "C") == std::vector<int>{1, 2});
			CHECK(g3.weights("C", "D") == std::vector<int>{3});
		}
	}
	SECTION("erase node") {
		// Use g2 to test
		// Erase the node which is not in the node set
		CHECK(g2.erase_node("f") == false);

		g2.insert_edge("b", "a", 2);
		g2.insert_edge("b", "a", 3);
		g2.insert_edge("c", "a", 5);
		// Normal case
		CHECK(g2.is_node("a") == true);
		CHECK(g2.is_connected("b", "a") == true);
		CHECK(g2.is_connected("c", "a") == true);

		g2.erase_node("a");
		CHECK(g2.is_node("a") == false);

		CHECK_THROWS_WITH(g2.is_connected("b", "a"),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
		CHECK_THROWS_WITH(g2.is_connected("c", "a"),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
	};
	SECTION("erase edge(src->dst with weight)") {
		// Erase the edge which is not in edge set
		auto fail_1 = g1.erase_edge("hello", "how", 10);
		CHECK(fail_1 == false);

		// Erase the edge which src not in the node set
		CHECK_THROWS_WITH(g1.erase_edge("qazxsw", "how", 3),
		                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
		                  "exist in the graph");

		// Erase the edge which dst not in the node set
		CHECK_THROWS_WITH(g1.erase_edge("hello", "ac", 8),
		                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
		                  "exist in the graph");

		// Erase the edge
		CHECK(g1.insert_edge("hello", "are", 2) == true); // Check cannot insert the edge
		g1.insert_edge("you?", "are", 5);
		g1.insert_edge("are", "you?", 5);
		auto pass_1 = g1.erase_edge("hello", "are", 2);

		CHECK(pass_1 == true);
		CHECK(g1.insert_edge("hello", "are", 2) == true); // Check now can insert the edge
	}
	SECTION("erase edge(iterator i)") {
		auto it_begin = g4.begin();
		auto next = g4.erase_edge(it_begin);
		auto [from_1, to_1, weight_1] = *(next);
		CHECK(from_1 == "a");
		CHECK(to_1 == "c");
		CHECK(weight_1 == 1);
	}
	SECTION("erase edge(erase all edges in the range[i, s])") {
		CHECK(ranges::distance(g4.connections("a")) == 2);
		auto it_begin = g4.begin();
		auto it_end = g4.end();
		g4.erase_edge(it_begin, it_end);
		CHECK(ranges::distance(g4.connections("a")) == 0);
	}
	SECTION("clear (erase all nodes from the graph)") {
		g4.clear();
		CHECK(ranges::distance(g4.nodes()) == 0);
	}
}
