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

#include "cong-intf.h"

#include "internal/libsemigroups-exception.h"
#include "internal/stl.h"

#include "constants.h"
#include "semigroup-base.h"

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////////
  // CongIntf - constructors + destructor - public
  ////////////////////////////////////////////////////////////////////////////

  CongIntf::CongIntf(congruence_type type)
      : Runner(),
        // Protected
        _non_trivial_classes(),
        _nr_generating_pairs(0),
        // Private
        _delete_quotient(false),
        _init_ntc_done(false),
        _nrgens(UNDEFINED),
        _parent(nullptr),
        _quotient(nullptr),
        _type(type) {}

  CongIntf::~CongIntf() {
    reset_quotient();
  }

  ////////////////////////////////////////////////////////////////////////////
  // CongIntf - non-pure virtual methods - public
  ////////////////////////////////////////////////////////////////////////////

  bool CongIntf::contains(word_type const& w1, word_type const& w2) {
    return w1 == w2 || word_to_class_index(w1) == word_to_class_index(w2);
  }

  CongIntf::result_type CongIntf::const_contains(word_type const& u,
                                                 word_type const& v) const {
    if (const_word_to_class_index(u) == UNDEFINED
        || const_word_to_class_index(v) == UNDEFINED) {
      return result_type::UNKNOWN;
    } else if (const_word_to_class_index(u) == const_word_to_class_index(v)) {
      return result_type::TRUE;
    } else if (finished()) {
      return result_type::FALSE;
    } else {
      return result_type::UNKNOWN;
    }
  }

  bool CongIntf::less(word_type const& w1, word_type const& w2) {
    return word_to_class_index(w1) < word_to_class_index(w2);
  }

  bool CongIntf::is_quotient_obviously_finite() {
    return false;
  }

  bool CongIntf::is_quotient_obviously_infinite() {
    return false;
  }

  void CongIntf::set_nr_generators(size_t n) {
    if (nr_generators() != UNDEFINED) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "the number of generators cannot be set more than once");
    }
    _nrgens = n;
  }

  /////////////////////////////////////////////////////////////////////////
  // CongIntf - non-virtual methods - public
  /////////////////////////////////////////////////////////////////////////

  void CongIntf::add_pair(std::initializer_list<size_t> l,
                          std::initializer_list<size_t> r) {
    add_pair(word_type(l), word_type(r));
  }

  std::vector<std::vector<word_type>>::const_iterator CongIntf::cbegin_ntc() {
    init_non_trivial_classes();
    return _non_trivial_classes.cbegin();
  }

  std::vector<std::vector<word_type>>::const_iterator CongIntf::cend_ntc() {
    init_non_trivial_classes();
    return _non_trivial_classes.cend();
  }

  size_t CongIntf::nr_generators() const noexcept {
    return _nrgens;
  }

  size_t CongIntf::nr_generating_pairs() const noexcept {
    return _nr_generating_pairs;
  }

  size_t CongIntf::nr_non_trivial_classes() {
    init_non_trivial_classes();
    return _non_trivial_classes.size();
  }

  SemigroupBase* CongIntf::parent_semigroup() const {
    if (!has_parent()) {
      throw LIBSEMIGROUPS_EXCEPTION("the parent semigroup is not defined");
    }
    return get_parent();
  }

  congruence_type CongIntf::type() const noexcept {
    return _type;
  }

  /////////////////////////////////////////////////////////////////////////
  // CongIntf - non-virtual methods - protected
  /////////////////////////////////////////////////////////////////////////

  SemigroupBase* CongIntf::get_quotient() const noexcept {
    return _quotient;
  }

  bool CongIntf::has_quotient() const noexcept {
    return _quotient != nullptr;
  }

  void CongIntf::reset_quotient() {
    if (_delete_quotient) {
      delete _quotient;
    }
    _delete_quotient = false;
    _quotient        = nullptr;
  }

  void CongIntf::set_quotient(SemigroupBase* qtnt, bool delete_it) {
    LIBSEMIGROUPS_ASSERT(qtnt != nullptr);
    LIBSEMIGROUPS_ASSERT(_quotient == nullptr);
    LIBSEMIGROUPS_ASSERT(_type == congruence_type::TWOSIDED);
    // _delete_quotient can be either true or false, depending on whether qtnt
    // is coming from outside or inside.
    _delete_quotient = delete_it;
    _quotient        = qtnt;
  }

  SemigroupBase* CongIntf::get_parent() const noexcept {
    return _parent;
  }

  bool CongIntf::has_parent() const noexcept {
    return _parent != nullptr;
  }

  void CongIntf::set_parent(SemigroupBase* prnt) {
    LIBSEMIGROUPS_ASSERT(prnt != nullptr || dead());
    if (prnt == _parent) {
      return;
    }
    LIBSEMIGROUPS_ASSERT(_parent == nullptr || dead());
    LIBSEMIGROUPS_ASSERT(prnt->nrgens() == nr_generators()
                         || nr_generators() == UNDEFINED || dead());
    _parent = prnt;
    if (_nr_generating_pairs == 0) {
      _quotient        = prnt;
      _delete_quotient = false;
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // CongIntf - non-pure virtual methods - private
  /////////////////////////////////////////////////////////////////////////

  void CongIntf::init_non_trivial_classes() {
    if (_init_ntc_done) {
      // There are no non-trivial classes, or we already found them.
      return;
    } else if (_parent == nullptr) {
      throw LIBSEMIGROUPS_EXCEPTION("there's no parent semigroup in which to "
                                    "find the non-trivial classes");
    }
    LIBSEMIGROUPS_ASSERT(nr_classes() != POSITIVE_INFINITY);

    _init_ntc_done = true;
    _non_trivial_classes.assign(nr_classes(), std::vector<word_type>());

    word_type w;
    for (size_t pos = 0; pos < _parent->size(); ++pos) {
      _parent->factorisation(w, pos);
      _non_trivial_classes[word_to_class_index(w)].push_back(w);
      LIBSEMIGROUPS_ASSERT(word_to_class_index(w)
                           < _non_trivial_classes.size());
    }

    _non_trivial_classes.erase(
        std::remove_if(_non_trivial_classes.begin(),
                       _non_trivial_classes.end(),
                       [](std::vector<word_type> const& klass) -> bool {
                         return klass.size() <= 1;
                       }),
        _non_trivial_classes.end());
  }

  CongIntf::class_index_type
  CongIntf::const_word_to_class_index(word_type const&) const {
    return UNDEFINED;
  }

  /////////////////////////////////////////////////////////////////////////
  // CongIntf - non-virtual methods - protected
  /////////////////////////////////////////////////////////////////////////

  bool CongIntf::validate_letter(letter_type c) const {
    if (nr_generators() == UNDEFINED) {
      throw LIBSEMIGROUPS_EXCEPTION("no generators have been defined");
    }
    return c < _nrgens;
  }

  void CongIntf::validate_word(word_type const& w) const {
    for (auto l : w) {
      // validate_letter throws if no generators are defined
      if (!validate_letter(l)) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "invalid letter " + to_string(l) + " in word " + to_string(w)
            + ", the valid range is [0, " + to_string(_nrgens) + ")");
      }
    }
  }

  void CongIntf::validate_relation(word_type const& l,
                                   word_type const& r) const {
    validate_word(l);
    validate_word(r);
  }

  void CongIntf::validate_relation(relation_type const& rel) const {
    validate_relation(rel.first, rel.second);
  }

  /////////////////////////////////////////////////////////////////////////
  // CongIntf - non-virtual static methods - protected
  /////////////////////////////////////////////////////////////////////////

  std::string const& CongIntf::congruence_type_to_string(congruence_type typ) {
    switch (typ) {
      case congruence_type::TWOSIDED:
        return STRING_TWOSIDED;
      case congruence_type::LEFT:
        return STRING_LEFT;
      case congruence_type::RIGHT:
        return STRING_RIGHT;
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // CongIntf - static data members - private
  /////////////////////////////////////////////////////////////////////////

  const std::string CongIntf::STRING_TWOSIDED = "two-sided";
  const std::string CongIntf::STRING_LEFT     = "left";
  const std::string CongIntf::STRING_RIGHT    = "right";

}  // namespace libsemigroups
