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

#include <array>             // for array
#include <cstddef>           // for size_t
#include <cstdint>           // for uint64_t, uint8_t
#include <cstring>           // for size_t, strlen
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance
#include <random>            // for mt19937
#include <string>            // for basic_string
#include <type_traits>       // for decay_t
#include <utility>           // for forward
#include <variant>           // for visit, operator==
#include <vector>            // for vector, operator==

#include "exception.hpp"  // for LibsemigroupsException
#include "types.hpp"      // for word_type

#include "detail/iterator.hpp"  // for default_postfix_increment

#include <rx/ranges.hpp>  // for begin, end

namespace libsemigroups {

  enum class Order : uint8_t;  // forward decl

  ////////////////////////////////////////////////////////////////////////
  // Words
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup Words
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

  // TODO to cpp
  static inline word_type random_word(size_t length, size_t nr_letters) {
    static std::random_device               rd;
    static std::mt19937                     gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, nr_letters - 1);
    word_type                               out;
    for (size_t i = 0; i < length; ++i) {
      out.push_back(dist(gen));
    }
    return out;
  }

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
      return std::visit([](auto& it) -> auto const& { return *it; }, _current);
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

  ////////////////////////////////////////////////////////////////////////
  // Strings -> Words
  ////////////////////////////////////////////////////////////////////////

  // TODO doc
  // The inverse of human_readable_char
  letter_type human_readable_index(char c);

  // TODO(later) could do a no_check version
  void to_word(word_type& w, std::string const& s);

  // TODO(later) could do a no_check version
  word_type to_word(std::string const& s);

  // TODO(later) doc, check args etc
  class ToWord {
   public:
    ToWord() : _lookup() {
      clear();
    }

    explicit ToWord(std::string const& alphabet) : _lookup() {
      init(alphabet);
    }

    ToWord& init(std::string const& alphabet) {
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

  ////////////////////////////////////////////////////////////////////////
  // Words -> Strings
  ////////////////////////////////////////////////////////////////////////

  // TODO doc
  // Returns the i-th human readable char.
  char human_readable_char(size_t i);

  // TODO(later) doc, check args etc
  void to_string(std::string const& alphabet,
                 word_type const&   input,
                 std::string&       output);

  template <typename Iterator>
  std::string to_string(std::string const& alphabet,
                        Iterator           first,
                        Iterator           last) {
    std::string output;
    output.reserve(std::distance(first, last));
    for (auto it = first; it != last; ++it) {
      output.push_back(alphabet[*it]);
    }
    return output;
  }

  inline std::string to_string(std::string const& alphabet,
                               word_type const&   w) {
    return to_string(alphabet, w.begin(), w.end());
  }

  ////////////////////////////////////////////////////////////////////////
  // Strings
  ////////////////////////////////////////////////////////////////////////

  class Strings {
   public:
    using size_type   = typename std::vector<std::string>::size_type;
    using output_type = std::string const&;

   private:
    mutable std::string _current;
    mutable bool        _current_valid;
    std::string         _letters;
    ToWord              _string_to_word;
    Words               _words;

    void init_current() const {
      if (!_current_valid) {
        _current = to_string(_letters, _words.get());
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
      return to_string(_letters, _words.first());
    }

    Strings& last(std::string const& lst) {
      _current_valid = false;
      _words.last(_string_to_word(lst));
      return *this;
    }

    [[nodiscard]] std::string last() const noexcept {
      return to_string(_letters, _words.last());
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

  ////////////////////////////////////////////////////////////////////////
  // Ranges
  ////////////////////////////////////////////////////////////////////////

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

      InputRange _input;
      ToWord     _string_to_word;

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

  // The next class is a custom combinator for rx::ranges to convert the output
  // of a Words object into strings
  template <typename = void>
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
          : _input(std::move(input)), _to_string(std::move(t_strng)) {}

      [[nodiscard]] output_type get() const noexcept {
        return to_string(_to_string._letters, _input.get());
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

  ////////////////////////////////////////////////////////////////////////
  // Literals
  ////////////////////////////////////////////////////////////////////////

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

    // TODO doc
    std::string operator"" _p(const char* w, size_t n);

    // TODO doc
    std::string operator"" _p(const char* w);
  }  // namespace literals

  ////////////////////////////////////////////////////////////////////////

  namespace words {

    ////////////////////////////////////////////////////////////////////////
    // Operators - declarations
    ////////////////////////////////////////////////////////////////////////

    //! \anchor operator_plus
    //! Concatenate two words or strings.
    //!
    //! Returns the concatenation of `u` and `w`.
    //!
    //! \param u a word or string
    //! \param w a word or string
    //!
    //! \returns A word_type or string
    //!
    //! \exceptions
    //! \noexcept
    word_type operator+(word_type const& u, word_type const& w);

    //! See \ref operator_plus "operator+".
    word_type operator+(word_type const& u, letter_type w);

    //! See \ref operator_plus "operator+".
    word_type operator+(letter_type w, word_type const& u);

    //! Concatenate a word/string with another word/string in-place.
    //!
    //! Changes `u` to `u + w` in-place. See \ref operator_plus "operator+".
    //!
    //! \param u a word or string
    //! \param w a word or string
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa \ref operator_plus "operator+"
    void operator+=(word_type& u, word_type const& w);

    // TODO doc
    inline void operator+=(word_type& u, letter_type a) {
      u.push_back(a);
    }

    // TODO doc
    inline void operator+=(letter_type a, word_type& u) {
      u.insert(u.begin(), a);
    }

#define ENABLE_IF_IS_WORD(Word)                                 \
  typename = std::enable_if_t < std::is_same_v<Word, word_type> \
             || std::is_same_v < Word,                          \
  std::string >>

    //! Take a power of a word or string.
    //!
    //! Returns the `n`th power of the word/string given by `w` .
    //!
    //! \param w a word
    //! \param n the power
    //!
    //! \returns A word_type
    //!
    //! \exceptions
    //! \noexcept
    template <typename Word, ENABLE_IF_IS_WORD(Word)>
    Word pow(Word const& w, size_t n);

    //! Take a power of a word.
    //!
    //! Change the word/string `w` to its `n`th power, in-place.
    //!
    //! \param w the word
    //! \param n the power
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \noexcept
    template <typename Word, ENABLE_IF_IS_WORD(Word)>
    void pow_inplace(Word& w, size_t n);

    //! Take a power of a word.
    //!
    //! Returns the `n`th power of the word corresponding to the initializer
    //! list `ilist`.
    //!
    //! \param ilist the initializer list
    //! \param n the power
    //!
    //! \returns A word_type or std::string
    //!
    //! \exceptions
    //! \noexcept
    word_type pow(std::initializer_list<size_t> ilist, size_t n);

    //! See \ref pow(Word const&, size_t)
    std::string pow(char const* w, size_t n);

    //! \anchor prod
    //! Take a product from a collection of letters.
    //!
    //! Let \p elts correspond to the ordered set \f$a_0, a_1, \ldots, a_{n -
    //! 1}\f$, \p first to \f$f\f$, \p last to \f$l\f$, and \p step to
    //! \f$s\f$. If \f$f \leq l\f$, let \f$k\f$ be the greatest positive
    //! integer such that \f$f + ks < l\f$. Then the function `prod` returns
    //! the word corresponding to \f$a_f a_{f + s} a_{f + 2s} \cdots a_{f +
    //! ks}\f$. All subscripts are taken modulo \f$n\f$.
    //!
    //! If there is no such \f$k\f$ (i.e. \f$s < 0\f$, or \f$f = l\f$), then
    //! the empty word is returned. Where \f$f > l\f$, the function works
    //! analogously, where \f$k\f$ is the greatest positive integer such that
    //! \f$f + k s > l\f$.
    //!
    //! \param elts the ordered set
    //! \param first the first index
    //! \param last the last index
    //! \param step the step
    //!
    //! \return A word_type or std::string
    //!
    //! \throws LibsemigroupsException if `step = 0`
    //! \throws LibsemigroupsException if \p elts is empty, but the specified
    //! range is not
    //!
    //! \par Examples
    //! \code
    //! word_type w = 012345_w
    //! prod(w, 0, 5, 2)  // Gives the word {0, 2, 4}
    //! prod(w, 1, 9, 2)  // Gives the word {1, 3, 5, 1}
    //! prod(std::string("abcde", 4, 1, -1)  // Gives the string "edc")
    //! \endcode
    template <typename Container,
              typename Word = Container,
              ENABLE_IF_IS_WORD(Word)>
    Word prod(Container const& elts, int first, int last, int step = 1);

    //! Returns the output of `prod` where \p elts is treated as a `word_type`
    //! instead of a vector. See \ref prod "prod".
    template <typename Word, ENABLE_IF_IS_WORD(Word)>
    Word
    prod(std::vector<Word> const& elts, int first, int last, int step = 1) {
      return prod<std::vector<Word>, Word, void>(elts, first, last, step);
    }

    //! Returns `prod(elts, 0, last, 1)` -- see \ref prod "prod".
    template <typename Word, ENABLE_IF_IS_WORD(Word)>
    Word prod(Word const& elts, size_t last) {
      return prod(elts, 0, static_cast<int>(last), 1);
    }

    //! See \ref prod "prod".
    word_type
    prod(std::initializer_list<size_t> ilist, int first, int last, int step);

#undef ENABLE_IF_IS_WORD

    ////////////////////////////////////////////////////////////////////////
    // Operators - implementations
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename>
    Word pow(Word const& x, size_t n) {
      Word y(x);
      pow_inplace(y, n);
      return y;
    }

    template <typename Word, typename>
    void pow_inplace(Word& x, size_t n) {
      Word y(x);
      x.reserve(x.size() * n);
      if (n % 2 == 0) {
        x = Word({});
      }

      while (n > 1) {
        y += y;
        n /= 2;
        if (n % 2 == 1) {
          x += y;
        }
      }
    }

    // Note: we could do a version of the below using insert on words, where
    // the step is +/- 1.
    template <typename Container, typename Word, typename>
    Word prod(Container const& elts, int first, int last, int step) {
      if (step == 0) {
        LIBSEMIGROUPS_EXCEPTION("the 4th argument must not be 0");
      } else if (((first < last && step > 0) || (first > last && step < 0))
                 && elts.size() == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "1st argument must be empty if the given range is not empty");
      }
      Word result;

      if (first < last) {
        if (step < 0) {
          return result;
        }
        result.reserve((last - first) / step);

        int i = first;
        for (; i < last && i < 0; i += step) {
          size_t a = ((-i / elts.size()) + 1) * elts.size() + i;
          result += elts[a];
        }
        for (; i < last; i += step) {
          result += elts[i % elts.size()];
        }
      } else {
        if (step > 0) {
          return result;
        }
        size_t steppos = static_cast<size_t>(-step);
        result.reserve((first - last) / steppos);
        int i = first;
        for (; i > last && i >= 0; i += step) {
          result += elts[i % elts.size()];
        }
        for (; i > last; i += step) {
          size_t a = ((-i / elts.size()) + 1) * elts.size() + i;
          result += elts[a];
        }
      }
      return result;
    }
  }  // namespace words
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_WORDS_HPP_
