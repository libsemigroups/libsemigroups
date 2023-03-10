//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 Murray T. Whyte
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

// This file contains

#include <cstddef>  // for size_t

#include "libsemigroups/present.hpp"

namespace libsemigroups {
  namespace presentation {

    word_type operator+(word_type const& u, word_type const& w) {
      word_type result(u);
      result.insert(result.end(), w.cbegin(), w.cend());
      return result;
    }

    word_type operator+(word_type const& u, size_t w) {
      word_type result(u);
      result.push_back(w);
      return result;
    }

    void operator+=(word_type& u, word_type const& v) {
      u.insert(u.end(), v.cbegin(), v.cend());
    }

    std::string pow(char const* w, size_t n) {
      return pow(std::string(w), n);
    }

    word_type pow(std::initializer_list<size_t> ilist, size_t n) {
      return pow(word_type(ilist), n);
    }

    word_type prod(std::initializer_list<size_t> ilist,
                   size_t                        first,
                   size_t                        last,
                   size_t                        step) {
      return prod(word_type(ilist), first, last, step);
    }
  }  // namespace presentation
}  // namespace libsemigroups
