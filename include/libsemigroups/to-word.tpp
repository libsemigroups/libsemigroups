//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2026 James Mitchell + Joseph Edwards
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

// This file contains implementations for classes related to converting to
// word-types and converting to strings.

namespace libsemigroups {
  namespace v4 {
    ////////////////////////////////////////////////////////////////////////
    // ToWord mem fns
    ////////////////////////////////////////////////////////////////////////

    template <typename From>
    ToWord<From>::ToWord(ToWord const&) = default;

    template <typename From>
    ToWord<From>::ToWord(ToWord&&) = default;

    template <typename From>
    ToWord<From>& ToWord<From>::operator=(ToWord const&) = default;

    template <typename From>
    ToWord<From>& ToWord<From>::operator=(ToWord&&) = default;

    template <typename From>
    ToWord<From>::~ToWord() = default;

    template <typename From>
    ToWord<From>& ToWord<From>::init(From const& alphabet) {
      if (alphabet.size() > 256) {
        // TODO replace 256 with numeric_limits::max - numeric_limits::min
        LIBSEMIGROUPS_EXCEPTION("The argument (alphabet) is too big, expected "
                                "at most 256, found {}",
                                alphabet.size());
      }
      auto _old_alphabet_map = _alphabet_map;
      init();
      LIBSEMIGROUPS_ASSERT(_alphabet_map.empty());
      for (letter_type l = 0; l < alphabet.size(); ++l) {
        auto it = _alphabet_map.emplace(alphabet[l], l);
        if (!it.second) {
          // Strong exception guarantee
          std::swap(_old_alphabet_map, _alphabet_map);

          // TODO(v4) Remove the libsemigroups prefix
          LIBSEMIGROUPS_EXCEPTION(
              "invalid alphabet {}, duplicate letter {}!",
              libsemigroups::detail::to_printable(alphabet),
              libsemigroups::detail::to_printable(alphabet[l]));
        }
      }
      return *this;
    }

    template <typename From>
    [[nodiscard]] From ToWord<From>::alphabet() const {
      if (empty()) {
        return From();
      }
      From output(_alphabet_map.size(), typename From::value_type());
      for (auto it : _alphabet_map) {
        output[it.second] = it.first;
      }
      return output;
    }

    template <typename From>
    void ToWord<From>::call_no_checks(word_type&  output,
                                      From const& input) const {
      // Empty alphabet implies conversion should use human_readable_index
      if (empty()) {
        // TODO remove this behaviour
        output.resize(input.size(), 0);
        std::transform(input.cbegin(),
                       input.cend(),
                       output.begin(),
                       [](char c) { return words::human_readable_index(c); });
      } else {  // Non-empty alphabet implies conversion should use the
                // alphabet.
        output.clear();
        output.reserve(input.size());
        for (auto const& c : input) {
          output.push_back(_alphabet_map.at(c));
        }
      }
    }

    template <typename From>
    void ToWord<From>::operator()(word_type& output, From const& input) const {
      if (!empty()) {
        for (auto const& c : input) {
          if (_alphabet_map.find(c) == _alphabet_map.cend()) {
            // TODO improve this like in presentation
            // TODO(v4) Remove the libsemigroups prefix
            LIBSEMIGROUPS_EXCEPTION(
                "invalid letter {} in the 2nd argument (input word), "
                "expected letters in the alphabet {}!",
                libsemigroups::detail::to_printable(c),
                libsemigroups::detail::to_printable(alphabet()));
          }
        }
      }
      call_no_checks(output, input);
    }
  }  // namespace v4
}  // namespace libsemigroups
