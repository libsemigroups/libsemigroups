//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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

// This file contains the implementation of the KambitesImpl class.

#include "libsemigroups/kambites.hpp"

#include <algorithm>  // for max, find_if, equal, copy, min
#include <string>     // for string
#include <tuple>      // for tie, tuple

#include <iostream>

#include "libsemigroups/constants.hpp"  // for UNDEFINED, POSITIVE_INFINITY
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"       // for FroidurePin
#include "libsemigroups/order.hpp"              // for lexicographical_compare
#include "libsemigroups/string.hpp"  // for is_prefix, maximum_common_suffix
#include "libsemigroups/types.hpp"   // for word_type, tril
#include "libsemigroups/uf.hpp"      // for Duf<>
#include "libsemigroups/words.hpp"   // for word_to_string

#include "libsemigroups/detail/int-range.hpp"  // for detail::IntRange

namespace libsemigroups {

  template <>
  word_type FroidurePin<
      detail::KE<std::string>,
      FroidurePinTraits<detail::KE<std::string>, Kambites<std::string>>>::
      factorisation(detail::KE<std::string> const& x) {
    return x.word(*state());
  }

  template <>
  word_type FroidurePin<detail::KE<detail::MultiStringView>,
                        FroidurePinTraits<detail::KE<detail::MultiStringView>,
                                          Kambites<detail::MultiStringView>>>::
      factorisation(detail::KE<detail::MultiStringView> const& x) {
    return x.word(*state());
  }

  template <>
  tril
  FroidurePin<detail::KE<std::string>,
              FroidurePinTraits<detail::KE<std::string>,
                                Kambites<std::string>>>::is_finite() const {
    return tril::FALSE;
  }

  template <>
  tril
  FroidurePin<detail::KE<detail::MultiStringView>,
              FroidurePinTraits<detail::KE<detail::MultiStringView>,
                                Kambites<detail::MultiStringView>>>::is_finite()
      const {
    return tril::FALSE;
  }
}  // namespace libsemigroups
