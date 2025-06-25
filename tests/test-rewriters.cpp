// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 Joseph Edwards
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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for AssertionHandler, ope...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/aho-corasick-class.hpp"  // for dot
#include "libsemigroups/word-range.hpp"          // for operator""_w

#include "libsemigroups/detail/report.hpp"     // for ReportGuard
#include "libsemigroups/detail/rewriters.hpp"  // for RewriteTrie

namespace libsemigroups {
  using literals::operator""_w;
  namespace detail {

    using namespace std::literals;
    LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "000", "initial test", "[quick]") {
      auto        rg = ReportGuard(false);
      RewriteTrie rt = RewriteTrie();
      REQUIRE(rt.number_of_active_rules() == 0);
      rt.increase_alphabet_size_by(2);
      rt.add_rule("ba"_w, "a"_w);  // TODO should be a helper
      REQUIRE(rt.number_of_active_rules() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "001", "simple test", "[quick]") {
      auto        rg = ReportGuard(false);
      RewriteTrie rt = RewriteTrie();

      rt.increase_alphabet_size_by(3);
      rt.add_rule("ac"_w, "ca"_w);
      rt.add_rule("aa"_w, "a"_w);
      rt.add_rule("ac"_w, "a"_w);
      rt.add_rule("ca"_w, "a"_w);
      rt.add_rule("bb"_w, "bb"_w);
      rt.add_rule("bc"_w, "cb"_w);
      rt.add_rule("bbb"_w, "b"_w);
      rt.add_rule("bc"_w, "b"_w);
      rt.add_rule("cb"_w, "b"_w);
      rt.add_rule("a"_w, "b"_w);

      REQUIRE(rt.confluent());

      std::string w1 = {0, 0};
      rt.rewrite(w1);
      REQUIRE(w1 == std::string({0}));

      std::string w2 = {0, 1};
      rt.rewrite(w2);
      REQUIRE(w2 == std::string({0}));

      std::string w3 = {0, 1, 2};
      rt.rewrite(w3);
      REQUIRE(w3 == std::string({0}));

      std::string w4 = {0, 1, 2, 0};
      rt.rewrite(w4);
      REQUIRE(w4 == std::string({0}));

      std::string w5 = {2, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 1, 0, 2, 1, 0, 2, 1,
                        0, 2, 0, 1, 0, 2, 0, 1, 1, 0, 2, 2, 0, 1, 1, 0, 2, 0, 1,
                        1, 0, 2, 2, 0, 1, 0, 2, 0, 1, 1, 0, 2, 0, 1, 1, 0};
      rt.rewrite(w5);
      REQUIRE(w5 == std::string({0}));
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteFromLeft",
                            "010",
                            "simple test",
                            "[quick]") {
      auto            rg = ReportGuard(false);
      RewriteFromLeft rfl;

      rfl.increase_alphabet_size_by(3);
      rfl.add_rule("ac"_w, "ca"_w);
      rfl.add_rule("aa"_w, "a"_w);
      rfl.add_rule("ac"_w, "a"_w);
      rfl.add_rule("ca"_w, "a"_w);
      rfl.add_rule("bb"_w, "bb"_w);
      rfl.add_rule("bc"_w, "cb"_w);
      rfl.add_rule("bbb"_w, "b"_w);
      rfl.add_rule("bc"_w, "b"_w);
      rfl.add_rule("cb"_w, "b"_w);
      rfl.add_rule("a"_w, "b"_w);

      REQUIRE(rfl.confluent());

      std::string w1 = {0, 0};
      rfl.rewrite(w1);
      REQUIRE(w1 == std::string({0}));

      std::string w2 = {0, 1};
      rfl.rewrite(w2);
      REQUIRE(w2 == std::string({0}));

      std::string w3 = {0, 1, 2};
      rfl.rewrite(w3);
      REQUIRE(w3 == std::string({0}));

      std::string w4 = {0, 1, 2, 0};
      rfl.rewrite(w4);
      REQUIRE(w4 == std::string({0}));

      std::string w5 = {2, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 1, 0, 2, 1, 0, 2, 1,
                        0, 2, 0, 1, 0, 2, 0, 1, 1, 0, 2, 2, 0, 1, 1, 0, 2, 0, 1,
                        1, 0, 2, 2, 0, 1, 0, 2, 0, 1, 1, 0, 2, 0, 1, 1, 0};
      rfl.rewrite(w5);
      REQUIRE(w5 == std::string({0}));
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "002",
                            "confluent fp semigroup 3 (infinite)",
                            "[quick]") {
      auto        rg = ReportGuard(false);
      RewriteTrie rt = RewriteTrie();
      rt.increase_alphabet_size_by(3);

      rt.add_rule("ab"_w, "ba"_w);
      rt.add_rule("ac"_w, "ca"_w);
      rt.add_rule("aa"_w, "a"_w);
      rt.add_rule("ac"_w, "a"_w);
      rt.add_rule("ca"_w, "a"_w);
      rt.add_rule("bb"_w, "bb"_w);
      rt.add_rule("bc"_w, "cb"_w);
      rt.add_rule("bbb"_w, "b"_w);
      rt.add_rule("bc"_w, "b"_w);
      rt.add_rule("cb"_w, "b"_w);
      rt.add_rule("a"_w, "b"_w);

      REQUIRE(rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "003",
                            "non-confluent fp semigroup from "
                            "wikipedia (infinite)",
                            "[quick]") {
      auto        rg = ReportGuard(false);
      RewriteTrie rt = RewriteTrie();
      rt.increase_alphabet_size_by(2);
      rt.add_rule("aaa"_w, ""_w);
      rt.add_rule("bbb"_w, ""_w);
      rt.add_rule("ababab"_w, ""_w);
      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "004",
                            "Example 5.1 in Sims (infinite)",
                            "[quick]") {
      auto        rg = ReportGuard(false);
      RewriteTrie rt = RewriteTrie();
      rt.increase_alphabet_size_by(4);
      rt.add_rule("ab"_w, ""_w);
      rt.add_rule("ba"_w, ""_w);
      rt.add_rule("cd"_w, ""_w);
      rt.add_rule("dc"_w, ""_w);
      rt.add_rule("ca"_w, "ac"_w);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "005",
                            "Example 5.1 in Sims (infinite)",
                            "[quick]") {
      auto        rg = ReportGuard(false);
      RewriteTrie rt = RewriteTrie();

      rt.increase_alphabet_size_by(4);
      rt.add_rule("ca"_w, ""_w);
      rt.add_rule("ac"_w, ""_w);
      rt.add_rule("db"_w, ""_w);
      rt.add_rule("bd"_w, ""_w);
      rt.add_rule("ba"_w, "ab"_w);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "006",
                            "Example 5.3 in Sims",
                            "[quick]") {
      auto        rg = ReportGuard(false);
      RewriteTrie rt = RewriteTrie();
      rt.increase_alphabet_size_by(2);
      rt.add_rule("aa"_w, ""_w);
      rt.add_rule("bbb"_w, ""_w);
      rt.add_rule("ababab"_w, ""_w);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "007",
                            "Example 5.4 in Sims",
                            "[quick]") {
      auto        rg = ReportGuard(false);
      RewriteTrie rt = RewriteTrie();
      rt.increase_alphabet_size_by(3);

      rt.add_rule("aa"_w, ""_w);
      rt.add_rule("bc"_w, ""_w);
      rt.add_rule("bbb"_w, ""_w);
      rt.add_rule("ababab"_w, ""_w);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "008",
                            "Example 6.4 in Sims (size 168)",
                            "[no-valgrind][quick]") {
      auto        rg = ReportGuard(false);
      RewriteTrie rt = RewriteTrie();
      rt.increase_alphabet_size_by(3);

      rt.add_rule("aa"_w, ""_w);
      rt.add_rule("bc"_w, ""_w);
      rt.add_rule("bbb"_w, ""_w);
      rt.add_rule("ababababababab"_w, ""_w);
      rt.add_rule("abacabacabacabac"_w, ""_w);

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "009",
                            "random example",
                            "[no-valgrind][quick]") {
      auto        rg = ReportGuard(false);
      RewriteTrie rt = RewriteTrie();

      rt.increase_alphabet_size_by(3);
      rt.add_rule("aaa"_w, "c"_w);
      rt.add_rule("bbb"_w, "c"_w);
      rt.add_rule("ababab"_w, "c"_w);
      rt.add_rule("ac"_w, "a"_w);
      rt.add_rule("bc"_w, "b"_w);
      rt.add_rule("bc"_w, "c"_w);

      REQUIRE(!rt.confluent());
    }
  }  // namespace detail
}  // namespace libsemigroups
