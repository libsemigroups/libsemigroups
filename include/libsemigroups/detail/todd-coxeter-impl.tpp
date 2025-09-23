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

    template <typename Node>
    ToddCoxeterImpl& ToddCoxeterImpl::init(congruence_kind        knd,
                                           WordGraph<Node> const& wg) {
      // FIXME check that wg is valid, which means what exactly?

      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
      detail::CongruenceCommon::init(knd);
      _finished = false;
      reset_settings_stack();
      _standardized   = Order::none;
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
      _finished = false;
      reset_settings_stack();
      _standardized   = Order::none;
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

      c = word_graph::follow_path_no_checks(
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
      if (!is_standardized()) {
        standardize(Order::shortlex);
      }
      return current_index_of_no_checks(first, last);
    }

    template <typename OutputIterator>
    OutputIterator
    ToddCoxeterImpl::current_word_of_no_checks(OutputIterator d_first,
                                               index_type     i) const {
      LIBSEMIGROUPS_ASSERT(i != UNDEFINED);
      if (!is_standardized()) {
        // We must standardize here o/w there's no bijection between the numbers
        // 0, ..., n - 1 on to the nodes of the word graph.
        // Or worse, there's no guarantee that _forest is populated or is a
        // spanning tree of the current word graph
        // TODO(1) bit fishy here too
        const_cast<ToddCoxeterImpl*>(this)->standardize(Order::shortlex);
      }

      size_t const offset
          = (internal_presentation().contains_empty_word() ? 0 : 1);

      return _forest.path_from_root_no_checks(d_first, i + offset);
    }

    template <typename OutputIterator>
    OutputIterator ToddCoxeterImpl::current_word_of(OutputIterator d_first,
                                                    index_type     i) const {
      size_t const offset
          = (internal_presentation().contains_empty_word() ? 0 : 1);

      if (!is_standardized()) {
        // We must standardize here o/w there's no bijection between the numbers
        // 0, ..., n - 1 on to the nodes of the word graph.
        // Or worse, there's no guarantee that _forest is populated or is a
        // spanning tree of the current word graph
        // TODO(1) bit fishy here too
        const_cast<ToddCoxeterImpl*>(this)->standardize(Order::shortlex);
      }
      if (i >= current_word_graph().number_of_nodes_active() - offset) {
        // We must standardize before doing this so that the index even makes
        // sense.
        LIBSEMIGROUPS_EXCEPTION("invalid class index, expected a value in "
                                "the range [0, {}), found {}",
                                current_word_graph().number_of_nodes_active()
                                    - offset,
                                i);
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
      // TODO(1) remove when is_free is implemented
      if (internal_presentation().rules.empty()
          && internal_generating_pairs().empty()
          && current_word_graph().number_of_nodes_active() == 1) {
        return std::equal(first1, last1, first2, last2);
      }
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
      // TODO(1) remove when is_free is implemented
      if (internal_presentation().rules.empty()
          && internal_generating_pairs().empty()
          && current_word_graph().number_of_nodes_active() == 1) {
        return std::equal(first1, last1, first2, last2);
      }
      return detail::CongruenceCommon::contains<ToddCoxeterImpl>(
          first1, last1, first2, last2);
    }

    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator
    ToddCoxeterImpl::reduce_no_run_no_checks(OutputIterator d_first,
                                             InputIterator1 first,
                                             InputIterator2 last) const {
      if (!is_standardized()) {
        // TODO(1) this is a bit fishy
        const_cast<ToddCoxeterImpl*>(this)->standardize(Order::shortlex);
      }
      if (finished()
          || (kind() == congruence_kind::onesided
              && !internal_generating_pairs().empty())) {
        index_type pos = current_index_of_no_checks(first, last);
        if (pos == UNDEFINED) {
          return std::copy(first, last, d_first);
        }
        return current_word_of_no_checks(d_first, pos);
      }

      node_type const s = current_word_graph().initial_node();
      word_type       u(first, last);
      auto            v_begin = u.begin();

      while (v_begin != u.end()) {
        auto [t, old_end] = word_graph::last_node_on_path_no_checks(
            current_word_graph(), s, v_begin, u.end());

        if (!std::equal(std::reverse_iterator(old_end),
                        std::reverse_iterator(v_begin),
                        _forest.cbegin_path_to_root_no_checks(t),
                        _forest.cend_path_to_root_no_checks(t))) {
          auto new_end = _forest.path_from_root_no_checks(v_begin, t);
          u.erase(new_end, old_end);
          v_begin = u.begin();
        } else {
          v_begin++;
        }
      }
      return std::copy(u.begin(), u.end(), d_first);
    }
  }  // namespace detail
}  // namespace libsemigroups
