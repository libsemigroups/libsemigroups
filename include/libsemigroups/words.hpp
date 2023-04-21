//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2023 James D. Mitchell
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

// TODO:
// * doc
// * code coverage

#ifndef LIBSEMIGROUPS_WORDS_HPP_
#define LIBSEMIGROUPS_WORDS_HPP_

#include <array>     // for std::array
#include <cstddef>   // for size_t
#include <cstdint>   // for uint64_t
#include <iterator>  // for distance
#include <string>    // for std::string
#include <variant>   // for variant

#include "iterator.hpp"  // for default_postfix_increment
#include "types.hpp"     // for word_type

#include <rx/ranges.hpp>  // for begin, end

namespace libsemigroups {

  enum class Order : uint8_t;  // forward decl

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

    inline std::string word_to_string(std::string const& alphabet,
                                      word_type const&   w) {
      return word_to_string(alphabet, w.begin(), w.end());
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

  class const_wilo_iterator {
   public:
    using size_type         = typename std::vector<word_type>::size_type;
    using difference_type   = typename std::vector<word_type>::difference_type;
    using const_pointer     = typename std::vector<word_type>::const_pointer;
    using pointer           = typename std::vector<word_type>::pointer;
    using const_reference   = typename std::vector<word_type>::const_reference;
    using reference         = typename std::vector<word_type>::reference;
    using value_type        = word_type;
    using iterator_category = std::forward_iterator_tag;

   private:
    word_type   _current;
    size_type   _index;
    letter_type _letter;
    size_type   _upper_bound;
    word_type   _last;
    size_type   _number_letters;

   public:
    // None of the constructors are noexcept because the corresponding
    // constructors for std::vector aren't (until C++17).

    const_wilo_iterator();
    const_wilo_iterator(const_wilo_iterator const&);
    const_wilo_iterator(const_wilo_iterator&&);
    const_wilo_iterator& operator=(const_wilo_iterator const&);
    const_wilo_iterator& operator=(const_wilo_iterator&&);
    ~const_wilo_iterator();

    const_wilo_iterator(size_type   n,
                        size_type   upper_bound,
                        word_type&& first,
                        word_type&& last);

    bool operator==(const_wilo_iterator const& that) const noexcept {
      return _index == that._index;
    }

    bool operator!=(const_wilo_iterator const& that) const noexcept {
      return !(this->operator==(that));
    }

    const_reference operator*() const noexcept {
      return _current;
    }

    const_pointer operator->() const noexcept {
      return &_current;
    }

    // prefix
    const_wilo_iterator const& operator++() noexcept;

    // postfix
    const_wilo_iterator operator++(int) noexcept {
      return detail::default_postfix_increment<const_wilo_iterator>(*this);
    }

    void swap(const_wilo_iterator& that) noexcept;
  };

  inline void swap(const_wilo_iterator& x, const_wilo_iterator& y) noexcept {
    x.swap(y);
  }

  //! Returns a forward iterator pointing to the 3rd parameter \p first.
  //!
  //! If incremented, the iterator will point to the next least lexicographic
  //! word after \p w over an \p n letter alphabet with length less than \p
  //! upper_bound.  Iterators of the type returned by this function are equal
  //! whenever they are obtained by advancing the return value of any call to
  //! \c cbegin_wilo by the same amount, or they are both obtained by any call
  //! to \c cend_wilo.
  //!
  //! \param n the number of letters in the alphabet;
  //! \param upper_bound   only words of length less than this value are
  //! considered;
  //! \param first the starting point for the iteration;
  //! \param last the value one past the end of the last value in the
  //! iteration.
  //!
  //! \returns An iterator of type \c const_wilo_iterator.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note
  //! The parameter \p upper_bound is required because lexicographical
  //! ordering is not a well-ordering, and there might be infinitely many words
  //! between a given pair of words.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the iterator \c it returned by \c cbegin_wilo is
  //! significantly cheaper than postfix incrementing \c it++.
  //!
  //! \warning
  //! Iterators constructed using different parameters may not be equal, so
  //! best not to loop over them.
  //!
  //! \sa cend_wilo
  //!
  //! \par Example
  //! \code
  //! std::vector<word_type>(cbegin_wilo(2, 3, {0}, {1, 1, 1}),
  //!                        cend_wilo(2, 3, {0}, {1, 1, 1}));
  //! // {{0}, {0, 0}, {0, 1}, {1}, {1, 0}, {1, 1}};
  //! \endcode
  const_wilo_iterator cbegin_wilo(size_t      n,
                                  size_t      upper_bound,
                                  word_type&& first,
                                  word_type&& last);

  //! \copydoc cbegin_wilo(size_t const, size_t const, word_type&&, word_type&&)
  const_wilo_iterator cbegin_wilo(size_t           n,
                                  size_t           upper_bound,
                                  word_type const& first,
                                  word_type const& last);

  //! Returns a forward iterator pointing to one after the end of the range
  //! from \p first to \p last.
  //!
  //! The iterator returned by this function is still dereferenceable and
  //! incrementable, but does not point to a word in the correct range.
  //!
  //! \sa cbegin_wilo
  const_wilo_iterator
  cend_wilo(size_t n, size_t upper_bound, word_type&& first, word_type&& last);

  //! \copydoc cend_wilo(size_t const, size_t const, word_type&&, word_type&&)
  const_wilo_iterator cend_wilo(size_t           n,
                                size_t           upper_bound,
                                word_type const& first,
                                word_type const& last);

  class const_wislo_iterator {
   public:
    using size_type         = typename std::vector<word_type>::size_type;
    using difference_type   = typename std::vector<word_type>::difference_type;
    using const_pointer     = typename std::vector<word_type>::const_pointer;
    using pointer           = typename std::vector<word_type>::pointer;
    using const_reference   = typename std::vector<word_type>::const_reference;
    using reference         = typename std::vector<word_type>::reference;
    using value_type        = word_type;
    using iterator_category = std::forward_iterator_tag;

   private:
    word_type _current;
    size_type _index;
    word_type _last;
    size_type _number_letters;

   public:
    // None of the constructors are noexcept because the corresponding
    // constructors for std::vector aren't (until C++17). TODO
    const_wislo_iterator();
    const_wislo_iterator(const_wislo_iterator const&);
    const_wislo_iterator(const_wislo_iterator&&);
    const_wislo_iterator& operator=(const_wislo_iterator const&);
    const_wislo_iterator& operator=(const_wislo_iterator&&);

    const_wislo_iterator(size_type n, word_type&& first, word_type&& last);

    ~const_wislo_iterator();

    bool operator==(const_wislo_iterator const& that) const noexcept {
      return _index == that._index;
    }

    bool operator!=(const_wislo_iterator const& that) const noexcept {
      return !(this->operator==(that));
    }

    const_reference operator*() const noexcept {
      return _current;
    }

    const_pointer operator->() const noexcept {
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

  //! Returns a forward iterator pointing to the 2nd parameter \p first.
  //!
  //! If incremented, the iterator will point to the next least short-lex
  //! word after \p w over an \p n letter alphabet. Iterators of the type
  //! returned by this function are equal whenever they are obtained by
  //! advancing the return value of any call to \c cbegin_wislo by the same
  //! amount, or they are both obtained by any call to \c cend_wislo.
  //!
  //! \param n the number of letters in the alphabet;
  //! \param first the starting point for the iteration;
  //! \param last the ending point for the iteration.
  //!
  //! \returns An iterator of type \c const_wislo_iterator.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the iterator \c it returned by \c cbegin_wislo is
  //! significantly cheaper than postfix incrementing \c it++.
  //!
  //! \warning
  //! Iterators constructed using different parameters may not be equal, so
  //! best not to loop over them.
  //!
  //! \sa cend_wislo
  //!
  //! \par Example
  //! \code
  //! std::vector<word_type>(cbegin_wislo(2, {0}, {0, 0, 0}),
  //!                        cend_wislo(2,  {0}, {0, 0, 0}));
  //! // {{0}, {1}, {0, 0}, {0, 1}, {1, 0}, {1, 1}};
  //! \endcode
  const_wislo_iterator cbegin_wislo(size_t      n,
                                    word_type&& first,
                                    word_type&& last);

  //! \copydoc cbegin_wislo(size_t const, word_type&&, word_type&&)
  const_wislo_iterator cbegin_wislo(size_t           n,
                                    word_type const& first,
                                    word_type const& last);

  //! Returns a forward iterator pointing to one after the end of the range
  //! from \p first to \p last.
  //!
  //! The iterator returned by this is still dereferenceable and incrementable,
  //! but does not point to a word in the correct range.
  //!
  //! \sa cbegin_wislo
  const_wislo_iterator cend_wislo(size_t      n,
                                  word_type&& first,
                                  word_type&& last);

  //! \copydoc cend_wislo(size_t const, word_type&&, word_type&&)
  const_wislo_iterator cend_wislo(size_t           n,
                                  word_type const& first,
                                  word_type const& last);

  inline void swap(const_wislo_iterator& x, const_wislo_iterator& y) noexcept {
    x.swap(y);
  }

  class Words {
   public:
    using size_type   = typename std::vector<word_type>::size_type;
    using output_type = word_type const&;

   private:
    using const_iterator
        = std::variant<const_wilo_iterator, const_wislo_iterator>;

    size_type              _number_of_letters;
    mutable const_iterator _current;
    mutable const_iterator _end;
    mutable bool           _current_valid;
    word_type              _first;
    word_type              _last;
    Order                  _order;
    size_type              _upper_bound;

    void set_iterator() const;

   public:
    output_type get() const noexcept {
      set_iterator();
      return std::visit(
          [](auto& it) -> auto const& { return *it; }, _current);
    }

    void next() noexcept {
      set_iterator();
      std::visit([](auto& it) { ++it; }, _current);
    }

    bool at_end() const noexcept {
      set_iterator();
      return _current == _end;
    }

    size_t size_hint() const noexcept {
      return number_of_words(_number_of_letters, _first.size(), _last.size());
      // This is only the actual size if _order is shortlex
    }

    size_t count() const noexcept;

    static constexpr bool is_finite     = true;  // This may not always be true
    static constexpr bool is_idempotent = true;

    Words() {
      init();
    }

    Words& init();

    Words(Words const&);
    Words(Words&&);
    Words& operator=(Words const&);
    Words& operator=(Words&&);
    ~Words();

    Words& letters(size_type n) {
      _current_valid &= (n == _number_of_letters);
      _number_of_letters = n;
      return *this;
    }

    [[nodiscard]] size_type letters() const noexcept {
      return _number_of_letters;
    }

    Words& first(word_type const& frst) {
      _current_valid &= (frst == _first);
      _first = frst;
      return *this;
    }

    [[nodiscard]] word_type const& first() const noexcept {
      return _first;
    }

    Words& last(word_type const& lst) {
      _current_valid &= (lst == _last);
      _last = lst;
      return *this;
    }

    [[nodiscard]] word_type const& last() const noexcept {
      return _last;
    }

    Words& order(Order val);

    [[nodiscard]] Order order() const noexcept {
      return _order;
    }

    Words& upper_bound(size_type n) {
      _current_valid &= (n == _upper_bound);
      _upper_bound = n;
      return *this;
    }

    [[nodiscard]] size_type upper_bound() const noexcept {
      return _upper_bound;
    }

    Words& min(size_type val) {
      first(word_type(val, 0));
      return *this;
    }

    Words& max(size_type val) {
      last(word_type(val, 0));
      return *this;
    }

    auto begin() const {
      return rx::begin(*this);
    }

    auto end() const {
      return rx::end(*this);
    }
  };

  // The next class is a custom combinator for rx::ranges to convert the output
  // of a Words object into strings
  template <typename = void>  // TODO why is typename = void here?
  struct to_strings {
    explicit to_strings(std::string const& lttrs) : _letters(lttrs) {}
    explicit to_strings(std::string&& lttrs) : _letters(std::move(lttrs)) {}
    explicit to_strings(char const* lttrs) : _letters(lttrs) {}

    std::string _letters;

    template <typename InputRange>
    struct Range {
      using output_type = std::string;

      static constexpr bool is_finite     = true;
      static constexpr bool is_idempotent = true;

      InputRange _input;
      to_strings _to_string;

      constexpr Range(InputRange input, to_strings t_strng) noexcept
          // TODO move correct?
          : _input(std::move(input)), _to_string(std::move(t_strng)) {}

      [[nodiscard]] output_type get() const noexcept {
        return detail::word_to_string(_to_string._letters, _input.get());
      }

      constexpr void next() noexcept {
        _input.next();
      }

      [[nodiscard]] constexpr bool at_end() const noexcept {
        return _input.at_end();
      }

      [[nodiscard]] constexpr size_t size_hint() const noexcept {
        return _input.size_hint();
      }
    };

    template <typename InputRange>
    [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
      using Inner = rx::get_range_type_t<InputRange>;
      return Range<Inner>(std::forward<InputRange>(input), *this);
    }
  };

  class Strings {
   public:
    using size_type   = typename std::vector<std::string>::size_type;
    using output_type = std::string const&;

   private:
    mutable std::string  _current;
    mutable bool         _current_valid;
    std::string          _letters;
    detail::StringToWord _string_to_word;
    Words                _words;

    void init_current() const {
      if (!_current_valid) {
        _current = detail::word_to_string(_letters, _words.get());
      }
    }

   public:
    output_type get() const noexcept {
      init_current();
      return _current;
    }

    void next() noexcept {
      _current_valid = false;
      _words.next();
    }

    bool at_end() const noexcept {
      return _words.at_end();
    }

    size_t size_hint() const noexcept {
      return _words.size_hint();
    }

    size_t count() const noexcept {
      return _words.count();
    }

    static constexpr bool is_finite     = true;  // This may not always be true
    static constexpr bool is_idempotent = true;

    Strings() {
      init();
    }

    Strings& init();

    Strings(Strings const&);
    Strings(Strings&&);
    Strings& operator=(Strings const&);
    Strings& operator=(Strings&&);
    ~Strings();

    Strings& letters(std::string const& x);

    [[nodiscard]] std::string const& letters() const noexcept {
      return _letters;
    }

    Strings& first(std::string const& frst) {
      _current_valid = false;
      _words.first(_string_to_word(frst));
      return *this;
    }

    [[nodiscard]] std::string first() const noexcept {
      return detail::word_to_string(_letters, _words.first());
    }

    Strings& last(std::string const& lst) {
      _current_valid = false;
      _words.last(_string_to_word(lst));
      return *this;
    }

    [[nodiscard]] std::string last() const noexcept {
      return detail::word_to_string(_letters, _words.last());
    }

    Strings& order(Order val) {
      _current_valid = false;
      _words.order(val);
      return *this;
    }

    [[nodiscard]] Order order() const noexcept {
      return _words.order();
    }

    Strings& upper_bound(size_type n) {
      _current_valid = false;
      _words.upper_bound(n);
      return *this;
    }

    [[nodiscard]] size_type upper_bound() const noexcept {
      return _words.upper_bound();
    }

    Strings& min(size_type val) {
      _current_valid = false;
      _words.min(val);
      return *this;
    }

    Strings& max(size_type val) {
      _current_valid = false;
      _words.max(val);
      return *this;
    }

    auto begin() const {
      return rx::begin(*this);
    }

    auto end() const {
      return rx::end(*this);
    }

    void swap(Strings& that) noexcept;

    word_type to_word(std::string const& x) const {
      return _string_to_word(x);
    }
  };

  // The next class is a custom combinator for rx::ranges to convert the output
  // of a Strings object into words
  template <typename = void>
  struct to_words {
    to_words() = default;

    explicit to_words(std::string const& lttrs) : _letters(lttrs) {}
    explicit to_words(std::string&& lttrs) : _letters(std::move(lttrs)) {}
    explicit to_words(char const* lttrs) : _letters(lttrs) {}

    std::string _letters;

    template <typename InputRange>
    struct Range {
      using output_type = word_type;

      static constexpr bool is_finite     = true;
      static constexpr bool is_idempotent = true;

      InputRange           _input;
      detail::StringToWord _string_to_word;

      explicit constexpr Range(InputRange&& input, to_words t_wrds) noexcept
          : _input(std::move(input)), _string_to_word(t_wrds._letters) {}

      explicit constexpr Range(Strings const& input)
          : _input(input), _string_to_word(input.letters()) {}

      [[nodiscard]] output_type get() const noexcept {
        return _string_to_word(_input.get());
      }

      constexpr void next() noexcept {
        _input.next();
      }

      [[nodiscard]] constexpr bool at_end() const noexcept {
        return _input.at_end();
      }

      [[nodiscard]] constexpr size_t size_hint() const noexcept {
        return _input.size_hint();
      }
    };

    [[nodiscard]] constexpr auto operator()(Strings const& input) const {
      return Range<Strings>(input);
    }

    template <typename InputRange,
              typename = std::enable_if_t<
                  !std::is_same_v<std::decay_t<InputRange>, Strings>>>
    [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
      using Inner = rx::get_range_type_t<InputRange>;
      return Range<Inner>(std::forward<InputRange>(input), *this);
    }
  };

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_WORDS_HPP_
