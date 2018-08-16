//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// Todd-Coxeter elements

// TODO 1. smart pointer to _tc?
//      2. put the implementation into a cpp file
#ifndef LIBSEMIGROUPS_INCLUDE_TCE_HPP_
#define LIBSEMIGROUPS_INCLUDE_TCE_HPP_

#include "adapters.hpp"
#include "constants.hpp"
#include "todd-coxeter.hpp"

namespace libsemigroups {

  class TCE {
    using class_index_type = CongBase::class_index_type;

   public:
    TCE() = default;
    TCE(congruence::ToddCoxeter* tc, class_index_type i)
    noexcept : _tc(tc), _index(i) {}

    TCE(congruence::ToddCoxeter& tc, class_index_type i)
    noexcept : TCE(&tc, i) {}

    bool operator==(TCE const& that) const noexcept {
      LIBSEMIGROUPS_ASSERT(_tc == that._tc);
      return _index == that._index;
    }

    bool operator<(TCE const& that) const noexcept {
      LIBSEMIGROUPS_ASSERT(_tc == that._tc);
      return _index < that._index;
    }

    // Only works when that is a generator!!
    inline TCE operator*(TCE const& that) const {
      // class_index_to_letter checks that "that" is really a generator
      return TCE(_tc,
                 _tc->table(_index, _tc->class_index_to_letter(that._index)));
    }

    inline TCE one() const noexcept {
      return TCE(_tc, 0);
    }

    friend std::ostringstream& operator<<(std::ostringstream& os,
                                          TCE const&          x) {
      os << "TCE(" << x._index << ")";
      return os;
    }

    friend std::ostream& operator<<(std::ostream& os, TCE const& tc) {
      os << to_string(tc);
      return os;
    }

    friend struct ::std::hash<TCE>;

   private:
    congruence::ToddCoxeter* _tc;
    class_index_type         _index;
  };

  static_assert(std::is_trivial<TCE>::value, "TCE is not trivial!");

  template <> struct complexity<TCE> {
    constexpr size_t operator()(TCE const&) const noexcept {
      return LIMIT_MAX;
    }
  };

  template <> struct degree<TCE> {
    constexpr size_t operator()(TCE const&) const noexcept {
      return 0;
    }
  };

  template <> struct less<TCE> {
    bool operator()(TCE const& x, TCE const& y) const noexcept {
      return x < y;
    }
  };

  template <> struct one<TCE> {
    TCE operator()(TCE const& x) const noexcept {
      return x.one();
    }
  };

  template <> struct product<TCE> {
    void operator()(TCE& xy, TCE const& x, TCE const& y, size_t = 0) const {
      xy = x * y;
    }
  };

  template <> struct swap<TCE> {
    void operator()(TCE& x, TCE& y) const noexcept {
      std::swap(x, y);
    }
  };
}  // namespace libsemigroups

namespace std {
  template <> struct hash<libsemigroups::TCE> {
    size_t operator()(libsemigroups::TCE const& x) const noexcept {
      return x._index;
    }
  };
}  // namespace std

#endif  // LIBSEMIGROUPS_INCLUDE_TCE_HPP_
