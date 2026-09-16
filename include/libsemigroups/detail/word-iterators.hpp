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

// This file contains declarations of the iterator classes for words.

#ifndef LIBSEMIGROUPS_DETAIL_WORD_ITERATORS_HPP_
#define LIBSEMIGROUPS_DETAIL_WORD_ITERATORS_HPP_

#include <algorithm>   // for make_heap, push_heap, pop_heap
#include <functional>  // for std::function
#include <iterator>    // for forward_iterator_tag
#include <utility>     // for move
#include <vector>      // for vector, vector<>::const_pointer

#include "iterator.hpp"                        // for default_postfix_increment
#include "libsemigroups/alphabet-helpers.hpp"  // for Alphabet, has_alphabet
#include "libsemigroups/constants.hpp"         // for UNDEFINED
#include "libsemigroups/exception.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/order.hpp"             // for lex_cmp
#include "libsemigroups/types.hpp"             // for word_type

#include "libsemigroups/detail/citow.hpp"                // for citow
#include "libsemigroups/detail/word-iterators-impl.hpp"  // for const_wilo_i...

namespace libsemigroups {
  namespace detail {
    //////////////////////////////////////////////////////////////////////
    // wio = words in order
    //////////////////////////////////////////////////////////////////////

    // There is no const_wio_iterator_impl because the comparator, which
    // contains an alphabet, is baked into the implementation of operator++.
    // Therefore, there is no need for a facade and an impl.
    template <typename Word>
    class const_wio_iterator {
     public:
      using size_type         = typename std::vector<Word>::size_type;
      using difference_type   = typename std::vector<Word>::difference_type;
      using const_pointer     = typename std::vector<Word>::const_pointer;
      using pointer           = typename std::vector<Word>::pointer;
      using const_reference   = typename std::vector<Word>::const_reference;
      using reference         = typename std::vector<Word>::reference;
      using value_type        = Word;
      using iterator_category = std::forward_iterator_tag;

     private:
      Alphabet<Word>                                _alphabet;
      std::function<bool(Word const&, Word const&)> _cmp;
      Word                                          _current;
      Word                                          _first;
      std::vector<Word>                             _frontier;
      size_type                                     _index;
      Word                                          _last;
      size_type                                     _upper_bound;

     public:
      const_wio_iterator();
      const_wio_iterator(const_wio_iterator const&);
      const_wio_iterator(const_wio_iterator&&);
      const_wio_iterator& operator=(const_wio_iterator const&);
      const_wio_iterator& operator=(const_wio_iterator&&);
      ~const_wio_iterator();

      template <typename Cmp,
                typename = typename std::enable_if_t<has_alphabet<Cmp>>>
      const_wio_iterator(size_type   upper_bound,
                         Word const& first,
                         Word const& last,
                         Cmp&&       cmp);

      // This is called reset rather than init because, unlike the constructor,
      // there is no comparator argument.
      void reset(size_type upper_bound, Word const& first, Word const& last);

      [[nodiscard]] bool
      operator==(const_wio_iterator const& that) const noexcept {
        return _index == that._index;
      }

      [[nodiscard]] bool
      operator!=(const_wio_iterator const& that) const noexcept {
        return !(this->operator==(that));
      }

      [[nodiscard]] const_reference operator*() const noexcept {
        return _current;
      }

      [[nodiscard]] const_pointer operator->() const noexcept {
        return &_current;
      }

      // prefix
      const_wio_iterator const& operator++();

      // postfix
      const_wio_iterator operator++(int) {
        return default_postfix_increment<const_wio_iterator>(*this);
      }

      Alphabet<Word> const& alphabet() const noexcept {
        return _alphabet;
      }

      void swap(const_wio_iterator& that);
    };

    template <typename Word>
    inline void swap(const_wio_iterator<Word>& x, const_wio_iterator<Word>& y) {
      x.swap(y);
    }

    //////////////////////////////////////////////////////////////////////
    // wilo = words in lex order
    //////////////////////////////////////////////////////////////////////

    template <typename Word = word_type>
    class const_wilo_iterator : public const_wilo_iterator_impl {
     public:
      using size_type       = typename std::vector<Word>::size_type;
      using difference_type = typename std::vector<Word>::difference_type;
      using const_pointer   = typename std::vector<Word>::const_pointer;
      using pointer         = typename std::vector<Word>::pointer;
      using const_reference = typename std::vector<Word>::const_reference;
      using reference       = typename std::vector<Word>::reference;
      using value_type      = Word;

     private:
      // _external_current and _external_current_set are mutable because we
      // defined the "state" of the iterator to be determined by the state of
      // the data defined in const_wilo_iterator_impl. The _external things are
      // merely wrappers.
      Alphabet<Word> _alphabet;
      mutable Word   _external_current;
      mutable bool   _external_current_set;

      void update_external_current() const;

     public:
      const_wilo_iterator();
      const_wilo_iterator(const_wilo_iterator const&);
      const_wilo_iterator(const_wilo_iterator&&);
      const_wilo_iterator& operator=(const_wilo_iterator const&);
      const_wilo_iterator& operator=(const_wilo_iterator&&);
      ~const_wilo_iterator();

      const_wilo_iterator(Alphabet<Word> const& alphabet,
                          size_type             upper_bound,
                          Word const&           first,
                          Word const&           last);

      // This is called reset for consistency with wio.
      void reset(size_type upper_bound, Word const& first, Word const& last);

      [[nodiscard]] const_reference operator*() const {
        if (!_external_current_set) {
          update_external_current();
        }
        return _external_current;
      }

      [[nodiscard]] const_pointer operator->() const {
        if (!_external_current_set) {
          update_external_current();
        }
        return &_external_current;
      }

      // prefix
      const_wilo_iterator const& operator++() {
        const_wilo_iterator_impl::operator++();
        _external_current_set = false;
        return *this;
      }

      // postfix
      const_wilo_iterator operator++(int) {
        return default_postfix_increment<const_wilo_iterator>(*this);
      }

      Alphabet<Word> const& alphabet() const noexcept {
        return _alphabet;
      }

      void swap(const_wilo_iterator& that);
    };

    template <typename Word>
    inline void swap(const_wilo_iterator<Word>& x,
                     const_wilo_iterator<Word>& y) {
      x.swap(y);
    }

    //////////////////////////////////////////////////////////////////////
    // wislo = words in shortlex order
    //////////////////////////////////////////////////////////////////////

    template <typename Word = word_type>
    class const_wislo_iterator : public const_wislo_iterator_impl {
     public:
      using size_type       = typename std::vector<Word>::size_type;
      using difference_type = typename std::vector<Word>::difference_type;
      using const_pointer   = typename std::vector<Word>::const_pointer;
      using pointer         = typename std::vector<Word>::pointer;
      using const_reference = typename std::vector<Word>::const_reference;
      using reference       = typename std::vector<Word>::reference;
      using value_type      = Word;

     private:
      // _external_current and _external_current_set are mutable because we
      // defined the "state" of the iterator to be determined by the state of
      // the date defined in const_wislo_iterator_impl. The _external things are
      // merely wrappers.
      Alphabet<Word> _alphabet;
      mutable Word   _external_current;
      mutable bool   _external_current_set;

      void update_external_current() const;

     public:
      const_wislo_iterator();
      const_wislo_iterator(const_wislo_iterator const&);
      const_wislo_iterator(const_wislo_iterator&&);
      const_wislo_iterator& operator=(const_wislo_iterator const&);
      const_wislo_iterator& operator=(const_wislo_iterator&&);
      ~const_wislo_iterator();

      const_wislo_iterator(Alphabet<Word> const& alphabet,
                           Word const&           first,
                           Word const&           last);

      // This is called reset for consistency with wio.
      void reset(size_type upper_bound, Word const& first, Word const& last);

      [[nodiscard]] const_reference operator*() const {
        if (!_external_current_set) {
          update_external_current();
        }
        return _external_current;
      }

      [[nodiscard]] const_pointer operator->() const {
        if (!_external_current_set) {
          update_external_current();
        }
        return &_external_current;
      }

      // prefix
      const_wislo_iterator const& operator++() {
        const_wislo_iterator_impl::operator++();
        _external_current_set = false;
        return *this;
      }

      // postfix
      const_wislo_iterator operator++(int) {
        return default_postfix_increment<const_wislo_iterator>(*this);
      }

      Alphabet<Word> const& alphabet() const noexcept {
        return _alphabet;
      }

      void swap(const_wislo_iterator& that);
    };

    template <typename Word>
    inline void swap(const_wislo_iterator<Word>& x,
                     const_wislo_iterator<Word>& y) {
      x.swap(y);
    }

  }  // namespace detail
}  // namespace libsemigroups

#include "word-iterators.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_WORD_ITERATORS_HPP_
