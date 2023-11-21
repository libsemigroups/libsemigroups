#ifndef LIBSEMIGROUPS_RULE_HPP_
#define LIBSEMIGROUPS_RULE_HPP_

#include <string>     // for basic_string, operator==
                      //
#include "debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "order.hpp"  // for shortlex_compare

#include "rx/ranges.hpp"  // for iterator_range

namespace libsemigroups {
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
}  // namespace libsemigroups
#endif