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
// congruence" algorithm for semigroups and monoids.
// TODO(Sims1):
// * implement joins (HopcroftKarp), meets (not sure), containment (find join
//   and check equality)?
// * generating pairs for congruences defined by "action digraph"?
// * is 2-sided congruence method. One approach would be to compute the kernel
//   of the associated homomorphism, which is the largest 2-sided congruence
//   contained in the right congruence. Not sure if this is a good approach.
// * generally fix reporting
// * be useful to have output when no congruences are found too (i.e. in
//   Heineken group example). Can't really think of a nice way of doing this at
//   present in 2022.
// * the stats() object returned by Sims1 seems to not be populated
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
#include <iostream>    // for ostream
#include <iterator>    // for forward_iterator_tag
#include <mutex>       // for mutex
#include <string>      // for operator+, basic_string
#include <thread>      // for thread, yield
#include <utility>     // for move
#include <vector>      // for vector

#include "config.hpp"           // for LIBSEMIGROUPS_ENABLE_STATS
#include "constants.hpp"        // for UNDEFINED
#include "debug.hpp"            // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"        // for LIBSEMIGROUPS_EXCEPTION
#include "felsch-graph.hpp"     // for FelschGraph
#include "froidure-pin.hpp"     // for FroidurePin
#include "presentation.hpp"     // for Presentation, Presentati...
#include "to-froidure-pin.hpp"  // for make
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
  //! On this page we describe the `Sims1Stats` struct. The purpose of this
  //! class is to collect some statistics related to `Sims1` class template.
  //!
  //! \sa \ref Sims1
  struct Sims1Stats {
    // TODO doc
    uint64_t count_last;  // TODO atomic
    // TODO doc
    uint64_t count_now;  // TODO atomic
    //! The maximum number of pending definitions.
    //!
    //! A *pending definition* is just an edge that will be defined at some
    //! point in the future in the WordGraph represented by a Sims1
    //! instance at any given moment.
    //!
    //! This member tracks the maximum number of such pending definitions that
    //! occur during the running of the algorithms in Sims1.
    uint64_t max_pending = 0;  // TODO atomic

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
    uint64_t total_pending = 0;  // TODO atomic

    Sims1Stats() : count_last(), count_now(), max_pending(), total_pending() {
      zero_stats();
    }

    Sims1Stats(Sims1Stats const& that)            = default;
    Sims1Stats& operator=(Sims1Stats const& that) = default;
    // TODO the move constructors

    //! Combine two Sims1Stats objects
    //!
    //! This function changes this Sims1Stats in-place so that \ref max_pending
    //! is the maximum of `this->max_pending` and `that.max_pending`; and \ref
    //! total_pending is the sum of `this->total_pending` and
    //! `that.total_pending`.
    Sims1Stats& operator+=(Sims1Stats const& that) {
      count_last += that.count_last;
      count_now += that.count_now;
      max_pending = std::max(max_pending, that.max_pending);
      total_pending += that.total_pending;
      return *this;
    }

    Sims1Stats& zero_stats() {
      max_pending   = 0;
      total_pending = 0;
      count_last    = 0;
      count_now     = 0;
      return *this;
    }
  };

  //! No doc
  // This class allows us to use the same interface for settings for Sims1,
  // RepOrc, and MinimalRepOrc without duplicating the code.
  template <typename Subclass>
  class Sims1Settings {
   private:
    // TODO add _exclude
    // TODO make it just a vector of word_type
    Presentation<word_type> _include;
    // TODO change to iterator into _shorts, and rename
    // shorts to presentation
    Presentation<word_type> _longs;
    size_t                  _num_threads;
    size_t                  _report_interval;
    Presentation<word_type> _shorts;
    // TODO remove this from here put it into RepOrc and MinimalRepOrc, since I
    // think it's unused in Sims1 itself?
    mutable Sims1Stats _stats;

   public:
    //! No doc
    Sims1Settings();

    //! No doc
    template <typename S>
    Sims1Settings(Sims1Settings<S> const& that);

    //! Returns the settings object of *this.
    //!
    //! The settings object contains all the settings that are common to
    //! `Sims1`, `RepOrc`, and `MinimalRepOrc`, which are currently:
    //! * \ref short_rules
    //! * \ref long_rules
    //! * \ref report_interval
    //! * \ref number_of_threads
    //! * \ref extra
    //!
    //! The return value of this function can be used to
    //! initialise another `Sims1`, `RepOrc`, or
    //! `MinimalRepOrc` with these settings.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns A const reference to `Sims1Settings`.
    //!
    //! \exceptions
    //! \noexcept
    // So that we can access the settings from the derived class T.
    Sims1Settings const& settings() const noexcept {
      return *this;
    }

    //! Copy the settings from \p that into `this`.
    //!
    //! The settings object contains all the settings that are common to
    //! `Sims1`, `RepOrc`, and `MinimalRepOrc`, which are currently:
    //! * \ref short_rules
    //! * \ref long_rules
    //! * \ref report_interval
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
    Subclass& settings(Sims1Settings const& that) {
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
    size_t number_of_threads() const noexcept {
      return _num_threads;
    }

    //! \anchor report_interval
    //! Set the report interval.
    //!
    //! Only report after \p val new congruences have been
    //! found.
    //!
    //! The default value is `1999`.
    //!
    //! \param val the new value for the report interval.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    // TODO Remove
    Subclass& report_interval(size_t val) noexcept {
      _report_interval = val;
      return static_cast<Subclass&>(*this);
    }

    //! Returns the current report interval.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns
    //! A `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    // TODO remove
    size_t report_interval() const noexcept {
      return _report_interval;
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
    template <typename P>
    Subclass& short_rules(P const& p);

    //! \anchor short_rules
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
    //! have been moved from \ref short_rules to \ref
    //! long_rules.
    Presentation<word_type> const& short_rules() const noexcept {
      return _shorts;
    }

    Presentation<word_type> const& presentation() const noexcept {
      return short_rules();
    }

    //! Set the long rules.
    //!
    //! These are the rules used after a complete
    //! deterministic word graph compatible with \ref
    //! short_rules has been found by `Sims1`. If such a
    //! word graph is compatible with the long rules
    //! specified by this function, then this word graph is
    //! accepted, and if not it is not accepted.
    //!
    //! If the template parameter \p P is not
    //! `Presentation<word_type>`, then the parameter \p p
    //! is first converted to a value of type
    //! `Presentation<word_type>` and it is this converted
    //! value that is used.
    //!
    //! \tparam P A specific value of the class template
    //! `Presentation`, must be derived from
    //! `PresentationBase`. \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if
    //! `to_presentation<word_type>(p)` throws \throws
    //! LibsemigroupsException if `p` is not valid \throws
    //! LibsemigroupsException if the alphabet of `p` is
    //! non-empty and not equal to that of \ref short_rules
    //! or \ref extra.
    template <typename P>
    Subclass& long_rules(P const& p);

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
    Presentation<word_type> const& long_rules() const noexcept {
      return _longs;
    }

    //! \anchor extra
    //! Returns a const reference to the additional defining pairs.
    //!
    //! The congruences computed by a Sims1 instance always contain the
    //! relations of this presentation. In other words, the congruences
    //! computed by this instance are only taken among those that contains the
    //! pairs of elements of the underlying semigroup (defined by the
    //! presentation returned by \ref short_rules and \ref long_rules)
    //! represented by the relations of the presentation returned by `extra()`.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns
    //! A const reference to `Presentation<word_type>`.
    //!
    //! \exceptions
    //! \noexcept
    Presentation<word_type> const& include() const noexcept {
      return _include;
    }

    //! Set the extra rules.
    //!
    //! The congruences computed by a `Sims1` instance always contain the
    //! relations of this presentation. In other words, the congruences
    //! computed by this instance are only taken among those that contains the
    //! pairs of elements of the underlying semigroup (defined by the
    //! presentation returned by \ref short_rules and \ref long_rules)
    //! represented by the relations of the presentation returned by `extra()`.
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
    //! equal to that of \ref short_rules or \ref long_rules.
    template <typename P>
    Subclass& extra(P const& p);

    //! Returns a const reference to the current stats
    //! object.
    //!
    //! The value returned by this function is a `Sims1Stats` object which
    //! contains some statistics related to the current `Sims1` instance and
    //! any part of the depth first search already conducted. If reporting is
    //! disabled, then no statistics are collected. FIXME
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns
    //! A const reference to `Sims1Stats`.
    //!
    //! \exceptions
    //! \noexcept
    Sims1Stats& stats() const noexcept {
      return _stats;
    }

   protected:
    Subclass const& stats(Sims1Stats const& stts) const {
      _stats = std::move(stts);
      return static_cast<Subclass const&>(*this);
    }

   public:
    //! \anchor long_rule_length
    //! Define the long rule length.
    //!
    //! This function modifies \ref short_rules and \ref long_rules so that
    //! \ref short_rules only contains those rules whose length (sum of the
    //! lengths of the two sides of the rules) is less than \p val (if any) and
    //! \ref long_rules only contains those rules of length at least \p val (if
    //! any). The rules contained in the union of \ref short_rules and \ref
    //! long_rules is invariant under this function, but the distribution of
    //! the rules between \ref short_rules and \ref long_rules is not.
    //!
    //! The relative orders of the rules within \ref short_rules and \ref
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

    //! \anchor split_at
    //! Split the rules in \ref short_rules and \ref
    //! long_rules.
    //!
    //! This function splits the relations in \ref short_rules and \ref
    //! long_rules so that \ref short_rules contains the first `2 * val` rules
    //! and \ref long_rules contains any remaining rules.
    //!
    //! The order of the relations is the same as the current order.
    //!
    //! \param val the relation to split at.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p val is out of
    //! bounds, i.e. if it exceeds
    //! `(short_rules().rules.size() +
    //! long_rules().rules.size()) / 2`.
    Sims1Settings& split_at(size_t val);

   protected:
    // TODO remove
    void validate_presentation(Presentation<word_type> const& arg,
                               Presentation<word_type> const& existing);
  };

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
  class Sims1 : public Sims1Settings<Sims1>, public ReporterV3 {
   public:
    //! Type for the nodes in the associated WordGraph
    //! objects.
    using node_type  = uint32_t;
    using label_type = typename WordGraph<node_type>::label_type;

    //! Type for letters in the underlying presentation.
    using letter_type = typename word_type::value_type;

    //! The size_type of the associated WordGraph objects.
    using size_type = typename WordGraph<node_type>::size_type;

    // We use WordGraph, even though the iterators produced by this class hold
    // FelschGraph's, none of the features of FelschGraph are useful for the
    // output, only for the implementation
    //! The type of the associated WordGraph objects.
    using digraph_type = WordGraph<node_type>;

   private:
    congruence_kind _kind;

    using Sims1Settings<Sims1>::validate_presentation;

   public:
    // TODO remove? check if same as other classes
    template <typename W>
    Sims1(congruence_kind ck, Presentation<W> p) : Sims1(ck) {
      short_rules(p);
    }

    //! Construct from \ref congruence_kind.
    //!
    //! \param ck the handedness of the congruences (left or right)
    //!
    //! \throws LibsemigroupsException if \p ck is \ref
    //! congruence_kind::twosided
    //!
    //! \sa \ref cbegin and \ref cend.
    explicit Sims1(congruence_kind ck);

    //! Default constructor - deleted!
    Sims1() = delete;

    //! Default copy constructor.
    Sims1(Sims1 const&) = default;

    //! Default move constructor.
    Sims1(Sims1&&) = default;

    //! Default copy assignment operator.
    Sims1& operator=(Sims1 const&) = default;

    //! Default move assignment operator.
    Sims1& operator=(Sims1&&) = default;

    // No doc
    ~Sims1();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    template <typename P>
    Sims1& short_rules(P const& p) {
      if (_kind == congruence_kind::left) {
        P q(p);
        presentation::reverse(q);
        return Sims1Settings<Sims1>::short_rules(q);
      } else {
        return Sims1Settings<Sims1>::short_rules(p);
      }
    }

    template <typename P>
    Sims1& long_rules(P const& p) {
      if (_kind == congruence_kind::left) {
        P q(p);
        presentation::reverse(q);
        return Sims1Settings<Sims1>::long_rules(q);
      } else {
        return Sims1Settings<Sims1>::long_rules(p);
      }
    }

    template <typename P>
    Sims1& extra(P const& p) {
      if (_kind == congruence_kind::left) {
        P q(p);
        presentation::reverse(q);
        return Sims1Settings<Sims1>::extra(q);
      } else {
        return Sims1Settings<Sims1>::extra(p);
      }
    }
#endif

    using Sims1Settings<Sims1>::short_rules;
    using Sims1Settings<Sims1>::long_rules;
    using Sims1Settings<Sims1>::extra;
    using Sims1Settings<Sims1>::number_of_threads;

    class iterator;  // forward decl

    //! Returns a forward iterator pointing at the first congruence.
    //!
    //! Returns a forward iterator pointing to the WordGraph representing the
    //! first congruence described by Sims1 object with at most \p n classes.
    //!
    //! If incremented, the iterator will point to the next such congruence.
    //! The order which the congruences are returned in is implementation
    //! specific. Iterators of the type returned by this function are equal
    //! whenever they point to equal objects. The iterator is exhausted if and
    //! only if it points to an WordGraph with zero nodes.
    //!
    //! The meaning of the WordGraph pointed at by Sims1 iterators depends on
    //! whether the input is a monoid presentation (i.e.
    //! Presentation::contains_empty_word() returns \c true) or a semigroup
    //! presentation. If the input is a monoid presentation for a monoid
    //! \f$M\f$, then the WordGraph pointed to by an iterator of this type has
    //! precisely \p n nodes, and the right action of \f$M\f$ on the nodes of
    //! the digraph is isomorphic to the action of \f$M\f$ on the classes of a
    //! right congruence.
    //!
    //! If the input is a semigroup presentation for a semigroup \f$S\f$, then
    //! the WordGraph has \p n + 1 nodes, and the right action of \f$S\f$ on
    //! the nodes \f$\{1, \ldots, n\}\f$ of the WordGraph is isomorphic to the
    //! action of \f$S\f$ on the classes of a right congruence. It'd probably
    //! be better in this case if node \f$0\f$ was not included in the output
    //! WordGraph, but it is required in the implementation of the low-index
    //! congruence algorithm, and to avoid unnecessary copies, we've left it in
    //! for the time being. \param n the maximum number of classes in a
    //! congruence.
    //!
    //! \returns
    //! An iterator \c it of type \c iterator pointing to an WordGraph with at
    //! most \p n nodes.
    //!
    //! \throws LibsemigroupsException if \p n is \c 0.
    //! \throws LibsemigroupsException if `short_rules()`
    //! has 0-generators and 0-relations (i.e. it has not
    //! been initialised).
    //!
    //! \warning
    //! Copying iterators of this type is expensive.  As a consequence, prefix
    //! incrementing \c ++it the returned  iterator \c it significantly cheaper
    //! than postfix incrementing \c it++.
    //!
    //! \sa
    //! \ref cend
    // TODO(Sims1) it'd be good to remove node 0 to avoid confusion. This seems
    // complicated however, and so isn't done at present.
    iterator cbegin(size_type n) const;

    //! Returns a forward iterator pointing one beyond the last congruence.
    //!
    //! Returns a forward iterator pointing to the empty WordGraph. If
    //! incremented, the returned iterator remains valid and continues to point
    //! at the empty WordGraph.
    //!
    //! \param n the maximum number of classes in a
    //! congruence.
    //!
    //! \returns
    //! An iterator \c it of type \c iterator pointing to an WordGraph with at
    //! most \p 0 nodes.
    //!
    //! \throws LibsemigroupsException if \p n is \c 0.
    //! \throws LibsemigroupsException if `short_rules()` has 0-generators and
    //! 0-relations (i.e. it has not been initialised).
    //!
    //! \warning
    //! Copying iterators of this type is expensive.  As a consequence, prefix
    //! incrementing \c ++it the returned  iterator \c it significantly cheaper
    //! than postfix incrementing \c it++.
    //!
    //! \sa
    //! \ref cbegin
    iterator cend(size_type n) const;

    //! Returns the number of one-sided congruences with up to a given number
    //! of classes.
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
    //! \throws LibsemigroupsException if `short_rules()` has 0-generators and
    //! 0-relations (i.e. it has not been initialised).
    uint64_t number_of_congruences(size_type n) const;

    //! Apply the function \p pred to every one-sided
    //! congruence with at most \p n classes
    //!
    //! This function is similar to
    //! `std::for_each(begin(n), end(n), pred)` and exists
    //! to:
    //! * provide some feedback on the progress of the
    //! computation if it runs for more than 1 second.
    //! * allow for the computation of
    //! `std::for_each(begin(n), end(n), pred)` to be
    //! performed using \ref number_of_threads in parallel.
    //!
    //! \param n the maximum number of congruence classes.
    //! \param pred the predicate applied to every
    //! congruence found.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p n is \c 0.
    //! \throws LibsemigroupsException if `short_rules()`
    //! has 0-generators and 0-relations (i.e. it has not
    //! been initialised).
    void for_each(size_type                                n,
                  std::function<void(digraph_type const&)> pred) const;

    //! Apply the function \p pred to every one-sided
    //! congruence with at most \p n classes, until it
    //! returns \c true.
    //!
    //! This function is similar to `std::find_if(begin(n),
    //! end(n), pred)` and exists to:
    //! * provide some feedback on the progress of the
    //! computation if it runs for more than 1 second.
    //! * allow for the computation of
    //! `std::find_if(begin(n), end(n), pred)` to be
    //! performed using \ref number_of_threads in parallel.
    //!
    //! \param n the maximum number of congruence classes.
    //! \param pred the predicate applied to every
    //! congruence found.
    //!
    //! \returns The first congruence whose WordGraph for
    //! which \p pred returns \c true.
    //!
    //! \throws LibsemigroupsException if \p n is \c 0.
    //! \throws LibsemigroupsException if `short_rules()`
    //! has 0-generators and 0-relations (i.e. it has not
    //! been initialised).
    digraph_type find_if(size_type                                n,
                         std::function<bool(digraph_type const&)> pred) const;

   private:
    // This class collects some common aspects of the iterator and
    // thread_iterator nested classes. The mutex does nothing for <iterator>
    // and is an actual std::mutex for <thread_iterator>.
    struct PendingDef;

    class iterator_base {
     public:
      //! No doc
      using const_reference =
          typename std::vector<digraph_type>::const_reference;

      //! No doc
      using const_pointer = typename std::vector<digraph_type>::const_pointer;

     private:
      size_type _max_num_classes;
      size_type _min_target_node;

     protected:
      // TODO ensure that _felsch_graph's settings are
      // properly initialised
      using Definition = std::pair<node_type, label_type>;

      FelschGraph<word_type, node_type, std::vector<Definition>> _felsch_graph;

      // This mutex does nothing for iterator, only does
      // something for thread_iterator
      std::mutex              _mtx;
      std::vector<PendingDef> _pending;
      Sims1 const*            _sims1;

      // Push initial PendingDef's into _pending, see tpp
      // file for explanation.
      void init(size_type n);

      // We could use the copy constructor, but there's no
      // point in copying anything except the FelschGraph
      // and so we only copy that.
      void copy_felsch_graph(iterator_base const& that) {
        _felsch_graph = that._felsch_graph;
      }

      // Try to make the definition represented by
      // PendingDef, returns false if it wasn't possible,
      // and true if it was.
      //! No doc
      bool try_define(PendingDef const&);

      // Try to pop from _pending into the argument
      // (reference), returns true if successful and false
      // if not.
      bool try_pop(PendingDef&);

     public:
      //! No doc
      iterator_base(Sims1 const* s, size_type n);

      // None of the constructors are noexcept because the
      // corresponding constructors for Presentation aren't
      // currently

      //! No doc
      iterator_base() = default;

      // Intentionally don't copy the mutex, it doesn't
      // compile, wouldn't make sense if the mutex was used
      // here.
      //! No doc
      iterator_base(iterator_base const& that);

      // Intentionally don't copy the mutex, it doesn't
      // compile, wouldn't make sense if the mutex was used
      // here.
      //! No doc
      iterator_base(iterator_base&& that);

      // Intentionally don't copy the mutex, it doesn't
      // compile, wouldn't make sense if the mutex was used
      // here.
      //! No doc
      iterator_base& operator=(iterator_base const& that);

      // Intentionally don't copy the mutex, it doesn't
      // compile, wouldn't make sense if the mutex was used
      // here.
      //! No doc
      iterator_base& operator=(iterator_base&& that);

      //! No doc
      ~iterator_base();

      //! No doc
      bool operator==(iterator_base const& that) const noexcept {
        return _felsch_graph == that._felsch_graph;
      }

      //! No doc
      bool operator!=(iterator_base const& that) const noexcept {
        return !(operator==(that));
      }

      //! No doc
      const_reference operator*() const noexcept {
        return _felsch_graph;
      }

      //! No doc
      const_pointer operator->() const noexcept {
        return &_felsch_graph;
      }

      //! No doc
      void swap(iterator_base& that) noexcept;

      //! No doc
      Sims1Stats& stats() noexcept {
        return _sims1->stats();
      }
    };  // class iterator_base

   public:
    //! The return type of \ref cbegin and \ref cend.
    //!
    //! This is a forward iterator values of this type are expensive to copy
    //! due to their internal state, and prefix increment should be preferred
    //! to postfix.
    class iterator : public iterator_base {
      using iterator_base::init;
      using iterator_base::try_define;
      using iterator_base::try_pop;

     public:
      //! No doc
      using const_pointer = typename iterator_base::const_pointer;
      //! No doc
      using const_reference = typename iterator_base::const_reference;

      //! No doc
      using size_type = typename std::vector<digraph_type>::size_type;
      //! No doc
      using difference_type =
          typename std::vector<digraph_type>::difference_type;
      //! No doc
      using pointer = typename std::vector<digraph_type>::pointer;
      //! No doc
      using reference = typename std::vector<digraph_type>::reference;
      //! No doc
      using value_type = digraph_type;
      //! No doc
      using iterator_category = std::forward_iterator_tag;

      //! No doc
      using iterator_base::iterator_base;

     private:
      // Only want Sims1 to be able to use this constructor.
      iterator(Sims1 const& s, size_type n);

      // So that we can use the constructor above.
      friend iterator Sims1::cbegin(Sims1::size_type) const;
      friend iterator Sims1::cend(Sims1::size_type) const;

     public:
      //! No doc
      ~iterator() = default;

      // prefix
      //! No doc
      iterator const& operator++();

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
    using time_point = std::chrono::high_resolution_clock::time_point;

    static void report_at_start(Presentation<word_type> const& shorts,
                                Presentation<word_type> const& longs,
                                size_t                         num_classes,
                                size_t                         num_threads);

    template <typename S>
    static void report_number_of_congruences(uint64_t    report_interval,
                                             time_point& start_time,
                                             time_point& last_report,
                                             S&          last_count,
                                             uint64_t    count_now,
                                             std::mutex& mtx);

    void report_progress_from_thread() const override;

    void report_stats() const;

    static void final_report_number_of_congruences(time_point& start_time,
                                                   uint64_t    count);

    // Nested classes
    class thread_iterator;
    class thread_runner;
  };

  std::ostream& operator<<(std::ostream& os, Sims1Stats const& stats);

  //! Defined in ``sims1.hpp``.
  //!
  //! This class is a helper for `Sims1` calling the `digraph` member function
  //! attempts to find a right congruence, represented as an WordGraph, of
  //! the semigroup or monoid defined by the presentation consisting of its
  //! \ref short_rules and \ref long_rules with the following properties:
  //! * the transformation semigroup defined by the WordGraph has size
  //!   \ref target_size;
  //! * the number of nodes in the WordGraph is at least \ref min_nodes
  //!   and at most \ref max_nodes.
  //!
  //! If no such WordGraph can be found, then an empty WordGraph is
  //! returned (with `0` nodes and `0` edges).
  class RepOrc : public Sims1Settings<RepOrc> {
   private:
    size_t _min;
    size_t _max;
    size_t _size;

   public:
    //! Default constructor.
    RepOrc() : _min(0), _max(0), _size(0) {}

    //! Construct from Sims1 or MinimalRepOrc.
    //!
    //! This constructor creates a new RepOrc instance with
    //! the same Sims1Settings as \p s but that is
    //! otherwise uninitialised.
    //!
    //! \tparam S the type of the argument \p s (which is
    //! derived from `Sims1Settings<S>`).
    //!
    //! \param s the Sims1 or MinimalRepOrc whose settings
    //! should be used.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename S>
    explicit RepOrc(Sims1Settings<S> const& s)
        : Sims1Settings<RepOrc>(s), _min(0), _max(0), _size(0) {}

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
    //! This function returns the current value for the
    //! minimum number of nodes in the WordGraph that we
    //! are seeking.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    size_t min_nodes() const noexcept {
      return _min;
    }

    //! Set the maximum number of nodes.
    //!
    //! This function sets the maximum number of nodes in
    //! the WordGraph that we are seeking.
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
    //! This function returns the current value for the
    //! maximum number of nodes in the WordGraph that we
    //! are seeking.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    size_t max_nodes() const noexcept {
      return _max;
    }

    //! Set the target size.
    //!
    //! This function sets the target size, i.e. the
    //! desired size of the transformation semigroup
    //! corresponding to the WordGraph returned by the
    //! function \ref digraph.
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
    //! This function returns the current value for the
    //! target size, i.e. the desired size of the
    //! transformation semigroup corresponding to the
    //! WordGraph returned by the function \ref digraph.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    size_t target_size() const noexcept {
      return _size;
    }

    //! Get the digraph.
    //!
    //! This function attempts to find a right congruence,
    //! represented as an WordGraph, of the semigroup or
    //! monoid defined by the presentation consisting of
    //! its \ref short_rules and \ref long_rules with the
    //! following properties:
    //! * the transformation semigroup defined by the
    //! WordGraph has size \ref target_size;
    //! * the number of nodes in the WordGraph is at least
    //! \ref min_nodes
    //!   and at most \ref max_nodes.
    //!
    //! If no such WordGraph can be found, then an empty
    //! WordGraph is returned (with `0` nodes and `0`
    //! edges).
    //!
    //! \warning The return value of this function is
    //! recomputed every time it is called.
    //!
    //! \warning If the return value of \ref
    //! number_of_threads is greater than \c 1, then the
    //! value returned by this function is
    //! non-deterministic, and may vary even for the same
    //! parameters.
    //!
    //! \tparam T the type of the nodes in the returned
    //! digraph. \param (None) this function has no
    //! parameters.
    //!
    //! \returns A value of type `WordGraph`.
    //!
    //! \exceptions \no_libsemigroups_except
    Sims1::digraph_type digraph() const;

    using Sims1Settings<RepOrc>::short_rules;
    using Sims1Settings<RepOrc>::long_rules;
  };

  //! Defined in ``sims1.hpp``.
  //!
  //! This class is a helper for `Sims1`, calling the `digraph` member
  //! function attempts to find a right congruence, represented as an
  //! WordGraph, with the minimum possible number of nodes such that the
  //! action of the semigroup or monoid defined by the presentation consisting
  //! of its \ref short_rules and \ref long_rules on the nodes of the
  //! WordGraph corresponds to a semigroup of size \ref target_size.
  //!
  //! If no such WordGraph can be found, then an empty WordGraph is
  //! returned (with `0` nodes and `0` edges).
  class MinimalRepOrc : public Sims1Settings<MinimalRepOrc> {
   private:
    size_t _size;

   public:
    using Sims1Settings<MinimalRepOrc>::stats;

    //! Default constructor
    MinimalRepOrc() = default;

    //! Set the target size.
    //!
    //! This function sets the target size, i.e. the
    //! desired size of the transformation semigroup
    //! corresponding to the WordGraph returned by the
    //! function \ref digraph.
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
    //! This function returns the current value for the
    //! target size, i.e. the desired size of the
    //! transformation semigroup corresponding to the
    //! WordGraph returned by the function \ref digraph.
    //!
    //! \param (None) this function has no parameters.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    size_t target_size() const noexcept {
      return _size;
    }

    //! Get the digraph.
    //!
    //! This function attempts to find a right congruence, represented as an
    //! WordGraph, with the minimum possible number of nodes such that the
    //! action of the semigroup or monoid defined by the presentation
    //! consisting of its \ref short_rules and \ref long_rules on the nodes of
    //! the WordGraph corresponds to a semigroup of size \ref target_size.
    //!
    //! If no such WordGraph can be found, then an empty WordGraph is returned
    //! (with `0` nodes and `0` edges).
    //!
    //! The algorithm implemented by this function repeatedly runs:
    //! \code RepOrc(*this)
    //!     .min_nodes(1)
    //!     .max_nodes(best)
    //!     .target_size(target_size())
    //!     .digraph();
    //! \endcode
    //! where `best` is initially \ref target_size, until the returned
    //! WordGraph is empty, and then the penultimate WordGraph is returned (if
    //! any).
    //!
    //! \warning The return value of this function is recomputed every time it
    //! is called.
    //!
    //! \warning If the return value of \ref number_of_threads is greater than
    //! \c 1, then the value returned by this function is non-deterministic,
    //! and may vary even for the same parameters.
    //!
    //! \tparam T the type of the nodes in the returned digraph. \param (None)
    //! this function has no parameters.
    //!
    //! \returns A value of type `WordGraph`.
    //!
    //! \exceptions \no_libsemigroups_except
    Sims1::digraph_type digraph() const;
  };

}  // namespace libsemigroups

#include "sims1.tpp"

#endif  // LIBSEMIGROUPS_SIMS1_HPP_
