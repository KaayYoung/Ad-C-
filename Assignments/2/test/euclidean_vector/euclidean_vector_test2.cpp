#include "comp6771/euclidean_vector.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <vector>

TEST_CASE("Part 1: Constructors") {
	SECTION("Default Constructor") {
		auto a1 = comp6771::euclidean_vector();
		REQUIRE(ranges::size(a1) == 1);
		CHECK(ranges::all_of(a1, [](auto const& x) { return x == 0.0; }));
		// CHECK(fmt::format("{}", a1) == "[0]");
		CHECK(Approx(a1[0]) == 0.0);
	}

	SECTION("Single-argument Constructor") {
		auto a2 = comp6771::euclidean_vector(1000); // test_case: set the number as a large number
		REQUIRE(ranges::size(a2) == 1000);
		CHECK(ranges::all_of(a2, [](auto const& x) { return x == 0.0; }));
	}

	SECTION("Constructor that takes the dimension and the magnitude") {
		auto a3 = comp6771::euclidean_vector(3, 3.0);
		REQUIRE(ranges::size(a3) == 3);
		// CHECK(ranges::all_of(a3, [](auto const& x) { return x == 3.0; }));
		// CHECK(fmt::format("{}", a3) == "[3 3 3]");
		CHECK(Approx(a3[0]) == 3.0);
		CHECK(Approx(a3[1]) == 3.0);
		CHECK(Approx(a3[2]) == 3.0);
		auto x = int{3};
		auto y = double{4.5};
		auto a4 = comp6771::euclidean_vector(x, y);
		CHECK(ranges::size(a4) == 3);
		CHECK(ranges::all_of(a4, [](auto const& x) { return x == 4.5; }));
	}

	SECTION("Constructor that takes the start and end of an iterator") {
		auto const v1 = std::vector<double>{5, 3, 1, 4, 10, 5.5};
		auto a5 = comp6771::euclidean_vector(v1.begin(), v1.end());
		REQUIRE(ranges::size(a5) == 6);
		// CHECK(fmt::format("{}", a5) == "[5 3 1 4 10 5.5]");
		CHECK(Approx(a5[0]) == 5.0);
		CHECK(Approx(a5[1]) == 3.0);
		CHECK(Approx(a5[2]) == 1.0);
		CHECK(Approx(a5[3]) == 4.0);
		CHECK(Approx(a5[4]) == 10.0);
		CHECK(Approx(a5[5]) == 5.5);
		// When Vector is null
		auto const v2 = std::vector<double>{};
		a5 = comp6771::euclidean_vector(v2.begin(), v2.end());
		CHECK(ranges::size(a5) == 0);
		CHECK(fmt::format("{}", a5) == "[]");
	}

	SECTION("Constructor that takes an initialiser list of double") {
		auto a6 = comp6771::euclidean_vector{1.0, 2.0, 3.0};
		REQUIRE(ranges::size(a6) == 3);

		a6 = comp6771::euclidean_vector(std::initializer_list<double>{});
		REQUIRE(ranges::size(a6) == 0);
	}

	SECTION("Copy Constructor") {
		auto a6 = comp6771::euclidean_vector{1.0, 2.0, 3.0};
		auto a7 = comp6771::euclidean_vector(a6);
		CHECK(ranges::size(a7) == 3);

		a6 = comp6771::euclidean_vector(4, 3);
		a7 = comp6771::euclidean_vector(a6);
		// CHECK(fmt::format("{}", a7) == "[3 3 3 3]");
		CHECK(Approx(a7[0]) == 3.0);
		CHECK(Approx(a7[1]) == 3.0);
		CHECK(Approx(a7[2]) == 3.0);
		CHECK(Approx(a7[3]) == 3.0);

		a6 = comp6771::euclidean_vector();
		a7 = comp6771::euclidean_vector(a6);
		REQUIRE(ranges::size(a7) == 1);
		// CHECK(fmt::format("{}", a7) == "[0]");
		CHECK(Approx(a7[0]) == 0.0);
	}

	SECTION("Move Constructor") {
		auto a6 = comp6771::euclidean_vector{1.0, 2.0, 3.0};
		auto a7 = comp6771::euclidean_vector(a6);
		auto a_move = comp6771::euclidean_vector(std::move(a7));
		REQUIRE(ranges::size(a6) == 3);
		REQUIRE(ranges::size(a7) == 0);
		CHECK(ranges::size(a_move) == 3);
	}
}
