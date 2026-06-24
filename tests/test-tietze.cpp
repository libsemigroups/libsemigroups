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

  LIBSEMIGROUPS_TEST_CASE("Subwords", "000", "strings", "[quick]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");

    Subwords subwords(p);

    REQUIRE(subwords.size_hint() == 21);

    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<std::string>(
                {"", "a", "ab", "aba", "abab", "b", "ba", "bab"}));
    REQUIRE((subwords | rx::count()) == 8);
    subwords.min_length(3);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<std::string>({"aba", "abab", "bab"}));
    subwords.min_length(4);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<std::string>({"abab"}));
    subwords.min_length(5);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<std::string>({}));
    subwords.min_length(2).max_length(3);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<std::string>({"ab", "aba", "ba", "bab"}));
    subwords.min_length(2).max_length(1);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<std::string>({}));
  }

  LIBSEMIGROUPS_TEST_CASE("Subwords", "001", "word_type", "[quick]") {
    using literals::        operator""_w;
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2).contains_empty_word(true);
    presentation::add_rule(p, 010100101_w, ""_w);

    Subwords subwords(p);
    subwords.min_length(3);

    REQUIRE((subwords

             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::count())
            == 24);
    REQUIRE(subwords.size_hint() == 45);
    subwords.min_length(1);
    REQUIRE((subwords

             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
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
    REQUIRE(!subwords.at_end());
    subwords.min_length(5);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<word_type>({{0, 1, 0, 1, 0},
                                       {0, 1, 0, 1, 0, 0},
                                       {0, 1, 0, 1, 0, 0, 1},
                                       {0, 1, 0, 1, 0, 0, 1, 0},
                                       {0, 1, 0, 1, 0, 0, 1, 0, 1},
                                       {1, 0, 1, 0, 0},
                                       {1, 0, 1, 0, 0, 1},
                                       {1, 0, 1, 0, 0, 1, 0},
                                       {1, 0, 1, 0, 0, 1, 0, 1},
                                       {0, 1, 0, 0, 1},
                                       {0, 1, 0, 0, 1, 0},
                                       {0, 1, 0, 0, 1, 0, 1},
                                       {1, 0, 0, 1, 0},
                                       {1, 0, 0, 1, 0, 1},
                                       {0, 0, 1, 0, 1}}));
    subwords.min_length(5).max_length(5);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<word_type>({{0, 1, 0, 1, 0},
                                       {1, 0, 1, 0, 0},
                                       {0, 1, 0, 0, 1},
                                       {1, 0, 0, 1, 0},
                                       {0, 0, 1, 0, 1}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Subwords", "002", "operator|", "[quick]") {
    auto rg = ReportGuard(false);

    std::vector<Presentation<std::string>> presents;
    Presentation<std::string>              p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");
    presents.push_back(p);
    p.alphabet("xy");
    p.rules = {"xyxyxyxyxyxy", ""};
    presents.push_back(p);

    auto present_range = rx::iterator_range(presents.begin(), presents.end());

    // TODO remove <std::string>
    auto range = present_range | Subwords<std::string>();
    REQUIRE((range
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<std::string>(
                {"",           "a",           "ab",
                 "aba",        "abab",        "b",
                 "ba",         "bab",         "",
                 "x",          "xy",          "xyx",
                 "xyxy",       "xyxyx",       "xyxyxy",
                 "xyxyxyx",    "xyxyxyxy",    "xyxyxyxyx",
                 "xyxyxyxyxy", "xyxyxyxyxyx", "xyxyxyxyxyxy",
                 "y",          "yx",          "yxy",
                 "yxyx",       "yxyxy",       "yxyxyx",
                 "yxyxyxy",    "yxyxyxyx",    "yxyxyxyxy",
                 "yxyxyxyxyx", "yxyxyxyxyxy"}));
  }

  LIBSEMIGROUPS_TEST_CASE("TietzeAddGenerators", "003", "strings", "[quick]") {
    using rx::operator|;

    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");
    Subwords subwords(p);
    subwords.min_length(1);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<std::string>(
                {"a", "ab", "aba", "abab", "b", "ba", "bab"}));

    REQUIRE(
        (subwords | TietzeAddGenerators(p)
         | rx::transform([](auto& p) { return p.rules; }) | rx::to_vector())
        == std::vector<std::vector<std::string>>({{"cbcb", "bc", "c", "a"},
                                                  {"cc", "ba", "c", "ab"},
                                                  {"cb", "ba", "c", "aba"},
                                                  {"c", "ba", "c", "abab"},
                                                  {"acac", "ca", "c", "b"},
                                                  {"acb", "c", "c", "ba"},
                                                  {"ac", "ba", "c", "bab"}}));
    REQUIRE((subwords | TietzeAddGenerators(p)
             | Subwords<std::string>().min_length(1)
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<std::string>(
                {"",    "c",    "cb",  "cbc", "cbcb", "b",   "bc", "bcb", "a",
                 "",    "c",    "cc",  "b",   "ba",   "a",   "ab", "",    "c",
                 "cb",  "b",    "ba",  "a",   "ab",   "aba", "",   "c",   "b",
                 "ba",  "a",    "ab",  "aba", "abab", "bab", "",   "a",   "ac",
                 "aca", "acac", "c",   "ca",  "cac",  "b",   "",   "a",   "ac",
                 "acb", "c",    "cb",  "b",   "ba",   "",    "a",  "ac",  "c",
                 "b",   "ba",   "bab", "ab"}));
  }

}  // namespace libsemigroups
