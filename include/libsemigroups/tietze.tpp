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

  namespace detail {
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
  }  // namespace detail

}  // namespace libsemigroups
