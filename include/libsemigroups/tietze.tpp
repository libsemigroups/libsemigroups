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

#include "libsemigroups/tietze.hpp"
namespace libsemigroups {

  namespace detail {
    ////////////////////////////////////////////////////////////////////////
    // SubwordsRange
    ////////////////////////////////////////////////////////////////////////

    template <typename InputRange>
    SubwordsRange<InputRange>::SubwordsRange(InputRange&&    input,
                                             Subwords const& subwords)
        : Settings(subwords),
          _current(),
          _current_rule(),
          _input(std::move(input)),
          _input_orig(_input),
          _prefix_end(),
          _seen(),
          _suffix_begin() {
      init_from_input();
    }

    template <typename InputRange>
    SubwordsRange<InputRange>::SubwordsRange(InputRange const& input,
                                             Subwords const&   subwords)
        : Settings(subwords),
          _current(),
          _current_rule(),
          _input(input),
          _input_orig(_input),
          _prefix_end(),
          _seen(),
          _suffix_begin() {
      init_from_input();
    }

    template <typename InputRange>
    SubwordsRange<InputRange>&
    SubwordsRange<InputRange>::max_length(size_t val) {
      Settings::max_length(val);
      _input = _input_orig;
      init_from_input();
      return *this;
    }

    template <typename InputRange>
    SubwordsRange<InputRange>&
    SubwordsRange<InputRange>::min_length(size_t val) {
      Settings::min_length(val);
      _input = _input_orig;
      init_from_input();
      return *this;
    }

    template <typename InputRange>
    SubwordsRange<InputRange>& SubwordsRange<InputRange>::proper(bool val) {
      Settings::proper(val);
      _input = _input_orig;
      init_from_input();
      return *this;
    }

    template <typename InputRange>
    void SubwordsRange<InputRange>::next() {
      while (_current_rule != _current.first.rules.size()) {
        auto const& rule = _current.first.rules[_current_rule];
        while (_suffix_begin < rule.size()) {
          size_t prefix_last = rule.size();
          if (_suffix_begin == 0) {
            prefix_last -= proper();
          }
          while (_prefix_end - _suffix_begin <= max_length()
                 && _prefix_end <= prefix_last) {
            auto first = rule.begin() + _suffix_begin;
            auto last  = rule.begin() + _prefix_end;
            _current.second.assign(first, last);
            auto [it, inserted] = _seen.emplace(_current.second, 1);
            if (inserted) {
              if (_prefix_end != rule.size()) {
                ++_prefix_end;
              } else {
                advance_prefix_suffix();
              }
              return;
            } else {
              ++(*it).second;
            }
            ++_prefix_end;
          }
          advance_prefix_suffix();
        }

        ++_current_rule;
        init_prefix_suffix();
      }
      if (!_input.at_end()) {
        _input.next();
        init_from_input();
      }
    }

    template <typename InputRange>
    size_t SubwordsRange<InputRange>::frequency(Word const& w) const {
      if (!at_end()) {
        LIBSEMIGROUPS_EXCEPTION(
            "The frequency of a word can only be found when the entire input "
            "range has been consumed, i.e. when at_end() returns true");
      }
      auto it = _seen.find(w);
      if (it == _seen.end()) {
        return 0;
      } else {
        return it->second;
      }
    }

    template <typename InputRange>
    void SubwordsRange<InputRange>::init_from_input() {
      if (!_input.at_end()) {
        _current.first = _input.get();
        _current_rule  = 0;
        _seen.clear();
        init_prefix_suffix();
        next();
      }
    }

    template <typename InputRange>
    void SubwordsRange<InputRange>::advance_prefix() {
      LIBSEMIGROUPS_ASSERT(_current_rule < _current.first.rules.size());
      size_t const n = _current.first.rules[_current_rule].size();
      if (_prefix_end + min_length() <= n) {
        _prefix_end += min_length();
      } else {
        _prefix_end = n + 1;
      }
    }

    template <typename InputRange>
    void SubwordsRange<InputRange>::init_prefix_suffix() {
      _suffix_begin = 0;
      _prefix_end   = 0;
      if (_current_rule < _current.first.rules.size()) {
        advance_prefix();
      }
    }

    template <typename InputRange>
    void SubwordsRange<InputRange>::advance_prefix_suffix() {
      LIBSEMIGROUPS_ASSERT(_current_rule < _current.first.rules.size());
      ++_suffix_begin;
      _prefix_end = _suffix_begin;
      advance_prefix();
    }

    ////////////////////////////////////////////////////////////////////////
    // SubwordsFreqRange
    ////////////////////////////////////////////////////////////////////////

    template <typename InputRange, typename Score>
    SubwordsFreqRange<InputRange, Score>::SubwordsFreqRange(
        InputRange const&          input,
        SubwordsFreq<Score> const& settings,
        Score const&               score)
        : Settings(settings),
          _index(UNDEFINED),
          _input(input),
          _input_orig(_input),
          _output_for_current_input(),
          _score(score) {
      init_from_input();
    }

    template <typename InputRange, typename Score>
    SubwordsFreqRange<InputRange, Score>&
    SubwordsFreqRange<InputRange, Score>::min_length(size_t val) {
      Settings::min_length(val);
      _input = _input_orig;
      init_from_input();
      return *this;
    }

    template <typename InputRange, typename Score>
    SubwordsFreqRange<InputRange, Score>&
    SubwordsFreqRange<InputRange, Score>::max_length(size_t val) {
      Settings::max_length(val);
      _input = _input_orig;
      init_from_input();
      return *this;
    }

    template <typename InputRange, typename Score>
    SubwordsFreqRange<InputRange, Score>&
    SubwordsFreqRange<InputRange, Score>::proper(bool val) {
      Settings::proper(val);
      _input = _input_orig;
      init_from_input();
      return *this;
    }

    template <typename InputRange, typename Score>
    void SubwordsFreqRange<InputRange, Score>::next() {
      ++_index;
      if (_index < _output_for_current_input.size()) {
        return;
      }
      _input.next();
      init_from_input();
    }

    template <typename InputRange, typename Score>
    void SubwordsFreqRange<InputRange, Score>::init_from_input() {
      if (!_input.at_end()) {
        _index = 0;
        _output_for_current_input.clear();
        // NOTE we pass *this to Subwords so that the settings are copied
        auto subwords = Subwords(*this)(_input.get());
        while (!subwords.at_end()) {
          auto& [p, w] = subwords.get();
          _output_for_current_input.emplace_back(p, w, 0);
          subwords.next();
        }
        // Now subwords is at_end(), so we can call frequency
        for (auto& [_, w, freq] : _output_for_current_input) {
          freq = subwords.frequency(w);
        }
        std::sort(_output_for_current_input.begin(),
                  _output_for_current_input.end(),
                  _score);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // TietzeAddGeneratorRange
    ////////////////////////////////////////////////////////////////////////

    template <typename InputRange>
    void TietzeAddGeneratorRange<InputRange>::next() {
      _input.next();
      if (!_input.at_end()) {
        _get_presentation = _input.get().first;
        presentation::replace_word_with_new_generator(_get_presentation,
                                                      _input.get().second);
      }
    }

    template <typename InputRange>
    void TietzeAddGeneratorRange<InputRange>::init_from_input() {
      if (!_input.at_end()) {
        auto const& value = _input.get();
        _get_presentation = value.first;
        presentation::replace_word_with_new_generator(_get_presentation,
                                                      value.second);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // TietzeAddRelationRange
    ////////////////////////////////////////////////////////////////////////

    template <typename InputRange>
    void TietzeAddRelationRange<InputRange>::init_from_input() {
      if (!_input.at_end()) {
        auto const& value = _input.get();
        _get_presentation = std::get<0>(value);
        presentation::add_rule(
            _get_presentation, std::get<1>(value), std::get<2>(value));
      }
    }

    template <typename InputRange>
    void TietzeAddRelationRange<InputRange>::next() {
      _input.next();
      if (!_input.at_end()) {
        auto const& value = _input.get();
        _get_presentation = std::get<0>(value);
        presentation::add_rule(
            _get_presentation, std::get<1>(value), std::get<2>(value));
      }
    }
  }  // namespace detail
}  // namespace libsemigroups
