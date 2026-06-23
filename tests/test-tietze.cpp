//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#include <string>  // for string
#include <vector>  // for vector

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/presentation.hpp"  // for Presentation
#include "libsemigroups/ranges.hpp"        // for iterator_range, to_vector
#include "libsemigroups/tietze.hpp"        // for TietzeAddGeneratorsRange
#include "libsemigroups/word-range.hpp"    // for operator""_w

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("RulesSubwords",
                          "000",
                          "strings",
                          "[quick][presentation][tietze]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");

    Subwords subwords(p);

    REQUIRE(subwords.size_hint() == 21);

    REQUIRE((subwords | rx::to_vector())
            == std::vector<std::string>(
                {"a", "ab", "aba", "abab", "b", "ba", "bab"}));
    REQUIRE((subwords | rx::count()) == 7);
    subwords.min_length(3);
    REQUIRE((subwords | rx::to_vector())
            == std::vector<std::string>({"aba", "abab", "bab"}));
    subwords.min_length(4);
    REQUIRE((subwords | rx::to_vector()) == std::vector<std::string>({"abab"}));
    subwords.min_length(5);
    REQUIRE((subwords | rx::to_vector()) == std::vector<std::string>({}));
  }

  LIBSEMIGROUPS_TEST_CASE("RulesSubwords",
                          "001",
                          "word_type",
                          "[quick][presentation][tietze]") {
    using literals::        operator""_w;
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2).contains_empty_word(true);
    presentation::add_rule(p, 010100101_w, ""_w);

    Subwords subwords(p);
    subwords.min_length(3);

    REQUIRE((subwords | rx::to_vector())
            == std::vector<word_type>({{0},
                                       {0, 1},
                                       {0, 1, 0},
                                       {0, 1, 0, 1},
                                       {0, 1, 0, 1, 0},
                                       {0, 1, 0, 1, 0, 0},
                                       {0, 1, 0, 1, 0, 0, 1},
                                       {0, 1, 0, 1, 0, 0, 1, 0},
                                       {0, 1, 0, 1, 0, 0, 1, 0, 1},
                                       {1},
                                       {1, 0},
                                       {1, 0, 1},
                                       {1, 0, 1, 0},
                                       {1, 0, 1, 0, 0},
                                       {1, 0, 1, 0, 0, 1},
                                       {1, 0, 1, 0, 0, 1, 0},
                                       {1, 0, 1, 0, 0, 1, 0, 1},
                                       {0, 1, 0, 0},
                                       {0, 1, 0, 0, 1},
                                       {0, 1, 0, 0, 1, 0},
                                       {0, 1, 0, 0, 1, 0, 1},
                                       {1, 0, 0},
                                       {1, 0, 0, 1},
                                       {1, 0, 0, 1, 0},
                                       {1, 0, 0, 1, 0, 1},
                                       {0, 0},
                                       {0, 0, 1},
                                       {0, 0, 1, 0},
                                       {0, 0, 1, 0, 1}}));
    REQUIRE((subwords | rx::count()) == 29);
    REQUIRE(subwords.size_hint() == 45);
  }

  // LIBSEMIGROUPS_TEST_CASE("TietzeAddGeneratorsRange",
  //                         "002",
  //                         "strings",
  //                         "[quick][presentation][tietze]") {
  //   auto                      rg = ReportGuard(false);
  //   Presentation<std::string> p;
  //   p.alphabet("ab");
  //   presentation::add_rule(p, "abab", "ba");

  //   TietzeAddGeneratorsRange tagr(p);

  //   tagr.depth_min(1).depth_max(1);

  //   REQUIRE((tagr | rx::transform([](auto& p) { return p.rules; })
  //            | rx::to_vector())
  //           == std::vector<std::vector<std::string>>());
  //   REQUIRE((tagr | rx::count()) == 5);
  //   REQUIRE(tagr.size_hint() == 0);
  //   REQUIRE(tagr.count() == 6);
  // }

}  // namespace libsemigroups
