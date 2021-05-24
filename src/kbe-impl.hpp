//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains the implementation of the KBE class in include/kbe.hpp
// and is included directly in knuth-bendix.cpp, because (for reasons of
// efficiency) it depends on some of the implementational details of the
// KnuthBendix class.

#ifndef LIBSEMIGROUPS_SRC_KBE_IMPL_HPP_
#define LIBSEMIGROUPS_SRC_KBE_IMPL_HPP_

namespace libsemigroups {
  namespace detail {

    KBE::KBE(internal_string_type const& w) : _kb_word(w) {}
    KBE::KBE(internal_string_type&& w) : _kb_word(std::move(w)) {}

    KBE::KBE(KnuthBendix& kb, internal_string_type const& w) : KBE(w) {
      kb._impl->internal_rewrite(&_kb_word);
    }

    KBE::KBE(KnuthBendix& kb, internal_string_type&& w) : KBE(std::move(w)) {
      kb._impl->internal_rewrite(&_kb_word);
    }

    KBE::KBE(KnuthBendix& kb, letter_type const& a)
        : KBE(kb, KnuthBendix::KnuthBendixImpl::uint_to_internal_string(a)) {}

    KBE::KBE(KnuthBendix& kb, word_type const& w)
        : KBE(kb, KnuthBendix::KnuthBendixImpl::word_to_internal_string(w)) {}

    bool KBE::operator==(KBE const& that) const {
      return that._kb_word == this->_kb_word;
    }

    bool KBE::operator<(KBE const& that) const {
      return shortlex_compare(_kb_word, that._kb_word);
    }

    void KBE::swap(KBE& x) {
      std::swap(x._kb_word, _kb_word);
    }

    KBE::internal_string_type const& KBE::string() const noexcept {
      return _kb_word;
    }

    word_type KBE::word(KnuthBendix const& kb) const {
      return kb._impl->internal_string_to_word(_kb_word);
    }

    std::string KBE::string(KnuthBendix const& kb) const {
      std::string out(_kb_word);
      kb._impl->internal_to_external_string(out);  // changes out in-place
      return out;
    }

  }  // namespace detail

  template <>
  word_type
  FroidurePin<detail::KBE,
              FroidurePinTraits<detail::KBE, fpsemigroup::KnuthBendix>>::
      factorisation(detail::KBE const& x) {
    return x.word(*state());
  }

  template <>
  tril FroidurePin<detail::KBE,
                   FroidurePinTraits<detail::KBE, fpsemigroup::KnuthBendix>>::
      is_finite() const {
    // Turns out that the FroidurePin can be finished without the state
    // being finished, this sounds like a bug, I'm not 100% sure how that
    // works. Probably the state of the FroidurePin and the underlying
    // KnuthBendix are not the same object, and one is being used when the
    // other one should be, or something.
    if (finished() || state()->is_obviously_finite()) {
      return tril::TRUE;
    }
    if (state()->is_obviously_infinite()) {
      return tril::FALSE;
    }
    return tril::unknown;
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_KBE_IMPL_HPP_
