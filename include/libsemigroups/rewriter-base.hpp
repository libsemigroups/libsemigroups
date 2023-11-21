#ifndef LIBSEMIGROUPS_REWRITER_HPP_
#define LIBSEMIGROUPS_REWRITER_HPP_

#include <atomic>         // for atomic
#include <set>            // for set
#include <stack>          // for stack
#include <unordered_set>  // for unordered_set

#include "rule.hpp"   //for Rule, Rules
#include "types.hpp"  // for word_type, tril

namespace libsemigroups {
  using external_string_type = std::string;
  using internal_string_type = std::string;
  using external_char_type   = char;
  using internal_char_type   = char;

  class Rewriter : public Rules {
    std::unordered_set<internal_char_type> _alphabet;
    mutable std::atomic<bool>              _confluent;
    mutable std::atomic<bool>              _confluence_known;
    std::atomic<bool>                      _requires_alphabet;
    std::stack<Rule*>                      _stack;

    using alphabet_citerator
        = std::unordered_set<internal_char_type>::const_iterator;

   public:
    Rewriter() = default;
    Rewriter& init();

    Rewriter(bool requires_alphabet) : Rewriter() {
      _requires_alphabet = requires_alphabet;
    }

    ~Rewriter();

    Rewriter& operator=(Rewriter const& that) {
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

    // TODO Remove bool and swap true for clear_stack?
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
}  // namespace libsemigroups
#endif