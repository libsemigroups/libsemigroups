//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell + Joseph Edwards
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

// This file contains declarations for classes related to converting to
// word-types and converting to strings.

#ifndef LIBSEMIGROUPS_TO_WORD_HPP_
#define LIBSEMIGROUPS_TO_WORD_HPP_

#include <cstddef>           // for size_t
#include <initializer_list>  // for initializer_list
#include <string>            // for basic_string
#include <string_view>       // for string_view
#include <type_traits>       // for enable_if_t
#include <unordered_map>     // for unordered_map, operator==
#include <utility>           // for forward

#include "debug.hpp"          // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"      // for LibsemigroupsException
#include "ranges.hpp"         // for begin, end
#include "types.hpp"          // for word_type
#include "words-helpers.hpp"  // for human_readable_index

namespace libsemigroups {
  ////////////////////////////////////////////////////////////////////////
  // Strings -> Words
  ////////////////////////////////////////////////////////////////////////

  namespace v4 {
    //! \ingroup words_group
    //! \brief Class for converting strings to \ref word_type with specified
    //! alphabet.
    //!
    //! Defined in `word-range.hpp`.
    //!
    //! An instance of this class is used to convert from std::string to
    //! \ref word_type. The characters in the string are converted to integers
    //! according to their position in the alphabet used to construct a ToWord
    //! instance if one is provided, or using \ref words::human_readable_index
    //! otherwise.
    //!
    //! \par Example
    //! \code
    //! ToWord toword("bac");
    //! toword("bac");       //-> 012_w
    //! toword("bababbbcbc");//-> 0101000202_w
    //!
    //! toword.init();
    //! toword("bac");        //-> 102_w
    //! \endcode
    // TODO (later) a version that takes a word_type, so that we can permute the
    // letters in a word
    // TODO(0) remove default template param
    template <typename From>
    class ToWord {
     private:
      std::unordered_map<typename From::value_type, letter_type> _alphabet_map;

     public:
      //! \brief The type of values an instance of ToWord will convert into
      //! \ref word_type.
      using from_type = From;

      //! \brief Default constructor.
      //!
      //! Constructs an empty object with no alphabet set.
      ToWord() : _alphabet_map() {
        init();
      }

      //! \brief Default copy constructor.
      //!
      //! Default copy constructor.
      ToWord(ToWord const&);

      //! \brief Default move constructor.
      //!
      //! Default move constructor.
      ToWord(ToWord&&);

      //! \brief Default copy assignment.
      //!
      //! Default copy assignment.
      ToWord& operator=(ToWord const&);

      //! \brief Default move assignment.
      //!
      //! Default move assignment.
      ToWord& operator=(ToWord&&);

      //! \brief Default destructor.
      //!
      //! Default destructor.
      ~ToWord();

      //! \brief Initialize an existing ToWord object.
      //!
      //! This function puts a ToWord object back into the same state as if it
      //! had been newly default constructed.
      //!
      //! \returns A reference to \c *this.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \sa ToWord()
      ToWord& init() {
        _alphabet_map.clear();
        return *this;
      }

      //! \brief Construct with given alphabet.
      //!
      //! Construct a ToWord object with the given alphabet.
      //!
      //! \param alphabet the alphabet.
      //!
      //! \throws LibsemigroupsException if there are repeated letters in
      //! \p alphabet.
      explicit ToWord(From const& alphabet) : _alphabet_map() {
        init(alphabet);
      }

      //! \brief Initialize an existing ToWord object.
      //!
      //! This function puts a ToWord object back into the same state as if it
      //! had been newly constructed from \p alphabet.
      //!
      //! \param alphabet the alphabet.
      //!
      //! \returns A reference to \c *this.
      //!
      //! \throws LibsemigroupsException if there are repeated letters in
      //! \p alphabet.
      //!
      //! \sa ToWord(From const& alphabet)
      ToWord& init(From const& alphabet);

      //! \brief Check if the alphabet is defined.
      //!
      //! This function returns \c true if no alphabet has been defined, and
      //! \c false otherwise.
      //!
      //! \returns A value of type \c bool.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] bool empty() const noexcept {
        return _alphabet_map.empty();
      }

      //! \brief Return the alphabet used for conversion.
      //!
      //! This function returns a std::string corresponding to the ordered-set
      //! alphabet \f$\{a_0, a_1, \dots, a_{n-1}\}\f$ that the initialised
      //! ToWord object will use to convert from std::string to \ref word_type.
      //! Specifically, \f$a_i \mapsto i\f$ where \f$a_i\f$ will correspond to a
      //! letter in a std::string, and \f$i\f$ is a \ref letter_type.
      //!
      //! If this function returns the empty string, then conversion will be
      //! performed using \ref words::human_readable_index.
      //!
      //! \returns A value of type std::string.
      //!
      //! \exceptions
      //! \no_libsemigroups_except.
      [[nodiscard]] from_type alphabet() const;

      //! Check if the current ToWord instance can convert a specified letter.
      //!
      //! This function returns \c true if \p c can can be converted to a
      //! \ref letter_type using this ToWord instance, and \c false otherwise.
      //!
      //! \param c the char to check the convertibility of.
      //!
      //! \returns A value of type bool.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      [[nodiscard]] bool
      can_convert_letter(typename from_type::value_type const& c) const {
        return _alphabet_map.count(c) == 1;
      }

      // TODO remove "string" from all the doc here
      //! \brief Convert a string to a word_type.
      //!
      //! This function converts its second argument \p input into a word_type
      //! and stores the result in the first argument \p output. The characters
      //! of
      //! \p input are converted using the alphabet used to construct the object
      //! or set via init(), or with \ref words::human_readable_index if
      //! \ref empty returns `true`.
      //!
      //! The contents of the first argument \p output, if any, is removed.
      //!
      //! \param output word to hold the result.
      //! \param input the string to convert.
      //!
      //! \warning This functions performs no checks on its arguments. In
      //! particular, if the alphabet used to define an instance of ToWord is
      //! not empty, and \p input contains letters that do not correspond to
      //! letters of the alphabet, then bad things will happen.
      //!
      //! \sa
      //! * \ref literals
      void call_no_checks(word_type& output, From const& input) const;

      //! \brief Convert a string to a word_type.
      //!
      //! This function converts its argument \p input into a word_type. The
      //! characters of \p input are converted using the alphabet used to
      //! construct the object or set via init(), or with
      //! \ref words::human_readable_index if \ref empty returns `true`.
      //!
      //! \param input the string to convert.
      //!
      //! \warning This functions performs no checks on its arguments. In
      //! particular, if the alphabet used to define an instance of ToWord is
      //! not empty, and \p input contains letters that do not correspond to
      //! letters of the alphabet, then bad things will happen.
      //!
      //! \sa
      //! * \ref literals
      [[nodiscard]] word_type call_no_checks(From const& input) const {
        word_type output;
        call_no_checks(output, input);
        return output;
      }

      //! \brief Convert a string to a word_type.
      //!
      //! This function converts its second argument \p input into a word_type
      //! and stores the result in the first argument \p output. The characters
      //! of
      //! \p input are converted using the alphabet used to construct the object
      //! or set via init(), or with \ref words::human_readable_index if
      //! \ref empty returns `true`.
      //!
      //! The contents of the first argument \p output, if any, is removed.
      //!
      //! \param output word to hold the result.
      //! \param input the string to convert.
      //!
      //! \throw LibsemigroupsException if the alphabet used to define an
      //! instance of ToWord is not empty and \p input contains letters that do
      //! not correspond to letters of the alphabet.
      //!
      //! \sa
      //! * \ref literals
      void operator()(word_type& output, From const& input) const;

      //! \brief Convert a string to a word_type.
      //!
      //! This function converts its argument \p input into a word_type The
      //! characters of \p input are converted using the alphabet used to
      //! construct the object or set via init(), or with
      //! \ref words::human_readable_index if \ref empty returns `true`.
      //!
      //! \param input the string to convert.
      //!
      //! \throw LibsemigroupsException if the alphabet used to define an
      //! instance of ToWord is not empty and \p input contains letters that do
      //! not correspond to letters of the alphabet.
      //!
      //! \sa
      //! * \ref literals
      [[nodiscard]] word_type operator()(From const& input) const {
        word_type output;
                  operator()(output, input);
        return output;
      }

      // TODO remove reference to char in the doc
      //! \brief Convert a `char` to a \ref letter_type.
      //!
      //! This function converts its argument \p input into a letter_type. It is
      //! converted using the alphabet used to construct the object or set via
      //! init(), or with \ref words::human_readable_index if \ref empty returns
      //! `true`.
      //!
      //! \param input the character to convert.
      //!
      //! \throw LibsemigroupsException if the alphabet used to define an
      //! instance of ToWord is not empty and \p input does not correspond to a
      //! letter of the alphabet.
      //!
      //! \sa
      //! * \ref literals
      [[nodiscard]] letter_type
      operator()(typename From::value_type input) const {
        // TODO improve this
        // FIXME(1) it also doesn't work for example to_word('a') returns 63 for
        // some reason
        word_type output;
        // operator()(output, std::string_view(&input, 1));
        operator()(output, std::string(input, 1));
        return output[0];
      }

      //! \brief Convert a `char` to a \ref letter_type.
      //!
      //! This function converts its argument \p input into a letter_type. It is
      //! converted using the alphabet used to construct the object or set via
      //! init(), or with \ref words::human_readable_index if \ref empty returns
      //! `true`.
      //!
      //! \param input the character to convert.
      //!
      //! \warning This functions performs no checks on its arguments. In
      //! particular, if the alphabet used to define an instance of ToWord is
      //! not empty, and \p input does not correspond to a letter of the
      //! alphabet, then bad things will happen.
      //!
      //! \sa
      //! * \ref literals
      [[nodiscard]] letter_type
      call_no_checks(typename From::value_type input) const {
        return _alphabet_map.find(input)->second;
      }

      template <typename InputRange>
      struct Range;

      //! \brief Call operator for combining with other range objects.
      //!
      //! A custom combinator for rx::ranges to convert the output of a
      //! StringRange object into \ref word_type, that can be combined with
      //! other combinators using `operator|`.
      //!
      //! \par Example
      //! \code
      //! using rx::operator|;
      //!  StringRange strings;
      //!  strings.alphabet("ab").first("a").last("bbbb");
      //!  auto words = (strings | ToWord("ba"));
      //!  // contains the words
      //!  // {1_w,    0_w,    11_w,   10_w,   01_w,   00_w,   111_w,
      //!  //  110_w,  101_w,  100_w,  011_w,  010_w,  001_w,  000_w,
      //!  //  1111_w, 1110_w, 1101_w, 1100_w, 1011_w, 1010_w, 1001_w,
      //!  //  1000_w, 0111_w, 0110_w, 0101_w, 0100_w, 0011_w, 0010_w,
      //!  //  0001_w}));
      //! \endcode
      template <typename InputRange,
                typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
      [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
        using Inner = rx::get_range_type_t<InputRange>;
        return Range<Inner>(std::forward<InputRange>(input), *this);
      }
    };  // class ToWord

    template <size_t N>
    ToWord(const char (&)[N]) -> ToWord<std::string>;

    template <typename From>
    template <typename InputRange>
    struct ToWord<From>::Range {
      using output_type = word_type;

      static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
      static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

      InputRange   _input;
      ToWord<From> _to_word;

      explicit Range(InputRange const& input, ToWord<From> const& t_wrd)
          : _input(input), _to_word(t_wrd) {}

      explicit Range(InputRange&& input, ToWord<From> const& t_wrd)
          : _input(std::move(input)), _to_word(t_wrd) {}

      explicit Range(InputRange const& input, ToWord<From>&& t_wrd)
          : _input(input), _to_word(std::move(t_wrd)) {}

      explicit Range(InputRange&& input, ToWord<From>&& t_wrd)
          : _input(std::move(input)), _to_word(std::move(t_wrd)) {}

      // Not noexcept because ToWord<From>()() isn't
      [[nodiscard]] output_type get() const {
        return _to_word.operator()(_input.get());
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

      [[nodiscard]] constexpr size_t count() const noexcept {
        return _input.count();
      }
    };

    //! \ingroup words_group
    //!
    //! \brief Return a human readable representation of a ToWord object.
    //!
    //! Return a human readable representation of a ToWord object.
    //!
    //! \param twrd the ToWord object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename From>
    [[nodiscard]] std::string to_human_readable_repr(ToWord<From> const& twrd) {
      return fmt::format("<ToWord object with alphabet \"{}\">",
                         twrd.alphabet());
    }

  }  // namespace v4

  using ToWord [[deprecated]] = v4::ToWord<std::string>;

  ////////////////////////////////////////////////////////////////////////
  // Words -> Strings
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup words_group
  //! \brief Class for converting \ref word_type into std::string with specified
  //! alphabet.
  //!
  //! Defined in `word-range.hpp`.
  //!
  //! An instance of this class is used to convert from \ref word_type to
  //! std::string. The letters in the word are converted to characters
  //! according to their position in the alphabet used to construct a ToString
  //! instance if one is provided, or using \ref words::human_readable_letter
  //! otherwise.
  //!
  //! \par Example
  //! \code
  //! ToString tostring("bac");
  //! tostring(word_type({1, 0, 2}));                 //-> "abc"
  //! tostring(word_type({0, 1, 1, 0, 1, 1, 0, 2}));  //-> "baabaabc"
  //!
  //! tostring.init();
  //! tostring(word_type({1, 0, 2}));                 //-> "bac"
  //! \endcode
  class ToString {
   public:
    //! \brief Default constructor.
    //!
    //! Constructs an empty object with no alphabet set.
    ToString() : _alphabet_map() {
      init();
    }

    // TODO (later) noexcept?
    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    ToString(ToString const&);

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    ToString(ToString&&);

    //! \brief Default copy assignment.
    //!
    //! Default copy assignment.
    ToString& operator=(ToString const&);

    //! \brief Default move assignment.
    //!
    //! Default move assignment.
    ToString& operator=(ToString&&);

    //! \brief Default destructor.
    //!
    //! Default destructor.
    ~ToString();

    //! \brief Initialize an existing ToString object.
    //!
    //! This function puts a ToString object back into the same state as if it
    //! had been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa ToString()
    ToString& init() noexcept {
      _alphabet_map.clear();
      return *this;
    }

    //! \brief Construct with given alphabet.
    //!
    //! Construct a ToString object with the given alphabet.
    //!
    //! \param alphabet the alphabet.
    //!
    //! \throws LibsemigroupsException if there are repeated letters in
    //! \p alphabet.
    explicit ToString(std::string const& alphabet) : _alphabet_map() {
      init(alphabet);
    }

    //! \brief Initialize an existing Tostring object.
    //!
    //! This function puts a ToString object back into the same state as if it
    //! had been newly constructed from \p alphabet.
    //!
    //! \param alphabet the alphabet.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if there are repeated letters in
    //! \p alphabet.
    //!
    //! \sa ToString(std::string const& alphabet)
    ToString& init(std::string const& alphabet);

    //! \brief Check if the alphabet is defined.
    //!
    //! This function return \c true if no alphabet has been defined, and
    //! \c false otherwise.
    //!
    //! \returns A value of type \c bool.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] bool empty() const noexcept {
      return _alphabet_map.empty();
    }

    //! \brief Return the alphabet used for conversion.
    //!
    //! This function returns a std::string corresponding to the ordered-set
    //! alphabet \f$\{a_0, a_1, \dots a_{n-1}\}\f$ that the initialised ToString
    //! object will use to convert from a \ref word_type to a std::string.
    //! Specifically, \f$i\mapsto a_i\f$ where \f$i\f$ will correspond to a
    //! letter in a word_type, and \f$a_i\f$ is a \c char.
    //!
    //! If this function returns the empty string, then conversion will be
    //! performed using \ref words::human_readable_index.
    //!
    //! \returns A value of type std::string.
    //!
    //! \exceptions
    //! \no_libsemigroups_except.
    [[nodiscard]] std::string alphabet() const;

    //! Check if the current ToString instance can convert a specified letter.
    //!
    //! This function returns \c true if \p l can can be converted to a `char`
    //! using this ToString instance, and \c false otherwise.
    //!
    //! \param l the letter to check the convertibility of.
    //!
    //! \returns A value of type bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    [[nodiscard]] bool can_convert_letter(letter_type const& l) const {
      return _alphabet_map.count(l) == 1;
    }

    //! \brief Convert a \ref word_type to a std::string.
    //!
    //! This function converts its second argument \p input into a std::string
    //! and stores the result in the first argument \p output. The characters of
    //! \p input are converted using the alphabet used to construct the object
    //! or set via init(), or with \ref words::human_readable_letter if
    //! \ref empty returns `true`.
    //!
    //! The contents of the first argument \p output, if any, is removed.
    //!
    //! \param output std::string to hold the result.
    //! \param input the \ref word_type to convert.
    //!
    //! \warning This functions performs no checks on its arguments. In
    //! particular, if the alphabet used to define an instance of ToString is
    //! not empty, and \p input contains letters that do not correspond to
    //! letters of the alphabet, then bad things will happen.
    //!
    //! \sa
    //! * \ref literals
    void call_no_checks(std::string& output, word_type const& input) const;

    //! \brief Convert a \ref word_type to a std::string.
    //!
    //! This function converts its argument \p input into a std::string. The
    //! characters of \p input are converted using the alphabet used to
    //! construct the object or set via init(), or with
    //! \ref words::human_readable_letter if \ref empty returns `true`.
    //!
    //! \param input the \ref word_type to convert.
    //!
    //! \warning This functions performs no checks on its arguments. In
    //! particular, if the alphabet used to define an instance of ToString is
    //! not empty, and \p input contains letters that do not correspond to
    //! letters of the alphabet, then bad things will happen.
    //!
    //! \sa
    //! * \ref literals
    [[nodiscard]] std::string call_no_checks(word_type const& input) const {
      std::string output;
      call_no_checks(output, input);
      return output;
    }

    //! \brief Convert a \ref word_type to a std::string.
    //!
    //! This function converts its second argument \p input into a std::string
    //! and stores the result in the first argument \p output. The characters of
    //! \p input are converted using the alphabet used to construct the object
    //! or set via init(), or with \ref words::human_readable_letter if
    //! \ref empty returns `true`.
    //!
    //! The contents of the first argument \p output, if any, is removed.
    //!
    //! \param output word to hold the result.
    //! \param input the string to convert.
    //!
    //! \throw LibsemigroupsException if the alphabet used to define an instance
    //! of ToString is not empty and \p input contains letters that do not
    //! correspond to letters of the alphabet.
    //!
    //! \sa
    //! * \ref literals
    void operator()(std::string& output, word_type const& input) const;

    //! \brief Convert a \ref word_type to a std::string.
    //!
    //! This function converts its argument \p input into a std::string. The
    //! characters of \p input are converted using the alphabet used to
    //! construct the object or set via init(), or with
    //! \ref words::human_readable_letter if \ref empty returns `true`.
    //!
    //! \param input the string to convert.
    //!
    //! \throw LibsemigroupsException if the alphabet used to define an instance
    //! of ToString is not empty and \p input contains letters that do not
    //! correspond to letters of the alphabet.
    //!
    //! \sa
    //! * \ref literals
    [[nodiscard]] std::string operator()(word_type const& input) const {
      std::string output;
                  operator()(output, input);
      return output;
    }

    //! \brief Convert a std::initializer_list to a std::string.
    //!
    //! This function converts its argument \p input into a std::string, by
    //! first converting it to a \ref word_type. The characters of \p input are
    //! converted using the alphabet used to construct the object or set via
    //! init(), or with \ref words::human_readable_letter if \ref empty returns
    //! `true`.
    //!
    //! \param input the initializer list to convert.
    //!
    //! \throw LibsemigroupsException if the alphabet used to define an instance
    //! of ToString is not empty and \p input contains letters that do not
    //! correspond to letters of the alphabet.
    //!
    //! \sa
    //! * \ref literals
    template <typename Int>
    [[nodiscard]] std::string
    operator()(std::initializer_list<Int> input) const {
      static_assert(std::is_integral_v<Int>);
      word_type copy(input.begin(), input.end());
      return    operator()(copy);
    }

    template <typename InputRange>
    struct Range;

    //! \brief Call operator for combining with other range objects.
    //!
    //! A custom combinator for rx::ranges to convert the output of a WordRange
    //! object into std::string, that can be combined with other combinators
    //! using `operator|`.
    //!
    //! \par Example
    //! \code
    //! using rx::operator|;
    //! WordRange words;
    //! words.alphabet_size(1).min(0).max(10);
    //!
    //! auto strings = (words | ToString("a"));
    //! // Contains the strings
    //! // {"", "a", "aa", "aaa", "aaaa", "aaaaa", "aaaaaa", "aaaaaaa",
    //! // "aaaaaaaa", "aaaaaaaaa"};
    //! \endcode
    template <typename InputRange,
              typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
      using Inner = rx::get_range_type_t<InputRange>;
      return Range<Inner>(std::forward<InputRange>(input), *this);
    }

   private:
    // We could use std::vector<char> (or similar) here, but an
    // unordered_ordered hap has been used instead to allow for potential future
    // conversions between different types.
    std::unordered_map<letter_type, char> _alphabet_map;
  };

  template <typename InputRange>
  struct ToString::Range {
    using output_type = std::string;

    static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
    static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

    InputRange _input;
    ToString   _to_string;

    Range(InputRange const& input, ToString const& t_str)
        : _input(input), _to_string(t_str) {}

    Range(InputRange&& input, ToString const& t_str)
        : _input(std::move(input)), _to_string(t_str) {}

    Range(InputRange const& input, ToString&& t_str)
        : _input(input), _to_string(std::move(t_str)) {}

    Range(InputRange&& input, ToString&& t_str)
        : _input(std::move(input)), _to_string(std::move(t_str)) {}

    ~Range();

    // Not noexcept because ToString()() isn't
    [[nodiscard]] output_type get() const {
      return _to_string(_input.get());
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

    [[nodiscard]] constexpr size_t count() const noexcept {
      return _input.count();
    }
  };

  // NOTE: This is a terrible hack to avoid compiler warnings. Maybe remove in
  // the future?
#if defined(__clang__)
  template <typename InputRange>
  ToString::Range<InputRange>::~Range<InputRange>() = default;
#elif defined(__GNUC__)
  template <typename InputRange>
  ToString::Range<InputRange>::~Range() = default;
#endif

  //! \ingroup words_group
  //!
  //! \brief Return a human readable representation of a ToString object.
  //!
  //! Return a human readable representation of a ToString object.
  //!
  //! \param tstr the ToString object.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  [[nodiscard]] inline std::string
  to_human_readable_repr(ToString const& tstr) {
    return fmt::format("<ToString object with alphabet \"{}\">",
                       tstr.alphabet());
  }
}  // namespace libsemigroups

#include "to-word.tpp"

#endif  // LIBSEMIGROUPS_TO_WORD_HPP_
