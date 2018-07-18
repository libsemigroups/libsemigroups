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

#ifndef LIBSEMIGROUPS_SRC_TCE_H_
#define LIBSEMIGROUPS_SRC_TCE_H_

#include "eltcont.h"
#include "todd-coxeter.h"

namespace libsemigroups {

  using class_index_t = congruence::Interface::class_index_t;
  class TCE {
   public:
    TCE() = default;
    TCE(congruence::ToddCoxeter* tc, class_index_t i) : _tc(tc), _index(i) {}

    ~TCE() = default;

    bool operator==(TCE const& that) const {
      return _index == that._index;
    }

    bool operator<(TCE const& that) const {
      return _index < that._index;
    }

    // Only works when that is a generator!!
    inline TCE operator*(TCE const& that) const {
      LIBSEMIGROUPS_ASSERT(that._index <= _tc->nr_generators());
      return TCE(_tc, _tc->right(_index, that._index - 1));
    }

    inline TCE one() const {
      return TCE(_tc, 0);
    }

    class_index_t class_index() const {
      return _index;
    }

   private:
    congruence::ToddCoxeter*  _tc;
    class_index_t _index;
  };

  template <> size_t ElementContainer<TCE>::complexity(TCE) const;
}  // namespace libsemigroups

namespace std {
  template <> struct hash<libsemigroups::TCE> {
    size_t operator()(libsemigroups::TCE const& x) const {
      return x.class_index();
    }
  };
}  // namespace std

#endif  // LIBSEMIGROUPS_SRC_TCE_H_
