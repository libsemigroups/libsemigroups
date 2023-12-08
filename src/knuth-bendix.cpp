//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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

#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, KnuthBe...

#include <algorithm>      // for max, min
#include <cstddef>        // for size_t
#include <iterator>       // for advance
#include <limits>         // for numeric_limits
#include <string>         // for allocator, basic_string, oper...
#include <unordered_map>  // for unordered_map, operator!=
#include <utility>        // for swap

#include <iostream>

#include "libsemigroups/constants.hpp"  // for Max, PositiveInfinity, operat...
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/obvinf.hpp"     // for is_obviously_infinite
#include "libsemigroups/order.hpp"      // for shortlex_compare
#include "libsemigroups/paths.hpp"      // for Paths
#include "libsemigroups/presentation.hpp"  // for Presentation
#include "libsemigroups/ranges.hpp"        // for operator<<
#include "libsemigroups/runner.hpp"        // for Runner
#include "libsemigroups/types.hpp"         // for word_type
#include "libsemigroups/word-graph.hpp"    // for WordGraph
#include "libsemigroups/words.hpp"         // for ToStrings

#include "libsemigroups/detail/multi-string-view.hpp"  // for is_prefix, maximum_common_prefix
#include "libsemigroups/detail/report.hpp"  // for Reporter, REPORT_DEFAULT, REP...
#include "libsemigroups/detail/string.hpp"  // for is_prefix, maximum_common_prefix

namespace libsemigroups {

  namespace {
    void prefixes_string(std::unordered_map<std::string, size_t>& st,
                         std::string const&                       x,
                         size_t&                                  n) {
      for (auto it = x.cbegin() + 1; it < x.cend(); ++it) {
        auto w   = std::string(x.cbegin(), it);
        auto wit = st.find(w);
        if (wit == st.end()) {
          st.emplace(w, n);
          n++;
        }
      }
    }

  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // KnuthBendix - nested classes
  ////////////////////////////////////////////////////////////////////////

  // Construct from KnuthBendix with new but empty internal_string_type's
  KnuthBendix::Rule::Rule(int64_t id)
      : _lhs(new internal_string_type()),
        _rhs(new internal_string_type()),
        _id(-1 * id) {
    LIBSEMIGROUPS_ASSERT(_id < 0);
  }

  void KnuthBendix::Rule::deactivate() noexcept {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    if (active()) {
      _id *= -1;
    }
  }

  void KnuthBendix::Rule::activate() noexcept {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    if (!active()) {
      _id *= -1;
    }
  }

  // Simple class wrapping a two iterators to an internal_string_type and a
  // Rule const*

  class KnuthBendix::RuleLookup {
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
      while (it_this > _first && it_that > that._first
             && *it_this == *it_that) {
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

  ////////////////////////////////////////////////////////////////////////
  // KnuthBendix::Rules
  ////////////////////////////////////////////////////////////////////////

  KnuthBendix::Rules::Stats::Stats() noexcept {
    init();
  }

  KnuthBendix::Rules::Stats& KnuthBendix::Rules::Stats::init() noexcept {
    max_stack_depth        = 0;
    max_word_length        = 0;
    max_active_word_length = 0;
    max_active_rules       = 0;
    min_length_lhs_rule    = std::numeric_limits<size_t>::max();
    total_rules            = 0;
    return *this;
  }

  KnuthBendix::Rules& KnuthBendix::Rules::init() {
    // Put all active rules and those rules in the stack into the
    // inactive_rules list
    for (Rule const* cptr : _active_rules) {
      Rule* ptr = const_cast<Rule*>(cptr);
      ptr->deactivate();
      _inactive_rules.insert(_inactive_rules.end(), ptr);
    }
    _active_rules.clear();
    for (auto& it : _cursors) {
      it = _active_rules.end();
    }
    return *this;
  }

  KnuthBendix::Rewriter& KnuthBendix::Rewriter::init() {
    Rules::init();
    // Put all active rules and those rules in the stack into the
    // inactive_rules list
    while (!_stack.empty()) {
      Rules::add_inactive_rule(_stack.top());
      _stack.pop();
    }
    _confluent        = false;
    _confluence_known = false;
    return *this;
  }

  KnuthBendix::RewriteFromLeft& KnuthBendix::RewriteFromLeft::init() {
    Rewriter::init();
    _set_rules.clear();
    return *this;
  }

  KnuthBendix::Rules& KnuthBendix::Rules::operator=(Rules const& that) {
    init();
    for (Rule const* rule : that) {
      add_rule(copy_rule(rule));
    }
    for (size_t i = 0; i < _cursors.size(); ++i) {
      _cursors[i] = _active_rules.begin();
      std::advance(
          _cursors[i],
          std::distance(that.begin(),
                        static_cast<const_iterator>(that._cursors[i])));
    }
    return *this;
  }

  KnuthBendix::RewriteFromLeft&
  KnuthBendix::RewriteFromLeft::operator=(RewriteFromLeft const& that) {
    init();
    KnuthBendix::Rewriter::operator=(that);
    for (auto* crule : that) {
      Rule* rule = const_cast<Rule*>(crule);
#ifdef LIBSEMIGROUPS_DEBUG
      LIBSEMIGROUPS_ASSERT(_set_rules.emplace(RuleLookup(rule)).second);
#else
      _set_rules.emplace(RuleLookup(rule));
#endif
    }
    return *this;
  }

  KnuthBendix::Rules::~Rules() {
    for (Rule const* rule : _active_rules) {
      delete const_cast<Rule*>(rule);
    }
    for (Rule* rule : _inactive_rules) {
      delete rule;
    }
  }

  KnuthBendix::Rewriter::~Rewriter() {
    while (!_stack.empty()) {
      Rule* rule = _stack.top();
      _stack.pop();
      delete rule;
    }
  }

  KnuthBendix::Rule* KnuthBendix::Rules::new_rule() {
    ++_stats.total_rules;
    Rule* rule;
    if (!_inactive_rules.empty()) {
      rule = _inactive_rules.front();
      rule->set_id(_stats.total_rules);
      _inactive_rules.erase(_inactive_rules.begin());
    } else {
      rule = new Rule(_stats.total_rules);
    }
    LIBSEMIGROUPS_ASSERT(!rule->active());
    return rule;
  }

  KnuthBendix::Rule* KnuthBendix::Rules::copy_rule(Rule const* rule) {
    return new_rule(rule->lhs()->cbegin(),
                    rule->lhs()->cend(),
                    rule->rhs()->cbegin(),
                    rule->rhs()->cend());
  }

  KnuthBendix::Rules::iterator
  KnuthBendix::Rules::erase_from_active_rules(iterator it) {
    // _stats.unique_lhs_rules.erase(*((*it)->lhs()));
    Rule* rule = const_cast<Rule*>(*it);
    rule->deactivate();

    if (it != _cursors[0] && it != _cursors[1]) {
      it = _active_rules.erase(it);
    } else if (it == _cursors[0] && it != _cursors[1]) {
      _cursors[0] = _active_rules.erase(it);
      it          = _cursors[0];
    } else if (it != _cursors[0] && it == _cursors[1]) {
      _cursors[1] = _active_rules.erase(it);
      it          = _cursors[1];
    } else {
      _cursors[0] = _active_rules.erase(it);
      _cursors[1] = _cursors[0];
      it          = _cursors[0];
    }
    return it;
  }

  KnuthBendix::RewriteFromLeft::iterator
  KnuthBendix::RewriteFromLeft::erase_from_active_rules(iterator it) {
    Rule* rule = const_cast<Rule*>(*it);
    rule->deactivate();
    push_stack(rule);
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(_set_rules.erase(RuleLookup(rule)));
#else
    _set_rules.erase(RuleLookup(rule));
#endif
    LIBSEMIGROUPS_ASSERT(_set_rules.size() == number_of_active_rules() - 1);
    return Rules::erase_from_active_rules(it);
  }

  void KnuthBendix::Rules::add_rule(Rule* rule) {
    LIBSEMIGROUPS_ASSERT(*rule->lhs() != *rule->rhs());
    _stats.max_word_length
        = std::max(_stats.max_word_length, rule->lhs()->size());
    _stats.max_active_rules
        = std::max(_stats.max_active_rules, number_of_active_rules());
    // _stats.unique_lhs_rules.insert(*rule->lhs());
    rule->activate();
    _active_rules.push_back(rule);
    for (auto& it : _cursors) {
      if (it == end()) {
        --it;
      }
    }
    if (rule->lhs()->size() < _stats.min_length_lhs_rule) {
      // TODO(later) this is not valid when using non-length reducing
      // orderings (such as RECURSIVE)
      _stats.min_length_lhs_rule = rule->lhs()->size();
    }
  }

  bool KnuthBendix::Rewriter::push_stack(Rule* rule) {
    LIBSEMIGROUPS_ASSERT(!rule->active());
    if (*rule->lhs() != *rule->rhs()) {
      _stack.emplace(rule);
      return true;
    } else {
      Rules::add_inactive_rule(rule);
      return false;
    }
  }

  void KnuthBendix::RewriteFromLeft::add_rule(Rule* rule) {
    Rules::add_rule(rule);
    // _stats.unique_lhs_rules.insert(*rule->lhs());
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(_set_rules.emplace(RuleLookup(rule)).second);
#else
    _set_rules.emplace(RuleLookup(rule));
#endif
    LIBSEMIGROUPS_ASSERT(_set_rules.size() == number_of_active_rules());
    confluent(tril::unknown);
  }

  // REWRITE_FROM_LEFT from Sims, p67
  // Caution: this uses the assumption that rules are length reducing, if they
  // are not, then u might not have sufficient space!
  void KnuthBendix::RewriteFromLeft::rewrite(internal_string_type& u) const {
    using iterator = internal_string_type::iterator;

    if (u.size() < stats().min_length_lhs_rule) {
      return;
    }

    iterator v_begin = u.begin();
    iterator v_end   = u.begin() + stats().min_length_lhs_rule - 1;
    iterator w_begin = v_end;
    iterator w_end   = u.end();

    RuleLookup lookup;

    while (w_begin != w_end) {
      *v_end = *w_begin;
      ++v_end;
      ++w_begin;

      auto it = _set_rules.find(lookup(v_begin, v_end));
      if (it != _set_rules.end()) {
        Rule const* rule = (*it).rule();
        if (rule->lhs()->size() <= static_cast<size_t>(v_end - v_begin)) {
          LIBSEMIGROUPS_ASSERT(detail::is_suffix(
              v_begin, v_end, rule->lhs()->cbegin(), rule->lhs()->cend()));
          v_end -= rule->lhs()->size();
          w_begin -= rule->rhs()->size();
          detail::string_replace(
              w_begin, rule->rhs()->cbegin(), rule->rhs()->cend());
        }
      }
      while (w_begin != w_end
             && stats().min_length_lhs_rule - 1
                    > static_cast<size_t>((v_end - v_begin))) {
        *v_end = *w_begin;
        ++v_end;
        ++w_begin;
      }
    }
    u.erase(v_end - u.cbegin());
  }

  void KnuthBendix::RewriteFromLeft::rewrite(Rule* rule) const {
    // LIBSEMIGROUPS_ASSERT(_id != 0);
    rewrite(*rule->lhs());
    rewrite(*rule->rhs());
    rule->reorder();
  }

  // TEST_2 from Sims, p76
  void KnuthBendix::RewriteFromLeft::clear_stack() {
    while (number_of_pending_rules() != 0) {
      // _stats.max_stack_depth = std::max(_stats.max_stack_depth,
      // _stack.size());

      Rule* rule1 = next_pending_rule();
      LIBSEMIGROUPS_ASSERT(!rule1->active());
      LIBSEMIGROUPS_ASSERT(*rule1->lhs() != *rule1->rhs());
      // Rewrite both sides and reorder if necessary . . .
      rewrite(rule1);

      if (*rule1->lhs() != *rule1->rhs()) {
        internal_string_type const* lhs = rule1->lhs();
        for (auto it = begin(); it != end();) {
          Rule* rule2 = const_cast<Rule*>(*it);
          if (rule2->lhs()->find(*lhs) != external_string_type::npos) {
            it = erase_from_active_rules(it);
            // rule2 is added to _inactive_rules or _active_rules by
            // clear_stack
          } else {
            if (rule2->rhs()->find(*lhs) != external_string_type::npos) {
              rewrite(*rule2->rhs());
            }
            ++it;
          }
        }
        add_rule(rule1);
        // rule1 is activated, we do this after removing rules that rule1
        // makes redundant to avoid failing to insert rule1 in _set_rules
      } else {
        add_inactive_rule(rule1);
      }
    }
  }

  void KnuthBendix::RewriteFromLeft::reduce() {
    for (Rule const* rule : *this) {
      // Copy rule and push_stack so that it is not modified by the
      // call to clear_stack.
      LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
      if (push_stack(copy_rule(rule))) {
        clear_stack();
      }
    }
  }

  bool KnuthBendix::RewriteFromLeft::confluent() const {
    if (number_of_pending_rules() != 0) {
      return false;
    } else if (confluence_known()) {
      return Rewriter::confluent();
    }
    // bool reported = false;
    confluent(tril::TRUE);
    internal_string_type word1;
    internal_string_type word2;
    // size_t               seen = 0;

    for (auto it1 = begin(); it1 != end(); ++it1) {
      Rule const* rule1 = *it1;
      // Seems to be much faster to do this in reverse.
      for (auto it2 = rbegin(); it2 != rend(); ++it2) {
        // seen++;
        Rule const* rule2 = *it2;
        for (auto it = rule1->lhs()->cend() - 1; it >= rule1->lhs()->cbegin();
             --it) {
          // Find longest common prefix of suffix B of rule1.lhs() defined by
          // it and R = rule2.lhs()
          auto prefix = detail::maximum_common_prefix(it,
                                                      rule1->lhs()->cend(),
                                                      rule2->lhs()->cbegin(),
                                                      rule2->lhs()->cend());
          if (prefix.first == rule1->lhs()->cend()
              || prefix.second == rule2->lhs()->cend()) {
            // Seems that this function isn't called enough to merit using
            // MSV's here.
            word1.assign(rule1->lhs()->cbegin(),
                         it);             // A
            word1.append(*rule2->rhs());  // S
            word1.append(prefix.first,
                         rule1->lhs()->cend());  // D

            word2.assign(*rule1->rhs());  // Q
            word2.append(prefix.second,
                         rule2->lhs()->cend());  // E

            if (word1 != word2) {
              rewrite(word1);
              rewrite(word2);
              if (word1 != word2) {
                confluent(tril::FALSE);
                return false;
              }
            }
          }
        }
      }
    }
    return confluent();
  }

  ////////////////////////////////////////////////////////////////////////

  struct KnuthBendix::ABC : KnuthBendix::OverlapMeasure {
    size_t operator()(Rule const*                                 AB,
                      Rule const*                                 BC,
                      internal_string_type::const_iterator const& it) {
      LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
      LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
      // |A| + |BC|
      return (it - AB->lhs()->cbegin()) + BC->lhs()->size();
    }
  };

  struct KnuthBendix::AB_BC : KnuthBendix::OverlapMeasure {
    size_t operator()(Rule const*                                 AB,
                      Rule const*                                 BC,
                      internal_string_type::const_iterator const& it) {
      LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
      LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
      (void) it;
      // |AB| + |BC|
      return AB->lhs()->size() + BC->lhs()->size();
    }
  };

  struct KnuthBendix::MAX_AB_BC : KnuthBendix::OverlapMeasure {
    size_t operator()(Rule const*                                 AB,
                      Rule const*                                 BC,
                      internal_string_type::const_iterator const& it) {
      LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
      LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
      (void) it;
      // max(|AB|, |BC|)
      return std::max(AB->lhs()->size(), BC->lhs()->size());
    }
  };

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix::Settings - constructor - public
  //////////////////////////////////////////////////////////////////////////

  KnuthBendix::Settings::Settings() noexcept {
    init();
  }

  KnuthBendix::Settings& KnuthBendix::Settings::init() noexcept {
    check_confluence_interval = 4'096;
    max_overlap               = POSITIVE_INFINITY;
    max_rules                 = POSITIVE_INFINITY;
    overlap_policy            = options::overlap::ABC;
    return *this;
  }

  KnuthBendix::Stats::Stats() noexcept {
    init();
  }

  KnuthBendix::Stats& KnuthBendix::Stats::init() noexcept {
    max_stack_depth        = 0;
    max_word_length        = 0;
    max_active_word_length = 0;
    max_active_rules       = 0;

    min_length_lhs_rule = std::numeric_limits<size_t>::max();

    prev_active_rules   = 0;
    prev_inactive_rules = 0;
    prev_total_rules    = 0;
    total_rules         = 0;
    unique_lhs_rules.clear();
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - setters for Settings - public
  //////////////////////////////////////////////////////////////////////////

  KnuthBendix& KnuthBendix::overlap_policy(options::overlap p) {
    if (p == _settings.overlap_policy && _overlap_measure != nullptr) {
      return *this;
    }
    delete _overlap_measure;
    switch (p) {
      case options::overlap::ABC:
        _overlap_measure = new ABC();
        break;
      case options::overlap::AB_BC:
        _overlap_measure = new AB_BC();
        break;
      case options::overlap::MAX_AB_BC:
        _overlap_measure = new MAX_AB_BC();
        break;
      default:
        LIBSEMIGROUPS_ASSERT(false);
    }
    _settings.overlap_policy = p;
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - constructors and destructor - public
  //////////////////////////////////////////////////////////////////////////

  KnuthBendix::KnuthBendix(congruence_kind knd)
      : CongruenceInterface(knd),
        _settings(),
        _stats(),
        _rewriter(),
        _gen_pairs_initted(),
        _gilman_graph(),
        _internal_is_same_as_external(),
        _overlap_measure(),
        _presentation() {
    init(knd);
  }

  KnuthBendix& KnuthBendix::init(congruence_kind knd) {
    _rewriter.init();

    CongruenceInterface::init(knd);
    _settings.init();
    _stats.init();

    _gen_pairs_initted = false;
    _gilman_graph.init(0, 0);
    _internal_is_same_as_external = false;
    _overlap_measure              = nullptr;
    _presentation.init();
    overlap_policy(_settings.overlap_policy);
    return *this;
  }

  KnuthBendix::KnuthBendix(KnuthBendix const& that) : KnuthBendix(that.kind()) {
    *this = that;
  }

  KnuthBendix::KnuthBendix(KnuthBendix&& that)
      : KnuthBendix(static_cast<KnuthBendix const&>(that)) {}

  KnuthBendix& KnuthBendix::operator=(KnuthBendix&& that) {
    *this = static_cast<KnuthBendix const&>(that);
    return *this;
  }

  KnuthBendix& KnuthBendix::operator=(KnuthBendix const& that) {
    Runner::operator=(that);
    _rewriter = that._rewriter;

    _settings                     = that._settings;
    _gilman_graph                 = that._gilman_graph;
    _internal_is_same_as_external = that._internal_is_same_as_external;
    _stats.min_length_lhs_rule    = that._stats.min_length_lhs_rule;
    _presentation                 = that._presentation;
    _stats.total_rules            = that._stats.total_rules;

    overlap_policy(_settings.overlap_policy);

    return *this;
  }

  KnuthBendix::~KnuthBendix() {
    delete _overlap_measure;
  }

  KnuthBendix& KnuthBendix::init(congruence_kind                  knd,
                                 Presentation<std::string> const& p) {
    return private_init(knd, p, true);
  }

  KnuthBendix& KnuthBendix::init(congruence_kind             knd,
                                 Presentation<std::string>&& p) {
    return private_init(knd, std::move(p), true);
  }

  KnuthBendix& KnuthBendix::private_init(congruence_kind                  knd,
                                         Presentation<std::string> const& p,
                                         bool call_init) {
    p.validate();
    if (call_init) {
      init(knd);
    }
    _presentation = p;
    init_from_presentation();
    return *this;
  }

  KnuthBendix& KnuthBendix::private_init(congruence_kind             knd,
                                         Presentation<std::string>&& p,
                                         bool call_init) {
    p.validate();
    if (call_init) {
      init(knd);
    }
    _presentation = std::move(p);
    init_from_presentation();
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - attributes - public
  //////////////////////////////////////////////////////////////////////////

  uint64_t KnuthBendix::number_of_classes() {
    if (is_obviously_infinite(*this)) {
      return POSITIVE_INFINITY;
    }

    int const modifier = (presentation().contains_empty_word() ? 0 : -1);
    if (presentation().alphabet().empty()) {
      return 1 + modifier;
    } else {
      uint64_t const out = knuth_bendix::normal_forms(*this).count();
      return (out == POSITIVE_INFINITY ? out : out + modifier);
    }
  }

  bool KnuthBendix::equal_to(std::string const& u, std::string const& v) {
    presentation().validate_word(u.cbegin(), u.cend());
    presentation().validate_word(v.cbegin(), v.cend());

    if (u == v) {
      return true;
    }

    external_string_type uu = u;
    external_string_type vv = v;

    if (kind() == congruence_kind::left) {
      std::reverse(uu.begin(), uu.end());
      std::reverse(vv.begin(), vv.end());
    }

    add_octo(uu);
    add_octo(vv);

    rewrite_inplace(uu);
    rewrite_inplace(vv);

    if (uu == vv) {
      return true;
    }

    run();
    external_to_internal_string(uu);
    external_to_internal_string(vv);
    _rewriter.rewrite(uu);
    _rewriter.rewrite(vv);
    return uu == vv;
  }

  std::string KnuthBendix::normal_form(std::string const& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    run();
    return rewrite(w);
  }

  void KnuthBendix::report_rules() const {
    using detail::group_digits;
    using detail::signed_group_digits;
    using std::chrono::duration_cast;
    using high_resolution_clock = std::chrono::high_resolution_clock;
    using nanoseconds           = std::chrono::nanoseconds;

    if (!report() && (!reporting_enabled() || !finished())) {
      return;
    }
    auto run_time = duration_cast<nanoseconds>(high_resolution_clock::now()
                                               - start_time());

    auto active   = number_of_active_rules();
    auto inactive = number_of_inactive_rules();
    auto defined  = _stats.total_rules;

    int64_t const active_diff   = active - _stats.prev_active_rules;
    int64_t const inactive_diff = inactive - _stats.prev_inactive_rules;
    int64_t const defined_diff  = defined - _stats.prev_total_rules;

    auto const mean_defined
        = group_digits(std::pow(10, 9) * static_cast<double>(defined)
                       / run_time.count())
          + "/s";
    auto const mean_killed
        = group_digits(std::pow(10, 9) * static_cast<double>(inactive)
                       / run_time.count())
          + "/s";

    auto msg = fmt::format("{:-<95}\n", "");
    msg += fmt_default(
        "KnuthBendix: rules {:>12} (active) | {:>12} (inactive) | {:>12} "
        "(defined)\n",
        group_digits(active),
        group_digits(inactive),
        group_digits(defined));

    msg += fmt_default(
        "KnuthBendix: diff  {:>12} (active) | {:>12} (inactive) | "
        "{:>12} (defined)\n",
        signed_group_digits(active_diff),
        signed_group_digits(inactive_diff),
        signed_group_digits(defined_diff));
    msg += fmt_default(
        "KnuthBendix: time  {:>12} (total)  | {:>12} (killed)   | {:>12} "
        "(defined)\n",
        detail::string_time(run_time),
        mean_killed,
        mean_defined);
    msg += fmt::format("{:-<95}\n", "");

    if (finished()) {
      auto width = group_digits(_stats.max_stack_depth).size();
      msg += fmt_default("KnuthBendix: max stack depth          {:>{width}}\n",
                         group_digits(_stats.max_stack_depth),
                         fmt::arg("width", width));
      msg += fmt_default("KnuthBendix: max rule length          {:>{width}}\n",
                         group_digits(_stats.max_word_length),
                         fmt::arg("width", width));
      msg += fmt_default("KnuthBendix: max active rule length   {:>{width}}\n",
                         group_digits(max_active_word_length()),
                         fmt::arg("width", width));
      msg += fmt_default("KnuthBendix: number of unique lhs     {:>{width}}\n",
                         group_digits(_stats.unique_lhs_rules.size()),
                         fmt::arg("width", width));
      msg += fmt::format("{:-<95}\n", "");
    }

    report_no_prefix(msg);
    stats_check_point();
  }

  void KnuthBendix::rewrite_inplace(external_string_type& w) const {
    if (kind() == congruence_kind::left) {
      std::reverse(w.begin(), w.end());
    }
    add_octo(w);
    external_to_internal_string(w);
    _rewriter.rewrite(w);
    internal_to_external_string(w);
    rm_octo(w);
    if (kind() == congruence_kind::left) {
      std::reverse(w.begin(), w.end());
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - other methods - private
  //////////////////////////////////////////////////////////////////////////

  void KnuthBendix::throw_if_started() const {
    if (started()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the presentation cannot be changed after Knuth-Bendix has "
          "started, maybe try `init` instead?");
    }
  }

  void KnuthBendix::stats_check_point() const {
    _stats.prev_active_rules   = _rewriter.number_of_active_rules();
    _stats.prev_inactive_rules = _rewriter.number_of_inactive_rules();
    _stats.prev_total_rules    = _stats.total_rules;
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - main methods - public
  //////////////////////////////////////////////////////////////////////////

  bool KnuthBendix::confluent_known() const noexcept {
    return _rewriter.confluence_known();
  }

  bool KnuthBendix::confluent() const {
    return _rewriter.confluent();
  }

  bool KnuthBendix::finished_impl() const {
    return confluent_known() && confluent();
  }

  void KnuthBendix::init_from_generating_pairs() {
    if (_gen_pairs_initted) {
      return;
    }

    _gen_pairs_initted = true;

    auto& p     = _presentation;
    auto  pairs = (generating_pairs() | rx::transform([&p](auto const& w) {
                    return to_string(p, w);
                  }))
                 | rx::in_groups_of_exactly(2);

    if (kind() != congruence_kind::twosided && (pairs | rx::count()) != 0) {
      p.alphabet(p.alphabet() + presentation::first_unused_letter(p));
    }
    for (auto&& x : pairs) {
      auto lhs = x.get();
      add_octo(lhs);
      x.next();
      auto rhs = x.get();
      add_octo(rhs);
      add_rule_impl(lhs, rhs);
      presentation::add_rule(p, lhs, rhs);
    }
  }

  void KnuthBendix::run_impl() {
    stats_check_point();
    reset_start_time();

    init_from_generating_pairs();
    if (_rewriter.consistent() && confluent() && !stopped()) {
      // _rewriter._stack can be non-empty if non-reduced rules were used to
      // define the KnuthBendix.  If _rewriter._stack is non-empty, then it
      // means that the rules in _rewriter might not define the system.
      report_default("KnuthBendix: the system is confluent already!\n");
      return;
    } else if (_rewriter.number_of_active_rules() >= max_rules()) {
      report_default(
          "KnuthBendix: too many rules, found {}, max_rules() is {}\n",
          _rewriter.number_of_active_rules(),
          max_rules());
      return;
    }
    _rewriter.reduce();

    auto& first  = _rewriter.cursor(0);
    auto& second = _rewriter.cursor(1);
    first        = _rewriter.begin();

    size_t nr = 0;
    while (first != _rewriter.end()
           && _rewriter.number_of_active_rules() < _settings.max_rules
           && !stopped()) {
      Rule const* rule1 = *first;
      // It is tempting to remove rule1 and rule2 here and use *first and
      // *second instead but this leads to some badness (which we didn't
      // understand, but it also didn't seem super important).
      second = first++;
      overlap(rule1, rule1);
      while (second != _rewriter.begin() && rule1->active()) {
        --second;
        Rule const* rule2 = *second;
        overlap(rule1, rule2);
        ++nr;
        if (rule1->active() && rule2->active()) {
          ++nr;
          overlap(rule2, rule1);
        }
      }
      if (nr > _settings.check_confluence_interval) {
        if (confluent()) {
          break;
        }
        nr = 0;
      }
    }

    // LIBSEMIGROUPS_ASSERT(_rewriter._stack.empty());
    // Seems that the stack can be non-empty here in KnuthBendix 12, 14, 16
    // and maybe more
    if (_settings.max_overlap == POSITIVE_INFINITY
        && _settings.max_rules == POSITIVE_INFINITY && !stopped()) {
      _rewriter.confluent(tril::TRUE);
    }
    report_rules();
    if (finished()) {
      report_default("KnuthBendix: finished!\n");
    } else {
      report_why_we_stopped();
    }
  }

  size_t KnuthBendix::number_of_active_rules() const noexcept {
    return _rewriter.number_of_active_rules();
  }

  WordGraph<size_t> const& KnuthBendix::gilman_graph() {
    if (_gilman_graph.number_of_nodes() == 0
        && !presentation().alphabet().empty()) {
      // reset the settings so that we really run!
      max_rules(POSITIVE_INFINITY);
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      LIBSEMIGROUPS_ASSERT(confluent());
      std::unordered_map<std::string, size_t> prefixes;
      prefixes.emplace("", 0);
      size_t n = 1;
      for (auto const* rule : _rewriter) {
        prefixes_string(prefixes, *rule->lhs(), n);
      }

      _gilman_graph_node_labels.resize(prefixes.size(), "");
      for (auto const& p : prefixes) {
        _gilman_graph_node_labels[p.second] = p.first;
        internal_to_external_string(_gilman_graph_node_labels[p.second]);
      }

      _gilman_graph.add_nodes(prefixes.size());
      _gilman_graph.add_to_out_degree(presentation().alphabet().size());

      for (auto& p : prefixes) {
        for (size_t i = 0; i < presentation().alphabet().size(); ++i) {
          auto s  = p.first + uint_to_internal_string(i);
          auto it = prefixes.find(s);
          if (it != prefixes.end()) {
            _gilman_graph.set_target(p.second, i, it->second);
          } else {
            auto t = s;
            _rewriter.rewrite(t);
            if (t == s) {
              while (!s.empty()) {
                s  = std::string(s.begin() + 1, s.end());
                it = prefixes.find(s);
                if (it != prefixes.end()) {
                  _gilman_graph.set_target(p.second, i, it->second);
                  break;
                }
              }
            }
          }
        }
      }
      if (kind() != congruence_kind::twosided
          && (generating_pairs() | rx::count()) != 0) {
        auto const& p    = presentation();
        auto        octo = p.index(p.alphabet().back());
        auto        src  = _gilman_graph.target_no_checks(0, octo);
        LIBSEMIGROUPS_ASSERT(src != UNDEFINED);
        _gilman_graph.remove_label_no_checks(octo);
        auto nodes = word_graph::nodes_reachable_from(_gilman_graph, src);
        LIBSEMIGROUPS_ASSERT(std::find(nodes.cbegin(), nodes.cend(), src)
                             != nodes.cend());
        // This is a bit awkward, it exists to ensure
        // that node 0 in the induced subdigraph is src.
        std::vector<decltype(src)> sorted_nodes(nodes.cbegin(), nodes.cend());
        // The order which nodes come out of nodes_reachable_from is
        // non-deterministic and so we sort the nodes
        std::sort(sorted_nodes.begin(), sorted_nodes.end());
        if (sorted_nodes[0] != src) {
          std::iter_swap(
              sorted_nodes.begin(),
              std::find(sorted_nodes.begin(), sorted_nodes.end(), src));
        }

        _gilman_graph.induced_subgraph_no_checks(sorted_nodes.cbegin(),
                                                 sorted_nodes.cend());
      }
    }
    return _gilman_graph;
  }

  //////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - pure virtual methods - private
  //////////////////////////////////////////////////////////////////////////

  void KnuthBendix::add_rule_impl(std::string const& p, std::string const& q) {
    if (p == q) {
      return;
    }
    if (_internal_is_same_as_external) {
      _rewriter.add_rule(p, q);
    } else {
      auto pp(p), qq(q);
      // Can't see an alternative to the copy here
      external_to_internal_string(pp);
      external_to_internal_string(qq);
      _rewriter.add_rule(pp, qq);
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendixImpl - converting ints <-> string/char - private
  //////////////////////////////////////////////////////////////////////////

  size_t KnuthBendix::internal_char_to_uint(internal_char_type c) {
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(c >= 97);
    return static_cast<size_t>(c - 97);
#else
    return static_cast<size_t>(c - 1);
#endif
  }

  KnuthBendix::internal_char_type KnuthBendix::uint_to_internal_char(size_t a) {
    LIBSEMIGROUPS_ASSERT(
        a <= size_t(std::numeric_limits<internal_char_type>::max()));
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(
        a <= size_t(std::numeric_limits<internal_char_type>::max() - 97));
    return static_cast<internal_char_type>(a + 97);
#else
    return static_cast<internal_char_type>(a + 1);
#endif
  }

  KnuthBendix::internal_string_type
  KnuthBendix::uint_to_internal_string(size_t i) {
    LIBSEMIGROUPS_ASSERT(
        i <= size_t(std::numeric_limits<internal_char_type>::max()));
    return internal_string_type({uint_to_internal_char(i)});
  }

  word_type
  KnuthBendix::internal_string_to_word(internal_string_type const& s) {
    word_type w;
    w.reserve(s.size());
    for (internal_char_type const& c : s) {
      w.push_back(internal_char_to_uint(c));
    }
    return w;
  }

  KnuthBendix::internal_char_type
  KnuthBendix::external_to_internal_char(external_char_type c) const {
    LIBSEMIGROUPS_ASSERT(!_internal_is_same_as_external);
    return uint_to_internal_char(presentation().index(c));
  }

  KnuthBendix::external_char_type
  KnuthBendix::internal_to_external_char(internal_char_type a) const {
    LIBSEMIGROUPS_ASSERT(!_internal_is_same_as_external);
    return presentation().letter_no_checks(internal_char_to_uint(a));
  }

  void KnuthBendix::external_to_internal_string(external_string_type& w) const {
    if (_internal_is_same_as_external) {
      return;
    }
    for (auto& a : w) {
      a = external_to_internal_char(a);
    }
  }

  void KnuthBendix::internal_to_external_string(internal_string_type& w) const {
    if (_internal_is_same_as_external) {
      return;
    }
    for (auto& a : w) {
      a = internal_to_external_char(a);
    }
  }

  void KnuthBendix::add_octo(external_string_type& w) const {
    if (kind() != congruence_kind::twosided
        && (generating_pairs() | rx::count()) != 0) {
      w = presentation().alphabet().back() + w;
    }
  }

  void KnuthBendix::rm_octo(external_string_type& w) const {
    if (kind() != congruence_kind::twosided
        && (generating_pairs() | rx::count()) != 0) {
      LIBSEMIGROUPS_ASSERT(w.front() == presentation().alphabet().back());
      w.erase(w.begin());
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendixImpl - methods for rules - private
  //////////////////////////////////////////////////////////////////////////

  void KnuthBendix::init_from_presentation() {
    auto const& p                 = _presentation;
    _internal_is_same_as_external = true;
    for (size_t i = 0; i < p.alphabet().size(); ++i) {
      if (uint_to_internal_char(i) != p.letter_no_checks(i)) {
        _internal_is_same_as_external = false;
        break;
      }
    }
    auto const first = p.rules.cbegin();
    auto const last  = p.rules.cend();
    for (auto it = first; it != last; it += 2) {
      auto lhs = *it, rhs = *(it + 1);
      if (kind() == congruence_kind::left) {
        std::reverse(lhs.begin(), lhs.end());
        std::reverse(rhs.begin(), rhs.end());
      }
      add_rule_impl(lhs, rhs);
    }
  }

  // OVERLAP_2 from Sims, p77
  void KnuthBendix::overlap(Rule const* u, Rule const* v) {
    LIBSEMIGROUPS_ASSERT(u->active() && v->active());
    auto const &ulhs = *(u->lhs()), vlhs = *(v->lhs());
    auto const &urhs = *(u->rhs()), vrhs = *(v->rhs());
    auto const  limit = ulhs.cend() - std::min(ulhs.size(), vlhs.size());

    int64_t const u_id = u->id(), v_id = v->id();
    for (auto it = ulhs.cend() - 1;
         it > limit && u_id == u->id() && v_id == v->id() && !stopped()
         && (_settings.max_overlap == POSITIVE_INFINITY
             || (*_overlap_measure)(u, v, it) <= _settings.max_overlap);
         --it) {
      // Check if B = [it, ulhs.cend()) is a prefix of v->lhs()
      if (detail::is_prefix(vlhs.cbegin(), vlhs.cend(), it, ulhs.cend())) {
        // u = P_i = AB -> Q_i and v = P_j = BC -> Q_j This version of
        // new_rule does not reorder _rewriter.add_rule(AQ_j, Q_iC);
        detail::MultiStringView x(ulhs.cbegin(), it);
        x.append(vrhs.cbegin(), vrhs.cend());
        detail::MultiStringView y(urhs.cbegin(), urhs.cend());
        y.append(vlhs.cbegin() + (ulhs.cend() - it),
                 vlhs.cend());  // rule = AQ_j -> Q_iC
        _rewriter.add_rule(x, y);
        // It can be that the iterator `it` is invalidated by the call to
        // push_stack (i.e. if `u` is deactivated, then rewritten, actually
        // changed, and reactivated) and that is the reason for the checks in
        // the for-loop above. If this is the case, then we should stop
        // considering the overlaps of u and v here, and note that they will
        // be considered later, because when the rule `u` is reactivated it is
        // added to the end of the active rules list.
      }
    }
  }

  size_t KnuthBendix::max_active_word_length() const {
    auto comp = [](Rule const* p, Rule const* q) -> bool {
      return p->lhs()->size() < q->lhs()->size();
    };
    auto max = std::max_element(_rewriter.begin(), _rewriter.end(), comp);
    if (max != _rewriter.end()) {
      _stats.max_active_word_length
          = std::max(_stats.max_active_word_length, (*max)->lhs()->size());
    }
    return _stats.max_active_word_length;
  }

  namespace knuth_bendix {
    // We are computing non_trivial_classes with respect to kb2 (the greater
    // congruence, with fewer classes)
    //
    // This should work ok if kb1 and kb2 represent different kinds of
    // congruence.
    std::vector<std::vector<std::string>>
    non_trivial_classes(KnuthBendix& kb1, KnuthBendix& kb2) {
      using rx::operator|;

      // It is intended that kb1 is defined using the same presentation as kb2
      // and some additional rules. The output might still be meaningful if
      // this is not the case.
      if (kb2.number_of_classes() == POSITIVE_INFINITY
          && kb1.number_of_classes() != POSITIVE_INFINITY) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument defines an infinite semigroup, and the 2nd "
            "argument defines a finite semigroup, so there is at least one "
            "infinite non-trivial class!");
      } else if (kb2.presentation().alphabet()
                 != kb1.presentation().alphabet()) {
        // It might be possible to handle this case too,
        // but doesn't seem worth it at present
        LIBSEMIGROUPS_EXCEPTION("the arguments must have presentations with "
                                "the same alphabets, found {} and {}",
                                kb2.presentation().alphabet(),
                                kb1.presentation().alphabet());
      }

      // We construct the WordGraph `ad` obtained by subtracting all of the
      // edges from the Gilman graph of kb1 from the Gilman graph of kb2. The
      // non-trivial classes are finite if and only if `ad` is acyclic. It
      // would be possible to do this without actually constructing `ad` but
      // constructing `ad` is simpler, and so we do that for now.

      auto g2 = kb2.gilman_graph();
      auto g1 = kb1.gilman_graph();

      LIBSEMIGROUPS_ASSERT(g2.number_of_nodes() > 0);
      LIBSEMIGROUPS_ASSERT(g1.number_of_nodes() > 0);

      if (g2.number_of_nodes() < g1.number_of_nodes()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the Gilman digraph of the 1st argument must have at least as "
            "many "
            "nodes as the Gilman digraph of the 2nd argument, found {} nodes "
            "and {} nodes",
            g2.number_of_nodes(),
            g1.number_of_nodes());
      }

      // We need to obtain a mappings from the nodes of
      // g2 to g1 and vice versa.

      using node_type = decltype(g2)::node_type;

      std::vector<node_type> to_g1(g2.number_of_nodes(),
                                   static_cast<node_type>(UNDEFINED));
      to_g1[0] = 0;
      std::vector<node_type> to_g2(g1.number_of_nodes(),
                                   static_cast<node_type>(UNDEFINED));
      to_g2[0] = 0;
      for (auto v : g2.nodes()) {
        for (auto e : g2.labels()) {
          auto ve2 = g2.target_no_checks(v, e);
          if (to_g1[v] != UNDEFINED && ve2 != UNDEFINED) {
            auto ve1 = g1.target_no_checks(to_g1[v], e);
            if (ve1 != UNDEFINED && to_g1[ve2] == UNDEFINED) {
              to_g1[ve2] = ve1;
              to_g2[ve1] = ve2;
            }
          }
        }
      }

      // We do a depth first search simultaneously for cycles, and edges E in
      // g2 not in g1. Pre order forcycle detection, post order for "can we
      // reach a node incident to an edge in E" and "number of paths through a
      // node is infinite"
      size_t const N = g2.number_of_nodes();
      // can_reach[v] == true if there is a path from v to a node incident to
      // an edge in g2 that's not in g1.
      std::vector<bool> can_reach(N, false);
      std::vector<bool> inf_paths(N, false);
      std::vector<bool> seen(N, false);

      std::stack<node_type> stck;
      stck.push(0);

      while (!stck.empty()) {
        auto v = stck.top();
        stck.pop();
        if (v >= N) {
          // post order
          v -= N;
          for (auto e : g2.labels()) {
            auto ve = g2.target_no_checks(v, e);
            if (ve != UNDEFINED) {
              can_reach[v] = (can_reach[v] || can_reach[ve]);
              if (can_reach[ve]) {
                inf_paths[v] = inf_paths[ve];
              }
              if (can_reach[v] && inf_paths[v]) {
                LIBSEMIGROUPS_EXCEPTION(
                    "there is an infinite non-trivial class!");
              }
            }
          }
        } else {
          seen[v] = true;
          // so we can tell when all of the descendants of v have been
          // processed out of the stack
          stck.push(v + N);
          if (to_g1[v] == UNDEFINED) {
            can_reach[v] = true;
          }
          for (auto e : g2.labels()) {
            auto ve2 = g2.target_no_checks(v, e);
            if (ve2 != UNDEFINED) {
              // Check if (v, e, ve2) corresponds to an edge in g1
              if (!can_reach[v]) {
                auto ve1 = g1.target_no_checks(to_g1[v], e);
                if (ve1 != UNDEFINED) {
                  // edges (v, e, ve2) and (to_g1[v], e, ve1) exist, so
                  // there's an edge in g2 not in g1 if the targets of these
                  // edges do not correspond to each other.
                  can_reach[v] = (ve2 != to_g2[ve1]);
                } else {
                  // There's no edge labelled by e incident to the node
                  // corresponding to v in g1, but there is such an edge in g2
                  // and so (v, e, ve2) is in g2 but not g1.
                  can_reach[v] = true;
                }
              }
              if (seen[ve2]) {
                // cycle detected
                inf_paths[v] = true;
              } else {
                stck.push(ve2);
              }
            }
          }
        }
      }

      // If we reach here, then the appropriate portion of g2 is acyclic, and
      // so all we do is enumerate the paths in that graph

      // Construct the "can_reach" subgraph of g2, could use a WordGraphView
      // here instead (but these don't yet exist) TODO(later)
      WordGraph<size_t> ad(g2.number_of_nodes(), g2.out_degree());

      for (auto v : ad.nodes()) {
        if (can_reach[v]) {
          for (auto e : ad.labels()) {
            auto ve = g2.target_no_checks(v, e);
            if (ve != UNDEFINED && can_reach[ve]) {
              ad.set_target_no_checks(v, e, ve);
            }
          }
        }
      }

      Paths paths(ad);
      // We only want those paths that pass through at least one of the edges
      // in g2 but not g1. Hence we require the `filter` in the next
      // expression.
      auto ntc
          = partition(kb1,
                      (paths.from(0) | rx::filter([&g1](word_type const& path) {
                         return word_graph::last_node_on_path(
                                    g1, 0, path.cbegin(), path.cend())
                                    .second
                                != path.cend();
                       })
                       | ToStrings(kb2.presentation().alphabet())));
      // The check in the next loop could be put into the lambda passed to
      // filter above, but then we'd have to convert `path` to a string, and
      // then discard the string, so better to do it here. Note that the
      // normal forms in `kb1` never contain an edge in g2 \ g1 and so we must
      // add in every normal form.
      for (auto& klass : ntc) {
        klass.push_back(kb1.normal_form(klass[0]));
      }
      return ntc;
    }

    void by_overlap_length(KnuthBendix& kb) {
      size_t prev_max_overlap               = kb.max_overlap();
      size_t prev_check_confluence_interval = kb.check_confluence_interval();
      kb.max_overlap(1);
      kb.check_confluence_interval(POSITIVE_INFINITY);

      while (!kb.confluent()) {
        kb.run();
        kb.max_overlap(kb.max_overlap() + 1);
      }
      kb.max_overlap(prev_max_overlap);
      kb.check_confluence_interval(prev_check_confluence_interval);
      kb.report_rules();
    }

  }  // namespace knuth_bendix

  std::ostream& operator<<(std::ostream& os, KnuthBendix const& kb) {
    os << kb.active_rules();
    return os;
  }
}  // namespace libsemigroups
