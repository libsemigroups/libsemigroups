//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2026 James D. Mitchell + Maria Tsalakou
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

#include <algorithm>      // for next_permutation
#include <atomic>         // for atomic_size_t
#include <chrono>         // for nanoseconds
#include <cstddef>        // for size_t
#include <memory>         // for make_shared
#include <mutex>          // for mutex, lock_guard
#include <numeric>        // for iota
#include <optional>       // for optional, make...
#include <queue>          // for queue
#include <stack>          // for stack
#include <stdint.h>       // for uint16_t, uint...
#include <string>         // for basic_string
#include <string_view>    // for basic_string_view
#include <type_traits>    // for is_same_v, con...
#include <unordered_set>  // for unordered_set
#include <utility>        // for move
#include <vector>         // for vector

#include <iostream>

#include "adapters.hpp"                  // ReturnFalse
#include "cong-common-helpers.hpp"       // for partition, add_gener...
#include "constants.hpp"                 // for UNDEFINED, POSITIVE_...
#include "debug.hpp"                     // for LIBSEMIGROUPS_ASSERT
#include "du-narendran-rusinowitch.hpp"  // for du_narendran_narendran
#include "exception.hpp"                 // for LIBSEMIGROUPS_EXCEPTION
#include "knuth-bendix-class.hpp"        // for KnuthBendix
#include "paths.hpp"                     // for Paths
#include "presentation.hpp"              // for Presentation
#include "ranges.hpp"                    // for seq, input_range_ite...
#include "types.hpp"                     // for congruence_kind, wor...
#include "word-graph-helpers.hpp"        // for word_graph
#include "word-graph.hpp"                // for WordGraph
#include "word-range.hpp"                // for ToString

#include "detail/fmt.hpp"               // for format
#include "detail/knuth-bendix-nf.hpp"   // for KnuthBendix, KnuthBe...
#include "detail/race.hpp"              // for Race
#include "detail/report.hpp"            // for ReportGuard
#include "detail/rewriting-system.hpp"  // for internal_string_type
#include "detail/stl.hpp"               // for apply_permutation
#include "detail/string.hpp"            // for group_digits
#include "detail/timer.hpp"             // for string_time

namespace libsemigroups {

  namespace congruence_common {
    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns a range object containing the normal forms.
    //!
    //! Defined in `knuth-bendix-helpers.hpp`.
    //!
    //! This function returns a range object containing normal forms of the
    //! classes of the congruence represented by an instance of
    //! \ref_knuth_bendix. The order of the classes, and the normal form that is
    //! returned, are controlled by the reduction order used to construct \p kb.
    //! This function triggers a full enumeration of \p kb.
    //!
    //! \tparam Word the type of the words contained in the output range.
    //! \tparam RewritingSystem the first template parameter for
    //! \ref_knuth_bendix.
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
    template <typename Word, typename RewritingSystem, typename ReductionOrder>
    [[nodiscard]] auto
    normal_forms(KnuthBendix<Word, RewritingSystem, ReductionOrder>& kb) {
      return detail::
          KnuthBendixNormalFormRange<Word, RewritingSystem, ReductionOrder>(kb);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    //! \brief Find the non-trivial classes of the quotient of one
    //! \ref_knuth_bendix instance in another.
    //!
    //! Defined in `knuth-bendix-helpers.hpp`.
    //!
    //! This function returns the classes with size at least \f$2\f$ in the
    //! normal forms of \p kb1 in \p kb2 (the greater congruence, with fewer
    //! classes). This function triggers a full enumeration of both \p kb1 and
    //! \p kb2.
    //!
    //! Note that this function does **not** compute the normal forms of \p kb1
    //! and try to compute the partition of these induced by \p kb2, before
    //! filtering out the classes of size \f$1\f$. In particular, it is possible
    //! to compute the non-trivial classes of the normal forms of \p kb1 in
    //! \p kb2 if there are only finitely many finite such classes, regardless
    //! of whether or not \p kb1 or \p kb2 has infinitely many classes.
    //!
    //! \tparam Word the type of the words contained in the output range
    //! (default: std::string).
    //! \tparam RewritingSystem the first template parameter for
    //! \ref_knuth_bendix.
    //! \tparam ReductionOrder the second template parameter for
    //! \ref_knuth_bendix.
    //!
    //! \param kb1 the \ref_knuth_bendix instance whose normal forms are being
    //! partitioned.
    //! \param kb2 the \ref_knuth_bendix instance being used to compute the
    //! partition.
    //!
    //! \returns The non-trivial classes of the normal forms of \p kb1 in
    //! \p kb2.
    //!
    //! \throws LibsemigroupsException if \p kb1 has infinitely many classes
    //! and \p kb2 has finitely many classes (so that there is at least one
    //! infinite non-trivial class).
    //!
    //! \throws LibsemigroupsException if the alphabets of the
    //! presentations of \p kb1 and \p kb2 are not equal.
    //!
    //! \throws LibsemigroupsException if the \ref_knuth_bendix::gilman_graph
    //! of \p kb1 has fewer nodes than that of \p kb2.
    //!
    //! \cong_common_warn_undecidable{Knuth-Bendix}.
    template <typename Word, typename RewritingSystem, typename ReductionOrder>
    [[nodiscard]] std::vector<std::vector<Word>> non_trivial_classes(
        KnuthBendix<Word, RewritingSystem, ReductionOrder>& kb1,
        KnuthBendix<Word, RewritingSystem, ReductionOrder>& kb2);

  }  // namespace congruence_common

  //! \defgroup knuth_bendix_helpers_group Knuth-Bendix helper functions
  //! \ingroup knuth_bendix_group
  //!
  //! \brief Helper functions for the \ref_knuth_bendix class.
  //!
  //! Defined in `knuth-bendix-helpers.hpp`.
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
    //! Defined in `knuth-bendix-helpers.hpp`.
    //!
    //! This function runs the Knuth-Bendix algorithm on the rewriting
    //! system represented by a \ref_knuth_bendix instance by considering all
    //! overlaps of a given length \f$n\f$ (according to the
    //! \ref KnuthBendix::options::overlap) before those overlaps of length \f$n
    //! + 1\f$.
    //!
    //! \tparam Word the type of the words in the
    //! \ref KnuthBendix::presentation.
    //! \tparam RewritingSystem the first template parameter for
    //! \ref_knuth_bendix.
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
    template <typename Word, typename RewritingSystem, typename ReductionOrder>
    void
    by_overlap_length(KnuthBendix<Word, RewritingSystem, ReductionOrder>& kb);

    //! \ingroup knuth_bendix_helpers_group
    //!
    //! \brief Check if the all rules are reduced with respect to each other.
    //!
    //! Defined in `knuth-bendix-helpers.hpp`.
    //!
    //! \tparam Word the type of the words in the
    //! \ref KnuthBendix::presentation.
    //! \tparam RewritingSystem the first template parameter for
    //! \ref_knuth_bendix.
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
    template <typename RewritingSystem, typename ReductionOrder>
    [[nodiscard]] bool
    is_reduced(detail::KnuthBendixImpl<RewritingSystem, ReductionOrder>& kb);
#else
    template <typename Word, typename RewritingSystem, typename ReductionOrder>
    [[nodiscard]] bool
    is_reduced(KnuthBendix<Word, RewritingSystem, ReductionOrder>& kb);
#endif
  }  // namespace knuth_bendix

  namespace detail {
    inline bool
    orient_and_check(Presentation<word_type>&                             p,
                     KnuthBendix<word_type, RewritingSystemTrie<RPOCmp>>& kb,
                     uint32_t orientation_index) {
      for (size_t i = 0; i < p.rules.size() / 2; ++i) {
        // Swap the ith lhs and rhs if the ith bit of rule_orientation_index
        // is a 1.
        word_type& lhs = p.rules[2 * i];
        word_type& rhs = p.rules[(2 * i) + 1];
        if (orientation_index & (1 << i)) {
          std::swap(lhs, rhs);
        }
        if (lhs.size() == 0) {
          std::cout << "skipping (empty left-hand-side)" << std::endl;
          return false;
        }
      }

      std::cout << "checking alphabet order ... ";
      word_type oriented_alphabet = du_narendran_rusinowitch(p);
      if (!oriented_alphabet.empty()) {
        std::cout << "success! Checking confluence ... ";
        p.alphabet(oriented_alphabet);
        kb.init(congruence_kind::twosided, p);
        kb.run_for(std::chrono::milliseconds(10));
        if (kb.finished()) {
          std::cout << "success!" << std::endl;
          p.alphabet(oriented_alphabet);
          return true;
        }
      }
      std::cout << "failure" << std::endl;
      return false;
    }
  }  // namespace detail

  namespace knuth_bendix {

    // Try all rule orientations, check for confluence, then check to see if
    // there is an alphabet order for which lhs >_rpo rhs for all of the rules
    inline bool order_search(Presentation<word_type>& p) {
      size_t num_rules = p.rules.size() / 2;
      if (num_rules > 31) {
        LIBSEMIGROUPS_EXCEPTION(
            "order_search can only be called with presentations that have at "
            "most 31 rules, found {}",
            num_rules);
      }

      Presentation<word_type> oriented_presentation;
      KnuthBendix<word_type, detail::RewritingSystemTrie<RPOCmp>> kb;
      uint32_t const num_rule_orientations = uint32_t{1} << num_rules;

      // The binary representation of each number in [0, num_rule_orientations)
      // specifies which left-hand-sides and right-hand-sides to swap.
      for (uint32_t orientation_index = 0;
           orientation_index < num_rule_orientations;
           ++orientation_index) {
        oriented_presentation = p;
        std::cout << "Checking " << orientation_index + 1 << "/"
                  << num_rule_orientations << " ("
                  << 100
                         * (static_cast<float>(orientation_index + 1)
                            / static_cast<float>(num_rule_orientations))
                  << "%): ";
        if (detail::orient_and_check(
                oriented_presentation, kb, orientation_index)) {
          p = oriented_presentation;
          return true;
        }
      }
      return false;
    }

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
    //! Defined in `knuth-bendix-helpers.hpp`.
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

    //! \ingroup knuth_bendix_helpers_group
    //!
    //! \brief Search for a finite complete rewriting system using Tietze
    //! transformations.
    //!
    //! Defined in `knuth-bendix-helpers.hpp`.
    //!
    //! This class searches for a presentation for which the Knuth-Bendix
    //! algorithm terminates, by introducing new generators for subwords of the
    //! rules and trying all different orders on the resulting alphabet.
    //!
    //! More precisely, an instance of this class starts with the presentation
    //! of the \ref_knuth_bendix instance used to construct it. It then forms
    //! presentations obtained by repeatedly replacing a non-empty subword of
    //! length at least 2 by a new generator. The number of such replacements is
    //! controlled by \ref depth_min and \ref depth_max. For every presentation
    //! in this search, the alphabet is permuted in every possible way, and the
    //! Knuth-Bendix algorithm is run for \ref run_each_for. The search
    //! succeeds if one of these runs produces a confluent rewriting system.
    //!
    //! Since this class derives from \ref Runner, the search can be run to
    //! completion using \ref Runner::run, for a bounded amount of time using
    //! \ref Runner::run_for, or until a predicate holds using
    //! \ref Runner::run_until. The member function \ref result is the usual
    //! way to run the search and obtain the successful \ref_knuth_bendix
    //! instance, if one was found.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \tparam RewritingSystem the type of the rewriting system used by the
    //! underlying \ref_knuth_bendix objects.
    //!
    //! \warning This class can try a very large number of presentations. If
    //! the initial alphabet has size \f$n\f$, then introducing \f$d\f$ new
    //! generators requires trying permutations of an alphabet with \f$n + d\f$
    //! letters. In particular, \ref Runner::run throws if the initial alphabet
    //! size plus \ref depth_max is greater than `20`. It's highly unlikely to
    //! complete on alphabets of size greater than `3` or `4`.
    //!
    //! \warning The result may depend on the amount of time allowed for each
    //! Knuth-Bendix run, the number of threads, and the order in which the
    //! search is explored.
    template <typename Word, typename RewritingSystem>
    class TietzeExplorer : public Runner {
     private:
      class TietzeRunner : public Runner {
        KnuthBendix<Word, RewritingSystem> _kb;
        TietzeExplorer*                    _enclosing;

       public:
        explicit TietzeRunner(TietzeExplorer* enclosing)
            : Runner(), _kb(enclosing->knuth_bendix()), _enclosing(enclosing) {
          Runner::report_prefix("TietzeExplorer");
        }

        KnuthBendix<Word, RewritingSystem> const&
        knuth_bendix() const noexcept {
          return _kb;
        }

       private:
        [[nodiscard]] bool run_one(Presentation<Word>       p,
                                   std::vector<Word> const& subwords);

        void run_impl() override;

        [[nodiscard]] bool finished_impl() const override {
          return _kb.finished();
        }
      };  // class TietzeRunner

      ////////////////////////////////////////////////////////////////////////
      // Private data
      ////////////////////////////////////////////////////////////////////////
      mutable std::atomic_size_t _counter;
      mutable std::vector<Word>  _current_subwords_replaced_with_new_generators;
      mutable KnuthBendix<Word, RewritingSystem> _kb;
      mutable std::mutex                         _mtx;
      mutable size_t                             _number_of_runs;
      mutable Presentation<Word>                 _presentation;
      mutable std::queue<std::vector<Word>>      _todo;
      mutable bool                               _todo_populated;

      size_t                   _depth_max;
      size_t                   _depth_min;
      bool                     _finished;
      size_t                   _number_of_threads;
      detail::Race             _race;
      std::chrono::nanoseconds _run_each_for;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Constructors + Initializers
      ////////////////////////////////////////////////////////////////////////

      //! \brief Construct from a \ref_knuth_bendix instance.
      //!
      //! Constructs a TietzeExplorer using the kind and presentation of
      //! \p kb.
      //!
      //! The default settings are:
      //! * \ref depth_min is `0`;
      //! * \ref depth_max is `3`;
      //! * \ref run_each_for is `std::chrono::milliseconds(5)`;
      //! * \ref number_of_threads is `1`.
      //!
      //! \param kb the \ref_knuth_bendix instance whose presentation is to be
      //! explored.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      explicit TietzeExplorer(KnuthBendix<Word, RewritingSystem>& kb);

      //! \brief Reinitialize an existing TietzeExplorer.
      //!
      //! This function puts a TietzeExplorer object back into the same state as
      //! if it had been newly constructed from \p kb.
      //!
      //! \param kb the \ref_knuth_bendix instance whose presentation is to be
      //! explored.
      //!
      //! \returns A reference to \c this.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      TietzeExplorer& init(KnuthBendix<Word, RewritingSystem>& kb);

      //! \brief Copy constructor.
      TietzeExplorer(TietzeExplorer const&) = default;

      //! \brief Move constructor.
      TietzeExplorer(TietzeExplorer&&) = default;

      //! \brief Copy assignment operator.
      TietzeExplorer& operator=(TietzeExplorer const&) = default;

      //! \brief Move assignment operator.
      TietzeExplorer& operator=(TietzeExplorer&&) = default;

      ~TietzeExplorer() = default;

      ////////////////////////////////////////////////////////////////////////
      // Settings
      ////////////////////////////////////////////////////////////////////////

      //! \brief Return the initial \ref_knuth_bendix instance.
      //!
      //! Returns a const reference to the \ref_knuth_bendix instance supplied
      //! at construction or most recent initialization.
      //!
      //! \returns A const reference to a \ref_knuth_bendix instance.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] KnuthBendix<Word, RewritingSystem> const&
      knuth_bendix() const noexcept {
        return _kb;
      }

      //! \brief Get the maximum search depth.
      //!
      //! Returns the maximum number of subword replacements by new generators
      //! to perform when constructing presentations for the search.
      //!
      //! The default value is `3`.
      //!
      //! \returns A maximum number of new generators introduced.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t depth_max() const noexcept {
        return _depth_max;
      }

      //! \brief Set the maximum search depth.
      //!
      //! This function sets the maximum number of subword replacements by new
      //! generators to perform when constructing presentations for the search.
      //!
      //! The default value is `3`.
      //!
      //! \param val the maximum search depth.
      //!
      //! \returns A reference to \c this.
      //!
      //! \throws LibsemigroupsException if the internal search queue has
      //! already been populated.
      TietzeExplorer& depth_max(size_t val) {
        throw_if_todo_populated("depth_max");
        _depth_max = val;
        return *this;
      }

      //! \brief Get the minimum search depth.
      //!
      //! Returns the minimum number of subword replacements by new generators
      //! required for a presentation to be tried.
      //!
      //! The default value is `0`.
      //!
      //! \returns The minimum number of new generators to introduce.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t depth_min() const noexcept {
        return _depth_min;
      }

      //! \brief Set the minimum search depth.
      //!
      //! This function sets the minimum number of subword replacements by new
      //! generators required for a presentation to be tried.
      //!
      //! The default value is `0`.
      //!
      //! \param val the minimum search depth.
      //!
      //! \returns A reference to \c this.
      //!
      //! \throws LibsemigroupsException if the internal search queue has
      //! already been populated.
      TietzeExplorer& depth_min(size_t val) {
        throw_if_todo_populated("depth_min");
        _depth_min = val;
        return *this;
      }

      //! \brief Get the time allowed for each Knuth-Bendix run.
      //!
      //! Returns the amount of time for which Knuth-Bendix is run for each
      //! presentation and alphabet order tried by the search.
      //!
      //! The default value is `std::chrono::milliseconds(5)`.
      //!
      //! \returns A value of type `std::chrono::nanoseconds`.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] std::chrono::nanoseconds run_each_for() const noexcept {
        return _run_each_for;
      }

      //! \brief Set the time allowed for each Knuth-Bendix run.
      //!
      //! This function sets the amount of time for which Knuth-Bendix is run
      //! for each presentation and alphabet order tried by the search.
      //!
      //! The default value is `std::chrono::milliseconds(5)`.
      //!
      //! \param val the amount of time to run each Knuth-Bendix instance for.
      //!
      //! \returns A reference to \c this.
      //!
      //! \exceptions
      //! \noexcept
      TietzeExplorer& run_each_for(std::chrono::nanoseconds val) noexcept {
        _run_each_for = val;
        return *this;
      }

      //! \brief Get the number of threads.
      //!
      //! Returns the number of threads used to run the search.
      //!
      //! The default value is `1`.
      //!
      //! \returns A `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t number_of_threads() const noexcept {
        return _number_of_threads;
      }

      //! \brief Set the number of threads.
      //!
      //! This function sets the number of threads used to run the search.
      //!
      //! The default value is `1`.
      //!
      //! \param val the number of threads to use.
      //!
      //! \returns A reference to \c this.
      //!
      //! \throws LibsemigroupsException if \p val is 0.
      TietzeExplorer& number_of_threads(size_t val);

      ////////////////////////////////////////////////////////////////////////
      // The main event
      ////////////////////////////////////////////////////////////////////////

      //! \brief Estimate the running time of the search.
      //!
      //! Returns \ref number_of_runs multiplied by \ref run_each_for and
      //! divided by \ref number_of_threads.
      //!
      //! \returns A value of type `std::chrono::nanoseconds`.
      //!
      //! \warning This is only a crude upper bound based on the number of
      //! presentations and alphabet orders to be tried. It does not account for
      //! early successful termination, or the overhead from generating the
      //! search.
      [[nodiscard]] std::chrono::nanoseconds estimated_run_time() const;

      //! \brief Return the number of Knuth-Bendix runs to try.
      //!
      //! Returns the number of presentations and alphabet orders that will be
      //! tried by the search, subject to the current values of \ref depth_min
      //! and \ref depth_max.
      //!
      //! \returns A `size_t`.
      //!
      //! \warning This function populates the internal search queue. Changing
      //! \ref depth_min or \ref depth_max after calling this function are not
      //! allowed and will throw an exception.
      [[nodiscard]] size_t number_of_runs() const;

      //! \brief Run the search and return a successful Knuth-Bendix instance.
      //!
      //! This function runs the search, if it has not already finished, and
      //! returns the first \ref_knuth_bendix instance found whose rewriting
      //! system is confluent.
      //!
      //! \returns A \c std::optional containing a \ref_knuth_bendix instance if
      //! the search succeeds, and \c std::nullopt otherwise.
      //!
      //! \throws LibsemigroupsException if the initial alphabet size plus
      //! \ref depth_max is greater than 20.
      //!
      //! \sa Runner::run
      [[nodiscard]] std::optional<KnuthBendix<Word, RewritingSystem>> result();

      //! \brief Check whether the search finished successfully.
      //!
      //! Returns \c true if the search has finished and found a successful
      //! \ref_knuth_bendix instance, and \c false otherwise.
      //!
      //! \returns A \c bool.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] bool success() const noexcept override {
        // TODO separate _race->finished() and success() and just return
        // _race.success() here
        return _finished && _race.finished();
      }

     private:
      void run_impl() override;

      [[nodiscard]] bool finished_impl() const override {
        // Can't use _race.finished() because it only returns true if there's a
        // winner. See comment above
        return _finished;
      }

      void dfs(Presentation<Word>& p, size_t depth = 0) const;

      void populate_todo() const;

      [[nodiscard]] bool try_pop_todo(std::vector<Word>& result);

      void report_before_run() const;
      void report_progress_from_thread() const;
      void report_after_run() const;

      void throw_if_todo_populated(std::string_view msg) const;
    };  // class TietzeExplorer
  }     // namespace knuth_bendix
}  // namespace libsemigroups

#include "knuth-bendix-helpers.tpp"

#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_HELPERS_HPP_
