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

// TODO clean this up

#include "cong-intf.h"

#include "internal/libsemigroups-exception.h"
#include "internal/stl.h"

#include "constants.h"
#include "semigroup-base.h"

namespace libsemigroups {
  CongIntf::CongIntf(congruence_type type)
      : Runner(),
        _delete_quotient(false),
        _is_nr_generators_defined(false),
        _nrgens(UNDEFINED),
        _parent(nullptr),
        _quotient(nullptr),
        _type(type) {}

  congruence_type CongIntf::type() const noexcept {
    return _type;
  }

  size_t CongIntf::nr_generators() const noexcept {
    return _nrgens;
  }

  void CongIntf::reset_quotient() {
    if (_delete_quotient) {
      delete _quotient;
    }
    _delete_quotient = false;
    _quotient        = nullptr;
  }

  void CongIntf::set_quotient(SemigroupBase* quotient) {
    LIBSEMIGROUPS_ASSERT(quotient != nullptr);
    LIBSEMIGROUPS_ASSERT(_quotient == nullptr);
    LIBSEMIGROUPS_ASSERT(_type == congruence_type::TWOSIDED);
    // FIXME _delete_quotient can be either true or false, depending on whether
    // quotient is coming from outside or inside.
    _delete_quotient = false;
    _quotient        = quotient;
  }

  void CongIntf::set_parent(SemigroupBase* prnt) {
    if (dead()) {
      return;  // TODO more like this
    }
    LIBSEMIGROUPS_ASSERT(prnt != nullptr);
    LIBSEMIGROUPS_ASSERT(_parent == nullptr);
    // TODO MORE asserts (check compatibility of parent and this)
    _parent = prnt;
    // TODO set quotient
  }

  bool CongIntf::has_parent() const noexcept {
    return _parent != nullptr;
  }

  SemigroupBase* CongIntf::parent() const noexcept {
    return _parent;
  }

  void CongIntf::set_nr_generators(size_t n) {
    if (_is_nr_generators_defined) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "the number of generators cannot be set more than once");
    }
    _is_nr_generators_defined = true;
    _nrgens                   = n;
  }

  bool CongIntf::has_quotient() const noexcept {
    return _quotient != nullptr;
  }

  SemigroupBase* CongIntf::quotient() const noexcept {
    return _quotient;
  }

  bool CongIntf::contains(word_type const& w1, word_type const& w2) {
    return w1 == w2 || word_to_class_index(w1) == word_to_class_index(w2);
  }

  bool CongIntf::const_contains(word_type const& u, word_type const& v) const {
    return (const_word_to_class_index(u) != UNDEFINED
            && const_word_to_class_index(u) == const_word_to_class_index(v));
  }

  bool CongIntf::less(word_type const& w1, word_type const& w2) {
    return word_to_class_index(w1) < word_to_class_index(w2);
  }

  void CongIntf::add_pair(std::initializer_list<size_t> l,
                          std::initializer_list<size_t> r) {
    add_pair(word_type(l), word_type(r));
  }

  bool CongIntf::is_quotient_obviously_finite() {
    return false;
  }

  bool CongIntf::is_quotient_obviously_infinite() {
    return false;
  }

  void CongIntf::init_non_trivial_classes() {
    // FIXME if not is proper quotient also!! If this is the case, then
    // we might end up enumerating an infinite semigroup in the loop below.
    if (!_non_trivial_classes.empty()) {
      // There are no non-trivial classes, or we already found them.
      // FIXME this doesn't cover the case when there are no non-triv classes
      return;
    } else if (_parent == nullptr) {
      throw LIBSEMIGROUPS_EXCEPTION("there's no parent semigroup in which to "
                                    "find the non-trivial classes");
    }

    LIBSEMIGROUPS_ASSERT(nr_classes() != POSITIVE_INFINITY);
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

  std::vector<std::vector<word_type>>::const_iterator CongIntf::cbegin_ntc() {
    init_non_trivial_classes();
    return _non_trivial_classes.cbegin();
  }

  std::vector<std::vector<word_type>>::const_iterator CongIntf::cend_ntc() {
    init_non_trivial_classes();
    return _non_trivial_classes.cend();
  }

  size_t CongIntf::nr_non_trivial_classes() {
    init_non_trivial_classes();
    return _non_trivial_classes.size();
  }

  CongIntf::class_index_type
  CongIntf::const_word_to_class_index(word_type const&) const {
    return UNDEFINED;
  }

}  // namespace libsemigroups
