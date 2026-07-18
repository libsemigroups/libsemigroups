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

#ifndef LIBSEMIGROUPS_DETAIL_REWRITING_SYSTEM_HPP_
#define LIBSEMIGROUPS_DETAIL_REWRITING_SYSTEM_HPP_

#include <atomic>         // for atomic
#include <chrono>         // for time_point
#include <list>           // for list
#include <set>            // for set
#include <stack>          // for stack
#include <string>         // for basic_string, operator==
#include <type_traits>    // for enable_if_t, is_same_v
#include <unordered_map>  // for unordered_map

#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/debug.hpp"   // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/order.hpp"   // for lenlex_cmp
#include "libsemigroups/runner.hpp"  // for delta
#include "libsemigroups/types.hpp"   // for u8string

#include "aho-corasick-impl.hpp"  // for AhoCorasickImpl
#include "multi-view.hpp"         // for MultiView
#include "report.hpp"             // for reporting_enabled
#include "rules.hpp"              // for Rules/Rule
#include "value-guard.hpp"        // for ValueGuard

namespace libsemigroups {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // RewritingSystemBase
    ////////////////////////////////////////////////////////////////////////

    class RewritingSystemBase : public Rules {
     public:
      using native_word_type = Rule::native_word_type;
      using rule_const_reference
          = std::pair<native_word_type const&, native_word_type const&>;

     private:
      struct Settings {
        size_t reduction_threshold = 128;
      };

      mutable std::atomic<bool>                     _cached_confluent;
      mutable std::atomic<bool>                     _confluence_known;
      Settings                                      _settings;
      std::function<bool(Rule const*, Rule const*)> _pending_rules_comparator;

     protected:
      enum class State : uint8_t {
        none,
        reducing_pending_rules,  // TODO(1) is this name good?
        checking_confluence
      };

      State _state;
      bool  _ticker_running;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Constructors + inits
      ////////////////////////////////////////////////////////////////////////

      RewritingSystemBase();
      RewritingSystemBase& init();

      RewritingSystemBase(RewritingSystemBase const& that)
          : RewritingSystemBase() {
        *this = that;
      }

      RewritingSystemBase(RewritingSystemBase&& that) : RewritingSystemBase() {
        *this = std::move(that);
      }

      RewritingSystemBase& operator=(RewritingSystemBase const& that);
      RewritingSystemBase& operator=(RewritingSystemBase&& that);

      virtual ~RewritingSystemBase();

      using Rules::stats;

      ////////////////////////////////////////////////////////////////////////
      // Settings
      ////////////////////////////////////////////////////////////////////////

      template <typename Compare>
      RewritingSystemBase& sort_pending_rules_by(Compare&& cmp) noexcept {
        _pending_rules_comparator = std::forward<Compare>(cmp);
        return *this;
      }

      ////////////////////////////////////////////////////////////////////////
      // Public mem fns
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] size_t number_of_rules() const noexcept {
        return Rules::pending_rules().size() + Rules::active_rules().size();
      }

      [[nodiscard]] auto rules() const {
        return chain(active_rules(), pending_rules())
               | rx::transform([](Rule const* rule) -> rule_const_reference {
                   return rule_const_reference(rule->lhs(), rule->rhs());
                 });
      }

      // Some rewriters require knowledge of the alphabet size, and some do
      // not. For those that do not we provide a default implementation that
      // does nothing.
      RewritingSystemBase& increase_alphabet_size_by(size_t) {
        return *this;
      }

      [[nodiscard]] bool confluent();

      [[nodiscard]] bool confluent_known() const {
        return _confluence_known;
      }

      // If there are no pending_rules, then the system is reduced. If there
      // are pending rules the system may be reduced or not depending on the
      // pending rules. There doesn't seem to be an easier way of checking if
      // they are reduced than just calling "reduce", so we opted to return a
      // tril here instead of bool.
      [[nodiscard]] tril is_reduced() const noexcept {
        return Rules::pending_rules().empty() ? tril::TRUE : tril::unknown;
      }

      [[nodiscard]] Settings& settings() noexcept {
        return _settings;
      }

      [[nodiscard]] Settings const& settings() const noexcept {
        return _settings;
      }

     protected:
      void sort_pending_rules();

      template <typename RewritingSystem>
      friend class KnuthBendixImpl;

      bool cached_confluent() const noexcept {
        return _cached_confluent;
      }

      void set_cached_confluent(tril val) const;

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

     private:
      [[nodiscard]] virtual bool confluent_impl(std::atomic_uint64_t& seen) = 0;

      virtual void report_checking_confluence(
          std::atomic_uint64_t const&                           seen,
          std::chrono::high_resolution_clock::time_point const& start_time)
          const
          = 0;

      virtual void report_reducing_rules(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const {}
    };  // class RewritingSystemBase

    ////////////////////////////////////////////////////////////////////////
    // RewritingSystemBaseWithOrder
    ////////////////////////////////////////////////////////////////////////

    template <template <typename> typename ReductionOrder>
    class RewritingSystemBaseWithOrder : public RewritingSystemBase {
      ReductionOrder<Default> _order;

     public:
      using reduction_order = ReductionOrder<Default>;

      ////////////////////////////////////////////////////////////////////////
      // Constructors + inits
      ////////////////////////////////////////////////////////////////////////

      RewritingSystemBaseWithOrder() = default;
      RewritingSystemBaseWithOrder& init();

      RewritingSystemBaseWithOrder(RewritingSystemBaseWithOrder const& that)
          = default;

      RewritingSystemBaseWithOrder(RewritingSystemBaseWithOrder&& that)
          = default;

      RewritingSystemBaseWithOrder&
      operator=(RewritingSystemBaseWithOrder const& that)
          = default;

      RewritingSystemBaseWithOrder&
      operator=(RewritingSystemBaseWithOrder&& that)
          = default;

      ~RewritingSystemBaseWithOrder() = default;

      ////////////////////////////////////////////////////////////////////////
      // Public member functions
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] ReductionOrder<Default> const& order() const noexcept {
        return _order;
      }

      template <typename... Args>
      RewritingSystemBaseWithOrder& emplace_order(Args&&... args) noexcept {
        _order.init(std::forward<Args>(args)...);
        return *this;
      }

      void reorder(Rule* rule) {
        if (_order(rule->lhs(), rule->rhs())) {
          std::swap(rule->lhs(), rule->rhs());
        }
      }
    };  // class RewritingSystemBaseWithOrder

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
    // RewritingSystemSet
    ////////////////////////////////////////////////////////////////////////

    template <template <typename> typename ReductionOrder>
    class RewritingSystemSet
        : public RewritingSystemBaseWithOrder<ReductionOrder> {
      ////////////////////////////////////////////////////////////////////////
      // Private aliases
      ////////////////////////////////////////////////////////////////////////
      using RewritingSystemBaseWithOrder_
          = RewritingSystemBaseWithOrder<ReductionOrder>;

      using iterator = Rules::iterator;

      ////////////////////////////////////////////////////////////////////////
      // Private data
      ////////////////////////////////////////////////////////////////////////
      std::set<RuleLookup> _set_rules;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Public aliases
      ////////////////////////////////////////////////////////////////////////

      using native_word_type     = Rule::native_word_type;
      using reduction_order      = ReductionOrder<Default>;
      using rule_const_reference = RewritingSystemBase::rule_const_reference;

      template <typename Word>
      using reduction_order_template
          = std::enable_if_t<std::is_same_v<Word, Default>,
                             ReductionOrder<Default>>;

      ////////////////////////////////////////////////////////////////////////
      // Constructors + initializers
      ////////////////////////////////////////////////////////////////////////

      RewritingSystemSet() = default;
      RewritingSystemSet& init();

      RewritingSystemSet(RewritingSystemSet const& that)
          : RewritingSystemSet() {
        *this = that;
      }
      RewritingSystemSet(RewritingSystemSet&&) = default;

      RewritingSystemSet& operator=(RewritingSystemSet const&);
      RewritingSystemSet& operator=(RewritingSystemSet&&) = default;

      ~RewritingSystemSet();

      ////////////////////////////////////////////////////////////////////////
      // Public member functions --- from RewritingSystemBase
      ////////////////////////////////////////////////////////////////////////

      using RewritingSystemBase::number_of_rules;

      ////////////////////////////////////////////////////////////////////////
      // Public member functions - alphabetical order
      ////////////////////////////////////////////////////////////////////////

      template <typename Iterator>
      RewritingSystemSet& add_rule(Iterator first1,
                                   Iterator last1,
                                   Iterator first2,
                                   Iterator last2);

      [[nodiscard]] std::pair<size_t, size_t> confluence_ratio();

      // Returns true if the system changes as a result of this call (i.e. it
      // wasn't reduced before but now it is)
      bool reduce();

      void rewrite(native_word_type& u);

     private:
      ////////////////////////////////////////////////////////////////////////
      // Private member functions
      ////////////////////////////////////////////////////////////////////////

      void add_active_rule(Rule* rule);

      iterator make_active_rule_pending(iterator it);

      void rewrite_no_reduce(native_word_type& u) const;

      ////////////////////////////////////////////////////////////////////////
      // Confluence
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] bool confluent_impl(std::atomic_uint64_t&) override;

      ////////////////////////////////////////////////////////////////////////
      // Reporting
      ////////////////////////////////////////////////////////////////////////

      void report_checking_confluence(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const override;
    };  // RewritingSystemSet

    ////////////////////////////////////////////////////////////////////////
    // RewritingSystemTrie
    ////////////////////////////////////////////////////////////////////////

    template <template <typename> typename ReductionOrder>
    class RewritingSystemTrie
        : public RewritingSystemBaseWithOrder<ReductionOrder> {
      ////////////////////////////////////////////////////////////////////////
      // Private aliases
      ////////////////////////////////////////////////////////////////////////

      using Trie = AhoCorasickImpl;
      using RewritingSystemBaseWithOrder_
          = RewritingSystemBaseWithOrder<ReductionOrder>;

      using iterator   = Rules::iterator;
      using index_type = Trie::index_type;

      ////////////////////////////////////////////////////////////////////////
      // Private data
      ////////////////////////////////////////////////////////////////////////

      Trie                                            _new_rule_trie;
      std::function<bool(RewritingSystemTrie const&)> _use_new_rule_trie;
      ReductionOrder<Default>                         _order;
      Trie                                            _rule_trie;
      bool                                            _ticker_running;
      mutable std::vector<index_type> _trie_nodes_visited_indices;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Public aliases
      ////////////////////////////////////////////////////////////////////////

      using native_word_type     = Rule::native_word_type;
      using rule_const_reference = RewritingSystemBase::rule_const_reference;
      using reduction_order      = ReductionOrder<Default>;

      template <typename Word>
      using reduction_order_template
          = std::enable_if_t<std::is_same_v<Word, Default>,
                             ReductionOrder<Default>>;

      ////////////////////////////////////////////////////////////////////////
      // Constructors + initializers
      ////////////////////////////////////////////////////////////////////////

      RewritingSystemTrie();
      RewritingSystemTrie& init();

      RewritingSystemTrie(RewritingSystemTrie const& that)
          : RewritingSystemTrie() {
        *this = that;
      }
      RewritingSystemTrie(RewritingSystemTrie&& that) = default;

      RewritingSystemTrie& operator=(RewritingSystemTrie const& that);
      RewritingSystemTrie& operator=(RewritingSystemTrie&& that) = default;

      ~RewritingSystemTrie();

      ////////////////////////////////////////////////////////////////////////
      // Public member functions --- from RewritingSystemBase
      ////////////////////////////////////////////////////////////////////////

      using RewritingSystemBase::number_of_rules;

      ////////////////////////////////////////////////////////////////////////
      // Public member functions - alphabetical order
      ////////////////////////////////////////////////////////////////////////

      template <typename Iterator>
      RewritingSystemTrie& add_rule(Iterator first1,
                                    Iterator last1,
                                    Iterator first2,
                                    Iterator last2);

      RewritingSystemTrie& increase_alphabet_size_by(size_t val) {
        _rule_trie.increase_alphabet_size_by(val);
        return *this;
      }

      [[nodiscard]] std::pair<size_t, size_t> confluence_ratio();

      // Returns true if the system changes as a result of this call (i.e. it
      // wasn't reduced before but now it is)
      bool reduce();

      void rewrite(native_word_type& u);

      [[nodiscard]] Trie const& trie() const noexcept {
        return _rule_trie;
      }

      [[nodiscard]] Trie& trie() noexcept {
        return _rule_trie;
      }

      auto use_new_rule_trie() const noexcept {
        return _use_new_rule_trie;
      }

      RewritingSystemTrie& use_new_rule_trie(
          std::function<bool(RewritingSystemTrie const&)> const& func) {
        _use_new_rule_trie = func;
        return *this;
      }

     private:
      ////////////////////////////////////////////////////////////////////////
      // Private member functions
      ////////////////////////////////////////////////////////////////////////

      void     add_active_rule(Rule* new_rule);
      iterator make_active_rule_pending(iterator it);

      void rewrite_no_reduce(native_word_type& u) const;

      ////////////////////////////////////////////////////////////////////////
      // Confluence
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] bool overlap_confluent(Rule const*       rule1,
                                           Rule const*       rule2,
                                           size_t const      overlap_length,
                                           native_word_type& word1,
                                           native_word_type& word2) const;

      [[nodiscard]] bool descendants_confluent(Rule const* rule1,
                                               index_type  current_node,
                                               size_t backtrack_depth) const;

      [[nodiscard]] std::pair<size_t, size_t>
      number_descendants_confluent(Rule const* rule1,
                                   index_type  current_node,
                                   size_t      overlap_length) const;

      [[nodiscard]] bool confluent_impl(std::atomic_uint64_t&) override;

      ////////////////////////////////////////////////////////////////////////
      // Reporting
      ////////////////////////////////////////////////////////////////////////

      void report_checking_confluence(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const override;

      void report_reducing_rules(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const override;
    };

    ////////////////////////////////////////////////////////////////////////
    // Helpers
    ////////////////////////////////////////////////////////////////////////

    namespace rewriting_system {

      template <typename RewritingSystem, typename Word>
      void add_rule(RewritingSystem& rs, Word const& lhs, Word const& rhs) {
        rs.add_rule(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
      }

      // Might never terminate if rws.reduce() doesn't terminate
      template <typename RewritingSystem>
      [[nodiscard]] bool
      is_length_non_increasing(RewritingSystem& rws) noexcept;

      template <typename RewritingSystem>
      [[nodiscard]] tril
      is_length_non_increasing_no_reduce(RewritingSystem const& rws) noexcept;

      template <typename RewritingSystem>
      [[nodiscard]] tril is_terminating(RewritingSystem& rws) noexcept;

      template <typename RewritingSystem>
      [[nodiscard]] tril
      is_terminating_no_reduce(RewritingSystem const& rws) noexcept;

    }  // namespace rewriting_system

    using RewriteTrie [[deprecated]]     = RewritingSystemTrie<LenLexCmp>;
    using RewriteFromLeft [[deprecated]] = RewritingSystemSet<LenLexCmp>;

  }  // namespace detail
}  // namespace libsemigroups

#include "rewriting-system.tpp"
#endif  // LIBSEMIGROUPS_DETAIL_REWRITING_SYSTEM_HPP_
