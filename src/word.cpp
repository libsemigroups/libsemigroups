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
//

// This file contains some functionality missing in some implementations of the
// stl, or to augment the stl implementations.

#include "libsemigroups/word.hpp"

#include <cmath>    // for std::pow
#include <cstring>  // for std::strlen

#include "libsemigroups/config.hpp"     // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/int-range.hpp"  // for IntegralRange
#include "libsemigroups/types.hpp"      // for word_type

namespace libsemigroups {
  namespace {
    uint64_t geometric_progression(size_t n, size_t a, size_t r) {
      return a * ((1 - std::pow(r, n)) / (1 - static_cast<float>(r)));
    }
  }  // namespace

  uint64_t number_of_words(size_t n, size_t min, size_t max) {
    if (max <= min) {
      return 0;
    }
    return geometric_progression(max, 1, n) - geometric_progression(min, 1, n);
  }

  void detail::word_to_string(std::string const& alphabet,
                              word_type const&   input,
                              std::string&       output) {
    output.clear();
    output.reserve(input.size());
    for (auto const x : input) {
      output.push_back(alphabet[x]);
    }
  }

  void detail::StringToWord::operator()(std::string const& input,
                                        word_type&         output) const {
    output.clear();
    output.reserve(input.size());
    for (auto const& c : input) {
      output.push_back(_lookup[c]);
    }
  }

  word_type detail::StringToWord::operator()(std::string const& input) const {
    word_type output;
              operator()(input, output);
    return output;
  }

  namespace literals {
    word_type operator"" _w(const char* w, size_t n) {
      word_type result;
#ifdef LIBSEMIGROUPS_DEBUG
      static const std::string valid_chars = "0123456789";
#endif
      for (size_t i = 0; i < n; ++i) {
        LIBSEMIGROUPS_ASSERT(valid_chars.find(w[i]) != std::string::npos);
        result.push_back(static_cast<letter_type>(w[i] - 48));
      }
      return result;
    }

    word_type operator"" _w(const char* w) {
      return operator"" _w(w, std::strlen(w));
    }
  }  // namespace literals

}  // namespace libsemigroups
