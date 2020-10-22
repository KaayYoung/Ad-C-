#include "comp6771/euclidean_vector.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <vector>

TEST_CASE("Part 6: Utility functions") {
	SECTION("Euclidean Norm") {
		// Check const vector
		auto const a1 = comp6771::euclidean_vector{3, 4};
		CHECK(comp6771::euclidean_norm(a1) == 5);

		auto a2 = comp6771::euclidean_vector{0, 1, 2, 5};
		auto norm = comp6771::euclidean_norm(a2);
		CHECK(norm == sqrt(30));
		norm = comp6771::euclidean_norm(a2); // Check cache
		CHECK(norm == sqrt(30));

		a2.at(0) = -1;
		norm = comp6771::euclidean_norm(a2);
		CHECK(norm == sqrt(31));
		norm = comp6771::euclidean_norm(a2); // Check cache
		CHECK(norm == sqrt(31));

		a2[1] = 3;
		// CHECK(fmt::format("{}", a2) == "[-1 3 2 5]");
		CHECK(Approx(a2[0]) == -1.0);
		CHECK(Approx(a2[1]) == 3.0);
		CHECK(Approx(a2[2]) == 2.0);
		CHECK(Approx(a2[3]) == 5.0);

		norm = comp6771::euclidean_norm(a2);
		CHECK(norm == sqrt(39));
		norm = comp6771::euclidean_norm(a2); // Check cache
		CHECK(norm == sqrt(39));
	}
	SECTION("Unit") {
		auto const a1 = comp6771::euclidean_vector{1, 2, 3, 4};

		auto a2 = comp6771::euclidean_vector{0, 0};
		CHECK_THROWS_WITH(comp6771::unit(a2),
		                  fmt::format("euclidean_vector with zero euclidean normal does not have a "
		                              "unit vector"));
		a2.at(0) = 2;
		a2 = comp6771::unit(a1);
		// CHECK(fmt::format("{}", comp6771::unit(a1)) == "[0.182574 0.365148 0.547723 0.730297]");
		CHECK(Approx(a2[0]) == 0.182574);
		CHECK(Approx(a2[1]) == 0.365148);
		CHECK(Approx(a2[2]) == 0.547723);
		CHECK(Approx(a2[3]) == 0.730297);
		// Test negative numbers
		a2 = comp6771::euclidean_vector{-4, -3, -5};
		auto unit_res = comp6771::unit(a2);
		CHECK(Approx(unit_res.at(0)) == -0.5656854249);
		CHECK(Approx(unit_res.at(1)) == -0.4242640687);
		CHECK(Approx(unit_res.at(2)) == -0.7071067812);
	}
	SECTION("Dot Product") {
		// Easy check
		auto a = comp6771::euclidean_vector{1, 2, 3};
		auto b = comp6771::euclidean_vector{1, 2, 3};
		auto c = double{comp6771::dot(a, b)};
		CHECK(c == 14.0);

		// Check exception
		a = comp6771::euclidean_vector(6);
		b = comp6771::euclidean_vector(5, 5);
		CHECK_THROWS_WITH(comp6771::dot(a, b), "Dimensions of LHS(6) and RHS(5) do not match");

		// Harder test
		a = comp6771::euclidean_vector(6, 1);
		b = comp6771::euclidean_vector{2, -1, -5, -10, 0, 7};
		CHECK(comp6771::dot(a, b) == -7.0);

		// Check can whether work fine with floating-point numbers
		a = comp6771::euclidean_vector(3, 2.35);
		b = comp6771::euclidean_vector{1.0453, 203.8923, -65.23401};
		CHECK(comp6771::dot(a, b) == 328.3034365);
	}
	SECTION("Combine all") {
		auto a0 = comp6771::euclidean_vector{};
		REQUIRE(ranges::size(a0) == 1);
		auto const e = fmt::format("euclidean_vector with zero euclidean normal does not have a unit "
		                           "vector");
		CHECK_THROWS_WITH(comp6771::unit(a0), e);
		auto const a1 = comp6771::euclidean_vector{3, 4};
		auto a2 = comp6771::euclidean_vector{0, 1, 2, 5};
		CHECK_THROWS_WITH(comp6771::dot(a1, a2),
		                  fmt::format("Dimensions of LHS(2) and RHS(4) do not match"));
		// Check cache
		CHECK(comp6771::euclidean_norm(a1) == 5);
		CHECK(comp6771::euclidean_norm(a1) == 5);
		CHECK(comp6771::euclidean_norm(a1) == 5);
		CHECK(a0.at(0) == 0);
	}
}
