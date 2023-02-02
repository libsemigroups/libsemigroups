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

// This file contains declarations for function related to words (counting, and
// converting) in libsemigroups.

#ifndef LIBSEMIGROUPS_WORD_HPP_
#define LIBSEMIGROUPS_WORD_HPP_

#include <array>    // for std::array
#include <cstddef>  // for size_t
#include <cstdint>  // for uint64_t
#include <string>   // for std::string

#include "types.hpp"  // for word_type

namespace libsemigroups {
  //! Returns the number of words over an alphabet with a given number of
  //! letters with length in a specified range.
  //!
  //! \param n the number of letters in the alphabet
  //! \param min the minimum length of a word
  //! \param max the maximum length of a word
  //!
  //! \returns
  //! A value of type `uint64_t`.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \warning If the number of words exceeds 2 ^ 64 - 1, then
  //! the return value of this function will not be correct.
  uint64_t number_of_words(size_t n, size_t min, size_t max);

  namespace detail {
    // TODO(later) doc, check args etc
    void word_to_string(std::string const& alphabet,
                        word_type const&   input,
                        std::string&       output);

    template <typename T>
    std::string word_to_string(std::string const& alphabet, T first, T last) {
      std::string output;
      output.reserve(std::distance(first, last));
      for (auto it = first; it != last; ++it) {
        output.push_back(alphabet[*it]);
      }
      return output;
    }

    // TODO(later) doc, check args etc
    class StringToWord {
     public:
      StringToWord() : _lookup() {
        clear();
      }

      explicit StringToWord(std::string const& alphabet) : _lookup() {
        init(alphabet);
      }

      StringToWord& init(std::string const& alphabet) {
        clear();
        _lookup.back() = alphabet.size();
        for (letter_type l = 0; l < alphabet.size(); ++l) {
          _lookup[alphabet[l]] = l;
        }
        return *this;
      }

      bool empty() const {
        return _lookup.back() == 0;
      }

      void clear() {
        _lookup.fill(0);
      }

      // TODO(later) doc
      void      operator()(std::string const& input, word_type& output) const;
      word_type operator()(std::string const& input) const;

     private:
      std::array<letter_type, 256> _lookup;
    };
  }  // namespace detail

  namespace literals {
    //! Literal for defining \ref word_type over integers less than 10.
    //!
    //! This operator provides a convenient brief means of constructing a  \ref
    //! word_type from an sequence of literal integer digits or a string. For
    //! example, `0123_w` produces the same output as `word_type({0, 1, 2,
    //! 3})`.
    //!
    //! There are some gotchas and this operator should be used with some care:
    //!
    //! * the parameter \p w must only consist of the integers \f$\{0, \ldots,
    //! 9\}\f$. For example, there are no guarantees about the value of
    //! `"abc"_w`.
    //! * if \p w starts with `0` and is follows by a value greater than `7`,
    //! then it is necessary to enclose \p w in quotes. For example, `08_w`
    //! will not compile because it is interpreted as an invalid octal. However
    //! `"08"_w` behaves as expected.
    //!
    //! \param w the letters of the word
    //! \param n the length of \p w (defaults to the length of \p w)
    //!
    //! \warning
    //! This operator performs no checks on its arguments whatsoever.
    //!
    //! \returns A value of type \ref word_type.
    //!
    //! \exceptions \no_libsemigroups_except
    word_type operator"" _w(const char* w, size_t n);

    //! \copydoc operator""_w
    word_type operator"" _w(const char* w);
  }  // namespace literals

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_WORD_HPP_
