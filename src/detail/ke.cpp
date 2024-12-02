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

#include "libsemigroups/detail/ke.hpp"

#include <string>  // for string

#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/types.hpp"         // for word_type, tril

namespace libsemigroups {
  namespace detail {
    class MultiStringView;  // forward decl
  }

  // TODO(0) uncomment
  //  template <>
  //  word_type FroidurePin<
  //      detail::KE<std::string>,
  //      FroidurePinTraits<detail::KE<std::string>, Kambites<std::string>>>::
  //      factorisation(detail::KE<std::string> const& x) {
  //    return x.to_word(*state());
  //  }
  //
  //  template <>
  //  word_type FroidurePin<detail::KE<detail::MultiStringView>,
  //                        FroidurePinTraits<detail::KE<detail::MultiStringView>,
  //                                          Kambites<detail::MultiStringView>>>::
  //      factorisation(detail::KE<detail::MultiStringView> const& x) {
  //    return x.to_word(*state());
  //  }
  //
  //  template <>
  //  word_type
  //  FroidurePin<detail::KE<word_type>,
  //              FroidurePinTraits<detail::KE<word_type>,
  //              Kambites<word_type>>>::
  //      factorisation(detail::KE<word_type> const& x) {
  //    return x.to_word(*state());
  //  }

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

  template <>
  tril FroidurePin<detail::KE<word_type>,
                   FroidurePinTraits<detail::KE<word_type>,
                                     Kambites<word_type>>>::is_finite() const {
    return tril::FALSE;
  }

  //   template <>
  //   size_t FroidurePin<detail::KE<std::string>,
  //                      FroidurePinTraits<detail::KE<std::string>,
  //                                        Kambites<std::string>>>::size() {
  //     return POSITIVE_INFINITY;
  //   }
  //
  //   template <>
  //   size_t
  //   FroidurePin<detail::KE<detail::MultiStringView>,
  //               FroidurePinTraits<detail::KE<detail::MultiStringView>,
  //                                 Kambites<detail::MultiStringView>>>::size()
  //                                 {
  //     return POSITIVE_INFINITY;
  //   }
  //
  //   template <>
  //   size_t FroidurePin<
  //       detail::KE<word_type>,
  //       FroidurePinTraits<detail::KE<word_type>,
  //       Kambites<word_type>>>::size() {
  //     return POSITIVE_INFINITY;
  //   }

  namespace detail {
    template <>
    word_type KE<word_type>::to_word(Kambites<word_type> const&) const {
      return _value;
    }
  }  // namespace detail
}  // namespace libsemigroups
