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
    //! Type of constructor argument.
    template <typename T>
    using vector_type = std::vector<std::vector<T>> const&;

    //! Type of constructor argument.
    template <typename T>
    using initializer_list_type = std::initializer_list<std::vector<T>> const&;

    //! Deleted.
    PBR() = delete;

    //! Default copy constructor.
    PBR(PBR const&) = default;

    //! Default move constructor.
    PBR(PBR&&) = default;

    //! Default copy assignment operator.
    PBR& operator=(PBR const&) = default;

    //! Default move assignment operator.
    PBR& operator=(PBR&&) = default;

    //! Construct from adjacencies \c 0 to `2n - 1`.
    //!
    //! The parameter \p x must be a container of vectors of
    //! \c uint32_t with size \f$2n\f$ for some integer
    //! \f$n\f$, the vector in position \f$i\f$ is the list of points adjacent
    //! to \f$i\f$ in the PBR constructed.
    //!
    //! \param  x the container of vectors of adjacencies.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks whatsoever on the validity of \p x are performed.
    //!
    //! \sa \ref libsemigroups::validate(PBR const&)
    explicit PBR(vector_type<uint32_t> x);

    //! \copydoc PBR(vector_type<uint32_t>)
    explicit PBR(initializer_list_type<uint32_t> x);

    //! Construct empty PBR of given \ref degree.
    //!
    //! \param n the degree
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    explicit PBR(size_t n);

    //! Construct from adjancencies \c 1 to \c n and \c -1 to \c
    //! -n.
    //!
    //! The parameters \p left and \p right should be containers of
    //! \f$n\f$ vectors of integer values, so that
    //! the vector in position \f$i\f$ of \p left is the list of points
    //! adjacent to \f$i\f$ in the PBR, and the vector in position \f$i\f$
    //! of \p right is the list of points adjacent to \f$n + i\f$ in the PBR.
    //! A negative value \f$i\f$ corresponds to \f$n - i\f$.
    //!
    //! \param left container of adjacencies of \c 1 to \c n
    //! \param right container of adjacencies of \c n + 1 to \c 2n.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks whatsoever on the validity of \p left or \p right are
    //! performed.
    //!
    //! \sa libsemigroups::validate(PBR const&) and
    //! make(initializer_list_type<int32_t>, initializer_list_type<int32_t>)
    PBR(initializer_list_type<int32_t> left,
        initializer_list_type<int32_t> right);

    // clang-format off
    //! \copydoc PBR(initializer_list_type<int32_t>, initializer_list_type<int32_t>) NOLINT(whitespace/line_length)
    // clang-format on
    PBR(vector_type<int32_t> left, vector_type<int32_t> right);

    //! Construct and validate.
    //!
    //! \tparam T the types of the arguments
    //!
    //! \param args the arguments to forward to the constructor.
    //!
    //! \returns
    //! A PBR constructed from \p args and validated.
    //!
    //! \throws LibsemigroupsException if libsemigroups::validate(PBR const&)
    //! throws when called with the constructed PBR.
    template <typename... T>
    static PBR make(T... args) {
      // TODO(later) validate_args
      PBR result(std::forward<T>(args)...);
      validate(result);
      return result;
    }

    //! Construct and validate.
    //!
    //! \param args the arguments to forward to the constructor.
    //!
    //! \returns
    //! A PBR constructed from \p args and validated.
    //!
    //! \throws LibsemigroupsException if libsemigroups::validate(PBR const&)
    //! throws when called with the constructed PBR.
    static PBR make(initializer_list_type<uint32_t> args) {
      return make<decltype(args)>(args);
    }

    //! Construct and validate.
    //!
    //! \param left the 1st argument to forward to the constructor.
    //! \param right the 2nd argument to forward to the constructor.
    //!
    //! \returns
    //! A PBR constructed from \p args and validated.
    //!
    //! \throws LibsemigroupsException if libsemigroups::validate(PBR const&)
    //! throws when called with the constructed PBR.
    static PBR make(initializer_list_type<int32_t> left,
                    initializer_list_type<int32_t> right) {
      return make<decltype(left), decltype(right)>(left, right);
    }

    //! Returns the degree of a PBR.
    //!
    //! The *degree* of a PBR is half the number of points in the PBR.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    size_t degree() const noexcept;

    //! Returns the identity PBR with degree degree().
    //!
    //! This member function returns a new PBR with degree equal to the degree
    //! of \c this where every value is adjacent to its negative. Equivalently,
    //! \f$i\f$ is adjacent \f$i + n\f$ and vice versa for every \f$i\f$ less
    //! than the degree \f$n\f$.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! A PBR.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    PBR identity() const;

    //! Returns the identity PBR with specified degree.
    //!
    //! This function returns a new PBR with degree equal to \p n where every
    //! value is adjacent to its negative. Equivalently, \f$i\f$ is adjacent
    //! \f$i + n\f$ and vice versa for every \f$i\f$ less than the degree
    //! \f$n\f$.
    //!
    //! \param n the degree.
    //!
    //! \returns
    //! A PBR.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    static PBR identity(size_t n);

    //! Multiply two PBR objects and store the product in \c this.
    //!
    //! Replaces the contents of \c this by the product of \p x and \p y.
    //!
    //! The parameter \p thread_id is required since some temporary storage is
    //! required to find the product of \p x and \p y.  Note that if different
    //! threads call this member function with the same value of \p thread_id
    //! then bad things will happen.
    //!
    //! \param x a PBR.
    //! \param y a PBR.
    //! \param thread_id the index of the calling thread (defaults to \c 0).
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks are made on whether or not the parameters are compatible. If
    //! \p x and \p y have different degrees, then bad things will happen.
    void product_inplace(PBR const& x, PBR const& y, size_t thread_id = 0);

    //! Check equality.
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

    //! Compare.
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

    //! Returns a reference to the points adjacent to a given point.
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

    //! Returns a const reference to the points adjacent to a given point.
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
    //! \ostringstream
    friend std::ostringstream& operator<<(std::ostringstream&, PBR const&);

    //! Insertion operator
    //!
    //! This member function allows PBR objects to be inserted into an \ostream.
    friend std::ostream& operator<<(std::ostream&, PBR const&);

   private:
    std::vector<std::vector<uint32_t>> _vector;
  };

  //! Validate a PBR.
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
  //! Linear in the PBR::degree of \p x.
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
  inline bool operator!=(PBR const& x, PBR const& y) {
    return !(x == y);
  }

  //! Convenience function that just calls ``operator<``.
  inline bool operator>(PBR const& x, PBR const& y) {
    return y < x;
  }

  //! Convenience function that just calls ``operator<`` and ``operator==``.
  inline bool operator<=(PBR const& x, PBR const& y) {
    return x < y || x == y;
  }

  //! Convenience function that just calls ``operator<=``.
  inline bool operator>=(PBR const& x, PBR const& y) {
    return y <= x;
  }

  namespace detail {

    template <typename T>
    struct IsPBRHelper : std::false_type {};

    template <>
    struct IsPBRHelper<PBR> : std::true_type {};

  }  // namespace detail

  //! Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p T is
  //! \ref PBR.
  //!
  //! \tparam T a type.
  template <typename T>
  static constexpr bool IsPBR = detail::IsPBRHelper<T>::value;

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

  template <>
  struct IncreaseDegree<PBR> {
    void operator()(PBR&, size_t) {}
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_PBR_HPP_
