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
#include "libsemigroups/exception.hpp"     // for LibsemigroupsException
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
    aho_corasick::add_word(ac, 00101_w);
    aho_corasick::add_word(ac, 010_w);

    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(aho_corasick::traverse_word(ac, 00101_w) == 5);
    REQUIRE(aho_corasick::traverse_word(ac, 010_w) == 7);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "001",
                          "all words size 4",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;

    Words words;
    words.number_of_letters(2).min(4).max(5);
    for (auto const& w : words) {
      aho_corasick::add_word(ac, w);
    }

    REQUIRE(ac.number_of_nodes() == 31);

    REQUIRE(aho_corasick::traverse_word(ac, 0000_w) == 4);
    REQUIRE(aho_corasick::traverse_word(ac, 0001_w) == 5);
    REQUIRE(aho_corasick::traverse_word(ac, 0010_w) == 7);
    REQUIRE(aho_corasick::traverse_word(ac, 0011_w) == 8);
    REQUIRE(aho_corasick::traverse_word(ac, 0100_w) == 11);
    REQUIRE(aho_corasick::traverse_word(ac, 0101_w) == 12);
    REQUIRE(aho_corasick::traverse_word(ac, 0110_w) == 14);
    REQUIRE(aho_corasick::traverse_word(ac, 0111_w) == 15);
    REQUIRE(aho_corasick::traverse_word(ac, 1000_w) == 19);
    REQUIRE(aho_corasick::traverse_word(ac, 1001_w) == 20);
    REQUIRE(aho_corasick::traverse_word(ac, 1010_w) == 22);
    REQUIRE(aho_corasick::traverse_word(ac, 1011_w) == 23);
    REQUIRE(aho_corasick::traverse_word(ac, 1100_w) == 26);
    REQUIRE(aho_corasick::traverse_word(ac, 1101_w) == 27);
    REQUIRE(aho_corasick::traverse_word(ac, 1110_w) == 29);
    REQUIRE(aho_corasick::traverse_word(ac, 1111_w) == 30);

    aho_corasick::rm_word(ac, 0111_w);
    REQUIRE(ac.number_of_nodes() == 30);
    REQUIRE(aho_corasick::traverse_word(ac, 0111_w)
            == aho_corasick::traverse_word(ac, 111_w));
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "002",
                          "add/rm_word",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    aho_corasick::add_word(ac, 00101_w);

    REQUIRE(ac.number_of_nodes() == 6);
    REQUIRE(aho_corasick::traverse_word(ac, 00101_w) == 5);
    REQUIRE(ac.node(5).is_terminal());
    REQUIRE(aho_corasick::traverse_word(ac, 010_w) == 1);
    REQUIRE(!ac.node(1).is_terminal());

    aho_corasick::add_word(ac, 010_w);
    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(aho_corasick::traverse_word(ac, 010_w) == 7);
    REQUIRE(ac.node(5).is_terminal());
    REQUIRE(ac.node(7).is_terminal());

    REQUIRE_THROWS_AS(aho_corasick::rm_word(ac, 01_w), LibsemigroupsException);
    REQUIRE_THROWS_AS(aho_corasick::rm_word(ac, 0101_w),
                      LibsemigroupsException);

    aho_corasick::rm_word(ac, 010_w);
    REQUIRE(ac.number_of_nodes() == 6);
    REQUIRE(aho_corasick::traverse_word(ac, 00101_w) == 5);
    REQUIRE(ac.node(5).is_terminal());
    REQUIRE(aho_corasick::traverse_word(ac, 010_w) == 1);
    REQUIRE(!ac.node(1).is_terminal());

    aho_corasick::add_word(ac, 010_w);
    REQUIRE_THROWS_AS(aho_corasick::add_word(ac, 010_w),
                      LibsemigroupsException);
    aho_corasick::add_word(ac, 00_w);
    aho_corasick::rm_word(ac, 00_w);

    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(aho_corasick::traverse_word(ac, 00101_w) == 5);
    REQUIRE(aho_corasick::traverse_word(ac, 00_w) == 2);
    REQUIRE(!ac.node(aho_corasick::traverse_word(ac, 00_w)).is_terminal());
    REQUIRE(ac.node(5).is_terminal());

    REQUIRE(aho_corasick::traverse_word(ac, 010_w) == 7);
    REQUIRE(ac.node(7).is_terminal());
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "003",
                          "path tree",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    aho_corasick::add_word(ac, 0_w);
    aho_corasick::add_word(ac, 00_w);
    aho_corasick::add_word(ac, 000_w);
    aho_corasick::add_word(ac, 0000_w);
    aho_corasick::add_word(ac, 00000_w);
    aho_corasick::add_word(ac, 000000_w);
    aho_corasick::add_word(ac, 0000000_w);

    REQUIRE(ac.number_of_nodes() == 8);

    for (auto i = 1; i <= 7; ++i) {
      REQUIRE(ac.node(i).is_terminal());
    }

    aho_corasick::rm_word(ac, 0000000_w);
    REQUIRE(ac.number_of_nodes() == 7);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "004",
                          "long word",
                          "[quick][aho-corasick]") {
    using words::pow;
    using index_type = AhoCorasick::index_type;
    AhoCorasick ac;
    aho_corasick::add_word(ac, pow(01_w, 1000));
    aho_corasick::add_word(ac, pow(010_w, 10'000));
    REQUIRE(ac.number_of_nodes() == 31'998);
    for (index_type n = 0; n < ac.number_of_nodes(); ++n) {
      std::ignore = ac.suffix_link(n);
    }

    REQUIRE(ac.number_of_nodes() == 31'998);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "005",
                          "initial test with strings",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    std::string x = "aabab";
    std::string y = "aba";
    ac.add_word(x.begin(), x.end());
    ac.add_word(y.begin(), y.end());

    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(aho_corasick::traverse_word(ac, 0, x.begin(), x.end()) == 5);
    REQUIRE(aho_corasick::traverse_word(ac, 0, y.begin(), y.end()) == 7);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "006",
                          "initial test with string helpers",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    std::string x = "aabab";
    std::string y = "aba";
    aho_corasick::add_word(ac, x);
    aho_corasick::add_word(ac, y);

    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(aho_corasick::traverse_word(ac, x) == 5);
    REQUIRE(aho_corasick::traverse_word(ac, y) == 7);

    aho_corasick::rm_word(ac, x);
    REQUIRE(ac.number_of_nodes() == 4);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "007",
                          "init",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;

    REQUIRE(ac.number_of_nodes() == 1);

    Words words;
    words.number_of_letters(2).min(4).max(5);
    for (auto const& w : words) {
      aho_corasick::add_word(ac, w);
    }
    REQUIRE(ac.number_of_nodes() == 31);

    aho_corasick::rm_word(ac, 0010_w);
    aho_corasick::rm_word(ac, 0110_w);
    REQUIRE(ac.number_of_nodes() == 29);

    ac.init();
    REQUIRE(ac.number_of_nodes() == 1);
    REQUIRE(ac.node(0).number_of_children() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "008",
                          "height",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    std::string x = "aabab";
    std::string y = "aba";
    ac.add_word(x.begin(), x.end());
    ac.add_word(y.begin(), y.end());

    REQUIRE(ac.height(0) == 0);
    REQUIRE(ac.height(1) == 1);
    REQUIRE(ac.height(5) == 5);
    REQUIRE(ac.height(6) == 2);
    REQUIRE(ac.height(7) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "009",
                          "child",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    std::string b = "aab";
    std::string c = "aac";
    std::string d = "aad";
    ac.add_word(b.begin(), b.end());
    ac.add_word(c.begin(), c.end());
    ac.add_word(d.begin(), d.end());

    REQUIRE(ac.child(0, 'a') == 1);
    REQUIRE(ac.child(0, 'b') == UNDEFINED);
    REQUIRE(ac.child(2, 'b') == 3);
    REQUIRE(ac.child(2, 'c') == 4);
    REQUIRE(ac.child(2, 'd') == 5);

    ac.rm_word(d.begin(), d.end());
    REQUIRE(ac.child(2, 'b') == 3);
    REQUIRE(ac.child(2, 'c') == 4);
    REQUIRE(ac.child(2, 'd') == UNDEFINED);
    REQUIRE(ac.traverse(2, 'd') == 0);
    REQUIRE(ac.traverse(2, 'a') == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "010",
                          "signature",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;

    Words words;
    words.number_of_letters(2).min(4).max(5);
    for (auto const& w : words) {
      aho_corasick::add_word(ac, w);
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
                          "011",
                          "dot",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;

    Words words;
    words.number_of_letters(2).min(4).max(5);
    for (auto const& w : words) {
      aho_corasick::add_word(ac, w);
    }
    REQUIRE(dot(ac).to_string() == R"(digraph {
  node [shape="box"]
  
  0  [label="&#949;"]
  1  [label="0"]
  10  [label="010"]
  11  [label="0100", peripheries="2"]
  12  [label="0101", peripheries="2"]
  13  [label="011"]
  14  [label="0110", peripheries="2"]
  15  [label="0111", peripheries="2"]
  16  [label="1"]
  17  [label="10"]
  18  [label="100"]
  19  [label="1000", peripheries="2"]
  2  [label="00"]
  20  [label="1001", peripheries="2"]
  21  [label="101"]
  22  [label="1010", peripheries="2"]
  23  [label="1011", peripheries="2"]
  24  [label="11"]
  25  [label="110"]
  26  [label="1100", peripheries="2"]
  27  [label="1101", peripheries="2"]
  28  [label="111"]
  29  [label="1110", peripheries="2"]
  3  [label="000"]
  30  [label="1111", peripheries="2"]
  4  [label="0000", peripheries="2"]
  5  [label="0001", peripheries="2"]
  6  [label="001"]
  7  [label="0010", peripheries="2"]
  8  [label="0011", peripheries="2"]
  9  [label="01"]
  0 -> 16  [color="#ff00ff", label="1"]
  0 -> 1  [color="#00ff00", label="0"]
  0 -> 0  [color="black", constraint="false", style="dashed"]
  1 -> 9  [color="#ff00ff", label="1"]
  1 -> 2  [color="#00ff00", label="0"]
  1 -> 0  [color="black", constraint="false", style="dashed"]
  2 -> 6  [color="#ff00ff", label="1"]
  2 -> 3  [color="#00ff00", label="0"]
  2 -> 1  [color="black", constraint="false", style="dashed"]
  3 -> 5  [color="#ff00ff", label="1"]
  3 -> 4  [color="#00ff00", label="0"]
  3 -> 2  [color="black", constraint="false", style="dashed"]
  4 -> 3  [color="black", constraint="false", style="dashed"]
  5 -> 6  [color="black", constraint="false", style="dashed"]
  6 -> 8  [color="#ff00ff", label="1"]
  6 -> 7  [color="#00ff00", label="0"]
  6 -> 9  [color="black", constraint="false", style="dashed"]
  7 -> 10  [color="black", constraint="false", style="dashed"]
  8 -> 13  [color="black", constraint="false", style="dashed"]
  9 -> 13  [color="#ff00ff", label="1"]
  9 -> 10  [color="#00ff00", label="0"]
  9 -> 16  [color="black", constraint="false", style="dashed"]
  10 -> 12  [color="#ff00ff", label="1"]
  10 -> 11  [color="#00ff00", label="0"]
  10 -> 17  [color="black", constraint="false", style="dashed"]
  11 -> 18  [color="black", constraint="false", style="dashed"]
  12 -> 21  [color="black", constraint="false", style="dashed"]
  13 -> 15  [color="#ff00ff", label="1"]
  13 -> 14  [color="#00ff00", label="0"]
  13 -> 24  [color="black", constraint="false", style="dashed"]
  14 -> 25  [color="black", constraint="false", style="dashed"]
  15 -> 28  [color="black", constraint="false", style="dashed"]
  16 -> 24  [color="#ff00ff", label="1"]
  16 -> 17  [color="#00ff00", label="0"]
  16 -> 0  [color="black", constraint="false", style="dashed"]
  17 -> 21  [color="#ff00ff", label="1"]
  17 -> 18  [color="#00ff00", label="0"]
  17 -> 1  [color="black", constraint="false", style="dashed"]
  18 -> 20  [color="#ff00ff", label="1"]
  18 -> 19  [color="#00ff00", label="0"]
  18 -> 2  [color="black", constraint="false", style="dashed"]
  19 -> 3  [color="black", constraint="false", style="dashed"]
  20 -> 6  [color="black", constraint="false", style="dashed"]
  21 -> 23  [color="#ff00ff", label="1"]
  21 -> 22  [color="#00ff00", label="0"]
  21 -> 9  [color="black", constraint="false", style="dashed"]
  22 -> 10  [color="black", constraint="false", style="dashed"]
  23 -> 13  [color="black", constraint="false", style="dashed"]
  24 -> 28  [color="#ff00ff", label="1"]
  24 -> 25  [color="#00ff00", label="0"]
  24 -> 16  [color="black", constraint="false", style="dashed"]
  25 -> 27  [color="#ff00ff", label="1"]
  25 -> 26  [color="#00ff00", label="0"]
  25 -> 17  [color="black", constraint="false", style="dashed"]
  26 -> 18  [color="black", constraint="false", style="dashed"]
  27 -> 21  [color="black", constraint="false", style="dashed"]
  28 -> 30  [color="#ff00ff", label="1"]
  28 -> 29  [color="#00ff00", label="0"]
  28 -> 24  [color="black", constraint="false", style="dashed"]
  29 -> 25  [color="black", constraint="false", style="dashed"]
  30 -> 28  [color="black", constraint="false", style="dashed"]
})");
  }

}  // namespace libsemigroups
