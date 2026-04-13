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

#ifndef LIBSEMIGROUPS_IS_MATRIX_HPP_
#define LIBSEMIGROUPS_IS_MATRIX_HPP_

#include <type_traits>

namespace libsemigroups {
  namespace detail {

    struct MatrixPolymorphicBase {};

    template <typename T>
    struct IsMatrixHelper {
      static constexpr bool value
          = std::is_base_of<detail::MatrixPolymorphicBase, T>::value;
    };

    template <typename T>
    struct IsStaticMatrixHelper : std::false_type {};

    template <typename T>
    struct IsMatWithSemiringHelper : std::false_type {};

    template <typename S, typename T = void>
    struct IsTruncMatHelper : std::false_type {};

    template <typename T>
    static constexpr bool IsTruncMat = IsTruncMatHelper<T>::value;

    template <typename T>
    struct IsIntMatHelper : std::false_type {};

    template <typename T>
    struct IsBMatHelper : std::false_type {};

    template <typename T>
    struct IsMaxPlusMatHelper : std::false_type {};

    template <typename T>
    struct IsMinPlusMatHelper : std::false_type {};

    template <typename T>
    struct IsMaxPlusTruncMatHelper : std::false_type {};

    template <typename T>
    struct IsMinPlusTruncMatHelper : std::false_type {};

    template <typename T>
    struct IsNTPMatHelper : std::false_type {};

    template <typename T>
    struct IsProjMaxPlusMatHelper : std::false_type {};

  }  // namespace detail

  //! \ingroup matrix_group
  //!
  //! \brief Helper variable template.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c T is
  //! derived from any of \ref DynamicMatrixStaticArith
  //! "DynamicMatrix (with compile-time arithmetic)",
  //! \ref DynamicMatrixDynamicArith
  //! "DynamicMatrix (with run-time arithmetic)",
  //! or StaticMatrix; and \c false otherwise.
  //!
  //! \tparam T the type to check.
  template <typename T>
  constexpr bool IsMatrix = detail::IsMatrixHelper<T>::value;

  //! \brief Helper variable template.
  //!
  //! \ingroup matrix_group
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c T is
  //! StaticMatrix; and \c false otherwise.
  //!
  //! \tparam T the type to check.
  template <typename T>
  constexpr bool IsStaticMatrix = detail::IsStaticMatrixHelper<T>::value;

  //! \ingroup matrix_group
  //!
  //! \brief Helper variable template.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c T is
  //! DynamicMatrix; and \c false otherwise.
  //!
  //! \tparam T the type to check.
  template <typename T>
  constexpr bool IsDynamicMatrix = IsMatrix<T> && !IsStaticMatrix<T>;

  //! \ingroup matrix_group
  //!
  //! \brief Helper variable template.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \p T is
  //! DynamicMatrix<Semiring, Scalar>; and \c false otherwise.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsMatWithSemiring
      = detail::IsMatWithSemiringHelper<T>::value;

  //! \ingroup matrix_group
  //!
  //! \brief Helper variable template.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c T is
  //! \ref IntMat; and \c false otherwise.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsIntMat = detail::IsIntMatHelper<T>::value;

  //! \ingroup bmat_group
  //!
  //! \brief Helper to check if a type is \ref BMat.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c T is the
  //! same as `BMat<R, C>` for some values of \c R and \c C.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsBMat = detail::IsBMatHelper<T>::value;

  //! \ingroup matrix_group
  //!
  //! \brief Helper variable template.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c T is
  //! \ref MaxPlusMat; and \c false otherwise.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsMaxPlusMat = detail::IsMaxPlusMatHelper<T>::value;

  //! \ingroup matrix_group
  //!
  //! \brief Helper variable template.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c T is
  //! \ref MinPlusMat; and \c false otherwise.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsMinPlusMat = detail::IsMinPlusMatHelper<T>::value;

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Helper to check if a type is \ref MaxPlusTruncMat.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c T is the
  //! same as \ref MaxPlusTruncMat for some template parameters; and \c false
  //! if it is not.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsMaxPlusTruncMat
      = detail::IsMaxPlusTruncMatHelper<T>::value;

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Helper to check if a type is \ref MinPlusTruncMat.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c T is the
  //! same as \ref MinPlusTruncMat for some template parameters; and \c false
  //! if it is not.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsMinPlusTruncMat
      = detail::IsMinPlusTruncMatHelper<T>::value;

  //! \ingroup ntpmat_group
  //!
  //! \brief Helper to check if a type is \ref NTPMat.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c U is the
  //! same as \ref NTPMat<T, P, R, C, Scalar> for some values of \c T, \c P,
  //! \c R, \c C, and `Scalar`; and \c false if it is not.
  //!
  //! \tparam U the type to check.
  template <typename U>
  static constexpr bool IsNTPMat = detail::IsNTPMatHelper<U>::value;

  //! \ingroup projmaxplus_group
  //!
  //! \brief Helper to check if a type is \ref ProjMaxPlusMat.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This variable has value \c true if the template parameter \c T is the
  //! same as \ref ProjMaxPlusMat<R, C, Scalar> for some values of \c R, \c C,
  //! and \c Scalar; and \c false if it is not.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsProjMaxPlusMat
      = detail::IsProjMaxPlusMatHelper<T>::value;
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_IS_MATRIX_HPP_
