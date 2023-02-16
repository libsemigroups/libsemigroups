//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains an implementation of the Todd-Coxeter algorithm for
// semigroups and monoids.

#include "libsemigroups/todd-coxeter-new.hpp"
#include "libsemigroups/obvinf.hpp"

namespace libsemigroups {

  using class_index_type = v3::CongruenceInterface::class_index_type;

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - constructors + initializers - public
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeter::ToddCoxeter()
      : v3::CongruenceInterface(),
        _finished(false),
        _forest(),
        _standardized(order::none),
        _word_graph() {
    _word_graph.definitions().init(this);
  }

  ToddCoxeter& ToddCoxeter::init() {
    v3::CongruenceInterface::init();
    _finished = false;
    _forest.init();
    _standardized = order::none;
    _word_graph.definitions().init(this);
    return *this;
  }

  ToddCoxeter::ToddCoxeter(congruence_kind knd) {
    init(knd);
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind knd) {
    v3::CongruenceInterface::init(knd);
    _finished = false;
    _forest.init();
    _settings     = Settings();
    _standardized = order::none;
    _word_graph.definitions().init(this);
    return *this;
  }

  ToddCoxeter::ToddCoxeter(congruence_kind knd, Presentation<word_type>&& p)
      : v3::CongruenceInterface(knd),
        _finished(false),
        _forest(),
        _settings(),
        _standardized(order::none),
        _word_graph() {
    if (knd == congruence_kind::left) {
      presentation::reverse(p);
    }
    _word_graph.init2(std::move(p));  // FIXME
    _word_graph.definitions().init(this);
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind           knd,
                                 Presentation<word_type>&& p) {
    init(knd);
    if (knd == congruence_kind::left) {
      presentation::reverse(p);
    }
    _word_graph.init2(std::move(p));  // FIXME
    _word_graph.definitions().init(this);
    return *this;
  }

  ToddCoxeter::ToddCoxeter(congruence_kind                knd,
                           Presentation<word_type> const& p)
      : v3::CongruenceInterface(knd),
        _finished(false),
        _forest(),
        _settings(),
        _standardized(order::none),
        _word_graph() {
    if (knd == congruence_kind::left) {
      Presentation<word_type> pp(p);
      presentation::reverse(pp);
      _word_graph.init2(std::move(pp));  // FIXME
    } else {
      _word_graph.init2(p);  // FIXME
    }
    _word_graph.definitions().init(this);
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind                knd,
                                 Presentation<word_type> const& p) {
    init(knd);
    if (knd == congruence_kind::left) {
      Presentation<word_type> pp(p);
      presentation::reverse(pp);
      _word_graph.init2(std::move(pp));  // FIXME
    } else {
      _word_graph.init2(p);  // FIXME
    }
    _word_graph.definitions().init(this);
    return *this;
  }

  ToddCoxeter::ToddCoxeter(congruence_kind knd, ToddCoxeter const& tc) {
    init(knd, tc);
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind knd, ToddCoxeter const& tc) {
    init(knd);
    if (tc.kind() != congruence_kind::twosided && knd != tc.kind()) {
      LIBSEMIGROUPS_EXCEPTION_V3(
          "incompatible types of congruence, found ({} / {}) but only (left "
          "/ left), (right / right), (two-sided / *) are valid",
          tc.kind(),
          knd);
    }
    _word_graph.init2(tc.presentation());  // FIXME
    auto& rules = _word_graph.presentation().rules;
    rules.insert(
        rules.end(), tc.cbegin_generating_pairs(), tc.cend_generating_pairs());
    if (kind() == congruence_kind::left && tc.kind() != congruence_kind::left) {
      presentation::reverse(_word_graph.presentation());
    }
    _word_graph.definitions().init(this);
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - settings - public
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeter& ToddCoxeter::strategy(options::strategy x) {
    _settings.strategy = x;
    return *this;
  }

  ToddCoxeter::options::strategy ToddCoxeter::strategy() const noexcept {
    return _settings.strategy;
  }

  ToddCoxeter& ToddCoxeter::lookahead_next(size_t n) noexcept {
    _settings.lookahead_next = n;
    return *this;
  }

  size_t ToddCoxeter::lookahead_next() const noexcept {
    return _settings.lookahead_next;
  }

  ToddCoxeter& ToddCoxeter::lookahead_min(size_t n) noexcept {
    _settings.lookahead_min = n;
    return *this;
  }

  size_t ToddCoxeter::lookahead_min() const noexcept {
    return _settings.lookahead_min;
  }

  ToddCoxeter& ToddCoxeter::lookahead_growth_factor(float val) {
    if (val < 1.0) {
      LIBSEMIGROUPS_EXCEPTION_V3("Expected a value >= 1.0, found {}", val);
    }
    _settings.lookahead_growth_factor = val;
    return *this;
  }

  float ToddCoxeter::lookahead_growth_factor() const noexcept {
    return _settings.lookahead_growth_factor;
  }

  ToddCoxeter& ToddCoxeter::lookahead_growth_threshold(size_t val) noexcept {
    _settings.lookahead_growth_threshold = val;
    return *this;
  }

  size_t ToddCoxeter::lookahead_growth_threshold() const noexcept {
    return _settings.lookahead_growth_threshold;
  }

  ToddCoxeter& ToddCoxeter::save(bool x) {
    _settings.save = x;
    return *this;
  }

  bool ToddCoxeter::save() const noexcept {
    return _settings.save;
  }

  // TODO not currently used for anything
  ToddCoxeter& ToddCoxeter::standardize(bool x) noexcept {
    _settings.standardize = x;
    return *this;
  }

  // TODO not currently used for anything
  bool ToddCoxeter::standardize() const noexcept {
    return _settings.standardize;
  }

  ToddCoxeter& ToddCoxeter::use_relations_in_extra(bool val) noexcept {
    _settings.use_relations_in_extra = val;
    return *this;
  }

  bool ToddCoxeter::use_relations_in_extra() const noexcept {
    return _settings.use_relations_in_extra;
  }

  ToddCoxeter& ToddCoxeter::lower_bound(size_t n) noexcept {
    _settings.lower_bound = n;
    return *this;
  }

  size_t ToddCoxeter::lower_bound() const noexcept {
    return _settings.lower_bound;
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - accessors - public
  ////////////////////////////////////////////////////////////////////////

  bool ToddCoxeter::contains(word_type const& lhs, word_type const& rhs) {
    validate_word(lhs);
    validate_word(rhs);
    if (presentation().rules.empty()
        && cbegin_generating_pairs() == cend_generating_pairs()
        && word_graph().number_of_nodes_active() == 1) {
      return lhs == rhs;
    }
    return v3::CongruenceInterface::contains(lhs, rhs);
  }

  bool ToddCoxeter::is_standardized(order val) const {
    // TODO this is probably not always valid
    return val == _standardized
           && _forest.number_of_nodes()
                  == word_graph().number_of_nodes_active();
  }

  bool ToddCoxeter::is_standardized() const {
    // TODO this is probably not always valid, i.e. if we are standardized,
    // then grow, then collapse, but end up with the same number of nodes
    // again.
    return _standardized != order::none
           && _forest.number_of_nodes()
                  == word_graph().number_of_nodes_active();
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - modifiers - public
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::shrink_to_fit() {
    if (!finished()) {
      // TODO Throw?
      return;
    }
    standardize(order::shortlex);
    _word_graph.shrink_to_fit(_word_graph.number_of_nodes_active());
    _word_graph.erase_free_nodes();
    _word_graph.restrict(_word_graph.number_of_nodes_active());
  }

  bool ToddCoxeter::standardize(order val) {
    if (is_standardized(val)) {
      return false;
    }
    bool result   = action_digraph::standardize(_word_graph, _forest, val);
    _standardized = val;
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // Runner - pure virtual member functions - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::run_impl() {
    if (is_obviously_infinite(*this)) {
      LIBSEMIGROUPS_EXCEPTION(
          "there are infinitely many classes in the congruence and "
          "Todd-Coxeter will never terminate");
    }
    if (report::should_report()) {
      fmt::print(
          FORMAT("#0: ToddCoxeter: using {} strategy . . .\n", strategy()));
    }

    init_run();

    if (strategy() == options::strategy::felsch) {
      felsch();
    } else if (strategy() == options::strategy::hlt) {
      hlt();
    }

    finalise_run();

    /*else if (strategy() == options::strategy::random) {
      if (running_for()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the strategy \"%s\" is incompatible with run_for!",
            detail::to_string(strategy()).c_str());
      }
      random();
    } else {
      if (running_until()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the strategy \"%s\" is incompatible with run_until!",
            detail::to_string(strategy()).c_str());
      }

      if (strategy() == options::strategy::CR) {
        CR_style();
      } else if (strategy() == options::strategy::R_over_C) {
        R_over_C_style();
      } else if (strategy() == options::strategy::Cr) {
        Cr_style();
      } else if (strategy() == options::strategy::Rc) {
        Rc_style();
      }
    }*/
  }

  ////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - pure virtual member functions - private
  ////////////////////////////////////////////////////////////////////////

  word_type ToddCoxeter::class_index_to_word_impl(class_index_type i) {
    run();
    LIBSEMIGROUPS_ASSERT(finished());
    if (!is_standardized()) {
      standardize(order::shortlex);
    }

    word_type w = *(forest::cbegin_paths(_forest) + i + 1);
    if (kind() != congruence_kind::left) {
      std::reverse(w.begin(), w.end());
    }
    return w;
  }

  size_t ToddCoxeter::number_of_classes_impl() {
    if (is_obviously_infinite(*this)) {
      return POSITIVE_INFINITY;
    }
    run();
    size_t const offset = (presentation().contains_empty_word() ? 0 : 1);
    return _word_graph.number_of_nodes_active() - offset;
  }

  class_index_type ToddCoxeter::word_to_class_index_impl(word_type const& w) {
    run();
    LIBSEMIGROUPS_ASSERT(finished());
    if (!is_standardized()) {
      standardize(order::shortlex);
    }
    return const_word_to_class_index(w);
    // c is in the range 1, ..., number_of_cosets_active() because 0
    // represents the identity coset, and does not correspond to an element.
  }

  class_index_type
  ToddCoxeter::const_word_to_class_index(word_type const& w) const {
    validate_word(w);
    node_type c = _word_graph.initial_node();

    if (kind() == congruence_kind::left) {
      c = action_digraph_helper::follow_path_nc(
          _word_graph, c, w.crbegin(), w.crend());
    } else {
      c = action_digraph_helper::follow_path_nc(
          _word_graph, c, w.cbegin(), w.cend());
    }
    return (c == UNDEFINED ? UNDEFINED : static_cast<node_type>(c - 1));
  }

  void ToddCoxeter::validate_word(word_type const& w) const {
    presentation().validate_word(w.cbegin(), w.cend());
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - main strategies - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::init_run() {
    if (is_obviously_infinite(*this)) {
      LIBSEMIGROUPS_EXCEPTION(
          "there are infinitely many classes in the congruence and "
          "Todd-Coxeter will never terminate");
    }

    _word_graph.settings(*this);
    auto       first = cbegin_generating_pairs();
    auto       last  = cend_generating_pairs();
    auto const id    = word_graph().initial_node();
    if (save() || strategy() == options::strategy::felsch) {
      for (auto it = first; it < last; it += 2) {
        _word_graph.push_definition_hlt<RegisterDefs>(id, *it, *(it + 1));
        _word_graph.process_coincidences<RegisterDefs>();
      }
    } else {
      for (auto it = first; it < last; it += 2) {
        _word_graph.push_definition_hlt<DoNotRegisterDefs>(id, *it, *(it + 1));
        _word_graph.process_coincidences<DoNotRegisterDefs>();
      }
    }
    if (strategy() == options::strategy::felsch && use_relations_in_extra()) {
      first = presentation().rules.cbegin();
      last  = presentation().rules.cend();

      for (auto it = first; it < last; it += 2) {
        _word_graph.push_definition_hlt<RegisterDefs>(id, *it, *(it + 1));
        _word_graph.process_coincidences<RegisterDefs>();
      }
    }

    if (kind() == congruence_kind::twosided
        && cbegin_generating_pairs() != cend_generating_pairs()) {
      // TODO avoid copy of presentation here
      Presentation<word_type> p = presentation();
      if (p.alphabet().size() != _word_graph.out_degree()) {
        LIBSEMIGROUPS_ASSERT(p.alphabet().size() == 0);
        p.alphabet(_word_graph.out_degree());
      }
      presentation::add_rules(
          p, cbegin_generating_pairs(), cend_generating_pairs());
      _word_graph.presentation(std::move(p));
    }

    if (save() || strategy() == options::strategy::felsch) {
      _word_graph.process_definitions();
    }
  }

  void ToddCoxeter::finalise_run() {
    if (!stopped()) {
      if (_word_graph.definitions().any_skipped()) {
        auto const& d = word_graph();
        if (d.number_of_nodes_active() != lower_bound() + 1
            || !action_digraph::is_complete(
                d, d.cbegin_active_nodes(), d.cend_active_nodes())) {
          //  TODO uncomment
          //  push_settings();
          lookahead_extent(options::lookahead_extent::full);
          lookahead_style(options::lookahead_style::hlt);
          perform_lookahead();
          // pop_settings();
          // }
        }
      }
      _finished = true;
    }
  }
  void ToddCoxeter::felsch() {
    _word_graph.process_definitions();

    auto& current  = _word_graph.cursor();
    current        = _word_graph.initial_node();
    size_t const n = _word_graph.out_degree();
    while (current != _word_graph.first_free_node() && !stopped()) {
      for (letter_type a = 0; a < n; ++a) {
        if (_word_graph.unsafe_neighbor(current, a) == UNDEFINED) {
          _word_graph.def_edge_nc<RegisterDefs>(
              current, a, _word_graph.new_node());
          _word_graph.process_definitions();
        }
      }
      current = _word_graph.next_active_node(current);
      if (report()) {
        report_active_nodes();
      }
    }
    if (!stopped()) {
      _finished = true;
    }
  }

  void ToddCoxeter::hlt() {
    // bool do_pop_settings = false;
    // if (save() && preferred_defs() == options::preferred_defs::deferred)
    // {
    //   push_settings();
    //   do_pop_settings = true;
    //   // The call to process_deductions in the main loop below could
    //   // potentially accummulate large numbers of preferred definitions
    //   in
    //   // the queue if the preferred_defs() setting is
    //   // options::preferred_defs::deferred, so we change it.
    //   preferred_defs(options::preferred_defs::none);
    // }

    auto& current    = _word_graph.cursor();
    current          = _word_graph.initial_node();
    auto const first = presentation().rules.cbegin();
    auto const last  = presentation().rules.cend();
    while (current != _word_graph.first_free_node() && !stopped()) {
      // if (!save()) {
      if (!save()) {
        for (auto it = first; it < last; it += 2) {
          // TODO check that the DoNotRegisterDefs honoured
          _word_graph.push_definition_hlt<DoNotRegisterDefs>(
              current, *it, *(it + 1));
          _word_graph.process_coincidences<DoNotRegisterDefs>();
        }
      } else {
        for (auto it = first; it < last; it += 2) {
          // TODO check that the RegisterDefs honoured
          _word_graph.push_definition_hlt<RegisterDefs>(
              current, *it, *(it + 1));
          _word_graph.process_definitions();
        }
      }
      // if (standardize()) {
      //   bool any_changes = false;
      //   for (letter_type x = 0; x < n; ++x) {
      //     any_changes |= standardize_immediate(_current, x);
      //   }
      //   if (any_changes) {
      //     _deduct->clear();
      //   }
      // }
      // if ((!save() || _word_graph.deductions().any_skipped())
      if (_word_graph.number_of_nodes_active() > lookahead_next()) {
        // If save() == true and no deductions were skipped, then we have
        // already run process_deductions, and so there's no point in doing a
        // lookahead.
        perform_lookahead();
      }
      if (report()) {
        report_active_nodes();
      }
      current = _word_graph.next_active_node(current);
      // if (do_pop_settings) {
      //   pop_settings();
      // }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - reporting - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::report_active_nodes() const {
    using detail::group_digits;

    if (report::should_report()) {
      fmt::print(
          FORMAT("#0: ToddCoxeter: nodes {:>11} (active) | {:>11} (killed) | "
                 "{:>11} (defined)\n",
                 group_digits(word_graph().number_of_nodes_active()),
                 group_digits(word_graph().number_of_nodes_killed()),
                 group_digits(word_graph().number_of_nodes_defined())));
    }
  }

  using class_index_type = v3::CongruenceInterface::class_index_type;

  void ToddCoxeter::report_next_lookahead(size_t old_value) const {
    if (report::should_report()) {
      static const std::string pad(8, ' ');
      int64_t                  diff = int64_t(lookahead_next()) - old_value;
      fmt::print(
          FORMAT("#0: ToddCoxeter: next lookahead at {0} | {1:>11} (nodes)  "
                 "|{2:>12} (diff)\n",
                 pad,
                 fmt::group_digits(lookahead_next()),
                 detail::group_digits(diff)));
    }
  }

  void ToddCoxeter::report_nodes_killed(int64_t N) const {
    if (report::should_report()) {
      fmt::print(FORMAT(
          "#0: ToddCoxeter: lookahead complete with    | {:>11} (killed) |\n",
          detail::group_digits(-1 * N)));
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - lookahead - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::perform_lookahead() {
    if (report::should_report()) {
      fmt::print("{:-<90}\n", "");
      fmt::print("#0: ToddCoxeter: performing {} {} lookahead . . .\n",
                 lookahead_extent(),
                 lookahead_style());
    }
    auto& current = _word_graph.lookahead_cursor();

    if (lookahead_extent() == options::lookahead_extent::partial) {
      // Start lookahead from the coset after _current
      current = _word_graph.next_active_node(_word_graph.cursor());
    } else {
      LIBSEMIGROUPS_ASSERT(lookahead_extent()
                           == options::lookahead_extent::full);
      current = _word_graph.initial_node();
    }
    size_t num_killed_by_me = 0;
    if (lookahead_style() == options::lookahead_style::hlt) {
      num_killed_by_me = hlt_lookahead();
    } else {
      LIBSEMIGROUPS_ASSERT(lookahead_style()
                           == options::lookahead_style::felsch);
      num_killed_by_me = felsch_lookahead();
    }

    report_nodes_killed(num_killed_by_me);

    size_t const num_nodes = _word_graph.number_of_nodes_active();

    size_t const old_lookahead_next = lookahead_next();

    if (num_nodes < (lookahead_next() / lookahead_growth_factor())
        && lookahead_next() > lookahead_min()) {
      // If the lookahead_next is much bigger than the current number of
      // nodes, then reduce the next lookahead.

      lookahead_next(lookahead_growth_factor() * num_nodes);
    } else if (num_nodes > lookahead_next()
               || num_killed_by_me
                      < (num_nodes / lookahead_growth_threshold())) {
      // Otherwise, if we already exceed the lookahead_next or too few
      // nodes were killed, then increase the next lookahead.
      _settings.lookahead_next *= lookahead_growth_factor();
    }
    report_next_lookahead(old_lookahead_next);
    if (report::should_report()) {
      fmt::print("{:-<90}\n", "");
    }
  }

  size_t ToddCoxeter::hlt_lookahead() {
    report_active_nodes();
    // _stats.hlt_lookahead_calls++; TODO re-enable

    size_t const old_number_of_killed = _word_graph.number_of_nodes_killed();
    _word_graph.make_compatible(presentation().rules.cbegin(),
                                presentation().rules.cend());
    _word_graph.process_coincidences<DoNotRegisterDefs>();
    if (report()) {
      report_active_nodes();
    }
    return _word_graph.number_of_nodes_killed() - old_number_of_killed;
  }

  size_t ToddCoxeter::felsch_lookahead() {
    report_active_nodes();
    //    _stats.f_lookahead_calls++;
    size_t const old_number_of_killed = _word_graph.number_of_nodes_killed();
    node_type&   current              = _word_graph.lookahead_cursor();
    size_t const n                    = _word_graph.out_degree();

    while (current != _word_graph.first_free_node()) {
      _word_graph.definitions().clear();
      for (size_t a = 0; a < n; ++a) {
        _word_graph.definitions().emplace_back(current, a);
      }
      _word_graph.process_definitions();
      current = _word_graph.next_active_node(current);
      if (report()) {
        report_active_nodes();
      }
    }
    return _word_graph.number_of_nodes_killed() - old_number_of_killed;
  }

}  // namespace libsemigroups
