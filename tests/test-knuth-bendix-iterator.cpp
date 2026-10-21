// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

// This file contains tests for the class KnuthBendixIterator

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEMPLATE_TEST_CASE

#include <type_traits>  // for is_default_constructible_v, is_copy_constructi...
#include <vector>       // for vector

#include "libsemigroups/adapters.hpp"               // for ReturnFalse
#include "libsemigroups/knuth-bendix-iterator.hpp"  // for Knuth...
#include "libsemigroups/presentation.hpp"  // for Presentation, presentation...
#include "libsemigroups/to-word.hpp"       // for ToWord
#include "libsemigroups/word-range-class.hpp"  // for WordRange

#include "libsemigroups/detail/rewriting-system.hpp"  // for RewritingSystemTrie

namespace libsemigroups {
  using std::literals::operator""s;
  using literals::     operator""_w;

  template <typename = Default, bool = false>
  struct NoOrder : public ReturnFalse {
    void init() const noexcept {}
  };

  template <typename X>
  void is_input_iterator() {
    using T = typename std::iterator_traits<X>::value_type;
    using R = typename std::iterator_traits<X>::reference;

    // LegacyIterator requirements
    static_assert(std::is_copy_constructible_v<X>,
                  "input_iterator must be copy-constructible");
    static_assert(std::is_copy_assignable_v<X>,
                  "input_iterator must be copy-assignable");
    static_assert(std::is_destructible_v<X>,
                  "input_iterator must be destructible");
    static_assert(std::is_swappable_v<X>, "input_iterator must be swappable");
    static_assert(!std::is_void_v<decltype(*std::declval<X&>())>,
                  "input_iterator must be dereferenceable");
    static_assert(!std::is_void_v<decltype(++std::declval<X&>())>,
                  "input_iterator must be incrementable");
    static_assert(std::is_same_v<decltype(++std::declval<X&>()), X&>,
                  "the return type of an incremented X& must be X&");

    // LegacyInputIterator requirements, non-const
    static_assert(std::is_same_v<decltype(*std::declval<X&>()), R>,
                  "a dereferenced input_iterator's type must agree with the "
                  "iterator_trait's reference type");
    static_assert(std::is_convertible_v<decltype(*std::declval<X&>()), T>,
                  "a dereferenced input_iterator's type must be convertible to "
                  "the iterator_trait's value type");
    static_assert(std::is_convertible_v<decltype(*std::declval<X&>()++), T>,
                  "a dereferenced incremented input_iterator must be "
                  "convertible to the iterator_trait's value type");

    // LegacyInputIterator requirements, const
    static_assert(
        std::is_same_v<decltype(*std::declval<const X&>()), R>,
        "a dereferenced const input_iterator's type must agree with the "
        "iterator_trait's reference type");
    static_assert(
        std::is_convertible_v<decltype(*std::declval<const X&>()), T>,
        "a dereferenced const input_iterator's type must be convertible to "
        "the iterator_trait's value type");

    // runtime checks
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixIterator",
                          "000",
                          "simple test 0",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abc"s);
    presentation::add_rule(p, "baa"s, "c"s);
    presentation::add_rule(p, "aba"s, "cc"s);

    KnuthBendixIterator       kbb(p, 100, 50);
    KnuthBendixIterator const end = end_knuth_bendix_backtrack(p, 100, 50);

    std::vector<std::vector<std::string>> expected_rules{
        {"baa",
         "c",
         "aba",
         "cc",
         "ac",
         "cca",
         "bcccca",
         "cba",
         "abcc",
         "ccba",
         "bcccccc",
         "cbcc"},
        {"baa",
         "c",
         "aba",
         "cc",
         "ac",
         "cca",
         "cba",
         "bcccca",
         "abcc",
         "cbcccca",
         "bccccaa",
         "cc",
         "bcccccc",
         "cbcc",
         "abcbcccca",
         "ccbcc",
         "cbcbcccca",
         "bcccccbcccca",
         "abcbcccccbcccca",
         "ccbccbcc",
         "cbcccccbcccccbcccca",
         "ccbcbcccccbcccca",
         "bcccccbcccccbcccca",
         "cbcbcccccbcccca"},
        {"baa",
         "c",
         "aba",
         "cc",
         "ac",
         "cca",
         "cba",
         "bcccca",
         "abcc",
         "cbcccca",
         "bccccaa",
         "cc",
         "bcccccc",
         "cbcc",
         "abcbcccca",
         "ccbcc",
         "bcccccbcccca",
         "cbcbcccca"},
    };

    size_t index = 0;

    while (kbb != end) {
      REQUIRE(kbb->rules == expected_rules[index]);
      ++kbb;
      ++index;
    }

    REQUIRE(index == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixIterator",
                          "001",
                          "simple test 1",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("ab"s);
    presentation::add_rule(p, "a"s, "b"s);
    KnuthBendixIterator kbb(p, 100, 10);

    Presentation<std::string> expected;
    expected.alphabet("ab"s);
    expected.rules = {"a", "b"};

    REQUIRE(*kbb == expected);

    ++kbb;
    expected.rules = {"b", "a"};
    REQUIRE(*kbb == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixIterator",
                          "002",
                          "constructors",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("ab"s);
    presentation::add_rule(p, "a"s, "b"s);

    KnuthBendixIterator kbb(p, 100, 10);
    KnuthBendixIterator copy(kbb);
    REQUIRE(copy == kbb);
    REQUIRE(*copy == *kbb);
    REQUIRE(copy.operator->() != kbb.operator->());

    KnuthBendixIterator moved(std::move(copy));
    REQUIRE(moved == kbb);
    REQUIRE(*moved == *kbb);

    p.alphabet("abc"s);
    presentation::add_rule(p, "b"s, "c"s);
    REQUIRE(kbb->alphabet() == "ab");
    REQUIRE(kbb->rules == std::vector<std::string>{"a", "b"});
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixIterator",
                          "003",
                          "accessors and comparison",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("ab"s);
    presentation::add_rule(p, "a"s, "b"s);

    KnuthBendixIterator kbb_1(p, 100, 10);
    KnuthBendixIterator kbb_2(p, 100, 10);

    auto const& presentation = *kbb_1;
    REQUIRE(kbb_1.operator->() == &presentation);
    REQUIRE(kbb_1->alphabet() == "ab");
    REQUIRE(kbb_1->rules == std::vector<std::string>{"a", "b"});

    REQUIRE(kbb_1 == kbb_2);
    REQUIRE(!(kbb_1 != kbb_2));

    // Forward iterator multi-pass guarantee
    kbb_1++;
    kbb_2++;
    REQUIRE(kbb_1 == kbb_2);
    bool res{((void) [](auto x) { ++x; }(kbb_1), *kbb_1) == *kbb_1};
    REQUIRE(res);

    auto const before = kbb_1++;
    REQUIRE(before == kbb_2);
    REQUIRE(before != kbb_1);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixIterator",
                          "004",
                          "confluence",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abc"s);
    presentation::add_rule(p, "baa"s, "c"s);
    presentation::add_rule(p, "aba"s, "cc"s);

    v4::ToWord to_word("abc");

    KnuthBendixIterator kbb = begin_knuth_bendix_backtrack(p, 100, 50);
    KnuthBendixIterator end = end_knuth_bendix_backtrack(p, 100, 50);
    detail::RewritingSystemTrie<NoOrder> rws;

    while (kbb != end) {
      rws.init();
      rws.increase_alphabet_size_by(3);
      for (size_t i = 0; i < kbb->rules.size(); i += 2) {
        detail::rewriting_system::add_rule(
            rws, to_word(kbb->rules[i]), to_word(kbb->rules[i + 1]));
      }
      REQUIRE(rws.confluent());
      ++kbb;
    }
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixIterator",
                          "005",
                          "one relation monoids",
                          "[extreme]") {
    std::string const first_word("a");
    std::string const last_word("aaaaaaaa");

    v4::WordRange<std::string> lhss;
    lhss.order(LenLexCmp(Alphabet("ab"s))).first(first_word).last(last_word);
    v4::WordRange<std::string> rhss;

    Presentation<std::string> p;
    p.alphabet("ab"s);
    p.contains_empty_word(true);

    KnuthBendixIterator       kbb;
    KnuthBendixIterator const end = end_knuth_bendix_backtrack(p, 20, 10);

    size_t total   = 0;
    size_t success = 0;

    for (auto const& lhs : lhss) {
      rhss.order(LenLexCmp(Alphabet("ab"s))).first(lhs).last(last_word);
      for (auto const& rhs : rhss) {
        ++total;
        p.rules = {rhs, lhs};
        kbb     = KnuthBendixIterator(p, 20, 10);
        if (kbb != end) {
          ++success;
        }
      }
    }
    REQUIRE(total == 32385);
    REQUIRE(success == 27407);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixIterator",
                          "006",
                          "confluence x2",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd"s);
    presentation::add_rule(p, "ab"s, "d"s);
    presentation::add_rule(p, "bc"s, "d"s);

    v4::ToWord to_word("abcd");

    KnuthBendixIterator kbb = begin_knuth_bendix_backtrack(p, 100, 50);
    KnuthBendixIterator end = end_knuth_bendix_backtrack(p, 100, 50);
    detail::RewritingSystemTrie<NoOrder> rws;

    while (kbb != end) {
      rws.init();
      rws.increase_alphabet_size_by(4);
      for (size_t i = 0; i < kbb->rules.size(); i += 2) {
        detail::rewriting_system::add_rule(
            rws, to_word(kbb->rules[i]), to_word(kbb->rules[i + 1]));
      }
      REQUIRE(rws.confluent());
      ++kbb;
    }
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixIterator",
                          "007",
                          "empty words",
                          "[quick]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("a"s);
    presentation::add_rule(p, "a"s, ""s);

    KnuthBendixIterator       kbb(p, 100, 20);
    KnuthBendixIterator const end = end_knuth_bendix_backtrack(p, 100, 20);

    size_t index = 0;
    while (kbb != end) {
      REQUIRE(kbb->rules == std::vector<std::string>{"a"s, ""s});
      ++kbb;
      ++index;
    }

    REQUIRE(index == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixIterator",
                          "008",
                          "input iterator",
                          "[quick]") {
    // static checks
    is_input_iterator<KnuthBendixIterator>();

    // runtime checks

    Presentation<std::string> p;
    p.alphabet("abc"s);
    presentation::add_rule(p, "baa"s, "c"s);
    presentation::add_rule(p, "aba"s, "cc"s);

    KnuthBendixIterator       kbb(p, 100, 50);
    KnuthBendixIterator       copy_constructed_kbb(kbb);
    KnuthBendixIterator       copy_assigned_kbb = kbb;
    KnuthBendixIterator const end = end_knuth_bendix_backtrack(p, 100, 50);

    SECTION("Satisfies EqualityComparable") {
      // Reflexive
      REQUIRE(kbb == kbb);

      // Symmetric
      REQUIRE(kbb == copy_assigned_kbb);
      REQUIRE(copy_assigned_kbb == kbb);
      REQUIRE(kbb == copy_constructed_kbb);
      REQUIRE(copy_constructed_kbb == kbb);

      // Transitive
      if (copy_assigned_kbb == copy_constructed_kbb) {
        REQUIRE(kbb == copy_constructed_kbb);
      }
    }

    SECTION("Valid equality in the domain of ==") {
      // If i and j are in the domain of ==, then `i != j` is equivalent to
      // `!(i == j)`
      REQUIRE((kbb != kbb) == !(kbb == kbb));
      REQUIRE((kbb != copy_assigned_kbb) == !(kbb == copy_assigned_kbb));
      REQUIRE((kbb != copy_constructed_kbb) == !(kbb == copy_constructed_kbb));
      REQUIRE((kbb != end) == !(kbb == end));
    }

    SECTION("Valid dereferencing") {
      // If i is dereferenceable, then the expression `(void)*i, *i` is
      // equivalent to `*i`.
      REQUIRE(((void) *kbb, *kbb) == (*kbb));

      // If i and j are in the domain of ==, and i == j, then `*i` is equivalent
      // to `*j`.
      REQUIRE(*kbb == *copy_assigned_kbb);
      REQUIRE(*kbb == *copy_constructed_kbb);
    }

    SECTION("Valid arrow operator") {
      // If i is dereferenceable, then `i->m` is equivalent to `(*i).m`.
      REQUIRE(kbb->rules == (*kbb).rules);
    }

    SECTION("Valid incrementing") {
      // `(void)r++` is equivalent to `(void)++r`
      KnuthBendixIterator& kbb1(kbb);
      KnuthBendixIterator& kbb2(kbb);
      REQUIRE(kbb1 == kbb2);
      ++kbb1;
      kbb2++;
      REQUIRE(kbb1 == kbb2);

      // `*r++` is equivalent to `T x = *r; ++r; return x;`
      KnuthBendixIterator&                                  kbb3(kbb);
      KnuthBendixIterator&                                  kbb4(kbb);
      std::iterator_traits<KnuthBendixIterator>::value_type p = *kbb3;
      ++kbb3;
      REQUIRE(*kbb4++ == p);
    }
  }

}  // namespace libsemigroups
