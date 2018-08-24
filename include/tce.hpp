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

//

#ifndef LIBSEMIGROUPS_INCLUDE_TCE_HPP_
#define LIBSEMIGROUPS_INCLUDE_TCE_HPP_

#include <cstddef>  // for size_t
#include <ostream>  // for ostream
#include <sstream>  // for ostringstream

#include "adapters.hpp"   // for complexity, degree, less, one, product, ...
#include "constants.hpp"  // for LIMIT_MAX

namespace libsemigroups {
  // Forward declarations
  namespace congruence {
    class ToddCoxeter;
  }  // namespace congruence
  namespace fpsemigroup {
    template <class T, bool S>
    class WrappedCong;
    using ToddCoxeter = WrappedCong<congruence::ToddCoxeter, true>;
  }  // namespace fpsemigroup

  class TCE {
   public:
    using class_index_type = size_t;
    TCE()                  = default;
    TCE(congruence::ToddCoxeter*, class_index_type) noexcept;
    TCE(congruence::ToddCoxeter&, class_index_type) noexcept;

    bool operator==(TCE const&) const noexcept;
    bool operator<(TCE const&) const noexcept;

    // Only works when that is a generator!!
    TCE operator*(TCE const&) const;
    TCE one() const noexcept;

    friend std::ostringstream& operator<<(std::ostringstream&, TCE const&);
    friend std::ostream&       operator<<(std::ostream&, TCE const&);
    friend struct ::std::hash<TCE>;

   private:
    congruence::ToddCoxeter* _tc;
    // Note that the class_index_type below is the actual class_index_type used
    // in the ToddCoxeter class and not that number minus 1, which is what
    // "class_index" means in the context of CongBase objects.
    class_index_type _index;
  };

  static_assert(std::is_trivial<TCE>::value, "TCE is not trivial!");

  template <>
  struct complexity<TCE> {
    constexpr size_t operator()(TCE const&) const noexcept {
      return LIMIT_MAX;
    }
  };

  template <>
  struct degree<TCE> {
    constexpr size_t operator()(TCE const&) const noexcept {
      return 0;
    }
  };

  template <>
  struct less<TCE> {
    bool operator()(TCE const& x, TCE const& y) const noexcept {
      return x < y;
    }
  };

  template <>
  struct one<TCE> {
    TCE operator()(TCE const& x) const noexcept {
      return x.one();
    }
  };

  template <>
  struct product<TCE> {
    void operator()(TCE& xy, TCE const& x, TCE const& y, size_t = 0) const {
      xy = x * y;
    }
  };
}  // namespace libsemigroups

namespace std {
  template <>
  struct hash<libsemigroups::TCE> {
    size_t operator()(libsemigroups::TCE const& x) const noexcept {
      return x._index;
    }
  };
}  // namespace std

#endif  // LIBSEMIGROUPS_INCLUDE_TCE_HPP_
