//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

// This file contains some wrapped iterator classes that can be used to convert
// between, say, std::string::iterator and word_type::iterator.

#ifndef LIBSEMIGROUPS_DETAIL_CITOW_HPP_
#define LIBSEMIGROUPS_DETAIL_CITOW_HPP_

#include <cstddef>   // for size_t
#include <iterator>  // for bidirectional_iterator_tag

#include "libsemigroups/types.hpp"  // for letter_type

namespace libsemigroups {

  template <typename Word>
  class Alphabet;

  namespace detail {
    // The following is a class for wrapping iterators. This is used by the
    // member functions that accept iterators (that point at words that might
    // not be the word_type used by ThingImpl) to convert the values
    // pointed at into word_types, and in the class itow, to allow assignment of
    // these values too.
    // CITOW = const_iterator_to_word
    template <typename Word, typename Iterator>
    class citow {
     protected:
      Iterator              _it;
      Alphabet<Word> const* _ptr;

     public:
      using internal_iterator_type = Iterator;
      using value_type             = letter_type;
      using reference              = letter_type;
      using const_reference        = value_type;
      using const_pointer          = value_type const*;
      using pointer                = value_type*;

      using size_type         = size_t;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      template <typename Thing>
      citow(Thing const* thing, Iterator it)
          : _it(it), _ptr(&thing->presentation().alphabet_v4()) {}

      citow(Alphabet<Word> const& alphabet, Iterator it)
          : _it(it), _ptr(&alphabet) {}

      reference operator*() const {
        return _ptr->index_no_checks(*_it);
      }

      const_pointer operator->() const {
        return &_ptr->index_no_checks(*_it);
      }

      bool operator==(citow const& that) const noexcept {
        return _it == that._it;
      }

      bool operator!=(citow const& that) const noexcept {
        return _it != that._it;
      }

      bool operator<=(citow const& that) const noexcept {
        return _it <= that._it;
      }

      bool operator>=(citow const& that) const noexcept {
        return _it >= that._it;
      }

      bool operator<(citow const& that) const noexcept {
        return _it < that._it;
      }

      bool operator>(citow const& that) const noexcept {
        return _it > that._it;
      }

      citow& operator++() {
        ++_it;
        return *this;
      }

      citow& operator+=(size_type val) noexcept {
        _it += val;
        return *this;
      }

      citow operator+(size_type val) const noexcept {
        citow result(*this);
        result += val;
        return result;
      }

      citow& operator--() {
        --_it;
        return *this;
      }

      citow& operator-=(size_type val) noexcept {
        _it -= val;
        return *this;
      }

      citow operator-(size_type val) const noexcept {
        citow result(*this);
        result -= val;
        return result;
      }

      [[nodiscard]] Iterator get() const noexcept {
        return _it;
      }
    };  // class citow

    // The following is a class for wrapping iterators, it does the opposite of
    // CITOW, for unwrapping word_types (used natively in, say ToddCoxeterImpl,
    // to whatever type is used by ToddCoxeter<Word>)
    // CIFRW = const_iterator_from_word
    template <typename Word, typename Iterator>
    class cifrw {
     protected:
      Iterator              _it;
      Alphabet<Word> const* _ptr;

     public:
      using internal_iterator_type = Iterator;
      using value_type             = typename Word::value_type;
      using reference              = value_type;
      using const_reference        = value_type;
      using const_pointer          = value_type const*;
      using pointer                = value_type*;

      using size_type         = size_t;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      template <typename Thing>
      cifrw(Thing const* thing, Iterator it)
          : _it(it), _ptr(&thing->presentation().alphabet_v4()) {}

      cifrw(Alphabet<Word> const& alphabet, Iterator it)
          : _it(it), _ptr(&alphabet) {}

      reference operator*() const {
        return _ptr->letter_no_checks(*_it);
      }

      const_pointer operator->() const {
        return &_ptr->letter_no_checks(*_it);
      }

      bool operator==(cifrw const& that) const noexcept {
        return _it == that._it;
      }

      bool operator!=(cifrw const& that) const noexcept {
        return _it != that._it;
      }

      bool operator<=(cifrw const& that) const noexcept {
        return _it <= that._it;
      }

      bool operator>=(cifrw const& that) const noexcept {
        return _it >= that._it;
      }

      bool operator<(cifrw const& that) const noexcept {
        return _it < that._it;
      }

      bool operator>(cifrw const& that) const noexcept {
        return _it > that._it;
      }

      cifrw& operator++() {
        ++_it;
        return *this;
      }

      cifrw& operator+=(size_type val) noexcept {
        _it += val;
        return *this;
      }

      cifrw operator+(size_type val) const noexcept {
        cifrw result(*this);
        result += val;
        return result;
      }

      cifrw& operator--() {
        --_it;
        return *this;
      }

      cifrw& operator-=(size_type val) noexcept {
        _it -= val;
        return *this;
      }

      cifrw operator-(size_type val) const noexcept {
        cifrw result(*this);
        result -= val;
        return result;
      }

      [[nodiscard]] Iterator get() const noexcept {
        return _it;
      }
    };  // class cifrw

    // itow only differs from citow in the dereference member function
    // returning a (non-const) reference. A proxy is returned instead which
    // permits assignment to an output iterator.
    template <typename Word, typename Iterator>
    class itow : public citow<Word, Iterator> {
      // Proxy class for reference to the returned values
      class proxy_ref {
       private:
        Iterator              _it;
        Alphabet<Word> const* _ptr;

       public:
        // Constructor from Alphabet* and iterator
        proxy_ref(Alphabet<Word> const* ptr, Iterator it) noexcept
            : _it(it), _ptr(ptr) {}

        // Assignment operator to allow setting the value via the proxy
        Iterator operator=(letter_type i) noexcept {
          *_it = _ptr->letter_no_checks(i);
          return _it;
        }

        // Conversion operator to obtain the letter corresponding to the
        // letter_type
        [[nodiscard]] operator letter_type() const noexcept {
          return _ptr->index_no_checks(*_it);
        }
      };  // class proxy_ref

     public:
      using internal_iterator_type = Iterator;
      using value_type             = letter_type;
      using reference              = proxy_ref;
      using const_reference        = value_type;

      // TODO(1) use proxy for pointers too?
      using const_pointer = value_type const*;
      using pointer       = value_type*;

      using size_type         = size_t;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      using citow<Word, Iterator>::citow;

      reference operator*() {
        return reference(this->_ptr, this->_it);
      }

      // TODO(1) probably require more of these
      itow& operator++() {
        citow<Word, Iterator>::operator++();
        return *this;
      }
    };  // class itow

    template <typename Word, typename Iterator>
    class ifrw : public cifrw<Word, Iterator> {
      // Proxy class for reference to the returned values
      class proxy_ref {
       private:
        Iterator              _it;
        Alphabet<Word> const* _ptr;

       public:
        // Constructor from Word and iterator
        proxy_ref(Alphabet<Word> const* ptr, Iterator it) noexcept
            : _it(it), _ptr(ptr) {}

        // Assignment operator to allow setting the value via the proxy
        Iterator operator=(letter_type i) noexcept {
          *_it = _ptr->index_no_checks(i);
          return _it;
        }

        // Conversion operator to obtain the letter corresponding to the
        // letter_type
        [[nodiscard]] operator letter_type() const noexcept {
          return _ptr->letter_no_checks(*_it);
        }
      };  // class proxy_ref

     public:
      using internal_iterator_type = Iterator;
      using value_type             = letter_type;
      using reference              = proxy_ref;
      using const_reference        = value_type;

      // TODO(1) use proxy for pointers too?
      using const_pointer = value_type const*;
      using pointer       = value_type*;

      using size_type         = size_t;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      using cifrw<Word, Iterator>::cifrw;

      reference operator*() {
        return reference(this->_ptr, this->_it);
      }

      // TODO(1) probably require more of these
      ifrw& operator++() {
        cifrw<Word, Iterator>::operator++();
        return *this;
      }
    };  // class itow

    template <typename Thing, typename Iterator>
    citow(Thing const*, Iterator)
        -> citow<typename Thing::native_word_type, Iterator>;

    template <typename Thing, typename Iterator>
    itow(Thing const*, Iterator)
        -> itow<typename Thing::native_word_type, Iterator>;

    template <typename Thing, typename Iterator>
    cifrw(Thing const*, Iterator)
        -> cifrw<typename Thing::native_word_type, Iterator>;

    template <typename Thing, typename Iterator>
    ifrw(Thing const*, Iterator)
        -> ifrw<typename Thing::native_word_type, Iterator>;

  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_CITOW_HPP_
