//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 Joseph Edwards
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

// This file contains declarations of the iterator classes for words.

#ifndef LIBSEMIGROUPS_WORD_ITERATORS_HPP_
#define LIBSEMIGROUPS_WORD_ITERATORS_HPP_

#include <vector>

#include "iterator.hpp"             // for default_postfix_increment
#include "libsemigroups/types.hpp"  // for word_type

namespace libsemigroups {
  namespace detail {
    class const_wilo_iterator {
     public:
      using size_type       = typename std::vector<word_type>::size_type;
      using difference_type = typename std::vector<word_type>::difference_type;
      using const_pointer   = typename std::vector<word_type>::const_pointer;
      using pointer         = typename std::vector<word_type>::pointer;
      using const_reference = typename std::vector<word_type>::const_reference;
      using reference       = typename std::vector<word_type>::reference;
      using value_type      = word_type;
      using iterator_category = std::forward_iterator_tag;

     private:
      word_type   _current;
      size_type   _index;
      letter_type _letter;
      size_type   _upper_bound;
      word_type   _last;
      size_type   _number_letters;

     public:
      const_wilo_iterator() noexcept;
      const_wilo_iterator(const_wilo_iterator const&);
      const_wilo_iterator(const_wilo_iterator&&) noexcept;
      const_wilo_iterator& operator=(const_wilo_iterator const&);
      const_wilo_iterator& operator=(const_wilo_iterator&&) noexcept;
      ~const_wilo_iterator();

      const_wilo_iterator(size_type   n,
                          size_type   upper_bound,
                          word_type&& first,
                          word_type&& last);

      [[nodiscard]] bool
      operator==(const_wilo_iterator const& that) const noexcept {
        return _index == that._index;
      }

      [[nodiscard]] bool
      operator!=(const_wilo_iterator const& that) const noexcept {
        return !(this->operator==(that));
      }

      [[nodiscard]] const_reference operator*() const noexcept {
        return _current;
      }

      [[nodiscard]] const_pointer operator->() const noexcept {
        return &_current;
      }

      // prefix
      const_wilo_iterator const& operator++() noexcept;

      // postfix
      const_wilo_iterator operator++(int) noexcept {
        return default_postfix_increment<const_wilo_iterator>(*this);
      }

      void swap(const_wilo_iterator& that) noexcept;
    };

    class const_wislo_iterator {
     public:
      using size_type       = typename std::vector<word_type>::size_type;
      using difference_type = typename std::vector<word_type>::difference_type;
      using const_pointer   = typename std::vector<word_type>::const_pointer;
      using pointer         = typename std::vector<word_type>::pointer;
      using const_reference = typename std::vector<word_type>::const_reference;
      using reference       = typename std::vector<word_type>::reference;
      using value_type      = word_type;
      using iterator_category = std::forward_iterator_tag;

     private:
      word_type _current;
      size_type _index;
      word_type _last;
      size_type _number_letters;

     public:
      const_wislo_iterator() noexcept;
      const_wislo_iterator(const_wislo_iterator const&);
      const_wislo_iterator(const_wislo_iterator&&) noexcept;
      const_wislo_iterator& operator=(const_wislo_iterator const&);
      const_wislo_iterator& operator=(const_wislo_iterator&&) noexcept;

      const_wislo_iterator(size_type n, word_type&& first, word_type&& last);

      ~const_wislo_iterator();

      [[nodiscard]] bool
      operator==(const_wislo_iterator const& that) const noexcept {
        return _index == that._index;
      }

      [[nodiscard]] bool
      operator!=(const_wislo_iterator const& that) const noexcept {
        return !(this->operator==(that));
      }

      [[nodiscard]] const_reference operator*() const noexcept {
        return _current;
      }

      [[nodiscard]] const_pointer operator->() const noexcept {
        return &_current;
      }

      // prefix
      const_wislo_iterator const& operator++() noexcept;

      // postfix
      const_wislo_iterator operator++(int) noexcept {
        return detail::default_postfix_increment<const_wislo_iterator>(*this);
      }

      void swap(const_wislo_iterator& that) noexcept;
    };

    inline void swap(const_wilo_iterator& x, const_wilo_iterator& y) noexcept {
      x.swap(y);
    }

    inline void swap(const_wislo_iterator& x,
                     const_wislo_iterator& y) noexcept {
      x.swap(y);
    }
  }  // namespace detail
}  // namespace libsemigroups
#endif