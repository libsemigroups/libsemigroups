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

#include "libsemigroups/constants.hpp"
#include "libsemigroups/detail/rewriting-system.hpp"
#include "libsemigroups/detail/rules.hpp"
#include "libsemigroups/types.hpp"
#include "rx/ranges.hpp"
#include <limits>
#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <chrono>
#include <string>  // for string
#include <vector>  // for vector

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/du-narendran-rusinowitch.hpp"  // for du_narendran_rusinowitch
#include "libsemigroups/knuth-bendix.hpp"              // for KnuthBendix
#include "libsemigroups/presentation.hpp"              // for Presentation
#include "libsemigroups/ranges.hpp"           // for iterator_range, to_vector
#include "libsemigroups/tietze.hpp"           // for TietzeAddGeneratorsRange
#include "libsemigroups/to-presentation.hpp"  // for to
#include "libsemigroups/word-range.hpp"       // for operator""_w

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {
  namespace {

    template <typename Iterator>
    auto most_frequent_subword(Iterator first, Iterator last) {
      using Word = std::conditional_t<
          std::is_same_v<std::remove_const_t<typename Iterator::value_type>,
                         word_type>,
          word_type,
          std::string>;

      std::unordered_map<Word, size_t> mp;
      Word                             tmp;

      for (auto it = first; it < last; ++it) {
        auto const& w = *it;
        for (auto suffix = w.cbegin(); suffix < w.cend(); ++suffix) {
          for (auto prefix = suffix + 2; prefix < w.cend(); ++prefix) {
            tmp.assign(suffix, prefix);
            auto [it, inserted] = mp.emplace(tmp, 1);
            if (!inserted) {
              ++(*it).second;
            }
          }
        }
      }

      return std::max_element(mp.begin(),
                              mp.end(),
                              [](auto const& x, auto const& y) {
                                return x.second < y.second;
                              })
          ->first;
    }
  }  // namespace

  using RPOTrie = detail::RewritingSystemTrie<RevRPOCmp>;

  LIBSEMIGROUPS_TEST_CASE("Subwords", "000", "strings", "[quick]") {
    using rx::operator|;
    auto      rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");

    auto subwords = (p | Subwords());

    REQUIRE(subwords.size_hint() == std::numeric_limits<size_t>::max());

    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(8) | rx::to_vector())
            == std::vector<std::string>(
                {"", "a", "ab", "aba", "abab", "b", "ba", "bab"}));
    REQUIRE((subwords | rx::count()) == 8);
    subwords.min_length(3);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(8) | rx::to_vector())
            == std::vector<std::string>({"aba", "abab", "bab"}));
    subwords.min_length(4);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(8) | rx::to_vector())
            == std::vector<std::string>({"abab"}));
    subwords.min_length(5);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(8) | rx::to_vector())
            == std::vector<std::string>({}));
    subwords.min_length(2).max_length(3);
    REQUIRE(!subwords.at_end());
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(8) | rx::to_vector())
            == std::vector<std::string>({"ab", "aba", "ba", "bab"}));
    subwords.min_length(2).max_length(1);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(8) | rx::to_vector())
            == std::vector<std::string>({}));
    // Check for proper subwords only
    subwords.min_length(0).max_length(POSITIVE_INFINITY).proper(true);
    REQUIRE(
        (subwords
         | rx::transform([](auto& pair) -> auto& { return pair.second; })
         | rx::take(8) | rx::to_vector())
        == std::vector<std::string>({"", "a", "ab", "aba", "b", "ba", "bab"}));
  }

  LIBSEMIGROUPS_TEST_CASE("SubwordsFreq", "001", "strings", "[quick]") {
    using rx::operator|;
    auto      rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");
    auto subwords = (p | SubwordsFreq());

    REQUIRE(subwords.size_hint() == std::numeric_limits<size_t>::max());

    REQUIRE((subwords | rx::transform([](auto& tup) {
               return std::pair(std::get<1>(tup), std::get<2>(tup));
             })
             | rx::take(8) | rx::to_vector())
            == std::vector<std::pair<std::string, size_t>>({{"", 6},
                                                            {"a", 3},
                                                            {"ab", 2},
                                                            {"aba", 1},
                                                            {"abab", 1},
                                                            {"b", 3},
                                                            {"ba", 2},
                                                            {"bab", 1}}));

    subwords.min_length(2).max_length(3);
    REQUIRE(subwords.min_length() == 2);
    REQUIRE(subwords.max_length() == 3);
    REQUIRE((subwords | rx::transform([](auto& tup) {
               return std::pair(std::get<1>(tup), std::get<2>(tup));
             })
             | rx::take(8) | rx::to_vector())
            == std::vector<std::pair<std::string, size_t>>(
                {{"ab", 2}, {"aba", 1}, {"ba", 2}, {"bab", 1}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Subwords", "002", "word_type", "[quick]") {
    using literals::operator""_w;
    using rx::      operator|;

    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2).contains_empty_word(true);
    presentation::add_rule(p, 010100101_w, ""_w);

    auto subwords = (p | Subwords());
    subwords.min_length(3);

    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::count())
            == 24);
    REQUIRE(subwords.size_hint() == std::numeric_limits<size_t>::max());
    subwords.min_length(1);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(100) | rx::to_vector())
            == std::vector(
                {0_w,       01_w,       010_w,      0101_w,      01010_w,
                 010100_w,  0101001_w,  01010010_w, 010100101_w, 1_w,
                 10_w,      101_w,      1010_w,     10100_w,     101001_w,
                 1010010_w, 10100101_w, 0100_w,     01001_w,     010010_w,
                 0100101_w, 100_w,      1001_w,     10010_w,     100101_w,
                 00_w,      001_w,      0010_w,     00101_w}));
    REQUIRE(!subwords.at_end());
    subwords.min_length(5);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(100) | rx::to_vector())
            == std::vector({01010_w,
                            010100_w,
                            0101001_w,
                            01010010_w,
                            010100101_w,
                            10100_w,
                            101001_w,
                            1010010_w,
                            10100101_w,
                            01001_w,
                            010010_w,
                            0100101_w,
                            10010_w,
                            100101_w,
                            00101_w}));
    subwords.min_length(5).max_length(5);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(100) | rx::to_vector())
            == std::vector({01010_w, 10100_w, 01001_w, 10010_w, 00101_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("Subwords", "003", "operator|", "[quick]") {
    auto rg = ReportGuard(false);

    std::vector<Presentation<std::string>> ps;

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");
    ps.push_back(p);

    p.alphabet("xy");
    p.rules = {"xyxyxyxyxyxy", ""};
    ps.push_back(p);

    REQUIRE((rx::iterator_range(ps) | Subwords()
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(100) | rx::to_vector())
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

  LIBSEMIGROUPS_TEST_CASE("TietzeAddGenerators", "004", "strings", "[quick]") {
    using rx::operator|;

    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");
    auto subwords = (p | Subwords());
    subwords.min_length(1);
    REQUIRE((subwords
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(100) | rx::to_vector())
            == std::vector<std::string>(
                {"a", "ab", "aba", "abab", "b", "ba", "bab"}));

    REQUIRE(
        (subwords | TietzeAddGenerators()
         | rx::transform([](auto& p) { return p.rules; }) | rx::take(100)
         | rx::to_vector())
        == std::vector<std::vector<std::string>>({{"cbcb", "bc", "c", "a"},
                                                  {"cc", "ba", "c", "ab"},
                                                  {"cb", "ba", "c", "aba"},
                                                  {"c", "ba", "c", "abab"},
                                                  {"acac", "ca", "c", "b"},
                                                  {"acb", "c", "c", "ba"},
                                                  {"ac", "ba", "c", "bab"}}));

    auto tmp = (subwords | TietzeAddGenerators() | Subwords().min_length(2));

    REQUIRE((tmp
             | rx::transform([](auto& pair) -> auto& { return pair.second; })
             | rx::take(100) | rx::to_vector())
            == std::vector<std::string>(
                {"cb",  "cbc", "cbcb", "bc",   "bcb", "cc",  "ba",  "ab",
                 "cb",  "ba",  "ab",   "aba",  "ba",  "ab",  "aba", "abab",
                 "bab", "ac",  "aca",  "acac", "ca",  "cac", "ac",  "acb",
                 "cb",  "ba",  "ac",   "ba",   "bab", "ab"}));

    REQUIRE((subwords | TietzeAddGenerators() | Subwords().min_length(2)
             | TietzeAddGenerators()
             | rx::transform([](auto& p) { return p.rules; }) | rx::take(100)
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
         | rx::transform([](auto& p) { return p.rules; }) | rx::take(100)
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

  LIBSEMIGROUPS_TEST_CASE("FindIf", "005", "first test", "[quick]") {
    using rx::operator|;

    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");

    KnuthBendix kb(congruence_kind::twosided, p);

    REQUIRE(!(p | Subwords().min_length(1) | TietzeAddGenerators()
              | FindIf([kb](auto const& p) mutable {
                  kb.init(congruence_kind::twosided, p);
                  kb.run_for(std::chrono::milliseconds(5));
                  return kb.finished();
                }))
                 .get()
                 .has_value());
  }

  LIBSEMIGROUPS_TEST_CASE("FindIf", "006", "abbab=baabb", "[extreme]") {
    using rx::operator|;

    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "abbab", "baabb");

    KnuthBendix kb(congruence_kind::twosided, p);

    auto result
        = (p | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | AllAlphabetOrders() | FindIf([kb](auto const& p) mutable {
                                     static std::atomic_size_t count = 0;
                                     ReportGuard               rg(true);
                                     report_default("{}\n", count++);
                                     kb.init(congruence_kind::twosided, p);
                                     kb.run_for(std::chrono::milliseconds(5));
                                     return kb.finished();
                                   }).number_of_threads(1))
              .get();
    REQUIRE(result.has_value());
    REQUIRE(result.value().alphabet() == "dbca");
    REQUIRE(
        result.value().rules
        == std::vector<std::string>({"cb", "db", "c", "abba", "d", "baab"}));

    kb.init(congruence_kind::twosided, result.value());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());

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

  LIBSEMIGROUPS_TEST_CASE("AllAlphabetOrders", "007", "strings", "[quick]") {
    using rx::operator|;
    auto      rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc");
    std::vector ps = {p};
    p.alphabet("xy");
    ps.push_back(p);

    REQUIRE((rx::iterator_range(ps) | AllAlphabetOrders()
             | rx::transform([](auto& p) -> auto& { return p.alphabet(); })
             | rx::take(8) | rx::to_vector())
            == std::vector<std::string>(
                {"abc", "acb", "bac", "bca", "cab", "cba", "xy", "yx"}));

    REQUIRE((p | AllAlphabetOrders()
             | rx::transform([](auto& p) -> auto& { return p.alphabet(); })
             | rx::take(9) | rx::to_vector())
            == std::vector<std::string>({"xy", "yx"}));
  }

  LIBSEMIGROUPS_TEST_CASE("FindIf", "008", "a=cc, c=bab", "[quick]") {
    using rx::operator|;

    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("bca");
    presentation::add_rule(p, "a", "cc");
    presentation::add_rule(p, "c", "bab");

    KnuthBendix<std::string, RPOTrie> kb(congruence_kind::twosided, p);

    auto result
        = (p | AllAlphabetOrders() | FindIf([kb](auto const& p) mutable {
             kb.init(congruence_kind::twosided, p);
             kb.run_for(std::chrono::milliseconds(5));
             return kb.rewriting_system().confluent();
           })).get();
    REQUIRE(result.has_value());
    REQUIRE(result.value().alphabet() == "bca");
    REQUIRE(result.value().rules
            == std::vector<std::string>({"a", "cc", "c", "bab"}));

    kb.init(congruence_kind::twosided, result.value());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());

    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((kb.active_rules() | rx::to_vector())
            == std::vector<rule_type>(
                {{"a", "cc"}, {"bccb", "c"}, {"bccc", "cccb"}}));
  }

  // Takes about 10 hours
  LIBSEMIGROUPS_TEST_CASE("FindIf", "009", "aaa=1, aBBBABAb=1", "[extreme]") {
    // https://math.stackexchange.com/questions/4942596

    using rx::                  operator|;
    using literals::            operator""_p;
    auto                        rg = ReportGuard(true);
    using std::string_literals::operator""s;

    Presentation<std::string> p;
    p.alphabet("abB");
    p.contains_empty_word(true);
    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "b^9"_p, "");

    KnuthBendix kb(congruence_kind::twosided, p);

    auto nf = knuth_bendix::normal_forms(kb);

    presentation::add_rule(p, "abaabbbaB", "");
    ToddCoxeter tc(congruence_kind::twosided, p);
    todd_coxeter::add_generating_pair(tc, "(ab)^6"_p, "");

    REQUIRE(tc.number_of_classes() == 1512);

    REQUIRE(nf.min(14).max(22).count() == 130'401'452);

    auto result
        = (nf.min(14).max(22)
           | rx::transform([&p](auto& w) { return std::tuple(p, w, ""); })
           | TietzeAddRelation() | FindIf([tc](auto const& p) mutable {
               tc.init(congruence_kind::twosided, p);
               tc.run_for(std::chrono::milliseconds(10));
               return tc.finished() && tc.number_of_classes() > 1'512;
             }))
              .number_of_threads(12)
              .get();
    REQUIRE(result.has_value());
    REQUIRE(result.value().alphabet() == "abB");
    REQUIRE(result.value().rules
            == std::vector<std::string>({"aaa",
                                         "",
                                         "Bb",
                                         "",
                                         "bB",
                                         "",
                                         "bbbbbbbbb",
                                         "",
                                         "abaabbbaB",
                                         "",
                                         "aaBaaBabaBBababab",
                                         ""}));
    tc.init(congruence_kind::twosided, result.value());
    REQUIRE(tc.number_of_classes() == 4'536);
    // Takes about 10 hours
    result = (nf.min(14).max(22)
              | rx::transform([&p](auto& w) { return std::tuple(p, w, ""); })
              | TietzeAddRelation() | FindIf([tc](auto const& p) mutable {
                  tc.init(congruence_kind::twosided, p);
                  tc.run_for(std::chrono::milliseconds(10));
                  return tc.finished() && tc.number_of_classes() > 4'536;
                }))
                 .number_of_threads(12)
                 .get();
    REQUIRE(!result.has_value());
  }

  LIBSEMIGROUPS_TEST_CASE("FindIf", "010", "baabaa=aba", "[quick]") {
    using rx::                operator|;
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);

    presentation::add_rule(p, "baabaa", "aba");

    KnuthBendix<std::string, detail::RewritingSystemTrie<RPOCmp>> kb(
        congruence_kind::twosided, p);
    auto result
        = (p | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | AllAlphabetOrders() | FindIf([kb](auto const& p) mutable {
               kb.init(congruence_kind::twosided, p);
               kb.run_for(std::chrono::milliseconds(4));
               return kb.rewriting_system().confluent();
             }))
              .get();
    REQUIRE(result.has_value());
    REQUIRE(result.value().alphabet() == "bca");
    REQUIRE(result.value().rules
            == std::vector<std::string>({"cc", "aba", "c", "baa"}));
    kb.init(congruence_kind::twosided, result.value());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());

    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((kb.active_rules() | rx::to_vector())
            == std::vector<rule_type>({{"aba", "cc"},
                                       {"baa", "c"},
                                       {"ac", "cca"},
                                       {"abcc", "ccba"},
                                       {"bcccca", "cba"},
                                       {"bcccccc", "cbcc"}}));
  }

  LIBSEMIGROUPS_TEST_CASE("FindIf", "011", "baaabaaa=aba", "[fail]") {
    using rx::                operator|;
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);

    presentation::add_rule(p, "baaabaaa", "aba");

    KnuthBendix<std::string, detail::RewritingSystemTrie<RPOCmp>> kb(
        congruence_kind::twosided, p);
    auto input
        = (p | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | AllAlphabetOrders());

    size_t num = (input | rx::count());

    auto result = (input
                   | FindIf([kb](auto const& p) mutable {
                       kb.init(congruence_kind::twosided, p);
                       kb.run_for(std::chrono::milliseconds(4));
                       return kb.rewriting_system().confluent();
                     })
                         .total(num)
                         .number_of_threads(10))
                      .get();
    REQUIRE(!result.has_value());
  }

  // About 7s
  LIBSEMIGROUPS_TEST_CASE("FindIf", "012", "ababbaaaaa=baaaaaba", "[extreme]") {
    using rx::operator|;

    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);

    presentation::add_rule(p, "ababbaaaaa", "baaaaaba");

    KnuthBendix<std::string, detail::RewritingSystemTrie<RPOCmp>> kb_rpo(
        congruence_kind::twosided, p);
    // KnuthBendix<std::string, detail::RewritingSystemTrie<RevRPOCmp>>
    // kb_rev_rpo(
    //     congruence_kind::twosided, p);
    auto result
        = (p | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | AllAlphabetOrders()
           | FindIf([kb_rpo](auto const& p) mutable {
               kb_rpo.init(congruence_kind::twosided, p);
               kb_rpo.run_for(std::chrono::milliseconds(4));
               // if (kb_rpo.rewriting_system().confluent()) {
               //   return true;
               // }
               // kb_rev_rpo.init(congruence_kind::twosided, p);
               // kb_rev_rpo.run_for(std::chrono::milliseconds(4));
               return kb_rpo.rewriting_system().confluent();
             }).number_of_threads(10))
              .get();
    REQUIRE(result.has_value());
    REQUIRE(result.value().alphabet() == "dacb");
    REQUIRE(
        result.value().rules
        == std::vector<std::string>({"ac", "dba", "c", "babd", "d", "baaaaa"}));

    rg = ReportGuard(false);
    kb_rpo.init(congruence_kind::twosided, result.value());
    kb_rpo.run_for(std::chrono::milliseconds(4));
    REQUIRE(kb_rpo.rewriting_system().confluent());

    using rule_type = typename decltype(kb_rpo)::rule_type;
    REQUIRE((kb_rpo.active_rules() | rx::to_vector())
            == std::vector<rule_type>({{"babd", "c"},
                                       {"baaaaa", "d"},
                                       {"dba", "ac"},
                                       {"babac", "cba"},
                                       {"acbd", "dc"},
                                       {"acaaaa", "dd"},
                                       {"baaaadc", "dcbd"},
                                       {"baaaadd", "dcaaaa"},
                                       {"acbac", "dcba"},
                                       {"dbdc", "accbd"},
                                       {"dbdd", "accaaaa"},
                                       {"ddcbd", "acaaadc"},
                                       {"ddcaaaa", "acaaadd"},
                                       {"baaaadac", "dcaaaaba"},
                                       {"dbdac", "accaaaaba"},
                                       {"ddcbac", "acaaadcba"}}));
  }

  // Fails after ~8s
  LIBSEMIGROUPS_TEST_CASE("FindIf", "013", "baaababaaa=aaba", "[fail]") {
    using rx::operator|;
    auto      rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ba");
    p.contains_empty_word(true);
    presentation::add_rule(p, "baaababaaa", "aaba");

    KnuthBendix kb(congruence_kind::twosided, p);

    auto range
        = (p | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | AllAlphabetOrders());

    size_t num = (range | rx::count());

    auto result = (range
                   | FindIf([kb](auto const& p) mutable {
                       kb.init(congruence_kind::twosided, p);
                       kb.run_for(std::chrono::milliseconds(4));
                       return kb.rewriting_system().confluent();
                     })
                         .number_of_threads(10)
                         .total(num))
                      .get();
    REQUIRE(!result.has_value());
  }

  // Fails in about 9 minutes
  LIBSEMIGROUPS_TEST_CASE("FindIf", "014", "aabbaab=aba", "[fail]") {
    using rx::operator|;
    auto      rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ba");
    p.contains_empty_word(true);
    presentation::add_rule(p, "aabbaab", "aba");

    KnuthBendix<std::string, detail::RewritingSystemTrie<RPOCmp>> kb_rpo(
        congruence_kind::twosided, p);
    KnuthBendix<std::string, detail::RewritingSystemTrie<RevRPOCmp>> kb_rev_rpo(
        congruence_kind::twosided, p);
    auto result
        = (p | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | Subwords().min_length(1).proper(true) | TietzeAddGenerators()
           | AllAlphabetOrders()
           | FindIf([kb_rpo, kb_rev_rpo](auto const& p) mutable {
               kb_rpo.init(congruence_kind::twosided, p);
               kb_rpo.run_for(std::chrono::milliseconds(4));
               if (kb_rpo.rewriting_system().confluent()) {
                 return true;
               }
               kb_rev_rpo.init(congruence_kind::twosided, p);
               kb_rev_rpo.run_for(std::chrono::milliseconds(4));
               return kb_rev_rpo.rewriting_system().confluent();
             }).number_of_threads(10))
              .get();
    REQUIRE(!result.has_value());
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix", "015", "morpho completion", "[fail]") {
    using rx::operator|;

    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "aabbaab", "aba");

    REQUIRE(most_frequent_subword(p.rules.begin(), p.rules.end()) == "ab");

    KnuthBendix kb(congruence_kind::twosided, p);

    auto morpho_complete
        = rx::transform([&kb](Presentation<std::string> const& p) {
            kb.init(congruence_kind::twosided, p);
            // kb.rewriting_system().sort_pending_rules_by(nullptr);
            // kb.run_for(std::chrono::microseconds(10));
            kb.max_rounds(2).run();
            kb.rewriting_system().settings().reduction_threshold
                = POSITIVE_INFINITY;
            kb.rewriting_system().reduce();
            return to<Presentation>(kb);
          });

    // auto q = (Singleton(p) | morpho_complete).get();
    // REQUIRE(q.rules
    //         == std::vector<std::string>({"aabbaab",
    //                                      "aba",
    //                                      "ababaab",
    //                                      "aabbaba",
    //                                      "aabbaaabbaba",
    //                                      "abaabaab",
    //                                      "ababaaabbaba",
    //                                      "aabbabaabaab",
    //                                      "abaabaabbabaaabbaba",
    //                                      "abaabaababbabaabaab",
    //                                      "abaabaababbaab",
    //                                      "abaabaabba",
    //                                      "abaabaabbabaab",
    //                                      "abaabaababbaba",
    //                                      "abaabaababbaaabbaba",
    //                                      "abaababaaab"}));

    // REQUIRE(most_frequent_subword(q.rules.begin(), q.rules.end()) == "ab");

    auto find_if = FindIf([kb](auto const& p) mutable {
                     kb.init(congruence_kind::twosided, p);
                     kb.run_for(std::chrono::milliseconds(2));
                     return kb.rewriting_system().confluent();
                   }).number_of_threads(12);
    {
      auto input = (p | AllAlphabetOrders() | morpho_complete
                    | Subwords().min_length(2).proper(true)
                    | rx::transform([&p](auto const& pair) {
                        Presentation<std::string> result(p);
                        result.alphabet(pair.first.alphabet());
                        return std::pair(result, pair.second);
                      })
                    | TietzeAddGenerators() | AllAlphabetOrderExts());

      auto num = (input | rx::count());
      REQUIRE(num == 654);

      auto result = (input | find_if.total(num)).get();
      REQUIRE(!result.has_value());
    }
    {
      auto input
          = (p | AllAlphabetOrders() | morpho_complete
             | Subwords().min_length(2).max_length(6)
             | rx::transform([p](auto const& pair) {
                 auto copy(p);
                 presentation::replace_word_with_new_generator(copy,
                                                               pair.second);
                 return copy;
               })
             | morpho_complete | Subwords().min_length(2).max_length(6)
             | rx::transform([p](auto const& pair) {
                 auto copy(p);
                 presentation::replace_word_with_new_generator(copy,
                                                               pair.second);
                 return copy;
               })
             | morpho_complete | Subwords().min_length(2).max_length(6)
             | rx::transform([p](auto const& pair) {
                 auto copy(p);
                 presentation::replace_word_with_new_generator(copy,
                                                               pair.second);
                 return copy;
               })
             | AllAlphabetOrderExts());

      auto num = (input | rx::count());
      // REQUIRE(num == 970'983);

      // size_t count = 0;
      // while (!input.at_end()) {
      //   input.next();
      //   count++;
      // }
      // REQUIRE(count == 2'096);

      auto result = (input | find_if.total(num)).get();
      REQUIRE(!result.has_value());
    }
  }

  // Takes approx. 4.3s
  LIBSEMIGROUPS_TEST_CASE("Tietze",
                          "016",
                          "morpho completion baaabaaa=aba",
                          "[extreme]") {
    using rx::operator|;

    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "baaabaaa", "aba");

    KnuthBendix<std::string, detail::RewritingSystemTrie<RPOCmp>> kb(
        congruence_kind::twosided, p);

    auto morpho_complete
        = rx::transform([&kb](Presentation<std::string> const& p) {
            kb.init(congruence_kind::twosided, p);
            kb.rewriting_system().sort_pending_rules_by(nullptr);
            kb.max_rounds(2).run();
            kb.rewriting_system().settings().reduction_threshold
                = POSITIVE_INFINITY;
            return to<Presentation>(kb);
          });

    Presentation<std::string> p0 = p, p1, p2;

    auto input
        = (p0 | AllAlphabetOrders() | morpho_complete
           | Subwords().min_length(2).max_length(3)
           | rx::transform([&p0](auto const& pair) {
               auto copy(p0);
               presentation::replace_word_with_new_generator(copy, pair.second);
               return copy;
             })
           | AllAlphabetOrderExts() | Ref(p1) | morpho_complete
           | Subwords().min_length(2).max_length(3)
           | rx::transform([&p1](auto const& pair) {
               auto copy(p1);
               presentation::replace_word_with_new_generator(copy, pair.second);
               return copy;
             })
           | AllAlphabetOrderExts() | Ref(p2) | morpho_complete
           | Subwords().min_length(2).max_length(3)
           | rx::transform([&p2](auto const& pair) {
               auto copy(p2);
               presentation::replace_word_with_new_generator(copy, pair.second);
               // fmt::print("C: {}\n", copy.alphabet());
               // fmt::print("C: {}\n\n", copy.rules);
               return copy;
             })
           | AllAlphabetOrderExts());

    auto num = (input | rx::count());

    REQUIRE(num == 399'620);

    auto find_if = FindIf([kb](auto const& p) mutable {
                     kb.init(congruence_kind::twosided, p);
                     kb.run_for(std::chrono::milliseconds(4));
                     return kb.rewriting_system().confluent();
                   }).number_of_threads(12);

    auto result = (input | find_if.total(num)).get();
    REQUIRE(result.has_value());
    REQUIRE(result.value().alphabet() == "cedab");
    REQUIRE(result.value().rules
            == std::vector<std::string>(
                {"cdcd", "ac", "c", "ba", "d", "aa", "e", "cdd"}));

    kb.init(congruence_kind::twosided, result.value());
    kb.run();
    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((kb.active_rules() | rx::to_vector())
            == std::vector<rule_type>(
                {{"ba", "c"},           {"aa", "d"},
                 {"cdd", "e"},          {"bd", "ca"},
                 {"ad", "da"},          {"ae", "cded"},
                 {"cdecd", "dc"},       {"bec", "ccecd"},
                 {"bcde", "ccd"},       {"cdeed", "de"},
                 {"dcd", "cdee"},       {"cdedc", "dcecd"},
                 {"bccdee", "cc"},      {"ac", "ccdee"},
                 {"bede", "cceeeed"},   {"bedc", "cceeecd"},
                 {"bee", "cceed"},      {"dccdee", "cdeecd"},
                 {"dde", "cdeeeed"},    {"ddc", "cdeeecd"},
                 {"cdede", "dceed"},    {"cdeecdee", "decd"},
                 {"cdeccdee", "dccd"},  {"ccdeeee", "ecd"},
                 {"ccdeeecd", "ec"},    {"ccdeeede", "eceed"},
                 {"ccdeeedc", "ececd"}, {"ccdeeeccdee", "eccd"}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Tietze",
                          "017",
                          "morpho completion baaabaaa=aba x2",
                          "[extreme]") {
    using rx::operator|;

    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "baaabaaa", "aba");

    KnuthBendix<std::string, detail::RewritingSystemTrie<RPOCmp>> kb(
        congruence_kind::twosided, p);

    auto morpho_complete
        = rx::transform([&kb](Presentation<std::string> const& p) {
            kb.init(congruence_kind::twosided, p);
            kb.rewriting_system().sort_pending_rules_by(nullptr);
            kb.max_rounds(2).run();
            kb.rewriting_system().settings().reduction_threshold
                = POSITIVE_INFINITY;
            return to<Presentation>(kb);
          });

    Presentation<std::string> p0 = p, p1, p2;

    size_t const m = 4 * 2048;

    auto input
        = (p0 | AllAlphabetOrders() | morpho_complete
           | SubwordsFreq().min_length(2).max_length(3) | rx::take(10'000)
           | rx::sort([](auto const& tup1, auto const& tup2) {
               return std::get<1>(tup1).size() * std::get<2>(tup1)
                      > std::get<1>(tup2).size() * std::get<2>(tup2);
             })
           | rx::take(m) | rx::transform([&p0](auto const& tup) {
               auto copy(p0);
               presentation::replace_word_with_new_generator(copy,
                                                             std::get<1>(tup));
               return copy;
             })
           | AllAlphabetOrderExts() | Ref(p1) | morpho_complete
           | SubwordsFreq().min_length(2).max_length(3) | rx::take(10'000)
           | rx::sort([](auto const& tup1, auto const& tup2) {
               return std::get<1>(tup1).size() * std::get<2>(tup1)
                      > std::get<1>(tup2).size() * std::get<2>(tup2);
             })
           | rx::take(m) | rx::transform([&p1](auto const& tup) {
               auto copy(p1);
               presentation::replace_word_with_new_generator(copy,
                                                             std::get<1>(tup));
               return copy;
             })
           | AllAlphabetOrderExts() | Ref(p2) | morpho_complete
           | SubwordsFreq().min_length(2).max_length(3) | rx::take(10'000)
           | rx::sort([](auto const& tup1, auto const& tup2) {
               return std::get<1>(tup1).size() * std::get<2>(tup1)
                      > std::get<1>(tup2).size() * std::get<2>(tup2);
             })
           | rx::take(m) | rx::transform([&p2](auto const& tup) {
               auto copy(p2);
               presentation::replace_word_with_new_generator(copy,
                                                             std::get<1>(tup));
               fmt::print("C: {}\n", copy.alphabet());
               fmt::print("C: {}\n\n", copy.rules);
               return copy;
             })
           | AllAlphabetOrderExts());

    size_t const num = (input | rx::count());

    // REQUIRE(num == 160);

    auto find_if = FindIf([kb](auto const& p) mutable {
                     kb.init(congruence_kind::twosided, p);
                     kb.run_for(std::chrono::milliseconds(4));
                     return kb.rewriting_system().confluent();
                   }).number_of_threads(12);

    auto result = (input | find_if.total(num)).get();
    REQUIRE(result.has_value());
  }

}  // namespace libsemigroups
