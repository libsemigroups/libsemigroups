//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "catch.hpp"                       // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/bmat8.hpp"         // for BMat8
#include "libsemigroups/config.hpp"        // for LIBSEMIGROUPS_SIZEOF_VO...
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin, FroidurePi...
#include "libsemigroups/todd-coxeter.hpp"  // for word_type
#include "libsemigroups/types.hpp"         // for word_type
#include "test-main.hpp"

namespace libsemigroups {
  struct LibsemigroupsException;

  bool constexpr REPORT = false;

#if (LIBSEMIGROUPS_SIZEOF_VOID_P == 8)

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "015",
                          "(BMat8) regular boolean mat monoid 4",
                          "[quick][froidure-pin][bmat8]") {
    auto               rg = ReportGuard(false);
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8> S(gens);

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

    REQUIRE(S.size() == 63904);
    REQUIRE(S.number_of_idempotents() == 2360);
    REQUIRE(S.current_position({0, 1, 2, 0, 1, 2}) == 378);
    REQUIRE(S.word_to_element({0, 1, 2, 0, 1, 2})
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
    REQUIRE(S.current_number_of_rules() == 13716);
    REQUIRE(S.prefix(0) == UNDEFINED);
    REQUIRE(S.suffix(0) == UNDEFINED);
    REQUIRE(S.first_letter(0) == 0);
    REQUIRE(S.final_letter(0) == 0);
    REQUIRE(S.batch_size() == 8192);
    REQUIRE(S.current_length(0) == 1);
    REQUIRE(S.current_length(7) == 2);
    REQUIRE(S.current_length(63903) == 21);
    REQUIRE(S.length(7) == 2);
    REQUIRE(S.length(63903) == 21);
    REQUIRE(S.product_by_reduction(0, 3) == 7);
    REQUIRE(S.fast_product(0, 3) == 7);
    REQUIRE(S.current_position(0) == 0);
    REQUIRE(S.current_position(1) == 1);
    REQUIRE(S.current_position(2) == 2);
    REQUIRE(S.current_position(3) == 3);
    REQUIRE(!S.is_idempotent(0));
    REQUIRE(S.is_idempotent(3));
    REQUIRE(!S.is_idempotent(7));
    REQUIRE(S.number_of_rules() == 13716);
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
    REQUIRE(S.sorted_position(S.generator(0)) == 18185);
    REQUIRE(S.sorted_position(S.generator(3)) == 33066);
    REQUIRE(S.sorted_position(S.generator(0) * S.generator(3)) == 18184);
    REQUIRE(S.position_to_sorted_position(0) == 18185);
    REQUIRE(S.position_to_sorted_position(3) == 33066);
    REQUIRE(S.position_to_sorted_position(7) == 18184);

    REQUIRE(S.at(7) == S.generator(0) * S.generator(3));
    REQUIRE(S[7] == S[0] * S[3]);

    REQUIRE(S.sorted_at(18185) == S.at(0));
    REQUIRE(S.sorted_at(33066) == S.at(3));
    REQUIRE(S.sorted_at(18184) == S.generator(0) * S.generator(3));

    REQUIRE(S.right(0, 3) == 7);
    REQUIRE(S.right(0, 3) == 7);

    REQUIRE(S.left(0, 3) == 7);
    REQUIRE(S.left(0, 3) == 7);

    word_type w;
    S.minimal_factorisation(w, 378);
    REQUIRE(w == word_type({0, 1, 2, 0, 1, 2}));
    REQUIRE(S.current_length(378) == 6);

    REQUIRE(S.minimal_factorisation(S.at(378))
            == word_type({0, 1, 2, 0, 1, 2}));

    REQUIRE_THROWS_AS(S.minimal_factorisation(BMat8({{1, 0, 0, 1, 1},
                                                     {0, 1, 0, 0, 1},
                                                     {1, 0, 1, 0, 1},
                                                     {0, 0, 1, 0, 1},
                                                     {0, 0, 0, 0, 0}})),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(S.minimal_factorisation(1000000), LibsemigroupsException);

    w.clear();
    S.factorisation(w, 378);
    REQUIRE(w == word_type({0, 1, 2, 0, 1, 2}));
    REQUIRE(S.current_length(378) == 6);

    REQUIRE(S.factorisation(S.at(378)) == word_type({0, 1, 2, 0, 1, 2}));

    REQUIRE_THROWS_AS(S.factorisation(BMat8({{1, 0, 0, 1, 1},
                                             {0, 1, 0, 0, 1},
                                             {1, 0, 1, 0, 1},
                                             {0, 0, 1, 0, 1},
                                             {0, 0, 0, 0, 0}})),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(S.factorisation(1000000), LibsemigroupsException);

    auto it = S.cbegin_rules();
    REQUIRE(*it == relation_type({2, 2}, {2}));
    ++it;
    REQUIRE(*it == relation_type({3, 0}, {0, 3}));
    ++it;
    REQUIRE(*it == relation_type({3, 2}, {3}));

    size_t pos = 0;
    for (auto it2 = S.cbegin(); it2 < S.cend(); ++it2) {
      REQUIRE(S.position(*it2) == pos);
      pos++;
    }
    REQUIRE(pos == S.size());
    std::vector<relation_type> v(S.cbegin_rules(), S.cend_rules());
    REQUIRE(v.size() == S.number_of_rules());
    REQUIRE(v.size() == 13716);

    // The next works but is a bit slow
    // congruence::ToddCoxeter tc(congruence_kind::twosided);
    // tc.set_number_of_generators(S.number_of_generators());
    // for (auto const& rel : v) {
    //   tc.add_pair(rel.first, rel.second);
    // }

    // REQUIRE(tc.number_of_classes() == 63904);
    // REQUIRE(tc.number_of_classes() == S.size());

    // Copy - after run
    FroidurePin<BMat8> T(S);
    REQUIRE(T.size() == 63904);
    REQUIRE(T.number_of_idempotents() == 2360);
    REQUIRE(T.current_position({0, 1, 2, 0, 1, 2}) == 378);
    REQUIRE(T.word_to_element({0, 1, 2, 0, 1, 2})
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

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "016",
                          "(BMat8) exception zero generators given",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens;

    REQUIRE_NOTHROW(FroidurePin<BMat8>(gens));
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "017",
                          "(BMat8) exception word_to_element",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    FroidurePin<BMat8> S(gens);

    REQUIRE_THROWS_AS(S.word_to_element({}), LibsemigroupsException);
    REQUIRE_NOTHROW(S.word_to_element({0}));
    REQUIRE_NOTHROW(S.word_to_element({0, 3, 0, 3, 1}));
    REQUIRE_THROWS_AS(S.word_to_element({0, 1, 0, 4}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "018",
                          "(BMat8) exception prefix",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);

    for (size_t i = 0; i < S.size(); ++i) {
      REQUIRE_NOTHROW(S.prefix(i));
      REQUIRE_THROWS_AS(S.prefix(i + S.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "019",
                          "(BMat8) exception suffix",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "020",
                          "(BMat8) exception first_letter",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);

    for (size_t i = 0; i < S.size(); ++i) {
      REQUIRE_NOTHROW(S.first_letter(i));
      REQUIRE_THROWS_AS(S.first_letter(i + S.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "021",
                          "(BMat8) exception final_letter",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "022",
                          "(BMat8) exception current_length",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);

    for (size_t i = 0; i < S.size(); ++i) {
      REQUIRE_NOTHROW(S.current_length(i));
      REQUIRE_THROWS_AS(S.current_length(i + S.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "023",
                          "(BMat8) exception product_by_reduction",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);

    for (size_t i = 1; i < S.size(); ++i) {
      for (size_t j = 1; j < S.size(); ++j) {
        REQUIRE_NOTHROW(S.product_by_reduction(i, j));
        REQUIRE_THROWS_AS(S.product_by_reduction(i + S.size(), j),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(S.product_by_reduction(i, j + S.size()),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(S.product_by_reduction(i + S.size(), j + S.size()),
                          LibsemigroupsException);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "024",
                          "(BMat8) exception fast_product",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);

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

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "025",
                          "(BMat8) exception is_idempotent",
                          "[quick][froidure-pin][bmat8][no-valgrind]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    FroidurePin<BMat8> S(gens);

    // S has size 63904
    for (size_t i = 0; i < 63904; ++i) {
      REQUIRE_NOTHROW(S.is_idempotent(i));
    }
    for (size_t i = 0; i < 20; ++i) {
      REQUIRE_THROWS_AS(S.is_idempotent(63904 + i), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "026",
                          "(BMat8) copy constructor",
                          "[quick][froidure-pin][bmat8][no-valgrind]") {
    auto               rg = ReportGuard(REPORT);
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    FroidurePin<BMat8> S(gens);
    SECTION("fully enumerated") {
      REQUIRE(S.size() == 63904);
      FroidurePin<BMat8> T(S);  // copy
      REQUIRE(T.size() == 63904);
    }
    SECTION("partially enumerated") {
      S.enumerate(8192);
      REQUIRE(!S.finished());
      FroidurePin<BMat8> T(S);  // copy
      REQUIRE(T.size() == 63904);
    }
    SECTION("not enumerated") {
      FroidurePin<BMat8> T(S);  // copy
      REQUIRE(T.size() == 63904);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "027",
                          "(BMat8) cbegin/end_rules",
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
            == std::vector<relation_type>({{{0, 0}, {0}},
                                           {{0, 1}, {1}},
                                           {{0, 2}, {2}},
                                           {{0, 3}, {3}},
                                           {{1, 0}, {0}},
                                           {{1, 1}, {1}},
                                           {{1, 2}, {2}},
                                           {{1, 3}, {3}},
                                           {{2, 0}, {0}},
                                           {{2, 1}, {1}},
                                           {{2, 2}, {2}},
                                           {{2, 3}, {3}},
                                           {{3, 0}, {0}},
                                           {{3, 1}, {1}},
                                           {{3, 2}, {2}},
                                           {{3, 3}, {3}}}));
  }

  // LIBSEMIGROUPS_TEST_CASE("FroidurePin",
  //                         "028",
  //                         "(BMat8) find an element",
  //                         "[quick][froidure-pin][bmat8]") {
  //   FroidurePin<BMat8> S({BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}),
  //                         BMat8({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
  //                         BMat8({{1, 0, 0}, {0, 1, 0}, {1, 0, 1}}),
  //                         BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 0}}),
  //                         BMat8({{1, 1, 0}, {1, 0, 1}, {0, 1, 1}})});
  //   REQUIRE(S.size() == 512);
  //   BMat8 y({{1, 1, 0}, {1, 0, 1}, {0, 0, 0}});
  //   for (auto x : S) {
  //     if (y.row_space_basis() == (y.row_space_basis() * x).row_space_basis())
  //     {
  //     }
  //   }
  // }

  //  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
  //                          "029",
  //                          "(BMat8) find an element",
  //                          "[quick][froidure-pin][bmat8]") {
  //    FroidurePin<BMat8> S(
  //        {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
  //         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
  //         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
  //         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});
  //    S.run();
  //    for (auto y : S) {
  //         std::cout << y.row_space_basis() << std::endl;
  //        for (auto x : S) {
  //            if (y.row_space_basis() == (y.row_space_basis() *
  //            x).row_space_basis()) {
  //                auto p = y.right_perm_action_on_basis(x);
  //                if (p == Permutation<uint8_t>({2,0,3,1,4,5,6,7}))
  //                    std::cout << "++++" << std::endl
  //                              << y << std::endl
  //                              << x << std::endl;
  //            }
  //        }
  //    }
  //  }

  //  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
  //                          "030",
  //                          "(BMat8) find an element",
  //                          "[quick][froidure-pin][bmat8]") {
  //    FroidurePin<BMat8> S({BMat8({{1, 0, 0, 0, 0},
  //                                 {0, 1, 0, 0, 0},
  //                                 {0, 0, 1, 0, 0},
  //                                 {0, 0, 0, 1, 0},
  //                                 {0, 0, 0, 0, 1}}),
  //                          BMat8({{0, 1, 0, 0, 0},
  //                                 {0, 0, 1, 0, 0},
  //                                 {0, 0, 0, 1, 0},
  //                                 {0, 0, 0, 0, 1},
  //                                 {1, 0, 0, 0, 0}}),
  //                          BMat8({{0, 1, 0, 0, 0},
  //                                 {1, 0, 0, 0, 0},
  //                                 {0, 0, 1, 0, 0},
  //                                 {0, 0, 0, 1, 0},
  //                                 {0, 0, 0, 0, 1}}),
  //                          BMat8({{1, 0, 0, 0, 0},
  //                                 {0, 1, 0, 0, 0},
  //                                 {0, 0, 1, 0, 0},
  //                                 {0, 0, 0, 1, 0},
  //                                 {1, 0, 0, 0, 1}}),
  //                          BMat8({{1, 1, 0, 0, 0},
  //                                 {1, 0, 1, 0, 0},
  //                                 {0, 1, 0, 1, 0},
  //                                 {0, 0, 1, 1, 0},
  //                                 {0, 0, 0, 0, 1}}),
  //                          BMat8({{1, 1, 0, 0, 0},
  //                                 {1, 0, 1, 0, 0},
  //                                 {0, 1, 1, 0, 0},
  //                                 {0, 0, 0, 1, 0},
  //                                 {0, 0, 0, 0, 1}}),
  //                          BMat8({{1, 1, 1, 0, 0},
  //                                 {1, 0, 0, 1, 0},
  //                                 {0, 1, 0, 1, 0},
  //                                 {0, 0, 1, 1, 0},
  //                                 {0, 0, 0, 0, 1}}),
  //                          BMat8({{1, 1, 0, 0, 0},
  //                                 {1, 0, 1, 0, 0},
  //                                 {0, 1, 0, 1, 0},
  //                                 {0, 0, 1, 0, 1},
  //                                 {0, 0, 0, 1, 1}}),
  //                          BMat8({{1, 1, 1, 1, 0},
  //                                 {1, 0, 0, 0, 1},
  //                                 {0, 1, 0, 0, 1},
  //                                 {0, 0, 1, 0, 1},
  //                                 {0, 0, 0, 1, 1}}),
  //                          BMat8({{1, 0, 0, 0, 0},
  //                                 {0, 1, 0, 0, 0},
  //                                 {0, 0, 1, 0, 0},
  //                                 {0, 0, 0, 1, 0},
  //                                 {0, 0, 0, 0, 0}}),
  //                          BMat8({{1, 1, 1, 0, 0},
  //                                 {1, 0, 0, 1, 0},
  //                                 {0, 1, 0, 1, 0},
  //                                 {0, 0, 1, 0, 1},
  //                                 {0, 0, 0, 1, 1}}),
  //                          BMat8({{1, 1, 1, 0, 0},
  //                                 {1, 0, 0, 1, 0},
  //                                 {1, 0, 0, 0, 1},
  //                                 {0, 1, 0, 1, 0},
  //                                 {0, 0, 1, 0, 1}}),
  //                          BMat8({{1, 1, 1, 0, 0},
  //                                 {1, 0, 0, 1, 1},
  //                                 {0, 1, 0, 1, 0},
  //                                 {0, 1, 0, 0, 1},
  //                                 {0, 0, 1, 1, 0}}),
  //                          BMat8({{1, 1, 1, 0, 0},
  //                                 {1, 1, 0, 1, 0},
  //                                 {1, 0, 0, 0, 1},
  //                                 {0, 1, 0, 0, 1},
  //                                 {0, 0, 1, 1, 1}})});
  //    auto               rg = ReportGuard();
  //    S.enumerate(100000);
  //    for (auto y : S) {
  //      for (auto x : S) {
  //        if (y.row_space_basis()
  //            == (y.row_space_basis() * x).row_space_basis()) {
  //          auto p = y.right_perm_action_on_basis(x);
  //          if (p == Permutation<uint8_t>({2, 0, 3, 1, 4, 5, 6, 7})) {
  //          }
  //        }
  //      }
  //    }
  //  }

}  // namespace libsemigroups
