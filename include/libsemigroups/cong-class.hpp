//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

// This file contains the declaration of the Congruence class template, for
// creating congruence over FroidurePin objects or over Presentation objects.

#ifndef LIBSEMIGROUPS_CONG_CLASS_HPP_
#define LIBSEMIGROUPS_CONG_CLASS_HPP_

#include <cstddef>      // for size_t
#include <cstdint>      // for uint64_t
#include <memory>       // for shared_ptr
#include <string>       // for string
#include <string_view>  // for string_view
#include <utility>      // for move
#include <vector>       // for vector

#include "knuth-bendix-class.hpp"  // for KnuthBendix
#include "presentation.hpp"        // for Presentation
#include "todd-coxeter-class.hpp"  // for ToddCoxeter
#include "types.hpp"               // for letter_type, wor...

#include "detail/cong-common-class.hpp"  // for detail::CongruenceCommon
#include "detail/race.hpp"               // for Race

namespace libsemigroups {
  class FroidurePinBase;  // Forward declaration, constructor parameters

  template <typename Node>
  class WordGraph;

  namespace detail {
    struct CongruenceBase {};
  }  // namespace detail

  //! \defgroup congruence_group Congruence
  //!
  //! This page contains links documentation related to the class template
  //! \ref_congruence.
  //!
  //! Helper functions for the class template \ref_congruence can be found in
  //! the namespaces \ref cong_common_helpers_group "congruence_common" and
  //! \ref congruence. At present the helper functions in these two namespaces
  //! are identical, because there are no helper functions that only apply to
  //! the \ref_congruence class template.

  //! \defgroup congruence_class_mem_types_group Member Types
  //!
  //! \ingroup congruence_class_group
  //!
  //! \brief Public member types.
  //!
  //! This page contains the documentation of the public member types of a
  //! \ref_congruence instance.

  //! \defgroup congruence_class_init_group Constructors + initializers
  //!
  //! \ingroup congruence_class_group
  //!
  //! \brief Construct or re-initialize a \ref_congruence
  //! instance (public member function).
  //!
  //! This page documents the constructors and initialisers for the
  //! \ref_congruence class.
  //!
  //! Every constructor (except the move + copy constructors, and the move
  //! and copy assignment operators) has a matching `init` function with the
  //! same signature that can be used to re-initialize a \ref_congruence
  //! instance as if it had just been constructed; but without necessarily
  //! releasing any previous allocated memory.

  //! \defgroup congruence_class_settings_group Settings
  //! \ingroup congruence_class_group
  //!
  //! \brief Settings that control the behaviour of a \ref_congruence
  //! instance.
  //!
  //! This page contains information about the member functions of
  //! \ref_congruence that control various settings that influence the
  //! running of the congruence-based algorithms.

  //! \defgroup congruence_class_intf_group Common member functions
  //!
  //! \ingroup congruence_class_group
  //!
  //! \brief Documentation of common member functions of \ref_congruence,
  //! \ref_kambites, \ref_knuth_bendix, and \ref_todd_coxeter.
  //!
  //! This page contains documentation of the member functions of
  //! \ref_congruence that are implemented in all of the classes
  //! \ref_congruence, \ref_kambites, \ref_knuth_bendix, and \ref_todd_coxeter.

  //! \defgroup congruence_class_accessors_group Accessors
  //!
  //! \ingroup congruence_class_group
  //!
  //! \brief Member functions that can be used to access the state of a
  //! \ref_congruence instance.
  //!
  //! This page contains the documentation of the various member functions of
  //! the \ref_congruence class that can be used to access the state of an
  //! instance.
  //!
  //! Those functions with the prefix `current_` do not perform any
  //! further enumeration.

  //! \defgroup congruence_class_group The Congruence class
  //!
  //! \ingroup congruence_group
  //!
  //! \brief Class for running \ref_kambites, \ref_knuth_bendix, and
  //! \ref_todd_coxeter in parallel.
  //!
  //! Defined in `cong-class.hpp`.
  //!
  //! On this page we describe the functionality relating to the class template
  //! \ref_congruence in `libsemigroups`. This class can be used for computing a
  //! congruence over a semigroup or monoid by running every applicable
  //! algorithm from `libsemigroups` (and some variants of the same algorithm)
  //! in parallel. This class is provided for convenience, at present it is not
  //! very customisable, and lacks some of the fine grained control offered by
  //! the classes implementing individual algorithms, such as \ref_kambites,
  //! \ref_knuth_bendix, and \ref_todd_coxeter.
  //!
  //! \tparam Word the type of the words used in the
  //! \ref Congruence::presentation and \ref Congruence::generating_pairs.
  //!
  //! \sa \ref cong_common_helpers_group for information about helper functions
  //! for the \ref_congruence class template.
  //!
  //! \par Example
  //! \code
  //! Presentation<word_type> p;
  //! p.alphabet(2);
  //! p.contains_empty_word(true);
  //! presentation::add_rule(p, {0, 1}, {});
  //!
  //! Congruence cong(congruence_kind::twosided, p);
  //! is_obviously_infinite(cong);  // true
  //! congruence_common::add_generating_pair(cong, {0, 0, 0}, {});
  //! cong.number_of_classes(); //-> 3
  //! \endcode
  template <typename Word>
  class Congruence : public detail::CongruenceCommon,
                     public detail::CongruenceBase {
    enum class RunnerKind : size_t { TC = 0, KB = 1, K = 2 };

    /////////////////////////////////////////////////////////////////////////
    // Congruence - data - private
    /////////////////////////////////////////////////////////////////////////

    mutable detail::Race    _race;
    mutable bool            _runners_initted;
    std::vector<RunnerKind> _runner_kinds;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Interface requirements - native-types
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup congruence_class_mem_types_group
    //!
    //! \brief Type of the words in the relations of the presentation stored in
    //! a \ref_congruence instance.
    //!
    //! The template parameter \c Word, which is the type of the words in the
    //! relations of the presentation stored in a \ref_congruence instance.
    using native_word_type = Word;

    //////////////////////////////////////////////////////////////////////////
    // Congruence - constructors - public
    //////////////////////////////////////////////////////////////////////////

    //! \ingroup congruence_class_init_group
    //!
    //! \brief Default constructor.
    //!
    //! This function default constructs an uninitialised \ref_congruence
    //! instance.
    Congruence();

    //! \ingroup congruence_class_init_group
    //!
    //! \brief Re-initialize a \ref_congruence instance.
    //!
    //! This function puts a \ref_congruence instance back into the state that
    //! it would have been in if it had just been newly default constructed.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Congruence& init();

    //! \ingroup congruence_class_init_group
    //!
    //! \brief Copy constructor.
    //!
    //! Copy constructor.
    Congruence(Congruence const&);

    //! \ingroup congruence_class_init_group
    //!
    //! \brief Move constructor.
    //!
    //! Move constructor.
    Congruence(Congruence&&);

    //! \ingroup congruence_class_init_group
    //!
    //! \brief Copy assignment operator.
    //!
    //! Copy assignment operator.
    Congruence& operator=(Congruence const&);

    //! \ingroup congruence_class_init_group
    //!
    //! \brief Move assignment operator.
    //!
    //! Move assignment operator.
    Congruence& operator=(Congruence&&);

    ~Congruence();

    //! \ingroup congruence_class_init_group
    //!
    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a \ref_congruence instance representing a
    //! congruence of kind \p knd over the semigroup or monoid defined by the
    //! presentation \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    // NOTE: No rvalue ref version because we anyway must copy p multiple times
    Congruence(congruence_kind knd, Presentation<Word> const& p)
        : Congruence() {
      init(knd, p);
    }

    //! \ingroup congruence_class_init_group
    //!
    //! \brief Re-initialize a \ref_congruence instance.
    //!
    //! This function puts a \ref_congruence instance back into the state that
    //! it would have been in if it had just been newly constructed from \p knd
    //! and \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    // NOTE:  No rvalue ref version because we anyway must copy p multiple times
    Congruence& init(congruence_kind knd, Presentation<Word> const& p);

    //////////////////////////////////////////////////////////////////////////
    // Congruence - interface requirements - add_generating_pair
    //////////////////////////////////////////////////////////////////////////

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented by a
    //! \ref_congruence instance.
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
    Congruence& add_generating_pair_no_checks(Iterator1 first1,
                                              Iterator2 last1,
                                              Iterator3 first2,
                                              Iterator4 last2) {
      _runners_initted = false;
      return detail::CongruenceCommon::add_internal_generating_pair_no_checks<
          Congruence>(first1, last1, first2, last2);
    }

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented by a
    //! \ref_congruence instance.
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
    Congruence& add_generating_pair(Iterator1 first1,
                                    Iterator2 last1,
                                    Iterator3 first2,
                                    Iterator4 last2) {
      _runners_initted = false;
      return detail::CongruenceCommon::add_generating_pair<Congruence>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // Congruence - interface requirements - number_of_classes
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Compute the number of classes in the congruence.
    //!
    //! This function computes the number of classes in the congruence
    //! represented by a \ref_congruence instance by running the congruence
    //! enumeration until it terminates.
    //!
    //! \returns The number of congruences classes of a \ref_congruence
    //! instance if this number is finite, or \ref POSITIVE_INFINITY in some
    //! cases if this number is not finite.
    //!
    //! \warning Determining the number of classes is undecidable in general,
    //! and this may never terminate.
    [[nodiscard]] uint64_t number_of_classes();

    ////////////////////////////////////////////////////////////////////////
    // Congruence - interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to be
    //! contained in the congruence represented by a \ref_congruence instance.
    //! This function performs no enumeration, so it is possible for the words
    //! to be contained in the congruence, but that this is not currently known.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] tril currently_contains_no_checks(Iterator1 first1,
                                                    Iterator2 last1,
                                                    Iterator3 first2,
                                                    Iterator4 last2) const;

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to be
    //! contained in the congruence represented by a \ref_congruence instance.
    //! This function performs no enumeration, so it is possible for the words
    //! to be contained in the congruence, but that this is not currently known.
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
    [[nodiscard]] tril currently_contains(Iterator1 first1,
                                          Iterator2 last1,
                                          Iterator3 first2,
                                          Iterator4 last2) const {
      return detail::CongruenceCommon::currently_contains<Congruence>(
          first1, last1, first2, last2);
    }

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to be
    //! contained in the congruence represented by a \ref_congruence instance.
    //! This function performs no enumeration, so it is possible for the words
    //! to be contained in the congruence, but that this is not currently known.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    //!
    //! \warning Determining the number of classes is undecidable in general,
    //! and this may never terminate.
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] bool contains_no_checks(Iterator1 first1,
                                          Iterator2 last1,
                                          Iterator3 first2,
                                          Iterator4 last2) {
      return detail::CongruenceCommon::contains_no_checks<Congruence>(
          first1, last1, first2, last2);
    }

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref_congruence instance. This function
    //! triggers a full enumeration, which may never terminate.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \warning Determining the number of classes is undecidable in general,
    //! and this may never terminate.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] bool contains(Iterator1 first1,
                                Iterator2 last1,
                                Iterator3 first2,
                                Iterator4 last2) {
      return detail::CongruenceCommon::contains<Congruence>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // Congruence - interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Reduce a word with no computation or checks.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator
    //! \p d_first. If \ref finished returns \c true, then the word output by
    //! this function is a normal form for the input word. If the
    //! \ref_congruence instance is not \ref finished, then it might be that
    //! equivalent input words produce different output words.
    //!
    //! \cong_common_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    template <typename OutputIterator, typename Iterator1, typename Iterator2>
    OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                           Iterator1      first,
                                           Iterator2      last) const;

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Reduce a word with no computation.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator
    //! \p d_first. If \ref finished returns \c true, then the word output by
    //! this function is a normal form for the input word. If the
    //! \ref_congruence instance is not \ref finished, then it might be that
    //! equivalent input words produce different output words.
    //!
    //! \cong_common_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    template <typename OutputIterator, typename Iterator1, typename Iterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 Iterator1      first,
                                 Iterator2      last) const {
      return detail::CongruenceCommon::reduce_no_run<Congruence>(
          d_first, first, last);
    }

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Reduce a word with no checks.
    //!
    //! This function fully computes the congruence and then writes
    //! a reduced word equivalent to the input word described by the
    //! iterator \p first and \p last to the output iterator \p d_first. The
    //! word output by this function is a normal form for the input word.
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
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    InputIterator1 first,
                                    InputIterator2 last) {
      return detail::CongruenceCommon::reduce_no_checks<Congruence>(
          d_first, first, last);
    }

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Reduce a word.
    //!
    //! This function fully computes the congruence and then writes
    //! a reduced word equivalent to the input word described by the
    //! iterator \p first and \p last to the output iterator \p d_first. The
    //! word output by this function is a normal form for the input word.
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
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last) {
      return detail::CongruenceCommon::reduce<Congruence>(d_first, first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // Congruence - interface requirements - throw_if_letter_not_in_alphabet
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup congruence_class_init_group
    //!
    //! \brief Throws if any letter in a range is out of bounds.
    //!
    //! This function throws a LibsemigroupsException if any value pointed at
    //! by an iterator in the range \p first to \p last is out of bounds (i.e.
    //! does not belong to the alphabet of the \ref presentation used to
    //! construct the \ref_congruence instance).
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \throw LibsemigroupsException if any letter in the range from \p first
    //! to \p last is out of bounds.
    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_not_in_alphabet(Iterator1 first, Iterator2 last) const;

    //////////////////////////////////////////////////////////////////////////
    // Congruence - member functions - public
    //////////////////////////////////////////////////////////////////////////

    //! \ingroup congruence_class_accessors_group
    //!
    //! \brief Get a derived class of detail::CongruenceCommon being used to
    //! compute a \ref_congruence instance.
    //!
    //! This function returns a std::shared_ptr to a \p Thing if such an object
    //! is being used or could be used to compute the congruence represented by
    //! a \ref_congruence instance. If no such \p Thing is available, then an
    //! exception is thrown.
    //!
    //! \tparam Thing the type of the detail::CongruenceCommon object being
    //! sought.
    //!
    //! \returns The derived class of detail::CongruenceCommon.
    //!
    //! \throws LibsemigroupsException if no \p Thing is available.
    //!
    //! \sa has.
    template <typename Thing>
    std::shared_ptr<Thing> get() const;

    //! \ingroup congruence_class_accessors_group
    //!
    //! \brief Check if a derived class of detail::CongruenceCommon being used
    //! to compute a \ref_congruence instance.
    //!
    //! This function returns \c true if a \p Thing is being used or could be
    //! used to compute the congruence represented by a \ref_congruence
    //! instance; or \c false if not.
    //!
    //! \tparam Thing the type of the detail::CongruenceCommon object being
    //! sought.
    //!
    //! \returns Whether or not a \p Thing is being used to compute the
    //! \ref_congruence instance.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa get.
    template <typename Thing>
    [[nodiscard]] bool has() const;

    //! \ingroup congruence_class_settings_group
    //!
    //! \brief Get the current maximum number of threads.
    //!
    //! This function returns the current maximum number of threads that a
    //! \ref_congruence instance can use.
    //!
    //! \returns
    //! The maximum number of threads to use.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t max_threads() const noexcept {
      return _race.max_threads();
    }

    //! \ingroup congruence_class_settings_group
    //!
    //! \brief Set the maximum number of threads.
    //!
    //! This function can be used to set the maximum number of threads that a
    //! \ref_congruence instance can use.
    //!
    //! \param val the number of threads.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Congruence& max_threads(size_t val) noexcept {
      _race.max_threads(val);
      return *this;
    }

    //! \ingroup congruence_class_settings_group
    //!
    //! \brief Get the number of runners.
    //!
    //! This function returns the number of distinct detail::CongruenceCommon
    //! instances that are contained in a \ref_congruence object.
    //!
    //! \returns The number of runners.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t number_of_runners() const noexcept {
      return _race.number_of_runners();
    }

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Get the presentation defining the parent semigroup of the
    //! congruence.
    //!
    //! This function returns the presentation used to construct a
    //! \ref_congruence object.
    //!
    //! \returns The presentation.
    //!
    //! \throws LibsemigroupsException if no presentation was used to
    //! construct or initialise the object.
    [[nodiscard]] Presentation<Word> const& presentation() const;

    //! \ingroup congruence_class_intf_group
    //!
    //! \brief Get the generating pairs of the congruence.
    //!
    //! This function returns the generating pairs of the congruence, as added
    //! to the congruence by \ref add_generating_pair.
    //!
    //! \returns The std::vector of generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<Word> const& generating_pairs() const;

#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    //! \ingroup congruence_class_intf_group
    //!
    //! \brief The kind of the congruence (1- or 2-sided).
    //!
    //! This function returns the kind of the congruence represented by a
    //! \ref_congruence instance; see \ref congruence_kind for details.
    //!
    //! \return The kind of the congruence (1- or 2-sided).
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] congruence_kind kind() const noexcept;

    //! \ingroup congruence_class_intf_group
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

   private:
    //////////////////////////////////////////////////////////////////////////
    // Congruence - member functions - private
    //////////////////////////////////////////////////////////////////////////

    void add_runner(std::shared_ptr<ToddCoxeter<Word>>&& ptr) {
      _race.add_runner(std::move(ptr));
      _runner_kinds.push_back(RunnerKind::TC);
    }

    void add_runner(std::shared_ptr<KnuthBendix<Word>>&& ptr) {
      _race.add_runner(std::move(ptr));
      _runner_kinds.push_back(RunnerKind::KB);
    }

    void add_runner(std::shared_ptr<Kambites<Word>>&& ptr) {
      _race.add_runner(std::move(ptr));
      _runner_kinds.push_back(RunnerKind::K);
    }

    template <typename Result, typename Node>
    friend auto to(congruence_kind        knd,
                   FroidurePinBase&       fpb,
                   WordGraph<Node> const& wg)
        -> std::enable_if_t<
            std::is_same_v<Congruence<typename Result::native_word_type>,
                           Result>,
            Result>;

    void init_runners() const;

    //////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////

    void run_impl() override;
    bool finished_impl() const override {
      return _race.finished();
    }
  };  // class Congruence

  //! \ingroup congruence_class_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `cong-class.hpp`.
  //!
  //! Deduction guide to construct a `Congruence<Word>` from a
  //! \ref congruence_kind and Presentation<Word> const reference.
  template <typename Word>
  Congruence(congruence_kind, Presentation<Word> const&) -> Congruence<Word>;

  //! \ingroup congruence_class_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `cong-class.hpp`.
  //!
  //! Deduction guide to construct a `Congruence<Word>` from a
  //! \ref congruence_kind and Presentation<Word> rvalue reference.
  // NOTE:there's no rvalue ref constructor, so it's possible this guide is
  // superfluous
  template <typename Word>
  Congruence(congruence_kind, Presentation<Word>&&) -> Congruence<Word>;

  //! \ingroup congruence_class_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `cong-class.hpp`.
  //!
  //! Deduction guide to construct a `Congruence<Word>` from a
  //! `Congruence<Word>`.
  template <typename Word>
  Congruence(Congruence<Word> const&) -> Congruence<Word>;

  //! \ingroup congruence_class_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `cong-class.hpp`.
  //!
  //! Deduction guide to construct a `Congruence<Word>` from a
  //! `Congruence<Word>`.
  template <typename Word>
  Congruence(Congruence<Word>&&) -> Congruence<Word>;

  //! \ingroup congruence_group
  //!
  //! \brief Return a human readable representation of a \ref_congruence object.
  //!
  //! Defined in `cong-class.hpp`.
  //!
  //! This function returns a human readable representation of a
  //! \ref_congruence object.
  //!
  //! \param c the \ref_congruence object.
  //!
  //! \returns A std::string containing the representation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Word>
  std::string to_human_readable_repr(Congruence<Word> const& c);
}  // namespace libsemigroups

#include "cong-class.tpp"

#endif  // LIBSEMIGROUPS_CONG_CLASS_HPP_
