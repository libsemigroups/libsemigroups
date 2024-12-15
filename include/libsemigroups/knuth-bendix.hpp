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

// This file contains a class KnuthBendix which implements the Knuth-Bendix
// algorithm for finitely presented monoids.

// TODO(1)
// * noexcept
// * separate rule container from Rules
// * nodiscard

#ifndef LIBSEMIGROUPS_KNUTH_BENDIX_HPP_
#define LIBSEMIGROUPS_KNUTH_BENDIX_HPP_

#include <algorithm>            // for max, min
#include <atomic>               // for atomic
#include <cstddef>              // for size_t
#include <cstdint>              // for int64_t, uint64_t
#include <iosfwd>               // for ostream
#include <iterator>             // for distance
#include <limits>               // for numeric_limits
#include <list>                 // for list
#include <map>                  // for map
#include <set>                  // for set
#include <stack>                // for stack
#include <string>               // for basic_string, operator==
#include <unordered_map>        // for unordered_map, operator!=
#include <utility>              // for forward, move, pair
#include <vector>               // for allocator, vector
                                //
#include "aho-corasick.hpp"     // for Aho-corasick
#include "cong-intf.hpp"        // for CongruenceInterface
#include "constants.hpp"        // for Max, PositiveInfinity, operat...
#include "debug.hpp"            // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"        // for LIBSEMIGROUPS_EXCEPTION
#include "obvinf.hpp"           // for is_obviously_infinite
#include "order.hpp"            // for ShortLexCompare
#include "paths.hpp"            // for Paths
#include "presentation.hpp"     // for Presentation
#include "ranges.hpp"           // for operator|
#include "runner.hpp"           // for Runner
#include "to-presentation.hpp"  // for to_presentation
#include "types.hpp"            // for word_type
#include "word-graph.hpp"       // for WordGraph
#include "word-range.hpp"       // for to_strings

#include "detail/multi-string-view.hpp"  // for MultiStringView
#include "detail/report.hpp"             // for Reporter, REPORT_DEFAULT, REP...
#include "detail/rewriters.hpp"          // for RewriteTrie
#include "detail/string.hpp"             // for is_prefix, maximum_common_prefix

namespace libsemigroups {
  // Forward declarations
  namespace detail {
    template <typename KnuthBendix_>
    class KBE;
  }  // namespace detail

  //! \defgroup knuth_bendix_group Knuth-Bendix
  //!
  //! This page contains links to the documentation related to the
  //! implementation of the Knuth-Bendix algorithm in ``libsemigroups``.

  //! \ingroup knuth_bendix_group
  //!
  //! \brief Class containing an implementation of the Knuth-Bendix Algorithm.
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
  //! \par Example
  //! \code
  //! Presentation<std::string> p;
  //! p.contains_empty_word(true);
  //! p.alphabet("abcd");
  //! presentation::add_rule_no_checks(p, "ab", "");
  //! presentation::add_rule_no_checks(p, "ba", "");
  //! presentation::add_rule_no_checks(p, "cd", "");
  //! presentation::add_rule_no_checks(p, "dc", "");
  //! presentation::add_rule_no_checks(p, "ca", "ac");
  //!
  //! KnuthBendix kb(twosided, p);
  //!
  //! !kb.confluent();              // true
  //! kb.run();
  //! kb.number_of_active_rules();  // 8
  //! kb.confluent();               // true
  //! kb.number_of_classes();       // POSITIVE_INFINITY
  //! \endcode
  template <typename Rewriter       = detail::RewriteTrie,
            typename ReductionOrder = ShortLexCompare>
  class KnuthBendix : public CongruenceInterface {
    // defined in detail/kbe.hpp
    friend class ::libsemigroups::detail::KBE<KnuthBendix>;

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - nested subclasses - private
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
    using rule_type = std::pair<std::string, std::string>;

    //! \brief Type of the letters in the relations of the presentation stored
    //! in a \ref KnuthBendix instance.
    //!
    //! A \ref KnuthBendix instance can be constructed or initialised from a
    //! presentation of arbitrary types of letters and words. Internally the
    //! letters are converted to \ref native_letter_type.
    using native_letter_type = char;

    //! \brief Type of the words in the relations of the presentation stored in
    //! a \ref KnuthBendix instance.
    //!
    //! A \ref KnuthBendix instance can be constructed or initialised from a
    //! presentation with arbitrary types of letters and words. Internally the
    //! words are converted to \ref native_word_type.
    using native_word_type = std::string;

    //! \brief Type of the presentation stored in a \ref KnuthBendix instance.
    //!
    //! A \ref KnuthBendix instance can be constructed or initialised from a
    //! presentation of arbitrary types of letters and words. Internally the
    //! presentation is stored as a \ref native_presentation_type.
    using native_presentation_type = Presentation<std::string>;

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - types - public
    //////////////////////////////////////////////////////////////////////////

    //! \brief Struct containing various options that can be used to control the
    //! behaviour of Knuth-Bendix.
    //!
    //! This struct containing various options that can be used to control the
    //! behaviour of Knuth-Bendix.
    struct options {
      //! \brief Values for specifying how to measure the length of an overlap.
      //!
      //! The values in this enum determine how a KnuthBendix instance
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
      // TODO(0) delete the alias
      using time_point = std::chrono::high_resolution_clock::time_point;
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
    // KnuthBendix - data - private
    ////////////////////////////////////////////////////////////////////////

    bool                            _gen_pairs_initted;
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
    // KnuthBendix - constructors and destructor - public
    //////////////////////////////////////////////////////////////////////////

    //! \brief Default constructor.
    //!
    //! Constructs a KnuthBendix instance with no rules, and the short-lex
    //! reduction ordering.
    //!
    //! This function default constructs an uninitialised \ref
    //! KnuthBendix instance.
    explicit KnuthBendix();

    //! \brief Remove the presentation and rewriter data
    //!
    //! This function clears the rewriter, presentation, settings and stats from
    //! the KnuthBendix object, putting it back into the state it would be in if
    //! it was newly default constructed.
    //!
    //! \returns
    //! A reference to \c this.
    KnuthBendix& init();

    //! \brief Copy constructor.
    //!
    //! Copy constructor.
    //!
    //! \param that the KnuthBendix instance to copy.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
    //! rules of \p that.
    KnuthBendix(KnuthBendix const& that);

    //! \brief Move constructor.
    //!
    //! Move constructor.
    KnuthBendix(KnuthBendix&&);

    //! \brief Copy assignment operator.
    //!
    //! Copy assignment operator.
    KnuthBendix& operator=(KnuthBendix const&);

    //! \brief Move assignment operator.
    //!
    //! Move assignment operator.
    KnuthBendix& operator=(KnuthBendix&&);

    ~KnuthBendix();

    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a \ref KnuthBendix instance representing a
    //! congruence of kind \p knd over the semigroup or monoid defined by the
    //! presentation \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    KnuthBendix(congruence_kind knd, Presentation<std::string> const& p);

    //! \brief Re-initialize a \ref KnuthBendix instance.
    //!
    //! This function puts a \ref KnuthBendix instance back into the state that
    //! it would have been in if it had just been newly constructed from \p knd
    //! and \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    KnuthBendix& init(congruence_kind knd, Presentation<std::string> const& p);

    //! \copydoc KnuthBendix(congruence_kind, Presentation<std::string> const&)
    KnuthBendix(congruence_kind knd, Presentation<std::string>&& p);

    //! \copydoc init(congruence_kind, Presentation<std::string> const&)
    KnuthBendix& init(congruence_kind knd, Presentation<std::string>&& p);

    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a \ref KnuthBendix instance representing a
    //! congruence of kind \p knd over the semigroup or monoid defined by the
    //! presentation \p p. The type of the words in \p p can be anything, but
    //! will be converted in to \ref native_word_type. This means that if the
    //! input presentation uses \ref word_type, for example, as the word type,
    //! then this presentation is converted into a \ref
    //! native_presentation_type. This converted presentation can be recovered
    //! using \ref presentation.
    //!
    //! \tparam Word the type of the words in the presentation \p p.
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    // No rvalue ref version because we can't use it.
    template <typename Word>
    explicit KnuthBendix(congruence_kind knd, Presentation<Word> const& p)
        // to_presentation throws in the next line if p isn't valid.
        // The next line looks weird but we are usually taking in letter_type's
        // and returning chars
        : KnuthBendix(knd, to_presentation<std::string>(p, [](auto const& x) {
                        return x;
                      })) {}

    //! \brief Re-initialize a \ref KnuthBendix
    //! instance.
    //!
    //! This function re-initializes a  \ref KnuthBendix instance as if it had
    //! been newly constructed from \p knd and \p p.
    //!
    //! \tparam Word the type of the words in the presentation \p p.
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    template <typename Word>
    KnuthBendix& init(congruence_kind knd, Presentation<Word> const& p) {
      // to_presentation throws in the next line if p isn't valid.
      // The next line looks weird but we are usually taking in letter_type's
      // and returning chars
      init(knd,
           to_presentation<std::string>(p, [](auto const& x) { return x; }));
      return *this;
    }

    // TODO(1) construct/init from kind and KnuthBendix const&, for consistency
    // with ToddCoxeter

   private:
    void init_from_generating_pairs();
    void init_from_presentation();

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - add_generating_pair
    //////////////////////////////////////////////////////////////////////////

   public:
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
    // NOTE THAT this is not the same as in ToddCoxeter, because the generating
    // pairs contained in CongruenceInterface are word_types, and so we don't
    // require any conversion here (since chars can be converted implicitly to
    // letter_types)
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    KnuthBendix& add_generating_pair_no_checks(Iterator1 first1,
                                               Iterator2 last1,
                                               Iterator3 first2,
                                               Iterator4 last2) {
      return CongruenceInterface::add_generating_pair_no_checks<KnuthBendix>(
          first1, last1, first2, last2);
    }

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
      return CongruenceInterface::add_generating_pair<KnuthBendix>(
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
    //! determine the number of classes of the congruence represented by \c this
    //! even if it is infinite. Moreover, the complexity of this function is at
    //! worst \f$O(mn)\f$ where \f$m\f$ is the number of letters in the
    //! alphabet, and \f$n\f$ is the number of nodes in the \ref gilman_graph.
    [[nodiscard]] uint64_t number_of_classes();

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
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] tril currently_contains_no_checks(Iterator1 first1,
                                                    Iterator2 last1,
                                                    Iterator3 first2,
                                                    Iterator4 last2) const;

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
    [[nodiscard]] tril currently_contains(Iterator1 first1,
                                          Iterator2 last1,
                                          Iterator3 first2,
                                          Iterator4 last2) const {
      throw_if_letter_out_of_bounds(first1, last1);
      throw_if_letter_out_of_bounds(first2, last2);
      return currently_contains_no_checks(first1, last1, first2, last2);
    }

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref KnuthBendix instance. This function
    //! triggers a full enumeration, which may never terminate.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_warn_undecidable{Knuth-Bendix}
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] bool contains_no_checks(Iterator1 first1,
                                          Iterator2 last1,
                                          Iterator3 first2,
                                          Iterator4 last2) {
      run();
      return currently_contains_no_checks(first1, last1, first2, last2)
             == tril::TRUE;
    }

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref KnuthBendix
    //! instance. This function triggers a full enumeration, which may never
    //! terminate.
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
    [[nodiscard]] bool contains(Iterator1 first1,
                                Iterator2 last1,
                                Iterator3 first2,
                                Iterator4 last2) {
      run();
      return currently_contains(first1, last1, first2, last2) == tril::TRUE;
    }

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

    //! \brief Reduce a word with no run and no checks.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator \p
    //! d_first. This function triggers no enumeration. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref KnuthBendix instance. If the \ref KnuthBendix instance is \ref
    //! finished, then the output word is a normal form for the input word. If
    //! the \ref KnuthBendix instance is not \ref finished, then it might be
    //! that equivalent input words produce different output words.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                           InputIterator1 first,
                                           InputIterator2 last) const;

    //! \brief Reduce a word with no enumeration.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator \p
    //! d_first. This function triggers no enumeration. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref KnuthBendix instance. If the \ref KnuthBendix instance is \ref
    //! finished, then the output word is a normal form for the input word. If
    //! the \ref KnuthBendix instance is not \ref finished, then it might be
    //! that equivalent input words produce different output words.
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
      throw_if_letter_out_of_bounds(first, last);
      return reduce_no_run_no_checks(d_first, first, last);
    }

    //! \brief Reduce a word with no checks.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref KnuthBendix instance. In other words, the output word is a normal
    //! form for the input word or equivalently a canconical representative of
    //! its congruence class.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    //! \cong_intf_warn_undecidable{Knuth-Bendix}
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    InputIterator1 first,
                                    InputIterator2 last) {
      run();
      return reduce_no_run_no_checks(d_first, first, last);
    }

    //! \brief Reduce a word.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref KnuthBendix instance. In other words, the output word is a normal
    //! form for the input word or equivalently a canconical representative of
    //! its congruence class.
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
      run();
      return reduce_no_run(d_first, first, last);
    }

    // TODO(1) implement reduce_inplace x4 if possible.

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - setters for optional parameters - public
    //////////////////////////////////////////////////////////////////////////

    //! \brief Set the number of rules to accumulate before they are processed.
    //!
    //! This function can be used to specify the number of pending rules that
    //! must accumulate before they are reduced, processed, and added to the
    //! system.
    //!
    //! The default value is \c 128, and should be set to \c 1 if \ref run
    //! should attempt to add each rule as they are created without waiting for
    //! rules to accumulate.
    //!
    //! \param val the new value of the batch size.
    //!
    //! \returns
    //! A reference to \c *this.
    //!
    //! \complexity
    //! Constant.
    KnuthBendix& max_pending_rules(size_t val) {
      _settings.max_pending_rules = val;
      return *this;
    }

    //! \brief Get the current number of rules to accumulate before processing.
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

    //! \brief Set the interval at which confluence is checked.
    //!
    //! The function \ref run periodically checks if the system is already
    //! confluent. This function can be used to set how frequently this happens,
    //! it is the number of new overlaps that should be considered before
    //! checking confluence. Setting this value too low can adversely affect the
    //! performance of \ref run.
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
    KnuthBendix& check_confluence_interval(size_t val) {
      _settings.check_confluence_interval = val;
      return *this;
    }

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
    KnuthBendix& max_overlap(size_t val) {
      _settings.max_overlap = val;
      return *this;
    }

    //! \brief Get the current maximum length of overlaps to be considered.
    //!
    //! This function returns the maximum length of the overlap of two left hand
    //! sides of rules that should be considered in \ref run.
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

    //! \brief Set the maximum number of rules.
    //!
    //! This function sets the (approximate) maximum number of rules
    //! that the system should contain. If this is number is exceeded in calls
    //! to \ref run or knuth_bendix_by_overlap_length, then they will terminate
    //! and the system may not be confluent.
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
    KnuthBendix& max_rules(size_t val) {
      _settings.max_rules = val;
      return *this;
    }

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
    //! This function returns the way that the length of an overlap of two words
    //! in the system is measured.
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
    [[nodiscard]] typename options::overlap overlap_policy() const noexcept {
      return _settings.overlap_policy;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - member functions for rules and rewriting - public
    //////////////////////////////////////////////////////////////////////////

    //! \brief Throws if any letter in a range is out of bounds.
    //!
    //! This function throws a LibsemigroupsException if any value pointed
    //! at by an iterator in the range \p first to \p last is out of bounds
    //! (i.e. does not belong to the alphabet of the \ref presentation used
    //! to construct the \ref Kambites instance).
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

    //! \brief Return the presentation defined by the rewriting system.
    //!
    //! This function returns the presentation defined by the rewriting system.
    //!
    //! \returns
    //! A const reference to the presentation, a value of type
    //! ``Presentation<std::string> const&``.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] Presentation<std::string> const&
    presentation() const noexcept {
      return _presentation;
    }

    //! \brief Return the current number of active rules in the KnuthBendix
    //! instance.
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
    // TODO(1) this should be const
    // TODO(1) add note about empty active rules after init and non-const-ness
    // (this only applies if this becomes const)
    [[nodiscard]] size_t number_of_active_rules() noexcept;

    //! \brief Return the current number of inactive rules in the KnuthBendix
    //! instance.
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
    [[nodiscard]] size_t number_of_inactive_rules() const noexcept {
      return _rewriter.number_of_inactive_rules();
    }

    //! \brief Return the number of rules that KnuthBendix has created
    //!
    //! This function returns the total number of Rule instances that have been
    //! created whilst whilst the Knuth-Bendix algorithm has been running. Note
    //! that this is not the sum of \ref number_of_active_rules and \ref
    //! number_of_inactive_rules, due to the re-initialisation of rules where
    //! possible.
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

    //! \brief Return a range object containing the active rules.
    //!
    //! This function returns a range object containing the pairs of
    //! strings which represent the rules of a KnuthBendix instance. The \c
    //! first entry in every such pair is greater than the \c second according
    //! to the reduction ordering of the KnuthBendix instance.
    //!
    //! \returns
    //! A range object containing the current active rules.
    // TODO(1) should be const
    // TODO(1) add note about empty active rules after, or better discuss that
    // there are three kinds of rules in the system: active, inactive, and
    // pending.
    // TODO(1) allow type of words output to be specified
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
    // KnuthBendix - main member functions - public
    //////////////////////////////////////////////////////////////////////////

    //! \brief Check confluence of the current rules.
    //!
    //! Check confluence of the current rules.
    //!
    //! \returns \c true if the KnuthBendix instance is
    //! [confluent](https://w.wiki/9DA) and \c false if it is not.
    [[nodiscard]] bool confluent() const;

    //! \brief Check if the current system knows the state of confluence of the
    //! current rules.
    //!
    //! Check if the current system knows the state of confluence of the
    //! current rules.
    //!
    //! \returns \c true if the confluence of the rules in the KnuthBendix
    //! instance is known, and \c false if it is not.
    [[nodiscard]] bool confluent_known() const noexcept;

    //! \brief Return the Gilman \ref WordGraph.
    //!
    //! This function returns the Gilman WordGraph of the system.
    //!
    //! The Gilman WordGraph is a digraph where the labels of the paths from
    //! the initial node (corresponding to the empty word) correspond to the
    //! shortlex normal forms of the semigroup elements.
    //!
    //! The semigroup is finite if the graph is cyclic, and infinite otherwise.
    //!
    //! \returns A const reference to a \ref
    //! WordGraph.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This will terminate when the KnuthBendix instance is reduced
    //! and confluent, which might be never.
    //!
    //! \sa \ref number_of_classes, and \ref knuth_bendix::normal_forms.
    WordGraph<uint32_t> const& gilman_graph();

    //! \brief Return the node labels of the Gilman \ref WordGraph
    //!
    //! This function returns the node labels of the Gilman \ref WordGraph,
    //! corresponding to the unique prefixes of the left-hand sides of the rules
    //! of the rewriting system.
    //!
    //! \return The node labels of the Gilman \ref WordGraph, a const reference
    //! to a ``std::vector<std::string>``.
    //!
    //! \sa \ref gilman_graph.
    [[nodiscard]] std::vector<std::string> const& gilman_graph_node_labels() {
      gilman_graph();  // to ensure that gilman_graph is initialised
      return _gilman_graph_node_labels;
    }

   private:
    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - private member functions
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
  };  // class KnuthBendix

// Special include for the KnuthBendixNormalFormRange, just to avoid cluttering
// this file.
#include "detail/knuth-bendix-nf.hpp"

  //! \ingroup knuth_bendix_group
  //!
  //! \brief Helper functions for the \ref KnuthBendix class.
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! This page contains documentation for some helper functions for the \ref
  //! KnuthBendix class. In particular, these functions include versions of
  //! several of the member functions of \ref KnuthBendix (that accept
  //! iterators) whose parameters are not iterators, but objects instead. The
  //! helpers documented on this page all belong to the namespace
  //! ``knuth_bendix``.
  //!
  //! \sa \ref cong_intf_helpers_group
  namespace knuth_bendix {

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix specific helpers
    ////////////////////////////////////////////////////////////////////////

    //! \brief Run the Knuth-Bendix algorithm by considering all overlaps of
    //! a given length.
    //!
    //! This function runs the Knuth-Bendix algorithm on the rewriting
    //! system represented by a KnuthBendix instance by considering all
    //! overlaps of a given length \f$n\f$ (according to the \ref
    //! KnuthBendix::options::overlap) before those overlaps of length \f$n +
    //! 1\f$.
    //!
    //! \tparam Rewriter the first template parameter for KnuthBendix.
    //! \tparam ReductionOrder the second template parameter for KnuthBendix.
    //!
    //! \param kb the KnuthBendix instance.
    //!
    //! \complexity
    //! See warning.
    //!
    //! \warning This will terminate when the KnuthBendix instance is
    //! confluent, which might be never.
    //!
    //! \sa \ref KnuthBendix::run.
    template <typename Rewriter, typename ReductionOrder>
    void by_overlap_length(KnuthBendix<Rewriter, ReductionOrder>& kb);

    //! \brief Check if the all rules are reduced with respect to each other.
    //!
    //! Defined in \c knuth-bendix.hpp.
    //!
    //! \tparam Rewriter the first template parameter for KnuthBendix.
    //! \tparam ReductionOrder the second template parameter for KnuthBendix.
    //!
    //! \param kb the KnuthBendix instance defining the rules that are to be
    //! checked for being reduced.
    //!
    //! \returns \c true if for each pair \f$(A, B)\f$ and \f$(C, D)\f$ of rules
    //! stored within the KnuthBendix instance, \f$C\f$ is neither a subword of
    //! \f$A\f$ nor \f$B\f$. Returns \c false otherwise.
    template <typename Rewriter, typename ReductionOrder>
    [[nodiscard]] bool is_reduced(KnuthBendix<Rewriter, ReductionOrder>& kb);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::add_generating_pair;
    using congruence_interface::add_generating_pair_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::contains;
    using congruence_interface::contains_no_checks;
    using congruence_interface::currently_contains;
    using congruence_interface::currently_contains_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::reduce;
    using congruence_interface::reduce_no_checks;
    using congruence_interface::reduce_no_run;
    using congruence_interface::reduce_no_run_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns a range object containing the normal forms.
    //!
    //! Defined in \c knuth-bendix.hpp.
    //!
    //! This function returns a range object containing normal forms of the
    //! classes of the congruence represented by an instance of KnuthBendix. The
    //! order of the classes, and the normal form that is returned, are
    //! controlled by the reduction order used to construct \p kb. This function
    //! triggers a full enumeration of \p kb.
    //!
    //! \tparam Word the type of the words contained in the output range
    //! (default: std::string).
    //! \tparam Rewriter the first template parameter for KnuthBendix.
    //! \tparam ReductionOrder the second template parameter for KnuthBendix.
    //!
    //! \param kb the \ref KnuthBendix instance.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \cong_intf_warn_undecidable{Knuth-Bendix}.
    template <typename Word = std::string,
              typename Rewriter,
              typename ReductionOrder>
    [[nodiscard]] auto normal_forms(KnuthBendix<Rewriter, ReductionOrder>& kb) {
      return detail::KnuthBendixNormalFormRange<Word, Rewriter, ReductionOrder>(
          kb);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::partition;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::non_trivial_classes;

    //! \brief Find the non-trivial classes of the quotient of one KnuthBendix
    //! instance in another.
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
    //! \tparam Rewriter the first template parameter for KnuthBendix.
    //! \tparam ReductionOrder the second template parameter for KnuthBendix.
    //!
    //! \param kb1 the first \ref KnuthBendix instance.
    //! \param kb2 the second \ref KnuthBendix instance.
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
    //! \throws LibsemigroupsException if the \ref KnuthBendix::gilman_graph of
    //! \p kb1 has fewer nodes than that of \p kb2.
    //!
    //! \cong_intf_warn_undecidable{Knuth-Bendix}.
    template <typename Word = std::string,
              typename Rewriter,
              typename ReductionOrder>
    [[nodiscard]] std::vector<std::vector<Word>>
    non_trivial_classes(KnuthBendix<Rewriter, ReductionOrder>& kb1,
                        KnuthBendix<Rewriter, ReductionOrder>& kb2);

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - redundant_rule
    ////////////////////////////////////////////////////////////////////////

    //! \brief Return an iterator pointing at the left hand side of a redundant
    //! rule.
    //!
    //! Defined in \c knuth-bendix.hpp.
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
    //! pointing to \c p.rules.cend() is returned.
    //!
    //! \tparam Time type of the 2nd parameter (time to try running
    //! Knuth-Bendix).
    //! \param p the presentation.
    //! \param t time to run KnuthBendix for every omitted rule.
    //!
    //! \returns An iterator pointing at the left-hand side of a redundant rule
    //! of \c p.rules.cend().
    //!
    //! \warning The progress of the Knuth-Bendix algorithm may differ between
    //! different calls to this function even if the parameters are identical.
    //! As such this is non-deterministic, and may produce different results
    //! with the same input.
    template <typename Time>
    [[nodiscard]] std::vector<std::string>::const_iterator
    redundant_rule(Presentation<std::string> const& p, Time t);

    //! \brief Return an iterator pointing at the left hand side of a redundant
    //! rule.
    //!
    //! Defined in \c knuth-bendix.hpp.
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
    //! \tparam Word type of words in the Presentation
    //! \tparam Time type of the 2nd parameter (time to try running
    //! Knuth-Bendix).
    //! \param p the presentation.
    //! \param t time to run KnuthBendix for every omitted rule.
    //!
    //! \warning The progress of the Knuth-Bendix algorithm may differ between
    //! different calls to this function even if the parameters are identical.
    //! As such this is non-deterministic, and may produce different results
    //! with the same input.
    template <typename Word, typename Time>
    [[nodiscard]] auto redundant_rule(Presentation<Word> const& p, Time t) {
      auto pp = to_presentation<std::string>(p);
      return p.rules.cbegin()
             + std::distance(pp.rules.cbegin(), redundant_rule(pp, t));
    }

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - try_equal_to
    ////////////////////////////////////////////////////////////////////////

    // TODO(1) Doc
    // TODO(1) template std::string
    // TODO(1) re-include later
    // template <typename T>
    // inline tril try_equal_to(Presentation<std::string>& p,
    //                          std::string const&         lhs,
    //                          std::string const&         rhs,
    //                          T t = std::chrono::seconds(1));
  }  // namespace knuth_bendix

  ////////////////////////////////////////////////////////////////////////
  // global functions - to_human_readable_repr
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup knuth_bendix_group
  //!
  //! \brief Insert into std::ostream.
  //!
  //! This function allows a KnuthBendix object to be left shifted
  //! into a std::ostream, such as std::cout. The currently active rules
  //! of the system are represented in the output.
  //!
  //! \param os the output stream to insert into.
  //! \param kb the KnuthBendix object.
  //!
  //! \returns A reference to the first argument.
  template <typename Rewriter, typename ReductionOrder>
  std::ostream& operator<<(std::ostream&                                os,
                           KnuthBendix<Rewriter, ReductionOrder> const& kb);

  //! \ingroup knuth_bendix_group
  //!
  //! \brief Return a string representation of a KnuthBendix instance
  //!
  //! This function returns a string representation of a KnuthBendix instance,
  //! specifying the size of the underlying alphabet and the number of active
  //! rules.
  //!
  //! \tparam Rewriter the first template parameter for KnuthBendix.
  //! \tparam ReductionOrder the second template parameter for KnuthBendix.
  //!
  //! \param kb the KnuthBendix instance.
  //!
  //! \returns The representation, a value of type \c std::string
  // TODO(1) preferably kb would be a const&
  template <typename Rewriter, typename ReductionOrder>
  std::string to_human_readable_repr(KnuthBendix<Rewriter, ReductionOrder>& kb);

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a KnuthBendix object.
  //!
  //! This function constructs and returns a Presentation object using the
  //! currently active rules of \p kb.
  //!
  //! No enumeration of the argument \p kb is performed, so it might be the
  //! case that the resulting presentation does not define the same
  //! semigroup/monoid as \p kb. To ensure that the resulting presentation
  //! defines the same semigroup as \p kb, run KnuthBendix::run (or any other
  //! function that fully enumerates \p kb) prior to calling this function.
  //!
  //! \tparam Word the type of the rules in the presentation being constructed.
  //!
  //! \param kb the KnuthBendix object from which to obtain the rules.
  //!
  //! \returns An object of type \c Presentation<Word>.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  // This cannot go into to-presentation.hpp since we include that here
  template <typename Word, typename Rewriter, typename ReductionOrder>
  Presentation<Word> to_presentation(KnuthBendix<Rewriter, ReductionOrder>& kb);
}  // namespace libsemigroups

#include "knuth-bendix.tpp"

#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_HPP_
