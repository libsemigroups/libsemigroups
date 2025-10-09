//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 James D. Mitchell + Maria Tsalakou
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

// This file contains the helpers for the KnuthBendix class template.

#ifndef LIBSEMIGROUPS_KNUTH_BENDIX_HELPERS_HPP_
#define LIBSEMIGROUPS_KNUTH_BENDIX_HELPERS_HPP_

#include <cstddef>      // for size_t
#include <iterator>     // for distance
#include <stack>        // for stack
#include <string>       // for basic_string, string
#include <type_traits>  // for is_same_v
#include <utility>      // for move
#include <vector>       // for vector

#include "cong-common-helpers.hpp"  // for partition, add_gener...
#include "constants.hpp"            // for UNDEFINED, POSITIVE_...
#include "debug.hpp"                // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"            // for LIBSEMIGROUPS_EXCEPTION
#include "knuth-bendix-class.hpp"   // for KnuthBendix
#include "paths.hpp"                // for Paths
#include "presentation.hpp"         // for Presentation
#include "ranges.hpp"               // for seq, input_range_ite...
#include "types.hpp"                // for congruence_kind, wor...
#include "word-graph-helpers.hpp"   // for word_graph
#include "word-graph.hpp"           // for WordGraph
#include "word-range.hpp"           // for ToString

#include "detail/fmt.hpp"              // for format
#include "detail/knuth-bendix-nf.hpp"  // for KnuthBendix, KnuthBe...
#include "detail/rewriters.hpp"        // for internal_string_type

namespace libsemigroups {

  namespace congruence_common {
    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns a range object containing the normal forms.
    //!
    //! Defined in \c knuth-bendix-helpers.hpp.
    //!
    //! This function returns a range object containing normal forms of the
    //! classes of the congruence represented by an instance of
    //! \ref_knuth_bendix. The order of the classes, and the normal form that is
    //! returned, are controlled by the reduction order used to construct \p kb.
    //! This function triggers a full enumeration of \p kb.
    //!
    //! \tparam Word the type of the words contained in the output range.
    //! \tparam Rewriter the first template parameter for \ref_knuth_bendix.
    //! \tparam ReductionOrder the second template parameter for
    //! \ref_knuth_bendix.
    //!
    //! \param kb the \ref_knuth_bendix instance.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \cong_common_warn_undecidable{Knuth-Bendix}.
    template <typename Word, typename Rewriter, typename ReductionOrder>
    [[nodiscard]] auto
    normal_forms(KnuthBendix<Word, Rewriter, ReductionOrder>& kb) {
      return detail::KnuthBendixNormalFormRange<Word, Rewriter, ReductionOrder>(
          kb);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    //! \brief Find the non-trivial classes of the quotient of one
    //! \ref_knuth_bendix instance in another.
    //!
    //! Defined in \c knuth-bendix-helpers.hpp.
    //!
    //! This function returns the classes with size at least \f$2\f$ in the
    //! normal forms of \p kb2 in \p kb1 (the greater congruence, with fewer
    //! classes). This function triggers a full enumeration of both \p kb2 and
    //! \p kb1.
    //!
    //! Note that this function does **not** compute the normal forms of \p kb2
    //! and try to compute the partition of these induced by \p kb1, before
    //! filtering out the classes of size \f$1\f$. In particular, it is possible
    //! to compute the non-trivial classes of \p kb1 in \p kb2 if there are only
    //! finitely many finite such classes, regardless of whether or not \p kb2
    //! or \p kb1 has infinitely many classes.
    //!
    //! \tparam Word the type of the words contained in the output range
    //! (default: std::string).
    //! \tparam Rewriter the first template parameter for \ref_knuth_bendix.
    //! \tparam ReductionOrder the second template parameter for
    //! \ref_knuth_bendix.
    //!
    //! \param kb1 the first \ref_knuth_bendix instance.
    //! \param kb2 the second \ref_knuth_bendix instance.
    //!
    //! \returns The non-trivial classes of \p kb1 in \p kb2.
    //!
    //! \throws LibsemigroupsException if \p kb1 has infinitely many classes
    //! and \p kb2 has finitely many classes (so that there is at least one
    //! infinite non-trivial class).
    //!
    //! \throws LibsemigroupsException if the alphabets of the
    //! presentations of \p kb1 and \p kb2 are not equal.
    //!
    //! \throws LibsemigroupsException if the \ref_knuth_bendix::gilman_graph
    //! of
    //! \p kb1 has fewer nodes than that of \p kb2.
    //!
    //! \cong_common_warn_undecidable{Knuth-Bendix}.
    template <typename Word, typename Rewriter, typename ReductionOrder>
    [[nodiscard]] std::vector<std::vector<Word>>
    non_trivial_classes(KnuthBendix<Word, Rewriter, ReductionOrder>& kb1,
                        KnuthBendix<Word, Rewriter, ReductionOrder>& kb2);

  }  // namespace congruence_common

  //! \defgroup knuth_bendix_helpers_group Knuth-Bendix helper functions
  //! \ingroup knuth_bendix_group
  //!
  //! \brief Helper functions for the \ref_knuth_bendix class.
  //!
  //! Defined in \c knuth-bendix-helpers.hpp.
  //!
  //! This page contains documentation for some helper functions for the
  //! \ref_knuth_bendix class. In particular, these functions include versions
  //! of several of the member functions of \ref_knuth_bendix (that accept
  //! iterators) whose parameters are not iterators, but objects instead. The
  //! helpers documented on this page all belong to the namespace
  //! \ref knuth_bendix.
  //!
  //! \sa \ref cong_common_helpers_group

  //! This page contains documentation for everything in the namespace
  //! \ref knuth_bendix. This includes everything from
  //! \ref cong_common_helpers_group and \ref knuth_bendix_helpers_group.
  namespace knuth_bendix {

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix specific helpers
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup knuth_bendix_helpers_group
    //!
    //! \brief Run the Knuth-Bendix algorithm by considering all overlaps of
    //! a given length.
    //!
    //! Defined in \c knuth-bendix-helpers.hpp.
    //!
    //! This function runs the Knuth-Bendix algorithm on the rewriting
    //! system represented by a \ref_knuth_bendix instance by considering all
    //! overlaps of a given length \f$n\f$ (according to the
    //! \ref KnuthBendix::options::overlap) before those overlaps of length \f$n
    //! + 1\f$.
    //!
    //! \tparam Word the type of the words in the
    //! \ref KnuthBendix::presentation.
    //! \tparam Rewriter the first template parameter for \ref_knuth_bendix.
    //! \tparam ReductionOrder the second template parameter for
    //! \ref_knuth_bendix.
    //!
    //! \param kb the \ref_knuth_bendix instance.
    //!
    //! \complexity
    //! See warning.
    //!
    //! \warning This will terminate when the \ref_knuth_bendix instance is
    //! confluent, which might be never.
    //!
    //! \sa KnuthBendix::run.
    template <typename Word, typename Rewriter, typename ReductionOrder>
    void by_overlap_length(KnuthBendix<Word, Rewriter, ReductionOrder>& kb);

    //! \ingroup knuth_bendix_helpers_group
    //!
    //! \brief Check if the all rules are reduced with respect to each other.
    //!
    //! Defined in \c knuth-bendix-helpers.hpp.
    //!
    //! \tparam Word the type of the words in the
    //! \ref KnuthBendix::presentation.
    //! \tparam Rewriter the first template parameter for \ref_knuth_bendix.
    //! \tparam ReductionOrder the second template parameter for
    //! \ref_knuth_bendix.
    //!
    //! \param kb the \ref_knuth_bendix instance defining the rules that are to
    //! be checked for being reduced.
    //!
    //! \returns \c true if for each pair \f$(A, B)\f$ and \f$(C, D)\f$ of rules
    //! stored within the \ref_knuth_bendix instance, \f$C\f$ is neither a
    //! subword of \f$A\f$ nor \f$B\f$. Returns \c false otherwise.
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    template <typename Rewriter, typename ReductionOrder>
    [[nodiscard]] bool
    is_reduced(detail::KnuthBendixImpl<Rewriter, ReductionOrder>& kb);
#else
    template <typename Word, typename Rewriter, typename ReductionOrder>
    [[nodiscard]] bool
    is_reduced(KnuthBendix<Word, Rewriter, ReductionOrder>& kb);
#endif

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::add_generating_pair;
    using congruence_common::add_generating_pair_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::contains;
    using congruence_common::contains_no_checks;
    using congruence_common::currently_contains;
    using congruence_common::currently_contains_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::reduce;
    using congruence_common::reduce_no_checks;
    using congruence_common::reduce_no_run;
    using congruence_common::reduce_no_run_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::normal_forms;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::partition;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::non_trivial_classes;

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - redundant_rule
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup knuth_bendix_helpers_group
    //! \brief Return an iterator pointing at the left hand side of a redundant
    //! rule.
    //!
    //! Defined in \c knuth-bendix-helpers.hpp.
    //!
    //! Starting with the last rule in the presentation, this function
    //! attempts to run the Knuth-Bendix algorithm on the rules of the
    //! presentation except for the given omitted rule. For every such omitted
    //! rule, Knuth-Bendix is run for the length of time indicated by the
    //! second parameter \p t, and then it is checked if the omitted rule can
    //! be shown to be redundant (rewriting both sides of the omitted rule
    //! using the other rules using the output of the, not necessarily
    //! finished, Knuth-Bendix algorithm).
    //!
    //! If the omitted rule can be shown to be redundant in this way, then an
    //! iterator pointing to its left hand side is returned.
    //!
    //! If no rule can be shown to be redundant in this way, then an iterator
    //! pointing to \c p.cend() is returned.
    //!
    //! \tparam Word type of words in the Presentation \p p.
    //! \tparam Time type of the 2nd parameter (time to try running
    //! Knuth-Bendix).
    //! \param p the presentation.
    //! \param t time to run \ref_knuth_bendix for every omitted rule.
    //!
    //! \warning The progress of the Knuth-Bendix algorithm may differ between
    //! different calls to this function even if the parameters are identical.
    //! As such this is non-deterministic, and may produce different results
    //! with the same input.
    template <typename Word, typename Time>
    [[nodiscard]] typename std::vector<Word>::const_iterator
    redundant_rule(Presentation<Word> const& p, Time t);

  }  // namespace knuth_bendix
}  // namespace libsemigroups

#include "knuth-bendix-helpers.tpp"
#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_HELPERS_HPP_
