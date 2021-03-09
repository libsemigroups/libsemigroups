//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

// This file contains some functionality for generating elements in the free
// monoid over an alphabet with a given number of letters up to a given length
// in a lexicographic or short-lex order.
//
// SILO  = Strings In Lexicographic Order
// SISLO = Strings In Short-Lex Order
// SISO  = Strings In Some Order

// TODO(later):
// 1. cbegin_silo and cend_silo for rvalue references to strings
// 2. cbegin_sislo and cend_sislo for rvalue references to strings

#ifndef LIBSEMIGROUPS_SISO_HPP_
#define LIBSEMIGROUPS_SISO_HPP_

#include <cstddef>   // for size_t, ptrdiff_t, ...
#include <iterator>  // for forward_iterator_tag
#include <string>    // for string
#include <utility>   // for pair

#include "iterator.hpp"  // for detail::ConstIteratorStateful
#include "wilo.hpp"      // for const_wilo_iterator
#include "wislo.hpp"     // for const_wislo_iterator
#include "word.hpp"      // for word_to_string

namespace libsemigroups {
  namespace detail {
    // This is a traits class for ConstIteratorStateful in iterator.hpp
    template <typename T>
    struct SisoIteratorTraits {
      // state_type::first = alphabet, state_type::second = current value
      using state_type             = std::pair<std::string, std::string>;
      using internal_iterator_type = T;
      using value_type             = std::string;
      using reference              = std::string&;
      using const_reference        = std::string const&;
      using difference_type        = std::ptrdiff_t;
      using size_type              = std::size_t;
      using const_pointer          = std::string const*;
      using pointer                = std::string*;
      using iterator_category      = std::forward_iterator_tag;

      struct Deref {
        const_reference
        operator()(state_type&                   state,
                   internal_iterator_type const& it) const noexcept {
          if (state.second.empty()) {
            detail::word_to_string(state.first, *it, state.second);
          }
          return state.second;
        }
      };

      struct AddressOf {
        const_pointer
        operator()(state_type&                   state,
                   internal_iterator_type const& it) const noexcept {
          Deref()(state, it);  // to ensure that state.second is initialised
          return &state.second;
        }
      };

      struct PrefixIncrement {
        void operator()(state_type&             state,
                        internal_iterator_type& it) const noexcept {
          ++it;
          state.second.clear();
        }
      };

      struct AddAssign {
        void operator()(state_type&             state,
                        internal_iterator_type& it,
                        size_t                  val) const noexcept {
          std::advance(it, val);
          state.second.clear();
        }
      };

      struct Swap {
        void operator()(internal_iterator_type& it_this,
                        internal_iterator_type& it_that,
                        state_type&             state_this,
                        state_type&             state_that) const noexcept {
          using std::swap;
          swap(it_this, it_that);
          swap(state_this, state_that);
        }
      };

      using EqualTo          = void;
      using NotEqualTo       = void;
      using PostfixIncrement = void;
    };
  }  // namespace detail

  //! No doc
  using const_silo_iterator = detail::ConstIteratorStateful<
      detail::SisoIteratorTraits<const_wilo_iterator>>;

  static_assert(std::is_default_constructible<const_silo_iterator>::value,
                "forward iterator requires default-constructible");
  static_assert(std::is_copy_constructible<const_silo_iterator>::value,
                "forward iterator requires copy-constructible");
  static_assert(std::is_copy_assignable<const_silo_iterator>::value,
                "forward iterator requires copy-assignable");
  static_assert(std::is_destructible<const_silo_iterator>::value,
                "forward iterator requires destructible");

  //! No doc
  using const_sislo_iterator = detail::ConstIteratorStateful<
      detail::SisoIteratorTraits<const_wislo_iterator>>;

  static_assert(std::is_default_constructible<const_sislo_iterator>::value,
                "forward iterator requires default-constructible");
  static_assert(std::is_copy_constructible<const_sislo_iterator>::value,
                "forward iterator requires copy-constructible");
  static_assert(std::is_copy_assignable<const_sislo_iterator>::value,
                "forward iterator requires copy-assignable");
  static_assert(std::is_destructible<const_sislo_iterator>::value,
                "forward iterator requires destructible");

  //! Returns a forward iterator pointing to the 3rd parameter \p first.
  //!
  //! If incremented, the iterator will point to the next least lexicographic
  //! string after \p w over \p alphabet with length less than \p upper_bound.
  //! Iterators of the type returned by this function are equal whenever they
  //! are obtained by advancing the return value of any call to \c cbegin_silo
  //! by the same amount, or they are both obtained by any call to
  //! \c cend_silo.
  //!
  //! \param alphabet the alphabet
  //! \param upper_bound  only strings of length less than this value are
  //! considered;
  //! \param first the starting point for the iteration;
  //! \param last the ending point for the iteration.
  //!
  //! \returns An iterator of type \c const_silo_iterator.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note
  //! The parameter \p upper_bound is required because lexicographical
  //! ordering is not a well-ordering, and there might be infinitely many
  //! strings between a given pair of strings.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the iterator \c it returned by \c cbegin_silo is
  //! significantly cheaper than postfix incrementing \c it++.
  //!
  //! \warning
  //! Iterators constructed using different parameters may not be equal, so
  //! best not to loop over them.
  //!
  //! \sa cend_silo
  //!
  //! \par Example
  //! \code
  //! std::vector<std::string>(cbegin_silo("ba", 3, "b", "aaa"),
  //!                          cend_silo("ba", 3, "b", "aaa"));
  //! // {"b", "bb", "ba", "a", "ab", "aa"};
  //! \endcode
  const_silo_iterator cbegin_silo(std::string const& alphabet,
                                  size_t const       upper_bound,
                                  std::string const& first,
                                  std::string const& last);

  //! Returns a forward iterator pointing to one after the end of the range
  //! from \p first to \p last.
  //!
  //! The iterator returned by this is still dereferencable and incrementable,
  //! but does not point to a string in the correct range.
  //!
  //! \sa cbegin_silo
  const_silo_iterator cend_silo(std::string const& alphabet,
                                size_t const       upper_bound,
                                std::string const& first,
                                std::string const& last);

  //! Returns a forward iterator pointing to the 2nd parameter \p first.
  //!
  //! If incremented, the iterator will point to the next least short-lex
  //! string after \p w over \p alphabet.
  //! Iterators of the type returned by this function are equal whenever they
  //! are obtained by advancing the return value of any call to \c cbegin_sislo
  //! by the same amount, or they are both obtained by any call to
  //! \c cend_sislo.
  //!
  //! \param alphabet the alphabet
  //! \param first the starting point for the iteration;
  //! \param last the ending point for the iteration.
  //!
  //! \returns An iterator of type \c const_sislo_iterator.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the iterator \c it returned by \c cbegin_sislo is
  //! significantly cheaper than postfix incrementing \c it++.
  //!
  //! \warning
  //! Iterators constructed using different parameters may not be equal, so
  //! best not to loop over them.
  //!
  //! \sa cend_sislo
  //!
  //! \par Example
  //! \code
  //! std::vector<std::string>(cbegin_sislo("ba", "b", "bbb"),
  //!                          cend_sislo("ba", "b", "bbb"));
  //! // {"b", "b", "bb", "ba","ab", "aa"};
  //! \endcode
  const_sislo_iterator cbegin_sislo(std::string const& alphabet,
                                    std::string const& first,
                                    std::string const& last);

  //! Returns a forward iterator pointing to one after the end of the range
  //! from \p first to \p last.
  //!
  //! The iterator returned by this is still dereferencable and incrementable,
  //! but does not point to a string in the correct range.
  //!
  //! \sa cbegin_sislo
  const_sislo_iterator cend_sislo(std::string const& alphabet,
                                  std::string const& first,
                                  std::string const& last);

  class Sislo {
   public:
    Sislo()             = default;
    Sislo(Sislo const&) = default;
    Sislo(Sislo&&)      = default;
    Sislo& operator=(Sislo const&) = default;
    Sislo& operator=(Sislo&&) = default;

    Sislo& alphabet(std::string const& lphbt) {
      _alphabet = lphbt;
      return *this;
    }

    std::string const& alphabet() const {
      return _alphabet;
    }

    char letter(size_t i) const {
      return _alphabet[i];
    }

    Sislo& first(std::string const& first) {
      _first = first;
      return *this;
    }

    Sislo& first(size_t min) {
      _first = std::string(min, letter(0));
      return *this;
    }

    std::string const& first() const noexcept {
      return _first;
    }

    Sislo& last(std::string const& last) {
      _last = last;
      return *this;
    }

    Sislo& last(size_t max) {
      _last = std::string(max, letter(0));
      return *this;
    }

    std::string const& last() const noexcept {
      return _last;
    }

    const_sislo_iterator cbegin() const noexcept {
      return cbegin_sislo(_alphabet, _first, _last);
    }

    const_sislo_iterator cend() const noexcept {
      return cend_sislo(_alphabet, _first, _last);
    }

   private:
    std::string _alphabet;
    std::string _first;
    std::string _last;
  };

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SISO_HPP_
