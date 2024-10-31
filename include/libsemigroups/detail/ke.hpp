//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains the declaration of the class KBE, which can be used as
// the element_type for a FroidurePin instance. This class just wraps a
// reduced word of a KnuthBendix instance.

#ifndef LIBSEMIGROUPS_DETAIL_KE_HPP_
#define LIBSEMIGROUPS_DETAIL_KE_HPP_

#include "libsemigroups/kambites.hpp"  // for Kambites

#include <cstddef>      // for size_t
#include <functional>   // for hash
#include <string>       // for basic_string
#include <type_traits>  // for enable_if_t, integral_constant

#include "libsemigroups/adapters.hpp"      // for Complexity
#include "libsemigroups/constants.hpp"     // for Max, LimitMax, LIMIT_MAX
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin, FroidurePi...
#include "libsemigroups/kambites.hpp"      // for Kambites
#include "libsemigroups/order.hpp"         // for shortlex_compare
#include "libsemigroups/presentation.hpp"  // for to_word
#include "libsemigroups/types.hpp"         // for word_type, tril
#include "libsemigroups/words.hpp"         // for ToWord, ToString

namespace libsemigroups {
  namespace detail {
    // This class is used to wrap libsemigroups::Kambites::value_type into an
    // object that can be used as generators for a FroidurePin object.
    template <typename Word>
    class KE {
     public:
      using value_type = typename Kambites<Word>::value_type;

     private:
      value_type _value;

     public:
      KE()                     = default;
      KE(KE const&)            = default;
      KE(KE&&)                 = default;
      KE& operator=(KE const&) = default;
      KE& operator=(KE&&)      = default;
      ~KE()                    = default;

      KE(Kambites<Word>& k, value_type const& w) : _value(k.normal_form(w)) {}

      KE(Kambites<Word>& k, value_type&& w)
          : _value(k.normal_form(std::move(w))) {}

      KE(Kambites<Word>& k, letter_type a)
          : KE(k, value_type({k.presentation().letter_no_checks(a)})) {}

      bool operator==(KE const& that) const {
        return that._value == this->_value;
      }

      bool operator<(KE const& that) const {
        return shortlex_compare(_value, that._value);
      }

      void swap(KE& x) {
        std::swap(x._value, _value);
      }

      value_type const& value() const noexcept {
        return _value;
      }

      word_type to_word(Kambites<Word> const& k) const {
        ToWord to_word(k.presentation().alphabet());
        return to_word(_value);
      }

      template <typename SFINAE = std::string>
      auto to_string() const noexcept
          -> std::enable_if_t<!std::is_same_v<Word, word_type>, SFINAE> {
        return _value;
      }
    };

    template <>
    word_type KE<word_type>::to_word(Kambites<word_type> const&) const;

    // The following are not really required but are here as a reminder that
    // KE are used in BruidhinnTraits which depends on the values in the
    // static_asserts below.
    static_assert(!std::is_trivial<KE<std::string>>::value,
                  "KE is not trivial!!!");
    static_assert(
        std::integral_constant<bool, (sizeof(KE<std::string>) <= 32)>::value,
        "KE's sizeof exceeds 32!!");

  }  // namespace detail

  template <typename Word>
  struct FroidurePinState<detail::KE<Word>> {
    using type = Kambites<Word>;
  };

  ////////////////////////////////////////////////////////////////////////
  // Adapters for KE class
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  struct Complexity<detail::KE<Word>> {
    constexpr size_t operator()(detail::KE<Word> const&) const noexcept {
      return LIMIT_MAX;
    }
  };

  template <typename Word>
  struct Degree<detail::KE<Word>> {
    constexpr size_t operator()(detail::KE<Word> const&) const noexcept {
      return 0;
    }
  };

  template <typename Word>
  struct IncreaseDegree<detail::KE<Word>> {
    void operator()(detail::KE<Word> const&) const noexcept {}
  };

  template <typename Word>
  struct One<detail::KE<Word>> {
    detail::KE<Word> operator()(detail::KE<Word> const&) {
      return detail::KE<Word>();
    }

    detail::KE<Word> operator()(size_t = 0) const {
      return detail::KE<Word>();
    }
  };

  template <typename Word>
  struct Product<detail::KE<Word>> {
    void operator()(detail::KE<Word>&       xy,
                    detail::KE<Word> const& x,
                    detail::KE<Word> const& y,
                    Kambites<Word>*         k,
                    size_t) {
      using value_type = typename detail::KE<Word>::value_type;
      using words::operator+=;
      value_type   w(x.value());  // string_type
      w += y.value();
      xy = detail::KE<Word>(*k, w);
    }
  };

#ifndef PARSED_BY_DOXYGEN
  // TODO(0) uncomment
  // template <>
  // word_type FroidurePin<
  //     detail::KE<std::string>,
  //     FroidurePinTraits<detail::KE<std::string>, Kambites<std::string>>>::
  //     factorisation(detail::KE<std::string> const& x);

  // template <>
  // word_type FroidurePin<detail::KE<detail::MultiStringView>,
  //                       FroidurePinTraits<detail::KE<detail::MultiStringView>,
  //                                         Kambites<detail::MultiStringView>>>::
  //     factorisation(detail::KE<detail::MultiStringView> const& x);

  // template <>
  // word_type
  // FroidurePin<detail::KE<word_type>,
  //             FroidurePinTraits<detail::KE<word_type>,
  //             Kambites<word_type>>>::
  //     factorisation(detail::KE<word_type> const& x);

  template <>
  tril FroidurePin<detail::KE<std::string>,
                   FroidurePinTraits<detail::KE<std::string>,
                                     Kambites<std::string>>>::is_finite() const;

  template <>
  tril
  FroidurePin<detail::KE<detail::MultiStringView>,
              FroidurePinTraits<detail::KE<detail::MultiStringView>,
                                Kambites<detail::MultiStringView>>>::is_finite()
      const;

  template <>
  tril FroidurePin<detail::KE<word_type>,
                   FroidurePinTraits<detail::KE<word_type>,
                                     Kambites<word_type>>>::is_finite() const;
#endif

}  // namespace libsemigroups

////////////////////////////////////////////////////////////////////////
// Specializations of std::hash and std::equal_to
////////////////////////////////////////////////////////////////////////

namespace std {
  template <typename Word>
  struct hash<libsemigroups::detail::KE<Word>> {
    size_t operator()(libsemigroups::detail::KE<Word> const& x) const {
      using value_type = typename libsemigroups::detail::KE<Word>::value_type;
      return hash<value_type>()(x.value());
    }
  };

  template <typename Word>
  struct equal_to<libsemigroups::detail::KE<Word>> {
    bool operator()(libsemigroups::detail::KE<Word> const& x,
                    libsemigroups::detail::KE<Word> const& y) const {
      return x == y;
    }
  };
}  // namespace std
#endif  // LIBSEMIGROUPS_DETAIL_KE_HPP_
