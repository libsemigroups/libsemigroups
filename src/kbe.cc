//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#include "kbe.h"

#include <algorithm>
#include <string>

#include "internal/stl.h"

namespace libsemigroups {
  //////////////////////////////////////////////////////////////////////////////
  // KBE - constructors - private
  //////////////////////////////////////////////////////////////////////////////

  KBE::KBE(KnuthBendix* kb, internal_string_type w, bool reduce)
      : Element(), _kb(kb), _kb_word(w) {
    if (reduce) {
      _kb->rewrite(&_kb_word);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // KBE - constructors - public
  //////////////////////////////////////////////////////////////////////////////

  KBE::KBE() : Element(), _kb(nullptr), _kb_word() {}

  KBE::KBE(KnuthBendix* kb) : KBE(kb, "", false) {}

  KBE::KBE(KnuthBendix* kb, internal_string_type const& w) : KBE(kb, w, true) {}

  // FIXME the derefence *KnuthBendix::word_to_internal_string causes a mem
  // leak
  KBE::KBE(KnuthBendix* kb, letter_type const& a)
      : KBE(kb, *KnuthBendix::uint_to_internal_string(a)) {}

  KBE::KBE(KnuthBendix& kb, letter_type const& a) : KBE(&kb, a) {}

  // FIXME the derefence *KnuthBendix::word_to_internal_string causes a mem
  // leak
  KBE::KBE(KnuthBendix* kb, word_type const& w)
      : KBE(kb, *KnuthBendix::word_to_internal_string(w)) {}

  KBE::KBE(KnuthBendix& kb, word_type const& w) : KBE(&kb, w) {}

  KBE::KBE(KBE const& cpy)
      : Element(cpy._hash_value), _kb(cpy._kb), _kb_word(cpy._kb_word) {}

  //////////////////////////////////////////////////////////////////////////////
  // Element - n methods - public
  //////////////////////////////////////////////////////////////////////////////

  bool KBE::operator==(Element const& that) const {
    return static_cast<KBE const&>(that)._kb_word == this->_kb_word;
  }

  bool KBE::operator<(Element const& that) const {
    internal_string_type const& u = this->_kb_word;
    internal_string_type const& v = static_cast<KBE const&>(that)._kb_word;
    if (u != v && (u.size() < v.size() || (u.size() == v.size() && u < v))) {
      // TODO allow other reduction orders here
      return true;
    } else {
      return false;
    }
  }

  void KBE::swap(Element& x) {
    auto& xx = static_cast<KBE&>(x);
    _kb_word.swap(xx._kb_word);
    std::swap(_kb, xx._kb);
    std::swap(this->_hash_value, xx._hash_value);
  }

  size_t KBE::complexity() const {
    return LIMIT_MAX;
  }

  size_t KBE::degree() const {
    return 0;
  }

  KBE KBE::identity() const {
    return KBE(_kb);
  }

  KBE* KBE::heap_copy() const {
    return new KBE(*this);
  }

  KBE* KBE::heap_identity() const {
    return this->identity().heap_copy();
  }

  void KBE::cache_hash_value() const {
    this->_hash_value = libsemigroups::hash<internal_string_type>{}(_kb_word);
  }

  void KBE::redefine(Element const& x, Element const& y, size_t) {
    auto const& xx = static_cast<KBE const&>(x);
    auto const& yy = static_cast<KBE const&>(y);
    LIBSEMIGROUPS_ASSERT(xx._kb == yy._kb);
    _kb_word.clear();
    _kb_word.append(xx._kb_word);
    _kb_word.append(yy._kb_word);
    _kb->rewrite(&_kb_word);
    this->reset_hash_value();
  }

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  Element* KBE::empty_key() const {
    return new KBE(_kb, "supercalifragilisticexpialidocious");
  }
#endif

  KBE::operator word_type() const {
    // FIXME leaks
    return *_kb->internal_string_to_word(&_kb_word);
  }

  template <> word_type Semigroup<KBE>::factorisation(KBE const& x) {
    return x;
  }
}  // namespace libsemigroups
