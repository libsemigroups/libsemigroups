// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 Joseph Edwards
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

#include <fstream>

#include "catch_amalgamated.hpp"  // for AssertionHandler, ope...
#include "test-main.hpp"          // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/aho-corasick.hpp"      // for dot
#include "libsemigroups/detail/rewriters.hpp"  // for RewriteTrie

namespace libsemigroups {
  namespace detail {
    using namespace std::literals;
    LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "000", "initial test", "[quick]") {
      RewriteTrie rt = RewriteTrie();
      REQUIRE(rt.number_of_active_rules() == 0);
      rt.add_rule("ba"s, "a"s);
      REQUIRE(rt.number_of_active_rules() == 1);
      REQUIRE(rt.requires_alphabet());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "001", "simple test", "[quick]") {
      RewriteTrie rt = RewriteTrie();

      rt.add_rule("ac"s, "ca"s);
      rt.add_rule("aa"s, "a"s);
      rt.add_rule("ac"s, "a"s);
      rt.add_rule("ca"s, "a"s);
      rt.add_rule("bb"s, "bb"s);
      rt.add_rule("bc"s, "cb"s);
      rt.add_rule("bbb"s, "b"s);
      rt.add_rule("bc"s, "b"s);
      rt.add_rule("cb"s, "b"s);
      rt.add_rule("a"s, "b"s);

      REQUIRE(rt.confluent());

      std::string w1 = "aa";
      rt.rewrite(w1);
      REQUIRE(w1 == "a"s);

      std::string w2 = "ab";
      rt.rewrite(w2);
      REQUIRE(w2 == "a"s);

      std::string w3 = "abc";
      rt.rewrite(w3);
      REQUIRE(w3 == "a"s);

      std::string w4 = "abca";
      rt.rewrite(w4);
      REQUIRE(w4 == "a"s);

      std::string w5
          = "cbcabcabcabcbacbacbacabacabbaccabbacabbaccabacabbacabba";
      rt.rewrite(w5);
      REQUIRE(w5 == "a"s);
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "002",
                            "confluent fp semigroup 3 (infinite)",
                            "[quick]") {
      RewriteTrie rt = RewriteTrie();

      rt.add_rule("01"s, "10"s);
      rt.add_rule("02"s, "20"s);
      rt.add_rule("00"s, "0"s);
      rt.add_rule("02"s, "0"s);
      rt.add_rule("20"s, "0"s);
      rt.add_rule("11"s, "11"s);
      rt.add_rule("12"s, "21"s);
      rt.add_rule("111"s, "1"s);
      rt.add_rule("12"s, "1"s);
      rt.add_rule("21"s, "1"s);
      rt.add_rule("0"s, "1"s);

      REQUIRE(rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "003",
                            "non-confluent fp semigroup from "
                            "wikipedia (infinite)",
                            "[quick]") {
      RewriteTrie rt = RewriteTrie();
      rt.add_rule("000"s, ""s);
      rt.add_rule("111"s, ""s);
      rt.add_rule("010101"s, ""s);
      rt.add_to_alphabet('0');
      rt.add_to_alphabet('1');
      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "004",
                            "Example 5.1 in Sims (infinite)",
                            "[quick]") {
      RewriteTrie rt = RewriteTrie();
      rt.add_rule("ab"s, ""s);
      rt.add_rule("ba"s, ""s);
      rt.add_rule("cd"s, ""s);
      rt.add_rule("dc"s, ""s);
      rt.add_rule("ca"s, "ac"s);

      rt.add_to_alphabet('a');
      rt.add_to_alphabet('b');
      rt.add_to_alphabet('c');
      rt.add_to_alphabet('d');

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "005",
                            "Example 5.1 in Sims (infinite)",
                            "[quick]") {
      RewriteTrie rt = RewriteTrie();

      rt.add_rule("Aa"s, ""s);
      rt.add_rule("aA"s, ""s);
      rt.add_rule("Bb"s, ""s);
      rt.add_rule("bB"s, ""s);
      rt.add_rule("ba"s, "ab"s);

      rt.add_to_alphabet('A');
      rt.add_to_alphabet('a');
      rt.add_to_alphabet('B');
      rt.add_to_alphabet('b');

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "006",
                            "Example 5.3 in Sims",
                            "[quick]") {
      RewriteTrie rt = RewriteTrie();

      rt.add_rule("aa"s, ""s);
      rt.add_rule("bbb"s, ""s);
      rt.add_rule("ababab"s, ""s);

      rt.add_to_alphabet('a');
      rt.add_to_alphabet('b');

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "007",
                            "Example 5.4 in Sims",
                            "[quick]") {
      RewriteTrie rt = RewriteTrie();

      rt.add_rule("aa"s, ""s);
      rt.add_rule("bB"s, ""s);
      rt.add_rule("bbb"s, ""s);
      rt.add_rule("ababab"s, ""s);

      rt.add_to_alphabet('a');
      rt.add_to_alphabet('b');
      rt.add_to_alphabet('B');

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "008",
                            "Example 6.4 in Sims (size 168)",
                            "[no-valgrind][quick]") {
      RewriteTrie rt = RewriteTrie();

      rt.add_rule("aa"s, ""s);
      rt.add_rule("bc"s, ""s);
      rt.add_rule("bbb"s, ""s);
      rt.add_rule("ababababababab"s, ""s);
      rt.add_rule("abacabacabacabac"s, ""s);

      rt.add_to_alphabet('a');
      rt.add_to_alphabet('b');

      REQUIRE(!rt.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("RewriteTrie",
                            "009",
                            "random example",
                            "[no-valgrind][quick]") {
      RewriteTrie rt = RewriteTrie();

      rt.add_rule("000"s, "2"s);
      rt.add_rule("111"s, "2"s);
      rt.add_rule("010101"s, "2"s);
      rt.add_rule("02"s, "0"s);
      rt.add_rule("12"s, "1"s);
      rt.add_rule("12"s, "2"s);

      rt.add_to_alphabet('0');
      rt.add_to_alphabet('1');
      rt.add_to_alphabet('2');

      REQUIRE(!rt.confluent());
    }

    // LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "001", "simple test", "[quick]")
    // {
    //   RewriteTrie rt = RewriteTrie();
    // }

    // LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "001", "simple test", "[quick]")
    // {
    //   RewriteTrie rt = RewriteTrie();
    // }

    // LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "001", "simple test", "[quick]")
    // {
    //   RewriteTrie rt = RewriteTrie();
    // }

    // LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "001", "simple test", "[quick]")
    // {
    //   RewriteTrie rt = RewriteTrie();
    // }

    // LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "001", "simple test", "[quick]")
    // {
    //   RewriteTrie rt = RewriteTrie();
    // }

    // LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "001", "simple test", "[quick]")
    // {
    //   RewriteTrie rt = RewriteTrie();
    // }

    // LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "001", "simple test", "[quick]")
    // {
    //   RewriteTrie rt = RewriteTrie();
    // }
  }  // namespace detail
}  // namespace libsemigroups
