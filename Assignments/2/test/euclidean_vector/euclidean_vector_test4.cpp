#include "comp6771/euclidean_vector.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <vector>

TEST_CASE("Part 3: Operations") {
	// namespace Matchers = Catch::Matchers;
	auto b = comp6771::euclidean_vector{1.0, 2.0, 3.0};
	SECTION("Copy Assignment") {
		auto a = comp6771::euclidean_vector(1);
		a = b;
		REQUIRE(ranges::size(a) == 3);
		REQUIRE(ranges::size(b) == 3);
		// CHECK(fmt::format("{}", a) == "[1 2 3]");
		CHECK(Approx(a[0]) == 1.0);
		CHECK(Approx(a[1]) == 2.0);
		CHECK(Approx(a[2]) == 3.0);

		// (int, double) constructor
		auto c = comp6771::euclidean_vector(10, 5);
		a = c;
		REQUIRE(ranges::size(a) == 10);
	}
	SECTION("Move Assignment") {
		auto a = comp6771::euclidean_vector(1);
		auto c = comp6771::euclidean_vector{1.0, 2.0, 3.0};
		a = std::move(c);
		CHECK(ranges::size(c) == 0);
		// CHECK(fmt::format("{}", a) == "[1 2 3]");
		CHECK(Approx(a[0]) == 1.0);
		CHECK(Approx(a[1]) == 2.0);
		CHECK(Approx(a[2]) == 3.0);
	}
	SECTION("Subscript") {
		double a{b[1]};
		b[2] = 5.0;
		CHECK(a == 2.0); // b[1] = 2.0
		// CHECK(fmt::format("{}", b) == "[1 2 5]");
		CHECK(Approx(b[0]) == 1.0);
		CHECK(Approx(b[1]) == 2.0);
		CHECK(Approx(b[2]) == 5.0);
	}
	SECTION("Unary plus") {
		auto a = comp6771::euclidean_vector{3.0, 3.5, 5.6};
		+a;
		// CHECK(fmt::format("{}", a) == "[3 3.5 5.6]");
		CHECK(Approx(a[0]) == 3.0);
		CHECK(Approx(a[1]) == 3.5);
		CHECK(Approx(a[2]) == 5.6);
	}
	SECTION("Negation") {
		auto const actual = comp6771::euclidean_vector{-6, 1};
		auto const expected = comp6771::euclidean_vector{6, -1};
		CHECK(expected == -actual);

		auto zero = comp6771::euclidean_vector{0.0, 0.0};
		auto copy = comp6771::euclidean_vector{-6, 1};
		copy += expected;
		CHECK(zero == copy);

		-zero;
		// CHECK(fmt::format("{}", zero) == "[0 0]");
		CHECK(Approx(zero[0]) == 0.0);
		CHECK(Approx(zero[1]) == 0.0);

		zero = comp6771::euclidean_vector{-5.3, 2.7};
		// Check double negation
		-(-zero);
		CHECK(Approx(zero[0]) == -5.3);
		CHECK(Approx(zero[1]) == 2.7);
	}
	SECTION("Compound Addition") {
		auto a1 = comp6771::euclidean_vector{3.0, 3.5, 5.6};
		auto a2 = comp6771::euclidean_vector{3.0, 3.5, 5.6, 6};
		a1 += b;
		// CHECK(fmt::format("{}", a1) == "[4 5.5 8.6]");
		CHECK(Approx(a1[0]) == 4.0);
		CHECK(Approx(a1[1]) == 5.5);
		CHECK(Approx(a1[2]) == 8.6);
		// CHECK_THROWS(a2 += b);
		// auto m = std::string();
		CHECK_THROWS_WITH(a2 += b, fmt::format("Dimensions of LHS(4) and RHS(3) do not match"));
		auto zero = comp6771::euclidean_vector{0.0, 0.0, 0.0};
		zero += a1;
		// CHECK(fmt::format("{}", zero) == "[4 5.5 8.6]");
		CHECK(Approx(zero[0]) == 4.0);
		CHECK(Approx(zero[1]) == 5.5);
		CHECK(Approx(zero[2]) == 8.6);
	}
	SECTION("Compound Subtraction") {
		auto a1 = comp6771::euclidean_vector{3.0, 3.5};
		auto a2 = comp6771::euclidean_vector{3.0, 3.5, 6};
		a2 -= b;
		// CHECK(fmt::format("{}", a2) == "[2 1.5 3]");
		CHECK(Approx(a2[0]) == 2.0);
		CHECK(Approx(a2[1]) == 1.5);
		CHECK(Approx(a2[2]) == 3.0);
		CHECK_THROWS_WITH(a1 -= b, fmt::format("Dimensions of LHS(2) and RHS(3) do not match"));
	}

	SECTION("Compound Multiplication") {
		auto d1 = 1.5;
		auto a1 = comp6771::euclidean_vector{1.0, 2.0, 3.0};
		a1 *= d1;
		// CHECK(fmt::format("{}", a1) == "[1.5 3 4.5]");
		CHECK(Approx(a1[0]) == 1.5);
		CHECK(Approx(a1[1]) == 3.0);
		CHECK(Approx(a1[2]) == 4.5);

		auto d2 = 0;
		a1 *= d2;
		// CHECK(fmt::format("{}", a1) == "[0 0 0]");
		CHECK(Approx(a1[0]) == 0.0);
		CHECK(Approx(a1[1]) == 0.0);
		CHECK(Approx(a1[2]) == 0.0);
	}

	SECTION("Compound Division") {
		auto d1 = 2.5;
		auto a1 = comp6771::euclidean_vector{5, 7.5};
		a1 /= d1;
		// CHECK(fmt::format("{}", a1) == "[2 3]");
		CHECK(Approx(a1[0]) == 2.0);
		CHECK(Approx(a1[1]) == 3.0);
		auto d3 = 0;
		CHECK_THROWS_WITH(a1 /= d3, "Invalid vector division by 0");
	}
	SECTION("Vector Type Conversion") {
		// const vector
		auto const a1 = comp6771::euclidean_vector{0.0, 1.0, 2.0};
		auto const vf = static_cast<std::vector<double>>(a1);

		CHECK(ranges::size(vf) == 3);

		auto const a2 = comp6771::euclidean_vector{0.0, 1.0, 2.0};
		auto vf2 = static_cast<std::vector<double>>(a2);
		vf2.at(1) = 3;
		CHECK(vf2.at(1) == 3);
		REQUIRE(ranges::size(vf2) == 3);

		auto a3 = comp6771::euclidean_vector();
		vf2 = static_cast<std::vector<double>>(a3);
		REQUIRE(ranges::size(vf2) == 1);

		a3 = comp6771::euclidean_vector(3, 6.6);
		vf2 = static_cast<std::vector<double>>(a3);
		REQUIRE(ranges::size(vf2) == 3);
		CHECK(Approx(vf2[0]) == 6.6);
		CHECK(Approx(vf2[1]) == 6.6);
		CHECK(Approx(vf2[2]) == 6.6);
	}

	SECTION("List Type Conversion") {
		// Initialiser list constructor
		auto const a = comp6771::euclidean_vector{5.0, 3.0, 1.0};
		auto lf = static_cast<std::list<double>>(a);
		CHECK(lf.front() == 5.0);
		REQUIRE(lf.size() == 3);

		// default constructor
		auto a1 = comp6771::euclidean_vector();
		lf = static_cast<std::list<double>>(a1);
		REQUIRE(lf.size() == 1);

		// (int, double) constructor
		a1 = comp6771::euclidean_vector(10, -5);
		lf = static_cast<std::list<double>>(a1);
		CHECK(ranges::all_of(a1, [](auto d) { return d == -5; }));
	}
}
