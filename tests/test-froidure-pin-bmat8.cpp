//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2'019 James D. Mitchell
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

#include "libsemigroups/to-froidure-pin.hpp"
#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "catch_amalgamated.hpp"  // for LIBSEMIGROUPS_TEST_CASE_V3
#include "test-main.hpp"

#include "libsemigroups/bmat8.hpp"         // for BMat8
#include "libsemigroups/config.hpp"        // for LIBSEMIGROUPS_SIZEOF_VO...
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin, FroidurePi...
#include "libsemigroups/types.hpp"         // for word_type

namespace libsemigroups {
  using namespace literals;

  struct LibsemigroupsException;

  bool constexpr REPORT = false;

#if (LIBSEMIGROUPS_SIZEOF_VOID_P == 8)

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "015",
                             "regular boolean mat monoid 4 - BMat8",
                             "[quick][froidure-pin][bmat8][no-valgrind]") {
    auto               rg = ReportGuard(false);
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    auto S = to_froidure_pin(gens);

    REQUIRE(S.current_max_word_length() == 1);
    REQUIRE(!S.finished());
    REQUIRE(!S.started());
    REQUIRE(S.current_position(S.generator(0) * S.generator(3)) == UNDEFINED);
    REQUIRE(S.current_position(BMat8({{1, 0, 0, 1, 1},
                                      {0, 1, 0, 0, 1},
                                      {1, 0, 1, 0, 1},
                                      {0, 0, 1, 0, 1},
                                      {0, 0, 0, 0, 0}}))
            == UNDEFINED);
    REQUIRE(S.current_size() == 4);
    REQUIRE(S.current_number_of_rules() == 0);
    REQUIRE(S.current_length(0) == 1);
    REQUIRE(S.length(5) == 2);

    REQUIRE(S.size() == 63'904);
    REQUIRE(S.number_of_idempotents() == 2'360);
    REQUIRE(froidure_pin::current_position(S, 012012_w) == 378);
    REQUIRE(froidure_pin::to_element(S, 012012_w)
            == BMat8({{1, 0, 0, 1}, {0, 1, 0, 0}, {1, 0, 1, 0}, {0, 0, 1, 0}}));
    REQUIRE(S.current_max_word_length() == 21);
    REQUIRE(S.degree() == 8);
    REQUIRE(S.number_of_generators() == 4);
    REQUIRE(S.generator(0)
            == BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    REQUIRE(S.generator(1)
            == BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    REQUIRE(S.generator(2)
            == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
    REQUIRE(S.generator(3)
            == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
    REQUIRE(S.finished());
    REQUIRE(S.started());
    REQUIRE(S.current_position(S.generator(0) * S.generator(3)) == 7);
    REQUIRE(S.current_position(BMat8({{1, 0, 0, 1, 1},
                                      {0, 1, 0, 0, 1},
                                      {1, 0, 1, 0, 1},
                                      {0, 0, 1, 0, 1},
                                      {0, 0, 0, 0, 0}}))
            == UNDEFINED);
    REQUIRE(S.current_number_of_rules() == 13'716);
    REQUIRE(S.prefix(0) == UNDEFINED);
    REQUIRE(S.suffix(0) == UNDEFINED);
    REQUIRE(S.first_letter(0) == 0);
    REQUIRE(S.final_letter(0) == 0);
    REQUIRE(S.batch_size() == 8'192);
    REQUIRE(S.current_length(0) == 1);
    REQUIRE(S.current_length(7) == 2);
    REQUIRE(S.current_length(63'903) == 21);
    REQUIRE(S.length(7) == 2);
    REQUIRE(S.length(63'903) == 21);
    REQUIRE(froidure_pin::product_by_reduction(S, 0, 3) == 7);
    REQUIRE(S.fast_product(0, 3) == 7);
    REQUIRE(S.position_of_generator(0) == 0);
    REQUIRE(S.position_of_generator(1) == 1);
    REQUIRE(S.position_of_generator(2) == 2);
    REQUIRE(S.position_of_generator(3) == 3);
    REQUIRE(!S.is_idempotent(0));
    REQUIRE(S.is_idempotent(3));
    REQUIRE(!S.is_idempotent(7));
    REQUIRE(S.number_of_rules() == 13'716);
    REQUIRE(S.contains(S.generator(1)));
    REQUIRE(!S.contains(BMat8({{1, 0, 0, 1, 1},
                               {0, 1, 0, 0, 1},
                               {1, 0, 1, 0, 1},
                               {0, 0, 1, 0, 1},
                               {0, 0, 0, 0, 0}})));
    REQUIRE(S.position(S.generator(1)) == 1);
    REQUIRE(S.position(S.generator(0) * S.generator(3)) == 7);
    REQUIRE(S.position(BMat8({{1, 0, 0, 1, 1},
                              {0, 1, 0, 0, 1},
                              {1, 0, 1, 0, 1},
                              {0, 0, 1, 0, 1},
                              {0, 0, 0, 0, 0}}))
            == UNDEFINED);

    REQUIRE(S.sorted_position(BMat8({{1, 0, 0, 1, 1},
                                     {0, 1, 0, 0, 1},
                                     {1, 0, 1, 0, 1},
                                     {0, 0, 1, 0, 1},
                                     {0, 0, 0, 0, 0}}))
            == UNDEFINED);
    REQUIRE(S.sorted_position(S.generator(0)) == 18'185);
    REQUIRE(S.sorted_position(S.generator(3)) == 33'066);
    REQUIRE(S.sorted_position(S.generator(0) * S.generator(3)) == 18'184);
    REQUIRE(S.to_sorted_position(0) == 18'185);
    REQUIRE(S.to_sorted_position(3) == 33'066);
    REQUIRE(S.to_sorted_position(7) == 18'184);

    REQUIRE(S.at(7) == S.generator(0) * S.generator(3));
    REQUIRE(S[7] == S[0] * S[3]);

    REQUIRE(S.sorted_at(18'185) == S.at(0));
    REQUIRE(S.sorted_at(33'066) == S.at(3));
    REQUIRE(S.sorted_at(18'184) == S.generator(0) * S.generator(3));

    REQUIRE(S.right_cayley_graph().target(0, 3) == 7);
    REQUIRE(S.right_cayley_graph().target(0, 3) == 7);

    REQUIRE(S.left_cayley_graph().target(0, 3) == 7);
    REQUIRE(S.left_cayley_graph().target(0, 3) == 7);

    REQUIRE(froidure_pin::minimal_factorisation(S, 378) == 012012_w);
    REQUIRE(S.current_length(378) == 6);

    REQUIRE(froidure_pin::minimal_factorisation(S, S.at(378)) == 012012_w);

    REQUIRE_THROWS_AS(
        froidure_pin::minimal_factorisation(S,
                                            BMat8({{1, 0, 0, 1, 1},
                                                   {0, 1, 0, 0, 1},
                                                   {1, 0, 1, 0, 1},
                                                   {0, 0, 1, 0, 1},
                                                   {0, 0, 0, 0, 0}})),
        LibsemigroupsException);

    REQUIRE_THROWS_AS(froidure_pin::minimal_factorisation(S, 1'000'000),
                      LibsemigroupsException);

    REQUIRE(froidure_pin::factorisation(S, 378) == 012012_w);
    REQUIRE(S.current_length(378) == 6);

    REQUIRE(froidure_pin::factorisation(S, S.at(378)) == 012012_w);

    REQUIRE_THROWS_AS(froidure_pin::factorisation(S,
                                                  BMat8({{1, 0, 0, 1, 1},
                                                         {0, 1, 0, 0, 1},
                                                         {1, 0, 1, 0, 1},
                                                         {0, 0, 1, 0, 1},
                                                         {0, 0, 0, 0, 0}})),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(froidure_pin::factorisation(S, 1'000'000),
                      LibsemigroupsException);

    auto it = S.cbegin_rules();
    REQUIRE(*it == relation_type(22_w, 2_w));
    ++it;
    REQUIRE(*it == relation_type(30_w, 03_w));
    ++it;
    REQUIRE(*it == relation_type(32_w, 3_w));

    size_t pos = 0;
    for (auto it2 = S.cbegin(); it2 < S.cend(); ++it2) {
      REQUIRE(S.position(*it2) == pos);
      pos++;
    }
    REQUIRE(pos == S.size());
    REQUIRE((froidure_pin::rules(S) | rx::count()) == S.number_of_rules());
    REQUIRE(S.number_of_rules() == 13'716);

    // Copy - after run
    FroidurePin T(S);
    REQUIRE(T.size() == 63'904);
    REQUIRE(T.number_of_idempotents() == 2'360);
    REQUIRE(froidure_pin::current_position(T, 012012_w) == 378);
    REQUIRE(froidure_pin::to_element(T, 012012_w)
            == BMat8({{1, 0, 0, 1}, {0, 1, 0, 0}, {1, 0, 1, 0}, {0, 0, 1, 0}}));
    REQUIRE(T.current_max_word_length() == 21);
    REQUIRE(T.degree() == 8);
    REQUIRE(T.number_of_generators() == 4);
    REQUIRE(T.generator(0)
            == BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    REQUIRE(T.generator(1)
            == BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    REQUIRE(T.generator(2)
            == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
    REQUIRE(T.generator(3)
            == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
    REQUIRE(T.finished());
    REQUIRE(T.started());
  }
#endif

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "016",
                             "exception zero generators given - BMat8",
                             "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens;

    REQUIRE_NOTHROW(to_froidure_pin(gens));
  }

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "017",
                             "exception to_element - BMat8",
                             "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    auto S = to_froidure_pin(gens);

    REQUIRE(!S.contains_one());
    REQUIRE_THROWS_AS(froidure_pin::to_element(S, {}), LibsemigroupsException);
    REQUIRE_NOTHROW(froidure_pin::to_element(S, {0}));
    REQUIRE_NOTHROW(froidure_pin::to_element(S, {0, 3, 0, 3, 1}));
    REQUIRE_THROWS_AS(froidure_pin::to_element(S, {0, 1, 0, 4}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "018",
                             "exception prefix - BMat8",
                             "[quick][froidure-pin][bmat8][no-valgrind]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    auto S = to_froidure_pin(gens);

    for (size_t i = 0; i < S.size(); ++i) {
      REQUIRE_NOTHROW(S.prefix(i));
      REQUIRE_THROWS_AS(S.prefix(i + S.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "020",
                             "exception first_letter - BMat8",
                             "[quick][froidure-pin][bmat8][no-valgrind]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    auto S = to_froidure_pin(gens);

    for (size_t i = 0; i < S.size(); ++i) {
      REQUIRE_NOTHROW(S.first_letter(i));
      REQUIRE_THROWS_AS(S.first_letter(i + S.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "022",
                             "exception current_length - BMat8",
                             "[quick][froidure-pin][bmat8][no-valgrind]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    auto S = to_froidure_pin(gens);

    for (size_t i = 0; i < S.size(); ++i) {
      REQUIRE_NOTHROW(S.current_length(i));
      REQUIRE_THROWS_AS(S.current_length(i + S.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "023",
                             "exception product_by_reduction - BMat8",
                             "[quick][froidure-pin][bmat8][no-valgrind]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    auto S = to_froidure_pin(gens);

    for (size_t i = 1; i < S.size(); ++i) {
      for (size_t j = 1; j < S.size(); ++j) {
        REQUIRE_NOTHROW(froidure_pin::product_by_reduction(S, i, j));
        REQUIRE_THROWS_AS(
            froidure_pin::product_by_reduction(S, i + S.size(), j),
            LibsemigroupsException);
        REQUIRE_THROWS_AS(
            froidure_pin::product_by_reduction(S, i, j + S.size()),
            LibsemigroupsException);
        REQUIRE_THROWS_AS(
            froidure_pin::product_by_reduction(S, i + S.size(), j + S.size()),
            LibsemigroupsException);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "024",
                             "exception fast_product - BMat8",
                             "[quick][froidure-pin][bmat8][024]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    auto S = to_froidure_pin(gens);

    for (size_t i = 1; i < S.size(); ++i) {
      for (size_t j = 1; j < S.size(); ++j) {
        REQUIRE_NOTHROW(S.fast_product(i, j));
        REQUIRE_THROWS_AS(S.fast_product(i + S.size(), j),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(S.fast_product(i, j + S.size()),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(S.fast_product(i + S.size(), j + S.size()),
                          LibsemigroupsException);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "025",
                             "exception is_idempotent - BMat8",
                             "[quick][froidure-pin][bmat8][no-valgrind]") {
    auto S = to_froidure_pin(
        {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});

    REQUIRE(S.size() == 63'904);

    size_t nr = 0;
    for (size_t i = 0; i < S.size(); ++i) {
      nr += S.is_idempotent(i);
    }
    REQUIRE(nr == S.number_of_idempotents());
    REQUIRE(nr == 2'360);
    REQUIRE((froidure_pin::idempotents(S) | rx::count()) == 2'360);

    REQUIRE_THROWS_AS(S.is_idempotent(63'904), LibsemigroupsException);

    auto nf = froidure_pin::normal_forms(S);
    for (size_t i = 0; i < 10'000; ++i) {
      nf.next();
    }
    REQUIRE((nf | rx::take(20) | rx::to_vector())
            == std::vector<word_type>(
                {1102312121_w, 1102313121_w, 1103111202_w, 1103112021_w,
                 1103112120_w, 1103112123_w, 1103120120_w, 1103120121_w,
                 1103120210_w, 1103120211_w, 1103120212_w, 1103121112_w,
                 1103121120_w, 1103121121_w, 1103121131_w, 1103121201_w,
                 1103121210_w, 1103121211_w, 1103121212_w, 1103121213_w}));
  }

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "026",
                             "copy constructor - BMat8",
                             "[quick][froidure-pin][bmat8][no-valgrind]") {
    auto rg = ReportGuard(REPORT);
    auto S  = to_froidure_pin(
        {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
          BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
          BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
          BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});

    SECTION("fully enumerated") {
      REQUIRE(S.size() == 63'904);
      FroidurePin T(S);  // copy
      REQUIRE(T.size() == 63'904);
    }
    SECTION("partially enumerated") {
      S.enumerate(8'192);
      REQUIRE(!S.finished());
      FroidurePin T(S);  // copy
      REQUIRE(T.size() == 63'904);
    }
    SECTION("not enumerated") {
      FroidurePin T(S);  // copy
      REQUIRE(T.size() == 63'904);
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("FroidurePin",
                             "027",
                             "cbegin/end_rules - BMat8",
                             "[quick][froidure-pin][bmat8]") {
    FroidurePin<BMat8> S;
    S.add_generator(
        BMat8({{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}}));
    S.add_generator(
        BMat8({{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}));
    S.add_generator(
        BMat8({{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}}));
    S.add_generator(
        BMat8({{0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}}));
    REQUIRE(S.size() == 4);
    REQUIRE(std::vector<relation_type>(S.cbegin_rules(), S.cend_rules())
            == std::vector<relation_type>({{00_w, 0_w},
                                           {01_w, 1_w},
                                           {02_w, 2_w},
                                           {03_w, 3_w},
                                           {10_w, 0_w},
                                           {11_w, 1_w},
                                           {12_w, 2_w},
                                           {13_w, 3_w},
                                           {20_w, 0_w},
                                           {21_w, 1_w},
                                           {22_w, 2_w},
                                           {23_w, 3_w},
                                           {30_w, 0_w},
                                           {31_w, 1_w},
                                           {32_w, 2_w},
                                           {33_w, 3_w}}));
  }
}  // namespace libsemigroups
