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

    // TODO(1) Add data to the StephenGraph which is a pointer to either the
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

    // TODO(1) Add data to the StephenGraph which is a pointer to either the
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
      // TODO(0): throw exception if that.labels() != this->labels()
      // TODO(1): the following requires that this and that are standardized
      // and that this and that are run to the end
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
        _something_changed(true),
        _finished(false),
        _presentation(std::make_shared<PresentationType>()),
        _word(),
        _word_graph() {
    _word_graph.report_prefix("Stephen");
  }

  template <typename PresentationType>
  Stephen<PresentationType>& Stephen<PresentationType>::init() {
    _accept_state      = UNDEFINED;
    _something_changed = true;
    _finished          = false;
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
    //  TODO(0): check ok
    _presentation = ptr;
    something_changed();
    _word.clear();
    return *this;
  }

  template <typename PresentationType>
  template <typename OtherPresentationType>
  Stephen<PresentationType>&
  Stephen<PresentationType>::init(OtherPresentationType const& q) {
    static_assert(
        ((IsInversePresentation<PresentationType>)
         == (IsInversePresentation<OtherPresentationType>) )
            && IsPresentation<PresentationType>
            && IsPresentation<OtherPresentationType>,
        "PresentationType and OtherPresentationType must both be presentation "
        "types and either both are inverse presentaton types or neither is");
    if constexpr (IsInversePresentation<PresentationType>
                  && IsInversePresentation<OtherPresentationType>) {
      return init(to_inverse_presentation<word_type>(q));
    } else {
      return init(to_presentation<word_type>(q));
    }
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::something_changed() noexcept {
    _something_changed = true;
    _finished          = false;
    _accept_state      = UNDEFINED;
  }

  ////////////////////////////////////////////////////////////////////////
  // Public
  ////////////////////////////////////////////////////////////////////////

  // TODO(0): implement init_graph_from_word
  template <typename PresentationType>
  Stephen<PresentationType>&
  Stephen<PresentationType>::set_word(word_type const& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    something_changed();
    _word = w;
    return *this;
  }

  template <typename PresentationType>
  Stephen<PresentationType>&
  Stephen<PresentationType>::set_word(word_type&& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    something_changed();
    _word = std::move(w);
    return *this;
  }

  template <typename PresentationType>
  typename Stephen<PresentationType>::node_type
  Stephen<PresentationType>::accept_state() {
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
  void Stephen<PresentationType>::standardize() {
    word_graph::standardize(_word_graph);
    _word_graph.induced_subgraph_no_checks(
        0, _word_graph.number_of_nodes_active());
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::throw_if_presentation_empty(
      presentation_type const& p) const {
    if (p.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("the presentation must not have 0 generators");
    }
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::throw_if_not_ready() const {
    if (presentation().rules.empty() && presentation().alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the presentation must be defined using init() before "
          "calling this function");
    } else if (word()) {
    }
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::run_impl() {
    reset_start_time();
    // TODO(0): report_after_run (including report_why_we_stopped) and
    // report_before_run
    if (reporting_enabled()) {
      detail::Ticker t([this]() { _word_graph.report_progress_from_thread(); });
      really_run_impl();
    } else {
      really_run_impl();
    }
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::really_run_impl() {
    if (_something_changed) {
      throw_if_presentation_empty(presentation());
      _something_changed = false;
      _word_graph.init(presentation());
      // TODO(0): do we need presentation here? Could we set the initial word
      // when we set_word?
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

  }  // namespace stephen

  template <typename PresentationType>
  [[nodiscard]] std::string
  to_human_readable_repr(Stephen<PresentationType> const& x) {
    std::string word;
    return fmt::format("<Stephen object over {} for {} with {} "
                       "nodes and {} edges>",
                       to_human_readable_repr(x.presentation()),
                       x.word().size() < 10
                           ? fmt::format("word {}", x.word())
                           : fmt::format("{} letter word", x.word().size()),
                       x.word_graph().number_of_nodes(),
                       x.word_graph().number_of_edges());
  }

}  // namespace libsemigroups
