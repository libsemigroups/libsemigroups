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

namespace libsemigroups {
  namespace detail {

    struct MatrixPolymorphicBase {};

    template <typename T>
    struct IsMatrixHelper {
      static constexpr bool value
          = std::is_base_of<detail::MatrixPolymorphicBase, T>::value;
    };

    template <typename T>
    struct IsIntMatHelper : std::false_type {};

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
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_IS_MATRIX_HPP_
