//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains a class for creating ranges of integers, which are
// random access.

// TODO(later):
// 1. doc
// 2. tests

#ifndef LIBSEMIGROUPS_INT_RANGE_HPP_
#define LIBSEMIGROUPS_INT_RANGE_HPP_

#include <cstddef>      // for size_t
#include <iterator>     // for random_access_iterator_tag
#include <type_traits>  // for is_integral
#include <vector>       // for vector

namespace libsemigroups {
  namespace detail {
    template <typename T>
    class IntRange {
      static_assert(
          std::is_integral_v<T>,
          "the template parameter value_type must be an integral type");

     public:
      using value_type      = T;
      using size_type       = typename std::vector<value_type>::size_type;
      using difference_type = typename std::vector<value_type>::difference_type;
      using const_pointer   = typename std::vector<value_type>::const_pointer;
      using pointer         = typename std::vector<value_type>::pointer;
      using const_reference = typename std::vector<value_type>::const_reference;
      using reference       = const_reference;

      constexpr IntRange(value_type begin, value_type end) noexcept
          : _begin(begin), _end(end) {}

      constexpr IntRange() noexcept                           = default;
      constexpr IntRange(IntRange const&) noexcept            = default;
      constexpr IntRange(IntRange&&) noexcept                 = default;
      constexpr IntRange& operator=(IntRange const&) noexcept = default;
      constexpr IntRange& operator=(IntRange&&) noexcept      = default;

      [[nodiscard]] constexpr value_type operator[](size_t i) const noexcept {
        return _begin + i;
      }

      class const_iterator {
        friend class IntRange;

       public:
        using value_type = T;
        using size_type  = typename std::vector<value_type>::size_type;
        using difference_type =
            typename std::vector<value_type>::difference_type;
        using const_pointer = typename std::vector<value_type>::const_pointer;
        using pointer       = typename std::vector<value_type>::pointer;
        using const_reference =
            typename std::vector<value_type>::const_reference;
        using reference         = value_type;
        using iterator_category = std::random_access_iterator_tag;

        constexpr const_iterator() noexcept                      = default;
        constexpr const_iterator(const_iterator const&) noexcept = default;
        constexpr const_iterator(const_iterator&&) noexcept      = default;
        constexpr const_iterator& operator=(const_iterator const&) noexcept
            = default;
        constexpr const_iterator& operator=(const_iterator&&) noexcept
            = default;

        ~const_iterator() = default;

        [[nodiscard]] constexpr bool
        operator==(const_iterator const& that) const noexcept {
          return _i == that._i;
        }

        [[nodiscard]] constexpr bool
        operator!=(const_iterator const& that) const noexcept {
          return _i != that._i;
        }

        [[nodiscard]] constexpr bool
        operator<(const_iterator const& that) const noexcept {
          return _i < that._i;
        }

        [[nodiscard]] constexpr bool
        operator>(const_iterator const& that) const noexcept {
          return _i > that._i;
        }

        [[nodiscard]] constexpr bool
        operator<=(const_iterator const& that) const noexcept {
          return operator<(that) || operator==(that);
        }

        [[nodiscard]] constexpr bool
        operator>=(const_iterator const& that) const noexcept {
          return operator>(that) || operator==(that);
        }

        constexpr const_iterator const& operator++() noexcept {
          ++_i;
          return *this;
        }

        [[nodiscard]] constexpr const_iterator operator++(int) noexcept {
          const_iterator copy(*this);
          ++_i;
          return copy;
        }

        [[nodiscard]] constexpr const_iterator operator--(int) noexcept {
          const_iterator  tmp(*this);
          const_iterator::operator--();
          return tmp;
        }

        [[nodiscard]] constexpr const_iterator
        operator+(size_type val) const noexcept {
          const_iterator out(*this);
          out._i += val;
          return out;
        }

        [[nodiscard]] friend constexpr const_iterator
        operator+(size_type val, const_iterator const& it) noexcept {
          return it + val;
        }

        [[nodiscard]] constexpr const_iterator
        operator-(size_type val) const noexcept {
          const_iterator out(*this);
          return out -= val;
        }

        constexpr const_iterator& operator--() noexcept {
          --_i;
          return *this;
        }

        constexpr const_iterator& operator+=(size_type val) noexcept {
          _i += val;
          return *this;
        }

        constexpr const_iterator& operator-=(size_type val) noexcept {
          _i -= val;
          return *this;
        }

        [[nodiscard]] constexpr difference_type
        operator-(const_iterator that) const noexcept {
          return _i - that._i;
        }

        [[nodiscard]] constexpr value_type
        operator[](size_type pos) const noexcept {
          return *(*this + pos);
        }

        [[nodiscard]] constexpr value_type operator*() const noexcept {
          return _i;
        }

        [[nodiscard]] constexpr const_pointer operator->() const noexcept {
          return &_i;
        }

       private:
        explicit constexpr const_iterator(value_type start) : _i(start) {}
        value_type _i;
      };

      [[nodiscard]] constexpr const_iterator cbegin() const {
        return _begin;
      }

      [[nodiscard]] constexpr const_iterator cend() const {
        return _end;
      }

      [[nodiscard]] constexpr const_iterator begin() const {
        return _begin;
      }

      [[nodiscard]] constexpr const_iterator end() const {
        return _end;
      }

      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      [[nodiscard]] constexpr const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
      }

      [[nodiscard]] constexpr const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
      }

      [[nodiscard]] constexpr const_reverse_iterator rbegin() const {
        return const_reverse_iterator(cend());
      }

      [[nodiscard]] constexpr const_reverse_iterator rend() const {
        return const_reverse_iterator(cbegin());
      }

     private:
      const_iterator _begin;
      const_iterator _end;
    };

    template <typename T>
    IntRange(T, T) -> IntRange<T>;

  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INT_RANGE_HPP_
