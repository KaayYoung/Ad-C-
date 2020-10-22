#include "comp6771/euclidean_vector.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <vector>

TEST_CASE("Friends") {
	auto const a1 = comp6771::euclidean_vector();
	auto const a2 = comp6771::euclidean_vector(2);
	auto const a3 = comp6771::euclidean_vector(3, 3.0);
	auto x = int{10};
	auto y = double{4.5};
	auto a4 = comp6771::euclidean_vector(x, y);
	auto const v = std::vector<double>{1, 2, 3, 4, 10, 5.5, 3, 7, 6, 9};
	auto const a5 = comp6771::euclidean_vector(v.begin(), v.end());
	auto const a6 = comp6771::euclidean_vector{1.2, 2.3, 3.5};

	SECTION("Equal") {
		// Check int, double define is equal
		auto copy_a = comp6771::euclidean_vector(3, 3.0);
		CHECK(a3 == copy_a);

		// Check list define with int, double define
		auto const copy_a2 = comp6771::euclidean_vector{3.0, 3.0, 3.0};
		CHECK(a3 == copy_a2);

		// Check vector define with int double define
		auto const v1 = std::vector<double>{3, 3, 3};
		copy_a = comp6771::euclidean_vector(v1.begin(), v1.end());
		CHECK(a3 == copy_a);

		// Test Division with equal
		auto const exp1 = comp6771::euclidean_vector(1, 0.2);
		auto vec1 = comp6771::euclidean_vector(1, 1.0);
		CHECK(exp1 == vec1 / sqrt(5) / sqrt(5));

		// Test Multiply with equal
		auto const exp2 = comp6771::euclidean_vector{-3.3, 314.1, 0.0, 0.3, 15.6};
		auto vec2 = comp6771::euclidean_vector{-1.1, 104.7, 0.0, 0.1, 5.2};
		CHECK((vec2 * 3) == exp2);
	}
	SECTION("Not Equal") {
		auto const copy1_a = comp6771::euclidean_vector(2, 3.0);
		auto const copy2_a = comp6771::euclidean_vector(3, 2.9);
		CHECK(a3 != copy1_a);
		CHECK(a3 != copy2_a);
	}
	SECTION("Addition") {
		auto a = comp6771::euclidean_vector();
		auto const b = comp6771::euclidean_vector{1, 2, 3};
		a = a6 + b;
		CHECK(fmt::format("{}", a) == "[2.2 4.3 6.5]");

		CHECK_THROWS_WITH(a = a5 + b, "Dimensions of LHS(10) and RHS(3) do not match");
	}
	SECTION("Subtraction") {
		auto a = comp6771::euclidean_vector();
		auto const b = comp6771::euclidean_vector{1, 2, 3};
		a = a6 - b;
		// CHECK(fmt::format("{}", a) == "[0.2 0.3 0.5]");
		CHECK(Approx(a[0]) == 0.2);
		CHECK(Approx(a[1]) == 0.3);
		CHECK(Approx(a[2]) == 0.5);
		CHECK_THROWS_WITH(a = a2 - b, "Dimensions of LHS(2) and RHS(3) do not match");
	}
	SECTION("Multiply") {
		auto a = comp6771::euclidean_vector();
		auto b = comp6771::euclidean_vector{1, 2, 3};
		a = b * 3;
		a = 3 * b;
		CHECK(fmt::format("{}", a) == "[3 6 9]");
		CHECK(Approx(a[0]) == 3.0);
		CHECK(Approx(a[1]) == 6.0);
		CHECK(Approx(a[2]) == 9.0);
		a = comp6771::euclidean_vector{1.2345, 2.93, 2.01, 10.555, 4.2987501, 52.4201};
		// CHECK(fmt::format("{}", a) == "[1.2345 2.93 2.01 10.555 4.2987501 52.4201]");
		CHECK(Approx(a.at(4)) == 4.29875);
		b = a * 4;
		CHECK(Approx(b.at(0)) == 4.938);
		CHECK(Approx(b.at(1)) == 11.72);
		CHECK(Approx(b.at(2)) == 8.04);
		CHECK(Approx(b.at(3)) == 42.22);
		CHECK(Approx(b.at(4)) == 17.1950004);
		CHECK(Approx(b.at(5)) == 209.6804);
	}
	SECTION("Divide") {
		auto b = comp6771::euclidean_vector(3, 3.0);
		double c = 2;
		auto a = b / c;
		CHECK(fmt::format("{}", a) == "[1.5 1.5 1.5]");

		b = comp6771::euclidean_vector{3.9999, 101.4, 1000.11102};
		b = b / 3;
		CHECK(Approx(b.at(0)) == 1.3333);
		CHECK(Approx(b.at(1)) == 33.8);
		CHECK(Approx(b.at(2)) == 333.3704);

		// Check exception
		b = comp6771::euclidean_vector(10, 0);
		c = 0;
		CHECK_THROWS_WITH(b = b / c, "Invalid vector division by 0");
	}
}
