//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Standard constructors + initializers
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  Alphabet<Word>::Alphabet() = default;

  template <typename Word>
  Alphabet<Word>& Alphabet<Word>::init() {
    _letters.clear();
    _letters_map.clear();
    return *this;
  }

  template <typename Word>
  Alphabet<Word>::Alphabet(Alphabet const&) = default;

  template <typename Word>
  Alphabet<Word>::Alphabet(Alphabet&&) = default;

  template <typename Word>
  Alphabet<Word>& Alphabet<Word>::operator=(Alphabet const&) = default;

  template <typename Word>
  Alphabet<Word>& Alphabet<Word>::operator=(Alphabet&&) = default;

  template <typename Word>
  Alphabet<Word>::~Alphabet() = default;

  ////////////////////////////////////////////////////////////////////////
  // Alphabet specific constructors + initializers
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  Alphabet<Word>::Alphabet(size_type n) : Alphabet() {
    init(n);
  }

  template <typename Word>
  Alphabet<Word>& Alphabet<Word>::init(size_type n) {
    // This checks that there are enough distinct Word::value_types to construct
    // an alphabet of size n. If the size of Word::value_type is the same as the
    // size of size_t, then one cannot specify the size of an alphabet large
    // enough for which there are not enough distinct letters to be contained
    // within it.
    if constexpr (sizeof(typename Word::value_type) < sizeof(size_t)) {
      if (n > 1 + std::numeric_limits<native_letter_type>::max()
                  - std::numeric_limits<native_letter_type>::min()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected a value in the range [0, {}), found {}",
            std::numeric_limits<native_letter_type>::max()
                - std::numeric_limits<native_letter_type>::min() + 2,
            n);
      }
    }
    native_word_type lphbt(n, 0);

    // The below assertions exist to insure that we are not badly assigning
    // values. The subsequent pragmas exist to suppress the false-positive
    // warnings produced by g++ 13.2.0
    static_assert(
        std::is_same_v<std::decay_t<decltype(*lphbt.begin())>,
                       decltype(words::human_readable_letter<Word>(0))>);
#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
    std::iota(
        lphbt.begin(), lphbt.end(), words::human_readable_letter<Word>(0));
#pragma GCC diagnostic pop
    return init(lphbt);
  }

  template <typename Word>
  Alphabet<Word>::Alphabet(native_word_type const& letters) : Alphabet() {
    init(letters);
  }

  template <typename Word>
  Alphabet<Word>& Alphabet<Word>::init(native_word_type const& lphbt) {
    // We copy the _letters_map for exception safety
    decltype(_letters_map) letters_map;
    auto                   old_letters(std::move(_letters));
    _letters = lphbt;
    try_set_letters(letters_map, old_letters);
    return *this;
  }

  template <typename Word>
  Alphabet<Word>::Alphabet(native_word_type&& lphbt) : Alphabet() {
    init(std::move(lphbt));
  }

  template <typename Word>
  Alphabet<Word>& Alphabet<Word>::init(native_word_type&& lphbt) {
    // We copy the _letters_map for exception safety
    decltype(_letters_map) letters_map;
    auto                   old_letters = std::move(_letters);
    _letters                           = std::move(lphbt);
    try_set_letters(letters_map, old_letters);
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Validation
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  void
  Alphabet<Word>::throw_if_letter_not_in_alphabet(native_letter_type c) const {
    if (empty()) {
      LIBSEMIGROUPS_EXCEPTION("there are no letters in the alphabet");
    } else if (_letters_map.find(c) == _letters_map.cend()) {
      auto msg = fmt::format("invalid letter {}, valid letters are {}",
                             detail::to_printable(c),
                             detail::to_printable(_letters));
      if constexpr (std::is_same_v<native_letter_type, char>) {
        if (!std::isprint(c) && detail::isprint(_letters)) {
          msg += fmt::format(
              " == {}", std::vector<int>(_letters.begin(), _letters.end()));
        }
      }
      LIBSEMIGROUPS_EXCEPTION(msg);
    }
  }

  template <typename Word>
  template <typename Iterator1, typename Iterator2>
  void Alphabet<Word>::throw_if_letter_not_in_alphabet(Iterator1 first,
                                                       Iterator2 last) const {
    for (auto it = first; it != last; ++it) {
      throw_if_letter_not_in_alphabet(*it);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Alphabet attributes
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  typename Alphabet<Word>::native_letter_type
  Alphabet<Word>::letter(size_type i) const {
    if (i >= size()) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected a value in [0, {}), found {}", size(), i);
    }
    return letter_no_checks(i);
  }

  ////////////////////////////////////////////////////////////////////////
  // Alphabet modifiers
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  Alphabet<Word>& Alphabet<Word>::add_letter_no_checks(
      typename Alphabet<Word>::native_letter_type x) {
    size_t index = _letters_map.size();
#ifdef LIBSEMIGROUPS_DEBUG
    auto inserted = _letters_map.emplace(x, index);
    LIBSEMIGROUPS_ASSERT(inserted.second);
#else
    _letters_map.emplace(x, index);
#endif
    _letters.push_back(x);
    return *this;
  }

  template <typename Word>
  Alphabet<Word>&
  Alphabet<Word>::add_letter(typename Alphabet<Word>::native_letter_type x) {
    if (contains(x)) {
      LIBSEMIGROUPS_EXCEPTION("the argument {} already belongs to the alphabet "
                              "{}, expected an unused letter",
                              detail::to_printable(x),
                              detail::to_printable(letters()));
    }
    return add_letter_no_checks(x);
  }

  ////////////////////////////////////////////////////////////////////////
  // Private mem fns
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  void Alphabet<Word>::try_set_letters(decltype(_letters_map)& letters_map,
                                       native_word_type&       old_letters) {
    try {
      throw_if_duplicate_letters(letters_map);
      _letters_map = std::move(letters_map);
    } catch (LibsemigroupsException&) {
      _letters = std::move(old_letters);
      throw;
    }
  }

  template <typename Word>
  void Alphabet<Word>::throw_if_duplicate_letters(
      decltype(_letters_map)& letters_map) const {
    LIBSEMIGROUPS_ASSERT(letters_map.empty());
    size_type index = 0;
    for (auto const& letter : _letters) {
      auto it = letters_map.emplace(letter, index++);
      if (!it.second) {
        LIBSEMIGROUPS_EXCEPTION("invalid alphabet {}, duplicate letter {}!",
                                detail::to_printable(_letters),
                                detail::to_printable(letter));
      }
    }
  }

  template <typename Word>
  Alphabet<Word>& Alphabet<Word>::remove_letter_no_checks(
      typename Alphabet<Word>::native_letter_type x) {
    size_t const index = _letters_map[x];
    _letters_map.erase(x);
    auto const start = _letters.begin() + index;
    for (auto it = start + 1; it != _letters.end(); ++it) {
      --_letters_map[*it];
    }
    _letters.erase(start, start + 1);
    return *this;
  }

  template <typename Word>
  Alphabet<Word>&
  Alphabet<Word>::remove_letter(typename Alphabet<Word>::native_letter_type x) {
    if (contains(x)) {
      remove_letter_no_checks(x);
    } else {
      LIBSEMIGROUPS_EXCEPTION("the argument {} does not belong to the alphabet "
                              "{}, expected an existing letter",
                              detail::to_printable(x),
                              detail::to_printable(letters()));
    }
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // String representations
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  std::string to_human_readable_repr(Alphabet<Word> const& alphabet) {
    using detail::group_digits;
    if (0 < alphabet.size() && alphabet.size() < 10) {
      return fmt::format("<alphabet {}>",
                         detail::to_printable(alphabet.letters()));
    }
    // Note that alphabet.size() != 1 below so don't need to de-pluralise
    // "letters"
    return fmt::format("<alphabet with {} letters>",
                       group_digits(alphabet.size()));
  }

  template <typename Word>
  std::string to_input_string(Alphabet<Word> const& alphabet,
                              std::string const&    braces) {
    if (braces.size() != 2) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd argument (braces) must have length 2, "
                              "but found {} of length {}",
                              braces,
                              braces.size());
    }
    return fmt::format("Alphabet({}{}{})",
                       braces[0],
                       fmt::join(alphabet.letters(), ", "),
                       braces[1]);
  }

  ////////////////////////////////////////////////////////////////////////
  // Alphabet -> Alphabet
  ////////////////////////////////////////////////////////////////////////

  template <typename Result, typename Word, typename Func>
  auto to(Alphabet<Word> const& alphabet, Func&& f) -> std::enable_if_t<
      std::is_same_v<Alphabet<typename Result::native_word_type>, Result>,
      Result> {
    using WordOutput = typename Result::native_word_type;

    static_assert(
        std::is_invocable_v<std::decay_t<Func>,
                            typename Alphabet<Word>::native_letter_type>);

    validate(alphabet);

    WordOutput new_letters;
    new_letters.resize(alphabet.letters().size());
    std::transform(alphabet.letters().cbegin(),
                   alphabet.letters().cend(),
                   new_letters.begin(),
                   f);
    return Result(std::move(new_letters));
  }

}  // namespace libsemigroups
