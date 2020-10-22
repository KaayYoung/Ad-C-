// Copyright (c) Christopher Di Bella.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
#include "comp6771/euclidean_vector.hpp"
#include <cstddef>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <numeric>
#include <range/v3/functional.hpp>
#include <utility>

using gsl_lite::narrow_cast;
namespace comp6771 {
	// namespace views = ranges::views;
	// Part1: Constrctors
	euclidean_vector::euclidean_vector() noexcept
	: dimension_(1)
	// NOLINTNEXTLINE(modernize-avoid-c-arrays)
	, magnitudes_(std::make_unique<double[]>(1))
	, cache_(-1) {
		magnitudes_[0] = 0.0;
	}

	euclidean_vector::euclidean_vector(int dim) noexcept
	: dimension_(dim)
	// NOLINTNEXTLINE(modernize-avoid-c-arrays)
	, magnitudes_(std::make_unique<double[]>(narrow_cast<std::size_t>(dim)))
	, cache_(-1) {
		ranges::fill(magnitudes_.get(), magnitudes_.get() + dim, 0.0);
	}

	euclidean_vector::euclidean_vector(int dim, double mag) noexcept
	: dimension_(dim)
	// NOLINTNEXTLINE(modernize-avoid-c-arrays)
	, magnitudes_(std::make_unique<double[]>(narrow_cast<std::size_t>(dim)))
	, cache_(-1) {
		ranges::fill(magnitudes_.get(), magnitudes_.get() + dim, mag);
	}

	euclidean_vector::euclidean_vector(std::vector<double>::const_iterator start,
	                                   std::vector<double>::const_iterator end) noexcept {
		dimension_ = int(end - start);
		auto d = narrow_cast<std::size_t>(dimension_);
		// NOLINTNEXTLINE(modernize-avoid-c-arrays)
		magnitudes_ = std::make_unique<double[]>(d);
		ranges::copy(start, end, magnitudes_.get());
		cache_ = -1;
	}

	euclidean_vector::euclidean_vector(std::initializer_list<double> l) noexcept {
		dimension_ = static_cast<int>(l.size());
		// NOLINTNEXTLINE(modernize-avoid-c-arrays)
		magnitudes_ = std::make_unique<double[]>(l.size());
		ranges::copy(l.begin(), l.end(), magnitudes_.get());
		cache_ = -1;
	}

	// Copy Constructor
	euclidean_vector::euclidean_vector(euclidean_vector const& ev) noexcept
	: dimension_(ev.dimension_)
	// NOLINTNEXTLINE(modernize-avoid-c-arrays)
	, magnitudes_{std::make_unique<double[]>(narrow_cast<std::size_t>(ev.dimension_))}
	, cache_(-1) {
		ranges::copy(ev.magnitudes_.get(), ev.magnitudes_.get() + ev.dimension_, magnitudes_.get());
	}

	// Move Constructor
	euclidean_vector::euclidean_vector(euclidean_vector&& ev) noexcept
	: dimension_(std::exchange(ev.dimension_, 0))
	, magnitudes_(std::exchange(ev.magnitudes_, nullptr))
	, cache_(-1) {}

	// Part3: Operations
	// Copy Assignment = (Create a new one and copy)
	auto euclidean_vector::euclidean_vector::operator=(const euclidean_vector& first) noexcept
	   -> euclidean_vector& {
		if (this == &first) {
			return *this;
		}
		euclidean_vector copy = first;
		std::swap(copy, *this);
		return *this;
	}

	// Move Assignment std::move (Transfer resources)
	auto euclidean_vector::euclidean_vector::operator=(euclidean_vector&& ev) noexcept
	   -> euclidean_vector& {
		if (this == &ev) {
			return *this;
		}
		this->dimension_ = ev.dimension_;
		ev.dimension_ = 0;
		this->magnitudes_ = std::move(ev.magnitudes_);
		this->cache_ = ev.cache_;
		ev.cache_ = -1;
		return *this;
	}

	// Subscript const
	auto euclidean_vector::euclidean_vector::operator[](int i) const noexcept -> double {
		assert(i >= 0 && i < dimension_);
		return this->magnitudes_[narrow_cast<std::size_t>(i)];
	}

	// Subscript non-const
	auto euclidean_vector::euclidean_vector::operator[](int i) noexcept -> double& {
		assert(i >= 0 && i < dimension_);
		this->cache_ = -1;
		return this->magnitudes_[narrow_cast<std::size_t>(i)];
	}

	// Compound Addition
	auto euclidean_vector::euclidean_vector::operator+=(euclidean_vector const& cur)
	   -> euclidean_vector& {
		if (this->dimension_ != cur.dimension_) {
			auto e = std::stringstream();
			e << "Dimensions of LHS(" << this->dimension_ << ") and RHS(" << cur.dimension_
			  << ") do not match";
			throw euclidean_vector_error(e.str());
		}

		ranges::transform(magnitudes_.get(),
		                  magnitudes_.get() + dimension_,
		                  cur.magnitudes_.get(),
		                  cur.magnitudes_.get() + dimension_,
		                  magnitudes_.get(),
		                  ranges::plus{});
		this->cache_ = -1;
		return *this;
	}

	// Compound Subtract
	auto euclidean_vector::euclidean_vector::operator-=(euclidean_vector const& cur)
	   -> euclidean_vector& {
		if (this->dimension_ != cur.dimension_) {
			auto e = std::stringstream();
			e << "Dimensions of LHS(" << this->dimension_ << ") and RHS(" << cur.dimension_
			  << ") do not match";
			throw euclidean_vector_error(e.str());
		}
		ranges::transform(magnitudes_.get(),
		                  magnitudes_.get() + dimension_,
		                  cur.magnitudes_.get(),
		                  cur.magnitudes_.get() + dimension_,
		                  magnitudes_.get(),
		                  ranges::minus{});
		this->cache_ = -1;
		return *this;
	}

	// Compound Multiplication
	auto euclidean_vector::euclidean_vector::operator*=(double d) noexcept -> euclidean_vector& {
		ranges::transform(magnitudes_.get(),
		                  magnitudes_.get() + dimension_,
		                  magnitudes_.get(),
		                  [&d](auto const& x) { return x * d; });
		this->cache_ = -1;
		return *this;
	}

	// Compound Division
	auto euclidean_vector::euclidean_vector::operator/=(double d) -> euclidean_vector& {
		if (d == 0) {
			throw euclidean_vector_error("Invalid vector division by 0");
		}
		ranges::transform(magnitudes_.get(),
		                  magnitudes_.get() + dimension_,
		                  magnitudes_.get(),
		                  [&d](auto const& x) { return x / d; });
		this->cache_ = -1;
		return *this;
	}

	// Vector Type Conversion
	euclidean_vector::euclidean_vector::operator std::vector<double>() const noexcept {
		auto vec = std::vector<double>{};
		ranges::copy(std::span<double>(magnitudes_.get(), narrow_cast<std::size_t>(dimension_)),
		             ranges::back_inserter(vec));
		return vec;
	}

	// List Type Conversion
	euclidean_vector::euclidean_vector::operator std::list<double>() const noexcept {
		auto l = std::list<double>{};
		ranges::copy(std::span<double>(magnitudes_.get(), narrow_cast<std::size_t>(dimension_)),
		             ranges::back_inserter(l));
		return l;
	}

	// Part4: Member Functions
	auto euclidean_vector::at(int i) const -> double {
		if (i < 0 || i >= this->dimension_) {
			auto e = std::stringstream();
			e << "Index " << i << " is not valid for this euclidean_vector object";
			throw euclidean_vector_error(e.str());
		}
		return this->magnitudes_[narrow_cast<std::size_t>(i)];
	}

	auto euclidean_vector::at(int i) -> double& {
		if (i < 0 || i >= this->dimension_) {
			auto e = std::stringstream();
			e << "Index " << i << " is not valid for this euclidean_vector object";
			throw euclidean_vector_error(e.str());
		}
		this->cache_ = -1;
		return this->magnitudes_[narrow_cast<std::size_t>(i)];
	}

	auto euclidean_vector::dimensions() const noexcept -> int {
		return this->dimension_;
	}

	// Part6: Utility functions
	auto euclidean_norm(euclidean_vector const& ev) -> double {
		if (ev.dimension_ == 0) {
			throw euclidean_vector_error("euclidean_vector with no dimensions does not have a "
			                             "norm");
		}
		// Calculate cache if cache is not set up
		if (ev.cache_ == -1) {
			auto sum_squares = double{0};
			ranges::for_each (ev.magnitudes_.get(),
			                  ev.magnitudes_.get() + ev.dimensions(),
			                  [&sum_squares](auto d) { sum_squares += d * d; });
			ev.cache_ = sqrt(sum_squares);
			return sqrt(sum_squares);
		}
		return ev.cache_;
	}
	auto unit(euclidean_vector const& ev) -> euclidean_vector {
		// Throw exception when dimensions = 0
		if (ev.dimensions() == 0) {
			throw euclidean_vector_error("euclidean_vector with no dimensions does not have a norm");
		}
		euclidean_vector unit_v{ev};
		auto norm = euclidean_norm(unit_v);
		if (norm == 0) {
			throw euclidean_vector_error("euclidean_vector with zero euclidean normal does not have a "
			                             "unit vector");
		}
		unit_v /= norm;

		return unit_v;
	}

	auto dot(euclidean_vector const& a, euclidean_vector const& b) -> double {
		if (a.dimensions() != b.dimensions()) {
			auto e = std::stringstream();
			e << "Dimensions of LHS(" << a.dimensions() << ") and RHS(" << b.dimensions()
			  << ") do not match";
			throw euclidean_vector_error(e.str());
		}
		return std::inner_product(a.begin(), a.end(), b.begin(), 0.0);
	}
} // namespace comp6771
