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

#ifndef LIBSEMIGROUPS_MATRIX_HELPERS_HPP_
#define LIBSEMIGROUPS_MATRIX_HELPERS_HPP_

#include <algorithm>      // for sort, fill, min
#include <bitset>         // for bitset
#include <cstddef>        // for size_t
#include <type_traits>    // for enable_if_t
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set
#include <utility>        // for move, forward
#include <vector>         // for vector

#include "bitset.hpp"        // for BitSet, IsBitSet
#include "constants.hpp"     // for UNDEFINED
#include "debug.hpp"         // for LIBSEMIGROUPS_...
#include "exception.hpp"     // for LIBSEMIGROUPS_...
#include "is-matrix.hpp"     // for IsBMat, IsStat...
#include "matrix-class.hpp"  // for DynamicNTPMatW...

#include "detail/containers.hpp"         // for StaticVector1
#include "detail/matrix-exceptions.hpp"  // for throw_if_not_s...

namespace libsemigroups {
  namespace detail {
    template <typename T>
    struct IsStdBitSetHelper : std::false_type {};

    template <size_t N>
    struct IsStdBitSetHelper<std::bitset<N>> : std::true_type {};

    template <typename T>
    static constexpr bool IsStdBitSet = IsStdBitSetHelper<T>::value;

    template <typename T>
    struct BitSetCapacity {
      static constexpr size_t value = BitSet<1>::max_size();
    };

    template <size_t R, size_t C>
    struct BitSetCapacity<StaticBMat<R, C>> {
      static_assert(R == C, "the number of rows and columns must be equal");
      static constexpr size_t value = R;
    };

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
    //! Defined in `matrix-helpers.hpp`.
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
    [[nodiscard]] constexpr auto threshold(Mat const&) noexcept
        -> std::enable_if_t<!detail::IsTruncMat<Mat>,
                            typename Mat::scalar_type> {
      return UNDEFINED;
    }

    //! \brief Returns the threshold of a matrix over a truncated semiring.
    //!
    //! Defined in `matrix-helpers.hpp`.
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
    [[nodiscard]] constexpr auto threshold(Mat const&) noexcept
        -> std::enable_if_t<detail::IsTruncMat<Mat> && !IsMatWithSemiring<Mat>,
                            typename Mat::scalar_type> {
      return detail::IsTruncMatHelper<Mat>::threshold;
    }

    //! \brief Returns the threshold of a matrix over a truncated semiring.
    //!
    //! Defined in `matrix-helpers.hpp`.
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
    [[nodiscard]] auto threshold(Mat const& x) noexcept
        -> std::enable_if_t<detail::IsTruncMat<Mat> && IsMatWithSemiring<Mat>,
                            typename Mat::scalar_type> {
      return x.semiring()->threshold();
    }

    //! \brief Returns the period of a static ntp matrix.
    //!
    //! Defined in `matrix-helpers.hpp`.
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
    [[nodiscard]] constexpr Scalar
    period(StaticNTPMat<T, P, R, C, Scalar> const&) noexcept {
      return P;
    }

    //! \brief Returns the period of a dynamic ntp matrix.
    //!
    //! Defined in `matrix-helpers.hpp`.
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
    [[nodiscard]] constexpr Scalar
    period(DynamicNTPMatWithoutSemiring<T, P, Scalar> const&) noexcept {
      return P;
    }

    //! \brief Returns the period of a dynamic ntp matrix.
    //!
    //! Defined in `matrix-helpers.hpp`.
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
    [[nodiscard]] Scalar
    period(DynamicNTPMatWithSemiring<Scalar> const& x) noexcept {
      return x.semiring()->period();
    }

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - pow
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns a power of a matrix.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function returns the matrix \p x to the power \p e.
    //! If \p e is \c 0, then the identity matrix is returned; if
    //! \p e is \c 1, then a copy of the parameter \p x is returned.
    //!
    //! \tparam Mat the type of the matrix \p x, must satisfy
    //! \ref IsMatrix<Mat>.
    //!
    //! \param x  the matrix  (must have equal number of rows and columns).
    //!
    //! \param e  the exponent (must be \f$\geq 0\f$).
    //!
    //! \returns The matrix \p x to the power \p e.
    //!
    //! \throws LibsemigroupsException if `Mat::scalar_type` is a signed type
    //! and the parameter \p e is less than \c 0.
    //!
    //! \throws LibsemigroupsException if \p x is not a square matrix.
    //!
    //! \complexity
    //! \f$O(m\log_2(e))\f$ where \f$m\f$ is the dimension of the matrix
    //! \p x and \f$e\f$ is the parameter \p e.
    //!
    //! \par Example
    //!
    //! \code
    //! auto x = make<ProjMaxPlusMat<>>({{-2, 2, 0}, {-1, 0, 0}, {1, -3,
    //! 1}});
    //! // returns {{-1, 0, -1}, {-2, -1, -2}, {-1, 0, -1}}
    //! matrix::pow(x, 100);
    //! \endcode
    // TODO(1) pow_no_checks
    // TODO(2) version that changes x in-place
    template <typename Mat>
    [[nodiscard]] Mat pow(Mat const& x, typename Mat::scalar_type e);

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - rows
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns a std::vector of row views into the rows of a dynamic
    //! matrix.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! Returns a std::vector of row views into the rows of the dynamic
    //! matrix \p x.
    //!
    //! \tparam Mat the type of \p x, must satisfy \ref IsDynamicMatrix<Mat>.
    //!
    //! \param x the matrix.
    //!
    //! \returns A std::vector of `Mat::RowView` of size `x.number_of_rows()`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is the number of rows in the matrix \p x.
    template <typename Mat, typename = std::enable_if_t<IsDynamicMatrix<Mat>>>
    [[nodiscard]] std::vector<typename Mat::RowView> rows(Mat const& x) {
      std::vector<typename Mat::RowView> container;
      x.rows(container);
      return container;
    }

    //! \brief Returns a static vector of row views into the rows of a static
    //! matrix.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! Returns a static vector of row views into the rows of the static
    //! matrix \p x.
    //!
    //! \tparam Mat the type of \p x, must satisfy \ref IsStaticMatrix<Mat>.
    //!
    //! \param x the matrix.
    //!
    //! \returns A static vector of `Mat::RowView` of size
    //! `x.number_of_rows()`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is the number of rows in the matrix \p x.
    template <typename Mat, typename = std::enable_if_t<IsStaticMatrix<Mat>>>
    [[nodiscard]] detail::StaticVector1<typename Mat::RowView, Mat::nr_rows>
    rows(Mat const& x) {
      detail::StaticVector1<typename Mat::RowView, Mat::nr_rows> container;
      x.rows(container);
      return container;
    }

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - bitset_rows
    ////////////////////////////////////////////////////////////////////////

    //! \brief Converts a container of row views of a boolean matrix to bit
    //! sets, and append them to another container.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function converts the row views (of a boolean matrix) in a
    //! container to bit sets and appends them to \p result.
    //!
    //! \tparam Mat the type of matrix. This must be a type so that
    //! IsBMat<Mat> is \c true.
    //!
    //! \tparam R an upper bound for the number of rows in `views`. This value
    //! must be at most `BitSet<1>::max_size()`.
    //!
    //! \tparam C an upper bound for the number of columns in each row
    //! represented in `views`. This value must be at most
    //! `BitSet<1>::max_size()`.
    //!
    //! \tparam Container the type of the container `views`, should be
    //! `std::vector` or `detail::StaticVector1`.
    //!
    //! \param views   a container of `Mat::RowView` or `std::vector<bool>`.
    //! \param result  a static vector of bit sets to contain the resulting
    //! bit sets.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the number of rows in `views` and
    //! and \f$n\f$ is the number of columns in any vector in `views`.
    template <typename Mat, size_t R, size_t C, typename Container>
    void bitset_rows(Container&&                          views,
                     detail::StaticVector1<BitSet<C>, R>& result);

    //! \brief Converts a container of row views of a boolean matrix to bit
    //! sets, and return them.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function converts the container of row views \p views of a
    //! boolean matrix to bit sets, and return them.
    //!
    //! \tparam Mat the type of matrix. This must be a type so that
    //! IsBMat<Mat> is \c true.
    //!
    //! \tparam R an upper bound for the number of rows in \p views. This
    //! value must be at most BitSet<1>::max_size().
    //!
    //! \tparam C an upper bound for the number of columns in each row
    //! represented in \p views. This value must be at most
    //! BitSet<1>::max_size().
    //!
    //! \tparam Container the type of the container \p views, should be
    //! `std::vector` or `detail::StaticVector1`.
    //!
    //! \param views  a container of `Mat::RowView` or `std::vector<bool>`.
    //!
    //! \returns  A value of type
    //! `detail::StaticVector1<BitSet<C>, R>`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the number of rows in \p views and
    //! and \f$n\f$ is the number of columns in any vector in \p views.
    template <typename Mat, size_t R, size_t C, typename Container>
    [[nodiscard]] auto bitset_rows(Container&& views);

    //! \brief Computes the rows of a boolean matrix as bit sets and appends
    //! them to a container.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! Computes the rows of the matrix \p x as bit sets and
    //! appends them to \p result.
    //!
    //! \tparam Mat the type of matrix. This must be a type so that
    //! IsBMat<Mat> is \c true.
    //!
    //! \tparam R an upper bound for the number of rows in `views`. This value
    //! must be at most BitSet<1>::max_size().
    //!
    //! \tparam C an upper bound for the number of columns in each row
    //! represented in `views`. This value must be at most
    //! BitSet<1>::max_size().
    //!
    //! \param x the boolean matrix.
    //!
    //! \param result the container to append the rows of \p x to.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the number of rows in \p x and and
    //! \f$n\f$ is the number of columns in \p x.
    template <typename Mat, size_t R, size_t C>
    void bitset_rows(Mat const& x, detail::StaticVector1<BitSet<C>, R>& result);

    //! \brief Computes the rows of a boolean matrix as bit sets.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This functions returns the rows of a boolean matrix as bit sets.
    //!
    //! \tparam Mat the type of matrix. Must satisfy \ref IsBMat<Mat>.
    //!
    //! \param x  the boolean matrix.
    //!
    //! \returns  A value of type `detail::StaticVector1<BitSet<C>, R>`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the number of rows in \p x and
    //! and \f$n\f$ is the number of columns in \p x.
    template <typename Mat>
    [[nodiscard]] auto bitset_rows(Mat const& x);

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - bitset_row_basis
    ////////////////////////////////////////////////////////////////////////

    //! \brief Appends a basis for the space spanned by some bitsets to a
    //! container.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! Appends a basis for the space spanned by the bitsets in \p rows to
    //! the container \p result.
    //!
    //! \tparam Mat a type such that \ref IsBMat<Mat> is \c true.
    //!
    //! \tparam Container a container type with `Container::value_type` being
    //! `BitSet<M>` or `std::bitset<M>` for some \c M.
    //!
    //! \param rows   container of spanning rows represented by bit sets.
    //! \param result  container for the resulting row basis.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(r ^ 2 c)\f$ where \f$r\f$ is the size of `rows` and
    //! \f$c\f$ is the size of each bitset in `rows`.
    // This works with std::vector and StaticVector1, with value_type equal
    // to std::bitset and BitSet.
    template <typename Mat, typename Container>
    void bitset_row_basis(Container&& rows, std::decay_t<Container>& result);

    //! \brief Returns a basis for the space spanned by some bit sets.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! Returns a basis for the space spanned by the bit sets in \p rows.
    //!
    //! \tparam Mat a type such that \ref IsBMat<Mat> is \c true.
    //!
    //! \tparam Container a container type such that `Container::value_type`
    //! is `BitSet<M>` or `std::bitset<M>` for some \c M.
    //!
    //! \param rows container of spanning rows represented by bit sets.
    //!
    //! \returns
    //! A container of type `std::decay_t<Container>` containing the row
    //! basis consisting of bit sets.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(r ^ 2 c)\f$ where \f$r\f$ is the size of \p rows and
    //! \f$c\f$ is the size of each bitset in \p rows.
    template <typename Mat, typename Container>
    [[nodiscard]] std::decay_t<Container> bitset_row_basis(Container&& rows);

    //! \brief Returns a basis for the space spanned by the rows of the
    //! boolean matrix.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function returns a basis for the space spanned by the rows of the
    //! boolean matrix \p x.
    //!
    //! \tparam Mat a type such that IsBMat<Mat> is \c true.
    //!
    //! \tparam M an upper bound for the dimensions of the returned container.
    //! If \ref IsStaticMatrix<Mat> is \c true, then \p M is the number of
    //! rows (or columns) in the square matrix \p x. Otherwise, if
    //! IsDynamicMatrix<Mat> is \c true, then \p M is BitSet<1>::max_size().
    //!
    //! \param x  the boolean matrix.
    //!
    //! \returns
    //! A container of type `detail::StaticVector1<BitSet<M>, M>>`
    //! containing the row basis of \p x consisting of bitsets.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(r ^ 2 c)\f$ where \f$r\f$ is the number of rows in \p x and
    //! \f$c\f$ is the number of columns in \p x.
    template <typename Mat, size_t M = detail::BitSetCapacity<Mat>::value>
    [[nodiscard]] detail::StaticVector1<BitSet<M>, M>
    bitset_row_basis(Mat const& x);

    //! \brief Appends a basis for the rowspace of a boolean matrix to a
    //! container.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function appends a basis for the rowspace of the boolean matrix
    //! \p x to the container \p result.
    //!
    //! \tparam Mat a type such that \ref IsBMat<Mat> is \c true.
    //!
    //! \tparam Container a container type with `Container::value_type` equal
    //! to `BitSet<M>` or `std::bitset<M>` for some \p M.
    //!
    //! \param x  the boolean matrix.
    //!
    //! \param result  container for the resulting rowbasis.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(r ^ 2 c)\f$ where \f$r\f$ is the number of rows in \p x
    //! and \f$c\f$ is the number of columns in \p x.
    template <typename Mat, typename Container>
    void bitset_row_basis(Mat const& x, Container& result);

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - row_basis - MaxPlusTruncMat
    ////////////////////////////////////////////////////////////////////////

    //! \brief Appends a basis for a space spanned by row views or bit sets to
    //! a container.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function appends a basis for the space spanned by the row
    //! views or bit sets in \p views to the container \p result.
    //!
    //! \tparam Mat a type satisfying \ref IsMaxPlusTruncMat<Mat> or
    //! \ref IsBMat<Mat>.
    //!
    //! \tparam Container a container type (such as `std::vector`, for
    //! example). The `Container::value_type` must be `Mat::RowView`, or if
    //! `Mat` is \ref BMat<N> for some \c N, then  `Container::value_type` can
    //! additionally be `BitSet<M>` or `std::bitset<M>` where \c M is
    //! greater than or equal to \c N.
    //!
    //! \param views   container of spanning row views or bit sets.
    //! \param result  container for the resulting row basis.
    //!
    //! \exceptions
    //! This function guarantees not to throw a
    //! LibsemigroupsException.
    //!
    //! \complexity
    //! \f$O(r ^ 2 c)\f$ where \f$r\f$ is the size of \p views and
    //! \f$c\f$ is the size of each row view or bit set in \p views.
    template <typename Mat, typename Container>
    std::enable_if_t<IsMaxPlusTruncMat<Mat>>
    row_basis(Container&& views, std::decay_t<Container>& result);

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - row_basis - BMat
    ////////////////////////////////////////////////////////////////////////

    // This version of row_basis for BMat's is used for compatibility
    // with the MatrixCommon framework, i.e. so that BMat's exhibit the same
    // interface/behaviour as other matrices.
    //
    // This version takes a container of row views of BMat, converts it to a
    // container of BitSets, computes the row basis using the BitSets, then
    // selects those row views in views that belong to the computed basis.

    //! \brief Appends a row space basis (consisting of row views) for the row
    //! space spanned by some rows.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function appends a basis (consisting of row views) for the row
    //! space spanned by the rows in \p views to the container \p result.
    //!
    //! \tparam Mat a type satisfying \ref IsBMat<Mat>.
    //!
    //! \tparam Container the type of a container for the rows.
    //!
    //! \param views  the spanning set of rows.
    //! \param result  the container to append the basis to the row space to.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(2) complexity
    template <typename Mat, typename Container>
    std::enable_if_t<IsBMat<Mat>> row_basis(Container&&              views,
                                            std::decay_t<Container>& result);

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - row_basis - generic helpers
    ////////////////////////////////////////////////////////////////////////

    //! \brief Appends a basis (consisting of row views) for the row space of
    //! a matrix to a container.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function appends a basis (consisting of row views) for the row
    //! space of the matrix \p x to the container \p result.
    //!
    //! \tparam Mat a type satisfying \ref IsMatrix<Mat>.
    //!
    //! \tparam Container a container type (such as std::vector).
    //!
    //! \param x  the matrix.
    //! \param result  the container to append the basis to the row space to.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(r ^ 2 c)\f$ where \f$r\f$ is the number of rows in \p x
    //! and \f$c\f$ is the number of columns in \p x.
    // Row basis of rowspace of matrix <x> appended to <result>
    template <typename Mat,
              typename Container,
              typename = std::enable_if_t<IsMatrix<Mat>>>
    void row_basis(Mat const& x, Container& result) {
      row_basis<Mat>(std::move(rows(x)), result);
    }

    //! \brief Returns a row space basis of a dynamic matrix as a std::vector
    //! of row views.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function returns a row space basis of the dynamic matrix \p x as
    //! a std::vector of row views.
    //!
    //! \tparam Mat a type satisfying IsDynamicMatrix<Mat>.
    //!
    //! \param x  the matrix.
    //! \returns The row space basis of \p x.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(r ^ 2 c)\f$ where \f$r\f$ is the number of rows in \p x
    //! and \f$c\f$ is the number of columns in \p x.
    // Row basis of rowspace of matrix <x>
    template <typename Mat, typename = std::enable_if_t<IsDynamicMatrix<Mat>>>
    [[nodiscard]] std::vector<typename Mat::RowView> row_basis(Mat const& x) {
      std::vector<typename Mat::RowView> container;
      row_basis(x, container);
      return container;
    }

    //! \brief Returns a row space basis of a static matrix as a static vector
    //! of row views.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function returns a row space basis of the static matrix \p x as a
    //! static vector of row views.
    //!
    //! \tparam Mat a type satisfying \ref IsStaticMatrix<Mat>.
    //!
    //! \param x  the matrix.
    //! \returns A static vector of `Mat::RowView`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(r ^ 2 c)\f$ where \f$r\f$ is the number of rows in \p x
    //! and \f$c\f$ is the number of columns in \p x.
    template <typename Mat, typename = std::enable_if_t<IsStaticMatrix<Mat>>>
    [[nodiscard]] detail::StaticVector1<typename Mat::RowView, Mat::nr_rows>
    row_basis(Mat const& x) {
      detail::StaticVector1<typename Mat::RowView, Mat::nr_rows> container;
      row_basis(x, container);
      return container;
    }

    //! \brief Returns a row basis for the space spanned by a container of
    //! row views.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function returns a row space basis for the space spanned by the
    //! container \p rows of row views.
    //!
    //! \tparam Mat a type satisfying \ref IsMatrix<Mat>.
    //!
    //! \tparam Container the type of the container \p rows (must have
    //! `value_type` equal to `Mat::RowView`).
    //!
    //! \param rows  a container whose `value_type` is `Mat::RowView`.
    //! \returns A container of type \p Container of `Mat::RowView`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(2) complexity
    template <typename Mat,
              typename Container,
              typename = std::enable_if_t<!IsMatrix<std::decay_t<Container>>>>
    [[nodiscard]] std::decay_t<Container> row_basis(Container&& rows);

    // TODO move
    template <typename T>
    struct RowSum {
      void operator()(T& res, T const& pt, T const& x) const {
        res = pt;
        res += x;
      }
    };

    // TODO(doc)
    template <typename Mat,
              typename Container,
              typename = std::enable_if_t<IsMatrix<Mat>>>
    void row_basis_rows(Mat const& x, Container& result);

    // TODO(doc)
    template <typename Mat,
              typename Container,
              typename = std::enable_if_t<IsStaticMatrix<Mat>>>
    [[nodiscard]] detail::StaticVector1<typename Mat::Row, Mat::nr_rows>
    row_basis_rows(Mat const& x);

    // TODO(doc)
    template <typename Mat,
              typename Container,
              typename = std::enable_if_t<IsDynamicMatrix<Mat>>>
    [[nodiscard]] std::vector<typename Mat::Row> row_basis_rows(Mat const& x) {
      std::vector<typename Mat::Row> container;
      row_basis_rows(x, container);
      return container;
    }

    // This modifies the argument rows by moving out those in the basis
    template <typename Mat,
              typename Container,
              typename = std::enable_if_t<IsStaticMatrix<Mat>>>
    [[nodiscard]] detail::StaticVector1<typename Mat::Row, Mat::nr_rows>
    row_basis_rows(Container&& rows);

    template <typename Mat,
              typename Container,
              typename = std::enable_if_t<IsDynamicMatrix<Mat>>>
    [[nodiscard]] std::vector<typename Mat::Row>
    row_basis_rows(Container&& rows);

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - row_space_size
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns the size of the row space of a boolean matrix.
    //!
    //! Defined in `matrix-helpers.hpp`.
    //!
    //! This function returns the size of the row space of the boolean matrix
    //! \p x. This is currently only implemented for types of matrix `Mat`
    //! satisfying \ref IsBMat<Mat>.
    //!
    //! \tparam Mat the type of \p x, must satisfy \ref IsBMat<Mat>.
    //! \param x  the matrix.
    //!
    //! \returns  The size of the row space of the matrix \p x.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the size of the row basis of \p x
    //! and \f$n\f$ is the size of the row space.
    //!
    //! \warning
    //! If \f$k\f$ is the number of rows of the matrix \p x, then
    //! \f$n\f$ can be as large as \f$2 ^ k\f$.
    //!
    //! \par Example
    //!
    //! \code
    //! auto x = make<BMat<>>({{1, 0, 0}, {0, 0, 1}, {0, 1, 0}});
    //! matrix::row_space_size(x); // returns 7
    //! \endcode
    template <typename Mat, typename = std::enable_if_t<IsBMat<Mat>>>
    [[nodiscard]] size_t row_space_size(Mat const& x);
  }  // namespace matrix
}  // namespace libsemigroups

#include "matrix-helpers.tpp"
#endif  // LIBSEMIGROUPS_MATRIX_HELPERS_HPP_
