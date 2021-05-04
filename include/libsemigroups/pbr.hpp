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

// This file contains the declaration of the PBR class.

#ifndef LIBSEMIGROUPS_PBR_HPP_
#define LIBSEMIGROUPS_PBR_HPP_

#include <cstddef>           // for size_t
#include <cstdint>           // for uint32_t, int32_t
#include <initializer_list>  // for initializer_list
#include <iosfwd>            // for ostream, ostringstream
#include <type_traits>       // for forward
#include <vector>            // for vector, operator<, operator==, allocator

#include "adapters.hpp"  // for Hash

namespace libsemigroups {

  //! Class for partitioned binary relations (PBR).
  //!
  //! Partitioned binary relations (PBRs) are a generalisation of bipartitions,
  //! which were introduced by
  //! [Martin and Mazorchuk](https://arxiv.org/abs/1102.0862).
  class PBR {
    friend void validate(PBR const& x);

   public:
    //! A constructor.
    //!
    //! Constructs a PBR defined by the vector pointed to by \p vector.
    //! The parameter \p vector should be a pointer to a vector of vectors of
    //! non-negative integer values of length \f$2n\f$ for some integer
    //! \f$n\f$, the vector in position \f$i\f$ is the list of points adjacent
    //! to \f$i\f$ in the PBR.
    explicit PBR(std::vector<std::vector<uint32_t>> const& vec);

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
    PBR(std::initializer_list<std::vector<int32_t>> const& left,
        std::initializer_list<std::vector<int32_t>> const& right);

    //! Construct and validates.
    //!
    //! Constructs a PBR initialized with the arguments \p cont
    //!
    //! \sa validate.
    template <typename... TArgs>
    static PBR make(TArgs... cont) {
      // TODO(later) validate_args
      PBR result(std::forward<TArgs>(cont)...);
      validate(result);
      return result;
    }

    //! Construct and validates.
    //!
    //! Constructs a PBR initialized with the arguments \p cont
    //!
    //! \sa validate.
    static PBR make(std::initializer_list<std::vector<uint32_t>> const& cont) {
      return make<decltype(cont)>(cont);
    }

    //! Construct and validates.
    //!
    //! Constructs a PBR initialized with the arguments \p cont
    //!
    //! \sa validate.
    static PBR make(std::initializer_list<std::vector<int32_t>> const& left,
                    std::initializer_list<std::vector<int32_t>> const& right) {
      return make<decltype(left), decltype(right)>(left, right);
    }

    //! Returns the degree of a PBR.
    //!
    //! The *degree* of a PBR is half the number of points in the PBR.
    size_t degree() const;

    //! Returns the identity PBR with degree equal to that of \c this.
    //!
    //! This member function returns a new PBR with degree equal to the degree
    //! of \c this where every value is adjacent to its negative. Equivalently,
    //! \f$i\f$ is adjacent \f$i + n\f$ and vice versa for every \f$i\f$ less
    //! than the degree \f$n\f$.
    PBR identity() const;

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
    void product_inplace(PBR const&, PBR const&, size_t = 0);

    //! Compare two PBRs for equality.
    //!
    //! \param that a PBR
    //!
    //! \returns \c true if \c this equals \p that, and \c false otherwise.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst linear in degree().
    bool operator==(PBR const& that) const {
      return _vector == that._vector;
    }

    //! Compare two PBRs for less.
    //!
    //! \param that a PBR object
    //!
    //! \returns \c true if \c this is less than \p that, and \c false
    //! otherwise.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst linear in degree().
    bool operator<(PBR const& that) const {
      return _vector < that._vector;
    }

    //! Returns a reference to the index of the vector of points related
    //! to a given point.
    //!
    //! \param i the point.
    //!
    //! \returns A value reference to a `std::vector<uint32_t>`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    std::vector<uint32_t>& operator[](size_t i) {
      return _vector[i];
    }

    //! Returns a const reference to the index of the vector of points related
    //! to a given point.
    //!
    //! \param i the point.
    //!
    //! \returns A value const reference to a `std::vector<uint32_t>`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    std::vector<uint32_t> const& operator[](size_t i) const {
      return _vector[i];
    }

    //! Returns a hash value for a PBR.
    //!
    //! This value is recomputed every time this function is called.
    //!
    //! \returns A hash value for a \c this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in `degree()`.
    //!
    //! \parameters
    //! (None)
    // not noexcept because Hash<T>::operator() isn't
    size_t hash_value() const {
      return Hash<std::vector<std::vector<uint32_t>>>()(_vector);
    }

    //! Insertion operator
    //!
    //! This member function allows PBR objects to be inserted into an
    //! ostringstream
    friend std::ostringstream& operator<<(std::ostringstream&, PBR const&);

    //! Insertion operator
    //!
    //! This member function allows PBR objects to be inserted into an ostream.
    friend std::ostream& operator<<(std::ostream&, PBR const&);

   private:
    std::vector<std::vector<uint32_t>> _vector;
  };

  //! Validates the data defining \c this.
  //!
  //! This function throws a LibsemigroupsException if
  //! the argument \p x is not valid.
  //!
  //! \param x the PBR to validate.
  //!
  //! \returns
  //! (None)
  //!
  //! \throws LibsemigroupsException if any of the following hold:
  //! * \p x does not describe a binary relation on an even number of points;
  //! * \p x has a point related to a point that is greater than degree()
  //! * a list of points related to a point is not sorted.
  //!
  //! \complexity
  //! Linear in the degree() of \p x.
  void validate(PBR const& x);

  //! Multiply two PBRs.
  //!
  //! Returns a newly constructed PBR equal to the product of \p x and \p y.
  //!
  //! \param x a PBR
  //! \param y a PBR
  //!
  //! \returns
  //! A value of type \c PBR
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! Cubic in degree().
  PBR operator*(PBR const& x, PBR const& y);

  //! Check PBRs for inequality.
  //!
  //! \param x a PBR
  //! \param y a PBR
  //!
  //! \returns
  //! A value of type \c bool.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst quadratic in the degree of \p x and \p y.
  bool operator!=(PBR const& x, PBR const& y);

  namespace detail {

    template <typename T>
    struct IsPBRHelper : std::false_type {};

    template <>
    struct IsPBRHelper<PBR> : std::true_type {};

  }  // namespace detail

  template <typename T>
  static constexpr bool IsPBR = detail::IsPBRHelper<std::decay_t<T>>::value;

  ////////////////////////////////////////////////////////////////////////
  // Adapters
  ////////////////////////////////////////////////////////////////////////

  //! Returns the approximate time complexity of multiplying PBRs.
  //!
  //! The approximate time complexity of multiplying PBRs is \f$2n ^ 3\f$
  //! where \f$n\f$ is the degree.
  template <>
  struct Complexity<PBR> {
    //! Call operator.
    //!
    //! \param x a PBR.
    //!
    //! \returns
    //! A value of type `size_t` representing the complexity of multiplying the
    //! parameter \p x by another PBR of the same degree.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t operator()(PBR const& x) const noexcept {
      return 8 * x.degree() * x.degree() * x.degree();
    }
  };

  template <>
  struct Degree<PBR> {
    size_t operator()(PBR const& x) const noexcept {
      return x.degree();
    }
  };

  template <>
  struct Hash<PBR> {
    size_t operator()(PBR const& x) const {
      return x.hash_value();
    }
  };

  template <>
  struct One<PBR> {
    PBR operator()(PBR const& x) const {
      return (*this)(x.degree());
    }

    PBR operator()(size_t N = 0) const {
      return PBR::identity(N);
    }
  };

  template <>
  struct Product<PBR> {
    void operator()(PBR& xy, PBR const& x, PBR const& y, size_t thread_id = 0) {
      xy.product_inplace(x, y, thread_id);
    }
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_PBR_HPP_
