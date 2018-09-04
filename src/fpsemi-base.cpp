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

#include <string>  // for operator+, basic_string

#include "internal/libsemigroups-config.hpp"     // for LIBSEMIGROUPS_DEBUG
#include "internal/libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "internal/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "internal/stl.hpp"                      // for to_string

#include "froidure-pin-base.hpp"  // for FroidurePinBase

namespace libsemigroups {

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - constructor + destructor - public
  //////////////////////////////////////////////////////////////////////////////

  FpSemiBase::FpSemiBase()
      : Runner(),
        // Non-mutable
        _alphabet(),
        _alphabet_map(),
        _identity(),
        _inverses(),
        _rules(),
        // Mutable
        _identity_defined(false),
        _isomorphic_non_fp_semigroup(nullptr),
        _is_obviously_finite_known(false),
        _is_obviously_finite(false),
        _is_obviously_infinite_known(false),
        _is_obviously_infinite(false) {}

  FpSemiBase::~FpSemiBase() {
    _isomorphic_non_fp_semigroup.free_from(this);
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - non-pure virtual methods - public
  //////////////////////////////////////////////////////////////////////////////

  bool FpSemiBase::equal_to(word_type const& u, word_type const& v) {
    validate_word(u);
    validate_word(v);
    return equal_to(word_to_string(u), word_to_string(v));
  }

  word_type FpSemiBase::normal_form(word_type const& w) {
    validate_word(w);
    return string_to_word(normal_form(word_to_string(w)));
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - non-virtual methods - public
  //////////////////////////////////////////////////////////////////////////////

  void FpSemiBase::set_alphabet(std::string const& lphbt) {
    if (!_alphabet.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "the alphabet cannot be set more than once");
    } else if (lphbt.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION("the alphabet must be non-empty");
    }
    for (size_t i = 0; i < lphbt.size(); ++i) {
      if (_alphabet_map.find(lphbt[i]) != _alphabet_map.end()) {
        _alphabet_map.clear();  // Strong exception guarantee
        throw LIBSEMIGROUPS_EXCEPTION(
            "invalid alphabet, it contains the duplicate letter "
            + to_string(lphbt[i]));
      }
      _alphabet_map.emplace(lphbt[i], i);
    }
    _alphabet = lphbt;
    set_alphabet_impl(lphbt);
    reset();
  }

  void FpSemiBase::set_alphabet(size_t const nr_letters) {
    if (!_alphabet.empty()) {
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
    set_alphabet_impl(nr_letters);
    reset();
  }

  std::string const& FpSemiBase::alphabet() const noexcept {
    return _alphabet;
  }

  void FpSemiBase::add_rule(std::string const& u, std::string const& v) {
    validate_word(u);
    validate_word(v);
    if (u == v
        || (has_isomorphic_non_fp_semigroup()
            && isomorphic_non_fp_semigroup().equal_to(string_to_word(u),
                                                      string_to_word(v)))) {
      return;
    }
    _rules.emplace_back(u, v);
    add_rule_impl(u, v);
    reset();
  }

  void FpSemiBase::add_rule(word_type const& u, word_type const& v) {
    validate_word(u);
    validate_word(v);
    if (u == v
        || (has_isomorphic_non_fp_semigroup()
            && isomorphic_non_fp_semigroup().equal_to(u, v))) {
      return;
    }
    _rules.emplace_back(word_to_string(u), word_to_string(v));
    add_rule_impl(u, v);
    reset();
  }

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

  void FpSemiBase::add_rules(FroidurePinBase& S) {
    if (!_alphabet.empty() && _alphabet.size() != S.nr_generators()) {
      throw LIBSEMIGROUPS_EXCEPTION("incompatible number of generators, found "
                                    + to_string(S.nr_generators())
                                    + ", should be at most "
                                    + to_string(_alphabet.size()));
    }
    add_rules_impl(S);
    reset();
  }

  void FpSemiBase::add_rules(
      std::vector<std::pair<std::string, std::string>> const& rels) {
    for (auto const& rel : rels) {
      add_rule(rel);
    }
  }

  size_t FpSemiBase::nr_rules() const noexcept {
    return _rules.size();
  }

  bool FpSemiBase::has_isomorphic_non_fp_semigroup() const noexcept {
    return _isomorphic_non_fp_semigroup != nullptr;
  }

  FroidurePinBase& FpSemiBase::isomorphic_non_fp_semigroup() {
    if (_isomorphic_non_fp_semigroup == nullptr) {
      _isomorphic_non_fp_semigroup = isomorphic_non_fp_semigroup_impl();
    }
    return *_isomorphic_non_fp_semigroup;
  }

  word_type FpSemiBase::normal_form(std::initializer_list<letter_type> w) {
    return normal_form(word_type(w));
  }

  bool FpSemiBase::equal_to(std::initializer_list<letter_type> u,
                            std::initializer_list<letter_type> v) {
    return equal_to(word_type(u), word_type(v));
  }

  // Note that this can be called repeatedly, and that's fine.
  void FpSemiBase::set_identity(std::string const& id) {
    if (validate_identity_impl(id) && id.length() == 1) {
      _identity = id[0];
      for (auto l : alphabet()) {
        if (l == id[0]) {
          add_rule(id + id, id);
        } else {
          add_rule(to_string(l) + id, to_string(l));
          add_rule(id + to_string(l), to_string(l));
        }
      }
    }
    _identity_defined = true;
  }

  void FpSemiBase::set_identity(letter_type id) {
    validate_letter(id);
    set_identity(std::string(1, _alphabet[id]));
  }

  std::string const& FpSemiBase::identity() const {
    if (_identity_defined) {
      return _identity;
    } else {
      throw LIBSEMIGROUPS_EXCEPTION("no identity has been defined");
    }
  }

  void FpSemiBase::set_inverses(std::string const& inv) {
    if (_alphabet.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "no alphabet has been defined, define an alphabet first");
    } else if (!_identity_defined) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "no identity has been defined, define an identity first");
    } else if (_alphabet.size() != inv.size()) {
      throw LIBSEMIGROUPS_EXCEPTION("invalid inverses, expected "
                                    + to_string(_alphabet.size())
                                    + " but found " + to_string(inv.size()));
    }

    validate_word(inv);

    std::string cpy = inv;
    std::sort(cpy.begin(), cpy.end());
    for (auto it = cpy.cbegin(); it < cpy.cend() - 1; ++it) {
      if (*it == *(it + 1)) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "invalid inverses, it contains the duplicate letter "
            + to_string(*it));
      }
    }

    _inverses = inv;

    for (size_t i = 0; i < _alphabet.size(); ++i) {
      add_rule(std::string(1, _alphabet[i]) + _inverses[i], _identity);
      add_rule(std::string(1, _inverses[i]) + _alphabet[i], _identity);
    }
  }

  std::string const& FpSemiBase::inverses() const {
    if (!_inverses.empty()) {
      return _inverses;
    } else {
      throw LIBSEMIGROUPS_EXCEPTION("no inverses have been defined");
    }
  }

  word_type FpSemiBase::string_to_word(std::string const& s) const {
    word_type w;
    w.reserve(s.size());
    for (char const& c : s) {
      w.push_back(char_to_uint(c));
    }
    return w;
  }

  std::string FpSemiBase::word_to_string(word_type const& w) const {
    std::string s;
    s.reserve(w.size());
    for (letter_type const& l : w) {
      s.push_back(uint_to_char(l));
    }
    return s;
  }

  FpSemiBase::const_iterator FpSemiBase::cbegin_rules() const {
    return _rules.cbegin();
  }

  FpSemiBase::const_iterator FpSemiBase::cend_rules() const {
    return _rules.cend();
  }

  bool FpSemiBase::is_obviously_infinite() {
    if (_is_obviously_infinite_known) {
      return _is_obviously_infinite;
    }
    REPORT("checking if the quotient is obviously infinite . . .");
    // If has_parent_semigroup(), then that is either finite (and so this is
    // not obviously infinite), or infinite, which is undecidable in general,
    // so we leave the answer to this question to
    // is_quotient_obviously_infinite_impl in the derived class.
    if (alphabet().empty()) {
      // If nr_generators() is undefined, then there is no quotient yet,
      // and so it is not obviously infinite, or anything!
      REPORT("not obviously infinite (no alphabet defined)");
      set_is_obviously_infinite(false);
      return false;
    } else if (has_isomorphic_non_fp_semigroup()
               && isomorphic_non_fp_semigroup().finished()) {
      // If the isomorphic FroidurePin is fully enumerated, it must be
      // finite, and hence this is not (obviously) infinite.
      REPORT("not obviously infinite (finite)");
      set_is_obviously_finite(true);  // FINITE!
      return false;
    } else if (is_obviously_infinite_impl()) {
      // The derived class of FpSemiBase knows the quotient is infinite
      set_is_obviously_infinite(true);
      return true;
    }
    set_is_obviously_infinite(false);
    return false;
  }

  bool FpSemiBase::is_obviously_finite() {
    if (_is_obviously_finite_known) {
      return _is_obviously_finite;
    }
    if ((has_isomorphic_non_fp_semigroup()
         && isomorphic_non_fp_semigroup().finished())
        || is_obviously_finite_impl()) {
      set_is_obviously_finite(true);
      return true;
    }
    set_is_obviously_finite(false);
    return false;
  }

  std::string FpSemiBase::to_gap_string() {
    std::string out = "free := FreeMonoid(";
    for (auto x : alphabet()) {
      out += std::string("\"") + x + "\"";
      if (x != alphabet().back()) {
        out += ",";
      }
    }
    out += ");\n";
    out += "AssignGeneratorVariables(free);\n";
    out += "rules := [\n";
    for (auto it = cbegin_rules(); it < cend_rules(); ++it) {
      out += "          [";
      if (it->first.empty()) {
        out += "One(free)";
      } else {
        for (auto const& l : (*it).first) {
          out += l;
          if (l != (*it).first.back()) {
            out += " * ";
          }
        }
        out += ", ";
      }
      if (it->second.empty()) {
        out += "One(free)";
      } else {
        for (auto const& l : (*it).second) {
          out += l;
          if (l != (*it).second.back()) {
            out += " * ";
          }
        }
      }
      if (it != cend_rules() - 1) {
        out += "],\n";
      } else {
        out += "]\n";
      }
    }
    out += "         ];\n";
    out += "S := free / rules;\n";
    return out;
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - non-virtual methods - protected
  //////////////////////////////////////////////////////////////////////////////

  size_t FpSemiBase::char_to_uint(char c) const {
    LIBSEMIGROUPS_ASSERT(_alphabet_map.find(c) != _alphabet_map.end());
    return (*_alphabet_map.find(c)).second;
  }

  char FpSemiBase::uint_to_char(size_t a) const noexcept {
    LIBSEMIGROUPS_ASSERT(!_alphabet.empty());
    LIBSEMIGROUPS_ASSERT(a < _alphabet.size());
    return _alphabet[a];
  }

  bool FpSemiBase::validate_letter(char c) const {
    if (_alphabet.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION("no alphabet has been defined");
    }
    return (_alphabet_map.find(c) != _alphabet_map.end());
  }

  bool FpSemiBase::validate_letter(letter_type c) const {
    if (_alphabet.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION("no alphabet has been defined");
    }
    return c < _alphabet.size();
  }

  void FpSemiBase::validate_word(std::string const& w) const {
    for (auto l : w) {
      // validate_letter throws if no generators are defined
      if (!validate_letter(l)) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "invalid letter " + std::string(1, l) + " in word " + w
            + ", valid letters are \"" + _alphabet + "\"");
      }
    }
    // Use validate_word_impl to impose or lift any further restrictions on
    // valid words, for example, ToddCoxeter does not allow empty words (and so
    // neither does FpSemiBase), but KnuthBendix does.
    validate_word_impl(w);
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
    // Use validate_word_impl to impose or lift any further restrictions on
    // valid words, for example, ToddCoxeter does not allow empty words (and so
    // neither does FpSemiBase), but KnuthBendix does.
    validate_word_impl(w);
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

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - non-pure virtual methods - private
  //////////////////////////////////////////////////////////////////////////////

  void FpSemiBase::set_alphabet_impl(std::string const&) {
    // do nothing
  }

  void FpSemiBase::set_alphabet_impl(size_t) {
    // do nothing
  }

  void FpSemiBase::add_rule_impl(word_type const& u, word_type const& v) {
    add_rule_impl(word_to_string(u), word_to_string(v));
  }

  void FpSemiBase::add_rules_impl(FroidurePinBase& S) {
    relations(S, [this](word_type lhs, word_type rhs) -> void {
      validate_word(lhs);
      validate_word(rhs);
      add_rule(word_to_string(lhs), word_to_string(rhs));
    });
  }

  bool FpSemiBase::is_obviously_finite_impl() {
    return false;
  }

  bool FpSemiBase::is_obviously_infinite_impl() {
    return false;
  }

  void FpSemiBase::validate_word_impl(std::string const& w) const {
    if (w.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION("invalid word, found the empty word but "
                                    "words must be non-empty");
    }
  }

  void FpSemiBase::validate_word_impl(word_type const& w) const {
    if (w.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION("invalid word, found the empty word but "
                                    "words must be non-empty");
    }
  }

  bool FpSemiBase::validate_identity_impl(std::string const& id) const {
    if (id.length() != 1) {
      throw LIBSEMIGROUPS_EXCEPTION("invalid identity, found "
                                    + to_string(id.length())
                                    + " letters, should be single letter");
    }
    validate_letter(id[0]);
    return true;
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - non-virtual methods - private
  //////////////////////////////////////////////////////////////////////////////

  void FpSemiBase::reset() noexcept {
    set_finished(false);
    _isomorphic_non_fp_semigroup = nullptr;
    _is_obviously_finite_known   = false;
    _is_obviously_finite         = false;
    _is_obviously_infinite_known = false;
    _is_obviously_infinite       = false;
  }

  void FpSemiBase::set_is_obviously_finite(bool val) const {
    if (_is_obviously_finite_known && val != _is_obviously_finite) {
      throw LIBSEMIGROUPS_EXCEPTION("value is already set to the opposite");
    }
    _is_obviously_finite_known = true;
    _is_obviously_finite       = val;
    if (val) {
      set_is_obviously_infinite(false);
    }
  }

  void FpSemiBase::set_is_obviously_infinite(bool val) const {
    if (_is_obviously_infinite_known && val != _is_obviously_infinite) {
      throw LIBSEMIGROUPS_EXCEPTION("value is already set to the opposite");
    }
    _is_obviously_infinite_known = true;
    _is_obviously_infinite       = val;
    if (val) {
      set_is_obviously_finite(false);
    }
  }
}  // namespace libsemigroups
