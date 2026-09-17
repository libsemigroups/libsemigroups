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
    //////////////////////////////////////////////////////////////////
    // wio
    //////////////////////////////////////////////////////////////////

    template <typename Word>
    const_wio_iterator<Word>::const_wio_iterator() = default;

    template <typename Word>
    const_wio_iterator<Word>::const_wio_iterator(const_wio_iterator const&)
        = default;

    template <typename Word>
    const_wio_iterator<Word>::const_wio_iterator(const_wio_iterator&&)
        = default;

    template <typename Word>
    const_wio_iterator<Word>&
    const_wio_iterator<Word>::operator=(const_wio_iterator const&)
        = default;

    template <typename Word>
    const_wio_iterator<Word>&
    const_wio_iterator<Word>::operator=(const_wio_iterator&&)
        = default;

    template <typename Word>
    const_wio_iterator<Word>::~const_wio_iterator() = default;

    template <typename Word>
    template <typename Cmp, typename>
    const_wio_iterator<Word>::const_wio_iterator(
        typename const_wio_iterator<Word>::size_type upper_bound,
        Word const&                                  first,
        Word const&                                  last,
        Cmp&&                                        cmp)
        : _alphabet(cmp.alphabet()),
          _cmp(std::forward<Cmp>(cmp)),
          _current(),
          _first(),
          _frontier(),
          _index(),
          _last(),
          _tmp_word(),
          _upper_bound() {
      reset(upper_bound, first, last);
    }

    template <typename Word>
    void const_wio_iterator<Word>::reset(
        typename const_wio_iterator<Word>::size_type upper_bound,
        Word const&                                  first,
        Word const&                                  last) {
      _alphabet.throw_if_letter_not_in_alphabet(first.cbegin(), first.cend());
      _alphabet.throw_if_letter_not_in_alphabet(last.cbegin(), last.cend());

      _current.clear();
      _upper_bound = upper_bound - 1;
      _first       = first;
      _last        = last;
      _frontier.clear();
      _index = (!_cmp(_first, _last) ? UNDEFINED : size_type(0));
      if (_index != UNDEFINED) {
        _frontier.push_back(Word());
      }
      operator++();
    }

    template <typename Word>
    const_wio_iterator<Word> const& const_wio_iterator<Word>::operator++() {
      if (_index != UNDEFINED) {
        ++_index;

        // We need flipped_cmp so that we can maintain a min-heap, rather than a
        // max-heap
        auto const& flipped_cmp
            = [&cmp = this->_cmp](Word const& lhs, Word const& rhs) {
                return cmp(rhs, lhs);
              };

        // Get the next word
        do {
          if (_frontier.empty()) {
            _index = UNDEFINED;
            break;
          }
          LIBSEMIGROUPS_ASSERT(!_frontier.empty());
          std::pop_heap(_frontier.begin(), _frontier.end(), flipped_cmp);
          _current = std::move(_frontier.back());
          _frontier.pop_back();

          if (_current.size() < _upper_bound) {
            // Update the frontier with words of length at most _upper_bound
            for (auto const& a : _alphabet.letters()) {
              _tmp_word.assign(_current.cbegin(), _current.cend());
              _tmp_word.push_back(a);
              if (!_cmp(_last, _tmp_word)) {
                _frontier.push_back(std::move(_tmp_word));
                std::push_heap(_frontier.begin(), _frontier.end(), flipped_cmp);
              }
            }
          }
        } while (_cmp(_current, _first));
        LIBSEMIGROUPS_ASSERT(_current.size() <= _upper_bound);
        LIBSEMIGROUPS_ASSERT(!_cmp(_last, _current));
      }
      return *this;
    }

    template <typename Word>
    void const_wio_iterator<Word>::swap(const_wio_iterator& that) {
      std::swap(_alphabet, that._alphabet);
      std::swap(_cmp, that._cmp);
      std::swap(_first, that._first);
      std::swap(_frontier, that._frontier);
      std::swap(_index, that._index);
      std::swap(_last, that._last);
      // There is no point swapping _tmp_word because its value is almost always
      // nonsense except for a brief period during a call to operator++
      std::swap(_upper_bound, that._upper_bound);
      _current.swap(that._current);
    }

    //////////////////////////////////////////////////////////////////
    // wilo
    //////////////////////////////////////////////////////////////////

    template <typename Word>
    const_wilo_iterator<Word>::const_wilo_iterator() = default;

    template <typename Word>
    const_wilo_iterator<Word>::const_wilo_iterator(const_wilo_iterator const&)
        = default;

    template <typename Word>
    const_wilo_iterator<Word>::const_wilo_iterator(const_wilo_iterator&&)
        = default;

    template <typename Word>
    const_wilo_iterator<Word>&
    const_wilo_iterator<Word>::operator=(const_wilo_iterator const&)
        = default;

    template <typename Word>
    const_wilo_iterator<Word>&
    const_wilo_iterator<Word>::operator=(const_wilo_iterator&&)
        = default;

    template <typename Word>
    const_wilo_iterator<Word>::~const_wilo_iterator() = default;

    template <typename Word>
    const_wilo_iterator<Word>::const_wilo_iterator(
        Alphabet<Word> const&                         alphabet,
        typename const_wilo_iterator<Word>::size_type upper_bound,
        Word const&                                   first,
        Word const&                                   last)
        : const_wilo_iterator_impl(),
          _alphabet(alphabet),
          _external_current(),
          _external_current_set(false) {
      reset(upper_bound, first, last);
    }

    template <typename Word>
    void const_wilo_iterator<Word>::reset(
        typename const_wilo_iterator<Word>::size_type upper_bound,
        Word const&                                   first,
        Word const&                                   last) {
      _alphabet.throw_if_letter_not_in_alphabet(first.cbegin(), first.cend());
      _alphabet.throw_if_letter_not_in_alphabet(last.cbegin(), last.cend());
      const_wilo_iterator_impl::init(_alphabet.size(),
                                     upper_bound,
                                     word_type(citow(_alphabet, first.cbegin()),
                                               citow(_alphabet, first.cend())),
                                     word_type(citow(_alphabet, last.cbegin()),
                                               citow(_alphabet, last.cend())));
      _external_current.clear();
      _external_current_set = false;
    }

    template <typename Word>
    void const_wilo_iterator<Word>::update_external_current() const {
      _external_current.assign(cifrw(_alphabet, _current.cbegin()),
                               cifrw(_alphabet, _current.cend()));
      _external_current_set = true;
    }

    template <typename Word>
    void const_wilo_iterator<Word>::swap(const_wilo_iterator& that) {
      const_wilo_iterator_impl::swap(that);
      std::swap(_alphabet, that._alphabet);
      std::swap(_external_current_set, that._external_current_set);
      _external_current.swap(that._external_current);
    }

    //////////////////////////////////////////////////////////////////
    // wislo
    //////////////////////////////////////////////////////////////////

    template <typename Word>
    const_wislo_iterator<Word>::const_wislo_iterator() = default;

    template <typename Word>
    const_wislo_iterator<Word>::const_wislo_iterator(
        const_wislo_iterator const&)
        = default;

    template <typename Word>
    const_wislo_iterator<Word>::const_wislo_iterator(const_wislo_iterator&&)
        = default;

    template <typename Word>
    const_wislo_iterator<Word>&
    const_wislo_iterator<Word>::operator=(const_wislo_iterator const&)
        = default;

    template <typename Word>
    const_wislo_iterator<Word>&
    const_wislo_iterator<Word>::operator=(const_wislo_iterator&&)
        = default;

    template <typename Word>
    const_wislo_iterator<Word>::~const_wislo_iterator() = default;

    template <typename Word>
    const_wislo_iterator<Word>::const_wislo_iterator(
        Alphabet<Word> const& alphabet,
        Word const&           first,
        Word const&           last)
        : const_wislo_iterator_impl(),
          _alphabet(alphabet),
          _external_current(),
          _external_current_set(false) {
      reset(0, first, last);
    }

    template <typename Word>
    void const_wislo_iterator<Word>::reset(
        typename const_wislo_iterator<Word>::size_type upper_bound,
        Word const&                                    first,
        Word const&                                    last) {
      // The parameter <upper_bound> is necessary for interface consistency with
      // wio and wilo, but otherwise unused
      std::ignore = upper_bound;
      _alphabet.throw_if_letter_not_in_alphabet(first.cbegin(), first.cend());
      _alphabet.throw_if_letter_not_in_alphabet(last.cbegin(), last.cend());

      const_wislo_iterator_impl::init(
          _alphabet.size(),
          word_type(citow(_alphabet, first.cbegin()),
                    citow(_alphabet, first.cend())),
          word_type(citow(_alphabet, last.cbegin()),
                    citow(_alphabet, last.cend())));
      _external_current.clear();
      _external_current_set = false;
    }

    template <typename Word>
    void const_wislo_iterator<Word>::update_external_current() const {
      _external_current.assign(cifrw(_alphabet, _current.cbegin()),
                               cifrw(_alphabet, _current.cend()));
      _external_current_set = true;
    }

    template <typename Word>
    void const_wislo_iterator<Word>::swap(const_wislo_iterator& that) {
      const_wislo_iterator_impl::swap(that);
      std::swap(_alphabet, that._alphabet);
      std::swap(_external_current_set, that._external_current_set);
      _external_current.swap(that._external_current);
    }

  }  // namespace detail
}  // namespace libsemigroups
