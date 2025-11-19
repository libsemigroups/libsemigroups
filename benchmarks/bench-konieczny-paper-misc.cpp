//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Finn Smith
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

#include <cstddef>  // for size_t
#include <string>   // for to_string

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for TEST_CASE, BENCHMARK, REQUIRE

#include "libsemigroups/bmat-fastest.hpp"  //
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/konieczny.hpp"     // for Konieczny
#include "libsemigroups/transf.hpp"

namespace libsemigroups {

  using LTransf = HPCombi::Transf16;
  using LPPerm  = HPCombi::PPerm16;
  using BM      = BMatFastest<5>;

  TEST_CASE("Example 1: full transformation monoid - deg 5 to 11",
            "[000][standard]") {
    auto rg = ReportGuard(false);

    BENCHMARK("Konieczny: deg 5") {
      std::vector<LTransf> gens = {make<LTransf>({1, 1, 2, 3, 4}),
                                   make<LTransf>({1, 0, 2, 3, 4}),
                                   make<LTransf>({4, 0, 1, 2, 3})};
      Konieczny            S    = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 6") {
      std::vector<LTransf> gens = {make<LTransf>({1, 1, 2, 3, 4, 5}),
                                   make<LTransf>({1, 0, 2, 3, 4, 5}),
                                   make<LTransf>({5, 0, 1, 2, 3, 4})};
      Konieczny            S    = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 7") {
      std::vector<LTransf> gens = {make<LTransf>({1, 1, 2, 3, 4, 5, 6}),
                                   make<LTransf>({1, 0, 2, 3, 4, 5, 6}),
                                   make<LTransf>({6, 0, 1, 2, 3, 4, 5})};
      Konieczny            S    = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 8") {
      std::vector<LTransf> gens = {make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({1, 0, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({7, 0, 1, 2, 3, 4, 5, 6})};
      Konieczny            S    = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 9") {
      std::vector<LTransf> gens = {make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8}),
                                   make<LTransf>({1, 0, 2, 3, 4, 5, 6, 7, 8}),
                                   make<LTransf>({8, 0, 1, 2, 3, 4, 5, 6, 7})};
      Konieczny            S    = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 10") {
      std::vector<LTransf> gens
          = {make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({9, 0, 1, 2, 3, 4, 5, 6, 7, 8})};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 11") {
      std::vector<LTransf> gens
          = {make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9})};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };
  }

  TEST_CASE("Example 2: full transformation monoid - deg 5 to 8",
            "[001][standard]") {
    auto rg = ReportGuard(false);

    BENCHMARK("FroidurePin: deg 5") {
      std::vector<LTransf> gens = {make<LTransf>({1, 1, 2, 3, 4}),
                                   make<LTransf>({1, 0, 2, 3, 4}),
                                   make<LTransf>({4, 0, 1, 2, 3})};
      FroidurePin          S    = make<FroidurePin>(gens);
      S.run();
    };

    BENCHMARK("FroidurePin: deg 6") {
      std::vector<LTransf> gens = {make<LTransf>({1, 1, 2, 3, 4, 5}),
                                   make<LTransf>({1, 0, 2, 3, 4, 5}),
                                   make<LTransf>({5, 0, 1, 2, 3, 4})};
      FroidurePin          S    = make<FroidurePin>(gens);
      S.run();
    };

    BENCHMARK("FroidurePin: deg 7") {
      std::vector<LTransf> gens = {make<LTransf>({1, 1, 2, 3, 4, 5, 6}),
                                   make<LTransf>({1, 0, 2, 3, 4, 5, 6}),
                                   make<LTransf>({6, 0, 1, 2, 3, 4, 5})};
      FroidurePin          S    = make<FroidurePin>(gens);
      S.run();
    };

    BENCHMARK("FroidurePin: deg 8") {
      std::vector<LTransf> gens = {make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({1, 0, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({7, 0, 1, 2, 3, 4, 5, 6})};
      FroidurePin          S    = make<FroidurePin>(gens);
      S.run();
    };
  }

  TEST_CASE("Example 3: symmetric inverse monoid - deg 6 to 11",
            "[002][standard]") {
    auto rg = ReportGuard(false);

    BENCHMARK("Konieczny: deg 5") {
      std::vector<LPPerm> gens
          = {make<LPPerm>({0, 1, 2, 3, 4}, {1, 0, 2, 3, 4}, 5),
             make<LPPerm>({0, 1, 2, 3, 4}, {4, 0, 1, 2, 3}, 5),
             make<LPPerm>({0, 1, 2, 3, 4}, {UNDEFINED, 1, 2, 3, 4}, 5)};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 6") {
      std::vector<LPPerm> gens
          = {make<LPPerm>({0, 1, 2, 3, 4, 5}, {1, 0, 2, 3, 4, 5}, 6),
             make<LPPerm>({0, 1, 2, 3, 4, 5}, {5, 0, 1, 2, 3, 4}, 6),
             make<LPPerm>({0, 1, 2, 3, 4, 5}, {UNDEFINED, 1, 2, 3, 4, 5}, 6)};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 7") {
      std::vector<LPPerm> gens
          = {make<LPPerm>({0, 1, 2, 3, 4, 5, 6}, {1, 0, 2, 3, 4, 5, 6}, 7),
             make<LPPerm>({0, 1, 2, 3, 4, 5, 6}, {6, 0, 1, 2, 3, 4, 5}, 7),
             make<LPPerm>(
                 {0, 1, 2, 3, 4, 5, 6}, {UNDEFINED, 1, 2, 3, 4, 5, 6}, 7)};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 8") {
      std::vector<LPPerm> gens = {
          make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 2, 3, 4, 5, 6, 7}, 8),
          make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7}, {7, 0, 1, 2, 3, 4, 5, 6}, 8),
          make<LPPerm>(
              {0, 1, 2, 3, 4, 5, 6, 7}, {UNDEFINED, 1, 2, 3, 4, 5, 6, 7}, 8)};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 9") {
      std::vector<LPPerm> gens
          = {make<LPPerm>(
                 {0, 1, 2, 3, 4, 5, 6, 7, 8}, {1, 0, 2, 3, 4, 5, 6, 7, 8}, 9),
             make<LPPerm>(
                 {0, 1, 2, 3, 4, 5, 6, 7, 8}, {8, 0, 1, 2, 3, 4, 5, 6, 7}, 9),
             make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8},
                          {UNDEFINED, 1, 2, 3, 4, 5, 6, 7, 8},
                          9)};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 10") {
      std::vector<LPPerm> gens
          = {make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                          {1, 0, 2, 3, 4, 5, 6, 7, 8, 9},
                          10),
             make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                          {9, 0, 1, 2, 3, 4, 5, 6, 7, 8},
                          10),
             make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                          {UNDEFINED, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                          10)};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 11") {
      std::vector<LPPerm> gens
          = {make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                          {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                          11),
             make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                          {10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                          11),
             make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                          {UNDEFINED, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                          11)};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };
  }

  TEST_CASE("Example 4: symmetric inverse monoid - deg 6 to 11",
            "[003][standard]") {
    auto rg = ReportGuard(false);

    BENCHMARK("FroidurePin: deg 5") {
      std::vector<LPPerm> gens
          = {make<LPPerm>({0, 1, 2, 3, 4}, {1, 0, 2, 3, 4}, 5),
             make<LPPerm>({0, 1, 2, 3, 4}, {4, 0, 1, 2, 3}, 5),
             make<LPPerm>({0, 1, 2, 3, 4}, {UNDEFINED, 1, 2, 3, 4}, 5)};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };

    BENCHMARK("FroidurePin: deg 6") {
      std::vector<LPPerm> gens
          = {make<LPPerm>({0, 1, 2, 3, 4, 5}, {1, 0, 2, 3, 4, 5}, 6),
             make<LPPerm>({0, 1, 2, 3, 4, 5}, {5, 0, 1, 2, 3, 4}, 6),
             make<LPPerm>({0, 1, 2, 3, 4, 5}, {UNDEFINED, 1, 2, 3, 4, 5}, 6)};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };

    BENCHMARK("FroidurePin: deg 7") {
      std::vector<LPPerm> gens
          = {make<LPPerm>({0, 1, 2, 3, 4, 5, 6}, {1, 0, 2, 3, 4, 5, 6}, 7),
             make<LPPerm>({0, 1, 2, 3, 4, 5, 6}, {6, 0, 1, 2, 3, 4, 5}, 7),
             make<LPPerm>(
                 {0, 1, 2, 3, 4, 5, 6}, {UNDEFINED, 1, 2, 3, 4, 5, 6}, 7)};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };

    BENCHMARK("FroidurePin: deg 8") {
      std::vector<LPPerm> gens = {
          make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 2, 3, 4, 5, 6, 7}, 8),
          make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7}, {7, 0, 1, 2, 3, 4, 5, 6}, 8),
          make<LPPerm>(
              {0, 1, 2, 3, 4, 5, 6, 7}, {UNDEFINED, 1, 2, 3, 4, 5, 6, 7}, 8)};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };

    BENCHMARK("FroidurePin: deg 9") {
      std::vector<LPPerm> gens
          = {make<LPPerm>(
                 {0, 1, 2, 3, 4, 5, 6, 7, 8}, {1, 0, 2, 3, 4, 5, 6, 7, 8}, 9),
             make<LPPerm>(
                 {0, 1, 2, 3, 4, 5, 6, 7, 8}, {8, 0, 1, 2, 3, 4, 5, 6, 7}, 9),
             make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8},
                          {UNDEFINED, 1, 2, 3, 4, 5, 6, 7, 8},
                          9)};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };

    /*
    BENCHMARK("FroidurePin: deg 10") {
      std::vector<LPPerm> gens
          = {make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                          {1, 0, 2, 3, 4, 5, 6, 7, 8, 9},
                          10),
             make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                          {9, 0, 1, 2, 3, 4, 5, 6, 7, 8},
                          10),
             make<LPPerm>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                          {UNDEFINED, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                          10)};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };
    */
  }

  TEST_CASE("Example 5: order endomorphisms - deg 6 to 14", "[004][standard]") {
    auto rg = ReportGuard(false);

    BENCHMARK("Konieczny: deg 6") {
      std::vector<LTransf> gens = {make<LTransf>({0, 1, 2, 3, 4, 5}),
                                   make<LTransf>({0, 0, 1, 2, 3, 4}),
                                   make<LTransf>({1, 1, 2, 3, 4, 5}),
                                   make<LTransf>({0, 2, 2, 3, 4, 5}),
                                   make<LTransf>({0, 1, 3, 3, 4, 5}),
                                   make<LTransf>({0, 1, 2, 4, 4, 5}),
                                   make<LTransf>({0, 1, 2, 3, 5, 5})};
      Konieczny            S    = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 7") {
      std::vector<LTransf> gens = {make<LTransf>({0, 1, 2, 3, 4, 5, 6}),
                                   make<LTransf>({0, 0, 1, 2, 3, 4, 5}),
                                   make<LTransf>({1, 1, 2, 3, 4, 5, 6}),
                                   make<LTransf>({0, 2, 2, 3, 4, 5, 6}),
                                   make<LTransf>({0, 1, 3, 3, 4, 5, 6}),
                                   make<LTransf>({0, 1, 2, 4, 4, 5, 6}),
                                   make<LTransf>({0, 1, 2, 3, 5, 5, 6}),
                                   make<LTransf>({0, 1, 2, 3, 4, 6, 6})};
      Konieczny            S    = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 8") {
      std::vector<LTransf> gens = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6}),
                                   make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7}),
                                   make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7}),
                                   make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7}),
                                   make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7}),
                                   make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7})};
      Konieczny            S    = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 9") {
      std::vector<LTransf> gens = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8}),
                                   make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8}),
                                   make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8}),
                                   make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8}),
                                   make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8}),
                                   make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8}),
                                   make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8}),
                                   make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8}),
                                   make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8})};
      Konieczny            S    = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 10") {
      std::vector<LTransf> gens
          = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7, 8}),
             make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8, 9}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8, 9}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8, 9}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 9, 9})};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 11") {
      std::vector<LTransf> gens
          = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 10})};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 12") {
      std::vector<LTransf> gens
          = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 11})};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 13") {
      std::vector<LTransf> gens
          = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 12})};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 14") {
      std::vector<LTransf> gens
          = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 13})};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };
  }

  TEST_CASE("Example 6: order endomorphisms - deg 6 to 14", "[005][standard]") {
    auto rg = ReportGuard(false);

    BENCHMARK("FroidurePin: deg 6") {
      std::vector<LTransf> gens = {make<LTransf>({0, 1, 2, 3, 4, 5}),
                                   make<LTransf>({0, 0, 1, 2, 3, 4}),
                                   make<LTransf>({1, 1, 2, 3, 4, 5}),
                                   make<LTransf>({0, 2, 2, 3, 4, 5}),
                                   make<LTransf>({0, 1, 3, 3, 4, 5}),
                                   make<LTransf>({0, 1, 2, 4, 4, 5}),
                                   make<LTransf>({0, 1, 2, 3, 5, 5})};
      FroidurePin          S    = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 7") {
      std::vector<LTransf> gens = {make<LTransf>({0, 1, 2, 3, 4, 5, 6}),
                                   make<LTransf>({0, 0, 1, 2, 3, 4, 5}),
                                   make<LTransf>({1, 1, 2, 3, 4, 5, 6}),
                                   make<LTransf>({0, 2, 2, 3, 4, 5, 6}),
                                   make<LTransf>({0, 1, 3, 3, 4, 5, 6}),
                                   make<LTransf>({0, 1, 2, 4, 4, 5, 6}),
                                   make<LTransf>({0, 1, 2, 3, 5, 5, 6}),
                                   make<LTransf>({0, 1, 2, 3, 4, 6, 6})};
      FroidurePin          S    = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 8") {
      std::vector<LTransf> gens = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6}),
                                   make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7}),
                                   make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7}),
                                   make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7}),
                                   make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7}),
                                   make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7})};
      FroidurePin          S    = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 9") {
      std::vector<LTransf> gens = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8}),
                                   make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7}),
                                   make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8}),
                                   make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8}),
                                   make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8}),
                                   make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8}),
                                   make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8}),
                                   make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8}),
                                   make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8}),
                                   make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8})};
      FroidurePin          S    = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 10") {
      std::vector<LTransf> gens
          = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7, 8}),
             make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8, 9}),
             make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8, 9}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8, 9}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8, 9}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 9, 9})};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 11") {
      std::vector<LTransf> gens
          = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9}),
             make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 10}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 10})};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 12") {
      std::vector<LTransf> gens
          = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
             make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 10, 11}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 11})};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 13") {
      std::vector<LTransf> gens
          = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
             make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 10, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 11, 12}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 12})};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 14") {
      std::vector<LTransf> gens
          = {make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}),
             make<LTransf>({1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 6, 6, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 7, 7, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 10, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 11, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 12, 13}),
             make<LTransf>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 13})};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };
  }
  TEST_CASE("Example 7: unitriangular bmats - deg 3 to 6", "[006][standard]") {
    auto rg = ReportGuard(false);

    BENCHMARK("Konieczny: deg 3") {
      std::vector<BM> gens = {BM({{1, 1, 0}, {0, 1, 0}, {0, 0, 1}}),
                              BM({{1, 0, 1}, {0, 1, 0}, {0, 0, 1}}),
                              BM({{1, 0, 0}, {0, 1, 1}, {0, 0, 1}})};
      Konieczny       S    = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 4") {
      std::vector<BM> gens
          = {BM({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 0, 1, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 0, 0, 1}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 0, 0, 0}, {0, 1, 0, 1}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}})};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };

    BENCHMARK("Konieczny: deg 5") {
      std::vector<BM> gens = {BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 1, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 1, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 1, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 1},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 1, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 1, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 1},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 1, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 1},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 1},
                                  {0, 0, 0, 0, 1}})};
      Konieczny       S    = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 6") {
      std::vector<BM> gens = {BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 1, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 1, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 1, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 1},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 1, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 1, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 1},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 1, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 1, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 1},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 1, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 1},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 1},
                                  {0, 0, 0, 0, 0, 1}})};
      Konieczny       S    = make<Konieczny>(gens);
      S.run();
    };
  }

  TEST_CASE("Example 8: unitriangular bmats - deg 3 to 6", "[007][standard]") {
    auto rg = ReportGuard(false);

    BENCHMARK("FroidurePin: deg 3") {
      std::vector<BM> gens = {BM({{1, 1, 0}, {0, 1, 0}, {0, 0, 1}}),
                              BM({{1, 0, 1}, {0, 1, 0}, {0, 0, 1}}),
                              BM({{1, 0, 0}, {0, 1, 1}, {0, 0, 1}})};
      FroidurePin     S    = make<FroidurePin>(gens);
      S.run();
    };

    BENCHMARK("FroidurePin: deg 4") {
      std::vector<BM> gens
          = {BM({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 0, 1, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 0, 0, 1}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 0, 0, 0}, {0, 1, 0, 1}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}})};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };

    BENCHMARK("FroidurePin: deg 5") {
      std::vector<BM> gens = {BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 1, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 1, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 1, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 1},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 1, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 1, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 1},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 1, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 1},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 1},
                                  {0, 0, 0, 0, 1}})};
      FroidurePin     S    = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 6") {
      std::vector<BM> gens = {BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 1, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 1, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 1, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 1},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 1, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 1, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 1},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 1, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 1, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 1},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 1, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 1},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1}}),
                              BM({{1, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 1},
                                  {0, 0, 0, 0, 0, 1}})};
      FroidurePin     S    = make<FroidurePin>(gens);
      S.run();
    };
  }

  TEST_CASE("Example 9: hall monoid - deg 3 to 5", "[008][standard]") {
    auto rg = ReportGuard(false);

    BENCHMARK("Konieczny: deg 3") {
      std::vector<BM> gens = {BM({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}),
                              BM({{0, 1, 1}, {1, 0, 1}, {1, 1, 0}}),
                              BM({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                              BM({{0, 0, 1}, {0, 1, 0}, {1, 0, 1}}),
                              BM({{0, 0, 1}, {0, 1, 0}, {1, 0, 0}})};
      Konieczny       S    = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 4") {
      std::vector<BM> gens
          = {BM({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
             BM({{0, 0, 1, 1}, {0, 1, 0, 1}, {1, 0, 1, 0}, {1, 1, 0, 0}}),
             BM({{0, 0, 1, 1}, {0, 1, 0, 1}, {1, 0, 0, 1}, {1, 1, 1, 0}}),
             BM({{0, 0, 0, 1}, {0, 1, 1, 0}, {1, 0, 1, 0}, {1, 1, 0, 0}}),
             BM({{0, 0, 0, 1}, {0, 0, 1, 0}, {0, 1, 0, 0}, {1, 0, 0, 1}})};
      Konieczny S = make<Konieczny>(gens);
      S.run();
    };
    BENCHMARK("Konieczny: deg 5") {
      std::vector<BM> gens = {BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1},
                                  {1, 0, 0, 0, 0}}),
                              BM({{0, 1, 0, 0, 0},
                                  {1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{0, 0, 0, 0, 1},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {1, 0, 0, 0, 1}}),
                              BM({{0, 0, 0, 0, 1},
                                  {0, 0, 0, 1, 0},
                                  {0, 1, 1, 0, 0},
                                  {1, 0, 1, 0, 0},
                                  {1, 1, 0, 0, 0}}),
                              BM({{0, 0, 0, 0, 1},
                                  {0, 0, 1, 1, 0},
                                  {0, 1, 0, 1, 0},
                                  {1, 0, 0, 1, 0},
                                  {1, 1, 1, 0, 0}}),
                              BM({{0, 0, 0, 0, 1},
                                  {0, 0, 1, 1, 0},
                                  {0, 1, 0, 1, 0},
                                  {1, 0, 1, 0, 0},
                                  {1, 1, 0, 0, 0}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 0, 0, 1},
                                  {1, 0, 0, 0, 1},
                                  {1, 1, 1, 1, 0}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 0, 0, 1},
                                  {1, 0, 0, 1, 0},
                                  {1, 1, 1, 0, 0}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 0, 1, 0},
                                  {1, 0, 1, 0, 0},
                                  {1, 1, 0, 0, 1}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 0, 1, 0},
                                  {1, 0, 1, 1, 0},
                                  {1, 1, 0, 0, 1}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 1, 1, 0},
                                  {1, 0, 1, 1, 0},
                                  {1, 1, 0, 0, 1}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 0, 1, 0},
                                  {1, 0, 1, 0, 0},
                                  {1, 1, 0, 0, 0}})};
      Konieczny       S    = make<Konieczny>(gens);
      S.run();
    };
  }

  TEST_CASE("Example 10: hall monoid - deg 3 to 5", "[009][standard]") {
    auto rg = ReportGuard(false);

    BENCHMARK("FroidurePin: deg 3") {
      std::vector<BM> gens = {BM({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}),
                              BM({{0, 1, 1}, {1, 0, 1}, {1, 1, 0}}),
                              BM({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                              BM({{0, 0, 1}, {0, 1, 0}, {1, 0, 1}}),
                              BM({{0, 0, 1}, {0, 1, 0}, {1, 0, 0}})};
      FroidurePin     S    = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 4") {
      std::vector<BM> gens
          = {BM({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BM({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
             BM({{0, 0, 1, 1}, {0, 1, 0, 1}, {1, 0, 1, 0}, {1, 1, 0, 0}}),
             BM({{0, 0, 1, 1}, {0, 1, 0, 1}, {1, 0, 0, 1}, {1, 1, 1, 0}}),
             BM({{0, 0, 0, 1}, {0, 1, 1, 0}, {1, 0, 1, 0}, {1, 1, 0, 0}}),
             BM({{0, 0, 0, 1}, {0, 0, 1, 0}, {0, 1, 0, 0}, {1, 0, 0, 1}})};
      FroidurePin S = make<FroidurePin>(gens);
      S.run();
    };
    BENCHMARK("FroidurePin: deg 5") {
      std::vector<BM> gens = {BM({{1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1},
                                  {1, 0, 0, 0, 0}}),
                              BM({{0, 1, 0, 0, 0},
                                  {1, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 1}}),
                              BM({{0, 0, 0, 0, 1},
                                  {0, 0, 0, 1, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {1, 0, 0, 0, 1}}),
                              BM({{0, 0, 0, 0, 1},
                                  {0, 0, 0, 1, 0},
                                  {0, 1, 1, 0, 0},
                                  {1, 0, 1, 0, 0},
                                  {1, 1, 0, 0, 0}}),
                              BM({{0, 0, 0, 0, 1},
                                  {0, 0, 1, 1, 0},
                                  {0, 1, 0, 1, 0},
                                  {1, 0, 0, 1, 0},
                                  {1, 1, 1, 0, 0}}),
                              BM({{0, 0, 0, 0, 1},
                                  {0, 0, 1, 1, 0},
                                  {0, 1, 0, 1, 0},
                                  {1, 0, 1, 0, 0},
                                  {1, 1, 0, 0, 0}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 0, 0, 1},
                                  {1, 0, 0, 0, 1},
                                  {1, 1, 1, 1, 0}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 0, 0, 1},
                                  {1, 0, 0, 1, 0},
                                  {1, 1, 1, 0, 0}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 0, 1, 0},
                                  {1, 0, 1, 0, 0},
                                  {1, 1, 0, 0, 1}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 0, 1, 0},
                                  {1, 0, 1, 1, 0},
                                  {1, 1, 0, 0, 1}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 1, 1, 0},
                                  {1, 0, 1, 1, 0},
                                  {1, 1, 0, 0, 1}}),
                              BM({{0, 0, 0, 1, 1},
                                  {0, 0, 1, 0, 1},
                                  {0, 1, 0, 1, 0},
                                  {1, 0, 1, 0, 0},
                                  {1, 1, 0, 0, 0}})};
      FroidurePin     S    = make<FroidurePin>(gens);
      S.run();
    };
  }

}  // namespace libsemigroups