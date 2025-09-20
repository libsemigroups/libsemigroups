//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
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

// This file contains the implementation of the Stephen class template.

namespace libsemigroups {

  template <typename PresentationType>
  class Stephen<PresentationType>::StephenGraph
      : public detail::NodeManagedGraph<uint32_t> {
    using WordGraphWithSources_ = detail::WordGraphWithSources<uint32_t>;

   public:
    using node_type = typename WordGraphWithSources_::node_type;
    using WordGraphWithSources_::init;

    StephenGraph& init(Presentation<word_type> const& p) {
      NodeManager<node_type>::clear();
      WordGraphWithSources_::init(NodeManager<node_type>::node_capacity(),
                                  p.alphabet().size());
      return *this;
    }

    StephenGraph& init(Presentation<word_type>&& p) {
      NodeManager<node_type>::clear();
      WordGraphWithSources_::init(NodeManager<node_type>::node_capacity(),
                                  p.alphabet().size());
      return *this;
    }

    using WordGraph<node_type>::target;
    using WordGraph<node_type>::target_no_checks;

    // TODO(2) Add data to the StephenGraph which is a pointer to either the
    // encompassing Stephen object or the presentation
    void target_no_checks(presentation_type const& p,
                          node_type                from,
                          label_type               letter,
                          node_type                to) {
      WordGraphWithSources_::target_no_checks(from, letter, to);
      if constexpr (is_specialization_of_v<presentation_type,
                                           InversePresentation>) {
        // convert l (which is an index)
        // -> actual letter
        // -> inverse of letter
        // -> index of inverse of letter
        auto ll             = p.index(p.inverse(p.letter(letter)));
        auto inverse_target = target_no_checks(to, ll);
        if (inverse_target != UNDEFINED && inverse_target != from) {
          merge_nodes_no_checks(from, inverse_target);
          process_coincidences();
          return;
        }
        WordGraphWithSources_::target_no_checks(to, ll, from);
      }
    }

    // TODO(2) Add data to the StephenGraph which is a pointer to either the
    // encompassing Stephen object or the presentation
    std::pair<bool, node_type>
    complete_path(presentation_type const&  p,
                  node_type                 c,
                  word_type::const_iterator first,
                  word_type::const_iterator last) noexcept {
      if (first == last) {
        return std::make_pair(false, c);
      }
      LIBSEMIGROUPS_ASSERT(first < last);
      word_type::const_iterator it;
      std::tie(c, it)
          = word_graph::last_node_on_path_no_checks(*this, c, first, last);
      bool result = false;
      for (; it < last; ++it) {
        node_type d = target_no_checks(c, *it);
        if (d == UNDEFINED) {
          d = new_node();
          StephenGraph::target_no_checks(p, c, *it, d);
          result = true;
        }
        c = d;
      }
      return std::make_pair(result, c);
    }

    void disjoint_union_inplace_no_checks(StephenGraph& that) {
      word_graph::standardize(that);
      LIBSEMIGROUPS_ASSERT(word_graph::is_standardized(that));
      size_t const N = number_of_nodes_active();
      // TODO(2): the following 2 lines are a bit awkward
      WordGraphWithSources_::add_nodes(that.number_of_nodes_active());
      NodeManager<node_type>::add_active_nodes(that.number_of_nodes_active());

      for (node_type n = 0; n < that.number_of_nodes_active(); ++n) {
        for (auto a : that.labels()) {
          auto m = that.target_no_checks(n, a);
          if (m != UNDEFINED) {
            WordGraphWithSources_::target_no_checks(n + N, a, m + N);
          }
        }
      }
    }
  };

  template <typename PresentationType>
  Stephen<PresentationType>::Stephen()
      : _accept_state(UNDEFINED),
        _finished(false),
        _is_word_set(false),
        _presentation(std::make_shared<PresentationType>()),
        _word(),
        _word_graph() {
    report_prefix("Stephen");
    _word_graph.report_prefix("Stephen");
    report_divider(fmt::format("{:+<32}\n", ""));
  }

  template <typename PresentationType>
  Stephen<PresentationType>& Stephen<PresentationType>::init() {
    _accept_state = UNDEFINED;
    _finished     = false;
    _is_word_set  = false;
    _presentation->init();
    _word.clear();
    _word_graph.init();
    report_prefix("Stephen");
    _word_graph.report_prefix("Stephen");
  }

  template <typename PresentationType>
  Stephen<PresentationType>& Stephen<PresentationType>::init(
      std::shared_ptr<PresentationType> const& ptr) {
    // TODO(1): move the commented out lines to the
    // make<Stephen<PresentationType>> function, once we have it
    // ptr->throw_if_bad_alphabet_or_rules();
    // throw_if_presentation_empty(*ptr);
    _presentation = ptr;
    _accept_state = UNDEFINED;
    _finished     = false;
    _is_word_set  = false;
    _word.clear();
    return *this;
  }

  template <typename PresentationType>
  Stephen<PresentationType>::~Stephen() = default;

  ////////////////////////////////////////////////////////////////////////
  // Public
  ////////////////////////////////////////////////////////////////////////

  template <typename PresentationType>
  template <typename Iterator1, typename Iterator2>
  Stephen<PresentationType>&
  Stephen<PresentationType>::set_word_no_checks(Iterator1 first,
                                                Iterator2 last) {
    _accept_state = UNDEFINED;
    _finished     = false;
    _word.assign(first, last);
    init_word_graph_from_word_no_checks();
    _is_word_set = true;
    return *this;
  }

  template <typename PresentationType>
  typename Stephen<PresentationType>::node_type
  Stephen<PresentationType>::accept_state() {
    throw_if_not_ready();
    if (_accept_state == UNDEFINED) {
      using word_graph::last_node_on_path_no_checks;
      run();
      _accept_state = last_node_on_path_no_checks(
                          _word_graph, 0, _word.cbegin(), _word.cend())
                          .first;
    }
    return _accept_state;
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::operator*=(Stephen<PresentationType>& that) {
    throw_if_not_ready();
    that.throw_if_not_ready();
    if (this->presentation() != that.presentation()) {
      LIBSEMIGROUPS_EXCEPTION(
          "this.presentation() must equal that.presentation() when appending "
          "Stephen instances")
    }

    append_no_checks(that);
  }

  template <typename PresentationType>
  void
  Stephen<PresentationType>::append_no_checks(Stephen<PresentationType>& that) {
    // NOTE: Appending seems to work fine without running both this and that.
    // If for some reason this is no longer true later on, uncomment the next
    // two lines and update docs to indicate algorithm is run when calling
    // append.

    // this->run();
    // that.run();

    // TODO (2) FIXME _word_graph has two mem fns number_nodes_active (in
    // NodeManager) and number_active_nodes (in WordGraph), this is super
    // confusing!
    size_t const N = _word_graph.number_of_nodes_active();
    _word_graph.disjoint_union_inplace_no_checks(that._word_graph);
    _word_graph.merge_nodes_no_checks(accept_state(), that.initial_state() + N);
    _word_graph.process_coincidences();
    _accept_state = UNDEFINED;
    _finished     = false;
    _word.insert(_word.end(), that._word.cbegin(), that._word.cend());
    _word_graph.cursor() = initial_state();
  }

  ////////////////////////////////////////////////////////////////////////
  // Private Member Functions
  ////////////////////////////////////////////////////////////////////////

  template <typename PresentationType>
  void Stephen<PresentationType>::throw_if_not_ready() const {
    if (presentation().alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the presentation must be defined using init() before "
          "calling this function");
    } else if (!_is_word_set) {
      LIBSEMIGROUPS_EXCEPTION("the word must be set using set_word() before "
                              "calling this function");
    }
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::report_before_run() const {
    if (reporting_enabled()) {
      report_no_prefix(report_divider());
      report_default("Stephen: STARTING . . .\n");
      report_default("Stephen: |w| = {}, {}",
                     word().size(),
                     presentation::to_report_string(presentation()));
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Reporting
  ////////////////////////////////////////////////////////////////////////

  template <typename PresentationType>
  void Stephen<PresentationType>::report_after_run() const {
    if (reporting_enabled()) {
      using detail::group_digits;
      report_no_prefix(report_divider());
      report_default("Stephen: found word graph with |V| = {} and |E| = {}\n",
                     group_digits(_word_graph.number_of_nodes_active()),
                     group_digits(_word_graph.number_of_edges()));

      report_default("Stephen: STOPPING -- {}\n",
                     finished() ? "finished" : string_why_we_stopped());
    }
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::report_progress_from_thread() const {
    using detail::group_digits;
    using detail::signed_group_digits;
    using detail::string_time;

    auto run_time = delta(start_time());

    auto const active  = _word_graph.number_of_nodes_active();
    auto const killed  = _word_graph.number_of_nodes_killed();
    auto const defined = _word_graph.number_of_nodes_defined();

    auto const& stats = _word_graph.stats();
    auto const  active_diff
        = signed_group_digits(active - stats.prev_active_nodes);
    auto const killed_diff
        = signed_group_digits(killed - stats.prev_nodes_killed);
    auto const defined_diff
        = signed_group_digits(defined - stats.prev_nodes_defined);

    auto const mean_killed
        = group_digits(std::pow(10, 9) * static_cast<double>(killed)
                       / run_time.count())
          + "/s";
    auto const mean_defined
        = group_digits(std::pow(10, 9) * static_cast<double>(defined)
                       / run_time.count())
          + "/s";

    std::string_view const line1
        = "{}: nodes {} (active) | {} (killed) | {} (defined)\n";

    detail::ReportCell<4> rc;
    rc.min_width(11).min_width(0, report_prefix().size());
    report_no_prefix(report_divider());
    rc(line1,
       report_prefix(),
       group_digits(active),
       group_digits(killed),
       group_digits(defined));
    rc("{}: diff  {} (active) | {} (killed) | {} (defined)\n",
       report_prefix(),
       active_diff,
       killed_diff,
       defined_diff);
    rc("{}: time  {} (total)  | {} (killed) | {} (defined)\n",
       report_prefix(),
       string_time(run_time),
       mean_killed,
       mean_defined);
    _word_graph.stats_check_point();
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::run_impl() {
    throw_if_not_ready();
    reset_start_time();

    report_before_run();
    if (reporting_enabled()) {
      // TODO(2): Thread sanitizer reports some sort of race condition here.
      // It's not really an issue since its just reporting, but would be nice
      // to silence the sanitizer. ToddCoxeter seems to do the same thing and
      // no sanitizer issues there, so what gives?
      detail::Ticker t([this]() { report_progress_from_thread(); });
      really_run_impl();
    } else {
      really_run_impl();
    }
    report_after_run();
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::really_run_impl() {
    node_type& current     = _word_graph.cursor();
    auto const rules_begin = presentation().rules.cbegin();
    auto const rules_end   = presentation().rules.cend();
    bool       did_change  = true;

    do {
      current    = 0;
      did_change = false;
      while (current != _word_graph.first_free_node() && !stopped()) {
        for (auto it = rules_begin; it < rules_end; it += 2) {
          node_type                 u_end;
          word_type::const_iterator rit;
          bool                      did_def = false;
          std::tie(u_end, rit) = word_graph::last_node_on_path_no_checks(
              _word_graph, current, it->cbegin(), it->cend());
          node_type c, v_end;
          if (rit == it->cend()) {
            ++it;
            if (it->empty()) {
              did_def = false;
              c       = current;
              v_end   = c;
            } else {
              std::tie(did_def, c) = _word_graph.complete_path(
                  presentation(), current, it->cbegin(), it->cend() - 1);
              v_end = _word_graph.target_no_checks(c, it->back());
            }
            if (v_end == UNDEFINED) {
              LIBSEMIGROUPS_ASSERT(!it->empty());
              did_def = true;
              _word_graph.target_no_checks(
                  presentation(), c, it->back(), u_end);
            } else if (u_end != v_end) {
              did_def = true;
              _word_graph.merge_nodes_no_checks(u_end, v_end);
              _word_graph.process_coincidences();
            }
            --it;
          } else {
            ++it;
            std::tie(v_end, rit) = word_graph::last_node_on_path_no_checks(
                _word_graph, current, it->cbegin(), it->cend());
            if (rit == it->cend()) {
              --it;
              if (it->empty()) {
                did_def = false;
                c       = current;
                u_end   = c;
              } else {
                std::tie(did_def, c) = _word_graph.complete_path(
                    presentation(), current, it->cbegin(), it->cend() - 1);
                u_end = _word_graph.target_no_checks(c, it->back());
              }
              if (u_end == UNDEFINED) {
                LIBSEMIGROUPS_ASSERT(!it->empty());
                did_def = true;
                _word_graph.target_no_checks(
                    presentation(), c, it->back(), v_end);
              } else if (u_end != v_end) {
                did_def = true;
                _word_graph.merge_nodes_no_checks(u_end, v_end);
                _word_graph.process_coincidences();
              }
            } else {
              --it;
            }
          }
          did_change |= did_def;
        }
        current = _word_graph.next_active_node(current);
      }
    } while (did_change && !stopped());
    if (!stopped()) {
      _finished = true;
      standardize();
    }
    // Here so we have accurate data when using to_human_readable_repr
    _word_graph.number_of_active_nodes(_word_graph.number_of_nodes_active());
  }

  namespace stephen {

    template <typename PresentationType>
    bool accepts(Stephen<PresentationType>& s, word_type const& w) {
      using word_graph::follow_path;
      using node_type = typename Stephen<PresentationType>::node_type;
      s.run();
      LIBSEMIGROUPS_ASSERT(s.accept_state() != UNDEFINED);
      return s.accept_state() == follow_path(s.word_graph(), node_type(0), w);
    }

    template <typename PresentationType>
    bool is_left_factor(Stephen<PresentationType>& s, word_type const& w) {
      using word_graph::last_node_on_path;
      using node_type = typename Stephen<PresentationType>::node_type;
      s.run();
      return last_node_on_path(
                 s.word_graph(), node_type(0), w.cbegin(), w.cend())
                 .second
             == w.cend();
    }

    template <typename PresentationType>
    Dot dot(Stephen<PresentationType>& s) {
      Dot result;
      result.kind(Dot::Kind::digraph);
      result.add_node("initial").add_attr("style", "invis");
      result.add_node("accept").add_attr("style", "invis");
      for (auto n : s.word_graph().nodes()) {
        result.add_node(n).add_attr("shape", "box");
      }
      result.add_edge("initial", s.initial_state());
      result.add_edge(s.accept_state(), "accept");

      size_t max_letters = s.presentation().alphabet().size();
      if constexpr (is_specialization_of_v<PresentationType,
                                           InversePresentation>) {
        max_letters /= 2;
      }

      for (auto n : s.word_graph().nodes()) {
        for (size_t a = 0; a < max_letters; ++a) {
          auto m = s.word_graph().target(n, a);
          if (m != UNDEFINED) {
            result.add_edge(n, m)
                .add_attr("color", result.colors[a])
                .add_attr("label", a)
                .add_attr("minlen", 2);
          }
        }
      }
      return result;
    }

  }  // namespace stephen

  template <typename PresentationType>
  std::string to_human_readable_repr(Stephen<PresentationType> const& x) {
    using detail::group_digits;
    if (!x.is_word_set()) {
      return fmt::format("<Stephen object over {} with no word set>",
                         to_human_readable_repr(x.presentation()));
    }
    return fmt::format(
        "<Stephen object over {} for {} with {} "
        "nodes and {} edges>",
        to_human_readable_repr(x.presentation()),
        x.word().size() < 10
            ? fmt::format("word {}", x.word())
            : fmt::format("{} letter word", group_digits(x.word().size())),
        group_digits(x.word_graph().number_of_active_nodes()),
        group_digits(x.word_graph().number_of_edges()));
  }

}  // namespace libsemigroups
