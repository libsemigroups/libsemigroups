//
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

// This file contains the implementations of the functionality declared in
// to-word.hpp.

#include "libsemigroups/to-word.hpp"

#include <algorithm>  // for lexicographical_...
#include <string>     // for allocator
#include <utility>    // for move

#include "libsemigroups/order.hpp"  // for order

#include "libsemigroups/detail/formatters.hpp"  // for magic_enum formatting

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // 3. Words -> Strings
  ////////////////////////////////////////////////////////////////////////

  ToString::ToString(ToString const&)            = default;
  ToString::ToString(ToString&&)                 = default;
  ToString& ToString::operator=(ToString const&) = default;
  ToString& ToString::operator=(ToString&&)      = default;
  ToString::~ToString()                          = default;

  ToString& ToString::init(std::string const& alphabet) {
    if (alphabet.size() > 256) {
      LIBSEMIGROUPS_EXCEPTION(
          "The argument (alphabet) is too big, expected at most 256, found {}",
          alphabet.size());
    }
    auto _old_alphabet_map = _alphabet_map;
    init();
    LIBSEMIGROUPS_ASSERT(_alphabet_map.empty());
    for (letter_type i = 0; i < alphabet.size(); ++i) {
      auto it = _alphabet_map.emplace(i, alphabet[i]);
      if (!it.second) {
        std::swap(_old_alphabet_map, _alphabet_map);
        LIBSEMIGROUPS_EXCEPTION("invalid alphabet {}, duplicate letter {}!",
                                detail::to_printable(alphabet),
                                detail::to_printable(alphabet[i]));
      }
    }
    return *this;
  }

  std::string ToString::alphabet() const {
    if (empty()) {
      return "";
    }

    std::string output(_alphabet_map.size(), '\0');
    for (auto it : _alphabet_map) {
      output[it.first] = it.second;
    }
    return output;
  }

  void ToString::call_no_checks(std::string&     output,
                                word_type const& input) const {
    // Empty alphabet implies conversion should use human_readable_index
    if (empty()) {
      output.resize(input.size(), 0);
      std::transform(
          input.cbegin(), input.cend(), output.begin(), [](letter_type c) {
            return words::human_readable_letter<>(c);
          });
    } else {  // Non-empty alphabet implies conversion should use the alphabet.
      output.clear();
      output.reserve(input.size());
      for (letter_type const& l : input) {
        output.push_back(_alphabet_map.at(l));
      }
    }
  }

  void ToString::operator()(std::string& output, word_type const& input) const {
    if (!empty()) {
      for (letter_type const& l : input) {
        if (_alphabet_map.find(l) == _alphabet_map.cend()) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid letter \'{}\' in the 2nd argument (input word), "
              "expected letters in the range  [0, {})!",
              l,
              _alphabet_map.size());
        }
      }
    }
    call_no_checks(output, input);
  }
}  // namespace libsemigroups
