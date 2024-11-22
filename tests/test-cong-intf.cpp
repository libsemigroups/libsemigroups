//
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

// The purpose of this file is to test the CongruenceInterface class.

// TODO(0):
// * to_froidure_pin

#include "catch_amalgamated.hpp"  // for REQUIRE
#include "libsemigroups/cong-intf.hpp"
#include "libsemigroups/to-froidure-pin.hpp"
#include "libsemigroups/to-todd-coxeter.hpp"
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/cong.hpp"    // for Congruence
#include "libsemigroups/transf.hpp"  // for Transf<>

#include "libsemigroups/detail/tce.hpp"  // for TCE

namespace libsemigroups {
  using namespace literals;

  congruence_kind constexpr twosided = congruence_kind::twosided;
  congruence_kind constexpr onesided = congruence_kind::onesided;

  TEMPLATE_TEST_CASE("CongruenceInterface: add_generating_pair",
                     "[000][quick]",
                     ToddCoxeter,
                     Congruence,
                     KnuthBendix<>) {
    // Kambites doesn't work in this example
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 0101_w, 00_w);

    TestType cong(twosided, p);
    REQUIRE(!cong.finished());
    REQUIRE(cong.number_of_classes() == 27);
    REQUIRE(cong.finished());
    REQUIRE(cong.started());
    REQUIRE_THROWS_AS(congruence_interface::add_generating_pair(cong, 0_w, 1_w),
                      LibsemigroupsException);
  }

  TEMPLATE_TEST_CASE("CongruenceInterface: contains",
                     "[001][quick]",
                     ToddCoxeter,
                     Congruence,
                     KnuthBendix<>) {
    // Kambites doesn't work in this example
    auto rg = ReportGuard(false);

    TestType cong;
    REQUIRE_THROWS_AS(congruence_interface::currently_contains(cong, {0}, {1}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(congruence_interface::currently_contains(cong, {0}, {0}),
                      LibsemigroupsException);

    REQUIRE(!cong.finished());
    REQUIRE(!cong.started());

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 0101_w, 00_w);

    cong.init(twosided, p);

    REQUIRE(!congruence_interface::contains(cong, 000_w, 00_w));
    REQUIRE(cong.finished());
    REQUIRE(congruence_interface::currently_contains(cong, 000_w, 00_w)
            == tril::FALSE);
  }

  TEMPLATE_TEST_CASE("CongruenceInterface: is_obviously_infinite",
                     "[002][quick]",
                     // ToddCoxeter, TODO(0) currently broken for ToddCoxeter
                     Congruence,
                     KnuthBendix<>) {
    auto rg = ReportGuard(false);

    TestType cong;

    REQUIRE(!is_obviously_infinite(cong));
    // So far cong is not-defined, and hence not finite or infinite

    Presentation<word_type> p;
    p.alphabet(2);

    cong.init(twosided, p);

    REQUIRE(is_obviously_infinite(cong));

    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 0101_w, 00_w);

    cong.init(twosided, p);

    REQUIRE(!is_obviously_infinite(cong));

    REQUIRE(cong.number_of_classes() == 27);
    REQUIRE(!is_obviously_infinite(cong));

    cong.init(onesided, p);
    congruence_interface::add_generating_pair(cong, 000_w, 00_w);

    REQUIRE(!is_obviously_infinite(cong));
    REQUIRE(cong.number_of_classes() == 24);
    REQUIRE(!is_obviously_infinite(cong));
  }

  TEMPLATE_TEST_CASE("CongruenceInterface: non_trivial_classes x1",
                     "[003][quick]",
                     ToddCoxeter,
                     Congruence,
                     KnuthBendix<>) {
    auto rg = ReportGuard(false);
    auto S  = to_froidure_pin(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    TestType cong(twosided, to_presentation<word_type>(S));

    congruence_interface::add_generating_pair(
        cong,
        froidure_pin::factorisation(S, Transf<>({3, 4, 4, 4, 4})),
        froidure_pin::factorisation(S, Transf<>({3, 1, 3, 3, 3})));

    REQUIRE(cong.number_of_classes() == 21);

    auto ntc = non_trivial_classes(cong, froidure_pin::normal_forms(S));
    REQUIRE(ntc.size() == 1);

    REQUIRE(ntc[0].size() == 68);
    std::vector<word_type> expect
        = {001_w,       101_w,       0001_w,     0010_w,     0011_w,
           0101_w,      1001_w,      1010_w,     1011_w,     00001_w,
           00010_w,     00011_w,     00100_w,    00101_w,    00110_w,
           01010_w,     01011_w,     10001_w,    10010_w,    10011_w,
           10100_w,     10101_w,     10110_w,    000010_w,   000011_w,
           000100_w,    000101_w,    000110_w,   001000_w,   001100_w,
           010001_w,    010100_w,    010101_w,   010110_w,   100010_w,
           100011_w,    100100_w,    100101_w,   100110_w,   101000_w,
           101100_w,    0000100_w,   0000101_w,  0000110_w,  0001000_w,
           0001100_w,   0010001_w,   0100010_w,  0100011_w,  0101000_w,
           0101100_w,   1000100_w,   1000101_w,  1000110_w,  1001000_w,
           1001100_w,   00001000_w,  00001100_w, 00100010_w, 01000100_w,
           01000101_w,  01000110_w,  10001000_w, 10001100_w, 001000100_w,
           001000101_w, 010001000_w, 010001100_w};
    REQUIRE(ntc[0] == expect);
  }

  TEMPLATE_TEST_CASE("CongruenceInterface: non_trivial_classes x2",
                     "[004][quick]",
                     ToddCoxeter,
                     Congruence,
                     KnuthBendix<>) {
    auto rg = ReportGuard(false);
    auto S  = to_froidure_pin(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    TestType cong(onesided, to_presentation<word_type>(S));
    congruence::add_generating_pair(
        cong,
        froidure_pin::factorisation(S, Transf<>({3, 4, 4, 4, 4})),
        froidure_pin::factorisation(S, Transf<>({3, 1, 3, 3, 3})));

    REQUIRE(cong.number_of_classes() == 72);

    auto ntc = non_trivial_classes(cong, froidure_pin::normal_forms(S));
    REQUIRE(ntc.size() == 4);

    std::vector<size_t> actual(4, 0);
    std::transform(ntc.begin(),
                   ntc.end(),
                   actual.begin(),
                   std::mem_fn(&std::vector<word_type>::size));
    std::sort(actual.begin(), actual.end());
    std::vector<size_t> expect = {3, 5, 5, 7};
    REQUIRE(actual == expect);
  }

  TEMPLATE_TEST_CASE("CongruenceInterface: no generating pairs added",
                     "[005][quick]",
                     ToddCoxeter,
                     Congruence,
                     KnuthBendix<>,
                     Kambites<>) {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(4);

    TestType cong(twosided, p);

    if constexpr (std::is_same_v<TestType, ToddCoxeter>
                  || std::is_same_v<TestType, KnuthBendix<>>) {
      REQUIRE(congruence_interface::currently_contains(cong, 1_w, 2222222222_w)
              == tril::unknown);
    } else {
      REQUIRE(congruence_interface::currently_contains(cong, 1_w, 2222222222_w)
              == tril::FALSE);
    }
    REQUIRE(!congruence_interface::contains(cong, 1_w, 2222222222_w));
    REQUIRE(cong.number_of_classes() == POSITIVE_INFINITY);
  }

  TEMPLATE_TEST_CASE("CongruenceInterface: to_froidure_pin",
                     "[006][quick]",
                     Kambites<>) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefg");

    presentation::add_rule(p, "abcd", "aaaeaa");
    presentation::add_rule(p, "ef", "dg");

    TestType cong(twosided, p);

    auto fp = to_froidure_pin(cong);

    fp.enumerate(1'000);
    REQUIRE(!fp.finished());
    REQUIRE(fp.current_size() == 8'205);

    REQUIRE((froidure_pin::current_normal_forms(fp) | ToString(p.alphabet())
             | rx::take(100) | rx::to_vector())
            == std::vector<std::string>(
                {"a",   "b",   "c",   "d",   "e",   "f",   "g",   "aa",  "ab",
                 "ac",  "ad",  "ae",  "af",  "ag",  "ba",  "bb",  "bc",  "bd",
                 "be",  "bf",  "bg",  "ca",  "cb",  "cc",  "cd",  "ce",  "cf",
                 "cg",  "da",  "db",  "dc",  "dd",  "de",  "df",  "dg",  "ea",
                 "eb",  "ec",  "ed",  "ee",  "eg",  "fa",  "fb",  "fc",  "fd",
                 "fe",  "ff",  "fg",  "ga",  "gb",  "gc",  "gd",  "ge",  "gf",
                 "gg",  "aaa", "aab", "aac", "aad", "aae", "aaf", "aag", "aba",
                 "abb", "abc", "abd", "abe", "abf", "abg", "aca", "acb", "acc",
                 "acd", "ace", "acf", "acg", "ada", "adb", "adc", "add", "ade",
                 "adf", "adg", "aea", "aeb", "aec", "aed", "aee", "aeg", "afa",
                 "afb", "afc", "afd", "afe", "aff", "afg", "aga", "agb", "agc",
                 "agd"}));
  }

  TEMPLATE_TEST_CASE("CongruenceInterface: to_froidure_pin",
                     "[007][quick]",
                     KnuthBendix<>,
                     ToddCoxeter) {
    auto rg = ReportGuard(false);
    using knuth_bendix::normal_forms;
    using todd_coxeter::normal_forms;

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("Bab");
    presentation::add_rule_no_checks(p, "aa", "");
    presentation::add_rule_no_checks(p, "bB", "");
    presentation::add_rule_no_checks(p, "bbb", "");
    presentation::add_rule_no_checks(p, "ababab", "");

    TestType cong(twosided, p);

    REQUIRE(cong.number_of_classes() == 12);
    REQUIRE(to_froidure_pin(cong).size() == 12);
  }

  TEMPLATE_TEST_CASE("CongruenceInterface: to_froidure_pin",
                     "[008][quick]",
                     Congruence) {
    auto rg = ReportGuard(false);
    using knuth_bendix::normal_forms;
    using todd_coxeter::normal_forms;

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("Bab");
    presentation::add_rule_no_checks(p, "aa", "");
    presentation::add_rule_no_checks(p, "bB", "");
    presentation::add_rule_no_checks(p, "bbb", "");
    presentation::add_rule_no_checks(p, "ababab", "");

    TestType cong(twosided, p);

    REQUIRE(cong.number_of_classes() == 12);
    REQUIRE(to_froidure_pin(cong)->size() == 12);

    p.init();
    p.alphabet("abcdefg");
    p.contains_empty_word(false);
    presentation::add_rule(p, "abcd", "aaaeaa");
    presentation::add_rule(p, "ef", "dg");

    cong.init(twosided, p);
    REQUIRE(cong.number_of_classes() == POSITIVE_INFINITY);

    auto fp = to_froidure_pin(cong);
    fp->enumerate(1'000);
    REQUIRE(fp->current_size() == 8'205);
  }

  TEMPLATE_TEST_CASE("CongruenceInterface: normal_forms",
                     "[009][quick]",
                     KnuthBendix<>,
                     ToddCoxeter) {
    auto rg = ReportGuard(false);
    using knuth_bendix::normal_forms;
    using todd_coxeter::normal_forms;

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("Bab");
    presentation::add_rule_no_checks(p, "aa", "");
    presentation::add_rule_no_checks(p, "bB", "");
    presentation::add_rule_no_checks(p, "bbb", "");
    presentation::add_rule_no_checks(p, "ababab", "");

    TestType cong(twosided, p);

    REQUIRE(cong.number_of_classes() == 12);
    REQUIRE((normal_forms<std::string>(cong) | rx::to_vector())
            == std::vector<std::string>({"",
                                         "B",
                                         "a",
                                         "b",
                                         "Ba",
                                         "aB",
                                         "ab",
                                         "ba",
                                         "BaB",
                                         "Bab",
                                         "aBa",
                                         "baB"}));
    REQUIRE((normal_forms<word_type>(cong) | rx::to_vector())
            == std::vector<word_type>({{},
                                       {66},
                                       {97},
                                       {98},
                                       {66, 97},
                                       {97, 66},
                                       {97, 98},
                                       {98, 97},
                                       {66, 97, 66},
                                       {66, 97, 98},
                                       {97, 66, 97},
                                       {98, 97, 66}}));
  }

}  // namespace libsemigroups
