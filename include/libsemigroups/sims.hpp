//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-24 James D. Mitchell
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
// congruence" algorithm for 1-sided or 2-sided congruences of semigroups and
// monoids.

// TODO:
// * review the function aliases, and remove them if they are unnecessary
// * doc
// * iwyu
// * const
// * noexcept
// * nodiscard
// * python bindings
// * gap bindings

// TODO(later):
// * use Pruno in RepOrc + MinimalRepOrc
// * a version which allows specifying the word_graph to Sims1 too
// * implement maximum_2_sided_congruence_contained to compute the kernel of the
//   associated homomorphism, which is the largest 2-sided congruence contained
//   in the right congruence.
// * change RepOrc and MinimalRepOrc to compute minimal 2-sided congruences
//   first (by using generating pairs), and then to try and find a right
//   congruence not containing any of the minimal 2-sided congruences.

#ifndef LIBSEMIGROUPS_SIMS_HPP_
#define LIBSEMIGROUPS_SIMS_HPP_

#include <iostream>

#include <algorithm>   // for max
#include <cstddef>     // for size_t
#include <cstdint>     // for uint64_t, uint32_t
#include <filesystem>  // for path, create_directories, temp_directory_path
#include <functional>  // for function
#include <iterator>    // for forward_iterator_tag
#include <mutex>       // for mutex
#include <string>      // for operator+, basic_string
#include <utility>     // for move
#include <vector>      // for vector

#include <fstream>

#include "debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"     // for LIBSEMIGROUPS_EXCEPTION
#include "felsch-graph.hpp"  // for FelschGraph
#include "knuth-bendix.hpp"  // for KnuthBendix
#include "libsemigroups/rewriters.hpp"
#include "presentation.hpp"     // for Presentation, Presentati...
#include "to-presentation.hpp"  // for to_presentation
#include "todd-coxeter.hpp"     // for ToddCoxeter
#include "types.hpp"            // for word_type,
#include "word-graph.hpp"       // for WordGraph

#include "matrix.hpp"

#include "detail/iterator.hpp"  // for detail/default_postfix_increment

#include "rx/ranges.hpp"

namespace libsemigroups {

  //! Defined in ``sims.hpp``.
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
  // Sims2, RepOrc, and MinimalRepOrc without duplicating the code.
  template <typename Subclass>
  class SimsSettings {
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

   private:
    std::vector<word_type>                 _exclude;
    size_t                                 _idle_thread_restarts;
    std::vector<word_type>                 _include;
    std::vector<word_type>::const_iterator _longs_begin;
    size_t                                 _num_threads;
    Presentation<word_type>                _presentation;
    std::vector<std::function<bool(word_graph_type const&)>> _pruners;
    mutable SimsStats                                        _stats;

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

    ~SimsSettings();

    //! Returns the settings object of *this.
    //!
    //! The settings object contains all the settings that are common to
    //! `Sims1`, `RepOrc`, and `MinimalRepOrc`, which are currently:
    //! * \ref presentation
    //! * \ref long_rules
    //! * \ref number_of_threads
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

    //! \brief Set the number of threads.
    //!
    //! This function sets the number of threads to be used by `Sims1`.
    //!
    //! The default value is `1`.
    //!
    //! \param val the maximum number of threads to use.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if the argument \p val is 0.
    //!
    //! \warning If \p val exceeds `std::thread::hardware_concurrency()`, then
    //! this is likely to have a negative impact on the performance of the
    //! algorithms implemented by `Sims1`.
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

    //! \brief Set the presentation over which the congruences produced by an
    //! instance are defined.
    //!
    //! These are the rules used at every node in the depth first search
    //! conducted by objects of this type.
    //!
    //! If the template parameter \p Word is not `word_type`, then
    //! the parameter \p p is first converted to a value of type
    //! `Presentation<word_type>` and it is this converted value that is used.
    //!
    //! \tparam Word the type of words in the input presentation.
    //!
    //! \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if `to_presentation<word_type>(p)`
    //! throws.
    //!
    //! \throws LibsemigroupsException if `p` is not valid.
    //!
    //! \throws LibsemigroupsException if the alphabet of `p` is non-empty and
    //! not equal to that of \ref long_rules or \ref extra.
    // TODO review the previous exception
    //!
    //! \throws LibsemigroupsException if `p` has 0-generators and 0-relations.
    template <typename Word>
    Subclass& presentation(Presentation<Word> const& p);

    //! \anchor presentation
    //! Returns a const reference to the current short rules.
    //!
    //! This function returns the defining presentation of a `Sims1` instance.
    //! The congruences computed by \ref cbegin and \ref cend are defined over
    //! the semigroup or monoid defined by this presentation.
    //!
    //! Note that it might not be the case that the value returned by this
    //! function and the presentation used to construct the object are the same.
    //! A `Sims1` object requires the generators of the defining presentation
    //! \f$\mathcal{P}\f$ to be \f$\{0, \ldots, n - 1\}\f$ where \f$n\f$ is the
    //! size of the alphabet of \f$\mathcal{P}\f$. Every occurrence of every
    //! generator \c a in the presentation \c p used to construct a `Sims1`
    //! instance is replaced by `p.index(a)`.
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

    //! \brief Set the beginning of the  long rules (iterator).
    //!
    //! Set the beginning of the long rules using an iterator.
    //!
    //! The "long rules" are the rules used after a complete deterministic word
    //! graph has been found in the search. If such a word graph is compatible
    //! with the long rules specified by this function, then this word graph is
    //! accepted, and if not it is rejected.
    //!
    //! The purpose of this is to improve the backtrack search by reducing the
    //! time spent processing "long" rules in each node of the search tree, and
    //! to only check them at the leaves.
    //!
    //! \param it iterator pointing at the left hand side of the first long
    //! rule.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if \p it does not point into
    //! `presentation().rules`
    //!
    //! \throws LibsemigroupsException if  \p it does not point at the left hand
    //! side of a rule (i.e. if it points at an odd indexed position in
    //! `presentation().rules`).
    Subclass& cbegin_long_rules(std::vector<word_type>::const_iterator it);

    // TODO(doc)
    Subclass& cbegin_long_rules(size_t pos);

    //! Returns the pointer to the first long rule.
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

    // TODO(doc)
    Subclass& clear_long_rules() {
      return cbegin_long_rules(_presentation.rules.cend());
    }

    // TODO(doc)
    [[nodiscard]] size_t number_of_long_rules() const noexcept {
      return std::distance(_longs_begin, _presentation.rules.cend()) / 2;
    }

    //! Define the length of a "long" rule.
    //!
    //! This function modifies \ref presentation so that the rules whose length
    //! (sum of the lengths of both sizes) is at least  \p val (if any) occur at
    //! the end of `presentation().rules` and so that `cbegin_long_rules` points
    //! at the such rule.
    //!
    //! The relative orders of the rules within \ref presentation
    //! may not be preserved.
    //!
    //! \param val the value of the long rule length.
    //!
    //! \returns
    //! A reference to `this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Subclass& long_rule_length(size_t val);

    template <typename Func>
    Subclass& add_pruner(Func&& func) {
      _pruners.emplace_back(func);
      // TODO could return an iterator to the inserted func in case we want to
      // remove it again
      return static_cast<Subclass&>(*this);
    }

    Subclass& clear_pruners() {
      _pruners.clear();
      return static_cast<Subclass&>(*this);
    }

    auto const& pruners() const noexcept {
      return _pruners;
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

    //! \brief Define a set of pairs that should be included in every
    //! congruence.
    //!
    //! Define a set of pairs that should be included in every congruence.
    //!
    //! The congruences computed by an instance of this type will always contain
    //! the relations input here. In other words, the congruences computed are
    //! only taken among those that contains the pairs of elements of the
    //! underlying semigroup (defined by the presentation returned by \ref
    //! presentation) represented by the relations returned by `include()`.
    //!
    //! \tparam Iterator the type of the arguments, an iterator pointing at a
    //! word_type.
    //!
    //! \param first an iterator pointing to the first rule to be included.
    //! \param last an iterator pointing one beyond the last rule to be
    //! included.
    //!
    //! \returns A reference to \c this.
    //!
    // TODO exceptions
    template <typename Iterator>
    Subclass& include(Iterator first, Iterator last) {
      return include_exclude(first, last, _include);
    }

    //! \brief Define a pair that should be included in every congruence.
    //!
    //! Define a pair that should be included in every congruence.
    //!
    //! The congruences computed by an instance of this type will always contain
    //! the relations input here. In other words, the congruences computed are
    //! only taken among those that contains the pairs of elements of the
    //! underlying semigroup (defined by the presentation returned by \ref
    //! presentation) represented by the relations returned by `include()`.
    //!
    //! \param lhs the left hand side of the rule being added.
    //! \param rhs the right hand side of the rule being added.
    //!
    //! \returns A reference to \c this.
    // TODO exceptions
    Subclass& include(word_type const& lhs, word_type const& rhs) {
      return include_exclude(lhs, rhs, _include);
    }

    // TODO move to helper namespace
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
    // TODO maybe should add instead of replacing similar to exclude of r pair?
    // Replaces current exclude with [first, last)
    template <typename Iterator>
    Subclass& exclude(Iterator first, Iterator last) {
      return include_exclude(first, last, _exclude);
    }

    // TODO(doc)
    // TODO move to helper namespace
    template <typename Container>
    Subclass& exclude(Container const& c) {
      exclude(std::begin(c), std::end(c));
      return static_cast<Subclass&>(*this);
    }

    // TODO(doc)
    // Adds a pair to exclude
    Subclass& exclude(word_type const& lhs, word_type const& rhs) {
      return include_exclude(lhs, rhs, _exclude);
    }

    // TODO(doc)
    [[nodiscard]] std::vector<word_type> const& exclude() const noexcept {
      return _exclude;
    }

    // TODO(doc)
    Subclass& clear_exclude() {
      // TODO remove the pruner
      _exclude.clear();
      return static_cast<Subclass&>(*this);
    }

    // TODO(later) ranges version of include/exclude?

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

    // TODO doc
    Subclass& idle_thread_restarts(size_t val);

    // TODO doc
    [[nodiscard]] size_t idle_thread_restarts() const noexcept {
      return _idle_thread_restarts;
    }

   protected:
    Subclass const& stats_copy_from(SimsStats const& stts) const {
      _stats = std::move(stts);
      return static_cast<Subclass const&>(*this);
    }

   private:
    template <typename OtherSubclass>
    SimsSettings& init_from(SimsSettings<OtherSubclass> const& that);

    template <typename Iterator>
    Subclass& include_exclude(Iterator                first,
                              Iterator                last,
                              std::vector<word_type>& include_or_exclude);

    Subclass& include_exclude(word_type const&        lhs,
                              word_type const&        rhs,
                              std::vector<word_type>& include_or_exclude);
  };

  ////////////////////////////////////////////////////////////////////////
  // SimsSettings - impl of template mem fns
  ////////////////////////////////////////////////////////////////////////

  template <typename Subclass>
  template <typename OtherSubclass>
  SimsSettings<Subclass>&
  SimsSettings<Subclass>::init(SimsSettings<OtherSubclass> const& that) {
    _exclude      = that.exclude();
    _include      = that.include();
    _presentation = that.presentation();

    _idle_thread_restarts = that.idle_thread_restarts();
    _longs_begin          = _presentation.rules.cbegin()
                   + std::distance(that.presentation().rules.cbegin(),
                                   that.cbegin_long_rules());
    _num_threads = that.number_of_threads();
    _stats       = that.stats();
    _pruners     = that.pruners();
    return *this;
  }

  template <typename Subclass>
  template <typename Word>
  Subclass& SimsSettings<Subclass>::presentation(Presentation<Word> const& p) {
    if (p.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument (a presentation) must not have 0 generators");
    }
    // This normalises the rules in the case they are of the right type but
    // not normalised
    auto p_copy = to_presentation<word_type>(p);
    p_copy.validate();
    try {
      presentation::validate_rules(
          p_copy, include().cbegin(), include().cend());
      presentation::validate_rules(
          p_copy, exclude().cbegin(), exclude().cend());
    } catch (LibsemigroupsException const& e) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument (a presentation) is not compatible with include() and "
          "exclude(), the following exception was thrown:\n{}",
          e.what());
    }
    _presentation = std::move(p_copy);
    _longs_begin  = _presentation.rules.cend();
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass>
  template <typename Iterator>
  Subclass& SimsSettings<Subclass>::include_exclude(
      Iterator                first,
      Iterator                last,
      std::vector<word_type>& include_or_exclude) {
    if (std::distance(first, last) % 2 != 0) {
      LIBSEMIGROUPS_EXCEPTION("expected the distance between the 1st and 2nd "
                              "arguments (iterators) to be even, found {}",
                              std::distance(first, last));
    }
    for (auto it = first; it != last; ++it) {
      presentation().validate_word(it->cbegin(), it->cend());
    }
    include_or_exclude.assign(first, last);
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass>
  SimsSettings<Subclass>::~SimsSettings() = default;

  ////////////////////////////////////////////////////////////////////////
  // Sims1 - Sims2 - forward decl
  ////////////////////////////////////////////////////////////////////////

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
      // TODO use SimsSettings::word_graph_type
      using word_graph_type = WordGraph<uint32_t>;

      //! Type for the nodes in the associated WordGraph objects.
      using node_type = typename word_graph_type::node_type;

      using label_type = typename word_graph_type::label_type;

      //! The size_type of the associated WordGraph objects.
      using size_type = typename word_graph_type::size_type;

      //! Type for letters in the underlying presentation.
      using letter_type = typename word_type::value_type;

      ////////////////////////////////////////////////////////////////////////
      // SimsBase nested classes - protected
      ////////////////////////////////////////////////////////////////////////

      // This class collects some common aspects of the iterator and
      // thread_iterator nested classes. The mutex does nothing for <iterator>
      // and is an actual std::mutex for <thread_iterator>. Also subclassed by
      // Sims2::iterator_base.
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
        Sims1or2 const*         _sims1or2;

        // Push initial PendingDef's into _pending, see tpp file for
        // explanation.
        void init(size_type n);

        // We could use the copy constructor, but there's no point in copying
        // anything except the FelschGraph and so we only copy that.
        void partial_copy_for_steal_from(IteratorBase const& that) {
          _felsch_graph = that._felsch_graph;
        }

        // Try to pop from _pending into the argument (reference), returns true
        // if successful and false if not.
        [[nodiscard]] bool try_pop(PendingDef& pd);

        // Try to make the definition represented by PendingDef, returns false
        // if it wasn't possible, and true if it was.
        bool try_define(PendingDef const& current);

        // Install any new pending definitions arising from the definition of
        // "current", this should only be called after "try_define(current)",
        // and is in a separate function so that we can define another version
        // of "try_define" in "Sims2".
        bool install_descendents(PendingDef const& current);

        IteratorBase(Sims1or2 const* s, size_type n);

       public:
        // None of the constructors are noexcept because the corresponding
        // constructors for Presentation aren't currently
        IteratorBase();
        IteratorBase(IteratorBase const& that);
        IteratorBase(IteratorBase&& that);
        IteratorBase& operator=(IteratorBase const& that);
        IteratorBase& operator=(IteratorBase&& that);
        ~IteratorBase();

        [[nodiscard]] bool operator==(IteratorBase const& that) const noexcept {
          return _felsch_graph == that._felsch_graph;
        }

        [[nodiscard]] bool operator!=(IteratorBase const& that) const noexcept {
          return !(operator==(that));
        }

        [[nodiscard]] const_reference operator*() const noexcept {
          return _felsch_graph;
        }

        [[nodiscard]] const_pointer operator->() const noexcept {
          return &_felsch_graph;
        }

        void swap(IteratorBase& that) noexcept;

        SimsStats& stats() noexcept {
          return _sims1or2->stats();
        }

        size_type maximum_number_of_classes() const noexcept {
          return _max_num_classes;
        }

        Sims1or2 const& sims() const noexcept {
          return *_sims1or2;
        }
      };  // class IteratorBase

     public:
      ////////////////////////////////////////////////////////////////////////
      // SimsBase nested classes - public
      ////////////////////////////////////////////////////////////////////////

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
        using const_pointer   = typename iterator_base::const_pointer;
        using const_reference = typename iterator_base::const_reference;
        using size_type = typename std::vector<word_graph_type>::size_type;
        using difference_type =
            typename std::vector<word_graph_type>::difference_type;
        using pointer    = typename std::vector<word_graph_type>::pointer;
        using reference  = typename std::vector<word_graph_type>::reference;
        using value_type = word_graph_type;
        using iterator_category = std::forward_iterator_tag;

        using sims_type = typename iterator_base::sims_type;

        using iterator_base::iterator_base;

       private:
        // Only want SimsBase to be able to use this constructor.
        iterator(Sims1or2 const* s, size_type n);

        // So that we can use the constructor above.
        friend iterator SimsBase::cbegin(SimsBase::size_type) const;
        friend iterator SimsBase::cend(SimsBase::size_type) const;

       public:
        ~iterator();

        // prefix
        iterator const& operator++();

        // postfix
        //! No doc
        iterator operator++(int) {
          return detail::default_postfix_increment(*this);
        }

        using iterator_base::swap;
      };  // class iterator

     private:
      class thread_runner;
      class thread_iterator;

      void report_at_start(size_t num_classes) const;
      void report_progress_from_thread() const;
      void report_final() const;

      void throw_if_not_ready(size_type n) const;

     public:
      SimsBase();
      SimsBase(SimsBase const& other)      = default;
      SimsBase(SimsBase&&)                 = default;
      SimsBase& operator=(SimsBase const&) = default;
      SimsBase& operator=(SimsBase&&)      = default;
      ~SimsBase();

      Sims1or2& init();

      // Required because we are inheriting from Reporter and SimsSettings
      using SimsSettings<Sims1or2>::presentation;
      using SimsSettings<Sims1or2>::number_of_threads;
      using SimsSettings<Sims1or2>::stats;
      using SimsSettings<Sims1or2>::include;
      using SimsSettings<Sims1or2>::exclude;
      using SimsSettings<Sims1or2>::cbegin_long_rules;

      [[nodiscard]] iterator cbegin(size_type n) const {
        throw_if_not_ready(n);
        return iterator(static_cast<Sims1or2 const*>(this), n);
      }

      [[nodiscard]] iterator cend(size_type n) const {
        throw_if_not_ready(n);
        return iterator(static_cast<Sims1or2 const*>(this), 0);
      }

      // Apply the function pred to every congruence with at
      // most n classes
      void for_each(size_type                                   n,
                    std::function<void(word_graph_type const&)> pred) const;

      word_graph_type
      find_if(size_type                                   n,
              std::function<bool(word_graph_type const&)> pred) const;

      uint64_t number_of_congruences(size_type n) const;
    };  // SimsBase
  }     // namespace detail

  namespace sims {
    class const_cgp_iterator;
    class const_rcgp_iterator;
  }  // namespace sims
  //! Defined in ``sims.hpp``.
  //!
  //! On this page we describe the functionality relating to the small index
  //! congruence algorithm for 1-sided congruences. The algorithm implemented by
  //! this class is essentially the low index subgroup algorithm for
  //! finitely presented groups described in Section 5.6 of [Computation with
  //! Finitely Presented Groups](https://doi.org/10.1017/CBO9780511574702) by C.
  //! Sims. The low index subgroups algorithm was adapted for semigroups and
  //! monoids by R. Cirpons, J. D. Mitchell, and M. Tsalakou; see
  //! \cite Anagnostopoulou-Merkouri2023aa
  //!
  //! The purpose of this class is to provide the functions \ref cbegin, \ref
  //! cend, \ref for_each, and \ref find_if which permit iterating through the
  //! one-sided congruences of a semigroup or monoid defined by a presentation
  //! containing (a possibly empty) set of pairs and with at most a given
  //! number of classes. An iterator returned by \ref cbegin points at an
  //! WordGraph instance containing the action of the semigroup or monoid
  //! on the classes of a congruence.
  //!
  //! \sa Sims2 for equivalent functionality for 2-sided congruences.
  //! \sa SimsSettings for the various things that can be set in a Sims1 object.
  class Sims1 : public detail::SimsBase<Sims1> {
    // Aliases
    using SimsBase      = detail::SimsBase<Sims1>;
    using iterator_base = IteratorBase;

    // Friends
    // so that SimsBase can access iterator_base, PendingDef, etc
    friend SimsBase;
    // so that Sims2 can access PendingDef
    friend Sims2;

    // Forward decl
    struct PendingDef;

   public:
    // We use WordGraph, even though the iterators produced by this class
    // hold FelschGraph's, none of the features of FelschGraph are useful
    // for the output, only for the implementation
    //! The type of the associated WordGraph objects.
    using word_graph_type = SimsBase::word_graph_type;

    //! Type of the nodes in the associated WordGraph objects.
    using node_type = word_graph_type::node_type;

    //! Type of the edge labels in the associated WordGraph objects.
    using label_type = word_graph_type::label_type;

    //! Type for letters in the underlying presentation.
    using letter_type = word_type::value_type;

    //! The \ref WordGraph::size_type of the associated WordGraph objects.
    using size_type = word_graph_type::size_type;

   private:
    using Definition = std::pair<node_type, label_type>;

    friend class sims::const_rcgp_iterator;
    friend class sims::const_cgp_iterator;

    using felsch_graph_type
        = FelschGraph<word_type, node_type, std::vector<Definition>>;

   public:
    //! Default constructor
    Sims1() = default;

    // TODO(doc)
    using SimsBase::init;

    //! \brief Construct from a presentation.
    //!
    //! Constructs an instance from a presentation of any kind.
    //!
    //! The rules of the presentation \p p are used at every node in the depth
    //! first search conducted by an object of this type.
    //!
    //! If the template parameter \p Word is not \ref word_type, then
    //! the parameter \p p is first converted to a value of type
    //! `Presentation<word_type>` and it is this converted value that is used.
    //!
    //! \throws LibsemigroupsException if `to_presentation<word_type>(p)` throws
    //! \throws LibsemigroupsException if `p` is not valid
    //! \throws LibsemigroupsException if `p` has 0-generators and 0-relations.
    //!
    //! \tparam Word the type of the words in the presentation \p p
    //! \param p the presentation
    //!
    //! \sa presentation
    //! \sa init
    template <typename Word>
    explicit Sims1(Presentation<Word> const& p) : Sims1() {
      presentation(p);
    }

    //! \copydoc Sims1::Sims1(Presentation<Word> const&)
    template <typename Word>
    explicit Sims1(Presentation<Word> const&& p) : Sims1() {
      presentation(std::move(p));
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

    //! \brief Initialize an existing object.
    //!
    //! This function puts an object back into the same state as if it had
    //! been newly constructed from the presentation \p p.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if `to_presentation<word_type>(p)` throws
    //! \throws LibsemigroupsException if `p` is not valid
    //! \throws LibsemigroupsException if `p` has 0-generators and 0-relations.
    //!
    //! \warning This function has no exception guarantee, the object will be
    //! in the same state as if it was default constructed if an exception is
    //! thrown.
    //!
    //! \tparam Word the type of the words in the presentation \p p
    //! \param p the presentation
    //!
    //! \sa presentation(Presentation<Word> const&)
    template <typename Word>
    Sims1& init(Presentation<Word> const& p) {
      init();
      presentation(p);
      return *this;
    }
    // TODO init from rvalue reference presentation

#ifdef PARSED_BY_DOXYGEN
    //! \brief Returns the number of one-sided congruences with up to a given
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
    uint64_t number_of_congruences(size_t n);

    //! \brief Apply a unary predicate to every one-sided congruence with at
    //! most a given number of classes.
    //!
    //! Apply the function \p pred to every one-sided congruence with at most
    //! \p n classes
    //!
    //!  This function is similar to `std::for_each(begin(n), end(n), pred)` and
    //!  exists to:
    //!  * provide some feedback on the progress of the computation if it runs
    //!  for more than 1 second.
    //!  * allow for the computation of `std::for_each(begin(n), end(n), pred)`
    //!  to be performed using \ref number_of_threads in parallel.
    //!
    //!  \param n the maximum number of congruence classes.
    //!  \param pred the predicate applied to every congruence found.
    //!
    //!  \returns (None)
    //!
    //!  \throws LibsemigroupsException if \p n is \c 0.
    //!  \throws LibsemigroupsException if `presentation()` has 0-generators and
    //!  0-relations (i.e. it has not been initialised).
    //!
    //!  \sa cbegin
    void for_each(size_type                                   n,
                  std::function<void(word_graph_type const&)> pred) const;

    //! \brief Apply a unary predicate to one-sided congruences with at  most a
    //! given number of classes, until it returns \c true.
    //!
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
    //! \returns The first WordGraph for which \p pred returns \c true.
    //!
    //! \throws LibsemigroupsException if \p n is \c 0.
    //! \throws LibsemigroupsException if `presentation()` has 0-generators and
    //! 0-relations (i.e. it has not been initialised).
    //!
    //! \sa cbegin
    word_graph_type
    find_if(size_type                                   n,
            std::function<bool(word_graph_type const&)> pred) const;

    //! \brief Returns a forward iterator pointing at the first congruence.
    //!
    //! Returns a forward iterator pointing to the WordGraph representing the
    //! first congruence described by an object of this type with at most \p n
    //! classes.
    //!
    //! If incremented, the iterator will point to the next such congruence.
    //! The order in which the congruences are returned is implementation
    //! specific. Iterators of the type returned by this function are equal
    //! whenever they point to equal objects. The iterator is exhausted if
    //! and only if it points to an WordGraph with zero nodes.
    //!
    //! The meaning of the WordGraph pointed at by the returned iterator depends
    //! on whether the input is a monoid presentation (i.e.
    //! Presentation::contains_empty_word() returns \c true) or a semigroup
    //! presentation. If the input is a monoid presentation for a monoid
    //! \f$M\f$, then the WordGraph pointed to by an iterator of this type has
    //! precisely \p n nodes, and the right action of \f$M\f$ on the nodes of
    //! the word graph is isomorphic to the action of \f$M\f$ on the classes of
    //! a right congruence.
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
    [[nodiscard]] iterator cbegin(size_type n) const;

    //! \brief Returns a forward iterator pointing one beyond the last
    //! congruence.
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
    [[nodiscard]] iterator cend(size_type n) const;
#endif
  };

  //! Defined in ``sims.hpp``.
  //!
  //! On this page we describe the functionality relating to the small index
  //! congruence algorithm for 2-sided congruences. The algorithm implemented by
  //! this class is described in \cite Anagnostopoulou-Merkouri2023aa.
  //!
  //! The purpose of this class is to provide the functions \ref cbegin, \ref
  //! cend, \ref for_each, and \ref find_if which permit iterating through the
  //! two-sided congruences of a semigroup or monoid defined by a presentation
  //! containing, or not containing, (possibly empty) sets of pairs and with at
  //! most a given number of classes. An iterator returned by \ref cbegin points
  //! at an WordGraph instance containing the action of the semigroup or monoid
  //! on the classes of a congruence.
  //!
  //! \sa Sims1 for equivalent functionality for 1-sided congruences.
  class Sims2 : public detail::SimsBase<Sims2> {
    using SimsBase = detail::SimsBase<Sims2>;
    // so that SimsBase can access iterator_base, PendingDef, etc
    friend SimsBase;

   public:
    //! \copydoc Sims1::node_type
    using node_type = SimsBase::node_type;
    //! \copydoc Sims1::label_type
    using label_type = SimsBase::label_type;
    //! \copydoc Sims1::letter_type
    using letter_type = SimsBase::letter_type;
    //! \copydoc Sims1::size_type
    using size_type = SimsBase::size_type;
    //! \copydoc Sims1::word_graph_type
    using word_graph_type = SimsBase::word_graph_type;

    //! Default constructor.
    Sims2() = default;
    //! Default copy constructor.
    Sims2(Sims2 const& other) = default;
    //! Default move constructor.
    Sims2(Sims2&&) = default;
    //! Default copy assignment operator.
    Sims2& operator=(Sims2 const&) = default;
    //! Default move assignment operator.
    Sims2& operator=(Sims2&&) = default;

    ~Sims2() = default;

    //! \copydoc Sims1::init
    Sims2& init() {
      SimsSettings<Sims2>::init();
      return *this;
    }

    //! \copydoc Sims1::Sims1(Presentation<Word> const&)
    template <typename Word>
    explicit Sims2(Presentation<Word> const& p) : Sims2() {
      presentation(p);
    }

    //! \copydoc Sims1::Sims1(Presentation<Word> const&)
    template <typename Word>
    explicit Sims2(Presentation<Word> const&& p) : Sims2() {
      presentation(std::move(p));
    }

#ifdef PARSED_BY_DOXYGEN
    //! \copydoc Sims1::cbegin
    [[nodiscard]] iterator cbegin(size_type n) const;

    //! \copydoc Sims1::cend
    [[nodiscard]] iterator cend(size_type n) const;
#endif

   private:
    struct PendingDef;

    // This class collects some common aspects of the iterator and
    // thread_iterator nested classes. The mutex does nothing for <iterator>
    // and is an actual std::mutex for <thread_iterator>.
    class iterator_base : public SimsBase::IteratorBase {
      class RuleContainer;

     public:
      using const_reference = SimsBase::IteratorBase::const_reference;
      using const_pointer   = SimsBase::IteratorBase::const_pointer;

     private:
      std::unique_ptr<RuleContainer> _2_sided_include;
      std::vector<word_type>         _2_sided_words;

     protected:
      // TODO delete after ensuring that compilation with GCC works
      // using SimsBase::IteratorBase::init;
      // using SimsBase::IteratorBase::try_pop;

      // We could use the copy constructor, but there's no point in copying
      // anything except the FelschGraph and so we only copy that.
      void partial_copy_for_steal_from(iterator_base const& that);

      // Try to make the definition represented by PendingDef, returns false if
      // it wasn't possible, and true if it was.
      //! No doc
      [[nodiscard]] bool try_define(PendingDef const&);

      iterator_base(Sims2 const* s, size_type n);

     public:
      iterator_base() = default;

      iterator_base(iterator_base const& that);
      iterator_base(iterator_base&& that);
      iterator_base& operator=(iterator_base const& that);
      iterator_base& operator=(iterator_base&& that);
      ~iterator_base();

      // TODO delete after ensuring that compilation with GCC works
      // using SimsBase::IteratorBase::operator==;
      // using SimsBase::IteratorBase::operator!=;
      // using SimsBase::IteratorBase::operator*;
      // using SimsBase::IteratorBase::operator->;

      //! No doc
      void swap(iterator_base& that) noexcept;

      using SimsBase::IteratorBase::stats;
    };  // class iterator_base
  };    // Sims2

  //! Defined in ``sims.hpp``.
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
  };

  //! Defined in ``sims.hpp``.
  //!
  //! This class is a helper for `Sims1`, calling the `word_graph` member
  //! function attempts to find a right congruence, represented as an
  //! WordGraph, with the minimum possible number of nodes such that the
  //! action of the semigroup or monoid defined by the presentation consisting
  //! of its \ref presentation on the nodes of the WordGraph corresponds to a
  //! semigroup of size \ref target_size.
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

  // This struct provides an alternative way of doing MinimalRepOrc, when the
  // generating pairs of the minimal 2-sided congruences are known. These pairs
  // should be added to forbid, and then your Pruno instance should be passed to
  // a Sims1 object via add_pruner.
  struct Pruno {
    std::vector<word_type> forbid;
    // TODO to cpp
    bool operator()(Sims1::word_graph_type const& wg) {
      auto first = forbid.cbegin(), last = forbid.cend();
      // TODO use 1 felsch tree per excluded pairs, and use it to check if
      // paths containing newly added edges, lead to the same place
      for (auto it = first; it != last; it += 2) {
        bool this_rule_compatible = true;
        for (uint32_t n = 0; n < wg.number_of_active_nodes(); ++n) {
          auto l = word_graph::follow_path_no_checks(wg, n, *it);
          if (l != UNDEFINED) {
            auto r = word_graph::follow_path_no_checks(wg, n, *(it + 1));
            if (r == UNDEFINED || (r != UNDEFINED && l != r)) {
              this_rule_compatible = false;
              break;
            }
          } else {
            this_rule_compatible = false;
            break;
          }
        }
        if (this_rule_compatible) {
          return false;
        }
      }
      return true;
    }
  };

  namespace sims {
    class const_cgp_iterator;

    // This is similar to FroidurePinBase::const_rule_iterator
    // Right Congruence Generating Pairs (rcgp)
    class const_rcgp_iterator {
     public:
      using size_type = typename std::vector<relation_type>::size_type;
      using difference_type =
          typename std::vector<relation_type>::difference_type;
      using const_pointer = typename std::vector<relation_type>::const_pointer;
      using pointer       = typename std::vector<relation_type>::pointer;
      using const_reference =
          typename std::vector<relation_type>::const_reference;
      using reference         = typename std::vector<relation_type>::reference;
      using value_type        = relation_type;
      using iterator_category = std::forward_iterator_tag;

      using word_graph_type   = Sims1::word_graph_type;
      using felsch_graph_type = Sims1::felsch_graph_type;
      using node_type         = word_graph_type::node_type;
      using label_type        = word_graph_type::label_type;

     protected:
      felsch_graph_type _reconstructed_word_graph;

     private:
      label_type             _gen;
      node_type              _source;
      mutable relation_type  _relation;
      Forest                 _tree;
      word_graph_type const* _word_graph;

      // Set source to ptr->number_of_active_nodes() for cend
      const_rcgp_iterator(Presentation<word_type> const& p,
                          word_graph_type const*         ptr,
                          node_type                      source,
                          label_type                     gen);

      // To allow the use of the above constructor
      template <typename Node>
      friend const_rcgp_iterator
      cbegin_right_generating_pairs_no_checks(WordGraph<Node> const&);

      template <typename Node>
      friend const_rcgp_iterator
      cbegin_right_generating_pairs_no_checks(Presentation<word_type> const&,
                                              WordGraph<Node> const&);

      // To allow the use of the above constructor
      template <typename Node>
      friend const_rcgp_iterator
      cend_right_generating_pairs_no_checks(WordGraph<Node> const&);

      template <typename Node>
      friend const_rcgp_iterator
      cend_right_generating_pairs_no_checks(Presentation<word_type> const&,
                                            WordGraph<Node> const&);

      // To allow the use of the private constructor from pointer
      template <typename Node>
      friend const_cgp_iterator
      cbegin_two_sided_generating_pairs_no_checks(WordGraph<Node> const&);

      template <typename Node>
      friend const_cgp_iterator cbegin_two_sided_generating_pairs_no_checks(
          Presentation<word_type> const&,
          WordGraph<Node> const&);

      // To allow the use of the above constructor
      template <typename Node>
      friend const_cgp_iterator
      cend_two_sided_generating_pairs_no_checks(WordGraph<Node> const&);

      template <typename Node>
      friend const_cgp_iterator
      cend_two_sided_generating_pairs_no_checks(Presentation<word_type> const&,
                                                WordGraph<Node> const&);

     public:
      // TODO add noexcept?
      const_rcgp_iterator()                                      = default;
      const_rcgp_iterator(const_rcgp_iterator const&)            = default;
      const_rcgp_iterator(const_rcgp_iterator&&)                 = default;
      const_rcgp_iterator& operator=(const_rcgp_iterator const&) = default;
      const_rcgp_iterator& operator=(const_rcgp_iterator&&)      = default;

      ~const_rcgp_iterator();

      [[nodiscard]] bool
      operator==(const_rcgp_iterator const& that) const noexcept {
        return _gen == that._gen && _source == that._source;
      }

      [[nodiscard]] bool
      operator!=(const_rcgp_iterator const& that) const noexcept {
        return !(this->operator==(that));
      }

      [[nodiscard]] const_reference operator*() const {
        populate_relation();
        return _relation;
      }

      [[nodiscard]] const_pointer operator->() const {
        populate_relation();
        return &_relation;
      }

      // prefix
      const_rcgp_iterator const& operator++();

      // postfix
      const_rcgp_iterator operator++(int) noexcept {
        return detail::default_postfix_increment(*this);
      }

      void swap(const_rcgp_iterator& that) noexcept;

     protected:
      [[nodiscard]] bool at_end() const noexcept {
        return _source == _word_graph->number_of_active_nodes();
      }

     private:
      bool populate_relation() const;
    };  // const_rcgp_iterator

    class const_cgp_iterator : public const_rcgp_iterator {
     public:
      using size_type         = const_rcgp_iterator::size_type;
      using difference_type   = const_rcgp_iterator::difference_type;
      using const_pointer     = const_rcgp_iterator::const_pointer;
      using pointer           = const_rcgp_iterator::pointer;
      using const_reference   = const_rcgp_iterator::const_reference;
      using reference         = const_rcgp_iterator::reference;
      using value_type        = relation_type;
      using iterator_category = std::forward_iterator_tag;

      using word_graph_type   = Sims1::word_graph_type;
      using felsch_graph_type = Sims1::felsch_graph_type;
      using node_type         = word_graph_type::node_type;
      using label_type        = word_graph_type::label_type;

      using const_rcgp_iterator::const_rcgp_iterator;

      ~const_cgp_iterator();

      using const_rcgp_iterator::operator==;
      using const_rcgp_iterator::operator!=;
      using const_rcgp_iterator::operator*;
      using const_rcgp_iterator::operator->;

      // prefix
      // TODO to cpp file
      const_cgp_iterator const& operator++() {
        size_type start = _reconstructed_word_graph.definitions().size();
        const_rcgp_iterator::operator++();
        if (at_end()) {
          return *this;
        }
        // Copy wasteful
        auto p = _reconstructed_word_graph.presentation();
        presentation::add_rule_no_checks(p, (**this).first, (**this).second);
        _reconstructed_word_graph.presentation(std::move(p));

        std::ignore = _reconstructed_word_graph.process_definitions(start);
        return *this;
      }

      // postfix
      const_cgp_iterator operator++(int) noexcept {
        return detail::default_postfix_increment(*this);
      }

      void swap(const_cgp_iterator& that) noexcept {
        const_rcgp_iterator::swap(that);
      }

     private:
      bool populate_relation() const;
    };  // const_cgp_iterator

    template <typename Node>
    rx::iterator_range<const_rcgp_iterator>
    right_generating_pairs(Presentation<word_type> const& p,
                           WordGraph<Node> const&         wg);

    template <typename Node>
    rx::iterator_range<const_rcgp_iterator>
    right_generating_pairs(WordGraph<Node> const& wg);

    template <typename Node>
    rx::iterator_range<const_cgp_iterator>
    two_sided_generating_pairs(Presentation<word_type> const& p,
                               WordGraph<Node> const&         wg);

    template <typename Node>
    rx::iterator_range<const_cgp_iterator>
    two_sided_generating_pairs(WordGraph<Node> const& wg);

    template <typename Node>
    bool is_right_congruence(Presentation<word_type> const& p,
                             WordGraph<Node> const&         wg) {
      if (p.alphabet().size() != wg.out_degree()) {
        return false;
      }
      auto const N     = wg.number_of_active_nodes();
      auto       first = wg.cbegin_nodes();
      auto       last  = wg.cbegin_nodes() + N;

      if (!word_graph::is_complete(wg, first, last)
          || !word_graph::is_compatible(
              wg, first, last, p.rules.cbegin(), p.rules.cend())) {
        return false;
      }
      auto norf = word_graph::nodes_reachable_from(wg, 0);
      return std::all_of(
          norf.begin(), norf.end(), [&N](auto n) { return n < N; });
    }

    template <typename Node>
    void validate_right_congruence(Presentation<word_type> const& p,
                                   WordGraph<Node> const&         wg) {
      if (!is_right_congruence(p, wg)) {
        LIBSEMIGROUPS_EXCEPTION("The 2nd argument (a word graph) does not "
                                "represent a right congruence of the semigroup "
                                "defined by the 1st argument (a presentation)")
      }
    }

    template <typename Node>
    bool is_right_congruence_of_dual(Presentation<word_type> const& p,
                                     WordGraph<Node> const&         wg) {
      auto p_rev(p);
      presentation::reverse(p_rev);
      return is_right_congruence(p_rev, wg);
    }

    template <typename Node>
    bool is_two_sided_congruence_no_checks(Presentation<word_type> const& p,
                                           WordGraph<Node> const&         wg) {
      auto const N     = wg.number_of_active_nodes();
      auto       first = wg.cbegin_nodes();
      auto       last  = wg.cbegin_nodes() + N;

      for (auto const& rule : right_generating_pairs(p, wg)) {
        if (!word_graph::is_compatible(
                wg, first, last, rule.first, rule.second)) {
          return false;
        }
      }
      return true;
    }

    template <typename Node>
    bool is_two_sided_congruence(Presentation<word_type> const& p,
                                 WordGraph<Node> const&         wg) {
      if (!is_right_congruence(p, wg)) {
        return false;
      }
      return is_two_sided_congruence_no_checks(p, wg);
    }

    template <typename Node>
    void validate_two_sided_congruence(Presentation<word_type> const& p,
                                       WordGraph<Node> const&         wg) {
      if (!is_two_sided_congruence(p, wg)) {
        LIBSEMIGROUPS_EXCEPTION(
            "The 2nd argument (a word graph) does not "
            "represent a 2-sided congruence of the semigroup "
            "defined by the 1st argument (a presentation)")
      }
    }

    // Returns an iterator pointing to the first generating pair of the right
    // congruence on the semigroup or monoid defined by \p p defined by the word
    // graph \p wg. Note that this is the generating pairs of the right
    // congruence so defined not the 2-sided congruence.
    template <typename Node>
    const_rcgp_iterator
    cbegin_right_generating_pairs_no_checks(Presentation<word_type> const& p,
                                            WordGraph<Node> const&         wg) {
      return const_rcgp_iterator(p, &wg, 0, 0);
    }

    template <typename Node>
    const_rcgp_iterator
    cbegin_right_generating_pairs(Presentation<word_type> const& p,
                                  WordGraph<Node> const&         wg) {
      validate_right_congruence(p, wg);
      return cbegin_right_generating_pairs(p, wg);
    }

    template <typename Node>
    const_cgp_iterator cbegin_two_sided_generating_pairs_no_checks(
        Presentation<word_type> const& p,
        WordGraph<Node> const&         wg) {
      return const_cgp_iterator(p, &wg, 0, 0);
    }

    template <typename Node>
    const_cgp_iterator
    cbegin_two_sided_generating_pairs(Presentation<word_type> const& p,
                                      WordGraph<Node> const&         wg) {
      validate_two_sided_congruence(p, wg);
      return cbegin_two_sided_generating_pairs_no_checks(p, wg);
    }

    template <typename Node>
    const_rcgp_iterator
    cend_right_generating_pairs_no_checks(Presentation<word_type> const& p,
                                          WordGraph<Node> const&         wg) {
      return const_rcgp_iterator(p, &wg, wg.number_of_active_nodes(), 0);
    }

    template <typename Node>
    const_rcgp_iterator
    cend_right_generating_pairs(Presentation<word_type> const& p,
                                WordGraph<Node> const&         wg) {
      validate_right_congruence(p, wg);
      return cend_right_generating_pairs_no_checks(p, wg);
    }

    template <typename Node>
    const_cgp_iterator
    cend_two_sided_generating_pairs_no_checks(Presentation<word_type> const& p,
                                              WordGraph<Node> const& wg) {
      return const_cgp_iterator(p, &wg, wg.number_of_active_nodes(), 0);
    }

    template <typename Node>
    const_cgp_iterator
    cend_two_sided_generating_pairs(Presentation<word_type> const& p,
                                    WordGraph<Node> const&         wg) {
      validate_two_sided_congruence(p, wg);
      return cend_two_sided_generating_pairs_no_checks(p, wg);
    }

    template <typename Node>
    rx::iterator_range<const_rcgp_iterator>
    right_generating_pairs_no_checks(Presentation<word_type> const& p,
                                     WordGraph<Node> const&         wg) {
      return rx::iterator_range(cbegin_right_generating_pairs_no_checks(p, wg),
                                cend_right_generating_pairs_no_checks(p, wg));
    }

    template <typename Node>
    rx::iterator_range<const_rcgp_iterator>
    right_generating_pairs(Presentation<word_type> const& p,
                           WordGraph<Node> const&         wg) {
      validate_right_congruence(p, wg);
      return right_generating_pairs_no_checks(p, wg);
    }

    template <typename Node>
    rx::iterator_range<const_rcgp_iterator>
    right_generating_pairs_no_checks(WordGraph<Node> const& wg) {
      Presentation<word_type> p;
      p.alphabet(wg.out_degree());
      return right_generating_pairs_no_checks(p, wg);
    }

    template <typename Node>
    rx::iterator_range<const_rcgp_iterator>
    right_generating_pairs(WordGraph<Node> const& wg) {
      Presentation<word_type> p;
      p.alphabet(wg.out_degree());
      validate_right_congruence(p, wg);
      return right_generating_pairs_no_checks(p, wg);
    }

    template <typename Node>
    rx::iterator_range<const_cgp_iterator>
    two_sided_generating_pairs_no_checks(Presentation<word_type> const& p,
                                         WordGraph<Node> const&         wg) {
      return rx::iterator_range(
          cbegin_two_sided_generating_pairs_no_checks(p, wg),
          cend_two_sided_generating_pairs_no_checks(p, wg));
    }

    template <typename Node>
    rx::iterator_range<const_cgp_iterator>
    two_sided_generating_pairs(Presentation<word_type> const& p,
                               WordGraph<Node> const&         wg) {
      validate_two_sided_congruence(p, wg);
      return two_sided_generating_pairs_no_checks(p, wg);
    }

    template <typename Node>
    rx::iterator_range<const_cgp_iterator>
    two_sided_generating_pairs_no_checks(WordGraph<Node> const& wg) {
      Presentation<word_type> p;
      p.alphabet(wg.out_degree());
      return rx::iterator_range(
          cbegin_two_sided_generating_pairs_no_checks(p, wg),
          cend_two_sided_generating_pairs_no_checks(p, wg));
    }

    template <typename Node>
    rx::iterator_range<const_cgp_iterator>
    two_sided_generating_pairs(WordGraph<Node> const& wg) {
      Presentation<word_type> p;
      p.alphabet(wg.out_degree());
      validate_two_sided_congruence(p, wg);
      return two_sided_generating_pairs_no_checks(p, wg);
    }

    template <typename Node>
    bool is_maximal_right_congruence(Presentation<word_type> const& p,
                                     WordGraph<Node> const&         wg) {
      if (!is_right_congruence(p, wg)) {
        return false;
      } else if (wg.number_of_active_nodes() == 1) {
        // Universal congruence
        return false;
      }

      ToddCoxeter tc(congruence_kind::right);

      auto   tree = word_graph::spanning_tree(wg, 0);
      size_t N    = wg.number_of_active_nodes();

      for (Node x = 0; x < N - 1; ++x) {
        auto wx = tree.path_to_root(x);
        std::reverse(wx.begin(), wx.end());
        for (Node y = x + 1; y < N; ++y) {
          auto wy = tree.path_to_root(y);
          std::reverse(wy.begin(), wy.end());
          auto copy = wg;
          // TODO avoid the copy here
          copy.induced_subgraph_no_checks(static_cast<Node>(0),
                                          wg.number_of_active_nodes());
          tc.init(tc.kind(), p, copy).add_pair(wx, wy);
          LIBSEMIGROUPS_ASSERT(tc.word_graph().number_of_nodes()
                               == wg.number_of_active_nodes());
          // fmt::print("x = {}, y = {}\n", x, y);
          // fmt::print("wx = {}, wy = {}\n", wx, wy);
          // std::cout << copy << std::endl;
          // fmt::print("tc.number_of_classes() == {}\n",
          // tc.number_of_classes()); fmt::print("copy.number_of_nodes() ==
          // {}\n", copy.number_of_nodes());
          LIBSEMIGROUPS_ASSERT(tc.number_of_classes()
                               < wg.number_of_active_nodes());
          if (tc.number_of_classes() > 1) {
            return false;
          }
        }
      }
      return true;
    }

    template <typename Iterator>
    BMat<> poset(Iterator first, Iterator last) {
      using WordGraph_ = std::decay_t<decltype(*first)>;
      std::vector<WordGraph_> graphs(first, last);
      size_t const            n = graphs.size();

      HopcroftKarp hk;

      BMat<> mat1(n, n);

      for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
          mat1(i, j) = hk.is_subrelation_no_checks(graphs[i], graphs[j]);
        }
      }

      auto   mat2 = mat1;
      auto   mat3 = mat1;
      BMat<> zero(n, n);
      auto   acc = zero;
      while (mat2 != zero) {
        mat3.product_inplace(mat2, mat1);
        std::swap(mat3, mat2);
        acc += mat2;
      }

      for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
          if (!acc(i, j) && mat1(i, j)) {
            zero(i, j) = true;
          }
        }
      }
      return zero;
    }

    // The following produces a self-contained Dot object which doesn't render
    // very well.
    template <typename Iterator>
    Dot dot_poset(Iterator first, Iterator last) {
      auto mat = poset(first, last);
      auto n   = mat.number_of_rows();

      Dot result;

      result.kind(Dot::Kind::digraph)
          .add_attr("node [shape=\"box\"]")
          .add_attr("rankdir=\"BT\"")
          .add_attr("compound=true");
      size_t index = 0;
      for (auto it = first; it != last; ++it) {
        auto copy = *it;
        copy.induced_subgraph_no_checks(0, copy.number_of_active_nodes());
        Dot dot_graph = word_graph::dot(copy);
        dot_graph.name(std::to_string(index++));
        result.add_subgraph(std::move(dot_graph));
      }
      for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
          if (mat(i, j)) {
            result
                .add_edge(fmt::format("cluster_{}_0", i),
                          fmt::format("cluster_{}_0", j))
                .add_attr("minlen", 2.5)
                .add_attr("ltail", fmt::format("cluster_{}", i))
                .add_attr("lhead", fmt::format("cluster_{}", j));
          }
        }
      }
      result.add_attr("splines=line");
      return result;
    }

    template <typename Iterator>
    void dot_poset(std::string_view work_dir,
                   std::string_view fname,
                   Iterator         first,
                   Iterator         last) {
      int  index = 0;
      auto wdir  = std::filesystem::path(work_dir);
      std::filesystem::create_directories(wdir);
      fmt::print("\nUsing temporary directory {} . . .\n", wdir.c_str());

      for (auto it = first; it != last; ++it) {
        auto copy = *it;
        copy.induced_subgraph_no_checks(0, copy.number_of_active_nodes());
        Dot         dot_graph = word_graph::dot(copy);
        std::string name      = std::to_string(index);
        dot_graph.name(name);
        wdir /= fmt::format("{}.gv", index++);
        std::ofstream f(wdir);
        f << dot_graph.to_string();  // TODO should be f << dot_graph;
        wdir.remove_filename();
      }

      auto mat = poset(first, last);
      auto n   = mat.number_of_rows();

      Dot result;

      result.kind(Dot::Kind::digraph)
          .add_attr("node [shape=\"box\"]")
          .add_attr("rankdir=\"BT\"");
      index = 0;
      for (size_t i = 0; i < n; ++i) {
        wdir /= fmt::format("{}.png", index);
        result.add_node(index)
            .add_attr("image", wdir.c_str())
            .add_attr("xlabel", std::to_string(index++))
            .add_attr("label", "__NONE__");
        wdir.remove_filename();
      }
      for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
          if (mat(i, j)) {
            result.add_edge(fmt::format("{}", i), fmt::format("{}", j))
                .add_attr("minlen", "2.5");
          }
        }
      }
      {
        std::ofstream f(std::string(fname) + ".gv");
        f << result.to_string();
      }
      int code = 0;
      while (--index >= 0 && code == 0) {
        wdir /= fmt::format("{}", index);
        auto cmd = fmt::format("dot -Tpng {0}.gv > {0}.png", wdir.c_str());
        code     = std::system(cmd.c_str());
        wdir.remove_filename();
      }
      // TODO handle code if not 0
      std::system(fmt::format("dot -Tpng {0}.gv > {0}.png", fname).c_str());
    }

    template <typename Iterator>
    void dot_poset(std::string_view fname, Iterator first, Iterator last) {
      auto tmp_dir = std::filesystem::temp_directory_path() / "libsemigroups";
      dot_poset(tmp_dir.c_str(), fname, first, last);
    }
  }  // namespace sims

  // FIXME: RC writes:
  // Second: I am writing a proof for the method of checking if a right
  // congruence is a right Rees congruence (i.e. right congruence corresponding
  // to quotient by right ideal), and there is an issue.

  // The condition we discussed in the meeting way back when was:
  // a) Check there is a unique sink
  // b) Check that no extra relations hold for paths leading to non-sink nodes.
  //
  // These conditions are both sufficient and necessary for two-sided
  // congruences, I think. However, to be uniform with our handling of right and
  // two-sided congruences, I want to tackle the right Rees congruences first.
  // But in the one-sided case they are not enough!
  //
  // Indeed, consider the f.p. monoid M=Mon<a, b | ba=b, b^2=b>. The
  // right-Cayley graph of M has infinitely many sinks b, ab, a^2b ... .
  // Furthermore, for each $n\in\mathbb{N}$ the quotient by the right ideal a^nM
  // has n+1 sinks. E.g. for n=1 we get the graph [[1, 2], [1, 1], [2, 2]]. So
  // we have a bit of a problem here. Somehow the sink at a^n is the "real sink"
  // and the others are "impostor sinks", which we should be suspicious of.
  //
  // I think a way of patching this would be to require that among all sinks
  // there is a unique sink, call it $\omega$, that has at least one letter
  // $a\in A$ and vertex $\alpha in V$ (alpha can be equal to omega as well
  // (something something biblical joke something something)) such that
  // $(\alpha, a, \omega)\in E$ and $w_\alpha a != w_\omega$ in $M$. This
  // guarantees that $\omega$ occurs as a result of a genuine quotienting and is
  // not implied by the relations. However where this does fail is on the
  // trivial congruence if that is a right Rees congruence (i.e. if we are
  // finite and have a right zero). E.g. in Mon<a, b | ba=b, b^2 = b, ab = a,
  // a^2 = a> we have two sinks a, b and e.g. aM = a, so M/aM = M. In this case
  // no quotienting occurs. To include this case we could change the above
  // condition so that there is AT MOST ONE sink `\omega` such that there is an
  // edge $(\alpha, a, \omega)\in E$ with $w_\alpha a \neq w_\omega$ in $M$. The
  // proof then considers the cases when we have the distinguished sink and when
  // we do not (establishing that the relation is trivial if this is not the
  // case).
  //
  // We can then mention the improvement of only needing to check if there is a
  // unique sink in the two sided case.
  //
  // When I originally was starting to write this I wasn't sure of how to
  // resolve the issue but I think in the process of writing it out I have the
  // fix. So Im mostly letting you know why the original idea didn't work in the
  // one sided case.
  class SimsRefinerIdeals {
   private:
    using node_type = uint32_t;
    KnuthBendix<> _knuth_bendix;

   public:
    explicit SimsRefinerIdeals(Presentation<std::string> const& p)
        : _knuth_bendix(congruence_kind::twosided, p) {
      // Here
    }

    explicit SimsRefinerIdeals(Presentation<word_type> const& p)
        : _knuth_bendix(congruence_kind::twosided,
                        to_presentation<std::string>(p)) {
      // Here
    }

    bool operator()(Sims1::word_graph_type const& wg) {
      using sims::right_generating_pairs_no_checks;
      _knuth_bendix.run();

      node_type sink = UNDEFINED;

      for (auto const& p : right_generating_pairs_no_checks(wg)) {
        auto const& u = p.first;
        auto const& v = p.second;
        if (!_knuth_bendix.contains(u, v)) {
          auto beta
              = word_graph::follow_path_no_checks(wg, 0, u.cbegin(), u.cend());
          if (sink == UNDEFINED) {
            sink = beta;
          } else if (sink != beta) {
            return false;
          }
        }
      }
      if (sink != UNDEFINED) {
        for (auto [a, t] : wg.labels_and_targets_no_checks(sink)) {
          if (t != UNDEFINED && t != sink) {
            return false;
          }
        }
      }
      return true;
    }

    // TODO
    // Check if the incoming word graph has any generating pairs (via the
    // functionality below) that are not equal in the underlying semigroup.
    // Require us to store either a KnuthBendix or Kambites object in this
    // using a presentation (also requires that the word problem is decidable
    // in the underlying semigroup).
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SIMS_HPP_
