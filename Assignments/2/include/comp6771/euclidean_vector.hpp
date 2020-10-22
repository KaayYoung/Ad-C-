#ifndef COMP6771_EUCLIDEAN_VECTOR_HPP
#define COMP6771_EUCLIDEAN_VECTOR_HPP

#include <__config>
#include <algorithm>
#include <array>
#include <compare>
#include <fmt/core.h>
#include <fmt/format.h>
#include <functional>
#include <gsl-lite.hpp>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <ostream>
#include <range/v3/algorithm.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/iterator.hpp>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace comp6771 {
	class euclidean_vector_error : public std::runtime_error {
	public:
		explicit euclidean_vector_error(std::string const& what) noexcept
		: std::runtime_error(what) {}
	};

	class euclidean_vector {
	public:
		// --------- Part1: Constructors ---------
		euclidean_vector() noexcept;
		explicit euclidean_vector(int) noexcept;
		euclidean_vector(int, double) noexcept;
		euclidean_vector(std::vector<double>::const_iterator,
		                 std::vector<double>::const_iterator) noexcept;
		euclidean_vector(std::initializer_list<double>) noexcept;
		euclidean_vector(euclidean_vector const&) noexcept;
		euclidean_vector(euclidean_vector&&) noexcept;

		// --------- Part2: Deconstructor ---------
		~euclidean_vector() noexcept = default;

		// --------- Part3: Operations ---------
		auto operator=(euclidean_vector const&) noexcept -> euclidean_vector&; // Copy Assignment
		auto operator=(euclidean_vector&&) noexcept -> euclidean_vector&; // Move Assignment

		auto operator[](int) const noexcept -> double; // Subscript: const
		auto operator[](int) noexcept -> double&; // Subscript

		// Unary plus
		friend auto operator+(const euclidean_vector& cur) noexcept -> euclidean_vector {
			euclidean_vector v{cur};
			return v;
		}
		// Negation
		friend auto operator-(const euclidean_vector& cur) noexcept -> euclidean_vector {
			euclidean_vector v{cur};
			std::transform(v.magnitudes_.get(),
			               v.magnitudes_.get() + v.dimension_,
			               v.magnitudes_.get(),
			               std::negate());
			return v;
		}

		auto operator+=(const euclidean_vector&) -> euclidean_vector&; // Compound Addition
		auto operator-=(const euclidean_vector&) -> euclidean_vector&; // Compound Subtraction

		auto operator*=(double) noexcept -> euclidean_vector&; // Compound Multiplication
		auto operator/=(double) -> euclidean_vector&; // Compound Division

		explicit operator std::vector<double>() const noexcept; // Vector Type Conversion
		explicit operator std::list<double>() const noexcept; // List Type Convercion

		// --------- Part4: Member Functions ---------
		[[nodiscard]] auto at(int) const -> double;
		[[nodiscard]] auto at(int) -> double&;
		[[nodiscard]] auto dimensions() const noexcept -> int;

		// --------- Part5: Friends ----------
		friend auto operator==(euclidean_vector const& v1, euclidean_vector const& v2) noexcept
		   -> bool {
			if (v1.dimension_ != v2.dimension_) {
				return false;
			}
			// Use ranges::equal also does same thing
			// return ranges::equal(v1.magnitudes_.get(),
			//               v1.magnitudes_.get() + v1.dimensions(),
			//               v2.magnitudes_.get(),
			//               v2.magnitudes_.get() + v2.dimensions(),
			//               [](auto const& x, auto const& y) { return std::fabs(x - y) < 1e-14; });
			auto const res = ranges::mismatch(v1.magnitudes_.get(),
			                                  v1.magnitudes_.get() + v1.dimensions(),
			                                  v2.magnitudes_.get(),
			                                  v2.magnitudes_.get() + v2.dimensions(),
			                                  [](auto d1, auto d2) { return fabs(d1 - d2) > 1e-14; });
			return (ranges::distance(v1.magnitudes_.get(), res.in1) == 0);
		}
		friend auto operator!=(euclidean_vector const& v1, euclidean_vector const& v2) noexcept
		   -> bool {
			return !(v1 == v2);
		}
		friend auto operator+(euclidean_vector const& v1, euclidean_vector const& v2)
		   -> euclidean_vector {
			if (v1.dimension_ != v2.dimension_) {
				auto e = std::stringstream();
				e << "Dimensions of LHS(" << v1.dimension_ << ") and RHS(" << v2.dimension_
				  << ") do not match";
				throw euclidean_vector_error(e.str());
			}
			euclidean_vector res{v1};
			res += v2;
			return res;
		}
		friend auto operator-(euclidean_vector const& v1, euclidean_vector const& v2)
		   -> euclidean_vector {
			if (v1.dimension_ != v2.dimension_) {
				auto e = std::stringstream();
				e << "Dimensions of LHS(" << v1.dimension_ << ") and RHS(" << v2.dimension_
				  << ") do not match";
				throw euclidean_vector_error(e.str());
			}
			euclidean_vector res{v1};
			res -= v2;
			return res;
		}

		// Should implement U * T and T * U
		friend auto operator*(euclidean_vector const& v, double d) noexcept -> euclidean_vector {
			euclidean_vector res{v};
			return res *= d;
		}
		friend auto operator*(double d, euclidean_vector const& v) noexcept -> euclidean_vector {
			euclidean_vector res{v};
			return res *= d;
		}
		friend auto operator/(euclidean_vector const& v, double d) -> euclidean_vector {
			if (d == 0) {
				throw euclidean_vector_error("Invalid vector division by 0");
			}
			euclidean_vector res{v};
			return res /= d;
		}
		friend auto operator<<(std::ostream& out, euclidean_vector const& v) -> std::ostream& {
			// auto const vectorized = std::vector<double>(v);
			// std::ostringstream os;
			// oss << fmt::format("[{}]", fmt::join(vectorized, " "));
			// auto oss = std::ostringstream();
			// oss << '[';
			// for (int i = 0; i < v.dimensions(); ++i) {
			// 	if (i != v.dimensions() - 1) {
			// 		oss << v.at(i);
			// 		oss << " ";
			// 	} else {
			// 		oss << v.at(i);
			// 	}
			// }
			// oss << ']';
			// out << oss.str();
			// oss << '[';
			out << '[';
			auto i = 0;
			auto const span_v =
			   std::span<double>(v.magnitudes_.get(), gsl::narrow_cast<std::size_t>(v.dimension_));
			ranges::for_each (span_v, [&i, &out, &v](auto const& mag) {
				out << mag;
				if (i != v.dimension_ - 1) {
					out << ' ';
				}
				++i;
			});
			out << ']';

			return out;
		}

		// Need to use private member, mark euclidean_norm as friend
		// Square root of the sum of the squares of the magnitudes
		friend auto euclidean_norm(euclidean_vector const& ev) -> double;

		// Used for Utility function: unit and dot product
		[[nodiscard]] auto begin() const noexcept -> double const* {
			return this->magnitudes_.get();
		};

		[[nodiscard]] auto end() const noexcept -> double const* {
			return this->magnitudes_.get() + dimension_;
		};

	private:
		int dimension_;
		// NOLINTNEXTLINE(modernize-avoid-c-arrays)
		std::unique_ptr<double[]> magnitudes_;
		// TODO (add cache_ for every constructor)
		mutable double cache_;
	};

	// ---------- Part6: Utility functions ---------
	// To avoid hidden friends
	auto euclidean_norm(euclidean_vector const& ev) -> double;
	// Unit vector of v
	auto unit(euclidean_vector const&) -> euclidean_vector;
	// The dot product of two vectors
	auto dot(euclidean_vector const&, euclidean_vector const&) -> double;
} // namespace comp6771

#endif // COMP6771_EUCLIDEAN_VECTOR_HPP
