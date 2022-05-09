//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#include "libsemigroups/make-present.hpp"

#include <cstdint>  // for uint64_t

#include "libsemigroups/exception.hpp"          // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase::cons...
#include "libsemigroups/present.hpp"            // for Presentation<>::word_...
#include "libsemigroups/word.hpp"               // for word_to_string

namespace libsemigroups {
  Presentation<std::string> make(FroidurePinBase&   fp,
                                 std::string const& alphabet) {
    if (alphabet.size() != fp.number_of_generators()) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd argument %s must have length %llu, the "
                              "number of generators of the 1st argument!",
                              alphabet.c_str(),
                              uint64_t(fp.number_of_generators()));
    }
    Presentation<std::string> p;
    p.alphabet(alphabet);
    for (auto it = fp.cbegin_rules(); it != fp.cend_rules(); ++it) {
      presentation::add_rule(
          p,
          detail::word_to_string(
              alphabet, it->first.cbegin(), it->first.cend()),
          detail::word_to_string(
              alphabet, it->second.cbegin(), it->second.cend()));
    }

    return p;
  }
}  // namespace libsemigroups
