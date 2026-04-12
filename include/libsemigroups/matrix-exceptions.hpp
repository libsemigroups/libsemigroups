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

// TODO(v4): it's not clear why some of these functions are in "matrix" and
// others in "detail", maybe better to move them all into detail, and then this
// file into the detail dir?

#ifndef LIBSEMIGROUPS_MATRIX_EXCEPTIONS_HPP_
#define LIBSEMIGROUPS_MATRIX_EXCEPTIONS_HPP_

#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "is-matrix.hpp"  // for IsMatrix
#include "matrix.hpp"     // for IntMat

#include "detail/matrix-common.hpp"      // for entry_repr
#include "detail/matrix-exceptions.hpp"  // for throw_if_semiring_nullptr

namespace libsemigroups::matrix {

  //! \ingroup intmat_group
  //!
  //! \brief Check that an integer matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function throws an exception if the entries of an integer matrix
  //! are not valid, which is if and only if any of the entries equal
  //! \ref POSITIVE_INFINITY or \ref NEGATIVE_INFINITY.
  //!
  //! \tparam Mat the type of the argument \p x, must satisfy
  //! \ref IsIntMat<Mat>.
  //!
  //! \param x the matrix to check.
  // TODO why isn't this just overloaded throw_if_bad_entry(IntMat const& x)??
  template <size_t R, size_t C, typename Scalar>
  void throw_if_bad_entry(IntMat<R, C, Scalar> const& x);

  //! \ingroup intmat_group
  //!
  //! \brief Check that an entry in an integer matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function throws an exception if the entry \p val of an integer
  //! matrix are not valid, which is if and only if the entry \p val equals
  //! \ref POSITIVE_INFINITY or \ref NEGATIVE_INFINITY.
  //!
  //! The 1st argument is used for overload resolution.
  //!
  //! \tparam Mat the type of the 1st argument, must satisfy
  //! \ref IsIntMat<Mat>.
  //!
  //! \param val the entry to check.
  //!
  //! \deprecated_warning{function}
  // NOTE this function isn't used and so should be deleted
  template <typename Mat>
  [[deprecated]] std::enable_if_t<IsIntMat<Mat>>
  throw_if_bad_entry(Mat const&, typename Mat::scalar_type val);

  //! \ingroup bmat_group
  //!
  //! \brief Check the entries in a boolean matrix are valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that a matrix contains values in
  //! the underlying semiring.
  //!
  //! \tparam Mat the type of \p m, must satisfy \ref IsBMat<Mat>.
  //!
  //! \param m the boolean matrix to check.
  //!
  //! \throws LibsemigroupsException if any of the entries in the matrix are
  //! not \c 0 or \c 1. The values in a boolean matrix are of type \c int,
  //! but a matrix shouldn't contain values except \c 0 and \c 1.
  template <typename Mat>
  std::enable_if_t<IsBMat<Mat>> throw_if_bad_entry(Mat const& m);

  //! \ingroup bmat_group
  //!
  //! \brief Check an entry in a boolean matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that a matrix contains values in
  //! the underlying semiring.
  //!
  //! \tparam Mat the type of the 1st argument, must satisfy
  //! \ref IsBMat<Mat>.
  //!
  //! \param val the entry to check.
  //!
  //! \throws LibsemigroupsException if \p val is not \c 0 or \c 1. The
  //! values in a boolean matrix are of type \c int, but a matrix shouldn't
  //! contain values except \c 0 and \c 1.
  //!
  //! \deprecated_warning{function}
  // NOTE this function isn't used and so should be deleted
  template <typename Mat>
  [[deprecated]] std::enable_if_t<IsBMat<Mat>>
  throw_if_bad_entry(Mat const&, typename Mat::scalar_type val);

  //! \ingroup maxplusmat_group
  //!
  //! \brief Check that a max-plus matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that a matrix contains values in
  //! the underlying semiring.
  //!
  //! \tparam Mat the type of the matrix, must satisfy \ref IsMaxPlusMat.
  //!
  //! \param x the matrix to check.
  //!
  //! \throws LibsemigroupsException if \p x contains
  //! \ref POSITIVE_INFINITY.
  template <typename Mat>
  auto throw_if_bad_entry(Mat const& x) -> std::enable_if_t<IsMaxPlusMat<Mat>>;

  //! \ingroup maxplusmat_group
  //!
  //! \brief Check that an entry in a max-plus matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that an entry in a matrix
  //! belongs to the underlying semiring.
  //!
  //! \tparam Mat the type of the matrix, must satisfy
  //! \ref IsMaxPlusMat<Mat>.
  //!
  //! \param val the entry to check.
  //!
  //! \throws LibsemigroupsException if \p val is \ref POSITIVE_INFINITY.
  //!
  //! \deprecated_warning{function}
  // NOTE this function isn't used and so should be deleted
  template <typename Mat>
  [[deprecated]] std::enable_if_t<IsMaxPlusMat<Mat>>
  throw_if_bad_entry(Mat const&, typename Mat::scalar_type val);

  //! \ingroup minplusmat_group
  //!
  //! \brief Check that a min-plus matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that a matrix contains values in
  //! the underlying semiring.
  //!
  //! \tparam Mat the type of the matrix, must satisfy \ref IsMinPlusMat.
  //!
  //! \param x the matrix to check.
  //!
  //! \throws LibsemigroupsException if \p x contains
  //! \ref NEGATIVE_INFINITY.
  template <typename Mat>
  std::enable_if_t<IsMinPlusMat<Mat>> throw_if_bad_entry(Mat const& x);

  //! \ingroup minplusmat_group
  //!
  //! \brief Check that an entry in a min-plus matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that an entry in a matrix
  //! belongs to the underlying semiring.
  //!
  //! \tparam Mat the type of the matrix, must satisfy
  //! \ref IsMinPlusMat<Mat>.
  //!
  //! \param val the entry to check.
  //!
  //! \throws LibsemigroupsException if \p val is \ref NEGATIVE_INFINITY.
  //!
  //! \deprecated_warning{function}
  // NOTE this function isn't used and so should be deleted
  template <typename Mat>
  [[deprecated]] std::enable_if_t<IsMinPlusMat<Mat>>
  throw_if_bad_entry(Mat const&, typename Mat::scalar_type val);

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Check that a truncated max-plus matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that a matrix contains values in
  //! the underlying semiring.
  //!
  //! \tparam Mat the type of \p m, must satisfy
  //! \ref IsMaxPlusTruncMat<Mat>.
  //!
  //! \param m the matrix to check.
  //!
  //! \throws LibsemigroupsException if any entry in the matrix is not in
  //! the set \f$\{0, 1, \ldots, t, -\infty\}\f$ where \f$t\f$ is the
  //! threshold of the matrix or if the underlying semiring is not defined
  //! (only applies to matrices with run time arithmetic).
  template <typename Mat>
  std::enable_if_t<IsMaxPlusTruncMat<Mat>> throw_if_bad_entry(Mat const& m) {
    // TODO(1) to tpp
    detail::throw_if_semiring_nullptr(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    auto it = std::find_if_not(m.cbegin(), m.cend(), [t](scalar_type x) {
      return x == NEGATIVE_INFINITY || (0 <= x && x <= t);
    });
    if (it != m.cend()) {
      auto [r, c] = m.coords(it);
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}, {} (= {})}} "
          "but found {} in entry ({}, {})",
          t,
          entry_repr(NEGATIVE_INFINITY),
          static_cast<scalar_type>(NEGATIVE_INFINITY),
          detail::entry_repr(*it),
          r,
          c);
    }
  }

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Check that an entry in a truncated max-plus matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that a matrix contains values in
  //! the underlying semiring.
  //!
  //! \tparam Mat the type the 1st argument, must satisfy
  //! \ref IsMaxPlusTruncMat<Mat>.
  //!
  //! \param m the matrix.
  //! \param val the entry to check.
  //!
  //! \throws LibsemigroupsException if the entry \p val is not in
  //! the set \f$\{0, 1, \ldots, t, -\infty\}\f$ where \f$t\f$ is the
  //! threshold of the matrix or if the underlying semiring is not defined
  //! (only applies to matrices with run time arithmetic).
  // TODO deprecate
  template <typename Mat>
  std::enable_if_t<IsMaxPlusTruncMat<Mat>>
  throw_if_bad_entry(Mat const& m, typename Mat::scalar_type val) {
    detail::throw_if_semiring_nullptr(m);
    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    if (val == POSITIVE_INFINITY || 0 > val || val > t) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}, -{} (= {})}} "
          "but found {}",
          t,
          entry_repr(NEGATIVE_INFINITY),
          static_cast<scalar_type>(NEGATIVE_INFINITY),
          detail::entry_repr(val));
    }
  }

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Check that a truncated min-plus matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that a matrix contains values in
  //! the underlying semiring.
  //!
  //! \tparam Mat the type of the matrix \p m (must satisfy
  //! \ref IsMinPlusTruncMat<Mat>).
  //!
  //! \param m the matrix to check.
  //!
  //! \throws LibsemigroupsException if any entry in the matrix is not in
  //! the set \f$\{0, 1, \ldots, t, \infty\}\f$ where \f$t\f$ is the
  //! threshold of the matrix or if the underlying semiring is not defined
  //! (only applies to matrices with run time arithmetic).
  // TODO(1) to tpp
  template <typename Mat>
  std::enable_if_t<IsMinPlusTruncMat<Mat>> throw_if_bad_entry(Mat const& m) {
    // Check that the semiring pointer isn't the nullptr if it shouldn't be
    detail::throw_if_semiring_nullptr(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    auto it = std::find_if_not(m.cbegin(), m.cend(), [t](scalar_type x) {
      return x == POSITIVE_INFINITY || (0 <= x && x <= t);
    });
    if (it != m.cend()) {
      uint64_t r, c;
      std::tie(r, c) = m.coords(it);

      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}, {} (= {})}} "
          "but found {} in entry ({}, {})",
          t,
          detail::entry_repr(POSITIVE_INFINITY),
          static_cast<scalar_type>(POSITIVE_INFINITY),
          detail::entry_repr(*it),
          r,
          c);
    }
  }

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Check that an entry in a truncated min-plus matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that a matrix contains values in
  //! the underlying semiring.
  //!
  //! \tparam Mat the type the 1st argument, must satisfy
  //! \ref IsMinPlusTruncMat<Mat>.
  //!
  //! \param m the matrix.
  //! \param val the entry to check.
  //!
  //! \throws LibsemigroupsException if the entry \p val is not in
  //! the set \f$\{0, 1, \ldots, t, \infty\}\f$ where \f$t\f$ is the
  //! threshold of the matrix or if the underlying semiring is not defined
  //! (only applies to matrices with run time arithmetic).
  // TODO deprecate
  template <typename Mat>
  std::enable_if_t<IsMinPlusTruncMat<Mat>>
  throw_if_bad_entry(Mat const& m, typename Mat::scalar_type val) {
    detail::throw_if_semiring_nullptr(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    if (!(val == POSITIVE_INFINITY || (0 <= val && val <= t))) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}, {} (= {})}} "
          "but found {}",
          t,
          detail::entry_repr(POSITIVE_INFINITY),
          static_cast<scalar_type>(POSITIVE_INFINITY),
          detail::entry_repr(val));
    }
  }
  //! \ingroup ntpmat_group
  //!
  //! \brief Check that the entries in an ntp matrix are valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that a matrix contains values in
  //! the underlying semiring.
  //!
  //! \tparam Mat the type of the matrix \p m (must satisfy
  //! \ref IsNTPMat<Mat>).
  //!
  //! \param m the matrix to check.
  //!
  //! \throws LibsemigroupsException if any entry in the matrix is not in
  //! the set \f$\{0, 1, \ldots, t + p - 1\}\f$ where \f$t\f$ is the
  //! threshold, and \f$p\f$ the period, of the matrix.
  //!
  //! \throws LibsemigroupsException if the underlying semiring is not
  //! defined (only applies to matrices with run time arithmetic).
  template <typename Mat>
  std::enable_if_t<IsNTPMat<Mat>> throw_if_bad_entry(Mat const& m) {
    detail::throw_if_semiring_nullptr(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    scalar_type const p = matrix::period(m);
    auto it = std::find_if_not(m.cbegin(), m.cend(), [t, p](scalar_type x) {
      return (0 <= x && x < p + t);
    });
    if (it != m.cend()) {
      uint64_t r, c;
      std::tie(r, c) = m.coords(it);

      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}}}, but "
          "found {} in entry ({}, {})",
          p + t,
          detail::entry_repr(*it),
          r,
          c);
    }
  }

  //! \ingroup ntpmat_group
  //!
  //! \brief Check that an entry in an ntp matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that a matrix contains values in
  //! the underlying semiring.
  //!
  //! \tparam Mat the type of the 1st argument (must satisfy
  //! \ref IsNTPMat<Mat>).
  //!
  //! \param m the matrix.
  //! \param val the entry to check.
  //!
  //! \throws LibsemigroupsException if the entry \p val is not in
  //! the set \f$\{0, 1, \ldots, t + p - 1\}\f$ where \f$t\f$ is the
  //! threshold, and \f$p\f$ the period, of the matrix.
  //!
  //! \throws LibsemigroupsException if the underlying semiring is not
  //! defined (only applies to matrices with run time arithmetic).
  // TODO deprecate
  template <typename Mat>
  std::enable_if_t<IsNTPMat<Mat>>
  throw_if_bad_entry(Mat const& m, typename Mat::scalar_type val) {
    detail::throw_if_semiring_nullptr(m);
    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    scalar_type const p = matrix::period(m);
    if (val < 0 || val >= p + t) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}}}, but "
          "found {}",
          p + t,
          detail::entry_repr(val));
    }
  }

  //! \ingroup projmaxplus_group
  //!
  //! \brief Check that the entries in a projective max-plus matrix are
  //! valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that the matrix \p x contains
  //! values in the underlying semiring, by checking the underlying matrix.
  //!
  //! \tparam Mat the type of the parameter (must satisfy
  //! \ref IsProjMaxPlusMat<Mat>).
  //!
  //! \param x the matrix to check.
  //!
  //! \throws LibsemigroupsException if
  //! `throw_if_bad_entry(x.underlying_matrix())` throws.
  template <typename Mat>
  constexpr std::enable_if_t<IsProjMaxPlusMat<Mat>>
  throw_if_bad_entry(Mat const& x) {
    throw_if_bad_entry(x.underlying_matrix());
  }

  //! \ingroup projmaxplus_group
  //!
  //! \brief Check that an entry in a projective max-plus matrix is valid.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function can be used to check that the matrix \p x contains
  //! values in the underlying semiring, using the underlying matrix.
  //!
  //! \tparam Mat the type of the parameter (must satisfy
  //! \ref IsProjMaxPlusMat<Mat>).
  //!
  //! \param x the matrix.
  //! \param val the entry.
  //!
  //! \throws LibsemigroupsException if
  //! `throw_if_bad_entry(x.underlying_matrix(), val)` throws.
  template <typename Mat>
  constexpr std::enable_if_t<IsProjMaxPlusMat<Mat>>
  throw_if_bad_entry(Mat const& x, typename Mat::scalar_type val) {
    throw_if_bad_entry(x.underlying_matrix(), val);
  }

}  // namespace libsemigroups::matrix

#include "matrix-exceptions.tpp"
#endif  // LIBSEMIGROUPS_MATRIX_EXCEPTIONS_HPP_
