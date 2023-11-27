//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 Joe Edwards + James D. Mitchell
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

#ifndef LIBSEMIGROUPS_REWRITERS_HPP_
#define LIBSEMIGROUPS_REWRITERS_HPP_

#include <atomic>         // for atomic
#include <set>            // for set
#include <string>         // for basic_string, operator==
#include <unordered_map>  // for unordered map
#include <unordered_set>  // for unordered set

#include "aho-corasick.hpp"
#include "debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "order.hpp"  // for shortlex_compare

namespace libsemigroups {
  // TODO remove from libsemigroups namespace and put into relevant class
  using external_string_type = std::string;
  using internal_string_type = std::string;
  using external_char_type   = char;
  using internal_char_type   = char;

  class Rule {
    internal_string_type* _lhs;
    internal_string_type* _rhs;
    int64_t               _id;

   public:
    // Construct from KnuthBendix with new but empty internal_string_type's
    Rule(int64_t id);

    Rule& operator=(Rule const& copy) = delete;
    Rule(Rule const& copy)            = delete;
    Rule(Rule&& copy)                 = delete;
    Rule& operator=(Rule&& copy)      = delete;

    // Destructor, deletes pointers used to create the rule.
    ~Rule() {
      delete _lhs;
      delete _rhs;
    }

    // Returns the left hand side of the rule, which is guaranteed to be
    // greater than its right hand side according to the reduction ordering
    // of the KnuthBendix used to construct this.
    [[nodiscard]] internal_string_type* lhs() const noexcept {
      return _lhs;
    }

    // Returns the right hand side of the rule, which is guaranteed to be
    // less than its left hand side according to the reduction ordering of
    // the KnuthBendix used to construct this.
    [[nodiscard]] internal_string_type* rhs() const noexcept {
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

    void set_id(int64_t id) noexcept {
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

      size_t   max_stack_depth;  // TODO remove this to RewriteFromLeft
      size_t   max_word_length;
      size_t   max_active_word_length;
      size_t   max_active_rules;
      size_t   min_length_lhs_rule;
      uint64_t total_rules;
      // std::unordered_set<internal_string_type> unique_lhs_rules;
    };

    // TODO remove const?
    std::list<Rule const*>  _active_rules;
    std::array<iterator, 2> _cursors;
    std::list<Rule*>        _inactive_rules;
    Stats                   _stats;

   public:
    Rules() = default;

    // Rules(Rules const& that);
    // Rules(Rules&& that);
    Rules& operator=(Rules const&);

    // TODO the other constructors

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

    iterator& cursor(size_t index) {
      LIBSEMIGROUPS_ASSERT(index < _cursors.size());
      return _cursors[index];
    }

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
    mutable std::atomic<bool>              _confluent;
    mutable std::atomic<bool>              _confluence_known;
    std::atomic<bool>                      _requires_alphabet;
    std::stack<Rule*>                      _stack;

    using alphabet_citerator
        = std::unordered_set<internal_char_type>::const_iterator;

   public:
    RewriterBase() = default;
    RewriterBase& init();

    RewriterBase(bool requires_alphabet) : RewriterBase() {
      _requires_alphabet = requires_alphabet;
    }

    ~RewriterBase();

    RewriterBase& operator=(RewriterBase const& that) {
      Rules::operator=(that);
      _confluent         = that._confluent.load();
      _confluence_known  = that._confluence_known.load();
      _requires_alphabet = that._requires_alphabet.load();
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

    // TODO remove?
    //  TODO to cpp if keeping it
    void confluent(tril val) const {
      if (val == tril::TRUE) {
        _confluence_known = true;
        _confluent        = true;
      } else if (val == tril::FALSE) {
        _confluence_known = true;
        _confluent        = false;
      } else {
        _confluence_known = false;
      }
    }

    // TODO this should be renamed, it's confusingly different than the
    // confluent() mem fn of RewriteFromLeft
    bool confluent() const noexcept {
      return _confluent;
    }

    [[nodiscard]] bool consistent() const noexcept {
      return _stack.empty();
    }

    [[nodiscard]] bool confluence_known() const {
      return _confluence_known;
    }

    bool push_stack(Rule* rule);

    void clear_stack();

    void reduce();

    void rewrite(Rule* rule) const {
      rewrite(*rule->lhs());
      rewrite(*rule->rhs());
      rule->reorder();
    }

    virtual void rewrite(internal_string_type& u) const = 0;

    virtual void add_rule(Rule* rule) = 0;

    virtual Rules::iterator erase_from_active_rules(Rules::iterator it) = 0;

    size_t number_of_pending_rules() const noexcept {
      return _stack.size();
    }

    Rule* next_pending_rule() {
      LIBSEMIGROUPS_ASSERT(_stack.size() != 0);
      Rule* rule = _stack.top();
      _stack.pop();
      return rule;
    }

    template <typename StringLike>
    void add_rule(StringLike const& lhs, StringLike const& rhs) {
      if (lhs != rhs) {
        if (push_stack(
                new_rule(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()))) {
          clear_stack();
        }
      }
    }

    void add_to_alphabet(internal_char_type letter) {
      _alphabet.emplace(letter);
    }
  };

  class RewriteFromLeft : public RewriterBase {
    std::set<RuleLookup> _set_rules;

   public:
    using RewriterBase::confluent;
    using Rules::stats;

    RewriteFromLeft() = default;

    // Rules(Rules const& that);
    // Rules(Rules&& that);
    RewriteFromLeft& operator=(RewriteFromLeft const&);

    // TODO the other constructors

    ~RewriteFromLeft() = default;  // TODO out-of-line this

    RewriteFromLeft& init();

    void rewrite(internal_string_type& u) const;

    [[nodiscard]] bool confluent() const;

    // TODO private?
    void add_rule(Rule* rule);

    using RewriterBase::add_rule;
    // template <typename StringLike>
    // void add_rule(StringLike const& lhs, StringLike const& rhs) {
    //   if (RewriterBase::add_rule(lhs, rhs)) {
    //     clear_stack();
    //   }
    // }

   private:
    void rewrite(Rule* rule) const;
    // void     clear_stack();
    iterator erase_from_active_rules(iterator);
  };

  class RewriteTrie : public RewriterBase {
    using index_type = AhoCorasick::index_type;

    std::unordered_map<index_type, Rule*> _rules;
    AhoCorasick                           _trie;

   public:
    using RewriterBase::confluent;
    using Rules::stats;
    using iterator = internal_string_type::iterator;

    RewriteTrie() : RewriterBase(true), _rules(), _trie() {}

    RewriteTrie(const RewriteTrie& that);
    RewriteTrie& operator=(RewriteTrie const& that) {
      init();
      RewriterBase::operator=(that);
      for (auto* crule : that) {
        Rule* rule = const_cast<Rule*>(crule);
        add_rule_to_trie(rule);
      }
      return *this;
    }

    ~RewriteTrie() = default;

    RewriteTrie& init() {
      RewriterBase::init();
      _trie.init();
      _rules.clear();
      return *this;
    }

    void rewrite(internal_string_type& u) const {
      // Check if u is rewriteable
      if (u.size() < stats().min_length_lhs_rule) {
        return;
      }

      std::stack<index_type> nodes;  // TODO better choice than stack?
      index_type             current = _trie.root;
      nodes.emplace(current);

#ifdef LIBSEMIGROUPS_DEBUG
      iterator v_begin = u.begin();
#endif
      iterator v_end   = u.begin();
      iterator w_begin = v_end;
      iterator w_end   = u.end();

      while (w_begin != w_end) {
        // Read first letter of W and traverse trie
        auto x = *w_begin;
        ++w_begin;
        current = _trie.traverse_from(current, static_cast<letter_type>(x));

        if (!_trie.node(current).is_terminal()) {
          nodes.emplace(current);
          *v_end = x;
          ++v_end;
        } else {
          // Find rule that corresponds to terminal node
          Rule const* rule     = _rules.find(current)->second;
          auto        lhs_size = rule->lhs()->size();

          // Check the lhs is smaller than the portion of the word that has
          // been read
          LIBSEMIGROUPS_ASSERT(lhs_size
                               <= static_cast<size_t>(v_end - v_begin) + 1);
          v_end -= lhs_size - 1;
          w_begin -= rule->rhs()->size();
          detail::string_replace(
              w_begin, rule->rhs()->cbegin(), rule->rhs()->cend());
          for (size_t i = 0; i < lhs_size - 1; ++i) {
            nodes.pop();
          }
          current = nodes.top();
        }
      }
      u.erase(v_end - u.cbegin());
    }

    // TODO Better option than vector of unordered_sets
    [[nodiscard]] bool confluent() const {
      if (number_of_pending_rules() != 0) {
        confluent(tril::FALSE);
        return false;
      } else if (confluence_known()) {
        return RewriterBase::confluent();
      }

      for (auto it = begin(); it != end(); ++it) {
        if (!backtrack_confluence(*it,
                                  _trie.traverse((*it)->lhs()->cbegin() + 1,
                                                 (*it)->lhs()->cend()),
                                  0)) {
          confluent(tril::FALSE);
          return false;
        }
      }
      // Set cached value
      confluent(tril::TRUE);
      return true;
    }

    void add_rule(Rule* rule) {
      Rules::add_rule(rule);
      add_rule_to_trie(rule);
      confluent(tril::unknown);
    }

    using RewriterBase::add_rule;
    // template <typename StringLike>
    // void add_rule(StringLike const& lhs, StringLike const& rhs) {
    //   if (RewriterBase::add_rule(lhs, rhs)) {
    //     clear_stack();
    //   }
    // }

   private:
    [[nodiscard]] bool backtrack_confluence(Rule const* rule1,
                                            index_type  current_node,
                                            size_t      backtrack_depth) const {
      if (current_node == _trie.root) {
        return true;
      }

      if (_trie.height(current_node) <= backtrack_depth) {
        return true;
      }

      // Don't check for overlaps of rules with lhs size 1
      if (rule1->lhs()->size() == 1) {
        return true;
      }

      if (_trie.node(current_node).is_terminal()) {
        Rule const* rule2 = _rules.find(current_node)->second;
        // Process overlap
        // Word looks like ABC where the LHS of rule1 corresponds to AB,
        // the LHS of rule2 corresponds to BC, and |C|=nodes.size() - 1.
        // AB -> X, BC -> Y
        // ABC gets rewritten to XC and AY
        auto overlap_length
            = rule2->lhs()->length() - (backtrack_depth);  // |B|

        internal_string_type word1;
        internal_string_type word2;

        word1.assign(*rule1->rhs());  // X
        word1.append(rule2->lhs()->cbegin() + overlap_length,
                     rule2->lhs()->cend());  // C

        word2.assign(rule1->lhs()->cbegin(),
                     rule1->lhs()->cend() - overlap_length);  // A
        word2.append(*rule2->rhs());                          // Y

        if (word1 != word2) {
          rewrite(word1);
          rewrite(word2);
          if (word1 != word2) {
            confluent(tril::FALSE);
            return false;
          }
        }
        return true;
      }

      for (auto x = alphabet_cbegin(); x != alphabet_cend(); ++x) {
        if (!backtrack_confluence(
                rule1,
                _trie.traverse_from(current_node, static_cast<letter_type>(*x)),
                backtrack_depth + 1)) {
          return false;
        }
      }
      return true;
    }

    void rewrite(Rule* rule) const {
      rewrite(*rule->lhs());
      rewrite(*rule->rhs());
      rule->reorder();
    }

    // TODO Fix unnecessary alphabet checking
    void add_rule_to_trie(Rule* rule) {
      index_type node = _trie.add_word_no_checks(rule->lhs()->cbegin(),
                                                 rule->lhs()->cend());
      _rules.emplace(node, rule);
      //         for (auto it = rule->lhs()->cbegin(); it !=
      //         rule->lhs()->cend(); ++it) {
      // #ifdef LIBSEMIGROUPS_DEBUG _alphabet.emplace(*it);
      //           LIBSEMIGROUPS_ASSERT(_alphabet.count(*it) == 1);
      // #else
      // _alphabet.emplace(*it);
      // #endif
      //         }
    }

    Rules::iterator erase_from_active_rules(Rules::iterator it) {
      Rule* rule = const_cast<Rule*>(*it);
      rule->deactivate();  // Done in Rules::erase_from
      push_stack(rule);
      index_type node
          = _trie.rm_word_no_checks(rule->lhs()->cbegin(), rule->lhs()->cend());
      _rules.erase(node);
      // TODO Add assertion that checks number of rules stored in trie is
      // equal to number of rules in number_of_active_rules()
      return Rules::erase_from_active_rules(it);
    }
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_REWRITERS_HPP_
