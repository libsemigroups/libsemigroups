// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 Joseph Edwards + James D. Mitchell
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

#include <string>  // for basic_string, allocator
#include <tuple>   // for __ignore_t, ignore
#include <vector>  // for vector, operator==

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for SourceLineInfo, operator""...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/aho-corasick.hpp"  // for traverse_word, AhoCorasick
#include "libsemigroups/aho-corasick.hpp"  // for AhoCorasick
#include "libsemigroups/constants.hpp"     // for operator==, operator!=
#include "libsemigroups/dot.hpp"           // for Dot
#include "libsemigroups/exception.hpp"     // for LibsemigroupsException
#include "libsemigroups/types.hpp"         // for word_type
#include "libsemigroups/word-range.hpp"    // for operator""_w, WordRange, pow

#include "libsemigroups/detail/aho-corasick-impl.hpp"  // for traverse_word, AhoCorasick

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

    WordRange words;
    words.alphabet_size(2).min(4).max(5);
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

    WordRange words;
    words.alphabet_size(2).min(4).max(5);
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

    WordRange words;
    words.alphabet_size(2).min(4).max(5);
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

    WordRange words;
    words.alphabet_size(2).min(4).max(5);
    for (auto const& w : words) {
      aho_corasick::add_word(ac, w);
    }
    REQUIRE(!aho_corasick::dot(ac).to_string().empty());

    AhoCorasick ac2;
    aho_corasick::add_word(ac2, 0101_w);
    aho_corasick::add_word(ac2, 0110_w);
    aho_corasick::add_word(ac2, 01101_w);
    aho_corasick::add_word(ac2, 01100_w);
    aho_corasick::rm_word(ac2, 0101_w);
    REQUIRE(!aho_corasick::dot(ac2).to_string().empty());
  }

  namespace detail {
    LIBSEMIGROUPS_TEST_CASE("AhoCorasickImpl",
                            "012",
                            "contains_no_checks",
                            "[quick][aho-corasick]") {
      AhoCorasickImpl ac(2);

      std::ignore = aho_corasick_impl::add_word_no_checks(ac, 0101_w);
      std::ignore = aho_corasick_impl::add_word_no_checks(ac, 0110_w);
      std::ignore = aho_corasick_impl::add_word_no_checks(ac, 01101_w);
      std::ignore = aho_corasick_impl::add_word_no_checks(ac, 01100_w);

      REQUIRE(aho_corasick_impl::contains_no_checks(ac, 0101_w));
      REQUIRE(!aho_corasick_impl::contains_no_checks(ac, 010_w));

      WordRange words;
      words.alphabet_size(2).min(0).max(7);
      size_t count = 0;
      for (auto const& w : words) {
        count += aho_corasick_impl::contains_no_checks(ac, w);
      }

      REQUIRE(count == 4);
    }

    LIBSEMIGROUPS_TEST_CASE("AhoCorasickImpl",
                            "013",
                            "search",
                            "[quick][aho-corasick]") {
      using words::   operator+;
      AhoCorasickImpl ac(2);

      std::vector         subwords = {0101_w, 0110_w, 01101_w, 01100_w};
      std::vector<size_t> indexes;

      for (auto const& word : subwords) {
        indexes.push_back(aho_corasick_impl::add_word_no_checks(ac, word));
      }
      REQUIRE(indexes == std::vector<size_t>({4, 6, 7, 8}));

      auto find = [](word_type const& haystack, word_type const& needle) {
        if (haystack.size() < needle.size()) {
          return false;
        }
        auto first = needle.begin(), last = needle.end();
        auto len = last - first;
        for (auto it = haystack.begin(); it <= haystack.end() - len; ++it) {
          if (std::equal(first, last, it, it + len)) {
            return true;
          }
        }
        return false;
      };

      REQUIRE(!find("haystack"_w, "needle"_w));
      REQUIRE(find("haystack"_w, "hay"_w));
      REQUIRE(find("haystack"_w, "yst"_w));
      REQUIRE(find(0101_w, 0101_w));

      REQUIRE(aho_corasick_impl::search_no_checks(ac, 000000_w) == UNDEFINED);
      REQUIRE(*aho_corasick_impl::begin_search_no_checks(ac, 000000_w) == 0);
      REQUIRE(aho_corasick_impl::search_no_checks(ac, 11_w) == UNDEFINED);
      REQUIRE(aho_corasick_impl::search_no_checks(ac, ""_w) == UNDEFINED);

      WordRange words;
      words.alphabet_size(2).min(0).max(4);
      for (auto const& w : words) {
        REQUIRE(aho_corasick_impl::search_no_checks(ac, w) == UNDEFINED);
      }

      std::vector expected = {indexes[0], indexes[1], indexes[1], indexes[1]};
      for (auto const& [i, word] : rx::enumerate(subwords)) {
        REQUIRE(std::pair(i, aho_corasick_impl::search_no_checks(ac, word))
                == std::pair(i, expected[i]));
      }

      REQUIRE(aho_corasick_impl::search_no_checks(
                  ac, 000000000011111111111110101010101010101111110000011110_w)
              == 4);
      REQUIRE(
          aho_corasick_impl::search_no_checks(
              ac, 0000000000111111111111100110101010101010101111110000011110_w)
          == 6);
    }

    LIBSEMIGROUPS_TEST_CASE("AhoCorasickImpl",
                            "014",
                            "begin_search_no_checks",
                            "[quick][aho-corasick]") {
      using words::   operator+;
      AhoCorasickImpl ac(2);
      AhoCorasick     ac2;

      std::vector         subwords = {000_w, 111_w, 1010_w, 001100_w, 1100_w};
      std::vector<size_t> index;

      for (auto const& word : subwords) {
        index.push_back(aho_corasick_impl::add_word_no_checks(ac, word));
        aho_corasick::add_word(ac2, word);
      }

      //       for (auto it1 = ac.cbegin_nodes(); it1 != ac.cend_nodes(); ++it1)
      //       {
      //         REQUIRE(std::make_pair(*it1, ac.suffix_link(*it1))
      //                 == std::make_pair(*it1, ac2.suffix_link(*it1)));
      //       }

      REQUIRE(index == std::vector<size_t>({3, 6, 9, 13, 15}));

      auto it = aho_corasick_impl::begin_search_no_checks(ac, 001100_w);
      REQUIRE(*it == index[3]);
      ++it;
      REQUIRE(*it == index[4]);
      ++it;
      REQUIRE(*it == 0);
      REQUIRE(it == aho_corasick_impl::end_search_no_checks(ac, 001100_w));
    }

    LIBSEMIGROUPS_TEST_CASE("AhoCorasickImpl",
                            "015",
                            "begin_search_no_checks",
                            "[quick][aho-corasick]") {
      using index_type = AhoCorasickImpl::index_type;

      AhoCorasickImpl ac(2);
      std::vector     subwords = {001100_w, 0_w, 1_w, 00_w, 01_w, 10_w, 11_w};
      //                          6,        1,   7,   2,    8,    9,    10}));
      std::vector<size_t> index;

      for (auto const& word : subwords) {
        index.push_back(aho_corasick_impl::add_word_no_checks(ac, word));
      }
      REQUIRE(index == std::vector<size_t>({6, 1, 7, 2, 8, 9, 10}));

      auto w     = 001100_w;
      auto first = aho_corasick_impl::begin_search_no_checks(ac, w);
      auto last  = aho_corasick_impl::end_search_no_checks(ac, w);
      REQUIRE(
          std::vector(first, last)
          == std::vector<index_type>({1, 2, 1, 8, 7, 10, 7, 9, 1, 6, 2, 1}));
    }

    LIBSEMIGROUPS_TEST_CASE("AhoCorasickImpl",
                            "016",
                            "all words size 4",
                            "[quick][aho-corasick]") {
      AhoCorasickImpl ac(2);
      AhoCorasick     ac2;

      WordRange words;
      words.alphabet_size(2).min(4).max(5);
      for (auto const& w : words) {
        std::ignore = aho_corasick_impl::add_word_no_checks(ac, w);
        // std::ignore = aho_corasick::add_word(ac2, w);
      }
      // REQUIRE(aho_corasick::dot(ac2).to_string() == "");

      // REQUIRE(ac.number_of_nodes() == 7);

      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 0000_w) == 4);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 0001_w) == 5);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 0010_w) == 7);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 0011_w) == 8);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 0100_w) == 11);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 0101_w) == 12);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 0110_w) == 14);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 0111_w) == 15);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 1000_w) == 19);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 1001_w) == 20);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 1010_w) == 22);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 1011_w) == 23);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 1100_w) == 26);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 1101_w) == 27);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 1110_w) == 29);
      REQUIRE(aho_corasick_impl::traverse_word_no_checks(ac, 1111_w) == 30);

      // aho_corasick::rm_word(ac, 0111_w);
      // REQUIRE(ac.number_of_nodes() == 30);
      // REQUIRE(aho_corasick::traverse_word(ac, 0111_w)
      //         == aho_corasick::traverse_word(ac, 111_w));
    }

  }  // namespace detail
}  // namespace libsemigroups
