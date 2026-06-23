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

#include "presentation.hpp"

namespace libsemigroups {

  template <typename Word>
  class Subwords {
   private:
    Word                                       _current;
    typename std::vector<Word>::const_iterator _current_rule;
    typename Word::const_iterator              _prefix;
    Presentation<Word>                         _presentation;
    std::unordered_set<Word, Hash<Word>>       _seen;
    typename Word::const_iterator              _suffix;

    // TODO mutable std::mutex                         _mtx;
    // TODO settings like min. length + max. length

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////
    static constexpr bool is_finite     = true;
    static constexpr bool is_idempotent = true;

    using output_type = Word const&;

    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////
    // TODO? init functions?
    Subwords()
        : _current(),
          _current_rule(),
          _prefix(),
          _presentation(),
          _seen(),
          _suffix() {}

    Subwords(Subwords const&)            = default;
    Subwords(Subwords&&)                 = default;
    Subwords& operator=(Subwords const&) = default;
    Subwords& operator=(Subwords&&)      = default;

    ~Subwords() = default;

    explicit Subwords(Presentation<Word> const& p) : Subwords() {
      _presentation = p;
      _current_rule = _presentation.rules.cbegin();
      _suffix       = _current_rule->cbegin();
      _prefix       = _current_rule->cbegin();
      if (!_current_rule->empty()) {
        ++_prefix;
      }
      next();
    }

    explicit Subwords(Presentation<Word>&& p) : Subwords() {
      _presentation = std::move(p);
      _current_rule = _presentation.rules.cbegin();
      _suffix       = _current_rule->cbegin();
      _prefix       = _current_rule->cbegin();
      if (!_current_rule->empty()) {
        ++_prefix;
      }
      next();
    }

    ////////////////////////////////////////////////////////////////////////
    // rx::ranges stuff
    ////////////////////////////////////////////////////////////////////////

    // TODO
    // template <typename InputRange>
    // struct Range;

    // TODO
    // template <typename InputRange,
    //           typename =
    //           std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    // [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
    //   using Inner = rx::get_range_type_t<InputRange>;
    //   return Range<Inner>(std::forward<InputRange>(input), *this);
    // }

    [[nodiscard]] output_type get() const {
      return _current;
    }

    void next() {
      while (_current_rule != _presentation.rules.cend()) {
        for (; _suffix < _current_rule->cend(); ++_suffix) {
          for (; _prefix <= _current_rule->cend(); ++_prefix) {
            if (_seen.emplace(_suffix, _prefix).second) {
              _current.assign(_suffix, _prefix);
              return;
            }
          }
        }
        ++_current_rule;
        if (_current_rule == _presentation.rules.cend()) {
          break;
        }
        _suffix = _current_rule->cbegin();
        _prefix = _current_rule->cbegin();
        if (!_current_rule->empty()) {
          ++_prefix;
        }
      }
    }

    [[nodiscard]] bool at_end() const noexcept {
      return _current_rule == _presentation.rules.cend();
    }

    [[nodiscard]] size_t size_hint() const {
      size_t const n = std::accumulate(
          _presentation.rules.begin(),
          _presentation.rules.end(),
          size_t(0),
          [](auto& acc, auto& rule) { return acc + rule.size(); });
      return n * (n + 1) / 2;
    }
    // TODO try_get_and_advance
  };  // class RulesSubwords

  template <typename Word>
  class TietzeAddGeneratorsRange {
    size_t             _count;
    Presentation<Word> _current;
    std::vector<Word>  _current_subwords_replaced_with_new_generators;
    size_t             _depth_max;
    size_t             _depth_min;
    // TODO mutable std::mutex                         _mtx;
    Presentation<Word>            _presentation;
    std::queue<std::vector<Word>> _todo;
    bool                          _todo_populated;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////
    static constexpr bool is_finite     = true;
    static constexpr bool is_idempotent = true;

    using output_type = Presentation<Word> const&;

    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////
    // TODO? init functions?
    TietzeAddGeneratorsRange()
        : _count(0),
          _current(),
          _current_subwords_replaced_with_new_generators(),
          _depth_max(0),
          _depth_min(0),
          _presentation(),
          _todo(),
          _todo_populated(false) {}

    TietzeAddGeneratorsRange(TietzeAddGeneratorsRange const&) = default;
    TietzeAddGeneratorsRange(TietzeAddGeneratorsRange&&)      = default;
    TietzeAddGeneratorsRange& operator=(TietzeAddGeneratorsRange const&)
        = default;
    TietzeAddGeneratorsRange& operator=(TietzeAddGeneratorsRange&&) = default;

    ~TietzeAddGeneratorsRange() = default;

    explicit TietzeAddGeneratorsRange(Presentation<Word> const& p)
        : TietzeAddGeneratorsRange() {
      _current      = p;
      _presentation = p;
    }

    explicit TietzeAddGeneratorsRange(Presentation<Word>&& p)
        : TietzeAddGeneratorsRange() {
      _current      = std::move(p);
      _presentation = _current;
    }

    ////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////

    //! \brief Get the maximum search depth.
    //!
    //! Returns the maximum number of subword replacements by new generators
    //! to perform when constructing presentations for the search.
    //!
    //! The default value is `3`.
    //!
    //! \returns A maximum number of new generators introduced.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t depth_max() const noexcept {
      return _depth_max;
    }

    //! \brief Set the maximum search depth.
    //!
    //! This function sets the maximum number of subword replacements by new
    //! generators to perform when constructing presentations for the search.
    //!
    //! The default value is `3`.
    //!
    //! \param val the maximum search depth.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if the internal search queue has
    //! already been populated.
    TietzeAddGeneratorsRange& depth_max(size_t val) {
      // TODO don't do this, just throw away _todo and recompute it
      throw_if_todo_populated("depth_max");
      _depth_max = val;
      return *this;
    }

    //! \brief Get the minimum search depth.
    //!
    //! Returns the minimum number of subword replacements by new generators
    //! required for a presentation to be tried.
    //!
    //! The default value is `0`.
    //!
    //! \returns The minimum number of new generators to introduce.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t depth_min() const noexcept {
      return _depth_min;
    }

    //! \brief Set the minimum search depth.
    //!
    //! This function sets the minimum number of subword replacements by new
    //! generators required for a presentation to be tried.
    //!
    //! The default value is `0`.
    //!
    //! \param val the minimum search depth.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if the internal search queue has
    //! already been populated.
    TietzeAddGeneratorsRange& depth_min(size_t val) {
      // TODO don't do this, just throw away _todo and recompute it
      throw_if_todo_populated("depth_min");
      _depth_min = val;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // rx::ranges stuff
    ////////////////////////////////////////////////////////////////////////

    // TODO
    // template <typename InputRange>
    // struct Range;

    // TODO
    // template <typename InputRange,
    //           typename =
    //           std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    // [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
    //   using Inner = rx::get_range_type_t<InputRange>;
    //   return Range<Inner>(std::forward<InputRange>(input), *this);
    // }

    [[nodiscard]] output_type get() const {
      return _current;
    }

    void next() {
      populate_todo();
      LIBSEMIGROUPS_ASSERT(!_todo.empty());
      // Put the presentation back into its original form
      _current      = _presentation;
      auto new_gens = std::move(_todo.front());
      _todo.pop();
      for (auto const& new_gen : new_gens) {
        presentation::replace_word_with_new_generator(_current, new_gen);
      }
    }

    [[nodiscard]] bool at_end() const {
      return _todo_populated && _todo.empty();
    }

    [[nodiscard]] size_t count() {
      populate_todo();
      return _count;
    }

    [[nodiscard]] size_t size_hint() const {
      // TODO improve
      return _count;
    }

    // TODO try_get_and_advance
   private:
    template <typename Iterator>
    auto subwords(Iterator first, Iterator last) {
      // TODO static_assert that Iterator::value_type is Word
      std::unordered_set<Word> mp;

      for (auto it = first; it < last; ++it) {
        auto const& w = *it;
        for (auto suffix = w.cbegin(); suffix < w.cend(); ++suffix) {
          for (auto prefix = suffix + 2; prefix < w.cend(); ++prefix) {
            mp.emplace(suffix, prefix);
          }
        }
      }
      std::vector<Word> words(mp.cbegin(), mp.cend());
      std::sort(words.begin(), words.end(), [](auto const& u, auto const& v) {
        return lenlex_cmp(v, u);
      });
      fmt::print("{}", words);
      return words;
    }

    void dfs(Presentation<Word>& p, size_t depth = 0) {
      if (depth >= _depth_min) {
        _count += 1;
      }
      if (depth != _depth_max) {
        auto sbwrds = subwords(p.rules.cbegin(), p.rules.cend());

        Presentation<Word> copy;
        for (auto const& w : sbwrds) {
          if (w.size() > 1) {
            copy = p;
            presentation::replace_word_with_new_generator(
                copy, w.cbegin(), w.cend());
            _current_subwords_replaced_with_new_generators.push_back(w);
            _todo.push(_current_subwords_replaced_with_new_generators);
            dfs(copy, depth + 1);
            _current_subwords_replaced_with_new_generators.pop_back();
          }
        }
      }
    }

    void populate_todo() {
      if (!_todo_populated) {
        LIBSEMIGROUPS_ASSERT(_todo.empty());
        _todo_populated = true;
        if (_depth_min == 0) {
          _count++;
        }
        auto copy = _presentation;
        dfs(copy);
      }
    }

    // TODO rm
    void throw_if_todo_populated(std::string_view msg) const {
      if (_todo_populated) {
        LIBSEMIGROUPS_EXCEPTION("it is not possible to set `{}` at this point, "
                                "please use `init`, and try again",
                                msg);
      }
    }
  };

  template <typename Word>
  TietzeAddGeneratorsRange(Presentation<Word> const&)
      -> TietzeAddGeneratorsRange<Word>;

  template <typename Word>
  TietzeAddGeneratorsRange(Presentation<Word>&&)
      -> TietzeAddGeneratorsRange<Word>;

  // template <typename Word>
  // template <typename InputRange>
  // struct TietzeAddGeneratorsRange<Word>::Range {
  //   using output_type = Presentation<Word>;

  //  static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
  //  static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

  //  InputRange                     _input;
  //  TietzeAddGeneratorsRange<Word> _tietze_add_generators;

  //  explicit Range(InputRange const&               input,
  //                 TietzeAddGeneratorsRange const& tagr)
  //      : _input(input), _tietze_add_generators(tagr) {}

  //  explicit Range(InputRange&& input, TietzeAddGeneratorsRange const& tagr)
  //      : _input(std::move(input)), _tietze_add_generators(tagr) {}

  //  explicit Range(InputRange const& input, TietzeAddGeneratorsRange&& tagr)
  //      : _input(input), _tietze_add_generators(std::move(tagr)) {}

  //  explicit Range(InputRange&& input, TietzeAddGeneratorsRange&& tagr)
  //      : _input(std::move(input)), _tietze_add_generators(std::move(tagr))
  //      {}

  //  [[nodiscard]] output_type get() const {
  //    auto copy = _tietze_add_generators._presentation;
  //    replace_word_with_new_generator(copy, _input.get());
  //    return copy;
  //  }

  //  constexpr void next() noexcept {
  //    _input.next();
  //  }

  //  [[nodiscard]] constexpr bool at_end() const noexcept {
  //    return _input.at_end();
  //  }

  //  [[nodiscard]] constexpr size_t size_hint() const noexcept {
  //    return _input.size_hint();
  //  }

  //  [[nodiscard]] constexpr size_t count() const noexcept {
  //    return _input.count();
  //  }
  //};
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TIETZE_HPP_
