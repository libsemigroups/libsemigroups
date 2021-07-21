//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell
//                    Reinis Cirpons
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

#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "bench-main.hpp"  // for LIBSEMIGROUPS_BENCHMARK
#include "catch.hpp"       // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/freeband.hpp"  // for FreeBan`
#include "libsemigroups/types.hpp"     // for word_type

namespace libsemigroups {

  word_type random_word(size_t length, size_t nr_letters) {
    static std::random_device               rd;
    static std::mt19937                     gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, nr_letters);
    word_type                               out;
    for (size_t i = 0; i < length; ++i) {
      out.push_back(dist(gen));
    }
    return out;
  }

  void random_tree_word_helper(std::vector<size_t>& cont,
                               word_type&           out,
                               size_t               padding) {
    if (cont.size() == 1) {
      out.push_back(cont[0]);
      for (size_t i = 0; i < padding; i++)
        out.push_back(cont[0]);
    } else {
      // left half
      size_t l = cont.back();
      // left subtree
      cont.pop_back();
      random_tree_word_helper(cont, out, padding);
      // left letter
      out.push_back(l);
      cont.push_back(l);
      // padding
      if (padding > 0) {
        word_type pad = random_word(padding, cont.size() - 1);
        for (size_t i = 0; i < pad.size(); i++)
          pad[i] = cont[pad[i]];
        out.insert(out.end(), pad.begin(), pad.end());
      }
      // right half
      word_type           right;
      std::vector<size_t> right_cont(cont.size());
      std::copy(cont.begin(), cont.end(), right_cont.begin());
      std::random_shuffle(right_cont.begin(), right_cont.end());
      size_t r = right_cont.back();
      // right letter
      out.push_back(r);
      // right subtree
      right_cont.pop_back();
      random_tree_word_helper(right_cont, right, padding);
      out.insert(out.end(), right.rbegin(), right.rend());
    }
  }

  word_type random_tree_word(size_t nr_letters, size_t padding) {
    word_type           out;
    std::vector<size_t> cont;
    for (size_t i = 0; i < nr_letters; i++)
      cont.push_back(i);
    std::random_shuffle(cont.begin(), cont.end());
    random_tree_word_helper(cont, out, padding);
    return out;
  }

  TEST_CASE("random words (length)", "[quick][000]") {
    size_t              a = 50;
    std::vector<size_t> L = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    for (auto const& l : L) {
      auto x = random_word(l, a), y = random_word(l, a);
      BENCHMARK("Random Word, Alphabet " + std::to_string(a) + " Length "
                + std::to_string(l)) {
        for (size_t i = 0; i < 100 / a; ++i) {
          freeband_equal_to(x, y);
        }
      };
    }
  }

  TEST_CASE("random words (alphabet)", "[quick][000]") {
    std::vector<size_t> A = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
    size_t              l = 1000;
    for (auto const& a : A) {
      auto x = random_word(l, a), y = random_word(l, a);
      BENCHMARK("Random Word, Alphabet " + std::to_string(a) + " Length "
                + std::to_string(l)) {
        for (size_t i = 0; i < 100 / a; ++i) {
          freeband_equal_to(x, y);
        }
      };
    }
  }

  TEST_CASE("random words (alphabet and length)", "[standard][000]") {
    std::vector<size_t> A = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
    std::vector<size_t> L = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    for (auto const& a : A)
      for (auto const& l : L) {
        auto x = random_word(l, a), y = random_word(l, a);
        BENCHMARK("Random Word, Alphabet " + std::to_string(a) + " Length "
                  + std::to_string(l)) {
          for (size_t i = 0; i < 100 / a; ++i) {
            freeband_equal_to(x, y);
          }
        };
      }
  }

  TEST_CASE("unpadded random tree words", "[quick][001]") {
    std::vector<size_t> A = {5, 6, 7, 8, 9, 10};
    for (auto const& a : A) {
      auto x = random_tree_word(a, 0), y = random_tree_word(a, 0);
      BENCHMARK("Random Tree Word, Alphabet " + std::to_string(a)) {
        freeband_equal_to(x, y);
      };
    }
  }

  TEST_CASE("padded random tree words", "[quick][002]") {
    std::vector<size_t> A = {5, 6, 7, 8, 9, 10};
    std::vector<size_t> P = {0, 5, 10, 15};
    for (auto const& a : A)
      for (auto const& p : P) {
        auto x = random_tree_word(a, p), y = random_tree_word(a, p);
        BENCHMARK("Random Tree Word, Alphabet " + std::to_string(a) + " Length "
                  + std::to_string(x.size() + y.size())) {
          freeband_equal_to(x, y);
        };
      }
  }

}  // namespace libsemigroups
