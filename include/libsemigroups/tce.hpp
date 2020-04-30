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

// This file contains the declaration of the class TCE, which is a
// wrapper around congruence::ToddCoxeter class indices, that can be used as
// the template parameter TElementType for the FroidurePin class template.
// This file also contains specializations of the adapters complexity, degree,
// less, one, product, and std::hash for TCE.

#ifndef LIBSEMIGROUPS_TCE_HPP_
#define LIBSEMIGROUPS_TCE_HPP_

#include <cstddef>     // for size_t
#include <functional>  // for hash
#include <memory>      // for shared_ptr
#include <sstream>     // for ostream, ostringstream

#include "adapters.hpp"    // for Complexity, Degree, Less, One, Product, ...
#include "constants.hpp"   // for LIMIT_MAX
#include "containers.hpp"  // for DynamicArray2

namespace libsemigroups {
  // Forward declarations

  namespace congruence {
    class ToddCoxeter;
  }  // namespace congruence

  namespace detail {
    class TCE {
      using ToddCoxeter = congruence::ToddCoxeter;

     public:
      using class_index_type = size_t;
      using Table            = detail::DynamicArray2<class_index_type>;

      TCE() noexcept           = default;
      TCE(TCE const&) noexcept = default;
      TCE(TCE&&) noexcept      = default;
      TCE& operator=(TCE const&) noexcept = default;
      TCE& operator=(TCE&&) noexcept = default;
      ~TCE()                         = default;

      explicit TCE(class_index_type i) noexcept : _index(i) {}

      bool operator==(TCE const& that) const noexcept {
        return _index == that._index;
      }

      bool operator<(TCE const& that) const noexcept {
        return _index < that._index;
      }

      TCE one() const noexcept {
        return TCE(0);
      }

      operator class_index_type() const {
        return _index;
      }

     private:
      // Note that the value of the class_index_type _value below is the actual
      // class_index_type  used in the ToddCoxeter class and not that number
      // minus 1, which is what "class_index" means in the context of
      // CongruenceInterface objects.
      class_index_type _index;
    };

    // The following are not really required but are here as a reminder that
    // KBE are used in BruidhinnTraits which depends on the values in the
    // static_asserts below.
    static_assert(std::is_trivial<TCE>::value, "TCE is not trivial!!!");
    static_assert(std::integral_constant<bool, (sizeof(TCE) <= 8)>::value,
                  "TCE's sizeof exceeds 8!!");
  }  // namespace detail

  //! No doc
  inline std::ostringstream& operator<<(std::ostringstream& os,
                                        detail::TCE const&  x) {
    os << "TCE(" << detail::TCE::class_index_type(x) << ")";
    return os;
  }

  //! No doc
  inline std::ostream& operator<<(std::ostream& os, detail::TCE const& x) {
    os << detail::to_string(x);
    return os;
  }

  // Specialization of the adapter libsemigroups::Complexity for detail::TCE.
  template <>
  struct Complexity<detail::TCE> {
    // Returns LIMIT_MAX, because it is not possible to multiply arbitrary TCE
    // instances, only arbitrary TCE by a TCE representing a generator (see
    // TCE::operator*).  This adapter is used by the FroidurePin class to
    // determine if it is better to trace paths in the Cayley graph or to
    // directly multiply elements (using the \c * operator). Since LIMIT_MAX is
    // returned, elements are not directly multiplied by the \c * operator
    // (except if the right hand argument represents a TCE).
    constexpr size_t operator()(detail::TCE const&) const noexcept {
      return LIMIT_MAX;
    }
  };

  template <>
  struct Degree<detail::TCE> {
    constexpr size_t operator()(detail::TCE const&) const noexcept {
      return 0;
    }
  };

  //! Specialization of the adapter libsemigroups::One for TCE.
  template <>
  struct One<detail::TCE> {
    //! This directly uses the member function TCE::one.
    detail::TCE operator()(detail::TCE const& x) const noexcept {
      return x.one();
    }
  };

  template <>
  struct Product<detail::TCE> {
    void operator()(detail::TCE&        xy,
                    detail::TCE const&  x,
                    detail::TCE const&  y,
                    detail::TCE::Table* t,
                    size_t = 0) const {
      xy = detail::TCE(t->get(x, y - 1));
    }
  };

}  // namespace libsemigroups

namespace std {
  template <>
  struct hash<libsemigroups::detail::TCE> {
    using class_index_type = libsemigroups::detail::TCE::class_index_type;
    size_t operator()(libsemigroups::detail::TCE const& x) const noexcept {
      return std::hash<class_index_type>()(x);
    }
  };
}  // namespace std

#endif  // LIBSEMIGROUPS_TCE_HPP_
