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
      BaseGraph::init(NodeManager<node_type>::node_capacity(),
                      p.alphabet().size());
      return *this;
    }

    StephenGraph& init(Presentation<word_type>&& p) {
      NodeManager<node_type>::clear();
      BaseGraph::init(NodeManager<node_type>::node_capacity(),
                      p.alphabet().size());
      return *this;
    }

    void set_target_no_checks(presentation_type const& p,
                              node_type                from,
                              label_type               letter,
                              node_type                to) {
      WordGraphWithSources::set_target_no_checks(from, letter, to);
      if constexpr (IsInversePresentation<presentation_type>) {
        // convert l (which is an index)
        // -> actual letter
        // -> inverse of letter
        // -> index of inverse of letter
        auto ll             = p.index(p.inverse(p.letter(letter)));
        auto inverse_target = target_no_checks(to, ll);
        if (inverse_target != UNDEFINED && inverse_target != from) {
          merge_nodes_no_checks(from, inverse_target);
          process_coincidences<DoNotRegisterDefs>();
          return;
        }
        WordGraphWithSources::set_target_no_checks(to, ll, from);
      }
    }

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
          StephenGraph::set_target_no_checks(p, c, *it, d);
          result = true;
        }
        c = d;
      }
      return std::make_pair(result, c);
    }

    void disjoint_union_inplace(StephenGraph const& that) {
      // TODO throw exception if that.labels() != this->labels()
      // TODO the following requires that this and that are standardized
      // and that this and that are run to the end
      size_t const N = number_of_nodes_active();
      // TODO the following 2 lines are a bit awkward
      BaseGraph::add_nodes(that.number_of_nodes());
      NodeManager<node_type>::add_active_nodes(that.number_of_nodes());

      for (auto n : that.nodes()) {
        for (auto a : that.labels()) {
          BaseGraph::set_target_no_checks(
              n + N, a, that.target_no_checks(n, a) + N);
        }
      }
    }
  };

  template <typename P>
  Stephen<P>::Stephen()
      : _accept_state(UNDEFINED),
        _something_changed(true),
        _finished(false),
        _presentation(),
        _word(),
        _word_graph() {}

  template <typename P>
  Stephen<P>& Stephen<P>::init() {
    _accept_state      = UNDEFINED;
    _something_changed = true;
    _finished          = false;
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
    deref_if_necessary(p).validate();
    throw_if_presentation_empty(deref_if_necessary(p));
    _presentation = std::move(p);
    something_changed();
    _word.clear();
    return *this;
  }

  template <typename P>
  Stephen<P>& Stephen<P>::init(P const& p) {
    deref_if_necessary(p).validate();
    throw_if_presentation_empty(deref_if_necessary(p));
    _presentation = p;
    something_changed();
    _word.clear();
    return *this;
  }

  template <typename P>
  void Stephen<P>::something_changed() noexcept {
    _something_changed = true;
    _finished          = false;
    _accept_state      = UNDEFINED;
  }

  ////////////////////////////////////////////////////////////////////////
  // Public
  ////////////////////////////////////////////////////////////////////////

  template <typename P>
  Stephen<P>& Stephen<P>::set_word(word_type const& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    something_changed();
    _word = w;
    return *this;
  }

  template <typename P>
  Stephen<P>& Stephen<P>::set_word(word_type&& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    something_changed();
    _word = std::move(w);
    return *this;
  }

  template <typename P>
  typename Stephen<P>::node_type Stephen<P>::accept_state() {
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
  void Stephen<P>::standardize() {
    word_graph::standardize(_word_graph);
    _word_graph.induced_subgraph_no_checks(
        0, _word_graph.number_of_nodes_active());
  }

  template <typename P>
  void
  Stephen<P>::throw_if_presentation_empty(presentation_type const& p) const {
    if (p.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("the presentation must not have 0 generators");
    }
  }

  template <typename P>
  void Stephen<P>::run_impl() {
    auto start_time = std::chrono::high_resolution_clock::now();

    if (_something_changed) {
      throw_if_presentation_empty(presentation());
      _something_changed = false;
      _word_graph.init(presentation());
      _word_graph.complete_path(
          presentation(), 0, _word.cbegin(), _word.cend());
    }
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
              _word_graph.set_target_no_checks(
                  presentation(), c, it->back(), u_end);
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
                std::tie(did_def, c) = _word_graph.complete_path(
                    presentation(), current, it->cbegin(), it->cend() - 1);
                u_end = _word_graph.target_no_checks(c, it->back());
              }
              if (u_end == UNDEFINED) {
                LIBSEMIGROUPS_ASSERT(!it->empty());
                did_def = true;
                _word_graph.set_target_no_checks(
                    presentation(), c, it->back(), v_end);
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
      using node_type = typename Stephen<P>::node_type;
      s.run();
      return last_node_on_path(
                 s.word_graph(), node_type(0), w.cbegin(), w.cend())
                 .second
             == w.cend();
    }
  }  // namespace stephen
}  // namespace libsemigroups
