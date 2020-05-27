//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#include "catch.hpp"      // for REQUIRE etc
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/order.hpp"  // for LexicographicalCompare
#include "libsemigroups/siso.hpp"   // for cbegin_silo, cbegin_sislo

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("silo",
                          "000",
                          "alphabet: a, min: 0, max: 10",
                          "[silo][quick]") {
    auto first = "";
    auto last  = "aaaaaaaaaa";
    auto w     = std::vector<std::string>(cbegin_silo("a", 10, first, last),
                                      cend_silo("a", 10, first, last));
    REQUIRE(w.size() == 10);
    REQUIRE(w
            == std::vector<std::string>({"",
                                         "a",
                                         "aa",
                                         "aaa",
                                         "aaaa",
                                         "aaaaa",
                                         "aaaaaa",
                                         "aaaaaaa",
                                         "aaaaaaaa",
                                         "aaaaaaaaa"}));
    w = std::vector<std::string>(cbegin_silo("", 10, first, last),
                                 cend_silo("", 10, first, last));
    REQUIRE(w.size() == 1);
    w = std::vector<std::string>(cbegin_silo("a", 4, "aa", last),
                                 cend_silo("a", 4, "aa", last));
    REQUIRE(w == std::vector<std::string>({"aa", "aaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("silo", "001", "corner cases", "[silo][quick]") {
    auto first = "";
    auto last  = "aaaaaaaaaa";
    auto w1    = std::vector<std::string>(cbegin_silo("ab", 4, last, first),
                                       cend_silo("ab", 4, last, first));
    REQUIRE(w1.empty());
    auto w2 = std::vector<std::string>(cbegin_silo("ab", 4, first, first),
                                       cend_silo("ab", 4, first, first));
    REQUIRE(w2.empty());
    auto w3 = std::vector<std::string>(cbegin_silo("ab", 2, "a", "bb"),
                                       cend_silo("ab", 2, "a", "bb"));
    REQUIRE(w3.size() == 2);
    REQUIRE(w3 == std::vector<std::string>({"a", "b"}));
    auto w4 = std::vector<std::string>(cbegin_silo("ab", 1, first, last),
                                       cend_silo("ab", 1, first, last));
    REQUIRE(w4 == std::vector<std::string>({""}));
  }

  LIBSEMIGROUPS_TEST_CASE("silo",
                          "002",
                          "alphabet: ab, min: 1, max: 4",
                          "[silo][quick]") {
    auto w = std::vector<std::string>(cbegin_silo("ab", 4, "a", "bbbbb"),
                                      cend_silo("ab", 4, "a", "bbbbb"));
    REQUIRE(w
            == std::vector<std::string>({"a",
                                         "aa",
                                         "aaa",
                                         "aab",
                                         "ab",
                                         "aba",
                                         "abb",
                                         "b",
                                         "ba",
                                         "baa",
                                         "bab",
                                         "bb",
                                         "bba",
                                         "bbb"}));
    REQUIRE(w.size() == 14);
    REQUIRE(std::is_sorted(
        w.cbegin(), w.cend(), LexicographicalCompare<std::string>()));
    REQUIRE(std::is_sorted(cbegin_silo("ab", 4, "a", "bbbbb"),
                           cend_silo("ab", 4, "a", "bbbbb"),
                           LexicographicalCompare<std::string>()));
    w = std::vector<std::string>(cbegin_silo("ba", 4, "b", "aaaaa"),
                                 cend_silo("ba", 4, "b", "aaaaa"));
    REQUIRE(w
            == std::vector<std::string>({"b",
                                         "bb",
                                         "bbb",
                                         "bba",
                                         "ba",
                                         "bab",
                                         "baa",
                                         "a",
                                         "ab",
                                         "abb",
                                         "aba",
                                         "aa",
                                         "aab",
                                         "aaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("silo",
                          "003",
                          "alphabet: abc, min: 0, max: 13",
                          "[silo][quick]") {
    auto w = std::vector<std::string>(
        cbegin_silo("abc", 13, "", std::string(13, 'c')),
        cend_silo("abc", 13, "", std::string(13, 'c')));
    REQUIRE(w.size() == number_of_words(3, 0, 13));
    REQUIRE(w.size() == 797161);
    REQUIRE(std::is_sorted(
        w.cbegin(), w.cend(), LexicographicalCompare<std::string>()));
  }

  LIBSEMIGROUPS_TEST_CASE("silo", "004", "code coverage", "[silo][quick]") {
    auto first = "aaa";
    auto last  = "bbbb";

    const_silo_iterator it;
    it = cbegin_silo("ab", 5, first, last);
    REQUIRE(*it == "aaa");
    REQUIRE(it->size() == 3);
    REQUIRE(*it++ == "aaa");
    REQUIRE(*it == "aaaa");

    auto it2 = it;
    REQUIRE(it == it2);
    it++;
    REQUIRE(it2 != it);
    REQUIRE(*it == "aaab");
    REQUIRE(*it2 == "aaaa");

    swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it2 == "aaab");
    REQUIRE(*it == "aaaa");

    std::swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it == "aaab");
    REQUIRE(*it2 == "aaaa");
    it2++;
    REQUIRE(it == it2);
    REQUIRE(it++ == it2++);
    REQUIRE(it == it2);
    REQUIRE(++it == ++it2);
  }

  LIBSEMIGROUPS_TEST_CASE("sislo",
                          "005",
                          "alphabet: a, min: 0, max: 10",
                          "[sislo][quick]") {
    auto first = "";
    auto last  = "aaaaaaaaaa";
    auto w     = std::vector<std::string>(cbegin_sislo("a", first, last),
                                      cend_sislo("a", first, last));
    REQUIRE(w.size() == 10);
    REQUIRE(w
            == std::vector<std::string>({"",
                                         "a",
                                         "aa",
                                         "aaa",
                                         "aaaa",
                                         "aaaaa",
                                         "aaaaaa",
                                         "aaaaaaa",
                                         "aaaaaaaa",
                                         "aaaaaaaaa"}));
    w = std::vector<std::string>(cbegin_sislo("a", "aa", "aaaa"),
                                 cend_sislo("a", "aa", "aaaa"));
    REQUIRE(w == std::vector<std::string>({"aa", "aaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("sislo", "006", "corner cases", "[sislo][quick]") {
    auto first = "";
    auto last  = "bbaaab";
    auto w1    = std::vector<std::string>(cbegin_sislo("ab", last, first),
                                       cend_sislo("ab", last, first));
    REQUIRE(w1.empty());
    auto w2 = std::vector<std::string>(cbegin_sislo("ab", first, first),
                                       cend_sislo("ab", first, first));
    REQUIRE(w2.empty());
    auto w3 = std::vector<std::string>(cbegin_sislo("ab", "a", "aa"),
                                       cend_sislo("ab", "a", "aa"));
    REQUIRE(w3.size() == 2);
    REQUIRE(w3 == std::vector<std::string>({"a", "b"}));
    auto w4 = std::vector<std::string>(cbegin_sislo("ab", first, last),
                                       cend_sislo("ab", first, last));
    REQUIRE(w4
            == std::vector<std::string>(
                {"",       "a",      "b",      "aa",     "ab",     "ba",
                 "bb",     "aaa",    "aab",    "aba",    "abb",    "baa",
                 "bab",    "bba",    "bbb",    "aaaa",   "aaab",   "aaba",
                 "aabb",   "abaa",   "abab",   "abba",   "abbb",   "baaa",
                 "baab",   "baba",   "babb",   "bbaa",   "bbab",   "bbba",
                 "bbbb",   "aaaaa",  "aaaab",  "aaaba",  "aaabb",  "aabaa",
                 "aabab",  "aabba",  "aabbb",  "abaaa",  "abaab",  "ababa",
                 "ababb",  "abbaa",  "abbab",  "abbba",  "abbbb",  "baaaa",
                 "baaab",  "baaba",  "baabb",  "babaa",  "babab",  "babba",
                 "babbb",  "bbaaa",  "bbaab",  "bbaba",  "bbabb",  "bbbaa",
                 "bbbab",  "bbbba",  "bbbbb",  "aaaaaa", "aaaaab", "aaaaba",
                 "aaaabb", "aaabaa", "aaabab", "aaabba", "aaabbb", "aabaaa",
                 "aabaab", "aababa", "aababb", "aabbaa", "aabbab", "aabbba",
                 "aabbbb", "abaaaa", "abaaab", "abaaba", "abaabb", "ababaa",
                 "ababab", "ababba", "ababbb", "abbaaa", "abbaab", "abbaba",
                 "abbabb", "abbbaa", "abbbab", "abbbba", "abbbbb", "baaaaa",
                 "baaaab", "baaaba", "baaabb", "baabaa", "baabab", "baabba",
                 "baabbb", "babaaa", "babaab", "bababa", "bababb", "babbaa",
                 "babbab", "babbba", "babbbb", "bbaaaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("sislo",
                          "007",
                          "alphabet: ab, min: 1, max: 4",
                          "[sislo][quick]") {
    auto w = std::vector<std::string>(cbegin_sislo("ab", "a", "aaaa"),
                                      cend_sislo("ab", "a", "aaaa"));
    REQUIRE(w
            == std::vector<std::string>({"a",
                                         "b",
                                         "aa",
                                         "ab",
                                         "ba",
                                         "bb",
                                         "aaa",
                                         "aab",
                                         "aba",
                                         "abb",
                                         "baa",
                                         "bab",
                                         "bba",
                                         "bbb"}));
    REQUIRE(w.size() == 14);
    REQUIRE(
        std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare<std::string>()));
    REQUIRE(std::is_sorted(cbegin_sislo("ab", "a", "bbbbb"),
                           cend_sislo("ab", "a", "bbbbb"),
                           ShortLexCompare<std::string>()));
    w = std::vector<std::string>(cbegin_sislo("ba", "b", "bbbb"),
                                 cend_sislo("ba", "b", "bbbb"));
    REQUIRE(w
            == std::vector<std::string>({"b",
                                         "a",
                                         "bb",
                                         "ba",
                                         "ab",
                                         "aa",
                                         "bbb",
                                         "bba",
                                         "bab",
                                         "baa",
                                         "abb",
                                         "aba",
                                         "aab",
                                         "aaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("sislo",
                          "008",
                          "alphabet: abc, min: 0, max: 13",
                          "[sislo][quick]") {
    auto w = std::vector<std::string>(
        cbegin_sislo("abc", "", std::string(13, 'a')),
        cend_sislo("abc", "", std::string(13, 'a')));
    REQUIRE(w.size() == number_of_words(3, 0, 13));
    REQUIRE(w.size() == 797161);
    REQUIRE(
        std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare<std::string>()));
  }

  LIBSEMIGROUPS_TEST_CASE("sislo", "009", "code coverage", "[sislo][quick]") {
    auto first = "aaa";
    auto last  = "bbbb";

    const_sislo_iterator it;
    it = cbegin_sislo("ab", first, last);
    REQUIRE(*it == "aaa");
    REQUIRE(it->size() == 3);
    REQUIRE(*it++ == "aaa");
    REQUIRE(*it == "aab");

    auto it2 = it;
    REQUIRE(it == it2);
    it++;
    REQUIRE(it2 != it);
    REQUIRE(*it == "aba");
    REQUIRE(*it2 == "aab");

    swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it == "aab");
    REQUIRE(*it2 == "aba");

    std::swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it == "aba");
    REQUIRE(*it2 == "aab");
    it2++;
    REQUIRE(it == it2);
    REQUIRE(it++ == it2++);
    REQUIRE(it == it2);
    REQUIRE(++it == ++it2);
  }
}  // namespace libsemigroups
