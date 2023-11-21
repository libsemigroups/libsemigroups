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

#ifndef LIBSEMIGROUPS_REWRITE_TRIE_HPP_
#define LIBSEMIGROUPS_REWRITE_TRIE_HPP_

#include "aho-corasick.hpp"
// #include "knuth-bendix.hpp"   // for KnuthBendix
#include "rewriter-base.hpp"  // for Rewriter

namespace libsemigroups {
  class RewriteTrie : public Rewriter {
    using external_string_type = std::string;
    using internal_string_type = std::string;
    using external_char_type   = char;
    using internal_char_type   = char;

    using index_type = AhoCorasick::index_type;

    std::unordered_map<index_type, Rule*> _rules;
    AhoCorasick                           _trie;

   public:
    using Rewriter::confluent;
    using Rules::stats;
    using iterator = internal_string_type::iterator;

    RewriteTrie() : Rewriter(true), _rules(), _trie() {}

    RewriteTrie(const RewriteTrie& that);
    RewriteTrie& operator=(RewriteTrie const& that) {
      init();
      Rewriter::operator=(that);
      for (auto* crule : that) {
        Rule* rule = const_cast<Rule*>(crule);
        add_rule_to_trie(rule);
      }
      return *this;
    }

    ~RewriteTrie() = default;

    RewriteTrie& init() {
      Rewriter::init();
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
        return false;
      } else if (confluence_known()) {
        return Rewriter::confluent();
      }

      // Set cached value
      confluent(tril::TRUE);

      for (auto it = begin(); it != end(); ++it) {
        if (!backtrack_confluence(*it,
                                  _trie.traverse((*it)->lhs()->cbegin() + 1,
                                                 (*it)->lhs()->cend()),
                                  0)) {
          return false;
        }
      }
      return true;
    }

    void add_rule(Rule* rule) {
      Rules::add_rule(rule);
      add_rule_to_trie(rule);
      confluent(tril::unknown);
    }

    void reduce() {
      for (Rule const* rule : *this) {
        // Copy rule and push_stack so that it is not modified by the
        // call to clear_stack.
        LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
        if (push_stack(copy_rule(rule))) {
          clear_stack();
        }
      }
    }

    using Rewriter::add_rule;
    // template <typename StringLike>
    // void add_rule(StringLike const& lhs, StringLike const& rhs) {
    //   if (Rewriter::add_rule(lhs, rhs)) {
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

    // TODO Make use of trie
    void clear_stack() {
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
            // TODO Does this need to happen? Can we ensure rules are always
            // reduced wrt each other?
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
#endif  // LIBSEMIGROUPS_REWRITE_TRIE_HPP_