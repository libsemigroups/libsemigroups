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

// This file contains TODO

#ifndef LIBSEMIGROUPS_DETAIL_CITOW_HPP_
#define LIBSEMIGROUPS_DETAIL_CITOW_HPP_

#include <cstddef>   // for size_t
#include <iterator>  // for pair

#include "libsemigroups/types.hpp"  // for letter_type

namespace libsemigroups {
  namespace detail {
    // The following is a class for wrapping iterators. This is used by the
    // member functions that accept iterators (that point at words that might
    // not be the word_type used by ThingImpl) to convert the values
    // pointed at into word_types, and in the class itow, to allow assignment of
    // these values too.
    // CITOW = const_iterator_to_word
    template <typename Thing, typename Iterator>
    class citow {
     protected:
      Iterator     _it;
      Thing const* _ptr;  // TODO change to Presentation

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

      citow(Thing const* thing, Iterator it) : _it(it), _ptr(thing) {}

      reference operator*() const {
        return _ptr->presentation().index_no_checks(*_it);
      }

      // TODO(1) operator-> ??

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

    // itow only differs from citow in the dereference member function
    // returning a (non-const) reference. A proxy is returned instead which
    // permits assignment to an output iterator.
    template <typename Thing, typename Iterator>
    class itow : public citow<Thing, Iterator> {
      // Proxy class for reference to the returned values
      class proxy_ref {
       private:
        Iterator     _it;
        Thing const* _ptr;  // TODO(0) use Presentation const& instead

       public:
        using native_letter_type =
            typename std::decay_t<decltype(_ptr->presentation())>::letter_type;

        // Constructor from Thing and iterator
        proxy_ref(Thing const* thing, Iterator it) noexcept
            : _it(it), _ptr(thing) {}

        // Assignment operator to allow setting the value via the proxy
        Iterator operator=(native_letter_type i) noexcept {
          *_it = _ptr->presentation().letter_no_checks(i);
          return _it;
        }

        // Conversion operator to obtain the letter corresponding to the
        // letter_type
        [[nodiscard]] operator native_letter_type() const noexcept {
          return _ptr->presentation().index_no_checks(*_it);
        }
      };  // class proxy_ref

     public:
      using internal_iterator_type = Iterator;
      using value_type             = typename proxy_ref::native_letter_type;
      using reference              = proxy_ref;
      using const_reference        = value_type;

      // TODO(1) use proxy for pointers too?
      using const_pointer = value_type const*;
      using pointer       = value_type*;

      using size_type         = size_t;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      using citow<Thing, Iterator>::citow;

      reference operator*() {
        return reference(this->_ptr, this->_it);
      }

      // TODO(1) probably require more of these
      itow& operator++() {
        citow<Thing, Iterator>::operator++();
        return *this;
      }
    };  // class itow

    template <typename Iterator, typename Thing>
    citow(Thing const*, Iterator) -> citow<Thing, Iterator>;

    template <typename Iterator, typename Thing>
    itow(Thing const*, Iterator) -> itow<Thing, Iterator>;

  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_CITOW_HPP_
