#include "libsemigroups/rule.hpp"

namespace libsemigroups {
  // Construct from KnuthBendix with new but empty internal_string_type's
  Rule::Rule(int64_t id)
      : _lhs(new internal_string_type()),
        _rhs(new internal_string_type()),
        _id(-1 * id) {
    LIBSEMIGROUPS_ASSERT(_id < 0);
  }

  void Rule::deactivate() noexcept {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    if (active()) {
      _id *= -1;
    }
  }

  void Rule::activate() noexcept {
    LIBSEMIGROUPS_ASSERT(_id != 0);
    if (!active()) {
      _id *= -1;
    }
  }

  bool RuleLookup::operator<(RuleLookup const& that) const {
    auto it_this = _last - 1;
    auto it_that = that._last - 1;
    while (it_this > _first && it_that > that._first && *it_this == *it_that) {
      --it_that;
      --it_this;
    }
    return *it_this < *it_that;
  }

  Rules::Stats::Stats() noexcept {
    init();
  }

  Rules::Stats& Rules::Stats::init() noexcept {
    max_stack_depth        = 0;
    max_word_length        = 0;
    max_active_word_length = 0;
    max_active_rules       = 0;
    min_length_lhs_rule    = std::numeric_limits<size_t>::max();
    total_rules            = 0;
    return *this;
  }

  Rules& Rules::init() {
    // Put all active rules and those rules in the stack into the
    // inactive_rules list
    for (Rule const* cptr : _active_rules) {
      Rule* ptr = const_cast<Rule*>(cptr);
      ptr->deactivate();
      _inactive_rules.insert(_inactive_rules.end(), ptr);
    }
    _active_rules.clear();
    for (auto& it : _cursors) {
      it = _active_rules.end();
    }
    return *this;
  }

  Rules& Rules::operator=(Rules const& that) {
    init();
    for (Rule const* rule : that) {
      add_rule(copy_rule(rule));
    }
    for (size_t i = 0; i < _cursors.size(); ++i) {
      _cursors[i] = _active_rules.begin();
      std::advance(
          _cursors[i],
          std::distance(that.begin(),
                        static_cast<const_iterator>(that._cursors[i])));
    }
    return *this;
  }

  Rules::~Rules() {
    for (Rule const* rule : _active_rules) {
      delete const_cast<Rule*>(rule);
    }
    for (Rule* rule : _inactive_rules) {
      delete rule;
    }
  }

  Rule* Rules::new_rule() {
    ++_stats.total_rules;
    Rule* rule;
    if (!_inactive_rules.empty()) {
      rule = _inactive_rules.front();
      rule->set_id(_stats.total_rules);
      _inactive_rules.erase(_inactive_rules.begin());
    } else {
      rule = new Rule(_stats.total_rules);
    }
    LIBSEMIGROUPS_ASSERT(!rule->active());
    return rule;
  }

  Rule* Rules::copy_rule(Rule const* rule) {
    return new_rule(rule->lhs()->cbegin(),
                    rule->lhs()->cend(),
                    rule->rhs()->cbegin(),
                    rule->rhs()->cend());
  }

  Rules::iterator Rules::erase_from_active_rules(iterator it) {
    // _stats.unique_lhs_rules.erase(*((*it)->lhs()));
    Rule* rule = const_cast<Rule*>(*it);
    rule->deactivate();

    if (it != _cursors[0] && it != _cursors[1]) {
      it = _active_rules.erase(it);
    } else if (it == _cursors[0] && it != _cursors[1]) {
      _cursors[0] = _active_rules.erase(it);
      it          = _cursors[0];
    } else if (it != _cursors[0] && it == _cursors[1]) {
      _cursors[1] = _active_rules.erase(it);
      it          = _cursors[1];
    } else {
      _cursors[0] = _active_rules.erase(it);
      _cursors[1] = _cursors[0];
      it          = _cursors[0];
    }
    return it;
  }

  void Rules::add_rule(Rule* rule) {
    LIBSEMIGROUPS_ASSERT(*rule->lhs() != *rule->rhs());
    _stats.max_word_length
        = std::max(_stats.max_word_length, rule->lhs()->size());
    _stats.max_active_rules
        = std::max(_stats.max_active_rules, number_of_active_rules());
    // _stats.unique_lhs_rules.insert(*rule->lhs());
    rule->activate();
    _active_rules.push_back(rule);
    for (auto& it : _cursors) {
      if (it == end()) {
        --it;
      }
    }
    if (rule->lhs()->size() < _stats.min_length_lhs_rule) {
      // TODO(later) this is not valid when using non-length reducing
      // orderings (such as RECURSIVE)
      _stats.min_length_lhs_rule = rule->lhs()->size();
    }
  }
}  // namespace libsemigroups
