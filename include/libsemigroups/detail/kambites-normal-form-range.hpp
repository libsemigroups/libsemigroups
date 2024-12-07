//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 James D. Mitchell
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

// This file contains an implementation of a range object for producing normal
// forms for a Kambites object.

#ifndef LIBSEMIGROUPS_DETAIL_KAMBITES_NORMAL_FORM_RANGE_HPP_
#define LIBSEMIGROUPS_DETAIL_KAMBITES_NORMAL_FORM_RANGE_HPP_

namespace libsemigroups {
  template <typename Word>
  class Kambites;

  namespace detail {
    template <typename Word>
    class KambitesNormalFormRange {
     private:
      mutable typename Kambites<Word>::native_word_type _current;
      FroidurePinBase::const_normal_form_iterator       _end;
      std::unique_ptr<FroidurePinBase>                  _fpb;
      FroidurePinBase::const_normal_form_iterator       _it;
      Kambites<Word>*                                   _k;

     public:
      using output_type = typename Kambites<Word>::native_word_type const&;

      KambitesNormalFormRange() = delete;

      explicit KambitesNormalFormRange(Kambites<Word>& k)
          : _current(), _end(), _fpb(), _it(), _k() {
        init(k);
      }

      KambitesNormalFormRange& init(Kambites<Word>& k) {
        _current.clear();
        _fpb = to_froidure_pin(k);
        _it  = _fpb->cbegin_current_normal_forms();
        _k   = &k;
        _end = _fpb->cend_current_normal_forms();
        return *this;
      }

      KambitesNormalFormRange(KambitesNormalFormRange const& that)
          : KambitesNormalFormRange(*that._k) {}

      KambitesNormalFormRange(KambitesNormalFormRange&& that) = default;

      KambitesNormalFormRange& operator=(KambitesNormalFormRange const& that) {
        return init(*that._k);
      }

      KambitesNormalFormRange& operator=(KambitesNormalFormRange&&) = default;

      // TODO(0) init?
      // TODO(1) allow setting of min/max etc like Paths

      output_type get() const {
        // TODO don't do this more than once per call
        auto const& w = *_it;
        _current.clear();
        for (auto c : w) {
          _current.push_back(_k->presentation().letter_no_checks(c));
        }
        return _current;
      }

      void next() {
        ++_it;
        if (_it == _end) {
          _fpb->enumerate(_fpb->current_size() + 1);
          _end = _fpb->cend_current_normal_forms();
        }
      }

      [[nodiscard]] bool at_end() const {
        return false;
      }

      [[nodiscard]] uint64_t size_hint() const {
        return POSITIVE_INFINITY;
      }

      [[nodiscard]] uint64_t count() const {
        return size_hint();
      }

      static constexpr bool is_finite     = false;
      static constexpr bool is_idempotent = true;
    };

    template <typename Word>
    KambitesNormalFormRange(Kambites<Word>&) -> KambitesNormalFormRange<Word>;

  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_KAMBITES_NORMAL_FORM_RANGE_HPP_
