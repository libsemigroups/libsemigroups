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

#include "constants.h"
#include "semigroup-base.h"

namespace libsemigroups {
  CongIntf::CongIntf(congruence_type type)
      : Runner(),
        _delete_quotient(false),
        _is_nr_generators_defined(false),
        _nrgens(UNDEFINED),
        _quotient(nullptr),
        _type(type) {}

  CongIntf::congruence_type CongIntf::type() const noexcept {
    return _type;
  }

  size_t CongIntf::nr_generators() const noexcept {
    return _nrgens;
  }

  void CongIntf::reset_quotient() {
    if (_delete_quotient) {
      delete _quotient;
    }
    _delete_quotient = true;
    _quotient        = nullptr;
  }

  void CongIntf::set_quotient(SemigroupBase* quotient) {
    reset_quotient();
    _delete_quotient = true;
    _quotient        = quotient;
  }

  void CongIntf::set_nr_generators(size_t n) {
    if (_is_nr_generators_defined) {
      throw LibsemigroupsException(
          "CongIntf::set_nr_generators: the number of geneators "
          "cannot be set more than once");
    }
    _is_nr_generators_defined = true;
    _nrgens                   = n;
  }

  SemigroupBase* CongIntf::get_quotient() const {
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
}  // namespace libsemigroups
