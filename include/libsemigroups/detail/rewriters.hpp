//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2024 Joseph Edwards + James D. Mitchell
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

#include "../aho-corasick.hpp"
#include "../debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "../order.hpp"  // for shortlex_compare

#include "multi-string-view.hpp"  // for MultiStringView

// TODO(2) Add a KnuthBendix pointer to the rewriter class so that overlap
// detection can be handled by the rewriter (and therefore depend on the
// implementation) rather than on the KB object.

//! \defgroup \rewriters_group Rewriters
//!
//! This file contains documentation for the functionality of the following
//! classes in `libsemigroups`:
//! * \ref libsemigroups::Rule "Rule"
//! * \ref RuleLookup
//! * \ref Rules
//! * \ref RewriterBase
//! * \ref RewriteFromLeft
//! * \ref RewriteTrie
namespace libsemigroups {
  namespace detail {
    // TODO(2) remove from libsemigroups namespace and put into relevant class

    //! \ingroup rewriters_group
    //!
    //! Alias for the type of word that can be input by the user
    using external_string_type = std::string;

    //! \ingroup rewriters_group
    //!
    //! Alias for the type of word used internally in the implementation
    using internal_string_type = std::string;

    //! \ingroup rewriters_group
    //!
    //! Alias for the type of letter that can be input by the user
    using external_char_type = char;

    //! \ingroup rewriters_group
    //!
    //! Alias for the type of letter used internally in the implementation
    using internal_char_type = char;

    //! \ingroup rewriters_group
    //!
    //! \brief For a rewriting rule
    //!
    //! Defined in ``rewriters.hpp``.
    //!
    //! This class implements a data structure for storing *rewriting rules*.
    //! Here, a rewriting rule is a rule of the form \f$A \to B\f$, where
    //! \f$A\f$ and \f$B\f$ are both words over some alphabet \f$\Sigma\f$.
    //!
    //! The left-hand and right-hand sides of a rule are specified externally
    //! with the type \ref external_string_type, and stored internally with type
    //! \ref internal_string_type.
    class Rule {
      internal_string_type* _lhs;
      internal_string_type* _rhs;
      int64_t               _id;

     public:
      //! \brief Construct with new empty left-hand and right-hand sides.
      //!
      //! Construct with new empty left-hand and right-hand sides.
      //!
      //! \param id the id of the new rule.
      //!
      //! \exception
      //! \no_libsemigroups_except
      explicit Rule(int64_t id);

      Rule& operator=(Rule const& copy) = delete;
      Rule(Rule const& copy)            = delete;
      Rule(Rule&& copy)                 = delete;
      Rule& operator=(Rule&& copy)      = delete;

      //! \brief Destruct the Rule.
      //!
      //! This function destructs a \ref Rule object by deleting the pointers
      //! used for the left-hand and right-hand sides.
      ~Rule() {
        delete _lhs;
        delete _rhs;
      }

      //! \brief Return the left-hand side of the rule.
      //!
      //! Return the left-hand side of the rule. If this rule was create by a
      //! \ref KnuthBendix, this is guaranteed to be greater than its right-hand
      //! side according to the reduction ordering of that \ref KnuthBendix.
      //!
      //! \returns A pointer to the left-hand side.
      //!
      //! \exception
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa
      //! \ref KnuthBendix
      [[nodiscard]] internal_string_type* lhs() const noexcept {
        return _lhs;
      }

      //! \brief Return the right-hand side of the rule.
      //!
      //! Return the right-hand side of the rule. If this rule was create by a
      //! \ref KnuthBendix, this is guaranteed to be less than its left-hand
      //! side according to the reduction ordering of that \ref KnuthBendix.
      //!
      //! \returns A pointer to the right-hand side.
      //!
      //! \exception
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa
      //! \ref KnuthBendix
      [[nodiscard]] internal_string_type* rhs() const noexcept {
        return _rhs;
      }

      //! \brief Check if the left-hand and right-hand sides are empty.
      //!
      //! Check if the words pointed to by both the left-hand and the right-hand
      //! sides are empty.
      //!
      //! \returns A value of type `bool`.
      //!
      //! \exception
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      [[nodiscard]] bool empty() const noexcept {
        return _lhs->empty() && _rhs->empty();
      }

      //! \brief Check if the Rule is active.
      //!
      //! Check if the rule is active.
      //!
      //! \returns A value of type `bool`.
      //!
      //! \exception
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa
      //! \ref RewriterBase::active_rules()
      // TODO check the above ref points to something sensible
      [[nodiscard]] inline bool active() const noexcept {
        LIBSEMIGROUPS_ASSERT(_id != 0);
        return (_id > 0);
      }

      //! \brief Deactivate a rule.
      //!
      //! Deactivate a rule, if it is active.
      //!
      //! \exception
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa
      //! \ref active
      void deactivate() noexcept;

      //! \brief Activate a rule.
      //!
      //! Activate a rule, if it is inactive.
      //!
      //! \exception
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa
      //! \ref active
      void activate() noexcept;

      //! \brief Set the id of a rule.
      //!
      //! Set the id of a rule.
      //!
      //! \param id the id to set.
      //!
      //! \exception
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \note
      //! This function does no checks on its parameters; however, the id of a
      //! rule should only be set if the rule is inactive, and the id of a rule
      //! should always be positive.
      void set_id_no_checks(int64_t id) noexcept {
        LIBSEMIGROUPS_ASSERT(id > 0);
        LIBSEMIGROUPS_ASSERT(!active());
        _id = -1 * id;
      }

      //! \brief Set the id of a rule.
      //!
      //! After checking that the Rule is inactive \p id is positive, this
      //! function performs the same as \ref set_id_no_checks.
      //!
      //! \throws LIBSEMIGROUPS_EXCEPTION if \p id is non-positive, or if `this`
      //! is active.
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

      //! \brief Return the id of a rule.
      //!
      //! Return the id of a rule.
      //!
      //! \returns A value of type `int64_t`
      //!
      //! \complexity
      //! Constant.
      //!
      //! \exception
      //! \noexcept
      [[nodiscard]] int64_t id() const noexcept {
        LIBSEMIGROUPS_ASSERT(_id != 0);
        return _id;
      }

      //! \brief Reorder the left-hand and right-hand sides.
      //!
      //! If the right-hand side is greater than the left-hand side of a rule,
      //! with regards to length-lexicographical order, then swap them.
      //!
      //! \exceptions
      //! Throws if \ref shortlex_compare(T* const, T* const) does.
      //!
      //! \complexity
      //! The same complexity as \ref shortlex_compare(T* const, T* const)
      //!
      //! \sa
      //! shortlex_compare(T* const, T* const)
      void reorder() {
        if (shortlex_compare(_lhs, _rhs)) {
          std::swap(_lhs, _rhs);
        }
      }
    };  // class Rule

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
      bool operator<(RuleLookup const& that) const;

     private:
      internal_string_type::const_iterator _first;
      internal_string_type::const_iterator _last;
      Rule const*                          _rule;
    };  // class RuleLookup

    class Rules {
     public:
      using iterator       = std::list<Rule const*>::iterator;
      using const_iterator = std::list<Rule const*>::const_iterator;
      using const_reverse_iterator
          = std::list<Rule const*>::const_reverse_iterator;

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
        // std::unordered_set<internal_string_type> unique_lhs_rules;
      };

      // TODO(2) remove const?
      std::list<Rule const*>  _active_rules;
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

      // TODO(0) is this ever called?
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

      // TODO(0) this feels like it should be add_active rule. The above
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
    };

    class RewriterBase : public Rules {
      std::unordered_set<internal_char_type> _alphabet;
      mutable std::atomic<bool>              _cached_confluent;
      mutable std::atomic<bool>              _confluence_known;
      size_t                                 _max_stack_depth;
      std::stack<Rule*>                      _pending_rules;
      std::atomic<bool>                      _requires_alphabet;

      using alphabet_citerator
          = std::unordered_set<internal_char_type>::const_iterator;

     public:
      // TODO(0) to cpp
      RewriterBase()
          : _alphabet(),
            _cached_confluent(false),
            _confluence_known(false),
            _max_stack_depth(0),
            _pending_rules(),
            _requires_alphabet() {}

      RewriterBase& init();

      explicit RewriterBase(bool requires_alphabet) : RewriterBase() {
        _requires_alphabet = requires_alphabet;
      }

      ~RewriterBase();

      RewriterBase& operator=(RewriterBase const& that) {
        Rules::operator=(that);
        _cached_confluent  = that._cached_confluent.load();
        _confluence_known  = that._confluence_known.load();
        _requires_alphabet = that._requires_alphabet.load();
        while (!_pending_rules.empty()) {
          _pending_rules.pop();
        }
        decltype(_pending_rules) tmp = that._pending_rules;
        while (!tmp.empty()) {
          auto const* rule = tmp.top();
          _pending_rules.push(copy_rule(rule));
          tmp.pop();
        }

        if (_requires_alphabet) {
          _alphabet = that._alphabet;
        }
        return *this;
      }

      bool requires_alphabet() const {
        return _requires_alphabet;
      }

      decltype(_alphabet) alphabet() const {
        return _alphabet;
      }

      alphabet_citerator alphabet_cbegin() const {
        return _alphabet.cbegin();
      }

      alphabet_citerator alphabet_cend() const {
        return _alphabet.cend();
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

      bool process_pending_rules();

      void reduce();

      void reduce_rhs();

      void rewrite(Rule* rule) const {
        rewrite(*rule->lhs());
        rewrite(*rule->rhs());
        rule->reorder();
      }

      // TODO(2) remove virtual functions
      virtual void rewrite(internal_string_type& u) const = 0;

      virtual void add_rule(Rule* rule) = 0;

      virtual Rules::iterator make_active_rule_pending(Rules::iterator it) = 0;

      size_t number_of_pending_rules() const noexcept {
        return _pending_rules.size();
      }

      Rule* next_pending_rule() {
        LIBSEMIGROUPS_ASSERT(_pending_rules.size() != 0);
        Rule* rule = _pending_rules.top();
        _pending_rules.pop();
        return rule;
      }

      template <typename StringLike>
      void add_rule(StringLike const& lhs, StringLike const& rhs) {
        if (lhs != rhs) {
          if (add_pending_rule(new_rule(
                  lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()))) {
            // TODO(0) only process_pending_rules when ready to run
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

      void add_to_alphabet(internal_char_type letter) {
        _alphabet.emplace(letter);
      }
    };

    class RewriteFromLeft : public RewriterBase {
      std::set<RuleLookup> _set_rules;

     public:
      using RewriterBase::cached_confluent;
      using Rules::stats;

      RewriteFromLeft() = default;

      RewriteFromLeft& operator=(RewriteFromLeft const&);

      // TODO(2) the other constructors

      ~RewriteFromLeft() = default;  // TODO(2) out-of-line this

      RewriteFromLeft& init();

      void rewrite(internal_string_type& u) const;

      [[nodiscard]] bool confluent() const;

      // TODO(0) private?
      void add_rule(Rule* rule);

      using RewriterBase::add_rule;

     private:
      void rewrite(Rule* rule) const;

      iterator make_active_rule_pending(iterator);

      void report_from_confluent(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const;

      bool confluent_impl(std::atomic_uint64_t&) const;
    };

    class RewriteTrie : public RewriterBase {
      using index_type = AhoCorasick::index_type;

      std::map<index_type, Rule*> _rules;
      AhoCorasick                 _trie;

     public:
      using RewriterBase::cached_confluent;
      using Rules::stats;
      using iterator      = internal_string_type::iterator;
      using rule_iterator = std::map<index_type, Rule*>::iterator;

      RewriteTrie() : RewriterBase(true), _rules(), _trie() {}

      RewriteTrie(const RewriteTrie& that);

      RewriteTrie& operator=(RewriteTrie const& that);

      ~RewriteTrie() = default;

      RewriteTrie& init();

      rule_iterator rules_begin() {
        return _rules.begin();
      }

      rule_iterator rules_end() {
        return _rules.end();
      }

      void all_overlaps();

      void rule_overlaps(index_type node);

      void add_overlaps(Rule* rule, index_type node, size_t overlap_length);

      void rewrite(internal_string_type& u) const;

      [[nodiscard]] bool confluent() const;

      void add_rule(Rule* rule) {
        Rules::add_rule(rule);
        add_rule_to_trie(rule);
        set_cached_confluent(tril::unknown);
      }

      using RewriterBase::add_rule;

     private:
      [[nodiscard]] bool descendants_confluent(Rule const* rule1,
                                               index_type  current_node,
                                               size_t backtrack_depth) const;

      // TODO (After removing virtual functions) Put in base
      void rewrite(Rule* rule) const {
        rewrite(*rule->lhs());
        rewrite(*rule->rhs());
        rule->reorder();
      }

      void add_rule_to_trie(Rule* rule) {
        index_type node = _trie.add_word_no_checks(rule->lhs()->cbegin(),
                                                   rule->lhs()->cend());
        _rules.emplace(node, rule);
      }

      Rules::iterator make_active_rule_pending(Rules::iterator it);

      void report_from_confluent(
          std::atomic_uint64_t const&,
          std::chrono::high_resolution_clock::time_point const&) const;

      bool confluent_impl(std::atomic_uint64_t&) const;
    };
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_REWRITERS_HPP_
