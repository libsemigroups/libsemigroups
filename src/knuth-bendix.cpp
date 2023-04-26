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

#include <algorithm>                     // for max, min
#include <cstddef>                       // for size_t
#include <iterator>                      // for advance
#include <limits>                        // for numeric_limits
#include <string>                        // for allocator, basic_string, oper...
#include <unordered_map>                 // for unordered_map, operator!=
#include <utility>                       // for swap
                                         //
#include "libsemigroups/constants.hpp"   // for Max, PositiveInfinity, operat...
#include "libsemigroups/debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/obvinf.hpp"      // for is_obviously_infinite
#include "libsemigroups/order.hpp"       // for shortlex_compare
#include "libsemigroups/paths.hpp"       // for Paths
#include "libsemigroups/present.hpp"     // for Presentation
#include "libsemigroups/ranges.hpp"      // for operator<<
#include "libsemigroups/report.hpp"      // for Reporter, REPORT_DEFAULT, REP...
#include "libsemigroups/runner.hpp"      // for Runner
#include "libsemigroups/string.hpp"      // for is_prefix, maximum_common_prefix
#include "libsemigroups/timer.hpp"       // for Timer
#include "libsemigroups/types.hpp"       // for word_type
#include "libsemigroups/word-graph.hpp"  // for WordGraph
#include "libsemigroups/words.hpp"       // for to_strings

#include "rx/ranges.hpp"

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

  // KnuthBendix::KnuthBendix(KnuthBendix&&) = default;

  ////////////////////////////////////////////////////////////////////////
  // KnuthBendix - nested classes
  ////////////////////////////////////////////////////////////////////////

  // Construct from KnuthBendix with new but empty internal_string_type's
  KnuthBendix::Rule::Rule(KnuthBendix const* kbimpl, int64_t id)
      : _kbimpl(kbimpl),
        _lhs(new internal_string_type()),
        _rhs(new internal_string_type()),
        _id(-1 * id) {
    LIBSEMIGROUPS_ASSERT(_id < 0);
  }

  void KnuthBendix::Rule::rewrite() {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    _kbimpl->internal_rewrite(*_lhs);
    _kbimpl->internal_rewrite(*_rhs);
    // reorder if necessary
    if (shortlex_compare(_lhs, _rhs)) {
      std::swap(_lhs, _rhs);
    }
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
    _check_confluence_interval = 4'096;
    _max_overlap               = POSITIVE_INFINITY;
    _max_rules                 = POSITIVE_INFINITY;
    _overlap_policy            = options::overlap::ABC;
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - setters for Settings - public
  //////////////////////////////////////////////////////////////////////////

  KnuthBendix& KnuthBendix::overlap_policy(options::overlap p) {
    if (p == _settings._overlap_policy && _overlap_measure != nullptr) {
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
    _settings._overlap_policy = p;
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - constructors and destructor - public
  //////////////////////////////////////////////////////////////////////////

  KnuthBendix::KnuthBendix(congruence_kind knd)
      : CongruenceInterface(knd),
        _settings(),  // TODO init all mems
        _active_rules(),
        _gen_pairs_initted(),
        _gilman_digraph(),
        _inactive_rules(),
        _presentation(),
        _set_rules(),
        _stack() {
    init(knd);
  }

  KnuthBendix& KnuthBendix::init(congruence_kind knd) {
    deactivate_all_rules();

    CongruenceInterface::init(knd);
    _settings.init();
    _gen_pairs_initted = false;
    _gilman_digraph.init(0, 0);
    _confluent                    = false;
    _confluence_known             = false;
    _internal_is_same_as_external = false;
    _min_length_lhs_rule          = std::numeric_limits<size_t>::max();
    _overlap_measure              = nullptr;
    _presentation.clear();
    _total_rules = 0;
    overlap_policy(_settings._overlap_policy);
#ifdef LIBSEMIGROUPS_VERBOSE
    _max_stack_depth        = 0;
    _max_word_length        = 0;
    _max_active_word_length = 0;
    _max_active_rules       = 0;
#endif
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
    deactivate_all_rules();

    for (Rule const* rule : that._active_rules) {
      add_rule(new_rule(rule));
    }
    _next_rule_it1 = _active_rules.begin();
    std::advance(
        _next_rule_it1,
        std::distance(that._active_rules.begin(),
                      static_cast<std::list<Rule const*>::const_iterator>(
                          that._next_rule_it1)));
    _next_rule_it2 = _active_rules.begin();
    std::advance(
        _next_rule_it2,
        std::distance(that._active_rules.begin(),
                      static_cast<std::list<Rule const*>::const_iterator>(
                          that._next_rule_it2)));
    // Don't copy the inactive rules, because why bother

    _settings                     = that._settings;
    _confluent                    = that._confluent.load();
    _confluence_known             = that._confluence_known.load();
    _gilman_digraph               = that._gilman_digraph;
    _internal_is_same_as_external = that._internal_is_same_as_external;
    _min_length_lhs_rule          = that._min_length_lhs_rule;
    _presentation                 = that._presentation;
    _total_rules                  = that._total_rules;

    overlap_policy(_settings._overlap_policy);

    return *this;
  }

  KnuthBendix::~KnuthBendix() {
    delete _overlap_measure;
    for (Rule const* rule : _active_rules) {
      delete const_cast<Rule*>(rule);
    }
    for (Rule* rule : _inactive_rules) {
      delete rule;
    }
    while (!_stack.empty()) {
      Rule* rule = _stack.top();
      _stack.pop();
      delete rule;
    }
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
    add_rules_from_presentation();
    return *this;
  }

  // TODO reduce code dupl with prev func
  KnuthBendix& KnuthBendix::private_init(congruence_kind             knd,
                                         Presentation<std::string>&& p,
                                         bool call_init) {
    p.validate();
    if (call_init) {
      init(knd);
    }
    _presentation = std::move(p);
    add_rules_from_presentation();
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
    internal_rewrite(uu);
    internal_rewrite(vv);
    return uu == vv;
  }

  std::string KnuthBendix::normal_form(std::string const& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    run();
    return rewrite(w);
  }

  void KnuthBendix::rewrite_inplace(external_string_type& w) const {
    if (kind() == congruence_kind::left) {
      std::reverse(w.begin(), w.end());
    }
    add_octo(w);
    external_to_internal_string(w);
    internal_rewrite(w);
    internal_to_external_string(w);
    rm_octo(w);
    if (kind() == congruence_kind::left) {
      std::reverse(w.begin(), w.end());
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - other methods - private
  //////////////////////////////////////////////////////////////////////////

  // REWRITE_FROM_LEFT from Sims, p67
  // Caution: this uses the assumption that rules are length reducing, if it
  // is not, then u might not have sufficient space!
  void KnuthBendix::internal_rewrite(internal_string_type& u) const {
    using iterator = internal_string_type::iterator;

    if (u.size() < _min_length_lhs_rule) {
      return;
    }

    iterator v_begin = u.begin();
    iterator v_end   = u.begin() + _min_length_lhs_rule - 1;
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
             && _min_length_lhs_rule - 1
                    > static_cast<size_t>((v_end - v_begin))) {
        *v_end = *w_begin;
        ++v_end;
        ++w_begin;
      }
    }
    u.erase(v_end - u.cbegin());
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - main methods - public
  //////////////////////////////////////////////////////////////////////////

  bool KnuthBendix::confluent_known() const noexcept {
    return _confluence_known;
  }

  bool KnuthBendix::confluent() const {
    if (!_stack.empty()) {
      return false;
    }
    if (!_confluence_known && (!running() || !stopped()) && !dead()) {
      LIBSEMIGROUPS_ASSERT(_stack.empty());
      _confluent        = true;
      _confluence_known = true;
      internal_string_type word1;
      internal_string_type word2;
      size_t               seen = 0;

      for (auto it1 = _active_rules.cbegin();
           it1 != _active_rules.cend() && (!running() || !stopped()) && !dead();
           ++it1) {
        Rule const* rule1 = *it1;
        // Seems to be much faster to do this in reverse.
        for (auto it2 = _active_rules.crbegin();
             it2 != _active_rules.crend() && (!running() || !stopped())
             && !dead();
             ++it2) {
          seen++;
          Rule const* rule2 = *it2;
          for (auto it = rule1->lhs()->cend() - 1;
               it >= rule1->lhs()->cbegin() && (!running() || !stopped())
               && !dead();
               --it) {
            // Find longest common prefix of suffix B of rule1.lhs() defined
            // by it and R = rule2.lhs()
            auto prefix = detail::maximum_common_prefix(it,
                                                        rule1->lhs()->cend(),
                                                        rule2->lhs()->cbegin(),
                                                        rule2->lhs()->cend());
            if (prefix.first == rule1->lhs()->cend()
                || prefix.second == rule2->lhs()->cend()) {
              word1.clear();
              word1.append(rule1->lhs()->cbegin(), it);          // A
              word1.append(*rule2->rhs());                       // S
              word1.append(prefix.first, rule1->lhs()->cend());  // D

              word2.clear();
              word2.append(*rule1->rhs());                        // Q
              word2.append(prefix.second, rule2->lhs()->cend());  // E

              if (word1 != word2) {
                internal_rewrite(word1);
                internal_rewrite(word2);
                if (word1 != word2) {
                  _confluent = false;
                  return _confluent;
                }
              }
            }
          }
        }
        if (report()) {
          REPORT_DEFAULT("checked %llu pairs of overlaps out of %llu\n",
                         uint64_t(seen),
                         uint64_t(_active_rules.size())
                             * uint64_t(_active_rules.size()));
        }
      }
      if (running() && stopped()) {
        _confluence_known = false;
      }
    }
    return _confluent;
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
    detail::Timer timer;
    init_from_generating_pairs();
    if (_stack.empty() && confluent() && !stopped()) {
      // _stack can be non-empty if non-reduced rules were used to define
      // the KnuthBendix.  If _stack is non-empty, then it means that the
      // rules in _active_rules might not define the system.
      REPORT_DEFAULT("the system is confluent already\n");
      return;
    } else if (_active_rules.size() >= _settings._max_rules) {
      REPORT_DEFAULT("too many rules\n");
      return;
    }
    // Reduce the rules
    _next_rule_it1 = _active_rules.begin();
    while (_next_rule_it1 != _active_rules.end() && !stopped()) {
      // Copy *_next_rule_it1 and push_stack so that it is not modified by
      // the call to clear_stack.
      LIBSEMIGROUPS_ASSERT((*_next_rule_it1)->lhs()
                           != (*_next_rule_it1)->rhs());
      push_stack(new_rule(*_next_rule_it1));
      ++_next_rule_it1;
    }
    _next_rule_it1 = _active_rules.begin();
    size_t nr      = 0;
    while (_next_rule_it1 != _active_rules.cend()
           && _active_rules.size() < _settings._max_rules && !stopped()) {
      Rule const* rule1 = *_next_rule_it1;
      _next_rule_it2    = _next_rule_it1;
      ++_next_rule_it1;
      overlap(rule1, rule1);
      while (_next_rule_it2 != _active_rules.begin() && rule1->active()) {
        --_next_rule_it2;
        Rule const* rule2 = *_next_rule_it2;
        overlap(rule1, rule2);
        ++nr;
        if (rule1->active() && rule2->active()) {
          ++nr;
          overlap(rule2, rule1);
        }
      }
      if (nr > _settings._check_confluence_interval) {
        if (confluent()) {
          break;
        }
        nr = 0;
      }
      if (_next_rule_it1 == _active_rules.cend()) {
        clear_stack();
      }
    }
    // LIBSEMIGROUPS_ASSERT(_stack.empty());
    // Seems that the stack can be non-empty here in KnuthBendix 12, 14, 16
    // and maybe more
    if (_settings._max_overlap == POSITIVE_INFINITY
        && _settings._max_rules == POSITIVE_INFINITY && !stopped()) {
      _confluence_known = true;
      _confluent        = true;
      for (Rule* rule : _inactive_rules) {
        delete rule;
      }
      _inactive_rules.clear();
    }

    REPORT_DEFAULT("stopping with active rules = %d, inactive rules = %d, "
                   "rules defined = %d\n",
                   _active_rules.size(),
                   _inactive_rules.size(),
                   _total_rules);
    REPORT_VERBOSE_DEFAULT("max stack depth = %d", _max_stack_depth);
    REPORT_TIME(timer);
  }

  void KnuthBendix::knuth_bendix_by_overlap_length() {
    detail::Timer timer;
    size_t        max_overlap            = _settings._max_overlap;
    size_t check_confluence_interval     = _settings._check_confluence_interval;
    _settings._max_overlap               = 1;
    _settings._check_confluence_interval = POSITIVE_INFINITY;
    while (!confluent()) {
      run();
      _settings._max_overlap++;
    }
    _settings._max_overlap               = max_overlap;
    _settings._check_confluence_interval = check_confluence_interval;
    REPORT_TIME(timer);
    report_why_we_stopped();
  }

  size_t KnuthBendix::number_of_active_rules() const noexcept {
    return _active_rules.size();
  }

  WordGraph<size_t> const& KnuthBendix::gilman_digraph() {
    if (_gilman_digraph.number_of_nodes() == 0
        && !presentation().alphabet().empty()) {
      // reset the settings so that we really run!
      max_rules(POSITIVE_INFINITY);
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      LIBSEMIGROUPS_ASSERT(confluent());
      std::unordered_map<std::string, size_t> prefixes;
      prefixes.emplace("", 0);
      auto   rules = _active_rules;
      size_t n     = 1;
      for (auto const* rule : rules) {
        prefixes_string(prefixes, *rule->lhs(), n);
      }

      // TODO implement these as gilman_digraph_node_labels or something
      // std::vector<std::string> tmp(prefixes.size(), "");
      // for (auto const& p : prefixes) {
      //   tmp[p.second] = p.first;
      // }
      // fmt::print(detail::to_string(tmp));

      _gilman_digraph.add_nodes(prefixes.size());
      _gilman_digraph.add_to_out_degree(presentation().alphabet().size());

      for (auto& p : prefixes) {
        for (size_t i = 0; i < presentation().alphabet().size(); ++i) {
          auto s  = p.first + uint_to_internal_string(i);
          auto it = prefixes.find(s);
          if (it != prefixes.end()) {
            _gilman_digraph.set_target(p.second, i, it->second);
          } else {
            auto t = s;
            internal_rewrite(t);
            if (t == s) {
              while (!s.empty()) {
                s  = std::string(s.begin() + 1, s.end());
                it = prefixes.find(s);
                if (it != prefixes.end()) {
                  _gilman_digraph.set_target(p.second, i, it->second);
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
        auto        src  = _gilman_digraph.target_no_checks(0, octo);
        LIBSEMIGROUPS_ASSERT(src != UNDEFINED);
        _gilman_digraph.remove_label_no_checks(octo);
        auto nodes = word_graph::nodes_reachable_from(_gilman_digraph, src);
        LIBSEMIGROUPS_ASSERT(std::find(nodes.cbegin(), nodes.cend(), src)
                             != nodes.cend());
        // TODO this is a bit awkward, it ensures that node 0 in the induced
        // subdigraph is src.
        std::vector<decltype(src)> sorted_nodes(nodes.cbegin(), nodes.cend());
        if (sorted_nodes[0] != src) {
          std::iter_swap(
              sorted_nodes.begin(),
              std::find(sorted_nodes.begin(), sorted_nodes.end(), src));
        }

        _gilman_digraph.induced_subgraph_no_checks(sorted_nodes.cbegin(),
                                                   sorted_nodes.cend());
      }
    }
    return _gilman_digraph;
  }

  //////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - pure virtual methods - private
  //////////////////////////////////////////////////////////////////////////

  void KnuthBendix::add_rule_impl(std::string const& p, std::string const& q) {
    if (p == q) {
      return;
    }
    auto pp = new external_string_type(p);
    auto qq = new external_string_type(q);
    external_to_internal_string(*pp);
    external_to_internal_string(*qq);
    push_stack(new_rule(pp, qq));
  }

  void KnuthBendix::add_rule(Rule* rule) {
    LIBSEMIGROUPS_ASSERT(*rule->lhs() != *rule->rhs());
#ifdef LIBSEMIGROUPS_VERBOSE
    _max_word_length  = std::max(_max_word_length, rule->lhs()->size());
    _max_active_rules = std::max(_max_active_rules, _active_rules.size());
    _unique_lhs_rules.insert(*rule->lhs());
#endif
    LIBSEMIGROUPS_ASSERT(_set_rules.emplace(RuleLookup(rule)).second);
#ifndef LIBSEMIGROUPS_DEBUG
    _set_rules.emplace(RuleLookup(rule));
#endif
    rule->activate();
    _active_rules.push_back(rule);
    if (_next_rule_it1 == _active_rules.end()) {
      --_next_rule_it1;
    }
    if (_next_rule_it2 == _active_rules.end()) {
      --_next_rule_it2;
    }
    _confluence_known = false;
    if (rule->lhs()->size() < _min_length_lhs_rule) {
      // TODO(later) this is not valid when using non-length reducing
      // orderings (such as RECURSIVE)
      _min_length_lhs_rule = rule->lhs()->size();
    }
    LIBSEMIGROUPS_ASSERT(_set_rules.size() == _active_rules.size());
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
    return presentation().letter(internal_char_to_uint(a));
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

  // TODO rename init_from_presentation
  void KnuthBendix::add_rules_from_presentation() {
    auto const& p                 = _presentation;
    _internal_is_same_as_external = true;
    for (size_t i = 0; i < p.alphabet().size(); ++i) {
      if (uint_to_internal_char(i) != p.letter(i)) {
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

  KnuthBendix::Rule* KnuthBendix::new_rule() const {
    ++_total_rules;
    Rule* rule;
    if (!_inactive_rules.empty()) {
      rule = _inactive_rules.front();
      rule->clear();
      rule->set_id(_total_rules);
      _inactive_rules.erase(_inactive_rules.begin());
    } else {
      rule = new Rule(this, _total_rules);
    }
    LIBSEMIGROUPS_ASSERT(!rule->active());
    return rule;
  }

  KnuthBendix::Rule* KnuthBendix::new_rule(internal_string_type* lhs,
                                           internal_string_type* rhs) const {
    Rule* rule = new_rule();
    delete rule->_lhs;
    delete rule->_rhs;
    if (shortlex_compare(rhs, lhs)) {
      rule->_lhs = lhs;
      rule->_rhs = rhs;
    } else {
      rule->_lhs = rhs;
      rule->_rhs = lhs;
    }
    return rule;
  }

  KnuthBendix::Rule* KnuthBendix::new_rule(Rule const* rule1) const {
    Rule* rule2 = new_rule();
    rule2->_lhs->append(*rule1->lhs());  // copies lhs
    rule2->_rhs->append(*rule1->rhs());  // copies rhs
    return rule2;
  }

  KnuthBendix::Rule*
  KnuthBendix::new_rule(internal_string_type::const_iterator begin_lhs,
                        internal_string_type::const_iterator end_lhs,
                        internal_string_type::const_iterator begin_rhs,
                        internal_string_type::const_iterator end_rhs) const {
    Rule* rule = new_rule();
    rule->_lhs->append(begin_lhs, end_lhs);
    rule->_rhs->append(begin_rhs, end_rhs);
    return rule;
  }

  // FIXME(later) there is a possibly infinite loop here clear_stack ->
  // push_stack -> clear_stack and so on
  void KnuthBendix::push_stack(Rule* rule) {
    LIBSEMIGROUPS_ASSERT(!rule->active());
    if (*rule->lhs() != *rule->rhs()) {
      _stack.emplace(rule);
      clear_stack();
    } else {
      _inactive_rules.push_back(rule);
    }
  }

  // OVERLAP_2 from Sims, p77
  void KnuthBendix::overlap(Rule const* u, Rule const* v) {
    LIBSEMIGROUPS_ASSERT(u->active() && v->active());
    auto limit
        = u->lhs()->cend() - std::min(u->lhs()->size(), v->lhs()->size());
    int64_t u_id = u->id();
    int64_t v_id = v->id();
    for (auto it = u->lhs()->cend() - 1;
         it > limit && u_id == u->id() && v_id == v->id() && !stopped()
         && (_settings._max_overlap == POSITIVE_INFINITY
             || (*_overlap_measure)(u, v, it) <= _settings._max_overlap);
         --it) {
      // Check if B = [it, u->lhs()->cend()) is a prefix of v->lhs()
      if (detail::is_prefix(
              v->lhs()->cbegin(), v->lhs()->cend(), it, u->lhs()->cend())) {
        // u = P_i = AB -> Q_i and v = P_j = BC -> Q_j
        // This version of new_rule does not reorder
        Rule* rule = new_rule(u->lhs()->cbegin(),
                              it,
                              u->rhs()->cbegin(),
                              u->rhs()->cend());  // rule = A -> Q_i
        rule->_lhs->append(*v->rhs());            // rule = AQ_j -> Q_i
        rule->_rhs->append(v->lhs()->cbegin() + (u->lhs()->cend() - it),
                           v->lhs()->cend());  // rule = AQ_j -> Q_iC
        // rule is reordered during rewriting in clear_stack
        push_stack(rule);
        // It can be that the iterator `it` is invalidated by the call to
        // push_stack (i.e. if `u` is deactivated, then rewritten, actually
        // changed, and reactivated) and that is the reason for the checks
        // in the for-loop above. If this is the case, then we should stop
        // considering the overlaps of u and v here, and note that they will
        // be considered later, because when the rule `u` is reactivated it
        // is added to the end of the active rules list.
      }
    }
  }

  // TEST_2 from Sims, p76
  void KnuthBendix::clear_stack() {
    while (!_stack.empty() && !stopped()) {
#ifdef LIBSEMIGROUPS_VERBOSE
      _max_stack_depth = std::max(_max_stack_depth, _stack.size());
#endif

      Rule* rule1 = _stack.top();
      _stack.pop();
      LIBSEMIGROUPS_ASSERT(!rule1->active());
      LIBSEMIGROUPS_ASSERT(*rule1->lhs() != *rule1->rhs());
      // Rewrite both sides and reorder if necessary . . .
      rule1->rewrite();

      if (*rule1->lhs() != *rule1->rhs()) {
        internal_string_type const* lhs = rule1->lhs();
        for (auto it = _active_rules.begin(); it != _active_rules.end();) {
          Rule* rule2 = const_cast<Rule*>(*it);
          if (rule2->lhs()->find(*lhs) != external_string_type::npos) {
            it = remove_rule(it);
            LIBSEMIGROUPS_ASSERT(*rule2->lhs() != *rule2->rhs());
            // rule2 is added to _inactive_rules by clear_stack
            _stack.emplace(rule2);
          } else {
            if (rule2->rhs()->find(*lhs) != external_string_type::npos) {
              internal_rewrite(*rule2->rhs());
            }
            ++it;
          }
        }
        add_rule(rule1);
        // rule1 is activated, we do this after removing rules that rule1
        // makes redundant to avoid failing to insert rule1 in _set_rules
      } else {
        _inactive_rules.push_back(rule1);
      }
      if (report()) {
        REPORT_DEFAULT(
            "active rules = %d, inactive rules = %d, rules defined = "
            "%d\n",
            _active_rules.size(),
            _inactive_rules.size(),
            _total_rules);
        REPORT_VERBOSE_DEFAULT("max stack depth        = %d\n"
                               "max word length        = %d\n"
                               "max active word length = %d\n"
                               "max active rules       = %d\n"
                               "number of unique lhs   = %d\n",
                               _max_stack_depth,
                               _max_word_length,
                               max_active_word_length(),
                               _max_active_rules,
                               _unique_lhs_rules.size());
      }
    }
  }

  std::list<KnuthBendix::Rule const*>::iterator
  KnuthBendix::remove_rule(std::list<Rule const*>::iterator it) {
#ifdef LIBSEMIGROUPS_VERBOSE
    _unique_lhs_rules.erase(*((*it)->lhs()));
#endif
    Rule* rule = const_cast<Rule*>(*it);
    rule->deactivate();
    if (it != _next_rule_it1 && it != _next_rule_it2) {
      it = _active_rules.erase(it);
    } else if (it == _next_rule_it1 && it != _next_rule_it2) {
      _next_rule_it1 = _active_rules.erase(it);
      it             = _next_rule_it1;
    } else if (it != _next_rule_it1 && it == _next_rule_it2) {
      _next_rule_it2 = _active_rules.erase(it);
      it             = _next_rule_it2;
    } else {
      _next_rule_it1 = _active_rules.erase(it);
      _next_rule_it2 = _next_rule_it1;
      it             = _next_rule_it1;
    }
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(_set_rules.erase(RuleLookup(rule)));
#else
    _set_rules.erase(RuleLookup(rule));
#endif
    LIBSEMIGROUPS_ASSERT(_set_rules.size() == _active_rules.size());
    return it;
  }

  void KnuthBendix::deactivate_all_rules() {
    // Put all active rules and those rules in the stack into the
    // inactive_rules list
    for (Rule const* cptr : _active_rules) {
      Rule* ptr = const_cast<Rule*>(cptr);
      ptr->deactivate();
      _inactive_rules.insert(_inactive_rules.end(), ptr);
    }
    _active_rules.clear();
    while (!_stack.empty()) {
      _inactive_rules.insert(_inactive_rules.end(), _stack.top());
      _stack.pop();
    }
    _next_rule_it1 = _active_rules.end();
    _next_rule_it2 = _active_rules.end();
    _set_rules.clear();
  }

#ifdef LIBSEMIGROUPS_VERBOSE
  size_t KnuthBendix::max_active_word_length() {
    auto comp = [](Rule const* p, Rule const* q) -> bool {
      return p->lhs()->size() < q->lhs()->size();
    };
    auto max
        = std::max_element(_active_rules.cbegin(), _active_rules.cend(), comp);
    if (max != _active_rules.cend()) {
      _max_active_word_length
          = std::max(_max_active_word_length, (*max)->lhs()->size());
    }
    return _max_active_word_length;
  }
#endif
  namespace knuth_bendix {

    // We are computing non_trivial_classes with respect to kb2 (the greater
    // congruence, with fewer classes)
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
        // It might be possible to handle this case too, but doesn't seem
        // worth it at present
        LIBSEMIGROUPS_EXCEPTION("the arguments must have presentations with "
                                   "the same alphabets, found {} and {}",
                                   kb2.presentation().alphabet(),
                                   kb1.presentation().alphabet());
      }

      // We construct the WordGraph `ad` obtained by subtracting all of
      // the edges from the Gilman graph of kb1 from the Gilman graph of kb2.
      // The non-trivial classes are finite if and only if `ad` is acyclic. It
      // would be possible to do this without actually constructing `ad` but
      // constructing `ad` is simpler, and so we do that for now.

      auto g2 = kb2.gilman_digraph();
      auto g1 = kb1.gilman_digraph();

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

      // We need to obtain a mappings from the nodes of g2 to g1 and vice
      // versa.

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

      // We do a depth first search simultaneously for cycles, and edges E
      // in g2 not in g1. Pre order forcycle detection, post order for "can we
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
            // TODO clean up
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
          stck.push(v + N);  // so we can tell when all of the descendants of
                             // v have been processed out of the stack
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
      // here instead (but these don't yet exist) TODO
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
      // We only want those paths that pass through at least one of the
      // edges in g2 but not g1. Hence we require the `filter` in the next
      // expression.
      auto ntc
          = partition(kb1,
                      (paths.from(0) | rx::filter([&g1](word_type const& path) {
                         return word_graph::last_node_on_path(
                                    g1, 0, path.cbegin(), path.cend())
                                    .second
                                != path.cend();
                       })
                       | to_strings(kb2.presentation().alphabet())));
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
  }  // namespace knuth_bendix

  std::ostream& operator<<(std::ostream& os, KnuthBendix const& kb) {
    os << kb.active_rules();
    return os;
  }
}  // namespace libsemigroups
