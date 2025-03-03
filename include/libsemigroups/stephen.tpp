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

#include "libsemigroups/detail/report.hpp"
namespace libsemigroups {

  template <typename PresentationType>
  class Stephen<PresentationType>::StephenGraph
      : public detail::NodeManagedGraph<
            detail::WordGraphWithSources<uint32_t>> {
    using WordGraphWithSources_ = detail::WordGraphWithSources<uint32_t>;

   public:
    using node_type = typename WordGraphWithSources_::node_type;

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
      if constexpr (IsInversePresentation<presentation_type>) {
        // convert l (which is an index)
        // -> actual letter
        // -> inverse of letter
        // -> index of inverse of letter
        auto ll             = p.index(p.inverse(p.letter(letter)));
        auto inverse_target = target_no_checks(to, ll);
        if (inverse_target != UNDEFINED && inverse_target != from) {
          merge_nodes_no_checks(from, inverse_target);
          process_coincidences<detail::DoNotRegisterDefs>();
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

    void disjoint_union_inplace_no_checks(StephenGraph const& that) {
      // TODO(1): the following requires that this and that are standardized
      // and that this and that are run to the end
      // TODO(0): make it work with incomplete non-standardized word graphs.
      size_t const N = number_of_nodes_active();
      // TODO(1): the following 2 lines are a bit awkward
      WordGraphWithSources_::add_nodes(that.number_of_nodes());
      NodeManager<node_type>::add_active_nodes(that.number_of_nodes());

      for (auto n : that.nodes()) {
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
    _word_graph.report_prefix("Stephen");
  }

  template <typename PresentationType>
  Stephen<PresentationType>& Stephen<PresentationType>::init() {
    _accept_state = UNDEFINED;
    _finished     = false;
    _is_word_set  = false;
    _presentation->init();
    _word.clear();
    _word_graph.init();
    _word_graph.report_prefix("Stephen");
  }

  template <typename PresentationType>
  Stephen<PresentationType>& Stephen<PresentationType>::init(
      std::shared_ptr<PresentationType> const& ptr) {
    ptr->validate();
    throw_if_presentation_empty(*ptr);
    _presentation = ptr;
    _accept_state = UNDEFINED;
    _finished     = false;
    _is_word_set  = false;
    _word.clear();
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Public
  ////////////////////////////////////////////////////////////////////////

  template <typename PresentationType>
  Stephen<PresentationType>&
  Stephen<PresentationType>::set_word(word_type const& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    _accept_state = UNDEFINED;
    _finished     = false;
    _word         = w;
    init_word_graph_from_word_no_checks();
    _is_word_set = true;
    return *this;
  }

  template <typename PresentationType>
  Stephen<PresentationType>&
  Stephen<PresentationType>::set_word(word_type&& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    _accept_state = UNDEFINED;
    _finished     = false;
    _word         = std::move(w);
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
  void Stephen<PresentationType>::report_before_run() {
    if (reporting_enabled()) {
      report_no_prefix("{:+<95}\n", "");
      report_default("Stephen: STARTING . . .\n");
      report_no_prefix("{:+<95}\n", "");
      using detail::group_digits;
      auto shortest_short = presentation::shortest_rule_length(presentation());
      auto longest_short  = presentation::longest_rule_length(presentation());
      report_default("Stephen: |w| = {}, |A| = {}, |R| = {}, "
                     "|u| + |v| \u2208 [{}, {}], \u2211(|u| + |v|) = {}\n",
                     word().size(),
                     presentation().alphabet().size(),
                     group_digits(presentation().rules.size() / 2),
                     shortest_short,
                     longest_short,
                     group_digits(presentation::length(presentation())));
    }
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::report_after_run() {
    if (reporting_enabled()) {
      report_no_prefix("{:-<95}\n", "");
      using detail::group_digits;
      report_default(
          "Stephen: Computed word graph with |V| = {} and |E| = {}\n",
          group_digits(_word_graph.number_of_nodes_active()),
          group_digits(_word_graph.number_of_edges()));
      report_no_prefix("{:+<95}\n", "");

      report_default("Stephen: STOPPING -- ");

      if (finished()) {
        report_no_prefix("finished!\n");
      } else {
        report_why_we_stopped();
      }
      report_no_prefix("{:+<95}\n", "");
    }
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::run_impl() {
    throw_if_not_ready();
    reset_start_time();

    report_before_run();
    if (reporting_enabled()) {
      detail::Ticker t([this]() { _word_graph.report_progress_from_thread(); });
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
              _word_graph
                  .template process_coincidences<detail::DoNotRegisterDefs>();
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
                _word_graph
                    .template process_coincidences<detail::DoNotRegisterDefs>();
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
      if constexpr (IsInversePresentation<PresentationType>) {
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
  [[nodiscard]] std::string
  to_human_readable_repr(Stephen<PresentationType> const& x) {
    if (!x.is_word_set()) {
      return fmt::format("<Stephen object over {} with no word set>",
                         to_human_readable_repr(x.presentation()));
    }
    return fmt::format(
        "<Stephen object over {} for {} with {} "
        "nodes and {} edges>",
        to_human_readable_repr(x.presentation()),
        x.word().size() < 10 ? fmt::format("word {}", x.word())
                             : fmt::format("{} letter word", x.word().size()),
        // TODO(0): want number of nodes active here, but WordGraph does not
        // have this, so we get some weird node counts unless Stephen has
        // finished (when we remove the induced verts). I guess this really
        // isn't that big of an issue though, since we give no guarantees about
        // the unfinished word graph?
        // TODO(0): In word graph member function of Stephen, set number of
        // active nodes
        x.word_graph().number_of_nodes(),
        x.word_graph().number_of_edges());
  }

}  // namespace libsemigroups
