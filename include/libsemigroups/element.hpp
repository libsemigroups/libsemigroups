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

// This file contains the declaration of the element class and its subclasses.

// TODO(later)
// 1. split this file into several files, avoid virtual inheritance, generally
//    improve this.

#ifndef LIBSEMIGROUPS_ELEMENT_HPP_
#define LIBSEMIGROUPS_ELEMENT_HPP_

#include <cstddef>  // for size_t
#include <math.h>   // for sqrt, pow

#include <algorithm>    // for max_element
#include <cstdint>      // for uint32_t, int32_t, int64_t
#include <limits>       // for numeric_limits
#include <numeric>      // for iota
#include <sstream>      // for ostream, ostringstream
#include <type_traits>  // for is_integral
#include <vector>       // for vector

#include "adapters.hpp"    // for complexity, degree, IncreaseDegree, . . .
#include "constants.hpp"   // for UNDEFINED
#include "containers.hpp"  // for detail::DynamicArray2
#include "libsemigroups-config.hpp"     // for LIBSEMIGROUPS_HPCOMBI
#include "libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "stl.hpp"                      // for Hash, EqualTo, Less
#include "string.hpp"                   // for to_string
#include "types.hpp"                    // for SmallestInteger

namespace libsemigroups {
  // Forward declarations
  class Blocks;
  struct BooleanSemiring;
  template <typename T>
  class Semiring;

  //! Abstract base class for semigroup elements
  //!
  //! The FroidurePin class consists of Element objects. Every derived class of
  //! Element implements the deleted member functions of Element, which are used
  //! by the FroidurePin class.
  class Element {
   public:
    //! A constructor.
    Element() : _hash_value(UNDEFINED) {}

    //! A constructor.
    //!
    //! Constructs an element with given hash value.
    explicit Element(size_t hv) : _hash_value(hv) {}

    //! A default destructor.
    virtual ~Element() = default;

    //! Returns \c true if \c this equals \p that.
    //!
    //! This member function checks the mathematical equality of two Element
    //! objects in the same subclass of Element.
    virtual bool operator==(Element const&) const = 0;

    //! Returns \c true if \c this is less than \p that.
    //!
    //! This member function defines a total order on the set of objects in a
    //! given subclass of Element with a given Element::degree. The definition
    //! of this total order depends on the member function for the operator < in
    //! the subclass.
    virtual bool operator<(Element const&) const = 0;

    //! Returns \c true if \c this is greater than \p that.
    //!
    //! This member function returns \c true if \c this is greater than \p that,
    //! under the ordering defined by the operator <.
    bool operator>(Element const& that) const {
      return that < *this;
    }

    //! Returns \c true if \c this is not equal to \p that.
    //!
    //! This member function returns \c true if \c this is mathematically not
    //! equal to \p that.
    bool operator!=(Element const& that) const {
      return !(*this == that);
    }

    //! Returns \c true if \c this is less than or equal to \p that.
    //!
    //! This member function returns \c true if \c this is less than (under the
    //! order defined by the operator <) or mathematically equal to \p that.
    bool operator<=(Element const& that) const {
      return *this < that || *this == that;
    }

    //! Returns \c true if \c this is less than or equal to \p that.
    //!
    //! This member function returns \c true if \c this is greater than (under
    //! the order defined by the operator <) or mathematically equal to \p that.
    bool operator>=(Element const& that) const {
      return that <= *this;
    }

    //! Returns the approximate time complexity of multiplying two
    //! Element objects in a given subclass.
    //!
    //! This member function returns an integer which represents the approximate
    //! time complexity of multiplying two objects in the same subclass of
    //! Element which have the same Element::degree. For example, the
    //! approximate time complexity of multiplying two \f$3\times 3\f$ matrices
    //! over a common semiring is \f$O(3 ^ 3)\f$, and 27 is returned by
    //! MatrixOverSemiring::complexity.
    //!
    //! The returned value is used in, for example, FroidurePin::fast_product
    //! and FroidurePin::nr_idempotents to decide if it is better to multiply
    //! elements or follow a path in the Cayley graph.
    virtual size_t complexity() const = 0;

    //! Returns the degree of an Element.
    //!
    //! This member function returns an integer which represents the size of the
    //! element, and is used to determine whether or not two elements are
    //! compatible for multiplication. For example, two Transformation objects
    //! of different degrees cannot be multiplied, and a Bipartition of degree
    //! 10 cannot be an element of a monoid of bipartitions of degree 3.
    //!
    //! See the relevant subclass for the particular meaning of the return value
    //! of this member function for each subclass.
    virtual size_t degree() const = 0;

    //! Return the hash value of an Element.
    //!
    //! This member function returns a hash value for an object in a subclass of
    //! Element. This value is only computed the first time this member function
    //! is called.
    inline size_t hash_value() const {
      if (_hash_value == UNDEFINED) {
        this->cache_hash_value();
      }
      return this->_hash_value;
    }

    //! Swap another Element with \c this.
    //!
    //! This member function swaps the defining data of \p x and \c this.
    virtual void swap(Element&) = 0;

    //! Multiplies \p x and \p y and stores the result in \c this.
    //!
    //! Redefine \c this to be the product of \p x and \p y. This is in-place
    //! multiplication to avoid allocation of memory for products which do not
    //! need to be stored for future use.
    //!
    //! The implementation of this member function in the Element base class
    //! simply calls the 3 parameter version with third parameter 0. Any
    //! subclass of Element can implement either a two or three parameter
    //! version of this member function and the base class member function
    //! implements the other member function.
    virtual void redefine(Element const& x, Element const& y) {
      redefine(x, y, 0);
    }

    //! Multiplies \p x and \p y and stores the result in \c this.
    //!
    //! This version of the member function takes const pointers rather than
    //! const references, but otherwise behaves like the other
    //! Element::redefine.
    void redefine(Element const* x, Element const* y) {
      redefine(*x, *y, 0);
    }

    //! Multiplies \p x and \p y and stores the result in \c this.
    //!
    //! Redefine \c this to be the product of \p x and \p y. This is in-place
    //! multiplication to avoid allocation of memory for products which do not
    //! need to be stored for future use.
    //!
    //! The implementation of this member function in the Element base class
    //! simply calls the 2 parameter version and ignores the third parameter \p
    //! thread_id. Any subclass of Element can implement either a two or three
    //! parameter version of this member function and the base class member
    //! function implements the other member function.
    //!
    //! The parameter \p thread_id is required in some derived classes of
    //! Element because some temporary storage is required to find the product
    //! of \p x and \p y.
    //!
    //! Note that if different threads call this member function on a derived
    //! class of Element where static temporary storage is used in the redefine
    //! member function with the same value of \p thread_id, then bad things may
    //! happen.
    virtual void redefine(Element const& x, Element const& y, size_t) {
      redefine(x, y);
    }

    //! Multiplies \p x and \p y and stores the result in \c this.
    //!
    //! This member function differs from the the previous only in taking
    //! pointers instead of references.
    void redefine(Element const* x, Element const* y, size_t) {
      redefine(*x, *y);
    }

    // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
    //    virtual Element* empty_key() const = 0;
    // #endif

    //! Increases the degree of \c this by \p deg.
    //! This does not make sense for all subclasses of Element.
    virtual void increase_degree_by(size_t) {}

    //! Returns a new element completely independent of \c this.
    //!
    //! This member function really copies an Element. To minimise the amount of
    //! copying when Element objects are inserted in a std::unordered_map and
    //! other containers, an Element behaves somewhat like a pointer, in that
    //! the actual data in an Element is only copied when this member function
    //! is called. Otherwise, if an Element is copied, then its defining data is
    //! only stored once.
    virtual Element* heap_copy() const = 0;

    //! Returns an independent copy of the identity.
    //!
    //! This member function returns a copy of the identity element (in the
    //! appropriate semigroup) which is independent from previous copies.
    virtual Element* heap_identity() const = 0;

   protected:
    //! Calculate and cache a hash value.
    //!
    //! This member function is used to compute and cache the hash value of \c
    //! this.
    virtual void cache_hash_value() const = 0;

    //! Reset the cached value used by Element::hash_value.
    //!
    //! This member function is used to reset the cached hash value to
    //! libsemigroups::UNDEFINED. This is required after running
    //! Element::redefine, Element::copy, or any other member function that
    //! changes the defining data of \c this.
    void reset_hash_value() const {
      _hash_value = UNDEFINED;
    }

    //! This data member holds a cached version of the hash value of an Element.
    //! It is stored here if it is ever computed. It is invalidated by
    //! libsemigroups::Element::redefine and sometimes by
    //! libsemigroups::Element::really_copy, and potentially any other
    //! non-const data member of Element or any of its subclasses.
    mutable size_t _hash_value;
  };

  namespace detail {
    //! Abstract base class for elements using a vector to store their defining
    //! data.
    //!
    //! The template parameter \p TValueType is the type entries in the vector
    //! containing the defining data.
    //!
    //! The template parameter \p TSubclass is the subclass of
    //! ElementWithVectorData used by certain member functions to construct new
    //! instances of subclasses of ElementWithVectorData.
    //!
    //! For example, Transformation&lt;uint128_t&gt; is a subclass of
    //! ElementWithVectorData&lt;uint128_t, Transformation&lt;uint128_t&gt;&gt;
    //! so that when the identity member function in this class is called it
    //! returns a Transformation and not an ElementWithVectorData.
    template <typename TValueType, class TSubclass>
    class ElementWithVectorData : public Element {
     public:
      //! A constructor.
      //!
      //! Returns an object with an uninitialised vector.
      ElementWithVectorData() : Element(), _vector() {}

      //! A constructor.
      //!
      //! Returns an object with an uninitialised vector of length \p n.
      explicit ElementWithVectorData(size_t n) : Element(), _vector(n) {}

      //! A constructor.
      //!
      //! The parameter \p vector should be a const reference to defining data
      //! of the element.
      //!
      //! Returns an object whose defining data is a copy of \p vector.
      explicit ElementWithVectorData(std::vector<TValueType> const& vector)
          : Element(), _vector(vector) {}

      //! A constructor.
      //!
      //! The parameter \p vector should be a rvalue reference to defining data
      //! of the element.
      //!
      //! Returns an object whose defining data is \p vec.
      //! This constructor moves the data from \p vec, meaning that \p vec is
      //! changed by this constructor.
      explicit ElementWithVectorData(std::vector<TValueType>&& vec)
          : Element(), _vector(std::move(vec)) {}

      //! A copy constructor.
      //!
      //! The size of the vector containing the defining data of \c this will be
      //! increased by \p IncreaseDegree.  If \p IncreaseDegree is not 0,
      //! then this member function must be overridden by any subclass of
      //! ElementWithVectorData since there is no way of knowing how a subclass
      //! is defined by the data in the vector.
      ElementWithVectorData(ElementWithVectorData const& copy)
          : Element(copy._hash_value),
            _vector(copy._vector.cbegin(), copy._vector.cend()) {}

      //! Returns the product of \c this and \p y
      //!
      //! This returns the product of \c this and \p y, as defined by
      //! Element::redefine, without altering \c this or \p y.
      virtual TSubclass operator*(ElementWithVectorData const& y) const {
        TSubclass xy(y.degree());
        xy.Element::redefine(*this, y);
        return xy;
      }

      //! Returns the \p pos entry in the vector containing the defining data.
      //!
      //! This member function returns the \p pos entry in the vector used to
      //! construct \c this. No checks are performed that \p pos in within the
      //! bounds of this vector.
      inline TValueType operator[](size_t pos) const {
        return _vector[pos];
      }

      //! Returns the \p pos entry in the vector containing the defining data.
      //!
      //! This member function returns the \p pos entry in the vector used to
      //! construct \c this.
      inline TValueType at(size_t pos) const {
        return _vector.at(pos);
      }

      //! Returns \c true if \c this equals \p that.
      //!
      //! This member function checks that the underlying vectors of \c this and
      //! \p that are equal.
      bool operator==(Element const& that) const override {
        return static_cast<ElementWithVectorData const&>(that)._vector
               == this->_vector;
      }

      //! Returns \c true if \c this is less than \p that.
      //!
      //! This member function defines a total order on the set of objects in
      //! ElementWithVectorData of a given Element::degree, which is the
      //! short-lex order.
      bool operator<(Element const& that) const override {
        auto& ewvd = static_cast<ElementWithVectorData const&>(that);
        if (this->_vector.size() != ewvd._vector.size()) {
          return this->_vector.size() < ewvd._vector.size();
        }
        return this->_vector < ewvd._vector;
      }

      //! Swap another Element with \c this.
      //!
      //! This member function swaps the defining data of \p x and \c this.
      //! This member function asserts that the degrees of \c this and \p x are
      //! equal and then swaps the underlying vector of \c this with the
      //! underlying vector of \p x. Any member function overriding should swap
      //! the hash_value of \c this and \p x, or call Element::reset_hash_value
      //! on \c this and \p x.
      void swap(Element& x) override {
        LIBSEMIGROUPS_ASSERT(x.degree() == this->degree());
        auto& xx = static_cast<ElementWithVectorData&>(x);
        _vector.swap(xx._vector);
        std::swap(this->_hash_value, xx._hash_value);
      }

      void swap(Element&& x) {
        LIBSEMIGROUPS_ASSERT(x.degree() == this->degree());
        auto&& xx = static_cast<ElementWithVectorData&&>(x);
        _vector.swap(xx._vector);
        std::swap(this->_hash_value, xx._hash_value);
      }

      // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
      //       Element* empty_key() const override {
      //         // + 2 since bipartition must be of even degree!
      //         std::vector<TValueType> vector(this->degree() + 2);
      //         return new TSubclass(std::move(vector));
      //       }
      // #endif

      //! Returns an iterator.
      //!
      //! This member function returns an iterator pointing at the first entry
      //! in the vector that is the underlying defining data of \c this.
      inline typename std::vector<TValueType>::const_iterator begin() const {
        return _vector.begin();
      }

      //! Returns an iterator.
      //!
      //! This member function returns an iterator referring to the past-the-end
      //! element of the vector that is the underlying defining data of \c this.
      inline typename std::vector<TValueType>::const_iterator end() const {
        return _vector.end();
      }

      //! Returns a const iterator.
      //!
      //! This member function returns a const_iterator pointing at the first
      //! entry in the vector that is the underlying defining data of \c this.
      inline typename std::vector<TValueType>::const_iterator cbegin() const {
        return _vector.cbegin();
      }

      //! Returns a const iterator.
      //!
      //! This member function returns a const iterator referring to the
      //! past-the-end element of the vector that is the underlying defining
      //! data of \c this.
      inline typename std::vector<TValueType>::const_iterator cend() const {
        return _vector.cend();
      }

      //! Returns the identity of the \c TSubclass
      //!
      //! Returns an element of type TSubclass that is an identity for elements
      //! of type TSubclass.
      //! This must be defined in subclasses of ElementWithVectorData.
      virtual TSubclass identity() const = 0;

      static TSubclass identity(size_t degree);

      //! Returns a new identity for TSubclass
      //!
      //! Returns a pointer to an element that is an identity for elements
      //! of type TSubclass, and is independent from other copies that already
      //! may exist.
      Element* heap_identity() const override {
        return this->identity().heap_copy();
      }

      //! Returns a pointer to a new copy of \c this.
      //!
      //! Returns a pointer to an element that has the same defining data as \c
      //! this, but is independent in memory.
      Element* heap_copy() const override {
        return new TSubclass(*static_cast<TSubclass const*>(this));
      }

      //! Insertion operator
      //!
      //! This member function allows ElementWithVectorData objects to be
      //! inserted into an ostringstream.
      friend std::ostringstream& operator<<(std::ostringstream&          os,
                                            ElementWithVectorData const& elt) {
        os << elt._vector;
        return os;
      }

      //! Insertion operator
      //!
      //! This member function allows ElementWithVectorData objects to be
      //! inserted into an ostream.
      friend std::ostream& operator<<(std::ostream&                os,
                                      ElementWithVectorData const& elt) {
        os << detail::to_string(elt);
        return os;
      }

     protected:
      // Cannot declare cache_hash_value here, since PBR's are vectors of
      // vectors, and there is not std::hash<vector<whatever>>.
      //! Returns a hash value for a vector provided there is a specialization
      //! of std::hash for the template type \p T.
      template <typename T>
      static inline size_t vector_hash(std::vector<T> const& vec) {
        size_t seed = 0;
        for (auto const& x : vec) {
          seed ^= std::hash<T>{}(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
      }

      //! The vector containing the defining data of \c this.
      //!
      //! The actual data defining of \c this is stored in _vector.
      std::vector<TValueType> _vector;
    };

    //! Abstract base class for elements using a vector to store their defining
    //! data and the default hash function for that underlying vector.
    //!
    //! This class is almost the same as ElementWithVectorData, except that it
    //! also implements a member function for cache_hash_value, which uses
    //! ElementWithVectorData::vector_hash.
    template <typename TValueType, class TSubclass>
    class ElementWithVectorDataDefaultHash
        : public ElementWithVectorData<TValueType, TSubclass> {
     public:
      using ElementWithVectorData<TValueType, TSubclass>::ElementWithVectorData;
      ElementWithVectorDataDefaultHash()
          : ElementWithVectorData<TValueType, TSubclass>() {}

     protected:
      //! This member function implements the default hash function for an
      //! ElementWithVectorData, which uses ElementWithVectorData::vector_hash.
      //! Derive from this class if you are defining a class derived from
      //! ElementWithVectorData and there is a specialization of std::hash for
      //! the template parameter \p TValueType, and you do not want to define a
      //! hash function explicitly in your derived class.
      void cache_hash_value() const override {
        this->_hash_value = this->vector_hash(this->_vector);
      }
    };
  }  // namespace detail

  //! Abstract class for partial transformations.
  //!
  //! This is a template class for partial transformations.
  //!
  //! The template parameter \p T is the type of image values, i.e.
  //! uint16_t, and so on.  The value of the template parameter \p T can be
  //! used to reduce (or increase) the amount of memory required by instances
  //! of this class.
  //!
  //! The template parameter \p TSubclass is the subclass of
  //! PartialTransformation used by the PartialTransformation::identity member
  //! function to construct an identity, so that the return value of the member
  //! function PartialTransformation::identity is of type \p TSubclass instead
  //! of type PartialTransformation.
  //!
  //! This class is abstract but this will likely change in a future version of
  //! libsemigroups.
  //!
  //! A *partial transformation* \f$f\f$ is just a function defined on a subset
  //! of \f$\{0, 1, \ldots, n - 1\}\f$ for some integer \f$n\f$ called the
  //! *degree*  of *f*.  A partial transformation is stored as a vector of the
  //! images of \f$\{0, 1, \ldots, n -1\}\f$, i.e.
  //! \f$\{(0)f, (1)f, \ldots, (n - 1)f\}\f$
  //! where the value libsemigroups::UNDEFINED is used to
  //! indicate that \f$(i)f\f$ is, you guessed it, undefined (i.e. not among
  //! the points where \f$f\f$ is defined).
  template <typename T, typename TSubclass>
  class PartialTransformation
      : public detail::ElementWithVectorDataDefaultHash<T, TSubclass> {
   public:
    using detail::ElementWithVectorDataDefaultHash<T, TSubclass>::
        ElementWithVectorDataDefaultHash;

    //! A constructor.
    //!
    //! Constructs an uninitialised PartialTransformation.
    PartialTransformation()
        : detail::ElementWithVectorDataDefaultHash<T, TSubclass>() {}

    //! A constructor.
    //!
    //! Constructs an PartialTransformation *f* from \p vec and validates it.
    //! The vector \p vec (of length some non-negative integer *n*) should
    //! contain in position *i* the image \f$(i)f\f$, or
    //! libsemigroups::UNDEFINED if *f* is not defined on *i*, for all
    //! \f$0 < i < n\f$.
    explicit PartialTransformation(std::vector<T> const& vec)
        : detail::ElementWithVectorDataDefaultHash<T, TSubclass>(vec) {
      validate();
    }

    //! A constructor.
    //!
    //! Constructs a vector from \p imgs and calls the corresponding
    //! constructor.
    PartialTransformation(std::initializer_list<T> imgs)
        : PartialTransformation<T, TSubclass>(std::vector<T>(imgs)) {}

    //! Validates the data defining \c this.
    //!
    //! This member function throws a libsemigroups::LibsemigroupsException if
    //! any value of \c this is out of bounds (i.e. less than 0, greater than
    //! or equal to \c degree(), and not libsemigroups::UNDEFINED).
    void validate() const {
      for (auto const& val : this->_vector) {
        if ((val < 0 || val >= this->degree()) && val != UNDEFINED) {
          LIBSEMIGROUPS_EXCEPTION(
              "image value out of bounds, found %d, must be less than %d",
              static_cast<size_t>(val),
              this->degree());
        }
      }
    }

    //! Returns the approximate time complexity of multiplying two partial
    //! transformations.
    //!
    //! The approximate time complexity of multiplying partial transformations
    //! is just their degree.
    size_t complexity() const override {
      return this->_vector.size();
    }

    //! Returns the degree of a partial transformation.
    //!
    //! The *degree* of a partial transformation is the number of points used
    //! in its definition.
    size_t degree() const final {
      return this->_vector.size();
    }

    //! Returns the rank of a partial transformation.
    //!
    //! The *rank* of a partial transformation is the number of its distinct
    //! image values, not including libsemigroups::UNDEFINED. This
    //! function recomputes the return value every time it is called.
    size_t crank() const {
      _lookup.clear();
      _lookup.resize(degree(), false);
      size_t r = 0;
      for (auto const& x : this->_vector) {
        if (x != UNDEFINED && !_lookup[x]) {
          _lookup[x] = true;
          r++;
        }
      }
      return r;
    }

    //! Returns the identity transformation with same degree as \c this.
    //!
    //! This member function returns a new partial transformation with degree
    //! equal to the degree of \c this that fixes every value from *0* to the
    //! degree of \c this.
    TSubclass identity() const override {
      std::vector<T> vector(this->degree());
      std::iota(vector.begin(), vector.end(), 0);
      return TSubclass(std::move(vector));
    }

    //! Returns the identity transformation with \p n.
    //!
    //! This member function returns a new partial transformation with degree
    //! equal to \p n and that fixes every value from *0* to \p n.
    static TSubclass identity(size_t n) {
      std::vector<T> v(n);
      std::iota(v.begin(), v.end(), 0);
      return TSubclass(std::move(v));
    }

   private:
    // Used for determining rank, TODO(later) make thread-safe
    static std::vector<bool> _lookup;
  };

  template <typename TValueType, typename TSubclass>
  std::vector<bool> PartialTransformation<TValueType, TSubclass>::_lookup
      = std::vector<bool>();

  namespace detail {
    // Base class for member function redefine for Permutations and
    // Transformations
    template <typename TValueType, typename TSubclass>
    class TransfBase : public PartialTransformation<TValueType, TSubclass> {
     public:
      using PartialTransformation<TValueType, TSubclass>::PartialTransformation;
      TransfBase() : PartialTransformation<TValueType, TSubclass>() {}

      //! Multiply \p x and \p y and stores the result in \c this.
      //!
      //! See Element::redefine for more details about this member function.
      //!
      //! This member function asserts that the degrees of \p x, \p y, and \c
      //! this, are all equal, and that neither \p x nor \p y equals \c this.
      void redefine(Element const& x, Element const& y) override {
        LIBSEMIGROUPS_ASSERT(x.degree() == y.degree());
        LIBSEMIGROUPS_ASSERT(x.degree() == this->degree());
        LIBSEMIGROUPS_ASSERT(&x != this && &y != this);
        auto const& xx
            = static_cast<TransfBase<TValueType, TSubclass> const&>(x);
        auto const& yy
            = static_cast<TransfBase<TValueType, TSubclass> const&>(y);
        size_t const n = this->_vector.size();
        for (TValueType i = 0; i < n; i++) {
          (this->_vector)[i] = yy[xx[i]];
        }
        this->reset_hash_value();
      }

      using Element::redefine;
    };
  }  // namespace detail

  //! Template class for transformations.
  //!
  //! The value of the template parameter \p T can be used to reduce the amount
  //! of memory required by instances of this class; see PartialTransformation
  //! for more details.
  //!
  //! A *transformation* \f$f\f$ is just a function defined on the whole of
  //! \f$\{0, 1, \ldots, n - 1\}\f$ for some integer \f$n\f$ called the
  //! *degree* of \f$f\f$.  A transformation is stored as a vector of the
  //! images of \f$\{0, 1, \ldots, n - 1\}\f$, i.e.
  //! \f$\{(0)f, (1)f, \ldots, (n - 1)f\}\f$.
  template <typename TValueType>
  class Transformation
      : public detail::TransfBase<TValueType, Transformation<TValueType>> {
    static_assert(std::is_integral<TValueType>::value,
                  "template parameter TValueType must be an integral type");
    static_assert(!std::numeric_limits<TValueType>::is_signed,
                  "template parameter TValueType must be unsigned");

   public:
    using detail::TransfBase<TValueType,
                             Transformation<TValueType>>::TransfBase;
    Transformation()
        : detail::TransfBase<TValueType, Transformation<TValueType>>() {}

    //! A constructor.
    //!
    //! Constructs a Transformation *f* of degree \p vec.size() from \p vec,
    //! where the image of a point *i* is given by \p vec[i].
    explicit Transformation(std::vector<TValueType> const& vec)
        : detail::TransfBase<TValueType, Transformation<TValueType>>(vec) {
      validate();
    }

    //! A constructor.
    //!
    //! The parameter \p vector should be a rvalue reference to defining data
    //! of the transformation.
    //!
    //! Returns an transformation whose defining data is \p vec.
    //! This constructor moves the data from \p vec, meaning that \p vec is
    //! changed by this constructor.
    explicit Transformation(std::vector<TValueType>&& vec)
        : detail::TransfBase<TValueType, Transformation<TValueType>>(
            std::move(vec)) {
      validate();
    }

    //! A constructor.
    //!
    //! Constructs a Transformation *f* of degree \p imgs.size() from \p imgs,
    //! where the image of a point *i* is given by \p imgs[i].
    Transformation(std::initializer_list<TValueType> imgs)
        : Transformation<TValueType>(std::vector<TValueType>(imgs)) {}

    //! A copy constructor.
    //!
    //! Constructs a Transformation which is mathematically equal to \p copy.
    Transformation(Transformation<TValueType> const& copy)
        : detail::TransfBase<TValueType, Transformation<TValueType>>(copy) {}

    //! Validates the data defining \c this.
    //!
    //! This member function throws a libsemigroups::LibsemigroupsException if
    //! any value of \c this is out of bounds (i.e. greater than or equal to \c
    //! this->degree()).
    void validate() const {
      size_t deg = this->degree();
      for (auto const& val : this->_vector) {
        if (val >= deg) {
          LIBSEMIGROUPS_EXCEPTION(
              "image value out of bounds, found %d, must be less than %d",
              static_cast<size_t>(val),
              deg);
        }
      }
    }

    void increase_degree_by(size_t m) override {
      this->_vector.resize(this->_vector.size() + m);
      std::iota(this->_vector.end() - m,
                this->_vector.end(),
                this->_vector.size() - m);
      this->reset_hash_value();
    }

   protected:
    //! This member function is included because it seems to give superior
    //! performance in some benchmarks.
    void cache_hash_value() const override {
      size_t seed = 0;
      size_t deg  = this->_vector.size();
      for (auto const& val : this->_vector) {
        seed *= deg;
        seed += val;
      }
      this->_hash_value = seed;
    }
  };

  //! Template class for partial permutations.
  //!
  //! \tparam T the value of this template parameter can be used to reduce the
  //! amount of memory required by instances of this class. This must be an
  //! unsigned integral type.
  //!
  //! \par
  //! A *partial permutation* \f$f\f$ is just an injective partial
  //! transformation, which is stored as a vector of the images of
  //! \f$\{0, 1, \ldots, n - 1\}\f$, i.e.
  //! \f$\{(0)f, (1)f, \ldots, (n - 1)f\}\f$
  //! where the value libsemigroups::UNDEFINED is used to indicate that
  //! \f$(i)f\f$ is undefined (i.e. not among the points where \f$f\f$ is
  //! defined).
  template <typename T>
  class PartialPerm : public PartialTransformation<T, PartialPerm<T>> {
    static_assert(std::is_integral<T>::value,
                  "template parameter T must be an integral type");
    static_assert(!std::numeric_limits<T>::is_signed,
                  "template parameter T must be unsigned");

   public:
    using PartialTransformation<T, PartialPerm<T>>::PartialTransformation;

    //! A constructor.
    //!
    //! Constructs a PartialPerm using the corresponding PartialTransformation
    //! constructor and then checks that it represents a valid PartialPerm.
    explicit PartialPerm(std::vector<T> const& vec)
        : PartialTransformation<T, PartialPerm<T>>(vec) {
      validate();
    }

    //! A constructor.
    //!
    //! The parameter \p vector should be a rvalue reference to defining data
    //! of a PartialPerm.
    //!
    //! Returns a PartialPerm whose defining data is \p vec.
    //! This constructor moves the data from \p vec, meaning that \p vec is
    //! changed by this constructor.
    explicit PartialPerm(std::vector<T>&& vec)
        : PartialTransformation<T, PartialPerm<T>>(std::move(vec)) {
      validate();
    }

    //! A constructor.
    //!
    //! Constructs a vector from \p imgs and uses the corresponding constructor.
    PartialPerm(std::initializer_list<T> imgs)
        : PartialPerm<T>(std::vector<T>(imgs)) {}

    //! A constructor.
    //!
    //! Constructs a partial perm of degree \p deg such that
    //! \code (dom[i])f = ran[i] \endcode
    //! for all \c i and which is undefined on every other
    //! value in the range 0 to (strictly less than \p deg). This member
    //! function asserts that \p dom and \p ran have equal size and that \p deg
    //! is greater than to the maximum value in \p dom or \p ran.
    PartialPerm(std::vector<T> const& dom,
                std::vector<T> const& ran,
                size_t                deg)
        // The vector passed in the next line shouldn't be necessary, but with
        // GCC5 PartialPerm fails to inherit the 0-param constructor from
        // PartialTransformation.
        : PartialTransformation<T, PartialPerm<T>>(std::vector<T>()) {
      if (dom.size() != ran.size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "domain and range size mismatch, domain has size %d "
            " but range has size %d",
            dom.size(),
            ran.size());
      } else if (!(dom.empty()
                   || deg > *std::max_element(dom.cbegin(), dom.cend()))) {
        LIBSEMIGROUPS_EXCEPTION(
            "domain value out of bounds, found %d, must be less than %d",
            static_cast<size_t>(*std::max_element(dom.cbegin(), dom.cend())),
            deg);
      }
      for (auto it = dom.cbegin(); it < dom.cend(); ++it) {
        if (std::find(it + 1, dom.cend(), *it) != dom.cend()) {
          LIBSEMIGROUPS_EXCEPTION("duplicate value %d in domain",
                                  static_cast<size_t>(*it));
        }
      }
      this->_vector.resize(deg, UNDEFINED);
      for (size_t i = 0; i < dom.size(); i++) {
        this->_vector[dom[i]] = ran[i];
      }
      validate();
    }

    //! A constructor.
    //!
    //! Constructs vectors from \p dom and \p ran and uses the constructor
    //! above.
    PartialPerm(std::initializer_list<T> dom,
                std::initializer_list<T> ran,
                size_t                   deg)
        : PartialPerm<T>(std::vector<T>(dom), std::vector<T>(ran), deg) {}

    //! Validates the data defining \c this.
    //!
    //! This member function throws a libsemigroups::LibsemigroupsException if
    //! any value of \c this is out of bounds (i.e. greater than or equal to \c
    //! degree()), and not equal to libsemigroups::UNDEFINED), or if any
    //! image appears more than once.
    void validate() const {
      size_t const      deg = this->degree();
      std::vector<bool> present(deg, false);
      for (auto const& val : this->_vector) {
        if (val != UNDEFINED) {
          if (val < 0 || val >= this->degree()) {
            LIBSEMIGROUPS_EXCEPTION(
                "image value out of bounds, found %d, must be less than %d",
                static_cast<size_t>(val),
                this->degree());
          } else if (present[val]) {
            LIBSEMIGROUPS_EXCEPTION("duplicate image value %d",
                                    static_cast<size_t>(val));
          }
          present[val] = true;
        }
      }
    }

    //! A copy constructor.
    PartialPerm<T>(PartialPerm const& copy)
        : PartialTransformation<T, PartialPerm<T>>(copy) {}

    void increase_degree_by(size_t m) override {
      this->_vector.insert(this->_vector.end(), m, UNDEFINED);
      this->reset_hash_value();
    }

    //! Returns \c true if \c this is less than \p that.
    //!
    //! This defines a total order on partial permutations that is equivalent
    //! to that used by GAP. It is not short-lex on the list of images.
    //!
    //! Returns \c true if something complicated is \c true and \c false if
    //! it is not.
    bool operator<(const Element& that) const override {
      auto pp_this = static_cast<const PartialPerm<T>*>(this);
      auto pp_that = static_cast<const PartialPerm<T>&>(that);

      size_t deg_this = pp_this->degree();
      for (auto it = pp_this->_vector.end() - 1; it >= pp_this->_vector.begin();
           it--) {
        if (*it == UNDEFINED) {
          deg_this--;
        } else {
          break;
        }
      }
      size_t deg_that = pp_that.degree();
      for (auto it = pp_that._vector.end() - 1;
           it >= pp_that._vector.begin() && deg_that >= deg_this;
           it--) {
        if (*it == UNDEFINED) {
          deg_that--;
        } else {
          break;
        }
      }

      if (deg_this != deg_that) {
        return deg_this < deg_that;
      }

      for (size_t i = 0; i < deg_this; i++) {
        if ((*pp_this)[i] != pp_that[i]) {
          return (*pp_this)[i] == UNDEFINED
                 || (pp_that[i] != UNDEFINED && (*pp_this)[i] < pp_that[i]);
        }
      }
      return false;
    }

    //! Multiply \p x and \p y and stores the result in \c this.
    //!
    //! See Element::redefine for more details about this member function.
    //!
    //! This member function asserts that the degrees of \p x, \p y, and \c
    //! this, are all equal, and that neither \p x nor \p y equals \c this.
    void redefine(Element const& x, Element const& y) override {
      LIBSEMIGROUPS_ASSERT(x.degree() == y.degree());
      LIBSEMIGROUPS_ASSERT(x.degree() == this->degree());
      LIBSEMIGROUPS_ASSERT(&x != this && &y != this);
      auto const&  xx = static_cast<PartialPerm<T> const&>(x);
      auto const&  yy = static_cast<PartialPerm<T> const&>(y);
      size_t const n  = this->degree();
      for (T i = 0; i < n; i++) {
        this->_vector[i] = (xx[i] == UNDEFINED ? UNDEFINED : yy[xx[i]]);
      }
      this->reset_hash_value();
    }

    using Element::redefine;

    //! Returns the rank of a partial permutation.
    //!
    //! The *rank* of a partial permutation is the number of its distinct image
    //! values, not including libsemigroups::UNDEFINED. This member function
    //! involves slightly less work than PartialTransformation::crank since a
    //! partial permutation is injective, and so every image value occurs
    //! precisely once. This member function recomputes the return value every
    //! time it is called.
    size_t crank() const {
      return this->_vector.size()
             - std::count(
                 this->_vector.cbegin(), this->_vector.cend(), UNDEFINED);
    }

    //! Returns the idempotent \f$x ^ {-1} x\f$ where \f$x\f$ is the partial
    //! perm represented by \c this.
    PartialPerm<T> right_one() const {
      std::vector<T> img(this->degree(), static_cast<T>(UNDEFINED));
      size_t const   size = this->_vector.size();
      for (size_t i = 0; i < size; ++i) {
        if (this->_vector[i] != UNDEFINED) {
          img[this->_vector[i]] = this->_vector[i];
        }
      }
      return PartialPerm<T>(img);
    }

    //! Returns the idempotent \f$xx ^ {-1}\f$ where \f$x\f$ is the partial
    //! perm represented by \c this.
    PartialPerm<T> left_one() const {
      std::vector<T> dom(this->degree(), static_cast<T>(UNDEFINED));
      size_t const   size = this->_vector.size();
      for (size_t i = 0; i < size; ++i) {
        if (this->_vector[i] != UNDEFINED) {
          dom[i] = i;
        }
      }
      return PartialPerm<T>(dom);
    }

    // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
    //     Element* empty_key() const override {
    //       std::vector<T> vector(this->degree() + 1);
    //       std::iota(vector.begin(), vector.end(), 0);
    //       return new PartialPerm(std::move(vector));
    //     }
    // #endif
  };

  //! Class for bipartitions.
  //!
  //! A *bipartition* is a partition of the set
  //! \f$\{0, ..., 2n - 1\}\f$ for some integer \f$n\f$; see the
  //! [Semigroups package for GAP
  //! documentation](https://gap-packages.github.io/Semigroups/doc/chap3_mj.html)
  //! for more details.

  //! The Bipartition class is more complex (i.e. has more member functions)
  //! than strictly required by the algorithms for a FroidurePin object because
  //! the extra member functions are used in the GAP package [Semigroups
  //! package for GAP](https://gap-packages.github.io/Semigroups/).
  class Bipartition
      : public detail::ElementWithVectorDataDefaultHash<uint32_t, Bipartition> {
    // TODO(later) add more explanation to the doc here
   public:
    //! A constructor.
    //!
    //! Constructs a uninitialised bipartition.
    Bipartition();

    //! A constructor.
    //!
    //! Constructs a uninitialised bipartition of degree \p degree.
    explicit Bipartition(size_t);

    //! A constructor.
    //!
    //! The parameter \p blocks must have length *2n* for some positive integer
    //! *n*, consist of non-negative integers, and have the property that if
    //! *i*, *i > 0*, occurs in \p blocks, then *i - 1* occurs earlier in
    //! blocks.
    //!
    //! The parameter \p blocks is copied.
    explicit Bipartition(std::vector<uint32_t> const&);

    //! A constructor.
    //!
    //! The parameter \p vector should be a rvalue reference to defining data
    //! of the Bipartition.
    //!
    //! Returns a Bipartition whose defining data is \p vec.
    //! This constructor moves the data from \p vec, meaning that \p vec is
    //! changed by this constructor.
    explicit Bipartition(std::vector<uint32_t>&&);

    //! A constructor.
    //!
    //! Converts \p blocks to a vector and uses corresponding constructor.
    Bipartition(std::initializer_list<uint32_t> blocks);

    //! A copy constructor.
    //!
    //! Constructs a Bipartition that is mathematically equal to \p copy.
    Bipartition(Bipartition const&) = default;

    //! A constructor.
    //!
    //! The argument \p blocks should be a list of vectors which partition the
    //! ranges [-n .. -1] U [1 .. n] for some positive integer *n*, called the
    //! degree of the bipartition. The bipartition constructed has equivalence
    //! classes given by the vectors in \p blocks.
    Bipartition(std::initializer_list<std::vector<int32_t>> const&);

    //! Validates the data defining \c this.
    //!
    //! This member function throws a libsemigroups::LibsemigroupsException if
    //! the data defining \c this is invalid, which could occur if:
    //!
    //! - \c this->_vector has odd length, or
    //!
    //! - a positive integer *i* occurs in \c this->_vector before the integer
    //! *i* - 1
    void validate() const;

    //! Returns the approximate time complexity of multiplication.
    //!
    //! In the case of a Bipartition of degree *n* the value *2n ^ 2* is
    //! returned.
    size_t complexity() const override;

    //! Returns the degree of the bipartition.
    //!
    //! A bipartition is of degree *n* if it is a partition of
    //! \f$\{0, \ldots, 2n -  1\}\f$.
    size_t degree() const override;

    //! Returns an identity bipartition.
    //!
    //! The *identity bipartition* of degree \f$n\f$ has blocks \f$\{i, -i\}\f$
    //! for all \f$i\in \{0, \ldots, n - 1\}\f$. This member function returns a
    //! new identity bipartition of degree equal to the degree of \c this.
    Bipartition identity() const override;

    //! Returns an identity bipartition.
    //!
    //! The *identity bipartition* of degree \f$n\f$ has blocks \f$\{i, -i\}\f$
    //! for all \f$i\in \{0, \ldots, n - 1\}\f$. This member function returns a
    //! new identity bipartition of degree equal to \p n.
    static Bipartition identity(size_t n);

    //! Multiply \p x and \p y and stores the result in \c this.
    //!
    //! This member function redefines \c this to be the product (as defined at
    //! the top of this page) of the parameters  \p x and \p y. This member
    //! function asserts that the degrees of \p x, \p y, and \c this, are all
    //! equal, and that neither \p x nor  \p y equals \c this.
    //!
    //! The parameter \p thread_id is required since some temporary storage is
    //! required to find the product of \p x and \p y.  Note that if different
    //! threads call this member function with the same value of \p thread_id
    //! then bad things will happen.
    void redefine(Element const&, Element const&, size_t) override;

    //! Returns the number of transverse blocks.
    //!
    //! The *rank* of a bipartition is the number of blocks containing both
    //! positive and negative values.  This value is cached after it is first
    //! computed.
    size_t rank();

    //! Returns the number of blocks in a bipartition.
    //!
    //! This member function differs for Bipartition::nr_blocks in that the
    //! number of blocks is not cached if it has not been previously computed.
    uint32_t const_nr_blocks() const;

    //! Returns the number of blocks in a bipartition.
    //!
    //! This value is cached the first time it is computed.
    uint32_t nr_blocks();

    //! Returns the number of blocks containing a positive integer.
    //!
    //! The *left blocks* of a bipartition is the partition of
    //! \f$\{0, \ldots, n - 1\}\f$ induced by the bipartition. This member
    //! function returns the number of blocks in this partition.
    uint32_t nr_left_blocks();

    //! Returns the number of blocks containing a negative integer.
    //!
    //! The *right blocks* of a bipartition is the partition of
    //! \f$\{n, \ldots, 2n - 1\}\f$ induced by the bipartition. This member
    //! function returns the number of blocks in this partition.
    uint32_t nr_right_blocks();

    //! Returns \c true if the block with index \p index is transverse.
    //!
    //! A block of a biparition is *transverse* if it contains integers less
    //! than and greater than \f$n\f$, which is the degree of the bipartition.
    //! This member function asserts that the parameter \p index is less than
    //! the number of blocks in the bipartition.
    bool is_transverse_block(size_t);

    //! Return the left blocks of a bipartition
    //!
    //! The *left blocks* of a bipartition is the partition of
    //! \f$\{0, \ldots, n - 1\}\f$ induced by the bipartition. This member
    //! function returns a Blocks object representing this partition.
    Blocks* left_blocks();

    //! Return the left blocks of a bipartition
    //!
    //! The *right blocks* of a bipartition is the partition of
    //! \f$\{n, \ldots, 2n - 1\}\f$ induced by the bipartition. This member
    //! function returns a Blocks object representing this partition.
    Blocks* right_blocks();

    //! Set the cached number of blocks
    //!
    //! This member function sets the cached value of the number of blocks of \c
    //! this to \p nr_blocks. It asserts that either there is no existing cached
    //! value or \p nr_blocks equals the existing cached value.
    void set_nr_blocks(size_t);

    //! Set the cached number of left blocks
    //!
    //! This member function sets the cached value of the number of left blocks
    //! of \c this to \p nr_left_blocks. It asserts that either there is no
    //! existing cached value or \p nr_left_blocks equals the existing cached
    //! value.
    void set_nr_left_blocks(size_t);

    //! Set the cached rank
    //!
    //! This member function sets the cached value of the rank of \c this to \p
    //! rank. It asserts that either there is no existing cached value or \p
    //! rank equals the existing cached value.
    void set_rank(size_t);

   private:
    static std::vector<uint32_t>
             blocks_to_list(std::vector<std::vector<int32_t>> blocks);
    uint32_t fuseit(std::vector<uint32_t>& fuse, uint32_t pos);
    void     init_trans_blocks_lookup();

    static std::vector<std::vector<uint32_t>> _fuse;
    static std::vector<std::vector<uint32_t>> _lookup;

    size_t            _nr_blocks;
    size_t            _nr_left_blocks;
    std::vector<bool> _trans_blocks_lookup;
    size_t            _rank;
  };

  namespace detail {
    //! Matrices over a semiring.
    //!
    //! This class is abstract since it does not implement all member functions
    //! required by the Element class, it exists to provide common member
    //! functions for its subclasses.
    //!
    //! The template parameter \p TValueType is the type of the entries in the
    //! matrix, which must also be the type of the corresponding template
    //! parameter for the Semiring object used to define an instance of
    //! a MatrixOverSemiringBase.
    //!
    //! The template parameter \p TSubclass is the type of a subclass of
    //! MatrixOverSemiringBase, it is used so that identity() can return an
    //! instance of the correct type.
    template <typename TValueType, class TSubclass>
    class MatrixOverSemiringBase
        : public ElementWithVectorDataDefaultHash<TValueType, TSubclass> {
     public:
      //! A constructor.
      //!
      //! Constructs a matrix defined by \p matrix.
      //!
      //! The parameter \p matrix should be a vector of integer values of length
      //! \f$n ^ 2\f$ for some integer \f$n\f$, so that the value in position
      //! \f$in + j\f$ is the entry in the \f$i\f$th row and \f$j\f$th column of
      //! the constructed matrix.
      //!
      //! The parameter \p semiring should be a pointer to a Semiring, which
      //! is the semiring over which the matrix is defined.
      //!
      //! This member function asserts that the parameter \p semiring is not a
      //! nullptr, that the vector \p matrix has size a non-zero perfect square.
      MatrixOverSemiringBase(std::vector<TValueType> const& matrix,
                             Semiring<TValueType> const*    semiring)
          : ElementWithVectorDataDefaultHash<TValueType, TSubclass>(matrix),
            _degree(sqrt(matrix.size())),
            _semiring(semiring) {
        validate();
      }

      //! A constructor.
      //!
      //! Constructs a matrix defined by \p matrix.
      //!
      //! The parameter \p matrix should be a rvalue reference to a vector of
      //! integer values of length \f$n ^ 2\f$ for some integer \f$n\f$, so that
      //! the value in position \f$in + j\f$ is the entry in the \f$i\f$th row
      //! and \f$j\f$th column of the constructed matrix.
      //!
      //! The parameter \p semiring should be a pointer to a Semiring, which
      //! is the semiring over which the matrix is defined.
      //!
      //! This member function asserts that the parameter \p semiring is not a
      //! nullptr, and that the vector \p matrix has size a non-zero perfect
      //! square.
      //!
      //! This member function moves the parameter \p matrix.
      MatrixOverSemiringBase(std::vector<TValueType>&&   matrix,
                             Semiring<TValueType> const* semiring)
          : ElementWithVectorDataDefaultHash<TValueType, TSubclass>(
              std::move(matrix)),
            _degree(sqrt(this->_vector.size())),
            _semiring(semiring) {
        validate();
      }

      //! A constructor.
      //!
      //! Constructs a matrix defined by \p matrix, which is copied into the
      //! constructed object.
      //!
      //! The parameter \p matrix should be a vector of integer values of length
      //! \f$n ^ 2\f$ for some integer \f$n\f$, so that the value in position
      //! \f$in + j\f$ is the entry in the \f$i\f$th row and \f$j\f$th column of
      //! the constructed matrix.
      //!
      //! The parameter \p semiring should be a pointer to a Semiring, which
      //! is the semiring over which the matrix is defined.
      //!
      //! This member function asserts that the paramater \p semiring is not a
      //! nullptr, that the vector \p matrix is not empty, and that every vector
      //! contained in \p matrix has the same length as \p matrix.
      //!
      //! Note that it is the responsibility of the caller to delete the
      //! parameter \p semiring, and that this should only be done after every
      //! MatrixOverSemiringBase object that was constructed using \p semiring
      //! has been deleted.
      MatrixOverSemiringBase(std::vector<std::vector<TValueType>> const& matrix,
                             Semiring<TValueType> const* semiring)
          : ElementWithVectorDataDefaultHash<TValueType, TSubclass>(),
            _degree(),
            _semiring(semiring) {
        if (semiring == nullptr) {
          LIBSEMIGROUPS_EXCEPTION("semiring is nullptr");
        } else if (matrix.empty()) {
          LIBSEMIGROUPS_EXCEPTION("matrix has dimension 0");
        } else if (!all_of(matrix.cbegin(),
                           matrix.cend(),
                           [&matrix](std::vector<TValueType> row) {
                             return row.size() == matrix.size();
                           })) {
          LIBSEMIGROUPS_EXCEPTION("matrix is not square");
        }
        _degree = matrix[0].size();
        this->_vector.reserve(matrix.size() * matrix.size());
        for (auto const& row : matrix) {
          this->_vector.insert(this->_vector.end(), row.begin(), row.end());
        }
        validate();
      }

      //! A copy constructor.
      //!
      //! The parameter \p IncreaseDegree must be 0, since it does not make
      //! sense to increase the degree of a matrix.
      MatrixOverSemiringBase(MatrixOverSemiringBase const& copy)
          : ElementWithVectorDataDefaultHash<TValueType, TSubclass>(
              copy._vector),
            _degree(copy._degree),
            _semiring(copy._semiring) {}

      //! Validates the data defining \c this.
      //!
      //! This member function throws a libsemigroups::LibsemigroupsException if
      //! any value of \c this is not in the underlying semiring.
      void validate() const {
        if (this->degree() * this->degree() != this->_vector.size()) {
          LIBSEMIGROUPS_EXCEPTION(
              "matrix must have size that is a perfect square");
        }
        for (auto x : this->_vector) {
          if (!this->_semiring->contains(x)) {
            LIBSEMIGROUPS_EXCEPTION(
                "matrix contains entry %d not in the underlying semiring",
                static_cast<size_t>(x));
          }
        }
      }

      //! Returns a pointer to the Semiring over which the matrix is defined.
      Semiring<TValueType> const* semiring() const {
        return _semiring;
      }

      //! Returns the approximate time complexity of multiplying two matrices.
      //!
      //! The approximate time complexity of multiplying matrices is \f$n ^ 3\f$
      //! where \f$n\f$ is the dimension of the matrix.
      size_t complexity() const override {
        return pow(this->degree(), 3);
      }

      //! Returns the dimension of the matrix.
      //!
      //! The *dimension* of a matrix is just the number of rows (or,
      //! equivalently columns).
      size_t degree() const override {
        return _degree;
      }

      //! Returns the identity matrix with dimension of \c this.
      //!
      //! This member function returns a new matrix with dimension equal to that
      //! of \c this, where the main diagonal consists of the value
      //! Semiring::one and every other entry Semiring::zero.
      TSubclass identity() const override {
        std::vector<TValueType> vector(this->_vector.size(), _semiring->zero());
        size_t                  n = this->degree();
        for (auto it = vector.begin(); it < vector.end(); it += n + 1) {
          (*it) = _semiring->one();
        }
        return TSubclass(vector, _semiring);
      }

      static TSubclass identity(size_t n, Semiring<TValueType>* sr) {
        std::vector<TValueType> vector(n, sr->zero());
        for (auto it = vector.begin(); it < vector.end(); it += n + 1) {
          (*it) = sr->one();
        }
        return TSubclass(vector, sr);
      }

      //! Multiplies \p x and \p y and stores the result in \c this.
      //!
      //! this member function asserts that the degrees of \p x, \p y, and \c
      //! this, are all equal, and that neither \p x nor \p y equals \c this. it
      //! does not currently verify that \c x, \c y, and \c this are defined
      //! over the same semiring.
      void redefine(Element const& x, Element const& y) override {
        auto xx = static_cast<MatrixOverSemiringBase const&>(x);
        auto yy = static_cast<MatrixOverSemiringBase const&>(y);
        LIBSEMIGROUPS_ASSERT(xx.degree() == yy.degree());
        LIBSEMIGROUPS_ASSERT(xx.degree() == this->degree());
        LIBSEMIGROUPS_ASSERT(&xx != this && &yy != this);
        // It can be that the elements are defined over semirings that are
        // distinct in memory but equal (for example, when one element comes
        // from a semigroup and another from an ideal of that semigroup).
        // LIBSEMIGROUPS_ASSERT(xx->semiring() == yy->semiring() &&
        // xx->semiring() == this->semiring());
        // TODO(later) verify that x, y, and this are defined over the same
        // semiring.
        size_t const deg = this->degree();

        for (size_t i = 0; i < deg; i++) {
          for (size_t j = 0; j < deg; j++) {
            int64_t v = _semiring->zero();
            for (size_t k = 0; k < deg; k++) {
              v = _semiring->plus(
                  v, _semiring->prod(xx[i * deg + k], yy[k * deg + j]));
            }
            (this->_vector)[i * deg + j] = v;
          }
        }
        after();  // post process this
        this->reset_hash_value();
      }
      using Element::redefine;

      // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
      //       Element* empty_key() const override {
      //         std::vector<TValueType> vector((this->degree() + 1)
      //                                        * (this->degree() + 1));
      //         return new TSubclass(std::move(vector), _semiring);
      //       }
      // #endif

     protected:
      friend class ElementWithVectorData<TValueType, TSubclass>;
      //! Constructs a MatrixOverSemiringBase whose underlying semiring is
      //! not defined. The underlying semiring must be set by any class deriving
      //! from this one.
      explicit MatrixOverSemiringBase(std::vector<TValueType> const& matrix)
          : ElementWithVectorDataDefaultHash<TValueType, TSubclass>(matrix),
            _degree(sqrt(matrix.size())),
            _semiring(nullptr) {}

      //! Constructs a MatrixOverSemiringBase of degree 0 whose underlying
      //! semiring is not defined. The underlying semiring must be set by any
      //! class deriving from this one.
      explicit MatrixOverSemiringBase(TValueType = 0)
          : ElementWithVectorDataDefaultHash<TValueType, TSubclass>(),
            _degree(0),
            _semiring(nullptr) {}

     private:
      // A function applied after redefinition
      virtual inline void         after() {}
      size_t                      _degree;
      Semiring<TValueType> const* _semiring;
    };
  }  // namespace detail

  //! Matrices over a semiring.
  //!
  //! This class is partial specialization of MatrixOverSemiringBase that has
  //! only one template parameter \p TValueType, and that uses
  //! MatrixOverSemiring<TValueType> as the second template parameter for
  //! MatrixOverSemiringBase.
  template <typename TValueType>
  class MatrixOverSemiring
      : public detail::MatrixOverSemiringBase<TValueType,
                                              MatrixOverSemiring<TValueType>> {
    friend class detail::ElementWithVectorData<TValueType,
                                               MatrixOverSemiring<TValueType>>;
    using detail::MatrixOverSemiringBase<
        TValueType,
        MatrixOverSemiring<TValueType>>::MatrixOverSemiringBase;

   public:
    //! Returns the product of \c this and \p y
    //!
    //! This returns the product of \c this and \p y, as defined by
    //! MatrixOverSemiring::redefine, without altering \c this or \p y.
    //! This is specialised to avoid trying to call
    //! MatrixOverSemiring::MatrixOverSemiring(degree) in Element::redefine.
    MatrixOverSemiring operator*(
        detail::ElementWithVectorData<TValueType,
                                      MatrixOverSemiring<TValueType>> const& y)
        const override {
      MatrixOverSemiring<TValueType> xy(
          std::vector<TValueType>(pow(y.degree(), 2)), this->semiring());
      xy.Element::redefine(*this, y);
      return xy;
    }
  };

  //! Class for projective max-plus matrices.
  //!
  //! These matrices belong to the quotient of the monoid of all max-plus
  //! matrices by the congruence where two matrices are related if they differ
  //! by a scalar multiple. \sa MaxPlusSemiring and MatrixOverSemiring.
  //!
  //! Matrices in this class are modified when constructed to be in a normal
  //! form which is obtained by subtracting the maximum finite entry in the
  //! matrix from the every finite entry.
  class ProjectiveMaxPlusMatrix
      : public detail::MatrixOverSemiringBase<int64_t,
                                              ProjectiveMaxPlusMatrix> {
   public:
    //! A constructor.
    //!
    //! See MatrixOverSemiring::MatrixOverSemiring for details about this
    //! member function.
    //!
    //! The parameter \p matrix is converted into its normal form when
    //! when the object is constructed.
    ProjectiveMaxPlusMatrix(std::vector<int64_t> const&,
                            Semiring<int64_t> const*);

    //! A constructor.
    //!
    //! See MatrixOverSemiring::MatrixOverSemiring for details about this
    //! member function.
    //!
    //! The copy of the parameter \p matrix in the object constructed is
    //! converted into its normal form when the object is constructed.
    ProjectiveMaxPlusMatrix(std::vector<std::vector<int64_t>> const&,
                            Semiring<int64_t> const*);
    //! A copy constructor.
    ProjectiveMaxPlusMatrix(ProjectiveMaxPlusMatrix const&) = default;

    //! Returns the product of \c this and \p y
    //!
    //! This returns the product of \c this and \p y, as defined by
    //! ProjectiveMaxPlusMatrix::redefine, without altering \c this or \p y.
    //! This is specialised to avoid trying to call
    //! ProjectiveMaxPlusMatrix::ProjectiveMaxPlusMatrix(degree) in
    //! Element::redefine (as for MatrixOverSemiring).
    ProjectiveMaxPlusMatrix operator*(ElementWithVectorData const&)
        const override;

   private:
    // This constructor only exists to make the empty_key member function for
    // detail::ElementWithVectorData work, and because the compiler complains
    // about the Element::operator* without it.
    explicit ProjectiveMaxPlusMatrix(int64_t);

    friend class detail::ElementWithVectorData<int64_t,
                                               ProjectiveMaxPlusMatrix>;

    // TODO(later) pass by value, really?
    explicit ProjectiveMaxPlusMatrix(std::vector<int64_t>);

    // A function applied after redefinition
    void after() final;
  };

  //! Template class for permutations.
  //!
  //! The value of the template parameter \p T can be used to reduce the amount
  //! of memory required by instances of this class; see PartialTransformation
  //! and detail::ElementWithVectorData for more details.
  //!
  //! A *permutation* \f$f\f$ is an injective transformation defined on the
  //! whole of \f$\{0, 1, \ldots, n - 1\}\f$ for some integer \f$n\f$ called the
  //! *degree* of \f$f\f$.
  //! A permutation is stored as a vector of the images of
  //! \f$(0, 1, \ldots, n - 1)\f$,
  //! i.e. \f$((0)f, (1)f, \ldots, (n - 1)f)\f$.
  template <typename TValueType>
  class Permutation
      : public detail::TransfBase<TValueType, Permutation<TValueType>> {
   public:
    using detail::TransfBase<TValueType, Permutation<TValueType>>::TransfBase;

    //! Validates the data defining \c this.
    //!
    //! This member function throws a libsemigroups::LibsemigroupsException if
    //! any value of \c this is out of bounds (i.e. greater than or equal to \c
    //! this->degree()), or if any image appears more than once.
    void validate() const {
      std::vector<bool> present(this->degree(), false);
      for (auto const& val : this->_vector) {
        if (val < 0 || val >= this->degree()) {
          LIBSEMIGROUPS_EXCEPTION(
              "image value out of bounds, found %d, must be less than %d",
              static_cast<size_t>(val),
              this->degree());
        } else if (present[val]) {
          LIBSEMIGROUPS_EXCEPTION("duplicate image value %d",
                                  static_cast<size_t>(val));
        }
        present[val] = true;
      }
    }

    //! \copydoc PartialTransformation::identity
    Permutation identity() const override {
      return this->PartialTransformation<TValueType,
                                         Permutation<TValueType>>::identity();
    }

    //! \copydoc PartialTransformation::identity(size_t)
    static Permutation identity(size_t n) {
      return PartialTransformation<TValueType,
                                   Permutation<TValueType>>::identity(n);
    }

    //! Returns the inverse of a permutation.
    //!
    //! The *inverse* of a permutation \f$f\f$ is the permutation \f$g\f$ such
    //! that \f$fg = gf\f$ is the identity permutation of degree \f$n\f$.
    Permutation inverse() const {
      size_t const n  = this->_vector.size();
      Permutation  id = this->identity();
      for (TValueType i = 0; i < n; i++) {
        id._vector[this->_vector[i]] = i;
      }
      return id;
    }
  };

  //! Matrices over the boolean semiring.
  //!
  //! A *boolean matrix* is a square matrix over the boolean semiring, under
  //! the usual multiplication of matrices.
  //!
  //! Every boolean matrix is defined over a single static copy of the boolean
  //! semiring.
  class BooleanMat : public detail::MatrixOverSemiringBase<bool, BooleanMat> {
   public:
    //! A constructor.
    //!
    //! Constructs a boolean matrix defined by \p matrix.
    //!
    //! The parameter \p matrix should be a vector of boolean values of length
    //! \f$n ^  2\f$ for some integer \f$n\f$, so that the value in position
    //! \f$ni + j\f$ is the entry in row \f$i\f$ and column \f$j\f$ of
    //! the constructed matrix.
    explicit BooleanMat(std::vector<bool> const&);

    //! A constructor.
    //!
    //! Constructs a boolean matrix defined by matrix, which is copied into the
    //! constructed boolean matrix; see BooleanMat::BooleanMat for more
    //! details.
    explicit BooleanMat(std::vector<std::vector<bool>> const&);

    //! A constructor.
    //!
    //! Constructs a boolean matrix of the specified degree
    explicit BooleanMat(size_t);

    //! A copy constructor.
    BooleanMat(BooleanMat const&);

    //! Multiplies \p x and \p y and stores the result in \c this.
    //!
    //! This member function asserts that the dimensions of \p x, \p y, and \c
    //! this, are all equal, and that neither \p x nor \p y equals \c this.
    void redefine(Element const&, Element const&);
    using Element::redefine;

    // There is a specialization of std::hash for std::vector<bool> but for
    // some reason it causes a dramatic slow down in some of the benchmarks if
    // it is used by cache_hash_value below. So, we leave this commented out as
    // a reminder not to use it.

    // protected:
    // void cache_hash_value() const override {
    //  this->_hash_value = std::hash<std::vector<bool>>{}(this->_vector);
    // }

   private:
    // This constructor only exists to make the empty_key member function for
    // detail::ElementWithVectorData work, and because the compiler complains
    // about the Element::operator* without it.
    explicit BooleanMat(bool);

    // The next constructor only exists to allow the identity member function
    // for MatrixOverSemiringBase to work.
    friend class MatrixOverSemiringBase<bool, BooleanMat>;
    // TODO(later) pass by value, really?
    BooleanMat(std::vector<bool>, Semiring<bool> const*);

    static BooleanSemiring const* const _semiring;
  };

  //! Class for partitioned binary relations (PBR).
  //!
  //! Partitioned binary relations (PBRs) are a generalisation of bipartitions,
  //! which were introduced by
  //! [Martin and Mazorchuk](https://arxiv.org/abs/1102.0862).
  class PBR : public detail::ElementWithVectorData<std::vector<uint32_t>, PBR> {
   public:
    //! A constructor.
    //!
    //! Constructs a PBR defined by the vector pointed to by \p vector.
    //! The parameter \p vector should be a pointer to a vector of vectors of
    //! non-negative integer values of length \f$2n\f$ for some integer
    //! \f$n\f$, the vector in position \f$i\f$ is the list of points adjacent
    //! to \f$i\f$ in the PBR.
    using detail::ElementWithVectorData<std::vector<uint32_t>,
                                        PBR>::ElementWithVectorData;

    //! A constructor.
    //!
    //! Constructs a PBR defined by the initializer list \p vec. This list
    //! should be interpreted in the same way as \p vector in the vector
    //! constructor PBR::PBR.
    explicit PBR(std::initializer_list<std::vector<uint32_t>>);

    //! A constructor.
    //!
    //! Constructs an empty (no relation) PBR of the given degree.
    explicit PBR(size_t);

    //! Constructs a PBR from two vectors
    //!
    //! The parameters \p left and \p right should be vectors of
    //! $\f$n\f$ vectors of non-negative integer values, so that
    //! the vector in position \f$i\f$ of \p left is the list of points
    //! adjacent to \f$i\f$ in the PBR, and the vector in position \f$i\f$
    //! of \p right is the list of points adjacent to \f$n + i\f$ in the PBR.
    PBR(std::initializer_list<std::vector<int32_t>> const&,
        std::initializer_list<std::vector<int32_t>> const&);

    //! Validates the data defining \c this.
    //!
    //! This member function throws a libsemigroups::LibsemigroupsException if
    //! the data defining \c this is invalid, which could occur if:
    //!
    //! - \c this->_vector has odd length, or
    //!
    //! - \c this->_vector contains a vector containing a value which is larger
    //! than \c this->_vector.size() (i.e. twice the degree of \c this).
    void validate() const;

    //! Returns the approximate time complexity of multiplying PBRs.
    //!
    //! The approximate time complexity of multiplying PBRs is \f$2n ^ 3\f$
    //! where \f$n\f$ is the degree.
    size_t complexity() const override;

    //! Returns the degree of a PBR.
    //!
    //! The *degree* of a PBR is half the number of points in the PBR.
    size_t degree() const override;

    //! Returns the identity PBR with degree equal to that of \c this.
    //!
    //! This member function returns a new PBR with degree equal to the degree
    //! of \c this where every value is adjacent to its negative. Equivalently,
    //! \f$i\f$ is adjacent \f$i + n\f$ and vice versa for every \f$i\f$ less
    //! than the degree \f$n\f$.
    PBR identity() const override;

    //! Returns the identity PBR with degree equal to \p n.
    //!
    //! This function returns a new PBR with degree equal to \p n where every
    //! value is adjacent to its negative. Equivalently, \f$i\f$ is adjacent
    //! \f$i + n\f$ and vice versa for every \f$i\f$ less than the degree
    //! \f$n\f$.
    static PBR identity(size_t n);

    //! Multiply \p x and \p y and stores the result in \c this.
    //!
    //! This member function redefines \c this to be the product
    //! of the parameters  \p x and \p y. This member function asserts
    //! that the degrees of \p x, \p y, and \c this, are all equal, and that
    //! neither \p x nor  \p y equals \c this.
    //!
    //! The parameter \p thread_id is required since some temporary storage is
    //! required to find the product of \p x and \p y.  Note that if different
    //! threads call this member function with the same value of \p thread_id
    //! then bad things will happen.
    void redefine(Element const&, Element const&, size_t) override;

    //! Insertion operator
    //!
    //! This member function allows PBR objects to be inserted into an
    //! ostringstream
    friend std::ostringstream& operator<<(std::ostringstream&, PBR const&);

    //! Insertion operator
    //!
    //! This member function allows PBR objects to be inserted into an ostream.
    friend std::ostream& operator<<(std::ostream&, PBR const&);

   protected:
    void cache_hash_value() const override;

   private:
    static std::vector<std::vector<uint32_t>>
    process_left_right(std::vector<std::vector<int32_t>> const&,
                       std::vector<std::vector<int32_t>> const&);

    void unite_rows(detail::DynamicArray2<bool>&,
                    detail::DynamicArray2<bool>&,
                    size_t const&,
                    size_t const&);

    void x_dfs(std::vector<bool>&,
               std::vector<bool>&,
               detail::DynamicArray2<bool>&,
               uint32_t const&,
               uint32_t const&,
               PBR const* const,
               PBR const* const,
               size_t const&);

    void y_dfs(std::vector<bool>&,
               std::vector<bool>&,
               detail::DynamicArray2<bool>&,
               uint32_t const&,
               uint32_t const&,
               PBR const* const,
               PBR const* const,
               size_t const&);

    static std::vector<std::vector<bool>>           _x_seen;
    static std::vector<std::vector<bool>>           _y_seen;
    static std::vector<detail::DynamicArray2<bool>> _out;
    static std::vector<detail::DynamicArray2<bool>> _tmp;
  };

  // Specialization of templates from adapters.hpp for classes derived from
  // the class Element, such as Transformation<size_t> and so on . . .
  //! Specialization of the adapter Complexity for pointers to subclasses of
  //! Element.
  //!
  //! \sa Complexity.
  template <typename TSubclass>
  struct Complexity<TSubclass*,
                    typename std::enable_if<
                        std::is_base_of<Element, TSubclass>::value>::type> {
    //! Returns \p x->complexity().
    inline size_t operator()(TSubclass const* x) const {
      return x->complexity();
    }
  };

  //! Specialization of the adapter Complexity for subclasses of
  //! Element.
  //!
  //! \sa Complexity.
  template <typename TSubclass>
  struct Complexity<TSubclass,
                    typename std::enable_if<
                        std::is_base_of<Element, TSubclass>::value>::type> {
    //! Returns \p x.complexity().
    inline size_t operator()(TSubclass const& x) const {
      return x.complexity();
    }
  };

  //! Specialization of the adapter Degree for pointers to subclasses of
  //! Element.
  //!
  //! \sa Degree.
  template <typename TSubclass>
  struct Degree<TSubclass*,
                typename std::enable_if<
                    std::is_base_of<Element, TSubclass>::value>::type> {
    //! Returns \p x->degree().
    inline size_t operator()(TSubclass const* x) const {
      return x->degree();
    }
  };

  //! Specialization of the adapter Degree for subclasses of
  //! Element.
  //!
  //! \sa Degree.
  template <typename TSubclass>
  struct Degree<TSubclass,
                typename std::enable_if<
                    std::is_base_of<Element, TSubclass>::value>::type> {
    //! Returns \p x.degree().
    inline size_t operator()(TSubclass const& x) const {
      return x.degree();
    }
  };

  //! Specialization of the adapter IncreaseDegree for pointers to subclasses
  //! of Element.
  //!
  //! \sa IncreaseDegree.
  template <typename TSubclass>
  struct IncreaseDegree<TSubclass*,
                        typename std::enable_if<
                            std::is_base_of<Element, TSubclass>::value>::type> {
    //! Returns \p x->increase_degree_by(\p n).
    inline void operator()(TSubclass* x, size_t n) const {
      return x->increase_degree_by(n);
    }
  };

  // FIXME why is there no specialisation for non-pointers for IncreaseDegree?

  //! Specialization of the adapter Less for pointers to subclasses
  //! of Element.
  //!
  //! \sa Less.
  template <typename TSubclass>
  struct Less<TSubclass*,
              typename std::enable_if<
                  std::is_base_of<Element, TSubclass>::value>::type> {
    //! Returns \c true if the object pointed to by \p x is less than the
    //! object pointed to by \p y.
    inline bool operator()(TSubclass const* x, TSubclass const* y) const {
      return *x < *y;
    }
  };

  //! Specialization of the adapter One for pointers to subclasses
  //! of Element.
  //!
  //! \sa One.
  template <typename TSubclass>
  struct One<TSubclass*,
             typename std::enable_if<
                 std::is_base_of<Element, TSubclass>::value>::type> {
    //! Returns a pointer to a new instance of the one of \p x.
    TSubclass* operator()(Element const* x) const {
      // return new TSubclass(std::move(x->identity<TSubclass>()));
      return static_cast<TSubclass*>(x->heap_identity());
    }

    //! Returns a pointer to a new instance of the one of \p x.
    TSubclass* operator()(size_t n) {
      return new TSubclass(std::move(TSubclass::one(n)));
    }
  };

  //! Specialization of the adapter One for subclasses of Element.
  //!
  //! \sa One.
  template <typename TSubclass>
  struct One<TSubclass,
             typename std::enable_if<
                 std::is_base_of<Element, TSubclass>::value>::type> {
    //! Returns a new instance of the one of \p x.
    TSubclass operator()(TSubclass const& x) const {
      return static_cast<TSubclass>(x.identity());
    }

    //! Returns a new instance of the one of \p x.
    TSubclass operator()(size_t n) {
      return TSubclass(std::move(TSubclass::identity(n)));
    }
  };

  //! Specialization of the adapter Product for pointers to subclasses of
  //! Element.
  //!
  //! \sa Product.
  template <typename TSubclass>
  struct Product<TSubclass*,
                 typename std::enable_if<
                     std::is_base_of<Element, TSubclass>::value>::type> {
    //! Changes \p xy in-place to hold the product of \p x and \p y using
    //! Element::redefine.
    void operator()(TSubclass*       xy,
                    TSubclass const* x,
                    TSubclass const* y,
                    size_t           tid = 0) {
      xy->redefine(*x, *y, tid);
    }
  };

  //! Specialization of the adapter Product for subclasses of Element.
  //!
  //! \sa Product.
  template <typename TSubclass>
  struct Product<TSubclass,
                 typename std::enable_if<
                     std::is_base_of<Element, TSubclass>::value>::type> {
    //! Changes \p xy in-place to hold the product of \p x and \p y using
    //! Element::redefine.
    void operator()(TSubclass&       xy,
                    TSubclass const& x,
                    TSubclass const& y,
                    size_t           tid = 0) {
      xy.redefine(x, y, tid);
    }
  };

  //! Specialization of the adapter Swap for subclasses of Element.
  //!
  //! \sa Swap.
  template <typename TSubclass>
  struct Swap<TSubclass*,
              typename std::enable_if<
                  std::is_base_of<Element, TSubclass>::value>::type> {
    //! Swaps the object pointed to by \p x with the one pointed to by \p y.
    void operator()(TSubclass* x, TSubclass* y) const {
      x->swap(*y);
    }
  };

  //! Specialization of the adapter ImageRightAction for pointers to
  //! Permutation instances.
  //!
  //! \sa ImageRightAction.
  template <typename TValueType>
  struct ImageRightAction<
      Permutation<TValueType>*,
      TValueType,
      typename std::enable_if<std::is_integral<TValueType>::value>::type> {
    //! Returns the image of \p pt under \p x.
    TValueType operator()(Permutation<TValueType> const* x,
                          TValueType const               pt) {
      return (*x)[pt];
    }
  };

  //! Specialization of the adapter Inverse for pointers to
  //! Permutation instances.
  //!
  //! \sa Inverse.
  template <typename TValueType>
  struct Inverse<Permutation<TValueType>*> {
    //! Returns a pointer to a new instance of the inverse of \p x.
    Permutation<TValueType>* operator()(Permutation<TValueType>* x) {
      return new Permutation<TValueType>(std::move(x->inverse()));
    }
  };

  //! Specialization of the adapter Inverse for Permutation instances.
  //!
  //! \sa Inverse.
  template <typename TValueType>
  struct Inverse<Permutation<TValueType>> {
    //! Returns the inverse of \p x.
    Permutation<TValueType> operator()(Permutation<TValueType> const& x) {
      return x.inverse();
    }
  };

  //! Specialization of the adapter Hash for subclasses of Element.
  //!
  //! \sa Hash.
  template <typename TSubclass>
  struct Hash<TSubclass,
              typename std::enable_if<
                  std::is_base_of<Element, TSubclass>::value>::type> {
    //! Returns \p x.hash_value()
    size_t operator()(TSubclass const& x) const {
      return x.hash_value();
    }
  };

  //! Specialization of the adapter Hash for pointers to subclasses of Element.
  //!
  //! \sa Hash.
  template <typename TSubclass>
  struct Hash<TSubclass*,
              typename std::enable_if<
                  std::is_base_of<Element, TSubclass>::value>::type> {
    //! Returns \p x->hash_value()
    size_t operator()(TSubclass const* x) const {
      return x->hash_value();
    }
  };

  //! Specialization of the adapter ImageRightAction for instance of
  //! PartialPerm.
  //!
  //! \sa ImageRightAction.
  template <typename TIntType>
  struct ImageRightAction<PartialPerm<TIntType>, PartialPerm<TIntType>> {
    //! Stores the idempotent \f$(xy) ^ {-1}xy\f$ in \p res.
    void operator()(PartialPerm<TIntType>&       res,
                    PartialPerm<TIntType> const& pt,
                    PartialPerm<TIntType> const& x) const noexcept {
      res.redefine(pt, x);
      res.swap(res.right_one());
    }
  };

  //! Specialization of the adapter ImageLeftAction for instances of
  //! PartialPerm.
  //!
  //! \sa ImageLeftAction.
  template <typename TIntType>
  struct ImageLeftAction<PartialPerm<TIntType>, PartialPerm<TIntType>> {
    //! Stores the idempotent \f$xy(xy) ^ {-1}\f$ in \p res.
    void operator()(PartialPerm<TIntType>&       res,
                    PartialPerm<TIntType> const& pt,
                    PartialPerm<TIntType> const& x) const noexcept {
      res.redefine(x, pt);
      res.swap(res.left_one());
    }
  };

  //! Specialization of the adapter EqualTo for pointers to subclasses of
  //! Element.
  //!
  //! \sa EqualTo.
  template <typename TSubclass>
  struct EqualTo<
      TSubclass*,
      typename std::enable_if<
          std::is_base_of<libsemigroups::Element, TSubclass>::value>::type> {
    //! Tests equality of two const Element pointers via equality of the
    //! Elements they point to.
    bool operator()(TSubclass const* x, TSubclass const* y) const {
      return *x == *y;
    }
  };

  //! Specialization of the adapter ImageRightAction for instances of
  //! Permutation.
  //!
  //! \sa ImageRightAction.
  template <typename TIntType>
  struct ImageRightAction<
      Permutation<TIntType>,
      TIntType,
      typename std::enable_if<std::is_integral<TIntType>::value>::type> {
    //! Stores the image of \p pt under the action of \p p in \p res.
    void operator()(TIntType&                    res,
                    TIntType const&              pt,
                    Permutation<TIntType> const& p) const noexcept {
      LIBSEMIGROUPS_ASSERT(pt < p.degree());
      res = p[pt];
    }

    //! Returns the image of \p pt under the action of \p p.
    TIntType operator()(TIntType const pt, Permutation<TIntType> const& x) {
      return x[pt];
    }
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_ELEMENT_HPP_
