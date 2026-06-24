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

#ifndef LIBSEMIGROUPS_TIETZE_HPP_
#define LIBSEMIGROUPS_TIETZE_HPP_

#include <algorithm>    // for sort
#include <cstddef>      // for size_t
#include <numeric>      // for accumulate
#include <queue>        // for queue
#include <string_view>  // for basic_string_view, string_view
#include <tuple>
#include <type_traits>    // for remove_cvref_t
#include <unordered_set>  // for unordered_set
#include <utility>        // for move
#include <vector>         // for vector

#include "adapters.hpp"      // for Hash
#include "constants.hpp"     // for POSITIVE_INFINITY
#include "debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"     // for LIBSEMIGROUPS_EXCEPTION
#include "presentation.hpp"  // for Presentation, operator!=, rep...

#include "detail/fmt.hpp"  // for print

namespace libsemigroups {

  template <typename Word>
  class Subwords {
   private:
    std::pair<Presentation<Word>, Word>  _current;
    size_t                               _current_rule;
    size_t                               _max_length;
    size_t                               _min_length;
    size_t                               _prefix_end;
    std::unordered_set<Word, Hash<Word>> _seen;
    size_t                               _suffix_begin;

    // TODO bool _proper_subwords;

    // TODO mutable std::mutex                         _mtx;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////
    static constexpr bool is_finite     = true;
    static constexpr bool is_idempotent = true;

    using output_type = std::pair<Presentation<Word>, Word> const&;

    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////
    Subwords()
        : _current(),
          _current_rule(),
          _max_length(POSITIVE_INFINITY),
          _min_length(0),
          _prefix_end(),
          _seen(),
          _suffix_begin() {}

    Subwords& init() {
      _current.first.init();
      _current.second.clear();
      _max_length = POSITIVE_INFINITY;
      _min_length = 0;
      _seen.clear();
      return *this;
    }

    Subwords(Subwords const&)            = default;
    Subwords(Subwords&&)                 = default;
    Subwords& operator=(Subwords const&) = default;
    Subwords& operator=(Subwords&&)      = default;

    ~Subwords() = default;

    explicit Subwords(Presentation<Word> const& p) : Subwords() {
      _current.first = p;
      reset();
    }

    explicit Subwords(Presentation<Word>&& p) : Subwords() {
      _current.first = std::move(p);
      reset();
    }

    Subwords& init(Presentation<Word> const& p) {
      init();
      _current.first = p;
      return reset();
    }

    Subwords& init(Presentation<Word>&& p) {
      init();
      _current.first = std::move(p);
      return reset();
    }

    Subwords& presentation(Presentation<Word> const& p) {
      _current.first = p;
      return reset();
    }

    Subwords& reset() {
      _current_rule = 0;
      _seen.clear();
      init_prefix_suffix();
      next();
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] size_t max_length() const noexcept {
      return _max_length;
    }

    Subwords& max_length(size_t val) {
      _max_length = val;
      reset();
      return *this;
    }

    [[nodiscard]] size_t min_length() const noexcept {
      return _min_length;
    }

    Subwords& min_length(size_t val) {
      _min_length = val;
      reset();
      return *this;
    }

    // Subwords&& min_length(size_t val) && {
    //   _min_length = val;
    //   reset();
    //   return std::move(*this);
    // }

    ////////////////////////////////////////////////////////////////////////
    // rx::ranges stuff
    ////////////////////////////////////////////////////////////////////////

    // Important note, you can modify _current.second (Word), because its
    // contents will be discarded at next call of next. You can also modify
    // _current.first (Presentation<Word>) if you put it back into its original
    // state before the next call to next().
    [[nodiscard]] output_type get() const {
      return _current;
    }

    void next() {
      while (_current_rule != _current.first.rules.size()) {
        auto const& rule = _current.first.rules[_current_rule];
        while (_suffix_begin < rule.size()) {
          while (_prefix_end - _suffix_begin <= _max_length
                 && _prefix_end <= rule.size()) {
            auto first = rule.begin() + _suffix_begin;
            auto last  = rule.begin() + _prefix_end;
            if (_seen.emplace(first, last).second) {
              _current.second.assign(first, last);
              if (_prefix_end != rule.size()) {
                ++_prefix_end;
              } else {
                advance_prefix_suffix();
              }
              return;
            }
            ++_prefix_end;
          }
          advance_prefix_suffix();
        }

        ++_current_rule;
        init_prefix_suffix();
      }
    }

    [[nodiscard]] bool at_end() const noexcept {
      return _current_rule == _current.first.rules.size();
    }

    [[nodiscard]] size_t size_hint() const {
      // TODO could be more naunced
      size_t const n = std::accumulate(
          _current.first.rules.begin(),
          _current.first.rules.end(),
          size_t(0),
          [](auto& acc, auto& rule) { return acc + rule.size(); });
      return n * (n + 1) / 2;
    }

    ////////////////////////////////////////////////////////////////////////
    // Piping
    ////////////////////////////////////////////////////////////////////////

    template <typename InputRange>
    struct Range;

    template <typename InputRange,
              typename Settings,
              typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    [[nodiscard]] constexpr auto operator()(InputRange&&    input,
                                            Settings const& settings) {
      // TODO static_assert the output_type of InputRange is a specialization of
      // Presentation
      using Inner = rx::get_range_type_t<InputRange>;
      _min_length = settings.min_length();
      _max_length = settings.max_length();
      return Range<Inner>(std::forward<InputRange>(input), *this);
    }

    ////////////////////////////////////////////////////////////////////////
    // Multi-threading range
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] bool try_get_and_advance(Word& result) {
      // TODO  std::lock_guard lg(_mtx);
      if (!at_end()) {
        result = get();
        next();
        return true;
      }
      return false;
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // Private
    ////////////////////////////////////////////////////////////////////////
    void advance_prefix() {
      LIBSEMIGROUPS_ASSERT(_current_rule < _current.first.rules.size());
      size_t const n = _current.first.rules[_current_rule].size();
      if (_prefix_end + _min_length <= n) {
        _prefix_end += _min_length;
      } else {
        _prefix_end = n + 1;
      }
    }

    void init_prefix_suffix() {
      _suffix_begin = 0;
      _prefix_end   = 0;
      if (_current_rule < _current.first.rules.size()) {
        advance_prefix();
      }
    }

    void advance_prefix_suffix() {
      LIBSEMIGROUPS_ASSERT(_current_rule < _current.first.rules.size());
      ++_suffix_begin;
      _prefix_end = _suffix_begin;
      advance_prefix();
    }
  };  // class Subwords

  template <typename Word>
  template <typename InputRange>
  // TODO struct -> class
  struct Subwords<Word>::Range {
    using output_type = typename Subwords<Word>::output_type;

    static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
    static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

    InputRange     _input;
    Subwords<Word> _subwords;

    // TODO static_assert that InputRange::output_type is Presentation<Word>
    Range(InputRange const& input, Subwords const& subwords)
        // Init _subwords with subwords to copy the settings
        : _input(input), _subwords(subwords) {
      if (!_input.at_end()) {
        // Reset the presentation, not init, so that we retain the settings
        _subwords.presentation(_input.get());
      }
    }

    // TODO static_assert that InputRange::output_type is Presentation<Word>
    Range(InputRange&& input, Subwords const& subwords)
        : _input(std::move(input)), _subwords(subwords) {
      if (!_input.at_end()) {
        // Reset the presentation, not init, so that we retain the settings
        _subwords.presentation(_input.get());
      }
    }

    [[nodiscard]] output_type get() const {
      return _subwords.get();
    }

    void next() noexcept {
      if (!at_end()) {
        _subwords.next();
        if (_subwords.at_end()) {
          _input.next();
          if (!_input.at_end()) {
            // Reset the presentation, not init, so that we retain the settings
            _subwords.presentation(_input.get());
          }
        }
      }
    }

    [[nodiscard]] constexpr bool at_end() const noexcept {
      return _input.at_end();
    }

    [[nodiscard]] constexpr size_t size_hint() const noexcept {
      return _input.size_hint() * _subwords.size_hint();
    }
  };  // struct Subwords::Range

  // TODO class
  class SubwordsOf {
   private:
    size_t _min_length;
    size_t _max_length;

   public:
    SubwordsOf() : _min_length(0), _max_length(POSITIVE_INFINITY) {}

    SubwordsOf(SubwordsOf const&)            = default;
    SubwordsOf(SubwordsOf&&)                 = default;
    SubwordsOf& operator=(SubwordsOf const&) = default;
    SubwordsOf& operator=(SubwordsOf&&)      = default;

    ~SubwordsOf() = default;

    template <typename InputRange>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      // TODO static_assert that InputRange::output_type is a specialization of
      // Presentation
      using Word = typename std::decay_t<  // Yuck!
          typename std::decay_t<InputRange>::output_type>::word_type;
      // We pass *this thru so that the settings are copied too
      return Subwords<Word>{}(std::forward<InputRange>(input), *this);
    }

    [[nodiscard]] size_t min_length() const noexcept {
      return _min_length;
    }

    SubwordsOf& min_length(size_t val) {
      _min_length = val;
      return *this;
    }

    [[nodiscard]] size_t max_length() const noexcept {
      return _max_length;
    }

    SubwordsOf& max_length(size_t val) {
      _max_length = val;
      return *this;
    }
  };

  template <typename InputRange>
  class TietzeAddGeneratorsRange {
   public:
    using native_word_type
        = std::tuple_element_t<1,
                               std::decay_t<typename InputRange::output_type>>;

   private:
    InputRange _input;
    // TODO remove, just modify the incoming presentation, then unmodify it
    Presentation<native_word_type> _get_presentation;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////
    static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
    static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

    using output_type = Presentation<native_word_type> const&;

    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////
    // TODO? init functions?
    TietzeAddGeneratorsRange(InputRange const& input)
        : _input(input), _get_presentation() {
      if (!_input.at_end()) {
        auto const& value = _input.get();
        _get_presentation = value.first;
        presentation::replace_word_with_new_generator(_get_presentation,
                                                      value.second);
      }
    }
    // TODO rval ref constructor

    ////////////////////////////////////////////////////////////////////////
    // rx::ranges stuff
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] output_type get() const {
      return _get_presentation;
    }

    void next() {
      _input.next();
      if (!_input.at_end()) {
        _get_presentation = _input.get().first;
        presentation::replace_word_with_new_generator(_get_presentation,
                                                      _input.get().second);
      }
    }

    [[nodiscard]] bool at_end() const {
      return _input.at_end();
    }

    [[nodiscard]] size_t size_hint() const {
      return _input.size_hint();
    }
  };  // class TietzeAddGeneratorsRange

  struct TietzeAddGenerators {
    TietzeAddGenerators() = default;

    template <typename InputRange>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      using Inner = rx::get_range_type_t<InputRange>;

      return TietzeAddGeneratorsRange<Inner>(std::forward<InputRange>(input));
    }
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TIETZE_HPP_
