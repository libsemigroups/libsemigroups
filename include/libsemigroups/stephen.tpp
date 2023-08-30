//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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
  namespace v3 {

    template <typename ConstructFrom>
    Stephen<ConstructFrom>::Stephen()
        : _finished(false),
          _accept_state(UNDEFINED),
          _presentation(),
          _word(),
          _word_graph() {}

    template <typename ConstructFrom>
    template <typename P,
              typename>  // 2nd template param is to avoid
                         // overloading the copy/move constructors
    Stephen<ConstructFrom>::Stephen(P&& p) : Stephen() {
      static_assert(can_construct_from<P>());
      init_impl(std::forward<P>(p), std::is_lvalue_reference<P>());
    }

    template <typename ConstructFrom>
    template <typename P>
    Stephen<ConstructFrom>& Stephen<ConstructFrom>::init(P&& p) {
      static_assert(can_construct_from<P>());
      deref_if_necessary(p).validate();
      init_impl(std::forward<P>(p), std::is_lvalue_reference<P>());
      return *this;
    }

    template <typename ConstructFrom>
    template <typename P>
    void Stephen<ConstructFrom>::init_impl(P&& p, lvalue_tag) {
      if constexpr (IsPresentation<construct_from_type>) {
        init_impl(std::move(make<construct_from_type>(p)), non_lvalue_tag());
      } else {
        init_impl(std::move(construct_from_type(p)), non_lvalue_tag());
      }
    }

    template <typename ConstructFrom>
    void Stephen<ConstructFrom>::init_impl(ConstructFrom&& p, non_lvalue_tag) {
      if (deref_if_necessary(p).alphabet().empty()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the argument (Presentation) must not have 0 generators");
      }
      reset();

      _presentation = std::move(p);
      presentation::normalize_alphabet(deref_if_necessary(_presentation));

      _word_graph.init(presentation());
      _word.clear();
    }

    ////////////////////////////////////////////////////////////////////////
    // Public
    ////////////////////////////////////////////////////////////////////////

    template <typename ConstructFrom>
    Stephen<ConstructFrom>&
    Stephen<ConstructFrom>::set_word(word_type const& w) {
      presentation().validate_word(w.cbegin(), w.cend());
      reset();
      _word = w;
      return *this;
    }

    template <typename ConstructFrom>
    Stephen<ConstructFrom>& Stephen<ConstructFrom>::set_word(word_type&& w) {
      presentation().validate_word(w.cbegin(), w.cend());
      reset();
      _word = std::move(w);
      return *this;
    }

    template <typename ConstructFrom>
    typename Stephen<ConstructFrom>::node_type
    Stephen<ConstructFrom>::accept_state() {
      if (_accept_state == UNDEFINED) {
        using action_digraph_helper::last_node_on_path_nc;
        run();
        _accept_state
            = last_node_on_path_nc(_word_graph, 0, _word.cbegin(), _word.cend())
                  .first;
      }
      return _accept_state;
    }

    ////////////////////////////////////////////////////////////////////////
    // Private Member Functions
    ////////////////////////////////////////////////////////////////////////

    template <typename ConstructFrom>
    void Stephen<ConstructFrom>::def_edge(internal_digraph_type&   wg,
                                          node_type                from,
                                          node_type                to,
                                          label_type               letter,
                                          presentation_type const& p) const {
      if constexpr (!IsInversePresentation<presentation_type>) {
        wg.add_edge_nc(from, to, letter);
      } else {
        wg.add_edge_nc(from, to, letter);
        // convert l (which is an index)
        // -> actual letter
        // -> inverse of letter
        // -> index of inverse of letter
        auto ll             = p.index(p.inverse(p.letter(letter)));
        auto inverse_target = wg.neighbor(to, ll);
        if (inverse_target != UNDEFINED && inverse_target != from) {
          wg.coincide_nodes(from, inverse_target);
          wg.process_coincidences();
          return;
        }
        wg.add_edge_nc(to, from, ll);
      }
    }

    template <typename ConstructFrom>
    std::pair<bool, typename Stephen<ConstructFrom>::node_type>
    Stephen<ConstructFrom>::complete_path(
        internal_digraph_type&    wg,
        node_type                 c,
        word_type::const_iterator first,
        word_type::const_iterator last) noexcept {
      if (first == last) {
        return std::make_pair(false, c);
      }
      LIBSEMIGROUPS_ASSERT(first < last);
      word_type::const_iterator it;
      std::tie(c, it)
          = action_digraph_helper::last_node_on_path_nc(wg, c, first, last);
      bool result = false;
      for (; it < last; ++it) {
        node_type d = wg.unsafe_neighbor(c, *it);
        if (d == UNDEFINED) {
          d = wg.new_node();
          def_edge(wg, c, d, *it, presentation());
          result = true;
        }
        c = d;
      }
      return std::make_pair(result, c);
    }

    template <typename ConstructFrom>
    void Stephen<ConstructFrom>::report_status(
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

    template <typename ConstructFrom>
    void Stephen<ConstructFrom>::reset() noexcept {
      _finished     = false;
      _accept_state = UNDEFINED;
    }

    template <typename ConstructFrom>
    void Stephen<ConstructFrom>::standardize() {
      digraph_with_sources::standardize(_word_graph);
      _word_graph.shrink_to_fit(_word_graph.number_of_nodes_active());
    }

    template <typename ConstructFrom>
    void Stephen<ConstructFrom>::validate() const {
      if (presentation().alphabet().empty()) {
        LIBSEMIGROUPS_EXCEPTION("no presentation defined, use Stephen::init to "
                                "set the presentation");
      }
    }

    template <typename ConstructFrom>
    void Stephen<ConstructFrom>::run_impl() {
      auto start_time = std::chrono::high_resolution_clock::now();
      validate();  // throws if no presentation is defined
      _word_graph.init(deref_if_necessary(presentation()));
      complete_path(_word_graph, 0, _word.cbegin(), _word.cend());
      node_type& current = _word_graph.cursor();
      auto const rules_begin
          = deref_if_necessary(presentation()).rules.cbegin();
      auto const rules_end  = deref_if_necessary(presentation()).rules.cend();
      bool       did_change = true;

      do {
        current    = 0;
        did_change = false;
        while (current != _word_graph.first_free_node() && !stopped()) {
          for (auto it = rules_begin; it < rules_end; it += 2) {
            node_type                 u_end;
            word_type::const_iterator rit;
            bool                      did_def = false;
            std::tie(u_end, rit) = action_digraph_helper::last_node_on_path_nc(
                _word_graph, current, it->cbegin(), it->cend());
            node_type c, v_end;
            if (rit == it->cend()) {
              ++it;
              if (it->empty()) {
                did_def = false;
                c       = current;
                v_end   = c;
              } else {
                std::tie(did_def, c) = complete_path(
                    _word_graph, current, it->cbegin(), it->cend() - 1);
                v_end = _word_graph.unsafe_neighbor(c, it->back());
              }
              if (v_end == UNDEFINED) {
                LIBSEMIGROUPS_ASSERT(!it->empty());
                did_def = true;
                def_edge(_word_graph, c, u_end, it->back(), presentation());
              } else if (u_end != v_end) {
                did_def = true;
                _word_graph.coincide_nodes(u_end, v_end);
                _word_graph.process_coincidences();
              }
              --it;
            } else {
              ++it;
              std::tie(v_end, rit)
                  = action_digraph_helper::last_node_on_path_nc(
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
                  u_end = _word_graph.unsafe_neighbor(c, it->back());
                }
                if (u_end == UNDEFINED) {
                  LIBSEMIGROUPS_ASSERT(!it->empty());
                  did_def = true;
                  def_edge(_word_graph, c, v_end, it->back(), presentation());
                } else if (u_end != v_end) {
                  did_def = true;
                  _word_graph.coincide_nodes(u_end, v_end);
                  _word_graph.process_coincidences();
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

      template <typename ConstructFrom>
      bool accepts(Stephen<ConstructFrom>& s, word_type const& w) {
        using action_digraph_helper::follow_path;
        s.run();
        LIBSEMIGROUPS_ASSERT(s.accept_state() != UNDEFINED);
        return s.accept_state() == follow_path(s.word_graph(), 0, w);
      }

      template <typename ConstructFrom>
      bool is_left_factor(Stephen<ConstructFrom>& s, word_type const& w) {
        using action_digraph_helper::last_node_on_path;
        s.run();
        return last_node_on_path(s.word_graph(), 0, w.cbegin(), w.cend()).second
               == w.cend();
      }
    }  // namespace stephen
  }    // namespace v3
}  // namespace libsemigroups
