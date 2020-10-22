#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>

// Testing rationale comment //
// graph_test5.cpp is used to test 2.5 Range access and Iterator

TEST_CASE("2.5 Range access && 2.8 Iterator") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(20);
	g1.insert_node(1);
	g1.insert_node(33);
	g1.insert_node(15);
	g1.insert_edge(1, 15, "abc");
	g1.insert_edge(15, 33, "qaz");
	auto g2 = gdwg::graph<int, std::string>{};

	SECTION("2.5 Range access") {
		auto is_begin = g1.begin();
		auto is_end = g1.end();
		auto [from, to, weight] = *(is_begin);
		CHECK(from == 1);
		CHECK(to == 15);
		CHECK(weight == "abc");
		auto [from_2, to_2, weight_2] = *(--is_end);
		CHECK(from_2 == 15);
		CHECK(to_2 == 33);
		CHECK(weight_2 == "qaz");
	}

	SECTION("2.8 Iterator") {
		auto is_begin = g1.begin();
		++is_begin;
		auto [from_1, to_1, weight_1] = *(is_begin);
		CHECK(from_1 == 15);
		CHECK(to_1 == 33);
		CHECK(weight_1 == "qaz");
		g1.insert_edge(15, 33, "xyzcds");
		--is_begin;
		auto [from_2, to_2, weight_2] = *(is_begin);
		CHECK(from_2 == 1);
		CHECK(to_2 == 15);
		CHECK(weight_2 == "abc");
	}
}
