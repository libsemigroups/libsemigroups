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

  word_type random_tree_word(size_t nr_letters)
  {
    static std::random_device               rd;
    static std::mt19937                     gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, nr_letters);
    word_type                               out;
    
  }

  TEST_CASE("random words against themselves", "[quick][000]") {
    std::vector<size_t> A = {4, 10, 50};
    std::vector<size_t> L = {100, 1000, 5000};
    size_t              N = 100;

    for (auto const& a : A)
      for (auto const& l : L)
        BENCHMARK("Random Word, Alphabet " + std::to_string(a) + " Length "
                  + std::to_string(l)) {
          for (size_t i = 0; i < N; i++) {
            word_type w = random_word(l, a);
            REQUIRE(freeband_equal_to(w, w));
          }
        };
  }

}  // namespace libsemigroups
