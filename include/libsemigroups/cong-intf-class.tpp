//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 James D. Mitchell
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

// This file contains the implementations of member function templates for the
// CongruenceInterface class.

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Out of line member functions for CongruenceInterface
  ////////////////////////////////////////////////////////////////////////

  template <typename Subclass,
            typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  tril CongruenceInterface::currently_contains(Iterator1 first1,
                                               Iterator2 last1,
                                               Iterator3 first2,
                                               Iterator4 last2) const {
    throw_if_letter_out_of_bounds<Subclass>(first1, last1);
    throw_if_letter_out_of_bounds<Subclass>(first2, last2);
    return static_cast<Subclass const&>(*this).currently_contains_no_checks(
        first1, last1, first2, last2);
  }

  template <typename Subclass,
            typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  bool CongruenceInterface::contains_no_checks(Iterator1 first1,
                                               Iterator2 last1,
                                               Iterator3 first2,
                                               Iterator4 last2) {
    if (std::equal(first1, last1, first2, last2)) {
      return true;
    }
    // TODO(1) implement is_free mem fn of derived classes and check equality
    // if this returns true
    run();
    return static_cast<Subclass&>(*this).currently_contains_no_checks(
               first1, last1, first2, last2)
           == tril::TRUE;
  }

  template <typename Subclass,
            typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  Subclass&
  CongruenceInterface::add_internal_generating_pair_no_checks(Iterator1 first1,
                                                              Iterator2 last1,
                                                              Iterator3 first2,
                                                              Iterator4 last2) {
    LIBSEMIGROUPS_ASSERT(!started());
    _internal_generating_pairs.emplace_back(first1, last1);
    _internal_generating_pairs.emplace_back(first2, last2);
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass,
            typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  Subclass& CongruenceInterface::add_generating_pair(Iterator1 first1,
                                                     Iterator2 last1,
                                                     Iterator3 first2,
                                                     Iterator4 last2) {
    throw_if_started();
    throw_if_letter_out_of_bounds<Subclass>(first1, last1);
    throw_if_letter_out_of_bounds<Subclass>(first2, last2);
    return static_cast<Subclass&>(*this).add_generating_pair_no_checks(
        first1, last1, first2, last2);
  }

  template <typename Subclass,
            typename OutputIterator,
            typename Iterator1,
            typename Iterator2>
  OutputIterator CongruenceInterface::reduce_no_run(OutputIterator d_first,
                                                    Iterator1      first,
                                                    Iterator2      last) const {
    throw_if_letter_out_of_bounds<Subclass>(first, last);
    if (finished() && !success()) {  // for Kambites
      LIBSEMIGROUPS_EXCEPTION(
          "cannot reduce words, the algorithm failed to finish successfully!")
    }
    return static_cast<Subclass const&>(*this).reduce_no_run_no_checks(
        d_first, first, last);
  }

  template <typename Subclass,
            typename OutputIterator,
            typename InputIterator1,
            typename InputIterator2>
  OutputIterator CongruenceInterface::reduce(OutputIterator d_first,
                                             InputIterator1 first,
                                             InputIterator2 last) {
    throw_if_letter_out_of_bounds<Subclass>(first, last);
    run();
    if (!success()) {  // for Kambites
      LIBSEMIGROUPS_EXCEPTION(
          "cannot reduce words, the algorithm failed to finish successfully!")
    }
    return static_cast<Subclass&>(*this).reduce_no_run_no_checks(
        d_first, first, last);
  }
}  // namespace libsemigroups
