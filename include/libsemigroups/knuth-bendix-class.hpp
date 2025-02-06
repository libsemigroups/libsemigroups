//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell + Joseph Edwards
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

// This file contains the declaration of the class template KnuthBendix which
// is really just a facade for detail::KnuthBendixBase.

// TODO(0) iwyu
// TODO(0) active rules, also anything else that returns anything related to
// the word type.

#ifndef LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_
#define LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_

#include "order.hpp"
#include "presentation.hpp"

#include "detail/knuth-bendix-base.hpp"
#include "detail/rewriters.hpp"

namespace libsemigroups {

  //! \defgroup knuth_bendix_group Knuth-Bendix
  //!
  //! This page contains links to the documentation related to the
  //! implementation of the Knuth-Bendix algorithm in `libsemigroups`.

  //! \defgroup knuth_bendix_class_group The KnuthBendix class
  //! \ingroup knuth_bendix_group
  //!
  //! \brief This page contains links to the documentation related to the
  //! KnuthBendix class.
  //!
  //! This page contains links to the documentation related to the
  //! KnuthBendix class.

  //! \ingroup knuth_bendix_class_group
  //! \hideinheritancegraph
  //!
  //! \brief Class template containing an implementation of the Knuth-Bendix
  //! Algorithm.
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! On this page we describe the functionality relating to the Knuth-Bendix
  //! algorithm for semigroups and monoids in \c libsemigroups. This page
  //! contains details of the member functions of the class KnuthBendix.
  //!
  //! This class is used to represent a [string rewriting
  //! system](https://w.wiki/9Re) defining a 1- or 2-sided congruence on a
  //! finitely presented monoid or semigroup.
  //!
  //! \tparam Word the type of the words in rules in the presentation.
  //! \tparam Rewriter the type of the rewriter.
  //! \tparam ReductionOrder the reduction ordering.
  //!
  //! \par Example
  //! \code
  //! Presentation<std::string> p;
  //! p.contains_empty_word(true);
  //! p.alphabet("abcd");
  //! presentation::add_rule_no_checks(p, "ab", "");
  //! presentation::add_rule_no_checks(p, "ba", "");
  //! presentation::add_rule_no_checks(p, "cd", "");
  //! presentation::add_rule_no_checks(p, "dc", "");
  //!
  //! KnuthBendix kb(twosided, p);
  //!
  //! !kb.confluent();              // true
  //! kb.run();
  //! kb.number_of_active_rules();  // 8
  //! kb.confluent();               // true
  //! kb.number_of_classes();       // POSITIVE_INFINITY
  //! \endcode
  template <typename Word,
            typename Rewriter       = detail::RewriteTrie,
            typename ReductionOrder = ShortLexCompare>
  class KnuthBendix : public detail::KnuthBendixBase<Rewriter, ReductionOrder> {
   private:
    using KnuthBendixBase_ = detail::KnuthBendixBase<Rewriter, ReductionOrder>;

    std::vector<Word>  _generating_pairs;
    Presentation<Word> _presentation;

   public:
#ifdef PARSED_BY_DOXYGEN
    //! \brief Struct containing various options that can be used to control
    //! the behaviour of Knuth-Bendix.
    //!
    //! This struct containing various options that can be used to control the
    //! behaviour of Knuth-Bendix.
    struct options {
      //! \brief Values for specifying how to measure the length of an
      //! overlap.
      //!
      //! The values in this enum determine how a KnuthBendixBase instance
      //! measures the length \f$d(AB, BC)\f$ of the overlap of two words
      //! \f$AB\f$ and \f$BC\f$:
      //!
      //! \sa overlap_policy(options::overlap).
      enum class overlap {
        //! \f$d(AB, BC) = |A| + |B| + |C|\f$
        ABC = 0,
        //! \f$d(AB, BC) = |AB| + |BC|\f$
        AB_BC = 1,
        //! \f$d(AB, BC) = max(|AB|, |BC|)\f$
        MAX_AB_BC = 2
      };
    };
#endif

    //! \brief Type of the letters in the relations of the presentation stored
    //! in a \ref KnuthBendix instance.
    //!
    //! Type of the letters in the relations of the presentation stored
    //! in a \ref KnuthBendix instance.
    using native_word_type = Word;
    // TODO(0) rm native_letter_type/presentation_type

    //! \brief Default constructor.
    //!
    //! This function default constructs an uninitialised \ref
    //! KnuthBendix instance.
    KnuthBendix() = default;

    //! \brief Remove the presentation and rewriter data
    //!
    //! This function clears the rewriter, presentation, settings and stats
    //! from the KnuthBendix object, putting it back into the state it
    //! would be in if it was newly default constructed.
    //!
    //! \returns
    //! A reference to \c this.
    KnuthBendix& init() {
      _generating_pairs.clear();
      _presentation.init();
      KnuthBendixBase_::init();
      return *this;
    }

    //! \brief Copy constructor.
    //!
    //! Copy constructor.
    //!
    //! \param that the KnuthBendix instance to copy.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
    //! rules of \p that.
    KnuthBendix(KnuthBendix const&) = default;

    //! \brief Move constructor.
    //!
    //! Move constructor.
    KnuthBendix(KnuthBendix&&) = default;

    //! \brief Copy assignment operator.
    //!
    //! Copy assignment operator.
    KnuthBendix& operator=(KnuthBendix const&) = default;

    //! \brief Move assignment operator.
    //!
    //! Move assignment operator.
    KnuthBendix& operator=(KnuthBendix&&) = default;

    //! \copydoc KnuthBendix(congruence_kind, Presentation<Word> const&)
    KnuthBendix(congruence_kind knd, Presentation<Word>&& p) : KnuthBendix() {
      init(knd, std::move(p));
    }

    //! \copydoc init(congruence_kind, Presentation<Word> const&)
    KnuthBendix& init(congruence_kind knd, Presentation<Word>&& p);

    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a \ref KnuthBendix instance representing
    //! a congruence of kind \p knd over the semigroup or monoid defined by
    //! the presentation \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    KnuthBendix(congruence_kind knd, Presentation<Word> const& p)
        // call the rval ref constructor
        : KnuthBendix(knd, Presentation<Word>(p)) {}

    //! \brief Re-initialize a \ref KnuthBendix instance.
    //!
    //! This function puts a \ref KnuthBendix instance back into the state
    //! that it would have been in if it had just been newly constructed from
    //! \p knd and \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    KnuthBendix& init(congruence_kind knd, Presentation<Word> const& p) {
      // call the rval ref init
      return init(knd, Presentation<Word>(p));
    }

    //! \brief Throws if any letter in a range is out of bounds.
    //!
    //! This function throws a LibsemigroupsException if any value pointed
    //! at by an iterator in the range \p first to \p last is out of bounds
    //! (i.e. does not belong to the alphabet of the \ref presentation used
    //! to construct the \ref KnuthBendix instance).
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the
    //! word.
    //!
    //! \throw LibsemigroupsException if any letter in the range from \p
    //! first to \p last is out of bounds.
    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      presentation().validate_word(first, last);
    }

    //! \brief Get the generating pairs of the congruence (if any).
    //!
    //! This function returns the generating pairs of the congruence as
    //! specified by \ref add_generating_pair (if any).
    //!
    //! \returns The std::vector of generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<Word> const& generating_pairs() const noexcept {
      return _generating_pairs;
    }

    //! \brief Return the presentation used to construct or initialize a
    //! KnuthBendix object.
    //!
    //! This function returns the presentation used to construct or initialize
    //! a KnuthBendix object.
    //!
    //! \returns
    //! A const reference to the presentation.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] Presentation<Word> const& presentation() const noexcept {
      return _presentation;
    }

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented by a
    //! \ref KnuthBendix instance.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns A reference to `*this`.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    //! \warning It is assumed that \ref started returns \c false. Adding
    //! generating pairs after \ref started is not permitted (but also not
    //! checked by this function).
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    KnuthBendix& add_generating_pair_no_checks(Iterator1 first1,
                                               Iterator2 last1,
                                               Iterator3 first2,
                                               Iterator4 last2);

    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented by a
    //! \ref KnuthBendix instance.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns A reference to `*this`.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \cong_intf_throws_if_started
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    KnuthBendix& add_generating_pair(Iterator1 first1,
                                     Iterator2 last1,
                                     Iterator3 first2,
                                     Iterator4 last2) {
      // Call detail::CongruenceCommon version so that we perform bound checks
      // in KnuthBendix and not KnuthBendixBase
      return detail::CongruenceCommon::add_generating_pair<KnuthBendix>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - number_of_classes
    ////////////////////////////////////////////////////////////////////////

    //! \brief Compute the number of classes in the congruence.
    //!
    //! This function computes the number of classes in the congruence
    //! represented by a \ref KnuthBendix instance by
    //! running the congruence enumeration until it terminates.
    //!
    //! \returns The number of congruences classes of a \ref KnuthBendix
    //! instance.
    //!
    //! \cong_intf_warn_undecidable{Knuth-Bendix}
    //!
    //! \note If \c this has been run until finished, then this function can
    //! determine the number of classes of the congruence represented by \c
    //! this even if it is infinite. Moreover, the complexity of this function
    //! is at worst \f$O(mn)\f$ where \f$m\f$ is the number of letters in the
    //! alphabet, and \f$n\f$ is the number of nodes in the \ref gilman_graph.
    using KnuthBendixBase_::number_of_classes;

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to be
    //! contained in the congruence represented by a \ref KnuthBendix instance.
    //! This function performs no enumeration, so it is possible for the words
    //! to be contained in the congruence, but that this is not currently known.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) const {
      // Call detail::CongruenceCommon version so that we perform bound checks
      // in KnuthBendix and not KnuthBendixBase_
      return detail::CongruenceCommon::currently_contains<KnuthBendix>(
          first1, last1, first2, last2);
    }

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref KnuthBendix
    //! instance. This function triggers a full enumeration,
    //! which may never terminate.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_warn_undecidable{Knuth-Bendix}
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains(Iterator1 first1,
                  Iterator2 last1,
                  Iterator3 first2,
                  Iterator4 last2);

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

    //! \brief Reduce a word with no enumeration.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator \p
    //! d_first. This function triggers no enumeration. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref KnuthBendix instance. If the KnuthBendix instance is \ref finished,
    //! then the output word is a normal form for the input word. If the
    //! KnuthBendix instance is not \ref finished, then it might be that
    //! equivalent input words produce different output words.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 InputIterator1 first,
                                 InputIterator2 last) const {
      return detail::CongruenceCommon::reduce_no_run<KnuthBendix>(
          d_first, first, last);
    }

    //! \brief Reduce a word.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! KnuthBendix instance. In other words, the output word is a normal form
    //! for the input word or equivalently a canconical representative of its
    //! congruence class.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \cong_intf_warn_undecidable{Knuth-Bendix}
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last) {
      // Call detail::CongruenceCommon version so that we perform bound checks
      // in KnuthBendix and not KnuthBendixBase_
      return detail::CongruenceCommon::reduce<KnuthBendix>(
          d_first, first, last);
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - setters for optional parameters - public
    //////////////////////////////////////////////////////////////////////////

#ifdef PARSED_BY_DOXYGEN
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
    KnuthBendix& max_pending_rules(size_t val);

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
    [[nodiscard]] size_t max_pending_rules() const noexcept;

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
    KnuthBendix& check_confluence_interval(size_t val);

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
    [[nodiscard]] size_t check_confluence_interval() const noexcept;

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
    KnuthBendix& max_overlap(size_t val);

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
    [[nodiscard]] size_t max_overlap() const noexcept;

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
    KnuthBendix& max_rules(size_t val);

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
    [[nodiscard]] size_t max_rules() const noexcept;

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
    //! \sa options::overlap.
    KnuthBendix& overlap_policy(typename options::overlap val);

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
    //! \sa options::overlap.
    [[nodiscard]] typename options::overlap overlap_policy() const noexcept;

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - member functions for rules - public
    //////////////////////////////////////////////////////////////////////////

    //! \brief Return the current number of active rules in the
    //! KnuthBendix instance.
    //!
    //! This function returns the current number of active rules in the
    //! KnuthBendix instance.
    //!
    //! \returns
    //! The current number of active rules, a value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    // NOTE: this technique for including the function + doc doesn't work with
    // overloaded functions.
    using KnuthBendixBase_::number_of_active_rules;

    //! \brief Return the current number of inactive rules in the
    //! KnuthBendix instance.
    //!
    //! This function returns the current number of inactive rules in the
    //! KnuthBendix instance.
    //!
    //! \returns
    //! The current number of inactive rules, a value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    using KnuthBendixBase_::number_of_inactive_rules;

    //! \brief Return the number of rules that KnuthBendix has created
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
    using KnuthBendixBase_::total_rules;

    //! \brief Return a range object containing the active rules.
    //!
    //! This function returns a range object containing the pairs of
    //! strings which represent the rules of a KnuthBendix instance. The
    //! \c first entry in every such pair is greater than the \c second
    //! according to the reduction ordering of the KnuthBendix instance.
    //!
    //! \returns
    //! A range object containing the current active rules.
    // TODO(0) renovate currently always returns strings
    using KnuthBendixBase_::active_rules;

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - main member functions - public
    //////////////////////////////////////////////////////////////////////////

    //! \brief Check confluence of the current rules.
    //!
    //! Check confluence of the current rules.
    //!
    //! \returns \c true if the KnuthBendix instance is
    //! [confluent](https://w.wiki/9DA) and \c false if it is not.
    using KnuthBendixBase_::confluent;

    //! \brief Check if the current system knows the state of confluence of
    //! the current rules.
    //!
    //! Check if the current system knows the state of confluence of the
    //! current rules.
    //!
    //! \returns \c true if the confluence of the rules in the KnuthBendix
    //! instance is known, and \c false if it is not.
    using KnuthBendixBase_::confluence_known;

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
    //! \returns A const reference to a \ref
    //! WordGraph.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This will terminate when the KnuthBendix instance is
    //! reduced and confluent, which might be never.
    //!
    //! \sa \ref number_of_classes, and \ref knuth_bendix::normal_forms.
    using KnuthBendixBase_::gilman_graph;

    //! \brief Return the node labels of the Gilman \ref WordGraph
    //!
    //! This function returns the node labels of the Gilman \ref WordGraph,
    //! corresponding to the unique prefixes of the left-hand sides of the
    //! rules of the rewriting system.
    //!
    //! \return The node labels of the Gilman \ref WordGraph, a const
    //! reference to a `std::vector<std::string>`.
    //!
    //! \sa \ref gilman_graph.
    // TODO(0) renovate currently always returns labels that are strings
    using KnuthBendixBase_::gilman_graph_node_labels;
#endif
  };  // class KnuthBendix

  //! \ingroup knuth_bendix_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `knuth-bendix-class.hpp`.
  //!
  //! Deduction guide to construct a `KnuthBendix<Word>` from a
  //! `Presentation<Word>` const reference.
  template <typename Word>
  KnuthBendix(congruence_kind, Presentation<Word> const&) -> KnuthBendix<Word>;

  //! \ingroup knuth_bendix_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `knuth-bendix-class.hpp`.
  //!
  //! Deduction guide to construct a `KnuthBendix<Word>` from a
  //!
  //! `Presentation<Word>` rvalue reference.
  template <typename Word>
  KnuthBendix(congruence_kind, Presentation<Word>&&) -> KnuthBendix<Word>;

  //! \ingroup knuth_bendix_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `knuth-bendix-class.hpp`.
  //!
  //! Deduction guide to construct a `KnuthBendix<Word>` from a
  //! `KnuthBendix<Word>` const reference.
  template <typename Word>
  KnuthBendix(KnuthBendix<Word> const&) -> KnuthBendix<Word>;

  //! \ingroup knuth_bendix_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `knuth-bendix-class.hpp`.
  //!
  //! Deduction guide to construct a `KnuthBendix<Word>` from a
  //! `KnuthBendix<Word>` rvalue reference.
  template <typename Word>
  KnuthBendix(KnuthBendix<Word>&&) -> KnuthBendix<Word>;

}  // namespace libsemigroups

#include "knuth-bendix-class.tpp"
#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_
