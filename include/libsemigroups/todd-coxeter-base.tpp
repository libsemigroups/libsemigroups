
//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 James D. Mitchell
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

// This file contains out-of-line ToddCoxeterBase mem fn templates

namespace libsemigroups {

  template <typename Node>
  ToddCoxeterBase& ToddCoxeterBase::init(congruence_kind        knd,
                                         WordGraph<Node> const& wg) {
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
    CongruenceInterface::init(knd);
    init();
    // FIXME setting the setting in the next line, and adding a Felsch runner to
    // the word graph version of Congruence leads to an incorrect answer for the
    // extreme test in congruence def_max(POSITIVE_INFINITY);
    _word_graph = wg;
    _word_graph.presentation().alphabet(wg.out_degree());
    copy_settings_into_graph();
    return *this;
  }

  template <typename Node>
  ToddCoxeterBase& ToddCoxeterBase::init(congruence_kind                knd,
                                         Presentation<word_type> const& p,
                                         WordGraph<Node> const&         wg) {
    init(knd, p);
    _word_graph = wg;
    _word_graph.presentation(p);  // TODO(0) does this throw if p is invalid?
    copy_settings_into_graph();
    return *this;
  }

  template <typename Iterator1, typename Iterator2>
  ToddCoxeterBase::index_type
  ToddCoxeterBase::current_index_of_no_checks(Iterator1 first,
                                              Iterator2 last) const {
    node_type c = current_word_graph().initial_node();

    c = word_graph::follow_path_no_checks(current_word_graph(), c, first, last);
    // c is in the range 1, ..., number_of_cosets_active() because 0
    // represents the identity coset, and does not correspond to an element,
    // unless internal_presentation().contains_empty_word()
    size_t const offset
        = (internal_presentation().contains_empty_word() ? 0 : 1);
    return (c == UNDEFINED ? UNDEFINED : static_cast<index_type>(c - offset));
  }

  template <typename Iterator1, typename Iterator2>
  ToddCoxeterBase::index_type
  ToddCoxeterBase::index_of_no_checks(Iterator1 first, Iterator2 last) {
    run();
    LIBSEMIGROUPS_ASSERT(finished());
    if (!is_standardized()) {
      standardize(Order::shortlex);
    }
    return current_index_of_no_checks(first, last);
  }

  template <typename OutputIterator>
  OutputIterator
  ToddCoxeterBase::current_word_of_no_checks(OutputIterator d_first,
                                             index_type     i) const {
    if (!is_standardized()) {
      // We must standardize here o/w there's no bijection between the numbers
      // 0, ..., n - 1 on to the nodes of the word graph.
      // Or worse, there's no guarantee that _forest is populated or is a
      // spanning tree of the current word graph
      // TODO(1) bit fishy here too
      const_cast<ToddCoxeterBase*>(this)->standardize(Order::shortlex);
    }
    if (!internal_presentation().contains_empty_word()) {
      ++i;
    }

    word_type result;  // TODO(1) avoid alloc here
    _forest.path_to_root_no_checks(std::back_inserter(result), i);
    return std::copy(result.crbegin(), result.crend(), d_first);
  }

  template <typename OutputIterator>
  OutputIterator ToddCoxeterBase::current_word_of(OutputIterator d_first,
                                                  index_type     i) const {
    size_t const offset
        = (internal_presentation().contains_empty_word() ? 0 : 1);

    if (!is_standardized()) {
      // We must standardize here o/w there's no bijection between the numbers
      // 0, ..., n - 1 on to the nodes of the word graph.
      // Or worse, there's no guarantee that _forest is populated or is a
      // spanning tree of the current word graph
      // TODO(1) bit fishy here too
      const_cast<ToddCoxeterBase*>(this)->standardize(Order::shortlex);
    }
    if (i >= _word_graph.number_of_nodes_active() - offset) {
      // Maybe we shouldn't standardize but should just check if corresponds to
      // an active node
      LIBSEMIGROUPS_EXCEPTION("invalid class index, expected a value in "
                              "the range [0, {}), found {}",
                              _word_graph.number_of_nodes_active() - offset,
                              i);
    }
    return current_word_of_no_checks(d_first, i);
  }

  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  tril ToddCoxeterBase::currently_contains_no_checks(Iterator1 first1,
                                                     Iterator2 last1,
                                                     Iterator3 first2,
                                                     Iterator4 last2) const {
    if (std::equal(first1, last1, first2, last2)) {
      return tril::TRUE;
    }
    auto i1 = current_index_of_no_checks(first1, last1);
    auto i2 = current_index_of_no_checks(first2, last2);
    if (i1 == UNDEFINED || i2 == UNDEFINED) {
      return tril::unknown;
    } else if (i1 == i2) {
      return tril::TRUE;
    } else if (finished()) {
      return tril::FALSE;
    } else {
      return tril::unknown;
    }
  }

  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  bool ToddCoxeterBase::contains_no_checks(Iterator1 first1,
                                           Iterator2 last1,
                                           Iterator3 first2,
                                           Iterator4 last2) {
    // TODO(1) remove when is_free is implemented
    if (internal_presentation().rules.empty()
        && internal_generating_pairs().empty()
        && current_word_graph().number_of_nodes_active() == 1) {
      return std::equal(first1, last1, first2, last2);
    }
    return CongruenceInterface::contains_no_checks<ToddCoxeterBase>(
        first1, last1, first2, last2);
  }

  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  bool ToddCoxeterBase::contains(Iterator1 first1,
                                 Iterator2 last1,
                                 Iterator3 first2,
                                 Iterator4 last2) {
    // TODO(1) remove when is_free is implemented
    if (internal_presentation().rules.empty()
        && internal_generating_pairs().empty()
        && current_word_graph().number_of_nodes_active() == 1) {
      return std::equal(first1, last1, first2, last2);
    }
    return CongruenceInterface::contains<ToddCoxeterBase>(
        first1, last1, first2, last2);
  }

  template <typename OutputIterator,
            typename InputIterator1,
            typename InputIterator2>
  OutputIterator
  ToddCoxeterBase::reduce_no_run_no_checks(OutputIterator d_first,
                                           InputIterator1 first,
                                           InputIterator2 last) const {
    if (!is_standardized()) {
      // TODO(1) this is a bit fishy
      const_cast<ToddCoxeterBase*>(this)->standardize(Order::shortlex);
    }
    return current_word_of_no_checks(d_first,
                                     current_index_of_no_checks(first, last));
  }
}  // namespace libsemigroups
