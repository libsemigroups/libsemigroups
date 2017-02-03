//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_ELEMENTS_H_
#define LIBSEMIGROUPS_ELEMENTS_H_

#include <assert.h>
#include <math.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "blocks.h"
#include "semiring.h"
#include "util/recvec.h"

#define PP_UNDEFINED PartialTransformation<T, PartialPerm<T>>::UNDEFINED

namespace libsemigroups {

  // Abstract
  //
  // Abstract base class for elements of a semigroup.
  class Element {
   public:
    // 0 or 1 parameter (hash value)
    // @hv the hash value (for caching) of the element being created (defaults
    // to <libsemigroups::Element::UNDEFINED>.
    explicit Element(size_t hv = Element::UNDEFINED) : _hash_value(hv) {}

    //
    // A default destructor.
    virtual ~Element() {}

    // const
    // @that Another element of the same type
    //
    // This method checks the mathematical equality of two objects in the same
    // subclass of <Element>.
    //
    // @return **true** if **this** equals <that>.
    virtual bool operator==(const Element& that) const = 0;

    // const
    // @that Another element of the same type
    //
    // This method defines a total order on the set of objects in the same
    // subclass of <Element> with a given <degree>. The definition of this total
    // order depends on the method for the operator < in the subclass.
    //
    // @return **true** if **this** is less than <that>.
    virtual bool operator<(const Element& that) const = 0;

    // const
    //
    // This method returns an integer which represents the complexity of
    // multiplying two objects in the same subclass of <Element> which have the
    // same <degree>. For example, the complexity of multiplying two *3* by *3*
    // matrices over a common semiring is *O(3 ^ 3)*, and 27 is returned by
    // <MatrixOverSemiring::complexity>.
    //
    // The returned value is used in, for example, <Semigroup::fast_product> and
    // <Semigroup::nr_idempotents> to decide if it is better to multiply
    // elements or follow a path in the Cayley graph.
    //
    // @return the complexity of multiplying two elements.
    virtual size_t complexity() const = 0;

    // const
    //
    // This method returns an integer which represents the size of the element,
    // and is used to determine whether or not two elements are compatible for
    // multiplication. For example, two <Transformation>s of different degrees
    // cannot be multiplied, or a <Bipartition> of degree 10 cannot be an
    // element of a monoid of bipartitions of degree 3.
    //
    // See the relevant subclass for the particular meaning of the return value
    // of this method for each subclass.
    //
    // @return the degree of an element.
    virtual size_t degree() const = 0;

    // const
    //
    // This method returns a hash value for an object in a subclass of
    // <Element>.
    //
    // @return the hash value of an element.
    inline size_t hash_value() const {
      if (_hash_value == UNDEFINED) {
        this->cache_hash_value();
      }
      return this->_hash_value;
    }

    // const
    //
    // This method returns the multiplicative identity element for an object in
    // a
    // subclass of <Element>. The returned identity belongs to the same subclass
    // and has the same <degree> as **this**.
    //
    // @return the identity element.
    virtual Element* identity() const = 0;

    // const
    // @increase_deg_by increase the size of the container of the defining data
    // of **this** by this amount.
    //
    // This method really copies an <Element>. To minimise the amount of copying
    // when <Element>s are inserted in unordered_maps and other containers,
    // <Element>s behave somewhat like pointers, in that the actual data in an
    // <Element> is only copied when this method is called. Otherwise, if an
    // <Element> is copied, then its defining data is only stored once.
    //
    // See also <really_delete>.
    //
    // @return a new element completely independent of **this**.
    virtual Element* really_copy(size_t increase_deg_by = 0) const = 0;

    // non-const
    // @x an element.
    //
    // This method copies <x> into **this** by changing **this** in-place.
    virtual void copy(Element const* x) = 0;

    // non-const
    //
    // This method really deletes an <Element>. To minimise the amount of
    // copying when <Element>s are inserted in unordered_maps and other
    // containers, <Element>s behave somewhat like pointers, in that the actual
    // data in an <Element> is only deleted when this method is called.
    // Otherwise, if an <Element> is deleted, then its defining data is not
    // deleted, since it may be contained in multiple copies of the element.
    //
    // See also <really_copy>.
    //
    // @return deletes the data defining an element.
    virtual void really_delete() = 0;

    // non-const
    // @x an element
    // @y an element
    //
    // See <libsemigroups::Element::redefine>.
    //
    // Redefine **this** to be the product of <x> and <y>.
    virtual void
    redefine(Element const* x, Element const* y, size_t const& thread_id) {
      (void) thread_id;
      redefine(x, y);
    }

    virtual void redefine(Element const* x, Element const* y) {
      redefine(x, y, 0);
    }

    struct Equal {
      // To keep cldoc happy
      // @x a pointer to a const Element
      // @y a pointer to a const Element
      //
      // @return **true** or **false**
      size_t operator()(const Element* x, const Element* y) const {
        return *x == *y;
      }
    };

    struct Hash {
      // To keep cldoc happy
      // @x a pointer to a const Element
      //
      // @return **true** or **false**
      size_t operator()(const Element* x) const {
        return x->hash_value();
      }
    };

   protected:
    // const
    //
    // This method is used to compute and cache the hash value of an element.
    virtual void cache_hash_value() const = 0;

    // const
    //
    // This method is used to reset the cached hash value to
    // <libsemigroups::Element::UNDEFINED>.
    void reset_hash_value() const {
      _hash_value = UNDEFINED;
    }

    // static
    //
    // This variable is used to indicate that a value is undefined, such as,
    // the cached hash value.
    static size_t const UNDEFINED;

    //
    // This data member holds a cached version of the hash value of an Element.
    // It is stored here if it is ever computed. It is invalidated by
    // <libsemigroups::Element::redefine> and sometimes by
    // <libsemigroups::Element::really_copy>.
    mutable size_t _hash_value;
  };

  // Abstract
  // @S Type of the entries in <_vector>
  // @T Subclass of <ElementWithVectorData>.
  //
  // Abstract base class for elements using a vector to store their defining
  // data. For example, <Transformation>&lt;u_int128_t&gt; is a subclass of
  // <ElementWithVectorData>&lt;u_int128_t,
  // Transformation&lt;u_int128_t&gt;&gt;.

  template <typename S, class T> class ElementWithVectorData : public Element {
   public:
    // 1 parameter (integer)
    // @size  Reserve <size> for <_vector>.
    //
    // Constructs an object with an uninitialised <_vector> which has size
    // <size> (defaults to 0).
    explicit ElementWithVectorData(size_t size = 0)
        : Element(), _vector(new std::vector<S>()) {
      _vector->resize(size);
    }

    // 1 or 2 parameter (vector pointer)
    // @vector Pointer to defining data of the element.
    // @hv     The hash value of the element being created.
    //
    // Constructs an object with <_vector> equal to <vector>, <vector> is not
    // copied, and should be deleted using <really_delete>.
    explicit ElementWithVectorData(std::vector<S>* vector,
                                   size_t          hv = Element::UNDEFINED)
        : Element(hv), _vector(vector) {}

    // 1 parameter (vector const reference)
    // @vector Defining data of the element.
    //
    // Constructs an object with <_vector> equal to a new copy of <vector>.
    explicit ElementWithVectorData(std::vector<S> const& vector)
        : ElementWithVectorData(new std::vector<S>(vector)) {}

    // const
    // @pos An integer.
    //
    // @return The <pos> position of <_vector>.
    inline S operator[](size_t pos) const {
      return (*_vector)[pos];
    }

    // const
    // @pos An integer.
    //
    // @return The <pos> position of <_vector>.
    inline S at(size_t pos) const {
      return _vector->at(pos);
    }

    // const
    // @that Compare **this** and <that>.
    //
    // @return **true** if the underlying <_vector>s are equal and **false**
    // otherwise.
    bool operator==(Element const& that) const override {
      return *(static_cast<T const&>(that)._vector) == *(this->_vector);
    }

    // const
    // @that Compare **this** and <that>.
    //
    // @return **true** if the <_vector> of **this** is short-lex less than the
    // <_vector> of <that>.
    bool operator<(const Element& that) const override {
      T const& ewvd = static_cast<T const&>(that);
      if (this->_vector->size() != ewvd._vector->size()) {
        return this->_vector->size() < ewvd._vector->size();
      }
      for (size_t i = 0; i < this->_vector->size(); i++) {
        if ((*this)[i] != ewvd[i]) {
          return (*this)[i] < ewvd[i];
        }
      }
      return false;
    }

    // const
    // @increase_deg_by increase the size of the <_vector> by this amount.
    //
    // See <Element::really_copy>.
    //
    // If <increase_deg_by> is not 0, then this method must be overridden since
    // there is no way of knowing how a subclass is defined by the data in
    // <_vector>.
    //
    // @return A pointer to a copy of **this**.
    Element* really_copy(size_t increase_deg_by) const override {
      assert(increase_deg_by == 0);
      (void) increase_deg_by;
      std::vector<S>* vector(new std::vector<S>(*_vector));
      return new T(vector, this->_hash_value);
    }

    void copy(Element const* x) override {
      assert(x->degree() == this->degree());
      auto   xx  = static_cast<ElementWithVectorData const*>(x);
      size_t deg = _vector->size();
      for (size_t i = 0; i < deg; i++) {
        (*_vector)[i] = (*xx)[i];
      }
      this->reset_hash_value();
    }

    // non-const
    //
    // See <Element::really_delete>.
    //
    // Deletes <_vector>.
    void really_delete() override {
      delete _vector;
    }

    // const
    //
    // @return An iterator pointing to the first value in <_vector>.
    inline typename std::vector<S>::iterator begin() const {
      return _vector->begin();
    }

    // const
    //
    // @return An iterator referring to the past-the-end element in <_vector>.
    inline typename std::vector<S>::iterator end() const {
      return _vector->end();
    }

    // const
    //
    // @return A const_iterator pointing to the first value in <_vector>.
    inline typename std::vector<S>::iterator cbegin() const {
      return _vector->cbegin();
    }

    // const
    //
    // @return A const_iterator referring to the past-the-end element in
    // <_vector>.
    inline typename std::vector<S>::iterator cend() const {
      return _vector->cend();
    }

   protected:
    //
    // The actual data defining of **this** is stored in this <_vector>.
    std::vector<S>* _vector;
  };

  // Abstract, template
  // @S Type of image values
  // @T Subclass of <PartialTransformation>.
  //
  // Template class for partial transformations, which is a subclass of
  // <ElementWithVectorData>. For example, <Transformation>&lt;u_int128_t&gt; is
  // a
  // subclass of <PartialTransformation>&lt;u_int128_t,
  // Transformation&lt;u_int128_t&gt;&gt;.
  //
  // The value of the template parameter <S> can be used to reduce the amount of
  // memory required by instances of this class.
  //
  // This class is abstract since it does not implement all methods required by
  // the <Element> class, it exists to provide common methods for its
  // subclasses.
  //
  // A *partial transformation* *f* is just a function defined on a subset of
  // *{0, 1, ..., n - 1}* for some integer *n* called the *degree* of *f*.
  // A partial transformation is stored as a vector of the images of *{0, 1,
  // ...,
  // n
  // -1}*, i.e. *[(0)f, (1)f, ..., (n - 1)f]* where the value <UNDEFINED> is
  // used
  // to indicate that *(i)f* is, you guessed it, undefined (i.e. not among the
  // points where *f* is defined).

  template <typename S, typename T>
  class PartialTransformation : public ElementWithVectorData<S, T> {
   public:
    // Uninitialised
    //
    // Constructs a partial transformation with no data at all.
    PartialTransformation() : ElementWithVectorData<S, T>() {}

    // 1 or 2 parameters (vector pointer, hash value)
    // @vector Pointer to defining data of the element.
    // @hv     The hash value of the element being created.
    //
    // Constructs a partial transformation with list of images equal to
    // <vector>, <vector> is not copied, and should be deleted using
    // <ElementWithVectorData::really_delete>.
    explicit PartialTransformation(std::vector<S>* vector,
                                   size_t          hv = Element::UNDEFINED)
        : ElementWithVectorData<S, T>(vector, hv) {}

    // 1 parameter (vector const ref)
    // @vector Defining data of the element.
    //
    // Constructs a partial transformation with list of images equal to
    // <vector>,
    // which is copied into the constructed object.
    explicit PartialTransformation(std::vector<S> const& vector)
        : ElementWithVectorData<S, T>(vector) {}

    // const
    //
    // See <Element::complexity> for more details.
    //
    // @return the degree of a partial transformation.
    size_t complexity() const override {
      return this->_vector->size();
    }

    // const
    //
    // @return the degree of a partial transformation.
    size_t degree() const override {
      return this->_vector->size();
    }

    // const
    //
    // The *rank* of a partial transformation is the number of its distinct
    // image
    // values, not including <UNDEFINED>.
    //
    // @return the rank of a partial transformation.
    size_t crank() const {
      _lookup.clear();
      _lookup.resize(degree(), false);
      size_t r = 0;
      for (auto const& x : *(this->_vector)) {
        if (x != UNDEFINED && !_lookup[x]) {
          _lookup[x] = true;
          r++;
        }
      }
      return r;
    }

    // const
    //
    // See <Element::hash_value>.
    //
    // @return a hash value for a partial transformation.
    void cache_hash_value() const override {
      size_t seed = 0;
      size_t deg  = this->degree();
      for (auto const& val : *(this->_vector)) {
        seed *= deg;
        seed += val;
      }
      this->_hash_value = seed;
    }

    // const
    //
    // See <Element::identity>.
    //
    // @return a new partial transformation with degree equal to <degree> that
    // fixes every value from *1* to <degree>.
    Element* identity() const override {
      std::vector<S>* vector(new std::vector<S>());
      vector->reserve(this->degree());
      for (size_t i = 0; i < this->degree(); i++) {
        vector->push_back(i);
      }
      return new T(vector);
    }

    // static
    //
    // This value is used to indicate that a partial transformation is not
    // defined on a value. The value of this is a cast to <S> of -1.
    static S UNDEFINED;

   private:
    static std::vector<bool> _lookup;
  };

  template <typename S, typename T>
  std::vector<bool> PartialTransformation<S, T>::_lookup = std::vector<bool>();

  template <typename S, typename T>
  S PartialTransformation<S, T>::UNDEFINED = (S) -1;

  // Template, non-abstract
  // @T Integer type
  //
  // Template class for transformations, which is a subclass of
  // <PartialTransformation>.
  //
  // The value of the template parameter <T> can be used to reduce the amount of
  // memory required by instances of this class.
  //
  // A *transformation* *f* is just a function defined on the whole of
  // *{0, 1, ..., n - 1}* for some integer *n* called the *degree* of *f*.
  // A transformation is stored as a vector of the images of *{0, 1, ..., n
  // -1}*, i.e. *[(0)f, (1)f, ..., (n - 1)f]*.

  template <typename T>
  class Transformation : public PartialTransformation<T, Transformation<T>> {
   public:
    // 1 or 2 parameters (vector pointer, hash value)
    // @vector Pointer to image of the transformation.
    // @hv     The hash value of the element being created.
    //
    // Constructs a transformation with list of images equal to <vector>,
    // <vector> is not copied, and should be deleted using
    // <ElementWithVectorData::really_delete>.
    explicit Transformation(std::vector<T>* vector,
                            size_t          hv = Element::UNDEFINED)
        : PartialTransformation<T, Transformation<T>>(vector, hv) {}

    // 1 parameters (vector const ref)
    // @vector The image of the transformation.
    //
    // Constructs a partial transformation with list of images equal to
    // <vector>,
    // which is copied into the constructed object.
    explicit Transformation(std::vector<T> const& vector)
        : PartialTransformation<T, Transformation<T>>(vector) {}

    // const
    // @increase_deg_by increase the degree by this amount (defaults to 0).
    //
    // See <Element::really_copy>.
    //
    // The copy returned by this method fixes all the values between the
    // <PartialTransformation::degree> of **this** and <increase_deg_by>.
    //
    // @return A pointer to a copy of **this**.
    Element* really_copy(size_t increase_deg_by = 0) const override {
      std::vector<T>* vector_copy = new std::vector<T>(*this->_vector);
      if (increase_deg_by == 0) {
        return new Transformation<T>(vector_copy, this->_hash_value);
      } else {
        size_t n = vector_copy->size();
        vector_copy->reserve(n + increase_deg_by);
        for (size_t i = n; i < n + increase_deg_by; i++) {
          vector_copy->push_back(i);
        }
        return new Transformation<T>(vector_copy);
      }
    }

    // non-const
    // @x transformation
    // @y transformation
    //
    // See <Element::redefine>.
    //
    // Redefine **this** to be the composition of <x> and <y>. This method
    void redefine(Element const* x, Element const* y) override {
      assert(x->degree() == y->degree());
      assert(x->degree() == this->degree());
      assert(x != this && y != this);
      Transformation<T> const* xx(static_cast<Transformation<T> const*>(x));
      Transformation<T> const* yy(static_cast<Transformation<T> const*>(y));
      size_t const             n = this->_vector->size();
      for (T i = 0; i < n; i++) {
        (*this->_vector)[i] = (*yy)[(*xx)[i]];
      }
      this->reset_hash_value();
    }
  };

  // Template, non-abstract
  // @T Integer type
  //
  // Template class for partial permutations, which is a subclass of
  // <PartialTransformation>.
  //
  // The value of the template parameter <T> can be used to reduce the amount of
  // memory required by instances of this class.
  //
  // A *partial permutation* *f* is just an injective partial transformation,
  // which is stored as a vector of the images of *{0, 1, ..., n -1}*, i.e.
  // *[(0)f, (1)f, ..., (n - 1)f]* where the value UNDEFINED (a cast to type
  // <T> of -1) is used to indicate that *(i)f* is, you guessed it, undefined
  // (i.e. not among the points where *f* is defined).

  template <typename T>
  class PartialPerm : public PartialTransformation<T, PartialPerm<T>> {
   public:
    // 1 or 2 parameters (vector pointer, hash value)
    // @vector Pointer to image of the partial perm.
    // @hv     The hash value of the element being created.
    //
    // Constructs a partial perm with list of images equal to <vector>,
    // <vector> is not copied, and should be deleted using
    // <ElementWithVectorData::really_delete>.
    explicit PartialPerm(std::vector<T>* vector, size_t hv = Element::UNDEFINED)
        : PartialTransformation<T, PartialPerm<T>>(vector, hv) {}

    // 1 parameter (vector const ref)
    // @vector Image of the partial perm.
    //
    // Constructs a partial perm with list of images equal to <vector>,
    // which is copied into the constructed object.
    explicit PartialPerm(std::vector<T> const& vector)
        : PartialTransformation<T, PartialPerm<T>>(vector) {}

    // 3 parameters
    // @dom The domain
    // @ran The range
    // @deg The intended degree of the partial perm. This must be at least the
    // maximum element occurring in <dom> or <ran>.
    //
    // Constructs a partial perm such that (<dom>[i])f = <ran>[i] and which is
    // undefined on every other value.
    explicit PartialPerm(std::vector<T> const& dom,
                         std::vector<T> const& ran,
                         size_t                deg)
        : PartialTransformation<T, PartialPerm<T>>() {
      assert(dom.size() == ran.size());
      assert(dom.empty() || deg >= *std::max_element(dom.begin(), dom.end()));

      this->_vector->resize(deg + 1, PP_UNDEFINED);
      for (size_t i = 0; i < dom.size(); i++) {
        (*this->_vector)[dom[i]] = ran[i];
      }
    }

    // const
    // @that Compare **this** and <that>.
    //
    // This defines a total order on partial permutations that is equivalent to
    // that used by GAP. It is not short-lex on the list of images.
    //
    // @return **true** if something complicated is **true** and **false** if
    // it is not.
    bool operator<(const Element& that) const override {
      auto pp_this = static_cast<const PartialPerm<T>*>(this);
      auto pp_that = static_cast<const PartialPerm<T>&>(that);

      size_t deg_this = pp_this->degree();
      for (auto it = pp_this->_vector->end() - 1;
           it >= pp_this->_vector->begin();
           it--) {
        if (*it == PP_UNDEFINED) {
          deg_this--;
        } else {
          break;
        }
      }
      size_t deg_that = pp_that.degree();
      for (auto it = pp_that._vector->end() - 1;
           it >= pp_that._vector->begin() && deg_that >= deg_this;
           it--) {
        if (*it == PP_UNDEFINED) {
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
          return (*pp_this)[i] == PP_UNDEFINED
                 || (pp_that[i] != PP_UNDEFINED && (*pp_this)[i] < pp_that[i]);
        }
      }
      return false;
    }

    // const
    // @increase_deg_by increase the degree by this amount (defaults to 0).
    //
    // See <Element::really_copy>.
    //
    // The copy returned by this method is undefined on all the values between
    // the <PartialTransformation::degree> of **this** and <increase_deg_by>.
    //
    // @return A pointer to a copy of **this**.
    Element* really_copy(size_t increase_deg_by = 0) const override {
      std::vector<T>* vector_copy = new std::vector<T>(*this->_vector);
      if (increase_deg_by == 0) {
        return new PartialPerm<T>(vector_copy, this->_hash_value);
      } else {
        size_t n = vector_copy->size();
        vector_copy->reserve(n + increase_deg_by);
        for (size_t i = n; i < n + increase_deg_by; i++) {
          vector_copy->push_back(PP_UNDEFINED);
        }
        return new PartialPerm<T>(vector_copy);
      }
    }

    // non-const
    // @x partial perm
    // @y partial perm
    //
    // See <Element::redefine>.
    //
    // Redefine **this** to be the composition of <x> and <y>. This method
    // asserts that the degrees of <x>, <y>, and **this**, are all equal, and
    // that neither <x> nor <y> equals **this**.
    void redefine(Element const* x, Element const* y) override {
      assert(x->degree() == y->degree());
      assert(x->degree() == this->degree());
      assert(x != this && y != this);
      PartialPerm<T> const* xx(static_cast<PartialPerm<T> const*>(x));
      PartialPerm<T> const* yy(static_cast<PartialPerm<T> const*>(y));
      size_t const          n = this->degree();
      for (T i = 0; i < n; i++) {
        (*this->_vector)[i] =
            ((*xx)[i] == PP_UNDEFINED ? PP_UNDEFINED : (*yy)[(*xx)[i]]);
      }
      this->reset_hash_value();
    }

    // const
    //
    // The *rank* of a partial permutation is the number of its distinct image
    // values, not including UNDEFINED. This method involves slightly less work
    // than <PartialTransformation::crank> since a partial permutation is
    // injective, and so every image value occurs precisely once.
    //
    // @return the rank of a partial permutation.
    size_t crank() const {
      size_t nr_defined = 0;
      for (auto const& x : *this->_vector) {
        if (x != PP_UNDEFINED) {
          nr_defined++;
        }
      }
      return nr_defined;
    }
  };

  // Non-abstract
  // Class for square matrices over the Boolean semiring, which is a subclass of
  // <ElementWithVectorData>.

  class BooleanMat : public ElementWithVectorData<bool, BooleanMat> {
   public:
    // 1 parameter (vector pointer)
    // @matrix Pointer to defining data of the matrix.
    // @hv     The hash value of the element being created.
    //
    // Constructs a Boolean matrix defined by <matrix>, <matrix> is not copied,
    // and should be deleted using <ElementWithVectorData::really_delete>.
    //
    // The argument <matrix> should be a vector of boolean values of length *n ^
    // 2* for some integer *n*, so that the value in position *in + j* is the
    // entry in the *i*th row and *j*th column of the constructed matrix.
    explicit BooleanMat(std::vector<bool>* matrix,
                        size_t             hv = Element::UNDEFINED)
        : ElementWithVectorData<bool, BooleanMat>(matrix, hv) {}

    // 1 parameter (vector of vectors ref)
    // @matrix The matrix.
    //
    // Constructs a boolean matrix defined by <matrix>, which is copied into the
    // constructed boolean matrix.
    explicit BooleanMat(std::vector<std::vector<bool>> const& matrix);

    // const
    //
    // See <Element::complexity> for more details.
    //
    // @return *n ^ 3* where *n* is the dimension of the matrix.
    size_t complexity() const override;

    // const
    //
    // See <Element::degree> for more details.
    //
    // @return the dimension of the matrix.
    size_t degree() const override;

    // const
    //
    // See <Element::hash_value>.
    //
    // @return a hash value for a boolean matrix.
    void cache_hash_value() const override;

    // const
    //
    // See <Element::identity>.
    //
    // @return a new boolean matrix with dimension equal to that of **this**,
    // where the main diagonal consists of the value **true** and every other
    // entry
    // is **false**.
    Element* identity() const override;

    // non-const
    // @x a Boolean matrix
    // @y a Boolean matrix
    //
    // See <Element::redefine>.
    //
    // Redefine **this** to be the product of <x> and <y>. This method asserts
    void redefine(Element const* x, Element const* y) override;
  };

  // Non-abstract
  // Class for bipartitions, which are partitions of the set *{0, ..., 2n -1}*
  // for some integer *n*, which is a subclass of <ElementWithVectorData>.
  //
  // The <Bipartition> class is more complex (i.e. has more methods) than
  // strictly required by the algorithms for <Semigroup>s because the extra
  // methods are used in the GAP package [Semigroups package for
  // GAP](https://gap-packages.github.io/Semigroups/).

  class Bipartition : public ElementWithVectorData<u_int32_t, Bipartition> {
    // TODO(JDM) add more explanation to the doc here
   public:
    // 1 parameters (integer)
    // @degree Degree of bipartition being constructed.
    //
    // Constructs a uninitialised bipartition of degree <degree>.
    explicit Bipartition(size_t degree)
        : ElementWithVectorData<u_int32_t, Bipartition>(2 * degree),
          _nr_blocks(Bipartition::UNDEFINED),
          _nr_left_blocks(Bipartition::UNDEFINED),
          _trans_blocks_lookup(),
          _rank(Bipartition::UNDEFINED) {}

    // 1 parameter (vector pointer)
    // @blocks lookup table for the bipartition being defined.
    // @hv     The hash value of the element being created (defaults to
    // Element::UNDEFINED).
    //
    // The argument <blocks> must have length *2n* for some integer *n*,
    // consist of non-negative integers, and have the property that if *i*, *i
    // > 0*, occurs in <blocks>, then *i - 1* occurs earlier in <blocks>.  None
    // of this is checked.  The argument <blocks> is not copied, and should be
    // deleted using <ElementWithVectorData::really_delete>
    explicit Bipartition(std::vector<u_int32_t>* blocks,
                         size_t                  hv = Element::UNDEFINED)
        : ElementWithVectorData<u_int32_t, Bipartition>(blocks, hv),
          _nr_blocks(Bipartition::UNDEFINED),
          _nr_left_blocks(Bipartition::UNDEFINED),
          _trans_blocks_lookup(),
          _rank(Bipartition::UNDEFINED) {}

    // 1 parameter (vector const reference)
    // @blocks lookup table for the bipartition being defined.
    //
    // The argument <blocks> must have length *2n* for some integer *n*,
    // consist of non-negative integers, and have the property that *i*, *i >
    // 0*,  occurs in <blocks>, then *i - 1* occurs earlier in <blocks>. None
    // of this is checked. The argument <blocks> is copied into the newly
    // constructed bipartition.
    explicit Bipartition(std::vector<u_int32_t> const& blocks)
        : Bipartition(new std::vector<u_int32_t>(blocks)) {}

    // TODO(JDM) another constructor that accepts an actual partition

    // const
    //
    // See <Element::complexity> for more details.
    //
    // @return *2n ^ 2* where *n* is the degree of the bipartition.
    size_t complexity() const override;

    // const
    //
    // A bipartition is of degree *n* if it is a partition of *{0, ..., 2n -
    // 1}*.  See <Element::degree> for more details.
    //
    // @return the degree of the bipartition.
    size_t degree() const override;

    // const
    //
    // See <Element::hash_value>.
    //
    // @return a hash value for a bipartition.
    void cache_hash_value() const override;

    // const
    //
    // See <Element::identity>.
    //
    // @return a new bipartition with degree equal to that of **this**,
    // whose blocks are *{i, -i}* for all *i* in *{0, ..., n - 1}*.
    Element* identity() const override;

    // non-const
    // @x a bipartition
    // @y a bipartition
    //
    // See <Element::redefine>.
    //
    // Redefine this to be the product (as defined at the top of this page) of
    // <x> and <y>. This method asserts that the dimensions of <x>, <y>, and
    // this, are all equal, and that neither <x> nor <y> equals **this**.
    void redefine(Element const* x,
                  Element const* y,
                  size_t const&  thread_id) override;

    // non-const
    //
    // The *rank* of a bipartition is the number of blocks containing both
    // positive and negative values.
    //
    // This value is cached after it is first computed.
    // @return the rank of a bipartition.
    size_t rank();

    // const
    // @pos a value between *0* and *2n - 1* where *n* is the degree
    //
    // This method asserts that <pos> is in the correct range of values.
    //
    // @return the index of the block containing <pos>.
    u_int32_t block(size_t pos) const;

    // const
    //
    // @return the number of blocks in a bipartition.
    u_int32_t const_nr_blocks() const;

    // non-const
    //
    // @return the number of blocks in a bipartition.
    u_int32_t nr_blocks();

    // non-const
    //
    // @return the number of blocks in a bipartition containing positive
    // integers.
    u_int32_t nr_left_blocks();

    // non-const
    //
    // @return the number of blocks in a bipartition containing negative
    // integers.
    u_int32_t nr_right_blocks();

    // non-const
    // @index index of a block in **this**
    //
    // @return **true** if the <index>th block of **this** contains both
    // positive and
    // negative values, and **false** otherwise.
    bool is_transverse_block(size_t index);

    // non-const
    //
    // @return the left blocks of a bipartition.
    Blocks* left_blocks();

    // non-const
    //
    // @return the right blocks of a bipartition.
    Blocks* right_blocks();

    // non-const
    // @nr_blocks an integer
    //
    // This method sets the cached value of the number of blocks of **this** to
    // <nr_blocks>. It asserts that either there is no existing cached value or
    // <nr_blocks> equals the existing cached value.
    inline void set_nr_blocks(size_t nr_blocks) {
      assert(_nr_blocks == Bipartition::UNDEFINED || _nr_blocks == nr_blocks);
      _nr_blocks = nr_blocks;
    }

    // non-const
    // @nr_left_blocks an integer
    //
    // This method sets the cached value of the number of left blocks of
    // **this** to <nr_left_blocks>. It asserts that either there is no
    // existing cached value or <nr_left_blocks> equals the existing cached
    // value.
    inline void set_nr_left_blocks(size_t nr_left_blocks) {
      assert(_nr_left_blocks == Bipartition::UNDEFINED
             || _nr_left_blocks == nr_left_blocks);
      _nr_left_blocks = nr_left_blocks;
    }

    // non-const
    // @rank an integer
    //
    // This method sets the cached value of the rank of **this** to <rank>. It
    // asserts that either there is no existing cached value or
    // <rank> equals the existing cached value.
    inline void set_rank(size_t rank) {
      assert(_rank == Bipartition::UNDEFINED || _rank == rank);
      _rank = rank;
    }

   private:
    u_int32_t fuseit(std::vector<u_int32_t>& fuse, u_int32_t pos);
    void init_trans_blocks_lookup();

    static std::vector<std::vector<u_int32_t>> _fuse;
    static std::vector<std::vector<u_int32_t>> _lookup;

    size_t            _nr_blocks;
    size_t            _nr_left_blocks;
    std::vector<bool> _trans_blocks_lookup;
    size_t            _rank;

    static u_int32_t const UNDEFINED;
  };

  // Non-abstract
  // Class for square matrices over semirings, which is a subclass of
  // <ElementWithVectorData>. See <Semiring>.
  //
  // This class is abstract since it does not implement all methods required by
  // the <Element> class, it exists to provide common methods for its
  // subclasses.

  class MatrixOverSemiring
      : public ElementWithVectorData<int64_t, MatrixOverSemiring> {
   public:
    // 2 or 3 parameters
    // @matrix    Pointer to defining data of the matrix.
    // @hv        The hash value of the element being created
    // @semiring  The semiring over which the matrix is defined.
    //
    // Constructs a matrix defined by <matrix>, <matrix> is not copied,
    // and should be deleted using <ElementWithVectorData::really_delete>.
    //
    // The argument <matrix> should be a vector of integer values of length *n ^
    // 2* for some integer *n*, so that the value in position *i * n + j* is the
    // entry in the *i*th row and *j*th column of the constructed matrix.

    explicit MatrixOverSemiring(std::vector<int64_t>* matrix,
                                size_t                hv,
                                Semiring*             semiring = nullptr)
        : ElementWithVectorData<int64_t, MatrixOverSemiring>(matrix, hv),
          _semiring(semiring) {}

    // 2 parameters (vector pointer, semiring pointer)
    // @matrix    Pointer to defining data of the matrix.
    // @semiring  The semiring over which the matrix is defined.
    //
    // Constructs a matrix defined by <matrix>, <matrix> is not copied,
    // and should be deleted using <ElementWithVectorData::really_delete>.
    //
    // The argument <matrix> should be a vector of integer values of length *n ^
    // 2* for some integer *n*, so that the value in position *i * n + j* is the
    // entry in the *i*th row and *j*th column of the constructed matrix.

    explicit MatrixOverSemiring(std::vector<int64_t>* matrix,
                                Semiring*             semiring = nullptr)
        : ElementWithVectorData<int64_t, MatrixOverSemiring>(matrix),
          _semiring(semiring) {}

    // 2 parameters (matrix const ref, semiring pointer)
    // @matrix    The matrix.
    // @semiring  A pointer to the semiring over which the matrix is defined.
    //
    // Constructs a matrix defined by <matrix>, which is copied into the
    // constructed matrix.
    explicit MatrixOverSemiring(std::vector<std::vector<int64_t>> const& matrix,
                                Semiring* semiring);

    // const
    //
    // @return the <Semiring> over which the matrix is defined.
    Semiring* semiring() const;

    // const
    //
    // See <Element::complexity> for more details.
    //
    // @return *n ^ 3* where *n* is the dimension of the matrix.
    size_t complexity() const override;

    // const
    //
    // See <Element::degree> for more details.
    //
    // @return the dimension of the matrix.
    size_t degree() const override;

    // const
    //
    // See <Element::hash_value>.
    //
    // @return a hash value for a matrix.
    void cache_hash_value() const override;

    // const
    //
    // See <Element::identity>.
    //
    // @return a new matrix with dimension equal to that of **this**, where the
    // main diagonal consists of the value <Semiring::one> and every other entry
    // <Semiring::zero>.
    Element* identity() const override;

    // const
    // @increase_deg_by this must be 0, since it does not make sense to increase
    // the degree of a matrix.
    //
    // See <Element::really_copy>.
    //
    // @return A pointer to a copy of **this**.
    Element* really_copy(size_t increase_deg_by = 0) const override;

    // non-const
    // @x a matrix
    // @y a matrix
    //
    // See <Element::redefine>.
    //
    // Redefine **this** to be the product of <x> and <y>. This method asserts
    // that the dimensions of <x>, <y>, and **this**, are all equal, and that
    // neither <x> nor <y> equals **this**.
    void redefine(Element const* x, Element const* y) override;

   private:
    // a function applied after redefinition
    virtual void after() {}

    Semiring* _semiring;
  };

  // Non-abstract
  // Class for projective square matrices  <MaxPlusSemiring> semiring, which is
  // a subclass of <ElementWithVectorData>. See <Semiring>.
  // Two projective matrices are equal if they differ by a scalar multiple.

  class ProjectiveMaxPlusMatrix : public MatrixOverSemiring {
   public:
    // 2 or 3 parameters
    // @matrix    Pointer to defining data of the matrix.
    // @hv        Hash value of the element being created (defaults to
    //            Element::UNDEFINED)
    // @semiring  Pointer to the semiring over which the matrix is defined.
    //
    // Constructs a matrix defined by <matrix>, <matrix> is not copied,
    // and should be deleted using <ElementWithVectorData::really_delete>.
    //
    // The argument <matrix> should be a vector of integer values of length *n ^
    // 2* for some integer *n*, so that the value in position *i * n + j* is the
    // entry in the *i*th row and *j*th column of the constructed matrix.
    explicit ProjectiveMaxPlusMatrix(std::vector<int64_t>* matrix,
                                     size_t    hv       = Element::UNDEFINED,
                                     Semiring* semiring = nullptr)
        : MatrixOverSemiring(matrix, hv, semiring) {
      after();  // this is to put the matrix in normal form
    }

    // 2 parameters (vector pointer, hash value)
    // @matrix    Pointer to defining data of the matrix.
    // @semiring  Pointer to the semiring over which the matrix is defined.
    //
    // Constructs a matrix defined by <matrix>, <matrix> is not copied,
    // and should be deleted using <ElementWithVectorData::really_delete>.
    //
    // The argument <matrix> should be a vector of integer values of length *n ^
    // 2* for some integer *n*, so that the value in position *i * n + j* is the
    // entry in the *i*th row and *j*th column of the constructed matrix.
    explicit ProjectiveMaxPlusMatrix(std::vector<int64_t>* matrix,
                                     Semiring*             semiring = nullptr)
        : MatrixOverSemiring(matrix, semiring) {
      after();  // this is to put the matrix in normal form
    }

    // 2 parameters (vector const ref, semiring pointer)
    // @matrix The matrix
    // @semiring  Pointer to the semiring over which the matrix is defined.
    //
    // Constructs a matrix defined by <matrix>, which is copied into the
    // constructed matrix.
    explicit ProjectiveMaxPlusMatrix(
        std::vector<std::vector<int64_t>> const& matrix,
        Semiring*                                semiring = nullptr)
        : MatrixOverSemiring(matrix, semiring) {
      after();  // this is to put the matrix in normal form
    }

    // const
    //
    // See <Element::hash_value>.
    //
    // @return a hash value for a matrix.
    void cache_hash_value() const override;

   private:
    // a function applied after redefinition
    void after() override;
  };

  // Non-abstract
  // Partitioned binary relations are a generalisation of bipartitions, which
  // were introduced by [Martin and Mazorchuk](https://arxiv.org/abs/1102.0862).
  // This is a subclass of <ElementWithVectorData>.

  class PBR : public ElementWithVectorData<std::vector<u_int32_t>, PBR> {
   public:
    // Default
    // @vector    Pointer to defining data of the matrix.
    // @hv     The hash value of the element being created.
    //
    // Constructs a PBR defined by <vector>, <vector> is not copied,
    // and should be deleted using <ElementWithVectorData::really_delete>.
    //
    // The argument <vector> should be a vector of vectors of non-negative
    // integer values of length *2n* for some integer *n*, the vector in
    // position *i* is the list of points adjacent to *i* in the PBR.
    explicit PBR(std::vector<std::vector<u_int32_t>>* vector,
                 size_t                               hv = Element::UNDEFINED)
        : ElementWithVectorData<std::vector<u_int32_t>, PBR>(vector, hv) {}
    // TODO(JDM) a more convenient constructor

    // const
    //
    // See <Element::complexity> for more details.
    //
    // @return the two times <ElementWithVectorData::degree> all cubed.
    size_t complexity() const override;

    // const
    //
    // See <Element::degree> for more details.
    //
    // @return half the number of points in the PBR, which is also half the
    // length of the underlying vector <ElementWithVectorData::_vector>.
    size_t degree() const override;

    // const
    //
    // See <Element::hash_value>.
    //
    // @return a hash value for a PBR.
    void cache_hash_value() const override;

    // const
    //
    // See <Element::identity>.
    //
    // @return a new PBR with degree equal to <degree> where every value is
    // adjacent to its negative (or *i* is adjacent *i +* <degree> and vice
    // versa for every *i* less than <degree>).
    Element* identity() const override;

    // non-const
    // @x PBR
    // @y PBR
    //
    // See <Element::redefine>.
    //
    // Redefine **this** to be the composition of <x> and <y>. This method
    // asserts that the degrees of <x>, <y>, and **this**, are all equal, and
    // that neither <x> nor <y> equals **this**.
    void redefine(Element const* x,
                  Element const* y,
                  size_t const&  thread_id) override;

   private:
    void unite_rows(RecVec<bool>& out,
                    RecVec<bool>& tmp,
                    size_t const& vertex1,
                    size_t const& vertex2);

    void x_dfs(std::vector<bool>& x_seen,
               std::vector<bool>& y_seen,
               RecVec<bool>&      tmp,
               u_int32_t const&   n,
               u_int32_t const&   i,
               PBR const* const   x,
               PBR const* const   y,
               size_t const&      adj);

    void y_dfs(std::vector<bool>& x_seen,
               std::vector<bool>& y_seen,
               RecVec<bool>&      tmp,
               u_int32_t const&   n,
               u_int32_t const&   i,
               PBR const* const   x,
               PBR const* const   y,
               size_t const&      adj);

    static std::vector<std::vector<bool>> _x_seen;
    static std::vector<std::vector<bool>> _y_seen;
    static std::vector<RecVec<bool>>      _out;
    static std::vector<RecVec<bool>>      _tmp;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_ELEMENTS_H_
