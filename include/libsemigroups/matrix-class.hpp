//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2026 James D. Mitchell
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

// TODO(1) there're no complete set of init methods for matrices

#ifndef LIBSEMIGROUPS_MATRIX_CLASS_HPP_
#define LIBSEMIGROUPS_MATRIX_CLASS_HPP_

#include <algorithm>         // for min
#include <array>             // for array
#include <bitset>            // for bitset
#include <cstddef>           // for size_t
#include <cstdint>           // for uint64_t
#include <initializer_list>  // for initializer_list
#include <iosfwd>            // for ostringstream
#include <iterator>          // for distance
#include <numeric>           // for inner_product
#include <ostream>           // for operator<<, basic_ostream
#include <string>            // for string
#include <tuple>             // for tie
#include <type_traits>       // for false_type, is_signed, true_type
#include <unordered_map>     // for unordered_map
#include <unordered_set>     // for unordered_set
#include <utility>           // for forward, make_pair, pair
#include <vector>            // for vector

#include "adapters.hpp"   // for Degree
#include "bitset.hpp"     // for BitSet
#include "config.hpp"     // for LIBSEMIGROUPS_PARSED_BY_DOXYGEN
#include "constants.hpp"  // for POSITIVE_INFINITY
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "is-matrix.hpp"  // for IsMatrix

#include "detail/containers.hpp"     // for StaticVector1
#include "detail/formatters.hpp"     // for formatter of POSITIVE_INFINITY ...
#include "detail/matrix-common.hpp"  // for entry_repr
#include "detail/matrix-exceptions.hpp"  // for throw_if...
#include "detail/string.hpp"             // for detail::to_string

namespace libsemigroups {

  //! \defgroup matrix_group Matrices
  //!
  //! This page contains links to the documentation of the functionality in
  //! libsemigroups for matrices.
  //!
  //! There are several different classes in `libsemigroups` for representing
  //! matrices over various semirings. There are up to three different
  //! representations for every type of matrix:
  //!
  //! 1. those whose dimension and arithmetic operations can be defined at
  //!    compile time: \ref StaticMatrix;
  //! 2. those whose arithmetic operation can be defined at compile time but
  //!    whose dimensions can be set at run time:
  //!    \ref DynamicMatrixStaticArith
  //!    "DynamicMatrix (with compile-time arithmetic)";
  //! 3. those whose arithmetic operations and dimensions can be set at run
  //!    time: \ref DynamicMatrixDynamicArith
  //!    "DynamicMatrix (with run-time arithmetic)".
  //!
  //! It's unlikely that you will want to use the classes described on this
  //! page directly, but rather through the aliases described on the other
  //! matrix pages (such as, for example, \ref BMat).
  //!
  //! \warning All of the matrices in `libsemigroups` have entries that are
  //! integer types, and the code does not currently attempt to detect
  //! overflows.
  //!
  //! ## Over specific semirings
  //!
  //! The following matrix classes are provided which define matrices over some
  //! specific semirings:
  //!
  //! * \ref bmat_index_group
  //! * \ref intmat_group
  //! * \ref maxplusmat_group
  //! * \ref minplusmat_group
  //! * \ref maxplustruncmat_group
  //! * \ref minplustruncmat_group
  //! * \ref ntpmat_group
  //! * \ref projmaxplus_group
  //!
  //! ## Over arbitrary semirings
  //!
  //! The following general matrix classes are provided which can be used to
  //! define matrices over arbitrary semirings:
  //!
  //! * \ref DynamicMatrixStaticArith
  //! "DynamicMatrix (with compile-time arithmetic)"
  //! * \ref DynamicMatrixDynamicArith
  //! "DynamicMatrix (with run-time arithmetic)"
  //! * \ref StaticMatrix
  //!
  //! ## Row views
  //!
  //! A row view is a lightweight representation of a row of a matrix. The
  //! following row view classes are provided:
  //!
  //! * \ref DynamicRowViewStaticArith
  //! "DynamicRowView (compile-time arithmetic)
  //! * \ref DynamicRowViewDynamicArith
  //! "DynamicRowView (run-time arithmetic)
  //! * \ref StaticRowView
  //!
  //! ## Adapters
  //!
  //! There are various specialisations of the adapters described on
  //! \ref adapters_group "this page" for the matrices described on this page:
  //!
  //! * \ref adapters_matrix_group
  //! * \ref adapters_bmat_group
  //! * \ref adapters_bmat8_group

  ////////////////////////////////////////////////////////////////////////
  // Matrix forward declarations
  ////////////////////////////////////////////////////////////////////////

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            size_t R,
            size_t C,
            typename Scalar>
  class StaticMatrix;

  template <typename... Args>
  class DynamicMatrix;

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            typename Scalar>
  class DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>;

  template <typename Semiring, typename Scalar>
  class DynamicMatrix<Semiring, Scalar>;
#endif

  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
  // StaticRowViews - static arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! \brief Class for views into a row of a matrix over a semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a class for views into a row of a matrix over a semiring. An
  //! instance of this class provides access to the elements in a row of a
  //! matrix and is cheap to create and copy. Addition, scalar multiplication,
  //! and other standard vector operations are defined for row views.
  //!
  //! This class is the type of row views into a StaticMatrix; see
  //! the documentation for StaticMatrix for further details.
  //!
  //! \tparam PlusOp a stateless type with a call operator of signature
  //! `scalar_type operator()(scalar_type, scalar_type)` implementing the
  //! addition of the semiring.
  //!
  //! \tparam ProdOp a stateless type with a call operator of signature
  //! `scalar_type operator()(scalar_type, scalar_type)` implementing the
  //! multiplication of the semiring.
  //!
  //! \tparam ZeroOp a stateless type with a call operator of signature
  //! `scalar_type operator()()` returning the zero of the semiring (the
  //! additive identity element).
  //!
  //! \tparam OneOp a stateless type with a call operator of signature
  //! `scalar_type operator()()` returning the one of the semiring (the
  //! multiplicative identity element).
  //!
  //! \tparam C  the number of columns of the underlying matrix.
  //!
  //! \tparam Scalar the type of the entries in the matrices (the type of
  //! elements in the underlying semiring).
  //!
  //! \warning
  //! If the underlying matrix is destroyed, then any row views for that
  //! matrix are invalidated.
  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            size_t C,
            typename Scalar>
  class StaticRowView
      : public detail::RowViewCommon<
            StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, 1, C, Scalar>,
            StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>>,
        public detail::
            MatrixStaticArithmetic<PlusOp, ProdOp, ZeroOp, OneOp, Scalar> {
   private:
    using RowViewCommon = detail::RowViewCommon<
        StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, 1, C, Scalar>,
        StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>>;
    friend RowViewCommon;

    template <size_t R>
    using StaticMatrix_ = ::libsemigroups::
        StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>;

   public:
    //! \copybrief StaticMatrix::const_iterator
    using const_iterator = typename RowViewCommon::const_iterator;

    //! \copybrief StaticMatrix::const_iterator
    using iterator = typename RowViewCommon::iterator;

    //! \copybrief StaticMatrix::scalar_type
    using scalar_type = Scalar;

    //! \copybrief StaticMatrix::scalar_reference
    using scalar_reference = typename RowViewCommon::scalar_reference;

    //! \copybrief StaticMatrix::scalar_const_reference
    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    using scalar_const_reference = typename RowViewCommon::scalar_const_reference;
    // clang-format on

    //! \brief Alias for the type of the underlying matrix.
    using matrix_type = typename RowViewCommon::matrix_type;

    //! \brief Alias for the type of a row in the underlying matrix.
    using Row = typename matrix_type::Row;

    //! \copydoc StaticMatrix::StaticMatrix()
    StaticRowView() = default;

    //! \copydoc StaticMatrix::StaticMatrix(StaticMatrix const&)
    StaticRowView(StaticRowView const&) = default;

    //! \copydoc StaticMatrix::StaticMatrix(StaticMatrix&&)
    StaticRowView(StaticRowView&&) = default;

    //! \copydoc StaticMatrix::operator=(StaticMatrix const&)
    StaticRowView& operator=(StaticRowView const&) = default;

    //! \copydoc StaticMatrix::operator=(StaticMatrix&&)
    StaticRowView& operator=(StaticRowView&&) = default;

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    using RowViewCommon::RowViewCommon;

    // TODO(2) This constructor should be private
    template <size_t R>
    StaticRowView(StaticMatrix_<R> const*,
                  typename RowViewCommon::iterator it,
                  size_t)
        : RowViewCommon(it) {}

    using RowViewCommon::size;
#else
    //! \brief Construct a row view from a \ref Row.
    //!
    //! This function constructs a row view from a \ref Row.
    //!
    //! \param r  the row.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    explicit StaticRowView(Row const& r);

    //! \brief Returns the size of the row.
    //!
    //! Returns the size of the underlying row which equals the template
    //! parameter \p C.
    //!
    //! \returns  The size of the row.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    static constexpr size_t size() const noexcept;

    //! \brief Returns a iterator pointing at the first entry.
    //!
    //! This function returns a (random access) iterator pointing at the first
    //! entry in the row.
    //!
    //! \returns  A value of type \ref iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    iterator begin() noexcept;

    //! \brief Returns a iterator pointing one beyond the last entry of the
    //! row.
    //!
    //! This function returns a (random access) iterator pointing at one
    //! beyond the last entry in the row.
    //!
    //! \returns  A value of type \ref iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    iterator end();

    //! \brief Returns a const iterator pointing at the first entry.
    //!
    //! This function returns a const (random access) iterator pointing at the
    //! first entry in the row.
    //!
    //! \returns  A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    const_iterator cbegin() const noexcept;

    //! \brief Returns a const iterator pointing one beyond the last entry of
    //! the row.
    //!
    //! This function returns a (random access) const iterator pointing at one
    //! beyond the last entry in the row.
    //!
    //! \returns  A value of type \ref iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    iterator cend();

    //! \brief Returns a reference to the specified entry of the row.
    //!
    //! This function returns a reference to the specified entry of the
    //! row.
    //!
    //! \param i  the index of the entry.
    //!
    //! \returns  A value of type \ref scalar_reference.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning
    //! No checks on the validity of the parameter \p i is performed.
    scalar_reference operator()(size_t i);

    //! \brief Returns a const reference to the specified entry of the row.
    //!
    //! This function returns a const reference to the specified entry of the
    //! row.
    //!
    //! \param i  the index of the entry.
    //!
    //! \returns  A value of type \ref scalar_const_reference.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning
    //! No checks on the validity of the parameter \p i is performed.
    scalar_const_reference operator()(size_t i) const;

    //! \copydoc operator()(size_t)
    scalar_reference operator[](size_t i);

    //! \copydoc operator()(size_t) const
    scalar_const_reference operator[](size_t i) const;

    //! \brief Equality operator.
    //!
    //! Equality operator.
    //!
    //! \tparam U  either \ref Row, \ref StaticRowView,
    //! \ref DynamicRowViewStaticArith
    //! "DynamicRowView (compile-time arithmetic)", or
    //! \ref DynamicRowViewDynamicArith "DynamicRowView (run-time arithmetic)"
    //! \param that  \ref Row, \ref StaticRowView,
    //! \ref DynamicRowViewStaticArith
    //! "DynamicRowView (compile-time arithmetic)", or
    //! \ref DynamicRowViewDynamicArith "DynamicRowView (run-time arithmetic)"
    //! object for comparison.
    //!
    //! \returns
    //! Returns \c true if the first \ref size entries of \p that
    //! equal the entries of `this`, and \c false if not.
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ is the size of the row view.
    template <typename U>
    bool operator==(U const& that) const;

    //! \brief Inequality operator.
    //!
    //! Inequality operator.
    //!
    //! \tparam U  either \ref Row, \ref StaticRowView,
    //! \ref DynamicRowViewStaticArith
    //! "DynamicRowView (compile-time arithmetic)", or
    //! \ref DynamicRowViewDynamicArith "DynamicRowView (run-time arithmetic)"
    //! \param that  \ref Row, \ref StaticRowView,
    //! \ref DynamicRowViewStaticArith
    //! "DynamicRowView (compile-time arithmetic)", or
    //! \ref DynamicRowViewDynamicArith "DynamicRowView (run-time arithmetic)"
    //! object for comparison.
    //!
    //! \returns
    //! Returns \c true if the first \ref size entries of \p that
    //! do not equal the entries of `this`, and \c false otherwise.
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ is the size of the row view.
    template <typename U>
    bool operator!=(U const& that) const;

    //! \brief Lexicographical comparison of rows.
    //!
    //! Lexicographical comparison of rows.
    //!
    //! \tparam U  either \ref Row, \ref StaticRowView,
    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    //! \ref DynamicRowViewStaticArith "DynamicRowView (compile-time arithmetic)",
    //! or
    //! \ref DynamicRowViewDynamicArith "DynamicRowView (run-time arithmetic)".
    // clang-format on
    //!
    //! \param that \ref Row, \ref StaticRowView,
    //! \ref DynamicRowViewStaticArith "DynamicRowView (compile-time
    //! arithmetic)", or \ref DynamicRowViewDynamicArith "DynamicRowView
    //! (run-time arithmetic)" object for comparison.
    //!
    //! \returns
    //! Returns \c true if `*this` is lex less than that `that` and
    //! \c false otherwise.
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ is the size of the row view.
    template <typename U>
    bool operator<(U const& that) const;

    //! \brief Lexicographical comparison of rows.
    //!
    //! Lexicographical comparison of rows.
    //!
    //! \tparam U  either \ref Row, \ref StaticRowView,
    //! \ref DynamicRowViewStaticArith
    //! "DynamicRowView (compile-time arithmetic)", or
    //! \ref DynamicRowViewDynamicArith "DynamicRowView (run-time arithmetic)"
    //!
    //! \param
    //! that \ref Row, \ref StaticRowView, \ref DynamicRowViewStaticArith
    //! "DynamicRowView (compile-time arithmetic)", or
    //! \ref DynamicRowViewDynamicArith "DynamicRowView (run-time arithmetic)"
    //! object for comparison.
    //!
    //! \returns
    //! Returns \c true if `this` is lex greater than that `that` and
    //! \c false otherwise.
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ is the \ref size of the row view.
    template <typename U>
    bool operator<(U const& that) const;

    //! \brief Sum row views.
    //!
    //! This function sums a row view with another row view and returns a
    //! newly allocated \ref Row.
    //!
    //! \param that  the row view to add.
    //! \returns  A \ref Row containing the sum.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is \ref size.
    //!
    //! \warning
    //! The two row views must be of the same \ref size, although this is not
    //! verified.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    Row plus_no_checks(StaticRowView const& that) const;

    //! \brief Sum row views.
    //!
    //! This function sums a row view with another row view and returns a
    //! newly allocated \ref Row.
    //!
    //! \param that  the row view to add.
    //! \returns  A \ref Row containing the sum.
    //!
    //! \throws LibsemigroupsException if the RowView pointed at by \c this
    //! and \p that do not have the same \ref size.
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is \ref size.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    Row operator+(StaticRowView const& that);

    //! \brief Sums a row view with another row view in-place.
    //!
    //! This function redefines a RowView object to be the sum of itself and
    //! \p that.
    //!
    //! \param that  the row view to add.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is \ref size.
    //!
    //! \warning
    //! The two row views must be of the same size, although this is not
    //! verified by the implementation.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    void plus_inplace_no_checks(StaticRowView const& that);

    //! \brief Sums a row view with another row view in-place.
    //!
    //! This function redefines a RowView object to be the sum of itself and
    //! \p that.
    //!
    //! \param that  the row view to add.
    //!
    //! \throws LibsemigroupsException if the RowView pointed at by \c this
    //! and \p that do not have the same \ref size.
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is \ref size.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    void operator+=(StaticRowView const& that);

    //! \brief Adds a scalar to every entry of a row in-place.
    //!
    //! This function adds a scalar to every entry of the row in-place.
    //!
    //! \param a the scalar to add.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is \ref size.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    void operator+=(scalar_type a);

    //! \brief Multiplies every entry of the row by a scalar.
    //!
    //! This function multiplies every entry of the row by a scalar.
    //!
    //! \param a the scalar.
    //!
    //! \returns
    //! A value of type \ref Row containing the original row multiplied by the
    //! scalar \p a.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is \ref size.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    Row operator*(scalar_type a) const;

    //! \brief Multiplies every entry of the row by a scalar in-place.
    //!
    //! This function multiplies every entry of the row by a scalar in-place.
    //!
    //! \param a  the scalar to multiply by.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is \ref size.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    void operator*=(scalar_type a);
#endif

   private:
    static constexpr size_t length_impl() noexcept {
      return C;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // DynamicRowViews - static arithmetic
  ////////////////////////////////////////////////////////////////////////

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  // Doxygen needs to ignore this so that the actual implementation of
  // DynamicRowView gets documented.
  template <typename... Args>
  class DynamicRowView;
#endif

  //! \anchor DynamicRowViewStaticArith
  //!
  //! \brief Row view class for \ref DynamicMatrixStaticArith
  //! "DynamicMatrix (compile-time arithmetic)".
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This class is the type of row views into a \ref DynamicMatrixStaticArith
  //! "DynamicMatrix (with compile-time arithmetic)" with the same template
  //! parameters.
  //!
  //! An instance of this class provides access to the elements in a row of a
  //! matrix and is cheap to create and copy. Addition, scalar multiplication,
  //! and other usual vector operations are defined for row views.
  //!
  //! \tparam PlusOp a stateless type with a call operator of signature
  //! `scalar_type operator()(scalar_type, scalar_type)` implementing the
  //! addition of the semiring.
  //!
  //! \tparam ProdOp a stateless type with a call operator of signature
  //! `scalar_type operator()(scalar_type, scalar_type)` implementing the
  //! multiplication of the semiring.
  //!
  //! \tparam ZeroOp a stateless type with a call operator of signature
  //! `scalar_type operator()()` returning the zero of the semiring (the
  //! additive identity element).
  //!
  //! \tparam OneOp a stateless type with a call operator of signature
  //! `scalar_type operator()()` returning the one of the semiring (the
  //! multiplicative identity element).
  //!
  //! \tparam Scalar the type of the entries in the matrices (the type of
  //! elements in the underlying semiring).
  //!
  //! \warning
  //! If the underlying matrix is destroyed, then any row views for that
  //! matrix are invalidated.
  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            typename Scalar>
  class DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>
      : public detail::RowViewCommon<
            DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>,
            DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>>,
        public detail::
            MatrixStaticArithmetic<PlusOp, ProdOp, ZeroOp, OneOp, Scalar> {
   private:
    using DynamicMatrix_ = DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>;
    using RowViewCommon  = detail::RowViewCommon<
         DynamicMatrix_,
         DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>>;
    friend RowViewCommon;

   public:
    //! \copybrief StaticMatrix::const_iterator
    using const_iterator = typename RowViewCommon::const_iterator;

    //! \copybrief StaticMatrix::const_iterator
    using iterator = typename RowViewCommon::iterator;

    //! \copybrief StaticMatrix::scalar_type
    using scalar_type = Scalar;

    //! \copybrief StaticMatrix::scalar_reference
    using scalar_reference = typename RowViewCommon::scalar_reference;

    //! \copybrief StaticMatrix::scalar_const_reference
    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    using scalar_const_reference = typename RowViewCommon::scalar_const_reference;
    // clang-format on

    //! \brief Alias for the type of the underlying matrix.
    using matrix_type = typename RowViewCommon::matrix_type;

    //! \brief Alias for the type of a row in the underlying matrix.
    using Row = typename matrix_type::Row;

    //! \copydoc StaticMatrix::StaticMatrix()
    DynamicRowView() = default;

    //! \copydoc StaticMatrix::StaticMatrix(StaticMatrix const&)
    DynamicRowView(DynamicRowView const&) = default;

    //! \copydoc StaticMatrix::StaticMatrix(StaticMatrix&&)
    DynamicRowView(DynamicRowView&&) = default;

    //! \copydoc StaticMatrix::operator=(StaticMatrix const&)
    DynamicRowView& operator=(DynamicRowView const&) = default;

    //! \copydoc StaticMatrix::operator=(StaticMatrix&&)
    DynamicRowView& operator=(DynamicRowView&&) = default;

    //! \copydoc StaticRowView::StaticRowView(Row const&)
    explicit DynamicRowView(Row const& r)
        : RowViewCommon(r), _length(r.number_of_cols()) {}

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    using RowViewCommon::RowViewCommon;

    // TODO(2) This constructor should be private
    DynamicRowView(DynamicMatrix_ const*, iterator const& it, size_t N)
        : RowViewCommon(it), _length(N) {}

    using RowViewCommon::size;
#else
    //! \copydoc StaticRowView::size
    size_t size() const noexcept;

    //! \copydoc StaticRowView::begin
    iterator begin() noexcept;

    //! \copydoc StaticRowView::end
    iterator end();

    //! \copydoc StaticRowView::cbegin
    const_iterator cbegin() const noexcept;

    //! \copydoc StaticRowView::cend
    iterator cend();

    //! \copydoc StaticRowView::operator()(size_t)
    scalar_reference operator()(size_t i);

    //! \copydoc StaticRowView::operator()(size_t) const
    scalar_const_reference operator()(size_t i) const;

    //! \copydoc StaticRowView::operator[](size_t)
    scalar_reference operator[](size_t i);

    //! \copydoc StaticRowView::operator[](size_t) const
    scalar_const_reference operator[](size_t i) const;

    //! \copydoc StaticRowView::operator==
    template <typename U>
    bool operator==(U const& that) const;

    //! \copydoc StaticRowView::operator!=
    template <typename U>
    bool operator!=(U const& that) const;

    //! \copydoc StaticRowView::operator<
    template <typename U>
    bool operator<(U const& that) const;

    //! \copydoc StaticRowView::operator<
    template <typename U>
    bool operator<(U const& that) const;

    //! \copydoc StaticRowView::plus_no_checks
    Row plus_no_checks(DynamicRowView const& that);

    //! \copydoc StaticRowView::operator+
    Row operator+(DynamicRowView const& that);

    //! \copydoc StaticRowView::plus_inplace_no_checks(StaticRowView const&)
    void plus_inplace_no_checks(DynamicRowView const& that);

    //! \copydoc StaticRowView::operator+=(StaticRowView const&)
    void operator+=(DynamicRowView const& that);

    //! \copydoc StaticRowView::operator+=(scalar_type)
    void operator+=(scalar_type a);

    //! \copydoc StaticRowView::operator*(scalar_type) const
    Row operator*(scalar_type a) const;

    //! \copydoc StaticRowView::operator*=(scalar_type)
    void operator*=(scalar_type a);
#endif  // LIBSEMIGROUPS_PARSED_BY_DOXYGEN

   private:
    size_t length_impl() const noexcept {
      return _length;
    }
    size_t _length;
  };

  ////////////////////////////////////////////////////////////////////////
  // DynamicRowViews - dynamic arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! \anchor DynamicRowViewDynamicArith
  //!
  //! \brief Row view class for \ref DynamicMatrixDynamicArith
  //! "DynamicMatrix (run-time arithmetic)".
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This class is the type of row views into a
  //! \ref DynamicMatrixDynamicArith "DynamicMatrix (run-time arithmetic)".
  //!
  //! \tparam Semiring the type of a semiring object which defines the
  //! semiring arithmetic (see requirements in \ref DynamicMatrixDynamicArith
  //! "DynamicMatrix (run-time arithmetic)".
  //!
  //! \tparam Scalar the type of the entries in the matrices (the type of
  //! elements in the underlying semiring).
  //!
  //! \warning
  //! If the underlying matrix is destroyed, then any row views for that
  //! matrix are invalidated.
  template <typename Semiring, typename Scalar>
  class DynamicRowView<Semiring, Scalar>
      : public detail::RowViewCommon<DynamicMatrix<Semiring, Scalar>,
                                     DynamicRowView<Semiring, Scalar>> {
   private:
    using DynamicMatrix_ = DynamicMatrix<Semiring, Scalar>;
    friend DynamicMatrix_;
    using RowViewCommon
        = detail::RowViewCommon<DynamicMatrix_,
                                DynamicRowView<Semiring, Scalar>>;
    friend RowViewCommon;

   public:
    //! \copybrief StaticMatrix::const_iterator
    using const_iterator = typename RowViewCommon::const_iterator;

    //! \copybrief StaticMatrix::const_iterator
    using iterator = typename RowViewCommon::iterator;

    //! \copybrief StaticMatrix::scalar_type
    using scalar_type = Scalar;

    //! \copybrief StaticMatrix::scalar_reference
    using scalar_reference = typename RowViewCommon::scalar_reference;

    //! \copybrief StaticMatrix::scalar_const_reference
    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    using scalar_const_reference = typename RowViewCommon::scalar_const_reference;
    // clang-format on

    //! \brief Alias for the type of the underlying matrix.
    using matrix_type = typename RowViewCommon::matrix_type;

    //! \brief Alias for the type of a row in the underlying matrix.
    using Row = typename matrix_type::Row;

    //! \copydoc StaticMatrix::StaticMatrix()
    DynamicRowView() = default;

    //! \copydoc StaticMatrix::StaticMatrix(StaticMatrix const&)
    DynamicRowView(DynamicRowView const&) = default;

    //! \copydoc StaticMatrix::StaticMatrix(StaticMatrix&&)
    DynamicRowView(DynamicRowView&&) = default;

    //! \copydoc StaticMatrix::operator=(StaticMatrix const&)
    DynamicRowView& operator=(DynamicRowView const&) = default;

    //! \copydoc StaticMatrix::operator=(StaticMatrix&&)
    DynamicRowView& operator=(DynamicRowView&&) = default;

    //! \copydoc StaticRowView::StaticRowView(Row const&)
    explicit DynamicRowView(Row const& r) : RowViewCommon(r), _matrix(&r) {}

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    using RowViewCommon::RowViewCommon;

    // TODO(2) This constructor should be private
    DynamicRowView(DynamicMatrix_ const* mat, iterator const& it, size_t)
        : RowViewCommon(it), _matrix(mat) {}

    using RowViewCommon::size;
#else
    //! \copydoc StaticRowView::size
    size_t size() const noexcept;

    //! \copydoc StaticRowView::begin
    iterator begin() noexcept;

    //! \copydoc StaticRowView::end
    iterator end();

    //! \copydoc StaticRowView::cbegin
    const_iterator cbegin() const noexcept;

    //! \copydoc StaticRowView::cend
    iterator cend();

    //! \copydoc StaticRowView::operator()(size_t)
    scalar_reference operator()(size_t i);

    //! \copydoc StaticRowView::operator()(size_t) const
    scalar_const_reference operator()(size_t i) const;

    //! \copydoc StaticRowView::operator[](size_t)
    scalar_reference operator[](size_t i);

    //! \copydoc StaticRowView::operator[](size_t) const
    scalar_const_reference operator[](size_t i) const;

    //! \copydoc StaticRowView::operator==
    template <typename U>
    bool operator==(U const& that) const;

    //! \copydoc StaticRowView::operator!=
    template <typename U>
    bool operator!=(U const& that) const;

    //! \copydoc StaticRowView::operator<
    template <typename U>
    bool operator<(U const& that) const;

    //! \copydoc StaticRowView::operator<
    template <typename U>
    bool operator<(U const& that) const;

    //! \copydoc StaticRowView::plus_no_checks
    Row plus_no_checks(DynamicRowView const& that);

    //! \copydoc StaticRowView::operator+
    Row operator+(DynamicRowView const& that);

    //! \copydoc StaticRowView::plus_inplace_no_checks(StaticRowView const&)
    void plus_inplace_no_checks(DynamicRowView const& that);

    //! \copydoc StaticRowView::operator+=(StaticRowView const&)
    void operator+=(DynamicRowView const& that);

    //! \copydoc StaticRowView::operator+=(scalar_type)
    void operator+=(scalar_type a);

    //! \copydoc StaticRowView::operator*(scalar_type) const
    Row operator*(scalar_type a) const;

    //! \copydoc StaticRowView::operator*=(scalar_type)
    void operator*=(scalar_type a);
#endif

   private:
    size_t length_impl() const noexcept {
      return _matrix->number_of_cols();
    }

    scalar_type plus_no_checks_impl(scalar_type x,
                                    scalar_type y) const noexcept {
      return _matrix->plus_no_checks_impl(x, y);
    }

    scalar_type product_no_checks_impl(scalar_type x,
                                       scalar_type y) const noexcept {
      return _matrix->product_no_checks_impl(x, y);
    }

    DynamicMatrix_ const* _matrix;
  };

  ////////////////////////////////////////////////////////////////////////
  // StaticMatrix with compile time semiring arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! \brief Static matrix class.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a class for matrices where both the arithmetic operations in
  //! the underlying semiring, and the dimensions of the matrices are known
  //! at compile time.
  //!
  //! \tparam PlusOp a stateless type with a call operator of signature
  //!    `scalar_type operator()(scalar_type, scalar_type)`
  //!    implementing the addition of the semiring.
  //!
  //! \tparam ProdOp a stateless type with a call operator of signature
  //!    `scalar_type operator()(scalar_type, scalar_type)`
  //!    implementing the multiplication of the semiring.
  //!
  //! \tparam ZeroOp a stateless type with a call operator of signature
  //!    `scalar_type operator()()` returning the zero of the semiring
  //!    (the additive identity element).
  //!
  //! \tparam OneOp a stateless type with a call operator of signature
  //!    `scalar_type operator()()` returning the one of the semiring
  //!    (the multiplicative identity element).
  //!
  //! \tparam R  the number of rows of the matrix.
  //! \tparam C  the number of columns of the matrix.
  //!
  //! \tparam Scalar the type of the entries in the matrices (the type of
  //! elements in the underlying semiring).
  //!
  //! \note Certain member functions only work for square matrices and some
  //! only
  //!    work for rows.
  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            size_t R,
            size_t C,
            typename Scalar>
  class StaticMatrix
      : public detail::
            MatrixStaticArithmetic<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>,
        public detail::MatrixCommon<
            std::array<Scalar, R * C>,
            StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>,
            StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>> {
    ////////////////////////////////////////////////////////////////////////
    // StaticMatrix - Aliases - private
    ////////////////////////////////////////////////////////////////////////

    using MatrixCommon = ::libsemigroups::detail::MatrixCommon<
        std::array<Scalar, R * C>,
        StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>,
        StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>>;
    friend MatrixCommon;

   public:
    ////////////////////////////////////////////////////////////////////////
    // StaticMatrix - Aliases - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Alias for the template parameter \p Scalar.
    using scalar_type = typename MatrixCommon::scalar_type;

    //! \brief Alias for references to the template parameter \p Scalar.
    using scalar_reference = typename MatrixCommon::scalar_reference;

    //! \brief Alias for const references to the template parameter \p Scalar.
    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    using scalar_const_reference = typename MatrixCommon::scalar_const_reference;
    // clang-format on

    //! \brief Alias for the type of the rows of a StaticMatrix.
    using Row = StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, 1, C, Scalar>;

    //! \brief Alias for the type of row views of a StaticMatrix.
    using RowView = StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>;

    //! \brief Alias for the template parameter \p PlusOp.
    using Plus = PlusOp;

    //! \brief Alias for the template parameter \p ProdOp.
    using Prod = ProdOp;

    //! \brief Alias for the template parameter \p ZeroOp.
    using Zero = ZeroOp;

    //! \brief Alias for the template parameter \p OneOp.
    using One = OneOp;

    //! \brief Alias for iterators pointing at entries of a matrix.
    using iterator = typename MatrixCommon::iterator;

    //! \brief Alias for const iterators pointing at entries of a matrix.
    using const_iterator = typename MatrixCommon::const_iterator;

    static constexpr size_t nr_rows = R;
    static constexpr size_t nr_cols = C;

    ////////////////////////////////////////////////////////////////////////
    // StaticMatrix - Constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Construct a row (from std::initializer_list).
    //!
    //! Construct a row from an std::initializer_list.
    //!
    //! \param c  the values to be copied into the row.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //!   \f$O(n)\f$ where \f$n\f$ is the size of the row being
    //!   constructed.
    //!
    //! \warning
    //!   This constructor only works for rows, i.e. when the template
    //!   parameter \c R is \c 1.
    explicit StaticMatrix(std::initializer_list<scalar_type> const& c)
        : MatrixCommon(c) {
      static_assert(R == 1,
                    "cannot construct Matrix from the given initializer list, "
                    "incompatible dimensions");
      LIBSEMIGROUPS_ASSERT(c.size() == C);
    }

    //! \brief Construct a matrix.
    //!
    //! Construct a matrix from an std::initializer_list of
    //! std::initializer_list.
    //!
    //! \param m  the values to be copied into the matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //!   \f$O(mn)\f$ where \f$m\f$ is the template parameter \c R
    //!   and \f$n\f$ is the template parameter \c C.
    //!
    //! \par Example
    //! \code
    //! using Mat = BMat8;
    //!  Mat m({{1, 1}, {0, 0}});
    //! \endcode
    explicit StaticMatrix(
        std::initializer_list<std::initializer_list<scalar_type>> const& m)
        : MatrixCommon(m) {}

    //! \brief Construct a matrix.
    //!
    //! Construct a matrix from an std::vector of std::vector.
    //!
    //! \param m  the values to be copied into the matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //!   \f$O(mn)\f$ where \f$m\f$ is the template parameter \c R
    //!   and \f$n\f$ is the template parameter \c C.
    explicit StaticMatrix(std::vector<std::vector<scalar_type>> const& m)
        : MatrixCommon(m) {}

    //! \brief Construct a row from a row view.
    //!
    //! Construct a row from a row view.
    //!
    //! \param rv  the row view.
    //!
    //! \exceptions
    //!   this function guarantees not to throw a
    //!   LibsemigroupsException.
    //!
    //! \complexity
    //!   \f$O(n)\f$ where \f$n\f$ is the size of the row being
    //!   constructed.
    //!
    //! \warning
    //!   This constructor will only compile for rows, i.e. when the template
    //!   parameter \c R is \c 1.
    explicit StaticMatrix(RowView const& rv) : MatrixCommon(rv) {
      static_assert(
          R == 1,
          "cannot construct Matrix with more than one row from RowView!");
    }

    //! \brief Default constructor.
    //!
    //! Default constructor.
    StaticMatrix() = default;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    StaticMatrix(StaticMatrix const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    StaticMatrix(StaticMatrix&&) = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    StaticMatrix& operator=(StaticMatrix const&) = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    StaticMatrix& operator=(StaticMatrix&&) = default;

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    // For uniformity of interface, the args do nothing
    StaticMatrix(size_t r, size_t c) : StaticMatrix() {
      (void) r;
      (void) c;
      LIBSEMIGROUPS_ASSERT(r == number_of_rows());
      LIBSEMIGROUPS_ASSERT(c == number_of_cols());
    }

    // For uniformity of interface, the first arg does nothing
    StaticMatrix(void const* ptr, std::initializer_list<scalar_type> const& c)
        : StaticMatrix(c) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    // For uniformity of interface, the first arg does nothing
    StaticMatrix(
        void const*                                                      ptr,
        std::initializer_list<std::initializer_list<scalar_type>> const& m)
        : StaticMatrix(m) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    // For uniformity of interface, the first arg does nothing
    explicit StaticMatrix(void const* ptr, RowView const& rv)
        : StaticMatrix(rv) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    // For uniformity of interface, no arg used for anything
    StaticMatrix(void const* ptr, size_t r, size_t c) : StaticMatrix(r, c) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }
#endif

    ~StaticMatrix() = default;

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    static StaticMatrix one(size_t n) {
      // If specified the value of n must equal R or otherwise weirdness will
      // ensue...
      LIBSEMIGROUPS_ASSERT(n == 0 || n == R);
      (void) n;
#if defined(__APPLE__) && defined(__clang__) \
    && (__clang_major__ == 13 || __clang_major__ == 14)
      // With Apple clang version 13.1.6 (clang-1316.0.21.2.5) something goes
      // wrong and the value R is optimized away somehow, meaning that the
      // values on the diagonal aren't actually set. This only occurs when
      // libsemigroups is compiled with -O2 or higher.
      size_t volatile const m = R;
#else
      size_t const m = R;
#endif
      StaticMatrix x(m, m);
      std::fill(x.begin(), x.end(), ZeroOp()());
      for (size_t r = 0; r < m; ++r) {
        x(r, r) = OneOp()();
      }
      return x;
    }

    static StaticMatrix one(void const* ptr, size_t n = 0) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
      LIBSEMIGROUPS_ASSERT(n == 0 || n == R);
      return one(n);
    }
#endif  // LIBSEMIGROUPS_PARSED_BY_DOXYGEN

    ////////////////////////////////////////////////////////////////////////
    // StaticMatrix - member function aliases - public
    ////////////////////////////////////////////////////////////////////////
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    //! \brief Returns a reference to the specified entry of the matrix.
    //!
    //! Returns a reference to the specified entry of the matrix.
    //!
    //! \param r  the index of the row of the entry.
    //! \param c  the index of the column of the entry.
    //!
    //! \returns  A value of type \ref scalar_reference.
    //!
    //! \exceptions
    //!   this function guarantees not to throw a LibsemigroupsException.
    //!
    //! \complexity
    //!   Constant
    //!
    //! \warning
    //!    No checks on the validity of the parameters \c r and \c c are
    //!    performed.
    scalar_reference operator()(size_t r, size_t c);

    //! \brief Returns a reference to the specified entry of the matrix.
    //!
    //! Returns a reference to the specified entry of the matrix.
    //!
    //! \param r  the index of the row of the entry.
    //! \param c  the index of the column of the entry.
    //!
    //! \returns  A value of type \ref scalar_reference.
    //!
    //! \throws LibsemigroupsException if \p r or \p c is out of bounds.
    //!
    //! \complexity
    //!   Constant
    scalar_reference at(size_t r, size_t c);

    //! \brief Returns a const reference to the specified entry of the matrix.
    //!
    //! Returns a const reference to the specified entry of the matrix.
    //!
    //! \param r  the index of the row of the entry.
    //! \param c  the index of the column of the entry.
    //!
    //! \returns  A value of type \ref scalar_const_reference.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning
    //! No checks on the validity of the parameters \c r and \c c are
    //! performed.
    scalar_const_reference operator()(size_t r, size_t c) const;

    //! \brief Returns a const reference to the specified entry of the matrix.
    //!
    //! Returns a const reference to the specified entry of the matrix.
    //!
    //! \param r  the index of the row of the entry.
    //! \param c  the index of the column of the entry.
    //!
    //! \returns  A value of type \ref scalar_const_reference.
    //!
    //! \throws LibsemigroupsException if \p r or \p c is out of bounds.
    //!
    //! \complexity
    //! Constant
    scalar_const_reference at(size_t r, size_t c) const;

    //! \brief Returns an iterator pointing at the first entry.
    //!
    //! This function returns a random access iterator point at the first
    //! entry of the matrix.
    //!
    //! \returns A value of type \ref StaticMatrix::iterator "iterator".
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning
    //! The order in which entries in the matrix are iterated over is not
    //! specified.
    iterator begin() noexcept;

    //! \brief Returns an iterator pointing one beyond the last entry in the
    //! matrix.
    //!
    //! \returns A value of type \ref StaticMatrix::iterator "iterator".
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning
    //! The order in which entries in the matrix are iterated over is not
    //! specified.
    iterator end();

    //! \brief Returns a const iterator pointing at the first entry.
    //!
    //! This function returns a const (random access) iterator pointing at the
    //! first entry in the matrix.
    //!
    //! \returns  A value of type \ref StaticMatrix::const_iterator
    //! "const_iterator".
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning
    //!   The order in which entries in the matrix are iterated over is not
    //!   specified.
    const_iterator cbegin() const noexcept;

    //! \brief Returns a const iterator pointing one beyond the last entry in
    //! the matrix.
    //!
    //! This function returns a const (random access) iterator pointing one
    //! passed the last entry of the matrix.
    //!
    //! \returns  A value of type \ref StaticMatrix::const_iterator
    //! "const_iterator".
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning
    //! The order in which entries in the matrix are iterated over is not
    //! specified.
    const_iterator cend();

    //! \brief Equality operator.
    //!
    //! This function returns \c true if `*this` is equal to \p that;
    //! and \c false otherwise.
    //!
    //! \param that  matrix or row view for comparison.
    //!
    //! \returns \c true if `*this` and `that` are equal and \c false if they
    //! are not.
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ is the number of rows and
    //! \f$n\f$ is the number of columns of the matrix.
    bool operator==(StaticMatrix const& that) const;

    //! \copydoc operator==
    bool operator==(RowView const& that) const;

    //! \brief Inequality operator.
    //!
    //! This function returns \c true if `*this` is not equal to \p that; and
    //! \c false otherwise.
    //!
    //! \param that  the matrix or row view for comparison.
    //!
    //! \returns The negation of `operator==(that)`.
    //!
    //! \complexity
    //! see operator==
    bool operator!=(StaticMatrix const& that) const;

    //! \copydoc operator!=
    bool operator!=(RowView const& that) const;

    //! \brief Less than operator.
    //!
    //! This operator defines a total order on the set of matrices of the
    //! same type.
    //!
    //! \param that the matrix or row view for comparison.
    //!
    //! \returns \c true if `*this` is less than \p that  and \c false if it
    //! is not.
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$n\f$ is \ref number_of_cols.
    bool operator<(StaticMatrix const& that) const;

    //! \copydoc operator<
    bool operator<(RowView const& that) const;

    //! \brief Greater than operator.
    //!
    //! This operator defines a total order on the set of matrices of the same
    //! type.
    //!
    //! \param that  the matrix for comparison.
    //!
    //! \returns \c true if `*this` is less than `that` and \c false if it is
    //! not.
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$m\f$ is \ref number_of_cols
    bool operator>(StaticMatrix const& that) const;

    //! \brief Get the coordinates of an iterator.
    //!
    //! This function returns a pair containing the row and columns
    //! corresponding to a \ref StaticMatrix::const_iterator "const_iterator"
    //! pointing into a matrix.
    //!
    //! \param it the \ref StaticMatrix::const_iterator "const_iterator".
    //!
    //! \returns A value of type `std::pair<scalar_type, scalar_type>`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    std::pair<scalar_type, scalar_type> coords(const_iterator it) const;

    //! \brief Returns the number of rows of the matrix.
    //!
    //! This function returns the number of rows of the matrix.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    size_t number_of_rows() const noexcept;

    //! \brief Returns the number of columns of the matrix.
    //!
    //! This function returns the number of columns of the matrix.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    size_t number_of_cols() const noexcept;

    //! \brief Returns the sum of two matrices.
    //!
    //! This function returns the sum of two matrices.
    //!
    //! \param y the matrix to add to `this`.
    //!
    //! \returns The sum of the two matrices.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$m\f$ \ref number_of_cols
    //!
    //! \warning
    //! The matrices must be of the same dimensions, although this is not
    //! verified.
    //! \warning This function does not detect overflows of \ref scalar_type.
    StaticMatrix plus_no_checks(StaticMatrix const& y) const;

    //! \brief Returns the sum of two matrices.
    //!
    //! This function returns the sum of two matrices.
    //!
    //! \param that  the matrix to add to `this`.
    //!
    //! \returns The sum of the two matrices.
    //!
    //! \throws LibsemigroupsException if the matrix pointed at by \c this and
    //! \p that do not have the same dimensions.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$m\f$ \ref number_of_cols
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    StaticMatrix operator+(StaticMatrix const& that);

    //! \brief Add a matrix to another matrix in-place.
    //!
    //! This function adds a matrix to another matrix  (or the row represented
    //! by a RowView) of the same shape in-place.
    //!
    //! \param that  the matrix or row view to add.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$m\f$ is
    //! \ref number_of_cols
    //!
    //! \warning
    //! The matrices must be of the same dimensions, although this is not
    //! checked.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    void plus_inplace_no_checks(StaticMatrix const& that);

    //! \copydoc plus_inplace_no_checks
    void plus_inplace_no_checks(RowView const& that);

    //! \brief Add a matrix to another matrix in-place.
    //!
    //! This function adds a matrix to another matrix  (or the row represented
    //! by a RowView) of the same shape in-place.
    //!
    //! \param that  the matrix to add.
    //!
    //! \throws LibsemigroupsException if the matrix pointed at by \c this and
    //! \p that do not have the same dimensions.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$m\f$ is
    //! \ref number_of_cols
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    void operator+=(StaticMatrix const& that);

    //! \copydoc operator+=
    void operator+=(RowView const& that);

    //! \brief Adds a scalar to every entry of the matrix in-place.
    //!
    //! This function adds a scalar to every entry of the matrix in-place.
    //!
    //! \param a the \ref scalar_type to add to a \ref StaticMatrix.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$m\f$ is
    //! \ref number_of_cols
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    void operator+=(scalar_type a);

    //! \brief Returns the product of two matrices.
    //!
    //! This function returns the product of two matrices.
    //!
    //! \param that the matrix to multiply by `this`.
    //!
    //! \returns The product of the two matrices.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$m\f$ is
    //! \ref number_of_cols.
    //!
    //! \warning
    //! The matrices must be of the same dimensions, although this is not
    //! verified.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    StaticMatrix product_no_checks(StaticMatrix const& that);

    //! \brief Returns the product of two matrices.
    //!
    //! This function returns the product of two matrices.
    //!
    //! \param that the matrix to multiply by `this`.
    //!
    //! \returns The product of the two matrices.
    //!
    //! \throws LibsemigroupsException if either of the following apply:
    //! * `this` does not point at a square matrix;
    //! * \p that does not have the same dimensions as the matrix pointed at
    //!   by `this`.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$m\f$ is
    //! \ref number_of_cols.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    StaticMatrix operator*(StaticMatrix const& that);

    //! \brief Multiplies every entry of a matrix by a scalar in-place.
    //!
    //! This function multiplies every entry of a matrix by a scalar in-place.
    //!
    //! \param a the scalar to multiply every entry by.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$m\f$ is \ref number_of_cols.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    void operator*=(scalar_type a);

    //! \brief Multiplies \p x and \p y and stores the result in `this`.
    //!
    //! This function redefines `this` to be the product of \p x and \p y.
    //! This is in-place multiplication to avoid allocation of memory for
    //! products which do not need to be stored for future use.
    //!
    //! \param x  the first matrix to multiply.
    //! \param y  the second matrix to multiply.
    //!
    //! \complexity
    //! \f$O(n ^ 3)\f$ where \f$n\f$ is \ref number_of_rows or
    //! \ref number_of_cols.
    //!
    //! \warning
    //! The arguments \c this, \p x, and \p y must all be square matrices of
    //! the same dimension, but this isn't verified.
    void product_inplace_no_checks(StaticMatrix const& x,
                                   StaticMatrix const& y);

    //! \brief Multiplies \p x and \p y and stores the result in `this`.
    //!
    //! This function redefines `this` to be the product of \p x and \p y.
    //! This is in-place multiplication to avoid allocation of memory for
    //! products which do not need to be stored for future use.
    //!
    //! \param x  the first matrix to multiply.
    //! \param y  the second matrix to multiply.
    //!
    //! \throws LibsemigroupsException if either of the following apply:
    //! * `this` does not point at a square matrix;
    //! * \p x or \p y do not have the same dimensions as the matrix pointed at
    //!   by `this`.
    //!
    //! \complexity
    //! \f$O(n ^ 3)\f$ where \f$n\f$ is \ref number_of_rows or
    //! \ref number_of_cols.
    void product_inplace(StaticMatrix const& x, StaticMatrix const& y);

    //! \brief Returns a view into a row.
    //!
    //! This function returns a \ref RowView into the row with index \p i.
    //!
    //! \param i  the index of the row.
    //!
    //! \returns  A value of type \ref RowView.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning No checks are made on the argument \p i.
    RowView row_no_checks(size_t i) const;

    //! \brief Returns a view into a row.
    //!
    //! This function returns a \ref RowView into the row with index \p i.
    //!
    //! \param i the index of the row.
    //!
    //! \returns A value of type \ref RowView.
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal
    //! to \ref number_of_rows.
    RowView row(size_t i) const;

    //! \brief Add row views for every row in the matrix to a container.
    //!
    //! This function adds a \ref RowView for each row in the matrix to the
    //! container \p x.
    //!
    //! \tparam T the type of the container.
    //! \param x  a container.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is the template parameter \c R.
    template <typename T>
    void rows(T& x) const;

    //! \brief Swaps the contents of `*this` with the contents of \p that.
    //!
    //! This function swaps the contents of `*this` with the contents of
    //! \p that.
    //!
    //! \param that  the matrix to swap contents with.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    void swap(StaticMatrix& that) noexcept;

    //! \brief Transpose a matrix in-place.
    //!
    //! This function transpose a matrix object in-place.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \throws LibsemigroupsException if the matrix isn't square.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and
    //! \f$n\f$ is \ref number_of_cols.
    void transpose();

    //! \brief Transpose a matrix in-place.
    //!
    //! This function transpose a matrix object in-place.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and
    //! \f$n\f$ is \ref number_of_cols.
    //!
    //! \warning
    //! This only works when \ref number_of_rows and \ref number_of_cols are
    //! equal (i.e. for square matrices), but this is not verified.
    void transpose_no_checks();

    //! \brief Returns an identity matrix.
    //!
    //! This function returns an \c R times \c R identity matrix.
    //!
    //! \returns  The identity matrix with \c R rows and \c R columns.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n ^ 2)\f$ where \f$n\f$ is the template parameter \c R.
    static StaticMatrix one() const;

    //! \brief Return a hash value of a matrix.
    //!
    //! This function returns a hash value for a matrix. The return value is
    //! recomputed every time this function is called.
    //!
    //! \returns A hash value for a \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the \ref number_of_rows and \f$n\f$ is
    //! \ref number_of_cols.
    size_t hash_value() const;

    //! \brief Less than or equal operator.
    //!
    //! This operator defines a total order on the set of matrices of the
    //! same type.
    //!
    //! \tparam U the type of the argument \p that.
    //! \param that the matrix or row view for comparison.
    //!
    //! \returns \c true if `*this` is less than or equal to \p that  and
    //! \c false if it is not.
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$n\f$ is \ref number_of_cols.
    template <typename U>
    bool operator<=(U const& that) const;

    //! \brief Greater than or equal operator.
    //!
    //! This operator defines a total order on the set of matrices of the
    //! same type.
    //!
    //! \tparam U the type of the argument \p that.
    //! \param that the matrix or row view for comparison.
    //!
    //! \returns \c true if `*this` is greater than or equal to \p that  and
    //! \c false if it is not.
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$n\f$ is \ref number_of_cols.
    template <typename U>
    bool operator>=(U const& that) const;

    //! \brief Multiply a matrix by a scalar.
    //!
    //! This function returns the product of the matrix `*this` and the scalar
    //! \p a.
    //!
    //! \param a the scalar to add.
    //! \returns  The product of `*this` and \p a.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This function does not detect overflows in \ref scalar_type.
    StaticMatrix operator*(scalar_type a);

    //! \brief Add a scalar to a matrix.
    //!
    //! This function returns the sum of the matrix `*this` and the scalar
    //! \p a.
    //!
    //! \param a the scalar to add.
    //! \returns  The sum of `*this` and \p a.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This function does not detect overflows in
    //! \ref scalar_type.
    StaticMatrix operator+(scalar_type a);

    //! \brief Returns the additive identity of the underlying semiring.
    //!
    //! This function returns the additive identity of the underlying
    //! semiring of a matrix.
    //!
    //! \returns The additive identity of the semiring, a
    //! \ref scalar_type.
    //!
    //! \exceptions
    //! \noexcept
    scalar_type scalar_zero() const noexcept;

    //! \brief Returns the multiplicative identity of the underlying semiring.
    //!
    //! This function returns the multiplicative identity of the underlying
    //! semiring of a matrix.
    //!
    //! \returns The multiplicative identity of the semiring, a
    //! \ref scalar_type.
    //!
    //! \exceptions
    //! \noexcept
    scalar_type scalar_one() const noexcept;

    //! \brief Returns the underlying semiring.
    //!
    //! Returns a const pointer to the underlying semiring (if any).
    //!
    //! \returns  A value of type `Semiring const*`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    semiring_type const* semiring() const noexcept;

#else
    using MatrixCommon::at;
    using MatrixCommon::begin;
    using MatrixCommon::cbegin;
    using MatrixCommon::cend;
    using MatrixCommon::coords;
    using MatrixCommon::end;
    using MatrixCommon::hash_value;
    using MatrixCommon::number_of_cols;
    using MatrixCommon::number_of_rows;
    using MatrixCommon::one;
    using MatrixCommon::operator!=;
    using MatrixCommon::operator();
    using MatrixCommon::operator*;
    using MatrixCommon::operator*=;
    using MatrixCommon::operator+;
    using MatrixCommon::operator+=;
    using MatrixCommon::operator<;  // NOLINT(whitespace/operators)
    using MatrixCommon::operator<=;
    using MatrixCommon::operator==;
    using MatrixCommon::operator>;  // NOLINT(whitespace/operators)
    using MatrixCommon::operator>=;
    using MatrixCommon::product_inplace_no_checks;
    using MatrixCommon::row;
    using MatrixCommon::row_no_checks;
    using MatrixCommon::rows;
    using MatrixCommon::scalar_one;
    using MatrixCommon::scalar_zero;
    using MatrixCommon::semiring;
    using MatrixCommon::swap;
    using MatrixCommon::transpose;
    using MatrixCommon::transpose_no_checks;
#endif  // LIBSEMIGROUPS_PARSED_BY_DOXYGEN

   private:
    ////////////////////////////////////////////////////////////////////////
    // StaticMatrix - implementation of MatrixCommon requirements - private
    ////////////////////////////////////////////////////////////////////////

    static constexpr size_t number_of_rows_impl() noexcept {
      return R;
    }
    static constexpr size_t number_of_cols_impl() noexcept {
      return C;
    }
  };  // class StaticMatrix

  ////////////////////////////////////////////////////////////////////////
  // DynamicMatrix with compile time semiring arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! \anchor DynamicMatrixStaticArith
  //!
  //! \brief Class for matrices with compile time arithmetic and run-time
  //! dimensions.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a class for matrices where the arithmetic operations in the
  //! underlying semiring are known at compile time, and the dimensions of the
  //! matrix can be set at run time.
  //!
  //! \tparam PlusOp a stateless type with a call operator of signature
  //! `scalar_type operator()(scalar_type, scalar_type)` implementing the
  //! addition of the semiring.
  //!
  //! \tparam ProdOp a stateless type with a call operator of signature
  //! `scalar_type operator()(scalar_type, scalar_type)` implementing the
  //! multiplication of the semiring.
  //!
  //! \tparam ZeroOp a stateless type with a call operator of signature
  //! `scalar_type operator()()` returning the zero of the semiring (the
  //! additive identity element).
  //!
  //! \tparam OneOp a stateless type with a call operator of signature
  //! `scalar_type operator()()` returning the one of the semiring (the
  //! multiplicative identity element).
  //!
  //! \tparam Scalar the type of the entries in the matrices (the type of
  //! elements in the underlying semiring).
  //!
  //! \note
  //! Certain member functions only work for square matrices and some only
  //! work for rows.
  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            typename Scalar>
  class DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>
      : public detail::MatrixDynamicDim<Scalar>,
        public detail::MatrixCommon<
            std::vector<Scalar>,
            DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>,
            DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>>,
        public detail::
            MatrixStaticArithmetic<PlusOp, ProdOp, ZeroOp, OneOp, Scalar> {
    using MatrixDynamicDim = ::libsemigroups::detail::MatrixDynamicDim<Scalar>;
    using MatrixCommon     = ::libsemigroups::detail::MatrixCommon<
            std::vector<Scalar>,
            DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>,
            DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>>;
    friend MatrixCommon;

   public:
    //! The type of the entries in the matrix.
    using scalar_type = typename MatrixCommon::scalar_type;

    //! \brief The type of references to the entries in the matrix.
    using scalar_reference = typename MatrixCommon::scalar_reference;

    //! \brief The type of const references to the entries in the matrix.
    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    using scalar_const_reference = typename MatrixCommon::scalar_const_reference;
    // clang-format on

    //! \brief The type of a row of a DynamicMatrix.
    using Row = DynamicMatrix;

    //! \brief The type of a row view into a DynamicMatrix.
    using RowView = DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>;

    //! \brief Alias for the template parameter \p PlusOp.
    using Plus = PlusOp;

    //! \brief Alias for the template parameter \p ProdOp.
    using Prod = ProdOp;

    //! \brief Alias for the template parameter \p ZeroOp.
    using Zero = ZeroOp;

    //! \brief Alias for the template parameter \p OneOp.
    using One = OneOp;

    //! \brief Alias for the semiring type (`void`).
    //!
    //! The type of the semiring over which the matrix is defined is `void`
    //! because there's no semiring object, the arithmetic is defined by the
    //! template parameters.
    using semiring_type = void;

    //! \brief Default constructor.
    //!
    //! Default constructor.
    DynamicMatrix() = default;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    DynamicMatrix(DynamicMatrix const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    DynamicMatrix(DynamicMatrix&&) = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    DynamicMatrix& operator=(DynamicMatrix const&) = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    DynamicMatrix& operator=(DynamicMatrix&&) = default;

    //! \brief Construct a matrix with given dimensions.
    //!
    //! This function constructs a matrix with the given dimensions.
    //!
    //! \param r the number of rows.
    //! \param c the number of columns.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \par Example
    //! \code
    //! using Mat = IntMat<>;
    //! Mat m(2, 3); // construct a 2 x 3 matrix
    //! \endcode
    DynamicMatrix(size_t r, size_t c) : MatrixDynamicDim(r, c), MatrixCommon() {
      resize(number_of_rows(), number_of_cols());
    }

    //! \brief Construct a vector from a std::initializer_list.
    //!
    //! This function constructs a \f$1\times n\f$ matrix from a
    //! std::initializer_list of the entries of the row.
    //!
    //! \param c  the values to be copied into the matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the number of columns in the matrix being
    //! constructed.
    //!
    //! \par Example
    //! \code
    //! using Mat = BMat<>;
    //! Mat m({1, 1, 0, 0});
    //! \endcode
    explicit DynamicMatrix(std::initializer_list<scalar_type> const& c)
        : MatrixDynamicDim(1, c.size()), MatrixCommon(c) {}

    //! \brief Construct a matrix from std::initializer_list of
    //! std::initializer_list of scalars.
    //!
    //! This function constructs a matrix from a std::initializer_list of
    //! std::initializer_list of the entries.
    //!
    //! \param m  the values to be copied into the matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the number of rows and \f$n\f$ is the
    //! number of columns in the matrix being constructed.
    //!
    //! \par Example
    //! \code
    //! using Mat = IntMat<>;
    //! Mat m({{1, 1}, {0, 0}});
    //! \endcode
    explicit DynamicMatrix(
        std::initializer_list<std::initializer_list<scalar_type>> const& m)
        : MatrixDynamicDim(m.size(), std::empty(m) ? 0 : m.begin()->size()),
          MatrixCommon(m) {}

    //! \brief Construct a matrix from std::vector of
    //! std::vector of scalars.
    //!
    //! This function constructs a matrix from a std::vector of
    //! std::vector of the entries.
    //!
    //! \param m  the values to be copied into the matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the number of rows and \f$n\f$ is the
    //! number of columns in the matrix being constructed.
    explicit DynamicMatrix(std::vector<std::vector<scalar_type>> const& m)
        : MatrixDynamicDim(m.size(), std::empty(m) ? 0 : m.begin()->size()),
          MatrixCommon(m) {}

    //! \brief Construct a row from a row view.
    //!
    //! Construct a row from a row view.
    //!
    //! \param rv the row view.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the size of the row being constructed.
    explicit DynamicMatrix(RowView const& rv)
        : MatrixDynamicDim(1, rv.size()), MatrixCommon(rv) {}

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    // For uniformity of interface, the first arg does nothing
    DynamicMatrix(void const* ptr, size_t r, size_t c) : DynamicMatrix(r, c) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    // For uniformity of interface, the first arg does nothing
    DynamicMatrix(void const* ptr, std::initializer_list<scalar_type> const& c)
        : DynamicMatrix(c) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    // For uniformity of interface, the first arg does nothing
    DynamicMatrix(
        void const*                                                      ptr,
        std::initializer_list<std::initializer_list<scalar_type>> const& m)
        : DynamicMatrix(m) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    static DynamicMatrix one(void const* ptr, size_t n) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
      return one(n);
    }
#endif  // LIBSEMIGROUPS_PARSED_BY_DOXYGEN

    ~DynamicMatrix() = default;

    //! \brief Construct the \f$n \times n\f$ identity matrix.
    //!
    //! Construct the \f$n \times n\f$ identity matrix.
    //!
    //! \param n the dimension.
    //! \returns The \f$n \times n\f$ identity matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n ^ 2)\f$.
    static DynamicMatrix one(size_t n) {
      DynamicMatrix x(n, n);
      std::fill(x.begin(), x.end(), ZeroOp()());
      for (size_t r = 0; r < n; ++r) {
        x(r, r) = OneOp()();
      }
      return x;
    }

#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    //! \copydoc StaticMatrix::at(size_t, size_t)
    scalar_reference at(size_t r, size_t c);

    //! \copydoc StaticMatrix::at(size_t, size_t) const
    scalar_reference at(size_t r, size_t c) const;

    //! \copydoc StaticMatrix::begin()
    iterator begin() noexcept;

    //! \copydoc StaticMatrix::cbegin()
    const_iterator cbegin() noexcept;

    //! \copydoc StaticMatrix::cend()
    const_iterator cend() noexcept;

    //! \copydoc StaticMatrix::coords
    std::pair<scalar_type, scalar_type> coords(const_iterator it) const;

    //! \copydoc StaticMatrix::end()
    iterator end() noexcept;

    //! \copydoc StaticMatrix::hash_value
    size_t hash_value() const;

    //! \copydoc StaticMatrix::number_of_cols
    size_t number_of_cols() const noexcept;

    //! \copydoc StaticMatrix::number_of_rows
    size_t number_of_rows() const noexcept;

    //! \copydoc StaticMatrix::operator!=
    bool operator!=(DynamicMatrix const& that) const;

    //! \copydoc StaticMatrix::operator!=
    bool operator!=(RowView const& that) const;

    //! \copydoc StaticMatrix::operator()(size_t, size_t)
    scalar_reference operator()(size_t r, size_t c);

    //! \copydoc StaticMatrix::operator()(size_t, size_t) const
    scalar_const_reference operator()(size_t r, size_t c) const;
    //! \brief Multiplies every entry of a matrix by a scalar in-place.
    //!
    //! This function multiplies every entry of a matrix by a scalar in-place.
    //!
    //! \param a the scalar to multiply every entry by.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$m\f$ is \ref number_of_cols.
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    DynamicMatrix operator*(scalar_type a);

    //! \copydoc StaticMatrix::product_no_checks
    DynamicMatrix product_no_checks(DynamicMatrix const& that);

    //! \copydoc StaticMatrix::operator*
    DynamicMatrix operator*(DynamicMatrix const& that);

    //! \copydoc StaticMatrix::operator*=
    void operator*=(scalar_type a);

    //! \copydoc StaticMatrix::plus_no_checks
    DynamicMatrix plus_no_checks(DynamicMatrix const& y) const;

    //! \copydoc StaticMatrix::operator+
    DynamicMatrix operator+(DynamicMatrix const& that);

    //! \copydoc StaticMatrix::plus_inplace_no_checks
    void plus_inplace_no_checks(DynamicMatrix const& that);

    //! \copydoc StaticMatrix::plus_inplace_no_checks
    void plus_inplace_no_checks(RowView const& that);

    //! \copydoc StaticMatrix::operator+=
    void operator+=(DynamicMatrix const& that);

    //! \copydoc StaticMatrix::operator+=
    void operator+=(RowView const& that);

    //! \brief Adds a scalar to every entry of the matrix in-place.
    //!
    //! This function adds a scalar to every entry of the matrix in-place.
    //!
    //! \param a the \ref scalar_type to add to a \ref DynamicMatrix.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$m\f$ is
    //! \ref number_of_cols
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    void operator+=(scalar_type a);

    //! \copydoc StaticMatrix::operator<
    bool operator<(DynamicMatrix const& that) const;

    //! \copydoc StaticMatrix::operator<
    bool operator<(RowView const& that) const;

    //! \copydoc StaticMatrix::operator<=
    template <typename T>
    bool operator<=(T const& that) const;

    //! \copydoc StaticMatrix::operator==
    bool operator==(DynamicMatrix const& that) const;

    //! \copydoc StaticMatrix::operator==
    bool operator==(RowView const& that) const;

    //! \copydoc StaticMatrix::operator>
    bool operator>(DynamicMatrix const& that) const;

    //! \copydoc StaticMatrix::operator>=
    template <typename T>
    bool operator>=(T const& that) const;

    //! \copydoc StaticMatrix::product_inplace_no_checks
    void product_inplace_no_checks(DynamicMatrix const& x,
                                   DynamicMatrix const& y);

    //! \copydoc StaticMatrix::product_inplace
    void product_inplace(DynamicMatrix const& x, DynamicMatrix const& y);

    //! \copydoc StaticMatrix::row
    RowView row(size_t i) const;

    //! \copydoc StaticMatrix::row_no_checks
    RowView row_no_checks(size_t i) const;

    //! \copydoc StaticMatrix::rows
    template <typename T>
    void rows(T& x) const;

    //! \copydoc StaticMatrix::scalar_one
    scalar_type scalar_one() const noexcept;

    //! \copydoc StaticMatrix::scalar_zero
    scalar_type scalar_zero() const noexcept;

    //! \copydoc StaticMatrix::semiring
    semiring_type const* semiring() const noexcept;

    //! \copydoc StaticMatrix::transpose
    void transpose();

    //! \copydoc StaticMatrix::transpose_no_checks
    void transpose_no_checks();
#else
    using MatrixCommon::at;
    using MatrixCommon::begin;
    using MatrixCommon::cbegin;
    using MatrixCommon::cend;
    using MatrixCommon::coords;
    using MatrixCommon::end;
    using MatrixCommon::hash_value;
    using MatrixCommon::number_of_cols;
    using MatrixCommon::number_of_rows;
    using MatrixCommon::one;
    using MatrixCommon::operator!=;
    using MatrixCommon::operator();
    using MatrixCommon::operator*;
    using MatrixCommon::operator*=;
    using MatrixCommon::operator+;
    using MatrixCommon::operator+=;
    using MatrixCommon::operator<;  // NOLINT(whitespace/operators)
    using MatrixCommon::operator<=;
    using MatrixCommon::operator==;
    using MatrixCommon::operator>;  // NOLINT(whitespace/operators)
    using MatrixCommon::operator>=;
    using MatrixCommon::product_inplace_no_checks;
    using MatrixCommon::row;
    using MatrixCommon::row_no_checks;
    using MatrixCommon::rows;
    using MatrixCommon::scalar_one;
    using MatrixCommon::scalar_zero;
    using MatrixCommon::semiring;
    // using MatrixCommon::swap; // Don't want this use the one below.
    using MatrixCommon::transpose;
    using MatrixCommon::transpose_no_checks;
#endif  // LIBSEMIGROUPS_PARSED_BY_DOXYGEN

    //! \copydoc StaticMatrix::swap
    void swap(DynamicMatrix& that) noexcept {
      static_cast<MatrixDynamicDim&>(*this).swap(
          static_cast<MatrixDynamicDim&>(that));
      static_cast<MatrixCommon&>(*this).swap(static_cast<MatrixCommon&>(that));
    }

   private:
    using MatrixCommon::resize;
  };

  ////////////////////////////////////////////////////////////////////////
  // DynamicMatrix with runtime semiring arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! \anchor DynamicMatrixDynamicArith
  //!
  //! \brief Class for matrices with run-time arithmetic and dimensions.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a class for matrices where both the arithmetic operations in
  //! the underlying semiring and the dimensions of the matrix can be set at
  //! run time.
  //!
  //! \tparam Semiring the type of a semiring object which defines the
  //! semiring arithmetic (see requirements below).
  //!
  //! \tparam Scalar the type of the entries in the matrices (the type of
  //! elements in the underlying semiring).
  //!
  //! \note
  //! Certain member functions only work for square matrices and some only
  //! work for rows.
  //!
  //! ## Semiring requirements
  //!
  //! The template parameter Semiring must have the following
  //! member functions:
  //! * `scalar_type scalar_zero()` that returns the multiplicative zero
  //! scalar in the semiring
  //! * `scalar_type scalar_one()` that returns the multiplicative identity
  //! scalar in the semiring
  //! * `scalar_type plus_no_checks(scalar_type x, scalar_type y)` that
  //! returns the sum in the semiring of the scalars \c x and \c y.
  //! * `scalar_type product_no_checks(scalar_type x, scalar_type y)` that
  //! returns the product in the semiring of the scalars \c x and \c y.
  //!
  //! See, for example, \ref MaxPlusTruncSemiring.
  template <typename Semiring, typename Scalar>
  class DynamicMatrix<Semiring, Scalar>
      : public detail::MatrixDynamicDim<Scalar>,
        public detail::MatrixCommon<std::vector<Scalar>,
                                    DynamicMatrix<Semiring, Scalar>,
                                    DynamicRowView<Semiring, Scalar>,
                                    Semiring> {
    using MatrixCommon = detail::MatrixCommon<std::vector<Scalar>,
                                              DynamicMatrix<Semiring, Scalar>,
                                              DynamicRowView<Semiring, Scalar>,
                                              Semiring>;
    friend MatrixCommon;
    using MatrixDynamicDim = ::libsemigroups::detail::MatrixDynamicDim<Scalar>;

   public:
    //! \copybrief StaticMatrix::scalar_type
    using scalar_type = typename MatrixCommon::scalar_type;

    //! \copybrief StaticMatrix::scalar_reference
    using scalar_reference = typename MatrixCommon::scalar_reference;

    //! \copybrief StaticMatrix::scalar_const_reference
    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    using scalar_const_reference = typename MatrixCommon::scalar_const_reference;
    // clang-format on

    //! \brief Alias for the type of the rows of a DynamicMatrix
    using Row = DynamicMatrix;

    //! \brief Alias for the type of row views of a DynamicMatrix
    using RowView = DynamicRowView<Semiring, Scalar>;

    friend RowView;

    //! \brief Alias for the template parameter Semiring.
    using semiring_type = Semiring;

    //! \brief Deleted.
    //!
    //! The default constructor for this variant of DynamicMatrix is deleted
    //! because a valid semiring object is required to define the arithmetic
    //! at run-time.
    DynamicMatrix() = delete;

    //! \copydoc StaticMatrix::StaticMatrix(StaticMatrix const&)
    DynamicMatrix(DynamicMatrix const&) = default;

    //! \copydoc StaticMatrix::StaticMatrix(StaticMatrix&&)
    DynamicMatrix(DynamicMatrix&&) = default;

    //! \copydoc StaticMatrix::operator=(StaticMatrix const&)
    DynamicMatrix& operator=(DynamicMatrix const&) = default;

    //! \copydoc StaticMatrix::operator=(StaticMatrix&&)
    DynamicMatrix& operator=(DynamicMatrix&&) = default;

    //! \brief Construct a matrix over a given semiring with given dimensions.
    //!
    //! Construct a matrix over the semiring `semiring` with the given
    //! dimensions.
    //!
    //! \param sr  a pointer to const semiring object.
    //! \param r  the number of rows in the matrix being constructed.
    //! \param c  the number of columns in the matrix being constructed.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    DynamicMatrix(Semiring const* sr, size_t r, size_t c)
        : MatrixDynamicDim(r, c), MatrixCommon(), _semiring(sr) {
      resize(number_of_rows(), number_of_cols());
    }

    //! \brief Construct a matrix over a given semiring (std::initializer_list
    //! of std::initializer_list).
    //!
    //! This function constructs a matrix over a given semiring from an
    //! std::initializer_list of the rows (std::initializer_list).
    //!
    //! \param sr  a pointer to const semiring object.
    //! \param rws  the values to be copied into the matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$n\f$ is
    //! \ref number_of_cols.
    explicit DynamicMatrix(
        Semiring const*                                                  sr,
        std::initializer_list<std::initializer_list<scalar_type>> const& rws)
        : MatrixDynamicDim(rws.size(),
                           std::empty(rws) ? 0 : rws.begin()->size()),
          MatrixCommon(rws),
          _semiring(sr) {}

    //! \brief Construct a matrix over a given semiring (std::vector
    //! of std::vector).
    //!
    //! This function constructs a matrix over a given semiring from an
    //! std::vector of the rows (std::vector).
    //!
    //! \param sr  a pointer to const semiring object.
    //! \param rws  the values to be copied into the matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$n\f$ is
    //! \ref number_of_cols.
    explicit DynamicMatrix(Semiring const*                              sr,
                           std::vector<std::vector<scalar_type>> const& rws)
        : MatrixDynamicDim(rws.size(), (rws.empty() ? 0 : rws.begin()->size())),
          MatrixCommon(rws),
          _semiring(sr) {}

    //! \brief Construct a row over a given semiring (std::initializer_list).
    //!
    //! Construct a row over a given semiring from a std::initializer_list of
    //! the entries.
    //!
    //! \param sr a pointer to const Semiring object.
    //! \param rw the values to be copied into the row.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_cols.
    explicit DynamicMatrix(Semiring const*                           sr,
                           std::initializer_list<scalar_type> const& rw)
        : MatrixDynamicDim(1, rw.size()), MatrixCommon(rw), _semiring(sr) {}

    //! \brief Construct a row over a given semiring (\ref RowView).
    //!
    //! Construct a row over a given semiring from a \ref RowView.
    //!
    //! \param rv  the row view.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the size of the row being constructed.
    explicit DynamicMatrix(RowView const& rv)
        : MatrixDynamicDim(1, rv.size()),
          MatrixCommon(rv),
          _semiring(rv._matrix->semiring()) {}

    //! \brief Construct the \f$n \times n\f$ identity matrix.
    //!
    //! Construct the \f$n \times n\f$ identity matrix.
    //!
    //! \param semiring the semiring.
    //! \param n the dimension.
    //!
    //! \returns The \f$n \times n\f$ identity matrix over \p semiring.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n ^ 2)\f$.
    // No static DynamicMatrix::one(size_t n) because we need a semiring!
    static DynamicMatrix one(Semiring const* semiring, size_t n) {
      DynamicMatrix x(semiring, n, n);
      std::fill(x.begin(), x.end(), x.scalar_zero());
      for (size_t r = 0; r < n; ++r) {
        x(r, r) = x.scalar_one();
      }
      return x;
    }

    ~DynamicMatrix() = default;

#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    //! \copydoc StaticMatrix::at(size_t, size_t)
    scalar_reference at(size_t r, size_t c);

    //! \copydoc StaticMatrix::at(size_t, size_t) const
    scalar_reference at(size_t r, size_t c) const;

    //! \copydoc StaticMatrix::begin()
    iterator begin() noexcept;

    //! \copydoc StaticMatrix::cbegin()
    const_iterator cbegin() noexcept;

    //! \copydoc StaticMatrix::cend()
    const_iterator cend() noexcept;

    //! \copydoc StaticMatrix::coords
    std::pair<scalar_type, scalar_type> coords(const_iterator it) const;

    //! \copydoc StaticMatrix::end()
    iterator end() noexcept;

    //! \copydoc StaticMatrix::hash_value
    size_t hash_value() const;

    //! \copydoc StaticMatrix::number_of_cols
    size_t number_of_cols() const noexcept;

    //! \copydoc StaticMatrix::number_of_rows
    size_t number_of_rows() const noexcept;

    //! \copydoc StaticMatrix::operator!=
    bool operator!=(DynamicMatrix const& that) const;

    //! \copydoc StaticMatrix::operator!=
    bool operator!=(RowView const& that) const;

    //! \copydoc StaticMatrix::operator()(size_t, size_t)
    scalar_reference operator()(size_t r, size_t c);

    //! \copydoc StaticMatrix::operator()(size_t, size_t) const
    scalar_const_reference operator()(size_t r, size_t c) const;

    //! \copydoc StaticMatrix::operator*(scalar_type)
    DynamicMatrix operator*(scalar_type a);

    //! \copydoc StaticMatrix::product_no_checks
    DynamicMatrix product_no_checks(DynamicMatrix const& that);

    //! \copydoc StaticMatrix::operator*
    DynamicMatrix operator*(DynamicMatrix const& that);

    //! \copydoc StaticMatrix::operator*=
    void operator*=(scalar_type a);

    //! \copydoc StaticMatrix::operator+
    DynamicMatrix operator+(DynamicMatrix const& that);

    //! \copydoc StaticMatrix::plus_inplace_no_checks
    void plus_inplace_no_checks(DynamicMatrix const& that);

    //! \copydoc StaticMatrix::plus_inplace_no_checks
    void plus_inplace_no_checks(RowView const& that);

    //! \copydoc StaticMatrix::operator+=
    void operator+=(DynamicMatrix const& that);

    //! \copydoc StaticMatrix::operator+=
    void operator+=(RowView const& that);

    //! \brief Adds a scalar to every entry of the matrix in-place.
    //!
    //! This function adds a scalar to every entry of the matrix in-place.
    //!
    //! \param a the \ref scalar_type to add to a \ref DynamicMatrix.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$m\f$ is
    //! \ref number_of_cols
    //!
    //! \warning This function does not detect overflows of \ref scalar_type.
    void operator+=(scalar_type a);

    //! \copydoc StaticMatrix::operator<
    bool operator<(DynamicMatrix const& that) const;

    //! \copydoc StaticMatrix::operator<
    bool operator<(RowView const& that) const;

    //! \copydoc StaticMatrix::operator<=
    template <typename T>
    bool operator<=(T const& that) const;

    //! \copydoc StaticMatrix::operator==
    bool operator==(DynamicMatrix const& that) const;

    //! \copydoc StaticMatrix::operator==
    bool operator==(RowView const& that) const;

    //! \copydoc StaticMatrix::operator>
    bool operator>(DynamicMatrix const& that) const;

    //! \copydoc StaticMatrix::operator>=
    template <typename T>
    bool operator>=(T const& that) const;

    //! \copydoc StaticMatrix::product_inplace_no_checks
    void product_inplace_no_checks(DynamicMatrix const& x,
                                   DynamicMatrix const& y);

    //! \copydoc StaticMatrix::product_inplace
    void product_inplace(DynamicMatrix const& x, DynamicMatrix const& y);

    //! \copydoc StaticMatrix::row
    RowView row(size_t i) const;

    //! \copydoc StaticMatrix::row_no_checks
    RowView row_no_checks(size_t i) const;

    //! \copydoc StaticMatrix::rows
    template <typename T>
    void rows(T& x) const;

    //! \copydoc StaticMatrix::scalar_one
    scalar_type scalar_one() const noexcept;

    //! \copydoc StaticMatrix::scalar_zero
    scalar_type scalar_zero() const noexcept;

    //! \copydoc StaticMatrix::semiring
    semiring_type const* semiring() const noexcept;

    //! \copydoc StaticMatrix::transpose
    void transpose();

    //! \copydoc StaticMatrix::transpose_no_checks
    void transpose_no_checks();
#else
    using MatrixCommon::at;
    using MatrixCommon::begin;
    using MatrixCommon::cbegin;
    using MatrixCommon::cend;
    using MatrixCommon::coords;
    using MatrixCommon::end;
    using MatrixCommon::hash_value;
    using MatrixCommon::number_of_cols;
    using MatrixCommon::number_of_rows;
    using MatrixCommon::one;
    using MatrixCommon::operator!=;
    using MatrixCommon::operator();
    using MatrixCommon::operator*;
    using MatrixCommon::operator*=;
    using MatrixCommon::operator+;
    using MatrixCommon::operator+=;
    using MatrixCommon::operator<;  // NOLINT(whitespace/operators)
    using MatrixCommon::operator<=;
    using MatrixCommon::operator==;
    using MatrixCommon::operator>;  // NOLINT(whitespace/operators)
    using MatrixCommon::operator>=;
    using MatrixCommon::product_inplace_no_checks;
    using MatrixCommon::row;
    using MatrixCommon::row_no_checks;
    using MatrixCommon::rows;
    using MatrixCommon::scalar_one;
    using MatrixCommon::scalar_zero;
    using MatrixCommon::semiring;
    // using MatrixCommon::swap; // Don't want this use the one below.
    using MatrixCommon::transpose;
    using MatrixCommon::transpose_no_checks;
#endif  // LIBSEMIGROUPS_PARSED_BY_DOXYGEN

    //! \copydoc StaticMatrix::swap
    void swap(DynamicMatrix& that) noexcept {
      static_cast<MatrixDynamicDim&>(*this).swap(
          static_cast<MatrixDynamicDim&>(that));
      static_cast<MatrixCommon&>(*this).swap(static_cast<MatrixCommon&>(that));
      std::swap(_semiring, that._semiring);
    }

   private:
    using MatrixCommon::resize;

    scalar_type plus_no_checks_impl(scalar_type x,
                                    scalar_type y) const noexcept {
      return _semiring->plus_no_checks(x, y);
    }

    scalar_type product_no_checks_impl(scalar_type x,
                                       scalar_type y) const noexcept {
      return _semiring->product_no_checks(x, y);
    }

    scalar_type one_impl() const noexcept {
      return _semiring->scalar_one();
    }

    scalar_type zero_impl() const noexcept {
      return _semiring->scalar_zero();
    }

    Semiring const* semiring_impl() const noexcept {
      return _semiring;
    }

    Semiring const* _semiring;
  };

  ////////////////////////////////////////////////////////////////////////
  // Helper structs to check if matrix is static, or has a pointer to a
  // semiring
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    template <typename PlusOp,
              typename ProdOp,
              typename ZeroOp,
              typename OneOp,
              size_t R,
              size_t C,
              typename Scalar>
    struct IsStaticMatrixHelper<
        StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>>
        : std::true_type {};

    template <typename Semiring, typename Scalar>
    struct IsMatWithSemiringHelper<DynamicMatrix<Semiring, Scalar>>
        : std::true_type {};
  }  // namespace detail

  //! \ingroup matrix_group
  //!
  //! \brief Namespace for helper functions for matrices.
  //!
  //! This namespace contains various helper functions for the various matrix
  //! classes in `libsemigroups`. These functions could have been member
  //! functions of the matrix classes but they only use public member
  //! functions, and so they are declared as free functions instead.
  namespace matrix {

    //! \brief Returns the threshold of a matrix.
    //!
    //! Defined in `matrix.hpp`.
    //!
    //! The threshold of a matrix that does not have entries in a truncated
    //! semiring is \ref UNDEFINED, and this function returns this value.
    //!
    //! \tparam Mat the type of the matrix.
    //!
    //! \returns The value \ref UNDEFINED.
    //!
    //! \exceptions
    //! \noexcept
    template <typename Mat>
    constexpr auto threshold(Mat const&) noexcept
        -> std::enable_if_t<!detail::IsTruncMat<Mat>,
                            typename Mat::scalar_type> {
      return UNDEFINED;
    }

    //! \brief Returns the threshold of a matrix over a truncated semiring.
    //!
    //! Defined in `matrix.hpp`.
    //!
    //! This function returns the threshold of a matrix over a truncated
    //! semiring.
    //!
    //! \tparam Mat the type of the matrix.
    //!
    //! \returns The threshold of any matrix of type \p Mat.
    //!
    //! \exceptions
    //! \noexcept
    template <typename Mat>
    constexpr auto threshold(Mat const&) noexcept
        -> std::enable_if_t<detail::IsTruncMat<Mat> && !IsMatWithSemiring<Mat>,
                            typename Mat::scalar_type> {
      return detail::IsTruncMatHelper<Mat>::threshold;
    }

    //! \brief Returns the threshold of a matrix over a truncated semiring.
    //!
    //! Defined in `matrix.hpp`.
    //!
    //! This function returns the threshold of a matrix over a truncated
    //! semiring.
    //!
    //! \tparam Mat the type of the matrix.
    //!
    //! \param x the matrix.
    //!
    //! \returns The threshold of \p x.
    //!
    //! \exceptions
    //! \noexcept
    template <typename Mat>
    auto threshold(Mat const& x) noexcept
        -> std::enable_if_t<detail::IsTruncMat<Mat> && IsMatWithSemiring<Mat>,
                            typename Mat::scalar_type> {
      return x.semiring()->threshold();
    }
  }  // namespace matrix

  ////////////////////////////////////////////////////////////////////////
  // Boolean matrices - compile time semiring arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup bmat_group BMat
  //!
  //! \ingroup bmat_index_group
  //!
  //! \brief For boolean matrices of arbitrary degree.
  //!
  //! This page describes the functionality for \f$m \times n\f$ boolean
  //! matrices for arbitrary dimensions \f$m\f$ and \f$n\f$. There are two
  //! types of such boolean matrices those whose dimension is known at
  //! compile-time, and those where it is not.  Both types can be accessed via
  //! the alias template `BMat<R, C>`: if \c R or \c C has value
  //! \c 0, then the dimensions can be set at run time, otherwise the
  //! dimensions are \c R and \c C. The default value of \c R is \c 0 and of
  //! \c C is \c R.
  //!
  //! The alias `BMat<R, C>` is either StaticMatrix,
  //! \ref DynamicMatrixStaticArith "DynamicMatrix (compile-time arithmetic)",
  //! or \ref DynamicMatrixDynamicArith "DynamicMatrix (run-time arithmetic)",
  //! please refer to the documentation of these class templates for more
  //! details. The only substantial difference in the interface of static
  //! and dynamic is that the former can be default constructed and the
  //! latter should be constructed using the dimensions.
  //!
  //! Adapters for \ref BMat objects are documented \ref adapters_bmat_group
  //! "here".
  //!
  //! \par Example
  //! \code
  //! // default construct an uninitialized 3 x 3 static matrix
  //! [[maybe_unused]] BMat<3> m3;
  //! // construct an uninitialized 4 x 4 dynamic matrix
  //! BMat<> m4(4, 4);
  //! \endcode

  //! \ingroup bmat_group
  //!
  //! \brief Function object for addition in the boolean semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `bool operator()(bool x, bool y) const noexcept` which returns `x || y`;
  //! representing addition in the boolean semiring.
  struct BooleanPlus {
    //! \brief Call operator for addition.
    //!
    //! This function returns the sum of its arguments in the boolean
    //! semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The sum of \p x and \p y in the boolean semiring.
    //!
    //! \exceptions
    //! \noexcept
    constexpr bool operator()(bool x, bool y) const noexcept {
      return x || y;
    }
  };

  //! \ingroup bmat_group
  //!
  //! \brief Function object for multiplication in the boolean semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `bool operator()(bool x, bool y) const noexcept` which returns `x && y`;
  //! representing multiplication in the boolean semiring.
  struct BooleanProd {
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in the boolean
    //! semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in the boolean semiring.
    //!
    //! \exceptions
    //! \noexcept
    constexpr bool operator()(bool x, bool y) const noexcept {
      return x & y;
    }
  };

  //! \ingroup bmat_group
  //!
  //! \brief Function object for returning the multiplicative identity.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `bool operator()() const noexcept` which returns \c true; representing
  //! the multiplicative identity of the boolean semiring.
  struct BooleanOne {
    //! \brief Call operator returning the multiplication identity \c true of
    //! the boolean semiring.
    //!
    //! Call operator returning the multiplication identity \c true of
    //! the boolean semiring.
    //!
    //! \returns The value \c true.
    //!
    //! \exceptions
    //! \noexcept
    constexpr bool operator()() const noexcept {
      return true;
    }
  };

  //! \ingroup bmat_group
  //!
  //! \brief Function object for returning the additive identity.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `bool operator()() const noexcept` which returns \c false;
  //! representing the additive identity of the boolean semiring.
  struct BooleanZero {
    //! \brief Call operator returning the additive identity \c false of
    //! the boolean semiring.
    //!
    //! Call operator returning the additive identity \c false of
    //! the boolean semiring.
    //!
    //! \returns The value \c false.
    //!
    //! \exceptions
    //! \noexcept
    constexpr bool operator()() const noexcept {
      return false;
    }
  };

  //! \ingroup bmat_group
  //!
  //! \brief Alias for dynamic boolean matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for the type of dynamic boolean matrices where the dimensions of
  //! the matrices can be defined at run time.
  // The use of `int` rather than `bool` as the scalar type for dynamic
  // boolean matrices is intentional, because the bit iterators implemented in
  // std::vector<bool> entail a significant performance penalty.
  using DynamicBMat
      = DynamicMatrix<BooleanPlus, BooleanProd, BooleanZero, BooleanOne, int>;

  //! \ingroup bmat_group
  //!
  //! \brief Alias for static boolean matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for the type of static boolean matrices where the dimensions of
  //! the matrices are defined at compile time.
  //!
  //! \tparam R the number of rows.
  //! \tparam C the number of columns.
  template <size_t R, size_t C>
  using StaticBMat = StaticMatrix<BooleanPlus,
                                  BooleanProd,
                                  BooleanZero,
                                  BooleanOne,
                                  R,
                                  C,
                                  int>;

  //! \ingroup bmat_group
  //!
  //! \brief Alias template for boolean matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias template for boolean matrices.
  //!
  //! \tparam R the number of rows of the matrix. A value of \c 0 (the default
  //! value) indicates that the dimensions will be set at run time.
  //!
  //! \tparam C the number of columns of the matrix. A value of \c 0 indicates
  //! that the dimensions will be set at run time (the default value is \c R).
  // FLS + JDM considered adding BMat8 and decided it wasn't a good idea.
  template <size_t R = 0, size_t C = R>
  using BMat
      = std::conditional_t<R == 0 || C == 0, DynamicBMat, StaticBMat<R, C>>;

  namespace detail {
    template <size_t R, size_t C>
    struct IsBMatHelper<StaticBMat<R, C>> : std::true_type {};

    template <>
    struct IsBMatHelper<DynamicBMat> : std::true_type {};
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Integer matrices - compile time semiring arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup intmat_group Integer matrices
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This page describes the functionality for \f$m \times n\f$  matrices of
  //! integers for arbitrary dimensions \f$m\f$ and \f$n\f$. There are two
  //! types of such matrices: those whose dimension is known at compile-time,
  //! and those where it is not.  Both types can be accessed via the alias
  //! template IntMat: if \c R or \c C has value \c 0, then the dimensions
  //! can be set at run time, otherwise \c R and \c C are the dimension. The
  //! default value of \c R is \c 0, and of \c C is \c R.
  //!
  //! The alias \ref IntMat is either StaticMatrix,
  //! \ref DynamicMatrixStaticArith "DynamicMatrix (compile-time arithmetic)",
  //! or \ref DynamicMatrixDynamicArith "DynamicMatrix (run-time arithmetic)",
  //! please refer to the documentation of these class templates for more
  //! details. The only substantial difference in the interface of static
  //! and dynamic is that the former can be default constructed and the
  //! latter should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //! // default construct an uninitialized 3 x 3 static matrix
  //! [[maybe_unused]] IntMat<3> m3;
  //! // construct an uninitialized 4 x 4 dynamic matrix
  //! IntMat<>  m4(4, 4);
  //! \endcode

  //! \ingroup intmat_group
  //!
  //! \brief Function object for addition in the ring of integers.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! which returns the usual sum `x + y` of \c x and \c y; representing
  //! addition in the integer semiring.
  //!
  //! \tparam Scalar the type of the entries in the matrix.
  template <typename Scalar>
  struct IntegerPlus {
    //! \brief Call operator for addition.
    //!
    //! This function returns the sum of its arguments in the ring of
    //! integers.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The sum of \p x and \p y in the boolean semiring.
    //!
    //! \exceptions
    //! \noexcept
    constexpr Scalar operator()(Scalar x, Scalar y) const noexcept {
      return x + y;
    }
  };

  //! \ingroup intmat_group
  //!
  //! \brief Function object for multiplication in the ring of integers.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! which returns the usual product `x * y` of \c x and \c y;
  //! representing multiplication in the integer semiring.
  //!
  //! \tparam Scalar the type of the entries in the matrix.
  template <typename Scalar>
  struct IntegerProd {
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in the ring of
    //! integers.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in the ring of integers.
    //!
    //! \exceptions
    //! \noexcept
    constexpr Scalar operator()(Scalar x, Scalar y) const noexcept {
      return x * y;
    }
  };

  //! \ingroup intmat_group
  //!
  //! \brief Function object for returning the additive identity.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()() const noexcept` which returns \c 0; representing
  //! the additive identity of the integer semiring.
  template <typename Scalar>
  struct IntegerZero {
    //! \brief Call operator returning the integer \c 0.
    //!
    //! This function returns the additive identity of the ring of
    //! integers (i.e. the integer \c 0).
    //!
    //! \returns The integer \c 0.
    //!
    //! \exceptions
    //! \noexcept
    constexpr Scalar operator()() const noexcept {
      return 0;
    }
  };

  //! \ingroup intmat_group
  //!
  //! \brief Function object for returning the multiplicative identity.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()() const noexcept` which returns \c 1; representing
  //! the multiplicative identity of the integer semiring.
  template <typename Scalar>
  struct IntegerOne {
    //! \brief Call operator returning the integer \c 1.
    //!
    //! This function returns the multiplicative identity of the ring of
    //! integers (i.e. the integer \c 1).
    //!
    //! \returns The integer \c 1.
    //!
    //! \exceptions
    //! \noexcept
    constexpr Scalar operator()() const noexcept {
      return 1;
    }
  };

  //! \ingroup intmat_group
  //!
  //! \brief Alias for dynamic integer matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for the type of dynamic integer matrices where the dimensions of
  //! the matrices can be defined at run time.
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <typename Scalar>
  using DynamicIntMat = DynamicMatrix<IntegerPlus<Scalar>,
                                      IntegerProd<Scalar>,
                                      IntegerZero<Scalar>,
                                      IntegerOne<Scalar>,
                                      Scalar>;

  //! \ingroup intmat_group
  //!
  //! \brief Alias for static integer matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for the type of static integer matrices where the dimensions of
  //! the matrices can be defined at compile time.
  //!
  //! \tparam R the number of rows of the matrix. A value of \c 0 (the default
  //! value) indicates that the dimensions will be set at run time.
  //!
  //! \tparam C the number of columns of the matrix. A value of \c 0 indicates
  //! that the dimensions will be set at run time (the default value is \c R).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t R, size_t C, typename Scalar>
  using StaticIntMat = StaticMatrix<IntegerPlus<Scalar>,
                                    IntegerProd<Scalar>,
                                    IntegerZero<Scalar>,
                                    IntegerOne<Scalar>,
                                    R,
                                    C,
                                    Scalar>;

  //! \ingroup intmat_group
  //!
  //! \brief Alias template for integer matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias template for integer matrices.
  //!
  //! \tparam R the number of rows of the matrix. A value of \c 0 (the
  //! default value) indicates that the dimensions will be set at run time.
  //!
  //! \tparam C the number of columns of the matrix. A value of \c 0
  //! indicates that the dimensions will be set at run time (the default value
  //! is \c R).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t R = 0, size_t C = R, typename Scalar = int>
  using IntMat = std::conditional_t<R == 0 || C == 0,
                                    DynamicIntMat<Scalar>,
                                    StaticIntMat<R, C, Scalar>>;
  namespace detail {
    template <size_t R, size_t C, typename Scalar>
    struct IsIntMatHelper<StaticIntMat<R, C, Scalar>> : std::true_type {};

    template <typename Scalar>
    struct IsIntMatHelper<DynamicIntMat<Scalar>> : std::true_type {};
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Max-plus matrices
  ////////////////////////////////////////////////////////////////////////
  //! \defgroup maxplusmat_group Max-plus matrices
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This page describes the functionality for \f$n \times n\f$  matrices
  //! over the max-plus semiring for arbitrary dimension \f$n\f$. There are
  //! two types of such matrices those whose dimension is known at
  //! compile-time, and those where it is not.  Both types can be accessed via
  //! the alias template
  //! \ref MaxPlusMat if \c N has value \c 0, then the dimensions can be set
  //! at run time, otherwise \c N is the dimension. The default value of \c N
  //! is \c 0.
  //!
  //! The alias \ref MaxPlusMat is either StaticMatrix,
  //! \ref DynamicMatrixStaticArith "DynamicMatrix (compile-time arithmetic)",
  //! or \ref DynamicMatrixDynamicArith "DynamicMatrix (run-time arithmetic)",
  //! please refer to the documentation of these class templates for more
  //! details. The only substantial difference in the interface of static
  //! and dynamic matrices is that the former can be default constructed and
  //! the latter should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //! // default construct an uninitialized 3 x 3 static matrix
  //! [[maybe_unused]] MaxPlusMat<3> m3;
  //! // construct an uninitialized 4 x 4 dynamic matrix
  //! MaxPlusMat<>  m4(4, 4);
  //! \endcode

  //! \ingroup maxplusmat_group
  //!
  //! \brief Function object for addition in the max-plus semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! that returns \f$x \oplus y\f$ which is defined by
  //!
  //! \f[
  //!    x\oplus y =
  //!    \begin{cases}
  //!    \max\{x, y\}   & \text{if } x \neq -\infty\text{ and }y \neq -\infty
  //!    \\ \mbox{}
  //!    -\infty & \text{if } x = -\infty \text{ or }y = -\infty;
  //!    \end{cases}
  //! \f]
  //!
  //! representing addition in the max-plus semiring.
  // Static arithmetic
  template <typename Scalar>
  struct MaxPlusPlus {
    static_assert(std::is_signed<Scalar>::value,
                  "MaxPlus requires a signed integer type as parameter!");
    //! \brief Call operator for addition.
    //!
    //! This function returns the sum of its arguments in the max-plus
    //! semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The sum of \p x and \p y in the max-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      if (x == NEGATIVE_INFINITY) {
        return y;
      } else if (y == NEGATIVE_INFINITY) {
        return x;
      }
      return std::max(x, y);
    }
  };

  //! \ingroup maxplusmat_group
  //!
  //! \brief Function object for multiplication in the max-plus semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //!  This is a stateless struct with a single call operator of signature:
  //!  `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //!  that returns \f$x \otimes y\f$ which is defined bitset_type
  //!  \f[
  //!     x\otimes y =
  //!     \begin{cases}
  //!     x + y   & \text{if } x \neq -\infty\text{ and }y \neq -\infty
  //!     \\ \mbox{}
  //!     -\infty & \text{if } x = -\infty \text{ or }y = -\infty;
  //!     \end{cases}
  //!  \f]
  //!  representing multiplication in the max-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  template <typename Scalar>
  struct MaxPlusProd {
    static_assert(std::is_signed<Scalar>::value,
                  "MaxPlus requires a signed integer type as parameter!");
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in the max-plus
    //! semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in the max-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
        return NEGATIVE_INFINITY;
      }
      return x + y;
    }
  };

  //! \ingroup maxplusmat_group
  //!
  //! \brief Function object for returning the additive identity of the
  //! max-plus semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()() const noexcept` which returns \f$-\infty\f$;
  //! representing the additive identity of the max-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  template <typename Scalar>
  struct MaxPlusZero {
    static_assert(std::is_signed<Scalar>::value,
                  "MaxPlus requires a signed integer type as parameter!");
    //! \brief Call operator for additive identity.
    //!
    //! This function returns the additive identity in the max-plus
    //! semiring.
    //!
    //! \returns The additive identity in the max-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    constexpr Scalar operator()() const noexcept {
      return NEGATIVE_INFINITY;
    }
  };

  //! \ingroup maxplusmat_group
  //!
  //! \brief Alias for dynamic max-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for the type of dynamic max-plus matrices where the dimensions of
  //! the matrices can be defined at run time.
  //!
  //! \tparam Scalar the type of the entries in the matrix.
  template <typename Scalar>
  using DynamicMaxPlusMat = DynamicMatrix<MaxPlusPlus<Scalar>,
                                          MaxPlusProd<Scalar>,
                                          MaxPlusZero<Scalar>,
                                          IntegerZero<Scalar>,
                                          Scalar>;

  //! \ingroup maxplusmat_group
  //!
  //! \brief Alias for static max-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for static max-plus matrices whose arithmetic and dimensions are
  //! defined at compile-time.
  //!
  //! \tparam R the number of rows.
  //! \tparam C the number of columns.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t R, size_t C, typename Scalar>
  using StaticMaxPlusMat = StaticMatrix<MaxPlusPlus<Scalar>,
                                        MaxPlusProd<Scalar>,
                                        MaxPlusZero<Scalar>,
                                        IntegerZero<Scalar>,
                                        R,
                                        C,
                                        Scalar>;

  //! \ingroup maxplusmat_group
  //!
  //! \brief Alias template for max-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias template for max-plus matrices.
  //!
  //! \tparam R the number of rows.  A value of \c 0 indicates that the value
  //! will be set at run time (default: \c 0).
  //!
  //! \tparam C the number of columns.  A value of \c 0 indicates that the
  //! value will be set at run time (default: \c R).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t R = 0, size_t C = R, typename Scalar = int>
  using MaxPlusMat = std::conditional_t<R == 0 || C == 0,
                                        DynamicMaxPlusMat<Scalar>,
                                        StaticMaxPlusMat<R, C, Scalar>>;

  namespace detail {
    template <size_t R, size_t C, typename Scalar>
    struct IsMaxPlusMatHelper<StaticMaxPlusMat<R, C, Scalar>> : std::true_type {
    };

    template <typename Scalar>
    struct IsMaxPlusMatHelper<DynamicMaxPlusMat<Scalar>> : std::true_type {};
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Min-plus matrices
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup minplusmat_group Min-plus matrices
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This page describes the functionality for \f$n \times n\f$  matrices
  //! over the min-plus semiring for arbitrary dimension \f$n\f$. There are
  //! two types of such matrices those whose dimension is known at
  //! compile-time, and those where it is not.  Both types can be accessed via
  //! the alias template \ref MinPlusMatAlias "MinPlusMat": if \c N has value
  //! \c 0, then the dimensions can be set at run time, otherwise \c N is the
  //! dimension. The default value of \c N is \c 0.
  //!
  //! The alias \ref MinPlusMatAlias "MinPlusMat" is either \ref StaticMatrix,
  //! \ref DynamicMatrixStaticArith "DynamicMatrix (compile-time arithmetic),
  //! or \ref DynamicMatrixDynamicArith "DynamicMatrix (run-time arithmetic)"
  //! please refer to the documentation of these class templates for more
  //! details. The only substantial difference in the interface of static and
  //! dynamic matrices is that the former can be default constructed and the
  //! latter should be constructed using the dimensions.
  //!
  //! \par Example
  //!
  //! \code
  //! // default construct an uninitialized 3 x 3 static matrix
  //! [[maybe_unused]] MinPlusMat<3> m3;
  //! // construct an uninitialized 4 x 4 dynamic matrix
  //! MinPlusMat<> m4(4, 4);
  //! \endcode

  //! \ingroup minplusmat_group
  //!
  //! \brief Function object for addition in the min-plus semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept` that returns \f$x
  //! \oplus y\f$ which is defined by
  //! \f[
  //!    x\oplus y =
  //!    \begin{cases}
  //!    \min\{x, y\}   & \text{if } x \neq \infty\text{ and }y \neq \infty
  //!    \\ \mbox{}
  //!    \infty & \text{if } x = \infty \text{ or }y = \infty;
  //!    \end{cases}
  //! \f]
  //! representing addition in the min-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  //!
  // Static arithmetic
  template <typename Scalar>
  struct MinPlusPlus {
    static_assert(std::is_signed<Scalar>::value,
                  "MinPlus requires a signed integer type as parameter!");
    //! \brief Call operator for addition.
    //!
    //! This function returns the sum of its arguments in the min-plus
    //! semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The sum of \p x and \p y in the min-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      if (x == POSITIVE_INFINITY) {
        return y;
      } else if (y == POSITIVE_INFINITY) {
        return x;
      }
      return std::min(x, y);
    }
  };

  //! \ingroup minplusmat_group
  //!
  //! \brief Function object for multiplication in the min-plus semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! that returns \f$x \otimes y\f$ which is defined by
  //! \f[
  //!     x\otimes y =
  //!     \begin{cases}
  //!     x + y  & \text{if } x \neq \infty\text{ and }y \neq \infty
  //!     \\ \mbox{}
  //!     \infty & \text{if } x = \infty \text{ or }y = \infty;
  //!     \end{cases}
  //! \f]
  //!  representing multiplication in the min-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  template <typename Scalar>
  struct MinPlusProd {
    static_assert(std::is_signed<Scalar>::value,
                  "MinPlus requires a signed integer type as parameter!");
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in the min-plus
    //! semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in the min-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      return x + y;
    }
  };

  //! \ingroup minplusmat_group
  //!
  //! \brief Function object for returning the additive identity of the
  //! min-plus semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()() const noexcept` which returns \f$\infty\f$;
  //! representing the additive identity of the min-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  template <typename Scalar>
  struct MinPlusZero {
    static_assert(std::is_signed<Scalar>::value,
                  "MinPlus requires a signed integer type as parameter!");
    //! \brief Call operator for additive identity.
    //!
    //! This function returns the additive identity in the min-plus
    //! semiring.
    //!
    //! \returns The additive identity in the min-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    constexpr Scalar operator()() const noexcept {
      return POSITIVE_INFINITY;
    }
  };

  //! \ingroup minplusmat_group
  //!
  //! \brief Alias for dynamic min-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for the type of dynamic min-plus matrices where the dimensions of
  //! the matrices can be defined at run time.
  //!
  //! \tparam Scalar the type of the entries in the matrix.
  template <typename Scalar>
  using DynamicMinPlusMat = DynamicMatrix<MinPlusPlus<Scalar>,
                                          MinPlusProd<Scalar>,
                                          MinPlusZero<Scalar>,
                                          IntegerZero<Scalar>,
                                          Scalar>;

  //! \ingroup minplusmat_group
  //!
  //! \brief Alias for static min-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for static min-plus matrices whose arithmetic and dimensions are
  //! defined at compile-time.
  //!
  //! \tparam R  the number of rows.
  //! \tparam C  the number of columns.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t R, size_t C, typename Scalar>
  using StaticMinPlusMat = StaticMatrix<MinPlusPlus<Scalar>,
                                        MinPlusProd<Scalar>,
                                        MinPlusZero<Scalar>,
                                        IntegerZero<Scalar>,
                                        R,
                                        C,
                                        Scalar>;
  //! \anchor MinPlusMatAlias
  //! \ingroup minplusmat_group
  //!
  //! \brief Alias template for min-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias template for min-plus matrices.
  //!
  //! \tparam R the number of rows.  A value of \c 0 indicates that the value
  //! will be set at run time (default: \c 0).
  //!
  //! \tparam C the number of columns.  A value of \c 0 indicates that the
  //! value will be set at run time (default: \c R).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t R = 0, size_t C = R, typename Scalar = int>
  using MinPlusMat = std::conditional_t<R == 0 || C == 0,
                                        DynamicMinPlusMat<Scalar>,
                                        StaticMinPlusMat<R, C, Scalar>>;

  namespace detail {
    template <size_t R, size_t C, typename Scalar>
    struct IsMinPlusMatHelper<StaticMinPlusMat<R, C, Scalar>> : std::true_type {
    };

    template <typename Scalar>
    struct IsMinPlusMatHelper<DynamicMinPlusMat<Scalar>> : std::true_type {};
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Max-plus matrices with threshold
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup maxplustruncmat_group Truncated max-plus matrices
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This page describes the functionality for \f$n \times n\f$  matrices
  //! over the finite quotient of the max-plus semiring by the congruence
  //! \f$t = t + 1\f$ for arbitrary \f$n\f$ and \f$t\f$. The value
  //! \f$t\f$ is referred to as the *threshold*.
  //!
  //! There are three types of such matrices where:
  //!
  //! 1. the dimension is known at compile-time;
  //! 2. the dimension is to be defined a run time but the arithmetic
  //! operations
  //!    are known at compile-time (i.e. the value of \f$t\f$ is known at
  //!    compile time)
  //! 3. both the dimension and the arithmetic operations (i.e. \f$t\f$) are
  //!    to be defined a run time.
  //!
  //! All three of these types can be accessed via the alias template
  //! \ref MaxPlusTruncMat if \c T has value \c 0,
  //! then the threshold can be set at run time, and if \c R or \c C is \c 0,
  //! then the dimension can be set at run time.  The default value of \c T is
  //! \c 0, \c R is \c 0, and of \c C is \c R.
  //!
  //! The alias \ref MaxPlusTruncMat is either
  //! StaticMatrix, \ref DynamicMatrixStaticArith "DynamicMatrix (compile-time
  //! arithmetic)", or \ref DynamicMatrixDynamicArith "DynamicMatrix (run-time
  //! arithmetic)", please refer to the documentation of these class templates
  //! for more details. The only substantial difference in the interface of
  //! static and dynamic matrices is that the former can be default
  //! constructed and the latter should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //! // construct an uninitialized 3 x 3 static matrix with threshold 11
  //! [[maybe_unused]] MaxPlusTruncMat<11, 3> m3_11;
  //! // construct an uninitialized 4 x 4 dynamic matrix with threshold 11
  //! MaxPlusTruncMat<11> m4_11(4, 4);
  //! // construct a truncated max-plus semiring with threshold 11
  //! MaxPlusTruncSemiring sr_11(11);
  //! // construct an uninitialized 5 x 5 dynamic matrix with threshold 11
  //! // (defined at run time)
  //! MaxPlusTruncMat<>  m5_11(&sr_11, 5, 5);
  //! \endcode

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Function object for multiplication in truncated max-plus
  //! semirings.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! that returns \f$x \otimes y\f$ which is defined by
  //! \f[
  //!    x\otimes y =
  //!    \begin{cases}
  //!    \min\{x + y, T\}   & \text{if } x \neq -\infty\text{ and }y \neq
  //!    -\infty \\ \mbox{}
  //!    -\infty & \text{if } x = -\infty \text{ or }y = -\infty;
  //!    \end{cases}
  //! \f]
  //! representing multiplication in the quotient of the max-plus semiring by
  //! the congruence \f$T = T + 1\f$.
  //!
  //! \tparam T the threshold (point at which the entries in the max-plus
  //! semiring are truncated).
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  template <size_t T, typename Scalar>
  struct MaxPlusTruncProd {
    static_assert(std::is_signed<Scalar>::value,
                  "MaxPlus requires a signed integer type as parameter!");
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in a max-plus
    //! truncated semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in truncated max-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= static_cast<Scalar>(T))
                           || x == NEGATIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= static_cast<Scalar>(T))
                           || y == NEGATIVE_INFINITY);
      if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
        return NEGATIVE_INFINITY;
      }
      return std::min(x + y, static_cast<Scalar>(T));
    }
  };

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Class representing a truncated max-plus semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This class represents the **truncated max-plus semiring** consists of
  //! the integers \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the
  //! **threshold** of the semiring) and \f$-\infty\f$. Instances of this
  //! class can be used to define the value of the threshold \f$t\f$ at run
  //! time.
  //!
  //! \tparam Scalar the type of the elements of the semiring. This must be a
  //! signed integer type (defaults to \c int).
  template <typename Scalar = int>
  class MaxPlusTruncSemiring {
    static_assert(std::is_signed<Scalar>::value,
                  "MaxPlus requires a signed integer type as parameter!");

   public:
    //! \brief Deleted default constructor.
    //!
    //! Deleted default constructor.
    MaxPlusTruncSemiring() = delete;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    MaxPlusTruncSemiring(MaxPlusTruncSemiring const&) noexcept = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    MaxPlusTruncSemiring(MaxPlusTruncSemiring&&) noexcept = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    MaxPlusTruncSemiring& operator=(MaxPlusTruncSemiring const&) noexcept
        = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    MaxPlusTruncSemiring& operator=(MaxPlusTruncSemiring&&) noexcept = default;

    ~MaxPlusTruncSemiring() = default;

    //! \brief Construct from threshold.
    //!
    //! Construct from threshold.
    //!
    //! \param threshold the threshold.
    //!
    //! \throws LibsemigroupsException if `threshold` is less than zero.
    //!
    //! \complexity
    //! Constant.
    explicit MaxPlusTruncSemiring(Scalar threshold) : _threshold(threshold) {
      if (threshold < 0) {
        LIBSEMIGROUPS_EXCEPTION("expected non-negative value, found {}",
                                threshold);
      }
    }

    //! \brief Get the multiplicative identity.
    //!
    //! This function returns the multiplicative identity in a truncated
    //! max-plus semiring.
    //!
    //! \returns The additive identity in a truncated max-plus semiring (the
    //! value \c 0).
    //!
    //! \exceptions
    //! \noexcept
    static constexpr Scalar scalar_one() noexcept {
      return 0;
    }

    //! \brief Get the additive identity.
    //!
    //! This function returns the additive identity in a truncated max-plus
    //! semiring.
    //!
    //! \returns The additive identity in a truncated max-plus semiring (the
    //! value `NEGATIVE_INFINITY`).
    //!
    //! \exceptions
    //! \noexcept
    static constexpr Scalar scalar_zero() noexcept {
      return NEGATIVE_INFINITY;
    }

    //! \brief Multiplication in a truncated max-plus semiring.
    //!
    //! This function returns \f$x \otimes y\f$ which is defined by
    //! \f[
    //!    x\otimes y =
    //!    \begin{cases}
    //!    \min\{x + y, t\}   & \text{if } x \neq -\infty\text{ and }y \neq
    //!    -\infty \\ \mbox{}
    //!    -\infty & \text{if } x = -\infty \text{ or }y = -\infty;
    //!    \end{cases}
    //! \f]
    //! where \f$t\f$ is the threshold; representing multiplication in the
    //! quotient of the max-plus semiring.
    //!
    //! \param x scalar.
    //! \param y scalar.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar product_no_checks(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold)
                           || x == NEGATIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold)
                           || y == NEGATIVE_INFINITY);
      if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
        return NEGATIVE_INFINITY;
      }
      return std::min(x + y, _threshold);
    }

    //! \brief Addition in a truncated max-plus semiring.
    //!
    //! Returns \f$x \oplus y\f$ which is defined by
    //! \f[
    //!   x\oplus y =
    //!   \begin{cases}
    //!     \max\{x, y\}   & \text{if } x \neq -\infty\text{ and }y \neq
    //!     -\infty
    //!     \\ \mbox{}
    //!     -\infty & \text{if } x = -\infty \text{ or }y = -\infty;
    //!   \end{cases}
    //! \f]
    //! representing addition in the max-plus semiring (and its quotient).
    //!
    //! \param x scalar.
    //! \param y scalar.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar plus_no_checks(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold)
                           || x == NEGATIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold)
                           || y == NEGATIVE_INFINITY);
      if (x == NEGATIVE_INFINITY) {
        return y;
      } else if (y == NEGATIVE_INFINITY) {
        return x;
      }
      return std::max(x, y);
    }

    //! \brief Get the threshold.
    //!
    //! Returns the threshold value used to construct
    //! \ref MaxPlusTruncSemiring instance.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar threshold() const noexcept {
      return _threshold;
    }

   public:
    Scalar const _threshold;
  };

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Alias for dynamic truncated max-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for the type of dynamic truncated max-plus matrices where the
  //! dimension is defined at run time, but the threshold is defined at
  //! compile-time.
  //!
  //! \tparam T the threshold.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t T, typename Scalar>
  using DynamicMaxPlusTruncMat = DynamicMatrix<MaxPlusPlus<Scalar>,
                                               MaxPlusTruncProd<T, Scalar>,
                                               MaxPlusZero<Scalar>,
                                               IntegerZero<Scalar>,
                                               Scalar>;

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Alias for static truncated max-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for static truncated max-plus matrices where the threshold and
  //! dimensions are defined at compile-time.
  //!
  //! \tparam T  the threshold.
  //! \tparam R  the number of rows.
  //! \tparam C  the number of columns.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t T, size_t R, size_t C, typename Scalar>
  using StaticMaxPlusTruncMat = StaticMatrix<MaxPlusPlus<Scalar>,
                                             MaxPlusTruncProd<T, Scalar>,
                                             MaxPlusZero<Scalar>,
                                             IntegerZero<Scalar>,
                                             R,
                                             C,
                                             Scalar>;
  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Alias template for truncated max-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias template for truncated max-plus matrices.
  //!
  //! \tparam T the threshold. A value of \c 0 indicates that the value will
  //! be set at run time (default: \c 0).
  //!
  //! \tparam R the number of rows.  A value of \c 0 indicates that the value
  //! will be set at run time (default: \c 0).
  //!
  //! \tparam C the number of columns.  A value of \c 0 indicates that the
  //! value will be set at run time (default: \c R).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t T = 0, size_t R = 0, size_t C = R, typename Scalar = int>
  using MaxPlusTruncMat = std::conditional_t<
      R == 0 || C == 0,
      std::conditional_t<T == 0,
                         DynamicMatrix<MaxPlusTruncSemiring<Scalar>, Scalar>,
                         DynamicMaxPlusTruncMat<T, Scalar>>,
      StaticMaxPlusTruncMat<T, R, C, Scalar>>;

  namespace detail {
    template <size_t T, size_t R, size_t C, typename Scalar>
    struct IsMaxPlusTruncMatHelper<StaticMaxPlusTruncMat<T, R, C, Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = T;
    };

    template <size_t T, typename Scalar>
    struct IsMaxPlusTruncMatHelper<DynamicMaxPlusTruncMat<T, Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = T;
    };

    template <typename Scalar>
    struct IsMaxPlusTruncMatHelper<
        DynamicMatrix<MaxPlusTruncSemiring<Scalar>, Scalar>> : std::true_type {
      static constexpr Scalar threshold = UNDEFINED;
    };

    template <typename T>
    struct IsTruncMatHelper<T, std::enable_if_t<IsMaxPlusTruncMat<T>>>
        : std::true_type {
      static constexpr typename T::scalar_type threshold
          = IsMaxPlusTruncMatHelper<T>::threshold;
    };
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Min-plus matrices with threshold
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup minplustruncmat_group Truncated min-plus matrices
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This page describes the functionality for \f$n \times n\f$  matrices
  //! over the finite quotient of the min-plus semiring by the congruence
  //! \f$t = t + 1\f$ for arbitrary \f$n\f$ and \f$t\f$. The value
  //! \f$t\f$ is referred to as the *threshold*.
  //!
  //! There are three types of such matrices where:
  //!
  //! 1. the dimension is known at compile-time;
  //! 2. the dimension is to be defined a run time but the arithmetic
  //! operations
  //!    are known at compile-time (i.e. the value of \f$t\f$ is known at
  //!    compile time)
  //! 3. both the dimension and the arithmetic operations (i.e. \f$t\f$) are
  //!    to be defined a run time.
  //!
  //! All three of these types can be accessed via the alias template
  //! \ref MinPlusTruncMat<T, P, R, C, Scalar> if \c T has value \c 0, then
  //! the threshold can be set at run time, and if \c R or \c C is \c 0, then
  //! the dimension can be set at run time. The default value of \c T is \c 0,
  //! \c R is \c 0, and of \c C is \c R.
  //!
  //! The alias \ref MinPlusTruncMat is either StaticMatrix,
  //! \ref DynamicMatrixStaticArith "DynamicMatrix (compile-time arithmetic)",
  //! or \ref DynamicMatrixDynamicArith "DynamicMatrix (run-time arithmetic)",
  //! please refer to the documentation of these class templates for more
  //! details. The only substantial difference in the interface of static
  //! and dynamic matrices is that the former can be default constructed and
  //! the latter should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //! // construct an uninitialized 3 x 3 static matrix with threshold 11
  //! [[maybe_unused]] MinPlusTruncMat<11, 3> m3_11;
  //! // construct an uninitialized 4 x 4 dynamic matrix with threshold 11
  //! MinPlusTruncMat<11> m4_11(4, 4);
  //! // construct a truncated min-plus semiring with threshold 11
  //! MinPlusTruncSemiring sr_11(11);
  //! // construct an uninitialized 5 x 5 dynamic matrix with threshold 11
  //! // (defined at run time)
  //! MinPlusTruncMat<>  m5_11(&sr_11, 5, 5);
  //! \endcode

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Function object for multiplication in min-plus truncated
  //! semirings.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept` that returns \f$x
  //! \otimes y\f$ which is defined by
  //! \f[
  //!    x\otimes y =
  //!    \begin{cases}
  //!    \min\{x + y, T\}   & \text{if } x \neq \infty\text{ and }y \neq
  //!    \infty \\ \mbox{}
  //!    \infty & \text{if } x = \infty \text{ or }y = \infty;
  //!    \end{cases}
  //! \f]
  //! representing multiplication in the quotient of the min-plus semiring
  //! by the congruence \f$T = T + 1\f$.
  //!
  //! \tparam T the threshold (point at which the entries in the min-plus
  //! semiring are truncated).
  //! \tparam Scalar the type of the values in the semiring.
  template <size_t T, typename Scalar>
  struct MinPlusTruncProd {
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in a truncated
    //! min-plus semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in truncated min-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= static_cast<Scalar>(T))
                           || x == POSITIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= static_cast<Scalar>(T))
                           || y == POSITIVE_INFINITY);
      if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      return std::min(x + y, static_cast<Scalar>(T));
    }
  };

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Class representing a truncated min-plus semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This class represents the **min-plus truncated semiring** consists of
  //! the integers \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the
  //! **threshold** of the semiring) and \f$\infty\f$. Instances of this class
  //! can be used to define the value of the threshold \f$t\f$ at run time.
  //!
  //! \tparam Scalar the type of the elements of the semiring. This must be an
  //! integral type.
  template <typename Scalar = int>
  class MinPlusTruncSemiring {
    static_assert(std::is_integral<Scalar>::value,
                  "MinPlus requires an integral type as parameter!");

   public:
    //! \brief Deleted default constructor.
    //!
    //! Deleted default constructor.
    MinPlusTruncSemiring() = delete;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    MinPlusTruncSemiring(MinPlusTruncSemiring const&) noexcept = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    MinPlusTruncSemiring(MinPlusTruncSemiring&&) noexcept = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    MinPlusTruncSemiring& operator=(MinPlusTruncSemiring const&) noexcept
        = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    MinPlusTruncSemiring& operator=(MinPlusTruncSemiring&&) noexcept = default;

    //! \brief Construct from threshold.
    //!
    //! Construct from threshold.
    //!
    //! \param threshold the threshold.
    //!
    //! \throws LibsemigroupsException if `threshold` is less than zero.
    //!
    //! \complexity
    //! Constant.
    explicit MinPlusTruncSemiring(Scalar threshold) : _threshold(threshold) {
      if (std::is_signed<Scalar>::value && threshold < 0) {
        LIBSEMIGROUPS_EXCEPTION("expected non-negative value, found {}",
                                threshold);
      }
    }

    //! \brief Get the multiplicative identity.
    //!
    //! This function returns the multiplicative identity in a truncated
    //! min-plus semiring.
    //!
    //! \returns The multiplicative identity in a truncated min-plus semiring
    //! (the value \c 0).
    //!
    //! \exceptions
    //! \noexcept
    static constexpr Scalar scalar_one() noexcept {
      return 0;
    }

    //! \brief Get the additive identity.
    //!
    //! This function returns the additive identity in a truncated min-plus
    //! semiring.
    //!
    //! \returns The additive identity in a truncated min-plus semiring (the
    //! value `POSITIVE_INFINITY`).
    //!
    //! \exceptions
    //! \noexcept
    // TODO(1) These mem fns (one and zero) aren't needed?
    static constexpr Scalar scalar_zero() noexcept {
      return POSITIVE_INFINITY;
    }

    //! \brief Multiplication in a truncated min-plus semiring.
    //!
    //! Returns \f$x \otimes y\f$ which is defined by
    //! \f[
    //!    x\otimes y =
    //!    \begin{cases}
    //!    \min\{x + y, t\}   & \text{if } x \neq \infty\text{ and }y \neq
    //!    \infty \\ \mbox{}
    //!    \infty & \text{if } x = \infty \text{ or }y = \infty;
    //!    \end{cases}
    //! \f]
    //! where \f$t\f$ is the threshold; representing multiplication in the
    //! quotient of the min-plus semiring.
    //!
    //! \param x scalar.
    //! \param y scalar.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar product_no_checks(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold)
                           || x == POSITIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold)
                           || y == POSITIVE_INFINITY);
      if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      return std::min(x + y, _threshold);
    }

    //! \brief Addition in a truncated min-plus semiring.
    //!
    //! Returns \f$x \oplus y\f$ which is defined by
    //! \f[
    //!   x\oplus y =
    //!   \begin{cases}
    //!   \min\{x, y\}   & \text{if } x \neq \infty\text{ and }y \neq \infty
    //!   \\ \mbox{}
    //!   \infty & \text{if } x = \infty \text{ or }y = \infty;
    //!   \end{cases}
    //! \f]
    //! representing addition in the min-plus truncated semiring (and its
    //! quotient).
    //!
    //! \param x scalar.
    //! \param y scalar.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar plus_no_checks(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold)
                           || x == POSITIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold)
                           || y == POSITIVE_INFINITY);
      if (x == POSITIVE_INFINITY) {
        return y;
      } else if (y == POSITIVE_INFINITY) {
        return x;
      }
      return std::min(x, y);
    }

    //! \brief Get the threshold.
    //!
    //! Returns the threshold value used to construct
    //! \ref MinPlusTruncSemiring instance.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar threshold() const noexcept {
      return _threshold;
    }

   public:
    Scalar const _threshold;
  };

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Alias for dynamic truncated min-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for the type of dynamic truncated min-plus matrices where the
  //! dimension is defined at run time, but the threshold is defined at
  //! compile-time.
  //!
  //! \tparam T the threshold.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t T, typename Scalar>
  using DynamicMinPlusTruncMat = DynamicMatrix<MinPlusPlus<Scalar>,
                                               MinPlusTruncProd<T, Scalar>,
                                               MinPlusZero<Scalar>,
                                               IntegerZero<Scalar>,
                                               Scalar>;

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Alias for static truncated min-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for static truncated min-plus matrices where the threshold and
  //! dimensions are defined at compile-time.
  //!
  //! \tparam T  the threshold.
  //! \tparam R  the number of rows.
  //! \tparam C  the number of columns.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t T, size_t R, size_t C, typename Scalar>
  using StaticMinPlusTruncMat = StaticMatrix<MinPlusPlus<Scalar>,
                                             MinPlusTruncProd<T, Scalar>,
                                             MinPlusZero<Scalar>,
                                             IntegerZero<Scalar>,
                                             R,
                                             C,
                                             Scalar>;

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Alias template for truncated min-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias template for truncated min-plus matrices.
  //!
  //! \tparam T the threshold. A value of \c 0 indicates that the value will
  //! be set at run time (default: \c 0).
  //!
  //! \tparam R the number of rows.  A value of \c 0 indicates that the value
  //! will be set at run time (default: \c 0).
  //!
  //! \tparam C the number of columns.  A value of \c 0 indicates that the
  //! value will be set at run time (default: \c R).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t T = 0, size_t R = 0, size_t C = R, typename Scalar = int>
  using MinPlusTruncMat = std::conditional_t<
      R == 0 || C == 0,
      std::conditional_t<T == 0,
                         DynamicMatrix<MinPlusTruncSemiring<Scalar>, Scalar>,
                         DynamicMinPlusTruncMat<T, Scalar>>,
      StaticMinPlusTruncMat<T, R, C, Scalar>>;

  namespace detail {
    template <size_t T, size_t R, size_t C, typename Scalar>
    struct IsMinPlusTruncMatHelper<StaticMinPlusTruncMat<T, R, C, Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = T;
    };

    template <size_t T, typename Scalar>
    struct IsMinPlusTruncMatHelper<DynamicMinPlusTruncMat<T, Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = T;
    };

    template <typename Scalar>
    struct IsMinPlusTruncMatHelper<
        DynamicMatrix<MinPlusTruncSemiring<Scalar>, Scalar>> : std::true_type {
      static constexpr Scalar threshold = UNDEFINED;
    };

    template <typename T>
    struct IsTruncMatHelper<T, std::enable_if_t<IsMinPlusTruncMat<T>>>
        : std::true_type {
      static constexpr typename T::scalar_type threshold
          = IsMinPlusTruncMatHelper<T>::threshold;
    };
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // NTP matrices
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup ntpmat_group Matrices over the natural numbers mod (t = t + p)
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This page describes the functionality for \f$n \times n\f$ matrices over
  //! the finite quotient of the usual semiring of natural number by the
  //! congruence \f$t = t + p\f$ for arbitrary \f$n\f$, \f$t\f$, and
  //! \f$p\f$. The value \f$t\f$ is referred to as the *threshold* and
  //! \f$p\f$ is called the *period*. The matrices of this type are referred
  //! to by the acronym **ntp** matrices, for "natural threshold period".  The
  //! NTPSemiring has elements \f$\{0, 1, ..., t, t +  1, ..., t
  //! + p - 1\}\f$ where  \f$t\f$, and \f$p\f$ are the threshold and period,
  //! respectively; addition and multiplication in the NTPSemiring is
  //! defined below.
  //!
  //! There are three types of such matrices where:
  //!
  //! 1. the dimension is known at compile-time;
  //! 2. the dimension is to be defined a run time but the arithmetic
  //!    operations are known at compile-time (i.e. the values of
  //!    \f$t\f$ and \f$p\f$ are known at compile time)
  //! 3. both the dimension and the arithmetic operations (i.e. \f$t\f$ and
  //!    \f$p\f$) are to be defined a run time.
  //!
  //! All three of these types can be accessed via the alias template
  //! \ref NTPMat<T, P, R, C, Scalar> if \c T and \c P have value \c 0, then
  //! the threshold and period can be set at run time, and if \c R or \c C is
  //! \c 0, then the dimension can be set at run time.  The default values of
  //! \c T, \c P, and \c R are \c 0, and the default value of \c C is \c R.
  //!
  //! The alias \ref NTPMat is one of StaticMatrix,
  //! \ref DynamicMatrixStaticArith "DynamicMatrix (compile-time arithmetic)",
  //! or \ref DynamicMatrixDynamicArith "DynamicMatrix (run-time arithmetic)";
  //! please refer to the documentation of these class templates for more
  //! details. The only substantial difference in the interface of static and
  //! dynamic matrices is that the former can be default constructed and the
  //! latter should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //! // construct an uninitialized 3 x 3 static matrix with threshold
  //! // 11, period 2
  //! [[maybe_unused]] NTPMat<11, 2, 3> m3_11_2;
  //! // construct an uninitialized 4 x 4 dynamic matrix with threshold 11,
  //! // period 2
  //! NTPMat<11, 2> m4_11_2(4, 4);
  //! // construct an ntp semiring with threshold 11, period 2
  //! NTPSemiring<> sr_11_2(11, 2);
  //! // construct an uninitialized 5 x 5 dynamic matrix with threshold 11,
  //! // period 2
  //! NTPMat<> m_5_11_2(&sr_11_2, 5, 5);
  //! \endcode

  namespace detail {
    template <size_t T, size_t P, typename Scalar>
    constexpr Scalar thresholdperiod(Scalar x) noexcept {
      if (x > T) {
        return T + (x - T) % P;
      }
      return x;
    }

    template <typename Scalar>
    inline Scalar thresholdperiod(Scalar x,
                                  Scalar threshold,
                                  Scalar period) noexcept {
      if (x > threshold) {
        return threshold + (x - threshold) % period;
      }
      return x;
    }
  }  // namespace detail

  //! \ingroup ntpmat_group
  //!
  //! \brief Function object for addition in ntp semirings.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! that returns \f$x \oplus y\f$ which is defined by
  //! \f[
  //!    x\oplus y =
  //!    \begin{cases}
  //!    x + y & \text{if } x + y \leq T \\ \mbox{}
  //!    T + ((x + y) - T \pmod{P}) & \text{if } x + y > T
  //!    \end{cases}
  //! \f]
  //! representing addition in the quotient of the semiring natural numbers by
  //! the congruence \f$(T = T + P)\f$.
  //!
  //! \tparam T the threshold.
  //! \tparam P the period.
  //! \tparam Scalar the type of the values in the semiring.
  // Static arithmetic
  template <size_t T, size_t P, typename Scalar>
  struct NTPPlus {
    //! \brief Call operator for addition.
    //!
    //! This function returns the sum of its arguments in an ntp semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The sum of \p x and \p y in an ntp semiring.
    //!
    //! \exceptions
    //! \noexcept
    constexpr Scalar operator()(Scalar x, Scalar y) const noexcept {
      return detail::thresholdperiod<T, P>(x + y);
    }
  };

  //! \ingroup ntpmat_group
  //!
  //! \brief Function object for multiplication in an ntp semirings.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! that returns \f$x \otimes y\f$ which is defined by
  //!
  //! \f[
  //!   x\otimes y =
  //!   \begin{cases}
  //!   xy & \text{if } xy \leq T \\ \mbox{}
  //!   T + ((xy - T) \pmod{P}) & \text{if } xy > T
  //!   \end{cases}
  //! \f]
  //! representing multiplication in the quotient of the semiring natural
  //! numbers by the congruence \f$(T = T + P)\f$.
  //!
  //! \tparam T the threshold.
  //! \tparam P the period.
  //! \tparam Scalar the type of the values in the semiring.
  template <size_t T, size_t P, typename Scalar>
  struct NTPProd {
    //! \brief Call operator for multiplication.
    //!
    //! Defined in `matrix.hpp`.
    //!
    //! This function returns the product of its arguments in an ntp semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in an ntp semiring.
    //!
    //! \exceptions
    //! \noexcept
    constexpr Scalar operator()(Scalar x, Scalar y) const noexcept {
      return detail::thresholdperiod<T, P>(x * y);
    }
  };

  //! \ingroup ntpmat_group
  //!
  //! \brief Class representing an ntp semiring.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This class represents the **ntp semiring** consists of the integers
  //! \f$\{0, 1, ..., t, t +  1, ..., t + p - 1\}\f$ for some  \f$t\f$ and
  //! \f$p\f$ (called the **threshold** and **period**). Instances of this
  //! class can be used to define the value of the threshold \f$t\f$ and
  //! period \f$p\f$ at run time.
  //!
  //! \tparam Scalar the type of the elements of the semiring.
  // Dynamic arithmetic
  template <typename Scalar = size_t>
  class NTPSemiring {
   public:
    //! \brief Deleted default constructor.
    //!
    //! Deleted default constructor.
    // Deleted to avoid uninitialised values of period and threshold.
    NTPSemiring() = delete;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    NTPSemiring(NTPSemiring const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    NTPSemiring(NTPSemiring&&) = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    NTPSemiring& operator=(NTPSemiring const&) = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    NTPSemiring& operator=(NTPSemiring&&) = default;

    ~NTPSemiring() = default;

    //! \brief Construct from threshold and period.
    //!
    //! Construct from threshold and period.
    //!
    //! \param t the threshold (\f$t \geq 0\f$).
    //! \param p the period (\f$p > 0\f$).
    //!
    //! \throws LibsemigroupsException if \p t is less than zero.
    //! \throws LibsemigroupsException if \p p is less than or equal to zero.
    //!
    //! \complexity
    //! Constant.
    NTPSemiring(Scalar t, Scalar p) : _period(p), _threshold(t) {
      if constexpr (std::is_signed<Scalar>::value) {
        if (t < 0) {
          LIBSEMIGROUPS_EXCEPTION(
              "expected non-negative value for 1st argument, found {}", t);
        }
      }
      if (p <= 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected positive value for 2nd argument, found {}", p);
      }
    }

    //! \brief Get the multiplicative identity.
    //!
    //! This function returns \f$1\f$; representing the multiplicative
    //! identity of the quotient of the semiring of natural numbers.
    //!
    //! \returns The multiplicative identity in an ntp semiring (the
    //! value \c 0).
    //!
    //! \exceptions
    //! \noexcept
    static constexpr Scalar scalar_one() noexcept {
      return 1;
    }

    //! \brief Get the additive identity.
    //!
    //! This function returns \f$0\f$ representing the additive identity of
    //! the quotient of the semiring of natural numbers.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    static constexpr Scalar scalar_zero() noexcept {
      return 0;
    }

    //! \brief Multiplication in an ntp semiring.
    //!
    //! This function returns \f$x \otimes y\f$ which is defined by
    //! \f[
    //!   x\otimes y =
    //!   \begin{cases}
    //!   xy & \text{if } xy \leq t \\ \mbox{}
    //!   t + ((xy - t)\pmod{p}) & \text{if } xy > t
    //!   \end{cases}
    //! \f]
    //! where \f$t\f$ is the \ref threshold and \f$p\f$ is the \ref period;
    //! representing multiplication in the quotient of the semiring of natural
    //! numbers.
    //!
    //! \param x scalar (`0 <= x < threshold() + period()`).
    //! \param y scalar (`0 <= y < threshold() + period()`).
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar product_no_checks(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT(x >= 0 && x <= _period + _threshold - 1);
      LIBSEMIGROUPS_ASSERT(y >= 0 && y <= _period + _threshold - 1);
      return detail::thresholdperiod(x * y, _threshold, _period);
    }

    //! \brief Addition in an ntp semiring.
    //!
    //! This function returns \f$x \oplus y\f$ which is defined by
    //! \f[
    //!   x\oplus y =
    //!   \begin{cases}
    //!   x + y                      & \text{if } x + y \leq t \\ \mbox{}
    //!   t + ((x + y - t) \pmod{p}) & \text{if } x + y > t
    //!   \end{cases}
    //! \f]
    //! where \f$t\f$ is the \ref threshold and \f$p\f$ is the \ref period;
    //! representing multiplication in the quotient of the semiring of natural
    //! numbers.
    //!
    //! \param x scalar (\f$0\leq x < t + p\f$).
    //! \param y scalar (\f$0\leq y < t + p\f$).
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar plus_no_checks(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT(x >= 0 && x <= _period + _threshold - 1);
      LIBSEMIGROUPS_ASSERT(y >= 0 && y <= _period + _threshold - 1);
      return detail::thresholdperiod(x + y, _threshold, _period);
    }

    //! \brief Get the threshold.
    //!
    //! Returns the threshold value used to construct \ref NTPSemiring
    //! instance.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar threshold() const noexcept {
      return _threshold;
    }

    //! \brief Get the period.
    //!
    //! Returns the period value used to construct \ref NTPSemiring
    //! instance.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar period() const noexcept {
      return _period;
    }

   private:
    Scalar _period;
    Scalar _threshold;
  };

  //! \ingroup ntpmat_group
  //!
  //! \brief Alias for ntp matrices with dynamic threshold and period.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for ntp matrices with dimensions, threshold, and period defined
  //! at runtime.
  //!
  //! \tparam Scalar the type of the entries in the matrix.
  template <typename Scalar>
  using DynamicNTPMatWithSemiring = DynamicMatrix<NTPSemiring<Scalar>, Scalar>;

  //! \ingroup ntpmat_group
  //!
  //! \brief Alias for ntp matrices with static threshold and period.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for the type of dynamic ntp matrices where the dimension
  //! is defined at run time, but the threshold and period are defined at
  //! compile-time.
  //!
  //! \tparam T       the threshold.
  //! \tparam P       the period.
  //! \tparam Scalar  the type of the entries in the matrix.
  template <size_t T, size_t P, typename Scalar>
  using DynamicNTPMatWithoutSemiring = DynamicMatrix<NTPPlus<T, P, Scalar>,
                                                     NTPProd<T, P, Scalar>,
                                                     IntegerZero<Scalar>,
                                                     IntegerOne<Scalar>,
                                                     Scalar>;

  //! \ingroup ntpmat_group
  //!
  //! \brief Alias for ntp matrices with static threshold and period, and
  //! dimensions.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for static ntp matrices where the threshold, period, and
  //! dimensions are defined at compile-time.
  //!
  //! \tparam T  the threshold.
  //!
  //! \tparam P  the period.
  //!
  //! \tparam R  the number of rows.
  //!
  //! \tparam C  the number of columns.
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t T, size_t P, size_t R, size_t C, typename Scalar>
  using StaticNTPMat = StaticMatrix<NTPPlus<T, P, Scalar>,
                                    NTPProd<T, P, Scalar>,
                                    IntegerZero<Scalar>,
                                    IntegerOne<Scalar>,
                                    R,
                                    C,
                                    Scalar>;

  //! \ingroup ntpmat_group
  //!
  //! \brief Alias template for ntp matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias template for ntp matrices.
  //!
  //! \tparam T the threshold. If both \c T and \c P are \c 0, this indicates
  //! that the value will be set at run time (default: \c 0).
  //!
  //! \tparam P the period. If both \c T and \c P are \c 0, this indicates
  //! that the value will be set at run time (default: \c 0).
  //!
  //! \tparam R the number of rows.  A value of \c 0 indicates that the value
  //! will be set at run time (default: \c 0).
  //!
  //! \tparam C the number of columns.  A value of \c 0 indicates that the
  //! value will be set at run time (default: \c R).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default:
  //! `size_t`).
  template <size_t T        = 0,
            size_t P        = 0,
            size_t R        = 0,
            size_t C        = R,
            typename Scalar = size_t>
  using NTPMat = std::conditional_t<
      R == 0 || C == 0,
      std::conditional_t<T == 0 && P == 0,
                         DynamicNTPMatWithSemiring<Scalar>,
                         DynamicNTPMatWithoutSemiring<T, P, Scalar>>,
      StaticNTPMat<T, P, R, C, Scalar>>;

  namespace detail {
    template <typename Scalar>
    struct IsNTPMatHelper<DynamicNTPMatWithSemiring<Scalar>> : std::true_type {
      static constexpr Scalar threshold = UNDEFINED;
      static constexpr Scalar period    = UNDEFINED;
    };

    template <size_t T, size_t P, typename Scalar>
    struct IsNTPMatHelper<DynamicNTPMatWithoutSemiring<T, P, Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = T;
      static constexpr Scalar period    = P;
    };

    template <size_t T, size_t P, size_t R, size_t C, typename Scalar>
    struct IsNTPMatHelper<StaticNTPMat<T, P, R, C, Scalar>> : std::true_type {
      static constexpr Scalar threshold = T;
      static constexpr Scalar period    = P;
    };

    template <typename T>
    struct IsTruncMatHelper<T, std::enable_if_t<IsNTPMat<T>>> : std::true_type {
      static constexpr typename T::scalar_type threshold
          = IsNTPMatHelper<T>::threshold;
      static constexpr typename T::scalar_type period
          = IsNTPMatHelper<T>::period;
    };
  }  // namespace detail

  namespace matrix {
    //! \brief Returns the period of a static ntp matrix.
    //!
    //! Defined in `matrix.hpp`.
    //!
    //! This function returns the template parameter \p P of a static ntp
    //! matrix.
    //!
    //! \tparam T  the threshold.
    //!
    //! \tparam P  the period.
    //!
    //! \tparam R  the number of rows.
    //!
    //! \tparam C  the number of columns.
    //!
    //! \tparam Scalar the type of the entries in the matrix.
    //!
    //! \returns The template parameter \p P.
    //!
    //! \exceptions
    //! \noexcept
    template <size_t T, size_t P, size_t R, size_t C, typename Scalar>
    constexpr Scalar period(StaticNTPMat<T, P, R, C, Scalar> const&) noexcept {
      return P;
    }

    //! \brief Returns the period of a dynamic ntp matrix.
    //!
    //! Defined in `matrix.hpp`.
    //!
    //! This function returns the template parameter \p P of a dynamic ntp
    //! matrix.
    //!
    //! \tparam T  the threshold.
    //!
    //! \tparam P  the period.
    //!
    //! \tparam Scalar the type of the entries in the matrix.
    //!
    //! \returns The template parameter \p P.
    //!
    //! \exceptions
    //! \noexcept
    template <size_t T, size_t P, typename Scalar>
    constexpr Scalar
    period(DynamicNTPMatWithoutSemiring<T, P, Scalar> const&) noexcept {
      return P;
    }

    //! \brief Returns the period of a dynamic ntp matrix.
    //!
    //! Defined in `matrix.hpp`.
    //!
    //! This function returns the period of the dynamic ntp
    //! matrix \p x using its underlying semiring.
    //!
    //! \tparam Scalar the type of the entries in the matrix.
    //!
    //! \param x the dynamic ntp matrix.
    //!
    //! \returns The period of the matrix \p x.
    //!
    //! \exceptions
    //! \noexcept
    template <typename Scalar>
    Scalar period(DynamicNTPMatWithSemiring<Scalar> const& x) noexcept {
      return x.semiring()->period();
    }
  }  // namespace matrix

  ////////////////////////////////////////////////////////////////////////
  // Projective max-plus matrices
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    template <typename T>
    class ProjMaxPlusMat : MatrixPolymorphicBase {
     public:
      using scalar_type            = typename T::scalar_type;
      using scalar_reference       = typename T::scalar_reference;
      using scalar_const_reference = typename T::scalar_const_reference;
      using semiring_type          = void;

      using container_type = typename T::container_type;
      using iterator       = typename T::iterator;
      using const_iterator = typename T::const_iterator;

      using underlying_matrix_type = T;

      using RowView = typename T::RowView;

      // Note that Rows are never normalised, and that's why we use the
      // underlying matrix Row type and not 1 x n ProjMaxPlusMat's instead
      // (since these will be normalised according to their entries, and
      // this might not correspond to the normalised entries of the matrix).
      using Row = typename T::Row;

      scalar_type scalar_one() const noexcept {
        return _underlying_mat.scalar_one();
      }

      scalar_type scalar_zero() const noexcept {
        return _underlying_mat.scalar_zero();
      }

      ////////////////////////////////////////////////////////////////////////
      // ProjMaxPlusMat - Constructors + destructor - public
      ////////////////////////////////////////////////////////////////////////

      ProjMaxPlusMat() : _is_normalized(false), _underlying_mat() {}
      ProjMaxPlusMat(ProjMaxPlusMat const&)            = default;
      ProjMaxPlusMat(ProjMaxPlusMat&&)                 = default;
      ProjMaxPlusMat& operator=(ProjMaxPlusMat const&) = default;
      ProjMaxPlusMat& operator=(ProjMaxPlusMat&&)      = default;

      ProjMaxPlusMat(size_t r, size_t c)
          : _is_normalized(false), _underlying_mat(r, c) {}

      // TODO(1) other missing constructors
      ProjMaxPlusMat(
          typename underlying_matrix_type::semiring_type const* semiring,
          size_t                                                r,
          size_t                                                c)
          : _is_normalized(false), _underlying_mat(semiring, r, c) {}

      explicit ProjMaxPlusMat(std::vector<std::vector<scalar_type>> const& m)
          : _is_normalized(false), _underlying_mat(m) {
        normalize();
      }

      ProjMaxPlusMat(
          std::initializer_list<std::initializer_list<scalar_type>> const& m)
          : ProjMaxPlusMat(
                std::vector<std::vector<scalar_type>>(m.begin(), m.end())) {}

      ~ProjMaxPlusMat() = default;

      ProjMaxPlusMat one() const {
        auto result = ProjMaxPlusMat(_underlying_mat.one());
        return result;
      }

      static ProjMaxPlusMat one(size_t n) {
        return ProjMaxPlusMat(T::one(n));
      }

      ////////////////////////////////////////////////////////////////////////
      // Comparison operators
      ////////////////////////////////////////////////////////////////////////

      bool operator==(ProjMaxPlusMat const& that) const {
        normalize();
        that.normalize();
        return _underlying_mat == that._underlying_mat;
      }

      bool operator!=(ProjMaxPlusMat const& that) const {
        return !(_underlying_mat == that._underlying_mat);
      }

      bool operator<(ProjMaxPlusMat const& that) const {
        normalize();
        that.normalize();
        return _underlying_mat < that._underlying_mat;
      }

      bool operator>(ProjMaxPlusMat const& that) const {
        return that < *this;
      }

      template <typename Thing>
      bool operator>=(Thing const& that) const {
        static_assert(IsMatrix<Thing> || std::is_same_v<Thing, RowView>);
        return that < *this || that == *this;
      }

      // not noexcept because operator< isn't
      template <typename Thing>
      bool operator<=(Thing const& that) const {
        static_assert(IsMatrix<Thing> || std::is_same_v<Thing, RowView>);
        return *this < that || that == *this;
      }

      ////////////////////////////////////////////////////////////////////////
      // Attributes
      ////////////////////////////////////////////////////////////////////////

      scalar_reference operator()(size_t r, size_t c) {
        // to ensure the returned value is normalised
        normalize();
        // to ensure that the matrix is renormalised if the returned scalar is
        // assigned.
        _is_normalized = false;
        return _underlying_mat(r, c);
      }

      scalar_reference at(size_t r, size_t c) {
        matrix::throw_if_bad_coords(*this, r, c);
        return this->operator()(r, c);
      }

      scalar_const_reference operator()(size_t r, size_t c) const {
        normalize();
        return _underlying_mat(r, c);
      }

      scalar_const_reference at(size_t r, size_t c) const {
        matrix::throw_if_bad_coords(*this, r, c);
        return this->operator()(r, c);
      }

      size_t number_of_rows() const noexcept {
        return _underlying_mat.number_of_rows();
      }

      size_t number_of_cols() const noexcept {
        return _underlying_mat.number_of_cols();
      }

      size_t hash_value() const {
        normalize();
        return Hash<T>()(_underlying_mat);
      }

      ////////////////////////////////////////////////////////////////////////
      // Arithmetic operators - in-place
      ////////////////////////////////////////////////////////////////////////

      void product_inplace_no_checks(ProjMaxPlusMat const& A,
                                     ProjMaxPlusMat const& B) {
        _underlying_mat.product_inplace_no_checks(A._underlying_mat,
                                                  B._underlying_mat);
        normalize(true);  // force normalize
      }

      void product_inplace(ProjMaxPlusMat const& A, ProjMaxPlusMat const& B) {
        _underlying_mat.product_inplace(A._underlying_mat, B._underlying_mat);
        normalize(true);  // force normalize
      }

      void plus_inplace_no_checks(ProjMaxPlusMat const& that) {
        _underlying_mat.plus_inplace_no_checks(that._underlying_mat);
        normalize(true);  // force normalize
      }

      void operator+=(ProjMaxPlusMat const& that) {
        _underlying_mat += that._underlying_mat;
        normalize(true);  // force normalize
      }

      void product_inplace_no_checks(ProjMaxPlusMat const& that) {
        _underlying_mat.product_inplace_no_checks(that._underlying_mat);
        normalize(true);  // force normalize
      }

      void operator*=(scalar_type a) {
        _underlying_mat *= a;
        normalize(true);  // force normalize
      }

      void operator+=(scalar_type a) {
        _underlying_mat += a;
        normalize(true);  // force normalize
      }

      void operator+=(RowView a) {
        _underlying_mat += a;
        normalize(true);  // force normalize
      }

      ProjMaxPlusMat operator*(scalar_type a) const {
        ProjMaxPlusMat result(*this);
        result *= a;
        return result;
      }

      ProjMaxPlusMat operator+(scalar_type a) const {
        ProjMaxPlusMat result(*this);
        result += a;
        return result;
      }

      ////////////////////////////////////////////////////////////////////////
      // Arithmetic operators - not in-place
      ////////////////////////////////////////////////////////////////////////

      ProjMaxPlusMat plus_no_checks(ProjMaxPlusMat const& that) const {
        return ProjMaxPlusMat(
            plus_no_checks(_underlying_mat, that._underlying_mat));
      }

      ProjMaxPlusMat operator+(ProjMaxPlusMat const& that) const {
        return ProjMaxPlusMat(_underlying_mat + that._underlying_mat);
      }

      ProjMaxPlusMat product_no_checks(ProjMaxPlusMat const& that) const {
        return ProjMaxPlusMat(
            product_no_checks(_underlying_mat, that._underlying_mat));
      }

      ProjMaxPlusMat operator*(ProjMaxPlusMat const& that) const {
        return ProjMaxPlusMat(_underlying_mat * that._underlying_mat);
      }

      ////////////////////////////////////////////////////////////////////////
      // Iterators
      ////////////////////////////////////////////////////////////////////////

      // The following should probably be commented out because I can't
      // currently think how to ensure that the matrix is normalised if it's
      // changed this way.

      iterator begin() noexcept {
        // to ensure the returned value is normalised
        normalize();
        // to ensure that the matrix is renormalised if the returned scalar is
        // assigned.
        _is_normalized = false;
        return _underlying_mat.begin();
      }

      iterator end() noexcept {
        // to ensure the returned value is normalised
        normalize();
        // to ensure that the matrix is renormalised if the returned scalar is
        // assigned.
        _is_normalized = false;
        return _underlying_mat.end();
      }

      const_iterator begin() const noexcept {
        normalize();
        return _underlying_mat.begin();
      }

      const_iterator end() const noexcept {
        normalize();
        return _underlying_mat.end();
      }

      const_iterator cbegin() const noexcept {
        normalize();
        return _underlying_mat.cbegin();
      }

      const_iterator cend() const noexcept {
        normalize();
        return _underlying_mat.cend();
      }

      ////////////////////////////////////////////////////////////////////////
      // Modifiers
      ////////////////////////////////////////////////////////////////////////

      void swap(ProjMaxPlusMat& that) noexcept {
        std::swap(_underlying_mat, that._underlying_mat);
      }

      void transpose() noexcept {
        _underlying_mat.transpose();
      }

      void transpose_no_checks() noexcept {
        _underlying_mat.transpose_no_checks();
      }

      ////////////////////////////////////////////////////////////////////////
      // Rows
      ////////////////////////////////////////////////////////////////////////

      RowView row(size_t i) const {
        normalize();
        return _underlying_mat.row(i);
      }

      template <typename C>
      void rows(C& x) const {
        normalize();
        return _underlying_mat.rows(x);
      }

      ////////////////////////////////////////////////////////////////////////
      // Friend functions
      ////////////////////////////////////////////////////////////////////////

      friend std::ostream& operator<<(std::ostream&         os,
                                      ProjMaxPlusMat const& x) {
        x.normalize();
        os << detail::to_string(x._underlying_mat);
        return os;
      }

      T const& underlying_matrix() const noexcept {
        normalize();
        return _underlying_mat;
      }

     private:
      explicit ProjMaxPlusMat(T&& mat)
          : _is_normalized(false), _underlying_mat(std::move(mat)) {
        normalize();
      }

      void normalize(bool force = false) const {
        if ((_is_normalized && !force)
            || (_underlying_mat.number_of_rows() == 0)
            || (_underlying_mat.number_of_cols() == 0)) {
          _is_normalized = true;
          return;
        }
        scalar_type const n = *std::max_element(_underlying_mat.cbegin(),
                                                _underlying_mat.cend());
        std::for_each(_underlying_mat.begin(),
                      _underlying_mat.end(),
                      [&n](scalar_type& s) {
                        if (s != NEGATIVE_INFINITY) {
                          s -= n;
                        }
                      });
        _is_normalized = true;
      }

      mutable bool _is_normalized;
      mutable T    _underlying_mat;
    };
  }  // namespace detail

  //! \defgroup projmaxplus_group Projective max-plus matrices
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This page describes the functionality for \f$n \times n\f$ projective
  //! matrices over the max-plus semiring. Projective max-plus matrices belong
  //! to the quotient of the monoid of all max-plus matrices by the congruence
  //! where two matrices are related if they differ by a scalar multiple; see
  //! MaxPlusMat.
  //!
  //! There are two types of such matrices those whose dimension is known at
  //! compile-time, and those where it is not.  Both types can be accessed via
  //! the alias template \ref ProjMaxPlusMat : if \c R or \c C has value
  //! \c 0, then the dimensions can be set at run time, otherwise \c R and
  //! \c C are the dimensions.  The default value of \c R is \c 0, \c C is
  //! \c R, and `Scalar` is `int`.
  //!
  //! Matrices in both these classes are modified when constructed to be in a
  //! normal form which is obtained by subtracting the maximum finite entry in
  //! the matrix from the every finite entry.
  //!
  //! A \ref ProjMaxPlusMat is neither a `StaticMatrix` nor a `DynamicMatrix`,
  //! but has the same interface as each of these types. Every instance of
  //! \ref ProjMaxPlusMat wraps a \ref MaxPlusMat.
  //!
  //! \note
  //! The types RowView and Row are the same as those
  //! in the wrapped matrix. This means that a Row object for a
  //! projective max-plus matrix is never normalised, because if they were
  //! they would be normalised according to their entries, and this might not
  //! correspond to the normalised entries of the matrix.
  //!
  //! Please refer to the documentation of these class templates for more
  //! details. The only substantial difference in the interface of
  //! static and dynamic matrices is that the former can be default
  //! constructed and the latter should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //! // default construct an uninitialized 3 x 3 static matrix
  //! ProjMaxPlusMat<3> m3;
  //! // construct an uninitialized 4 x 4 dynamic matrix
  //! ProjMaxPlusMat<>  m4(4, 4);
  //! \endcode

  //! \ingroup projmaxplus_group
  //!
  //! \brief Alias for static projective max-plus matrices with compile-time
  //! arithmetic and dimensions.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for static projective max-plus matrices whose arithmetic and
  //! dimensions are defined at compile-time.
  //!
  //! \tparam R the number of rows.
  //! \tparam C the number of columns.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t R, size_t C, typename Scalar>
  using StaticProjMaxPlusMat
      = detail::ProjMaxPlusMat<StaticMaxPlusMat<R, C, Scalar>>;

  //! \ingroup projmaxplus_group
  //!
  //! \brief Alias for dynamic projective max-plus matrices with run-time
  //! dimensions.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias for the type of dynamic projective max-plus matrices where the
  //! dimensions of the matrices can be defined at run time.
  //!
  //! \tparam Scalar the type of the entries in the matrix.
  template <typename Scalar>
  using DynamicProjMaxPlusMat
      = detail::ProjMaxPlusMat<DynamicMaxPlusMat<Scalar>>;

  //! \ingroup projmaxplus_group
  //! \brief Alias template for projective max-plus matrices.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Alias template for projective max-plus matrices.
  //!
  //! \tparam R the number of rows.  A value of \c 0 indicates that the value
  //! will be set at run time (default: \c 0).
  //!
  //! \tparam C the number of columns.  A value of \c 0 indicates that the
  //! value will be set at run time (default: \c R).
  //!
  //! \tparam Scalar The type of the entries in the matrix (default: `int`).
  template <size_t R = 0, size_t C = R, typename Scalar = int>
  using ProjMaxPlusMat = std::conditional_t<R == 0 || C == 0,
                                            DynamicProjMaxPlusMat<Scalar>,
                                            StaticProjMaxPlusMat<R, C, Scalar>>;

  namespace detail {
    template <size_t R, size_t C, typename Scalar>
    struct IsProjMaxPlusMatHelper<StaticProjMaxPlusMat<R, C, Scalar>>
        : std::true_type {};

    template <typename Scalar>
    struct IsProjMaxPlusMatHelper<DynamicProjMaxPlusMat<Scalar>>
        : std::true_type {};
  }  // namespace detail

  //! \ingroup matrix_group
  //!
  //! \brief Add a scalar to a matrix.
  //!
  //! This function returns the sum of the matrix \p x and the scalar
  //! \p a.
  //!
  //! \tparam Mat the type of the 2nd argument, must satisfy
  //! \ref IsMatrix<Mat>.
  //!
  //! \param a the scalar to add.
  //! \param x the matrix.
  //!
  //! \returns  The sum of \p a and \p x.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \warning This function does not detect overflows of `Mat::scalar_type`.
  template <typename Mat>
  auto operator+(typename Mat::scalar_type a, Mat const& x)
      -> std::enable_if_t<IsMatrix<Mat>, Mat> {
    return x + a;
  }

  //! \ingroup matrix_group
  //!
  //! \brief Multiply a matrix by a scalar.
  //!
  //! This function returns the product of the matrix \p x and the scalar
  //! \p a.
  //!
  //! \tparam Mat the type of the 2nd argument, must satisfy
  //! \ref IsMatrix<Mat>.
  //!
  //! \param a the scalar to multiply.
  //! \param x the matrix.
  //!
  //! \returns The product of \p a and \p x.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \warning This function does not detect overflows of `Mat::scalar_type`.
  template <typename Mat>
  auto operator*(typename Mat::scalar_type a, Mat const& x)
      -> std::enable_if_t<IsMatrix<Mat>, Mat> {
    return x * a;
  }
}  // namespace libsemigroups

namespace std {
  template <size_t N,
            typename Mat,
            std::enable_if_t<libsemigroups::IsMatrix<Mat>>>
  inline void swap(Mat& x, Mat& y) noexcept {
    x.swap(y);
  }
}  // namespace std

#endif  // LIBSEMIGROUPS_MATRIX_CLASS_HPP_
