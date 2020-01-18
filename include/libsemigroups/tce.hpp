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

#ifndef LIBSEMIGROUPS_INCLUDE_TCE_HPP_
#define LIBSEMIGROUPS_INCLUDE_TCE_HPP_

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
    // This class is a wrapper around congruence::ToddCoxeter class indices,
    // that can be used as the template parameter TElementType for the
    // FroidurePin class template.
    class TCE {
      using ToddCoxeter = congruence::ToddCoxeter;

     public:
      // The type of the wrapped indices.
      using class_index_type = size_t;

      // The type of the underlying coset table produced by an instance of
      // ToddCoxeter.
      using CosetTable = detail::DynamicArray2<class_index_type>;

      // A default constructor.
      TCE() = default;

      // A constructor from a pointer to a ToddCoxeter instance. This
      // constructor ensures that the ToddCoxeter is run to conclusion (if
      // possible), and standardized according to ToddCoxeter::order::shortlex.
      // A copy of the resulting TCE::CosetTable object is stored in a
      // std::shared_ptr. The TCE::CosetTable is shared by the TCE constructed
      // by this constructor (let's call it \c x) and any instance of TCE
      // constructed using the constructor TCE::TCE(TCE const&,
      // class_index_type) with first parameter \c x.
      // The class index of the TCE constructed is UNDEFINED.
      explicit TCE(ToddCoxeter*) noexcept;

      // A constructor from a TCE instance and class index. The resulting
      // object has a shared pointer to a TCE::CosetTable of the ToddCoxeter
      // instance that this object is derived from. It is necessary to create
      // one element using the constructor TCE::TCE(ToddCoxeter*) which can
      // then be given as the first parameter of this constructor.
      TCE(TCE const&, class_index_type) noexcept;

      // Two TCE objects are equal if they have equal class index, even if they
      // refer to different ToddCoxeter instances.
      bool operator==(TCE const&) const noexcept;

      // A TCE is less than another TCE if the class index is less. This
      // corresponds to the short-lex order on words in the generators of the
      // original ToddCoxeter instance.
      bool operator<(TCE const&) const noexcept;

      // Instances of TCE are multiplied according to the TCE::CosetTable used
      // to construct the original TCE (using TCE::TCE(ToddCoxeter*)). This
      // operator can only be used to multiply a TCE by a TCE that represents a
      // generator of the original ToddCoxeter instance. This is one of the
      // minimum requirements to ensure that the FroidurePin algorithm applies
      // to TCE.
      TCE operator*(TCE const&) const;

      // The one of a TCE is just a multiplicative identity, in the sense that
      // any TCE \c x multiplied by the one, on either side, yields \c x. This
      // is always the TCE with class index 0.
      TCE one() const noexcept;

      // Friend function to allow stringstream insertion of a TCE instance.
      friend std::ostringstream& operator<<(std::ostringstream&, TCE const&);

      // Friend function to allow specialization of std::hash for TCE.
      friend struct ::std::hash<TCE>;

     private:
      std::shared_ptr<CosetTable> _table;
      // Note that the class_index_type below is the actual class_index_type
      // used in the ToddCoxeter class and not that number minus 1, which is
      // what "class_index" means in the context of CongruenceInterface objects.
      class_index_type _index;
    };
  }  // namespace detail

  //! Specialization of the adapter libsemigroups::Complexity for detail::TCE.
  template <>
  struct Complexity<detail::TCE> {
    //! Returns LIMIT_MAX, because it is not possible to
    //! multiply arbitrary TCE instances, only arbitrary TCE by a TCE
    //! representing a generator (see TCE::operator*).  This adapter is used by
    //! the FroidurePin class to determine if it is better to trace paths in
    //! the Cayley graph or to directly multiply elements (using the \c *
    //! operator). Since LIMIT_MAX is returned, elements are not directly
    //! multiplied by the \c * operator (except if the right hand argument
    //! represents a TCE).
    constexpr size_t operator()(detail::TCE const&) const noexcept {
      return LIMIT_MAX;
    }
  };

  //! Specialization of the adapter Degree for TCE.
  template <>
  struct Degree<detail::TCE> {
    //! This returns 0, because there is no meaningful notion of degree for TCE
    //! instances.
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

  // Specialization of the adapter less for TCE.
  //
  template <>
  struct Product<detail::TCE> {
    // The call operator directly uses the member function TCE::operator*.
    //
    // \warning If the parameter \p y does not represent a generator of the
    // original ToddCoxeter instance, then the behaviour of the call operator
    // of product is undefined.
    void operator()(detail::TCE&       xy,
                    detail::TCE const& x,
                    detail::TCE const& y,
                    size_t = 0) const {
      xy = x * y;
    }
  };

  //! No doc
  std::ostream& operator<<(std::ostream&                     os,
                           libsemigroups::detail::TCE const& tc);
}  // namespace libsemigroups

namespace std {
  // Specialization of
  // [std::hash](https://en.cppreference.com/w/cpp/utility/hash) for
  // libsemigroups::TCE, that directly used the class index of the element.
  template <>
  struct hash<libsemigroups::detail::TCE> {
    size_t operator()(libsemigroups::detail::TCE const& x) const noexcept {
      return x._index;
    }
  };
}  // namespace std

#endif  // LIBSEMIGROUPS_INCLUDE_TCE_HPP_
