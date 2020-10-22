#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
#include <sstream>

// Testing rationale comment //
// This file is used to test constructors

TEST_CASE("2.2 Constructors") {
	using graph = gdwg::graph<int, int>;
	auto v = std::vector<graph::value_type>{
	   {4, 1, -4},
	   {3, 2, 2},
	   {2, 4, 2},
	   {2, 1, 1},
	   {6, 2, 5},
	   {6, 3, 10},
	   {1, 5, -1},
	   {3, 6, -8},
	   {4, 5, 3},
	   {5, 2, 7},
	};
	auto g = graph(v.begin(), v.end());
	SECTION("default constructor") {
		// auto default_g = gdwg::graph();
	}
	SECTION("Initialiser list") {
		auto il = std::initializer_list<int>{1, 5, 10, 21};
		auto graph_il = gdwg::graph<int, int>(il);
		CHECK(graph_il.nodes() == std::vector<int>{1, 5, 10, 21});
	}

	SECTION("Move constructor") {
		CHECK(g.connections(1) == std::vector<int>{5});
		auto g2 = gdwg::graph(g); // Copy constructor
		auto g2_move = std::move(g2); // Move constructor

		// Do a basic test to make sure works fine
		CHECK(g2_move.nodes() == std::vector<int>{1, 2, 3, 4, 5, 6});
		CHECK(g2_move.connections(1) == std::vector<int>{5});
		CHECK(g2_move.weights(5, 2) == std::vector<int>{7});
	}
	SECTION("Move assignment") {
		auto g2 = gdwg::graph(g);
		auto g3 = std::move(g2); // Move assignment

		// Do a basic test to make sure works fine
		CHECK(g3.nodes() == std::vector<int>{1, 2, 3, 4, 5, 6});
		CHECK(g3.connections(1) == std::vector<int>{5});
		CHECK(g3.weights(1, 5) == std::vector<int>{-1});
	}
	SECTION("Copy constructor") {
		auto g2 = gdwg::graph(g);
		// Do a basic test to make sure works fine
		CHECK(g2.nodes() == std::vector<int>{1, 2, 3, 4, 5, 6});
		CHECK(g2.connections(1) == std::vector<int>{5});
		CHECK(g2.weights(6, 3) == std::vector<int>{10});
	}
	SECTION("Copy assignment") {
		const auto& g2 = g; // Copy assignment

		// Do a basic test to make sure works fine
		CHECK(g2.nodes() == std::vector<int>{1, 2, 3, 4, 5, 6});
		CHECK(g2.connections(1) == std::vector<int>{5});
		CHECK(g2.weights(3, 2) == std::vector<int>{2});
	}
}
