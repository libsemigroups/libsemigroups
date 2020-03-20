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

// This file contains declarations for function related to words (counting, and
// converting) in libsemigroups.

#ifndef LIBSEMIGROUPS_WORD_HPP_
#define LIBSEMIGROUPS_WORD_HPP_

#include <array>    // for std::array
#include <cstddef>  // for size_t
#include <string>   // for std::string

#include "types.hpp"  // for word_type

namespace libsemigroups {
  // TODO doc
  size_t number_of_words(size_t nr_gens, size_t min, size_t max);

  // TODO doc
  void word_to_string(std::string const& alphabet,
                      word_type const&   input,
                      std::string&       output);

  // TODO doc, lots
  class StringToWord {
   public:
    explicit StringToWord(std::string const& alphabet) : _lookup() {
      _lookup.fill(0);
      for (letter_type l = 0; l < alphabet.size(); ++l) {
        _lookup[alphabet[l]] = l;
      }
    }
    // TODO doc
    void      operator()(std::string const& input, word_type& output) const;
    word_type operator()(std::string const& input) const;

   private:
    std::array<letter_type, 256> _lookup;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_WORD_HPP_
