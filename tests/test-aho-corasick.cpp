// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 Joseph Edwards + James D. Mitchell
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

#include <iostream>

#include "libsemigroups/aho-corasick.hpp"  // for AhoCorasick
#include "libsemigroups/types.hpp"         // for word_type

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  using namespace literals;

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "000",
                          "initial test",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    aho_corasick::add_word_no_checks(ac, 00101_w);
    aho_corasick::add_word_no_checks(ac, 010_w);

    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(aho_corasick::traverse(ac, 00101_w) == 5);
    REQUIRE(aho_corasick::traverse(ac, 010_w) == 7);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "001",
                          "all words size 4",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;

    Words words;
    words.number_of_letters(2).min(4).max(5);
    for (auto const& w : words) {
      aho_corasick::add_word_no_checks(ac, w);
    }

    REQUIRE(ac.number_of_nodes() == 31);

    REQUIRE(aho_corasick::traverse(ac, 0000_w) == 4);
    REQUIRE(aho_corasick::traverse(ac, 0001_w) == 5);
    REQUIRE(aho_corasick::traverse(ac, 0010_w) == 7);
    REQUIRE(aho_corasick::traverse(ac, 0011_w) == 8);
    REQUIRE(aho_corasick::traverse(ac, 0100_w) == 11);
    REQUIRE(aho_corasick::traverse(ac, 0101_w) == 12);
    REQUIRE(aho_corasick::traverse(ac, 0110_w) == 14);
    REQUIRE(aho_corasick::traverse(ac, 0111_w) == 15);
    REQUIRE(aho_corasick::traverse(ac, 1000_w) == 19);
    REQUIRE(aho_corasick::traverse(ac, 1001_w) == 20);
    REQUIRE(aho_corasick::traverse(ac, 1010_w) == 22);
    REQUIRE(aho_corasick::traverse(ac, 1011_w) == 23);
    REQUIRE(aho_corasick::traverse(ac, 1100_w) == 26);
    REQUIRE(aho_corasick::traverse(ac, 1101_w) == 27);
    REQUIRE(aho_corasick::traverse(ac, 1110_w) == 29);
    REQUIRE(aho_corasick::traverse(ac, 1111_w) == 30);

    // Should do nothing
    aho_corasick::rm_word_no_checks(ac, 000_w);
    REQUIRE(ac.number_of_nodes() == 31);

    aho_corasick::rm_word_no_checks(ac, 0111_w);
    REQUIRE(ac.number_of_nodes() == 30);
    REQUIRE(aho_corasick::traverse(ac, 0111_w)
            == aho_corasick::traverse(ac, 111_w));
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "002",
                          "add/rm_word",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    aho_corasick::add_word_no_checks(ac, 00101_w);

    REQUIRE(ac.number_of_nodes() == 6);
    REQUIRE(aho_corasick::traverse(ac, 00101_w) == 5);
    REQUIRE(ac.node(5).is_terminal());
    REQUIRE(aho_corasick::traverse(ac, 010_w) == 1);
    REQUIRE(!ac.node(1).is_terminal());

    aho_corasick::add_word_no_checks(ac, 010_w);
    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(aho_corasick::traverse(ac, 010_w) == 7);
    REQUIRE(ac.node(5).is_terminal());
    REQUIRE(ac.node(7).is_terminal());

    aho_corasick::rm_word_no_checks(ac, 010_w);
    REQUIRE(ac.number_of_nodes() == 6);
    REQUIRE(aho_corasick::traverse(ac, 00101_w) == 5);
    REQUIRE(ac.node(5).is_terminal());
    REQUIRE(aho_corasick::traverse(ac, 010_w) == 1);
    REQUIRE(!ac.node(1).is_terminal());

    aho_corasick::add_word_no_checks(ac, 010_w);
    aho_corasick::add_word_no_checks(ac, 00_w);
    aho_corasick::rm_word_no_checks(ac, 00_w);

    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(aho_corasick::traverse(ac, 00101_w) == 5);
    REQUIRE(aho_corasick::traverse(ac, 00_w) == 2);
    REQUIRE(!ac.node(aho_corasick::traverse(ac, 00_w)).is_terminal());
    REQUIRE(ac.node(5).is_terminal());

    REQUIRE(aho_corasick::traverse(ac, 010_w) == 7);
    REQUIRE(ac.node(7).is_terminal());
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "003",
                          "long word",
                          "[quick][aho-corasick]") {
    using words::pow;
    using index_type = AhoCorasick::index_type;
    AhoCorasick ac;
    aho_corasick::add_word_no_checks(ac, pow(01_w, 1000));
    aho_corasick::add_word_no_checks(ac, pow(010_w, 10'000));
    REQUIRE(ac.number_of_nodes() == 31'998);
    for (index_type n = 0; n < ac.number_of_nodes(); ++n) {
      std::ignore = ac.suffix_link(n);
    }

    REQUIRE(ac.number_of_nodes() == 31'998);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "004",
                          "initial test with strings",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    std::string x = "aabab";
    std::string y = "aba";
    ac.add_word_no_checks(x.begin(), x.end());
    ac.add_word_no_checks(y.begin(), y.end());

    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(aho_corasick::traverse_from(ac, 0, x.begin(), x.end()) == 5);
    REQUIRE(aho_corasick::traverse_from(ac, 0, y.begin(), y.end()) == 7);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "005",
                          "initial test with string helpers",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    std::string x = "aabab";
    std::string y = "aba";
    aho_corasick::add_word_no_checks(ac, x);
    aho_corasick::add_word_no_checks(ac, y);

    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(aho_corasick::traverse(ac, x) == 5);
    REQUIRE(aho_corasick::traverse(ac, y) == 7);

    aho_corasick::rm_word_no_checks(ac, x);
    REQUIRE(ac.number_of_nodes() == 4);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "006",
                          "init",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;

    REQUIRE(ac.number_of_nodes() == 1);

    Words words;
    words.number_of_letters(2).min(4).max(5);
    for (auto const& w : words) {
      aho_corasick::add_word_no_checks(ac, w);
    }
    REQUIRE(ac.number_of_nodes() == 31);

    aho_corasick::rm_word_no_checks(ac, 0010_w);
    aho_corasick::rm_word_no_checks(ac, 0110_w);
    REQUIRE(ac.number_of_nodes() == 29);

    ac.init();
    REQUIRE(ac.number_of_nodes() == 1);
    REQUIRE(ac.node(0).number_of_children() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "007",
                          "height",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    std::string x = "aabab";
    std::string y = "aba";
    ac.add_word_no_checks(x.begin(), x.end());
    ac.add_word_no_checks(y.begin(), y.end());

    REQUIRE(ac.height(0) == 0);
    REQUIRE(ac.height(1) == 1);
    REQUIRE(ac.height(5) == 5);
    REQUIRE(ac.height(6) == 2);
    REQUIRE(ac.height(7) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "008",
                          "child",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    std::string b = "aab";
    std::string c = "aac";
    std::string d = "aad";
    ac.add_word_no_checks(b.begin(), b.end());
    ac.add_word_no_checks(c.begin(), c.end());
    ac.add_word_no_checks(d.begin(), d.end());

    REQUIRE(ac.child(0, 'a') == 1);
    REQUIRE(ac.child(0, 'b') == UNDEFINED);
    REQUIRE(ac.child(2, 'b') == 3);
    REQUIRE(ac.child(2, 'c') == 4);
    REQUIRE(ac.child(2, 'd') == 5);

    ac.rm_word_no_checks(d.begin(), d.end());
    REQUIRE(ac.child(2, 'b') == 3);
    REQUIRE(ac.child(2, 'c') == 4);
    REQUIRE(ac.child(2, 'd') == UNDEFINED);
    REQUIRE(aho_corasick::traverse_from(ac, 2, 'd') == 0);
    REQUIRE(aho_corasick::traverse_from(ac, 2, 'a') == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "009",
                          "signature",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;

    Words words;
    words.number_of_letters(2).min(4).max(5);
    for (auto const& w : words) {
      aho_corasick::add_word_no_checks(ac, w);
    }

    word_type w;
    ac.signature(w, 4);
    REQUIRE(w == 0000_w);
    ac.signature(w, 5);
    REQUIRE(w == 0001_w);
    ac.signature(w, 7);
    REQUIRE(w == 0010_w);
    ac.signature(w, 8);
    REQUIRE(w == 0011_w);
    ac.signature(w, 11);
    REQUIRE(w == 0100_w);
    ac.signature(w, 12);
    REQUIRE(w == 0101_w);
    ac.signature(w, 14);
    REQUIRE(w == 0110_w);
    ac.signature(w, 15);
    REQUIRE(w == 0111_w);
    ac.signature(w, 19);
    REQUIRE(w == 1000_w);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "010",
                          "dot",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;

    Words words;
    words.number_of_letters(2).min(4).max(5);
    for (auto const& w : words) {
      aho_corasick::add_word_no_checks(ac, w);
    }
    std::cout << "\n" << dot(ac).to_string() << "\n";
  }

}  // namespace libsemigroups
