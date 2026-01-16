//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 James D. Mitchell
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

// This file contains the struct is_specialization_of and the alias
// is_specialization_of_v.

// TODO(v4): rename this file to use "-" and not "_" for consistency!

#ifndef LIBSEMIGROUPS_IS_SPECIALIZATION_OF_HPP_
#define LIBSEMIGROUPS_IS_SPECIALIZATION_OF_HPP_

#include <type_traits>  // for true_type

namespace libsemigroups {
  //! \ingroup types_group
  //!
  //! \brief Struct for checking if a template parameter is a specialization of
  //! another.
  //!
  //! This struct contains the unique member `value` which indicates whether or
  //! not the template parameter \p Thing is a specialization of the template
  //! template parameter \p Primary.
  //! The implementation is based on
  //! [this](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r0.pdf).
  //!
  //! \tparam Thing the type we are checking is a specialization of \p Primary.
  //! \tparam Primary the type we are checking \p Thing against.
  //!
  //! \par Example
  //! \code
  //! is_specialization_of_v<std::vector<int>, std::vector>; //-> true
  //! is_specialization_of_v<std::vector<int>,std::unordered_map>; //-> false
  //! \endcode
  //!
  //! \note The template parameters of \p Primary must be types, so, for
  //! example, `is_specialization_of<std::array<int, 2>, std::array>` will not
  //! compile.
  //!
  //! \sa is_specialization_of_v
  // From https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r0.pdf
  template <typename Thing, template <typename...> typename Primary>
  struct is_specialization_of : std::false_type {};

  //! \copydoc is_specialization_of
  template <template <typename...> typename Primary, typename... Args>
  struct is_specialization_of<Primary<Args...>, Primary> : std::true_type {};

  //! \ingroup types_group
  //!
  //! \brief Helper variable template for \ref is_specialization_of.
  //!
  //! This helper is just a short version of
  //! `is_specialization_of<Thing, Primary>::value`.
  //!
  //! \tparam Thing the type we are checking is a specialization of \p Primary.
  //! \tparam Primary the type we are checking \p Thing against.
  //!
  //! \par Example
  //! \code
  //! is_specialization_of_v<std::vector<int>, std::vector>; //-> true
  //! is_specialization_of_v<std::vector<int>, std::unordered_map>; //-> false
  //! \endcode
  //!
  //! \note The template parameters of \p Primary must be types, so, for
  //! example, `is_specialization_of<std::array<int, 2>, std::array>` will not
  //! compile.
  template <typename Thing, template <typename...> typename Primary>
  inline constexpr bool is_specialization_of_v
      = is_specialization_of<Thing, Primary>::value;
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_IS_SPECIALIZATION_OF_HPP_
