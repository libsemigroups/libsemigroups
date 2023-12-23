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

// This file contains a declaration of a class for performing the "low-index
// congruence" algorithm for 1-sided congruences of semigroups and monoids.

// TODO(Sims1):
// * implement joins (HopcroftKarp), meets (not sure), containment (find join
//   and check equality)?
// * generating pairs for congruences defined by "word_graph"?
// * is 2-sided congruence method. One approach would be to compute the kernel
//   of the associated homomorphism, which is the largest 2-sided congruence
//   contained in the right congruence. Not sure if this is a good approach.
// * a version which allows specifying the word_graph to Sims1 too
//
// Notes:
// 1. In 2022, when first writing this file, JDM tried templating the word_type
//    used by the presentations in Sims1 (so that we could use StaticVector1
//    for example, using smaller integer types for letters, and the stack to
//    hold the words rather than the heap), but this didn't seem to give any
//    performance improvement, and so I backed out the changes.

#ifndef LIBSEMIGROUPS_SIMS1_HPP_
#define LIBSEMIGROUPS_SIMS1_HPP_

#include <algorithm>   // for max
#include <chrono>      // for high_resolution_clock
#include <cstddef>     // for size_t
#include <cstdint>     // for uint64_t, uint32_t
#include <functional>  // for function
#include <iterator>    // for forward_iterator_tag
#include <mutex>       // for mutex
#include <string>      // for operator+, basic_string
#include <thread>      // for thread, yield
#include <utility>     // for move
#include <vector>      // for vector

#include "constants.hpp"        // for UNDEFINED
#include "debug.hpp"            // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"        // for LIBSEMIGROUPS_EXCEPTION
#include "felsch-graph.hpp"     // for FelschGraph
#include "presentation.hpp"     // for Presentation, Presentati...
#include "to-presentation.hpp"  // for make
#include "transf.hpp"           // for Transf
#include "types.hpp"            // for word_type, congruence_kind
#include "word-graph.hpp"       // for WordGraph

#include "detail/report.hpp"  // for REPORT_DEFAULT, Reporter
#include "detail/stl.hpp"     // for JoinThreads
#include "detail/timer.hpp"   // for Timer

namespace libsemigroups {

  //! Defined in ``sims1.hpp``.
  //!
  //! On this page we describe the `SimsStats` class. The purpose of this
  //! class is to collect some statistics related to `Sims1` class template.
  //!
  //! \sa \ref Sims1
  class SimsStats {
   public:
    // TODO(doc)
    // Not atomic because this is only accessed by report_progress_from_thread
    uint64_t count_last;
    // TODO(doc)
    // Atomic so as to avoid races between report_progress_from_thread and the
    // threads modifying count_last
    std::atomic_uint64_t count_now;

    //! The maximum number of pending definitions.
    //!
    //! A *pending definition* is just an edge that will be defined at some
    //! point in the future in the WordGraph represented by a Sims1
    //! instance at any given moment.
    //!
    //! This member tracks the maximum number of such pending definitions that
    //! occur during the running of the algorithms in Sims1.
    std::atomic_uint64_t max_pending;

    //! The total number of pending definitions.
    //!
    //! A *pending definition* is just an edge that will be defined at some
    //! point in the future in the WordGraph represented by a Sims1
    //! instance at any given moment.
    //!
    //! This member tracks the total number of pending definitions that
    //! occur during the running of the algorithms in Sims1. This is the same
    //! as the number of nodes in the search tree encounter during the running
    //! of Sims1.
    // Not atomic because this is only accessed by report_progress_from_thread
    uint64_t total_pending_last;

    // TODO(doc)
    // Atomic so as to avoid races between report_progress_from_thread and the
    // threads modifying total_pending_now
    std::atomic_uint64_t total_pending_now;

    SimsStats();

    SimsStats(SimsStats const& that) : SimsStats() {
      init_from(that);
    }

    SimsStats& operator=(SimsStats const& that) {
      return init_from(that);
    }

    SimsStats(SimsStats&& that) : SimsStats() {
      init_from(that);
    }

    SimsStats& operator=(SimsStats&& that) {
      return init_from(that);
    }

    SimsStats& stats_zero();

    SimsStats& stats_check_point() {
      count_last         = count_now;
      total_pending_last = total_pending_now;
      return *this;
    }

   private:
    SimsStats& init_from(SimsStats const& that);
  };

  //! No doc
  // This class allows us to use the same interface for settings for Sims1,
  // RepOrc, and MinimalRepOrc without duplicating the code.
  template <typename Subclass>
  class SimsSettings {
   protected:
    // These are protected so that Sims1 can reverse them if necessary for
    // left congruences.
    std::vector<word_type>  _exclude;
    std::vector<word_type>  _include;
    Presentation<word_type> _presentation;

   private:
    size_t                                 _idle_thread_restarts;
    std::vector<word_type>::const_iterator _longs_begin;
    size_t                                 _num_threads;
    mutable SimsStats                      _stats;

   public:
    // TODO(doc)
    SimsSettings();

    // TODO(doc)
    // TODO(tests)
    Subclass& init();

    // Copy constructor is explicitly required, the constructor template is not
    // a substitute. If no copy constructor is implemented, then _longs_begin
    // is not properly initialised, and leads to badness.
    SimsSettings(SimsSettings const& that) {
      init(that);
    }

    SimsSettings& operator=(SimsSettings const& that) {
      init(that);
      return *this;
    }

    //! Construct from SimsSettings with different subclass.
    template <typename OtherSubclass>
    SimsSettings(SimsSettings<OtherSubclass> const& that) {
      init(that);
    }

    template <typename OtherSubclass>
    SimsSettings& init(SimsSettings<OtherSubclass> const& that);

    //! Returns the settings object of *this.
    //!
    //! The settings object contains all the settings that are common to
    //! `Sims1`, `RepOrc`, and `MinimalRepOrc`, which are currently:
    //! * \ref presentation
    //! * \ref long_rules
    //! * \ref number_of_threads
    //! * \ref extra
    //!
    //! The return value of this function can be used to
    //! initialise another `Sims1`, `RepOrc`, or
    //! `MinimalRepOrc` with these settings.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns A const reference to `SimsSettings`.
    //!
    //! \exceptions
    //! \noexcept
    // So that we can access the settings from the derived class T.
    [[nodiscard]] SimsSettings const& settings() const noexcept {
      return *this;
    }

    //! Copy the settings from \p that into `this`.
    //!
    //! The settings object contains all the settings that are common to
    //! `Sims1`, `RepOrc`, and `MinimalRepOrc`, which are currently:
    //! * \ref presentation
    //! * \ref long_rules
    //! * \ref number_of_threads
    //! * \ref extra
    //!
    //! The return value of this function can be used to initialise another
    //! `Sims1`, `RepOrc`, or `MinimalRepOrc` with these settings.
    //!
    //! \param that the object to copy the settings from.
    //!
    //! \returns A const reference to `this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Subclass& settings_copy_from(SimsSettings const& that) {
      *this = that;
      return static_cast<Subclass&>(*this);
    }

    //! \anchor number_of_threads
    //! Set the number of threads.
    //!
    //! This function sets the number of threads to be used
    //! by `Sims1`.
    //!
    //! The default value is `1`.
    //!
    //! \param val the maximum number of threads to use.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if the argument \p
    //! val is 0.
    //!
    //! \warning If \p val exceeds
    //! `std::thread::hardware_concurrency()`, then this is
    //! likely to have a negative impact on the performance
    //! of the algorithms implemented by `Sims1`.
    Subclass& number_of_threads(size_t val);

    //! Returns the current number of threads.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns
    //! A `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t number_of_threads() const noexcept {
      return _num_threads;
    }

    //! Set the short rules.
    //!
    //! These are the rules used at every node in the depth first search
    //! conducted by `Sims1`.
    //!
    //! If the template parameter \p P is not `Presentation<word_type>`, then
    //! the parameter \p p is first converted to a value of type
    //! `Presentation<word_type>` and it is this converted value that is used.
    //!
    //! \tparam P A specific value of the class template `Presentation`, must
    //! be derived from `PresentationBase`. \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if `to_presentation<word_type>(p)`
    //! throws
    //! \throws LibsemigroupsException if `p` is not valid
    //! \throws LibsemigroupsException if the alphabet of `p` is non-empty and
    //! not equal to that of \ref long_rules or \ref extra. \throws
    //! LibsemigroupsException if `p` has 0-generators and 0-relations.
    template <typename PresentationOfSomeKind>
    Subclass& presentation(PresentationOfSomeKind const& p);

    //! \anchor presentation
    //! Returns a const reference to the current short rules.
    //!
    //! This function returns the defining presentation of
    //! a `Sims1` instance. The congruences computed by
    //! \ref cbegin and \ref cend are defined over the
    //! semigroup or monoid defined by this presentation.
    //!
    //! Note that it might not be the case that the value
    //! returned by this function and the presentation used
    //! to construct the object are the same. A `Sims1`
    //! object requires the generators of the defining
    //! presentation \f$\mathcal{P}\f$ to be \f$\{0,
    //! \ldots, n - 1\}\f$ where \f$n\f$ is the size of the
    //! alphabet of \f$\mathcal{P}\f$. Every occurrence of
    //! every generator \c a in the presentation \c p used
    //! to construct a `Sims1` instance is replaced by
    //! `p.index(a)`.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns
    //! A const reference to `Presentation<word_type>`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning
    //! If \ref split_at or \ref long_rule_length have been
    //! called, then some of the defining relations may
    //! have been moved from \ref presentation to \ref
    //! long_rules.
    [[nodiscard]] Presentation<word_type> const& presentation() const noexcept {
      return _presentation;
    }

    //! Set the long rules.
    //!
    //! These are the rules used after a complete deterministic word graph
    //! compatible with \ref presentation has been found by `Sims1`. If such a
    //! word graph is compatible with the long rules specified by this
    //! function, then this word graph is accepted, and if not it is not
    //! accepted.
    //!
    //! If the template parameter \p P is not `Presentation<word_type>`, then
    //! the parameter \p p is first converted to a value of type
    //! `Presentation<word_type>` and it is this converted value that is used.
    //!
    //! \tparam P A specific value of the class template `Presentation`, must
    //! be derived from `PresentationBase`. \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if `to_presentation<word_type>(p)` throws
    //! \throws LibsemigroupsException if `p` is not valid
    //! \throws LibsemigroupsException if the alphabet of `p` is non-empty and
    //! not equal to that of \ref presentation or \ref extra.
    Subclass& cbegin_long_rules(std::vector<word_type>::const_iterator p);

    // TODO(doc)
    Subclass& cbegin_long_rules(size_t pos);

    // TODO(doc)
    Subclass& clear_long_rules() {
      return cbegin_long_rules(_presentation.rules.cend());
    }

    // TODO(doc)
    [[nodiscard]] size_t number_of_long_rules() const noexcept {
      return std::distance(_longs_begin, _presentation.rules.cend()) / 2;
    }

    //! \anchor long_rules
    //! Returns the current long rules.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns
    //! A const reference to `Presentation<word_type>`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<word_type>::const_iterator
    cbegin_long_rules() const noexcept {
      LIBSEMIGROUPS_ASSERT(_presentation.rules.cbegin() <= _longs_begin);
      LIBSEMIGROUPS_ASSERT(_longs_begin <= _presentation.rules.cend());
      return _longs_begin;
    }

    //! \anchor extra
    //! Returns a const reference to the additional defining pairs.
    //!
    //! The congruences computed by a Sims1 instance always contain the
    //! relations of this presentation. In other words, the congruences
    //! computed by this instance are only taken among those that contains the
    //! pairs of elements of the underlying semigroup (defined by the
    //! presentation returned by \ref presentation and \ref long_rules)
    //! represented by the relations of the presentation returned by
    //! `extra()`.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns
    //! A const reference to `Presentation<word_type>`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<word_type> const& include() const noexcept {
      return _include;
    }

    //! Set the extra rules.
    //!
    //! The congruences computed by a `Sims1` instance always contain the
    //! relations of this presentation. In other words, the congruences
    //! computed by this instance are only taken among those that contains the
    //! pairs of elements of the underlying semigroup (defined by the
    //! presentation returned by \ref presentation and \ref long_rules)
    //! represented by the relations of the presentation returned by
    //! `extra()`.
    //!
    //! If the template parameter \p P is not `Presentation<word_type>`, then
    //! the parameter \p p is first converted to a value of type
    //! `Presentation<word_type>` and it is this converted value that is used.
    //!
    //! \tparam P A specific value of the class template `Presentation`, must
    //! be derived from `PresentationBase`.
    //!
    //! \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if `to_presentation<word_type>(p)`
    //! throws
    //! \throws LibsemigroupsException if `p` is not valid \throws
    //! LibsemigroupsException if the alphabet of `p` is non-empty and not
    //! equal to that of \ref presentation or \ref long_rules.
    template <typename iterator>
    Subclass& include(iterator first, iterator last);

    // TODO(doc)
    Subclass& include(word_type const& lhs, word_type const& rhs);

    // TODO(doc)
    template <typename Container>
    Subclass& include(Container const& c) {
      include(std::begin(c), std::end(c));
      return static_cast<Subclass&>(*this);
    }

    // TODO(doc)
    Subclass& clear_include() {
      _include.clear();
      return static_cast<Subclass&>(*this);
    }

    // TODO(doc)
    template <typename iterator>
    Subclass& exclude(iterator first, iterator last);

    // TODO(doc)
    template <typename Container>
    Subclass& exclude(Container const& c) {
      exclude(std::begin(c), std::end(c));
      return static_cast<Subclass&>(*this);
    }

    // TODO(doc)
    Subclass& exclude(word_type const& lhs, word_type const& rhs);

    // TODO(doc)
    [[nodiscard]] std::vector<word_type> const& exclude() const noexcept {
      return _exclude;
    }

    // TODO(doc)
    Subclass& clear_exclude() {
      _exclude.clear();
      return static_cast<Subclass&>(*this);
    }

    // TODO(Sims1) ranges version of include/exclude?

    //! Returns a const reference to the current stats object.
    //!
    //! The value returned by this function is a `SimsStats` object which
    //! contains some statistics related to the current `Sims1` instance and
    //! any part of the depth first search already conducted.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns
    //! A const reference to `SimsStats`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] SimsStats& stats() const noexcept {
      return _stats;
    }

    //! \anchor long_rule_length
    //! Define the long rule length.
    //!
    //! This function modifies \ref presentation and \ref long_rules so that
    //! \ref presentation only contains those rules whose length (sum of the
    //! lengths of the two sides of the rules) is less than \p val (if any)
    //! and \ref long_rules only contains those rules of length at least \p
    //! val (if any). The rules contained in the union of \ref presentation
    //! and \ref long_rules is invariant under this function, but the
    //! distribution of the rules between \ref presentation and \ref
    //! long_rules is not.
    //!
    //! The relative orders of the rules within \ref presentation and \ref
    //! long_rules may not be preserved.
    //!
    //! \param val the value of the long rule length.
    //!
    //! \returns
    //! A const reference to `this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Subclass& long_rule_length(size_t val);

    // TODO to tpp
    Subclass& idle_thread_restarts(size_t val) {
      if (val == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "the argument (idle thread restarts) must be non-zero");
      }
      _idle_thread_restarts = val;
      return static_cast<Subclass&>(*this);
    }

    [[nodiscard]] size_t idle_thread_restarts() const noexcept {
      return _idle_thread_restarts;
    }

   protected:
    Subclass const& stats_copy_from(SimsStats const& stts) const {
      _stats = std::move(stts);
      return static_cast<Subclass const&>(*this);
    }

    void reverse(std::vector<word_type>& vec) {
      std::for_each(vec.begin(), vec.end(), [](word_type& w) {
        std::reverse(w.begin(), w.end());
      });
    }

   private:
    template <typename OtherSubclass>
    SimsSettings& init_from(SimsSettings<OtherSubclass> const& that);
  };

  class Sims1;
  class Sims2;

  namespace detail {
    template <typename Sims1or2>
    class SimsBase : public SimsSettings<Sims1or2>, public Reporter {
      static_assert(std::is_same_v<Sims1or2, Sims1>
                    || std::is_same_v<Sims1or2, Sims2>);

     public:
      // We use WordGraph, even though the iterators produced by this class
      // hold FelschGraph's, none of the features of FelschGraph are useful
      // for the output, only for the implementation
      //! The type of the associated WordGraph objects.
      using word_graph_type = WordGraph<uint32_t>;

      //! Type for the nodes in the associated WordGraph objects.
      using node_type = typename word_graph_type::node_type;

      using label_type = typename word_graph_type::label_type;

      //! The size_type of the associated WordGraph objects.
      using size_type = typename word_graph_type::size_type;

      //! Type for letters in the underlying presentation.
      using letter_type = typename word_type::value_type;

      SimsBase() {
        init();
      }

      SimsBase(SimsBase const& other)      = default;
      SimsBase(SimsBase&&)                 = default;
      SimsBase& operator=(SimsBase const&) = default;
      SimsBase& operator=(SimsBase&&)      = default;
      ~SimsBase()                          = default;

      Sims1or2& init() {
        if constexpr (std::is_same_v<Sims1or2, Sims1>) {
          report_prefix("Sims1");
        } else {
          report_prefix("Sims2");
        }
        SimsSettings<Sims1or2>::init();
        return static_cast<Sims1or2&>(*this);
      }

      using SimsSettings<Sims1or2>::presentation;
      using SimsSettings<Sims1or2>::number_of_threads;
      using SimsSettings<Sims1or2>::stats;
      using SimsSettings<Sims1or2>::include;
      using SimsSettings<Sims1or2>::exclude;
      using SimsSettings<Sims1or2>::cbegin_long_rules;

     protected:
      struct PendingDefBase {
        PendingDefBase() = default;

        PendingDefBase(node_type   s,
                       letter_type g,
                       node_type   t,
                       size_type   e,
                       size_type   n,
                       bool) noexcept
            : source(s), generator(g), target(t), num_edges(e), num_nodes(n) {}

        node_type   source;
        letter_type generator;
        node_type   target;
        size_type   num_edges;  // Number of edges in the graph when
                                // *this was added to the stack
        size_type num_nodes;    // Number of nodes in the graph
                                // after the definition is made
      };

      // This class collects some common aspects of the iterator and
      // thread_iterator nested classes. The mutex does nothing for <iterator>
      // and is an actual std::mutex for <thread_iterator>. Also subclassed by
      // Sims2::IteratorBase.
      class IteratorBase {
       public:
        using const_reference = word_graph_type const&;
        using const_pointer   = word_graph_type const*;
        using sims_type       = Sims1or2;

       private:
        size_type _max_num_classes;
        size_type _min_target_node;

       protected:
        using PendingDef = typename Sims1or2::PendingDef;
        // TODO(Sims1) ensure that _felsch_graph's settings are
        // properly initialised
        using Definition = std::pair<node_type, label_type>;

        FelschGraph<word_type, node_type, std::vector<Definition>>
            _felsch_graph;

        // This mutex does nothing for iterator, only does something for
        // thread_iterator
        std::mutex              _mtx;
        std::vector<PendingDef> _pending;
        Sims1or2 const*         _sims1;

        // Push initial PendingDef's into _pending, see tpp
        // file for explanation.
        // The following function is separated from the constructor so that it
        // isn't called in the constructor of every thread_iterator

        void init(size_type n) {
          if (n != 0) {
            if (n > 1 || _min_target_node == 1) {
              _pending.emplace_back(0, 0, 1, 0, 2, true);
            }
            if (_min_target_node == 0) {
              _pending.emplace_back(0, 0, 0, 0, 1, false);
            }
          }
        }

        // We could use the copy constructor, but there's no point in copying
        // anything except the FelschGraph and so we only copy that.
        void copy_felsch_graph(IteratorBase const& that) {
          _felsch_graph = that._felsch_graph;
        }

        // Try to pop from _pending into the argument (reference), returns true
        // if successful and false if not.
        [[nodiscard]] bool try_pop(PendingDef& pd) {
          std::lock_guard<std::mutex> lock(_mtx);
          if (_pending.empty()) {
            return false;
          }
          pd = std::move(_pending.back());
          _pending.pop_back();
          return true;
        }

        // Try to make the definition represented by PendingDef, returns false
        // if it wasn't possible, and true if it was.
        //! No doc
        bool try_define(PendingDef const& current) {
          LIBSEMIGROUPS_ASSERT(current.target < current.num_nodes);
          LIBSEMIGROUPS_ASSERT(current.num_nodes <= _max_num_classes);
          std::lock_guard<std::mutex> lock(_mtx);
          // Backtrack if necessary
          _felsch_graph.reduce_number_of_edges_to(current.num_edges);

          // It might be that current.target is a new node, in which case
          // _felsch_graph.number_of_active_nodes() includes this new node even
          // before the edge current.source -> current.target is defined.
          _felsch_graph.number_of_active_nodes(current.num_nodes);

          LIBSEMIGROUPS_ASSERT(
              _felsch_graph.target_no_checks(current.source, current.generator)
              == UNDEFINED);

          // Don't call number_of_edges because this calls the function in
          // WordGraph
          size_type start = _felsch_graph.definitions().size();

          _felsch_graph.set_target_no_checks(
              current.source, current.generator, current.target);

          auto first = _sims1->include().cbegin();
          auto last  = _sims1->include().cend();
          if (!felsch_graph::make_compatible<RegisterDefs>(
                  _felsch_graph, 0, 1, first, last)
              || !_felsch_graph.process_definitions(start)) {
            // Seems to be important to check include() first then
            // process_definitions
            return false;
          }

          first          = _sims1->exclude().cbegin();
          last           = _sims1->exclude().cend();
          node_type root = 0;

          for (auto it = first; it != last; it += 2) {
            auto l
                = word_graph::follow_path_no_checks(_felsch_graph, root, *it);
            if (l != UNDEFINED) {
              auto r = word_graph::follow_path_no_checks(
                  _felsch_graph, root, *(it + 1));
              if (l == r) {
                return false;
              }
            }
          }
          return true;
        }

        bool install_descendents(PendingDef const& current) {
          letter_type     a        = current.generator + 1;
          size_type const M        = _felsch_graph.number_of_active_nodes();
          size_type const N        = _felsch_graph.number_of_edges();
          size_type const num_gens = _felsch_graph.out_degree();
          auto&           stats    = _sims1->stats();

          for (node_type next = current.source; next < M; ++next) {
            for (; a < num_gens; ++a) {
              if (_felsch_graph.target_no_checks(next, a) == UNDEFINED) {
                std::lock_guard<std::mutex> lock(_mtx);
                if (M < _max_num_classes) {
                  _pending.emplace_back(next, a, M, N, M + 1, true);
                }
                for (node_type b = M; b-- > _min_target_node;) {
                  _pending.emplace_back(next, a, b, N, M, false);
                }
                stats.total_pending_now
                    += M - _min_target_node + (M < _max_num_classes);

                // Mutex must be locked here so that we can call _pending.size()
                stats.max_pending
                    = std::max(static_cast<uint64_t>(_pending.size()),
                               stats.max_pending.load());
                return false;
              }
            }
            a = 0;
          }
          // No undefined edges, word graph is complete
          LIBSEMIGROUPS_ASSERT(N == M * num_gens);

          auto first = _sims1->cbegin_long_rules();
          auto last  = _sims1->presentation().rules.cend();

          bool result
              = word_graph::is_compatible(_felsch_graph,
                                          _felsch_graph.cbegin_nodes(),
                                          _felsch_graph.cbegin_nodes() + M,
                                          first,
                                          last);
          if (result) {
            // stats.count_now is atomic so this is ok
            ++stats.count_now;
          }
          return result;
        }

        //! No doc
        IteratorBase(Sims1or2 const* s,
                     size_type       n)
            :  // private
              _max_num_classes(s->presentation().contains_empty_word() ? n
                                                                       : n + 1),
              _min_target_node(s->presentation().contains_empty_word() ? 0 : 1),
              // protected
              _felsch_graph(),
              _mtx(),
              _pending(),
              _sims1(s) {
          Presentation<word_type> p = s->presentation();
          size_t m = std::distance(s->presentation().rules.cbegin(),
                                   s->cbegin_long_rules());
          p.rules.erase(p.rules.begin() + m, p.rules.end());
          _felsch_graph.init(std::move(p));
          // n == 0 only when the iterator is cend
          _felsch_graph.number_of_active_nodes(n == 0 ? 0 : 1);
          // = 0 indicates iterator is done
          _felsch_graph.add_nodes(n);
        }

       public:
        // None of the constructors are noexcept because the corresponding
        // constructors for Presentation aren't currently

        //! No doc
        IteratorBase() = default;

        //! No doc
        // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
        // sense if the mutex was used here.
        IteratorBase(IteratorBase const& that)
            :  // private
              _max_num_classes(that._max_num_classes),
              _min_target_node(that._min_target_node),
              // protected
              _felsch_graph(that._felsch_graph),
              _mtx(),
              _pending(that._pending),
              _sims1(that._sims1) {}

        // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
        // sense if the mutex was used here.
        IteratorBase(IteratorBase&& that)
            :  // private
              _max_num_classes(std::move(that._max_num_classes)),
              _min_target_node(std::move(that._min_target_node)),
              // protected
              _felsch_graph(std::move(that._felsch_graph)),
              _mtx(),
              _pending(std::move(that._pending)),
              _sims1(that._sims1) {}

        // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
        // sense if the mutex was used here.

        IteratorBase& operator=(IteratorBase const& that) {
          // private
          _max_num_classes = that._max_num_classes;
          _min_target_node = that._min_target_node;
          // protected
          _felsch_graph = that._felsch_graph;
          // keep our own _mtx
          _pending = that._pending;
          _sims1   = that._sims1;

          return *this;
        }

        // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
        // sense if the mutex was used here.

        IteratorBase& operator=(IteratorBase&& that) {
          // private
          _max_num_classes = std::move(that._max_num_classes);
          _min_target_node = std::move(that._min_target_node);

          // protected
          _felsch_graph = std::move(that._felsch_graph);
          _pending      = std::move(that._pending);
          // keep our own _mtx
          _sims1 = that._sims1;
          return *this;
        }

        //! No doc
        ~IteratorBase() = default;

        //! No doc
        [[nodiscard]] bool operator==(IteratorBase const& that) const noexcept {
          return _felsch_graph == that._felsch_graph;
        }

        //! No doc
        [[nodiscard]] bool operator!=(IteratorBase const& that) const noexcept {
          return !(operator==(that));
        }

        //! No doc
        [[nodiscard]] const_reference operator*() const noexcept {
          return _felsch_graph;
        }

        //! No doc
        [[nodiscard]] const_pointer operator->() const noexcept {
          return &_felsch_graph;
        }

        //! No doc
        void swap(IteratorBase& that) noexcept {
          // private
          std::swap(_max_num_classes, that._max_num_classes);
          std::swap(_min_target_node, that._min_target_node);
          // protected
          std::swap(_felsch_graph, that._felsch_graph);
          std::swap(_pending, that._pending);
          std::swap(_sims1, that._sims1);
        }

        //! No doc
        SimsStats& stats() noexcept {
          return _sims1->stats();
        }

        size_type maximum_number_of_classes() const noexcept {
          return _max_num_classes;
        }
      };  // class IteratorBase

     public:
      //! The return type of \ref cbegin and \ref cend.
      //!
      //! This is a forward iterator values of this type are expensive to copy
      //! due to their internal state, and prefix increment should be
      //! preferred to postfix.
      class iterator : public Sims1or2::iterator_base {
        using iterator_base = typename Sims1or2::iterator_base;
        using PendingDef    = typename Sims1or2::PendingDef;

        using iterator_base::init;
        using iterator_base::install_descendents;
        using iterator_base::try_define;
        using iterator_base::try_pop;

       public:
        //! No doc
        using const_pointer = typename iterator_base::const_pointer;
        //! No doc
        using const_reference = typename iterator_base::const_reference;

        //! No doc
        using size_type = typename std::vector<word_graph_type>::size_type;
        //! No doc
        using difference_type =
            typename std::vector<word_graph_type>::difference_type;
        //! No doc
        using pointer = typename std::vector<word_graph_type>::pointer;
        //! No doc
        using reference = typename std::vector<word_graph_type>::reference;
        //! No doc
        using value_type = word_graph_type;
        //! No doc
        using iterator_category = std::forward_iterator_tag;

        using sims_type = typename iterator_base::sims_type;

        //! No doc
        using iterator_base::iterator_base;

       private:
        // Only want Sims1 to be able to use this constructor.
        // TODO to tpp
        iterator(sims_type const* s, size_type n) : iterator_base(s, n) {
          if (this->_felsch_graph.number_of_active_nodes() == 0) {
            return;
          }
          init(n);
          ++(*this);
          // The increment above is required so that when dereferencing any
          // instance of this type we obtain a valid word graph (o/w the value
          // pointed to here is empty).
        }

        // So that we can use the constructor above.
        friend iterator SimsBase<sims_type>::cbegin(SimsBase::size_type) const;
        friend iterator SimsBase<sims_type>::cend(SimsBase::size_type) const;

       public:
        //! No doc
        ~iterator() = default;

        // prefix
        //! No doc
        // TODO to tpp
        iterator const& operator++() {
          PendingDef current;
          while (try_pop(current)) {
            if (try_define(current) && install_descendents(current)) {
              return *this;
            }
          }
          this->_felsch_graph.number_of_active_nodes(0);
          // indicates that the iterator is done
          this->_felsch_graph.induced_subgraph_no_checks(0, 0);
          return *this;
        }

        // postfix
        //! No doc
        iterator operator++(int) {
          iterator copy(*this);
          ++(*this);
          return copy;
        }

        using iterator_base::swap;
      };  // class iterator

     private:
      class thread_runner;

      // Note that this class is private, and not really an iterator in the
      // usual sense. It is designed solely to work with thread_runner.
      class thread_iterator : public Sims1or2::iterator_base {
        using PendingDef    = typename Sims1or2::PendingDef;
        using iterator_base = typename Sims1or2::iterator_base;

        friend class thread_runner;

        using iterator_base::copy_felsch_graph;

       public:
        using sims_type = typename iterator_base::sims_type;

        //! No doc
        thread_iterator(sims_type const* s, size_type n)
            : iterator_base(s, n) {}

        // None of the constructors are noexcept because the corresponding
        // constructors for std::vector aren't (until C++17).
        //! No doc
        thread_iterator() = delete;
        //! No doc
        thread_iterator(thread_iterator const&) = delete;
        //! No doc
        thread_iterator(thread_iterator&&) = delete;
        //! No doc
        thread_iterator& operator=(thread_iterator const&) = delete;
        //! No doc
        thread_iterator& operator=(thread_iterator&&) = delete;

        //! No doc
        ~thread_iterator() = default;

        using iterator_base::stats;

       public:
        void push(PendingDef pd) {
          this->_pending.push_back(std::move(pd));
        }

        void steal_from(thread_iterator& that) {
          // WARNING <that> must be locked before calling this function
          std::lock_guard<std::mutex> lock(this->_mtx);
          LIBSEMIGROUPS_ASSERT(this->_pending.empty());
          size_t const n = that._pending.size();
          if (n == 1) {
            return;
          }
          // Copy the FelschGraph from that into *this
          copy_felsch_graph(that);

          // Unzip that._pending into _pending and that._pending, this seems to
          // give better performance in the search than splitting that._pending
          // into [begin, begin + size / 2) and [begin + size / 2, end)
          size_t i = 0;
          for (; i < n - 2; i += 2) {
            this->_pending.push_back(std::move(that._pending[i]));
            that._pending[i / 2] = std::move(that._pending[i + 1]);
          }
          this->_pending.push_back(std::move(that._pending[i]));
          if (i == n - 2) {
            that._pending[i / 2] = std::move(that._pending[i + 1]);
          }

          that._pending.erase(that._pending.cbegin() + that._pending.size() / 2,
                              that._pending.cend());
        }

        bool try_steal(thread_iterator& q) {
          std::lock_guard<std::mutex> lock(this->_mtx);
          if (this->_pending.empty()) {
            return false;
          }
          // Copy the FelschGraph and half pending from *this into q
          q.steal_from(
              *this);  // Must call steal_from on q, so that q is locked
          return true;
        }
      };  // thread_iterator

      class thread_runner {
       private:
        using ThreadIt   = typename Sims1or2::thread_iterator;
        using PendingDef = typename Sims1or2::PendingDef;

        std::atomic_bool                       _done;
        std::vector<std::unique_ptr<ThreadIt>> _theives;
        std::vector<std::thread>               _threads;
        std::mutex                             _mtx;
        size_type                              _num_threads;
        word_graph_type                        _result;
        Sims1or2 const*                        _sims1;

        void worker_thread(unsigned                                    my_index,
                           std::function<bool(word_graph_type const&)> hook) {
          PendingDef pd;
          auto const restarts = _sims1->idle_thread_restarts();
          for (size_t i = 0; i < restarts; ++i) {
            while ((pop_from_local_queue(pd, my_index)
                    || pop_from_other_thread_queue(pd, my_index))
                   && !_done) {
              if (_theives[my_index]->try_define(pd)
                  && _theives[my_index]->install_descendents(pd)) {
                if (hook(**_theives[my_index])) {
                  // hook returns true to indicate that we should stop early
                  std::lock_guard<std::mutex> lock(_mtx);
                  if (!_done) {
                    _done   = true;
                    _result = **_theives[my_index];
                  }
                  return;
                }
              }
            }
            std::this_thread::yield();
            // It's possible to reach here before all of the work is done,
            // because by coincidence there's nothing in the local queue and
            // nothing in any other queue either, this sometimes leads to
            // threads shutting down earlier than desirable. On the other hand,
            // maybe this is a desirable.
          }
        }

        bool pop_from_local_queue(PendingDef& pd, unsigned my_index) {
          return _theives[my_index]->try_pop(pd);
        }

        bool pop_from_other_thread_queue(PendingDef& pd, unsigned my_index) {
          for (size_t i = 0; i < _theives.size() - 1; ++i) {
            unsigned const index = (my_index + i + 1) % _theives.size();
            // Could always do something different here, like find
            // the largest queue and steal from that? I tried this and it didn't
            // seem to be faster.
            if (_theives[index]->try_steal(*_theives[my_index])) {
              return pop_from_local_queue(pd, my_index);
            }
          }
          return false;
        }

       public:
        thread_runner(Sims1or2 const* s, size_type n, size_type num_threads)
            : _done(false),
              _theives(),
              _threads(),
              _mtx(),
              _num_threads(num_threads),
              _result(),
              _sims1(s) {
          for (size_t i = 0; i < _num_threads; ++i) {
            _theives.push_back(std::make_unique<ThreadIt>(s, n));
          }
          _theives.front()->init(n);
        }

        ~thread_runner() = default;

        word_graph_type const& word_graph() const {
          return _result;
        }

        void run(std::function<bool(word_graph_type const&)> hook) {
          try {
            detail::JoinThreads joiner(_threads);
            for (size_t i = 0; i < _num_threads; ++i) {
              _threads.push_back(std::thread(
                  &thread_runner::worker_thread, this, i, std::ref(hook)));
            }
          } catch (...) {
            _done = true;
            throw;
          }
        }
      };  // class thread_runner

      // TODO to tpp
      void report_at_start(size_t num_classes) const {
        std::string num_threads = "0";
        if (number_of_threads() != 1) {
          num_threads = fmt::format("{} / {}",
                                    number_of_threads(),
                                    std::thread::hardware_concurrency());
        }
        auto shortest_short
            = presentation::shortest_rule_length(presentation());
        auto longest_short = presentation::longest_rule_length(presentation());

        std::string pairs;
        if (!include().empty() && !exclude().empty()) {
          pairs = fmt::format(", including {} + excluding {} pairs",
                              include().size() / 2,
                              exclude().size() / 2);
        } else if (!include().empty()) {
          pairs = fmt::format(", including {} pairs", include().size() / 2);
        } else if (!exclude().empty()) {
          pairs = fmt::format(", excluding {} pairs", exclude().size() / 2);
        }

        report_no_prefix("{:+<80}\n", "");
        report_default("{}: STARTING with {} additional threads . . . \n",
                       report_prefix(),
                       num_threads);
        report_no_prefix("{:+<80}\n", "");
        report_default("{}: \u2264 {} classes{} for \u27E8A|R\u27E9 with:\n",
                       report_prefix(),
                       num_classes,
                       pairs);
        report_default("{}: |A| = {}, |R| = {}, "
                       "|u| + |v| \u2208 [{}, {}], \u2211(|u| + |v|) = {}\n",
                       report_prefix(),
                       presentation().alphabet().size(),
                       presentation().rules.size() / 2,
                       shortest_short,
                       longest_short,
                       presentation::length(presentation()));

        if (cbegin_long_rules() != presentation().rules.cend()) {
          auto first = presentation().rules.cbegin(),
               last  = cbegin_long_rules();

          report_default("{}: {} \"short\" relations with: ",
                         report_prefix(),
                         std::distance(first, last) / 2);
          report_no_prefix(
              "|u| + |v| \u2208 [{}, {}] and \u2211(|u| + |v|) = {}\n",
              presentation::shortest_rule_length(first, last),
              presentation::longest_rule_length(first, last),
              presentation::length(first, last));

          first = cbegin_long_rules(), last = presentation().rules.cend();
          report_default("{}: {} \"long\" relations with: ",
                         report_prefix(),
                         std::distance(first, last) / 2);
          report_no_prefix(
              "|u| + |v| \u2208 [{}, {}] and \u2211(|u| + |v|) = {}\n",
              presentation::shortest_rule_length(first, last),
              presentation::longest_rule_length(first, last),
              presentation::length(first, last));
        }
        Reporter::reset_start_time();
      }

      // TODO to tpp
      void report_progress_from_thread() const {
        using namespace detail;       // NOLINT(build/namespaces)
        using namespace std::chrono;  // NOLINT(build/namespaces)

        auto time_total_ns = delta(start_time());
        auto time_diff     = delta(last_report());

        // Stats
        auto count_now         = stats().count_now.load();
        auto count_diff        = count_now - stats().count_last;
        auto total_pending_now = stats().total_pending_now.load();
        auto total_pending_diff
            = total_pending_now - stats().total_pending_last;

        constexpr uint64_t billion = 1'000'000'000;
        uint64_t congs_per_sec = (billion * count_now) / time_total_ns.count();
        uint64_t nodes_per_sec
            = (billion * total_pending_now) / time_total_ns.count();

        nanoseconds time_per_cong_last_sec(0);
        if (count_diff != 0) {
          time_per_cong_last_sec = time_diff / count_diff;
        }

        nanoseconds time_per_node_last_sec(0);
        if (total_pending_diff != 0) {
          time_per_node_last_sec = time_diff / total_pending_diff;
        }

        nanoseconds time_per_cong(0);
        if (count_now != 0) {
          time_per_cong = time_total_ns / count_now;
        }

        nanoseconds time_per_node(0);
        if (total_pending_now != 0) {
          time_per_node = time_total_ns / total_pending_now;
        }

        ReportCell<3> rc;
        rc.min_width(0, 4).min_width(1, 7).min_width(2, 11);
        rc("{}: total        {} (cong.)   | {} (nodes) \n",
           report_prefix(),
           group_digits(count_now),
           group_digits(total_pending_now));

        rc("{}: diff         {} (cong.)   | {} (nodes)\n",
           report_prefix(),
           signed_group_digits(count_diff),
           signed_group_digits(total_pending_diff));

        rc("{}: mean         {} (cong./s) | {} (node/s)\n",
           report_prefix(),
           group_digits(congs_per_sec),
           group_digits(nodes_per_sec));

        rc("{}: time last s. {} (/cong.)  | {} (/node)\n",
           report_prefix(),
           string_time(time_per_cong_last_sec),
           string_time(time_per_node_last_sec));

        rc("{}: mean time    {} (/cong.)  | {} (/node)\n",
           report_prefix(),
           string_time(time_per_cong),
           string_time(time_per_node));

        rc("{}: time         {} (total)   |\n",
           report_prefix(),
           string_time(time_total_ns));

        reset_last_report();
        stats().stats_check_point();
      }

      // TODO to tpp
      void report_final() const {
        report_progress_from_thread();
        report_no_prefix("{:+<80}\n", "");
        report_default("{}: FINISHED!\n", report_prefix());
        report_no_prefix("{:+<80}\n", "");
      }

      // TODO to tpp
     private:
      void throw_if_not_ready(size_type n) const {
        if (n == 0) {
          LIBSEMIGROUPS_EXCEPTION("the argument (size_type) must be non-zero");
        } else if (presentation().rules.empty()
                   && presentation().alphabet().empty()) {
          LIBSEMIGROUPS_EXCEPTION("the presentation() must be defined before "
                                  "calling this function");
        }
      }

     public:
      // TODO to tpp
      [[nodiscard]] iterator cbegin(size_type n) const {
        throw_if_not_ready(n);
        return iterator(static_cast<Sims1or2 const*>(this), n);
      }

      // TODO to tpp
      [[nodiscard]] iterator cend(size_type n) const {
        throw_if_not_ready(n);
        return iterator(static_cast<Sims1or2 const*>(this), 0);
      }

     public:
      // Apply the function pred to every one-sided congruence with at
      // most n classes
      void for_each(size_type                                   n,
                    std::function<void(word_graph_type const&)> pred) const {
        throw_if_not_ready(n);

        report_at_start(n);
        if (number_of_threads() == 1) {
          if (!reporting_enabled()) {
            // Don't care about stats in this case
            std::for_each(cbegin(n), cend(n), pred);
          } else {
            stats().stats_zero();
            detail::Ticker t([this]() { report_progress_from_thread(); });
            auto           it   = cbegin(n);
            auto const     last = cend(n);
            for (; it != last; ++it) {
              pred(*it);
            }
            report_final();
          }
        } else {
          thread_runner den(
              static_cast<Sims1or2 const*>(this), n, number_of_threads());
          auto pred_wrapper = [&pred](word_graph_type const& ad) {
            pred(ad);
            return false;
          };
          if (!reporting_enabled()) {
            den.run(pred_wrapper);
          } else {
            stats().stats_zero();
            detail::Ticker t([this]() { report_progress_from_thread(); });
            den.run(pred_wrapper);
            report_final();
          }
        }
      }

      word_graph_type
      find_if(size_type                                   n,
              std::function<bool(word_graph_type const&)> pred) const {
        throw_if_not_ready(n);
        report_at_start(n);
        if (number_of_threads() == 1) {
          if (!reporting_enabled()) {
            return *std::find_if(cbegin(n), cend(n), pred);
          } else {
            stats().stats_zero();
            detail::Ticker t([this]() { report_progress_from_thread(); });

            auto       it   = cbegin(n);
            auto const last = cend(n);

            for (; it != last; ++it) {
              if (pred(*it)) {
                report_final();
                return *it;
              }
            }
            report_final();
            return *last;  // the empty digraph
          }
        } else {
          thread_runner den(
              static_cast<Sims1or2 const*>(this), n, number_of_threads());
          if (!reporting_enabled()) {
            den.run(pred);
            return den.word_graph();
          } else {
            stats().stats_zero();
            detail::Ticker t([this]() { report_progress_from_thread(); });
            den.run(pred);
            report_final();
            return den.word_graph();
          }
        }
      }

      // TODO to tpp
      uint64_t number_of_congruences(size_type n) const {
        if (number_of_threads() == 1) {
          uint64_t result = 0;
          for_each(n, [&result](word_graph_type const&) { ++result; });
          return result;
        } else {
          std::atomic_uint64_t result(0);
          for_each(n, [&result](word_graph_type const&) { ++result; });
          return result;
        }
      }
    };
  }  // namespace detail

  //! Defined in ``sims1.hpp``.
  //!
  //! On this page we describe the functionality relating to the small index
  //! congruence algorithm. The algorithm implemented by this class template
  //! is essentially the low index subgroup algorithm for finitely presented
  //! groups described in Section 5.6 of [Computation with Finitely Presented
  //! Groups](https://doi.org/10.1017/CBO9780511574702) by C. Sims. The low
  //! index subgroups algorithm was adapted for semigroups and monoids by J.
  //! D. Mitchell and M. Tsalakou.
  //!
  //! The purpose of this class is to provide the functions \ref cbegin, \ref
  //! cend, \ref for_each, and \ref find_if which permit iterating through the
  //! one-sided congruences of a semigroup or monoid defined by a presentation
  //! containing (a possibly empty) set of pairs and with at most a given
  //! number of classes. An iterator returned by \ref cbegin points at an
  //! WordGraph instance containing the action of the semigroup or monoid
  //! on the classes of a congruence.
  class Sims1 : public detail::SimsBase<Sims1> {
    using SimsBase = detail::SimsBase<Sims1>;

    friend SimsBase;

    using iterator_base = IteratorBase;
    using PendingDef    = PendingDefBase;

   public:
    //! Type for the nodes in the associated WordGraph
    //! objects.
    using node_type  = uint32_t;
    using label_type = typename WordGraph<node_type>::label_type;

    //! Type for letters in the underlying presentation.
    using letter_type = typename word_type::value_type;

    //! The size_type of the associated WordGraph objects.
    using size_type = typename WordGraph<node_type>::size_type;

    // We use WordGraph, even though the iterators produced by this class
    // hold FelschGraph's, none of the features of FelschGraph are useful
    // for the output, only for the implementation
    //! The type of the associated WordGraph objects.
    using word_graph_type = WordGraph<node_type>;

   private:
    congruence_kind _kind;

   public:
    //! Default constructor
    Sims1() = default;

    // TODO(doc)
    Sims1& init() {
      return SimsBase::init();
    }

    //! Construct from \ref congruence_kind.
    //!
    //! \param ck the handedness of the congruences (left or right)
    //!
    //! \throws LibsemigroupsException if \p ck is \ref
    //! congruence_kind::twosided
    //!
    //! \sa \ref cbegin and \ref cend.
    explicit Sims1(congruence_kind ck) : SimsBase(), _kind() {
      kind(ck);
    }

    // TODO(doc)
    template <typename W>
    Sims1(congruence_kind ck, Presentation<W> p) : Sims1(ck) {
      presentation(p);
    }

    //! Default copy constructor.
    Sims1(Sims1 const& other) = default;

    //! Default move constructor.
    Sims1(Sims1&&) = default;

    //! Default copy assignment operator.
    Sims1& operator=(Sims1 const&) = default;

    //! Default move assignment operator.
    Sims1& operator=(Sims1&&) = default;

    // No doc
    ~Sims1() = default;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    template <typename PresentationOfSomeKind>
    Sims1& presentation(PresentationOfSomeKind const& p) {
      SimsBase::presentation(p);
      if (_kind == congruence_kind::left) {
        presentation::reverse(_presentation);
      }
      return *this;
    }

    // Must accept at least one argument so that we're not calling the 0-arg
    // include() which is const!
    template <typename Arg, typename... Args>
    Sims1& include(Arg arg, Args&&... args) {
      SimsBase::include(arg, std::forward<Args>(args)...);
      if (_kind == congruence_kind::left) {
        SimsBase::reverse(_include);
      }
      return *this;
    }

    // Must accept at least one argument so that we're not calling the 0-arg
    // exclude() which is const!
    template <typename Arg, typename... Args>
    Sims1& exclude(Arg arg, Args&&... args) {
      SimsBase::exclude(arg, std::forward<Args>(args)...);
      if (_kind == congruence_kind::left) {
        SimsBase::reverse(_exclude);
      }
      return *this;
    }

    // This is required because "using SimsBase::include;" tries to
    // drag in all of the include mem fns from SimsBase, which clash with
    // the fns above.
    [[nodiscard]] std::vector<word_type> const& include() const noexcept {
      return SimsBase::include();
    }

    // This is required because "using SimsBase::exclude;" tries to
    // drag in all of the include mem fns from SimsBase, which clash with
    // the fns above.
    [[nodiscard]] std::vector<word_type> const& exclude() const noexcept {
      return SimsBase::exclude();
    }
#endif

    using SimsBase::cbegin_long_rules;
    using SimsBase::number_of_threads;
    using SimsBase::presentation;

    // TODO(doc)
    [[nodiscard]] congruence_kind kind() const noexcept {
      return _kind;
    }

    // TODO(doc)
    Sims1& kind(congruence_kind ck);

    //! Returns the number of one-sided congruences with up to a given
    //! number of classes.
    //!
    //! This function is similar to `std::distance(begin(n), end(n))` and
    //! exists to:
    //! * provide some feedback on the progress of the computation if it runs
    //!   for more than 1 second.
    //! * allow for the computation of `std::distance(begin(n), end(n))` to be
    //!   performed using \ref number_of_threads in parallel.
    //!
    //! \param n the maximum number of congruence classes.
    //!
    //! \returns A value of type \c uint64_t.
    //!
    //! \throws LibsemigroupsException if \p n is \c 0.
    //! \throws LibsemigroupsException if `presentation()` has 0-generators
    //! and 0-relations (i.e. it has not been initialised).
    using SimsBase::number_of_congruences;

    //! Apply the function \p pred to every one-sided
    //! congruence with at most \p n classes
    //!
    //! This function is similar to
    //! `std::for_each(begin(n), end(n), pred)` and exists
    //! to:
    //! * provide some feedback on the progress of the
    //!   computation if it runs for more than 1 second.
    //! * allow for the computation of
    //!   `std::for_each(begin(n), end(n), pred)` to be
    //!   performed using \ref number_of_threads in parallel.
    //!
    //! \param n the maximum number of congruence classes.
    //! \param pred the predicate applied to every congruence found.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p n is \c 0.
    //! \throws LibsemigroupsException if `presentation()` has 0-generators and
    //! 0-relations (i.e. it has not been initialised).
    // void for_each(size_type                                   n,
    //              std::function<void(word_graph_type const&)> pred) const;
    using SimsBase::for_each;

    //! Apply the function \p pred to every one-sided congruence with at most \p
    //! n classes, until it returns \c true.
    //!
    //! This function is similar to `std::find_if(begin(n),
    //! end(n), pred)` and exists to:
    //! * provide some feedback on the progress of the computation if it runs
    //!   for more than 1 second.
    //! * allow for the computation of `std::find_if(begin(n), end(n), pred)`
    //!   to be performed using \ref number_of_threads in parallel.
    //!
    //! \param n the maximum number of congruence classes.
    //! \param pred the predicate applied to every congruence found.
    //!
    //! \returns The first congruence whose WordGraph for which \p pred returns
    //! \c true.
    //!
    //! \throws LibsemigroupsException if \p n is \c 0.
    //! \throws LibsemigroupsException if `presentation()` has 0-generators and
    //! 0-relations (i.e. it has not been initialised).
    using SimsBase::find_if;

    //! Returns a forward iterator pointing at the first congruence.
    //!
    //! Returns a forward iterator pointing to the WordGraph representing
    //! the first congruence described by Sims1 object with at most \p n
    //! classes.
    //!
    //! If incremented, the iterator will point to the next such congruence.
    //! The order which the congruences are returned in is implementation
    //! specific. Iterators of the type returned by this function are equal
    //! whenever they point to equal objects. The iterator is exhausted if
    //! and only if it points to an WordGraph with zero nodes.
    //!
    //! The meaning of the WordGraph pointed at by Sims1 iterators depends
    //! on whether the input is a monoid presentation (i.e.
    //! Presentation::contains_empty_word() returns \c true) or a semigroup
    //! presentation. If the input is a monoid presentation for a monoid
    //! \f$M\f$, then the WordGraph pointed to by an iterator of this type
    //! has precisely \p n nodes, and the right action of \f$M\f$ on the
    //! nodes of the word graph is isomorphic to the action of \f$M\f$ on the
    //! classes of a right congruence.
    //!
    //! If the input is a semigroup presentation for a semigroup \f$S\f$,
    //! then the WordGraph has \p n + 1 nodes, and the right action of
    //! \f$S\f$ on the nodes \f$\{1, \ldots, n\}\f$ of the WordGraph is
    //! isomorphic to the action of \f$S\f$ on the classes of a right
    //! congruence. It'd probably be better in this case if node \f$0\f$ was
    //! not included in the output WordGraph, but it is required in the
    //! implementation of the low-index congruence algorithm, and to avoid
    //! unnecessary copies, we've left it in for the time being. \param n
    //! the maximum number of classes in a congruence.
    //!
    //! \returns
    //! An iterator \c it of type \c iterator pointing to an WordGraph with
    //! at most \p n nodes.
    //!
    //! \throws LibsemigroupsException if \p n is \c 0.
    //! \throws LibsemigroupsException if `presentation()`
    //! has 0-generators and 0-relations (i.e. it has not
    //! been initialised).
    //!
    //! \warning
    //! Copying iterators of this type is expensive.  As a consequence,
    //! prefix incrementing \c ++it the returned  iterator \c it
    //! significantly cheaper than postfix incrementing \c it++.
    //!
    //! \sa
    //! \ref cend
    // TODO(Sims1) it'd be good to remove node 0 to avoid confusion. This
    // seems complicated however, and so isn't done at present.
    using SimsBase::cbegin;

    //! Returns a forward iterator pointing one beyond the last congruence.
    //!
    //! Returns a forward iterator pointing to the empty WordGraph. If
    //! incremented, the returned iterator remains valid and continues to
    //! point at the empty WordGraph.
    //!
    //! \param n the maximum number of classes in a
    //! congruence.
    //!
    //! \returns
    //! An iterator \c it of type \c iterator pointing to an WordGraph with
    //! at most \p 0 nodes.
    //!
    //! \throws LibsemigroupsException if \p n is \c 0.
    //! \throws LibsemigroupsException if `presentation()` has 0-generators
    //! and 0-relations (i.e. it has not been initialised).
    //!
    //! \warning
    //! Copying iterators of this type is expensive.  As a consequence,
    //! prefix incrementing \c ++it the returned  iterator \c it
    //! significantly cheaper than postfix incrementing \c it++.
    //!
    //! \sa
    //! \ref cbegin
    using SimsBase::cend;
  };

  //! Defined in ``sims1.hpp``.
  //!
  //! This class is a helper for `Sims1` calling the `word_graph` member
  //! function attempts to find a right congruence, represented as an
  //! WordGraph, of the semigroup or monoid defined by the presentation
  //! consisting of its \ref presentation and \ref long_rules with the
  //! following properties:
  //! * the transformation semigroup defined by the WordGraph has size
  //!   \ref target_size;
  //! * the number of nodes in the WordGraph is at least \ref min_nodes
  //!   and at most \ref max_nodes.
  //!
  //! If no such WordGraph can be found, then an empty WordGraph is
  //! returned (with `0` nodes and `0` edges).
  class RepOrc : public SimsSettings<RepOrc> {
   private:
    size_t _min;
    size_t _max;
    size_t _size;

   public:
    //! Default constructor.
    RepOrc() : _min(), _max(), _size() {
      init();
    }
    // TODO(doc)
    RepOrc& init();

    //! Construct from Sims1 or MinimalRepOrc.
    //!
    //! This constructor creates a new RepOrc instance with
    //! the same SimsSettings as \p s but that is
    //! otherwise uninitialised.
    //!
    //! \tparam S the type of the argument \p s (which is
    //! derived from `SimsSettings<S>`).
    //!
    //! \param s the Sims1 or MinimalRepOrc whose settings
    //! should be used.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename OtherSubclass>
    explicit RepOrc(SimsSettings<OtherSubclass> const& s) : RepOrc() {
      SimsSettings<RepOrc>::init(s);
    }

    template <typename OtherSubclass>
    RepOrc& init(SimsSettings<OtherSubclass> const& s) {
      SimsSettings<RepOrc>::init(s);
      return *this;
    }

    //! Set the minimum number of nodes.
    //!
    //! This function sets the minimal number of nodes in
    //! the WordGraph that we are seeking.
    //!
    //! \param val the minimum number of nodes
    //!
    //! \returns A reference to `this`.
    //!
    //! \exceptions
    //! \noexcept
    RepOrc& min_nodes(size_t val) noexcept {
      _min = val;
      return *this;
    }

    //! The current minimum number of nodes.
    //!
    //! This function returns the current value for the minimum number of nodes
    //! in the WordGraph that we are seeking.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t min_nodes() const noexcept {
      return _min;
    }

    //! Set the maximum number of nodes.
    //!
    //! This function sets the maximum number of nodes in the WordGraph that we
    //! are seeking.
    //!
    //! \param val the maximum number of nodes
    //!
    //! \returns A reference to `this`.
    //!
    //! \exceptions
    //! \noexcept
    RepOrc& max_nodes(size_t val) noexcept {
      _max = val;
      return *this;
    }

    //! The current maximum number of nodes.
    //!
    //! This function returns the current value for the maximum number of nodes
    //! in the WordGraph that we are seeking.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t max_nodes() const noexcept {
      return _max;
    }

    //! Set the target size.
    //!
    //! This function sets the target size, i.e. the desired size of the
    //! transformation semigroup corresponding to the WordGraph returned by the
    //! function \ref word_graph.
    //!
    //! \param val the target size.
    //!
    //! \returns A reference to `this`.
    //!
    //! \exceptions
    //! \noexcept
    RepOrc& target_size(size_t val) noexcept {
      _size = val;
      return *this;
    }

    //! The current target size.
    //!
    //! This function returns the current value for the target size, i.e. the
    //! desired size of the transformation semigroup corresponding to the
    //! WordGraph returned by the function \ref word_graph.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t target_size() const noexcept {
      return _size;
    }

    //! Get the word_graph.
    //!
    //! This function attempts to find a right congruence, represented as an
    //! WordGraph, of the semigroup or monoid defined by the presentation
    //! consisting of its \ref presentation and \ref long_rules with the
    //! following properties:
    //! * the transformation semigroup defined by the WordGraph has size \ref
    //!   target_size;
    //! * the number of nodes in the WordGraph is at least \ref min_nodes
    //!   and at most \ref max_nodes.
    //!
    //! If no such WordGraph can be found, then an empty WordGraph is returned
    //! (with `0` nodes and `0` edges).
    //!
    //! \warning The return value of this function is recomputed every time it
    //! is called.
    //!
    //! \warning If the return value of \ref number_of_threads is greater than
    //! \c 1, then the value returned by this function is non-deterministic, and
    //! may vary even for the same parameters.
    //!
    //! \tparam T the type of the nodes in the returned word graph. \param
    //! (None) this function has no parameters.
    //!
    //! \returns A value of type `WordGraph`.
    //!
    //! \exceptions \no_libsemigroups_except
    [[nodiscard]] Sims1::word_graph_type word_graph() const;

    using SimsSettings<RepOrc>::presentation;
    using SimsSettings<RepOrc>::cbegin_long_rules;
  };

  //! Defined in ``sims1.hpp``.
  //!
  //! This class is a helper for `Sims1`, calling the `word_graph` member
  //! function attempts to find a right congruence, represented as an
  //! WordGraph, with the minimum possible number of nodes such that the
  //! action of the semigroup or monoid defined by the presentation consisting
  //! of its \ref presentation and \ref long_rules on the nodes of the
  //! WordGraph corresponds to a semigroup of size \ref target_size.
  //!
  //! If no such WordGraph can be found, then an empty WordGraph is
  //! returned (with `0` nodes and `0` edges).
  class MinimalRepOrc : public SimsSettings<MinimalRepOrc> {
   private:
    size_t _size;

   public:
    using SimsSettings<MinimalRepOrc>::stats;

    //! Default constructor
    MinimalRepOrc() = default;

    MinimalRepOrc& init() {
      _size = 0;
      return *this;
    }

    //! Set the target size.
    //!
    //! This function sets the target size, i.e. the desired size of the
    //! transformation semigroup corresponding to the WordGraph returned by
    //! the function \ref word_graph.
    //!
    //! \param val the target size.
    //!
    //! \returns A reference to `this`.
    //!
    //! \exceptions
    //! \noexcept
    MinimalRepOrc& target_size(size_t val) noexcept {
      _size = val;
      return *this;
    }

    //! The current target size.
    //!
    //! This function returns the current value for the target size, i.e. the
    //! desired size of the transformation semigroup corresponding to the
    //! WordGraph returned by the function \ref word_graph.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t target_size() const noexcept {
      return _size;
    }

    //! Get the word graph.
    //!
    //! This function attempts to find a right congruence, represented as an
    //! WordGraph, with the minimum possible number of nodes such that the
    //! action of the semigroup or monoid defined by the presentation
    //! consisting of its \ref presentation and \ref long_rules on the nodes
    //! of the WordGraph corresponds to a semigroup of size \ref
    //! target_size.
    //!
    //! If no such WordGraph can be found, then an empty WordGraph is
    //! returned (with `0` nodes and `0` edges).
    //!
    //! The algorithm implemented by this function repeatedly runs:
    //! \code RepOrc(*this)
    //!     .min_nodes(1)
    //!     .max_nodes(best)
    //!     .target_size(target_size())
    //!     .word_graph();
    //! \endcode
    //! where `best` is initially \ref target_size, until the returned
    //! WordGraph is empty, and then the penultimate WordGraph is returned
    //! (if any).
    //!
    //! \warning The return value of this function is recomputed every time
    //! it is called.
    //!
    //! \warning If the return value of \ref number_of_threads is greater
    //! than \c 1, then the value returned by this function is
    //! non-deterministic, and may vary even for the same parameters.
    //!
    //! \param
    //! (None) this function has no parameters.
    //!
    //! \returns A value of type `WordGraph<uint32_t>`.
    //!
    //! \exceptions \no_libsemigroups_except
    [[nodiscard]] Sims1::word_graph_type word_graph() const;
  };

}  // namespace libsemigroups

#include "sims1.tpp"

#endif  // LIBSEMIGROUPS_SIMS1_HPP_
