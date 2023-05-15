//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains TODO

// TODO implement "cartesian" combinator for rx-ranges
// TODO add typename = std::enable_if_t<rx::is_input_or_sink_v<T>>> everywhere

#ifndef LIBSEMIGROUPS_RANGES_HPP_
#define LIBSEMIGROUPS_RANGES_HPP_

#include <algorithm>

#include "detail/string.hpp"  // for detail::to_string

#include <rx/ranges.hpp>

namespace libsemigroups {

  template <typename Range, typename Compare>
  constexpr bool is_sorted(Range r, Compare comp) {
    if (!r.at_end()) {
      auto first = r.get();
      r.next();
      while (!r.at_end()) {
        auto second = r.get();
        if (comp(second, first)) {
          return false;
        }
        first = second;
        r.next();
      }
    }
    return true;
  }

  template <typename Range>
  constexpr bool is_sorted(Range r) {
    return std::is_sorted(r, std::less<>());
  }

  template <typename Range1, typename Range2>
  constexpr bool equal(Range1 r1, Range2 r2) {
    while (!r1.at_end()) {
      if (r1.get() != r2.get()) {
        return false;
      }
      r1.next();
      r2.next();
    }
    return true;
  }

  template <typename Range1, typename Range2>
  bool lexicographical_compare(Range1 r1, Range2 r2) {
    while (!r1.at_end() && !r2.at_end()) {
      auto next1 = r1.get();
      auto next2 = r2.get();
      if (next1 < next2) {
        return true;
      }
      if (next2 < next1) {
        return false;
      }
      r1.next();
      r2.next();
    }
    return r1.at_end() && !r2.at_end();
  }

  template <typename Range1, typename Range2>
  bool shortlex_compare(Range1 r1, Range2 r2) {
    size_t n1 = rx::count()(r1);
    size_t n2 = rx::count()(r2);
    if (n1 != n2) {
      return n1 < n2;
    }
    return lexicographical_compare(r1, r2);
  }

  // A << function for ranges
  template <typename Range,
            typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
  std::ostream& operator<<(std::ostream& os, Range r) {
    os << "{{";  // {{ is an escaped single { for fmt
    std::string sep = "";
    for (auto const& item : r) {
      os << sep << detail::to_string(item);
      sep = ", ";
    }
    os << "}}";
    return os;
  }

  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  auto chain(T const& x, T const& y) {
    return rx::chain(rx::iterator_range(x.cbegin(), x.cend()),
                     rx::iterator_range(y.cbegin(), y.cend()));
  }

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_RANGES_HPP_
