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

// This file contains the implementations of the function templates in
// exception.hpp.

namespace libsemigroups {
  namespace detail {
    template <typename Iterator, typename Map, typename Ignore>
    std::pair<Iterator, size_t>
    find_duplicates(Iterator first, Iterator last, Map& seen, Ignore&& ignore) {
      seen.clear();
      size_t pos = 0;
      for (auto it = first; it != last; ++it, ++pos) {
        if (!ignore(*it)) {
          auto [prev_it, inserted] = seen.emplace(*it, pos);
          if (!inserted) {
            return std::pair(it, prev_it->second);
          }
        }
      }
      return std::pair(last, pos);
    }

    template <typename Iterator>
    std::pair<Iterator, size_t> find_duplicates(Iterator first, Iterator last) {
      std::unordered_map<std::decay_t<decltype(*first)>, size_t> seen;
      return find_duplicates(first, last, seen);
    }

    template <typename Iterator, typename Ignore>
    bool has_duplicates(Iterator first, Iterator last, Ignore&& ignore) {
      std::unordered_map<std::decay_t<decltype(*first)>, size_t> seen;
      return find_duplicates(first, last, seen, std::forward<Ignore>(ignore))
                 .first
             != last;
    }

    template <typename Iterator, typename Ignore>
    void throw_if_duplicates(Iterator         first,
                             Iterator         last,
                             std::string_view what,
                             Ignore&&         ignore) {
      std::unordered_map<std::decay_t<decltype(*first)>, size_t> seen;
      auto [it, pos]
          = find_duplicates(first, last, seen, std::forward<Ignore>(ignore));
      if (it != last) {
        LIBSEMIGROUPS_EXCEPTION("duplicate {}, found {} in position {}, first "
                                "occurrence in position {}",
                                what,
                                to_printable(*it),
                                std::distance(first, it),
                                pos);
        // TODO include [first, last) in the exception message, "duplicate {}
        // in {}", requires adding a to_printable for iterators
      }
    }

    template <typename Iterator, typename Sentinel>
    void throw_if_any_not_less(Iterator         first,
                               Sentinel         last,
                               size_t           upper,
                               std::string_view prefix) {
      throw_if_any_not_in_range(first, last, 0, upper, prefix);
    }

    template <typename Iterator, typename Sentinel>
    void throw_if_any_not_in_range(Iterator         first,
                                   Sentinel         last,
                                   size_t           lower,
                                   size_t           upper,
                                   std::string_view prefix) {
      static_assert(std::is_same_v<Iterator, Sentinel>
                    || std::is_same_v<Sentinel, rx::input_range_iterator_end>);
      size_t pos = 0;
      for (auto it = first; it != last; ++it, ++pos) {
        if (*it >= upper || *it < lower) {
          LIBSEMIGROUPS_EXCEPTION("{}value out of bounds, expected value in "
                                  "the range [{}, {}), got {} in position {}",
                                  prefix,
                                  lower,
                                  upper,
                                  *it,
                                  pos);
          // NOTE: can't use std::distance here because it doesn't work when
          // Sentinel != Iterator.
        }
      }
    }

  }  // namespace detail
}  // namespace libsemigroups
