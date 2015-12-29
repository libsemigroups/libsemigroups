/*******************************************************************************
 * Semigroups++
 *
 * This file contains classes for creating elements of a semigroup.
 *
 *******************************************************************************/

#ifndef SEMIGROUPS_ELEMENTS_H
#define SEMIGROUPS_ELEMENTS_H
// #define NDEBUG

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Header includes . . .
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "semiring.h"
#include "blocks.hh"

#include <assert.h>
#include <functional>
#include <iostream>
#include <math.h>
#include <vector>
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Namespaces
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

using namespace semiring;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Abstract base class for elements of a semigroup
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Element {

  public:
    virtual ~Element () {}

    bool operator == (const Element& that) const {
      return this->equals(&that);
    };

    bool operator < (const Element& that) const {
      return this->less(&that);
    };

    virtual size_t   complexity    ()                               const = 0;
    virtual size_t   degree        ()                               const = 0;
    virtual bool     less          (const Element*)                 const = 0;
    virtual bool     equals        (const Element*)                 const = 0;
    virtual size_t   hash_value    ()                               const = 0;
    virtual Element* identity      ()                               const = 0;
    virtual Element* really_copy   (size_t = 0)                     const = 0;
    virtual void     really_delete ()                                     = 0;
    virtual void     redefine      (Element const*, Element const*)       = 0;
};

class myequal {
public:
   size_t operator()(const Element* x, const Element* y) const{
     return x->equals(y);
   }
};

namespace std {
  template<>
    struct hash<const Element* > {
    size_t operator() (const Element* x) const {
      return x->hash_value();
    }
  };
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Abstract base class for elements using a vector to store the data.
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename S, class T>
class ElementWithVectorData : public Element {

 public:
    explicit ElementWithVectorData (size_t size) : _vector(new std::vector<S>()) {
      _vector->resize(size);
    }

    explicit ElementWithVectorData (std::vector<S>* vector) : _vector(vector) {}

    inline S operator[] (size_t pos) const {
      return (*_vector)[pos];
    }

    inline S at (size_t pos) const {
      return _vector->at(pos);
    }

    bool equals (const Element* that) const override {
      return *(static_cast<const T*>(that)->_vector)
        == *(this->_vector);
    }

    virtual bool less (const Element* that) const override {
      auto ewvd = static_cast<const ElementWithVectorData*>(that);
      if (this->_vector->size() != ewvd->_vector->size()) {
        return this->_vector->size() < ewvd->_vector->size();
      }
      for (size_t i = 0; i < this->_vector->size(); i++) {
        if ((*this)[i] != (*ewvd)[i]) {
          return (*this)[i] < (*ewvd)[i];
        }
      }
      return false;
    }

    // this must be overridden if increase_deg_by != 0
    virtual Element* really_copy (size_t increase_deg_by) const override {
      assert(increase_deg_by == 0);
      std::vector<S>* vector(new std::vector<S>(*_vector));
      return new T(vector);
    }

    virtual void really_delete () override {
      delete _vector;
    };

 protected:
    std::vector<S>* _vector;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Template for partial transformations
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename S, typename T>
class PartialTransformation :
  public ElementWithVectorData<S, T> {

  public:

    explicit PartialTransformation (std::vector<S>* vector) :
      ElementWithVectorData<S, T>(vector) {}

    size_t complexity () const override {
      return this->_vector->size();
    }

    size_t degree () const override {
      return this->_vector->size();
    }

    size_t hash_value () const override {
      size_t seed = 0;
      size_t deg = this->degree();
      for (size_t i = 0; i < deg; i++) {
        seed = ((seed * deg) + this->_vector->at(i));
      }
      return seed;
    }

    Element* identity () const override {
      std::vector<S>* vector(new std::vector<S>());
      vector->reserve(this->degree());
      for (size_t i = 0; i < this->degree(); i++) {
        vector->push_back(i);
      }
      return new T(vector);
    }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Template for transformations
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T>
class Transformation :
  public PartialTransformation<T, Transformation<T> > {

  public:

    Transformation (std::vector<T>* vector) :
      PartialTransformation<T, Transformation<T> >(vector) {}

    Element* really_copy (size_t increase_deg_by = 0) const override {
      std::vector<T>* out(new std::vector<T>(*this->_vector));
      size_t n   = this->_vector->size();
      for (size_t i = n; i < n + increase_deg_by; i++) {
        out->push_back(i);
      }
      return new Transformation<T>(out);
    }

    // multiply x and y into this
    void redefine (Element const* x, Element const* y) override {
      assert(x->degree() == y->degree());
      assert(x->degree() == this->degree());
      Transformation<T> xx(*static_cast<Transformation<T> const*>(x));
      Transformation<T> yy(*static_cast<Transformation<T> const*>(y));

      for (T i = 0; i < this->degree(); i++) {
        this->_vector->at(i) = yy[xx[i]];
      }
    }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Template for partial perms
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T>
class PartialPerm : public PartialTransformation<T, PartialPerm<T> > {

  public:

    explicit PartialPerm (std::vector<T>* vector) :
      PartialTransformation<T, PartialPerm<T> >(vector) {}

    // this is required so that we sort in the same way as GAP :(
    // FIXME this doesn't work
    virtual bool less (const Element* that) const override {
      auto pp_this = static_cast<const PartialPerm<T>*>(this);
      auto pp_that = static_cast<const PartialPerm<T>*>(that);

      size_t deg_this = pp_this->degree();
      for (auto it = pp_this->_vector->end() - 1; it >= pp_this->_vector->begin(); it--) {
        if (*it == UNDEFINED) {
          deg_this--;
        } else {
          break;
        }
      }
      size_t deg_that = pp_that->degree();
      for (auto it = pp_that->_vector->end() - 1; it >=
          pp_that->_vector->begin() && deg_that >= deg_this; it--) {
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
        if ((*pp_this)[i] != (*pp_that)[i]) {
          return (*pp_this)[i] == UNDEFINED || ((*pp_that)[i] != UNDEFINED &&
              (*pp_this)[i] < (*pp_that)[i]);
        }
      }
      return false;
    }

    Element* really_copy (size_t increase_deg_by = 0) const override {
      std::vector<T>* vector(new std::vector<T>(*this->_vector));
      for (size_t i = 0; i < increase_deg_by; i++) {
        vector->push_back(UNDEFINED);
      }
      return new PartialPerm<T>(vector);
    }

    // multiply x and y into this
    void redefine (Element const* x, Element const* y) override {
      assert(x->degree() == y->degree());
      assert(x->degree() == this->degree());
      PartialPerm<T> xx(*static_cast<PartialPerm<T> const*>(x));
      PartialPerm<T> yy(*static_cast<PartialPerm<T> const*>(y));

      for (T i = 0; i < this->degree(); i++) {
        this->_vector->at(i) = (xx[i] == UNDEFINED ? UNDEFINED : yy[xx[i]]);
      }
    }

    size_t rank () const {
      size_t nr_defined = 0;
      for (auto x: *this->_vector) {
        if (x != UNDEFINED) {
          nr_defined++;
        }
      }
      return nr_defined;
    }

  private:

    T UNDEFINED = (T) -1;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Boolean matrices
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class BooleanMat: public ElementWithVectorData<bool, BooleanMat> {

  public:

    BooleanMat             (std::vector<bool>* matrix) :
      ElementWithVectorData<bool, BooleanMat>(matrix) {}

    size_t   complexity ()                               const override;
    size_t   degree     ()                               const override;
    size_t   hash_value ()                               const override;
    Element* identity   ()                               const override;
    void     redefine   (Element const*, Element const*)       override;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Bipartitions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Bipartition : public ElementWithVectorData<u_int32_t, Bipartition> {

 public:
    explicit Bipartition (size_t degree) :
      ElementWithVectorData<u_int32_t, Bipartition> (2 * degree),
      _nr_blocks(Bipartition::UNDEFINED),
      _nr_left_blocks(Bipartition::UNDEFINED),
      _trans_blocks_lookup(),
      _rank(Bipartition::UNDEFINED)
      {}

    explicit Bipartition (std::vector<u_int32_t>* blocks) :
      ElementWithVectorData<u_int32_t, Bipartition>(blocks),
      _nr_blocks(Bipartition::UNDEFINED),
      _nr_left_blocks(Bipartition::UNDEFINED),
      _trans_blocks_lookup(),
      _rank(Bipartition::UNDEFINED)
     {}

    size_t   complexity()                             const override;
    size_t   degree()                                 const override;
    size_t   hash_value()                             const override;
    Element* identity()                               const override;
    void     redefine(Element const*, Element const*)       override;

    u_int32_t block(size_t pos)                       const;
    u_int32_t const_nr_blocks()                       const;

    u_int32_t             nr_blocks();
    u_int32_t             nr_left_blocks();
    u_int32_t             nr_right_blocks();
    u_int32_t             rank();
    bool                  is_transverse_block(size_t index);
    Blocks*               left_blocks(); //FIXME should be const
    Blocks*               right_blocks();//FIXME should be const

    inline void set_nr_blocks (size_t nr_blocks) {
      _nr_blocks = nr_blocks;
    }

    inline void set_nr_left_blocks (size_t nr_left_blocks) {
      _nr_left_blocks = nr_left_blocks;
    }

    inline void set_rank (size_t rank) {
      _rank = rank;
    }

    inline typename std::vector<u_int32_t>::iterator begin () const {
      return _vector->begin();
    }

 private:
    u_int32_t          fuseit(u_int32_t);
    void               init_trans_blocks_lookup();

    size_t             _nr_blocks;
    size_t             _nr_left_blocks;
    std::vector<bool>  _trans_blocks_lookup;
    size_t             _rank;

    static std::vector<u_int32_t> _fuse;
    static std::vector<u_int32_t> _lookup;
    static u_int32_t              UNDEFINED;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Matrices over semirings
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class MatrixOverSemiring :
  public ElementWithVectorData<long, MatrixOverSemiring> {

  public:

    MatrixOverSemiring (std::vector<long>* matrix,
                        Semiring*          semiring = nullptr) :

                       ElementWithVectorData<long,
                                             MatrixOverSemiring>(matrix),
                       _semiring(semiring) {}

             Semiring* semiring     ()                               const;
             void      set_semiring (Semiring* semiring);

             size_t   complexity  ()                               const override;
             size_t   degree      ()                               const override;
    virtual  size_t   hash_value  ()                               const override;
             Element* identity    ()                               const override;
             Element* really_copy (size_t increase_deg_by)         const override;
             void     redefine    (Element const*, Element const*) override;

  private:

    // a function applied after redefinition
    virtual void after () {}

    Semiring* _semiring;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Projective max-plus matrices
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class ProjectiveMaxPlusMatrix: public MatrixOverSemiring {

  public:

    ProjectiveMaxPlusMatrix(std::vector<long>* matrix,
                            Semiring*          semiring) :
      MatrixOverSemiring(matrix, semiring) {};

    size_t hash_value () const override;

  private:

    // a function applied after redefinition
    void after () override;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Partitioned binary relations
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class PBR: public ElementWithVectorData<std::vector<u_int32_t>, PBR> {

  public:

    PBR (std::vector<std::vector<u_int32_t> >* vector) :
      ElementWithVectorData<std::vector<u_int32_t>, PBR>(vector) {}

    size_t   complexity ()                               const override;
    size_t   degree     ()                               const override;
    size_t   hash_value ()                               const override;
    Element* identity   ()                               const override;
    void     redefine   (Element const*, Element const*)       override;

  private:

    void add_adjacency (size_t vertex1, size_t vertex2);

    void x_dfs (u_int32_t          n,
                u_int32_t          i,
                u_int32_t          v,        // the vertex we're currently doing
                std::vector<bool>& x_seen,
                std::vector<bool>& y_seen,
                PBR const*         x,
                PBR const*         y      );

    void y_dfs (u_int32_t          n,
                u_int32_t          i,
                u_int32_t          v,        // the vertex we're currently doing
                std::vector<bool>& x_seen,
                std::vector<bool>& y_seen,
                PBR const*         x,
                PBR const*         y      );
};

#endif
