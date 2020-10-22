#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
#include <range/v3/algorithm/permutation.hpp>

// Testing rationale comment //
// Because Modifiers" uses many functions from "Accessors"
// test the section "Accessors" first, but here only perform
// some easy tests, some other hard cases will be tested
// with functions from the section "Modifiers"
// The graph I used in this test_case is graph<int, string>

TEST_CASE("2.4 Accessors") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(20);
	g1.insert_node(1);
	g1.insert_node(33);
	g1.insert_node(15);
	g1.insert_edge(1, 15, "abc");
	g1.insert_edge(15, 33, "qaz");
	auto g2 = gdwg::graph<int, std::string>{};
	SECTION("is node") {
		CHECK(g1.is_node(1) == true);
		CHECK(g1.is_node(15) == true);

		// CHECK the node not in the node set
		CHECK(g1.is_node(66) == false);
	}
	SECTION("empty") {
		CHECK(g1.empty() == false);
		CHECK(g2.empty() == true);
	}
	SECTION("is_connected") {
		// CHECK connected nodes;
		CHECK(g1.is_connected(1, 15) == true);
		CHECK(g1.is_connected(15, 33) == true);

		// CHECK not connected
		CHECK(g1.is_connected(1, 33) == false);

		// CHECK exception
		CHECK_THROWS_WITH(g1.is_connected(2, 33),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
		CHECK_THROWS_WITH(g1.is_connected(33, 51),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
	}
	SECTION("nodes") {
		// Normal case
		CHECK(g1.nodes() == std::vector<int>{1, 15, 20, 33});

		// Add then check
		g2.insert_node(50);
		g2.insert_node(100);
		g2.insert_node(88);
		CHECK(g2.nodes() == std::vector<int>{50, 88, 100});

		// CHECK whether sorted when add new node
		g1.insert_node(2);
		g1.insert_node(17);
		CHECK(g1.nodes() == std::vector<int>{1, 2, 15, 17, 20, 33});
	}
	SECTION("weights") {
		CHECK(g1.weights(1, 15) == std::vector<std::string>{"abc"});
		g1.insert_edge(1, 15, "def");
		CHECK(g1.weights(1, 15) == std::vector<std::string>{"abc", "def"});
		// CHECK(g1.weights(15, 1).empty() == true);
	};
	SECTION("find (return an iterator)") {
		auto g3 = gdwg::graph<int, std::string>{};
		g3.insert_node(20);
		g3.insert_node(1);
		g3.insert_node(16);
		g3.insert_edge(20, 1, "a");
		g3.insert_edge(16, 1, "d");
		g3.insert_edge(20, 16, "c");
		auto it = g3.find(20, 16, "c");
		CHECK(it != g3.end());
	}
	SECTION("connections") {
		// Basic case
		CHECK(g1.connections(1) == std::vector<int>{15});

		// Add edges
		g1.insert_edge(1, 20, "pwd");
		g1.insert_edge(1, 20, "bpq");
		CHECK(g1.connections(1) == std::vector<int>{15, 20});

		// Check go out of scope
		{
			g2.insert_node(50);
			g2.insert_node(100);
			g2.insert_node(88);
			g2.insert_edge(50, 88, "bwt");
		}
		CHECK(g2.connections(50) == std::vector<int>{88});
		CHECK(g2.nodes() == std::vector<int>{50, 88, 100});
		CHECK(ranges::distance(g2.connections(80)) == 0);
	}
}
