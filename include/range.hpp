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

// This file contains a class for creating ranges of integers, for use with
// SchreierSims.

#ifndef LIBSEMIGROUPS_INCLUDE_RANGE_HPP_
#define LIBSEMIGROUPS_INCLUDE_RANGE_HPP_

#include <type_traits>
#include <vector>

namespace libsemigroups {
  template <typename TIntegralType, size_t TBegin = 0, size_t TEnd = 0>
  class IntegralRange {
    static_assert(
        std::is_integral<TIntegralType>::value,
        "the template parameter TIntegralType must be an integral type");

   public:
    using value_type = TIntegralType;
    using size_type = TIntegralType;

    IntegralRange() : _begin(TBegin), _end(TEnd) {}

    IntegralRange(TIntegralType begin, TIntegralType end)
        : _begin(begin), _end(end) {}

    class iterator {
      friend class IntegralRange;

     using size_type = typename std::vector<TIntegralType>::size_type;
      using difference_type =
          typename std::vector<TIntegralType>::difference_type;
      using const_pointer = typename std::vector<TIntegralType>::const_pointer;
      using pointer       = typename std::vector<TIntegralType>::pointer;
      using const_reference =
          typename std::vector<TIntegralType>::const_reference;
      using reference         = const_reference;
      using value_type        = TIntegralType;
      using iterator_category = std::random_access_iterator_tag;

     public:
      TIntegralType operator*() const {
        return _i;
      }

      iterator const& operator++() {
        ++_i;
        return *this;
      }

      iterator operator++(int) {
        iterator copy(*this);
        ++_i;
        return copy;
      }

      bool operator==(iterator const& other) const {
        return _i == other._i;
      }

      bool operator!=(iterator const& other) const {
        return _i != other._i;
      }

      bool operator<(iterator const& other) const {
        return _i < other._i;
      }

      iterator operator+(size_type val) const {
        iterator out(*this);
        out._i += val;
        return out;
      }

     protected:
      explicit iterator(TIntegralType start) : _i(start) {}

     private:
      TIntegralType _i;
    };

    iterator cbegin() const {
      return _begin;
    }

    iterator cend() const {
      return _end;
    }

    using const_iterator = iterator;

   private:
    iterator _begin;
    iterator _end;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_RANGE_HPP_
