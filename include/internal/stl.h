//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// This file contains some functionality missing in some implementations of the
// stl, or to augment the stl implementations.

#ifndef LIBSEMIGROUPS_INCLUDE_INTERNAL_STL_H_
#define LIBSEMIGROUPS_INCLUDE_INTERNAL_STL_H_

#include <functional>
#include <memory>
#include <sstream>

namespace libsemigroups {
  template <typename TValueType, typename = void> struct hash {
    size_t operator()(TValueType const& x) const {
      return std::hash<TValueType>()(x);
    }
  };

  template <typename TValueType, typename = void> struct equal_to {
    size_t operator()(TValueType const& x, TValueType const& y) const {
      return std::equal_to<TValueType>()(x, y);
    }
  };

  // Forward declaration
  template <typename T> std::string to_string(const T& n);

  // A << method for vectors
  template <typename T>
  std::ostringstream& operator<<(std::ostringstream&   os,
                                 std::vector<T> const& vec) {
    os << "{";
    for (auto it = vec.cbegin(); it < vec.cend() - 1; it++) {
      os << libsemigroups::to_string(*it) << ", ";
    }
    os << libsemigroups::to_string(*(vec.cend() - 1)) << "}";
    return os;
  }

  //! Returns a string representing an object of type \c T.
  //!
  //! It appears that GCC 4.9.1 (at least) do not have std::to_string
  //! implemented, so we implement our own. This requires the operator \c <<
  //! to be implemented for an \c ostringstream& and const T& element.
  template <typename T> std::string to_string(const T& n) {
    std::ostringstream stm;
    stm << n;
    return stm.str();
  }

  // C++11 is missing make_unique. The following implementation is from Item 21
  // in "Effective Modern C++" by Scott Meyers.
  template <typename T, typename... Ts>
  std::unique_ptr<T> make_unique(Ts&&... params) {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_INTERNAL_STL_H_
