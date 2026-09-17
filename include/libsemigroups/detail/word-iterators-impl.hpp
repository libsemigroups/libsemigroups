//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 Joseph Edwards
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

// This file contains declarations of the impl versions of iterator classes for
// words.

#ifndef LIBSEMIGROUPS_DETAIL_WORD_ITERATORS_IMPL_HPP_
#define LIBSEMIGROUPS_DETAIL_WORD_ITERATORS_IMPL_HPP_

#include <iterator>  // for forward_iterator_tag
#include <vector>    // for vector, vector<>::const_pointer

#include "iterator.hpp"             // for default_postfix_increment
#include "libsemigroups/types.hpp"  // for word_type

namespace libsemigroups {
  namespace detail {

    //////////////////////////////////////////////////////////////////////
    // wilo = words in lex order
    //////////////////////////////////////////////////////////////////////

    class const_wilo_iterator_impl {
     public:
      using size_type       = typename std::vector<word_type>::size_type;
      using difference_type = typename std::vector<word_type>::difference_type;
      using const_pointer   = typename std::vector<word_type>::const_pointer;
      using pointer         = typename std::vector<word_type>::pointer;
      using const_reference = typename std::vector<word_type>::const_reference;
      using reference       = typename std::vector<word_type>::reference;
      using value_type      = word_type;
      using iterator_category = std::forward_iterator_tag;

     protected:
      word_type   _current;
      size_type   _index;
      letter_type _letter;
      size_type   _upper_bound;
      word_type   _last;
      size_type   _number_letters;

     public:
      const_wilo_iterator_impl();
      const_wilo_iterator_impl(const_wilo_iterator_impl const&);
      const_wilo_iterator_impl(const_wilo_iterator_impl&&);
      const_wilo_iterator_impl& operator=(const_wilo_iterator_impl const&);
      const_wilo_iterator_impl& operator=(const_wilo_iterator_impl&&);
      ~const_wilo_iterator_impl();

      const_wilo_iterator_impl(size_type   n,
                               size_type   upper_bound,
                               word_type&& first,
                               word_type&& last);

      template <typename Iterator>
      const_wilo_iterator_impl(size_type n,
                               size_type upper_bound,
                               Iterator  first_begin,
                               Iterator  first_end,
                               Iterator  last_begin,
                               Iterator  last_end)
          : const_wilo_iterator_impl() {
        init(n, upper_bound, first_begin, first_end, last_begin, last_end);
      }

      void init(size_type   n,
                size_type   upper_bound,
                word_type&& first,
                word_type&& last);

      template <typename Iterator>
      void init(size_type n,
                size_type upper_bound,
                Iterator  first_begin,
                Iterator  first_end,
                Iterator  last_begin,
                Iterator  last_end) {
        _current.assign(first_begin, first_end);
        _last.assign(last_begin, last_end);
        init(n, upper_bound);
      }

      [[nodiscard]] bool
      operator==(const_wilo_iterator_impl const& that) const noexcept {
        return _index == that._index;
      }

      [[nodiscard]] bool
      operator!=(const_wilo_iterator_impl const& that) const noexcept {
        return !(this->operator==(that));
      }

      [[nodiscard]] const_reference operator*() const noexcept {
        return _current;
      }

      [[nodiscard]] const_pointer operator->() const noexcept {
        return &_current;
      }

      // prefix
      const_wilo_iterator_impl const& operator++();

      // postfix
      const_wilo_iterator_impl operator++(int) {
        return default_postfix_increment<const_wilo_iterator_impl>(*this);
      }

      void swap(const_wilo_iterator_impl& that);

     private:
      void init(size_type n, size_type upper_bound);
    };

    inline void swap(const_wilo_iterator_impl& x, const_wilo_iterator_impl& y) {
      x.swap(y);
    }

    //////////////////////////////////////////////////////////////////////
    // wislo = words in shortlex order
    //////////////////////////////////////////////////////////////////////
    class const_wislo_iterator_impl {
     public:
      using size_type       = typename std::vector<word_type>::size_type;
      using difference_type = typename std::vector<word_type>::difference_type;
      using const_pointer   = typename std::vector<word_type>::const_pointer;
      using pointer         = typename std::vector<word_type>::pointer;
      using const_reference = typename std::vector<word_type>::const_reference;
      using reference       = typename std::vector<word_type>::reference;
      using value_type      = word_type;
      using iterator_category = std::forward_iterator_tag;

     protected:
      word_type _current;
      size_type _index;
      word_type _last;
      size_type _number_letters;

     public:
      const_wislo_iterator_impl();
      const_wislo_iterator_impl(const_wislo_iterator_impl const&);
      const_wislo_iterator_impl(const_wislo_iterator_impl&&);
      const_wislo_iterator_impl& operator=(const_wislo_iterator_impl const&);
      const_wislo_iterator_impl& operator=(const_wislo_iterator_impl&&);
      ~const_wislo_iterator_impl();

      const_wislo_iterator_impl(size_type   n,
                                word_type&& first,
                                word_type&& last);

      template <typename Iterator>
      const_wislo_iterator_impl(size_type n,
                                Iterator  first_begin,
                                Iterator  first_end,
                                Iterator  last_begin,
                                Iterator  last_end)
          : const_wislo_iterator_impl() {
        init(n, first_begin, first_end, last_begin, last_end);
      }

      void init(size_type n, word_type&& first, word_type&& last);

      template <typename Iterator>
      void init(size_type n,
                Iterator  first_begin,
                Iterator  first_end,
                Iterator  last_begin,
                Iterator  last_end) {
        _current.assign(first_begin, first_end);
        _last.assign(last_begin, last_end);
        init(n);
      }

      [[nodiscard]] bool
      operator==(const_wislo_iterator_impl const& that) const noexcept {
        return _index == that._index;
      }

      [[nodiscard]] bool
      operator!=(const_wislo_iterator_impl const& that) const noexcept {
        return !(this->operator==(that));
      }

      [[nodiscard]] const_reference operator*() const noexcept {
        return _current;
      }

      [[nodiscard]] const_pointer operator->() const noexcept {
        return &_current;
      }

      // prefix
      const_wislo_iterator_impl const& operator++();

      // postfix
      const_wislo_iterator_impl operator++(int) {
        return detail::default_postfix_increment<const_wislo_iterator_impl>(
            *this);
      }

      void swap(const_wislo_iterator_impl& that);

     private:
      void init(size_type n);
    };

    inline void swap(const_wislo_iterator_impl& x,
                     const_wislo_iterator_impl& y) {
      x.swap(y);
    }
  }  // namespace detail
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_DETAIL_WORD_ITERATORS_IMPL_HPP_
