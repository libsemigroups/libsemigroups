//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

// This file contains the declaration of the TLCODE class.

#ifndef LIBSEMIGROUPS_TLCODE_HPP_
#define LIBSEMIGROUPS_TLCODE_HPP_

#include <cstddef>           // for size_t
#include <cstdint>           // for uint32_t, int32_t
#include <initializer_list>  // for initializer_list
#include <iosfwd>            // for ostream, ostringstream
#include <string>            // for string
#include <type_traits>       // for forward
#include <utility>           // for forward
#include <vector>            // for vector, operator<, operator==, allocator

#include "adapters.hpp"  // for Hash
#include "types.hpp"     // for enable_if_is_same

namespace libsemigroups {

  //! \defgroup tlcode_group Partitioned binary relations (TLCODEs)
  //!
  //! Defined `tlcode.hpp`.
  //!
  //! This page contains an overview of the functionality in `libsemigroups`
  //! for partitioned binary relations (TLCODEs).
  //!
  //! TLCODEs are a generalisation of bipartitions, which were introduced by
  //! Martin and Mazorchuk in \cite Martin2011aa.
  //!
  //! Helper functions for TLCODEs are documented at
  //! \ref libsemigroups::tlcode "Helper functions for TLCODEs"
  //!
  //! @{

  //! \brief Class for representing TLCodes.
  //!
  //! Defined in `tlcode.hpp`.
  //!
  //! *Partitioned binary relations* (TLCodes) are a generalisation of a
  //! bipartitions, and were introduced by Martin and Mazorchuk in
  //! \cite Martin2011aa.
  //!
  //! \sa tlcode::throw_if_invalid(TLCode const&).
  class TLCode {
   public:
    //! \brief Type of constructor argument.
    //!
    //! Type of constructor argument.
    template <typename T>
    using vector_type = std::vector<std::vector<T>> const&;

    //! \brief Type of constructor argument.
    //!
    //! Type of constructor argument.
    template <typename T>
    using initializer_list_type = std::initializer_list<std::vector<T>> const&;

    //! \brief Deleted.
    //!
    //! Deleted. To avoid the situation where the underlying container is not
    //! defined, it is not possible to default construct a TLCode object.
    TLCode() = delete;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    TLCode(TLCode const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    TLCode(TLCode&&) = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    TLCode& operator=(TLCode const&) = default;

    //! \brief Default move assignment operator.
    //!
    TLCode& operator=(TLCode&&) = default;

    //! \brief Construct from adjacencies \c 0 to `2n - 1`.
    //!
    //! Construct from adjacencies \c 0 to `2n - 1`.
    //!
    //! The parameter \p x must be a container of vectors of \c uint32_t with
    //! size \f$2n\f$ for some integer \f$n\f$, and the vector in position
    //! \f$i\f$ is the list of points adjacent to \f$i\f$ in the TLCode
    //! constructed.
    //!
    //! \param  x the container of vectors of adjacencies.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks whatsoever on the validity of \p x are performed.
    //!
    //! \sa \ref tlcode::throw_if_invalid(TLCode const&)
    // explicit TLCode(vector_type<uint32_t> x);

    //! \copydoc TLCode(vector_type<uint32_t>)
    // explicit TLCode(initializer_list_type<uint32_t> x);

    explicit TLCode(std::vector<uint32_t> x);
    //! \brief Construct empty TLCode of given \ref degree.
    //!
    //! Construct empty TLCode of given \ref degree.
    //!
    //! \param n the degree.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    explicit TLCode(size_t n);

    //! \copydoc TLCode(vector_type<int32_t>,vector_type<int32_t>)
    TLCode(initializer_list_type<int32_t> left,
        initializer_list_type<int32_t> right);

    //! \brief Construct from adjacencies \c 1 to \c n and \c -1 to \c -n.
    //!
    //! Construct from adjacencies \c 1 to \c n and \c -1 to \c -n.
    //!
    //! The parameters \p left and \p right should be containers of
    //! \f$n\f$ vectors of integer values, so that
    //! the vector in position \f$i\f$ of \p left is the list of points
    //! adjacent to \f$i\f$ in the TLCode, and the vector in position \f$i\f$
    //! of \p right is the list of points adjacent to \f$n + i\f$ in the TLCode.
    //! A negative value \f$i\f$ corresponds to \f$n - i\f$.
    //!
    //! \param left container of adjacencies of \c 1 to \c n.
    //! \param right container of adjacencies of \c n + 1 to \c 2n.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks whatsoever on the validity of \p left or \p right are
    //! performed.
    //!
    //! \sa \ref tlcode::throw_if_invalid(TLCode const&) and
    //! \ref make(initializer_list_type<int32_t>,
    //! initializer_list_type<int32_t>)
    TLCode(vector_type<int32_t> left, vector_type<int32_t> right);

    //! \brief Returns the degree of a TLCode.
    //!
    //! Returns the degree of a TLCode, where the *degree* of a TLCode is half the
    //! number of points in the TLCode.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t degree() const noexcept { return _code.size(); }

    //! \brief Returns the number of points of a TLCode.
    //!
    //! Returns the number of points of a TLCode.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t number_of_points() const noexcept;

    //! \brief Multiply two TLCode objects and store the product in \c this.
    //!
    //! Replaces the contents of \c this by the product of \p x and \p y.
    //!
    //! The parameter \p thread_id is required since some temporary storage is
    //! required to find the product of \p x and \p y.  Note that if different
    //! threads call this member function with the same value of \p thread_id
    //! then bad things will happen.
    //!
    //! \param x a TLCode.
    //! \param y a TLCode.
    //! \param thread_id the index of the calling thread (defaults to \c 0).
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks are made on whether or not the parameters are compatible. If
    //! \p x and \p y have different degrees, then bad things will happen.
    void product_inplace_no_checks(TLCode const& x,
                                   TLCode const& y,
                                   size_t     thread_id = 0);

    //! \brief Multiply two TLCode objects and store the product in \c this.
    //!
    //! Replaces the contents of \c this by the product of \p x and \p y.
    //!
    //! The parameter \p thread_id is required since some temporary storage is
    //! required to find the product of \p x and \p y.  Note that if different
    //! threads call this member function with the same value of \p thread_id
    //! then bad things will happen.
    //!
    //! \param x a TLCode.
    //! \param y a TLCode.
    //! \param thread_id the index of the calling thread (defaults to \c 0).
    //!
    //! \throws LibsemigroupsException if:
    //!   * the \ref degree() of \p x is not the same as the \ref degree() of
    //!   \p y;
    //!   * the \ref degree() of `*this` is not the same as the \ref degree() of
    //!   \p x;
    //!   * one if the addresses `&x` and `&y` is the same as that of `this`; or
    //!   * either \p x or \p y are invalid.
    //!
    //! \sa
    //! \ref tlcode::throw_if_invalid.
    void product_inplace(TLCode const& x, TLCode const& y, size_t thread_id = 0);

    //! \brief Compare two TLCodes for equality.
    //!
    //! Compare two TLCodes for equality.
    //!
    //! \param that a TLCode to compare with.
    //!
    //! \returns \c true if \c this equals \p that, and \c false otherwise.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst linear in degree().
    // TODO(later): a better explanation of what equality means for TLCodes
    bool operator==(TLCode const& that) const {
      return _code == that._code;
    }

    //! \brief Compare for less.
    //!
    //! Compare for less.
    //!
    //! \param that a TLCode to compare with.
    //!
    //! \returns \c true if \c this is less than \p that, and \c false
    //! otherwise.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst linear in degree().
    bool operator<(TLCode const& that) const {
      return _code < that._code;
    }

    //! \brief Returns a hash value for a TLCode.
    //!
    //! Returns a hash value for a TLCode.
    //!
    //!
    //! \returns A hash value for a \c this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in `degree()`.
    //!
    //! \note
    //! This value is recomputed every time this function is called.
    // not noexcept because Hash<T>::operator() isn't
    size_t hash_value() const {
      return Hash<std::vector<uint32_t>>()(_code);
    }

    //! \brief Throws if a TLCode has a point related to a point that is greater
    //! than degree().
    //!
    //! This function throws a LibsemigroupsException if the argument \p x has a
    //! point related to a point that is greater than \ref TLCode::degree().
    //!
    //! \param x the TLCode to check.
    //!
    //! \throws LibsemigroupsException if \p x has a point related to a point
    //! that is greater than degree().
    //!
    //! \complexity
    //! Linear in the TLCode::degree of \p x.
    void throw_if_entry_out_of_bounds() const;

    //! \brief action of the i-th generator in *this.
    void product_by_generator_inplace_no_checks(uint32_t t);
  private:
    //! \brief action of the i-th generator in *this[0 .. pos].
    void product_by_generator_inplace_no_checks(uint32_t t, size_t pos);
  public:
    void product_inplace_no_checks(TLCode const& x);
    void product_inplace(TLCode const& x);

    //! \brief Return the max v for wich `this[0..pos-1, v]` is still a TL code
    uint32_t TL_max(size_t pos) const;

    //! \brief Insertion operator.
    //!
    //! This member function allows TLCode objects to be inserted into a
    //! std::ostringstream.
    friend std::ostringstream& operator<<(std::ostringstream&, TLCode const&);

    //! \brief Insertion operator.
    //!
    //! This member function allows TLCode objects to be inserted into a
    //! std::ostream.
    friend std::ostream& operator<<(std::ostream&, TLCode const&);

   private:
    std::vector<uint32_t> _code;
  };

  //! \brief Namespace for TLCode helper functions.
  //!
  //! This namespace contains helper functions for the TLCode class.
  namespace tlcode {
    //! \brief Returns the identity TLCode with specified degree.
    //!
    //! This function returns a new TLCode with degree equal to \p n where every
    //! value is adjacent to its negative. Equivalently, \f$i\f$ is adjacent
    //! \f$i + n\f$ and vice versa for every \f$i\f$ less than the degree
    //! \f$n\f$.
    //!
    //! \param n the degree.
    //!
    //! \returns
    //! A TLCode.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa
    //! \ref one(TLCode const&)
    TLCode one(size_t n);

    //! \brief Returns the identity TLCode with degree `x.degree()`.
    //!
    //! This member function returns a new \ref TLCode with degree equal to the
    //! degree of \p x, where every value is adjacent to its negative.
    //! Equivalently, \f$i\f$ is adjacent \f$i + n\f$ and vice versa for every
    //! \f$i\f$ less than the degree \f$n\f$.
    //!
    //! \param x A TLCode.
    //!
    //! \returns
    //! A TLCode.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa
    //! \ref one(size_t)
    TLCode one(TLCode const& x);

    //! \brief Throws if a TLCode is invalid.
    //!
    //! This function throws a LibsemigroupsException if the argument \p x is
    //! not a valid TLCode.
    //!
    //! \param x the TLCode to check.
    //!
    //! \throws LibsemigroupsException if any of the following occur:
    //! * \p x does not describe a binary relation on an even number of points;
    //! * \p x has a point related to a point that is greater than degree();
    //! * a list of points related to a point is not sorted.
    //!
    //! \complexity
    //! Linear in the TLCode::degree x.
    //!
    //! \sa
    //! * \ref throw_if_entry_out_of_bounds(TLCode const&);
    void inline throw_if_invalid(TLCode const& x) {
      x.throw_if_entry_out_of_bounds();
    }

  }  // namespace tlcode

  // end tlcode_group
  //! @}

  namespace detail {
    std::vector<std::vector<uint32_t>>
    process_left_right(TLCode::vector_type<int32_t> left,
                       TLCode::vector_type<int32_t> right);
  }

  //! \relates TLCode
  //!
  //! \brief Construct and check a \ref TLCode.
  //!
  //! Construct and check a \ref TLCode.
  //!
  //! \tparam Return the return type. Must satisfy `std::is_same<Return, TLCode>`.
  //! \tparam T the types of the arguments.
  //!
  //! \param args the arguments to forward to the \ref TLCode constructor.
  //!
  //! \returns
  //! A TLCode constructed from \p args and checked.
  //!
  //! \throws LibsemigroupsException if libsemigroups::throw_if_invalid(TLCode
  //! const&) throws when called with the constructed TLCode.
  //!
  //! \warning
  //! No checks are performed on the validity of \p args prior to the
  //! construction of the TLCode object.
  //!
  //! \sa
  //! #TLCode.
  template <typename Return, typename... T>
  [[nodiscard]] enable_if_is_same<Return, TLCode> make(T... args) {
    // TODO(later) throw_if_bad_args
    TLCode result(std::forward<T>(args)...);
    tlcode::throw_if_invalid(result);
    return result;
  }

  //! \defgroup make_tlcode_group make<TLCode>
  //! \ingroup tlcode_group
  //!
  //! \brief Safely construct a \ref TLCode instance.
  //!
  //! This page contains documentation related to safely constructing a
  //! \ref TLCode instance.
  //!
  //! \sa \ref make_group for an overview of possible uses of the `make`
  //! function.

  //! \ingroup make_tlcode_group
  //!
  //! \brief Construct and check a \ref TLCode.
  //!
  //! Construct and check a \ref TLCode.
  //!
  //! \tparam Return the return type. Must satisfy `std::is_same<Return, TLCode>`.
  //!
  //! \param args the arguments to forward to the constructor.
  //!
  //! \returns
  //! A TLCode constructed from \p args and checked.
  //!
  //! \throws LibsemigroupsException if libsemigroups::throw_if_invalid(TLCode
  //! const&) throws when called with the constructed TLCode.
  //!
  //! \warning
  //! No checks are performed on the validity of \p args prior to the
  //! construction of the TLCode object.
  //!
  //! \sa
  //! TLCode(initializer_list_type<uint32_t>).
  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, TLCode>
  make(TLCode::initializer_list_type<uint32_t> args) {
    return make<TLCode, decltype(args)>(args);
  }

  //! \ingroup make_tlcode_group
  //!
  //! \copydoc make(TLCode::vector_type<int32_t>, TLCode::vector_type<int32_t>)
  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, TLCode>
  make(TLCode::initializer_list_type<int32_t> left,
       TLCode::initializer_list_type<int32_t> right) {
    return TLCode(detail::process_left_right(left, right));
  }

  //! \ingroup make_tlcode_group
  //!
  //! \brief Construct and check a \ref TLCode.
  //!
  //! Construct and check a \ref TLCode.
  //!
  //! \tparam Return the return type. Must satisfy `std::is_same<Return, TLCode>`.
  //!
  //! \param left the 1st argument to forward to the constructor.
  //! \param right the 2nd argument to forward to the constructor.
  //!
  //! \returns
  //! A TLCode constructed from \p args and checked.
  //!
  //! \throws LibsemigroupsException if libsemigroups::throw_if_invalid(TLCode
  //! const&) throws when called with the constructed TLCode.
  //!
  //! \warning
  //! No checks are performed on the validity of \p left or \p right prior to
  //! the construction of the TLCode object.
  //!
  //! \sa
  //! TLCode(initializer_list_type<int32_t>, initializer_list_type<int32_t>).
  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, TLCode>
  make(TLCode::vector_type<int32_t> left, TLCode::vector_type<int32_t> right) {
    return TLCode(detail::process_left_right(left, right));
  }

  //! \relates TLCode
  //!
  //! \brief Return a human readable representation of a TLCode.
  //!
  //! Return a human readable representation of a TLCode.
  //!
  //! \param x the TLCode object.
  //!
  //! \returns
  //! A value of type std::string.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  [[nodiscard]] std::string to_human_readable_repr(TLCode const& x);

  //! \relates TLCode
  //!
  //! \brief Multiply two TLCodes.
  //!
  //! Returns a newly constructed TLCode equal to the product of \p x and \p y.
  //!
  //! \param x a TLCode.
  //! \param y a TLCode.
  //!
  //! \returns
  //! A value of type \ref TLCode
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! Cubic in degree().
  TLCode operator*(TLCode const& x, TLCode const& y);

  //! \relates TLCode
  //!
  //! \brief Compare two TLCodes for inequality.
  //!
  //! Compare two TLCodes for inequality.
  //!
  //! \param x a TLCode.
  //! \param y a TLCode.
  //!
  //! \returns
  //! \returns \c true if \c this is not equal to \p that, and \c false
  //! otherwise.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst quadratic in the degree of \p x and \p y.
  inline bool operator!=(TLCode const& x, TLCode const& y) {
    return !(x == y);
  }

  //! \relates TLCode
  //!
  //! \brief Convenience function that just calls \ref TLCode::operator<
  //! "operator<".
  //!
  //! Convenience function that just calls \ref TLCode::operator< "operator<".
  inline bool operator>(TLCode const& x, TLCode const& y) {
    return y < x;
  }

  //! \relates TLCode
  //!
  //! \brief Convenience function that just calls \ref TLCode::operator<
  //! "operator<" and \ref TLCode::operator== "operator==".
  //!
  //! Convenience function that just calls \ref TLCode::operator< "operator<" and
  //! \ref TLCode::operator== "operator==".
  inline bool operator<=(TLCode const& x, TLCode const& y) {
    return x < y || x == y;
  }

  //! \relates TLCode
  //!
  //! \brief Convenience function that just calls \ref operator<=(TLCode const&,
  //! TLCode const&) "operator<=".
  //!
  //! Convenience function that just calls \ref operator<=(TLCode const&,
  //! TLCode const&) "operator<=".
  inline bool operator>=(TLCode const& x, TLCode const& y) {
    return y <= x;
  }

  namespace detail {

    template <typename T>
    struct IsTLCodeHelper : std::false_type {};

    template <>
    struct IsTLCodeHelper<TLCode> : std::true_type {};

  }  // namespace detail

  //! \brief Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p T is
  //! \ref TLCode.
  //!
  //! \tparam T a type.
  template <typename T>
  static constexpr bool IsTLCode = detail::IsTLCodeHelper<T>::value;

  ////////////////////////////////////////////////////////////////////////
  // Adapters
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup adapters_tlcode_group Adapters for TLCode
  //!
  //! This page contains links to the specific specialisations for some of the
  //! adapters on \ref adapters_group "this page" for the \ref TLCode class.
  //!
  //! @{

  //! \brief Specialization of the adapter Complexity for instances of TLCode.
  //!
  //! Specialization of the adapter Complexity for instances of TLCode.
  //!
  //! \sa
  //! \ref Complexity.
  template <>
  struct Complexity<TLCode> {
    //! \brief Returns the approximate time complexity of multiplying TLCodes.
    //!
    //! Returns the approximate time complexity of multiplying TLCodes, which is
    //! \f$2n ^ 3\f$ where \f$n\f$ is the degree.
    //!
    //! \param x a TLCode.
    //!
    //! \returns
    //! A value of type `size_t` representing the complexity of multiplying the
    //! parameter \p x by another TLCode of the same degree.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t operator()(TLCode const& x) const noexcept {
      return 8 * x.degree() * x.degree() * x.degree();
    }
  };

  //! \brief Specialization of the adapter Degree for instances of TLCode.
  //!
  //! Specialization of the adapter Degree for instances of TLCode.
  //!
  //! \sa
  //! \ref Degree.
  template <>
  struct Degree<TLCode> {
    //! \brief Returns the degree of \p x.
    //!
    //! Returns the degree of \p x.
    //!
    //! \param x a TLCode.
    //!
    //! \returns
    //! A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! \ref TLCode::degree
    size_t operator()(TLCode const& x) const noexcept {
      return x.degree();
    }
  };

  //! \brief Specialization of the adapter Hash for instances of TLCode.
  //!
  //! Specialization of the adapter Hash for instances of TLCode.
  //!
  //! \sa
  //! \ref Hash.
  template <>
  struct Hash<TLCode> {
    //! \brief Returns a hash value for \p x.
    //!
    //! Returns a hash value for \p x.
    //!
    //! \param x a TLCode.
    //!
    //! \returns
    //! A value of `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in `degree()`.
    //!
    //! \sa
    //! \ref TLCode::hash_value
    size_t operator()(TLCode const& x) const {
      return x.hash_value();
    }
  };

  //! \brief Specialization of the adapter One for instances of TLCode.
  //!
  //! Specialization of the adapter One for instances of TLCode.
  //!
  //! \sa
  //! \ref One.
  template <>
  struct One<TLCode> {
    //! \brief Returns the identity TLCode with degree `x.degree()`.
    //!
    //! This member function returns a new \ref TLCode with degree equal to the
    //! degree of \p x, where every value is adjacent to its negative.
    //! Equivalently, \f$i\f$ is adjacent \f$i + n\f$ and vice versa for every
    //! \f$i\f$ less than the degree \f$n\f$.
    //!
    //! \returns
    //! A TLCode.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa
    //! \ref tlcode::one
    TLCode operator()(TLCode const& x) const {
      return tlcode::one(x);
    }

    //! \brief Returns the identity TLCode with specified degree.
    //!
    //! This function returns a new TLCode with degree equal to \p N where every
    //! value is adjacent to its negative. Equivalently, \f$i\f$ is adjacent
    //! \f$i + N\f$ and vice versa for every \f$i\f$ less than the degree
    //! \f$N\f$.
    //!
    //! \param N the degree.
    //!
    //! \returns
    //! A TLCode.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa
    //! \ref tlcode::one
    TLCode operator()(size_t N = 0) const {
      return tlcode::one(N);
    }
  };

  //! \brief Specialization of the adapter Product for instances of TLCode.
  //!
  //! Specialization of the adapter Product for instances of TLCode.
  //!
  //! \sa
  //! \ref Product.
  template <>
  struct Product<TLCode> {
    //! \brief Multiply two TLCode objects and store the product in a third.
    //!
    //! Replaces the contents of \p xy by the product of \p x and \p y.
    //!
    //! The parameter \p thread_id is required since some temporary storage is
    //! required to find the product of \p x and \p y.  Note that if different
    //! threads call this member function with the same value of \p thread_id
    //! then bad things will happen.
    //!
    //! \param xy a TLCode whose contents (if any) will be cleared.
    //! \param x a TLCode.
    //! \param y a TLCode.
    //! \param thread_id the index of the calling thread (defaults to \c 0).
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks are made on whether or not the parameters are compatible. If
    //! \p x and \p y have different degrees, then bad things will happen.
    //!
    //! \sa
    //! \ref TLCode::product_inplace_no_checks
    void operator()(TLCode& xy, TLCode const& x, TLCode const& y, size_t thread_id = 0) {
      xy.product_inplace_no_checks(x, y, thread_id);
    }
  };

  //! \brief Specialization of the adapter IncreaseDegree for instances of TLCode.
  //!
  //! Specialization of the adapter IncreaseDegree for instances of TLCode.
  //!
  //! \sa
  //! \ref IncreaseDegree.
  template <>
  struct IncreaseDegree<TLCode> {
    //! \brief Do nothing.
    void operator()(TLCode&, size_t) {}
  };

  // end adapters_tlcode_group
  //! @}

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TLCODE_HPP_
