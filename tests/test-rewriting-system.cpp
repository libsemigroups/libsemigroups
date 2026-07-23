// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2026 Joseph Edwards
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

#include <algorithm>    // for any_of
#include <functional>   // for operator!=
#include <list>         // for operator!=
#include <ostream>      // for basic_ostream
#include <string>       // for basic_string
#include <string_view>  // for literals
#include <type_traits>  // for is_same_v
#include <utility>      // for pair, forward
#include <vector>       // for vector, operat...

#include "Catch2-3.14.0/catch_amalgamated.hpp"  // for AssertionHandler, ope...
#include "test-main.hpp"                        // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/adapters.hpp"      // for ReturnFalse
#include "libsemigroups/aho-corasick.hpp"  // for dot
#include "libsemigroups/order.hpp"         // for LenLexCmp
#include "libsemigroups/ranges.hpp"        // for rx::
#include "libsemigroups/types.hpp"         // for tril, word_type
#include "libsemigroups/word-range.hpp"    // for operator""_w

#include "libsemigroups/detail/aho-corasick-impl.hpp"  // for AhoCorasickImpl
#include "libsemigroups/detail/fmt.hpp"                // for fmt
#include "libsemigroups/detail/report.hpp"             // for ReportGuard
#include "libsemigroups/detail/rewriting-system.hpp"  // for RewritingSystemTrie
#include "libsemigroups/detail/rules.hpp"             // for reorder, Rules
#include "libsemigroups/detail/value-guard.hpp"       // for ValueGuard

// The following might sometimes be useful for debugging, but generates compile
// warnings (we are not allowed to put overloads in namespace std except with
// user-defined types).

// namespace std {
//   std::ostream& operator<<(std::ostream& os, std::string const& value);
//
//   std::ostream& operator<<(std::ostream& os, std::string const& value) {
//     for (auto c : value) {
//       if (c < 10) {
//         os << int(c);
//       } else {
//         os << c;
//       }
//     }
//     return os;
//   }
// }  // namespace std

namespace libsemigroups {
  using literals::operator""_w;

  namespace detail {

    template <typename = Default, bool = false>
    using NoOrder = ReturnFalse;

    using string_type = RewritingSystemTrie<LenLexCmp>::native_word_type;

    using namespace std::literals;

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<LenLexCmp>",
                            "000",
                            "initial test",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rt;
      REQUIRE(rt.number_of_rules() == 0);
      rt.increase_alphabet_size_by(2);
      rewriting_system::add_rule(rt, "ba"_w, "a"_w);
      REQUIRE(rt.number_of_rules() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<LenLexCmp>",
                            "001",
                            "simple test",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rt;

      rt.increase_alphabet_size_by(3);
      rewriting_system::add_rule(rt, "ac"_w, "ca"_w);
      rewriting_system::add_rule(rt, "aa"_w, "a"_w);
      rewriting_system::add_rule(rt, "ac"_w, "a"_w);
      rewriting_system::add_rule(rt, "ca"_w, "a"_w);
      rewriting_system::add_rule(rt, "bb"_w, "bb"_w);
      rewriting_system::add_rule(rt, "bc"_w, "cb"_w);
      rewriting_system::add_rule(rt, "bbb"_w, "b"_w);
      rewriting_system::add_rule(rt, "bc"_w, "b"_w);
      rewriting_system::add_rule(rt, "cb"_w, "b"_w);
      rewriting_system::add_rule(rt, "a"_w, "b"_w);

      REQUIRE(rt.confluent());

      string_type w1 = {0, 0};
      rt.rewrite(w1);
      REQUIRE(w1 == string_type({0}));

      string_type w2 = {0, 1};
      rt.rewrite(w2);
      REQUIRE(w2 == string_type({0}));

      string_type w3 = {0, 1, 2};
      rt.rewrite(w3);
      REQUIRE(w3 == string_type({0}));

      string_type w4 = {0, 1, 2, 0};
      rt.rewrite(w4);
      REQUIRE(w4 == string_type({0}));

      string_type w5 = {2, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 1, 0, 2, 1, 0, 2, 1,
                        0, 2, 0, 1, 0, 2, 0, 1, 1, 0, 2, 2, 0, 1, 1, 0, 2, 0, 1,
                        1, 0, 2, 2, 0, 1, 0, 2, 0, 1, 1, 0, 2, 0, 1, 1, 0};
      rt.rewrite(w5);
      REQUIRE(w5 == string_type({0}));
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemSet<LenLexCmp>",
                            "002",
                            "simple test",
                            "[quick]") {
      using rule_type = std::pair<std::string, std::string>;

      auto rg = ReportGuard(false);

      RewritingSystemSet<LenLexCmp> rws;

      rws.increase_alphabet_size_by(3);
      rewriting_system::add_rule(rws, "ac"_w, "ca"_w);
      rewriting_system::add_rule(rws, "aa"_w, "a"_w);
      rewriting_system::add_rule(rws, "ac"_w, "a"_w);
      rewriting_system::add_rule(rws, "ca"_w, "a"_w);
      rewriting_system::add_rule(rws, "bb"_w, "bb"_w);
      rewriting_system::add_rule(rws, "bc"_w, "cb"_w);
      rewriting_system::add_rule(rws, "bbb"_w, "b"_w);
      rewriting_system::add_rule(rws, "bc"_w, "b"_w);
      rewriting_system::add_rule(rws, "cb"_w, "b"_w);
      rewriting_system::add_rule(rws, "a"_w, "b"_w);

      REQUIRE(rws.confluent());
      REQUIRE(rws.number_of_rules() == 4);
      REQUIRE((rws.rules()
               | rx::transform([](auto const& pair) { return rule_type(pair); })
               | rx::to_vector())
              == std::vector<std::pair<std::string, std::string>>(
                  {{{2, 0}, {0}}, {{0, 2}, {0}}, {{1}, {0}}, {{0, 0}, {0}}}));

      string_type w1 = {0, 0};
      rws.rewrite(w1);
      REQUIRE(w1 == string_type({0}));

      string_type w2 = {0, 1};
      rws.rewrite(w2);
      REQUIRE(w2 == string_type({0}));

      string_type w3 = {0, 1, 2};
      rws.rewrite(w3);
      REQUIRE(w3 == string_type({0}));

      string_type w4 = {0, 1, 2, 0};
      rws.rewrite(w4);
      REQUIRE(w4 == string_type({0}));

      string_type w5 = {2, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 1, 0, 2, 1, 0, 2, 1,
                        0, 2, 0, 1, 0, 2, 0, 1, 1, 0, 2, 2, 0, 1, 1, 0, 2, 0, 1,
                        1, 0, 2, 2, 0, 1, 0, 2, 0, 1, 1, 0, 2, 0, 1, 1, 0};
      rws.rewrite(w5);
      REQUIRE(w5 == string_type({0}));
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<LenLexCmp>",
                            "003",
                            "confluent",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rws;
      rws.increase_alphabet_size_by(3);

      rewriting_system::add_rule(rws, "ab"_w, "ba"_w);
      rewriting_system::add_rule(rws, "ac"_w, "ca"_w);
      rewriting_system::add_rule(rws, "aa"_w, "a"_w);
      rewriting_system::add_rule(rws, "ac"_w, "a"_w);
      rewriting_system::add_rule(rws, "ca"_w, "a"_w);
      rewriting_system::add_rule(rws, "bb"_w, "bb"_w);
      rewriting_system::add_rule(rws, "bc"_w, "cb"_w);
      rewriting_system::add_rule(rws, "bbb"_w, "b"_w);
      rewriting_system::add_rule(rws, "bc"_w, "b"_w);
      rewriting_system::add_rule(rws, "cb"_w, "b"_w);
      rewriting_system::add_rule(rws, "a"_w, "b"_w);

      REQUIRE(rws.number_of_rules() == 10);
      rewriting_system::add_rule(rws, "a"_w, "a"_w);
      REQUIRE(rws.number_of_rules() == 10);

      REQUIRE(rws.confluent());
      v4::ToWord to_word({0, 1, 2});
      REQUIRE((rws.rules() | rx::transform([&to_word](auto const& pair) {
                 return std::pair(to_word(pair.first), to_word(pair.second));
               })
               | rx::to_vector())
              == std::vector<std::pair<word_type, word_type>>(
                  {{{2, 0}, {0}}, {{0, 2}, {0}}, {{1}, {0}}, {{0, 0}, {0}}}));
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<LenLexCmp>",
                            "004",
                            "non-confluent",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rt;
      rt.increase_alphabet_size_by(2);
      rewriting_system::add_rule(rt, "aaa"_w, ""_w);
      rewriting_system::add_rule(rt, "bbb"_w, ""_w);
      rewriting_system::add_rule(rt, "ababab"_w, ""_w);
      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<LenLexCmp>",
                            "005",
                            "Example 5.1 in Sims (infinite)",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rt;
      rt.increase_alphabet_size_by(4);
      rewriting_system::add_rule(rt, "ab"_w, ""_w);
      rewriting_system::add_rule(rt, "ba"_w, ""_w);
      rewriting_system::add_rule(rt, "cd"_w, ""_w);
      rewriting_system::add_rule(rt, "dc"_w, ""_w);
      rewriting_system::add_rule(rt, "ca"_w, "ac"_w);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<LenLexCmp>",
                            "006",
                            "non-confluent",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rt;

      rt.increase_alphabet_size_by(4);
      rewriting_system::add_rule(rt, "ca"_w, ""_w);
      rewriting_system::add_rule(rt, "ac"_w, ""_w);
      rewriting_system::add_rule(rt, "db"_w, ""_w);
      rewriting_system::add_rule(rt, "bd"_w, ""_w);
      rewriting_system::add_rule(rt, "ba"_w, "ab"_w);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<LenLexCmp>",
                            "007",
                            "Example 5.3 in Sims",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rt;
      rt.increase_alphabet_size_by(2);
      rewriting_system::add_rule(rt, "aa"_w, ""_w);
      rewriting_system::add_rule(rt, "bbb"_w, ""_w);
      rewriting_system::add_rule(rt, "ababab"_w, ""_w);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<LenLexCmp>",
                            "008",
                            "Example 5.4 in Sims",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rt;
      rt.increase_alphabet_size_by(3);

      rewriting_system::add_rule(rt, "aa"_w, ""_w);
      rewriting_system::add_rule(rt, "bc"_w, ""_w);
      rewriting_system::add_rule(rt, "bbb"_w, ""_w);
      rewriting_system::add_rule(rt, "ababab"_w, ""_w);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<LenLexCmp>",
                            "009",
                            "Example 6.4 in Sims (size 168)",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rt;
      rt.increase_alphabet_size_by(3);

      rewriting_system::add_rule(rt, "aa"_w, ""_w);
      rewriting_system::add_rule(rt, "bc"_w, ""_w);
      rewriting_system::add_rule(rt, "bbb"_w, ""_w);
      rewriting_system::add_rule(rt, "ababababababab"_w, ""_w);
      rewriting_system::add_rule(rt, "abacabacabacabac"_w, ""_w);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<LenLexCmp>",
                            "010",
                            "random example",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rt;

      rt.increase_alphabet_size_by(3);
      rewriting_system::add_rule(rt, "aaa"_w, "c"_w);
      rewriting_system::add_rule(rt, "bbb"_w, "c"_w);
      rewriting_system::add_rule(rt, "ababab"_w, "c"_w);
      rewriting_system::add_rule(rt, "ac"_w, "a"_w);
      rewriting_system::add_rule(rt, "bc"_w, "b"_w);
      rewriting_system::add_rule(rt, "bc"_w, "c"_w);

      REQUIRE(rt.number_of_rules() == 6);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<ReturnFalse>",
                            "011",
                            "not obviously terminating example",
                            "[quick]") {
      using rule_type                 = std::pair<std::string, std::string>;
      auto                         rg = ReportGuard(false);
      RewritingSystemTrie<NoOrder> rws;

      rws.increase_alphabet_size_by(3);
      rewriting_system::add_rule(rws, "aaa"_w, "c"_w);
      rewriting_system::add_rule(rws, "c"_w, "bbb"_w);
      rewriting_system::add_rule(rws, "ababab"_w, "c"_w);
      rewriting_system::add_rule(rws, "a"_w, "ac"_w);
      rewriting_system::add_rule(rws, "bc"_w, "b"_w);
      rewriting_system::add_rule(rws, "bc"_w, "c"_w);

      REQUIRE((rws.rules()
               | rx::transform([](auto const& pair) { return rule_type(pair); })
               | rx::to_vector())
              == std::vector<rule_type>({{{0, 0, 0}, {2}},
                                         {{2}, {1, 1, 1}},
                                         {{0, 1, 0, 1, 0, 1}, {2}},
                                         {{0}, {0, 2}},
                                         {{1, 2}, {1}},
                                         {{1, 2}, {2}}}));
      REQUIRE(rewriting_system::is_length_non_increasing_no_reduce(rws)
              == tril::unknown);
      REQUIRE(rewriting_system::is_terminating_no_reduce(rws) == tril::unknown);
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystemTrie<ReturnFalse>",
                            "012",
                            "not obviously terminating example",
                            "[quick]") {
      using rule_type                 = std::pair<std::string, std::string>;
      auto                         rg = ReportGuard(false);
      RewritingSystemTrie<NoOrder> rws;

      rws.increase_alphabet_size_by(3);
      rewriting_system::add_rule(rws, "aa"_w, "bbb"_w);
      rewriting_system::add_rule(rws, "bbb"_w, "ccc"_w);

      REQUIRE((rws.rules()
               | rx::transform([](auto const& pair) { return rule_type(pair); })
               | rx::sort() | rx::to_vector())
              == std::vector<rule_type>(
                  {{{0, 0}, {1, 1, 1}}, {{1, 1, 1}, {2, 2, 2}}}));
      rws.reduce();
      REQUIRE((rws.rules()
               | rx::transform([](auto const& pair) { return rule_type(pair); })
               | rx::sort() | rx::to_vector())
              == std::vector<rule_type>(
                  {{{0, 0}, {2, 2, 2}}, {{1, 1, 1}, {2, 2, 2}}}));
      REQUIRE(!rws.confluent());

      REQUIRE(!rewriting_system::is_length_non_increasing(rws));
      REQUIRE(rewriting_system::is_terminating(rws) == tril::unknown);

      std::string w({0, 0});
      rws.rewrite(w);
      REQUIRE(w == std::string({2, 2, 2}));
      REQUIRE(rewriting_system::is_terminating(rws) == tril::unknown);
    }

    LIBSEMIGROUPS_TEST_CASE("Rules", "013", "constructors/init", "[quick]") {
      auto  rg = ReportGuard(false);
      Rules rules1;

      Rules rules2(rules1);
      Rules rules3(std::move(rules1));
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystem",
                            "014",
                            "constructors/init",
                            "[quick]") {
      auto                           rg = ReportGuard(false);
      RewritingSystemTrie<LenLexCmp> rws;

      rws.increase_alphabet_size_by(3);
      rewriting_system::add_rule(rws, "aaa"_w, "c"_w);
      rewriting_system::add_rule(rws, "c"_w, "bbb"_w);
      rewriting_system::add_rule(rws, "ababab"_w, "c"_w);
      rewriting_system::add_rule(rws, "a"_w, "ac"_w);
      rewriting_system::add_rule(rws, "bc"_w, "b"_w);
      rewriting_system::add_rule(rws, "bc"_w, "c"_w);
      REQUIRE(rws.number_of_rules() == 6);

      rws.init();
      REQUIRE(rws.number_of_rules() == 0);
      REQUIRE(rws.trie().number_of_nodes() == 1);
      REQUIRE(rewriting_system::is_length_non_increasing(rws));
      REQUIRE(rewriting_system::is_terminating(rws) == tril::TRUE);

      rws.increase_alphabet_size_by(3);
      rewriting_system::add_rule(rws, "aaa"_w, "c"_w);
      rewriting_system::add_rule(rws, "bbb"_w, "c"_w);
      rewriting_system::add_rule(rws, "ababab"_w, "c"_w);
      rewriting_system::add_rule(rws, "ac"_w, "a"_w);
      rewriting_system::add_rule(rws, "bc"_w, "b"_w);
      rewriting_system::add_rule(rws, "bc"_w, "c"_w);

      auto copy = rws;
      REQUIRE(rws.number_of_rules() == 6);
      REQUIRE(!rws.confluent());
      REQUIRE(copy.number_of_rules() == 6);
      REQUIRE(!copy.confluent());

      copy = rws;
      REQUIRE(rws.number_of_rules() == 4);
      REQUIRE(!rws.confluent());
      REQUIRE(copy.number_of_rules() == 4);
      REQUIRE(!copy.confluent());

      rws.init();
      copy = std::move(rws);
      REQUIRE(copy.number_of_rules() == 0);
      REQUIRE(copy.trie().number_of_nodes() == 1);

      copy.increase_alphabet_size_by(3);
      rewriting_system::add_rule(copy, "aaa"_w, "c"_w);
      rewriting_system::add_rule(copy, "bbb"_w, "c"_w);
      rewriting_system::add_rule(copy, "ababab"_w, "c"_w);
      rewriting_system::add_rule(copy, "ac"_w, "a"_w);
      rewriting_system::add_rule(copy, "bc"_w, "b"_w);
      rewriting_system::add_rule(copy, "bc"_w, "c"_w);

      auto other_copy(copy);
      REQUIRE(copy.number_of_rules() == 6);
      REQUIRE(copy.trie().number_of_nodes() == 1);
      REQUIRE(other_copy.number_of_rules() == 6);
      REQUIRE(other_copy.trie().number_of_nodes() == 1);

      auto other_other_copy(std::move(copy));
      REQUIRE(other_other_copy.number_of_rules() == 6);
      REQUIRE(other_other_copy.trie().number_of_nodes() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("RewritingSystem",
                            "015",
                            "is_terminating",
                            "[quick]") {
      auto                         rg = ReportGuard(false);
      RewritingSystemTrie<NoOrder> rws;
      rws.increase_alphabet_size_by(3);
      rewriting_system::add_rule(rws, "bbb"_w, "aa"_w);
      rewriting_system::add_rule(rws, "bbb"_w, "ccc"_w);
      REQUIRE(rewriting_system::is_terminating_no_reduce(rws) == tril::unknown);
      REQUIRE(rws.is_reduced() == tril::unknown);
      REQUIRE(rewriting_system::is_terminating(rws) == tril::TRUE);
      REQUIRE(rws.is_reduced() == tril::TRUE);
    }

    LIBSEMIGROUPS_TEMPLATE_TEST_CASE("RewritingSystem",
                                     "016",
                                     "confluence_ratio",
                                     "[quick]",
                                     RewritingSystemSet<LenLexCmp>,
                                     RewritingSystemTrie<LenLexCmp>) {
      auto     rg = ReportGuard(false);
      TestType rws;
      REQUIRE(rws.confluence_ratio() == std::pair<size_t, size_t>{0, 0});

      rws.increase_alphabet_size_by(2);
      rewriting_system::add_rule(rws, "ab"_w, "b"_w);
      REQUIRE(rws.confluence_ratio() == std::pair<size_t, size_t>{0, 0});

      rws.increase_alphabet_size_by(1);
      rewriting_system::add_rule(rws, "ca"_w, "c"_w);
      REQUIRE(rws.confluence_ratio() == std::pair<size_t, size_t>{1, 1});
      REQUIRE(rws.confluent());

      rewriting_system::add_rule(rws, "ba"_w, "a"_w);
      REQUIRE(!rws.confluent_known());
      REQUIRE(rws.confluence_ratio() == std::pair<size_t, size_t>{1, 3});
      REQUIRE(rws.confluent_known());
      REQUIRE(!rws.confluent());
    }

    LIBSEMIGROUPS_TEMPLATE_TEST_CASE("RewritingSystem",
                                     "017",
                                     "WtLenLex",
                                     "[quick]",
                                     RewritingSystemSet<WtLenLexCmp>,
                                     RewritingSystemTrie<WtLenLexCmp>) {
      auto rg         = ReportGuard(false);
      using rule_type = std::pair<std::string, std::string>;

      TestType rws;
      rws.increase_alphabet_size_by(2);
      rws.init();
      rws.order().init(std::vector<size_t>({1, 1}));
      rewriting_system::add_rule(rws, "aab"_w, "bb"_w);
      REQUIRE((rws.rules()
               | rx::transform([](auto const& pair) { return rule_type(pair); })
               | rx::to_vector())
              == std::vector<rule_type>({{{0, 0, 1}, {1, 1}}}));

      rws.init();
      rws.order().init({1, 3});
      rws.increase_alphabet_size_by(2);
      rewriting_system::add_rule(rws, "aab"_w, "bb"_w);
      REQUIRE((rws.rules()
               | rx::transform([](auto const& pair) { return rule_type(pair); })
               | rx::to_vector())
              == std::vector<rule_type>({{{1, 1}, {0, 0, 1}}}));
    }

    LIBSEMIGROUPS_TEMPLATE_TEST_CASE("RewritingSystem",
                                     "018",
                                     "Wr",
                                     "[quick]",
                                     RewritingSystemSet<WrCmp>,
                                     RewritingSystemTrie<WrCmp>) {
      auto rg         = ReportGuard(false);
      using rule_type = std::pair<std::string, std::string>;

      static_assert(std::is_same_v<typename TestType::reduction_order,
                                   WrCmp<Default, false>>);

      TestType rws;
      rws.increase_alphabet_size_by(3);
      rws.order().init({0, 0, 1});
      rewriting_system::add_rule(rws, "ac"_w, "bc"_w);
      REQUIRE((rws.rules()
               | rx::transform([](auto const& pair) { return rule_type(pair); })
               | rx::to_vector())
              == std::vector<rule_type>({{{1, 2}, {0, 2}}}));
    }
  }  // namespace detail
}  // namespace libsemigroups
