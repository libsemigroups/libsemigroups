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

// This file contains a base class for f.p. semigroup like classes.

#include "fpsemi-base.hpp"

#include "internal/libsemigroups-debug.hpp"
#include "internal/libsemigroups-exception.hpp"
#include "internal/stl.hpp"

#include "froidure-pin-base.hpp"

namespace libsemigroups {

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - constructor + destructor - public
  //////////////////////////////////////////////////////////////////////////////

  FpSemiBase::FpSemiBase()
      : Runner(),
        _alphabet(),
        _alphabet_map(),
        _delete_isomorphic_non_fp_semigroup(false),
        _is_alphabet_defined(false),
        _isomorphic_non_fp_semigroup(nullptr) {}

  FpSemiBase::~FpSemiBase() {
    reset_isomorphic_non_fp_semigroup();
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - non-pure virtual methods - public
  //////////////////////////////////////////////////////////////////////////////

  void FpSemiBase::add_rule(word_type const& lhs, word_type const& rhs) {
    if (!is_alphabet_defined()) {
      throw LIBSEMIGROUPS_EXCEPTION("cannot add rules "
                                    "before an alphabet is defined");
    }
    validate_word(lhs);
    validate_word(rhs);
    add_rule(word_to_string(lhs), word_to_string(rhs));
  }

  void FpSemiBase::add_rules(
      std::vector<std::pair<std::string, std::string>> const& rels) {
    for (auto const& rel : rels) {
      add_rule(rel);
    }
  }

  bool FpSemiBase::equal_to(word_type const& u, word_type const& v) {
    validate_word(u);
    validate_word(v);
    return equal_to(word_to_string(u), word_to_string(v));
  }

  word_type FpSemiBase::normal_form(word_type const& w) {
    validate_word(w);
    return string_to_word(normal_form(word_to_string(w)));
  }

  void FpSemiBase::set_alphabet(std::string const& lphbt) {
    if (_is_alphabet_defined) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "the alphabet cannot be set more than once");
    } else if (lphbt.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION("the alphabet must be non-empty");
    }
    for (size_t i = 0; i < lphbt.size(); ++i) {
      if (_alphabet_map.find(lphbt[i]) != _alphabet_map.end()) {
        _alphabet_map.clear();  // Strong exception guarantee
        throw LIBSEMIGROUPS_EXCEPTION("duplicate letter " + to_string(lphbt[i])
                                      + " in alphabet");
      }
      _alphabet_map.emplace(lphbt[i], i);
    }
    _alphabet            = lphbt;
    _is_alphabet_defined = true;
  }

  void FpSemiBase::set_alphabet(size_t const nr_letters) {
    if (_is_alphabet_defined) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "the alphabet cannot be set more than once");
    } else if (nr_letters == 0) {
      throw LIBSEMIGROUPS_EXCEPTION("the alphabet must be non-empty");
    } else if (nr_letters > 256) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "the alphabet must contain at most 256 letters");
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

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - non-virtual methods - public
  //////////////////////////////////////////////////////////////////////////////

  void FpSemiBase::add_rule(std::initializer_list<size_t> l,
                            std::initializer_list<size_t> r) {
    add_rule(word_type(l), word_type(r));
  }

  void FpSemiBase::add_rule(relation_type rel) {
    add_rule(rel.first, rel.second);
  }

  void FpSemiBase::add_rule(std::pair<std::string, std::string> rel) {
    add_rule(rel.first, rel.second);
  }

  void FpSemiBase::add_rules(FroidurePinBase* S) {
    if (is_alphabet_defined() && _alphabet.size() != S->nr_generators()) {
      throw LIBSEMIGROUPS_EXCEPTION("incompatible number of generators, found "
                                    + to_string(S->nr_generators())
                                    + ", should be at most "
                                    + to_string(_alphabet.size()));
    }
    relations(S, [this](word_type lhs, word_type rhs) -> void {
      validate_word(lhs);
      validate_word(rhs);
      add_rule(word_to_string(lhs), word_to_string(rhs));
    });
  }

  std::string const& FpSemiBase::alphabet() const {
    if (!_is_alphabet_defined) {
      throw LIBSEMIGROUPS_EXCEPTION("no alphabet has been defined");
    }
    return _alphabet;
  }

  bool FpSemiBase::has_isomorphic_non_fp_semigroup() const noexcept {
    return _isomorphic_non_fp_semigroup != nullptr;
  }

  word_type FpSemiBase::normal_form(std::initializer_list<letter_type> w) {
    return normal_form(word_type(w));
  }

  bool FpSemiBase::equal_to(std::initializer_list<letter_type> u,
                            std::initializer_list<letter_type> v) {
    return equal_to(word_type(u), word_type(v));
  }

  void FpSemiBase::set_identity(std::string const& id) {
    if (id.length() != 1) {
      throw LIBSEMIGROUPS_EXCEPTION("invalid identity, found "
                                    + to_string(id.length())
                                    + " letters, should be single letter");
    }
    validate_letter(id[0]);
    for (auto l : alphabet()) {
      if (l == id[0]) {
        add_rule(id + id, id);
      } else {
        add_rule(to_string(l) + id, to_string(l));
        add_rule(id + to_string(l), to_string(l));
      }
    }
  }

  void FpSemiBase::set_identity(letter_type id) {
    validate_letter(id);
    set_identity(std::string(1, _alphabet[id]));
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - non-virtual methods - protected
  //////////////////////////////////////////////////////////////////////////////

  size_t FpSemiBase::char_to_uint(char c) const {
    LIBSEMIGROUPS_ASSERT(_alphabet_map.find(c) != _alphabet_map.end());
    return (*_alphabet_map.find(c)).second;
  }

  char FpSemiBase::uint_to_char(size_t a) const noexcept {
    LIBSEMIGROUPS_ASSERT(_is_alphabet_defined);
    LIBSEMIGROUPS_ASSERT(a < _alphabet.size());
    return _alphabet[a];
  }

  // TODO return rvalue reference
  word_type FpSemiBase::string_to_word(std::string const& s) const {
    word_type w;
    w.reserve(s.size());
    for (char const& c : s) {
      w.push_back(char_to_uint(c));
    }
    return w;
  }

  // TODO return rvalue reference
  std::string FpSemiBase::word_to_string(word_type const& w) const {
    std::string s;
    s.reserve(w.size());
    for (letter_type const& l : w) {
      s.push_back(uint_to_char(l));
    }
    return s;
  }

  FroidurePinBase* FpSemiBase::get_isomorphic_non_fp_semigroup() const
      noexcept {
    return _isomorphic_non_fp_semigroup;
  }

  void FpSemiBase::reset_isomorphic_non_fp_semigroup() noexcept {
    if (_delete_isomorphic_non_fp_semigroup) {
      delete _isomorphic_non_fp_semigroup;
    }
    _delete_isomorphic_non_fp_semigroup = false;
    _isomorphic_non_fp_semigroup        = nullptr;
  }

  void FpSemiBase::set_isomorphic_non_fp_semigroup(
      FroidurePinBase* ismrphc_nn_fp_smgrp,
      bool             delete_it) noexcept {
    LIBSEMIGROUPS_ASSERT(ismrphc_nn_fp_smgrp != nullptr);
    LIBSEMIGROUPS_ASSERT(_isomorphic_non_fp_semigroup == nullptr);
    // _delete_isomorphic_non_fp_semigroup can be either true or false,
    // depending on whether ismrphc_nn_fp_smgrp is coming from outside or
    // inside.
    _delete_isomorphic_non_fp_semigroup = delete_it;
    _isomorphic_non_fp_semigroup        = ismrphc_nn_fp_smgrp;
  }

  bool FpSemiBase::is_alphabet_defined() const noexcept {
    return _is_alphabet_defined;
  }

  bool FpSemiBase::validate_letter(char c) const {
    if (!_is_alphabet_defined) {
      throw LIBSEMIGROUPS_EXCEPTION("no alphabet has been defined");
    }
    return (_alphabet_map.find(c) != _alphabet_map.end());
  }

  bool FpSemiBase::validate_letter(letter_type c) const {
    if (!_is_alphabet_defined) {
      throw LIBSEMIGROUPS_EXCEPTION("no alphabet has been defined");
    }
    return c < _alphabet.size();
  }

  void FpSemiBase::validate_word(std::string const& w) const {
    for (auto l : w) {
      if (!validate_letter(l)) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "invalid letter " + to_string(l) + " in word " + w
            + ", valid letters are \"" + _alphabet + "\"");
      }
    }
  }

  void FpSemiBase::validate_word(word_type const& w) const {
    for (auto l : w) {
      // validate_letter throws if no generators are defined
      if (!validate_letter(l)) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "invalid letter " + to_string(l) + " in word " + to_string(w)
            + ", the valid range is [0, " + to_string(_alphabet.size()) + ")");
      }
    }
  }

  void FpSemiBase::validate_relation(std::string const& l,
                                     std::string const& r) const {
    validate_word(l);
    validate_word(r);
  }

  void FpSemiBase::validate_relation(
      std::pair<std::string, std::string> const& p) const {
    validate_relation(p.first, p.second);
  }

  void FpSemiBase::validate_relation(word_type const& l,
                                     word_type const& r) const {
    validate_word(l);
    validate_word(r);
  }

  void FpSemiBase::validate_relation(relation_type const& r) const {
    validate_relation(r.first, r.second);
  }

}  // namespace libsemigroups
