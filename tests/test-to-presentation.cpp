//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#include "Catch2-3.7.1/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bipart.hpp"           // for Bipartition
#include "libsemigroups/constants.hpp"        // for operator!=, operator==
#include "libsemigroups/exception.hpp"        // for LibsemigroupsException
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/presentation.hpp"     // for Presentation, change_...
#include "libsemigroups/to-presentation.hpp"  // for to<Presentation>
#include "libsemigroups/types.hpp"            // for word_type

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
    S.add_generator(Bipartition({{1, -1}, {2, -2}, {3, -3}, {4, -4}}));
    S.add_generator(Bipartition({{1, -2}, {2, -3}, {3, -4}, {4, -1}}));
    S.add_generator(Bipartition({{1, -2}, {2, -1}, {3, -3}, {4, -4}}));
    S.add_generator(Bipartition({{1, 2}, {3, -3}, {4, -4}, {-1, -2}}));
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
    p.validate();
  }

  LIBSEMIGROUPS_TEST_CASE("to<Presentation>",
                          "014",
                          "from FroidurePin and alphabet",
                          "[quick][to_presentation]") {
    FroidurePin<Bipartition> S;
    S.add_generator(Bipartition({{1, -1}, {2, -2}, {3, -3}, {4, -4}}));
    S.add_generator(Bipartition({{1, -2}, {2, -3}, {3, -4}, {4, -1}}));
    S.add_generator(Bipartition({{1, -2}, {2, -1}, {3, -3}, {4, -4}}));
    S.add_generator(Bipartition({{1, 2}, {3, -3}, {4, -4}, {-1, -2}}));
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
    p.validate();

    Presentation<W2> q = to<Presentation<W2>>(p);
    REQUIRE(q.contains_empty_word());

    if constexpr (std::is_same_v<W2, std::string>) {
      REQUIRE(q.alphabet() == "abc");
      REQUIRE(q.rules == std::vector<W2>({"abc", "ab", "abc", ""}));
    } else {
      REQUIRE(q.alphabet() == W2({0, 1, 2}));
      REQUIRE(q.rules == std::vector<W2>({{0, 1, 2}, {0, 1}, {0, 1, 2}, {}}));
    }
    q.validate();

    // Check p hasn't been destroyed
    REQUIRE(p.contains_empty_word());
    if constexpr (std::is_same_v<W1, std::string>) {
      REQUIRE(p.alphabet() == "abc");
      REQUIRE(p.rules == std::vector<W1>({"abc", "ab", "abc", ""}));
    } else {
      REQUIRE(p.alphabet() == W1({0, 1, 2}));
      REQUIRE(p.rules == std::vector<W1>({{0, 1, 2}, {0, 1}, {0, 1, 2}, {}}));
    }
    p.validate();

    // Check two conversions gets you back to where you started
    REQUIRE(p == to<Presentation<W1>>(q));
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
    Presentation<W2> q = to<Presentation<W2>>(p, f1);
    REQUIRE(q.contains_empty_word());
    if constexpr (std::is_same_v<W2, std::string>) {
      REQUIRE(q.alphabet() == "hij");
      REQUIRE(q.rules == std::vector<W2>({"hij", "hi", "hij", ""}));
    } else {
      REQUIRE(q.alphabet() == W2({7, 8, 9}));
      REQUIRE(q.rules == std::vector<W2>({{7, 8, 9}, {7, 8}, {7, 8, 9}, {}}));
    }
    q.validate();

    auto             f2 = [&p](auto val) { return p.index(val); };
    Presentation<W2> r  = to<Presentation<W2>>(p, f2);
    REQUIRE(r.contains_empty_word());
    REQUIRE(r.alphabet() == W2({0, 1, 2}));
    REQUIRE(r.rules == std::vector<W2>({{0, 1, 2}, {0, 1}, {0, 1, 2}, {}}));

    r.validate();
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
    REQUIRE_THROWS_AS(p.validate(), LibsemigroupsException);
    REQUIRE_THROWS_AS(to<Presentation<std::string>>(p), LibsemigroupsException);

    p.alphabet_from_rules();
    REQUIRE(p.alphabet() == word_type({0, 1, 2}));
    p.validate();
    REQUIRE(p.contains_empty_word());
    auto q = to<Presentation<std::string>>(p);
    presentation::change_alphabet(q, "abc");
    REQUIRE(q.alphabet() == "abc");
    REQUIRE(q.contains_empty_word());
    REQUIRE(q.rules == std::vector<std::string>({"abc", "ab", "abc", ""}));
    q.validate();
  }

  LIBSEMIGROUPS_TEST_CASE("to<Presentation>",
                          "018",
                          "use human readable alphabet for to<Presentation>",
                          "[quick][presentation]") {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule(p, {0, 1}, {});

    auto q = to<Presentation<std::string>>(p);
    REQUIRE(q.alphabet() == "ab");
    REQUIRE(q.rules == std::vector<std::string>({"ab", ""}));
    q = to<Presentation<std::string>>(p);
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
    ip.validate();

    auto iq = to<InversePresentation<W2>>(ip);
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
    iq.validate();

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
    ip.validate();

    // Check two conversions gets you back to where you
    // started
    REQUIRE(ip == to<InversePresentation<W1>>(iq));

    auto                    f  = [&ip](auto val) { return ip.index(val) + 3; };
    InversePresentation<W2> ir = to<InversePresentation<W2>>(ip, f);
    REQUIRE(ir.contains_empty_word());
    REQUIRE(ir.alphabet() == W2({3, 4, 5}));
    REQUIRE(ir.rules == std::vector<W2>({{3, 4, 5}, {3, 4}, {3, 4, 5}, {}}));
    REQUIRE(ir.inverses() == W2({5, 4, 3}));

    ir.validate();
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
    p.validate();

    InversePresentation<TestType> ip = to<InversePresentation>(p);
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
      REQUIRE_NOTHROW(to<InversePresentation>(q));
      q.alphabet(32769);
      REQUIRE_THROWS(to<InversePresentation>(q));
    }
  }

}  // namespace libsemigroups
