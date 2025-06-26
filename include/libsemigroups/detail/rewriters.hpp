//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 Joseph Edwards + James D. Mitchell
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

// This file contains the implementation of a Rule object containers for Rule
// objects. It also includes rewriter classes that can be used to rewrite
// strings relative to a collection of rules.

#ifndef LIBSEMIGROUPS_DETAIL_REWRITERS_HPP_
#define LIBSEMIGROUPS_DETAIL_REWRITERS_HPP_

#include <atomic>         // for atomic
#include <chrono>         // for time_point
#include <set>            // for set
#include <string>         // for basic_string, operator==
#include <unordered_map>  // for unordered map
#include <unordered_set>  // for unordered set

#include "../debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "../order.hpp"  // for shortlex_compare

#include "aho-corasick-impl.hpp"  // for AhoCorasickImpl
#include "multi-string-view.hpp"  // for MultiStringView

// TODO(2) Add a KnuthBendixImpl pointer to the rewriter class so that overlap
// detection can be handled by the rewriter (and therefore depend on the
// implementation) rather than on the KB object.

namespace libsemigroups {
  namespace detail {
    // TODO(2) remove from libsemigroups namespace and put into relevant class

    ////////////////////////////////////////////////////////////////////////
    // Rule
    ////////////////////////////////////////////////////////////////////////

    class Rule {
      // TODO Why are _lhs and _rhs pointers?
      std::string* _lhs;
      std::string* _rhs;
      int64_t      _id;

     public:
      explicit Rule(int64_t id);

      Rule& operator=(Rule const& copy) = delete;
      Rule(Rule const& copy)            = delete;
      Rule(Rule&& copy)                 = delete;
      Rule& operator=(Rule&& copy)      = delete;

      ~Rule() {
        delete _lhs;
        delete _rhs;
      }

      [[nodiscard]] std::string* lhs() const noexcept {
        return _lhs;
      }

      [[nodiscard]] std::string* rhs() const noexcept {
        return _rhs;
      }

      [[nodiscard]] bool empty() const noexcept {
        return _lhs->empty() && _rhs->empty();
      }

      [[nodiscard]] inline bool active() const noexcept {
        LIBSEMIGROUPS_ASSERT(_id != 0);
        return (_id > 0);
      }

      void deactivate() noexcept;

      void activate() noexcept;

      void set_id_no_checks(int64_t id) noexcept {
        LIBSEMIGROUPS_ASSERT(id > 0);
        LIBSEMIGROUPS_ASSERT(!active());
        _id = -1 * id;
      }

      // TODO to cpp
      void set_id(int64_t id) {
        if (id <= 0) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid id, expected a value greater than 0, found {}", id);
        }
        if (active()) {
          LIBSEMIGROUPS_EXCEPTION("cannot set the id of an active rule");
        }
        set_id_no_checks(id);
      }

      [[nodiscard]] int64_t id() const noexcept {
        LIBSEMIGROUPS_ASSERT(_id != 0);
        return _id;
      }

      void reorder() {
        if (shortlex_compare(_lhs, _rhs)) {
          std::swap(_lhs, _rhs);
        }
      }
    };  // class Rule

    ////////////////////////////////////////////////////////////////////////
    // RuleLookup
    ////////////////////////////////////////////////////////////////////////

    class RuleLookup {
     public:
      RuleLookup() : _rule(nullptr) {}

      explicit RuleLookup(Rule* rule)
          : _first(rule->lhs()->cbegin()),
            _last(rule->lhs()->cend()),
            _rule(rule) {}

      RuleLookup& operator()(std::string::iterator const& first,
                             std::string::iterator const& last) {
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
      bool operator<(RuleLookup const& that) const;

     private:
      std::string::const_iterator _first;
      std::string::const_iterator _last;
      Rule const*                 _rule;
    };  // class RuleLookup

    ////////////////////////////////////////////////////////////////////////
    // Rules
    ////////////////////////////////////////////////////////////////////////

    class Rules {
     public:
      using iterator               = std::list<Rule*>::iterator;
      using const_iterator         = std::list<Rule*>::const_iterator;
      using const_reverse_iterator = std::list<Rule*>::const_reverse_iterator;

     private:
      struct Stats {
        Stats() noexcept;
        Stats& init() noexcept;

        Stats(Stats const&) noexcept            = default;
        Stats(Stats&&) noexcept                 = default;
        Stats& operator=(Stats const&) noexcept = default;
        Stats& operator=(Stats&&) noexcept      = default;

        size_t   max_word_length;
        size_t   max_active_word_length;
        size_t   max_active_rules;
        size_t   min_length_lhs_rule;
        uint64_t total_rules;
        // std::unordered_set<std::string> unique_lhs_rules;
      };

      // TODO(2) remove const?
      std::list<Rule*>        _active_rules;
      std::array<iterator, 2> _cursors;
      std::list<Rule*>        _inactive_rules;
      mutable Stats           _stats;

     public:
      Rules() = default;

      // Rules(Rules const& that);
      // Rules(Rules&& that);
      Rules& operator=(Rules const&);

      // TODO(1) the other constructors

      ~Rules();

      Rules& init();

      const_iterator begin() const noexcept {
        return _active_rules.cbegin();
      }

      const_iterator end() const noexcept {
        return _active_rules.cend();
      }

      iterator begin() noexcept {
        return _active_rules.begin();
      }

      iterator end() noexcept {
        return _active_rules.end();
      }

      const_reverse_iterator rbegin() const noexcept {
        return _active_rules.crbegin();
      }

      const_reverse_iterator rend() const noexcept {
        return _active_rules.crend();
      }

      [[nodiscard]] size_t number_of_active_rules() const noexcept {
        return _active_rules.size();
      }

      [[nodiscard]] size_t number_of_inactive_rules() const noexcept {
        return _inactive_rules.size();
      }

      [[nodiscard]] size_t max_active_word_length() const;

      iterator& cursor(size_t index) {
        LIBSEMIGROUPS_ASSERT(index < _cursors.size());
        return _cursors[index];
      }

      // TODO(1) is this ever called?
      void add_active_rule(Rule* rule) {
        _active_rules.push_back(rule);
      }

      void add_inactive_rule(Rule* rule) {
        _inactive_rules.push_back(rule);
      }

      Stats const& stats() const {
        return _stats;
      }

      [[nodiscard]] iterator erase_from_active_rules(iterator it);

      // TODO(1) this feels like it should be add_active rule. The above
      // add_active_rule seems a bit dangerous
      void add_rule(Rule* rule);

      [[nodiscard]] Rule* copy_rule(Rule const* rule);

      //  private:
      [[nodiscard]] Rule* new_rule();

     protected:
      template <typename Iterator>
      [[nodiscard]] Rule* new_rule(Iterator begin_lhs,
                                   Iterator end_lhs,
                                   Iterator begin_rhs,
                                   Iterator end_rhs) {
        Rule* rule = new_rule();
        rule->lhs()->assign(begin_lhs, end_lhs);
        rule->rhs()->assign(begin_rhs, end_rhs);
        rule->reorder();
        return rule;
      }
    };  // class Rules

    ////////////////////////////////////////////////////////////////////////
    // RewriterBase
    ////////////////////////////////////////////////////////////////////////

    class RewriterBase : public Rules {
      mutable std::atomic<bool> _cached_confluent;
      mutable std::atomic<bool> _confluence_known;
      size_t                    _max_stack_depth;

     protected:
      std::vector<Rule*> _pending_rules;

     public:
      // TODO(0) everything is public!!!
      // TODO(1) to cpp
      RewriterBase()
          : _cached_confluent(false),
            _confluence_known(false),
            _max_stack_depth(0),
            _pending_rules() {}

      RewriterBase& init();

      virtual ~RewriterBase();

      // TODO(1) to cpp
      RewriterBase& operator=(RewriterBase const& that) {
        Rules::operator=(that);
        _cached_confluent = that._cached_confluent.load();
        _confluence_known = that._confluence_known.load();
        _pending_rules.clear();

        for (auto const* rule : that._pending_rules) {
          _pending_rules.emplace_back(copy_rule(rule));
        }
        return *this;
      }

      RewriterBase& increase_alphabet_size_by(size_t) {
        return *this;
      }

      void set_cached_confluent(tril val) const;

      bool cached_confluent() const noexcept {
        return _cached_confluent;
      }

      [[nodiscard]] bool consistent() const noexcept {
        return _pending_rules.empty();
      }

      [[nodiscard]] bool confluence_known() const {
        return _confluence_known;
      }

      [[nodiscard]] size_t max_stack_depth() const {
        return _max_stack_depth;
      }

      bool add_pending_rule(Rule* rule);

      void report_progress_from_thread(
          std::chrono::high_resolution_clock::time_point start_time);

      bool process_pending_rules();

      void reduce();

      void rewrite(Rule* rule) const {
        rewrite(*rule->lhs());
        rewrite(*rule->rhs());
        rule->reorder();
      }

      // Rewrite <u> without using <rule>, nullptr for rule indicates no rule
      // is disabled.
      [[nodiscard]] virtual bool rewrite_with_disabled_rule(std::string& u,
                                                            Rule const*  rule
                                                            = nullptr)
          = 0;

      void rewrite(std::string& u) {
        // TODO improve
        std::ignore = rewrite_with_disabled_rule(u);
      }

      void rewrite(std::string& u) const {
        // TODO improve
        std::ignore
            = const_cast<RewriterBase*>(this)->rewrite_with_disabled_rule(u);
      }

      bool rewrite_active_rule(Rule* rule) {
        return rewrite_with_disabled_rule(*rule->lhs(), rule)
               || rewrite_with_disabled_rule(*rule->rhs(), rule);
      }

      virtual void add_rule(Rule* rule) = 0;

      virtual Rules::iterator make_active_rule_pending(Rules::iterator it) = 0;

      size_t number_of_pending_rules() const noexcept {
        return _pending_rules.size();
      }

      Rule* next_pending_rule() {
        LIBSEMIGROUPS_ASSERT(_pending_rules.size() != 0);
        Rule* rule = _pending_rules.back();
        _pending_rules.pop_back();
        return rule;
      }

      template <typename StringLike>
      void add_rule(StringLike const& lhs, StringLike const& rhs) {
        if (lhs != rhs) {
          if (add_pending_rule(new_rule(
                  lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()))) {
            // TODO(1) only process_pending_rules when ready to run
            process_pending_rules();
          }
        }
      }

      template <typename StringLike>
      void add_pending_rule(StringLike const& lhs, StringLike const& rhs) {
        if (lhs != rhs) {
          add_pending_rule(
              new_rule(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()));
        }
      }
    };  // class RewriterBase

    ////////////////////////////////////////////////////////////////////////
    // RewriteFromLeft
    ////////////////////////////////////////////////////////////////////////

    class RewriteFromLeft : public RewriterBase {
      std::set<RuleLookup> _set_rules;

     public:
      using RewriterBase::add_rule;

      RewriteFromLeft() = default;
      RewriteFromLeft& operator=(RewriteFromLeft const&);
      // TODO(2) the other constructors
      ~RewriteFromLeft();

      RewriteFromLeft& init();

      [[nodiscard]] bool confluent() const;

     private:
      [[nodiscard]] bool
      rewrite_with_disabled_rule(std::string& u,
                                 Rule const*  disabled_rule) override;

      void add_rule(Rule* rule) override;

      iterator make_active_rule_pending(iterator) override;

      void report_from_confluent(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const;

      bool confluent_impl(std::atomic_uint64_t&) const;
    };

    ////////////////////////////////////////////////////////////////////////
    // RewriteTrie
    ////////////////////////////////////////////////////////////////////////

    class RewriteTrie : public RewriterBase {
     public:
      using index_type = AhoCorasickImpl::index_type;
      using RewriterBase::cached_confluent;
      using Rules::stats;
      using iterator      = std::string::iterator;
      using rule_iterator = std::unordered_map<index_type, Rule*>::iterator;

     private:
      std::vector<index_type> _nodes;

      std::unordered_map<index_type, Rule*> _rules;
      AhoCorasickImpl                       _trie;

     public:
      using RewriterBase::add_rule;

      RewriteTrie() : RewriterBase(), _rules(), _trie(0) {}
      RewriteTrie& init();

      RewriteTrie(RewriteTrie const& that);
      RewriteTrie& operator=(RewriteTrie const& that);

      // TODO(1) move constructor, and move assignment operator

      ~RewriteTrie();

      RewriteTrie& increase_alphabet_size_by(size_t val) {
        _trie.increase_alphabet_size_by(val);
        return *this;
      }

      [[nodiscard]] bool confluent() const;

      bool process_pending_rules();

     private:
      // TODO should be no_checks
      // TODO add a check version
      void add_rule(Rule* rule) override {
        Rules::add_rule(rule);
        add_rule_to_trie(rule);
        set_cached_confluent(tril::unknown);
      }

      [[nodiscard]] bool
      rewrite_with_disabled_rule(std::string& u,
                                 Rule const*  disabled_rule) override;

      [[nodiscard]] bool descendants_confluent(Rule const* rule1,
                                               index_type  current_node,
                                               size_t backtrack_depth) const;

      void add_rule_to_trie(Rule* rule) {
        index_type node = _trie.add_word_no_checks(rule->lhs()->cbegin(),
                                                   rule->lhs()->cend());
        _rules.emplace(node, rule);
      }

      Rules::iterator make_active_rule_pending(Rules::iterator it) override;

      void report_from_confluent(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const;

      bool confluent_impl(std::atomic_uint64_t&) const;
    };
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_REWRITERS_HPP_
