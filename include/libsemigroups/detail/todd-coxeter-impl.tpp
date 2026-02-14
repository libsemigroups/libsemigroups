//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

// This file contains out-of-line ToddCoxeterImpl mem fn templates.

namespace libsemigroups {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl::Settings
    ////////////////////////////////////////////////////////////////////////

    struct ToddCoxeterImpl::Settings {
      size_t                    def_max;
      options::def_policy       def_policy;
      size_t                    hlt_defs;
      size_t                    f_defs;
      options::lookahead_extent lookahead_extent;
      float                     lookahead_growth_factor;
      size_t                    lookahead_growth_threshold;
      size_t                    lookahead_min;
      size_t                    lookahead_next;
      std::chrono::nanoseconds  lookahead_stop_early_interval;
      float                     lookahead_stop_early_ratio;
      options::lookahead_style  lookahead_style;
      size_t                    lookbehind_threshold;
      size_t                    lower_bound;
      bool                      save;
      options::strategy         strategy;
      bool                      use_relations_in_extra;

      Settings()
          : def_max(),
            def_policy(),
            hlt_defs(),
            f_defs(),
            lookahead_extent(),
            lookahead_growth_factor(),
            lookahead_growth_threshold(),
            lookahead_min(),
            lookahead_next(),
            lookahead_stop_early_interval(),
            lookahead_stop_early_ratio(),
            lookahead_style(),
            lower_bound(),
            save(),
            strategy(),
            use_relations_in_extra() {
        init();
      }

      Settings(Settings const&)            = default;
      Settings(Settings&&)                 = default;
      Settings& operator=(Settings const&) = default;
      Settings& operator=(Settings&&)      = default;

      Settings& init() {
        def_max                    = 2'000;
        def_policy                 = options::def_policy::no_stack_if_no_space;
        hlt_defs                   = 200'000;
        f_defs                     = 100'000;
        lookahead_extent           = options::lookahead_extent::partial;
        lookahead_growth_factor    = 2.0;
        lookahead_growth_threshold = 4;
        lower_bound                = UNDEFINED;
        lookahead_min              = 10'000;
        lookahead_next             = 5'000'000;
        lookahead_stop_early_interval = std::chrono::seconds(1);
        lookahead_stop_early_ratio    = 0.01;
        lookahead_style               = options::lookahead_style::hlt;
        lookbehind_threshold          = 32'768;

        save                   = false;
        strategy               = options::strategy::hlt;
        use_relations_in_extra = false;
        return *this;
      }
    };  // class ToddCoxeterImpl::Settings

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl::SettingsGuard
    ////////////////////////////////////////////////////////////////////////

    class ToddCoxeterImpl::SettingsGuard {
      ToddCoxeterImpl* _tc;

     public:
      explicit SettingsGuard(ToddCoxeterImpl* tc) : _tc(tc) {
        _tc->_settings_stack.push_back(std::make_unique<Settings>());
      }

      ~SettingsGuard() {
        _tc->_settings_stack.pop_back();
        LIBSEMIGROUPS_ASSERT(!_tc->_settings_stack.empty());
      }
    };  // class ToddCoxeterImpl::SettingsGuard

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl::Graph
    ////////////////////////////////////////////////////////////////////////

    template <typename Functor>
    void ToddCoxeterImpl::Graph::process_coincidences(Functor&& func) {
      // FelschGraph_::process_coincidences returns true if any changes are
      // made to the graph.
      if (FelschGraph_::process_coincidences(std::forward<Functor>(func))) {
        _forest_valid          = false;
        _standardization_order = Order::none;
      }
    }
    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl
    ////////////////////////////////////////////////////////////////////////

    template <typename Node>
    ToddCoxeterImpl& ToddCoxeterImpl::init(congruence_kind        knd,
                                           WordGraph<Node> const& wg) {
      // FIXME check that wg is valid, which means what exactly?

      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
      detail::CongruenceCommon::init(knd);
      _finished  = false;
      _never_run = true;
      reset_settings_stack();
      _ticker_running = false;

      // TODO(1) if &wg == &_word_graph, what then? Should be handled in
      // Graph::operator=
      _word_graph = wg;
      _word_graph.presentation().alphabet(wg.out_degree());
      copy_settings_into_graph();
      // FIXME(1) setting the setting in the next line, and adding a Felsch
      // runner to the word graph version of Congruence leads to an incorrect
      // answer for the extreme test in congruence def_max(POSITIVE_INFINITY);
      report_prefix("ToddCoxeter");
      return *this;
    }

    template <typename Node>
    ToddCoxeterImpl& ToddCoxeterImpl::init(congruence_kind                knd,
                                           Presentation<word_type> const& p,
                                           WordGraph<Node> const&         wg) {
      // FIXME check that wg is valid, which means what exactly?
      p.throw_if_bad_alphabet_or_rules();
      presentation::throw_if_not_normalized(p);

      detail::CongruenceCommon::init(knd);
      _finished  = false;
      _never_run = true;
      reset_settings_stack();
      _ticker_running = false;

      // FIXME(1) this doesn't seem to reset _word_graph
      // properly, in particular, the node managed part isn't reset.
      _word_graph.init(p, wg);  // this does not throw when p is invalid
      copy_settings_into_graph();
      report_prefix("ToddCoxeter");
      return *this;
    }

    template <typename Iterator1, typename Iterator2>
    ToddCoxeterImpl::index_type
    ToddCoxeterImpl::current_index_of_no_checks(Iterator1 first,
                                                Iterator2 last) const {
      node_type c = current_word_graph().initial_node();

      c = v4::word_graph::follow_path_no_checks(
          current_word_graph(), c, first, last);
      // c is in the range 1, ..., number_of_cosets_active() because 0
      // represents the identity coset, and does not correspond to an element,
      // unless internal_presentation().contains_empty_word()
      size_t const offset
          = (internal_presentation().contains_empty_word() ? 0 : 1);
      return (c == UNDEFINED ? UNDEFINED : static_cast<index_type>(c - offset));
    }

    template <typename Iterator1, typename Iterator2>
    ToddCoxeterImpl::index_type
    ToddCoxeterImpl::index_of_no_checks(Iterator1 first, Iterator2 last) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      if (!current_word_graph().is_standardized()) {
        _word_graph.standardize(Order::shortlex);
      }
      return current_index_of_no_checks(first, last);
    }

    template <typename OutputIterator>
    OutputIterator
    ToddCoxeterImpl::current_word_of_no_checks(OutputIterator d_first,
                                               index_type     i) const {
      LIBSEMIGROUPS_ASSERT(i != UNDEFINED);
      if (!current_word_graph().is_standardized()) {
        // We must standardize here o/w there's no bijection between the numbers
        // 0, ..., n - 1 on to the nodes of the word graph.
        // Or worse, there's no guarantee that _tree is populated or is a
        // spanning tree of the current word graph
        // TODO(1) bit fishy here too
        const_cast<Graph&>(_word_graph).standardize(Order::shortlex);
      }

      size_t const offset
          = (internal_presentation().contains_empty_word() ? 0 : 1);

      return current_spanning_tree().path_from_root_no_checks(d_first,
                                                              i + offset);
    }

    template <typename OutputIterator>
    OutputIterator ToddCoxeterImpl::current_word_of(OutputIterator d_first,
                                                    index_type     i) const {
      size_t const offset
          = (internal_presentation().contains_empty_word() ? 0 : 1);

      if (i >= current_word_graph().number_of_nodes_active() - offset) {
        // We must standardize before doing this so that the index even makes
        // sense.
        LIBSEMIGROUPS_EXCEPTION("invalid class index, expected a value in "
                                "the range [0, {}), found {}",
                                current_word_graph().number_of_nodes_active()
                                    - offset,
                                i);
      }
      if (!current_word_graph().is_standardized()) {
        // We must standardize here o/w there's no bijection between the numbers
        // 0, ..., n - 1 on to the nodes of the word graph.
        // Or worse, there's no guarantee that _tree is populated or is a
        // spanning tree of the current word graph
        // TODO(1) bit fishy here too
        const_cast<Graph&>(_word_graph).standardize(Order::shortlex);
      }
      return current_word_of_no_checks(d_first, i);
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril ToddCoxeterImpl::currently_contains_no_checks(Iterator1 first1,
                                                       Iterator2 last1,
                                                       Iterator3 first2,
                                                       Iterator4 last2) const {
      // TODO(1) remove when is_free is implemented
      if (internal_presentation().rules.empty()
          && internal_generating_pairs().empty()
          && current_word_graph().number_of_nodes_active() == 1) {
        return std::equal(first1, last1, first2, last2) ? tril::TRUE
                                                        : tril::FALSE;
      }
      auto index1 = current_index_of_no_checks(first1, last1);
      auto index2 = current_index_of_no_checks(first2, last2);

      if (finished()) {
        return index1 == index2 ? tril::TRUE : tril::FALSE;
      }

      if (index1 != index2 || index1 == UNDEFINED) {
        word_type word1, word2;
        reduce_no_run_no_checks(std::back_inserter(word1), first1, last1);
        reduce_no_run_no_checks(std::back_inserter(word2), first2, last2);
        if (std::equal(
                word1.cbegin(), word1.cend(), word2.cbegin(), word2.cend())) {
          return tril::TRUE;
        }
        return tril::unknown;
      } else {
        return tril::TRUE;
      }
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool ToddCoxeterImpl::contains_no_checks(Iterator1 first1,
                                             Iterator2 last1,
                                             Iterator3 first2,
                                             Iterator4 last2) {
      return detail::CongruenceCommon::contains_no_checks<ToddCoxeterImpl>(
          first1, last1, first2, last2);
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool ToddCoxeterImpl::contains(Iterator1 first1,
                                   Iterator2 last1,
                                   Iterator3 first2,
                                   Iterator4 last2) {
      // TODO(1) should be
      // return detail::CongruenceCommon::contains<ToddCoxeterImpl>(
      //     first1, last1, first2, last2);
      // when is_free is implemented. Not currently so that the analogue of
      // is_free is called.
      internal_presentation().throw_if_letter_not_in_alphabet(first1, last1);
      internal_presentation().throw_if_letter_not_in_alphabet(first2, last2);
      return contains_no_checks(first1, last1, first2, last2);
    }

    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator
    ToddCoxeterImpl::reduce_no_run_no_checks(OutputIterator d_first,
                                             InputIterator1 first,
                                             InputIterator2 last) const {
      if (finished()
          || (kind() == congruence_kind::onesided
              && !internal_generating_pairs().empty())) {
        if (!current_word_graph().is_standardized()) {
          // TODO(1) this is a bit fishy
          const_cast<Graph&>(_word_graph).standardize(Order::shortlex);
        }
        index_type pos = current_index_of_no_checks(first, last);
        if (pos == UNDEFINED) {
          return std::copy(first, last, d_first);
        }
        return current_word_of_no_checks(d_first, pos);
      }
      node_type const s = current_word_graph().initial_node();
      Forest const&   f = current_spanning_tree();

      word_type u(first, last);
      auto      v_begin = u.begin();

      while (v_begin != u.end()) {
        auto [t, old_end] = v4::word_graph::last_node_on_path_no_checks(
            current_word_graph(), s, v_begin, u.end());

        if (!std::equal(std::reverse_iterator(old_end),
                        std::reverse_iterator(v_begin),
                        f.cbegin_path_to_root_no_checks(t),
                        f.cend_path_to_root_no_checks(t))) {
          LIBSEMIGROUPS_ASSERT(
              forest::depth_no_checks(f, t)
              <= static_cast<size_t>(std::distance(v_begin, old_end)));
          auto new_end = f.path_from_root_no_checks(v_begin, t);
          LIBSEMIGROUPS_ASSERT(new_end <= old_end);
          u.erase(new_end, old_end);
          v_begin = u.begin();
        } else {
          v_begin++;
        }
      }
      return std::copy(u.begin(), u.end(), d_first);
    }

    ////////////////////////////////////////////////////////////////////////
    // Lookbehind
    ////////////////////////////////////////////////////////////////////////

    template <typename Func>
    ToddCoxeterImpl&
    ToddCoxeterImpl::perform_lookbehind_no_checks(Func&& collapser) {
      SettingsGuard sg(this);
      strategy(options::strategy::lookbehind);
      _lookbehind_collapser = std::forward<Func>(collapser);
      run();
      return *this;
    }

    template <typename Func>
    ToddCoxeterImpl& ToddCoxeterImpl::perform_lookbehind_for_no_checks(
        std::chrono::nanoseconds t,
        Func&&                   collapser) {
      SettingsGuard sg(this);
      strategy(options::strategy::lookbehind);
      _lookbehind_collapser = std::forward<Func>(collapser);
      run_for(t);
      return *this;
    }

    template <typename Func>
    ToddCoxeterImpl& ToddCoxeterImpl::perform_lookbehind_until_no_checks(
        std::function<bool()>&& pred,
        Func&&                  collapser) {
      SettingsGuard sg(this);
      strategy(options::strategy::lookbehind);
      _lookbehind_collapser = std::forward<Func>(collapser);
      run_until(std::move(pred));
      return *this;
    }

  }  // namespace detail
}  // namespace libsemigroups
