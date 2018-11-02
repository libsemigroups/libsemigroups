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

#include "cong-base.hpp"

#include <algorithm>  // for iota

#include "constants.hpp"                // for UNDEFINED
#include "froidure-pin-base.hpp"        // for FroidurePinBase
#include "libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "stl.hpp"                      // for internal::to_string

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////////
  // CongBase - constructors + destructor - public
  ////////////////////////////////////////////////////////////////////////////

  CongBase::CongBase(congruence_type type)
      : Runner(),
        // Non-mutable
        _gen_pairs(),
        _nr_gens(UNDEFINED),
        _parent(nullptr),
        _type(type),
        // Mutable
        _init_ntc_done(),
        _is_obviously_finite(false),
        _is_obviously_infinite(false),
        _quotient(nullptr),
        _non_trivial_classes() {
    reset();
  }

  CongBase::~CongBase() {
    _quotient.free_from(this);
  }

  ////////////////////////////////////////////////////////////////////////////
  // CongBase - non-pure virtual methods - public
  ////////////////////////////////////////////////////////////////////////////

  bool CongBase::contains(word_type const& w1, word_type const& w2) {
    return w1 == w2 || word_to_class_index(w1) == word_to_class_index(w2);
  }

  result_type CongBase::const_contains(word_type const& u,
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

  bool CongBase::less(word_type const& w1, word_type const& w2) {
    return word_to_class_index(w1) < word_to_class_index(w2);
  }

  bool CongBase::is_quotient_obviously_finite_impl() {
    return false;
  }

  bool CongBase::is_quotient_obviously_infinite_impl() {
    return false;
  }

  void CongBase::set_nr_generators(size_t n) {
    if (nr_generators() != UNDEFINED) {
      if (nr_generators() != n) {
        LIBSEMIGROUPS_EXCEPTION("cannot change the number of generators");
      } else {
        return;  // do nothing
      }
    }
    _nr_gens = n;
    set_nr_generators_impl(n);
    reset();
  }

  /////////////////////////////////////////////////////////////////////////
  // CongBase - non-virtual methods - public
  /////////////////////////////////////////////////////////////////////////

  void CongBase::add_pair(std::initializer_list<size_t> l,
                          std::initializer_list<size_t> r) {
    add_pair(word_type(l), word_type(r));
  }

  void CongBase::add_pair(word_type const& u, word_type const& v) {
    validate_word(u);
    validate_word(v);
    if (u == v) {
      return;
    } else if (has_parent_semigroup() && parent_semigroup().equal_to(u, v)) {
      return;
    }
    // Note that _gen_pairs might contain pairs of distinct words that
    // represent the same element of the parent semigroup (if any).
    _gen_pairs.emplace_back(u, v);
    add_pair_impl(u, v);
    reset();
  }

  CongBase::const_iterator CongBase::cbegin_generating_pairs() const {
    return _gen_pairs.cbegin();
  }

  CongBase::const_iterator CongBase::cend_generating_pairs() const {
    return _gen_pairs.cend();
  }

  std::vector<std::vector<word_type>>::const_iterator CongBase::cbegin_ntc() {
    init_non_trivial_classes();
    return _non_trivial_classes->cbegin();
  }

  std::vector<std::vector<word_type>>::const_iterator CongBase::cend_ntc() {
    init_non_trivial_classes();
    return _non_trivial_classes->cend();
  }

  size_t CongBase::nr_generators() const noexcept {
    return _nr_gens;
  }

  size_t CongBase::nr_generating_pairs() const noexcept {
    return _gen_pairs.size();
  }

  size_t CongBase::nr_non_trivial_classes() {
    init_non_trivial_classes();
    return _non_trivial_classes->size();
  }

  bool CongBase::has_parent_semigroup() const noexcept {
    return _parent != nullptr;
  }

  FroidurePinBase& CongBase::parent_semigroup() const {
    if (!has_parent_semigroup()) {
      LIBSEMIGROUPS_EXCEPTION("the parent semigroup is not defined");
    }
    return *_parent;
  }

  bool CongBase::has_quotient_semigroup() const noexcept {
    return _quotient != nullptr;
  }

  FroidurePinBase& CongBase::quotient_semigroup() {
    if (_quotient != nullptr) {
      LIBSEMIGROUPS_ASSERT(type() == congruence_type::TWOSIDED);
      return *_quotient;
    } else if (type() != congruence_type::TWOSIDED) {
      LIBSEMIGROUPS_EXCEPTION("the congruence must be two-sided");
    } else if (is_quotient_obviously_infinite()) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot find the quotient semigroup, it is infinite");
    }
    _quotient = quotient_impl();
    return *_quotient;
  }

  congruence_type CongBase::type() const noexcept {
    return _type;
  }

  bool CongBase::is_quotient_obviously_infinite() {
    REPORT("checking if the quotient is obviously infinite . . .");
    // If has_parent_semigroup(), then that is either finite (and so this is
    // not obviously infinite), or infinite, which is undecidable in general,
    // so we leave the answer to this question to
    // is_quotient_obviously_infinite_impl in the derived class.
    if (nr_generators() == UNDEFINED) {
      // If nr_generators() is undefined, then there is no quotient yet,
      // and so it is not obviously infinite, or anything!
      REPORT("not obviously infinite (no generators yet defined)");
      return false;
    } else if (has_quotient_semigroup() && quotient_semigroup().finished()) {
      // If the quotient FroidurePin is fully enumerated, it must be
      // finite, and hence this is not (obviously) infinite.
      REPORT("not obviously infinite (finite)");
      return false;
    } else if (is_quotient_obviously_infinite_impl()) {
      // The derived class of CongBase knows the quotient is infinite
      return true;
    }
    return false;
  }

  bool CongBase::is_quotient_obviously_finite() {
    if ((has_quotient_semigroup() && quotient_semigroup().finished())
        || (has_parent_semigroup() && parent_semigroup().finished())
        || is_quotient_obviously_finite_impl()) {
      return true;
    }
    return false;
  }

  /////////////////////////////////////////////////////////////////////////
  // CongBase - non-virtual methods - protected
  /////////////////////////////////////////////////////////////////////////

  void CongBase::set_parent_semigroup(FroidurePinBase& prnt) {
    if (&prnt == _parent) {
      return;
    }
    LIBSEMIGROUPS_ASSERT(_parent == nullptr || dead());
    LIBSEMIGROUPS_ASSERT(prnt.nr_generators() == nr_generators()
                         || nr_generators() == UNDEFINED || dead());
    _parent = &prnt;
    reset();
  }

  /////////////////////////////////////////////////////////////////////////
  // CongBase - non-pure virtual methods - private
  /////////////////////////////////////////////////////////////////////////

  CongBase::class_index_type
  CongBase::const_word_to_class_index(word_type const&) const {
    return UNDEFINED;
  }

  void CongBase::set_nr_generators_impl(size_t) {
    // do nothing
  }

  std::shared_ptr<CongBase::non_trivial_classes_type>
  CongBase::non_trivial_classes_impl() {
    if (_parent == nullptr) {
      // This means this was constructed from an fp semigroup that did not have
      // an isomorphic FroidurePin instance computed at the time. Since we
      // don't currently store the fp semigroup, we cannot ask for the
      // isomorphic FroidurePin now.
      //
      // TODO(later) store the FpSemigroup used to create this and here try
      // to compute the isomorphic FroidurePin.
      LIBSEMIGROUPS_EXCEPTION("there's no parent semigroup in which to "
                              "find the non-trivial classes");
    }
    auto ntc = non_trivial_classes_type(nr_classes(), std::vector<word_type>());

    word_type w;
    for (size_t pos = 0; pos < _parent->size(); ++pos) {
      _parent->factorisation(w, pos);
      ntc[word_to_class_index(w)].push_back(w);
      LIBSEMIGROUPS_ASSERT(word_to_class_index(w) < ntc.size());
    }
    ntc.erase(std::remove_if(ntc.begin(),
                             ntc.end(),
                             [](std::vector<word_type> const& klass) -> bool {
                               return klass.size() <= 1;
                             }),
              ntc.end());
    return std::make_shared<non_trivial_classes_type>(ntc);
  }

  /////////////////////////////////////////////////////////////////////////
  // CongBase - non-virtual methods - private
  /////////////////////////////////////////////////////////////////////////

  void CongBase::init_non_trivial_classes() {
    if (!_init_ntc_done) {
      _non_trivial_classes = non_trivial_classes_impl();
      _init_ntc_done       = true;
    }
  }

  void CongBase::reset() {
    set_finished(false);
    _non_trivial_classes.reset();
    _init_ntc_done = false;
    _quotient.free_from(this);
    _is_obviously_finite   = false;
    _is_obviously_infinite = false;
  }

  /////////////////////////////////////////////////////////////////////////
  // CongBase - non-virtual methods - protected
  /////////////////////////////////////////////////////////////////////////

  bool CongBase::validate_letter(letter_type c) const {
    if (nr_generators() == UNDEFINED) {
      LIBSEMIGROUPS_EXCEPTION("no generators have been defined");
    }
    return c < _nr_gens;
  }

  void CongBase::validate_word(word_type const& w) const {
    for (auto l : w) {
      // validate_letter throws if no generators are defined
      if (!validate_letter(l)) {
        LIBSEMIGROUPS_EXCEPTION("invalid letter " + internal::to_string(l)
                                + " in word " + internal::to_string(w)
                                + ", the valid range is [0, "
                                + internal::to_string(_nr_gens) + ")");
      }
    }
  }

  void CongBase::validate_relation(word_type const& l,
                                   word_type const& r) const {
    validate_word(l);
    validate_word(r);
  }

  void CongBase::validate_relation(relation_type const& rel) const {
    validate_relation(rel.first, rel.second);
  }

  /////////////////////////////////////////////////////////////////////////
  // CongBase - non-virtual static methods - protected
  /////////////////////////////////////////////////////////////////////////

  std::string const& CongBase::congruence_type_to_string(congruence_type typ) {
    switch (typ) {
      case congruence_type::TWOSIDED:
        return STRING_TWOSIDED;
      case congruence_type::LEFT:
        return STRING_LEFT;
      case congruence_type::RIGHT:
        return STRING_RIGHT;
      default:
        LIBSEMIGROUPS_EXCEPTION("incorrect type");
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // CongBase - static data members - private
  /////////////////////////////////////////////////////////////////////////

  const std::string CongBase::STRING_TWOSIDED = "two-sided";
  const std::string CongBase::STRING_LEFT     = "left";
  const std::string CongBase::STRING_RIGHT    = "right";

}  // namespace libsemigroups
