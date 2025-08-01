//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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
// reduced word of a KnuthBendixImpl instance.

#ifndef LIBSEMIGROUPS_DETAIL_KBE_HPP_
#define LIBSEMIGROUPS_DETAIL_KBE_HPP_

#include <cstddef>  // for size_t
#include <string>   // for string
#include <utility>  // for hash

#include "libsemigroups/adapters.hpp"      // for One
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/obvinf.hpp"        // for is_obviously_infinite
#include "libsemigroups/types.hpp"         // for word_type, letter_type
#include "libsemigroups/word-range.hpp"    // for namespace words

namespace libsemigroups {
  namespace detail {
    // This class is used to wrap KnuthBendix_::native_word_type into an
    // object that can be used as generators for a FroidurePin object.
    template <typename KnuthBendix_>
    class KBE {
     public:
      using knuth_bendix_type = KnuthBendix_;
      using native_word_type  = typename KnuthBendix_::native_word_type;
      using native_letter_type =
          typename KnuthBendix_::native_word_type::value_type;

      KBE() = default;

      KBE(KBE const&) = default;
      KBE(KBE&&)      = default;

      KBE& operator=(KBE const&) = default;
      KBE& operator=(KBE&&)      = default;

      ~KBE() = default;

      // Construct from external types
      KBE(knuth_bendix_type&, native_letter_type const&);
      KBE(knuth_bendix_type&, native_word_type const&);

      bool operator==(KBE const&) const;
      bool operator<(KBE const&) const;
      void swap(KBE&);

      native_word_type const& word() const noexcept;

      friend std::ostringstream& operator<<(std::ostringstream& os,
                                            KBE const&          kbe) {
        os << kbe.word();
        return os;
      }

     private:
      native_word_type _kb_word;
    };

  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Adapters for KBE class
  ////////////////////////////////////////////////////////////////////////

  template <typename KnuthBendix_>
  struct Complexity<detail::KBE<KnuthBendix_>> {
    constexpr size_t
    operator()(detail::KBE<KnuthBendix_> const&) const noexcept {
      return LIMIT_MAX;
    }
  };

  template <typename KnuthBendix_>
  struct Degree<detail::KBE<KnuthBendix_>> {
    constexpr size_t
    operator()(detail::KBE<KnuthBendix_> const&) const noexcept {
      return 0;
    }
  };

  template <typename KnuthBendix_>
  struct IncreaseDegree<detail::KBE<KnuthBendix_>> {
    void operator()(detail::KBE<KnuthBendix_> const&, size_t) const noexcept {}
  };

  template <typename KnuthBendix_>
  struct One<detail::KBE<KnuthBendix_>> {
    detail::KBE<KnuthBendix_>
    operator()(detail::KBE<KnuthBendix_> const&) const noexcept {
      return detail::KBE<KnuthBendix_>();
    }

    detail::KBE<KnuthBendix_> operator()(size_t = 0) const noexcept {
      return detail::KBE<KnuthBendix_>();
    }
  };

  template <typename KnuthBendix_>
  struct Product<detail::KBE<KnuthBendix_>> {
    void operator()(detail::KBE<KnuthBendix_>&       xy,
                    detail::KBE<KnuthBendix_> const& x,
                    detail::KBE<KnuthBendix_> const& y,
                    KnuthBendix_*                    kb,
                    size_t) {
      using words::operator+=;
      using KBE_ = detail::KBE<KnuthBendix_>;
      auto w(x.word());
      w += y.word();
      // TODO improve
      xy = KBE_(*kb, w);
    }
  };

  template <typename KnuthBendix_>
  struct FroidurePinState<detail::KBE<KnuthBendix_>> {
    using type = KnuthBendix_;
  };

  // template <>
  // word_type FroidurePin<detail::KBE<KnuthBendixImpl<>>>::factorisation(
  //     detail::KBE<KnuthBendixImpl<>> const& x);

  // template <>
  // tril FroidurePin<detail::KBE<KnuthBendixImpl<>>>::is_finite() const;

}  // namespace libsemigroups

////////////////////////////////////////////////////////////////////////
// Specializations of std::hash and std::equal_to
////////////////////////////////////////////////////////////////////////

namespace std {
  template <typename KnuthBendix_>
  struct hash<libsemigroups::detail::KBE<KnuthBendix_>> {
    size_t operator()(libsemigroups::detail::KBE<KnuthBendix_> const& x) const {
      return hash<typename libsemigroups::detail::KBE<
          KnuthBendix_>::native_word_type>()(x.word());
    }
  };

  template <typename KnuthBendix_>
  struct equal_to<libsemigroups::detail::KBE<KnuthBendix_>> {
    bool operator()(libsemigroups::detail::KBE<KnuthBendix_> const& x,
                    libsemigroups::detail::KBE<KnuthBendix_> const& y) const {
      return x == y;
    }
  };
}  // namespace std

#include "kbe.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_KBE_HPP_
