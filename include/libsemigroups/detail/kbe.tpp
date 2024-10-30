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

// This file contains the implementation of the KBE class.

namespace libsemigroups {
  namespace detail {

    template <typename KnuthBendix_>
    KBE<KnuthBendix_>::KBE(internal_string_type const& w) : _kb_word(w) {}

    template <typename KnuthBendix_>
    KBE<KnuthBendix_>::KBE(internal_string_type&& w) : _kb_word(std::move(w)) {}

    template <typename KnuthBendix_>
    KBE<KnuthBendix_>::KBE(KnuthBendix_& kb, internal_string_type const& w)
        : KBE(w) {
      kb._rewriter.rewrite(_kb_word);
    }

    template <typename KnuthBendix_>
    KBE<KnuthBendix_>::KBE(KnuthBendix_& kb, internal_string_type&& w)
        : KBE(std::move(w)) {
      kb._rewriter.rewrite(_kb_word);
    }

    template <typename KnuthBendix_>
    KBE<KnuthBendix_>::KBE(KnuthBendix_& kb, word_type const& w)
        : KBE(kb,
              std::accumulate(w.cbegin(),
                              w.cend(),
                              std::string(),
                              [](std::string& acc, letter_type a) {
                                acc += KnuthBendix_::uint_to_internal_string(a);
                                return acc;
                              })) {}

    template <typename KnuthBendix_>
    KBE<KnuthBendix_>::KBE(KnuthBendix_& kb, letter_type const& a)
        : KBE(kb, KnuthBendix_::uint_to_internal_string(a)) {}

    template <typename KnuthBendix_>
    bool KBE<KnuthBendix_>::operator==(KBE const& that) const {
      return that._kb_word == this->_kb_word;
    }

    template <typename KnuthBendix_>
    bool KBE<KnuthBendix_>::operator<(KBE const& that) const {
      return shortlex_compare(_kb_word, that._kb_word);
    }

    template <typename KnuthBendix_>
    void KBE<KnuthBendix_>::swap(KBE& x) {
      std::swap(x._kb_word, _kb_word);
    }

    template <typename KnuthBendix_>
    typename KBE<KnuthBendix_>::internal_string_type const&
    KBE<KnuthBendix_>::string() const noexcept {
      return _kb_word;
    }

    template <typename KnuthBendix_>
    word_type KBE<KnuthBendix_>::word(KnuthBendix_ const& kb) const {
      return kb.internal_string_to_word(_kb_word);
    }

    template <typename KnuthBendix_>
    std::string KBE<KnuthBendix_>::string(KnuthBendix_ const& kb) const {
      std::string out(_kb_word);
      kb.internal_to_external_string(out);  // changes out in-place
      return out;
    }

  }  // namespace detail

  // TODO(0) uncomment and/or move to cpp file, and implement for other types of
  // KnuthBendix also
  // template <> word_type
  // FroidurePin<detail::KBE<KnuthBendix<>>>::factorisation(
  //     detail::KBE<KnuthBendix<>> const& x) {
  //   return x.word(*state());
  // }

  // template <>
  // tril FroidurePin<detail::KBE<KnuthBendix<>>>::is_finite() const {
  //   // Turns out that the FroidurePin can be finished without the state
  //   // being finished, this sounds like a bug, I'm not 100% sure how that
  //   // works. Probably the state of the FroidurePin and the underlying
  //   // KnuthBendix_ are not the same object, and one is being used when the
  //   // other one should be, or something.
  //   if (state()->finished()
  //       && state()->number_of_classes() != POSITIVE_INFINITY) {
  //     return tril::TRUE;
  //   }
  //   if (is_obviously_infinite(*state())) {
  //     return tril::FALSE;
  //   }
  //   return tril::unknown;
  // }
}  // namespace libsemigroups
