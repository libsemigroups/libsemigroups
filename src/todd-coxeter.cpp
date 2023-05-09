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

#include "libsemigroups/todd-coxeter.hpp"

#include "libsemigroups/obvinf.hpp"

#include "libsemigroups/detail/report.hpp"

namespace libsemigroups {
  using node_type = typename ToddCoxeter::node_type;

  ////////////////////////////////////////////////////////////////////////
  // Digraph
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeter::Digraph&
  ToddCoxeter::Digraph::init(Presentation<word_type> const& p) {
    NodeManager<node_type>::clear();
    FelschGraph_::init(p);
    // FIXME shouldn't add nodes here because then there'll be more than
    // there should be (i.e. NodeManager and FelschGraph_ will have
    // different numbers of nodes
    FelschGraph_::add_nodes(NodeManager<node_type>::node_capacity());
    return *this;
  }

  ToddCoxeter::Digraph&
  ToddCoxeter::Digraph::init(Presentation<word_type>&& p) {
    NodeManager<node_type>::clear();
    FelschGraph_::init(std::move(p));
    // FIXME shouldn't add nodes here because then there'll be more than
    // there should be (i.e. NodeManager and FelschGraph_ will have
    // different numbers of nodes
    FelschGraph_::add_nodes(NodeManager<node_type>::node_capacity());
    return *this;
  }

  void ToddCoxeter::Digraph::process_definitions() {
    CollectCoincidences incompat(_coinc);
    using NoPreferredDefs = typename FelschGraph_::NoPreferredDefs;
    NoPreferredDefs pref_defs;

    auto&      defs = FelschGraph_::definitions();
    Definition d;
    while (!defs.empty()) {
      while (!defs.empty()) {
        defs.pop(d);
        if (NodeManager<node_type>::is_active_node(d.first)) {
          FelschGraph_::process_definition(d, incompat, pref_defs);
        }
      }
      process_coincidences<RegisterDefs>();
    }
  }

  template <bool RegDefs>
  void ToddCoxeter::Digraph::push_definition_hlt(node_type const& c,
                                                 word_type const& u,
                                                 word_type const& v) noexcept {
    LIBSEMIGROUPS_ASSERT(NodeManager<node_type>::is_active_node(c));

    node_type   x, y;
    letter_type a, b;

    if (!u.empty()) {
      x = complete_path<RegDefs>(c, u.begin(), u.cend() - 1).second;
      a = u.back();
    } else {
      x = c;
      a = UNDEFINED;
    }

    if (!v.empty()) {
      y = complete_path<RegDefs>(c, v.begin(), v.cend() - 1).second;
      b = v.back();
    } else {
      y = c;
      b = UNDEFINED;
    }

    CollectCoincidences incompat(_coinc);
    auto                pref_defs
        = [this](node_type x, letter_type a, node_type y, letter_type b) {
            node_type d = new_node();
            set_target_no_checks<RegDefs>(x, a, d);
            if (a != b || x != y) {
              set_target_no_checks<RegDefs>(y, b, d);
            }
          };

    FelschGraph_::merge_targets_of_nodes_if_possible<RegDefs>(
        x, a, y, b, incompat, pref_defs);
  }

  template <typename RuleIterator>
  size_t ToddCoxeter::Digraph::make_compatible(node_type&   current,
                                               RuleIterator first,
                                               RuleIterator last,
                                               bool         stop_early) {
    detail::Timer t;
    size_t const  old_number_of_killed
        = NodeManager<node_type>::number_of_nodes_killed();
    CollectCoincidences                    incompat(_coinc);
    typename FelschGraph_::NoPreferredDefs prefdefs;
    while (current != NodeManager<node_type>::first_free_node()) {
      // TODO(later) when we have an RuleIterator into the active nodes, we
      // should remove the while loop, and use that in make_compatible
      // instead. At present there is a cbegin/cend_active_nodes in
      // NodeManager but the RuleIterators returned by them are invalidated by
      // any changes to the graph, such as those made by
      // felsch_graph::make_compatible.
      felsch_graph::make_compatible<DoNotRegisterDefs>(
          *this, current, current + 1, first, last, incompat, prefdefs);
      // Using NoPreferredDefs is just a (more or less) arbitrary
      // choice, could allow the other choices here too (which works,
      // but didn't seem to be very useful).
      process_coincidences<DoNotRegisterDefs>();
      current = NodeManager<node_type>::next_active_node(current);
      if (stop_early && t > std::chrono::seconds(1)) {
        // TODO setting for this
        size_t killed_last_second
            = number_of_nodes_killed() - stats().prev_nodes_killed;
        if (killed_last_second < number_of_nodes_active() / 100) {
          report_default("ToddCoxeter: too few nodes killed, expected >= "
                         "{}, found {}, aborting lookahead . . .\n",
                         number_of_nodes_active() / 100,
                         killed_last_second);
          report_no_prefix("{:-<93}\n", "");
          break;
        }
        report_active_nodes();
      }
    }
    return NodeManager<node_type>::number_of_nodes_killed()
           - old_number_of_killed;
  }

  ////////////////////////////////////////////////////////////////////////
  // Definitions
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::Definitions::emplace_back(node_type c, label_type x) {
    using def_policy = typename options::def_policy;

    if (_tc == nullptr  // this can be the case if for example we're
                        // in the FelschGraph constructor from
                        // WordGraph, in that case we any want
                        // all of the definitions
        || _tc->def_policy() == def_policy::unlimited
        || _definitions.size() < _tc->def_max()) {
      _definitions.emplace_back(c, x);
      return;
    }

    // We will skip the input definition (c, x)!
    _any_skipped = true;
    switch (_tc->def_policy()) {
      case def_policy::purge_from_top: {
        while (!_definitions.empty()
               && !is_active_node(_definitions.back().first)) {
          _definitions.pop_back();
        }
        break;
      }
      case def_policy::purge_all: {
        std::remove_if(
            _definitions.begin(),
            _definitions.end(),
            [this](Definition const& d) { return !is_active_node(d.first); });
        break;
      }
      case def_policy::discard_all_if_no_space: {
        clear();
        break;
      }
      default: {
        break;
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - constructors + initializers - public
  ////////////////////////////////////////////////////////////////////////

  // TODO remove overlap between these, and ensure that thye really work

  ToddCoxeter::ToddCoxeter()
      : CongruenceInterface(),
        _finished(false),
        _forest(),
        _standardized(Order::none),
        _word_graph() {
    // This is where we pass through from _settings to the
    // _word_graph.definitions
    _word_graph.definitions().init(this);
    _word_graph.set_prefix("ToddCoxeter");
  }

  ToddCoxeter& ToddCoxeter::init() {
    CongruenceInterface::init();
    _finished = false;
    _forest.init();
    _standardized = Order::none;
    _word_graph.definitions().init(this);
    _word_graph.set_prefix("ToddCoxeter");
    return *this;
  }

  ToddCoxeter::ToddCoxeter(congruence_kind knd) {
    init(knd);
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind knd) {
    CongruenceInterface::init(knd);
    _finished = false;
    _forest.init();
    _settings     = Settings();
    _standardized = Order::none;
    _word_graph.definitions().init(this);
    _word_graph.set_prefix("ToddCoxeter");
    return *this;
  }

  ToddCoxeter::ToddCoxeter(congruence_kind knd, Presentation<word_type>&& p)
      : CongruenceInterface(knd),
        _finished(false),
        _forest(),
        _settings(),
        _standardized(Order::none),
        _word_graph() {
    if (knd == congruence_kind::left) {
      presentation::reverse(p);
    }
    _word_graph.init(std::move(p));
    _word_graph.definitions().init(this);
    _word_graph.set_prefix("ToddCoxeter");
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind           knd,
                                 Presentation<word_type>&& p) {
    init(knd);
    if (knd == congruence_kind::left) {
      presentation::reverse(p);
    }
    _word_graph.init(std::move(p));
    _word_graph.definitions().init(this);
    _word_graph.set_prefix("ToddCoxeter");
    return *this;
  }

  ToddCoxeter::ToddCoxeter(congruence_kind                knd,
                           Presentation<word_type> const& p)
      : CongruenceInterface(knd),
        _finished(false),
        _forest(),
        _settings(),
        _standardized(Order::none),
        _word_graph() {
    p.validate();  // TODO this is probably missing all over the place here
    if (knd == congruence_kind::left) {
      Presentation<word_type> pp(p);
      presentation::reverse(pp);
      _word_graph.init(std::move(pp));
    } else {
      _word_graph.init(p);
    }
    _word_graph.definitions().init(this);
    _word_graph.set_prefix("ToddCoxeter");
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind                knd,
                                 Presentation<word_type> const& p) {
    init(knd);
    if (knd == congruence_kind::left) {
      Presentation<word_type> pp(p);
      presentation::reverse(pp);
      _word_graph.init(std::move(pp));
    } else {
      _word_graph.init(p);
    }
    _word_graph.definitions().init(this);
    _word_graph.set_prefix("ToddCoxeter");
    return *this;
  }

  ToddCoxeter::ToddCoxeter(congruence_kind knd, ToddCoxeter const& tc) {
    init(knd, tc);
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind knd, ToddCoxeter const& tc) {
    init(knd);
    if (tc.kind() != congruence_kind::twosided && knd != tc.kind()) {
      LIBSEMIGROUPS_EXCEPTION(
          "incompatible types of congruence, found ({} / {}) but only (left "
          "/ left), (right / right), (two-sided / *) are valid",
          tc.kind(),
          knd);
    }
    _word_graph.init(tc.presentation());
    auto& rules = _word_graph.presentation().rules;
    rules.insert(rules.end(),
                 tc.generating_pairs().cbegin(),
                 tc.generating_pairs().cend());
    if (kind() == congruence_kind::left && tc.kind() != congruence_kind::left) {
      presentation::reverse(_word_graph.presentation());
    }
    _word_graph.definitions().init(this);
    _word_graph.set_prefix("ToddCoxeter");
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
      LIBSEMIGROUPS_EXCEPTION("Expected a value >= 1.0, found {}", val);
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

  ToddCoxeter& ToddCoxeter::large_collapse(size_t n) noexcept {
    _word_graph.large_collapse(n);
    return *this;
  }

  size_t ToddCoxeter::large_collapse() const noexcept {
    return _word_graph.large_collapse();
  }

  ToddCoxeter& ToddCoxeter::hlt_defs(size_t val) {
    auto l = presentation::length(presentation());
    if (val == 0) {
      LIBSEMIGROUPS_EXCEPTION("Expected a value != 0!");
    } else if (val < l) {
      LIBSEMIGROUPS_EXCEPTION("Expected a value >= {}, found {}!", l, val);
    }
    _settings.hlt_defs = val;
    return *this;
  }

  size_t ToddCoxeter::hlt_defs() const noexcept {
    return _settings.hlt_defs;
  }

  ToddCoxeter& ToddCoxeter::f_defs(size_t val) {
    if (val == 0) {
      LIBSEMIGROUPS_EXCEPTION("Expected a value != 0!");
    }
    _settings.f_defs = val;
    return *this;
  }

  size_t ToddCoxeter::f_defs() const noexcept {
    return _settings.f_defs;
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - accessors - public
  ////////////////////////////////////////////////////////////////////////

  bool ToddCoxeter::contains(word_type const& lhs, word_type const& rhs) {
    validate_word(lhs);
    validate_word(rhs);
    if (presentation().rules.empty() && generating_pairs().empty()
        && word_graph().number_of_nodes_active() == 1) {
      return lhs == rhs;
    }
    return lhs == rhs || word_to_class_index(lhs) == word_to_class_index(rhs);
  }

  bool ToddCoxeter::is_standardized(Order val) const {
    // TODO this is probably not always valid
    return val == _standardized
           && _forest.number_of_nodes()
                  == word_graph().number_of_nodes_active();
  }

  bool ToddCoxeter::is_standardized() const {
    // TODO this is probably not always valid, i.e. if we are standardized,
    // then grow, then collapse, but end up with the same number of nodes
    // again.
    return _standardized != Order::none
           && _forest.number_of_nodes()
                  == word_graph().number_of_nodes_active();
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - modifiers - public
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::shrink_to_fit() {
    if (!finished()) {
      return;
    }
    standardize(Order::shortlex);
    _word_graph.erase_free_nodes();
    _word_graph.induced_subgraph_no_checks(
        0, _word_graph.number_of_nodes_active());
  }

  bool ToddCoxeter::standardize(Order val) {
    if (is_standardized(val)) {
      return false;
    }
    bool result   = word_graph::standardize(_word_graph, _forest, val);
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
    auto msg = fmt::format("{:-<93}\n", "");
    msg += fmt_default("ToddCoxeter: using {} strategy . . .\n", strategy());
    msg += fmt::format("{:-<93}\n", "");
    report_no_prefix(msg);

    init_run();

    if (strategy() == options::strategy::felsch) {
      felsch();
    } else if (strategy() == options::strategy::hlt) {
      hlt();
    } else {
      if (running_until()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the strategy {} cannot be used with run_until!", strategy());
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
    }
    finalise_run();
  }

  ////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - pure virtual member functions - private
  ////////////////////////////////////////////////////////////////////////

  word_type ToddCoxeter::class_index_to_word_impl(node_type i) {
    run();
    LIBSEMIGROUPS_ASSERT(finished());
    if (!is_standardized()) {
      standardize(Order::shortlex);
    }
    if (!presentation().contains_empty_word()) {
      ++i;
    }

    word_type w = _forest.path_to_root(i);
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

  node_type ToddCoxeter::word_to_class_index_impl(word_type const& w) {
    run();
    LIBSEMIGROUPS_ASSERT(finished());
    if (!is_standardized()) {
      standardize(Order::shortlex);
    }
    return const_word_to_class_index(w);
    // c is in the range 1, ..., number_of_cosets_active() because 0
    // represents the identity coset, and does not correspond to an element,
    // unless presentation().contains_empty_word()
  }

  node_type ToddCoxeter::const_word_to_class_index(word_type const& w) const {
    validate_word(w);
    node_type c = _word_graph.initial_node();

    if (kind() != congruence_kind::left) {
      c = word_graph::follow_path_no_checks(
          _word_graph, c, w.cbegin(), w.cend());
    } else {
      c = word_graph::follow_path_no_checks(
          _word_graph, c, w.crbegin(), w.crend());
    }
    size_t const offset = (presentation().contains_empty_word() ? 0 : 1);
    return (c == UNDEFINED ? UNDEFINED : static_cast<node_type>(c - offset));
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
    _word_graph.stats().start_time = std::chrono::high_resolution_clock::now();
    _word_graph.stats_check_point();
    auto       first = generating_pairs().cbegin();
    auto       last  = generating_pairs().cend();
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

    if (kind() == congruence_kind::twosided && !generating_pairs().empty()) {
      // TODO(later) avoid copy of presentation here, if possible
      Presentation<word_type> p = presentation();
      if (p.alphabet().size() != _word_graph.out_degree()) {
        LIBSEMIGROUPS_ASSERT(p.alphabet().size() == 0);
        p.alphabet(_word_graph.out_degree());
      }
      presentation::add_rules(
          p, generating_pairs().cbegin(), generating_pairs().cend());
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
        if (d.number_of_nodes_active() != lower_bound()
            || !word_graph::is_complete(
                d, d.cbegin_active_nodes(), d.cend_active_nodes())) {
          // TODO uncomment
          // push_settings();
          lookahead_extent(options::lookahead_extent::full);
          lookahead_style(options::lookahead_style::hlt);
          perform_lookahead(DoNotStopEarly);
          // pop_settings();
          // }
        }
      }
      _word_graph.report_active_nodes();
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
        if (_word_graph.target_no_checks(current, a) == UNDEFINED) {
          _word_graph.set_target_no_checks<RegisterDefs>(
              current, a, _word_graph.new_node());
          _word_graph.process_definitions();
        }
      }
      current = _word_graph.next_active_node(current);
    }
    if (!stopped()) {
      _finished = true;
    }
  }

  void ToddCoxeter::hlt() {
    auto& current    = _word_graph.cursor();
    current          = _word_graph.initial_node();
    auto const first = presentation().rules.cbegin();
    auto const last  = presentation().rules.cend();
    while (current != _word_graph.first_free_node() && !stopped()) {
      if (!save()) {
        for (auto it = first; it < last; it += 2) {
          _word_graph.push_definition_hlt<DoNotRegisterDefs>(
              current, *it, *(it + 1));
          _word_graph.process_coincidences<DoNotRegisterDefs>();
        }
      } else {
        for (auto it = first; it < last; it += 2) {
          _word_graph.push_definition_hlt<RegisterDefs>(
              current, *it, *(it + 1));
          _word_graph.process_definitions();
        }
      }
      if ((!save() || _word_graph.definitions().any_skipped())
          && (_word_graph.number_of_nodes_active() > lookahead_next())) {
        // If save() == true and no deductions were skipped, then we have
        // already run process_definitions, and so there's no point in doing a
        // lookahead.
        perform_lookahead(StopEarly);
      } else if (report()) {
        word_graph().report_active_nodes();
      }
      current = _word_graph.next_active_node(current);
    }
  }

  void ToddCoxeter::CR_style() {
    size_t N = presentation::length(presentation());
    // push_settings();
    while (!finished()) {
      strategy(options::strategy::felsch);
      auto M = _word_graph.number_of_nodes_active();
      run_until([this, &M]() -> bool {
        return word_graph().number_of_nodes_active() >= M + f_defs();
      });
      word_graph().report_active_nodes();
      if (finished()) {
        break;
      }
      strategy(options::strategy::hlt);
      M = _word_graph.number_of_nodes_active();
      run_until([this, &M, &N]() -> bool {
        return word_graph().number_of_nodes_active() >= M + (hlt_defs() / N);
      });
      word_graph().report_active_nodes();
    }
    lookahead_extent(options::lookahead_extent::full);
    lookahead_style(options::lookahead_style::hlt);
    perform_lookahead(DoNotStopEarly);
    // pop_settings();
  }

  void ToddCoxeter::R_over_C_style() {
    // push_settings();
    strategy(options::strategy::hlt);
    run_until([this]() -> bool {
      return word_graph().number_of_nodes_active() >= lookahead_next();
    });
    lookahead_extent(options::lookahead_extent::full);
    perform_lookahead(StopEarly);
    CR_style();
    // pop_settings();
  }

  void ToddCoxeter::Cr_style() {
    // push_settings();
    strategy(options::strategy::felsch);
    auto M = word_graph().number_of_nodes_active();
    run_until([this, &M]() -> bool {
      return word_graph().number_of_nodes_active() >= M + f_defs();
    });
    word_graph().report_active_nodes();
    strategy(options::strategy::hlt);
    M        = word_graph().number_of_nodes_active();
    size_t N = presentation::length(presentation());
    run_until([this, &M, &N]() -> bool {
      return word_graph().number_of_nodes_active() >= (hlt_defs() / N) + M;
    });
    word_graph().report_active_nodes();
    strategy(options::strategy::felsch);
    run();
    lookahead_extent(options::lookahead_extent::full);
    lookahead_style(options::lookahead_style::hlt);
    perform_lookahead(DoNotStopEarly);
    // pop_settings();
  }

  void ToddCoxeter::Rc_style() {
    // push_settings();
    strategy(options::strategy::hlt);
    auto   M = word_graph().number_of_nodes_active();
    size_t N = presentation::length(presentation());
    run_until([this, &M, &N]() -> bool {
      return word_graph().number_of_nodes_active() >= (hlt_defs() / N) + M;
    });
    word_graph().report_active_nodes();
    strategy(options::strategy::felsch);
    M = word_graph().number_of_nodes_active();
    run_until([this, &M]() -> bool {
      return word_graph().number_of_nodes_active() >= f_defs() + M;
    });
    word_graph().report_active_nodes();
    strategy(options::strategy::hlt);
    run();
    lookahead_extent(options::lookahead_extent::full);
    lookahead_style(options::lookahead_style::hlt);
    perform_lookahead(DoNotStopEarly);
    // pop_settings();
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - reporting - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::report_next_lookahead(size_t old_value) const {
    static const std::string pad(8, ' ');
    int64_t                  diff = int64_t(lookahead_next()) - old_value;
    report_default("ToddCoxeter: next lookahead at {}  | {:>12} (nodes)  "
                   "| {:>12} (diff)\n",
                   pad,
                   fmt::group_digits(lookahead_next()),
                   detail::signed_group_digits(diff));
    report_no_prefix("{:-<93}\n", "");
  }

  void ToddCoxeter::report_nodes_killed(int64_t N) const {
    report_default(
        "ToddCoxeter: lookahead complete with     | {:>12} (killed) |\n",
        detail::group_digits(-1 * N));
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - lookahead - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::perform_lookahead(bool stop_early) {
    report_default("ToddCoxeter: performing {} {} lookahead . . .\n",
                   lookahead_extent(),
                   lookahead_style());
    report_no_prefix("{:-<93}\n", "");
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
      num_killed_by_me = hlt_lookahead(stop_early);
    } else {
      LIBSEMIGROUPS_ASSERT(lookahead_style()
                           == options::lookahead_style::felsch);
      num_killed_by_me = felsch_lookahead();
    }

    report_nodes_killed(num_killed_by_me);

    size_t const num_nodes = _word_graph.number_of_nodes_active();

    size_t const old_lookahead_next = lookahead_next();

    if (num_nodes < (lookahead_next() / lookahead_growth_factor())) {
      // If the lookahead_next is much bigger than the current number of
      // nodes, then reduce the next lookahead.
      if (lookahead_growth_factor() * num_nodes > lookahead_min()) {
        lookahead_next(lookahead_growth_factor() * num_nodes);
      } else if (lookahead_next() / lookahead_growth_factor()
                 > lookahead_min()) {
        lookahead_next(lookahead_next() / lookahead_growth_factor());
      } else {
        lookahead_next(lookahead_min());
      }
    } else if (num_nodes > lookahead_next()
               || num_killed_by_me
                      < (num_nodes / lookahead_growth_threshold())) {
      // Otherwise, if we already exceed the lookahead_next or too few
      // nodes were killed, then increase the next lookahead.
      _settings.lookahead_next *= lookahead_growth_factor();
    }
    report_next_lookahead(old_lookahead_next);
  }

  size_t ToddCoxeter::hlt_lookahead(bool stop_early) {
    _word_graph.report_active_nodes();

    size_t const old_number_of_killed = _word_graph.number_of_nodes_killed();
    _word_graph.make_compatible(_word_graph.lookahead_cursor(),
                                presentation().rules.cbegin(),
                                presentation().rules.cend(),
                                stop_early);
    return _word_graph.number_of_nodes_killed() - old_number_of_killed;
  }

  size_t ToddCoxeter::felsch_lookahead() {
    _word_graph.report_active_nodes();
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
        _word_graph.report_active_nodes();
      }
    }
    return _word_graph.number_of_nodes_killed() - old_number_of_killed;
  }

  namespace todd_coxeter {
    tril is_non_trivial(ToddCoxeter&              tc,
                        size_t                    tries,
                        std::chrono::milliseconds try_for,
                        float                     threshold) {
      using detail::node_managed_graph::random_active_node;

      if (is_obviously_infinite(tc)) {
        return tril::TRUE;
      } else if (tc.finished()) {
        return tc.number_of_classes() == 1 ? tril::FALSE : tril::TRUE;
      }

      for (size_t try_ = 0; try_ < tries; ++try_) {
        report_default(
            "trying to show non-triviality: {} / {}\n", try_ + 1, tries);
        ToddCoxeter copy(tc);
        copy.save(true);
        while (!copy.finished()) {
          copy.run_for(try_for);
          size_t limit = copy.word_graph().number_of_nodes_active();
          while (copy.word_graph().number_of_nodes_active() >= threshold * limit
                 && !copy.finished()) {
            node_type c1 = random_active_node(copy.word_graph());
            node_type c2 = random_active_node(copy.word_graph());
            auto&     wg
                = const_cast<ToddCoxeter::digraph_type&>(copy.word_graph());
            wg.merge_nodes_no_checks(c1, c2);
            wg.process_coincidences<RegisterDefs>();
            wg.process_definitions();
            copy.run_for(try_for);
          }
        }
        if (copy.number_of_classes() > 1) {
          report_default("successfully showed non-triviality!\n");
          return tril::TRUE;
        }
      }
      report_default("failed to show non-triviality!\n");
      return tril::unknown;
    }
  }  // namespace todd_coxeter
}  // namespace libsemigroups
