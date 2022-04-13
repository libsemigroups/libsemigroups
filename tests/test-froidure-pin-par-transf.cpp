//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 Ewan Gilligan
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

#include "catch.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/froidure-pin-parallel.hpp"  // for FroidurePinParallel<>::element_index_type
#include "libsemigroups/froidure-pin.hpp"
#include "libsemigroups/transf.hpp"  // for Transf
#include "test-main.hpp"

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  namespace {
    template <typename T>
    void test_same_result(std::vector<T> gens) {
      auto rg = ReportGuard(REPORT);

      FroidurePin<T>         S;
      FroidurePinParallel<T> S_par(4);
      S.add_generators(gens.begin(), gens.end());
      S_par.add_generators(gens.begin(), gens.end());

      REQUIRE(S_par.size() == S.size());

      // Check all elements are the same
      auto it     = S.cbegin_sorted();
      auto it_par = S_par.cbegin_sorted();
      while (it < S.cend_sorted() && it_par < S_par.cend_sorted()) {
        REQUIRE(EqualTo<T>()(*it, *it_par));
        it++;
        it_par++;
      }
    }

    template <typename T>
    void test000() {
      auto rg = ReportGuard(REPORT);

      std::vector<T> gens = {T({1, 7, 2, 6, 0, 4, 1, 5}),
                             T({2, 4, 6, 1, 4, 5, 2, 7}),
                             T({3, 0, 7, 2, 4, 6, 2, 4}),
                             T({3, 2, 3, 4, 5, 3, 0, 1}),
                             T({4, 3, 7, 7, 4, 5, 0, 4}),
                             T({5, 6, 3, 0, 3, 0, 5, 1}),
                             T({6, 0, 1, 1, 1, 6, 3, 4}),
                             T({7, 7, 4, 0, 6, 4, 1, 7})};

      test_same_result<T>(gens);
    }

    template <typename T>
    void test001() {
      auto           rg = ReportGuard(REPORT);
      std::vector<T> gens1;
      REQUIRE_NOTHROW(FroidurePinParallel<T>(gens1, 1));
    }

    template <typename T>
    void test002() {
      auto                   rg = ReportGuard(REPORT);
      FroidurePinParallel<T> S(2);
      S.add_generator(T({2, 4, 6, 1, 4, 5, 2, 7, 3}));
      // For dynamic Transf exception is thrown by FroidurePin because degree
      // is wrong, for static Transf exception is thrown by make, because the
      // container has the wrong size
      REQUIRE_THROWS_AS(S.add_generator(T::make({1, 7, 2, 6, 0, 0, 1, 2})),
                        LibsemigroupsException);
    }

  }  // namespace

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePinParallel<Transf<>>",
      "063",
      "JDM favourite (dynamic)",
      "[standard][froidure-pin-parallel][transformation][transf]") {
    test000<Transf<>>();
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePinParallel<Transf<>>",
      "064",
      "JDM favourite (static)",
      "[standard][froidure-pin-parallel][transformation][transf]") {
    test000<Transf<8>>();
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePinParallel<Transf<>>",
      "065",
      "no exception zero generators given",
      "[quick][froidure-pin-parallel][transformation][transf]") {
    test001<Transf<>>();
    test001<Transf<8>>();
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePinParallel<Transf<>>",
      "066",
      "exception generators of "
      "different degrees",
      "[quick][froidure-pin-parallel][transformation][transf]") {
    test002<Transf<>>();
    test002<Transf<9>>();
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePinParallel<Transf<>>",
      "067",
      "exception adding generators after start",
      "[quick][froidure-pin-parallel][transformation][transf]") {
    auto                          rg = ReportGuard(REPORT);
    FroidurePinParallel<Transf<>> S(2);
    S.add_generator(Transf<>({1, 2, 0}));
    S.run();
    REQUIRE_THROWS_AS(S.add_generator(Transf<>({1, 1, 1})),
                      LibsemigroupsException);
  }

  // LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
  //                         "067",
  //                         "exception current_position",
  //                         "[quick][froidure-pin][transformation][transf]")
  //                         {
  //   auto                  rg   = ReportGuard(REPORT);
  //   std::vector<Transf<>> gens = {Transf<>({0, 1, 2, 3, 4, 5}),
  //                                 Transf<>({1, 0, 2, 3, 4, 5}),
  //                                 Transf<>({4, 0, 1, 2, 3, 5}),
  //                                 Transf<>({5, 1, 2, 3, 4, 5}),
  //                                 Transf<>({1, 1, 2, 3, 4, 5})};
  //   FroidurePin<Transf<>> U(gens);

  //   REQUIRE_THROWS_AS(U.current_position({}), LibsemigroupsException);
  //   REQUIRE_NOTHROW(U.current_position({0, 0, 1, 2}));
  //   REQUIRE_THROWS_AS(U.current_position({5}), LibsemigroupsException);
  // }

  // LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
  //                         "077",
  //                         "exception current_position",
  //                         "[quick][froidure-pin][transformation][transf]")
  //                         {
  //   auto rg = ReportGuard(REPORT);
  //   for (size_t i = 1; i < 20; ++i) {
  //     std::vector<Transf<>> gens;
  //     using value_type = typename Transf<>::value_type;

  //     for (size_t j = 0; j < i; ++j) {
  //       std::vector<value_type> trans;
  //       for (size_t k = 0; k < i; ++k) {
  //         trans.push_back((k + j) % i);
  //       }
  //       gens.push_back(Transf<>(trans));
  //     }
  //     FroidurePin<Transf<>> S(gens);

  //     for (size_t j = 0; j < i; ++j) {
  //       REQUIRE_NOTHROW(S.current_position(j));
  //     }
  //     REQUIRE_THROWS_AS(S.current_position(i), LibsemigroupsException);
  //   }
  // }

  // LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
  //                         "079",
  //                         "exception add_generators",
  //                         "[quick][froidure-pin][transformation][transf]")
  //                         {
  //   auto                  rg   = ReportGuard(REPORT);
  //   std::vector<Transf<>> gens = {Transf<>({1, 7, 2, 6, 0, 0, 1, 2}),
  //                                 Transf<>({2, 4, 6, 1, 4, 5, 2, 7})};
  //   FroidurePin<Transf<>> T(gens);

  //   std::vector<Transf<>> additional_gens_1 = {
  //       Transf<>({1, 2, 2, 2, 1, 1, 3, 4}), Transf<>({1, 2, 1, 3, 1, 4, 1,
  //       5})};
  //   std::vector<Transf<>> additional_gens_2
  //       = {Transf<>({1, 2, 2, 2, 1, 1, 3, 4}),
  //          Transf<>({1, 2, 1, 3, 1, 4, 1, 5, 1})};

  //   REQUIRE_NOTHROW(T.add_generators(additional_gens_1));
  //   REQUIRE_THROWS_AS(T.add_generators(additional_gens_2),
  //                     LibsemigroupsException);
  // }

  LIBSEMIGROUPS_TEST_CASE("FroidurePinParallel<Transf<>>",
                          "081",
                          "small semigroup",
                          "[quick][froidure-pin-parallel][transf]") {
    std::vector<Transf<>> gens = {Transf<>({0, 1, 0}), Transf<>({0, 1, 2})};
    test_same_result<Transf<>>(gens);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePinParallel<Transf<>>",
      "082",
      "large semigroup",
      "[quick][froidure-pin-parallel][transf][no-valgrind]") {
    std::vector<Transf<>> gens = {Transf<>({0, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 0, 2, 3, 4, 5}),
                                  Transf<>({4, 0, 1, 2, 3, 5}),
                                  Transf<>({5, 1, 2, 3, 4, 5}),
                                  Transf<>({1, 1, 2, 3, 4, 5})};
    test_same_result<Transf<>>(gens);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePinParallel<Transf<>>",
      "083",
      "monogenic semigroup",
      "[quick][froidure-pin-parallel][transf][no-valgrind]") {
    std::vector<Transf<>> gens
        = {Transf<>({2, 2, 2, 5, 9, 8, 5, 1, 1, 7, 4, 6, 9})};
    test_same_result<Transf<>>(gens);
  }

  // LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
  //                         "084",
  //                         "run",
  //                         "[quick][froidure-pin][transf][no-valgrind]") {
  //   auto                  rg = ReportGuard(REPORT);
  //   FroidurePin<Transf<>> S;
  //   S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
  //   S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
  //   S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
  //   S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
  //   S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

  //   S.batch_size(1024);

  //   S.enumerate(3000);
  //   REQUIRE(S.current_size() == 3000);
  //   REQUIRE(S.current_number_of_rules() == 526);
  //   REQUIRE(S.current_max_word_length() == 9);

  //   S.enumerate(3001);
  //   REQUIRE(S.current_size() == 4024);
  //   REQUIRE(S.current_number_of_rules() == 999);
  //   REQUIRE(S.current_max_word_length() == 10);

  //   S.enumerate(7000);
  //   REQUIRE(S.current_size() == 7000);
  //   REQUIRE(S.current_number_of_rules() == 2044);
  //   REQUIRE(S.current_max_word_length() == 12);

  //   REQUIRE(S.size() == 7776);
  //   REQUIRE(S.degree() == 6);
  //   REQUIRE(S.number_of_idempotents() == 537);
  //   REQUIRE(S.number_of_generators() == 5);
  //   REQUIRE(S.number_of_rules() == 2459);
  // }

  // LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
  //                         "085",
  //                         "run [many stops and starts]",
  //                         "[quick][froidure-pin][transf][no-valgrind]") {
  //   auto rg = ReportGuard(REPORT);

  //   FroidurePin<Transf<>> S;
  //   S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
  //   S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
  //   S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
  //   S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
  //   S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));

  //   S.batch_size(128);

  //   for (size_t i = 1; !S.finished(); i++) {
  //     S.enumerate(i * 128);
  //   }

  //   REQUIRE(S.size() == 7776);
  //   REQUIRE(S.degree() == 6);
  //   REQUIRE(S.number_of_idempotents() == 537);
  //   REQUIRE(S.number_of_generators() == 5);
  //   REQUIRE(S.number_of_rules() == 2459);
  // }

  // LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
  //                         "096",
  //                         "finished, started",
  //                         "[quick][froidure-pin][transf]") {
  //   auto                  rg = ReportGuard(REPORT);
  //   FroidurePin<Transf<>> S;
  //   S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
  //   S.add_generator(Transf<>({1, 0, 2, 3, 4, 5}));
  //   S.add_generator(Transf<>({4, 0, 1, 2, 3, 5}));
  //   S.add_generator(Transf<>({5, 1, 2, 3, 4, 5}));
  //   S.add_generator(Transf<>({1, 1, 2, 3, 4, 5}));
  //
  //   REQUIRE(!S.started());
  //   REQUIRE(!S.finished());

  //   S.batch_size(1024);
  //   S.enumerate(10);
  //   REQUIRE(S.started());
  //   REQUIRE(!S.finished());

  //   S.enumerate(8000);
  //   REQUIRE(S.started());
  //   REQUIRE(S.finished());
  // }

  // LIBSEMIGROUPS_TEST_CASE("FroidurePin<Transf<>>",
  //                         "135",
  //                         "batch_size (for an extremely large value)",
  //                         "[quick][froidure-pin][transf]") {
  //   auto                  rg = ReportGuard(REPORT);
  //   FroidurePin<Transf<>> S;
  //   S.add_generator(Transf<>({1, 1, 4, 5, 4, 5}));
  //   S.add_generator(Transf<>({2, 3, 2, 3, 5, 5}));

  //   S.batch_size(LIMIT_MAX);
  //   S.run();

  //   REQUIRE(S.size() == 5);
  // }

  LIBSEMIGROUPS_TEST_CASE("FroidurePinParallel<Transf<>>",
                          "137",
                          "exception: generators of different degrees",
                          "[quick][froidure-pin-parallel][transf]") {
    std::vector<Transf<>> gens
        = {Transf<>({0, 1, 2, 3, 4, 5}), Transf<>({0, 1, 2, 3, 4, 5, 5})};
    REQUIRE_THROWS_AS(FroidurePinParallel<Transf<>>(gens, 2),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePinParallel<Transf<>>",
                          "141",
                          "exception: add_generators",
                          "[quick][froidure-pin-parallel][transf]") {
    FroidurePinParallel<Transf<>> S(1);
    S.add_generator(Transf<>({0, 1, 2, 3, 4, 5}));
    S.add_generator(Transf<>({1, 2, 3, 2, 2, 3}));

    REQUIRE_NOTHROW(S.add_generator(Transf<>({0, 1, 2, 3, 3, 3})));
    REQUIRE_THROWS_AS(S.add_generator(Transf<>({0, 1, 2, 3, 3, 3, 3})),
                      LibsemigroupsException);
  }
  LIBSEMIGROUPS_TEST_CASE("FroidurePinParallel<Transf<>>",
                          "142",
                          "exception: zero threads",
                          "[quick][froidure-pin-parallel][transf]") {
    REQUIRE_THROWS_AS(FroidurePinParallel<Transf<>>(0), LibsemigroupsException);
  }
}  // namespace libsemigroups
