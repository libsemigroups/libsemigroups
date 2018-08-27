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

#include "internal/containers.hpp"               // for RecVec
#include "internal/libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "internal/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "internal/stl.hpp"                      // for to_string

#include "constants.hpp"          // for UNDEFINED
#include "froidure-pin-base.hpp"  // for FroidurePinBase

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
        _is_obviously_finite_known(false),
        _is_obviously_finite(false),
        _is_obviously_infinite_known(false),
        _is_obviously_infinite(false),
        _quotient(),
        _non_trivial_classes() {
    reset();
  }

  CongBase::~CongBase() {}

  ////////////////////////////////////////////////////////////////////////////
  // CongBase - non-pure virtual methods - public
  ////////////////////////////////////////////////////////////////////////////

  bool CongBase::contains(word_type const& w1, word_type const& w2) {
    return w1 == w2 || word_to_class_index(w1) == word_to_class_index(w2);
  }

  CongBase::result_type CongBase::const_contains(word_type const& u,
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
        throw LIBSEMIGROUPS_EXCEPTION("cannot change the number of generators");
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
      throw LIBSEMIGROUPS_EXCEPTION("the parent semigroup is not defined");
    }
    return *_parent;
  }

  bool CongBase::has_quotient_semigroup() const noexcept {
    return _quotient != nullptr;
  }

  FroidurePinBase& CongBase::quotient_semigroup() {
    if (type() != congruence_type::TWOSIDED) {
      throw LIBSEMIGROUPS_EXCEPTION("the congruence must be two-sided");
    } else if (is_quotient_obviously_infinite()) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "cannot find the quotient semigroup, it is infinite");
    } else if (_quotient == nullptr) {
      _quotient = quotient_impl();
    }
    return *_quotient;
  }

  congruence_type CongBase::type() const noexcept {
    return _type;
  }

  // FIXME(now) the following method does not work when used with an
  // FpSemigroup (in particular, when it is infinite), because the relations of
  // the FpSemigroup are not in _gen_pairs (which is correct), and there is no
  // parent semigroup, and so the method below returns false positives.

  bool CongBase::is_quotient_obviously_infinite() {
    if (_is_obviously_infinite_known) {
      return _is_obviously_infinite;
    }
    REPORT("checking if the quotient is obviously infinite . . .");
    if (nr_generators() == UNDEFINED || (has_parent_semigroup())
        || (has_quotient_semigroup() && quotient_semigroup().finished())) {
      // In the case that has_ parent_semigroup() it is not possible to
      // determine whether or not the quotient is infinite, since the parent
      // semigroup can be finite or infinite (it is undecidable in general
      // whether or not it is infinite), and the quotient is not defined by the
      // generating pairs, so analysing the generating pairs (as below) does not
      // yield the correct answer.
      REPORT("the quotient is not obviously infinite (generators undefined, or "
             "obviously finite)");
      _is_obviously_infinite_known = true;
      _is_obviously_infinite       = false;
      return false;
    } else if (is_quotient_obviously_infinite_impl()) {
      // The derived class of CongBase knows the quotient is infinite, or more
      // relations than generators
      REPORT("the quotient is obviously infinite (infinite parent)");
      _is_obviously_finite_known   = true;
      _is_obviously_finite         = false;
      _is_obviously_infinite_known = true;
      _is_obviously_infinite       = true;
      return true;
    }
    size_t n = nr_generators();
    // If i = 0, ..., n - 1, then seen[i] is set to false if generator i either
    // does not occur in any relation, or in every relation there are the same
    // number of generators i on both sides of the relation.
    //
    // If i = n, ..., 2n - 1, then seen[i] is set to false if there is no
    // relation where one side consists solely of generator i - n.
    std::vector<bool> res(2 * n, false);

    std::unordered_map<letter_type, int64_t> map;
    bool                                     skip = false;

    for (auto const& pair : _gen_pairs) {
      map.clear();
      if (!skip && (pair.first.empty() || pair.second.empty())) {
        skip = true;
        std::fill(res.begin() + n, res.end(), true);
      }
      for (auto x : pair.first) {
        auto it = map.find(x);
        if (it == map.end()) {
          map.emplace(x, 1);
        } else {
          it->second++;
        }
      }
      if (map.size() == 1) {
        res[map.begin()->first + n] = true;
      }
      for (auto x : pair.second) {
        auto it = map.find(x);
        if (it == map.end()) {
          map.emplace(x, -1);
        } else {
          it->second--;
        }
      }
      if (!skip && !pair.second.empty()
          && std::all_of(pair.second.cbegin() + 1,
                         pair.second.cend(),
                         [&pair](letter_type i) -> bool {
                           return i == pair.second[0];
                         })) {
        res[pair.second[0] + n] = true;
      }
      for (auto const& x : map) {
        if (x.second != 0) {
          res[x.first] = true;
        }
      }
    }
    auto it = std::find_if_not(
            res.cbegin(), res.cend(), [](bool val) -> bool { return val; });
    if (it != res.cend()) {
      auto gen = static_cast<size_t>(it - res.cbegin());
      gen = (gen > n ? gen - n : gen);
      REPORT("the quotient is obviously infinite (generator ",
             gen,
             " has infinite order)");
      _is_obviously_finite_known   = true;
      _is_obviously_finite         = false;
      _is_obviously_infinite_known = true;
      _is_obviously_infinite       = true;
      return true;
    } else {
      REPORT("the quotient is not obviously infinite");
      _is_obviously_infinite_known = true;
      _is_obviously_infinite       = false;
      return false;
    }
  }

  bool CongBase::is_quotient_obviously_finite() {
    if (is_quotient_obviously_finite_impl()
        || (has_quotient_semigroup() && quotient_semigroup().finished())
        || (has_parent_semigroup() && parent_semigroup().finished())) {
      _is_obviously_finite_known   = true;
      _is_obviously_finite         = true;
      _is_obviously_infinite_known = true;
      _is_obviously_infinite       = false;
      return true;
    } else {
      _is_obviously_finite_known = true;
      _is_obviously_finite       = false;
      return false;
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // CongBase - non-virtual methods - protected
  /////////////////////////////////////////////////////////////////////////

  void CongBase::set_parent_semigroup(FroidurePinBase* prnt) {
    LIBSEMIGROUPS_ASSERT(prnt != nullptr || dead());
    if (prnt == _parent) {
      return;
    }
    LIBSEMIGROUPS_ASSERT(_parent == nullptr || dead());
    LIBSEMIGROUPS_ASSERT(prnt->nr_generators() == nr_generators()
                         || nr_generators() == UNDEFINED || dead());
    _parent = prnt;
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
      throw LIBSEMIGROUPS_EXCEPTION("there's no parent semigroup in which to "
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
    _init_ntc_done               = false;
    _quotient                    = nullptr;
    _is_obviously_finite_known   = false;
    _is_obviously_finite         = false;
    _is_obviously_infinite_known = false;
    _is_obviously_infinite       = false;
  }

  /////////////////////////////////////////////////////////////////////////
  // CongBase - non-virtual methods - protected
  /////////////////////////////////////////////////////////////////////////

  bool CongBase::validate_letter(letter_type c) const {
    if (nr_generators() == UNDEFINED) {
      throw LIBSEMIGROUPS_EXCEPTION("no generators have been defined");
    }
    return c < _nr_gens;
  }

  void CongBase::validate_word(word_type const& w) const {
    for (auto l : w) {
      // validate_letter throws if no generators are defined
      if (!validate_letter(l)) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "invalid letter " + to_string(l) + " in word " + to_string(w)
            + ", the valid range is [0, " + to_string(_nr_gens) + ")");
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
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // CongBase - static data members - private
  /////////////////////////////////////////////////////////////////////////

  const std::string CongBase::STRING_TWOSIDED = "two-sided";
  const std::string CongBase::STRING_LEFT     = "left";
  const std::string CongBase::STRING_RIGHT    = "right";

}  // namespace libsemigroups
