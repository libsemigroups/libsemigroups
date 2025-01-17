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

#ifndef LIBSEMIGROUPS_KNUTH_BENDIX_BASE_HPP_
#define LIBSEMIGROUPS_KNUTH_BENDIX_BASE_HPP_

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

#include "cong-intf-class.hpp"  // for CongruenceInte...
#include "constants.hpp"        // for POSITIVE_INFINITY
#include "debug.hpp"            // for LIBSEMIGROUPS_...
#include "obvinf.hpp"           // for is_obviously_infinite
#include "order.hpp"            // for ShortLexCompare
#include "presentation.hpp"     // for operator!=
#include "ranges.hpp"           // for count, iterato...
#include "to-presentation.hpp"  // for to_presentation
#include "types.hpp"            // for congruence_kind
#include "word-graph.hpp"       // for WordGraph, to_...
#include "word-range.hpp"       // for to_human_reada...

#include "detail/fmt.hpp"                // for format, print
#include "detail/multi-string-view.hpp"  // for MultiStringView
#include "detail/report.hpp"             // for report_no_prefix
#include "detail/rewriters.hpp"          // for Rule, internal...
#include "detail/string.hpp"             // for group_digits
#include "detail/timer.hpp"              // for string_time

namespace libsemigroups {
  // Forward declarations
  namespace detail {
    template <typename KnuthBendix_>
    class KBE;
  }  // namespace detail

  //! \defgroup knuth_bendix_group Knuth-Bendix
  //!
  //! This page contains links to the documentation related to the
  //! implementation of the Knuth-Bendix algorithm in `libsemigroups`.

  //! \ingroup knuth_bendix_group
  //!
  //! \brief Class containing an implementation of the Knuth-Bendix Algorithm.
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! On this page we describe the functionality relating to the Knuth-Bendix
  //! algorithm for semigroups and monoids in \c libsemigroups. This page
  //! contains details of the member functions of the class KnuthBendixBase.
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
  //! KnuthBendixBase kb(twosided, p);
  //!
  //! !kb.confluent();              // true
  //! kb.run();
  //! kb.number_of_active_rules();  // 8
  //! kb.confluent();               // true
  //! kb.number_of_classes();       // POSITIVE_INFINITY
  //! \endcode
  template <typename Rewriter       = detail::RewriteTrie,
            typename ReductionOrder = ShortLexCompare>
  class KnuthBendixBase : public CongruenceInterface {
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
    using rule_type = std::pair<std::string, std::string>;

    //! \brief Type of the letters in the relations of the presentation stored
    //! in a \ref KnuthBendixBase instance.
    //!
    //! A \ref KnuthBendixBase instance can be constructed or initialised from a
    //! presentation of arbitrary types of letters and words. Internally the
    //! letters are converted to \ref native_letter_type.
    using native_letter_type = char;

    //! \brief Type of the words in the relations of the presentation stored in
    //! a \ref KnuthBendixBase instance.
    //!
    //! A \ref KnuthBendixBase instance can be constructed or initialised from a
    //! presentation with arbitrary types of letters and words. Internally the
    //! words are converted to \ref native_word_type.
    using native_word_type = std::string;

    //! \brief Type of the presentation stored in a \ref KnuthBendixBase
    //! instance.
    //!
    //! A \ref KnuthBendixBase instance can be constructed or initialised from a
    //! presentation of arbitrary types of letters and words. Internally the
    //! presentation is stored as a \ref native_presentation_type.
    using native_presentation_type = Presentation<std::string>;

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixBase - types - public
    //////////////////////////////////////////////////////////////////////////

    //! \brief Struct containing various options that can be used to control the
    //! behaviour of Knuth-Bendix.
    //!
    //! This struct containing various options that can be used to control the
    //! behaviour of Knuth-Bendix.
    struct options {
      //! \brief Values for specifying how to measure the length of an overlap.
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

    //! \brief Default constructor.
    //!
    //! Constructs a KnuthBendixBase instance with no rules, and the short-lex
    //! reduction ordering.
    //!
    //! This function default constructs an uninitialised \ref
    //! KnuthBendixBase instance.
    explicit KnuthBendixBase();

    //! \brief Remove the presentation and rewriter data
    //!
    //! This function clears the rewriter, presentation, settings and stats from
    //! the KnuthBendixBase object, putting it back into the state it would be
    //! in if it was newly default constructed.
    //!
    //! \returns
    //! A reference to \c this.
    KnuthBendixBase& init();

    //! \brief Copy constructor.
    //!
    //! Copy constructor.
    //!
    //! \param that the KnuthBendixBase instance to copy.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
    //! rules of \p that.
    KnuthBendixBase(KnuthBendixBase const& that);

    //! \brief Move constructor.
    //!
    //! Move constructor.
    KnuthBendixBase(KnuthBendixBase&&);

    //! \brief Copy assignment operator.
    //!
    //! Copy assignment operator.
    KnuthBendixBase& operator=(KnuthBendixBase const&);

    //! \brief Move assignment operator.
    //!
    //! Move assignment operator.
    KnuthBendixBase& operator=(KnuthBendixBase&&);

    ~KnuthBendixBase();

    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a \ref KnuthBendixBase instance representing a
    //! congruence of kind \p knd over the semigroup or monoid defined by the
    //! presentation \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    KnuthBendixBase(congruence_kind knd, Presentation<std::string> const& p);

    //! \brief Re-initialize a \ref KnuthBendixBase instance.
    //!
    //! This function puts a \ref KnuthBendixBase instance back into the state
    //! that it would have been in if it had just been newly constructed from \p
    //! knd and \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    KnuthBendixBase& init(congruence_kind                  knd,
                          Presentation<std::string> const& p);

    //! \copydoc KnuthBendixBase(congruence_kind, Presentation<std::string>
    //! const&)
    KnuthBendixBase(congruence_kind knd, Presentation<std::string>&& p);

    //! \copydoc init(congruence_kind, Presentation<std::string> const&)
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
    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented by a
    //! \ref KnuthBendixBase instance.
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
    // NOTE THAT this is not the same as in ToddCoxeterBase, because the
    // generating pairs contained in CongruenceInterface are word_types, and so
    // we don't require any conversion here (since chars can be converted
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
      return CongruenceInterface::add_internal_generating_pair_no_checks<
          KnuthBendixBase>(first1, last1, first2, last2);
    }

    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented by a
    //! \ref KnuthBendixBase instance.
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
    KnuthBendixBase& add_generating_pair(Iterator1 first1,
                                         Iterator2 last1,
                                         Iterator3 first2,
                                         Iterator4 last2) {
      return CongruenceInterface::add_generating_pair<KnuthBendixBase>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendixBase - interface requirements - number_of_classes
    ////////////////////////////////////////////////////////////////////////

    //! \brief Compute the number of classes in the congruence.
    //!
    //! This function computes the number of classes in the congruence
    //! represented by a \ref KnuthBendixBase instance by
    //! running the congruence enumeration until it terminates.
    //!
    //! \returns The number of congruences classes of a \ref KnuthBendixBase
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
    // KnuthBendixBase - interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to be
    //! contained in the congruence represented by a \ref KnuthBendixBase
    //! instance. This function performs no enumeration, so it is possible for
    //! the words to be contained in the congruence, but that this is not
    //! currently known.
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
    //! contained in the congruence represented by a \ref KnuthBendixBase
    //! instance. This function performs no enumeration, so it is possible for
    //! the words to be contained in the congruence, but that this is not
    //! currently known.
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
      return CongruenceInterface::currently_contains<KnuthBendixBase>(
          first1, last1, first2, last2);
    }

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref KnuthBendixBase instance. This function
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
      return CongruenceInterface::contains_no_checks<KnuthBendixBase>(
          first1, last1, first2, last2);
    }

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref KnuthBendixBase
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
      return CongruenceInterface::contains<KnuthBendixBase>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendixBase - interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

    //! \brief Reduce a word with no run and no checks.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator \p
    //! d_first. This function triggers no enumeration. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref KnuthBendixBase instance. If the \ref KnuthBendixBase instance is
    //! \ref finished, then the output word is a normal form for the input word.
    //! If the \ref KnuthBendixBase instance is not \ref finished, then it might
    //! be that equivalent input words produce different output words.
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
    //! \ref KnuthBendixBase instance. If the \ref KnuthBendixBase instance is
    //! \ref finished, then the output word is a normal form for the input word.
    //! If the \ref KnuthBendixBase instance is not \ref finished, then it might
    //! be that equivalent input words produce different output words.
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
      return CongruenceInterface::reduce_no_run<KnuthBendixBase>(
          d_first, first, last);
    }

    //! \brief Reduce a word with no checks.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref KnuthBendixBase instance. In other words, the output word is a
    //! normal form for the input word or equivalently a canconical
    //! representative of its congruence class.
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
      return CongruenceInterface::reduce_no_checks<KnuthBendixBase>(
          d_first, first, last);
    }

    //! \brief Reduce a word.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref KnuthBendixBase instance. In other words, the output word is a
    //! normal form for the input word or equivalently a canconical
    //! representative of its congruence class.
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
      return CongruenceInterface::reduce<KnuthBendixBase>(d_first, first, last);
    }

    // TODO(1) implement reduce_inplace x4 if possible.

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixBase - setters for optional parameters - public
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
    KnuthBendixBase& max_pending_rules(size_t val) {
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
    KnuthBendixBase& check_confluence_interval(size_t val) {
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
    KnuthBendixBase& max_overlap(size_t val) {
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
    KnuthBendixBase& max_rules(size_t val) {
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
    KnuthBendixBase& overlap_policy(typename options::overlap val);

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
    // KnuthBendixBase - member functions for rules and rewriting - public
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
      internal_presentation().validate_word(first, last);
    }

    //! \brief Return the presentation defined by the rewriting system.
    //!
    //! This function returns the presentation defined by the rewriting system.
    //!
    //! \returns
    //! A const reference to the presentation, a value of type
    //! `Presentation<std::string> const&`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] native_presentation_type const&
    internal_presentation() const noexcept {
      return _presentation;
    }

    //! \brief Get the generating pairs of the congruence.
    //!
    //! This function returns the generating pairs of the congruence. The words
    //! comprising the generating pairs are converted to \ref native_word_type
    //! as they are added via \ref add_generating_pair. This function returns
    //! the std::vector of these \ref native_word_type.
    //!
    //! \returns The std::vector of generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<native_word_type> const&
    generating_pairs() const noexcept {
      return _input_generating_pairs;
    }

    //! \brief Return the current number of active rules in the KnuthBendixBase
    //! instance.
    //!
    //! This function returns the current number of active rules in the
    //! KnuthBendixBase instance.
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

    //! \brief Return the current number of inactive rules in the
    //! KnuthBendixBase instance.
    //!
    //! This function returns the current number of inactive rules in the
    //! KnuthBendixBase instance.
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

    //! \brief Return the number of rules that KnuthBendixBase has created
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
    //! strings which represent the rules of a KnuthBendixBase instance. The \c
    //! first entry in every such pair is greater than the \c second according
    //! to the reduction ordering of the KnuthBendixBase instance.
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
    // KnuthBendixBase - main member functions - public
    //////////////////////////////////////////////////////////////////////////

    //! \brief Check confluence of the current rules.
    //!
    //! Check confluence of the current rules.
    //!
    //! \returns \c true if the KnuthBendixBase instance is
    //! [confluent](https://w.wiki/9DA) and \c false if it is not.
    [[nodiscard]] bool confluent() const;

    //! \brief Check if the current system knows the state of confluence of the
    //! current rules.
    //!
    //! Check if the current system knows the state of confluence of the
    //! current rules.
    //!
    //! \returns \c true if the confluence of the rules in the KnuthBendixBase
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
    //! \warning This will terminate when the KnuthBendixBase instance is
    //! reduced and confluent, which might be never.
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
    //! to a `std::vector<std::string>`.
    //!
    //! \sa \ref gilman_graph.
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

  ////////////////////////////////////////////////////////////////////////
  // global functions - to_human_readable_repr
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup knuth_bendix_group
  //!
  //! \brief Insert into std::ostream.
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! This function allows a KnuthBendixBase object to be left shifted into a
  //! std::ostream, such as std::cout. The currently active rules of the system
  //! are represented in the output.
  //!
  //! \param os the output stream to insert into.
  //! \param kb the KnuthBendixBase object.
  //!
  //! \returns A reference to the first argument.
  template <typename Rewriter, typename ReductionOrder>
  std::ostream& operator<<(std::ostream&                                    os,
                           KnuthBendixBase<Rewriter, ReductionOrder> const& kb);

  //! \ingroup knuth_bendix_group
  //!
  //! \brief Return a string representation of a KnuthBendixBase instance
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! This function returns a string representation of a KnuthBendixBase
  //! instance, specifying the size of the underlying alphabet and the number of
  //! active rules.
  //!
  //! \tparam Rewriter the first template parameter for KnuthBendixBase.
  //! \tparam ReductionOrder the second template parameter for KnuthBendixBase.
  //!
  //! \param kb the KnuthBendixBase instance.
  //!
  //! \returns The representation, a value of type \c std::string
  // TODO(1) preferably kb would be a const&
  template <typename Rewriter, typename ReductionOrder>
  std::string
  to_human_readable_repr(KnuthBendixBase<Rewriter, ReductionOrder>& kb);

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a KnuthBendixBase object.
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! This function constructs and returns a Presentation object using the
  //! currently active rules of \p kb.
  //!
  //! No enumeration of the argument \p kb is performed, so it might be the
  //! case that the resulting presentation does not define the same
  //! semigroup/monoid as \p kb. To ensure that the resulting presentation
  //! defines the same semigroup as \p kb, run KnuthBendixBase::run (or any
  //! other function that fully enumerates \p kb) prior to calling this
  //! function.
  //!
  //! \tparam Word the type of the rules in the presentation being constructed.
  //!
  //! \param kb the KnuthBendixBase object from which to obtain the rules.
  //!
  //! \returns An object of type \c Presentation<Word>.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  // This cannot go into to-presentation.hpp since we include that here
  template <typename Word, typename Rewriter, typename ReductionOrder>
  Presentation<Word>
  to_presentation(KnuthBendixBase<Rewriter, ReductionOrder>& kb);
}  // namespace libsemigroups

#include "knuth-bendix-base.tpp"

#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_BASE_HPP_
