//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2026 James D. Mitchell
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

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/du-narendran-rusinowitch.hpp"  // for du_...

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "000",
                          "Simple test 1",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcde");
    p.rules = {"dbcbace", "cbbaec", "bcbad", "badbc"};

    REQUIRE(du_narendran_rusinowitch(p) == "edcab");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "001",
                          "Simple test 2",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    p.rules = {"a", "cc", "d", "bcc", "bccb", "c", "cccb", "bccc"};

    REQUIRE(du_narendran_rusinowitch(p) == "bcda");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "002",
                          "Simple test 3",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("aAbBc");
    p.contains_empty_word(true);
    p.rules = {"AA", "bc",     "bB",  "",     "Bb", "",    "Abc", "bcA",
               "a",  "cbA",    "AB",  "bbAb", "cc", "BB",  "cbb", "bbc",
               "Ac", "bbcbAb", "Abb", "BBA",  "cB", "BBcb"};

    REQUIRE(du_narendran_rusinowitch(p) == "bBcAa");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "003",
                          "Simple test 4",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    p.contains_empty_word(true);
    p.rules = {"bbba",
               "d",
               "ac",
               "a",
               "dc",
               "d",
               "bbc",
               "ddab",
               "cadaddab",
               "cc",
               "cba",
               "cadad",
               "aadaddab",
               "a",
               "dadaddab",
               "d",
               "aba",
               "aadad",
               "dba",
               "dadad",
               "aadab",
               "aadadda",
               "dadab",
               "dadadda",
               "cadaddaadad",
               "cca",
               "abc",
               "aadaddaddab",
               "dbc",
               "dadaddaddab",
               "cadaadaddad",
               "ca",
               "dbba",
               "dadaddad",
               "abba",
               "aadaddad",
               "ccadaadad",
               "cadaddaa",
               "aadaadad",
               "aadaddaa",
               "dadaadad",
               "dadaddaa",
               "aadaddaadad",
               "aa",
               "dadaddaadad",
               "da",
               "cadaadadb",
               "cadaadaddda",
               "caab",
               "caadda",
               "caaddab",
               "cadaadadd",
               "cadab",
               "cadaadadddaadadd",
               "cadaadadddaadadddab",
               "cadaadadddaadadddadda",
               "bad",
               "cadaadadddaadadddaddab",
               "cbc",
               "cadaadadddaadaddddab",
               "cadaadadddaadaddb",
               "c",
               "cbba",
               "cadaadadddaadaddd",
               "aaab",
               "aaadda",
               "daab",
               "daadda",
               "aaaddab",
               "aadaddaad",
               "daaddab",
               "dadaddaad",
               "aaaadad",
               "aaaddaa",
               "daaadad",
               "daaddaa",
               "caaadad",
               "caaddaa",
               "aaaddaadad",
               "aadaddaada",
               "daaddaadad",
               "dadaddaada",
               "caaddaadad",
               "cadaadadda",
               "cadaadadadad",
               "cadaadadddaa",
               "cadaadadddaadadda",
               "cadaadad",
               "cadaadadddaadadddaddaada",
               "cadaadadddaadad",
               "cadaadadddaadadddaadad",
               "cadaadadddaadadddaddaa",
               "cadaadadddab",
               "cadaadadddaadadddaddaad",
               "cadaadadddaadadb",
               "cadaadadddaadaddda",
               "cadaadadddaadadadad",
               "cadaadadddaadadddaa"};

    REQUIRE(du_narendran_rusinowitch(p) == "dcab");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "004",
                          "Simple test 5",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    p.contains_empty_word(true);
    p.rules = {
        "c", "bdad", "ab", "d", "ddad", "a", "adad", "ddaa", "aad", "ddddaa"};

    REQUIRE(du_narendran_rusinowitch(p) == "dbac");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "005",
                          "Simple test 6",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    p.contains_empty_word(true);
    // codespell:begin-ignore
    p.rules
        = {"c",     "bdaab",  "aba",  "ddaab",    "abd", "dab",        "adaabb",
           "a",     "ddaabb", "d",    "dddad",    "a",   "aaabb",      "ddda",
           "addad", "dddaa",  "adad", "ddddddaa", "aad", "dddddddddaa"};
    // codespell:end-ignore
    REQUIRE(du_narendran_rusinowitch(p) == "dabc");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "006",
                          "Simple test 7",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    p.contains_empty_word(true);
    p.rules = {"ac",       "a",
               "dc",       "d",
               "bad",      "dba",
               "baba",     "ddab",
               "cadabd",   "caddab",
               "dadabb",   "d",
               "cadabb",   "c",
               "aadabb",   "a",
               "ddabba",   "d",
               "ddabc",    "ddab",
               "ddabd",    "dddab",
               "cadabc",   "cadab",
               "aadabd",   "aaddab",
               "dadabd",   "daddab",
               "dddabab",  "ba",
               "caddabab", "ddddaba",
               "ddddad",   "ca",
               "aadabc",   "aadab",
               "dadabc",   "dadab",
               "caabb",    "dddd",
               "aaddabab", "addddaba",
               "daddabab", "dddddaba",
               "baa",      "dddaddab",
               "caabd",    "cadab",
               "cadddad",  "ddddaa",
               "aaabb",    "adddd",
               "daabb",    "ddddd",
               "cadabab",  "ddddddddaba",
               "aaabd",    "aadab",
               "daabd",    "dadab",
               "aadddad",  "addddaa",
               "dadddad",  "dddddaa",
               "caabc",    "caab",
               "aadabab",  "addddddddaba",
               "dadabab",  "dddddddddaba",
               "cadad",    "ddddddddddddaa",
               "caabab",   "ddddddddddddaba",
               "aadad",    "addddddddddddaa",
               "caddad",   "ddddddddaa",
               "aaabc",    "aaab",
               "daabc",    "daab",
               "dadad",    "dddddddddddddaa",
               "aaabab",   "addddddddddddaba",
               "daabab",   "dddddddddddddaba",
               "caad",     "ddddddddddddddddaa",
               "aaddad",   "addddddddaa",
               "daddad",   "dddddddddaa",
               "aaad",     "addddddddddddddddaa",
               "daad",     "dddddddddddddddddaa"};
    REQUIRE(du_narendran_rusinowitch(p) == "dcab");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "007",
                          "Simple test 8",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    p.contains_empty_word(true);
    p.rules = {"ddabd",
               "dddab",
               "bad",
               "dba",
               "aadabd",
               "aaddab",
               "aadabb",
               "a",
               "aaddababb",
               "adba",
               "ddabba",
               "d",
               "aaddababd",
               "aadddabab",
               "daddababd",
               "dadddabab",
               "dadabd",
               "daddab",
               "dadabb",
               "d",
               "daddababb",
               "ddba",
               "ddddadddababb",
               "ba",
               "adddddad",
               "aa",
               "ddddddad",
               "da",
               "aaabb",
               "addddd",
               "daabb",
               "dddddd",
               "baba",
               "dddabab",
               "baa",
               "ddddaddab",
               "c",
               "dddddaddabb",
               "aaababb",
               "adddddddddddba",
               "daababb",
               "ddddddddddddba",
               "aadababb",
               "addddddba",
               "aadababd",
               "aaddabab",
               "aaabd",
               "aadab",
               "aaddddad",
               "adddddaa",
               "dadababb",
               "dddddddba",
               "aaababd",
               "aadabab",
               "daababd",
               "dadabab",
               "dadababd",
               "daddabab",
               "daabd",
               "dadab",
               "daddddad",
               "ddddddaa",
               "dddababba",
               "ddab",
               "dddababd",
               "ddddabab",
               "aadddad",
               "addddddddddaa",
               "dadddad",
               "dddddddddddaa",
               "aaddad",
               "adddddddddddddddaa",
               "daddad",
               "ddddddddddddddddaa",
               "dadad",
               "dddddddddddddddddddddaa",
               "aadad",
               "addddddddddddddddddddaa",
               "aaad",
               "adddddddddddddddddddddddddaa",
               "daad",
               "ddddddddddddddddddddddddddaa"};

    REQUIRE(du_narendran_rusinowitch(p) == "dabc");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "008",
                          "Simple test 9",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    p.contains_empty_word(true);
    p.rules = {"ddabd",
               "dddab",
               "bad",
               "dba",
               "aadabd",
               "aaddab",
               "aadabb",
               "a",
               "aaddababb",
               "adba",
               "ddabba",
               "d",
               "aaddababd",
               "aadddabab",
               "daddababd",
               "dadddabab",
               "dadabd",
               "daddab",
               "dadabb",
               "d",
               "daddababb",
               "ddba",
               "ddddababab",
               "baba",
               "addddddad",
               "aa",
               "dddddddad",
               "da",
               "bababa",
               "dddabab",
               "aadddababab",
               "addddddababa",
               "dadddababab",
               "dddddddababa",
               "aaabb",
               "adddddd",
               "daabb",
               "ddddddd",
               "aadababb",
               "adddddddba",
               "dadababb",
               "ddddddddba",
               "aadababd",
               "aaddabab",
               "aaabd",
               "aadab",
               "daabd",
               "dadab",
               "baa",
               "dddddaddab",
               "aadddddad",
               "addddddaa",
               "dadddddad",
               "dddddddaa",
               "c",
               "ddddddaddabb",
               "aaddababab",
               "addddddddddddababa",
               "daddababab",
               "dddddddddddddababa",
               "dddababba",
               "ddab",
               "dddababd",
               "ddddabab",
               "dadababd",
               "daddabab",
               "aaababb",
               "adddddddddddddba",
               "daababb",
               "ddddddddddddddba",
               "dddddadddababb",
               "ba",
               "aaddddad",
               "addddddddddddaa",
               "daddddad",
               "dddddddddddddaa",
               "aadababab",
               "addddddddddddddddddababa",
               "dadababab",
               "dddddddddddddddddddababa",
               "aaddad",
               "addddddddddddddddddddddddaa",
               "daddad",
               "dddddddddddddddddddddddddaa",
               "aaababd",
               "aadabab",
               "daababd",
               "dadabab",
               "aadad",
               "addddddddddddddddddddddddddddddaa",
               "aadddad",
               "addddddddddddddddddaa",
               "dadddad",
               "dddddddddddddddddddaa",
               "aaababab",
               "addddddddddddddddddddddddababa",
               "daababab",
               "dddddddddddddddddddddddddababa",
               "dadad",
               "dddddddddddddddddddddddddddddddaa",
               "aaad",
               "addddddddddddddddddddddddddddddddddddaa",
               "daad",
               "dddddddddddddddddddddddddddddddddddddaa"};

    REQUIRE(du_narendran_rusinowitch(p) == "dabc");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "009",
                          "Empty rules + alphabet",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("");
    p.contains_empty_word(true);
    REQUIRE(du_narendran_rusinowitch(p) == "");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "010",
                          "Empty rules",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    p.contains_empty_word(true);
    REQUIRE(du_narendran_rusinowitch(p) == "abcd");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "011",
                          "Unused letters",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcdefghij");
    p.contains_empty_word(true);
    p.rules = {"j", ""};
    REQUIRE(du_narendran_rusinowitch(p) == "jihgfedcba");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "012",
                          "Cyclic rules",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    p.rules = {"a", "b", "b", "c", "c", "d", "d", "a"};
    REQUIRE(du_narendran_rusinowitch(p) == "");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "013",
                          "Common suffixes",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    p.rules = {"abac", "acac", "acac", "abac"};
    REQUIRE(du_narendran_rusinowitch(p) == "");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "014",
                          "Equal rules",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    p.rules = {"abac", "abac"};
    REQUIRE(du_narendran_rusinowitch(p) == "cba");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "015",
                          "Common suffix and prefix",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    // p.rules = {"aaaaaababaaaaaa", "a"};
    // REQUIRE(du_narendran_rusinowitch(p) == "cba");
    p.rules = {"a", "aaaaaababaaaaaa"};
    REQUIRE(du_narendran_rusinowitch(p) == "");
  }

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "016",
                          "Knuth-Bendix backtrack",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abc");

    presentation::add_rule(p, "baa", "c");
    presentation::add_rule(p, "aba", "cc");
    presentation::add_rule(p, "ac", "cca");
    presentation::add_rule(p, "bcccca", "cba");
    presentation::add_rule(p, "abcc", "ccba");
    presentation::add_rule(p, "bcccccc", "cbcc");
    //           [baa -> c, aba -> cc, ac -> cca, bcccca -> cba, abcc -> ccba,
    //           bcccccc -> cbcc]]

    REQUIRE(du_narendran_rusinowitch(p) == "cba");

    //    [
    //      baa->c,
    //      aba->cc,
    //      ac->cca,
    //      cba->bcccca,
    //      abcc->cbcccca,
    //      bccccaa->cc,
    //      bcccccc->cbcc,
    //      abcbcccca->ccbcc,
    //      cbcbcccca->bcccccbcccca,
    //      abcbcccccbcccca->ccbccbcc,
    //      cbcccccbcccccbcccca->ccbcbcccccbcccca,
    //      bcccccbcccccbcccca->cbcbcccccbcccca
    //    ]
    //
    //        [baa->c,
    //         aba->cc,
    //         ac->cca,
    //         cba->bcccca,
    //         abcc->cbcccca,
    //         bccccaa->cc,
    //         bcccccc->cbcc,
    //         abcbcccca->ccbcc,
    //         bcccccbcccca->cbcbcccca]
  }
}  // namespace libsemigroups
