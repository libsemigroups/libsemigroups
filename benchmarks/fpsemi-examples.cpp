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

#include "fpsemi-examples.hpp"

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "libsemigroups/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {
  namespace {
    template <typename T>
    inline void hash_combine(size_t& s, T const& v) {
      std::hash<T> h;
      s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
    }

    template <typename T>
    inline size_t hash_combine(std::vector<T> const& v) {
      size_t s = 0;
      for (auto const& x : v) {
        hash_combine(s, x);
      }
      return s;
    }

    template <typename T>
    size_t create_id(T const& x) {
      using item_type          = decltype(x.A);
      std::vector<item_type> A = {x.A};
      auto                   R = x.R;
      for (auto& p : R) {
        A.push_back(p.first);
        A.push_back(p.second);
      }
      return hash_combine(A);
    }
  }  // namespace

  StringPresentation const& string_infinite_examples(size_t id) {
    for (auto const& p : string_infinite_examples()) {
      if (p.id == id) {
        return p;
      }
    }
    LIBSEMIGROUPS_EXCEPTION("id %llu unknown!", id);
  }

  std::vector<StringPresentation> const& string_infinite_examples() {
    static std::vector<StringPresentation> examples = {
        {.id   = 0x6BA1D9229DC22AFB,
         .name = "KnuthBendix 004",
         .A    = "012",
         .R    = {{"01", "10"},
               {"02", "20"},
               {"00", "0"},
               {"02", "0"},
               {"20", "0"},
               {"11", "11"},
               {"12", "21"},
               {"111", "1"},
               {"12", "1"},
               {"21", "1"},
               {"0", "1"}}},
        {.id   = 0x222CC398A1FBBC47,
         .name = "KnuthBendix 007 (wikipedia)",
         .A    = "01",
         .R    = {{"000", ""}, {"111", ""}, {"010101", ""}}},
        {.id   = 0xB558B78731C80849,
         .name = "Example 5.1 in Sims",
         .A    = "aAbB",
         .R = {{"aA", ""}, {"Aa", ""}, {"bB", ""}, {"Bb", ""}, {"ba", "ab"}}},
        {.id   = 0xB3E816A734D092E0,
         .name = "Chapter 10, Section 4 in NR",
         .A    = "abc",
         .R    = {{"aaaa", "a"},
               {"bbbb", "b"},
               {"cccc", "c"},
               {"abab", "aaa"},
               {"bcbc", "bbb"}}},
        {.id   = 0x88F91469E1FD9427,
         .name = "Bicyclic monoid",
         .A    = "bc",
         .R    = {{"bc", ""}}},
        {.id   = 0xE98EAA9E59EADB12,
         .name = "Plactic monoid degree 2",
         .A    = "abc",
         .R    = {{"aba", "baa"},
               {"bba", "bab"},
               {"ac", ""},
               {"ca", ""},
               {"bc", ""},
               {"cb", ""}}},
        {.id   = 0x9AB44C55E7EF3FE6,
         .name = "Chapter 7, Proposition 1.1 in NR",
         .A    = "ab",
         .R    = {{"aa", "a"}, {"bb", "b"}}},
        {.id   = 0x108096C3079B0302,
         .name = "Chapter 11, Lemma 1.8 (q = 6, r = 5) in NR",
         .A    = "ABCabc",
         .R    = {{"aA", ""},
               {"Aa", ""},
               {"bB", ""},
               {"Bb", ""},
               {"cC", ""},
               {"Cc", ""},
               {"aa", ""},
               {"bbb", ""},
               {"abaBaBabaBab", ""}}},
        {.id   = 0x327F33C8DC363DEA,
         .name = "Chapter 8, Theorem 4.2 in NR",
         .A    = "ab",
         .R    = {{"aaa", "a"},
               {"bbbb", "b"},
               {"bababababab", "b"},
               {"baab", "babbbab"}}},
        {.id   = 0x9E13A24681757CC,
         .name = "GAP smalloverlap 1",
         .A    = "abcdefg",
         .R    = {{"abcd", "ce"}, {"df", "dg"}}},
        {.id   = 0x84B2E77D4EA9AEFF,
         .name = "GAP smalloverlap 2",
         .A    = "abcdefgh",
         .R    = {{"abcd", "ce"}, {"df", "hd"}}},
        {.id   = 0x5AFDDD8E430D1787,
         .name = "GAP smalloverlap 3",
         .A    = "abcdefgh",
         .R    = {{"afh", "bgh"}, {"hc", "d"}}},
        {.id   = 0xC24BAB32F318E8B9,
         .name = "GAP smalloverlap 4",
         .A    = "abcdefghij",
         .R    = {{"afh", "bgh"}, {"hc", "de"}, {"ei", "j"}}},
        {.id   = 0x6578EB84FB90291,
         .name = "GAP smalloverlap 5",
         .A    = "abcdefghijkl",
         .R    = {{"afh", "bgh"}, {"hc", "de"}, {"ei", "j"}, {"fhk", "ghl"}}},
        {.id   = 0xB31C21EC18298FC5,
         .name = "Von Dyck (2,3,7) group",
         .A    = "ABabc",
         .R    = {{"aaaa", "AAA"}, {"bb", "B"}, {"BA", "c"}}},
        {.id   = 0x777131204E94A930,
         .name = "KnuthBendix 040",
         .A    = "abc",
         .R    = {{"bbbbbbb", "b"},
               {"ccccc", "c"},
               {"bccba", "bccb"},
               {"bccbc", "bccb"},
               {"bbcbca", "bbcbc"},
               {"bbcbcb", "bbcbc"}}},
        {.id   = 0x7FC4713FF479BDD8,
         .name = "KnuthBendix 041",
         .A    = "ab",
         .R    = {{"aaa", "a"}, {"ab", "ba"}, {"aa", "a"}}},
        {.id   = 0xA5CA113505A6EEFB,
         .name = "kbmag/standalone/kb_data/ab2",
         .A    = "aAbB",
         .R    = {{"Bab", "a"}}},
        {.id   = 0xCEA8D789D5C595C2,
         .name = "kbmag/standalone/kb_data/torus",
         .A    = "aAcCbBdD",
         .R    = {{"ABab", "DCdc"}}},
        {.id   = 0x1565E7D947EC2828,
         .name = "KnuthBendix 079, Von Dyck (2,3,7) group",
         .A    = "aAbBc",
         .R    = {{"aA", ""},
               {"Aa", ""},
               {"Aa", ""},
               {"aA", ""},
               {"bB", ""},
               {"Bb", ""},
               {"Bb", ""},
               {"bB", ""},
               {"cc", ""},
               {"cc", ""},
               {"BA", "c"},
               {"Bb", "bB"},
               {"bb", "B"},
               {"AAAa", "aAAA"},
               {"aaaa", "AAA"},
               {"BaAAA", "cAAa"},
               {"BaaAAA", "cAAaa"},
               {"BaAaAAA", "cAAaAa"},
               {"BaaaAAA", "cAAaaa"},
               {"BaAAaAAA", "cAAaAAa"},
               {"BaAaaAAA", "cAAaAaa"},
               {"BaaAaAAA", "cAAaaAa"},
               {"BaAAaaAAA", "cAAaAAaa"},
               {"BaAaAaAAA", "cAAaAaAa"},
               {"BaAaaaAAA", "cAAaAaaa"},
               {"BaaAAaAAA", "cAAaaAAa"},
               {"BaaAaaAAA", "cAAaaAaa"},
               {"BaAAaAaAAA", "cAAaAAaAa"},
               {"BaAAaaaAAA", "cAAaAAaaa"},
               {"BaAaAAaAAA", "cAAaAaAAa"},
               {"BaAaAaaAAA", "cAAaAaAaa"},
               {"BaAaaAaAAA", "cAAaAaaAa"},
               {"BaaAAaaAAA", "cAAaaAAaa"},
               {"BaaAaAaAAA", "cAAaaAaAa"},
               {"BaAAaAAaAAA", "cAAaAAaAAa"},
               {"BaAAaAaaAAA", "cAAaAAaAaa"},
               {"BaAAaaAaAAA", "cAAaAAaaAa"},
               {"BaAaAAaaAAA", "cAAaAaAAaa"},
               {"BaAaAaAaAAA", "cAAaAaAaAa"},
               {"BaAaaAAaAAA", "cAAaAaaAAa"},
               {"BaaAAaAaAAA", "cAAaaAAaAa"},
               {"BaaAaAAaAAA", "cAAaaAaAAa"},
               {"BaAAaAAaaAAA", "cAAaAAaAAaa"},
               {"BaAAaAaAaAAA", "cAAaAAaAaAa"},
               {"BaAAaaAAaAAA", "cAAaAAaaAAa"},
               {"BaAaAAaAaAAA", "cAAaAaAAaAa"},
               {"BaAaAaAAaAAA", "cAAaAaAaAAa"},
               {"BaaAAaAAaAAA", "cAAaaAAaAAa"},
               {"BaAAaAAaAaAAA", "cAAaAAaAAaAa"},
               {"BaAAaAaAAaAAA", "cAAaAAaAaAAa"},
               {"BaAaAAaAAaAAA", "cAAaAaAAaAAa"},
               {"BaAAaAAaAAaAAA", "cAAaAAaAAaAAa"}}}};
    // std::cout << "0x" << std::hex << std::uppercase
    //           << create_id(examples.back()) << std::endl;
    return examples;
  }
}  // namespace libsemigroups
