//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#include <cstddef>  // for size_t
#include <string>   // for string

#include "libsemigroups/cong-intf.hpp"          // for CongruenceInterface
#include "libsemigroups/debug.hpp"              // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/word-graph.hpp"            // for WordGraph
#include "libsemigroups/exception.hpp"          // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/fpsemi-intf.hpp"        // for FpSemigroupInterface
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"       // for FroidurePin
#include "libsemigroups/kbe.hpp"                // for detail::KBE
#include "libsemigroups/knuth-bendix.hpp"       // for KnuthBendix, KnuthBe...
#include "libsemigroups/obvinf.hpp"             // for IsObviouslyInfinitePairs
#include "libsemigroups/types.hpp"              // for word_type

// Include kbe-impl.hpp after knuth-bendix-impl.hpp since detail::KBE depends on
// KnuthBendixImpl.
#include "kbe-impl.hpp"

namespace libsemigroups {
  using froidure_pin_type
      = FroidurePin<detail::KBE,
                    FroidurePinTraits<detail::KBE, fpsemigroup::KnuthBendix>>;

  namespace {
    bool is_subword(std::string const& x, std::string const& y) {
      return y.find(x) != std::string::npos;
    }

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

  namespace fpsemigroup {
    // KnuthBendix::KnuthBendix(KnuthBendix&&) = default;

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix::Settings - constructor - public
    //////////////////////////////////////////////////////////////////////////

    KnuthBendix::Settings::Settings()
        : _check_confluence_interval(4096),
          _max_overlap(POSITIVE_INFINITY),
          _max_rules(POSITIVE_INFINITY),
          _overlap_policy(options::overlap::ABC) {}

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

    KnuthBendix::KnuthBendix()
        : FpSemigroupInterface(),
          _gilman_digraph(),
          _active_rules(),
          _confluent(false),
          _confluence_known(false),
          _inactive_rules(),
          _internal_is_same_as_external(false),
          _contains_empty_string(false),
          _min_length_lhs_rule(std::numeric_limits<size_t>::max()),
          _overlap_measure(nullptr),
          _stack(),
          _tmp_word1(new internal_string_type()),
          _tmp_word2(new internal_string_type()),
          _total_rules(0) {
      _next_rule_it1 = _active_rules.end();  // null
      _next_rule_it2 = _active_rules.end();  // null
      overlap_policy(options::overlap::ABC);
#ifdef LIBSEMIGROUPS_VERBOSE
      _max_stack_depth        = 0;
      _max_word_length        = 0;
      _max_active_word_length = 0;
      _max_active_rules       = 0;
#endif
    }

    KnuthBendix::KnuthBendix(KnuthBendix const& kb) : KnuthBendix() {
      init_from(kb);
    }

    KnuthBendix::~KnuthBendix() {
      delete _overlap_measure;
      delete _tmp_word1;
      delete _tmp_word2;
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

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - initialisers - private
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::init_from(FroidurePinBase& S) {
      if (S.number_of_generators() != 0) {
        if (alphabet().empty()) {
          set_alphabet(S.number_of_generators());
        }
        // throws if rules contain letters that are not in the alphabet.
        add_rules(S);
      }
      // Do not set froidure_pin so we are guaranteed that it
      // returns a FroidurePin of detail::KBE's.
    }

    void KnuthBendix::init_from(KnuthBendix const& kb, bool add) {
      // TODO(later)   set confluence if known? Other things?
      if (!kb.alphabet().empty()) {
        if (alphabet().empty()) {
          set_alphabet(kb.alphabet());
        }
        // throws if rules contain letters that are not in the alphabet.
        if (add) {
          add_rules(kb.active_rules());
        }
      }
      // TODO(later) copy other settings
      _settings._overlap_policy = kb._settings._overlap_policy;
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////

    KnuthBendix::const_normal_form_iterator
    KnuthBendix::cbegin_normal_forms(std::string const& lphbt,
                                     size_t const       min,
                                     size_t const       max) {
      using state_type = NormalFormsIteratorTraits::state_type;
      auto it          = const_normal_form_iterator(
          state_type(lphbt, ""), cbegin_pislo(gilman_digraph(), 0, min, max));
      if (min == 0 && !contains_empty_string()) {
        ++it;
      }
      return it;
    }

    uint64_t KnuthBendix::size() {
      if (is_obviously_infinite()) {
        return POSITIVE_INFINITY;
      } else if (alphabet().empty()) {
        return 0;
      } else {
        int const  modifier = (contains_empty_string() ? 0 : -1);
        auto const out      = number_of_paths(gilman_digraph(), 0);
        return (out == POSITIVE_INFINITY ? out : out + modifier);
      }
    }

    bool KnuthBendix::equal_to(std::string const& u, std::string const& v) {
      validate_word(u);
      validate_word(v);
      if (u == v) {
        return true;
      }
      external_string_type uu = rewrite(u);
      external_string_type vv = rewrite(v);
      if (uu == vv) {
        return true;
      }
      run();
      external_to_internal_string(uu);
      external_to_internal_string(vv);
      internal_rewrite(&uu);
      internal_rewrite(&vv);
      return uu == vv;
    }

    std::string KnuthBendix::normal_form(std::string const& w) {
      validate_word(w);
      run();
      return rewrite(w);
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix public methods for rules and rewriting
    //////////////////////////////////////////////////////////////////////////

    std::vector<std::pair<std::string, std::string>>
    KnuthBendix::active_rules() const {
      std::vector<std::pair<external_string_type, external_string_type>> rules;
      rules.reserve(_active_rules.size());
      for (Rule const* rule : _active_rules) {
        internal_string_type lhs = internal_string_type(*rule->lhs());
        internal_string_type rhs = internal_string_type(*rule->rhs());
        internal_to_external_string(lhs);
        internal_to_external_string(rhs);
        rules.emplace_back(lhs, rhs);
      }
      std::sort(
          rules.begin(),
          rules.end(),
          [](std::pair<external_string_type, external_string_type> rule1,
             std::pair<external_string_type, external_string_type> rule2) {
            return shortlex_compare(rule1.first, rule2.first)
                   || (rule1.first == rule2.first
                       && shortlex_compare(rule1.second, rule2.second));
          });
      return rules;
    }

    KnuthBendix::external_string_type*
    KnuthBendix::rewrite(external_string_type* w) const {
      external_to_internal_string(*w);
      internal_rewrite(w);
      internal_to_external_string(*w);
      return w;
    }
    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixImpl - other methods - private
    //////////////////////////////////////////////////////////////////////////

    // REWRITE_FROM_LEFT from Sims, p67
    // Caution: this uses the assumption that rules are length reducing, if it
    // is not, then u might not have sufficient space!
    void KnuthBendix::internal_rewrite(internal_string_type* u) const {
      if (u->size() < _min_length_lhs_rule) {
        return;
      }
      internal_string_type::iterator const& v_begin = u->begin();
      internal_string_type::iterator        v_end
          = u->begin() + _min_length_lhs_rule - 1;
      internal_string_type::iterator        w_begin = v_end;
      internal_string_type::iterator const& w_end   = u->end();

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
      u->erase(v_end - u->cbegin());
    }

    std::ostream& operator<<(std::ostream& os, KnuthBendix const& kb) {
      os << detail::to_string(kb.active_rules()) << "\n";
      return os;
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
      if (!_confluence_known && (!running() || !stopped())) {
        LIBSEMIGROUPS_ASSERT(_stack.empty());
        _confluent        = true;
        _confluence_known = true;
        internal_string_type word1;
        internal_string_type word2;
        size_t               seen = 0;

        for (auto it1 = _active_rules.cbegin();
             it1 != _active_rules.cend() && (!running() || !stopped());
             ++it1) {
          Rule const* rule1 = *it1;
          // Seems to be much faster to do this in reverse.
          for (auto it2 = _active_rules.crbegin();
               it2 != _active_rules.crend() && (!running() || !stopped());
               ++it2) {
            seen++;
            Rule const* rule2 = *it2;
            for (auto it = rule1->lhs()->cend() - 1;
                 it >= rule1->lhs()->cbegin() && (!running() || !stopped());
                 --it) {
              // Find longest common prefix of suffix B of rule1.lhs() defined
              // by it and R = rule2.lhs()
              auto prefix
                  = detail::maximum_common_prefix(it,
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
                  internal_rewrite(&word1);
                  internal_rewrite(&word2);
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

    void KnuthBendix::run_impl() {
      detail::Timer timer;
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
      size_t        max_overlap        = _settings._max_overlap;
      size_t check_confluence_interval = _settings._check_confluence_interval;
      _settings._max_overlap           = 1;
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

    // TODO(later) noexcept?
    bool KnuthBendix::contains_empty_string() const {
      return _contains_empty_string || (has_identity() && identity().empty());
    }

    uint64_t KnuthBendix::number_of_normal_forms(size_t min, size_t max) {
      if (alphabet().empty()) {
        return 0;
      } else {
        int const modifier = (contains_empty_string() ? 0 : -1);

        auto const out = number_of_paths(gilman_digraph(), 0, min, max);
        return (out == POSITIVE_INFINITY ? out : out + modifier);
      }
    }

    size_t KnuthBendix::number_of_active_rules() const noexcept {
      return _active_rules.size();
    }

    WordGraph<size_t> const& KnuthBendix::gilman_digraph() {
      if (_gilman_digraph.number_of_nodes() == 0 && !alphabet().empty()) {
        // reset the settings so that we really run!
        max_rules(POSITIVE_INFINITY);
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        LIBSEMIGROUPS_ASSERT(confluent());
        std::unordered_map<std::string, size_t> prefixes;
        prefixes.emplace("", 0);
        // FIXME(later)
        auto rules = active_rules();

        size_t n = 1;
        for (auto const& rule : rules) {
          prefixes_string(prefixes, rule.first, n);
        }

        _gilman_digraph.add_nodes(prefixes.size());
        _gilman_digraph.add_to_out_degree(alphabet().size());

        for (auto& p : prefixes) {
          for (size_t i = 0; i < alphabet().size(); ++i) {
            auto s  = p.first + alphabet()[i];
            auto it = prefixes.find(s);
            if (it != prefixes.end()) {
              _gilman_digraph.add_edge(p.second, it->second, i);
            } else {
              if (!std::any_of(
                      rules.cbegin(),
                      rules.cend(),
                      [&s](std::pair<std::string, std::string> const& rule) {
                        return is_subword(rule.first, s);
                      })) {
                while (!s.empty()) {
                  s  = std::string(s.begin() + 1, s.end());
                  it = prefixes.find(s);
                  if (it != prefixes.end()) {
                    _gilman_digraph.add_edge(p.second, it->second, i);
                    break;
                  }
                }
              }
            }
          }
        }
      }
      return _gilman_digraph;
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual methods - private
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::add_rule_impl(std::string const& p,
                                    std::string const& q) {
      LIBSEMIGROUPS_ASSERT(p != q);
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
      if (!_contains_empty_string) {
        _contains_empty_string = rule->lhs()->empty() || rule->rhs()->empty();
      }
      LIBSEMIGROUPS_ASSERT(_set_rules.size() == _active_rules.size());
    }

    std::shared_ptr<FroidurePinBase> KnuthBendix::froidure_pin_impl() {
      LIBSEMIGROUPS_ASSERT(!alphabet().empty());
      run();
      auto ptr = std::make_shared<froidure_pin_type>(*this);
      for (size_t i = 0; i < alphabet().size(); ++i) {
        ptr->add_generator(detail::KBE(*this, i));
      }
      return ptr;
    }

    bool KnuthBendix::is_obviously_infinite_impl() {
      if (finished()) {
        return !word_graph::is_acyclic(gilman_digraph());
      } else if (alphabet().size() > number_of_rules()) {
        return true;
      }
      detail::IsObviouslyInfinite ioi(alphabet().size());
      ioi.add_rules(alphabet(), cbegin_rules(), cend_rules());
      return ioi.result();
    }

    bool KnuthBendix::is_obviously_finite_impl() {
      if (finished()) {
        return word_graph::is_acyclic(gilman_digraph());
      } else if (has_froidure_pin() && froidure_pin()->finished()) {
        return true;
      }
      return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-pure virtual methods - private
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::set_alphabet_impl(std::string const& lphbt) {
      _internal_is_same_as_external = true;
      for (size_t i = 0; i < lphbt.size(); ++i) {
        if (uint_to_internal_char(i) != lphbt[i]) {
          _internal_is_same_as_external = false;
          return;
        }
      }
    }

    void KnuthBendix::set_alphabet_impl(size_t) {
      set_alphabet_impl("");
    }

    bool KnuthBendix::validate_identity_impl(std::string const& id) const {
      if (id.length() > 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid identity, found %d letters, should be 0 or 1 letters",
            id.length());
      }
      if (id.length() == 1) {
        validate_letter(id[0]);
        return true;  // Add rules for the identity
      } else {
        return false;  // Don't add rules for the identity
      }
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - main member functions - public
    //////////////////////////////////////////////////////////////////////////

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

    KnuthBendix::internal_char_type
    KnuthBendix::uint_to_internal_char(size_t a) {
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

    KnuthBendix::internal_string_type*
    KnuthBendix::word_to_internal_string(word_type const&      w,
                                         internal_string_type* ww) {
      ww->clear();
      for (size_t const& a : w) {
        (*ww) += uint_to_internal_char(a);
      }
      return ww;
    }

    KnuthBendix::internal_string_type
    KnuthBendix::word_to_internal_string(word_type const& u) {
      internal_string_type v;
      v.reserve(u.size());
      for (size_t const& l : u) {
        v += uint_to_internal_char(l);
      }
      return v;
    }

    KnuthBendix::internal_char_type
    KnuthBendix::external_to_internal_char(external_char_type c) const {
      LIBSEMIGROUPS_ASSERT(!_internal_is_same_as_external);
      return uint_to_internal_char(char_to_uint(c));
    }

    KnuthBendix::external_char_type
    KnuthBendix::internal_to_external_char(internal_char_type a) const {
      LIBSEMIGROUPS_ASSERT(!_internal_is_same_as_external);
      return uint_to_char(internal_char_to_uint(a));
    }

    void
    KnuthBendix::external_to_internal_string(external_string_type& w) const {
      if (_internal_is_same_as_external) {
        return;
      }
      for (auto& a : w) {
        a = external_to_internal_char(a);
      }
    }

    void
    KnuthBendix::internal_to_external_string(internal_string_type& w) const {
      if (_internal_is_same_as_external) {
        return;
      }
      for (auto& a : w) {
        a = internal_to_external_char(a);
      }
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixImpl - methods for rules - private
    //////////////////////////////////////////////////////////////////////////

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
                internal_rewrite(rule2->rhs());
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

  }  // namespace fpsemigroup

  namespace congruence {
    using class_index_type = CongruenceInterface::class_index_type;

    ////////////////////////////////////////////////////////////////////////////
    // KnuthBendix - constructors - public
    ////////////////////////////////////////////////////////////////////////////

    KnuthBendix::KnuthBendix()
        : CongruenceInterface(congruence_kind::twosided),
          _kb(std::make_unique<fpsemigroup::KnuthBendix>()) {}

    KnuthBendix::KnuthBendix(fpsemigroup::KnuthBendix const& kb)
        : KnuthBendix() {
      _kb->init_from(kb, false);  // false = don't add rules
      if (!_kb->alphabet().empty()) {
        set_number_of_generators(_kb->alphabet().size());
      }
      // TODO(later):
      // The following lines don't do anything because _kb does not get the
      // FroidurePin from kb. Could be kb->has_froidure_pin()?
      // if (_kb->has_froidure_pin()) {
      //  set_parent_froidure_pin(_kb->froidure_pin());
      // }
      for (auto const& r : kb.active_rules()) {
        add_pair(kb.string_to_word(r.first), kb.string_to_word(r.second));
      }
    }

    KnuthBendix::KnuthBendix(std::shared_ptr<FroidurePinBase> S)
        : KnuthBendix() {
      _kb->init_from(*S);
      set_number_of_generators(S->number_of_generators());
      set_parent_froidure_pin(S);
    }

    KnuthBendix::~KnuthBendix() = default;

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    tril KnuthBendix::const_contains(word_type const& lhs,
                                     word_type const& rhs) const {
      validate_word(lhs);
      validate_word(rhs);
      if (lhs == rhs) {
        return tril::TRUE;
      }
      std::string u, v;
      u = _kb->rewrite(_kb->word_to_string(lhs));
      v = _kb->rewrite(_kb->word_to_string(rhs));
      if (u == v) {
        return tril::TRUE;
      } else if (_kb->confluent()) {
        return tril::FALSE;
      } else {
        return tril::unknown;
      }
    }

    bool KnuthBendix::is_quotient_obviously_finite_impl() {
      return finished()
             || (has_parent_froidure_pin() && parent_froidure_pin()->finished())
             || (_kb->is_obviously_finite()
                 && number_of_generators() != UNDEFINED);
    }

    bool KnuthBendix::contains(word_type const& lhs, word_type const& rhs) {
      if (const_contains(lhs, rhs) == tril::TRUE) {
        return true;
      }
      _kb->run();
      return const_contains(lhs, rhs) == tril::TRUE;
    }

    bool KnuthBendix::is_quotient_obviously_infinite_impl() {
      return _kb->is_obviously_infinite();
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual methods - private
    ////////////////////////////////////////////////////////////////////////////

    word_type KnuthBendix::class_index_to_word_impl(class_index_type i) {
      return _kb->froidure_pin()->minimal_factorisation(i);
    }

    size_t KnuthBendix::number_of_classes_impl() {
      run();  // required so that the state of this is correctly set.
      return _kb->size();
    }

    std::shared_ptr<FroidurePinBase> KnuthBendix::quotient_impl() {
      return _kb->froidure_pin();
    }

    class_index_type
    KnuthBendix::word_to_class_index_impl(word_type const& word) {
      auto   S   = static_cast<froidure_pin_type*>(_kb->froidure_pin().get());
      size_t pos = S->position(detail::KBE(*_kb, word));
      LIBSEMIGROUPS_ASSERT(pos != UNDEFINED);
      return pos;
    }

    void KnuthBendix::run_impl() {
      auto stppd = [this]() -> bool { return stopped(); };
      _kb->run_until(stppd);
      report_why_we_stopped();
    }

    bool KnuthBendix::finished_impl() const {
      return _kb->finished();
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual methods - private
    ////////////////////////////////////////////////////////////////////////////

    void KnuthBendix::add_pair_impl(word_type const& u, word_type const& v) {
      _kb->add_rule(u, v);
    }

    void KnuthBendix::set_number_of_generators_impl(size_t n) {
      if (_kb->alphabet().empty()) {
        _kb->set_alphabet(n);
      }
    }
  }  // namespace congruence
}  // namespace libsemigroups
