//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 James D. Mitchell + Joseph Edwards
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

// This file contains a class KnuthBendixBase which implements the Knuth-Bendix
// algorithm for finitely presented monoids.

// TODO(1)
// * noexcept
// * separate rule container from Rules
// * nodiscard

#ifndef LIBSEMIGROUPS_DETAIL_KNUTH_BENDIX_BASE_HPP_
#define LIBSEMIGROUPS_DETAIL_KNUTH_BENDIX_BASE_HPP_

#include <algorithm>      // for max, min, copy
#include <atomic>         // for atomic_bool
#include <cctype>         // for isprint
#include <chrono>         // for duration_cast
#include <cmath>          // for pow
#include <cstddef>        // for size_t
#include <cstdint>        // for int64_t, uint32_t
#include <iterator>       // for back_inserter
#include <memory>         // for allocator, uni...
#include <ostream>        // for ostream
#include <string>         // for basic_string
#include <string_view>    // for basic_string_view
#include <type_traits>    // for is_same_v
#include <unordered_map>  // for unordered_map
#include <utility>        // for move, make_pair
#include <vector>         // for vector

#include "libsemigroups/constants.hpp"        // for POSITIVE_INFINITY
#include "libsemigroups/debug.hpp"            // for LIBSEMIGROUPS_...
#include "libsemigroups/obvinf.hpp"           // for is_obviously_infinite
#include "libsemigroups/order.hpp"            // for ShortLexCompare
#include "libsemigroups/presentation.hpp"     // for operator!=
#include "libsemigroups/ranges.hpp"           // for count, iterato...
#include "libsemigroups/to-presentation.hpp"  // for to_presentation
#include "libsemigroups/types.hpp"            // for congruence_kind
#include "libsemigroups/word-graph.hpp"       // for WordGraph, to_...
#include "libsemigroups/word-range.hpp"       // for to_human_reada...

#include "cong-common-class.hpp"  // for CongruenceInte...
#include "fmt.hpp"                // for format, print
#include "report.hpp"             // for report_no_prefix
#include "rewriters.hpp"          // for Rule, internal...
#include "string.hpp"             // for group_digits
#include "timer.hpp"              // for string_time

namespace libsemigroups {
  // Forward declarations
  namespace detail {
    template <typename KnuthBendix_>
    class KBE;
    //! \defgroup knuth_bendix_class_mem_types_group Member types
    //! \ingroup knuth_bendix_class_group
    //!
    //! \brief Public member types
    //!
    //! This page contains the documentation of the public member types of a
    //! \ref_knuth_bendix instance.

    //! \defgroup knuth_bendix_class_init_group Constructors + initializers
    //! \ingroup knuth_bendix_class_group
    //!
    //! \brief Construct or re-initialize a \ref_knuth_bendix
    //! instance (public member function).
    //!
    //! This page documents the constructors and initialisers for the
    //! \ref_knuth_bendix class.
    //!
    //! Every constructor (except the move + copy constructors, and the move
    //! and copy assignment operators) has a matching `init` function with the
    //! same signature that can be used to re-initialize a \ref_knuth_bendix
    //! instance as if it had just been constructed; but without necessarily
    //! releasing any previous allocated memory.

    //! \defgroup knuth_bendix_class_settings_group Settings
    //! \ingroup knuth_bendix_class_group
    //!
    //! \brief Settings that control the behaviour of a \ref_knuth_bendix
    //! instance.
    //!
    //! This page contains information about the member functions of the
    //! \ref_knuth_bendix that control various settings that influence the
    //! running of the Knuth-Bendix algorithm.
    //!
    //! There are a fairly large number of settings, they can profoundly alter
    //! the run time, but it is hard to predict what settings will work best for
    //! any particular input.
    //!
    //! See also \ref Runner for further settings.

    //! \defgroup knuth_bendix_class_intf_group Common member functions
    //! \ingroup knuth_bendix_class_group
    //!
    //! \brief Documentation of common member functions of \ref Congruence,
    //! \ref Kambites, \ref_knuth_bendix, and \ref_todd_coxeter.
    //!
    //! This page contains documentation of the member functions of
    //! \ref_knuth_bendix that are implemented in all of the classes Congruence,
    //! Kambites, \ref_knuth_bendix, and \ref_todd_coxeter.

    //! \defgroup knuth_bendix_class_accessors_group Accessors
    //!
    //! \ingroup knuth_bendix_class_group
    //!
    //! \brief Member functions that can be used to access the state of a
    //! \ref_knuth_bendix instance.
    //!
    //! This page contains the documentation of the various member functions of
    //! the \ref_knuth_bendix class that can be used to access the state of an
    //! instance.
    //!
    //! Those functions with the prefix `current_` do not perform any
    //! further enumeration.

    template <typename Rewriter       = detail::RewriteTrie,
              typename ReductionOrder = ShortLexCompare>
    class KnuthBendixBase : public CongruenceCommon {
      // defined in detail/kbe.hpp
      friend class ::libsemigroups::detail::KBE<KnuthBendixBase>;

      ////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - nested subclasses - private
      ////////////////////////////////////////////////////////////////////////

      // Overlap measures
      struct OverlapMeasure {
        virtual size_t
        operator()(detail::Rule const*,
                   detail::Rule const* examples,
                   detail::internal_string_type::const_iterator const&)
            = 0;
        virtual ~OverlapMeasure() {}
      };

      struct ABC;
      struct AB_BC;
      struct MAX_AB_BC;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Interface requirements - native-types
      ////////////////////////////////////////////////////////////////////////

      //! Type of the rules in the system.
      // TODO(0) import into KnuthBendix<>
      using rule_type = std::pair<std::string, std::string>;

      using native_word_type = std::string;

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - types - public
      //////////////////////////////////////////////////////////////////////////

      struct options {
        enum class overlap { ABC = 0, AB_BC = 1, MAX_AB_BC = 2 };
      };

     private:
      struct Settings {
        Settings() noexcept;
        Settings& init() noexcept;

        Settings(Settings const&) noexcept            = default;
        Settings(Settings&&) noexcept                 = default;
        Settings& operator=(Settings const&) noexcept = default;
        Settings& operator=(Settings&&) noexcept      = default;

        size_t                    max_pending_rules;
        size_t                    check_confluence_interval;
        size_t                    max_overlap;
        size_t                    max_rules;
        typename options::overlap overlap_policy;
      };

      struct Stats {
        Stats() noexcept;
        Stats& init() noexcept;

        Stats(Stats const&) noexcept            = default;
        Stats(Stats&&) noexcept                 = default;
        Stats& operator=(Stats const&) noexcept = default;
        Stats& operator=(Stats&&) noexcept      = default;

        size_t prev_active_rules;
        size_t prev_inactive_rules;
        size_t prev_total_rules;
      };

      ////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - data - private
      ////////////////////////////////////////////////////////////////////////

      bool                            _gen_pairs_initted;
      std::vector<std::string>        _input_generating_pairs;
      WordGraph<uint32_t>             _gilman_graph;
      std::vector<std::string>        _gilman_graph_node_labels;
      bool                            _internal_is_same_as_external;
      std::unique_ptr<OverlapMeasure> _overlap_measure;
      Presentation<std::string>       _presentation;
      Rewriter                        _rewriter;
      Settings                        _settings;
      mutable Stats                   _stats;
      mutable std::string             _tmp_element1;

     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - constructors and destructor - public
      //////////////////////////////////////////////////////////////////////////

      KnuthBendixBase();
      KnuthBendixBase& init();
      KnuthBendixBase(KnuthBendixBase const& that);

      KnuthBendixBase(KnuthBendixBase&&);

      KnuthBendixBase& operator=(KnuthBendixBase const&);

      KnuthBendixBase& operator=(KnuthBendixBase&&);

      ~KnuthBendixBase();

      KnuthBendixBase(congruence_kind knd, Presentation<std::string> const& p);

      KnuthBendixBase& init(congruence_kind                  knd,
                            Presentation<std::string> const& p);

      KnuthBendixBase(congruence_kind knd, Presentation<std::string>&& p);

      KnuthBendixBase& init(congruence_kind knd, Presentation<std::string>&& p);

      // TODO(1) construct/init from kind and KnuthBendixBase const&, for
      // consistency with ToddCoxeterBase

     private:
      void init_from_generating_pairs();
      void init_from_internal_presentation();

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - interface requirements - add_generating_pair
      //////////////////////////////////////////////////////////////////////////

     public:
      // NOTE THAT this is not the same as in ToddCoxeterBase, because the
      // generating pairs contained in CongruenceCommon are word_types, and
      // so we don't require any conversion here (since chars can be converted
      // implicitly to letter_types)
      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      KnuthBendixBase& add_generating_pair_no_checks(Iterator1 first1,
                                                     Iterator2 last1,
                                                     Iterator3 first2,
                                                     Iterator4 last2) {
        LIBSEMIGROUPS_ASSERT(!started());
        _input_generating_pairs.emplace_back(first1, last1);
        _input_generating_pairs.emplace_back(first2, last2);
        return CongruenceCommon::add_internal_generating_pair_no_checks<
            KnuthBendixBase>(first1, last1, first2, last2);
      }

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      KnuthBendixBase& add_generating_pair(Iterator1 first1,
                                           Iterator2 last1,
                                           Iterator3 first2,
                                           Iterator4 last2) {
        return CongruenceCommon::add_generating_pair<KnuthBendixBase>(
            first1, last1, first2, last2);
      }

      ////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - interface requirements - number_of_classes
      ////////////////////////////////////////////////////////////////////////

      //! \ingroup knuth_bendix_class_intf_group
      //! \brief Compute the number of classes in the congruence.
      //!
      //! This function computes the number of classes in the congruence
      //! represented by a \ref_knuth_bendix instance by
      //! running the congruence enumeration until it terminates.
      //!
      //! \returns The number of congruences classes of a \ref_knuth_bendix
      //! instance.
      //!
      //! \cong_intf_warn_undecidable{Knuth-Bendix}
      //!
      //! \note If \c this has been run until finished, then this function can
      //! determine the number of classes of the congruence represented by \c
      //! this even if it is infinite. Moreover, the complexity of this function
      //! is at worst \f$O(mn)\f$ where \f$m\f$ is the number of letters in the
      //! alphabet, and \f$n\f$ is the number of nodes in the \ref gilman_graph.
      [[nodiscard]] uint64_t number_of_classes();

      ////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - interface requirements - contains
      ////////////////////////////////////////////////////////////////////////

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      [[nodiscard]] tril currently_contains_no_checks(Iterator1 first1,
                                                      Iterator2 last1,
                                                      Iterator3 first2,
                                                      Iterator4 last2) const;

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      [[nodiscard]] tril currently_contains(Iterator1 first1,
                                            Iterator2 last1,
                                            Iterator3 first2,
                                            Iterator4 last2) const {
        return CongruenceCommon::currently_contains<KnuthBendixBase>(
            first1, last1, first2, last2);
      }

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      [[nodiscard]] bool contains_no_checks(Iterator1 first1,
                                            Iterator2 last1,
                                            Iterator3 first2,
                                            Iterator4 last2) {
        return CongruenceCommon::contains_no_checks<KnuthBendixBase>(
            first1, last1, first2, last2);
      }

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      [[nodiscard]] bool contains(Iterator1 first1,
                                  Iterator2 last1,
                                  Iterator3 first2,
                                  Iterator4 last2) {
        return CongruenceCommon::contains<KnuthBendixBase>(
            first1, last1, first2, last2);
      }

      ////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - interface requirements - reduce
      ////////////////////////////////////////////////////////////////////////

      template <typename OutputIterator,
                typename InputIterator1,
                typename InputIterator2>
      OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                             InputIterator1 first,
                                             InputIterator2 last) const;

      template <typename OutputIterator,
                typename InputIterator1,
                typename InputIterator2>
      OutputIterator reduce_no_run(OutputIterator d_first,
                                   InputIterator1 first,
                                   InputIterator2 last) const {
        return CongruenceCommon::reduce_no_run<KnuthBendixBase>(
            d_first, first, last);
      }

      template <typename OutputIterator,
                typename InputIterator1,
                typename InputIterator2>
      OutputIterator reduce_no_checks(OutputIterator d_first,
                                      InputIterator1 first,
                                      InputIterator2 last) {
        return CongruenceCommon::reduce_no_checks<KnuthBendixBase>(
            d_first, first, last);
      }

      template <typename OutputIterator,
                typename InputIterator1,
                typename InputIterator2>
      OutputIterator reduce(OutputIterator d_first,
                            InputIterator1 first,
                            InputIterator2 last) {
        return CongruenceCommon::reduce<KnuthBendixBase>(d_first, first, last);
      }

      // TODO(1) implement reduce_inplace x4 if possible.

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - setters for optional parameters - public
      //////////////////////////////////////////////////////////////////////////

      //! \ingroup knuth_bendix_class_settings_group
      //!
      //! \brief Set the number of rules to accumulate before they are
      //! processed.
      //!
      //! This function can be used to specify the number of pending rules that
      //! must accumulate before they are reduced, processed, and added to the
      //! system.
      //!
      //! The default value is \c 128, and should be set to \c 1 if \ref run
      //! should attempt to add each rule as they are created without waiting
      //! for rules to accumulate.
      //!
      //! \param val the new value of the batch size.
      //!
      //! \returns
      //! A reference to \c *this.
      //!
      //! \complexity
      //! Constant.
      KnuthBendixBase& max_pending_rules(size_t val) {
        _settings.max_pending_rules = val;
        return *this;
      }

      //! \ingroup knuth_bendix_class_settings_group
      //!
      //! \brief Get the current number of rules to accumulate before
      //! processing.
      //!
      //! This function can be used to return the number of pending rules that
      //! must accumulate before they are reduced, processed, and added to the
      //! system.
      //!
      //! The default value is \c 128.
      //!
      //! \returns
      //! The batch size, a value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      [[nodiscard]] size_t max_pending_rules() const noexcept {
        return _settings.max_pending_rules;
      }

      //! \ingroup knuth_bendix_class_settings_group
      //!
      //! \brief Set the interval at which confluence is checked.
      //!
      //! The function \ref run periodically checks if the system is already
      //! confluent. This function can be used to set how frequently this
      //! happens, it is the number of new overlaps that should be considered
      //! before checking confluence. Setting this value too low can adversely
      //! affect the performance of \ref run.
      //!
      //! The default value is \c 4096, and should be set to \ref LIMIT_MAX if
      //! \ref run should never check if the system is already confluent.
      //!
      //! \param val the new value of the interval.
      //!
      //! \returns
      //! A reference to \c *this.
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa \ref run.
      KnuthBendixBase& check_confluence_interval(size_t val) {
        _settings.check_confluence_interval = val;
        return *this;
      }

      //! \ingroup knuth_bendix_class_settings_group
      //!
      //! \brief Get the current interval at which confluence is checked.
      //!
      //! The function \ref run periodically checks if
      //! the system is already confluent. This function can be used to
      //! return how frequently this happens, it is the number of new overlaps
      //! that should be considered before checking confluence.
      //!
      //! \returns
      //! The interval at which confluence is checked a value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa \ref run.
      [[nodiscard]] size_t check_confluence_interval() const noexcept {
        return _settings.check_confluence_interval;
      }

      //! \ingroup knuth_bendix_class_settings_roup
      //!
      //! \brief Set the maximum length of overlaps to be considered.
      //!
      //! This function can be used to specify the maximum length of the
      //! overlap of two left hand sides of rules that should be considered in
      //! \ref run.
      //!
      //! If this value is less than the longest left hand side of a rule, then
      //! \ref run can terminate without the system being
      //! confluent.
      //!
      //! \param val the new value of the maximum overlap length.
      //!
      //! \returns
      //! A reference to \c *this.
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa \ref run.
      KnuthBendixBase& max_overlap(size_t val) {
        _settings.max_overlap = val;
        return *this;
      }

      //! \ingroup knuth_bendix_class_settings_group
      //!
      //! \brief Get the current maximum length of overlaps to be considered.
      //!
      //! This function returns the maximum length of the overlap of two left
      //! hand sides of rules that should be considered in \ref run.
      //!
      //! \returns
      //! The maximum length of the overlaps to be considered a value of type
      //! \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa \ref run.
      [[nodiscard]] size_t max_overlap() const noexcept {
        return _settings.max_overlap;
      }

      //! \ingroup knuth_bendix_class_settings_group
      //!
      //! \brief Set the maximum number of rules.
      //!
      //! This function sets the (approximate) maximum number of rules
      //! that the system should contain. If this is number is exceeded in calls
      //! to \ref run or knuth_bendix::by_overlap_length, then they will
      //! terminate and the system may not be confluent.
      //!
      //! By default this value is \ref POSITIVE_INFINITY.
      //!
      //! \param val the maximum number of rules.
      //!
      //! \returns
      //! A reference to \c *this.
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa \ref run.
      KnuthBendixBase& max_rules(size_t val) {
        _settings.max_rules = val;
        return *this;
      }

      //! \ingroup knuth_bendix_class_settings_group
      //!
      //! \brief Get the current maximum number of rules.
      //!
      //! This function returns the (approximate) maximum number of rules
      //! that the system should contain. If this is number is exceeded in
      //! calls to \ref run or \ref knuth_bendix::by_overlap_length, then they
      //! will terminate and the system may not be confluent.
      //!
      //! \returns
      //! The maximum number of rules the system should contain, a value of type
      //! \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa \ref run.
      [[nodiscard]] size_t max_rules() const noexcept {
        return _settings.max_rules;
      }

      //! \ingroup knuth_bendix_class_settings_group
      //!
      //! \brief Set the overlap policy.
      //!
      //! This function can be used to determine the way that the length
      //! of an overlap of two words in the system is measured.
      //!
      //! \param val the overlap policy.
      //!
      //! \returns
      //! A reference to \c *this.
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa \ref KnuthBendix::options::overlap.
      KnuthBendixBase& overlap_policy(typename options::overlap val);

      //! \ingroup knuth_bendix_class_settings_group
      //!
      //! \brief Get the current overlap policy.
      //!
      //! This function returns the way that the length of an overlap of two
      //! words in the system is measured.
      //!
      //! \returns
      //! The overlap policy.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa \ref KnuthBendix::options::overlap.
      [[nodiscard]] typename options::overlap overlap_policy() const noexcept {
        return _settings.overlap_policy;
      }

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - member functions for rules and rewriting - public
      //////////////////////////////////////////////////////////////////////////

      template <typename Iterator1, typename Iterator2>
      void throw_if_letter_out_of_bounds(Iterator1 first,
                                         Iterator2 last) const {
        internal_presentation().validate_word(first, last);
      }

      [[nodiscard]] Presentation<std::string> const&
      internal_presentation() const noexcept {
        return _presentation;
      }

      // TODO(0) remove this
      [[nodiscard]] std::vector<native_word_type> const&
      generating_pairs() const noexcept {
        // TODO(0) remove _input_generating_pairs data member completely
        return _input_generating_pairs;
      }

      //! \ingroup knuth_bendix_class_accessors_group
      //! \brief Return the current number of active rules in the
      //! \ref_knuth_bendix instance.
      //!
      //! This function returns the current number of active rules in the
      //! \ref_knuth_bendix instance.
      //!
      //! \returns
      //! The current number of active rules, a value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      // TODO(1) this should be const
      // TODO(1) add note about empty active rules after init and non-const-ness
      // (this only applies if this becomes const)
      [[nodiscard]] size_t number_of_active_rules() noexcept;

      //! \ingroup knuth_bendix_class_accessors_group
      //!
      //! \brief Return the current number of inactive rules in the
      //! \ref_knuth_bendix instance.
      //!
      //! This function returns the current number of inactive rules in the
      //! \ref_knuth_bendix instance.
      //!
      //! \returns
      //! The current number of inactive rules, a value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      [[nodiscard]] size_t number_of_inactive_rules() const noexcept {
        return _rewriter.number_of_inactive_rules();
      }

      //! \ingroup knuth_bendix_class_accessors_group
      //!
      //! \brief Return the number of rules that \ref_knuth_bendix has created
      //!
      //! This function returns the total number of Rule instances that have
      //! been created whilst whilst the Knuth-Bendix algorithm has been
      //! running. Note that this is not the sum of \ref number_of_active_rules
      //! and \ref number_of_inactive_rules, due to the re-initialisation of
      //! rules where possible.
      //!
      //! \returns
      //! The total number of rules, a value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      [[nodiscard]] size_t total_rules() const noexcept {
        return _rewriter.stats().total_rules;
      }

      // Documented in KnuthBendix
      // TODO(1) should be const
      // TODO(1) add note about empty active rules after, or better discuss that
      // there are three kinds of rules in the system: active, inactive, and
      // pending.
      [[nodiscard]] auto active_rules();

     private:
      // TODO(1) remove this ...
      void rewrite_inplace(std::string& w);

      // TODO(1) remove this ...
      [[nodiscard]] std::string rewrite(std::string w) {
        rewrite_inplace(w);
        return w;
      }

     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - main member functions - public
      //////////////////////////////////////////////////////////////////////////

      //! \ingroup knuth_bendix_class_accessors_group
      //!
      //! \brief Check confluence of the current rules.
      //!
      //! Check confluence of the current rules.
      //!
      //! \returns \c true if the \ref_knuth_bendix instance is
      //! [confluent](https://w.wiki/9DA) and \c false if it is not.
      [[nodiscard]] bool confluent() const;

      //! \ingroup knuth_bendix_class_accessors_group
      //!
      //! \brief Check if the current system knows the state of confluence of
      //! the current rules.
      //!
      //! Check if the current system knows the state of confluence of the
      //! current rules.
      //!
      //! \returns \c true if the confluence of the rules in the
      //! \ref_knuth_bendix instance is known, and \c false if it is not.
      [[nodiscard]] bool confluent_known() const noexcept;

      //! \ingroup knuth_bendix_class_accessors_group
      //!
      //! \brief Return the Gilman \ref WordGraph.
      //!
      //! This function returns the Gilman WordGraph of the system.
      //!
      //! The Gilman WordGraph is a digraph where the labels of the paths from
      //! the initial node (corresponding to the empty word) correspond to the
      //! shortlex normal forms of the semigroup elements.
      //!
      //! The semigroup is finite if the graph is cyclic, and infinite
      //! otherwise.
      //!
      //! \returns A const reference to a \ref WordGraph.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \warning This will terminate when the \ref_knuth_bendix instance is
      //! reduced and confluent, which might be never.
      //!
      //! \sa \ref number_of_classes, and \ref knuth_bendix::normal_forms.
      WordGraph<uint32_t> const& gilman_graph();

      // Documented in KnuthBendix
      [[nodiscard]] std::vector<std::string> const& gilman_graph_node_labels() {
        gilman_graph();  // to ensure that gilman_graph is initialised
        return _gilman_graph_node_labels;
      }

     private:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixBase - private member functions
      //////////////////////////////////////////////////////////////////////////

      void report_presentation(Presentation<std::string> const&) const;
      void report_before_run();
      void report_progress_from_thread(std::atomic_bool const&);
      void report_after_run();

      void stats_check_point();

      [[nodiscard]] static detail::internal_char_type
      uint_to_internal_char(size_t a);
      [[nodiscard]] static size_t
      internal_char_to_uint(detail::internal_char_type c);

      [[nodiscard]] static detail::internal_string_type
      uint_to_internal_string(size_t i);

      [[nodiscard]] static word_type
      internal_string_to_word(detail::internal_string_type const& s);

      [[nodiscard]] detail::internal_char_type
      external_to_internal_char(detail::external_char_type c) const;
      [[nodiscard]] detail::external_char_type
      internal_to_external_char(detail::internal_char_type a) const;

      void external_to_internal_string(detail::external_string_type& w) const;
      void internal_to_external_string(detail::internal_string_type& w) const;

      void add_octo(detail::external_string_type& w) const;
      void rm_octo(detail::external_string_type& w) const;

      void add_rule_impl(std::string const& p, std::string const& q);

      void overlap(detail::Rule const* u, detail::Rule const* v);

      [[nodiscard]] size_t max_active_word_length() const {
        return _rewriter.max_active_word_length();
      }

      void               run_real(std::atomic_bool&);
      [[nodiscard]] bool stop_running() const;

      //////////////////////////////////////////////////////////////////////////
      // Runner - pure virtual member functions - private
      //////////////////////////////////////////////////////////////////////////

      void run_impl() override;
      bool finished_impl() const override;
    };  // class KnuthBendixBase
  }     // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // global functions - to_human_readable_repr
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup knuth_bendix_group
  //!
  //! \brief Insert into std::ostream.
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! This function allows a \ref_knuth_bendix object to be left shifted into a
  //! std::ostream, such as std::cout. The currently active rules of the
  //! system are represented in the output.
  //!
  //! \param os the output stream to insert into.
  //! \param kb the \ref_knuth_bendix object.
  //!
  //! \returns A reference to the first argument.
#if PARSED_BY_DOXYGEN
  template <typename Word, typename Rewriter, typename ReductionOrder>
  std::ostream&
  operator<<(std::ostream&                                      os,
             KnuthBendix<Word, Rewriter, ReductionOrder> const& kb);
#else
  template <typename Rewriter, typename ReductionOrder>
  std::ostream&
  operator<<(std::ostream&                                            os,
             detail::KnuthBendixBase<Rewriter, ReductionOrder> const& kb);
#endif

  //! \ingroup knuth_bendix_group
  //!
  //! \brief Return a string representation of a \ref_knuth_bendix instance
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! This function returns a string representation of a \ref_knuth_bendix
  //! instance, specifying the size of the underlying alphabet and the number
  //! of active rules.
  //!
  //! \tparam Rewriter the first template parameter for \ref_knuth_bendix.
  //! \tparam ReductionOrder the second template parameter for
  //! \ref_knuth_bendix.
  //!
  //! \param kb the \ref_knuth_bendix instance.
  //!
  //! \returns The representation, a value of type \c std::string.
  // TODO(1) preferably kb would be a const&
#if PARSED_BY_DOXYGEN
  template <typename Word, typename Rewriter, typename ReductionOrder>
  std::string
  to_human_readable_repr(KnuthBendix<Word, Rewriter, ReductionOrder>& kb);
#else
  template <typename Rewriter, typename ReductionOrder>
  std::string
  to_human_readable_repr(detail::KnuthBendixBase<Rewriter, ReductionOrder>& kb);
#endif

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a \ref_knuth_bendix object.
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! This function constructs and returns a Presentation object using the
  //! currently active rules of \p kb.
  //!
  //! No enumeration of the argument \p kb is performed, so it might be the
  //! case that the resulting presentation does not define the same
  //! semigroup/monoid as \p kb. To ensure that the resulting presentation
  //! defines the same semigroup as \p kb, run \ref KnuthBendix::run (or any
  //! other function that fully enumerates \p kb) prior to calling this
  //! function.
  //!
  //! \tparam Word the type of the rules in the presentation being
  //! constructed.
  //!
  //! \param kb the \ref_knuth_bendix object from which to obtain the rules.
  //!
  //! \returns An object of type \c Presentation<Word>.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  // This cannot go into to-presentation.hpp since we include that here
#if PARSED_BY_DOXYGEN
  template <typename Word, typename Rewriter, typename ReductionOrder>
  Presentation<Word>
  to_presentation(KnuthBendix<Word, Rewriter, ReductionOrder>& kb);
#else
  template <typename Word, typename Rewriter, typename ReductionOrder>
  Presentation<Word>
  to_presentation(detail::KnuthBendixBase<Rewriter, ReductionOrder>& kb);
#endif

}  // namespace libsemigroups

#include "knuth-bendix-base.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_KNUTH_BENDIX_BASE_HPP_
