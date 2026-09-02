//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

// This file contains implementations of the iterator classes for words.

namespace libsemigroups {
  namespace detail {

    template <typename Cmp, typename>
    const_wio_iterator::const_wio_iterator(size_type   upper_bound,
                                           word_type&& first,
                                           word_type&& last,
                                           Cmp&&       cmp)
        : const_wio_iterator(
            Alphabet(cmp.alphabet()),
            upper_bound,
            std::move(first),
            std::move(last),
            std::function<bool(word_type const&, word_type const&)>(
                std::forward<Cmp>(cmp))) {}

    template <typename Cmp, typename>
    const_wio_iterator::const_wio_iterator(Alphabet<word_type>&& alphabet,
                                           size_type             upper_bound,
                                           word_type&&           first,
                                           word_type&&           last,
                                           Cmp&&                 cmp)
        : _current(),
          _index(),
          _upper_bound(upper_bound),
          _first(std::move(first)),
          _last(std::move(last)),
          _frontier(),
          _alphabet(std::move(alphabet)),
          _cmp(std::forward<Cmp>(cmp)) {
      _index = (_first == _last ? UNDEFINED : size_type(0));
      if (_first != _last) {
        _frontier.push_back(word_type());
      }
      operator++();
    }

  }  // namespace detail
}  // namespace libsemigroups
