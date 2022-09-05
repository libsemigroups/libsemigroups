//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 Euan Lacy
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

#ifndef LIBSEMIGROUPS_KNUTH_BENDIX_HPP_
#define LIBSEMIGROUPS_KNUTH_BENDIX_HPP_

#include "libsemigroups/cong-intf.hpp"
#include "libsemigroups/string-view.hpp"
#include "libsemigroups/types.hpp"

#include <cstdint>
#include <iostream>
#include <unordered_map>

namespace libsemigroups {
namespace knuthbendix {

// /// This defines the interface to a lookup strategy.
// /// 
// /// @brief Lookup strategies provide methods to rewrite strings, and to update
// /// their internal data structures when rules are inserted and removed.
// /// 
// /// Lookups are intended to take and store references to the rule list, active
// /// list and number of active rules.
// template<class L>
// concept Lookup = requires(
//     L l,
//     const std::vector<Rule>& rules,
//     const std::vector<bool>& active,
//     const int& n_active,
//     const Rule& rule,
//     size_t idx,
//     std::string& string
// ) {
//     { L(rules, active, n_active, 100) } -> std::same_as<L>;
//     /// Insert a new rule, @p rule, to internal state with index @p idx in @ref
//     /// rules.
//     l.insert(rule, idx);
//     /// Removes the rule @p rule from internal state.
//     l.remove(rule);
//     /// Exhaustively rewrite the string using the active rules.
//     l.rewrite(string);
// };

/// @brief A rule contains two strings, and defines a mapping from the left to the right.
struct Rule {
    /// The left hand side of the mapping, is greater than the rhs under shortlex 
    std::string left;
    /// The right hand side of the mapping, is less than the lhs under shortlex 
    std::string right;

    Rule() = delete;

    Rule(std::string const& _left, std::string const& _right) 
        : left(std::move(_left))
        , right(std::move(_right)) 
    {};

    Rule(std::string&& _left, std::string&& _right) 
        : left(std::move(_left))
        , right(std::move(_right)) 
    {};

    void print_rule() const {
        for (auto c : left) {
            std::cout << (int) c;
        }

        std::cout << " -> ";

        for (auto c : right) {
            std::cout << (int) c;
        }
        std::cout << std::endl;
    }

    bool should_reorder() const {
        if (this->left.length() < this->right.length()) return true;
        if (this->left.length() > this->right.length()) return false;
        return this->left < this->right;
    }

    /// @brief Swaps left and right such that left <= right under
    ///        length-lexicographical ordering.
    void order() {
        if (should_reorder()) {
            std::swap(this->left, this->right);
        }
    }
};

template <size_t ALPHABET_SIZE>
struct TrieLookup {
    using RuleIndex = size_t;

    struct OptionalIndex {
        bool present {false};
        RuleIndex index {0};
    };

    struct TrieNode;

    // TODO: custom allocator which allocates TrieNodes contiguously for better
    //       cache performance.
    using TrieNodePtr = std::unique_ptr<TrieNode>;

    struct TrieNode {
        std::array<TrieNodePtr, ALPHABET_SIZE> children; 
        OptionalIndex index;
    };

    const std::vector<Rule>& rules;
    const std::vector<bool>& active;
    const int& n_active;

    std::unique_ptr<TrieNode> root;

    TrieLookup() = delete;

    TrieLookup(
        const std::vector<Rule>& rules,
        const std::vector<bool>& active,
        const int& n_active
    ) : rules(rules), active(active), n_active(n_active) {
        this->root = std::make_unique<TrieNode>();
    }

    OptionalIndex find(const detail::StringView string) {
        OptionalIndex index;
        TrieNode* trie = this->root.get();

        for (char c : string) {
            if (!trie->children[c]) {
                return index;
            }
            trie = trie->children[c].get();
            if (trie->index.present) {
                index = trie->index;
            }
        }

        return index;
    }

    void rewrite(std::string& string) {
        size_t pos = 0;

        while (pos != string.length()) {
            OptionalIndex index = find(
                detail::StringView {string.cbegin() + pos, string.cend()}
            );

            if (index.present) {
                const Rule& rule = this->rules[index.index];
                // Copy the rhs of the rule
                string.replace(pos, rule.right.length(), rule.right);

                // Delete the difference between the lhs and rhs of the rule
                string.erase(pos + rule.right.length(), rule.left.length() - rule.right.length());

                pos = 0;
            } else {
                pos++;
            }
        }
    }

    void insert(const Rule& rule, RuleIndex idx) {
        TrieNode* trie = this->root.get();

        for (char c : rule.left) {
            TrieNodePtr& ptr = trie->children[c];

            if (!ptr) {
                ptr = std::make_unique<TrieNode>();
            }

            trie = ptr.get();
        }

        trie->index.index = idx;
        trie->index.present = true;
    }

    // Assumes the rule must exist
    void remove(const Rule& rule) {
        TrieNode* trie = this->root.get();

        for (char c : rule.left) {
            trie = trie->children[c].get();
        }

        trie->index.present = false;
    }
};

/// @brief true if first starts with second
inline bool starts_with(detail::StringView first, detail::StringView second) {
    if (second.size() > first.size()) return false;

    auto it1 = first.begin();
    auto it2 = second.begin();

    while (it2 != second.end()) {
        if (*it1 != *it2) return false;

        it1++;
        it2++;
    }

    return true;
}

/// @brief Contains the state of the Knuth-Bendix procedure.
template<class LookUp, size_t ALPHABET_SIZE>
struct KnuthBendixState {
    /// The stack of rules which are yet to be added to the list of active rules.
    std::vector<Rule> stack;

    // TODO: This grows continuously, could pause the procedure and prune after
    //       a number of inactive rules?
    /// The list of all rules which have been active. 
    std::vector<Rule> rules;

    /// If an entry is true, then the corresponding entry in @ref rules is active.
    std::vector<bool> active;

    /// The number of active rules.
    int n_active {0};

    /// The rule lookup struct.
    LookUp lookup;

    /// @brief Returns whether the rule at index @p i in @ref rules is active.
    inline bool is_active(size_t i) const {
        return this->active[i];
    }

    /// @brief Adds a new rule to the list of active rules.
    void add_rule(Rule&& rule) {
        this->active.push_back(true);
        this->n_active += 1;
        this->rules.push_back(std::move(rule));

        this->lookup.insert(this->rules[rules.size() - 1], rules.size() - 1);
    }

    /// @brief Removes a rule from the list of active rules and places it on the stack.
    void remove_rule(Rule&& to_check, size_t i) {
        this->active[i] = false;
        this->n_active -= 1;
        this->stack.push_back(to_check);

        this->lookup.remove(to_check);
    }

    /// @brief Pops each rule from the stack, adds it to the rewriting system,
    /// and removes newly redundent rules.
    void clear_stack() {
        while (!this->stack.empty()) {
            Rule rule = std::move(this->stack.back());
            this->stack.pop_back();
            this->lookup.rewrite(rule.left);
            this->lookup.rewrite(rule.right);

            if (rule.left != rule.right) {
                rule.order();
                this->add_rule(std::move(rule));
                Rule& rule_ = this->rules.back();

                for (size_t i = 0; i < this->rules.size() - 1; i++) {
                    if (this->is_active(i)) {
                        Rule& to_check = this->rules[i];
                        if (to_check.left.find(rule_.left) != std::string::npos) {
                            this->remove_rule(std::move(to_check), i);
                        } else if (to_check.right.find(rule_.left) != std::string::npos) {
                            this->lookup.rewrite(to_check.right);
                        }
                    }
                }
            }
        }
    }

    /// @brief checks for all overlaps between the end of the rule at index @p i
    /// and the rule at index @p j. Creates a new rule if rewriting the
    /// resulting string differs using the two rules.
    void overlap(size_t i, size_t j) {
        for (size_t k = 1; k < std::min(rules[i].left.size(), rules[j].left.size()); k++) {
            Rule& a = this->rules[i];
            Rule& b = this->rules[j];
            detail::StringView suffix(a.left.begin() + (a.left.length() - k), a.left.end());

            if (starts_with(detail::StringView {b.left.cbegin(), b.left.cend()}, suffix)) {
                // a.left = XY, b.left = YZ
                // xbr = X|b.right, arz = a.right|Z
                std::string xbr;
                xbr.reserve(a.left.length() - k + b.right.length());
                xbr.append(a.left, 0, a.left.length() - k);
                xbr.append(b.right);

                std::string arz;
                arz.reserve(a.right.length() + b.left.length() - k);
                arz.append(a.right);
                arz.append(b.left, k, b.left.length() - k);

                this->stack.emplace_back(xbr, arz);
                this->clear_stack();

                if (!(this->is_active(i) && this->is_active(j))) {
                    return;
                }
            }
        }
    }

    KnuthBendixState()
        : lookup(rules, active, n_active)
    {}

    KnuthBendixState(const std::vector<Rule>& initial_rules)
        : lookup(rules, active, n_active, initial_rules.size())
    {
        this->stack.reserve(initial_rules.size());
        this->rules.reserve(initial_rules.size());
        this->active.reserve(initial_rules.size());
        this->n_active = 0;

        for (const Rule& rule : initial_rules) {
            this->stack.push_back(rule);
            this->clear_stack();
        }
    }
};

template<class LookUp, size_t ALPHABET_SIZE>
class KnuthBendix final : public CongruenceInterface {
private:
    bool finished {false};

    /// Internal Knuth-Bendix procedure state.
    KnuthBendixState<LookUp, ALPHABET_SIZE> state;
    /* /// Maps from internal character representation to normal characters. */
    /* std::vector<char> internal_to_char; */
    /* /// Maps from characters to the internal representation. */
    /* std::unordered_map<char, size_t> char_to_internal; */

    /* std::string to_internal(const std::string& string) { */
    /*     std::string internal; */
    /*     internal.reserve(string.length()); */

    /*     for (auto c : string) { */
    /*         internal.push_back(char_to_internal[c]); */
    /*     } */

    /*     return internal; */
    /* } */

    /* std::string from_internal(const std::string& internal) { */
    /*     std::string external; */
    /*     external.reserve(internal.length()); */

    /*     for (auto c : internal) { */
    /*         external.push_back(internal_to_char[c]); */
    /*     } */

    /*     return external; */
    /* } */

    void init() {
        for (auto it = cbegin_generating_pairs(); it < cend_generating_pairs(); it++) {
            std::string left;
            for (auto c : it->first) {
                left.push_back(c);
            }
            std::string right;
            for (auto c : it->second) {
                right.push_back(c);
            }
            this->state.stack.push_back(Rule { left, right});
            this->state.clear_stack();
        }
    }

public:
    /* void set_alphabet(std::string char_list) { */
    /*     LIBSEMIGROUPS_ASSERT(char_list.length() <= ALPHABET_SIZE) */

    /*     for (auto c : char_list) { */
    /*         internal_to_char.push_back(c); */
    /*         char_to_internal.insert({c, internal_to_char.size() - 1}); */
    /*     } */
    /* } */

    /* void add_rule(std::string left, std::string right) { */
    /*     for (auto c : left) { */
    /*         LIBSEMIGROUPS_ASSERT(char_to_internal.find(c) != char_to_internal.end()); */
    /*     } */

    /*     auto rule = relation_type {to_internal(left), to_internal(right)}; */
    /*     state.stack.push_back(rule); */
    /* } */

    KnuthBendix(congruence_kind kind)
        : CongruenceInterface(kind)
    {
        LIBSEMIGROUPS_ASSERT(kind == congruence_kind::twosided);
    };

    void set_alphabet(size_t alphabet_size) {}

    bool confluent() {
        return finished;
    }

    bool finished_impl() const override {
        return finished;
    }

    void run_impl() override {
        init();
        state.clear_stack();

        for (size_t i = 0; i < state.rules.size(); i++) {
            for (size_t j = 0; j <= i && state.is_active(i); j++) {
                if (state.is_active(j)) {
                    state.overlap(i, j);
                }

                if (j < i && state.is_active(j) && state.is_active(i)) {
                    state.overlap(j, i);
                }
            }
        }

        finished = true;

        std::cout << "number of active rules: " << state.n_active << std::endl;
    }

    // TODO finish impls
    word_type class_index_to_word_impl(class_index_type i) override {
        return word_type {};
    }

    class_index_type word_to_class_index_impl(word_type const& w) override {
        return 0;
    }

    size_t number_of_classes_impl() override {
        return 0;
    }

    bool is_quotient_obviously_finite_impl() override {
        return false;
    }

    std::shared_ptr<FroidurePinBase> quotient_impl() override {
        return nullptr;
    }
};

}

template<size_t N>
using KnuthBendix = knuthbendix::KnuthBendix<knuthbendix::TrieLookup<N>, N>;
}

#endif
