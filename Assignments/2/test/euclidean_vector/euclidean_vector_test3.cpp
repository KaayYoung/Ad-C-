#include "comp6771/euclidean_vector.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <vector>

TEST_CASE("Member Functions") {
	// TODO (const and non-const function at())
	auto a0 = comp6771::euclidean_vector();
	auto const a1 = comp6771::euclidean_vector();
	auto const a2 = comp6771::euclidean_vector(2);
	auto const a3 = comp6771::euclidean_vector(3, 3.0);
	auto x = int{10};
	auto y = double{4.5};
	auto a4 = comp6771::euclidean_vector(x, y);
	auto const v = std::vector<double>{1, 2, 3, 4, 10, 5.5, 3, 7, 6, 9};
	auto const a5 = comp6771::euclidean_vector(v.begin(), v.end());
	auto a6 = comp6771::euclidean_vector{1.2, 2.3, 3.5};

	SECTION("Function at(): return value (const)") {
		CHECK(a1.at(0) == 0);
		CHECK(a2.at(1) == 0);
		CHECK(a3.at(2) == 3);
		CHECK(a4.at(3) == 4.5);
		CHECK(a5.at(4) == 10);
		CHECK(a6.at(1) == 2.3);
		CHECK_THROWS_WITH(a0.at(1) = 2,
		                  fmt::format("Index {} is not valid for this euclidean_vector object", 1));
		CHECK_THROWS_WITH(a0.at(1) == 2,
		                  fmt::format("Index {} is not valid for this euclidean_vector object", 1));
		CHECK_THROWS_WITH(a6.at(-3) = 0,
		                  fmt::format("Index {} is not valid for this euclidean_vector object", -3));
		CHECK_THROWS_WITH(a6.at(3) == 0,
		                  fmt::format("Index {} is not valid for this euclidean_vector object", 3));
	}
	SECTION("Function at(): return reference (non-const)") {
		a6.at(2) = 3;
		CHECK(a6.at(2) == 3);
		REQUIRE(ranges::size(a6) == 3);

		a0 = comp6771::euclidean_vector(2);
		CHECK_THROWS_WITH(a0.at(2) == 0,
		                  fmt::format("Index {} is not valid for this euclidean_vector object", 2));

		a6 = comp6771::euclidean_vector{1.2, 2.3, 3.5, 4};
		CHECK_THROWS_WITH(a0.at(-4) == 0,
		                  fmt::format("Index {} is not valid for this euclidean_vector object", -4));
	}
	SECTION("Function dimensions(): return the number of dimensions") {
		CHECK(ranges::size(a1) == gsl_lite::narrow_cast<std::size_t>(a1.dimensions()));
		CHECK(ranges::size(a2) == gsl_lite::narrow_cast<std::size_t>(a2.dimensions()));
		CHECK(ranges::size(a3) == gsl_lite::narrow_cast<std::size_t>(a3.dimensions()));
		CHECK(ranges::size(a4) == gsl_lite::narrow_cast<std::size_t>(a4.dimensions()));
		CHECK(ranges::size(a5) == gsl_lite::narrow_cast<std::size_t>(a5.dimensions()));
		CHECK(ranges::size(a6) == gsl_lite::narrow_cast<std::size_t>(a6.dimensions()));
	}
}
