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

// This file contains TODO

#ifndef LIBSEMIGROUPS_SRC_KNUTH_BENDIX_RULE_HPP_
#define LIBSEMIGROUPS_SRC_KNUTH_BENDIX_RULE_HPP_

class Rule {
 public:
  // Construct from KnuthBendix with new but empty internal_string_type's
  explicit Rule(KnuthBendixImpl const* kbimpl, int64_t id)
      : _kbimpl(kbimpl),
        _lhs(new internal_string_type()),
        _rhs(new internal_string_type()),
        _id(-1 * id) {
    LIBSEMIGROUPS_ASSERT(_id < 0);
  }

  // The Rule class does not support an assignment contructor to avoid
  // accidental copying.
  Rule& operator=(Rule const& copy) = delete;

  // The Rule class does not support a copy contructor to avoid
  // accidental copying.
  Rule(Rule const& copy) = delete;

  // Destructor, deletes pointers used to create the rule.
  ~Rule() {
    delete _lhs;
    delete _rhs;
  }

  // Returns the left hand side of the rule, which is guaranteed to be
  // greater than its right hand side according to the reduction ordering
  // of the KnuthBendix used to construct this.
  internal_string_type* lhs() const {
    return _lhs;
  }

  // Returns the right hand side of the rule, which is guaranteed to be
  // less than its left hand side according to the reduction ordering of
  // the KnuthBendix used to construct this.
  internal_string_type* rhs() const {
    return _rhs;
  }

  void rewrite() {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    _kbimpl->internal_rewrite(_lhs);
    _kbimpl->internal_rewrite(_rhs);
    // reorder if necessary
    if ((*(_kbimpl->_order))(_rhs, _lhs)) {
      std::swap(_lhs, _rhs);
    }
  }

  // TODO remove this method and do this in deactivate instead
  void clear() {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    _lhs->clear();
    _rhs->clear();
  }

  inline bool active() const {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    return (_id > 0);
  }

  void deactivate() {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    if (active()) {
      _id *= -1;
    }
  }

  void activate() {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    if (!active()) {
      _id *= -1;
    }
  }

  void set_id(int64_t id) {
    LIBSEMIGROUPS_ASSERT(id > 0);
    LIBSEMIGROUPS_ASSERT(!active());
    _id = -1 * id;
  }

  int64_t id() const {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    return _id;
  }

  KnuthBendixImpl const* _kbimpl;
  internal_string_type*  _lhs;
  internal_string_type*  _rhs;
  int64_t                _id;
};  // struct Rule

// Simple class wrapping a two iterators to an internal_string_type and a
// Rule const*

class RuleLookup {
 public:
  RuleLookup() : _rule(nullptr) {}

  explicit RuleLookup(Rule* rule)
      : _first(rule->lhs()->cbegin()),
        _last(rule->lhs()->cend()),
        _rule(rule) {}

  RuleLookup& operator()(internal_string_type::iterator const& first,
                         internal_string_type::iterator const& last) {
    _first = first;
    _last  = last;
    return *this;
  }

  Rule const* rule() const {
    return _rule;
  }

  // This implements reverse lex comparison of this and that, which
  // satisfies the requirement of std::set that equivalent items be
  // incomparable, so, for example bcbc and abcbc are considered
  // equivalent, but abcba and bcbc are not.
  bool operator<(RuleLookup const& that) const {
    auto it_this = _last - 1;
    auto it_that = that._last - 1;
    while (it_this > _first && it_that > that._first && *it_this == *it_that) {
      --it_that;
      --it_this;
    }
    return *it_this < *it_that;
  }

 private:
  internal_string_type::const_iterator _first;
  internal_string_type::const_iterator _last;
  Rule const*                          _rule;
};  // class RuleLookup

#endif  // LIBSEMIGROUPS_SRC_KNUTH_BENDIX_RULE_HPP_
