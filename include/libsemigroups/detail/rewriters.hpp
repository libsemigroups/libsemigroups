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
#include <list>           // for list
#include <set>            // for set
#include <string>         // for basic_string, operator==
#include <unordered_map>  // for unordered_map

#include "libsemigroups/debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/order.hpp"  // for shortlex_compare
#include "libsemigroups/types.hpp"  // for u8string

#include "aho-corasick-impl.hpp"  // for AhoCorasickImpl
#include "multi-view.hpp"         // for MultiView

// TODO(2) Add a KnuthBendixImpl pointer to the rewriter class so that overlap
// detection can be handled by the rewriter (and therefore depend on the
// implementation) rather than on the KB object.

namespace libsemigroups {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // Rule
    ////////////////////////////////////////////////////////////////////////

    class Rule {
     public:
      using native_word_type = u8string;

     private:
      native_word_type _lhs;
      native_word_type _rhs;
      int64_t          _id;

     public:
      explicit Rule(int64_t id);

      Rule()                            = delete;
      Rule& operator=(Rule const& copy) = delete;
      Rule(Rule const& copy)            = delete;
      Rule(Rule&& copy)                 = delete;
      Rule& operator=(Rule&& copy)      = delete;

      ~Rule() = default;

      [[nodiscard]] native_word_type const& lhs() const noexcept {
        return _lhs;
      }

      [[nodiscard]] native_word_type const& rhs() const noexcept {
        return _rhs;
      }

      [[nodiscard]] native_word_type& lhs() noexcept {
        return _lhs;
      }

      [[nodiscard]] native_word_type& rhs() noexcept {
        return _rhs;
      }

      [[nodiscard]] bool empty() const noexcept {
        return _lhs.empty() && _rhs.empty();
      }

      [[nodiscard]] inline bool active() const noexcept {
        LIBSEMIGROUPS_ASSERT(_id != 0);
        return _id > 0;
      }

      void activate_no_checks() noexcept;
      void deactivate_no_checks() noexcept;

      void set_id_no_checks(int64_t id) noexcept {
        LIBSEMIGROUPS_ASSERT(id > 0);
        LIBSEMIGROUPS_ASSERT(!active());
        _id = -1 * id;
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
      using native_word_type = Rule::native_word_type;

      RuleLookup() : _rule(nullptr) {}

      explicit RuleLookup(Rule* rule)
          : _first(rule->lhs().cbegin()),
            _last(rule->lhs().cend()),
            _rule(rule) {}

      RuleLookup& operator()(native_word_type::iterator first,
                             native_word_type::iterator last) {
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
      native_word_type::const_iterator _first;
      native_word_type::const_iterator _last;
      Rule const*                      _rule;
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
      };

      std::list<Rule*>        _active_rules;
      std::array<iterator, 2> _cursors;
      std::list<Rule*>        _inactive_rules;
      mutable Stats           _stats;

     public:
      Rules() = default;

      Rules(Rules const& that) : Rules() {
        *this = that;
      }
      Rules(Rules&& that) = default;

      Rules& operator=(Rules const&);
      Rules& operator=(Rules&& that);

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

      Stats const& stats() const {
        return _stats;
      }

      void add_rule(Rule* rule);

     protected:
      template <typename Iterator>
      [[nodiscard]] Rule* new_rule(Iterator begin_lhs,
                                   Iterator end_lhs,
                                   Iterator begin_rhs,
                                   Iterator end_rhs) {
        Rule* rule = new_rule();
        rule->lhs().assign(begin_lhs, end_lhs);
        rule->rhs().assign(begin_rhs, end_rhs);
        rule->reorder();
        return rule;
      }

      [[nodiscard]] Rule*    copy_rule(Rule const* rule);
      [[nodiscard]] iterator erase_from_active_rules(iterator it);
      void                   add_inactive_rule(Rule* rule) {
        _inactive_rules.push_back(rule);
      }

     private:
      [[nodiscard]] Rule* new_rule();
    };  // class Rules

    ////////////////////////////////////////////////////////////////////////
    // RewriteBase
    ////////////////////////////////////////////////////////////////////////

    class RewriteBase : public Rules {
      mutable std::atomic<bool> _cached_confluent;
      mutable std::atomic<bool> _confluence_known;
      size_t                    _max_pending_rules;

     protected:
      enum class State : uint8_t {
        none,
        adding_pending_rules,
        reducing_pending_rules,
        checking_confluence
      };

      std::vector<Rule*> _pending_rules;
      State              _state;
      bool               _ticker_running;

     public:
      using native_word_type = Rule::native_word_type;

      ////////////////////////////////////////////////////////////////////////
      // Constructors + inits
      ////////////////////////////////////////////////////////////////////////

      RewriteBase();
      RewriteBase& init();
      RewriteBase(RewriteBase const& that) : RewriteBase() {
        *this = that;
      }
      RewriteBase(RewriteBase&& that);
      RewriteBase& operator=(RewriteBase const& that);
      RewriteBase& operator=(RewriteBase&& that);

      virtual ~RewriteBase();

      ////////////////////////////////////////////////////////////////////////
      // Public mem fns
      ////////////////////////////////////////////////////////////////////////

      RewriteBase& increase_alphabet_size_by(size_t) {
        return *this;
      }

      bool cached_confluent() const noexcept {
        return _cached_confluent;
      }

      [[nodiscard]] bool confluence_known() const {
        return _confluence_known;
      }

      [[nodiscard]] size_t max_pending_rules() const {
        return _max_pending_rules;
      }

      size_t number_of_pending_rules() const noexcept {
        return _pending_rules.size();
      }

      Rule* next_pending_rule();

      [[nodiscard]] bool confluent();

      template <typename StringLike>
      void add_rule(StringLike const& lhs, StringLike const& rhs);

      void set_cached_confluent(tril val) const;

     protected:
      ////////////////////////////////////////////////////////////////////////
      // Member functions - protected
      ////////////////////////////////////////////////////////////////////////

      void report_progress_from_thread(
          std::atomic_uint64_t const&                           seen,
          std::chrono::high_resolution_clock::time_point const& start_time);

      void report_progress_from_thread(
          std::chrono::high_resolution_clock::time_point const& start_time) {
        report_progress_from_thread(0, start_time);
      }

      bool add_pending_rule(Rule* rule);

     private:
      virtual bool confluent_impl(std::atomic_uint64_t& seen) = 0;

      virtual void report_checking_confluence(
          std::atomic_uint64_t const&                           seen,
          std::chrono::high_resolution_clock::time_point const& start_time)
          const
          = 0;

      virtual void report_reducing_rules(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const {}
    };  // class RewriteBase

    // RewriteBase out-of-lined mem fn template
    template <typename StringLike>
    void RewriteBase::add_rule(StringLike const& lhs, StringLike const& rhs) {
      if (lhs != rhs) {
        add_pending_rule(
            new_rule(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()));
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // RewriteFromLeft
    ////////////////////////////////////////////////////////////////////////

    class RewriteFromLeft : public RewriteBase {
      std::set<RuleLookup> _set_rules;

     public:
      using native_word_type = Rule::native_word_type;

      using RewriteBase::add_rule;

      RewriteFromLeft() = default;

      RewriteFromLeft(RewriteFromLeft const& that) : RewriteFromLeft() {
        *this = that;
      }
      RewriteFromLeft(RewriteFromLeft&&) = default;

      RewriteFromLeft& operator=(RewriteFromLeft const&);
      RewriteFromLeft& operator=(RewriteFromLeft&&) = default;

      ~RewriteFromLeft();

      RewriteFromLeft& init();

      bool process_pending_rules();

      void rewrite(native_word_type& u);

      void rewrite(native_word_type& u) const {
        const_cast<RewriteFromLeft*>(this)->rewrite(u);
      }

     private:
      void rewrite(Rule* rule) const {
        rewrite(rule->lhs());
        rewrite(rule->rhs());
        rule->reorder();
      }

      void add_rule(Rule* rule);

      iterator make_active_rule_pending(iterator);

      void report_checking_confluence(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const override;

      bool confluent_impl(std::atomic_uint64_t&) override;
    };

    ////////////////////////////////////////////////////////////////////////
    // RewriteTrie
    ////////////////////////////////////////////////////////////////////////

    class RewriteTrie : public RewriteBase {
     public:
      using index_type       = AhoCorasickImpl::index_type;
      using iterator         = native_word_type::iterator;
      using rule_iterator    = std::unordered_map<index_type, Rule*>::iterator;
      using native_word_type = Rule::native_word_type;

     private:
      std::unordered_map<index_type, Rule*> _new_rule_map;
      AhoCorasickImpl                       _new_rule_trie;
      std::vector<index_type>               _rewrite_tmp_buf;
      std::unordered_map<index_type, Rule*> _rule_map;
      AhoCorasickImpl                       _rule_trie;
      bool                                  _ticker_running;

     public:
      using Rules::stats;

      using RewriteBase::add_rule;
      using RewriteBase::cached_confluent;

      RewriteTrie();

      RewriteTrie& init();

      RewriteTrie(RewriteTrie const& that) : RewriteTrie() {
        *this = that;
      }
      RewriteTrie(RewriteTrie&& that) = default;
      RewriteTrie& operator=(RewriteTrie const& that);
      RewriteTrie& operator=(RewriteTrie&& that) = default;

      ~RewriteTrie();

      RewriteTrie& increase_alphabet_size_by(size_t val) {
        _rule_trie.increase_alphabet_size_by(val);
        return *this;
      }

      bool process_pending_rules();

      // TODO(1) iterators
      void rewrite(native_word_type& u);

      void rewrite(Rule* rule) const {
        rewrite(rule->lhs());
        rewrite(rule->rhs());
        rule->reorder();
      }

      void rewrite(native_word_type& u) const {
        const_cast<RewriteTrie*>(this)->rewrite(u);
      }

     private:
      void add_rule(Rule* rule) {
        Rules::add_rule(rule);
        index_type node = _rule_trie.add_word_no_checks(rule->lhs().cbegin(),
                                                        rule->lhs().cend());
        _rule_map.emplace(node, rule);
        set_cached_confluent(tril::unknown);
      }

      [[nodiscard]] bool descendants_confluent(Rule const* rule1,
                                               index_type  current_node,
                                               size_t backtrack_depth) const;

      Rules::iterator make_active_rule_pending(Rules::iterator it);

      bool confluent_impl(std::atomic_uint64_t&) override;

      void report_checking_confluence(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const override;

      void report_reducing_rules(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const override;
    };
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_REWRITERS_HPP_
