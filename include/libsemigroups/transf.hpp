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

// This file contains the declaration of the partial transformation class and
// its subclasses.
//
// WARNING Nothing in this file is documented (in the online manual), and this
// is a work in progress, and we don't currently recommend using it.

#ifndef LIBSEMIGROUPS_TRANSF_HPP_
#define LIBSEMIGROUPS_TRANSF_HPP_

#include <limits>       // for numeric_limits
#include <numeric>      // for std::iota
#include <type_traits>  // for is_integral
#include <vector>       // for vector

#include "constants.hpp"  // for UNDEFINED
#include "hpcombi.hpp"    // for HPCombi::Transf16, ...
#include "types.hpp"      // for SmallestInteger

namespace libsemigroups {

  //! Abstract class for partial transformations.
  //!
  //! This is a template class for partial transformations, which is a subclass
  //! of ElementWithVectorDataDefaultHash. For example,
  //! Transformation<uint128_t> is a subclass of
  //! PTransformation<uint128_t, Transformation<uint128_t>>.
  //!
  //! The template parameter \p TValueType is the type of image values, i.e.
  //! uint16_t, and so on.  The value of the template parameter \p S can be
  //! used to reduce (or increase) the amount of memory required by instances
  //! of this class.
  //!
  //! The template parameter \p TSubclass is the subclass of
  //! PTransformation used by the PTransformation::identity method
  //! to construct an identity, so that the return value of the method
  //! PTransformation::identity is of type \p TSubclass instead of type
  //! PTransformation.
  //!
  //! This class is abstract since it does not implement all methods required
  //! by the Element class, it exists to provide common methods for its
  //! subclasses.
  //!
  //! A *partial transformation* \f$f\f$ is just a function defined on a subset
  //! of \f$\{0, 1, \ldots, n - 1\}\f$ for some integer \f$n\f$ called the
  //! *degree*  of *f*.  A partial transformation is stored as a vector of the
  //! images of \f$\{0, 1, \ldots, n -1\}\f$, i.e.
  //! \f$\{(0)f, (1)f, \ldots, (n - 1)f\}\f$
  //! where the value PTransformation::UNDEFINED is used to
  //! indicate that \f$(i)f\f$ is, you guessed it, undefined (i.e. not among
  //! the points where \f$f\f$ is defined).

  struct PolymorphicPTransf {};

  template <typename TValueType, typename TContainer>
  class BasePTransf : public PolymorphicPTransf {
   public:
    //! A constructor.
    //!
    //! Constructs an PTransf *f* from \p vec and validates it.  The
    //! vector \p vec (of length some non-negative integer *n*) should contain
    //! in position *i* the image \f$(i)f\f$, or UNDEFINED if *f* is not
    //! defined on *i*, for all \f$0 < i < n\f$.
    BasePTransf() : _container() {}

    explicit BasePTransf(TContainer&& cont) : _container(std::move(cont)) {
      // validate();
    }

    explicit BasePTransf(TContainer const& cont) : _container(cont) {
      // validate();
    }

    BasePTransf(BasePTransf const&) = default;

    bool operator<(BasePTransf const& that) const {
      return this->_container < that._container;
    }

    bool operator==(BasePTransf const& that) const {
      return this->_container == that._container;
    }

    TValueType& operator[](size_t i) noexcept {
      return _container[i];
    }

    TValueType const& operator[](size_t i) const noexcept {
      return _container[i];
    }

    // TODO(later) other operators
    template <typename TSubclass>
    TSubclass operator*(TSubclass const& y) const {
      TSubclass xy(y.degree());
      xy.in_place_product(*static_cast<TSubclass const*>(this), y);
      return xy;
    }
    //! Returns the rank of a partial transformation.
    //!
    //! The *rank* of a partial transformation is the number of its distinct
    //! image values, not including UNDEFINED. This
    //! method recomputes the return value every time it is called.
    size_t rank() const {
      return std::count_if(
          _container.cbegin(),
          _container.cend(),
          [](TValueType const& val) -> bool { return val == UNDEFINED; });
      // TODO(later) double check this is correct

      // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
      //     Element empty_key() const override {
      //       std::vector<TValueType> vector(this->degree() + 1);
      //       std::iota(vector.begin(), vector.end(), 0);
      //       return new PPerm(std::move(vector));
      //     }
      // #endif
    }

   protected:
    //! Validates the data defining \c this.
    //!
    //! This method throws a LIBSEMIGROUPS_EXCEPTION if any value of \c this is
    //! out of bounds (i.e. less than 0, greater than or equal to \c
    //! this->degree(), and not PTransf::UNDEFINED).
    // void validate() const {
    //   for (auto const& val : _container) {
    //     if ((val < 0 || val >= this->degree()) && val != UNDEFINED) {
    //       LIBSEMIGROUPS_EXCEPTION("image value out of bounds, found "
    //                               << static_cast<size_t>(val)
    //                               << ", must be less than " <<
    //                               this->degree());
    //     }
    //   }
    // }
    TContainer _container;
  };

  template <typename TValueType, typename TContainer>
  class PTransf;

  template <typename TValueType>
  class PTransf<TValueType, std::vector<TValueType>>
      : public BasePTransf<TValueType, std::vector<TValueType>> {
   public:
    using value_type     = TValueType;
    using container_type = std::vector<TValueType>;

    using BasePTransf<TValueType, std::vector<TValueType>>::BasePTransf;

    explicit PTransf(size_t n)
        : BasePTransf<TValueType, std::vector<TValueType>>() {
      this->_container.resize(n);
    }

    //! Returns the degree of a partial transformation.
    //!
    //! The *degree* of a partial transformation is the number of points used
    //! in its definition, which is equal to the size of the underlying
    //! container.
    size_t degree() const noexcept {
      return this->_container.size();
    }

    //! Returns the identity transformation with degrees of \c this.
    //!
    //! This method returns a new partial transformation with degree equal to
    //! the degree of \c this that fixes every value from *0* to the degree of
    //! \c this.
    template <typename TSubclass>
    TSubclass identity() const {
      return identity<TSubclass>(degree());
    }

    template <typename TSubclass>
    static TSubclass identity(size_t n) {
      std::vector<TValueType> cont(n);
      std::iota(cont.begin(), cont.end(), 0);
      return TSubclass(std::move(cont));
    }

    void increase_degree_by(size_t m) {
      this->_container.resize(this->_container.size() + m);
      std::iota(this->_container.end() - m,
                this->_container.end(),
                this->_container.size() - m);
    }
  };

  template <typename TValueType, size_t N>
  class PTransf<TValueType, std::array<TValueType, N>>
      : public BasePTransf<TValueType, std::array<TValueType, N>> {
   public:
    using value_type     = TValueType;
    using container_type = std::array<TValueType, N>;

    using BasePTransf<TValueType, std::array<TValueType, N>>::BasePTransf;

    explicit PTransf(size_t = 0)
        : BasePTransf<TValueType, std::array<TValueType, N>>() {}

    //! Returns the degree of a partial transformation.
    //!
    //! The *degree* of a partial transformation is the number of points used
    //! in its definition, which is equal to the size of the underlying
    //! container.
    constexpr size_t degree() const noexcept {
      return N;
    }

    //! Returns the identity transformation with degree of \c this.
    //!
    //! This method returns a new partial transformation with degree equal to
    //! the degree of \c this that fixes every value from *0* to the degree of
    //! \c this.
    template <typename TSubclass>
    static TSubclass identity(size_t = 0) {
      std::array<TValueType, N> cont;
      std::iota(cont.begin(), cont.end(), 0);
      return TSubclass(std::move(cont));
    }
  };

  //! Template class for transformations.
  //!
  //! The value of the template parameter \p T can be used to reduce the amount
  //! of memory required by instances of this class; see PTransformation
  //! and ElementWithVectorData for more details.
  //!
  //! A *transformation* \f$f\f$ is just a function defined on the whole of
  //! \f$\{0, 1, \ldots, n - 1\}\f$ for some integer \f$n\f$ called the
  //! *degree* of \f$f\f$.  A transformation is stored as a vector of the
  //! images of \f$\{0, 1, \ldots, n - 1\}\f$, i.e.
  //! \f$\{(0)f, (1)f, \ldots, (n - 1)f\}\f$.
  template <typename TValueType, typename TContainer = std::vector<TValueType>>
  class Transf : public PTransf<TValueType, TContainer> {
    static_assert(std::is_integral<TValueType>::value,
                  "template parameter TValueType must be an integral type");
    static_assert(!std::numeric_limits<TValueType>::is_signed,
                  "template parameter TValueType must be unsigned");

   public:
    using PTransf<TValueType, TContainer>::PTransf;

    //! Multiply \p x and \p y and stores the result in \c this.
    //!
    //! See Element::redefine for more details about this method.
    //!
    //! This method asserts that the degrees of \p x, \p y, and \c this, are
    //! all equal, and that neither \p x nor \p y equals \c this.
    void in_place_product(Transf const& x, Transf const& y) {
      LIBSEMIGROUPS_ASSERT(x.degree() == y.degree());
      LIBSEMIGROUPS_ASSERT(x.degree() == this->degree());
      LIBSEMIGROUPS_ASSERT(&x != this && &y != this);
      size_t const n = this->degree();
      for (TValueType i = 0; i < n; ++i) {
        this->_container[i] = y[x[i]];
      }
    }

    //! Validates the data defining \c this.
    //!
    //! This method throws a LIBSEMIGROUPS_EXCEPTION if any value of \c this is
    //! out of bounds (i.e. greater than or equal to \c this->degree()).
    // void validate() const {
    //   size_t deg = this->degree();
    //   for (auto const& val : this->_container) {
    //     if (val >= deg) {
    //       LIBSEMIGROUPS_EXCEPTION("image value out of bounds, found "
    //                               << static_cast<size_t>(val)
    //                               << ", must be less than " << deg);
    //     }
    //   }
    // }
  };

  //! Template class for partial permutations.
  //!
  //! The value of the template parameter \p T can be used to reduce the
  //! amount of memory required by instances of this class; see
  //! PTransformation and ElementWithVectorData for more details.
  //!
  //! A *partial permutation* \f$f\f$ is just an injective partial
  //! transformation, which is stored as a vector of the images of
  //! \f$\{0, 1, \ldots, n - 1\}\f$, i.e.
  //! i.e. \f$\{(0)f, (1)f, \ldots, (n - 1)f\}\f$ where the value
  //! PTransformation::UNDEFINED is
  //! used to indicate that \f$(i)f\f$ is undefined (i.e. not among
  //! the points where \f$f\f$ is defined).
  template <typename TValueType, typename TContainer = std::vector<TValueType>>
  class PPerm : public PTransf<TValueType, TContainer> {
    static_assert(std::is_integral<TValueType>::value,
                  "template parameter TValueType must be an integral type");
    static_assert(!std::numeric_limits<TValueType>::is_signed,
                  "template parameter TValueType must be unsigned");

   public:
    using PTransf<TValueType, TContainer>::PTransf;

    //! A constructor.
    //!
    //! Constructs a partial perm of degree \p deg such that \code (dom[i])f =
    //! ran[i] \endcode for all \c i and which is undefined on every other
    //! value in the range 0 to (strictly less than \p deg). This method
    //! asserts that \p dom and \p ran have equal size and that \p deg is
    //! greater than to the maximum value in \p dom or \p ran.
    PPerm(TContainer const& dom, TContainer const& ran, size_t deg)
        // The vector passed in the next line shouldn't be necessary, but with
        // GCC5 PPerm fails to inherit the 0-param constructor from
        // PTransf.
        : PTransf<TValueType, TContainer>(TContainer()) {
      if (dom.size() != ran.size()) {
        LIBSEMIGROUPS_EXCEPTION("domain and range size mismatch, domain has "
                                "size %d but range has size %d",
                                dom.size(),
                                ran.size());
      } else if (!(dom.empty()
                   || deg > *std::max_element(dom.cbegin(), dom.cend()))) {
        LIBSEMIGROUPS_EXCEPTION(
            "domain value out of bounds, found %d, must be less than %d",
            static_cast<size_t>(*std::max_element(dom.cbegin(), dom.cend())),
            deg);
      }
      this->_container.resize(deg, UNDEFINED);
      for (size_t i = 0; i < dom.size(); i++) {
        this->_container[dom[i]] = ran[i];
      }
    }

    //! A constructor.
    //!
    //! Constructs vectors from \p dom and \p ran and uses the constructor
    //! above.
    PPerm(std::initializer_list<TValueType> dom,
          std::initializer_list<TValueType> ran,
          size_t                            deg)
        : PPerm<TValueType>(TContainer(dom), TContainer(ran), deg) {}

    //! Validates the data defining \c this.
    //!
    //! This method throws a LIBSEMIGROUPS_EXCEPTION if any value of \c this is
    //! out of bounds (i.e. greater than or equal to \c this->degree()), and not
    //! equal to PPerm::UNDEFINED), or if any image appears more than
    //! once.
    // void validate() const {
    //   size_t const      deg = this->degree();
    //   std::vector<bool> present(deg, false);
    //   for (auto const& val : this->_container) {
    //     if (val != UNDEFINED) {
    //       if (val < 0 || val >= this->degree()) {
    //         LIBSEMIGROUPS_EXCEPTION("image value out of bounds, found "
    //                                 << static_cast<size_t>(val)
    //                                 << ", must be less than "
    //                                 << this->degree());
    //       } else if (present[val]) {
    //         LIBSEMIGROUPS_EXCEPTION("duplicate image value "
    //                                 << static_cast<size_t>(val));
    //       }
    //       present[val] = true;
    //     }
    //   }
    // }

    // void increase_degree_by(size_t m) override {
    //  this->_container.insert(this->_container.end(), m, UNDEFINED);
    //  this->reset_hash_value();
    // }

    //! Multiply \p x and \p y and stores the result in \c this.
    //!
    //! See Element::redefine for more details about this method.
    //!
    //! This method asserts that the degrees of \p x, \p y, and \c this, are
    //! all equal, and that neither \p x nor \p y equals \c this.
    void in_place_product(PPerm const& x, PPerm const& y) override {
      LIBSEMIGROUPS_ASSERT(x.degree() == y.degree());
      LIBSEMIGROUPS_ASSERT(x.degree() == this->degree());
      LIBSEMIGROUPS_ASSERT(&x != this && &y != this);
      size_t const n = this->degree();
      for (TValueType i = 0; i < n; i++) {
        this->_container[i] = (x[i] == UNDEFINED ? UNDEFINED : y[x[i]]);
      }
    }

    PPerm<TValueType, TContainer> right_one() const {
      std::vector<TValueType> img(this->degree(),
                                  static_cast<TValueType>(UNDEFINED));
      size_t const            n = this->degree();
      for (size_t i = 0; i < n; ++i) {
        if (this->_container[i] != UNDEFINED) {
          img[this->_container[i]] = this->_container[i];
        }
      }
      return PPerm<TValueType, TContainer>(img);
    }

    PPerm<TValueType> left_one() const {
      std::vector<TValueType> dom(this->degree(),
                                  static_cast<TValueType>(UNDEFINED));
      size_t const            n = this->_container.size();
      for (size_t i = 0; i < n; ++i) {
        if (this->_container[i] != UNDEFINED) {
          dom[i] = i;
        }
      }
      return PPerm<TValueType>(dom);
    }
  };

  //! Template class for permutations.
  //!
  //! The value of the template parameter \p T can be used to reduce the amount
  //! of memory required by instances of this class; see PTransfformation
  //! and ElementWithVectorData for more details.
  //!
  //! A *permutation* \f$f\f$ is an injective transformation defined on the
  //! whole of \f$\{0, 1, \ldots, n - 1\}\f$ for some integer \f$n\f$ called the
  //! *degree* of \f$f\f$.
  //! A permutation is stored as a vector of the images of
  //! \f$(0, 1, \ldots, n - 1)\f$,
  //! i.e. \f$((0)f, (1)f, \ldots, (n - 1)f)\f$.
  template <typename TValueType, typename TContainer>
  class Perm : public Transf<TValueType, TContainer> {
   public:
    using Transf<TValueType, TContainer>::Transf;

    //! Validates the data defining \c this.
    //!
    //! This method throws a LIBSEMIGROUPS_EXCEPTION if any value of \c this is
    //! out of bounds (i.e. greater than or equal to \c this->degree()), or if
    //! any image appears more than once.
    // void validate() const {
    //   std::vector<bool> present(this->degree(), false);
    //   for (auto const& val : this->_container) {
    //     if (val < 0 || val >= this->degree()) {
    //       LIBSEMIGROUPS_EXCEPTION("image value out of bounds, found "
    //                               << static_cast<size_t>(val)
    //                               << ", must be less than " <<
    //                               this->degree());
    //     } else if (present[val]) {
    //       LIBSEMIGROUPS_EXCEPTION("duplicate image value "
    //                               << static_cast<size_t>(val));
    //     }
    //     present[val] = true;
    //   }
    // }

    //! Returns the inverse of a permutation.
    //!
    //! The *inverse* of a permutation \f$f\f$ is the permutation \f$g\f$ such
    //! that \f$fg = gf\f$ is the identity permutation of degree \f$n\f$.
    Perm inverse() const {
      size_t const n  = this->degree();
      Perm         id = this->template identity<Perm>();
      for (TValueType i = 0; i < n; i++) {
        id[(*this)[i]] = i;
      }
      return id;
    }
  };

  template <typename TSubclass>
  struct Degree<
      TSubclass,
      typename std::enable_if<
          std::is_base_of<PolymorphicPTransf, TSubclass>::value>::type> {
    inline size_t operator()(TSubclass const& x) const {
      return x.degree();
    }
  };

  template <typename TSubclass>
  struct One<TSubclass,
             typename std::enable_if<
                 std::is_base_of<PolymorphicPTransf, TSubclass>::value>::type> {
    TSubclass operator()(TSubclass const& x) const {
      return static_cast<TSubclass>(x.identity());
    }

    TSubclass operator()(size_t N = 0) {
      return TSubclass::template identity<TSubclass>(N);
    }
  };

  template <typename TValueType, typename TContainerType>
  struct Inverse<Perm<TValueType, TContainerType>> {
    Perm<TValueType, TContainerType>
    operator()(Perm<TValueType, TContainerType> const& x) {
      return x.inverse();
    }
  };

  template <typename TValueType, typename TContainerType>
  struct ImageRightAction<
      Perm<TValueType, TContainerType>,
      TValueType,
      typename std::enable_if<std::is_integral<TValueType>::value>::type> {
    TValueType operator()(TValueType const                        pt,
                          Perm<TValueType, TContainerType> const& x) {
      return x[pt];
    }
  };

  template <typename TSubclass>
  struct Product<
      TSubclass,
      typename std::enable_if<
          std::is_base_of<PolymorphicPTransf, TSubclass>::value>::type> {
    void operator()(TSubclass& xy, TSubclass const& x, TSubclass const& y) {
      xy.in_place_product(x, y);
    }
  };

  template <size_t N>
  struct NewTransfHelper {
    using int_type = typename SmallestInteger<N>::type;
#ifdef LIBSEMIGROUPS_HPCOMBI
    using type =
        typename std::conditional<N >= 17,
                                  Transf<int_type, std::array<int_type, N>>,
                                  HPCombi::Transf16>::type;
#else
    using type = Transf<int_type, std::array<int_type, N>>;
#endif
  };

  template <size_t N>
  struct NewPPermHelper {
    using int_type = typename SmallestInteger<N>::type;
#ifdef LIBSEMIGROUPS_HPCOMBI
    using type =
        typename std::conditional<N >= 17,
                                  PPerm<int_type, std::array<int_type, N>>,
                                  HPCombi::PPerm16>::type;
#else
    using type = PPerm<int_type, std::array<int_type, N>>;
#endif
  };

  template <size_t N>
  struct NewPermHelper {
    using int_type = typename SmallestInteger<N>::type;
#ifdef LIBSEMIGROUPS_HPCOMBI
    using type =
        typename std::conditional<N >= 17,
                                  Perm<int_type, std::array<int_type, N>>,
                                  HPCombi::Perm16>::type;
#else
    using type = Perm<int_type, std::array<int_type, N>>;
#endif
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TRANSF_HPP_
