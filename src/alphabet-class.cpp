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

// This file contains implementations of the non-template functions declared in
// alphabet-class.hpp.

#include "libsemigroups/alphabet-class.hpp"

#include <algorithm>  // for all_of
#include <cctype>     // for isprint
#include <string>     // for string
#include <vector>     // for vector

#include "libsemigroups/detail/fmt.hpp"  // for format, join

namespace libsemigroups {

  std::string to_input_string(Alphabet<std::string> const& alphabet,
                              std::string const&           braces) {
    if (braces.size() != 2) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd argument (braces) must have length 2, "
                              "but found {} of length {}",
                              braces,
                              braces.size());
    }
    if (std::all_of(alphabet.letters().begin(),
                    alphabet.letters().end(),
                    [](auto val) { return std::isprint(val); })) {
      return fmt::format("Alphabet(\"{}\")", alphabet.letters());
    }
    return fmt::format("Alphabet({}{}{})",
                       braces[0],
                       fmt::join(std::vector<int>(alphabet.letters().begin(),
                                                  alphabet.letters().end()),
                                 ", "),
                       braces[1]);
  }
}  // namespace libsemigroups
