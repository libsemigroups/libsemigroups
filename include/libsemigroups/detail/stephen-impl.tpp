//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2026 James D. Mitchell
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

// This file contains the implementation of the StephenImpl class template.

namespace libsemigroups {
  namespace detail {

    template <typename PresentationType>
    class StephenImpl<PresentationType>::StephenGraph
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
      // encompassing StephenImpl object or the presentation
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
      // encompassing StephenImpl object or the presentation
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
        std::tie(c, it) = v4::word_graph::last_node_on_path_no_checks(
            *this, c, first, last);
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
        v4::word_graph::standardize(that);
        LIBSEMIGROUPS_ASSERT(v4::word_graph::is_standardized(that));
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
    StephenImpl<PresentationType>::StephenImpl()
        : _accept_state(UNDEFINED),
          _finished(false),
          _is_word_set(false),
          _internal_presentation(std::make_shared<PresentationType>()),
          _internal_word(),
          _ticker_running(false),
          _word_graph() {
      report_prefix("Stephen");
      _word_graph.report_prefix("Stephen");
      report_divider(fmt::format("{:+<32}\n", ""));
    }

    template <typename PresentationType>
    StephenImpl<PresentationType>& StephenImpl<PresentationType>::init() {
      _accept_state = UNDEFINED;
      _finished     = false;
      _is_word_set  = false;
      _internal_presentation->init();
      _internal_word.clear();
      _ticker_running = false;
      _word_graph.init();
      report_prefix("Stephen");
      _word_graph.report_prefix("Stephen");
      return *this;
    }

    template <typename PresentationType>
    StephenImpl<PresentationType>& StephenImpl<PresentationType>::init(
        std::shared_ptr<PresentationType> const& ptr) {
      throw_if_presentation_empty(*ptr);
      ptr->throw_if_bad_alphabet_or_rules();
      presentation::throw_if_not_normalized(*ptr);
      init();
      _internal_presentation = ptr;
      return *this;
    }

    template <typename PresentationType>
    StephenImpl<PresentationType>::~StephenImpl() = default;

    ////////////////////////////////////////////////////////////////////////
    // Public
    ////////////////////////////////////////////////////////////////////////

    template <typename PresentationType>
    template <typename Iterator1, typename Iterator2>
    StephenImpl<PresentationType>&
    StephenImpl<PresentationType>::set_internal_word_no_checks(Iterator1 first,
                                                               Iterator2 last) {
      _accept_state = UNDEFINED;
      _finished     = false;
      _internal_word.assign(first, last);
      _word_graph.init(internal_presentation());
      _word_graph.report_prefix("Stephen");
      _word_graph.complete_path(internal_presentation(),
                                0,
                                _internal_word.cbegin(),
                                _internal_word.cend());
      // Here so we have accurate data when using to_human_readable_repr
      _word_graph.number_of_active_nodes(_word_graph.number_of_nodes_active());
      _is_word_set = true;
      return *this;
    }

    template <typename PresentationType>
    typename StephenImpl<PresentationType>::node_type
    StephenImpl<PresentationType>::accept_state() {
      throw_if_not_ready();
      if (_accept_state == UNDEFINED) {
        using v4::word_graph::last_node_on_path_no_checks;
        run();
        _accept_state = last_node_on_path_no_checks(_word_graph,
                                                    0,
                                                    _internal_word.cbegin(),
                                                    _internal_word.cend())
                            .first;
      }
      return _accept_state;
    }

    template <typename PresentationType>
    void StephenImpl<PresentationType>::operator*=(
        StephenImpl<PresentationType>& that) {
      throw_if_not_ready();
      that.throw_if_not_ready();
      if (internal_presentation() != that.internal_presentation()) {
        LIBSEMIGROUPS_EXCEPTION("this->internal_presentation() must equal "
                                "that.internal_presentation() when appending "
                                "StephenImpl instances")
      }

      append_no_checks(that);
    }

    template <typename PresentationType>
    void StephenImpl<PresentationType>::append_no_checks(
        StephenImpl<PresentationType>& that) {
      // NOTE: Appending seems to work fine without running both this and that.
      // If for some reason this is no longer true later on, uncomment the next
      // two lines and update docs to indicate algorithm is run when calling
      // append.

      // this->run();
      // that.run();

      // TODO(2) _word_graph has two mem fns number_nodes_active (in
      // NodeManager) and number_active_nodes (in WordGraph), this is super
      // confusing!
      size_t const N = _word_graph.number_of_nodes_active();
      _word_graph.disjoint_union_inplace_no_checks(that._word_graph);
      _word_graph.merge_nodes_no_checks(accept_state(),
                                        that.initial_state() + N);
      _word_graph.process_coincidences();
      _accept_state = UNDEFINED;
      _finished     = false;
      _internal_word.insert(_internal_word.end(),
                            that._internal_word.cbegin(),
                            that._internal_word.cend());
      _word_graph.cursor() = initial_state();
    }

    ////////////////////////////////////////////////////////////////////////
    // Private Member Functions
    ////////////////////////////////////////////////////////////////////////

    template <typename PresentationType>
    void StephenImpl<PresentationType>::throw_if_not_ready() const {
      if (internal_presentation().alphabet().empty()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the presentation must be defined using init() before "
            "calling this function");
      } else if (!_is_word_set) {
        LIBSEMIGROUPS_EXCEPTION("the word must be set using set_word() before "
                                "calling this function");
      }
    }

    template <typename PresentationType>
    void StephenImpl<PresentationType>::throw_if_presentation_empty(
        presentation_type const& p) const {
      if (p.alphabet().empty()) {
        LIBSEMIGROUPS_EXCEPTION("the presentation must not have 0 generators");
      }
    }

    template <typename PresentationType>
    void StephenImpl<PresentationType>::report_before_run() const {
      if (reporting_enabled()) {
        report_no_prefix(report_divider());
        report_default("Stephen: STARTING . . .\n");
        report_default("Stephen: |w| = {}, {}",
                       internal_word().size(),
                       presentation::to_report_string(internal_presentation()));
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Reporting
    ////////////////////////////////////////////////////////////////////////

    template <typename PresentationType>
    void StephenImpl<PresentationType>::report_after_run() const {
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
    void StephenImpl<PresentationType>::report_progress_from_thread() const {
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
    void StephenImpl<PresentationType>::run_impl() {
      using std::chrono::duration_cast;
      using std::chrono::seconds;

      throw_if_not_ready();
      reset_start_time();

      Ticker     ticker;
      ValueGuard tg(_ticker_running);
      if (!_ticker_running && reporting_enabled()
          && (!running_for()
              || duration_cast<seconds>(running_for_how_long())
                     >= seconds(1))) {
        _ticker_running = true;
        ticker([this]() { report_progress_from_thread(); });
      }
      report_before_run();
      node_type& current     = _word_graph.cursor();
      auto const rules_begin = internal_presentation().rules.cbegin();
      auto const rules_end   = internal_presentation().rules.cend();
      bool       did_change  = true;

      do {
        current    = 0;
        did_change = false;
        while (current != _word_graph.first_free_node() && !stopped()) {
          for (auto it = rules_begin; it < rules_end; it += 2) {
            node_type                 u_end;
            word_type::const_iterator rit;
            bool                      did_def = false;
            std::tie(u_end, rit) = v4::word_graph::last_node_on_path_no_checks(
                _word_graph, current, it->cbegin(), it->cend());
            node_type c, v_end;
            if (rit == it->cend()) {
              ++it;
              if (it->empty()) {
                did_def = false;
                c       = current;
                v_end   = c;
              } else {
                std::tie(did_def, c)
                    = _word_graph.complete_path(internal_presentation(),
                                                current,
                                                it->cbegin(),
                                                it->cend() - 1);
                v_end = _word_graph.target_no_checks(c, it->back());
              }
              if (v_end == UNDEFINED) {
                LIBSEMIGROUPS_ASSERT(!it->empty());
                did_def = true;
                _word_graph.target_no_checks(
                    internal_presentation(), c, it->back(), u_end);
              } else if (u_end != v_end) {
                did_def = true;
                _word_graph.merge_nodes_no_checks(u_end, v_end);
                _word_graph.process_coincidences();
              }
              --it;
            } else {
              ++it;
              std::tie(v_end, rit)
                  = v4::word_graph::last_node_on_path_no_checks(
                      _word_graph, current, it->cbegin(), it->cend());
              if (rit == it->cend()) {
                --it;
                if (it->empty()) {
                  did_def = false;
                  c       = current;
                  u_end   = c;
                } else {
                  std::tie(did_def, c)
                      = _word_graph.complete_path(internal_presentation(),
                                                  current,
                                                  it->cbegin(),
                                                  it->cend() - 1);
                  u_end = _word_graph.target_no_checks(c, it->back());
                }
                if (u_end == UNDEFINED) {
                  LIBSEMIGROUPS_ASSERT(!it->empty());
                  did_def = true;
                  _word_graph.target_no_checks(
                      internal_presentation(), c, it->back(), v_end);
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
      report_after_run();
    }

    namespace stephen {

      template <typename PresentationType, typename Iterator>
      bool accepts(StephenImpl<PresentationType>& s,
                   Iterator                       first,
                   Iterator                       last) {
        using v4::word_graph::follow_path;
        using node_type = typename StephenImpl<PresentationType>::node_type;
        s.run();
        LIBSEMIGROUPS_ASSERT(s.accept_state() != UNDEFINED);
        return s.accept_state()
               == follow_path(s.word_graph_no_run(), node_type(0), first, last);
      }

      template <typename PresentationType, typename Iterator>
      bool is_left_factor(StephenImpl<PresentationType>& s,
                          Iterator                       first,
                          Iterator                       last) {
        using v4::word_graph::last_node_on_path;
        using node_type = typename StephenImpl<PresentationType>::node_type;
        s.run();
        return last_node_on_path(
                   s.word_graph_no_run(), node_type(0), first, last)
                   .second
               == last;
      }
    }  // namespace stephen
  }    // namespace detail
}  // namespace libsemigroups
