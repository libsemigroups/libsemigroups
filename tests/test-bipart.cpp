//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2026 James D. Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <algorithm>         // for equal
#include <cstddef>           // for size_t
#include <cstdint>           // for uint32_t, int32_t
#include <initializer_list>  // for initializer_list
#include <string>            // for allocator, basic_string
#include <unordered_set>     // for unordered_set
#include <utility>           // for move
#include <vector>            // for operator==

#include "libsemigroups/bipart.hpp"     // for Bipartition, Blocks, make
#include "libsemigroups/exception.hpp"  // for LibsemigroupsException

#include "libsemigroups/detail/fmt.hpp"  // for format

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for SourceLineInfo, operat...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {

  struct LibsemigroupsException;
  namespace {
    Blocks construct_blocks(std::vector<uint32_t> const& blocks,
                            std::vector<bool> const&     lookup) {
      Blocks result(blocks.begin(), blocks.end());
      for (size_t i = 0; i < lookup.size(); ++i) {
        result.is_transverse_block(i, lookup[i]);
      }
      blocks::throw_if_invalid(result);
      return result;
    }

    // The upper quantile should be calculated using some quantile Chi-squared
    // function which, to JDE's knowledge, isn't readily available in C++.
    // In R, use:
    // > qchisq(0.99, df)
    // to calculate a 99% quantile where df is one less than the total number
    // of bipartitions of the given degree.
    //
    // We specify the expected number of times each bipartition should appear
    // rather than the number of samples to generate because the validity of the
    // test is weakend if the expected number is too small.
    //
    // The Chi squared statistic is Σ((O_i - E)^2/E) where:
    // O_i = Number of occurrences of the ith bipartition
    // E = expected
    //
    // Reject the hypothesis that our distribution is uniform (i.e. fail the
    // test) if the test statistic is larger than upper_quantile.
    void test_uniform_bipartition(size_t  deg,
                                  size_t  num_total_bipartitions,
                                  float_t upper_quantile,
                                  float_t expected = 20) {
      // Perform the sampling
      size_t num_trials = expected * num_total_bipartitions;
      std::unordered_map<Bipartition, size_t, Hash<Bipartition>> map;

      for (size_t i = 0; i < num_trials; ++i) {
        ++map[bipartition::uniform_random(deg)];
      }

      // Calculate the test statistic
      float_t statistic               = 0;
      float_t correction              = 0;
      size_t  nr_unfound_bipartitions = num_total_bipartitions - map.size();

      if (expected < 10) {
        // Perform Yates's continuity correction
        correction = 0.5;
      }

      for (const auto& [bip, count] : map) {
        float_t difference = std::abs(count - expected) - correction;
        statistic += (difference * difference / expected);
      }

      statistic
          += nr_unfound_bipartitions
             * ((expected - correction) * (expected - correction) / expected);

      REQUIRE(statistic < upper_quantile);
    }

  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("Blocks", "000", "empty blocks", "[quick]") {
    Blocks b1 = make<Blocks>({});
    Blocks b2 = make<Blocks>({{4, 2}, {-1, -5}, {-3, -6}});
    REQUIRE(b2.lookup() == std::vector({true, false, true}));
    REQUIRE(b1 == b1);
    REQUIRE(!(b1 == b2));
    REQUIRE(b1 < b2);
    REQUIRE(!(b2 < b1));
    REQUIRE(b1.degree() == 0);
    REQUIRE(b1.number_of_blocks() == 0);
    REQUIRE(b1.rank() == 0);
    REQUIRE(blocks::underlying_partition(b2)
            == std::vector<std::vector<int32_t>>({{-1, -5}, {2, 4}, {-3, -6}}));
    REQUIRE(to_human_readable_repr(b2)
            == "Blocks({{-1, -5}, {2, 4}, {-3, -6}})");
  }

  LIBSEMIGROUPS_TEST_CASE("Blocks", "001", "non-empty blocks", "[quick]") {
    Blocks b = construct_blocks({0, 1, 2, 1, 0, 2}, {true, false, true});
    REQUIRE(b == b);
    REQUIRE_NOTHROW(b.block_no_checks(0, 0));
    REQUIRE_NOTHROW(b.block(0, 0));
    REQUIRE_THROWS_AS(b.block(10, 0), LibsemigroupsException);
    REQUIRE(!(b < b));
    REQUIRE(b.degree() == 6);
    REQUIRE(std::vector(b.cbegin_lookup(), b.cend_lookup())
            == std::vector({true, false, true}));
    REQUIRE(b.number_of_blocks() == 3);
    REQUIRE(b.rank() == 2);
    REQUIRE(b.is_transverse_block(0));
    REQUIRE(!b.is_transverse_block(1));
    REQUIRE(b.is_transverse_block(2));
    REQUIRE_THROWS_AS(b.is_transverse_block(10), LibsemigroupsException);

    REQUIRE(std::vector(b.cbegin(), b.cend())
            == std::vector<uint32_t>({0, 1, 2, 1, 0, 2}));
    // The validity of the 2nd argument isn't checked
    REQUIRE_NOTHROW(b.block(0, 10));
    REQUIRE_THROWS_AS(blocks::throw_if_invalid(b), LibsemigroupsException);
    REQUIRE_NOTHROW(b.block(0, 0));
    REQUIRE_THROWS_AS(blocks::throw_if_invalid(b), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Blocks",
                          "002",
                          "left blocks of bipartition",
                          "[quick]") {
    auto x = make<Bipartition>(
        {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
    Blocks* b = x.left_blocks();
    REQUIRE(b == b);
    REQUIRE(!(b < b));
    REQUIRE(b->degree() == 10);
    REQUIRE(std::vector(b->cbegin_lookup(), b->cend_lookup())
            == std::vector({true, true, true}));
    REQUIRE(b->number_of_blocks() == 3);
    REQUIRE(b->rank() == 3);
    REQUIRE(b->is_transverse_block(0));
    REQUIRE(b->is_transverse_block(1));
    REQUIRE(b->is_transverse_block(2));
    REQUIRE(std::vector(b->cbegin(), b->cend())
            == std::vector<uint32_t>({0, 1, 2, 1, 0, 2, 1, 0, 2, 2}));
    delete b;
  }

  LIBSEMIGROUPS_TEST_CASE("Blocks",
                          "003",
                          "right blocks of bipartition",
                          "[quick]") {
    auto x = make<Bipartition>(
        {0, 1, 1, 1, 1, 2, 3, 2, 4, 4, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2});
    Blocks* b = x.right_blocks();
    REQUIRE(b == b);
    REQUIRE(!(b < b));
    REQUIRE(b->degree() == 10);
    REQUIRE(std::vector(b->cbegin_lookup(), b->cend_lookup())
            == std::vector({false, true, true, true, true}));
    REQUIRE(b->number_of_blocks() == 5);
    REQUIRE(b->rank() == 4);
    REQUIRE(!b->is_transverse_block(0));
    REQUIRE(b->is_transverse_block(1));
    REQUIRE(b->is_transverse_block(2));
    REQUIRE(b->is_transverse_block(3));
    REQUIRE(b->is_transverse_block(4));
    REQUIRE(std::vector(b->cbegin(), b->cend())
            == std::vector<uint32_t>({0, 1, 2, 1, 3, 3, 3, 1, 4, 1}));
    delete b;
  }

  LIBSEMIGROUPS_TEST_CASE("Blocks", "004", "copy [empty blocks]", "[quick]") {
    Blocks b = construct_blocks({}, {});
    Blocks c(b);
    REQUIRE(b.degree() == 0);
    REQUIRE(b.number_of_blocks() == 0);
    REQUIRE(b.rank() == 0);

    REQUIRE(c.degree() == 0);
    REQUIRE(c.number_of_blocks() == 0);
    REQUIRE(c.rank() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("Blocks",
                          "005",
                          "copy [non-empty blocks]",
                          "[quick]") {
    Blocks b = construct_blocks({0, 0, 1, 0, 2, 0, 1, 2, 2, 1, 0},
                                {false, true, false});
    Blocks c(b);

    REQUIRE(b.degree() == 11);
    REQUIRE(std::vector(b.cbegin_lookup(), b.cend_lookup())
            == std::vector({false, true, false}));
    REQUIRE(b.number_of_blocks() == 3);
    REQUIRE(b.rank() == 1);

    REQUIRE(c.degree() == 11);
    REQUIRE(std::equal(b.cbegin_lookup(),
                       b.cend_lookup(),
                       c.cbegin_lookup(),
                       c.cend_lookup()));
    REQUIRE(c.number_of_blocks() == 3);
    REQUIRE(c.rank() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Blocks", "006", "hash value", "[quick]") {
    Blocks b = construct_blocks({0, 0, 1, 0, 2, 0, 1, 2, 2, 1, 0},
                                {false, true, false});
    Blocks c = construct_blocks({0, 0, 1, 0, 2, 0, 1, 2, 2, 1, 0},
                                {false, true, true});
    REQUIRE(std::vector(b.cbegin_lookup(), b.cend_lookup())
            == std::vector({false, true, false}));
    REQUIRE(std::vector(c.cbegin_lookup(), c.cend_lookup())
            == std::vector({false, true, true}));
    REQUIRE(b != c);
    REQUIRE(b.hash_value() != c.hash_value());
    b = construct_blocks({}, {});
    REQUIRE(b.hash_value() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("Blocks", "007", "operator<", "[quick]") {
    Blocks b = construct_blocks({0, 0, 1, 0, 2, 0, 1, 2, 2, 1, 0},
                                {false, true, false});
    Blocks c = construct_blocks({0, 0, 1, 0, 2, 0, 1, 2, 2, 1, 0},
                                {false, true, true});
    REQUIRE(c < b);
    REQUIRE(!(b < c));

    c = construct_blocks({0, 1, 1, 0, 2, 0, 1, 2, 2, 1, 0},
                         {false, true, true});
    REQUIRE(b < c);
    REQUIRE(!(c < b));

    b = construct_blocks({}, {});
    REQUIRE(b < c);
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "008",
                          "mem fns 1",
                          "[quick][bipartition]") {
    auto x = make<Bipartition>(
        {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
    auto y = make<Bipartition>(
        {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2});
    auto z = make<Bipartition>(
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    REQUIRE(!(y == z));

    z.product_inplace_no_checks(x, y, 0);
    auto expected = make<Bipartition>(
        {0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1});
    REQUIRE(z == expected);

    expected = make<Bipartition>(
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 2, 1});
    z.product_inplace_no_checks(y, x, 0);
    REQUIRE(z == expected);

    REQUIRE(!(y < z));
    REQUIRE(x.degree() == 10);
    REQUIRE(y.degree() == 10);
    REQUIRE(z.degree() == 10);
    REQUIRE(Complexity<Bipartition>()(y) == 100);
    REQUIRE(Complexity<Bipartition>()(z) == 100);
    REQUIRE(Degree<Bipartition>()(y) == 10);
    REQUIRE(Degree<Bipartition>()(z) == 10);
    REQUIRE(Hash<Bipartition>()(y) != 0);
    REQUIRE(Hash<Bipartition>()(z) != 0);
    REQUIRE(z * One<Bipartition>()(z) == z);
    REQUIRE(One<Bipartition>()(10) * z == z);

    Product<Bipartition>()(x, y, z);
    REQUIRE(x == y * z);

    auto id = bipartition::one(x);
    z.product_inplace_no_checks(id, x, 0);
    REQUIRE(z == x);
    z.product_inplace_no_checks(x, id, 0);
    REQUIRE(z == x);
    z.product_inplace_no_checks(id, y, 0);
    REQUIRE(z == y);
    z.product_inplace_no_checks(y, id, 0);
    REQUIRE(z == y);

    REQUIRE(bipartition::underlying_partition(x)
            == std::vector<std::vector<int32_t>>(
                {{1},
                 {2, 3, 4, 5, 6, 7, 8, 9, -1, -2, -3, -4, -8, -10},
                 {10},
                 {-5, -9},
                 {-6, -7}}));
    REQUIRE(make<Bipartition>(bipartition::underlying_partition(x)) == x);
    REQUIRE(to_human_readable_repr(x)
            == "<bipartition of degree 10 with 5 blocks and rank 1>");
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "009",
                          "hash",
                          "[quick][bipartition][no-valgrind]") {
    auto x = make<Bipartition>(
        {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
    auto expected = x.hash_value();
    for (size_t i = 0; i < 1'000'000; i++) {
      REQUIRE(x.hash_value() == expected);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "010",
                          "mem fns 2",
                          "[quick][bipartition]") {
    auto x = make<Bipartition>(
        {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});

    REQUIRE(x.rank() == 3);
    REQUIRE(x.at(0) == 0);
    REQUIRE(x.at(6) == 1);
    REQUIRE(x.at(10) == 0);
    REQUIRE(x.number_of_blocks() == 5);
    REQUIRE(x.number_of_blocks() == 5);
    REQUIRE(x.number_of_blocks() == 5);
    REQUIRE(x.number_of_blocks() == 5);
    REQUIRE(x.number_of_left_blocks() == 3);
    REQUIRE(x.number_of_right_blocks() == 5);
    REQUIRE(x.is_transverse_block(0));
    REQUIRE(x.is_transverse_block(1));
    REQUIRE(x.is_transverse_block(2));
    REQUIRE(!x.is_transverse_block(3));
    REQUIRE(!x.is_transverse_block(4));

    auto y = make<Bipartition>(
        {0, 0, 1, 2, 3, 3, 0, 4, 1, 1, 0, 0, 0, 0, 0, 0, 1, 2, 0, 1});

    Blocks* a = x.left_blocks();
    Blocks* b = y.right_blocks();
    REQUIRE(*a == *b);
    delete a;
    delete b;
    a = x.right_blocks();
    b = y.left_blocks();
    REQUIRE(*a == *b);
    delete a;
    delete b;

    x = make<Bipartition>(
        {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
    x.set_number_of_blocks(5);
    REQUIRE(x.number_of_blocks() == 5);

    x = make<Bipartition>(
        {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
    x.set_number_of_left_blocks(3);
    REQUIRE(x.number_of_left_blocks() == 3);
    REQUIRE(x.number_of_right_blocks() == 5);
    REQUIRE(x.number_of_blocks() == 5);

    x = make<Bipartition>(
        {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
    x.set_rank(3);
    REQUIRE(x.rank() == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "011",
                          "delete/copy",
                          "[quick][bipartition]") {
    auto x = make<Bipartition>({0, 0, 0, 0});
    auto y(x);

    auto expected = make<Bipartition>({0, 0, 0, 0});
    REQUIRE(y == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "012",
                          "degree 0",
                          "[quick][bipartition]") {
    auto x = make<Bipartition>(std::vector<uint32_t>({}));
    REQUIRE(x.number_of_blocks() == 0);
    REQUIRE(x.number_of_left_blocks() == 0);

    Blocks* b = x.left_blocks();
    REQUIRE(b->degree() == 0);
    REQUIRE(b->number_of_blocks() == 0);
    delete b;

    b = x.right_blocks();
    REQUIRE(b->degree() == 0);
    REQUIRE(b->number_of_blocks() == 0);
    delete b;
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "013",
                          "exceptions",
                          "[quick][bipartition]") {
    REQUIRE_NOTHROW(Bipartition(std::vector<uint32_t>()));
    REQUIRE_THROWS_AS(make<Bipartition>({0}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Bipartition>({1, 0}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "014",
                          "convenience constructor",
                          "[quick][bipartition]") {
    auto xx = make<Bipartition>(
        {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});

    auto x = make<Bipartition>({{1, 2, 3, 4, 5, 6, 9, -1, -2, -7},
                                {7, 10, -3, -9, -10},
                                {8, -4},
                                {-5, -6},
                                {-8}});

    REQUIRE_THROWS_AS(make<Bipartition>({{1, 2, 3, 4, 5, 6, 9, 0, -2, -7},
                                         {7, 10, -3, -9, -10},
                                         {8, -4},
                                         {-5, -6},
                                         {-8}}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Bipartition>({{1, 2, 3, 4, 5, 6, 9, -2, -7},
                                         {7, 10, -3, -9, -10},
                                         {8, -4},
                                         {-5, -6},
                                         {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(make<Bipartition>({{1, 2, 3, 4, 5, 6, 9, -2, -7},
                                         {7, 10, -3, -9, -10},
                                         {8, -4},
                                         {-5, 0x40000000},
                                         {-8}}),
                      LibsemigroupsException);
    REQUIRE(x == xx);

    REQUIRE(x.rank() == 3);
    REQUIRE(x.at(0) == 0);
    REQUIRE(x.at(6) == 1);
    REQUIRE(x.at(10) == 0);
    REQUIRE(x.number_of_blocks() == 5);
    REQUIRE(x.number_of_blocks() == 5);
    REQUIRE(x.number_of_blocks() == 5);
    REQUIRE(x.number_of_blocks() == 5);
    REQUIRE(x.number_of_left_blocks() == 3);
    REQUIRE(x.number_of_right_blocks() == 5);
    REQUIRE(x.is_transverse_block(0));
    REQUIRE(x.is_transverse_block(1));
    REQUIRE(x.is_transverse_block(2));
    REQUIRE(!x.is_transverse_block(3));
    REQUIRE(!x.is_transverse_block(4));

    auto yy = make<Bipartition>(
        {0, 0, 1, 2, 3, 3, 0, 4, 1, 1, 0, 0, 0, 0, 0, 0, 1, 2, 0, 1});

    auto y = make<Bipartition>({{1, 2, 7, -1, -2, -3, -4, -5, -6, -9},
                                {3, 9, 10, -7, -10},
                                {4, -8},
                                {5, 6},
                                {8}});

    REQUIRE(y == yy);

    Blocks* a = x.left_blocks();
    Blocks* b = y.right_blocks();
    REQUIRE(*a == *b);
    delete a;
    delete b;
    a = x.right_blocks();
    b = y.left_blocks();
    REQUIRE(*a == *b);
    delete a;
    delete b;

    xx = make<Bipartition>(
        {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
    x = make<Bipartition>({{1, 2, 3, 4, 5, 6, 9, -1, -2, -7},
                           {7, 10, -3, -9, -10},
                           {8, -4},
                           {-5, -6},
                           {-8}});
    REQUIRE(x == xx);
    x.set_number_of_blocks(5);
    REQUIRE(x.number_of_blocks() == 5);

    xx = make<Bipartition>(
        {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
    x = make<Bipartition>({{1, 2, 3, 4, 5, 6, 9, -1, -2, -7},
                           {7, 10, -3, -9, -10},
                           {8, -4},
                           {-5, -6},
                           {-8}});
    REQUIRE(x == xx);
    x.set_number_of_left_blocks(3);
    REQUIRE(x.number_of_left_blocks() == 3);
    REQUIRE(x.number_of_right_blocks() == 5);
    REQUIRE(x.number_of_blocks() == 5);

    x = make<Bipartition>({{1, 2, 3, 4, 5, 6, 9, -1, -2, -7},
                           {7, 10, -3, -9, -10},
                           {8, -4},
                           {-5, -6},
                           {-8}});
    x.set_rank(3);
    REQUIRE(x.rank() == 3);

    Bipartition xxx;
    REQUIRE_NOTHROW(bipartition::throw_if_invalid(xxx));
    REQUIRE(x != xxx);
    REQUIRE(xx != xxx);
    REQUIRE(xx > xxx);
    REQUIRE(xxx < xx);
    REQUIRE(xxx <= xx);
    REQUIRE(xx >= xxx);

    // Check for odd degree
    REQUIRE_THROWS_AS(make<Bipartition>({0, 1, 2}), LibsemigroupsException);

    REQUIRE_THROWS_AS(make<Bipartition>({{0, 2, 3, 4, 5, 6, 9, -1, -2, -7},
                                         {7, 10, -3, -9, -10},
                                         {8, -4},
                                         {-5, -6},
                                         {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(make<Bipartition>({{1, 2, 3, 4, 5, 6, 9, 11, -1, -2, -7},
                                         {7, 10, -3, -9, -10},
                                         {8, -4},
                                         {-5, -6},
                                         {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(make<Bipartition>({{1, 2, 3, 4, 5, 6, 11, -1, -2, -7},
                                         {7, 10, -3, -9, -10},
                                         {8, -4},
                                         {-5, -6},
                                         {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(make<Bipartition>({{1, 2, 3, 4, 5, 6, -11, -1, -2, -7},
                                         {7, 10, -3, -9, -10},
                                         {8, -4},
                                         {-5, -6},
                                         {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(make<Bipartition>({{0, 2, 3, 4, 5, 6, 9, -1},
                                         {7, 10, -3, -9, -10},
                                         {8, -4},
                                         {-5, -6},
                                         {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(make<Bipartition>({{0, 2, 3, 4, 5, 6, 9, -1, -2},
                                         {7, 10, -3, -9, -10},
                                         {8, -4},
                                         {-5, -6},
                                         {-8}}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "015",
                          "force copy constructor over move constructor",
                          "[quick][bipartition]") {
    auto x = make<Bipartition>(
        {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
    auto y = make<Bipartition>(
        {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2});
    auto z = make<Bipartition>(
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    REQUIRE(!(y == z));

    z.product_inplace_no_checks(x, y);
    auto expected = make<Bipartition>(
        {0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1});
    REQUIRE(z == expected);

    expected = make<Bipartition>(
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 2, 1});
    z.product_inplace_no_checks(y, x);
    REQUIRE(z == expected);

    REQUIRE(!(y < z));
    REQUIRE(x.degree() == 10);
    REQUIRE(y.degree() == 10);
    REQUIRE(z.degree() == 10);
    REQUIRE(Complexity<Bipartition>()(x) == 100);
    REQUIRE(Complexity<Bipartition>()(y) == 100);
    REQUIRE(Complexity<Bipartition>()(z) == 100);

    auto id = bipartition::one(x);
    z.product_inplace_no_checks(id, x, 0);
    REQUIRE(z == x);
    z.product_inplace_no_checks(x, id, 0);
    REQUIRE(z == x);
    z.product_inplace_no_checks(id, y, 0);
    REQUIRE(z == y);
    z.product_inplace_no_checks(y, id, 0);
    REQUIRE(z == y);

    auto copy1 = x;
    REQUIRE(x == copy1);
    auto copy2 = std::move(x);
    REQUIRE(copy1 == copy2);
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition", "016", "adapters", "[quick][bipart]") {
    auto x = make<Bipartition>(
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 2, 1});
    REQUIRE_NOTHROW(IncreaseDegree<Bipartition>()(x, 0));
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition", "017", "bug", "[quick][bipart]") {
    auto x = make<Bipartition>({{1, -2, -3}, {-1}, {2, 3}});
    REQUIRE_NOTHROW(bipartition::throw_if_invalid(x));
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "018",
                          "random x 1",
                          "[quick][bipart]") {
    auto x = bipartition::random(10);
    REQUIRE(x.degree() == 10);
    REQUIRE_NOTHROW(bipartition::throw_if_invalid(x));

    std::unordered_map<Bipartition, size_t, Hash<Bipartition>> map;

    for (size_t i = 0; i < 3417; ++i) {
      auto [it, _] = map.emplace(bipartition::random(3), 0);
      it->second++;
    }
    REQUIRE(map.size() == 203);

    // std::vector<size_t> occur(map.size(), 0);
    // std::transform(map.begin(), map.end(), occur.begin(), [](auto const&
    // pair) {
    //   return pair.second;
    // });
    // std::sort(occur.begin(), occur.end());
    // REQUIRE(occur == std::vector<size_t>());

    REQUIRE_NOTHROW(bipartition::random(0));
    REQUIRE(bipartition::random(0) == Bipartition());
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "019",
                          "random x 2",
                          "[quick][bipart][no-valgrind]") {
    std::unordered_set<Bipartition, Hash<Bipartition>> map;
    for (size_t i = 0; i < 1000; ++i) {
      map.emplace(bipartition::random(100));
    }
    REQUIRE(map.size() == 1000);
    // REQUIRE(to_human_readable_repr(bipartition::random(100)) == "");
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "020",
                          "random x 3",
                          "[quick][bipart][no-valgrind]") {
    std::unordered_set<Bipartition, Hash<Bipartition>> map;

    for (size_t i = 0; i < 82'138; ++i) {
      map.emplace(bipartition::random(4));
    }
    REQUIRE(map.size() == 4'140);
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "021",
                          "uniform_random",
                          "[quick][bipart]") {
    REQUIRE_EXCEPTION_MSG(
        bipartition::uniform_random(1000),
        "the degree (1000) of the argument <x> (a bipartition) is too large, "
        "please use random(x) instead");
  }

  // We would expect this test to fail about ~4% of the time due to the nature
  // of the statistical test being used, but this is too flaky to include in the
  // CI
  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "022",
                          "uniform_random chi-squared test x1",
                          "[fail][bipart]") {
    // The third parameter here is a 99th percentile for the chi squared
    // distribution with 1, 14, 202 and 1439 degrees of freedom respectively.
    test_uniform_bipartition(1, 2, 6.634897, 500);
    test_uniform_bipartition(2, 15, 29.14124, 500);
    test_uniform_bipartition(3, 203, 251.6773, 500);
    test_uniform_bipartition(4, 4140, 4353.596, 30);
  }

  // We would expect this test to fail about ~1% of the time due to the nature
  // of the statistical test being used, but this is too flaky to include in the
  // CI
  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "023",
                          "uniform_random chi-squared test x2",
                          "[fail][bipart]") {
    test_uniform_bipartition(5, 115'975, 117'097.3, 10);
  }

}  // namespace libsemigroups
