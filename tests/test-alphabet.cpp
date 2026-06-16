//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#include <string>         // for basic_string, operator==
#include <unordered_set>  // for unordered_set
#include <utility>        // for move

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/alphabet.hpp"    // for Alphabet
#include "libsemigroups/exception.hpp"   // for LibsemigroupsException
#include "libsemigroups/types.hpp"       // for word_type
#include "libsemigroups/word-range.hpp"  // for human_readable_letter

#include "libsemigroups/detail/containers.hpp"  // for StaticVector1
#include "libsemigroups/detail/report.hpp"      // for ReportGuard

namespace libsemigroups {
  using StaticVector = detail::StaticVector1<uint16_t, 64>;

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Alphabet",
                                   "000",
                                   "constructors",
                                   "[quick]",
                                   word_type,
                                   std::string) {
    auto rg = ReportGuard(false);
    using W = TestType;

    Alphabet<W> a;
    REQUIRE(a.empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.letters() == W());

    a.init(W({0, 1, 2}));
    REQUIRE(a.letters() == W({0, 1, 2}));
    REQUIRE(a.size() == 3);
    REQUIRE(!a.empty());

    Alphabet<W> aa(a);
    REQUIRE(aa.letters() == a.letters());

    Alphabet<W> b(std::move(a));
    REQUIRE(b.letters() == aa.letters());

    a = aa;
    REQUIRE(a.letters() == aa.letters());

    Alphabet<W> c;
    c = std::move(b);
    REQUIRE(c.letters() == aa.letters());

    c.init();
    REQUIRE(c.empty());
    REQUIRE(c.letters() == W());

    W           const_ref = {0, 2};
    Alphabet<W> d(const_ref);
    REQUIRE(d.letters() == const_ref);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Alphabet",
                                   "001",
                                   "letters",
                                   "[quick]",
                                   word_type,
                                   std::string) {
    auto rg = ReportGuard(false);
    using W = TestType;

    Alphabet<W> a(W({0, 1, 2}));
    REQUIRE(a.letters() == W({0, 1, 2}));
    REQUIRE(a.letter_no_checks(0) == 0);
    REQUIRE(a.letter_no_checks(1) == 1);
    REQUIRE(a.letter_no_checks(2) == 2);
    REQUIRE(a.letter(0) == 0);
    REQUIRE(a.letter(1) == 1);
    REQUIRE(a.letter(2) == 2);
    REQUIRE_THROWS_AS(a.letter(3), LibsemigroupsException);

    REQUIRE(a.index_no_checks(0) == 0);
    REQUIRE(a.index_no_checks(1) == 1);
    REQUIRE(a.index_no_checks(2) == 2);
    REQUIRE(a.index(0) == 0);
    REQUIRE(a.index(1) == 1);
    REQUIRE(a.index(2) == 2);
    REQUIRE(a.contains(0));
    REQUIRE(a.contains(1));
    REQUIRE(a.contains(2));
    REQUIRE(!a.contains(3));
    REQUIRE_THROWS_AS(a.index(3), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Alphabet",
                                   "002",
                                   "init by size",
                                   "[quick]",
                                   word_type,
                                   std::string) {
    auto rg = ReportGuard(false);
    using W = TestType;

    Alphabet<W> a(4);
    if constexpr (std::is_same_v<W, std::string>) {
      REQUIRE(a.letters() == "abcd");
    } else {
      REQUIRE(a.letters() == W({0, 1, 2, 3}));
    }

    a.init(1);
    if constexpr (std::is_same_v<W, std::string>) {
      REQUIRE(a.letters() == "a");
    } else {
      REQUIRE(a.letters() == W({0}));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet", "003", "init by size limits", "[quick]") {
    auto rg = ReportGuard(false);
    using words::human_readable_letter;

    std::string const letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHI"
                                "JKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < letters.size(); ++i) {
      REQUIRE(letters[i] == human_readable_letter<std::string>(i));
    }
    REQUIRE_THROWS_AS(human_readable_letter<std::string>(256),
                      LibsemigroupsException);

    Alphabet<std::string> a(256);
    REQUIRE(a.size() == 256);
    REQUIRE_EXCEPTION_MSG(a.init(257),
                          "expected a value in the range [0, 257), found 257");
    REQUIRE(a.size() == 256);
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet", "004", "duplicate letters", "[quick]") {
    auto                rg = ReportGuard(false);
    Alphabet<word_type> a(word_type({0, 1, 2}));
    REQUIRE_EXCEPTION_MSG(a.init(word_type({0, 1, 0})),
                          "invalid alphabet [0, 1, 0], duplicate letter 0!");
    REQUIRE(a.letters() == word_type({0, 1, 2}));

    Alphabet<std::string> b("abc");
    REQUIRE_EXCEPTION_MSG(b.init("aba"),
                          "invalid alphabet \"aba\", duplicate letter 'a'!");
    REQUIRE(b.letters() == "abc");
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet", "005", "letter validation", "[quick]") {
    auto                  rg = ReportGuard(false);
    Alphabet<std::string> a("ab");
    REQUIRE_NOTHROW(a.throw_if_letter_not_in_alphabet('a'));
    REQUIRE_NOTHROW(a.throw_if_letter_not_in_alphabet('b'));
    REQUIRE_EXCEPTION_MSG(a.throw_if_letter_not_in_alphabet('c'),
                          "invalid letter 'c', valid letters are \"ab\"");

    REQUIRE_EXCEPTION_MSG(a.throw_if_letter_not_in_alphabet(0),
                          "invalid letter (char with value) 0, valid letters "
                          "are \"ab\" == [97, 98]");

    Alphabet b(word_type({0, 1}));
    REQUIRE_NOTHROW(b.throw_if_letter_not_in_alphabet(0));
    REQUIRE_NOTHROW(b.throw_if_letter_not_in_alphabet(1));
    REQUIRE_EXCEPTION_MSG(b.throw_if_letter_not_in_alphabet(2),
                          "invalid letter 2, valid letters are [0, 1]");

    word_type w = {0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1};
    REQUIRE_NOTHROW(b.throw_if_letter_not_in_alphabet(w.begin(), w.end()));
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet",
                          "006",
                          "add and remove letters",
                          "[quick]") {
    auto                  rg = ReportGuard(false);
    Alphabet<std::string> a("ab");
    a.add_letter('c');
    REQUIRE(a.letters() == "abc");
    REQUIRE(a.index('c') == 2);
    REQUIRE_EXCEPTION_MSG(a.add_letter('a'),
                          "the argument 'a' already belongs to the alphabet "
                          "\"abc\", expected an unused letter");

    a.remove_letter('b');
    REQUIRE(a.letters() == "ac");
    REQUIRE(a.index('c') == 1);
    REQUIRE(!a.contains('b'));
    REQUIRE_EXCEPTION_MSG(a.remove_letter('b'),
                          "the argument 'b' does not belong to the alphabet "
                          "\"ac\", expected an existing letter");
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet", "007", "first_unused_letter", "[quick]") {
    auto rg = ReportGuard(false);
    using words::human_readable_letter;
    Alphabet<std::string> alphabet("ab");

    REQUIRE(alphabet::first_unused_letter(alphabet) == 'c');
    alphabet.init("abcdefghijklmnopq");
    REQUIRE(alphabet::first_unused_letter(alphabet) == 'r');
    alphabet.init("abcdefghijklmnopqrstuvwxyz");
    REQUIRE(alphabet::first_unused_letter(alphabet) == 'A');
    alphabet.init("abcdefgijklmnopqrstuvwxyz");
    REQUIRE(alphabet::first_unused_letter(alphabet) == 'h');
    alphabet.init("abcdefghijklmnopqrstuvwxyzABCD"
                  "EFGHIJKLMNOPQRSTUVWXYZ");
    REQUIRE(alphabet::first_unused_letter(alphabet) == '0');
    alphabet.init("abcdefghijklmnopqrstuvwxyzABCD"
                  "EFGHIJKLMNOPQRSTUVWXYZ02");
    REQUIRE(alphabet::first_unused_letter(alphabet) == '1');
    std::string const letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHI"
                                "JKLMNOPQRSTUVWXYZ0123456789";
    std::unordered_set<letter_type> set;
    for (size_t i = 0; i < letters.size(); ++i) {
      REQUIRE(letters[i] == human_readable_letter(i));
      REQUIRE(set.insert(letters[i]).second);
    }
    for (size_t i = letters.size(); i < 256; ++i) {
      REQUIRE(set.insert(human_readable_letter(i)).second);
    }
    REQUIRE_THROWS_AS(human_readable_letter(256), LibsemigroupsException);
    alphabet.init(256);
    REQUIRE_THROWS_AS(alphabet::first_unused_letter(alphabet),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(alphabet.init(257), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet",
                          "008",
                          "add_letter (std::string)",
                          "[quick]") {
    auto            rg = ReportGuard(false);
    using literals::operator""_w;

    {
      Alphabet<std::string> alphabet("ab");
      alphabet::add_letter(alphabet);
      REQUIRE(alphabet.letters() == "abc");
    }
    {
      Alphabet<std::string> alphabet("ac");
      alphabet::add_letter(alphabet);
      REQUIRE(alphabet.letters() == "acb");
    }
    {
      Alphabet<std::string> alphabet("ac");
      alphabet.add_letter('b');
      REQUIRE(alphabet.letters() == "acb");
    }
    {
      Alphabet<std::string> alphabet("ac");
      REQUIRE_EXCEPTION_MSG(alphabet.add_letter('c'),
                            "the argument 'c' already belongs to the alphabet "
                            "\"ac\", expected an unused letter");
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet",
                          "009",
                          "add_letter (word_type)",
                          "[quick]") {
    auto rg = ReportGuard(false);
    {
      Alphabet<word_type> alphabet({0, 1});
      alphabet::add_letter(alphabet);
      REQUIRE(alphabet.letters() == word_type({0, 1, 2}));
    }
    {
      Alphabet<word_type> alphabet({0, 2});
      alphabet::add_letter(alphabet);
      REQUIRE(alphabet.letters() == word_type({0, 2, 1}));
    }
    {
      Alphabet<word_type> alphabet({0, 2});
      alphabet.add_letter(1);
      REQUIRE(alphabet.letters() == word_type({0, 2, 1}));
    }
    {
      Alphabet<word_type> alphabet({0, 2});
      REQUIRE_EXCEPTION_MSG(alphabet.add_letter(2),
                            "the argument 2 already belongs to the alphabet "
                            "[0, 2], expected an unused letter");
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Alphabet",
                                   "010",
                                   "remove_letter",
                                   "[quick]",
                                   std::string,
                                   word_type,
                                   StaticVector) {
    using W = TestType;
    using words::human_readable_letter;

    auto        rg = ReportGuard(false);
    Alphabet<W> alphabet(10);
    alphabet.remove_letter_no_checks(human_readable_letter<W>(4));
    alphabet.remove_letter(human_readable_letter<W>(7));
    alphabet.remove_letter_no_checks(human_readable_letter<W>(9));
    if constexpr (std::is_same_v<W, std::string>) {
      REQUIRE(alphabet.letters() == "abcdfgi");
    } else {
      REQUIRE(alphabet.letters() == W({0, 1, 2, 3, 5, 6, 8}));
    }
    REQUIRE(alphabet.index(human_readable_letter<W>(0)) == 0);
    REQUIRE(alphabet.index(human_readable_letter<W>(1)) == 1);
    REQUIRE(alphabet.index(human_readable_letter<W>(2)) == 2);
    REQUIRE(alphabet.index(human_readable_letter<W>(3)) == 3);
    REQUIRE_THROWS_AS(alphabet.index(human_readable_letter<W>(4)),
                      LibsemigroupsException);
    REQUIRE(alphabet.index(human_readable_letter<W>(5)) == 4);
    REQUIRE(alphabet.index(human_readable_letter<W>(6)) == 5);
    REQUIRE_THROWS_AS(alphabet.index(human_readable_letter<W>(7)),
                      LibsemigroupsException);
    REQUIRE(alphabet.index(human_readable_letter<W>(8)) == 6);
    REQUIRE_THROWS_AS(alphabet.index(human_readable_letter<W>(9)),
                      LibsemigroupsException);
    REQUIRE(alphabet.letter(0) == human_readable_letter<W>(0));
    REQUIRE(alphabet.letter(1) == human_readable_letter<W>(1));
    REQUIRE(alphabet.letter(2) == human_readable_letter<W>(2));
    REQUIRE(alphabet.letter(3) == human_readable_letter<W>(3));
    REQUIRE(alphabet.letter(4) == human_readable_letter<W>(5));
    REQUIRE(alphabet.letter(5) == human_readable_letter<W>(6));
    REQUIRE(alphabet.letter(6) == human_readable_letter<W>(8));

    REQUIRE_THROWS_AS(alphabet.remove_letter(human_readable_letter<W>(11)),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Alphabet",
                                   "011",
                                   "operator==/!=",
                                   "[quick]",
                                   word_type,
                                   StaticVector) {
    using W = TestType;
    Alphabet<W> alphabet1(5);
    Alphabet<W> alphabet2({0, 1, 2, 3, 4});
    REQUIRE(alphabet1 == alphabet2);
    alphabet2.init({1, 0, 2, 3, 4});
    REQUIRE(alphabet1 != alphabet2);
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet",
                          "012",
                          "operator==/!= - std::string",
                          "[quick]") {
    Alphabet<std::string> alphabet1(5);
    Alphabet<std::string> alphabet2("abcde");
    REQUIRE(alphabet1 == alphabet2);
    alphabet2.init("bacde");
    REQUIRE(alphabet1 != alphabet2);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Alphabet",
                                   "013",
                                   "to_input_string",
                                   "[quick]",
                                   word_type,
                                   StaticVector) {
    using W = TestType;
    Alphabet<W> alphabet(5);
    REQUIRE(to_input_string(alphabet) == "Alphabet({0, 1, 2, 3, 4})");
    REQUIRE(to_input_string(alphabet, "[]") == "Alphabet([0, 1, 2, 3, 4])");
    REQUIRE_EXCEPTION_MSG(
        std::ignore = to_input_string(alphabet, ""),
        "the 2nd argument (braces) must have length 2, but found  of length 0");
    std::ostringstream ss;
    ss << alphabet;
    REQUIRE(ss.str() == to_input_string(alphabet));
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet", "014", "to_input_string", "[quick]") {
    Alphabet<std::string> alphabet(5);
    REQUIRE(to_input_string(alphabet) == "Alphabet(\"abcde\")");
    alphabet.init({0, 1, 2, 3, 4});
    REQUIRE(to_input_string(alphabet) == "Alphabet({0, 1, 2, 3, 4})");
    std::ostringstream ss;
    ss << alphabet;
    REQUIRE(ss.str() == to_input_string(alphabet));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Alphabet",
                                   "015",
                                   "to_human_readable_repr",
                                   "[quick]",
                                   word_type,
                                   StaticVector) {
    using W = TestType;
    Alphabet<W> alphabet(5);
    REQUIRE(to_human_readable_repr(alphabet) == "<alphabet [0, 1, 2, 3, 4]>");
    alphabet.init(32);
    REQUIRE(to_human_readable_repr(alphabet) == "<alphabet with 32 letters>");
    alphabet.init(0);
    REQUIRE(to_human_readable_repr(alphabet) == "<alphabet with 0 letters>");
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet",
                          "016",
                          "to_human_readable_repr",
                          "[quick]") {
    Alphabet<std::string> alphabet(5);
    REQUIRE(to_human_readable_repr(alphabet) == "<alphabet \"abcde\">");
    alphabet.init({0, 1, 2, 3, 4});
    REQUIRE(to_human_readable_repr(alphabet)
            == "<alphabet (char values) [0, ..., 4]>");
    alphabet.init({4, 3, 0, 10, 13});
    REQUIRE(to_human_readable_repr(alphabet)
            == "<alphabet (char values) [4, 3, 0, 10, 13]>");
    alphabet.init(200);
    REQUIRE(to_human_readable_repr(alphabet) == "<alphabet with 200 letters>");
  }

  LIBSEMIGROUPS_TEST_CASE("Alphabet", "017", "to<Alphabet>", "[quick]") {
    Alphabet<std::string> alphabet(5);

    REQUIRE(to<Alphabet<word_type>>(alphabet).letters()
            == word_type({0, 1, 2, 3, 4}));
    REQUIRE(to<Alphabet<std::string>>(to<Alphabet<word_type>>(alphabet))
            == alphabet);
    REQUIRE(to<Alphabet<std::string>>(alphabet) == alphabet);
    REQUIRE(to<Alphabet<word_type>>(to<Alphabet<word_type>>(alphabet))
            == to<Alphabet<word_type>>(alphabet));
  }
}  // namespace libsemigroups
