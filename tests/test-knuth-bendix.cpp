// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// TODO(later)
// 1. The other examples from Sims' book (Chapters 5 and 6) which use
//    reduction orderings different from shortlex
// 2. Examples from MAF

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <iostream>  // for ostringstream
#include <string>    // for string
#include <utility>   // for pair
#include <vector>    // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/element-helper.hpp"  // for TransfHelper
#include "libsemigroups/element.hpp"         // for Element, Transf, Transf...
#include "libsemigroups/froidure-pin.hpp"    // for FroidurePin
#include "libsemigroups/kbe.hpp"             // for detail::KBE
#include "libsemigroups/knuth-bendix.hpp"    // for KnuthBendix, operator<<
#include "libsemigroups/libsemigroups-config.hpp"  // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/report.hpp"                // for ReportGuard
#include "libsemigroups/siso.hpp"                  // for cbegin_silo
#include "libsemigroups/types.hpp"                 // for word_type

namespace libsemigroups {
  struct LibsemigroupsException;
  constexpr bool REPORT = false;

  template <typename TElementType>
  void delete_gens(std::vector<TElementType>& gens) {
    for (auto x : gens) {
      delete x;
    }
  }

  namespace fpsemigroup {

    using FroidurePinKBE = KnuthBendix::froidure_pin_type;

    std::vector<std::string> get_strings(FroidurePinKBE& S) {
      auto get_string
          = [&S](detail::KBE const& x) { return x.string(*S.state()); };
      std::vector<std::string> result(S.current_size(), "");
      std::transform(S.cbegin(), S.cend(), result.begin(), get_string);
      return result;
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "001",
                            "(fpsemi) transformation semigroup (size 4)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg      = ReportGuard(REPORT);
      using Transf = TransfHelper<2>::type;

      FroidurePin<Transf> S({Transf({1, 0}), Transf({0, 0})});
      REQUIRE(S.size() == 4);
      REQUIRE(S.nr_rules() == 4);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.nr_active_rules() == 4);
      REQUIRE(kb.size() == 4);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "002",
                            "(fpsemi) transformation semigroup (size 9)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto                  rg = ReportGuard(REPORT);
      std::vector<Element*> gens
          = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
             new Transformation<uint16_t>({0, 0, 0, 0, 0})};
      FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);
      REQUIRE(S.size() == 9);
      REQUIRE(S.degree() == 5);
      REQUIRE(S.nr_rules() == 3);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 3);
      REQUIRE(kb.size() == 9);
      delete_gens(gens);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "003",
                            "(fpsemi) transformation semigroup (size 88)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto                  rg = ReportGuard(REPORT);
      std::vector<Element*> gens
          = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
             new Transformation<uint16_t>({3, 2, 1, 3, 3})};
      FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);
      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);
      REQUIRE(S.nr_rules() == 18);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 18);
      REQUIRE(kb.size() == 88);
      delete_gens(gens);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "004",
                            "(fpsemi) confluent fp semigroup 1 (infinite)",
                            "[quick][knuth-bendix][fpsemi][fpsemigroup]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet(3);
      kb.add_rule({0, 1}, {1, 0});
      kb.add_rule({0, 2}, {2, 0});
      kb.add_rule({0, 0}, {0});
      kb.add_rule({0, 2}, {0});
      kb.add_rule({2, 0}, {0});
      kb.add_rule({1, 1}, {1, 1});
      kb.add_rule({1, 2}, {2, 1});
      kb.add_rule({1, 1, 1}, {1});
      kb.add_rule({1, 2}, {1});
      kb.add_rule({2, 1}, {1});
      kb.add_rule({0}, {1});

      REQUIRE(kb.confluent());
#ifdef LIBSEMIGROUPS_DEBUG
      REQUIRE(kb.alphabet() == "abc");
#endif
      REQUIRE(kb.nr_active_rules() == 4);
      REQUIRE(kb.normal_form({2, 0}) == word_type({0}));
      REQUIRE(kb.normal_form({0, 2}) == word_type({0}));
      REQUIRE(kb.equal_to(word_type({2, 0}), word_type({0})));
      REQUIRE(kb.equal_to(word_type({0, 2}), word_type({0})));
#ifdef LIBSEMIGROUPS_DEBUG
      REQUIRE(kb.normal_form("ac") == "a");
#endif
      REQUIRE(kb.is_obviously_infinite());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "005",
                            "(fpsemi) confluent fp semigroup 2 (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      REQUIRE_THROWS_AS(kb.add_rule({0, 1}, {1, 0}), LibsemigroupsException);

      kb.set_alphabet(3);
      kb.add_rule({0, 2}, {2, 0});
      kb.add_rule({0, 0}, {0});
      kb.add_rule({0, 2}, {0});
      kb.add_rule({2, 0}, {0});
      kb.add_rule({1, 1}, {1, 1});
      kb.add_rule({1, 2}, {2, 1});
      kb.add_rule({1, 1, 1}, {1});
      kb.add_rule({1, 2}, {1});
      kb.add_rule({2, 1}, {1});
      kb.add_rule({0}, {1});

      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 4);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "006",
                            "(fpsemi) confluent fp semigroup 3 (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("012");

      REQUIRE(kb.alphabet() == "012");
      REQUIRE(kb.nr_active_rules() == 0);

      kb.add_rule("01", "10");
      kb.add_rule("02", "20");
      kb.add_rule("00", "0");
      kb.add_rule("02", "0");
      kb.add_rule("20", "0");
      kb.add_rule("11", "11");
      kb.add_rule("12", "21");
      kb.add_rule("111", "1");
      kb.add_rule("12", "1");
      kb.add_rule("21", "1");
      kb.add_rule("0", "1");

      REQUIRE(kb.nr_active_rules() == 4);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 4);
      auto S = static_cast<FroidurePinKBE&>(*kb.froidure_pin());

      // At this point only the generators are known
      REQUIRE(S.current_size() == 2);

      std::vector<std::string> result = get_strings(S);
      std::vector<std::string> expected({"0", "2"});

      REQUIRE(result == expected);

      S.batch_size(10);
      S.enumerate(10);
      REQUIRE(S.current_size() == 12);
      auto get_string
          = [&S](detail::KBE const& x) { return x.string(*S.state()); };
      result.resize(S.current_size());
      std::transform(S.cbegin(), S.cend(), result.begin(), get_string);

      REQUIRE(result.size() == S.current_size());
      REQUIRE(result
              == std::vector<std::string>({"0",
                                           "2",
                                           "22",
                                           "222",
                                           "2222",
                                           "22222",
                                           "222222",
                                           "2222222",
                                           "22222222",
                                           "222222222",
                                           "2222222222",
                                           "22222222222"}));
    }
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "007",
                            "(fpsemi) non-confluent fp semigroup from "
                            "wikipedia (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("01");

      kb.add_rule("000", "");
      kb.add_rule("111", "");
      kb.add_rule("010101", "");

      REQUIRE(kb.alphabet() == "01");
      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 4);
      REQUIRE(kb.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "008",
                            "(fpsemi) Example 5.1 in Sims (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abcd");

      kb.add_rule("ab", "");
      kb.add_rule("ba", "");
      kb.add_rule("cd", "");
      kb.add_rule("dc", "");
      kb.add_rule("ca", "ac");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 8);
      REQUIRE(kb.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "009",
                            "(fpsemi) Example 5.1 in Sims (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbB");

      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");
      kb.add_rule("ba", "ab");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 8);
      REQUIRE(kb.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "010",
                            "(fpsemi) Example 5.3 in Sims",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aa", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 6);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "011",
                            "(fpsemi) Example 5.4 in Sims",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("Bab");

      kb.add_rule("aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 11);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "012",
        "(fpsemi) Example 6.4 in Sims (size 168)",
        "[no-valgrind][quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabac", "");

      REQUIRE(kb.alphabet() == "abc");
      REQUIRE(!kb.confluent());
      REQUIRE(!kb.is_obviously_infinite());
      REQUIRE(!kb.is_obviously_finite());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 40);
      REQUIRE(kb.confluent());
      REQUIRE(kb.normal_form("cc") == "b");
      REQUIRE(kb.normal_form("ccc") == "");

      REQUIRE(kb.size() == 168);
      auto S = static_cast<FroidurePinKBE&>(*kb.froidure_pin());
      REQUIRE(S.size() == 168);
      REQUIRE(S.generator(2).string(kb) == "c");
      // FIXME the next line compiles but leaves T in an invalid state.
      // auto T = FroidurePinKBE({S.generator(2)});
      auto T = FroidurePinKBE(kb);
      T.add_generator(S.generator(2));
      REQUIRE(T.size() == 3);
      REQUIRE(get_strings(T) == std::vector<std::string>({"c", "b", ""}));
    }

    // Takes approx. 1m33s
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "013",
                            "(fpsemi) Example 6.6 in Sims",
                            "[extreme][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabacabacabacabacabac", "");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.nr_active_rules() == 1026);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 10752);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "014",
                            "(fpsemi) Chapter 10, Section 4 in NR (infinite)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("aaaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("cccc", "c");
      kb.add_rule("abab", "aaa");
      kb.add_rule("bcbc", "bbb");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 31);
      REQUIRE(kb.confluent());
    }

    // Note: the fourth relator in NR's thesis incorrectly has exponent 3, it
    // should be 2. With exponent 3, the presentation defines the trivial
    // group, with exponent of 2, it defines the symmetric group as desired.
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "015",
                            "(fpsemi) Sym(5) from Chapter 3, Proposition "
                            "1.1 in NR "
                            "(size 120)",
                            "[no-valgrind][knuth-bendix][fpsemi][quick]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("ABab");

      kb.add_rule("aa", "");
      kb.add_rule("bbbbb", "");
      kb.add_rule("babababa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");
      kb.add_rule("BabBab", "");
      kb.add_rule("aBBabbaBBabb", "");
      kb.add_rule("aBBBabbbaBBBabbb", "");
      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.nr_active_rules() == 36);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 120);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "016",
        "(fpsemi) SL(2, 7) from Chapter 3, Proposition "
        "1.5 in NR "
        "(size 336)",
        "[no-valgrind][quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abAB");

      kb.add_rule("aaaaaaa", "");
      kb.add_rule("bb", "ababab");
      kb.add_rule("bb", "aaaabaaaabaaaabaaaab");
      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");

      // kb.clear_stack_interval(10);

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.nr_active_rules() == 152);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 336);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "017",
                            "(fpsemi) bicyclic monoid (infinite)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("ab", "");

      REQUIRE(kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 1);
      REQUIRE(kb.confluent());
      REQUIRE(kb.is_obviously_infinite());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "018",
                            "(fpsemi) plactic monoid of degree 2 (infinite)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("aba", "baa");
      kb.add_rule("bba", "bab");
      kb.add_rule("ac", "");
      kb.add_rule("ca", "");
      kb.add_rule("bc", "");
      kb.add_rule("cb", "");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.nr_active_rules() == 3);
      REQUIRE(kb.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "019",
                            "(fpsemi) example before Chapter 7, Proposition "
                            "1.1 in "
                            "NR (infinite)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aa", "a");
      kb.add_rule("bb", "b");

      REQUIRE(kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 2);
      REQUIRE(kb.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "020",
                            "(fpsemi) Chapter 7, Theorem 3.6 in NR (size 243)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("ababababab", "aa");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.nr_active_rules() == 12);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 243);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "092",
                            "(fpsemi) finite semigroup (size 99)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("abababab", "aa");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.nr_active_rules() == 9);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 99);
    }

    // See KBFP 07 also.
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "021",
        "(fpsemi) Chapter 7, Theorem 3.9 in NR (size 240)",
        "[no-valgrind][knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("abbba", "aa");
      kb.add_rule("baab", "bb");
      kb.add_rule("aabababababa", "aa");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 24);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 240);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "022",
                            "(fpsemi) F(2, 5) - Chapter 9, Section 1 in NR "
                            "(size 11)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcde");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "e");
      kb.add_rule("de", "a");
      kb.add_rule("ea", "b");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 24);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 11);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "023",
                            "(fpsemi) F(2, 6) - Chapter 9, Section 1 in NR",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcdef");

      kb.add_rule("ab", "");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "e");
      kb.add_rule("de", "f");
      kb.add_rule("ef", "a");
      kb.add_rule("fa", "b");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 35);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "024",
                            "(fpsemi) fp semigroup (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet(3);
      kb.add_rule({0, 1}, {1, 0});
      kb.add_rule({0, 2}, {2, 0});
      kb.add_rule({0, 0}, {0});
      kb.add_rule({0, 2}, {0});
      kb.add_rule({2, 0}, {0});
      kb.add_rule({1, 1}, {1, 1});
      kb.add_rule({1, 2}, {2, 1});
      kb.add_rule({1, 1, 1}, {1});
      kb.add_rule({1, 2}, {1});
      kb.add_rule({2, 1}, {1});
      kb.add_rule({0}, {1});

      REQUIRE(kb.confluent());
      // We could rewrite here and check equality by this is simpler since all
      // allocation and deletion is handled in equal_to
      REQUIRE(kb.equal_to(word_type({0, 0}), word_type({0})));
      REQUIRE(kb.equal_to(word_type({1, 1}), word_type({1, 1})));
      REQUIRE(kb.equal_to(word_type({1, 2}), word_type({2, 1})));
      REQUIRE(kb.equal_to(word_type({1, 0}), word_type({2, 2, 0, 1, 2})));
      REQUIRE(kb.equal_to(word_type({2, 1}), word_type({1, 1, 1, 2})));
      REQUIRE(!kb.equal_to(word_type({1, 0}), word_type({2})));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "025",
                            "(fpsemi) Chapter 11, Section 1 (q = 4, r = 3) "
                            "in NR (size 86)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbbb", "b");
      kb.add_rule("abbbabb", "bba");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.nr_active_rules() == 20);
      REQUIRE(kb.confluent());

      // Check that rewrite to a non-pointer argument does not rewrite its
      // argument
      std::string w = "aaa";
      REQUIRE(kb.rewrite(w) == "a");
      REQUIRE(w == "aaa");

      // defining relations
      REQUIRE(kb.rewrite("aaa") == kb.rewrite("a"));
      REQUIRE(kb.rewrite("bbbbb") == kb.rewrite("b"));
      REQUIRE(kb.rewrite("abbbabb") == kb.rewrite("bba"));

      // consequential relations (Chapter 11, Lemma 1.1 in NR)
      REQUIRE(kb.rewrite("babbbb") == kb.rewrite("ba"));
      REQUIRE(kb.rewrite("baabbbb") == kb.rewrite("baa"));
      REQUIRE(kb.rewrite("aabbbbbbbbbba") == kb.rewrite("bbbbbbbbbba"));
      REQUIRE(kb.rewrite("babbbbbbbbaa") == kb.rewrite("babbbbbbbb"));
      REQUIRE(kb.rewrite("baabbbbbbaa") == kb.rewrite("baabbbbbb"));
      REQUIRE(kb.rewrite("bbbbaabbbbaa") == kb.rewrite("bbbbaa"));
      REQUIRE(kb.rewrite("bbbaa") == kb.rewrite("baabb"));
      REQUIRE(kb.rewrite("abbbaabbba") == kb.rewrite("bbbbaa"));

      REQUIRE(kb.size() == 86);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "026",
        "(fpsemi) Chapter 11, Section 1 (q = 8, r = 5) in NR "
        "(size 746)",
        "[no-valgrind][knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbbbbbbb", "b");
      kb.add_rule("abbbbbabb", "bba");

      // kb.clear_stack_interval(0);

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.nr_active_rules() == 105);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 746);

      // defining relations
      REQUIRE(kb.rewrite("aaa") == kb.rewrite("a"));
      REQUIRE(kb.rewrite("bbbbbbbbb") == kb.rewrite("b"));
      REQUIRE(kb.rewrite("abbbbbabb") == kb.rewrite("bba"));

      // consequential relations (Chapter 11, Lemma 1.1 in NR)
      REQUIRE(kb.rewrite("babbbbbbbb") == kb.rewrite("ba"));
      REQUIRE(kb.rewrite("baabbbbbbbb") == kb.rewrite("baa"));
      REQUIRE(kb.rewrite("aabbbbbbbbbbbba") == kb.rewrite("bbbbbbbbbbbba"));
      REQUIRE(kb.rewrite("babbbbbbbbbbaa") == kb.rewrite("babbbbbbbbbb"));
      REQUIRE(kb.rewrite("baabbbbbbbbaa") == kb.rewrite("baabbbbbbbb"));
      REQUIRE(kb.rewrite("bbbbbbbbaabbbbbbbbaa") == kb.rewrite("bbbbbbbbaa"));
      REQUIRE(kb.rewrite("bbbaa") == kb.rewrite("baabb"));
      REQUIRE(kb.rewrite("abbbbbaabbbbba") == kb.rewrite("bbbbbbbbaa"));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "027",
        "(fpsemi) Chapter 11, Lemma 1.8 (q = 6, r = 5) in NR "
        "(infinite)",
        "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ABCabc");

      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");
      kb.add_rule("cC", "");
      kb.add_rule("Cc", "");
      kb.add_rule("aa", "");
      kb.add_rule("bbb", "");
      kb.add_rule("abaBaBabaBab", "");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 16);
      REQUIRE(kb.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "028",
                            "(fpsemi) Chapter 11, Section 2 (q = 6, r = 2, "
                            "alpha = "
                            "abaabba) in NR (size 4)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbbbbb", "b");
      kb.add_rule("abaabba", "bb");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 4);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 4);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "029",
                            "(fpsemi) Chapter 8, Theorem 4.2 in NR (infinite)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("bababababab", "b");
      kb.add_rule("baab", "babbbab");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.nr_active_rules() == 8);
      REQUIRE(kb.confluent());

      //  REQUIRE(!kb.less("bababababab", "aaaaa"));
      //  REQUIRE(kb.less("aaaaa", "bababababab"));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "030",
                            "(fpsemi) equal_to fp semigroup",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("ab", "ba");
      kb.add_rule("ac", "ca");
      kb.add_rule("aa", "a");
      kb.add_rule("ac", "a");
      kb.add_rule("ca", "a");
      kb.add_rule("bb", "bb");
      kb.add_rule("bc", "cb");
      kb.add_rule("bbb", "b");
      kb.add_rule("bc", "b");
      kb.add_rule("cb", "b");
      kb.add_rule("a", "b");

      REQUIRE(kb.equal_to("aa", "a"));
      REQUIRE(kb.equal_to("bb", "bb"));
      REQUIRE(kb.equal_to("bc", "cb"));
      REQUIRE(kb.equal_to("ba", "ccabc"));
      REQUIRE(kb.equal_to("cb", "bbbc"));
      REQUIRE(!kb.equal_to("ba", "c"));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "031",
        "(fpsemi) equal_to free semigroup",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet(2);
      REQUIRE(!kb.equal_to(word_type({0}), word_type({1})));
      REQUIRE(kb.equal_to(word_type({0}), word_type({0})));
      REQUIRE(kb.equal_to(word_type({0, 0, 0, 0, 0, 0, 0}),
                          word_type({0, 0, 0, 0, 0, 0, 0})));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "032",
        "(fpsemi) from GAP smalloverlap gap/test.gi (infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcdefg");

      kb.add_rule("abcd", "ce");
      kb.add_rule("df", "dg");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      REQUIRE(kb.equal_to("dfabcdf", "dfabcdg"));
      REQUIRE(kb.equal_to("abcdf", "ceg"));
      REQUIRE(kb.equal_to("abcdf", "cef"));

      kb.run();
      REQUIRE(kb.nr_active_rules() == 3);
      REQUIRE(kb.confluent());
      REQUIRE(kb.equal_to("dfabcdf", "dfabcdg"));
      REQUIRE(kb.equal_to("abcdf", "ceg"));
      REQUIRE(kb.equal_to("abcdf", "cef"));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "033: (fpsemi) from GAP smalloverlap gap/test.gi",
        "49 "
        "(infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcdefgh");

      kb.add_rule("abcd", "ce");
      kb.add_rule("df", "hd");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(kb.confluent());

      REQUIRE(kb.equal_to("abchd", "abcdf"));
      REQUIRE(!kb.equal_to("abchf", "abcdf"));
      REQUIRE(kb.equal_to("abchd", "abchd"));
      REQUIRE(kb.equal_to("abchdf", "abchhd"));
      // Test cases (4) and (5)
      REQUIRE(kb.equal_to("abchd", "cef"));
      REQUIRE(kb.equal_to("cef", "abchd"));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "034: (fpsemi) from GAP smalloverlap gap/test.gi",
        "63 "
        "(infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcdefgh");

      kb.add_rule("afh", "bgh");
      kb.add_rule("hc", "d");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      // Test case (6)
      REQUIRE(kb.equal_to("afd", "bgd"));

      kb.run();
      REQUIRE(kb.nr_active_rules() == 3);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "035: (fpsemi) from GAP smalloverlap gap/test.gi",
        "70 "
        "(infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto rg = ReportGuard(REPORT);
      // The following permits a more complex test of case (6), which also
      // involves using the case (2) code to change the prefix being looked for:
      KnuthBendix kb;
      kb.set_alphabet("abcdefghij");

      kb.add_rule("afh", "bgh");
      kb.add_rule("hc", "de");
      kb.add_rule("ei", "j");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      REQUIRE(kb.equal_to("afdj", "bgdj"));
      REQUIRE_THROWS_AS(!kb.equal_to("xxxxxxxxxxxxxxxxxxxxxxx", "b"),
                        LibsemigroupsException);

      kb.run();
      REQUIRE(kb.nr_active_rules() == 5);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "036: (fpsemi) from GAP smalloverlap gap/test.gi",
        "77 "
        "(infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto rg = ReportGuard(REPORT);
      // A slightly more complicated presentation for testing case (6), in which
      // the max piece suffixes of the first two relation words no longer agree
      // (since fh and gh are now pieces).
      KnuthBendix kb;
      kb.set_alphabet("abcdefghijkl");

      kb.add_rule("afh", "bgh");
      kb.add_rule("hc", "de");
      kb.add_rule("ei", "j");
      kb.add_rule("fhk", "ghl");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      REQUIRE(kb.equal_to("afdj", "bgdj"));

      kb.run();
      REQUIRE(kb.nr_active_rules() == 7);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "037",
        "(fpsemi) from GAP smalloverlap gap/test.gi:85 (infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("cab");  // runs forever with a different order

      kb.add_rule("aabc", "acba");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(kb.confluent());  // Confirmed with GAP

      REQUIRE(!kb.equal_to("a", "b"));
      REQUIRE(kb.equal_to("aabcabc", "aabccba"));

      kb.run();
      REQUIRE(kb.nr_active_rules() == 1);
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.active_rules()
              == std::vector<std::pair<std::string, std::string>>(
                  {{"aabc", "acba"}}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "038",
        "(fpsemi) Von Dyck (2,3,7) group (infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap][kbmag]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("ABabc");

      kb.add_rule("aaaa", "AAA");
      kb.add_rule("bb", "B");
      kb.add_rule("BA", "c");

      REQUIRE(!kb.confluent());
      kb.run();

      REQUIRE(kb.nr_active_rules() == 6);
      REQUIRE(kb.confluent());
      REQUIRE(!kb.equal_to("a", "b"));
      REQUIRE(!kb.equal_to("aabcabc", "aabccba"));
    }

    // Does not finish knuth_bendix
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "039",
                            "(fpsemi) Von Dyck (2,3,7) group - different "
                            "presentation (infinite)",
                            "[no-valgrind][quick][knuth-bendix][fpsemigroup]["
                            "fpsemi][smalloverlap][kbmag]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ABabc");

      kb.add_rule("aaaa", "AAA");
      kb.add_rule("bb", "B");
      kb.add_rule("abababa", "BABABAB");
      kb.add_rule("BA", "c");

      REQUIRE(!kb.confluent());
      kb.overlap_policy(KnuthBendix::policy::overlap::MAX_AB_BC);
      kb.max_rules(100);
      kb.run();
      REQUIRE(kb.nr_active_rules() == 101);
      kb.run();
      REQUIRE(kb.nr_active_rules() == 101);
      kb.max_rules(250);
      kb.run();
      REQUIRE(kb.nr_active_rules() == 255);
      // kb.max_rules(POSITIVE_INFINITY);
      // kb.run();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "040",
        "(fpsemi) rewriting system from KnuthBendixCongruenceByPairs 08",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap][kbmag]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("bbbbbbb", "b");
      kb.add_rule("ccccc", "c");
      kb.add_rule("bccba", "bccb");
      kb.add_rule("bccbc", "bccb");
      kb.add_rule("bbcbca", "bbcbc");
      kb.add_rule("bbcbcb", "bbcbc");

      REQUIRE(!kb.confluent());
      REQUIRE(kb.nr_active_rules() == 6);
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 8);

      REQUIRE(kb.equal_to("bbbbbbb", "b"));
      REQUIRE(kb.equal_to("ccccc", "c"));
      REQUIRE(kb.equal_to("bccba", "bccb"));
      REQUIRE(kb.equal_to("bccbc", "bccb"));
      REQUIRE(kb.equal_to("bcbca", "bcbc"));
      REQUIRE(kb.equal_to("bcbcb", "bcbc"));
      REQUIRE(kb.equal_to("bcbcc", "bcbc"));
      REQUIRE(kb.equal_to("bccbb", "bccb"));
      REQUIRE(kb.equal_to("bccb", "bccbb"));
      REQUIRE(!kb.equal_to("aaaa", "bccbb"));

      using rule_type = std::pair<std::string, std::string>;

      std::vector<rule_type> rules = kb.active_rules();
      REQUIRE(rules[0] == rule_type("bcbca", "bcbc"));
      REQUIRE(rules[1] == rule_type("bcbcb", "bcbc"));
      REQUIRE(rules[2] == rule_type("bcbcc", "bcbc"));
      REQUIRE(rules[3] == rule_type("bccba", "bccb"));
      REQUIRE(rules[4] == rule_type("bccbb", "bccb"));
      REQUIRE(rules[5] == rule_type("bccbc", "bccb"));
      REQUIRE(rules[6] == rule_type("ccccc", "c"));
      REQUIRE(rules[7] == rule_type("bbbbbbb", "b"));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "041",
                            "(fpsemi) rewriting system from Congruence 20",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("ab", "ba");
      kb.add_rule("aa", "a");
      kb.run();

      REQUIRE(kb.equal_to("abbbbbbbbbbbbbb", "aabbbbbbbbbbbbbb"));
    }

    // The next test meets the definition of a standard test but causes valgrind
    // on travis to timeout.
    // Takes approx. 2.6s
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "042",
                            "(fpsemi) Example 6.6 in Sims (with limited "
                            "overlap lengths)",
                            "[standard][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabacabacabacabacabac", "");

      REQUIRE(!kb.confluent());

      // In Sims it says to use 44 here, but that doesn't seem to work.
      kb.max_overlap(45);
      // Avoid checking confluence since this is very slow, essentially takes
      // the same amount of time as running Knuth-Bendix (fpsemi) 13.
      kb.check_confluence_interval(LIMIT_MAX);

      kb.run();
      REQUIRE(kb.nr_active_rules() == 1026);
    }

    // This example verifies the nilpotence of the group using the Sims
    // algorithm. The original presentation was <a,b| [b,a,a,a], [b^-1,a,a,a],
    // [a,b,b,b], [a^-1,b,b,b], [a,a*b,a*b,a*b], [a^-1,a*b,a*b,a*b] >. (where []
    // mean left-normed commutators. The presentation here was derived by first
    // applying the NQA to find the maximal nilpotent quotient, and then
    // introducing new generators for the PCP generators.
    // LIBSEMIGROUPS_TEST_CASE(
    //     "KnuthBendix",
    //     "043",
    //     "(fpsemi) (from kbmag/standalone/kb_data/heinnilp)",
    //     "[fail][knuth-bendix][fpsemigroup][fpsemi][kbmag][recursive]") {
    //   // TODO(later) fails because internal_rewrite expect rules to be length
    //   reducing KnuthBendix kb(new RECURSIVE(), "fFyYdDcCbBaA");
    //   kb.add_rule("BAba", "c");
    //   kb.add_rule("CAca", "d");
    //   kb.add_rule("CBcb", "y");
    //   kb.add_rule("DBdb", "f");
    //   kb.add_rule("cBCb", "bcBC");
    //   kb.add_rule("babABaBA", "abABaBAb");
    //   kb.add_rule("cBACab", "abcBAC");
    //   kb.add_rule("BabABBAbab", "aabABBAb");
    //   auto rg = ReportGuard(REPORT);

    //   REQUIRE(!kb.confluent());

    //   kb.run();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.nr_active_rules() == 32767);
    //  }

    // Fibonacci group F(2,7) - without inverses
    // Takes approx. 36s
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "044",
        "(fpsemi) (from kbmag/standalone/kb_data/f27) (infinite) (1 / 2)",
        "[extreme][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("aAbBcCdDyYfFgG");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "y");
      kb.add_rule("dy", "f");
      kb.add_rule("yf", "g");
      kb.add_rule("fg", "a");
      kb.add_rule("ga", "b");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      // Fails to terminate, or is very slow, with knuth_bendix
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 47);
      REQUIRE(kb.size() == POSITIVE_INFINITY);
    }

    // An extension of 2^6 be L32
    // Takes approx. 1m33s
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "045",
        "(fpsemi) (from kbmag/standalone/kb_data/l32ext)",
        "[extreme][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("abB");
      kb.set_identity("");
      kb.set_inverses("aBb");

      kb.add_rule("aa", "");
      kb.add_rule("BB", "b");
      kb.add_rule("BaBaBaB", "abababa");
      kb.add_rule("aBabaBabaBabaBab", "BabaBabaBabaBaba");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 1026);
      REQUIRE(kb.size() == 10752);
    }

    // 2-generator free abelian group (with this ordering KB terminates - but no
    // all)
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "046",
        "(fpsemi) (from kbmag/standalone/kb_data/ab2)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aAbB");

      kb.add_rule("Bab", "a");

      REQUIRE(kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 1);

      REQUIRE(kb.equal_to("Bab", "a"));
    }

    // This group is actually D_22 (although it wasn't meant to be). All
    // generators are unexpectedly involutory.
    // FIXME what? The semigroup below is infinite, should add inverses and
    // identity

    // knuth_bendix does not terminate with the given ordering, terminates
    // almost immediately with the standard order.
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "047",
        "(fpsemi) (from kbmag/standalone/kb_data/d22) (1 / 3)"
        "(infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      // KnuthBendix kb;
      // kb.set_alphabet("aAbBcCdDyYfF");

      KnuthBendix kb;
      kb.set_alphabet("ABCDYFabcdyf");

      kb.add_rule("aCAd", "");
      kb.add_rule("bfBY", "");
      kb.add_rule("cyCD", "");
      kb.add_rule("dFDa", "");
      kb.add_rule("ybYA", "");
      kb.add_rule("fCFB", "");
      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 8);

      REQUIRE(kb.equal_to("bfBY", ""));
      REQUIRE(kb.equal_to("cyCD", ""));
      REQUIRE(kb.equal_to("ybYA", ""));
      REQUIRE(kb.equal_to("fCFB", ""));
      REQUIRE(kb.equal_to("CAd", "dFD"));
      REQUIRE(kb.equal_to("FDa", "aCA"));
      REQUIRE(kb.equal_to("adFD", ""));
      REQUIRE(kb.equal_to("daCA", ""));

      REQUIRE(
          kb.active_rules()
          == std::vector<std::pair<std::string, std::string>>({{"aCA", "FDa"},
                                                               {"dFD", "CAd"},
                                                               {"CAda", ""},
                                                               {"FDad", ""},
                                                               {"bfBY", ""},
                                                               {"cyCD", ""},
                                                               {"fCFB", ""},
                                                               {"ybYA", ""}}));
      // Verified with GAP

      REQUIRE(kb.size() == POSITIVE_INFINITY);
    }

    // No generators - no anything!
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "048",
        "(fpsemi) (from kbmag/standalone/kb_data/degen1)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;

      REQUIRE(kb.confluent());
      REQUIRE_THROWS_AS(kb.run(), LibsemigroupsException);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 0);
    }

    // Symmetric group S_4
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "049",
        "(fpsemi) (from kbmag/standalone/kb_data/s4)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abB");
      kb.set_identity("");
      kb.set_inverses("aBb");

      kb.add_rule("bb", "B");
      kb.add_rule("BaBa", "abab");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 11);
      REQUIRE(kb.size() == 24);
    }

    // This example verifies the nilpotence of the group using the Sims
    // algorithm. The original presentation was <a,b| [b,a,b], [b,a,a,a,a],
    // [b,a,a,a,b,a,a] >. (where [] mean left-normed commutators). The
    // presentation here was derived by first applying the NQA to find the
    // maximal nilpotent quotient, and then introducing new generators for the
    // PCP generators. It is essential for success that reasonably low values of
    // the maxstoredlen parameter are given.
    // LIBSEMIGROUPS_TEST_CASE(
    //     "KnuthBendix",
    //     "050",
    //     "(fpsemi) (from kbmag/standalone/kb_data/verifynilp)",
    //     "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][recursive]") {}
    //   KnuthBendix kb(new RECURSIVE(), "hHgGfFyYdDcCbBaA");
    //   kb.add_rule("BAba", "c");
    //   kb.add_rule("CAca", "d");
    //   kb.add_rule("DAda", "y");
    //   kb.add_rule("YByb", "f");
    //   kb.add_rule("FAfa", "g");
    //   kb.add_rule("ga", "ag");
    //   kb.add_rule("GBgb", "h");
    //   kb.add_rule("cb", "bc");
    //   kb.add_rule("ya", "ay");
    //   auto rg = ReportGuard(REPORT);
    //
    //   REQUIRE(kb.confluent());
    //
    //   kb.run();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.nr_active_rules() == 9);
    //
    //   REQUIRE(kb.equal_to("BAba", "c"));
    //   REQUIRE(kb.equal_to("CAca", "d"));
    //   REQUIRE(kb.equal_to("DAda", "y"));
    //   REQUIRE(kb.equal_to("YByb", "f"));
    //   REQUIRE(kb.equal_to("FAfa", "g"));
    //   REQUIRE(kb.equal_to("ga", "ag"));
    //   REQUIRE(kb.equal_to("GBgb", "h"));
    //   REQUIRE(kb.equal_to("cb", "bc"));
    //   REQUIRE(kb.equal_to("ya", "ay"));
    // REQUIRE(kb.active_rules() == std::vector<std::pair<std::string,
    // std::string>>({}));
    // }

    // Fibonacci group F(2,5) - monoid presentation - has order 12 (group
    // elements + empty word)
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "051",
                            "(from kbmag/standalone/kb_data/f25monoid)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abcde");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "e");
      kb.add_rule("de", "a");
      kb.add_rule("ea", "b");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 24);

      REQUIRE(kb.equal_to("ab", "c"));
      REQUIRE(kb.equal_to("bc", "d"));
      REQUIRE(kb.equal_to("cd", "e"));
      REQUIRE(kb.equal_to("de", "a"));
      REQUIRE(kb.equal_to("ea", "b"));
      REQUIRE(kb.equal_to("cc", "ad"));
      REQUIRE(kb.equal_to("dd", "be"));
      REQUIRE(kb.equal_to("ee", "ca"));
      REQUIRE(kb.equal_to("ec", "bb"));
      REQUIRE(kb.equal_to("db", "aa"));
      REQUIRE(kb.equal_to("aac", "be"));
      REQUIRE(kb.equal_to("bd", "aa"));
      REQUIRE(kb.equal_to("bbe", "aad"));
      REQUIRE(kb.equal_to("aaa", "e"));
      REQUIRE(kb.equal_to("eb", "be"));
      REQUIRE(kb.equal_to("ba", "c"));
      REQUIRE(kb.equal_to("da", "ad"));
      REQUIRE(kb.equal_to("ca", "ac"));
      REQUIRE(kb.equal_to("ce", "bb"));
      REQUIRE(kb.equal_to("cb", "d"));
      REQUIRE(kb.equal_to("ed", "a"));
      REQUIRE(kb.equal_to("dc", "e"));
      REQUIRE(kb.equal_to("ae", "b"));
      REQUIRE(kb.equal_to("bbb", "a"));
      REQUIRE(
          kb.active_rules()
          == std::vector<std::pair<std::string, std::string>>(
              {{"ab", "c"},  {"ae", "b"},   {"ba", "c"},   {"bc", "d"},
               {"bd", "aa"}, {"ca", "ac"},  {"cb", "d"},   {"cc", "ad"},
               {"cd", "e"},  {"ce", "bb"},  {"da", "ad"},  {"db", "aa"},
               {"dc", "e"},  {"dd", "be"},  {"de", "a"},   {"ea", "b"},
               {"eb", "be"}, {"ec", "bb"},  {"ed", "a"},   {"ee", "ca"},
               {"aaa", "e"}, {"aac", "be"}, {"bbb", "ed"}, {"bbe", "aad"}}));
    }

    // trivial group - BHN presentation
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "052",
                            "(from kbmag/standalone/kb_data/degen4a)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbBcC");

      kb.add_rule("Aba", "bb");
      kb.add_rule("Bcb", "cc");
      kb.add_rule("Cac", "aa");

      REQUIRE(kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 3);

      REQUIRE(kb.equal_to("Aba", "bb"));
      REQUIRE(kb.equal_to("Bcb", "cc"));
      REQUIRE(kb.equal_to("Cac", "aa"));
      REQUIRE(kb.active_rules()
              == std::vector<std::pair<std::string, std::string>>(
                  {{"Aba", "bb"}, {"Bcb", "cc"}, {"Cac", "aa"}}));
    }

    // Torus group
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "053",
                            "(from kbmag/standalone/kb_data/torus)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAcCbBdD");

      kb.add_rule("ABab", "DCdc");

      REQUIRE(kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 1);

      REQUIRE(kb.equal_to("DCdc", "ABab"));
      REQUIRE(kb.active_rules()
              == std::vector<std::pair<std::string, std::string>>(
                  {{"DCdc", "ABab"}}));
    }

    // monoid presentation of F(2,7) - should produce a monoid of length 30
    // which is the same as the group, together with the empty word. This is a
    // very difficult calculation indeed, however.
    //
    // KBMAG does not terminate when SHORTLEX order is used.
    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "054",
    //                         "(from kbmag/standalone/kb_data/f27monoid)",
    //                         "[fail][knuth-bendix][kbmag][recursive]") {
    //   KnuthBendix kb(new RECURSIVE(), "abcdefg");
    //   kb.add_rule("ab", "c");
    //   kb.add_rule("bc", "d");
    //   kb.add_rule("cd", "e");
    //   kb.add_rule("de", "f");
    //   kb.add_rule("ef", "g");
    //   kb.add_rule("fg", "a");
    //   kb.add_rule("ga", "b");
    //   auto rg = ReportGuard(REPORT);

    //   REQUIRE(!kb.confluent());

    //   kb.run();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.nr_active_rules() == 32767);
    // }

    //  3-fold cover of A_6
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "055",
                            "(from kbmag/standalone/kb_data/3a6)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abAB");

      kb.add_rule("aaa", "");
      kb.add_rule("bbb", "");
      kb.add_rule("abababab", "");
      kb.add_rule("aBaBaBaBaB", "");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 11);

      REQUIRE(kb.equal_to("aaa", ""));
      REQUIRE(kb.equal_to("bbb", ""));
      REQUIRE(kb.equal_to("BaBaBaBaB", "aa"));
      REQUIRE(kb.equal_to("bababa", "aabb"));
      REQUIRE(kb.equal_to("ababab", "bbaa"));
      REQUIRE(kb.equal_to("aabbaa", "babab"));
      REQUIRE(kb.equal_to("bbaabb", "ababa"));
      REQUIRE(kb.equal_to("bababbabab", "aabbabbaa"));
      REQUIRE(kb.equal_to("ababaababa", "bbaabaabb"));
      REQUIRE(kb.equal_to("bababbabaababa", "aabbabbaabaabb"));
      REQUIRE(kb.equal_to("bbaabaabbabbaa", "ababaababbabab"));
      REQUIRE(kb.active_rules()
              == std::vector<std::pair<std::string, std::string>>(
                  {{"aaa", ""},
                   {"bbb", ""},
                   {"aabbaa", "babab"},
                   {"ababab", "bbaa"},
                   {"bababa", "aabb"},
                   {"bbaabb", "ababa"},
                   {"BaBaBaBaB", "aa"},
                   {"ababaababa", "bbaabaabb"},
                   {"bababbabab", "aabbabbaa"},
                   {"bababbabaababa", "aabbabbaabaabb"},
                   {"bbaabaabbabbaa", "ababaababbabab"}}));
    }

    //  Free group on 2 generators
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "056",
                            "(from kbmag/standalone/kb_data/f2)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbB");
      // FIXME this isn't the free group

      REQUIRE(kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 0);
    }

    //  A nonhopfian group
    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "057",
    //                         "(from kbmag/standalone/kb_data/nonhopf)",
    //                         "[quick][knuth-bendix][kbmag][recursive]") {
    //   KnuthBendix kb(new RECURSIVE(), "aAbB");
    //   kb.add_rule("Baab", "aaa");
    //   auto rg = ReportGuard(REPORT);

    //   REQUIRE(kb.confluent());

    //   kb.run();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.nr_active_rules() == 1);

    //   REQUIRE(kb.equal_to("Baab", "aaa"));
    //   REQUIRE(kb.active_rules()
    //           == std::vector<std::pair<std::string, std::string>>({}));
    // }

    // Symmetric group S_16
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "058",
        "(from kbmag/standalone/kb_data/s16)",
        "[quick][knuth-bendix][kbmag][shortlex][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcdefghijklmno");
      kb.set_identity("");
      kb.set_inverses("abcdefghijklmno");

      kb.add_rule("bab", "aba");
      kb.add_rule("ca", "ac");
      kb.add_rule("da", "ad");
      kb.add_rule("ea", "ae");
      kb.add_rule("fa", "af");
      kb.add_rule("ga", "ag");
      kb.add_rule("ha", "ah");
      kb.add_rule("ia", "ai");
      kb.add_rule("ja", "aj");
      kb.add_rule("ka", "ak");
      kb.add_rule("la", "al");
      kb.add_rule("ma", "am");
      kb.add_rule("na", "an");
      kb.add_rule("oa", "ao");
      kb.add_rule("cbc", "bcb");
      kb.add_rule("db", "bd");
      kb.add_rule("eb", "be");
      kb.add_rule("fb", "bf");
      kb.add_rule("gb", "bg");
      kb.add_rule("hb", "bh");
      kb.add_rule("ib", "bi");
      kb.add_rule("jb", "bj");
      kb.add_rule("kb", "bk");
      kb.add_rule("lb", "bl");
      kb.add_rule("mb", "bm");
      kb.add_rule("nb", "bn");
      kb.add_rule("ob", "bo");
      kb.add_rule("dcd", "cdc");
      kb.add_rule("ec", "ce");
      kb.add_rule("fc", "cf");
      kb.add_rule("gc", "cg");
      kb.add_rule("hc", "ch");
      kb.add_rule("ic", "ci");
      kb.add_rule("jc", "cj");
      kb.add_rule("kc", "ck");
      kb.add_rule("lc", "cl");
      kb.add_rule("mc", "cm");
      kb.add_rule("nc", "cn");
      kb.add_rule("oc", "co");
      kb.add_rule("ede", "ded");
      kb.add_rule("fd", "df");
      kb.add_rule("gd", "dg");
      kb.add_rule("hd", "dh");
      kb.add_rule("id", "di");
      kb.add_rule("jd", "dj");
      kb.add_rule("kd", "dk");
      kb.add_rule("ld", "dl");
      kb.add_rule("md", "dm");
      kb.add_rule("nd", "dn");
      kb.add_rule("od", "do");
      kb.add_rule("fef", "efe");
      kb.add_rule("ge", "eg");
      kb.add_rule("he", "eh");
      kb.add_rule("ie", "ei");
      kb.add_rule("je", "ej");
      kb.add_rule("ke", "ek");
      kb.add_rule("le", "el");
      kb.add_rule("me", "em");
      kb.add_rule("ne", "en");
      kb.add_rule("oe", "eo");
      kb.add_rule("gfg", "fgf");
      kb.add_rule("hf", "fh");
      kb.add_rule("if", "fi");
      kb.add_rule("jf", "fj");
      kb.add_rule("kf", "fk");
      kb.add_rule("lf", "fl");
      kb.add_rule("mf", "fm");
      kb.add_rule("nf", "fn");
      kb.add_rule("of", "fo");
      kb.add_rule("hgh", "ghg");
      kb.add_rule("ig", "gi");
      kb.add_rule("jg", "gj");
      kb.add_rule("kg", "gk");
      kb.add_rule("lg", "gl");
      kb.add_rule("mg", "gm");
      kb.add_rule("ng", "gn");
      kb.add_rule("og", "go");
      kb.add_rule("ihi", "hih");
      kb.add_rule("jh", "hj");
      kb.add_rule("kh", "hk");
      kb.add_rule("lh", "hl");
      kb.add_rule("mh", "hm");
      kb.add_rule("nh", "hn");
      kb.add_rule("oh", "ho");
      kb.add_rule("jij", "iji");
      kb.add_rule("ki", "ik");
      kb.add_rule("li", "il");
      kb.add_rule("mi", "im");
      kb.add_rule("ni", "in");
      kb.add_rule("oi", "io");
      kb.add_rule("kjk", "jkj");
      kb.add_rule("lj", "jl");
      kb.add_rule("mj", "jm");
      kb.add_rule("nj", "jn");
      kb.add_rule("oj", "jo");
      kb.add_rule("lkl", "klk");
      kb.add_rule("mk", "km");
      kb.add_rule("nk", "kn");
      kb.add_rule("ok", "ko");
      kb.add_rule("mlm", "lml");
      kb.add_rule("nl", "ln");
      kb.add_rule("ol", "lo");
      kb.add_rule("nmn", "mnm");
      kb.add_rule("om", "mo");
      kb.add_rule("ono", "non");

      REQUIRE(!kb.confluent());

      // kb.knuth_bendix_by_overlap_length();
      kb.run();  // faster
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 211);
      // REQUIRE(kb.size() == 20922789888000);
      // TODO(later) uncomment this line, currently this uses the Froidure-Pin
      // algorithm which will use too much memory.
    }

    // Presentation of group A_4 regarded as monoid presentation - gives
    // infinite monoid.
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "059",
                            "(from kbmag/standalone/kb_data/a4monoid)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abB");

      kb.add_rule("bb", "B");
      kb.add_rule("BaB", "aba");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 6);

      REQUIRE(kb.equal_to("bb", "B"));
      REQUIRE(kb.equal_to("BaB", "aba"));
      REQUIRE(kb.equal_to("Bb", "bB"));
      REQUIRE(kb.equal_to("Baaba", "abaaB"));
      REQUIRE(kb.equal_to("BabB", "abab"));
      REQUIRE(kb.equal_to("Bababa", "ababaB"));
      REQUIRE(kb.active_rules()
              == std::vector<std::pair<std::string, std::string>>(
                  {{{"Bb", "bB"},
                    {"bb", "B"},
                    {"BaB", "aba"},
                    {"BabB", "abab"},
                    {"Baaba", "abaaB"},
                    {"Bababa", "ababaB"}}}));
    }

    // fairly clearly the trivial group
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "060",
                            "(from kbmag/standalone/kb_data/degen3)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbB");
      kb.add_rule("ab", "");
      kb.add_rule("abb", "");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 2);

      REQUIRE(kb.equal_to("b", ""));
      REQUIRE(kb.equal_to("a", ""));
      REQUIRE(kb.active_rules()
              == std::vector<std::pair<std::string, std::string>>(
                  {{"a", ""}, {"b", ""}}));
    }

    // Symmetric group S_9
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "061",
                            "(from kbmag/standalone/kb_data/s9)",
                            "[standard][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abcdefgh");
      kb.set_identity("");
      kb.set_inverses("abcdefgh");

      kb.add_rule("bab", "aba");
      kb.add_rule("ca", "ac");
      kb.add_rule("da", "ad");
      kb.add_rule("ea", "ae");
      kb.add_rule("fa", "af");
      kb.add_rule("ga", "ag");
      kb.add_rule("ha", "ah");
      kb.add_rule("cbc", "bcb");
      kb.add_rule("db", "bd");
      kb.add_rule("eb", "be");
      kb.add_rule("fb", "bf");
      kb.add_rule("gb", "bg");
      kb.add_rule("hb", "bh");
      kb.add_rule("dcd", "cdc");
      kb.add_rule("ec", "ce");
      kb.add_rule("fc", "cf");
      kb.add_rule("gc", "cg");
      kb.add_rule("hc", "ch");
      kb.add_rule("ede", "ded");
      kb.add_rule("fd", "df");
      kb.add_rule("gd", "dg");
      kb.add_rule("hd", "dh");
      kb.add_rule("fef", "efe");
      kb.add_rule("ge", "eg");
      kb.add_rule("he", "eh");
      kb.add_rule("gfg", "fgf");
      kb.add_rule("hf", "fh");
      kb.add_rule("hgh", "ghg");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 57);
      REQUIRE(kb.size() == 362880);
    }

    // infinite cyclic group
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "062",
                            "(from kbmag/standalone/kb_data/ab1)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aA");
      // FIXME not a group!

      REQUIRE(kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 0);
    }

    // A generator, but trivial.
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "063",
                            "(from kbmag/standalone/kb_data/degen2)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aA");
      kb.add_rule("a", "");

      REQUIRE(kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 1);

      REQUIRE(kb.equal_to("a", ""));
      REQUIRE(kb.active_rules()
              == std::vector<std::pair<std::string, std::string>>({{"a", ""}}));
    }

    // Fibonacci group F(2,5)
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "064",
                            "(from kbmag/standalone/kb_data/f25)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbBcCdDyY");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "y");
      kb.add_rule("dy", "a");
      kb.add_rule("ya", "b");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 24);

      REQUIRE(kb.equal_to("ab", "c"));
      REQUIRE(kb.equal_to("bc", "d"));
      REQUIRE(kb.equal_to("cd", "y"));
      REQUIRE(kb.equal_to("dy", "a"));
      REQUIRE(kb.equal_to("ya", "b"));
      REQUIRE(kb.equal_to("cc", "ad"));
      REQUIRE(kb.equal_to("dd", "by"));
      REQUIRE(kb.equal_to("yy", "ac"));
      REQUIRE(kb.equal_to("yc", "bb"));
      REQUIRE(kb.equal_to("db", "aa"));
      REQUIRE(kb.equal_to("aac", "by"));
      REQUIRE(kb.equal_to("bd", "aa"));
      REQUIRE(kb.equal_to("bby", "aad"));
      REQUIRE(kb.equal_to("aaa", "y"));
      REQUIRE(kb.equal_to("yb", "by"));
      REQUIRE(kb.equal_to("ba", "c"));
      REQUIRE(kb.equal_to("da", "ad"));
      REQUIRE(kb.equal_to("ca", "ac"));
      REQUIRE(kb.equal_to("cy", "bb"));
      REQUIRE(kb.equal_to("cb", "d"));
      REQUIRE(kb.equal_to("yd", "a"));
      REQUIRE(kb.equal_to("dc", "y"));
      REQUIRE(kb.equal_to("ay", "b"));
      REQUIRE(kb.equal_to("bbb", "a"));
      REQUIRE(
          kb.active_rules()
          == std::vector<std::pair<std::string, std::string>>(
              {{"ab", "c"},  {"ay", "b"},   {"ba", "c"},   {"bc", "d"},
               {"bd", "aa"}, {"ca", "ac"},  {"cb", "d"},   {"cc", "ad"},
               {"cd", "y"},  {"cy", "bb"},  {"da", "ad"},  {"db", "aa"},
               {"dc", "y"},  {"dd", "by"},  {"dy", "a"},   {"ya", "b"},
               {"yb", "by"}, {"yc", "bb"},  {"yd", "a"},   {"yy", "ca"},
               {"aaa", "y"}, {"aac", "by"}, {"bbb", "yd"}, {"bby", "aad"}}));
    }

    // Second of BHN's series of increasingly complicated presentations of 1.
    // Works quickest with large value of tidyint
    // Takes > 1m (knuth_bendix), didn't run to the end
    // Takes approx. 26s (knuth_bendix_by_overlap_length)
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "065",
                            "(from kbmag/standalone/kb_data/degen4b)",
                            "[extreme][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("aAbBcC");
      kb.set_identity("");
      kb.set_inverses("AaBbCc");

      kb.add_rule("bbABaBcbCCAbaBBccBCbccBCb", "");
      kb.add_rule("ccBCbCacAABcbCCaaCAcaaCAc", "");
      kb.add_rule("aaCAcAbaBBCacAAbbABabbABa", "");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 6);
      REQUIRE(kb.size() == 1);
    }

    // Free nilpotent group of rank 2 and class 2
    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "066",
    //                         "(from kbmag/standalone/kb_data/nilp2)",
    //                         "[quick][knuth-bendix][kbmag][recursive]") {
    //   KnuthBendix kb(new RECURSIVE(), "cCbBaA");
    //   kb.add_rule("ba", "abc");
    //   kb.add_rule("ca", "ac");
    //   kb.add_rule("cb", "bc");
    //   auto rg = ReportGuard(REPORT);
    //
    //   REQUIRE(kb.confluent());
    //
    //   kb.run();
    //   REQUIRE(kb.confluent());
    //
    //   REQUIRE(kb.nr_active_rules() == 3);
    // }

    // Takes approx. 2s
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "067",
                            "(from kbmag/standalone/kb_data/funny3)",
                            "[standard][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aAbBcC");
      kb.set_identity("");
      kb.set_inverses("AaBbCc");

      kb.add_rule("aaa", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ccc", "");
      kb.add_rule("ABa", "BaB");
      kb.add_rule("bcB", "cBc");
      kb.add_rule("caC", "aCa");
      kb.add_rule("abcABCabcABCabcABC", "");
      kb.add_rule("BcabCABcabCABcabCA", "");
      kb.add_rule("cbACBacbACBacbACBa", "");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      // kb.run() // also works, but is slower
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 8);
      REQUIRE(kb.size() == 3);
    }

    // Two generator presentation of Fibonacci group F(2,7) - order 29. Large
    // value of tidyint works better.
    // Takes approx. 19s (knuth_bendix_by_overlap_length)
    // Takes > 19s (knuth_bendix), didn't run to the end
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "068",
                            "(from kbmag/standalone/kb_data/f27_2gen)",
                            "[extreme][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("aAbB");
      kb.set_identity("");
      kb.set_inverses("AaBb");

      kb.add_rule("bababbababbabbababbab", "a");
      kb.add_rule("abbabbababbaba", "b");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 19);
      REQUIRE(kb.size() == 29);
    }

    // Mathieu group M_11
    // Takes approx. 2m9s (majority in checking confluence)
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "069",
                            "(from kbmag/standalone/kb_data/m11)",
                            "[extreme][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("abB");
      kb.set_identity("");
      kb.set_inverses("aBb");

      kb.add_rule("BB", "bb");
      kb.add_rule("BaBaBaBaBaB", "abababababa");
      kb.add_rule("bbabbabba", "abbabbabb");
      kb.add_rule("aBaBababaBabaBBaBab", "");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 1731);
      REQUIRE(kb.size() == 7920);
    }

    // Weyl group E8 (all gens involutory).
    // Takes approx. 8s
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "070",
                            "(from kbmag/standalone/kb_data/e8)",
                            "[extreme][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("abcdefgh");
      kb.set_identity("");
      kb.set_inverses("abcdefgh");
      kb.add_rule("bab", "aba");
      kb.add_rule("ca", "ac");
      kb.add_rule("da", "ad");
      kb.add_rule("ea", "ae");
      kb.add_rule("fa", "af");
      kb.add_rule("ga", "ag");
      kb.add_rule("ha", "ah");
      kb.add_rule("cbc", "bcb");
      kb.add_rule("db", "bd");
      kb.add_rule("eb", "be");
      kb.add_rule("fb", "bf");
      kb.add_rule("gb", "bg");
      kb.add_rule("hb", "bh");
      kb.add_rule("dcd", "cdc");
      kb.add_rule("ece", "cec");
      kb.add_rule("fc", "cf");
      kb.add_rule("gc", "cg");
      kb.add_rule("hc", "ch");
      kb.add_rule("ed", "de");
      kb.add_rule("fd", "df");
      kb.add_rule("gd", "dg");
      kb.add_rule("hd", "dh");
      kb.add_rule("fef", "efe");
      kb.add_rule("ge", "eg");
      kb.add_rule("he", "eh");
      kb.add_rule("gfg", "fgf");
      kb.add_rule("hf", "fh");
      kb.add_rule("hgh", "ghg");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 192);
      // REQUIRE(kb.size() == 696729600);
      // TODO(later) uncomment this line, currently this uses the Froidure-Pin
      // algorithm which will use too much memory.
    }

    // Von Dyck (2,3,7) group - infinite hyperbolic - small tidyint works better
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "071",
                            "(from kbmag/standalone/kb_data/237)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aAbBc");
      kb.set_identity("");
      kb.set_inverses("AaBbc");

      kb.add_rule("aaaa", "AAA");
      kb.add_rule("bb", "B");
      kb.add_rule("BA", "c");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 32);
      REQUIRE(kb.active_rules()
              == std::vector<std::pair<std::string, std::string>>(
                  {{"Aa", ""},
                   {"Ac", "b"},
                   {"BA", "c"},
                   {"BB", "b"},
                   {"Bb", ""},
                   {"Bc", "bA"},
                   {"aA", ""},
                   {"ab", "c"},
                   {"bB", ""},
                   {"ba", "AB"},
                   {"bb", "B"},
                   {"bc", "A"},
                   {"cB", "a"},
                   {"ca", "B"},
                   {"cb", "aB"},
                   {"cc", ""},
                   {"BaB", "bAb"},
                   {"bAB", "Ba"},
                   {"cAB", "aBa"},
                   {"AAAA", "aaa"},
                   {"AAAb", "aaac"},
                   {"aaaa", "AAA"},
                   {"bAbA", "Bac"},
                   {"cAAA", "Baaa"},
                   {"cAbA", "aBac"},
                   {"ABaaa", "bAAA"},
                   {"Baaac", "cAAb"},
                   {"bAABaac", "BacAAb"},
                   {"cAABaac", "aBacAAb"},
                   {"BaaaBaaa", "cAAbAAA"},
                   {"bAABaaBaaa", "BacAAbAAA"},
                   {"cAABaaBaaa", "aBacAAbAAA"}}));
    }

    // Cyclic group of order 2.
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "072",
                            "(from kbmag/standalone/kb_data/c2)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("a");
      kb.add_rule("aa", "");

      REQUIRE(kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 1);

      REQUIRE(
          kb.active_rules()
          == std::vector<std::pair<std::string, std::string>>({{"aa", ""}}));
    }

    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "073",
    //                         "(from kbmag/standalone/kb_data/freenilpc3)",
    //                         "[quick][knuth-bendix][kbmag][recursive]") {
    //   KnuthBendix kb(new RECURSIVE(), "yYdDcCbBaA");
    //   kb.add_rule("BAba", "c");
    //   kb.add_rule("CAca", "d");
    //   kb.add_rule("CBcb", "y");
    //   kb.add_rule("da", "ad");
    //   kb.add_rule("ya", "ay");
    //   kb.add_rule("db", "bd");
    //   kb.add_rule("yb", "by");
    //   auto rg = ReportGuard(REPORT);

    //   REQUIRE(kb.confluent());

    //   kb.run();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.nr_active_rules() == 7);

    //   REQUIRE(kb.equal_to("BAba", "c"));
    //   REQUIRE(kb.equal_to("CAca", "d"));
    //   REQUIRE(kb.equal_to("CBcb", "y"));
    //   REQUIRE(kb.equal_to("da", "ad"));
    //   REQUIRE(kb.equal_to("ya", "ay"));
    //   REQUIRE(kb.equal_to("db", "bd"));
    //   REQUIRE(kb.equal_to("yb", "by"));
    //   REQUIRE(kb.active_rules()
    //           == std::vector<std::pair<std::string, std::string>>({}));
    // }

    // The group is S_4, and the subgroup H of order 4. There are 30 reduced
    // words - 24 for the group elements, and 6 for the 6 cosets Hg.
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "074",
                            "(from kbmag/standalone/kb_data/cosets)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("HaAbB");

      kb.add_rule("aaa", "");
      kb.add_rule("bbbb", "");
      kb.add_rule("abab", "");
      kb.add_rule("Hb", "H");
      kb.add_rule("HH", "H");
      kb.add_rule("aH", "H");
      kb.add_rule("bH", "H");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 14);

      REQUIRE(kb.equal_to("aaa", ""));
      REQUIRE(kb.equal_to("Hb", "H"));
      REQUIRE(kb.equal_to("HH", "H"));
      REQUIRE(kb.equal_to("aH", "H"));
      REQUIRE(kb.equal_to("bH", "H"));
      REQUIRE(kb.equal_to("bab", "aa"));
      REQUIRE(kb.equal_to("bbb", "aba"));
      REQUIRE(kb.equal_to("Hab", "Haa"));
      REQUIRE(kb.equal_to("abaab", "bbaa"));
      REQUIRE(kb.equal_to("baaba", "aabb"));
      REQUIRE(kb.equal_to("Haabb", "Haaba"));
      REQUIRE(kb.equal_to("bbaabb", "abba"));
      REQUIRE(kb.equal_to("aabbaa", "baab"));
      REQUIRE(kb.equal_to("baabba", "abbaab"));
      REQUIRE(kb.active_rules()
              == std::vector<std::pair<std::string, std::string>>(
                  {{{"HH", "H"},
                    {"Hb", "H"},
                    {"aH", "H"},
                    {"bH", "H"},
                    {"Hab", "Haa"},
                    {"aaa", ""},
                    {"bab", "aa"},
                    {"bbb", "aba"},
                    {"Haabb", "Haaba"},
                    {"abaab", "bbaa"},
                    {"baaba", "aabb"},
                    {"aabbaa", "baab"},
                    {"baabba", "abbaab"},
                    {"bbaabb", "abba"}}}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "075",
                            "Example 5.1 in Sims (KnuthBendix 09 again)",
                            "[quick][knuth-bendix][fpsemigroup]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbB");

      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");
      kb.add_rule("ba", "ab");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.nr_active_rules() == 8);
      REQUIRE(kb.confluent());
    }

    // knuth_bendix/knuth_bendix_by_overlap_length fail to terminate
    // TODO(later) implement "maxstoredlen" as in KBMAG, see
    // kbmag/standalone/kb_data/verifynilp
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "076",
        "(KnuthBendix 050 again) (from kbmag/standalone/kb_data/verifynilp)",
        "[fail][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("hHgGfFyYdDcCbBaA");
      kb.set_identity("");
      kb.set_inverses("HhGgFfYyDdCcBbAa");

      kb.add_rule("BAba", "c");
      kb.add_rule("CAca", "d");
      kb.add_rule("DAda", "y");
      kb.add_rule("YByb", "f");
      kb.add_rule("FAfa", "g");
      kb.add_rule("ga", "ag");
      kb.add_rule("GBgb", "h");
      kb.add_rule("cb", "bc");
      kb.add_rule("ya", "ay");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "077",
        "(KnuthBendix 066 again) (from kbmag/standalone/kb_data/nilp2)",
        "[quick][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("cCbBaA");
      kb.set_identity("");
      kb.set_inverses("CcBbAa");

      kb.add_rule("ba", "abc");
      kb.add_rule("ca", "ac");
      kb.add_rule("cb", "bc");

      REQUIRE(!kb.confluent());

      // The following never terminates (requires recursive order?)
      // kb.knuth_bendix_by_overlap_length();
      // REQUIRE(kb.confluent());
      // REQUIRE(kb.nr_active_rules() == 32758);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "078",
                            "Example 6.4 in Sims",
                            "[quick][knuth-bendix][fpsemigroup][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abc");
      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabac", "");

      REQUIRE(kb.nr_active_rules() == 5);
      REQUIRE(!kb.confluent());

      kb.max_rules(10);
      kb.run();
      REQUIRE(kb.nr_active_rules() == 10);
      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.nr_active_rules() == 10);
      REQUIRE(!kb.confluent());

      kb.max_rules(20);
      kb.run();
      REQUIRE(kb.nr_active_rules() == 21);
      REQUIRE(!kb.confluent());

      kb.max_rules(LIMIT_MAX);
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 40);
    }

    // Von Dyck (2,3,7) group - infinite hyperbolic
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "079",
                            "KnuthBendix 071 again",
                            "[no-valgrind][quick][knuth-bendix][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aAbBc");
      kb.set_identity("");
      kb.set_inverses("AaBbc");

      kb.add_rule("BA", "c");
      kb.add_rule("Bb", "bB");
      kb.add_rule("bb", "B");
      kb.add_rule("AAAa", "aAAA");
      kb.add_rule("aaaa", "AAA");
      kb.add_rule("BaAAA", "cAAa");
      kb.add_rule("BaaAAA", "cAAaa");
      kb.add_rule("BaAaAAA", "cAAaAa");
      kb.add_rule("BaaaAAA", "cAAaaa");
      kb.add_rule("BaAAaAAA", "cAAaAAa");
      kb.add_rule("BaAaaAAA", "cAAaAaa");
      kb.add_rule("BaaAaAAA", "cAAaaAa");
      kb.add_rule("BaAAaaAAA", "cAAaAAaa");
      kb.add_rule("BaAaAaAAA", "cAAaAaAa");
      kb.add_rule("BaAaaaAAA", "cAAaAaaa");
      kb.add_rule("BaaAAaAAA", "cAAaaAAa");
      kb.add_rule("BaaAaaAAA", "cAAaaAaa");
      kb.add_rule("BaAAaAaAAA", "cAAaAAaAa");
      kb.add_rule("BaAAaaaAAA", "cAAaAAaaa");
      kb.add_rule("BaAaAAaAAA", "cAAaAaAAa");
      kb.add_rule("BaAaAaaAAA", "cAAaAaAaa");
      kb.add_rule("BaAaaAaAAA", "cAAaAaaAa");
      kb.add_rule("BaaAAaaAAA", "cAAaaAAaa");
      kb.add_rule("BaaAaAaAAA", "cAAaaAaAa");
      kb.add_rule("BaAAaAAaAAA", "cAAaAAaAAa");
      kb.add_rule("BaAAaAaaAAA", "cAAaAAaAaa");
      kb.add_rule("BaAAaaAaAAA", "cAAaAAaaAa");
      kb.add_rule("BaAaAAaaAAA", "cAAaAaAAaa");
      kb.add_rule("BaAaAaAaAAA", "cAAaAaAaAa");
      kb.add_rule("BaAaaAAaAAA", "cAAaAaaAAa");
      kb.add_rule("BaaAAaAaAAA", "cAAaaAAaAa");
      kb.add_rule("BaaAaAAaAAA", "cAAaaAaAAa");
      kb.add_rule("BaAAaAAaaAAA", "cAAaAAaAAaa");
      kb.add_rule("BaAAaAaAaAAA", "cAAaAAaAaAa");
      kb.add_rule("BaAAaaAAaAAA", "cAAaAAaaAAa");
      kb.add_rule("BaAaAAaAaAAA", "cAAaAaAAaAa");
      kb.add_rule("BaAaAaAAaAAA", "cAAaAaAaAAa");
      kb.add_rule("BaaAAaAAaAAA", "cAAaaAAaAAa");
      kb.add_rule("BaAAaAAaAaAAA", "cAAaAAaAAaAa");
      kb.add_rule("BaAAaAaAAaAAA", "cAAaAAaAaAAa");
      kb.add_rule("BaAaAAaAAaAAA", "cAAaAaAAaAAa");
      kb.add_rule("BaAAaAAaAAaAAA", "cAAaAAaAAaAAa");

      // REQUIRE(std::vector<std::pair<std::string,
      //     std::string>>(kb.cbegin_rules(), kb.cbegin_rules() + 14) ==
      //     std::vector<std::pair<std::string, std::string>>());

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 69);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "080",
                            "Example 5.4 in Sims (KnuthBendix 11 again) "
                            "(different overlap policy)",
                            "[quick][knuth-bendix][fpsemigroup]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("Bab");
      kb.add_rule("aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");
      kb.overlap_policy(KnuthBendix::policy::overlap::AB_BC);

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.nr_active_rules() == 11);
      REQUIRE(kb.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "081",
                            "Example 5.4 in Sims (KnuthBendix 11 again) "
                            "(different overlap policy)",
                            "[quick][knuth-bendix][fpsemigroup]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("Bab");
      kb.add_rule("aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");
      kb.overlap_policy(KnuthBendix::policy::overlap::MAX_AB_BC);
      // The next line tests that we don't delete the old OverlapMeasure.
      kb.overlap_policy(KnuthBendix::policy::overlap::MAX_AB_BC);

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.nr_active_rules() == 11);
      REQUIRE(kb.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "082",
                            "operator<<",
                            "[quick][knuth-bendix]") {
      std::ostringstream os;
      KnuthBendix        kb1;
      kb1.set_alphabet("Bab");
      kb1.add_rule("aa", "");
      kb1.add_rule("bB", "");
      kb1.add_rule("bbb", "");
      kb1.add_rule("ababab", "");
      os << kb1;  // Does not do anything visible
      KnuthBendix kb2;
      kb2.set_alphabet("cbaB");
      kb2.add_rule("aa", "");
      kb2.add_rule("bB", "");
      kb2.add_rule("bbb", "");
      kb2.add_rule("ababab", "");
      os << kb2;  // Does not do anything visible
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "083",
                            "confluence_interval",
                            "[quick][knuth-bendix]") {
      KnuthBendix kb;
      kb.set_alphabet("Bab");
      kb.add_rule("aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");
      kb.check_confluence_interval(LIMIT_MAX);
      kb.check_confluence_interval(10);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "084",
                            "max_overlap",
                            "[quick][knuth-bendix]") {
      KnuthBendix kb;
      kb.set_alphabet("Bab");

      kb.add_rule("aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");
      kb.max_overlap(10);
      kb.max_overlap(-11);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "085",
                            "Ceitin's undecidable word problem example",
                            "[fail][knuth-bendix]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("abcde");
      kb.add_rule("ac", "ca");
      kb.add_rule("ad", "da");
      kb.add_rule("bc", "cb");
      kb.add_rule("bd", "db");
      kb.add_rule("eca", "ce");
      kb.add_rule("edb", "de");
      kb.add_rule("cca", "ccae");
      kb.run();  // I guess this shouldn't work, and indeed it doesn't!
    }

    // FIXME(JDM): temporarily commented out to because of change to
    // FpSemigroupInterface that forbids adding rules after started(), and
    // because the copy constructors for KnuthBendix et al. don't currently work
    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "093",
    //                         "add_rule after knuth_bendix",
    //                         "[quick][knuth-bendix][fpsemigroup]") {
    //   auto        rg = ReportGuard(REPORT);
    //   KnuthBendix kb;
    //   kb.set_alphabet("Bab");
    //   kb.add_rule("aa", "");
    //   kb.add_rule("bB", "");
    //   kb.add_rule("bbb", "");
    //   kb.add_rule("ababab", "");

    //   REQUIRE(!kb.confluent());
    //   kb.run_for(FOREVER);
    //   REQUIRE(kb.finished());
    //   // The next line tests what happens when run_for is called when
    //   finished. kb.run_for(FOREVER); REQUIRE(kb.nr_active_rules() == 11);
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.size() == 12);

    //   REQUIRE(kb.equal_to("aa", ""));
    //   REQUIRE(!kb.equal_to("a", "b"));

    //   KnuthBendix kb2(&kb);
    //   REQUIRE(kb2.nr_active_rules() == 11);
    //   kb2.add_rule("a", "b");
    //   REQUIRE(kb2.nr_rules() == 5);
    //   // Adding a rule does not change the number of active rules until
    //   *after*
    //   // kb.run() is called again.
    //   REQUIRE(kb2.nr_active_rules() == 11);

    //   using rules_type = std::vector<std::pair<std::string, std::string>>;

    //   REQUIRE(rules_type(kb2.cbegin_rules(), kb2.cend_rules())
    //           == rules_type({{"aa", ""},
    //                          {"bB", ""},
    //                          {"bbb", ""},
    //                          {"ababab", ""},
    //                          {"a", "b"}}));

    //   REQUIRE(!kb2.confluent());
    //   REQUIRE(kb2.size() == 1);
    //   REQUIRE(kb2.confluent());
    //   REQUIRE(kb2.nr_active_rules() == 3);
    //   REQUIRE(kb2.active_rules() == rules_type({{"B", ""}, {"a", ""}, {"b",
    //   "a"}}));
    // }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "096",
        "(fpsemi) (from kbmag/standalone/kb_data/d22) (2 / 3) (finite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("ABCDYFabcdyf");
      kb.set_identity("");
      kb.set_inverses("abcdyfABCDYF");

      kb.add_rule("aCAd", "");
      kb.add_rule("bfBY", "");
      kb.add_rule("cyCD", "");
      kb.add_rule("dFDa", "");
      kb.add_rule("ybYA", "");
      kb.add_rule("fCFB", "");
      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 30);
      REQUIRE(kb.size() == 22);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "097",
        "(fpsemi) (from kbmag/standalone/kb_data/d22) (3 / 3) (finite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aAbBcCdDyYfF");
      kb.set_identity("");
      kb.set_inverses("AaBbCcDdYyFf");

      kb.add_rule("aCAd", "");
      kb.add_rule("bfBY", "");
      kb.add_rule("cyCD", "");
      kb.add_rule("dFDa", "");
      kb.add_rule("ybYA", "");
      kb.add_rule("fCFB", "");
      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 30);
      REQUIRE(kb.size() == 22);
    }

    // Fibonacci group F(2,7) - order 29 - works better with largish tidyint
    // Takes approx. 50s
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "098",
        "(fpsemi) (from kbmag/standalone/kb_data/f27) (finite) (2 / 2)",
        "[extreme][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("aAbBcCdDyYfFgG");
      kb.set_identity("");
      kb.set_inverses("AaBbCcDdYyFfGg");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "y");
      kb.add_rule("dy", "f");
      kb.add_rule("yf", "g");
      kb.add_rule("fg", "a");
      kb.add_rule("ga", "b");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 194);
      REQUIRE(kb.size() == 29);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "100",
        "(fpsemi) small example",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");
      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("ababababab", "aa");
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 243);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "050",
        "(fpsemi) construct from shared_ptr<FroidurePin>",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][shortlex]") {
      auto rg      = ReportGuard(REPORT);
      using Transf = TransfHelper<5>::type;
      auto ptr     = std::make_shared<FroidurePin<Transf>>(std::vector<Transf>(
          {Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})}));

      KnuthBendix kb(ptr);
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 88);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "057",
                            "internal_string_to_word",
                            "[quick]") {
      std::vector<Element*>       gens = {new Transformation<uint16_t>({1, 0}),
                                    new Transformation<uint16_t>({0, 0})};
      FroidurePin<Element const*> S    = FroidurePin<Element const*>(gens);
      auto                        rg   = ReportGuard(REPORT);
      delete_gens(gens);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());

      auto x = detail::KBE(kb, 0);
      REQUIRE(x.word(kb) == word_type({0}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix", "066", "code coverage", "[quick]") {
      KnuthBendix kb1;
      KnuthBendix kb2(kb1);
      REQUIRE(kb1.size() == 0);

      kb1.set_alphabet("ab");
      kb1.add_rule("aaa", "a");
      KnuthBendix kb3(kb1);
      REQUIRE(kb3.nr_rules() == 1);
      REQUIRE_THROWS_AS(kb3.set_identity("ab"), LibsemigroupsException);
      REQUIRE_NOTHROW(kb3.set_identity("a"));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "102",
                            "small overlap 1",
                            "[extreme]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("BCA");
      kb.add_rule("AABC", "ACBA");
      REQUIRE(kb.confluent());
      REQUIRE(kb.normal_form("CBACBAABCAABCACBACBA") == "CBACBACBAACBAACBACBA");
      REQUIRE(kb.equal_to("CBAABCABCAABCAABCABC", "CBACBAABCAABCACBACBA"));
      REQUIRE(kb.equal_to("AABCAABCCACAACBBCBCCACBBAABCBA",
                          "ACBAACBACACAACBBCBCCACBBACBABA"));
      REQUIRE(kb.equal_to("CACCBABACCBABACCAAAABCAABCBCAA",
                          "CACCBABACCBABACCAAACBAACBABCAA"));
      REQUIRE(kb.equal_to("CAAACAABCCBABCCBCCBCACABACBBAC",
                          "CAAACACBACBABCCBCCBCACABACBBAC"));
      REQUIRE(kb.equal_to("BABCACBACBCCCCCAACCAAABAABCBCC",
                          "BABCACBACBCCCCCAACCAAABACBABCC"));
      // REQUIRE(std::vector<std::string>(cbegin_silo("BCA", 5, 6),
      //                                cend_silo("BCA", 5, 6))
      //         == std::vector<std::string>());
      // REQUIRE(number_of_words(3, 20, 21) == size_t(18446744071562067968ULL));

      // auto lex_normal_form = [&kb](std::string const& w) {
      //   auto ww = kb.normal_form(w);
      //   auto it = std::find_if(cbegin_silo("BCA", ww, 4 * w.size()),
      //                          cend_silo("BCA", ww.size(), 4 * w.size()),
      //                          [&kb, &ww](std::string const& u) {
      //                            return kb.normal_form(u) == ww;
      //                          });
      //   return *it;
      // };
      // kb.run();
      // REQUIRE(kb.finished());
      // REQUIRE(lex_normal_form("BBBBB") == "BBBBB");
      // REQUIRE(kb.normal_form("AABCB") == "ACBAB");
      // REQUIRE(lex_normal_form("AABCB") == "");
      //      std::vector<std::string> result(number_of_words(3, 5, 20));
      //      std::transform(cbegin_
      // TODO The following code spends the majority of its time in
      // FpSemigroupInterface::validate_letter
      //  auto it =
      //  REQUIRE(it != cend_silo("BCA", 0, 80));
      //  REQUIRE(*it ==  "CBACBAABCAABCACBACBA");
    }
  }  // namespace fpsemigroup

  namespace congruence {

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "086",
                            "(cong) free semigroup congruence (5 classes)",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({0}, {1, 1});

      REQUIRE(!kb.finished());
      REQUIRE(kb.nr_classes() == 5);
      REQUIRE(kb.finished());

      REQUIRE(kb.word_to_class_index({0, 0, 1}) == 4);
      REQUIRE(kb.word_to_class_index({0, 0, 0, 0, 1}) == 4);
      REQUIRE(kb.word_to_class_index({0, 1, 1, 0, 0, 1}) == 4);
      REQUIRE(kb.word_to_class_index({0, 0, 0}) == 0);
      REQUIRE(kb.word_to_class_index({1}) == 1);
      REQUIRE(kb.word_to_class_index({0, 0, 0, 0}) == 2);
    }

    // FIXME(JDM): temporarily commented out to because of change to
    // FpSemigroupInterface that forbids adding rules after started(), and
    // because the copy constructors for KnuthBendix et al. don't currently work
    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "087",
    //                         "(cong) finite transformation semigroup "
    //                         "congruence (21 classes)",
    //                         "[quick][congruence][knuth-bendix][cong]") {
    //   auto rg      = ReportGuard(REPORT);
    //   using Transf = TransfHelper<5>::type;
    //   FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3,
    //   3})});

    //   REQUIRE(S.size() == 88);
    //   REQUIRE(S.nr_rules() == 18);

    //   KnuthBendix kb(S);
    //   auto&       P = kb.quotient_froidure_pin();
    //   REQUIRE(P.size() == 88);
    //   kb.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
    //               S.factorisation(Transf({3, 1, 3, 3, 3})));
    //   // P is now invalid, it's a reference to something that was deleted in
    //   // kb.

    //   REQUIRE(kb.nr_classes() == 21);
    //   REQUIRE(kb.nr_classes() == 21);
    //   auto& Q = kb.quotient_froidure_pin();  // quotient

    //   REQUIRE(Q.size() == 21);
    //   REQUIRE(Q.nr_idempotents() == 3);

    //   std::vector<word_type>
    //   v(static_cast<FroidurePin<detail::KBE>&>(Q).cbegin(),
    //                            static_cast<FroidurePin<detail::KBE>&>(Q).cend());
    //   REQUIRE(v
    //           == std::vector<word_type>({{0},
    //                                      {1},
    //                                      {0, 0},
    //                                      {0, 1},
    //                                      {1, 0},
    //                                      {1, 1},
    //                                      {0, 0, 0},
    //                                      {0, 0, 1},
    //                                      {0, 1, 0},
    //                                      {0, 1, 1},
    //                                      {1, 0, 0},
    //                                      {1, 1, 0},
    //                                      {0, 0, 0, 0},
    //                                      {0, 1, 0, 0},
    //                                      {0, 1, 1, 0},
    //                                      {1, 0, 0, 0},
    //                                      {1, 1, 0, 0},
    //                                      {0, 1, 0, 0, 0},
    //                                      {0, 1, 1, 0, 0},
    //                                      {1, 1, 0, 0, 0},
    //                                      {0, 1, 1, 0, 0, 0}}));

    //   REQUIRE(
    //       kb.word_to_class_index(S.factorisation(Transf({1, 3, 1, 3, 3})))
    //       == kb.word_to_class_index(S.factorisation(Transf({4, 2, 4, 4,
    //       2}))));

    //   REQUIRE(kb.nr_non_trivial_classes() == 1);
    //   REQUIRE(kb.nr_generators() == 2);
    //   REQUIRE(kb.cbegin_ntc()->size() == 68);
    // }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "088",
                            "free semigroup congruence (6 classes)",
                            "[quick][cong][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(5);
      kb.add_pair({0, 0}, {0});
      kb.add_pair({0, 1}, {1});
      kb.add_pair({1, 0}, {1});
      kb.add_pair({0, 2}, {2});
      kb.add_pair({2, 0}, {2});
      kb.add_pair({0, 3}, {3});
      kb.add_pair({3, 0}, {3});
      kb.add_pair({0, 4}, {4});
      kb.add_pair({4, 0}, {4});
      kb.add_pair({1, 2}, {0});
      kb.add_pair({2, 1}, {0});
      kb.add_pair({3, 4}, {0});
      kb.add_pair({4, 3}, {0});
      kb.add_pair({2, 2}, {0});
      kb.add_pair({1, 4, 2, 3, 3}, {0});
      kb.add_pair({4, 4, 4}, {0});

      REQUIRE(kb.nr_classes() == 6);
      // Throws because there's no parent semigroup
      REQUIRE_THROWS_AS(kb.nr_non_trivial_classes(), LibsemigroupsException);
      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({2}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "089",
                            "(cong) free semigroup congruence (16 classes)",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(4);
      kb.add_pair({3}, {2});
      kb.add_pair({0, 3}, {0, 2});
      kb.add_pair({1, 1}, {1});
      kb.add_pair({1, 3}, {1, 2});
      kb.add_pair({2, 1}, {2});
      kb.add_pair({2, 2}, {2});
      kb.add_pair({2, 3}, {2});
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({0, 0, 1}, {1});
      kb.add_pair({0, 0, 2}, {2});
      kb.add_pair({0, 1, 2}, {1, 2});
      kb.add_pair({1, 0, 0}, {1});
      kb.add_pair({1, 0, 2}, {0, 2});
      kb.add_pair({2, 0, 0}, {2});
      kb.add_pair({0, 1, 0, 1}, {1, 0, 1});
      kb.add_pair({0, 2, 0, 2}, {2, 0, 2});
      kb.add_pair({1, 0, 1, 0}, {1, 0, 1});
      kb.add_pair({1, 2, 0, 1}, {1, 0, 1});
      kb.add_pair({1, 2, 0, 2}, {2, 0, 2});
      kb.add_pair({2, 0, 1, 0}, {2, 0, 1});
      kb.add_pair({2, 0, 2, 0}, {2, 0, 2});

      REQUIRE(kb.nr_classes() == 16);
      REQUIRE(kb.word_to_class_index({2}) == kb.word_to_class_index({3}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "090",
                            "(cong) free semigroup congruence (6 classes)",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(11);
      kb.add_pair({2}, {1});
      kb.add_pair({4}, {3});
      kb.add_pair({5}, {0});
      kb.add_pair({6}, {3});
      kb.add_pair({7}, {1});
      kb.add_pair({8}, {3});
      kb.add_pair({9}, {3});
      kb.add_pair({10}, {0});
      kb.add_pair({0, 2}, {0, 1});
      kb.add_pair({0, 4}, {0, 3});
      kb.add_pair({0, 5}, {0, 0});
      kb.add_pair({0, 6}, {0, 3});
      kb.add_pair({0, 7}, {0, 1});
      kb.add_pair({0, 8}, {0, 3});
      kb.add_pair({0, 9}, {0, 3});
      kb.add_pair({0, 10}, {0, 0});
      kb.add_pair({1, 1}, {1});
      kb.add_pair({1, 2}, {1});
      kb.add_pair({1, 4}, {1, 3});
      kb.add_pair({1, 5}, {1, 0});
      kb.add_pair({1, 6}, {1, 3});
      kb.add_pair({1, 7}, {1});
      kb.add_pair({1, 8}, {1, 3});
      kb.add_pair({1, 9}, {1, 3});
      kb.add_pair({1, 10}, {1, 0});
      kb.add_pair({3, 1}, {3});
      kb.add_pair({3, 2}, {3});
      kb.add_pair({3, 3}, {3});
      kb.add_pair({3, 4}, {3});
      kb.add_pair({3, 5}, {3, 0});
      kb.add_pair({3, 6}, {3});
      kb.add_pair({3, 7}, {3});
      kb.add_pair({3, 8}, {3});
      kb.add_pair({3, 9}, {3});
      kb.add_pair({3, 10}, {3, 0});
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({0, 0, 1}, {1});
      kb.add_pair({0, 0, 3}, {3});
      kb.add_pair({0, 1, 3}, {1, 3});
      kb.add_pair({1, 0, 0}, {1});
      kb.add_pair({1, 0, 3}, {0, 3});
      kb.add_pair({3, 0, 0}, {3});
      kb.add_pair({0, 1, 0, 1}, {1, 0, 1});
      kb.add_pair({0, 3, 0, 3}, {3, 0, 3});
      kb.add_pair({1, 0, 1, 0}, {1, 0, 1});
      kb.add_pair({1, 3, 0, 1}, {1, 0, 1});
      kb.add_pair({1, 3, 0, 3}, {3, 0, 3});
      kb.add_pair({3, 0, 1, 0}, {3, 0, 1});
      kb.add_pair({3, 0, 3, 0}, {3, 0, 3});

      REQUIRE(kb.nr_classes() == 16);
      REQUIRE(kb.word_to_class_index({0}) == kb.word_to_class_index({5}));
      REQUIRE(kb.word_to_class_index({0}) == kb.word_to_class_index({10}));
      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({2}));
      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({7}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({4}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({6}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({8}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({9}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "091",
        "(cong) free semigroup congruence (240 classes)",
        "[no-valgrind][quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 0}, {0, 0});
      kb.add_pair({1, 0, 0, 1}, {1, 1});
      kb.add_pair({0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0});

      REQUIRE(kb.nr_classes() == 240);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "043",
        "(cong) free semigroup congruence (240 classes)",
        "[no-valgrind][quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 0}, {0, 0});
      kb.add_pair({1, 0, 0, 1}, {1, 1});
      kb.add_pair({0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0});

      REQUIRE_NOTHROW(kb.knuth_bendix().froidure_pin());
      REQUIRE_THROWS_AS(kb.add_pair({0}, {1}), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "094",
        "(cong) less",
        "[no-valgrind][quick][congruence][knuth-bendix][cong]") {
      auto rg = ReportGuard(REPORT);
      {
        KnuthBendix kb;
        kb.set_nr_generators(2);
        kb.add_pair({0, 0, 0}, {0});
        kb.add_pair({1, 1, 1, 1, 1}, {1});
        kb.add_pair({0, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
        kb.run();

        REQUIRE(!kb.less({0, 1, 1, 1, 0, 0, 1, 1, 1, 0}, {1, 1, 1, 1, 0, 0}));
        REQUIRE(!kb.less({0, 1, 1, 0}, {0, 1, 1, 0}));
      }
      {
        KnuthBendix kb;
        kb.set_nr_generators(2);
        kb.add_pair({0, 0, 0}, {0});
        kb.add_pair({1, 1, 1, 1, 1}, {1});
        kb.add_pair({0, 1, 1, 1, 0, 1, 1}, {1, 1, 0});

        REQUIRE(!kb.less({0, 1, 1, 1, 0, 0, 1, 1, 1, 0}, {1, 1, 1, 1, 0, 0}));
        REQUIRE(!kb.less({0, 1, 1, 0}, {0, 1, 1, 0}));
      }
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "095",
        "(cong) less",
        "[quick][congruence][knuth-bendix][cong][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1, 1, 1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});

      REQUIRE(kb.less({0, 0, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "054",
        "(cong) constructors",
        "[quick][congruence][knuth-bendix][cong][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1, 1, 1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
      REQUIRE(kb.nr_classes() == 746);
      SECTION("copy constructor") {
        auto copy(kb);
        REQUIRE(copy.nr_classes() == 746);
        REQUIRE(copy.nr_generators() == 2);
        // the copy uses the "active rules" of kb, of which there are 105 since
        // knuth-bendix has already been run.
        REQUIRE(copy.nr_generating_pairs() == 105);
      }
      SECTION("FroidurePin") {
        using Transf = TransfHelper<5>::type;
        FroidurePin<Transf> S(
            {Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

        KnuthBendix kb(S);
        kb.run();
        REQUIRE(kb.knuth_bendix().confluent());
        REQUIRE(kb.nr_classes() == 88);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "073",
                            "(cong) const_contains/contains",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});

      REQUIRE(kb.const_contains({0, 0, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1})
              == tril::unknown);
      REQUIRE(kb.const_contains({0, 0, 0}, {0, 0, 0}) == tril::TRUE);
      REQUIRE_THROWS_AS(kb.const_contains({0, 0, 2}, {0, 0, 0}),
                        LibsemigroupsException);
      REQUIRE(kb.const_contains({0, 0, 0}, {0}) == tril::TRUE);
      REQUIRE(kb.nr_classes() == 12);
      REQUIRE(kb.const_contains({0, 0, 0}, {1, 1}) == tril::FALSE);
      REQUIRE(!kb.contains({0, 0, 0}, {1, 1}));
      REQUIRE(kb.contains({0, 0, 0}, {0, 0, 0}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "087",
                            "(cong) is_quotient_obviously_finite",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
      REQUIRE(!kb.is_quotient_obviously_finite());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "093",
                            "(cong) class_index_to_word",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
      REQUIRE(kb.class_index_to_word(0) == word_type({0}));
      REQUIRE(kb.class_index_to_word(1) == word_type({1}));
      REQUIRE(kb.class_index_to_word(2) == word_type({0, 0}));
      REQUIRE(kb.class_index_to_word(3) == word_type({0, 1}));
      REQUIRE(kb.class_index_to_word(4) == word_type({1, 0}));
      REQUIRE(kb.class_index_to_word(5) == word_type({1, 1}));
      REQUIRE(kb.class_index_to_word(6) == word_type({0, 0, 1}));
      REQUIRE(kb.class_index_to_word(7) == word_type({0, 1, 1}));
      REQUIRE(kb.class_index_to_word(8) == word_type({1, 1, 1}));
      REQUIRE(kb.class_index_to_word(9) == word_type({0, 0, 1, 1}));
      REQUIRE(kb.class_index_to_word(10) == word_type({0, 1, 1, 1}));
      REQUIRE(kb.class_index_to_word(11) == word_type({0, 0, 1, 1, 1}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "099",
                            "(cong) quotient_froidure_pin",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
      REQUIRE(kb.quotient_froidure_pin()->size() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "101",
                            "(cong) set_nr_generators",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      REQUIRE_NOTHROW(kb.set_nr_generators(2));
      REQUIRE_THROWS_AS(kb.set_nr_generators(3), LibsemigroupsException);
      REQUIRE_NOTHROW(kb.set_nr_generators(2));
    }
  }  // namespace congruence
}  // namespace libsemigroups
