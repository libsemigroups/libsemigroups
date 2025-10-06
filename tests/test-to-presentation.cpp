//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
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

#include <algorithm>         // for fill,  max_element
#include <cctype>            // for isprint
#include <cstdint>           // for uint8_t, uint16_t
#include <initializer_list>  // for initializer_list
#include <string>            // for basic_string, operator==
#include <unordered_map>     // for operator==, operator!=
#include <utility>           // for move
#include <vector>            // for vector

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bipart.hpp"           // for Bipartition
#include "libsemigroups/constants.hpp"        // for operator!=, operator==
#include "libsemigroups/exception.hpp"        // for LibsemigroupsException
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/kambites-class.hpp"   // for Kambites
#include "libsemigroups/presentation.hpp"     // for Presentation, change_...
#include "libsemigroups/to-presentation.hpp"  // for v4::to<Presentation>
#include "libsemigroups/types.hpp"            // for word_type, congruence_kind

#include "libsemigroups/detail/containers.hpp"  // for StaticVector1, operat...
#include "libsemigroups/detail/report.hpp"      // for ReportGuard
#include "libsemigroups/detail/string.hpp"      // for operator<<

namespace libsemigroups {

  namespace {

    template <typename W1, typename W2>
    void check_to_inverse_presentation() {}

    template <typename Word>
    void check_to_inverse_presentation_from_presentation() {}
  }  // namespace

  using detail::StaticVector1;

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("to<Presentation>",
                                   "013",
                                   "from FroidurePin",
                                   "[quick][to_presentation]",
                                   word_type,
                                   (StaticVector1<uint16_t, 8>),
                                   std::string) {
    auto                     rg = ReportGuard(false);
    FroidurePin<Bipartition> S;
    S.add_generator(make<Bipartition>({{1, -1}, {2, -2}, {3, -3}, {4, -4}}));
    S.add_generator(make<Bipartition>({{1, -2}, {2, -3}, {3, -4}, {4, -1}}));
    S.add_generator(make<Bipartition>({{1, -2}, {2, -1}, {3, -3}, {4, -4}}));
    S.add_generator(make<Bipartition>({{1, 2}, {3, -3}, {4, -4}, {-1, -2}}));
    REQUIRE(S.size() == 105);

    auto p = to<Presentation<TestType>>(S);
    REQUIRE(p.alphabet().size() == 4);
    REQUIRE(p.rules.size() == 86);
    REQUIRE(presentation::length(p) == 359);
    REQUIRE(std::max_element(p.rules.cbegin(),
                             p.rules.cend(),
                             [](auto const& x, auto const& y) {
                               return x.size() < y.size();
                             })
                ->size()
            == 8);
    p.throw_if_bad_alphabet_or_rules();
  }

  LIBSEMIGROUPS_TEST_CASE("to<Presentation>",
                          "014",
                          "from FroidurePin and alphabet",
                          "[quick][to_presentation]") {
    FroidurePin<Bipartition> S;
    S.add_generator(make<Bipartition>({{1, -1}, {2, -2}, {3, -3}, {4, -4}}));
    S.add_generator(make<Bipartition>({{1, -2}, {2, -3}, {3, -4}, {4, -1}}));
    S.add_generator(make<Bipartition>({{1, -2}, {2, -1}, {3, -3}, {4, -4}}));
    S.add_generator(make<Bipartition>({{1, 2}, {3, -3}, {4, -4}, {-1, -2}}));
    REQUIRE(S.size() == 105);

    auto p = to<Presentation<std::string>>(S);
    // Alphabet too small
    REQUIRE_THROWS_AS(presentation::change_alphabet(p, "abc"),
                      LibsemigroupsException);
    // Alphabet contains repeats
    REQUIRE_THROWS_AS(presentation::change_alphabet(p, "abca"),
                      LibsemigroupsException);
    // Alphabet too long
    REQUIRE_THROWS_AS(presentation::change_alphabet(p, "abcde"),
                      LibsemigroupsException);
    presentation::change_alphabet(p, "abcd");

    REQUIRE(p.alphabet().size() == 4);
    REQUIRE(p.rules[8] == "ba");
    REQUIRE(p.rules[9] == "b");
    REQUIRE(presentation::longest_subword_reducing_length(p) == "bcb");
  }

  using string_string     = std::pair<std::string, std::string>;
  using string_word       = std::pair<std::string, word_type>;
  using string_static_vec = std::pair<std::string, StaticVector1<uint8_t, 3>>;

  using word_string     = std::pair<word_type, std::string>;
  using word_word       = std::pair<word_type, word_type>;
  using word_static_vec = std::pair<word_type, StaticVector1<uint8_t, 3>>;

  using static_vec_string = std::pair<StaticVector1<uint8_t, 3>, std::string>;
  using static_vec_word   = std::pair<StaticVector1<uint8_t, 3>, word_type>;
  using static_vec_static_vec
      = std::pair<StaticVector1<uint8_t, 3>, StaticVector1<uint8_t, 3>>;

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("to<Presentation>",
                                   "015",
                                   "from present.",
                                   "[quick][to_presentation]",
                                   string_string,
                                   string_word,
                                   string_static_vec,
                                   word_string,
                                   word_word,
                                   word_static_vec,
                                   static_vec_string,
                                   static_vec_word,
                                   static_vec_static_vec) {
    using W1 = typename TestType::first_type;
    using W2 = typename TestType::second_type;

    Presentation<W1> p;
    p.alphabet(3);
    p.contains_empty_word(true);
    if constexpr (std::is_same_v<W1, std::string>) {
      presentation::add_rule_no_checks(p, "abc", "ab");
      presentation::add_rule_no_checks(p, "abc", "");
    } else {
      presentation::add_rule_no_checks(p, {0, 1, 2}, {0, 1});
      presentation::add_rule_no_checks(p, {0, 1, 2}, {});
    }
    p.throw_if_bad_alphabet_or_rules();

    Presentation<W2> q = v4::to<Presentation<W2>>(p);
    REQUIRE(q.contains_empty_word());

    if constexpr (std::is_same_v<W2, std::string>) {
      REQUIRE(q.alphabet() == "abc");
      REQUIRE(q.rules == std::vector<W2>({"abc", "ab", "abc", ""}));
    } else {
      REQUIRE(q.alphabet() == W2({0, 1, 2}));
      REQUIRE(q.rules == std::vector<W2>({{0, 1, 2}, {0, 1}, {0, 1, 2}, {}}));
    }
    q.throw_if_bad_alphabet_or_rules();

    // Check p hasn't been destroyed
    REQUIRE(p.contains_empty_word());
    if constexpr (std::is_same_v<W1, std::string>) {
      REQUIRE(p.alphabet() == "abc");
      REQUIRE(p.rules == std::vector<W1>({"abc", "ab", "abc", ""}));
    } else {
      REQUIRE(p.alphabet() == W1({0, 1, 2}));
      REQUIRE(p.rules == std::vector<W1>({{0, 1, 2}, {0, 1}, {0, 1, 2}, {}}));
    }
    p.throw_if_bad_alphabet_or_rules();

    // Check two conversions gets you back to where you started
    REQUIRE(p == v4::to<Presentation<W1>>(q));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("to<Presentation>",
                                   "016",
                                   "from present. + func.",
                                   "[quick][to_presentation]",
                                   string_string,
                                   string_word,
                                   string_static_vec,
                                   word_string,
                                   word_word,
                                   word_static_vec,
                                   static_vec_string,
                                   static_vec_word,
                                   static_vec_static_vec) {
    using W1 = typename TestType::first_type;
    using W2 = typename TestType::second_type;
    Presentation<W1> p;
    p.alphabet(3);
    p.contains_empty_word(true);
    if constexpr (std::is_same_v<W1, std::string>) {
      presentation::add_rule_no_checks(p, "abc", "ab");
      presentation::add_rule_no_checks(p, "abc", "");
    } else {
      presentation::add_rule_no_checks(p, {0, 1, 2}, {0, 1});
      presentation::add_rule_no_checks(p, {0, 1, 2}, {});
    }

    auto f1 = [&p](auto val) {
      return words::human_readable_letter<W2>(p.index(val) + 7);
    };
    Presentation<W2> q = v4::to<Presentation<W2>>(p, f1);
    REQUIRE(q.contains_empty_word());
    if constexpr (std::is_same_v<W2, std::string>) {
      REQUIRE(q.alphabet() == "hij");
      REQUIRE(q.rules == std::vector<W2>({"hij", "hi", "hij", ""}));
    } else {
      REQUIRE(q.alphabet() == W2({7, 8, 9}));
      REQUIRE(q.rules == std::vector<W2>({{7, 8, 9}, {7, 8}, {7, 8, 9}, {}}));
    }
    q.throw_if_bad_alphabet_or_rules();

    auto             f2 = [&p](auto val) { return p.index(val); };
    Presentation<W2> r  = v4::to<Presentation<W2>>(p, f2);
    REQUIRE(r.contains_empty_word());
    REQUIRE(r.alphabet() == W2({0, 1, 2}));
    REQUIRE(r.rules == std::vector<W2>({{0, 1, 2}, {0, 1}, {0, 1, 2}, {}}));

    r.throw_if_bad_alphabet_or_rules();
  }

  LIBSEMIGROUPS_TEST_CASE("to<Presentation>",
                          "017",
                          "from present. and alphabet",
                          "[quick][to_presentation]") {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(false);
    presentation::add_rule_no_checks(p, {0, 1, 2}, {0, 1});
    presentation::add_rule_no_checks(p, {0, 1, 2}, {});
    // intentionally bad
    REQUIRE_THROWS_AS(p.throw_if_bad_alphabet_or_rules(),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(v4::to<Presentation<std::string>>(p),
                      LibsemigroupsException);

    p.alphabet_from_rules();
    REQUIRE(p.alphabet() == word_type({0, 1, 2}));
    p.throw_if_bad_alphabet_or_rules();
    REQUIRE(p.contains_empty_word());
    auto q = v4::to<Presentation<std::string>>(p);
    presentation::change_alphabet(q, "abc");
    REQUIRE(q.alphabet() == "abc");
    REQUIRE(q.contains_empty_word());
    REQUIRE(q.rules == std::vector<std::string>({"abc", "ab", "abc", ""}));
    q.throw_if_bad_alphabet_or_rules();
  }

  LIBSEMIGROUPS_TEST_CASE(
      "to<Presentation>",
      "018",
      "use human readable alphabet for v4::to<Presentation>",
      "[quick][presentation]") {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule(p, {0, 1}, {});

    auto q = v4::to<Presentation<std::string>>(p);
    REQUIRE(q.alphabet() == "ab");
    REQUIRE(q.rules == std::vector<std::string>({"ab", ""}));
    q = v4::to<Presentation<std::string>>(p);
    presentation::change_alphabet(q, "xy");
    REQUIRE(q.alphabet() == "xy");
    REQUIRE(q.rules == std::vector<std::string>({"xy", ""}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("to<InversePresentation>",
                                   "019",
                                   "from inv. present.",
                                   "[quick][to_presentation]",
                                   string_string,
                                   string_word,
                                   string_static_vec,
                                   word_string,
                                   word_word,
                                   word_static_vec,
                                   static_vec_string,
                                   static_vec_word,
                                   static_vec_static_vec) {
    using W1 = typename TestType::first_type;
    using W2 = typename TestType::second_type;
    InversePresentation<W1> ip;
    ip.alphabet(3);
    ip.contains_empty_word(true);
    if constexpr (std::is_same_v<W1, std::string>) {
      presentation::add_rule_no_checks(ip, "abc", "ab");
      presentation::add_rule_no_checks(ip, "abc", "");
      ip.inverses_no_checks("cba");
    } else {
      presentation::add_rule_no_checks(ip, {0, 1, 2}, {0, 1});
      presentation::add_rule_no_checks(ip, {0, 1, 2}, {});
      ip.inverses_no_checks({2, 1, 0});
    }
    ip.throw_if_bad_alphabet_or_rules();

    auto iq = v4::to<InversePresentation<W2>>(ip);
    REQUIRE(iq.contains_empty_word());

    if constexpr (std::is_same_v<W2, std::string>) {
      REQUIRE(iq.alphabet() == "abc");
      REQUIRE(iq.rules == std::vector<W2>({"abc", "ab", "abc", ""}));
      REQUIRE(iq.inverses() == "cba");
    } else {
      REQUIRE(iq.alphabet() == W2({0, 1, 2}));
      REQUIRE(iq.rules == std::vector<W2>({{0, 1, 2}, {0, 1}, {0, 1, 2}, {}}));
      REQUIRE(iq.inverses() == W2({2, 1, 0}));
    }
    iq.throw_if_bad_alphabet_or_rules();

    // Check p hasn't been destroyed
    REQUIRE(ip.contains_empty_word());
    if constexpr (std::is_same_v<W1, std::string>) {
      REQUIRE(ip.alphabet() == "abc");
      REQUIRE(ip.rules == std::vector<W1>({"abc", "ab", "abc", ""}));
      REQUIRE(ip.inverses() == "cba");
    } else {
      REQUIRE(ip.alphabet() == W1({0, 1, 2}));
      REQUIRE(ip.rules == std::vector<W1>({{0, 1, 2}, {0, 1}, {0, 1, 2}, {}}));
      REQUIRE(ip.inverses() == W1({2, 1, 0}));
    }
    ip.throw_if_bad_alphabet_or_rules();

    // Check two conversions gets you back to where you
    // started
    REQUIRE(ip == v4::to<InversePresentation<W1>>(iq));

    auto                    f  = [&ip](auto val) { return ip.index(val) + 3; };
    InversePresentation<W2> ir = v4::to<InversePresentation<W2>>(ip, f);
    REQUIRE(ir.contains_empty_word());
    REQUIRE(ir.alphabet() == W2({3, 4, 5}));
    REQUIRE(ir.rules == std::vector<W2>({{3, 4, 5}, {3, 4}, {3, 4, 5}, {}}));
    REQUIRE(ir.inverses() == W2({5, 4, 3}));

    ir.throw_if_bad_alphabet_or_rules();
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("to<InversePresentation>",
                                   "020",
                                   "from present.",
                                   "[quick][to_presentation][no-valgrind]",
                                   std::string,
                                   word_type,
                                   (StaticVector1<uint8_t, 6>) ) {
    Presentation<TestType> p;
    p.alphabet(3);
    if constexpr (std::is_same_v<TestType, std::string>) {
      presentation::add_rule_no_checks(p, "abc", "ab");
      presentation::add_rule_no_checks(p, "acb", "c");
    } else {
      presentation::add_rule_no_checks(p, {0, 1, 2}, {0, 1});
      presentation::add_rule_no_checks(p, {0, 2, 1}, {2});
    }
    p.throw_if_bad_alphabet_or_rules();

    InversePresentation<TestType> ip = v4::to<InversePresentation>(p);
    REQUIRE(!ip.contains_empty_word());
    if constexpr (std::is_same_v<TestType, std::string>) {
      REQUIRE(ip.alphabet() == "abcdef");
      REQUIRE(ip.rules == std::vector<TestType>({"abc", "ab", "acb", "c"}));
      REQUIRE(ip.inverses() == "defabc");
    } else {
      REQUIRE(ip.alphabet() == TestType({0, 1, 2, 3, 4, 5}));
      REQUIRE(ip.rules
              == std::vector<TestType>({{0, 1, 2}, {0, 1}, {0, 2, 1}, {2}}));
      REQUIRE(ip.inverses() == TestType({3, 4, 5, 0, 1, 2}));
    }

    {
      Presentation<std::vector<uint16_t>> q;
      q.alphabet(32768);
      REQUIRE_NOTHROW(v4::to<InversePresentation>(q));
      q.alphabet(32769);
      REQUIRE_THROWS(v4::to<InversePresentation>(q));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("to<Presentation<std::string>>",
                          "021",
                          "from KnuthBendix<std::string>",
                          "[quick][to_presentation]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("hijkl");
    presentation::add_rule(p, "hi", "j");
    presentation::add_rule(p, "ij", "k");
    presentation::add_rule(p, "jk", "l");
    presentation::add_rule(p, "kl", "h");
    presentation::add_rule(p, "lh", "i");

    KnuthBendix<std::string> kb(congruence_kind::twosided, p);
    kb.run();

    auto q = to<Presentation<std::string>>(kb);
    REQUIRE(q == to<Presentation>(kb));

    REQUIRE(q.alphabet() == p.alphabet());
    REQUIRE(q.rules.size() == 48);

    presentation::sort_each_rule(q);
    presentation::sort_rules(q);
    REQUIRE(q.rules
            == std::vector<std::string>{
                "hi",  "j",  "hl",  "i",  "ih",  "j",  "ij",  "k",  "ji", "k",
                "jk",  "l",  "kj",  "l",  "kl",  "h",  "lh",  "i",  "lk", "h",
                "hhh", "l",  "iii", "h",  "ik",  "hh", "jh",  "hj", "jj", "hk",
                "jl",  "ii", "kh",  "hk", "ki",  "hh", "kk",  "il", "li", "il",
                "lj",  "ii", "ll",  "hj", "hhj", "il", "iil", "hhk"});
  }

  LIBSEMIGROUPS_TEST_CASE("to<Presentation<std::string>>",
                          "022",
                          "from KnuthBendix<word_type>",
                          "[quick][to_presentation]") {
    using literals::operator""_w;
    auto            rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet("56789"_w);
    presentation::add_rule(p, "56"_w, "7"_w);
    presentation::add_rule(p, "67"_w, "8"_w);
    presentation::add_rule(p, "78"_w, "9"_w);
    presentation::add_rule(p, "89"_w, "5"_w);
    presentation::add_rule(p, "95"_w, "6"_w);

    KnuthBendix<word_type> kb(congruence_kind::twosided, p);
    kb.run();

    auto q = to<Presentation<std::string>>(kb);

    REQUIRE(q.alphabet() == "abcde");
    REQUIRE(q.rules.size() == 48);

    presentation::sort_each_rule(q);
    presentation::sort_rules(q);
    REQUIRE(q.rules
            == std::vector<std::string>{
                "ab",  "c",  "ae",  "b",  "ba",  "c",  "bc",  "d",  "cb", "d",
                "cd",  "e",  "dc",  "e",  "de",  "a",  "ea",  "b",  "ed", "a",
                "aaa", "e",  "bbb", "a",  "bd",  "aa", "ca",  "ac", "cc", "ad",
                "ce",  "bb", "da",  "ad", "db",  "aa", "dd",  "be", "eb", "be",
                "ec",  "bb", "ee",  "ac", "aac", "be", "bbe", "aad"});
  }

  LIBSEMIGROUPS_TEST_CASE("to<Presentation<word_type>>",
                          "023",
                          "from KnuthBendix<std::string>",
                          "[quick][to_presentation]") {
    using literals::operator""_w;
    auto            rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("hijkl");
    presentation::add_rule(p, "hi", "j");
    presentation::add_rule(p, "ij", "k");
    presentation::add_rule(p, "jk", "l");
    presentation::add_rule(p, "kl", "h");
    presentation::add_rule(p, "lh", "i");

    KnuthBendix<std::string> kb(congruence_kind::twosided, p);
    kb.run();

    auto q = to<Presentation<word_type>>(kb);

    REQUIRE(q.alphabet() == "01234"_w);
    REQUIRE(q.rules.size() == 48);

    presentation::sort_each_rule(q);
    presentation::sort_rules(q);
    REQUIRE(q.rules
            == std::vector<word_type>{
                "01"_w, "2"_w,   "04"_w,  "1"_w,  "10"_w,  "2"_w,  "12"_w,
                "3"_w,  "21"_w,  "3"_w,   "23"_w, "4"_w,   "32"_w, "4"_w,
                "34"_w, "0"_w,   "40"_w,  "1"_w,  "43"_w,  "0"_w,  "000"_w,
                "4"_w,  "111"_w, "0"_w,   "13"_w, "00"_w,  "20"_w, "02"_w,
                "22"_w, "03"_w,  "24"_w,  "11"_w, "30"_w,  "03"_w, "31"_w,
                "00"_w, "33"_w,  "14"_w,  "41"_w, "14"_w,  "42"_w, "11"_w,
                "44"_w, "02"_w,  "002"_w, "14"_w, "114"_w, "003"_w});
  }

  LIBSEMIGROUPS_TEST_CASE("to<Presentation<word_type>>",
                          "024",
                          "from KnuthBendix<word_type>",
                          "[quick][to_presentation]") {
    using literals::operator""_w;
    auto            rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet("56789"_w);
    presentation::add_rule(p, "56"_w, "7"_w);
    presentation::add_rule(p, "67"_w, "8"_w);
    presentation::add_rule(p, "78"_w, "9"_w);
    presentation::add_rule(p, "89"_w, "5"_w);
    presentation::add_rule(p, "95"_w, "6"_w);

    KnuthBendix<word_type> kb(congruence_kind::twosided, p);
    kb.run();

    auto q = to<Presentation<word_type>>(kb);
    REQUIRE(q == to<Presentation>(kb));

    REQUIRE(q.alphabet() == p.alphabet());
    REQUIRE(q.rules.size() == 48);

    presentation::sort_each_rule(q);
    presentation::sort_rules(q);
    REQUIRE(q.rules
            == std::vector<word_type>{
                "56"_w, "7"_w,   "59"_w,  "6"_w,  "65"_w,  "7"_w,  "67"_w,
                "8"_w,  "76"_w,  "8"_w,   "78"_w, "9"_w,   "87"_w, "9"_w,
                "89"_w, "5"_w,   "95"_w,  "6"_w,  "98"_w,  "5"_w,  "555"_w,
                "9"_w,  "666"_w, "5"_w,   "68"_w, "55"_w,  "75"_w, "57"_w,
                "77"_w, "58"_w,  "79"_w,  "66"_w, "85"_w,  "58"_w, "86"_w,
                "55"_w, "88"_w,  "69"_w,  "96"_w, "69"_w,  "97"_w, "66"_w,
                "99"_w, "57"_w,  "557"_w, "69"_w, "669"_w, "558"_w});
  }

  LIBSEMIGROUPS_TEST_CASE("to<Presentation<word_type>>",
                          "025",
                          "from Kambites<Word>",
                          "[quick][to_presentation]") {
    using literals::        operator""_w;
    Presentation<word_type> p;
    p.alphabet("56789"_w);
    presentation::add_rule(p, "56"_w, "7"_w);
    presentation::add_rule(p, "67"_w, "8"_w);
    presentation::add_rule(p, "78"_w, "9"_w);
    presentation::add_rule(p, "89"_w, "5"_w);
    presentation::add_rule(p, "95"_w, "6"_w);

    Kambites k(congruence_kind::twosided, p);
    REQUIRE(to<Presentation<word_type>>(k) == p);
    REQUIRE(to<Presentation<std::string>>(k).rules
            == std::vector<std::string>(
                {"ab", "c", "bc", "d", "cd", "e", "de", "a", "ea", "b"}));

    Presentation<std::string> p_str;
    p_str.alphabet("abc");
    presentation::add_rule(p_str, "aa", "b");
    presentation::add_rule(p_str, "bb", "c");
    presentation::add_rule(p_str, "cc", "a");

    Kambites k_str(congruence_kind::twosided, p_str);
    REQUIRE(to<Presentation<std::string>>(k_str) == p_str);
    REQUIRE(to<Presentation<word_type>>(k_str)
            == v4::to<Presentation<word_type>>(p_str));
  }

}  // namespace libsemigroups
