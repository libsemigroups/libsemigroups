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

// This file contains declarations of some ranges (in the sense of rx-ranges)
// that augment those in rx-ranges.

#ifndef LIBSEMIGROUPS_RANGES_HPP_
#define LIBSEMIGROUPS_RANGES_HPP_

#include <algorithm>    // for is_sorted
#include <cstddef>      // for size_t
#include <functional>   // for less
#include <ostream>      // for operator<<, ostream
#include <random>       // for mt19937, random_device, uniform_int_distribution
#include <type_traits>  // for enable_if_t
#include <utility>      // for forward

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <rx/ranges.hpp>  // for count, iterator_range
#pragma GCC diagnostic pop

#include "detail/string.hpp"  // for to_string

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Custom ranges
  ////////////////////////////////////////////////////////////////////////

  // TODO(doc)
  struct random {
    template <typename InputRange>
    struct Range {
      using output_type = typename InputRange::output_type;

      static constexpr bool is_finite     = true;
      static constexpr bool is_idempotent = false;

      bool       _at_end;
      InputRange _input;

      explicit constexpr Range(InputRange&& input) noexcept
          : _at_end(false), _input(std::move(input)) {}

      [[nodiscard]] output_type get() const noexcept;

      constexpr void next() noexcept {
        _at_end = true;
      }

      [[nodiscard]] constexpr bool at_end() const noexcept {
        return _at_end;
      }

      [[nodiscard]] constexpr size_t size_hint() const noexcept {
        return 1;
      }
    };

    template <typename InputRange>
    [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
      using Inner = rx::get_range_type_t<InputRange>;
      return Range<Inner>(std::forward<InputRange>(input));
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Comparison functions
  ////////////////////////////////////////////////////////////////////////

  // TODO(doc)
  template <typename Range, typename Compare>
  constexpr bool is_sorted(Range r, Compare comp);

  // TODO(doc)
  template <typename Range>
  constexpr bool is_sorted(Range r) {
    return std::is_sorted(r, std::less<>());
  }

  // TODO(doc)
  template <typename Range1, typename Range2>
  constexpr bool equal(Range1 r1, Range2 r2);

  // TODO(doc)
  template <typename Range1, typename Range2>
  constexpr bool lexicographical_compare(Range1 r1, Range2 r2);

  // TODO(doc)
  template <typename Range1, typename Range2>
  constexpr bool shortlex_compare(Range1 r1, Range2 r2);

  ////////////////////////////////////////////////////////////////////////
  // Custom combinators
  ////////////////////////////////////////////////////////////////////////

  // TODO(doc)
  template <typename T>
  auto chain(T const& x, T const& y) {
    return rx::chain(rx::iterator_range(x.cbegin(), x.cend()),
                     rx::iterator_range(y.cbegin(), y.cend()));
  }

  // TODO(doc)
  template <typename T>
  auto enumerate(T const& thing) {
    return rx::enumerate(rx::iterator_range(thing));
  }

  ////////////////////////////////////////////////////////////////////////
  // String representation
  ////////////////////////////////////////////////////////////////////////

  // TODO(doc)
  // A << function for ranges
  template <typename Range,
            typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
  std::ostream& operator<<(std::ostream& os, Range r);

}  // namespace libsemigroups

#include "ranges.tpp"

#endif  // LIBSEMIGROUPS_RANGES_HPP_
