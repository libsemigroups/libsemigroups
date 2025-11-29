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
// is really just a facade for detail::KnuthBendixImpl.

#ifndef LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_
#define LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_

#include <algorithm>  // for for_each
#include <utility>    // for move
#include <vector>     // for vector

#include "order.hpp"         // for ShortLexCompare
#include "presentation.hpp"  // for Presentation

#include "detail/citow.hpp"              // for citow
#include "detail/cong-common-class.hpp"  // for CongruenceCommon
#include "detail/knuth-bendix-impl.hpp"  // for KnuthBendixImpl
#include "detail/rewriters.hpp"          // for RewriteTrie

namespace libsemigroups {
  enum class congruence_kind;
  enum class tril;

  //! \defgroup knuth_bendix_group Knuth-Bendix
  //!
  //! This page contains links to the documentation related to the
  //! implementation of the Knuth-Bendix algorithm in `libsemigroups`.
  //!
  //! The purpose of this algorithm is to find a locally confluent presentation
  //! for a semigroup or monoid, with respect to some alphabet order and
  //! reduction ordering.

  //! \defgroup knuth_bendix_class_group KnuthBendix class
  //! \ingroup knuth_bendix_group
  //!
  //! \brief Class template containing an implementation of the Knuth-Bendix
  //! Algorithm.
  //!
  //! Defined in \c knuth-bendix-class.hpp.
  //!
  //! On this page we describe the functionality relating to the Knuth-Bendix
  //! algorithm for semigroups and monoids in \c libsemigroups. This page
  //! contains details of the member functions of the class \ref_knuth_bendix.
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
  //! KnuthBendix kb(congruence_kind::twosided, p);
  //!
  //! kb.number_of_active_rules();  //-> 0
  //! kb.number_of_pending_rules(); //-> 4
  //! kb.run();
  //! kb.number_of_active_rules();  //-> 4
  //! kb.number_of_pending_rules(); //-> 0
  //! kb.confluent();               //-> true
  //! kb.number_of_classes();       //-> POSITIVE_INFINITY
  //! \endcode
  //!
  //! \warning At present it is only possible to create KnuthBendix objects from
  //! presentations with alphabets containing at most:
  //! * 128 letters if `char` a signed integer;
  //! * 256 letters if `char` is an unsigned integer.
  template <typename Word,
            typename Rewriter       = detail::RewriteTrie,
            typename ReductionOrder = ShortLexCompare>
  class KnuthBendix : public detail::KnuthBendixImpl<Rewriter, ReductionOrder> {
   private:
    using KnuthBendixImpl_ = detail::KnuthBendixImpl<Rewriter, ReductionOrder>;

    bool               _extra_letter_added;
    std::vector<Word>  _generating_pairs;
    Presentation<Word> _presentation;

    // defined in detail/kbe.hpp
    friend class ::libsemigroups::detail::KBE<KnuthBendix>;

   public:
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    //! \ingroup knuth_bendix_class_mem_types_group
    //! \brief Struct containing various options that can be used to control
    //! the behaviour of Knuth-Bendix.
    //!
    //! This struct containing various options that can be used to control the
    //! behaviour of Knuth-Bendix.
    struct options {
      //! \brief Values for specifying how to measure the length of an
      //! overlap.
      //!
      //! The values in this enum determine how a \ref_knuth_bendix instance
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

    //! \ingroup knuth_bendix_class_mem_types_group
    //!
    //! \brief Type of the letters in the relations of the presentation stored
    //! in a \ref_knuth_bendix instance.
    //!
    //! Type of the letters in the relations of the presentation stored
    //! in a \ref_knuth_bendix instance.
    using native_word_type = Word;

    //! \ingroup knuth_bendix_class_mem_types_group
    //!
    //! \brief Type of the rules in the system.
    //!
    //! Type of the rules in the system.
    using rule_type = std::pair<Word, Word>;

    //! \ingroup knuth_bendix_class_init_group
    //!
    //! \brief Default constructor.
    //!
    //! This function default constructs an uninitialised \ref_knuth_bendix
    //! instance.
    KnuthBendix();

    //! \ingroup knuth_bendix_class_init_group
    //!
    //! \brief Remove the presentation and rewriter data.
    //!
    //! This function clears the rewriter, presentation, settings and stats
    //! from the \ref_knuth_bendix object, putting it back into the state it
    //! would be in if it was newly default constructed.
    //!
    //! \returns
    //! A reference to \c this.
    // TODO(1) to tpp file
    KnuthBendix& init() {
      _extra_letter_added = false;
      _generating_pairs.clear();
      _presentation.init();
      KnuthBendixImpl_::init();
      return *this;
    }

    //! \ingroup knuth_bendix_class_init_group
    //!
    //! \brief Copy constructor.
    //!
    //! Copy constructor.
    //!
    //! \param that the \ref_knuth_bendix instance to copy.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
    //! rules of \p that.
    KnuthBendix(KnuthBendix const& that);

    //! \ingroup knuth_bendix_class_init_group
    //!
    //! \brief Move constructor.
    //!
    //! Move constructor.
    KnuthBendix(KnuthBendix&&);

    //! \ingroup knuth_bendix_class_init_group
    //!
    //! \brief Copy assignment operator.
    //!
    //! Copy assignment operator.
    KnuthBendix& operator=(KnuthBendix const&);

    //! \ingroup knuth_bendix_class_init_group
    //!
    //! \brief Move assignment operator.
    //!
    //! Move assignment operator.
    KnuthBendix& operator=(KnuthBendix&&);

    ~KnuthBendix();

    //! \copydoc KnuthBendix(congruence_kind, Presentation<Word> const&)
    KnuthBendix(congruence_kind knd, Presentation<Word>&& p) : KnuthBendix() {
      init(knd, std::move(p));
    }

    //! \copydoc init(congruence_kind, Presentation<Word> const&)
    KnuthBendix& init(congruence_kind knd, Presentation<Word>&& p);

    //! \ingroup knuth_bendix_class_init_group
    //!
    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a \ref_knuth_bendix instance representing
    //! a congruence of kind \p knd over the semigroup or monoid defined by
    //! the presentation \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    //! \throws LibsemigroupsException if \p p has too many letters in its
    //! alphabet, see the warning below.
    //!
    //! \warning At present it is only possible to create KnuthBendix objects
    //! from presentations with alphabets containing at most:
    //! * 128 letters if `char` is a signed integer;
    //! * 256 letters if `char` is an unsigned integer.
    KnuthBendix(congruence_kind knd, Presentation<Word> const& p)
        // call the rval ref constructor
        : KnuthBendix(knd, Presentation<Word>(p)) {}

    //! \ingroup knuth_bendix_class_init_group
    //!
    //! \brief Re-initialize a \ref_knuth_bendix instance.
    //!
    //! This function puts a \ref_knuth_bendix instance back into the state
    //! that it would have been in if it had just been newly constructed from
    //! \p knd and \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    //! \throws LibsemigroupsException if \p p has too many letters in its
    //! alphabet, see the warning below.
    //!
    //! \warning At present it is only possible to create KnuthBendix objects
    //! from presentations with alphabets containing at most:
    //! * 128 letters if `char` a signed integer;
    //! * 256 letters if `char` is an unsigned integer.
    KnuthBendix& init(congruence_kind knd, Presentation<Word> const& p) {
      // call the rval ref init
      return init(knd, Presentation<Word>(p));
    }

    //! \ingroup knuth_bendix_class_init_group
    //!
    //! \brief Throws if any letter in a range is out of bounds.
    //!
    //! This function throws a LibsemigroupsException if any value pointed
    //! at by an iterator in the range \p first to \p last is out of bounds
    //! (i.e. does not belong to the alphabet of the \ref presentation used
    //! to construct the \ref_knuth_bendix instance, see the warning below).
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the
    //! word.
    //!
    //! \throw LibsemigroupsException if any letter in the range from
    //! \p first to \p last is out of bounds.
    //!
    //! \warning If a KnuthBendix instance represents a 1-sided congruence
    //! (\ref kind) and there are any generating pairs
    //! (\ref number_of_generating_pairs `> 0`), then the alphabet of the
    //! presentation will contain one more letter than the original input
    //! alphabet. This extra letter cannot be used in the input of any
    //! function, and is required for the algorithm to work in
    //! this case, and is present in the output of, for example,
    //! \ref active_rules.
    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_not_in_alphabet(Iterator1 first, Iterator2 last) const;

#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    //! \ingroup knuth_bendix_class_intf_group
    //!
    //! \brief The kind of the congruence (1- or 2-sided).
    //!
    //! This function returns the kind of the congruence represented by a
    //! \ref_knuth_bendix instance; see \ref congruence_kind for details.
    //!
    //! \return The kind of the congruence (1- or 2-sided).
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] congruence_kind kind() const noexcept;

    //! \ingroup knuth_bendix_class_intf_group
    //!
    //! \brief Returns the number of generating pairs.
    //!
    //! This function returns the number of generating pairs, which is the
    //! size of \ref generating_pairs divided by \f$2\f$.
    //!
    //! \returns
    //! The number of generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t number_of_generating_pairs() const noexcept;
#endif

    //! \ingroup knuth_bendix_class_intf_group
    //!
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

    //! \ingroup knuth_bendix_class_intf_group
    //!
    //! \brief Return the presentation used to construct or initialize a
    //! \ref_knuth_bendix object.
    //!
    //! This function returns the presentation used to construct or initialize
    //! a \ref_knuth_bendix object.
    //!
    //! \returns
    //! A const reference to the presentation.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning If a KnuthBendix instance represents a 1-sided congruence
    //! (\ref kind) and there are any generating pairs
    //! (\ref number_of_generating_pairs `> 0`), then the alphabet of the
    //! presentation will contain one more letter than the original input
    //! alphabet. This extra letter is required for the algorithm to work in
    //! this case, and is present in the output of, for example,
    //! \ref active_rules.
    [[nodiscard]] Presentation<Word> const& presentation() const noexcept {
      return _presentation;
    }

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup knuth_bendix_class_intf_group
    //!
    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented by
    //! a
    //! \ref_knuth_bendix instance.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns A reference to `*this`.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
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

    //! \ingroup knuth_bendix_class_intf_group
    //!
    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented by
    //! a
    //! \ref_knuth_bendix instance.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns A reference to `*this`.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    //!
    //! \cong_common_throws_if_started
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    KnuthBendix& add_generating_pair(Iterator1 first1,
                                     Iterator2 last1,
                                     Iterator3 first2,
                                     Iterator4 last2);

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

    // TODO: contains_no_checks is implemented
    // and documented in KnuthBendixImpl

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains_no_checks(Iterator1 first1,
                                      Iterator2 last1,
                                      Iterator3 first2,
                                      Iterator4 last2) const {
      return KnuthBendixImpl_::currently_contains_no_checks(
          detail::citow(this, first1),
          detail::citow(this, last1),
          detail::citow(this, first2),
          detail::citow(this, last2));
    }

    //! \ingroup knuth_bendix_class_intf_group
    //!
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to
    //! be contained in the congruence represented by a \ref_knuth_bendix
    //! instance. This function performs no enumeration, so it is possible for
    //! the words to be contained in the congruence, but that this is not
    //! currently known.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) const {
      // Call CongruenceCommon version so that we perform bound checks
      // in KnuthBendix and not KnuthBendixImpl_
      return detail::CongruenceCommon::currently_contains<KnuthBendix>(
          first1, last1, first2, last2);
    }

    //! \ingroup knuth_bendix_class_intf_group
    //!
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref_knuth_bendix
    //! instance. This function triggers a full enumeration,
    //! which may never terminate.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_common_warn_undecidable{Knuth-Bendix}
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
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

    //! \ingroup knuth_bendix_class_intf_group
    //! \brief Reduce a word with no run and no checks.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator
    //! \p d_first. This function triggers no enumeration. The word output by
    //! this function is equivalent to the input word in the congruence
    //! defined by a
    //! \ref_knuth_bendix instance. If the \ref_knuth_bendix instance is
    //! \ref finished, then the output word is a normal form for the input
    //! word. If the \ref_knuth_bendix instance is not \ref finished, then
    //! it might be that equivalent input words produce different output
    //! words.
    //!
    //! \cong_common_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                           InputIterator1 first,
                                           InputIterator2 last) const {
      return KnuthBendixImpl_::reduce_no_run_no_checks(
                 detail::itow(this, d_first),
                 detail::citow(this, first),
                 detail::citow(this, last))
          .get();
    }

    //! \ingroup knuth_bendix_class_intf_group
    //! \brief Reduce a word with no checks.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first
    //! and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by
    //! a
    //! \ref_knuth_bendix instance. In other words, the output word is a
    //! normal form for the input word or equivalently a canconical
    //! representative of its congruence class.
    //!
    //! \cong_common_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    //!
    //! \cong_common_warn_undecidable{Knuth-Bendix}
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    InputIterator1 first,
                                    InputIterator2 last) {
      return KnuthBendixImpl_::reduce_no_checks(detail::itow(this, d_first),
                                                detail::citow(this, first),
                                                detail::citow(this, last))
          .get();
    }

    //! \ingroup knuth_bendix_class_intf_group
    //!
    //! \brief Reduce a word with no enumeration.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator
    //! \p d_first. This function triggers no enumeration. The word output by
    //! this function is equivalent to the input word in the congruence
    //! defined by a \ref_knuth_bendix instance. If the \ref_knuth_bendix
    //! instance is
    //! \ref finished, then the output word is a normal form for the input
    //! word. If the \ref_knuth_bendix instance is not \ref finished, then it
    //! might be that equivalent input words produce different output words.
    //!
    //! \cong_common_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 InputIterator1 first,
                                 InputIterator2 last) const {
      // Call CongruenceCommon version so that we perform bound checks
      // in KnuthBendix and not KnuthBendixImpl_
      return detail::CongruenceCommon::reduce_no_run<KnuthBendix>(
          d_first, first, last);
    }

    //! \ingroup knuth_bendix_class_intf_group
    //!
    //! \brief Reduce a word.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first
    //! and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by
    //! a
    //! \ref_knuth_bendix instance. In other words, the output word is a
    //! normal form for the input word or equivalently a canconical
    //! representative of its congruence class.
    //!
    //! \cong_common_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    //!
    //! \cong_common_warn_undecidable{Knuth-Bendix}
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last) {
      // Call CongruenceCommon version so that we perform bound checks
      // in KnuthBendix and not KnuthBendixImpl_
      return detail::CongruenceCommon::reduce<KnuthBendix>(
          d_first, first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix specific stuff
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup knuth_bendix_class_accessors_group
    //! \brief Return a range object containing the active rules.
    //!
    //! This function returns a range object containing the pairs of
    //! strings which represent the rules of a \ref_knuth_bendix instance. The
    //! \c first entry in every such pair is greater than the \c second
    //! according to the reduction ordering of the \ref_knuth_bendix instance.
    //!
    //! \returns
    //! A range object containing the current active rules.
    // TODO(1) should be const
    auto active_rules();

    //! \brief Return the node labels of the Gilman \ref WordGraph.
    //!
    //! This function returns the node labels of the Gilman \ref WordGraph,
    //! corresponding to the unique prefixes of the left-hand sides of the
    //! rules of the rewriting system.
    //!
    //! \return The node labels of the Gilman \ref WordGraph, a const
    //! reference to a `std::vector<Word>`.
    //!
    //! \sa \ref gilman_graph.
    std::vector<Word> gilman_graph_node_labels();

   private:
    void run_impl();

    [[nodiscard]] bool requires_extra_letter() const {
      return (!generating_pairs().empty()
              && detail::CongruenceCommon::kind() == congruence_kind::onesided);
    }
  };  // class KnuthBendix

  //! \ingroup knuth_bendix_class_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `knuth-bendix-class.hpp`.
  //!
  //! Deduction guide to construct a `KnuthBendix<Word>` from a
  //! `Presentation<Word>` const reference.
  template <typename Word>
  KnuthBendix(congruence_kind, Presentation<Word> const&) -> KnuthBendix<Word>;

  //! \ingroup knuth_bendix_class_group
  //!
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
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `knuth-bendix-class.hpp`.
  //!
  //! Deduction guide to construct a `KnuthBendix<Word>` from a
  //! `KnuthBendix<Word>` const reference.
  template <typename Word>
  KnuthBendix(KnuthBendix<Word> const&) -> KnuthBendix<Word>;

  //! \ingroup knuth_bendix_class_group
  //!
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
