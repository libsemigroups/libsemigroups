//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// This file contains an interface for f.p. semigroup like classes. These are
// implementations of non-pure methods that call pure methods only.

#include "fpsemi-intf.h"

#include "internal/libsemigroups-debug.h"
#include "internal/libsemigroups-exception.h"
#include "internal/stl.h"

#include "semigroup-base.h"

namespace libsemigroups {
  //////////////////////////////////////////////////////////////////////////////
  // FpSemiIntf - constructor + destructor - public
  //////////////////////////////////////////////////////////////////////////////

  FpSemiIntf::FpSemiIntf()
      : _alphabet(),
        _alphabet_map(),
        _delete_isomorphic_non_fp_semigroup(false),
        _is_alphabet_defined(false),
        _isomorphic_non_fp_semigroup(nullptr) {}

  FpSemiIntf::~FpSemiIntf() {
    if (_delete_isomorphic_non_fp_semigroup) {
      delete _isomorphic_non_fp_semigroup;
    }
  }

  ////////////////////////////////
  // Public non-virtual methods //
  ////////////////////////////////

  std::string const& FpSemiIntf::alphabet() const {
    if (!_is_alphabet_defined) {
      throw LibsemigroupsException(
          "FpSemiIntf::alphabet: no alphabet has been defined");
    }
    return _alphabet;
  }

  bool FpSemiIntf::has_isomorphic_non_fp_semigroup() const noexcept {
    return _isomorphic_non_fp_semigroup != nullptr;
  }

  //////////////////////////////////////
  // Non-pure syntactic sugar methods //
  //////////////////////////////////////

  void FpSemiIntf::add_rule(std::initializer_list<size_t> l,
                            std::initializer_list<size_t> r) {
    add_rule(word_type(l), word_type(r));
  }

  // TODO delete or implement these
  //  void FpSemiIntf::add_rule(relation_type rel) {
  //    add_rule(rel.first, rel.second);
  //  }

  void FpSemiIntf::add_rule(std::pair<std::string, std::string> rel) {
    add_rule(rel.first, rel.second);
  }

  void FpSemiIntf::add_rules(
      std::vector<std::pair<std::string, std::string>> const& rels) {
    for (auto const& rel : rels) {
      add_rule(rel);
    }
  }

  void FpSemiIntf::add_rules(SemigroupBase* S) {
    // The call to add_rule in the lambda below should validate the relations,
    // and that _alphabet has been defined already.
    if (is_alphabet_defined() && _alphabet.size() != S->nrgens()) {
      throw LibsemigroupsException(
          "FpSemiIntf::add_rules: incompatible number of generators, found "
          + libsemigroups::to_string(S->nrgens()) + ", should be at most "
          + libsemigroups::to_string(_alphabet.size()));
    }
    relations(S, [this](word_type lhs, word_type rhs) -> void {
      add_rule(word_to_string(lhs), word_to_string(rhs));
    });
  }

  // TODO delete or implement these
  //  bool FpSemiIntf::equal_to(std::initializer_list<size_t> const& u,
  //                            std::initializer_list<size_t> const& v) {
  //    return equal_to(word_type(u), word_type(v));
  //  }
  //
  //  word_type FpSemiIntf::normal_form(std::initializer_list<size_t> const& w)
  //  {
  //    return normal_form(word_type(w));
  //  }

  ////////////////////////////
  // Public virtual methods //
  ////////////////////////////

  void FpSemiIntf::add_rule(word_type const& lhs, word_type const& rhs) {
    if (!is_alphabet_defined()) {
      throw LibsemigroupsException("FpSemiIntf::add_rule: cannot add rules "
                                   "before an alphabet is defined");
    }
    validate_word(lhs);
    validate_word(rhs);
    add_rule(word_to_string(lhs), word_to_string(rhs));
  }

  bool FpSemiIntf::equal_to(word_type const& u, word_type const& v) {
    validate_word(u);
    validate_word(v);
    return equal_to(word_to_string(u), word_to_string(v));
  }

  word_type FpSemiIntf::normal_form(word_type const& w) {
    validate_word(w);
    return string_to_word(normal_form(word_to_string(w)));
  }

  void FpSemiIntf::set_alphabet(std::string const& lphbt) {
    if (_is_alphabet_defined) {
      throw LibsemigroupsException("FpSemiIntf::set_alphabet: the alphabet "
                                   "cannot be set more than once");
    } else if (lphbt.empty()) {
      throw LibsemigroupsException("FpSemiIntf::set_alphabet: the alphabet "
                                   "must be non-empty");
    }
    for (size_t i = 0; i < lphbt.size(); ++i) {
      if (_alphabet_map.find(lphbt[i]) != _alphabet_map.end()) {
        _alphabet_map.clear();  // Strong exception guarantee
        throw LibsemigroupsException(
            "FpSemiIntf::set_alphabet: duplicate letter "
            + libsemigroups::to_string(lphbt[i]) + " in alphabet");
      }
      _alphabet_map.emplace(lphbt[i], i);
    }
    _alphabet            = lphbt;
    _is_alphabet_defined = true;
  }

  void FpSemiIntf::set_alphabet(size_t nr_letters) {
    // TODO check that nr_letters isn't too big
    if (_is_alphabet_defined) {
      throw LibsemigroupsException("FpSemiIntf::set_alphabet: the alphabet "
                                   "cannot be set more than once");
    } else if (nr_letters == 0) {
      throw LibsemigroupsException("FpSemiIntf::set_alphabet: the alphabet "
                                   "must be non-empty");
    }
    for (size_t i = 0; i < nr_letters; ++i) {
#ifdef LIBSEMIGROUPS_DEBUG
      _alphabet += static_cast<char>(i + 97);
#else
      _alphabet += static_cast<char>(i + 1);
#endif
      _alphabet_map.emplace(_alphabet[i], i);
    }
    _is_alphabet_defined = true;
  }

  ////////////////////////////////
  // Protected methods and data //
  ////////////////////////////////

  char FpSemiIntf::uint_to_char(size_t a) const {
    LIBSEMIGROUPS_ASSERT(_is_alphabet_defined);
    LIBSEMIGROUPS_ASSERT(a < _alphabet.size());
    return _alphabet[a];
  }

  size_t FpSemiIntf::char_to_uint(char c) const {
    LIBSEMIGROUPS_ASSERT(_alphabet_map.find(c) != _alphabet_map.end());
    return (*_alphabet_map.find(c)).second;
  }

  word_type FpSemiIntf::string_to_word(std::string const& s) const {
    word_type w;
    w.reserve(s.size());
    for (char const& c : s) {
      w.push_back(char_to_uint(c));
    }
    return w;
  }

  std::string FpSemiIntf::word_to_string(word_type const& w) const {
    std::string s;
    s.reserve(w.size());
    for (letter_type const& l : w) {
      s.push_back(uint_to_char(l));
    }
    return s;
  }

  bool FpSemiIntf::is_alphabet_defined() const noexcept {
    return _is_alphabet_defined;
  }

  SemigroupBase* FpSemiIntf::get_isomorphic_non_fp_semigroup() const noexcept {
    return _isomorphic_non_fp_semigroup;
  }

  void FpSemiIntf::reset_isomorphic_non_fp_semigroup() {
    if (_delete_isomorphic_non_fp_semigroup) {
      delete _isomorphic_non_fp_semigroup;
    }
    _delete_isomorphic_non_fp_semigroup = false;
    _isomorphic_non_fp_semigroup        = nullptr;
  }

  void FpSemiIntf::set_isomorphic_non_fp_semigroup(
      SemigroupBase* isomorphic_non_fp_semigroup) {
    LIBSEMIGROUPS_ASSERT(isomorphic_non_fp_semigroup != nullptr);
    LIBSEMIGROUPS_ASSERT(_isomorphic_non_fp_semigroup == nullptr);
    // FIXME _delete_isomorphic_non_fp_semigroup can be either true or false,
    // depending on whether quotient is coming from outside or inside.
    _delete_isomorphic_non_fp_semigroup = false;
    _isomorphic_non_fp_semigroup        = isomorphic_non_fp_semigroup;
  }

  bool FpSemiIntf::validate_letter(char c) const {
    if (!_is_alphabet_defined) {
      throw LibsemigroupsException(
          "FpSemiIntf::validate_letter: no alphabet has been defined");
    }
    return (_alphabet_map.find(c) != _alphabet_map.end());
  }

  void FpSemiIntf::validate_word(std::string const& w) const {
    for (auto l : w) {
      if (!validate_letter(l)) {
        throw LibsemigroupsException(
            "FpSemiIntf::validate_word 1: invalid letter "
            + libsemigroups::to_string(l) + " in word " + w);
      }
    }
  }

  void FpSemiIntf::validate_word(word_type const& w) const {
    for (auto l : w) {
      if (!validate_letter(uint_to_char(l))) {
        throw LibsemigroupsException(
            "FpSemiIntf::validate_word 2: invalid letter "
            + libsemigroups::to_string(l) + " in word "
            + libsemigroups::to_string(w));
      }
    }
  }

  void FpSemiIntf::validate_relation(std::string const& l,
                                     std::string const& r) const {
    validate_word(l);
    validate_word(r);
  }

  void FpSemiIntf::validate_relation(
      std::pair<std::string, std::string> const& p) const {
    validate_relation(p.first, p.second);
  }

  void FpSemiIntf::validate_relation(word_type const& l,
                                     word_type const& r) const {
    validate_word(l);
    validate_word(r);
  }

  void FpSemiIntf::validate_relation(relation_type const& r) const {
    validate_relation(r.first, r.second);
  }

}  // namespace libsemigroups
