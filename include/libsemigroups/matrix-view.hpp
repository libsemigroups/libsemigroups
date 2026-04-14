//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_MATRIX_VIEW_HPP_
#define LIBSEMIGROUPS_MATRIX_VIEW_HPP_

#include <cstddef>  // for size_t

#include "detail/matrix-common.hpp"  // for RowViewCommon

namespace libsemigroups {
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
#endif

  ////////////////////////////////////////////////////////////////////////
  // StaticRowViews - static arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! \brief Class for views into a row of a matrix over a semiring.
  //!
  //! Defined in `matrix-view.hpp`.
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
  //! Defined in `matrix-view.hpp`.
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
  //! Defined in `matrix-view.hpp`.
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
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_MATRIX_VIEW_HPP_
