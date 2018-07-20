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

#ifndef LIBSEMIGROUPS_INCLUDE_TCE_H_
#define LIBSEMIGROUPS_INCLUDE_TCE_H_

#include "adapters.h"
#include "todd-coxeter.h"

namespace libsemigroups {

  class TCE {
    using class_index_type = CongIntf::class_index_type;

   public:
    TCE() = default;
    // TODO smart pointer?
    TCE(congruence::ToddCoxeter* tc, class_index_type i) : _tc(tc), _index(i) {}

    bool operator==(TCE const& that) const {
      return _index == that._index;
    }

    bool operator<(TCE const& that) const {
      return _index < that._index;
    }

    // Only works when that is a generator!!
    inline TCE operator*(TCE const& that) const {
      // LIBSEMIGROUPS_ASSERT(that._index <= _tc->nr_generators());
      return TCE(_tc, _tc->right(_index, that._index - 1));
    }

    inline TCE one() const {
      return TCE(_tc, 0);
    }

    class_index_type class_index() const {
      return _index;
    }

   private:
    congruence::ToddCoxeter* _tc;
    class_index_type         _index;
  };

  template <> struct complexity<TCE> {
    constexpr size_t operator()(TCE const&) const noexcept {
      return LIMIT_MAX;
    }
  };

  template <> struct degree<TCE> {
    constexpr size_t operator()(TCE const&) const noexcept {
      return 16;
    }
  };

  template <>
  struct less<
      TCE> {
    bool operator()(TCE const& x,
                    TCE const& y) const noexcept {
      return x < y;
    }
  };

  template <>
  struct one<TCE> {
    TCE operator()(TCE const& x) const noexcept {
      return x.one();
    }
  };

  template <> struct product<TCE> {
    void operator()(TCE& xy, TCE const& x, TCE const& y, size_t = 0) const
        noexcept {
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
    size_t operator()(libsemigroups::TCE const& x) const {
      return x.class_index();
    }
  };
}  // namespace std

#endif  // LIBSEMIGROUPS_INCLUDE_TCE_H_
