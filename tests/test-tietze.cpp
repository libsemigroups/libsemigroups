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

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <chrono>
#include <string>  // for string
#include <vector>  // for vector

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix
#include "libsemigroups/presentation.hpp"  // for Presentation
#include "libsemigroups/ranges.hpp"        // for iterator_range, to_vector
#include "libsemigroups/tietze.hpp"        // for TietzeAddGeneratorsRange
#include "libsemigroups/word-range.hpp"    // for operator""_w

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("SubwordsOf", "000", "strings", "[quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("SubwordsOf", "001", "word_type", "[quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("SubwordsOf", "002", "operator|", "[quick]") {
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

    auto range = present_range | SubwordsOf();
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
        (subwords | TietzeAddGenerators()
         | rx::transform([](auto& p) { return p.rules; }) | rx::to_vector())
        == std::vector<std::vector<std::string>>({{"cbcb", "bc", "c", "a"},
                                                  {"cc", "ba", "c", "ab"},
                                                  {"cb", "ba", "c", "aba"},
                                                  {"c", "ba", "c", "abab"},
                                                  {"acac", "ca", "c", "b"},
                                                  {"acb", "c", "c", "ba"},
                                                  {"ac", "ba", "c", "bab"}}));

    auto tmp = (subwords | TietzeAddGenerators() | SubwordsOf().min_length(2));

    REQUIRE((tmp
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::to_vector())
            == std::vector<std::string>(
                {"cb",  "cbc", "cbcb", "bc",   "bcb", "cc",  "ba",  "ab",
                 "cb",  "ba",  "ab",   "aba",  "ba",  "ab",  "aba", "abab",
                 "bab", "ac",  "aca",  "acac", "ca",  "cac", "ac",  "acb",
                 "cb",  "ba",  "ac",   "ba",   "bab", "ab"}));

    REQUIRE((subwords | TietzeAddGenerators() | SubwordsOf().min_length(2)
             | TietzeAddGenerators()
             | rx::transform([](auto& p) { return p.rules; }) | rx::to_vector()
             | rx::to_vector())
            == std::vector<std::vector<std::string>>(
                {{"dd", "bc", "c", "a", "d", "cb"},
                 {"db", "bc", "c", "a", "d", "cbc"},
                 {"d", "bc", "c", "a", "d", "cbcb"},
                 {"cdb", "d", "c", "a", "d", "bc"},
                 {"cd", "bc", "c", "a", "d", "bcb"},
                 {"d", "ba", "c", "ab", "d", "cc"},
                 {"cc", "d", "c", "ab", "d", "ba"},
                 {"cc", "ba", "c", "d", "d", "ab"},
                 {"d", "ba", "c", "aba", "d", "cb"},
                 {"cb", "d", "c", "ad", "d", "ba"},
                 {"cb", "ba", "c", "da", "d", "ab"},
                 {"cb", "ba", "c", "d", "d", "aba"},
                 {"c", "d", "c", "adb", "d", "ba"},
                 {"c", "ba", "c", "dd", "d", "ab"},
                 {"c", "ba", "c", "db", "d", "aba"},
                 {"c", "ba", "c", "d", "d", "abab"},
                 {"c", "ba", "c", "ad", "d", "bab"},
                 {"dd", "ca", "c", "b", "d", "ac"},
                 {"dc", "ca", "c", "b", "d", "aca"},
                 {"d", "ca", "c", "b", "d", "acac"},
                 {"adc", "d", "c", "b", "d", "ca"},
                 {"ad", "ca", "c", "b", "d", "cac"},
                 {"db", "c", "c", "ba", "d", "ac"},
                 {"d", "c", "c", "ba", "d", "acb"},
                 {"ad", "c", "c", "ba", "d", "cb"},
                 {"acb", "c", "c", "d", "d", "ba"},
                 {"d", "ba", "c", "bab", "d", "ac"},
                 {"ac", "d", "c", "db", "d", "ba"},
                 {"ac", "ba", "c", "d", "d", "bab"},
                 {"ac", "ba", "c", "bd", "d", "ab"}}));

    StringRange strings;
    strings.alphabet("ab").min(2).max(4);
    REQUIRE(
        (strings | rx::transform([&p](auto& w) { return std::pair(p, w); })
         | TietzeAddGenerators()
         | rx::transform([](auto& p) { return p.rules; }) | rx::to_vector()
         | rx::to_vector())
        == std::vector<std::vector<std::string>>({{"abab", "ba", "c", "aa"},
                                                  {"cc", "ba", "c", "ab"},
                                                  {"acb", "c", "c", "ba"},
                                                  {"abab", "ba", "c", "bb"},
                                                  {"abab", "ba", "c", "aaa"},
                                                  {"abab", "ba", "c", "aab"},
                                                  {"cb", "ba", "c", "aba"},
                                                  {"abab", "ba", "c", "abb"},
                                                  {"abab", "ba", "c", "baa"},
                                                  {"ac", "ba", "c", "bab"},
                                                  {"abab", "ba", "c", "bba"},
                                                  {"abab", "ba", "c", "bbb"}}));
  }

  LIBSEMIGROUPS_TEST_CASE("FindIf", "004", "first test", "[quick]") {
    using rx::operator|;

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");

    KnuthBendix kb(congruence_kind::twosided, p);

    REQUIRE(!(Subwords(p).min_length(1) | TietzeAddGenerators()
              | FindIf([kb](auto const& p) mutable {
                  kb.init(congruence_kind::twosided, p);
                  kb.run_for(std::chrono::milliseconds(5));
                  return kb.finished();
                }))
                 .result()
                 .has_value());
  }

  LIBSEMIGROUPS_TEST_CASE("FindIf", "005", "second test", "[quick]") {
    using rx::operator|;

    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "abbab", "baabb");

    KnuthBendix kb(congruence_kind::twosided, p);

    auto result = (Subwords(p).min_length(1) | TietzeAddGenerators()
                   | SubwordsOf().min_length(1) | TietzeAddGenerators()
                   | AllAlphabetOrders() | FindIf([kb](auto const& p) mutable {
                       kb.init(congruence_kind::twosided, p);
                       kb.run_for(std::chrono::milliseconds(5));
                       return kb.finished();
                     }))
                      .result();
    REQUIRE(result.has_value());
    REQUIRE(result.value().alphabet() == "dbca");
    REQUIRE(
        result.value().rules
        == std::vector<std::string>({"cb", "db", "c", "abba", "d", "baab"}));

    kb.init(congruence_kind::twosided, result.value());
    kb.run();
    REQUIRE(kb.confluent());

    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((kb.active_rules() | rx::to_vector())
            == std::vector<rule_type>({{"abba", "c"},
                                       {"cb", "db"},
                                       {"baab", "d"},
                                       {"abbc", "dbba"},
                                       {"abd", "cab"},
                                       {"baad", "daab"},
                                       {"bac", "dba"},
                                       {"cd", "dd"},
                                       {"abbdb", "dbbab"},
                                       {"abbdd", "dbbad"},
                                       {"badb", "dbab"},
                                       {"badd", "dbad"}}));
  }

  LIBSEMIGROUPS_TEST_CASE("AllAlphabetOrders", "006", "strings", "[quick]") {
    using rx::operator|;
    auto      rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc");
    std::vector ps = {p};
    p.alphabet("xy");
    ps.push_back(p);

    auto range = rx::iterator_range(ps.begin(), ps.end());

    REQUIRE((range | AllAlphabetOrders()
             | rx::transform([](auto& p) -> auto& { return p.alphabet(); })
             | rx::to_vector())
            == std::vector<std::string>(
                {"abc", "acb", "bac", "bca", "cab", "cba", "xy", "yx"}));
  }
}  // namespace libsemigroups
