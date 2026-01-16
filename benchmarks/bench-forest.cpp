// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 James D. Mitchell
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

// This file contains some benchmarks for forest::PathsFromRoots.

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/forest.hpp"        // for Forest
#include "libsemigroups/todd-coxeter.hpp"  // for ToddCoxeter
#include "libsemigroups/word-range.hpp"    // for operator""_p

namespace libsemigroups {

  namespace {
    Forest forest_example(size_t tail, size_t fan) {
      Forest result;
      result.add_nodes(tail + fan);
      for (size_t m = 1; m < tail; ++m) {
        result.set_parent_and_label(m, m - 1, m % 2);
      }
      for (size_t m = tail; m < tail + fan; ++m) {
        result.set_parent_and_label(m, tail - 1, m % 2);
      }
      return result;
    }
  }  // namespace

  TEST_CASE("Forest - PathsFromRoots", "[quick][000]") {
    auto                   f = forest_example(1'000, 1'000);
    std::vector<word_type> expected(f.number_of_nodes());
    for (size_t n = 0; n != f.number_of_nodes(); ++n) {
      expected[n] = word_type(f.cbegin_path_to_root(n), f.cend_path_to_root(n));
      std::reverse(expected[n].begin(), expected[n].end());
    }

    BENCHMARK("fan with tail of length 1'000 + fan 1'000") {
      forest::PathsFromRoots paths(f);

      while (!paths.at_end()) {
        auto const& path = paths.get();
        auto const& n    = paths.target();
        REQUIRE(std::equal(
            path.begin(), path.end(), expected[n].begin(), expected[n].end()));
        paths.next();
      }
    };
  }

  TEST_CASE("Forest - PathsToRoots", "[quick][004]") {
    auto                   f = forest_example(1'000, 1'000);
    std::vector<word_type> expected(f.number_of_nodes());
    for (size_t n = 0; n != f.number_of_nodes(); ++n) {
      expected[n] = word_type(f.cbegin_path_to_root(n), f.cend_path_to_root(n));
    }

    BENCHMARK("fan with tail of length 1'000 + fan 1'000") {
      forest::PathsToRoots paths(f);

      while (!paths.at_end()) {
        auto const& path = paths.get();
        auto const& n    = paths.target();
        REQUIRE(std::equal(
            path.begin(), path.end(), expected[n].begin(), expected[n].end()));
        paths.next();
      }
    };
  }

  TEST_CASE("Forest - cbegin_path_to_root", "[quick][001]") {
    auto                   f = forest_example(1'000, 1'000);
    std::vector<word_type> expected(f.number_of_nodes());
    for (size_t n = 0; n != f.number_of_nodes(); ++n) {
      expected[n] = word_type(f.cbegin_path_to_root(n), f.cend_path_to_root(n));
    }
    word_type path;
    BENCHMARK("fan with tail of length 1'000 + fan 1'000") {
      for (size_t n = 0; n != f.number_of_nodes(); ++n) {
        forest::path_to_root(f, path, n);
        REQUIRE(path == expected[n]);
        path.clear();
      }
    };
  }

  TEST_CASE("Forest - ToddCoxeter example", "[quick][002]") {
    auto                      rg = ReportGuard(false);
    using literals::          operator""_p;
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "y^4"_p, "");
    presentation::add_rule(p, "(xy)^11"_p, "");
    presentation::add_rule(p, "(xy^2)^5"_p, "");
    presentation::add_rule(p, "(XYxy)^6"_p, "");
    presentation::add_rule(p, "((XY)^2(xy)^2)^3"_p, "");
    presentation::add_rule(p, "((xy)^2xY)^5"_p, "");

    ToddCoxeter tc(congruence_kind::twosided, p);
    tc.run();

    for (auto order : {Order::shortlex, Order::recursive, Order::lex}) {
      tc.standardize(order);
      forest::PathsFromRoots paths(tc.current_spanning_tree());
      REQUIRE(paths.count() == 443'520);
      size_t N;
      if (order == Order::lex) {
        N = 5'000;
      } else {
        N = paths.count();
      }
      size_t count = N;

      std::vector expected = (paths | rx::take(count) | rx::to_vector());

      BENCHMARK(
          fmt::format("PathsFromRoots - {} - {} paths", order, N).c_str()) {
        count = N;

        forest::PathsFromRoots paths(tc.current_spanning_tree());

        while (!paths.at_end()) {
          auto const& path = paths.get();
          auto const& n    = paths.target();
          REQUIRE(std::pair(n, path) == std::pair(n, expected[n]));
          paths.next();
          count--;
          if (count == 0) {
            break;
          }
        }
      };

      word_type path;
      BENCHMARK(
          fmt::format("path_from_root_no_checks - {} - {} paths", order, N)
              .c_str()) {
        auto const& f = tc.current_spanning_tree();
        for (size_t n = 0; n != N; ++n) {
          forest::path_from_root_no_checks(f, path, n);
          REQUIRE(std::pair(n, path) == std::pair(n, expected[n]));
          path.clear();
        }
      };
    }
  }

  TEST_CASE("Forest - ToddCoxeter example", "[quick][003]") {
    auto                      rg = ReportGuard(false);
    using literals::          operator""_p;
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "y^4"_p, "");
    presentation::add_rule(p, "(xy)^11"_p, "");
    presentation::add_rule(p, "(xy^2)^5"_p, "");
    presentation::add_rule(p, "(XYxy)^6"_p, "");
    presentation::add_rule(p, "((XY)^2(xy)^2)^3"_p, "");
    presentation::add_rule(p, "((xy)^2xY)^5"_p, "");

    ToddCoxeter tc(congruence_kind::twosided, p);
    tc.run();

    for (auto order : {Order::shortlex, Order::recursive, Order::lex}) {
      tc.standardize(order);
      forest::PathsToRoots paths(tc.current_spanning_tree());
      REQUIRE(paths.count() == 443'520);
      size_t N;
      if (order == Order::lex) {
        N = 5'000;
      } else {
        N = paths.count();
      }
      size_t count = N;

      std::vector expected = (paths | rx::take(count) | rx::to_vector());

      BENCHMARK(fmt::format("PathsToRoots - {} - {} paths", order, N).c_str()) {
        count = N;

        forest::PathsToRoots paths(tc.current_spanning_tree());

        while (!paths.at_end()) {
          auto const& path = paths.get();
          auto const& n    = paths.target();
          REQUIRE(std::pair(n, path) == std::pair(n, expected[n]));
          paths.next();
          count--;
          if (count == 0) {
            break;
          }
        }
      };

      word_type path;
      BENCHMARK(fmt::format("path_to_root_no_checks - {} - {} paths", order, N)
                    .c_str()) {
        auto const& f = tc.current_spanning_tree();
        for (size_t n = 0; n != N; ++n) {
          forest::path_to_root_no_checks(f, path, n);
          REQUIRE(std::pair(n, path) == std::pair(n, expected[n]));
          path.clear();
        }
      };
    }
  }
}  // namespace libsemigroups
