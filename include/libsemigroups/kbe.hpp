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

// This file contains the declaration of the class KBE, which can be used as
// the element_type for a FroidurePin instance. This class just wraps a
// reduced word of a KnuthBendix instance.

// For technical reasons this is implemented in src/kbe-impl.hpp

#ifndef LIBSEMIGROUPS_KBE_HPP_
#define LIBSEMIGROUPS_KBE_HPP_

#include <cstddef>  // for size_t
#include <string>   // for string
#include <utility>  // for hash

#include "adapters.hpp"      // for One
#include "froidure-pin.hpp"  // for FroidurePin
#include "knuth-bendix.hpp"  // for fpsemigroup::KnuthBendix
#include "types.hpp"         // for word_type, letter_type

namespace libsemigroups {
  namespace detail {
    // This class is used to wrap libsemigroups::internal_string_type into an
    // object that can be used as generators for a FroidurePin object.
    class KBE final {
      using KnuthBendix          = fpsemigroup::KnuthBendix;
      using internal_string_type = std::string;

      KBE(internal_string_type const&);
      KBE(internal_string_type&&);

     public:
      KBE() = default;

      KBE(KBE const&) = default;
      KBE(KBE&&)      = default;

      KBE& operator=(KBE const&) = default;
      KBE& operator=(KBE&&) = default;

      ~KBE() = default;

      // Construct from internal string
      KBE(KnuthBendix&, internal_string_type const&);
      KBE(KnuthBendix&, internal_string_type&&);

      // Construct from external types
      KBE(KnuthBendix&, letter_type const&);
      KBE(KnuthBendix&, word_type const&);

      bool operator==(KBE const&) const;
      bool operator<(KBE const&) const;
      void swap(KBE&);

      internal_string_type const& string() const noexcept;
      std::string                 string(KnuthBendix const& kb) const;
      word_type                   word(KnuthBendix const& kb) const;

      friend std::ostringstream& operator<<(std::ostringstream& os,
                                            KBE const&          kbe) {
        os << kbe.string();
        return os;
      }

     private:
      internal_string_type _kb_word;
    };
    // The following are not really required but are here as a reminder that
    // KBE are used in BruidhinnTraits which depends on the values in the
    // static_asserts below.
    static_assert(!std::is_trivial<KBE>::value, "KBE is trivial!!!");
    static_assert(std::integral_constant<bool, (sizeof(KBE) <= 32)>::value,
                  "KBE's sizeof exceeds 32!!");

  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Adapters for KBE class
  ////////////////////////////////////////////////////////////////////////

  template <>
  struct Complexity<detail::KBE> {
    constexpr size_t operator()(detail::KBE const&) const noexcept {
      return LIMIT_MAX;
    }
  };

  template <>
  struct Degree<detail::KBE> {
    constexpr size_t operator()(detail::KBE const&) const noexcept {
      return 0;
    }
  };

  template <>
  struct IncreaseDegree<detail::KBE> {
    void operator()(detail::KBE const&, size_t) const noexcept {}
  };

  template <>
  struct One<detail::KBE> {
    detail::KBE operator()(detail::KBE const&) const noexcept {
      return detail::KBE();
    }

    detail::KBE operator()(size_t = 0) const noexcept {
      return detail::KBE();
    }
  };

  template <>
  struct Product<detail::KBE> {
    void operator()(detail::KBE&              xy,
                    detail::KBE const&        x,
                    detail::KBE const&        y,
                    fpsemigroup::KnuthBendix* kb,
                    size_t) {
      std::string w(x.string());  // internal_string_type
      w += y.string();
      xy = detail::KBE(*kb, w);
    }
  };

  template <>
  word_type
  FroidurePin<detail::KBE,
              FroidurePinTraits<detail::KBE, fpsemigroup::KnuthBendix>>::
      factorisation(detail::KBE const& x);

  template <>
  tril FroidurePin<detail::KBE,
                   FroidurePinTraits<detail::KBE, fpsemigroup::KnuthBendix>>::
      is_finite() const;
}  // namespace libsemigroups

////////////////////////////////////////////////////////////////////////
// Specializations of std::hash and std::equal_to
////////////////////////////////////////////////////////////////////////

namespace std {
  template <>
  struct hash<libsemigroups::detail::KBE> {
    size_t operator()(libsemigroups::detail::KBE const& x) const {
      return hash<string>()(x.string());
    }
  };

  template <>
  struct equal_to<libsemigroups::detail::KBE> {
    bool operator()(libsemigroups::detail::KBE const& x,
                    libsemigroups::detail::KBE const& y) const {
      return x == y;
    }
  };
}  // namespace std
#endif  // LIBSEMIGROUPS_KBE_HPP_
