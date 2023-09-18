//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2023 James D. Mitchell
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

  template <typename P>
  class Stephen<P>::StephenGraph
      : public detail::NodeManagedGraph<WordGraphWithSources<uint32_t>> {
    using BaseGraph = WordGraphWithSources<uint32_t>;

   public:
    using node_type = typename BaseGraph::node_type;

    StephenGraph& init(Presentation<word_type> const& p) {
      NodeManager<node_type>::clear();
      BaseGraph::init(NodeManager<word_graph_type::node_type>::node_capacity(),
                      p.alphabet().size());
      return *this;
    }

    StephenGraph& init(Presentation<word_type>&& p) {
      NodeManager<node_type>::clear();
      BaseGraph::init(NodeManager<node_type>::node_capacity(),
                      p.alphabet().size());
      return *this;
    }
  };

  template <typename P>
  Stephen<P>::Stephen()
      : _accept_state(UNDEFINED),
        _finished(false),
        _presentation(),
        _word(),
        _word_graph() {}

  template <typename P>
  Stephen<P>& Stephen<P>::init() {
    _accept_state = UNDEFINED;
    _finished     = false;
    _presentation.init();
    _word.clear();
    _word_graph.init();
  }

  template <typename P>
  Stephen<P>::Stephen(P const& p) : Stephen() {
    init(p);
  }

  template <typename P>
  Stephen<P>::Stephen(P&& p) : Stephen() {
    init(std::move(p));
  }

  template <typename P>
  Stephen<P>& Stephen<P>::init(P&& p) {
    validate(deref_if_necessary(p));
    _presentation = std::move(p);
    return init_after_presentation_set();
  }

  template <typename P>
  Stephen<P>& Stephen<P>::init(P const& p) {
    validate(deref_if_necessary(p));
    _presentation = p;
    return init_after_presentation_set();
  }

  template <typename P>
  Stephen<P>& Stephen<P>::init_after_presentation_set() {
    reset();
    _word_graph.init(presentation());
    _word.clear();
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Public
  ////////////////////////////////////////////////////////////////////////

  template <typename P>
  Stephen<P>& Stephen<P>::set_word(word_type const& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    reset();
    _word = w;
    return *this;
  }

  template <typename P>
  Stephen<P>& Stephen<P>::set_word(word_type&& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    reset();
    _word = std::move(w);
    return *this;
  }

  template <typename P>
  typename Stephen<P>::word_graph_type::node_type Stephen<P>::accept_state() {
    if (_accept_state == UNDEFINED) {
      using word_graph::last_node_on_path_no_checks;
      run();
      _accept_state = last_node_on_path_no_checks(
                          _word_graph, 0, _word.cbegin(), _word.cend())
                          .first;
    }
    return _accept_state;
  }

  ////////////////////////////////////////////////////////////////////////
  // Private Member Functions
  ////////////////////////////////////////////////////////////////////////

  // TODO rename to set_target_
  // TODO reorder args
  template <typename P>
  void Stephen<P>::def_edge(StephenGraph&              wg,
                            word_graph_type::node_type from,
                            word_graph_type::node_type to,
                            label_type                 letter,
                            presentation_type const&   p) const {
    if constexpr (!IsInversePresentation<presentation_type>) {
      wg.set_target_no_checks(from, letter, to);
    } else {
      wg.set_target_no_checks(from, letter, to);
      // convert l (which is an index)
      // -> actual letter
      // -> inverse of letter
      // -> index of inverse of letter
      auto ll             = p.index(p.inverse(p.letter(letter)));
      auto inverse_target = wg.target_no_checks(to, ll);
      if (inverse_target != UNDEFINED && inverse_target != from) {
        wg.merge_nodes_no_checks(from, inverse_target);
        wg.template process_coincidences<DoNotRegisterDefs>();
        return;
      }
      wg.set_target_no_checks(to, ll, from);
    }
  }

  template <typename P>
  std::pair<bool, typename Stephen<P>::word_graph_type::node_type>
  Stephen<P>::complete_path(StephenGraph&              wg,
                            word_graph_type::node_type c,
                            word_type::const_iterator  first,
                            word_type::const_iterator  last) noexcept {
    if (first == last) {
      return std::make_pair(false, c);
    }
    LIBSEMIGROUPS_ASSERT(first < last);
    word_type::const_iterator it;
    std::tie(c, it)
        = word_graph::last_node_on_path_no_checks(wg, c, first, last);
    bool result = false;
    for (; it < last; ++it) {
      word_graph_type::node_type d = wg.target_no_checks(c, *it);
      if (d == UNDEFINED) {
        d = wg.new_node();
        def_edge(wg, c, d, *it, presentation());
        result = true;
      }
      c = d;
    }
    return std::make_pair(result, c);
  }

  template <typename P>
  void Stephen<P>::report_status(
      std::chrono::high_resolution_clock::time_point const& start_time) {
    if (!report()) {
      return;
    }
    using std::chrono::duration_cast;
    using std::chrono::seconds;
    auto now        = std::chrono::high_resolution_clock::now();
    auto total_time = duration_cast<seconds>(now - start_time);

    auto&   stats = _word_graph.stats();
    int64_t diff  = int64_t(_word_graph.number_of_nodes_active()
                           - stats.prev_active_nodes);

    // TODO(v3) use fmtlib
    static bool first_call = true;
    if (first_call) {
      first_call = false;
      REPORT_DEFAULT_V3("Stephen: " + std::string(60, '-') + "\n");
      REPORT_DEFAULT_V3("Stephen: %11s | %11s | %11s | %11s |\n",
                        "nodes",
                        "defined",
                        "killed",
                        "diff");
      REPORT_DEFAULT_V3("Stephen: " + std::string(60, '-') + "\n");
    }
    using ::libsemigroups::detail::group_digits;
    REPORT_DEFAULT_V3(
        "Stephen: %11s | %11s | %11s | %11s | "
        "(%llus)\n",
        group_digits(_word_graph.number_of_nodes_active()).c_str(),
        ("+"
         + group_digits(int64_t(_word_graph.number_of_nodes_defined()
                                - stats.prev_nodes_defined)))
            .c_str(),
        ("-"
         + group_digits(int64_t(_word_graph.number_of_nodes_killed()
                                - stats.prev_nodes_killed)))
            .c_str(),
        ((diff < 0 ? "" : "+") + group_digits(diff)).c_str(),
        total_time.count());
    _word_graph.stats_check_point();
  }

  template <typename P>
  void Stephen<P>::reset() noexcept {
    _finished     = false;
    _accept_state = UNDEFINED;
  }

  template <typename P>
  void Stephen<P>::standardize() {
    word_graph::standardize(_word_graph);
    _word_graph.induced_subgraph_no_checks(
        0, _word_graph.number_of_nodes_active());
  }

  template <typename P>
  void Stephen<P>::validate(presentation_type const& p) const {
    p.validate();
    if (p.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("the presentation must not have 0 generators");
    }
  }

  template <typename P>
  void Stephen<P>::run_impl() {
    auto start_time = std::chrono::high_resolution_clock::now();
    validate(presentation());  // throws if no presentation is defined
    _word_graph.init(deref_if_necessary(presentation()));
    complete_path(_word_graph, 0, _word.cbegin(), _word.cend());
    word_graph_type::node_type& current = _word_graph.cursor();
    auto const rules_begin = deref_if_necessary(presentation()).rules.cbegin();
    auto const rules_end   = deref_if_necessary(presentation()).rules.cend();
    bool       did_change  = true;

    do {
      current    = 0;
      did_change = false;
      while (current != _word_graph.first_free_node() && !stopped()) {
        for (auto it = rules_begin; it < rules_end; it += 2) {
          word_graph_type::node_type u_end;
          word_type::const_iterator  rit;
          bool                       did_def = false;
          std::tie(u_end, rit) = word_graph::last_node_on_path_no_checks(
              _word_graph, current, it->cbegin(), it->cend());
          word_graph_type::node_type c, v_end;
          if (rit == it->cend()) {
            ++it;
            if (it->empty()) {
              did_def = false;
              c       = current;
              v_end   = c;
            } else {
              std::tie(did_def, c) = complete_path(
                  _word_graph, current, it->cbegin(), it->cend() - 1);
              v_end = _word_graph.target_no_checks(c, it->back());
            }
            if (v_end == UNDEFINED) {
              LIBSEMIGROUPS_ASSERT(!it->empty());
              did_def = true;
              def_edge(_word_graph, c, u_end, it->back(), presentation());
            } else if (u_end != v_end) {
              did_def = true;
              _word_graph.merge_nodes_no_checks(u_end, v_end);
              _word_graph.template process_coincidences<DoNotRegisterDefs>();
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
                std::tie(did_def, c) = complete_path(
                    _word_graph, current, it->cbegin(), it->cend() - 1);
                u_end = _word_graph.target_no_checks(c, it->back());
              }
              if (u_end == UNDEFINED) {
                LIBSEMIGROUPS_ASSERT(!it->empty());
                did_def = true;
                def_edge(_word_graph, c, v_end, it->back(), presentation());
              } else if (u_end != v_end) {
                did_def = true;
                _word_graph.merge_nodes_no_checks(u_end, v_end);
                _word_graph.template process_coincidences<DoNotRegisterDefs>();
              }
            } else {
              --it;
            }
          }
          did_change |= did_def;
        }
        report_status(start_time);
        current = _word_graph.next_active_node(current);
      }
    } while (did_change && !stopped());
    if (!stopped()) {
      _finished = true;
      standardize();
    }
  }

  namespace stephen {

    template <typename P>
    bool accepts(Stephen<P>& s, word_type const& w) {
      using word_graph::follow_path;
      using node_type = typename Stephen<P>::node_type;
      s.run();
      LIBSEMIGROUPS_ASSERT(s.accept_state() != UNDEFINED);
      return s.accept_state() == follow_path(s.word_graph(), node_type(0), w);
    }

    template <typename P>
    bool is_left_factor(Stephen<P>& s, word_type const& w) {
      using word_graph::last_node_on_path;
      using node_type = typename Stephen<P>::word_graph_type::node_type;
      s.run();
      return last_node_on_path(
                 s.word_graph(), node_type(0), w.cbegin(), w.cend())
                 .second
             == w.cend();
    }
  }  // namespace stephen
}  // namespace libsemigroups
