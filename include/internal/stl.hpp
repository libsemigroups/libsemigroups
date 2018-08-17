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

#ifndef LIBSEMIGROUPS_INCLUDE_INTERNAL_STL_HPP_
#define LIBSEMIGROUPS_INCLUDE_INTERNAL_STL_HPP_

#include <functional>
#include <memory>
#include <vector>
#include <sstream>
#include <type_traits>

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
      os << to_string(*it) << ", ";
    }
    os << to_string(*(vec.cend() - 1)) << "}";
    return os;
  }

  // A << method for pairs
  template <typename T, typename S>
  std::ostringstream& operator<<(std::ostringstream&    os,
                                 std::pair<T, S> const& pair) {
    os << "{" << to_string(pair.first) << ", " << to_string(pair.second) << "}";
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

  // Since std::is_invocable is only introduced in C++17, we use this
  // from: https://stackoverflow.com/q/15393938/
  // Only works if there are no overloads of operator() in type T.
  template <typename T, typename = void>
  struct is_callable : std::is_function<T> {};

  template <typename T>
  struct is_callable<
      T,
      typename std::enable_if<
          std::is_same<decltype(void(&T::operator())), void>::value>::type>
      : std::true_type {};

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_INTERNAL_STL_HPP_
