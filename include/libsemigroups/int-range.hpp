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

// This file contains a class for creating ranges of integers.

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
  template <typename T, T TBegin = 0, T TEnd = 0>
  class IntegralRange final {
    static_assert(std::is_integral<T>::value,
                  "the template parameter value_type must be an integral type");

   public:
    using value_type      = T;
    using size_type       = typename std::vector<value_type>::size_type;
    using difference_type = typename std::vector<value_type>::difference_type;
    using const_pointer   = typename std::vector<value_type>::const_pointer;
    using pointer         = typename std::vector<value_type>::pointer;
    using const_reference = typename std::vector<value_type>::const_reference;
    using reference       = const_reference;

    IntegralRange() noexcept : _begin(TBegin), _end(TEnd) {}

    IntegralRange(value_type begin, value_type end) noexcept
        : _begin(begin), _end(end) {}

    IntegralRange(IntegralRange const&) noexcept = default;
    IntegralRange(IntegralRange&&) noexcept      = default;
    IntegralRange& operator=(IntegralRange const&) noexcept = default;
    IntegralRange& operator=(IntegralRange&&) noexcept = default;

    value_type operator[](size_t i) const noexcept {
      return _begin + i;
    }

    class const_iterator final {
      friend class IntegralRange;

     public:
      using value_type      = T;
      using size_type       = typename std::vector<value_type>::size_type;
      using difference_type = typename std::vector<value_type>::difference_type;
      using const_pointer   = typename std::vector<value_type>::const_pointer;
      using pointer         = typename std::vector<value_type>::pointer;
      using const_reference = typename std::vector<value_type>::const_reference;
      using reference       = value_type;
      using iterator_category = std::random_access_iterator_tag;

      const_iterator() noexcept                      = default;
      const_iterator(const_iterator const&) noexcept = default;
      const_iterator(const_iterator&&) noexcept      = default;
      const_iterator& operator=(const_iterator const&) noexcept = default;
      const_iterator& operator=(const_iterator&&) noexcept = default;

      ~const_iterator() = default;

      bool operator==(const_iterator const& that) const noexcept {
        return _i == that._i;
      }

      bool operator!=(const_iterator const& that) const noexcept {
        return _i != that._i;
      }

      bool operator<(const_iterator const& that) const noexcept {
        return _i < that._i;
      }

      bool operator>(const_iterator const& that) const noexcept {
        return _i > that._i;
      }

      bool operator<=(const_iterator const& that) const noexcept {
        return operator<(that) || operator==(that);
      }

      bool operator>=(const_iterator const& that) const noexcept {
        return operator>(that) || operator==(that);
      }

      const_iterator const& operator++() noexcept {
        ++_i;
        return *this;
      }

      const_iterator operator++(int) noexcept {
        const_iterator copy(*this);
        ++_i;
        return copy;
      }

      const_iterator operator--(int) noexcept {
        const_iterator  tmp(*this);
        const_iterator::operator--();
        return tmp;
      }

      const_iterator operator+(size_type val) const noexcept {
        const_iterator out(*this);
        out._i += val;
        return out;
      }

      friend const_iterator operator+(size_type             val,
                                      const_iterator const& it) noexcept {
        return it + val;
      }

      const_iterator operator-(size_type val) const noexcept {
        const_iterator out(*this);
        return out -= val;
      }

      const_iterator& operator--() noexcept {
        --_i;
        return *this;
      }

      const_iterator& operator+=(size_type val) noexcept {
        _i += val;
        return *this;
      }

      const_iterator& operator-=(size_type val) noexcept {
        _i -= val;
        return *this;
      }

      difference_type operator-(const_iterator that) const noexcept {
        return _i - that._i;
      }

      value_type operator[](size_type pos) const noexcept {
        return *(*this + pos);
      }

      value_type operator*() const noexcept {
        return _i;
      }

      const_pointer operator->() const noexcept {
        return &_i;
      }

     private:
      explicit const_iterator(value_type start) : _i(start) {}
      value_type _i;
    };

    const_iterator cbegin() const {
      return _begin;
    }

    const_iterator cend() const {
      return _end;
    }

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    const_reverse_iterator crbegin() const {
      return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
      return const_reverse_iterator(cbegin());
    }

   private:
    const_iterator _begin;
    const_iterator _end;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INT_RANGE_HPP_
