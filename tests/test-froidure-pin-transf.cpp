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

#include <algorithm>  // for min
#include <cstddef>    // for size_t
#include <cstdint>    // for uint_fast8_t, uint16_t
#include <vector>     // for vector

#include "catch.hpp"                       // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin<>::element_index_type
#include "libsemigroups/transf.hpp"        // for Transf
#include "test-main.hpp"

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  namespace {
    template <typename T>
    void test000() {
      auto rg = ReportGuard(REPORT);

      FroidurePin<T> S;
      S.add_generator(T({1, 7, 2, 6, 0, 4, 1, 5}));
      S.add_generator(T({2, 4, 6, 1, 4, 5, 2, 7}));
      S.add_generator(T({3, 0, 7, 2, 4, 6, 2, 4}));
      S.add_generator(T({3, 2, 3, 4, 5, 3, 0, 1}));
      S.add_generator(T({4, 3, 7, 7, 4, 5, 0, 4}));
      S.add_generator(T({5, 6, 3, 0, 3, 0, 5, 1}));
      S.add_generator(T({6, 0, 1, 1, 1, 6, 3, 4}));
      S.add_generator(T({7, 7, 4, 0, 6, 4, 1, 7}));
      S.reserve(597369);

      REQUIRE(S.size() == 597369);
      REQUIRE(S.number_of_idempotents() == 8194);
      size_t pos = 0;
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        REQUIRE(S.position(*it) == pos);
        pos++;
      }

      S.add_generators({T({7, 1, 2, 6, 7, 4, 1, 5})});
      REQUIRE(S.size() == 826713);
      S.closure({T({7, 1, 2, 6, 7, 4, 1, 5})});
      REQUIRE(S.size() == 826713);
      REQUIRE(S.minimal_factorisation(T({7, 1, 2, 6, 7, 4, 1, 5})
                                      * T({2, 4, 6, 1, 4, 5, 2, 7}))
              == word_type({8, 1}));
      REQUIRE(S.minimal_factorisation(10) == word_type({0, 2}));
      REQUIRE(S.at(10) == T({0, 4, 7, 2, 3, 4, 0, 6}));
      REQUIRE_THROWS_AS(S.minimal_factorisation(1000000000),
                        LibsemigroupsException);
      pos = 0;
      for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); ++it) {
        REQUIRE(*it * *it == *it);
        pos++;
      }
      REQUIRE(pos == S.number_of_idempotents());
      for (auto it = S.cbegin_sorted() + 1; it < S.cend_sorted(); ++it) {
        REQUIRE(*(it - 1) < *it);
      }
    }

    template <typename T>
    void test001() {
      auto           rg = ReportGuard(REPORT);
      std::vector<T> gens1;
      REQUIRE_NOTHROW(FroidurePin<T>(gens1));
    }

    template <typename T>
    void test002() {
      auto           rg = ReportGuard(REPORT);
      FroidurePin<T> S;
      S.add_generator(T({2, 4, 6, 1, 4, 5, 2, 7, 3}));
      // For dynamic Transf exception is thrown by FroidurePin because degree
      // is wrong, for static Transf exception is thrown by make, because the
      // container has the wrong size
      REQUIRE_THROWS_AS(S.add_generator(T::make({1, 7, 2, 6, 0, 0, 1, 2})),
                        LibsemigroupsException);
    }

    void test_idempotent(FroidurePin<Transf<>>& S, Transf<> const& x) {
      REQUIRE(S.is_idempotent(S.position(x)));
      Transf<> y(x);
      y.product_inplace(x, x);
      REQUIRE(x == y);
      REQUIRE(S.fast_product(S.position(x), S.position(x)) == S.position(x));
    }

    void test_rules_iterator(FroidurePin<Transf<>>& S) {
      size_t nr = 0;
      for (auto it = S.cbegin_rules(); it != S.cend_rules(); ++it) {
        REQUIRE(S.current_position(it->first)
                == S.current_position(it->second));
        nr++;
      }
      REQUIRE(nr == S.current_number_of_rules());
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "063",
                          "JDM favourite (dynamic)",
                          "[standard][froidure-pin][transformation][transf]") {
    test000<Transf<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "064",
                          "JDM favourite (static)",
                          "[standard][froidure-pin][transformation][transf]") {
    test000<Transf<8>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "065",
                          "no exception zero generators given",
                          "[quick][froidure-pin][transformation][transf]") {
    test001<Transf<>>();
    test001<Transf<8>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "066",
                          "exception generators of "
                          "different degrees",
                          "[quick][froidure-pin][transformation][transf]") {
    test002<Transf<>>();
    test002<Transf<9>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "067",
                          "exception current_position",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({0, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 0, 2, 3, 4, 5}),
                                  Transf<>({4, 0, 1, 2, 3, 5}),
                                  Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Transf<>> U(gens);

    REQUIRE_THROWS_AS(U.current_position({}), LibsemigroupsException);
    REQUIRE_NOTHROW(U.current_position({0, 0, 1, 2}));
    REQUIRE_THROWS_AS(U.current_position({5}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "068",
                          "exception word_to_element",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({0, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 0, 2, 3, 4, 5}),
                                  Transf<>({4, 0, 1, 2, 3, 5}),
                                  Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Transf<>> U(gens);

    REQUIRE_THROWS_AS(U.word_to_element({}), LibsemigroupsException);
    REQUIRE_THROWS_AS(U.word_to_element({5}), LibsemigroupsException);

    Transf<> u = U.word_to_element({0, 0, 1, 2});
    REQUIRE(u
            == Transf<>({0, 1, 2, 3, 4, 5}) * Transf<>({0, 1, 2, 3, 4, 5})
                   * Transf<>({1, 0, 2, 3, 4, 5})
                   * Transf<>({4, 0, 1, 2, 3, 5}));
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "069",
                          "exception gens",
                          "[quick][froidure-pin][transformation][transf]") {
    auto rg = ReportGuard(REPORT);
    for (size_t i = 1; i < 20; ++i) {
      std::vector<Transf<>> gens;
      using value_type = typename Transf<>::value_type;

      for (size_t j = 0; j < i; ++j) {
        std::vector<value_type> trans;
        for (size_t k = 0; k < i; ++k) {
          trans.push_back((k + j) % i);
        }
        gens.push_back(Transf<>(trans));
      }
      FroidurePin<Transf<>> S(gens);

      for (size_t j = 0; j < i; ++j) {
        REQUIRE_NOTHROW(S.generator(j));
      }
      REQUIRE_THROWS_AS(S.generator(i), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "070",
                          "exception prefix",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({1, 0, 2, 3, 4, 5}),
                                  Transf<>({4, 0, 1, 2, 3, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Transf<>> U(gens);

    for (size_t i = 0; i < U.size(); ++i) {
      REQUIRE_NOTHROW(U.prefix(i));
      REQUIRE_THROWS_AS(U.prefix(i + U.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "071",
                          "exception suffix",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({0, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 0, 2, 3, 4, 5}),
                                  Transf<>({4, 0, 1, 2, 3, 5}),
                                  Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Transf<>> U(gens);
    REQUIRE(U.size() == 7776);

    for (size_t i = 0; i < U.size(); ++i) {
      REQUIRE_NOTHROW(U.suffix(i));
      REQUIRE_THROWS_AS(U.suffix(i + U.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "072",
                          "exception first_letter",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({0, 1, 2, 3, 4, 5}),
                                  Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Transf<>> U(gens);

    for (size_t i = 0; i < U.size(); ++i) {
      REQUIRE_NOTHROW(U.first_letter(i));
      REQUIRE_THROWS_AS(U.first_letter(i + U.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "073",
                          "exception final_letter",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({0, 1, 2, 3, 4, 5}),
                                  Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Transf<>> U(gens);

    for (size_t i = 0; i < U.size(); ++i) {
      REQUIRE_NOTHROW(U.final_letter(i));
      REQUIRE_THROWS_AS(U.final_letter(i + U.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "074",
                          "exception current_length",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({0, 1, 2, 3, 4, 5}),
                                  Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Transf<>> U(gens);

    for (size_t i = 0; i < U.size(); ++i) {
      REQUIRE_NOTHROW(U.current_length(i));
      REQUIRE_THROWS_AS(U.current_length(i + U.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "075",
                          "exception product_by_reduction",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Transf<>> gens
        = {Transf<>({0, 1, 2, 3}), Transf<>({3, 1, 1, 2})};
    FroidurePin<Transf<>> U(gens);

    for (size_t i = 0; i < U.size(); ++i) {
      for (size_t j = 0; j < U.size(); ++j) {
        REQUIRE_NOTHROW(U.product_by_reduction(i, j));
        REQUIRE_THROWS_AS(U.product_by_reduction(i + U.size(), j),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(U.product_by_reduction(i, j + U.size()),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(U.product_by_reduction(i + U.size(), j + U.size()),
                          LibsemigroupsException);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "076",
                          "exception fast_product",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Transf<>> gens
        = {Transf<>({0, 1, 2, 3}), Transf<>({3, 1, 1, 2})};
    FroidurePin<Transf<>> U(gens);

    for (size_t i = 0; i < U.size(); ++i) {
      for (size_t j = 0; j < U.size(); ++j) {
        REQUIRE_NOTHROW(U.fast_product(i, j));
        REQUIRE_THROWS_AS(U.fast_product(i + U.size(), j),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(U.fast_product(i, j + U.size()),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(U.fast_product(i + U.size(), j + U.size()),
                          LibsemigroupsException);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "077",
                          "exception current_position",
                          "[quick][froidure-pin][transformation][transf]") {
    auto rg = ReportGuard(REPORT);
    for (size_t i = 1; i < 20; ++i) {
      std::vector<Transf<>> gens;
      using value_type = typename Transf<>::value_type;

      for (size_t j = 0; j < i; ++j) {
        std::vector<value_type> trans;
        for (size_t k = 0; k < i; ++k) {
          trans.push_back((k + j) % i);
        }
        gens.push_back(Transf<>(trans));
      }
      FroidurePin<Transf<>> S(gens);

      for (size_t j = 0; j < i; ++j) {
        REQUIRE_NOTHROW(S.current_position(j));
      }
      REQUIRE_THROWS_AS(S.current_position(i), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "078",
                          "exception is_idempotent",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({0, 1, 2, 3, 4, 5}),
                                  Transf<>({5, 1, 3, 3, 2, 5}),
                                  Transf<>({2, 1, 2, 3, 4, 4}),
                                  Transf<>({5, 5, 2, 1, 1, 2})};
    FroidurePin<Transf<>> S(gens);

    // S has size 441
    for (size_t i = 0; i < 441; ++i) {
      REQUIRE_NOTHROW(S.is_idempotent(i));
    }
    for (size_t i = 0; i < 20; ++i) {
      REQUIRE_THROWS_AS(S.is_idempotent(441 + i), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "079",
                          "exception add_generators",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({1, 7, 2, 6, 0, 0, 1, 2}),
                                  Transf<>({2, 4, 6, 1, 4, 5, 2, 7})};
    FroidurePin<Transf<>> T(gens);

    std::vector<Transf<>> additional_gens_1 = {
        Transf<>({1, 2, 2, 2, 1, 1, 3, 4}), Transf<>({1, 2, 1, 3, 1, 4, 1, 5})};
    std::vector<Transf<>> additional_gens_2
        = {Transf<>({1, 2, 2, 2, 1, 1, 3, 4}),
           Transf<>({1, 2, 1, 3, 1, 4, 1, 5, 1})};

    REQUIRE_NOTHROW(T.add_generators(additional_gens_1));
    REQUIRE_THROWS_AS(T.add_generators(additional_gens_2),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "080",
                          "multithread number_of_idempotents",
                          "[quick][froidure-pin][transformation][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S({Transf<>({1, 7, 2, 6, 0, 0, 1, 2}),
                             Transf<>({2, 4, 6, 1, 4, 5, 2, 7})});
    S.max_threads(2).concurrency_threshold(0);
    REQUIRE(S.max_threads()
            == std::min(static_cast<unsigned int>(2),
                        std::thread::hardware_concurrency()));
    REQUIRE(S.concurrency_threshold() == 0);
    REQUIRE(S.number_of_idempotents() == 72);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "081",
                          "small semigroup",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 0}));
    S.add_generator(Transf<>({0, 1, 2}));

    REQUIRE(S.size() == 2);
    REQUIRE(S.degree() == 3);
    REQUIRE(S.number_of_idempotents() == 2);
    REQUIRE(S.number_of_generators() == 2);
    REQUIRE(S.number_of_rules() == 4);

    REQUIRE(S[0] == Transf<>({0, 1, 0}));
    REQUIRE(S[1] == Transf<>({0, 1, 2}));

    REQUIRE(S.position(Transf<>({0, 1, 0})) == 0);
    REQUIRE(S.contains(Transf<>({0, 1, 0})));

    REQUIRE(S.position(Transf<>({0, 1, 2})) == 1);
    REQUIRE(S.contains(Transf<>({0, 1, 2})));

    REQUIRE(S.position(Transf<>({0, 0, 0})) == UNDEFINED);
    REQUIRE(!S.contains(Transf<>({0, 0, 0})));
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "082",
                          "large semigroup",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto rg = ReportGuard(REPORT);

    FroidurePin<Transf<>> S;

    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.number_of_idempotents() == 537);
    REQUIRE(S.number_of_generators() == 5);
    REQUIRE(S.number_of_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "083",
                          "at, position, current_*",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto rg = ReportGuard(REPORT);

    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    S.batch_size(1024);

    REQUIRE(S.at(100) == Transf<>({5, 3, 4, 1, 2, 5}));
    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_number_of_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);

    REQUIRE(S.position(Transf<>({5, 3, 4, 1, 2, 5})) == 100);

    REQUIRE(S.at(1023) == Transf<>({5, 4, 3, 4, 1, 5}));
    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_number_of_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);

    REQUIRE(S.position(Transf<>({5, 4, 3, 4, 1, 5})) == 1023);

    REQUIRE(S.at(3000) == Transf<>({5, 3, 5, 3, 4, 5}));
    REQUIRE(S.current_size() == 3001);
    REQUIRE(S.current_number_of_rules() == 526);
    REQUIRE(S.current_max_word_length() == 9);

    REQUIRE(S.position(Transf<>({5, 3, 5, 3, 4, 5})) == 3000);

    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.number_of_idempotents() == 537);
    REQUIRE(S.number_of_generators() == 5);
    REQUIRE(S.number_of_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "084",
                          "run",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    S.batch_size(1024);

    S.enumerate(3000);
    REQUIRE(S.current_size() == 3000);
    REQUIRE(S.current_number_of_rules() == 526);
    REQUIRE(S.current_max_word_length() == 9);

    S.enumerate(3001);
    REQUIRE(S.current_size() == 4024);
    REQUIRE(S.current_number_of_rules() == 999);
    REQUIRE(S.current_max_word_length() == 10);

    S.enumerate(7000);
    REQUIRE(S.current_size() == 7000);
    REQUIRE(S.current_number_of_rules() == 2044);
    REQUIRE(S.current_max_word_length() == 12);

    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.number_of_idempotents() == 537);
    REQUIRE(S.number_of_generators() == 5);
    REQUIRE(S.number_of_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "085",
                          "run [many stops and starts]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto rg = ReportGuard(REPORT);

    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    S.batch_size(128);

    for (size_t i = 1; !S.finished(); i++) {
      S.enumerate(i * 128);
    }

    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.number_of_idempotents() == 537);
    REQUIRE(S.number_of_generators() == 5);
    REQUIRE(S.number_of_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "086",
                          "factorisation, length [1 element]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    S.batch_size(1024);

    word_type result;
    S.factorisation(result, 5537);
    word_type expected = {1, 2, 2, 2, 3, 2, 4, 1, 2, 2, 3};
    REQUIRE(result == expected);
    REQUIRE(S.current_length(5537) == 11);
    REQUIRE(S.length(5537) == 11);
    REQUIRE(S.current_max_word_length() == 11);

    REQUIRE(S.current_size() == 5539);
    REQUIRE(S.current_number_of_rules() == 1484);
    REQUIRE(S.current_max_word_length() == 11);

    REQUIRE(S.length(7775) == 16);
    REQUIRE(S.current_max_word_length() == 16);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "087",
                          "factorisation, products [all elements]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    S.batch_size(1024);

    word_type result;
    for (size_t i = 0; i < S.size(); i++) {
      S.factorisation(result, i);
      REQUIRE(S.current_position(result) == i);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "088",
                          "first/final letter, prefix, suffix, products",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    S.enumerate(1000);  // fully enumerates

    REQUIRE(S.first_letter(6377) == 2);
    REQUIRE(S.prefix(6377) == 5049);
    REQUIRE(S.final_letter(6377) == 2);
    REQUIRE(S.suffix(6377) == 5149);
    REQUIRE(S.fast_product(S.prefix(6377), S.final_letter(6377)) == 6377);
    REQUIRE(S.fast_product(S.first_letter(6377), S.suffix(6377)) == 6377);
    REQUIRE(S.product_by_reduction(S.prefix(6377), S.final_letter(6377))
            == 6377);
    REQUIRE(S.product_by_reduction(S.first_letter(6377), S.suffix(6377))
            == 6377);

    REQUIRE(S.first_letter(2103) == 3);
    REQUIRE(S.prefix(2103) == 1050);
    REQUIRE(S.final_letter(2103) == 1);
    REQUIRE(S.suffix(2103) == 860);
    REQUIRE(S.fast_product(S.prefix(2103), S.final_letter(2103)) == 2103);
    REQUIRE(S.fast_product(S.first_letter(2103), S.suffix(2103)) == 2103);
    REQUIRE(S.product_by_reduction(S.prefix(2103), S.final_letter(2103))
            == 2103);
    REQUIRE(S.product_by_reduction(S.first_letter(2103), S.suffix(2103))
            == 2103);

    REQUIRE(S.first_letter(3407) == 2);
    REQUIRE(S.prefix(3407) == 1923);
    REQUIRE(S.final_letter(3407) == 3);
    REQUIRE(S.suffix(3407) == 2115);
    REQUIRE(S.fast_product(S.prefix(3407), S.final_letter(3407)) == 3407);
    REQUIRE(S.fast_product(S.first_letter(3407), S.suffix(3407)) == 3407);
    REQUIRE(S.product_by_reduction(S.prefix(3407), S.final_letter(3407))
            == 3407);
    REQUIRE(S.product_by_reduction(S.first_letter(3407), S.suffix(3407))
            == 3407);

    REQUIRE(S.first_letter(4245) == 2);
    REQUIRE(S.prefix(4245) == 2767);
    REQUIRE(S.final_letter(4245) == 3);
    REQUIRE(S.suffix(4245) == 2319);
    REQUIRE(S.fast_product(S.prefix(4225), S.final_letter(4225)) == 4225);
    REQUIRE(S.fast_product(S.first_letter(4225), S.suffix(4225)) == 4225);
    REQUIRE(S.product_by_reduction(S.prefix(4225), S.final_letter(4225))
            == 4225);
    REQUIRE(S.product_by_reduction(S.first_letter(4225), S.suffix(4225))
            == 4225);

    REQUIRE(S.first_letter(3683) == 4);
    REQUIRE(S.prefix(3683) == 2246);
    REQUIRE(S.final_letter(3683) == 2);
    REQUIRE(S.suffix(3683) == 1685);
    REQUIRE(S.fast_product(S.prefix(3683), S.final_letter(3683)) == 3683);
    REQUIRE(S.fast_product(S.first_letter(3683), S.suffix(3683)) == 3683);
    REQUIRE(S.product_by_reduction(S.prefix(3683), S.final_letter(3683))
            == 3683);
    REQUIRE(S.product_by_reduction(S.first_letter(3683), S.suffix(3683))
            == 3683);

    REQUIRE(S.first_letter(0) == 0);
    REQUIRE(S.prefix(0) == UNDEFINED);
    REQUIRE(S.final_letter(0) == 0);
    REQUIRE(S.suffix(0) == UNDEFINED);

    REQUIRE(S.first_letter(7775) == 1);
    REQUIRE(S.prefix(7775) == 7760);
    REQUIRE(S.final_letter(7775) == 2);
    REQUIRE(S.suffix(7775) == 7768);
    REQUIRE(S.fast_product(S.prefix(7775), S.final_letter(7775)) == 7775);
    REQUIRE(S.fast_product(S.first_letter(7775), S.suffix(7775)) == 7775);
    REQUIRE(S.product_by_reduction(S.prefix(7775), S.final_letter(7775))
            == 7775);
    REQUIRE(S.product_by_reduction(S.first_letter(7775), S.suffix(7775))
            == 7775);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "089",
                          "current_position [standard]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(S.current_position(0) == 0);
    REQUIRE(S.current_position(1) == 1);
    REQUIRE(S.current_position(2) == 2);
    REQUIRE(S.current_position(3) == 3);
    REQUIRE(S.current_position(4) == 4);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "090",
                          "current_position [duplicate gens]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(S.current_position(0) == 0);
    REQUIRE(S.current_position(1) == 1);
    REQUIRE(S.current_position(2) == 1);
    REQUIRE(S.current_position(3) == 1);
    REQUIRE(S.current_position(4) == 1);
    REQUIRE(S.current_position(10) == 1);
    REQUIRE(S.current_position(12) == 3);

    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.number_of_idempotents() == 537);
    REQUIRE(S.number_of_generators() == 32);
    REQUIRE(S.number_of_rules() == 2621);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "091",
                          "current_position [after add_generators]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));

    REQUIRE(S.size() == 1);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.number_of_idempotents() == 1);
    REQUIRE(S.number_of_generators() == 1);
    REQUIRE(S.number_of_rules() == 1);

    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    REQUIRE(S.size() == 2);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.number_of_idempotents() == 1);
    REQUIRE(S.number_of_generators() == 2);
    REQUIRE(S.number_of_rules() == 4);

    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    REQUIRE(S.size() == 120);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.number_of_idempotents() == 1);
    REQUIRE(S.number_of_generators() == 3);
    REQUIRE(S.number_of_rules() == 25);

    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    REQUIRE(S.size() == 1546);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.number_of_idempotents() == 32);
    REQUIRE(S.number_of_generators() == 4);
    REQUIRE(S.number_of_rules() == 495);

    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));
    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.number_of_idempotents() == 537);
    REQUIRE(S.number_of_generators() == 5);
    REQUIRE(S.number_of_rules() == 2459);

    REQUIRE(S.current_position(0) == 0);
    REQUIRE(S.current_position(1) == 1);
    REQUIRE(S.current_position(2) == 2);
    REQUIRE(S.current_position(3) == 120);
    REQUIRE(S.current_position(4) == 1546);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "092",
                          "cbegin_idempotents/cend [1 thread]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    size_t nr = 0;
    for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
      test_idempotent(S, *it);
      nr++;
    }
    REQUIRE(nr == S.number_of_idempotents());
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "093",
                          "idempotent_cend/cbegin [1 thread]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    size_t nr  = 0;
    auto   end = S.cend_idempotents();
    for (auto it = S.cbegin_idempotents(); it < end; it++) {
      test_idempotent(S, *it);
      nr++;
    }
    REQUIRE(nr == S.number_of_idempotents());
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "094",
                          "is_idempotent [1 thread]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    S.max_threads(1000);

    size_t nr = 0;
    for (size_t i = 0; i < S.size(); i++) {
      if (S.is_idempotent(i)) {
        nr++;
      }
    }
    REQUIRE(nr == S.number_of_idempotents());
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePin<Transf<>>",
      "095",
      "cbegin_idempotents/cend, is_idempotent [2 threads]",
      "[standard][froidure-pin][transf][multithread][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 2, 3, 4, 5, 6, 0}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5, 6}));
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5, 0}));
    S.max_threads(2);

    size_t nr = 0;

    for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
      test_idempotent(S, *it);
      nr++;
    }
    REQUIRE(nr == S.number_of_idempotents());
    REQUIRE(nr == 6322);

    nr = 0;
    for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
      test_idempotent(S, *it);
      nr++;
    }
    REQUIRE(nr == S.number_of_idempotents());
    REQUIRE(nr == 6322);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "096",
                          "finished, started",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    S.batch_size(1024);
    S.enumerate(10);
    REQUIRE(S.started());
    REQUIRE(!S.finished());

    S.enumerate(8000);
    REQUIRE(S.started());
    REQUIRE(S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "097",
                          "current_position",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(S.current_position(S.generator(0)) == 0);
    REQUIRE(S.current_position(S.generator(1)) == 1);
    REQUIRE(S.current_position(S.generator(2)) == 2);
    REQUIRE(S.current_position(S.generator(3)) == 3);
    REQUIRE(S.current_position(S.generator(4)) == 4);

    S.batch_size(1024);
    S.enumerate(1024);

    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_number_of_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);
    REQUIRE(S.current_position(S.at(1024)) == 1024);

    REQUIRE(S.current_position(Transf<>({5, 1, 5, 5, 2, 5})) == 1028);
    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_number_of_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);

    REQUIRE(S.current_position(Transf<>({5, 1, 5, 5, 2, 5, 6})) == UNDEFINED);
    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_number_of_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);

    REQUIRE(S.current_position(Transf<>({5, 4, 5, 1, 0, 5})) == UNDEFINED);
    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_number_of_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);
    REQUIRE(S.position(Transf<>({5, 4, 5, 1, 0, 5})) == 1029);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "098",
                          "sorted_position, sorted_at",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(S.sorted_position(S.generator(0)) == 310);
    REQUIRE(S.sorted_at(310) == S.generator(0));
    REQUIRE(S.sorted_at(310) == S.at(0));

    REQUIRE(S.sorted_position(S.generator(1)) == 1390);
    REQUIRE(S.sorted_at(1390) == S.generator(1));
    REQUIRE(S.sorted_at(1390) == S.at(1));

    REQUIRE(S.sorted_position(S.generator(2)) == 5235);
    REQUIRE(S.sorted_at(5235) == S.generator(2));
    REQUIRE(S.sorted_at(5235) == S.at(2));

    REQUIRE(S.sorted_position(S.generator(3)) == 6790);
    REQUIRE(S.sorted_at(6790) == S.generator(3));
    REQUIRE(S.sorted_at(6790) == S.at(3));

    REQUIRE(S.sorted_position(S.generator(4)) == 1606);
    REQUIRE(S.sorted_at(1606) == S.generator(4));
    REQUIRE(S.sorted_at(1606) == S.at(4));

    REQUIRE(S.finished());

    REQUIRE(S.sorted_position(S.at(1024)) == 6810);
    REQUIRE(S.position_to_sorted_position(1024) == 6810);

    REQUIRE(S.sorted_at(6810) == S.at(1024));
    REQUIRE(S.sorted_at(6810) == S.at(1024));

    REQUIRE(S.sorted_position(Transf<>({5, 1, 5, 5, 2, 5})) == 6908);
    REQUIRE(
        S.position_to_sorted_position(S.position(Transf<>({5, 1, 5, 5, 2, 5})))
        == 6908);
    REQUIRE(S.sorted_at(6908) == Transf<>({5, 1, 5, 5, 2, 5}));
    REQUIRE(S.sorted_at(6908)
            == S.at(S.position(Transf<>({5, 1, 5, 5, 2, 5}))));

    REQUIRE(S.sorted_position(Transf<>({5, 5, 5, 1, 5, 5, 6})) == UNDEFINED);

    REQUIRE_THROWS_AS(S.sorted_at(100000), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.at(100000), LibsemigroupsException);
    REQUIRE(S.position_to_sorted_position(100000) == UNDEFINED);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "099",
                          "right/left Cayley graph",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(S.right(0, 0) == 0);
    REQUIRE(S.left(0, 0) == 0);

    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    Transf<> tmp(6);
    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      for (size_t i = 0; i < 5; ++i) {
        tmp.product_inplace(*it, S.generator(i));
        REQUIRE(S.position(tmp) == S.right(S.position(*it), i));
        tmp.product_inplace(S.generator(i), *it);
        REQUIRE(S.position(tmp) == S.left(S.position(*it), i));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "100",
                          "iterator",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(S.current_size() == 5);
    size_t size = S.current_size();
    for (auto it = S.cbegin(); it < S.cend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.current_size());
    REQUIRE(5 == S.current_size());

    S.batch_size(1024);
    S.enumerate(1000);
    REQUIRE(S.current_size() < 7776);

    size = S.current_size();
    for (auto it = S.cbegin(); it < S.cend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.current_size());
    REQUIRE(S.current_size() < 7776);

    REQUIRE(S.size() == 7776);
    size = S.size();
    for (auto it = S.cbegin(); it < S.cend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.size());
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "101",
                          "reverse iterator",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(S.current_size() == 5);
    size_t size = S.current_size();
    for (auto it = S.crbegin(); it < S.crend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.crbegin(); it < S.crend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.current_size());
    REQUIRE(5 == S.current_size());

    S.batch_size(1024);
    S.enumerate(1000);
    REQUIRE(S.current_size() < 7776);

    size = S.current_size();
    for (auto it = S.crbegin(); it < S.crend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.crbegin(); it < S.crend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.current_size());
    REQUIRE(S.current_size() < 7776);

    REQUIRE(S.size() == 7776);
    size = S.size();
    for (auto it = S.crbegin(); it < S.crend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.crbegin(); it < S.crend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.size());
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "102",
                          "iterator arithmetic",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(S.size() == 7776);
    auto it = S.cbegin();

    for (int64_t i = 0; i < static_cast<int64_t>(S.size()); i++) {
      REQUIRE(*(it + i) == S.at(i));
      it += i;
      REQUIRE(*it == S.at(i));
      it -= i;
      REQUIRE(*it == S.at(0));
      REQUIRE(it == S.cbegin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
    for (int64_t i = S.size(); i < static_cast<int64_t>(2 * S.size()); i++) {
      it += i;
      it -= i;
      REQUIRE(*it == S.at(0));
      REQUIRE(it == S.cbegin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "103",
                          "iterator sorted",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    // Calling cbegin/cend_sorted fully enumerates the semigroup
    { auto it = S.cbegin_sorted(); }
    REQUIRE(S.finished());

    size_t pos = 0;
    for (auto it = S.cbegin_sorted(); it < S.cend_sorted(); it++) {
      REQUIRE(S.sorted_position(*it) == pos);
      REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
      pos++;
    }
    REQUIRE(pos == S.size());

    pos = 0;
    for (auto it = S.cbegin_sorted(); it < S.cend_sorted(); ++it) {
      REQUIRE(S.sorted_position(*it) == pos);
      REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
      pos++;
    }
    REQUIRE(pos == S.size());

    for (auto it = S.crbegin_sorted(); it < S.crend_sorted(); it++) {
      pos--;
      REQUIRE(S.sorted_position(*it) == pos);
      REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
    }
    REQUIRE(pos == 0);

    pos = S.size();
    for (auto it = S.crbegin_sorted(); it < S.crend_sorted(); ++it) {
      pos--;
      REQUIRE(S.sorted_position(*it) == pos);
      REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
    }
    REQUIRE(pos == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "104",
                          "iterator sorted arithmetic",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(S.size() == 7776);
    auto it = S.cbegin_sorted();

    for (int64_t i = 0; i < static_cast<int64_t>(S.size()); i++) {
      REQUIRE(*(it + i) == S.sorted_at(i));
      it += i;
      REQUIRE(*it == S.sorted_at(i));
      it -= i;
      REQUIRE(*it == S.sorted_at(0));
      REQUIRE(it == S.cbegin_sorted());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
    for (int64_t i = S.size(); i < static_cast<int64_t>(2 * S.size()); i++) {
      it += i;
      it -= i;
      REQUIRE(*it == S.sorted_at(0));
      REQUIRE(it == S.cbegin_sorted());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "105",
                          "copy [not enumerated]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE(!S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.current_size() == 5);
    REQUIRE(S.current_number_of_rules() == 0);
    REQUIRE(S.current_max_word_length() == 1);
    REQUIRE(S.current_position(S.generator(1)) == 1);

    auto T(S);

    REQUIRE(!T.started());
    REQUIRE(!T.finished());
    REQUIRE(T.number_of_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.current_size() == 5);
    REQUIRE(T.current_number_of_rules() == 0);
    REQUIRE(T.current_max_word_length() == 1);
    REQUIRE(T.current_position(S.generator(1)) == 1);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.number_of_idempotents() == 537);
    REQUIRE(T.number_of_rules() == 2459);
    REQUIRE(T.started());
    REQUIRE(T.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "106",
                          "copy_closure [not enumerated]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));

    REQUIRE(!S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.number_of_generators() == 2);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 2);
    REQUIRE(S.current_number_of_rules() == 0);
    REQUIRE(S.current_max_word_length() == 1);

    std::vector<Transf<>> coll = {Transf<>({4, 0, 1, 2, 3, 5}),
                                  Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Transf<>> T = S.copy_closure(coll);

    REQUIRE(T.started());
    REQUIRE(!T.finished());
    REQUIRE(T.number_of_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.current_size() == 7719);
    REQUIRE(T.current_number_of_rules() == 2418);
    REQUIRE(T.current_max_word_length() == 14);
    REQUIRE(T.current_position(S.generator(1)) == 1);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.finished());
    REQUIRE(T.number_of_idempotents() == 537);
    REQUIRE(T.number_of_rules() == 2459);

    FroidurePin<Transf<>> U = T.copy_closure({Transf<>({6, 0, 1, 2, 3, 5, 6})});

    REQUIRE(U.started());
    REQUIRE(U.finished());
    REQUIRE(U.number_of_generators() == 6);
    REQUIRE(U.degree() == 7);
    REQUIRE(U.current_size() == 16807);
    REQUIRE(U.current_max_word_length() == 16);
    REQUIRE(U.number_of_idempotents() == 1358);
    REQUIRE(U.number_of_rules() == 7901);

    FroidurePin<Transf<>> V = U.copy_closure({});

    REQUIRE(&V != &U);
    REQUIRE(V.started());
    REQUIRE(V.finished());
    REQUIRE(V.number_of_generators() == 6);
    REQUIRE(V.degree() == 7);
    REQUIRE(V.current_size() == 16807);
    REQUIRE(V.current_max_word_length() == 16);
    REQUIRE(V.number_of_idempotents() == 1358);
    REQUIRE(V.number_of_rules() == 7901);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "107",
                          "copy_add_generators [not enumerated]",
                          "[quick][froidure-pin][transf][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));

    REQUIRE(!S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.number_of_generators() == 2);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 2);
    REQUIRE(S.current_number_of_rules() == 0);
    REQUIRE(S.current_max_word_length() == 1);

    std::vector<Transf<>> coll = {Transf<>({4, 0, 1, 2, 3, 5}),
                                  Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Transf<>> T = S.copy_add_generators(coll);

    REQUIRE(!T.started());
    REQUIRE(!T.finished());
    REQUIRE(T.number_of_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.current_size() == 5);
    REQUIRE(T.current_number_of_rules() == 0);
    REQUIRE(T.current_max_word_length() == 1);
    REQUIRE(T.current_position(S.generator(1)) == 1);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.finished());
    REQUIRE(T.number_of_idempotents() == 537);
    REQUIRE(T.number_of_rules() == 2459);

    FroidurePin<Transf<>> U
        = T.copy_add_generators({Transf<>({6, 0, 1, 2, 3, 5, 6})});

    REQUIRE(U.number_of_generators() == 6);
    REQUIRE(U.degree() == 7);
    REQUIRE(U.current_size() == 16807);
    REQUIRE(U.current_max_word_length() == 16);
    REQUIRE(U.number_of_idempotents() == 1358);
    REQUIRE(U.number_of_rules() == 7901);
    REQUIRE(U.finished());

    FroidurePin<Transf<>> V = U.copy_add_generators({});

    REQUIRE(&V != &U);
    REQUIRE(V.started());
    REQUIRE(V.finished());
    REQUIRE(V.number_of_generators() == 6);
    REQUIRE(V.degree() == 7);
    REQUIRE(V.current_size() == 16807);
    REQUIRE(V.current_max_word_length() == 16);
    REQUIRE(V.number_of_idempotents() == 1358);
    REQUIRE(V.number_of_rules() == 7901);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "108",
                          "copy [partly enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));
    S.batch_size(1000);
    S.enumerate(1001);

    REQUIRE(S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.current_size() == 1006);
    REQUIRE(S.current_number_of_rules() == 70);
    REQUIRE(S.current_max_word_length() == 7);
    REQUIRE(S.current_position(S.generator(1)) == 1);

    auto T(S);

    REQUIRE(T.started());
    REQUIRE(!T.finished());
    REQUIRE(T.number_of_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.current_size() == 1006);
    REQUIRE(T.current_number_of_rules() == 70);
    REQUIRE(T.current_max_word_length() == 7);

    REQUIRE(T.current_position(Transf<>({0, 1, 2, 3, 4, 5})) == 0);
    REQUIRE(T.current_position(Transf<>({1, 0, 2, 3, 4, 5})) == 1);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.number_of_idempotents() == 537);
    REQUIRE(T.number_of_rules() == 2459);
    REQUIRE(T.started());
    REQUIRE(T.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "109",
                          "copy_closure [partly enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.batch_size(60);
    S.enumerate(60);

    REQUIRE(S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.number_of_generators() == 3);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 63);
    REQUIRE(S.current_number_of_rules() == 11);
    REQUIRE(S.current_max_word_length() == 7);

    auto coll = {Transf<>({5, 1, 2, 3, 4, 5}), Transf<>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Transf<>> T = S.copy_closure(coll);

    REQUIRE(Transf<>({5, 1, 2, 3, 4, 5}) == T.generator(3));
    REQUIRE(Transf<>({1, 1, 2, 3, 4, 5}) == T.generator(4));

    REQUIRE(T.started());
    REQUIRE(!T.finished());
    REQUIRE(T.number_of_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.current_size() == 7719);
    REQUIRE(T.current_number_of_rules() == 2418);
    REQUIRE(T.current_max_word_length() == 14);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.finished());
    REQUIRE(T.number_of_idempotents() == 537);
    REQUIRE(T.number_of_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "110",
                          "copy_add_generators [partly enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.batch_size(60);
    S.enumerate(60);

    REQUIRE(S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.number_of_generators() == 3);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 63);
    REQUIRE(S.current_number_of_rules() == 11);
    REQUIRE(S.current_max_word_length() == 7);

    auto coll = {Transf<>({5, 1, 2, 3, 4, 5}), Transf<>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Transf<>> T = S.copy_add_generators(coll);

    REQUIRE(Transf<>({5, 1, 2, 3, 4, 5}) == T.generator(3));
    REQUIRE(Transf<>({1, 1, 2, 3, 4, 5}) == T.generator(4));

    REQUIRE(!T.finished());
    REQUIRE(T.number_of_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.current_size() == 818);
    REQUIRE(T.current_number_of_rules() == 55);
    REQUIRE(T.current_max_word_length() == 7);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.finished());
    REQUIRE(T.number_of_idempotents() == 537);
    REQUIRE(T.number_of_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "111",
                          "copy [fully enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    S.enumerate(8000);

    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.size() == 7776);
    REQUIRE(S.number_of_idempotents() == 537);
    REQUIRE(S.number_of_rules() == 2459);

    auto T(S);
    REQUIRE(T.started());
    REQUIRE(T.finished());
    REQUIRE(T.number_of_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.size() == 7776);
    REQUIRE(T.number_of_idempotents() == 537);
    REQUIRE(T.number_of_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "112",
                          "copy_closure [fully enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.enumerate(121);

    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.number_of_generators() == 3);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 120);
    REQUIRE(S.current_number_of_rules() == 25);
    REQUIRE(S.current_max_word_length() == 11);

    FroidurePin<Transf<>> T = S.copy_closure(
        {Transf<>({5, 1, 2, 3, 4, 5}), Transf<>({1, 1, 2, 3, 4, 5})});

    REQUIRE(Transf<>({5, 1, 2, 3, 4, 5}) == T.generator(3));
    REQUIRE(Transf<>({1, 1, 2, 3, 4, 5}) == T.generator(4));

    REQUIRE(T.started());
    REQUIRE(!T.finished());
    REQUIRE(T.number_of_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.current_size() == 7719);
    REQUIRE(T.current_number_of_rules() == 2418);
    REQUIRE(T.current_max_word_length() == 14);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.finished());
    REQUIRE(T.number_of_idempotents() == 537);
    REQUIRE(T.number_of_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "113",
                          "copy_add_generators [fully enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.enumerate(121);

    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.number_of_generators() == 3);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 120);
    REQUIRE(S.current_number_of_rules() == 25);
    REQUIRE(S.current_max_word_length() == 11);

    FroidurePin<Transf<>> T = S.copy_add_generators(
        {Transf<>({5, 1, 2, 3, 4, 5}), Transf<>({1, 1, 2, 3, 4, 5})});

    REQUIRE(Transf<>({5, 1, 2, 3, 4, 5}) == T.generator(3));
    REQUIRE(Transf<>({1, 1, 2, 3, 4, 5}) == T.generator(4));

    REQUIRE(!T.finished());
    REQUIRE(T.number_of_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.current_size() == 6842);
    REQUIRE(T.current_number_of_rules() == 1970);
    REQUIRE(T.current_max_word_length() == 12);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.finished());
    REQUIRE(T.number_of_idempotents() == 537);
    REQUIRE(T.number_of_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "114",
                          "relations [duplicate gens]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));

    S.run();
    auto it = S.cbegin_rules();

    REQUIRE(*it == relation_type({1}, {0}));

    ++it;
    REQUIRE(*it == relation_type({3}, {2}));
    size_t nr = 2;

    while (it != S.cend_rules()) {
      ++it;
      ++nr;
    }
    REQUIRE(S.number_of_rules() == nr - 1);
    std::vector<relation_type> rules(S.cbegin_rules(), S.cend_rules());
    REQUIRE(S.number_of_rules() == rules.size());
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "115",
                          "relations",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));
    // No rules, because not enumerated
    REQUIRE(S.cbegin_rules() == S.cend_rules());
    S.run_until([&S]() { return S.current_number_of_rules() >= 2; });
    REQUIRE(!S.finished());
    {  // test cbegin_rules, cend_rules on partially enumerated S
      auto it = S.cbegin_rules();
      REQUIRE(*it == relation_type({0, 0}, {0}));
      ++it;
      REQUIRE(*it == relation_type({0, 1}, {1}));
      test_rules_iterator(S);
      REQUIRE(!S.finished());
      REQUIRE(S.current_number_of_rules() == 15);
    }

    S.run();
    REQUIRE(S.finished());
    REQUIRE(S.number_of_rules() == 2459);
    {
      auto it = S.cbegin_rules();
      REQUIRE(*it == relation_type({0, 0}, {0}));
      ++it;
      REQUIRE(*it == relation_type({0, 1}, {1}));

      test_rules_iterator(S);
      test_rules_iterator(S);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "116",
                          "relations [copy_closure, duplicate gens]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));

    S.run();
    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.number_of_generators() == 5);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 120);
    REQUIRE(S.size() == 120);
    REQUIRE(S.current_number_of_rules() == 33);
    REQUIRE(S.number_of_rules() == 33);
    REQUIRE(S.current_max_word_length() == 11);

    std::vector<Transf<>> coll = {Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({0, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 0, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Transf<>> T = S.copy_closure(coll);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.finished());
    REQUIRE(T.number_of_idempotents() == 537);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "117",
                          "relations [copy_add_generators, duplicate gens]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));

    S.run();
    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.number_of_generators() == 5);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 120);
    REQUIRE(S.size() == 120);
    REQUIRE(S.current_number_of_rules() == 33);
    REQUIRE(S.number_of_rules() == 33);
    REQUIRE(S.current_max_word_length() == 11);

    std::vector<Transf<>> coll = {Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({0, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 0, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Transf<>> T = S.copy_add_generators(coll);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.finished());
    REQUIRE(T.number_of_idempotents() == 537);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "118",
                          "relations [from copy, not enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    auto T(S);
    REQUIRE(T.current_number_of_rules() == S.current_number_of_rules());
    REQUIRE(!T.finished());

    test_rules_iterator(T);
    test_rules_iterator(T);
    T.run();
    REQUIRE(T.finished());
    test_rules_iterator(T);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "119",
                          "relations [from copy, partly enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    S.batch_size(1023);
    S.enumerate(1000);

    auto T(S);
    REQUIRE(T.current_number_of_rules() == S.current_number_of_rules());

    test_rules_iterator(T);
    test_rules_iterator(T);

    T.run();
    REQUIRE(T.finished());
    REQUIRE(T.number_of_rules() == S.number_of_rules());
    test_rules_iterator(T);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "120",
                          "relations [from copy, fully enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    S.enumerate(8000);

    auto T(S);
    REQUIRE(T.number_of_rules() == S.number_of_rules());

    REQUIRE(T.current_number_of_rules() == S.current_number_of_rules());

    test_rules_iterator(T);
    test_rules_iterator(T);

    T.run();
    REQUIRE(T.finished());
    REQUIRE(T.number_of_rules() == S.number_of_rules());
    test_rules_iterator(T);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "121",
                          "relations [from copy_closure, not enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));

    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    FroidurePin<Transf<>> T = S.copy_closure(
        {Transf<>({5, 1, 2, 3, 4, 5}), Transf<>({1, 1, 2, 3, 4, 5})});

    REQUIRE(Transf<>({5, 1, 2, 3, 4, 5}) == T.generator(3));
    REQUIRE(Transf<>({1, 1, 2, 3, 4, 5}) == T.generator(4));

    REQUIRE(!T.finished());
    REQUIRE(T.current_number_of_rules() == 2418);
    test_rules_iterator(T);
    REQUIRE(!T.finished());
    REQUIRE(T.current_number_of_rules() == 2418);
    test_rules_iterator(T);
    REQUIRE(T.current_number_of_rules() == 2418);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePin<Transf<>>",
      "122",
      "relations [from copy_add_generators, not enumerated]",
      "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));

    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    std::vector<Transf<>> coll
        = {Transf<>({5, 1, 2, 3, 4, 5}), Transf<>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Transf<>> T = S.copy_add_generators(coll);
    REQUIRE(coll[0] == T.generator(3));
    REQUIRE(coll[1] == T.generator(4));

    test_rules_iterator(T);
    REQUIRE(T.number_of_rules() == 2459);
    test_rules_iterator(T);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "123",
                          "relations [from copy_closure, partly enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    S.batch_size(100);

    S.enumerate(10);

    REQUIRE(S.started());
    REQUIRE(!S.finished());

    std::vector<Transf<>> coll
        = {Transf<>({5, 1, 2, 3, 4, 5}), Transf<>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Transf<>> T = S.copy_closure(coll);
    test_rules_iterator(T);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePin<Transf<>>",
      "124",
      "relations [from copy_add_generators, partly enumerated]",
      "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));

    S.batch_size(100);

    S.enumerate(10);

    REQUIRE(S.started());
    REQUIRE(!S.finished());

    std::vector<Transf<>> coll
        = {Transf<>({5, 1, 2, 3, 4, 5}), Transf<>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Transf<>> T = S.copy_add_generators(coll);

    test_rules_iterator(T);
    REQUIRE(T.number_of_rules() == 2459);
    test_rules_iterator(T);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "125",
                          "relations [from copy_closure, fully enumerated]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));

    S.enumerate(8000);

    REQUIRE(S.started());
    REQUIRE(S.finished());

    std::vector<Transf<>> coll
        = {Transf<>({5, 1, 2, 3, 4, 5}), Transf<>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Transf<>> T = S.copy_closure(coll);

    test_rules_iterator(T);
    REQUIRE(T.number_of_rules() == 2459);
    test_rules_iterator(T);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePin<Transf<>>",
      "126",
      "relations [from copy_add_generators, fully enumerated]",
      "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));

    S.enumerate(8000);

    REQUIRE(S.started());
    REQUIRE(S.finished());

    std::vector<Transf<>> coll
        = {Transf<>({5, 1, 2, 3, 4, 5}), Transf<>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Transf<>> T = S.copy_add_generators(coll);

    test_rules_iterator(T);
    REQUIRE(T.number_of_rules() == 2459);
    test_rules_iterator(T);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "127",
                          "add_generators [duplicate generators]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 0, 3, 4, 5}));
    S.add_generator(Transf<>({0, 1, 0, 3, 4, 5}));

    REQUIRE(S.size() == 1);
    REQUIRE(S.number_of_generators() == 2);

    S.add_generators({});
    REQUIRE(S.size() == 1);
    REQUIRE(S.number_of_generators() == 2);

    S.add_generator(S.generator(0));
    REQUIRE(S.size() == 1);
    REQUIRE(S.number_of_generators() == 3);

    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    REQUIRE(S.size() == 2);
    REQUIRE(S.number_of_generators() == 4);

    S.add_generator(Transf<>({0, 1, 3, 5, 5, 4}));
    REQUIRE(S.size() == 7);
    REQUIRE(S.number_of_generators() == 5);

    S.add_generator(Transf<>({1, 0, 2, 4, 4, 5}));
    REQUIRE(S.size() == 18);
    REQUIRE(S.number_of_generators() == 6);

    S.add_generator(Transf<>({4, 3, 3, 1, 0, 5}));
    REQUIRE(S.size() == 87);
    REQUIRE(S.number_of_generators() == 7);

    S.add_generator(Transf<>({4, 3, 5, 1, 0, 5}));
    REQUIRE(S.size() == 97);
    REQUIRE(S.number_of_generators() == 8);

    S.add_generator(Transf<>({5, 5, 2, 3, 4, 0}));
    REQUIRE(S.size() == 119);
    REQUIRE(S.number_of_generators() == 9);
    REQUIRE(S.number_of_rules() == 213);

    S.add_generator(Transf<>({1, 0, 2, 4, 4, 5})
                    * Transf<>({4, 3, 3, 1, 0, 5}));

    REQUIRE(S.size() == 119);
    REQUIRE(S.number_of_generators() == 10);
    REQUIRE(S.number_of_rules() == 267);

    REQUIRE(S.current_position(0) == 0);
    REQUIRE(S.current_position(1) == 0);
    REQUIRE(S.current_position(2) == 0);
    REQUIRE(S.current_position(3) == 1);
    REQUIRE(S.current_position(4) == 2);
    REQUIRE(S.current_position(5) == 7);
    REQUIRE(S.current_position(6) == 18);
    REQUIRE(S.current_position(7) == 87);
    REQUIRE(S.current_position(8) == 97);
    REQUIRE(S.current_position(9) == 21);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "128",
                          "add_generators [incremental 1]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;

    S.add_generators({});
    S.add_generator(Transf<>({0, 1, 0, 3, 4, 5}));
    S.add_generator(Transf<>({0, 1, 0, 3, 4, 5}));
    S.add_generator(Transf<>({0, 1, 0, 3, 4, 5}));
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({0, 1, 3, 5, 5, 4}));
    S.add_generator(Transf<>({1, 0, 2, 4, 4, 5}));
    REQUIRE(S.size() == 18);
    REQUIRE(S.number_of_generators() == 6);

    S.add_generator(Transf<>({4, 3, 3, 1, 0, 5}));
    S.add_generator(Transf<>({4, 3, 5, 1, 0, 5}));
    REQUIRE(S.size() == 97);
    REQUIRE(S.number_of_generators() == 8);
    REQUIRE(S.number_of_rules() == 126);

    S.add_generators({S.generator(4), S.generator(5)});
    S.add_generator(S.generator(5));
    S.add_generator(Transf<>({5, 5, 2, 3, 4, 0}));
    S.add_generators({S.generator(0), S.generator(0)});
    REQUIRE(S.size() == 119);
    REQUIRE(S.number_of_generators() == 14);
    REQUIRE(S.number_of_rules() == 253);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "129",
                          "add_generators [incremental 2]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> T;
    T.add_generator(Transf<>({0, 1, 0, 3, 4, 5}));
    T.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    T.add_generator(Transf<>({0, 1, 3, 5, 5, 4}));
    T.add_generator(Transf<>({1, 0, 2, 4, 4, 5}));
    T.add_generator(Transf<>({4, 3, 3, 1, 0, 5}));
    T.add_generator(Transf<>({4, 3, 5, 1, 0, 5}));
    T.add_generator(Transf<>({5, 5, 2, 3, 4, 0}));

    REQUIRE(T.size() == 119);

    FroidurePin<Transf<>> S({T.generator(0), T.generator(0)});

    S.add_generators({});
    S.add_generators({T.generator(0)});
    S.run();
    S.add_generators({T.generator(1)});
    S.run();
    S.add_generators({T.generator(2)});
    S.run();
    REQUIRE(S.current_size() == 7);
    S.add_generators({T.generator(3), T.generator(4), T.generator(5)});
    REQUIRE(S.number_of_generators() == 8);
    REQUIRE(S.current_position(5) == 7);
    REQUIRE(S.current_position(6) == 8);
    REQUIRE(S.current_position(7) == 9);
    REQUIRE(S.current_size() == 55);

    S.add_generator(S.at(44));
    REQUIRE(S.number_of_generators() == 9);
    REQUIRE(S.current_size() == 73);
    REQUIRE(S.size() == 97);

    S.add_generator(S.at(75));
    REQUIRE(S.number_of_generators() == 10);
    REQUIRE(S.current_size() == 97);
    REQUIRE(S.size() == 97);

    S.add_generators({T.generator(6)});
    REQUIRE(S.number_of_generators() == 11);
    REQUIRE(S.size() == 119);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "130",
                          "closure [duplicate generators]",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 0, 3, 4, 5}));
    S.add_generator(Transf<>({0, 1, 0, 3, 4, 5}));

    REQUIRE(S.size() == 1);
    REQUIRE(S.number_of_generators() == 2);

    S.closure({});
    REQUIRE(S.size() == 1);
    REQUIRE(S.number_of_generators() == 2);

    S.closure({S.generator(0)});
    REQUIRE(S.size() == 1);
    REQUIRE(S.number_of_generators() == 2);

    S.closure({Transf<>({0, 1, 2, 3, 4, 5})});
    REQUIRE(S.size() == 2);
    REQUIRE(S.number_of_generators() == 3);

    S.closure({Transf<>({0, 1, 3, 5, 5, 4})});
    REQUIRE(S.size() == 7);
    REQUIRE(S.number_of_generators() == 4);

    S.closure({Transf<>({1, 0, 2, 4, 4, 5})});
    REQUIRE(S.size() == 18);
    REQUIRE(S.number_of_generators() == 5);

    S.closure({Transf<>({4, 3, 3, 1, 0, 5})});
    REQUIRE(S.size() == 87);
    REQUIRE(S.number_of_generators() == 6);

    S.closure({Transf<>({4, 3, 5, 1, 0, 5})});
    REQUIRE(S.size() == 97);
    REQUIRE(S.number_of_generators() == 7);

    S.closure({Transf<>({5, 5, 2, 3, 4, 0})});
    REQUIRE(S.size() == 119);
    REQUIRE(S.number_of_generators() == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "131",
                          "closure ",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    std::vector<Transf<>> gens
        = {Transf<>({0, 0, 0}), Transf<>({0, 0, 1}), Transf<>({0, 0, 2}),
           Transf<>({0, 1, 0}), Transf<>({0, 1, 1}), Transf<>({0, 1, 2}),
           Transf<>({0, 2, 0}), Transf<>({0, 2, 1}), Transf<>({0, 2, 2}),
           Transf<>({1, 0, 0}), Transf<>({1, 0, 1}), Transf<>({1, 0, 2}),
           Transf<>({1, 1, 0}), Transf<>({1, 1, 1}), Transf<>({1, 1, 2}),
           Transf<>({1, 2, 0}), Transf<>({1, 2, 1}), Transf<>({1, 2, 2}),
           Transf<>({2, 0, 0}), Transf<>({2, 0, 1}), Transf<>({2, 0, 2}),
           Transf<>({2, 1, 0}), Transf<>({2, 1, 1}), Transf<>({2, 1, 2}),
           Transf<>({2, 2, 0}), Transf<>({2, 2, 1}), Transf<>({2, 2, 2})};

    S.add_generator(gens[0]);

    S.closure(gens);
    REQUIRE(S.size() == 27);
    REQUIRE(S.number_of_generators() == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "132",
                          "factorisation ",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 1, 4, 5, 4, 5}));
    S.add_generator(Transf<>({2, 3, 2, 3, 5, 5}));

    REQUIRE(S.factorisation(2) == word_type({0, 1}));
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "133",
                          "my favourite example with reserve",
                          "[standard][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 7, 2, 6, 0, 4, 1, 5}));
    S.add_generator(Transf<>({2, 4, 6, 1, 4, 5, 2, 7}));
    S.add_generator(Transf<>({3, 0, 7, 2, 4, 6, 2, 4}));
    S.add_generator(Transf<>({3, 2, 3, 4, 5, 3, 0, 1}));
    S.add_generator(Transf<>({4, 3, 7, 7, 4, 5, 0, 4}));
    S.add_generator(Transf<>({5, 6, 3, 0, 3, 0, 5, 1}));
    S.add_generator(Transf<>({6, 0, 1, 1, 1, 6, 3, 4}));
    S.add_generator(Transf<>({7, 7, 4, 0, 6, 4, 1, 7}));
    S.reserve(597369);

    REQUIRE(S.size() == 597369);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "134",
                          "minimal_factorisation ",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 1, 4, 5, 4, 5}));

    REQUIRE(S.minimal_factorisation(S.generator(0)) == word_type({0}));

    REQUIRE(S.factorisation(S.generator(0)) == word_type({0}));

    REQUIRE_THROWS_AS(S.minimal_factorisation(Transf<>({4, 1, 4, 1, 4, 5})),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(S.minimal_factorisation(10000000),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "135",
                          "batch_size (for an extremely large value)",
                          "[quick][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 1, 4, 5, 4, 5}));
    S.add_generator(Transf<>({2, 3, 2, 3, 5, 5}));

    S.batch_size(LIMIT_MAX);
    S.run();

    REQUIRE(S.size() == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "136",
                          "my favourite example without reserve",
                          "[standard][froidure-pin][transf]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 7, 2, 6, 0, 4, 1, 5}));
    S.add_generator(Transf<>({2, 4, 6, 1, 4, 5, 2, 7}));
    S.add_generator(Transf<>({3, 0, 7, 2, 4, 6, 2, 4}));
    S.add_generator(Transf<>({3, 2, 3, 4, 5, 3, 0, 1}));
    S.add_generator(Transf<>({4, 3, 7, 7, 4, 5, 0, 4}));
    S.add_generator(Transf<>({5, 6, 3, 0, 3, 0, 5, 1}));
    S.add_generator(Transf<>({6, 0, 1, 1, 1, 6, 3, 4}));
    S.add_generator(Transf<>({7, 7, 4, 0, 6, 4, 1, 7}));

    REQUIRE(S.size() == 597369);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "137",
                          "exception: generators of different degrees",
                          "[quick][froidure-pin][transf]") {
    REQUIRE_THROWS_AS(FroidurePin<Transf<>>({Transf<>({0, 1, 2, 3, 4, 5}),
                                             Transf<>({0, 1, 2, 3, 4, 5, 5})}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "138",
                          "exception: current_position",
                          "[quick][froidure-pin][transf]") {
    FroidurePin<Transf<>> U;
    U.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    U.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    U.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    U.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    U.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE_THROWS_AS(U.current_position({}), LibsemigroupsException);
    REQUIRE_NOTHROW(U.current_position({0, 0, 1, 2}));
    REQUIRE_THROWS_AS(U.current_position({5}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "139",
                          "exception: word_to_element",
                          "[quick][froidure-pin][transf]") {
    FroidurePin<Transf<>> U;
    U.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    U.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
    U.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
    U.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
    U.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

    REQUIRE_THROWS_AS(U.word_to_element({}), LibsemigroupsException);
    REQUIRE_THROWS_AS(U.word_to_element({5}), LibsemigroupsException);

    REQUIRE(U.word_to_element({0, 0, 1, 2})
            == U.generator(0) * U.generator(0) * U.generator(1)
                   * U.generator(2));
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "140",
                          "exception: gens, current_position",
                          "[quick][froidure-pin][transf]") {
    using value_type = typename Transf<>::value_type;
    for (size_t i = 1; i < 20; ++i) {
      std::vector<Transf<>> gens;
      for (size_t j = 0; j < i; ++j) {
        std::vector<value_type> trans;
        for (size_t k = 0; k < i; ++k) {
          trans.push_back((k + j) % i);
        }
        gens.push_back(Transf<>(trans));
      }
      FroidurePin<Transf<>> S(gens);

      for (size_t j = 0; j < i; ++j) {
        REQUIRE_NOTHROW(S.current_position(j));
        REQUIRE_NOTHROW(S.generator(j));
      }
      REQUIRE_THROWS_AS(S.generator(i), LibsemigroupsException);
      REQUIRE_THROWS_AS(S.current_position(i), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
                          "141",
                          "exception: add_generators",
                          "[quick][froidure-pin][transf]") {
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 2, 3, 2, 2, 3}));

    REQUIRE_NOTHROW(S.add_generator(Transf<>({0, 1, 2, 3, 3, 3})));
    REQUIRE_THROWS_AS(S.add_generator(Transf<>({0, 1, 2, 3, 3, 3, 3})),
                      LibsemigroupsException);
  }

}  // namespace libsemigroups
