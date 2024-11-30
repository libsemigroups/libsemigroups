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
// TODO(0)
// * fix doc
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
#include "words.hpp"            // for to_strings

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

  //! \defgroup knuth_bendix_class_group The Knuth-Bendix class
  //!
  //! This page TODO

  // TODO(0) update the description
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! On this page we describe the functionality relating to the Knuth-Bendix
  //! algorithm for semigroups and monoids that is available in
  //! \c libsemigroups. This page contains details of the member functions
  //! of the class KnuthBendix.
  //!
  //! This class is used to represent a
  //! [string rewriting system](https://w.wiki/9Re)
  //! defining a finitely presented monoid or semigroup.
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
    // KnuthBendix - typedefs/aliases - private
    ////////////////////////////////////////////////////////////////////////

    // using external_string_type = std::string;
    // using internal_string_type = std::string;
    // using external_char_type   = char;
    // using internal_char_type   = char;

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

    using native_letter_type       = char;
    using native_word_type         = std::string;
    using native_presentation_type = Presentation<std::string>;

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - types - public
    //////////////////////////////////////////////////////////////////////////

    //! This type contains various enums for specifying certain options to a
    //! KnuthBendix instance.
    struct options {
      //! Values for specifying how to measure the length of an overlap.
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

      size_t                    batch_size;
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
    //! \param knd The kind of congruence to be used. Either \c left, \c right
    //! or \c twosided.
    //!
    //! \complexity
    //! Constant.
    explicit KnuthBendix();

    //! \brief Remove the presentation and rewriter data
    //!
    //! This function clears the rewriter, presentation, settings and stats from
    //! the KnuthBendix object, putting it back into the state it would be in if
    //! it was newly constructed.
    //!
    //! \param knd The kind of congruence to be used. Either \c left, \c right
    //! or \c twosided.
    //!
    //! \returns
    //! A reference to \c this.
    KnuthBendix& init();

    //! \brief Copy constructor.
    //!
    //! Copy constructor.
    //!
    //! \param copy the KnuthBendix instance to copy.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
    //! rules of \p copy.
    KnuthBendix(KnuthBendix const& that);

    // TODO(0) doc
    KnuthBendix(KnuthBendix&&);

    // TODO(0) doc
    KnuthBendix& operator=(KnuthBendix const&);

    // TODO(0) doc
    KnuthBendix& operator=(KnuthBendix&&);

    ~KnuthBendix();

    // TODO(0) doc
    KnuthBendix(congruence_kind knd, Presentation<std::string> const& p);

    // TODO(0) doc
    KnuthBendix& init(congruence_kind knd, Presentation<std::string> const& p);

    // TODO(0) doc
    KnuthBendix(congruence_kind knd, Presentation<std::string>&& p);

    // TODO(0) doc
    KnuthBendix& init(congruence_kind knd, Presentation<std::string>&& p);

    // TODO(0) doc
    // No rvalue ref version because we can't use it.
    template <typename Word>
    explicit KnuthBendix(congruence_kind knd, Presentation<Word> const& p)
        : KnuthBendix(knd, to_presentation<std::string>(p, [](auto const& x) {
                        return x;
                      })) {}

    // TODO(0) doc
    template <typename Word>
    KnuthBendix& init(congruence_kind knd, Presentation<Word> const& p) {
      init(knd,
           to_presentation<std::string>(p, [](auto const& x) { return x; }));
      return *this;
    }

   private:
    void init_from_generating_pairs();
    void init_from_presentation();

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - add_generating_pair
    //////////////////////////////////////////////////////////////////////////

   public:
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

    //!
    //! \note If \c this has been run until finished, then this function can
    //! determine the size of the semigroup represented by \c this even if it is
    //! infinite. Moreover, the complexity of this function is at worst
    //! \f$O(mn)\f$ where \f$m\f$ is the number of letters in the alphabet, and
    //! \f$n\f$ is the number of nodes in the \ref gilman_graph.
    [[nodiscard]] uint64_t number_of_classes();

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) should be const, isn't because rewrite_inplace isn't
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] tril currently_contains_no_checks(Iterator1 first1,
                                                    Iterator2 last1,
                                                    Iterator3 first2,
                                                    Iterator4 last2) const {
      if (std::equal(first1, last1, first2, last2)) {
        return tril::TRUE;
      }
      // TODO(1) remove allocations here
      std::string w1, w2;
      reduce_no_run_no_checks(std::back_inserter(w1), first1, last1);
      reduce_no_run_no_checks(std::back_inserter(w2), first2, last2);
      if (w1 == w2) {
        return tril::TRUE;
      } else if (finished()) {
        return tril::FALSE;
      }
      return tril::unknown;
    }

    // TODO(0) should be const, isn't because rewrite_inplace isn't
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

    // TODO(0) should be const
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                           InputIterator1 first,
                                           InputIterator2 last) const {
      // TODO(1) improve this to not require _tmp_element1
      if constexpr (std::is_same_v<InputIterator1, char const*>) {
        static_assert(std::is_same_v<InputIterator2, char const*>);
        _tmp_element1.assign(first, std::distance(first, last));
      } else {
        _tmp_element1.assign(first, last);
      }
      const_cast<KnuthBendix<Rewriter, ReductionOrder>&>(*this).rewrite_inplace(
          _tmp_element1);
      return std::copy(
          std::begin(_tmp_element1), std::end(_tmp_element1), d_first);
    }

    // TODO(0) should be const
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 InputIterator1 first,
                                 InputIterator2 last) const {
      throw_if_letter_out_of_bounds(first, last);
      return reduce_no_run_no_checks(d_first, first, last);
    }

    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    InputIterator1 first,
                                    InputIterator2 last) {
      run();
      return reduce_no_run_no_checks(d_first, first, last);
    }

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
    //!
    //! \sa \ref run and \ref process_pending_rules.
    KnuthBendix& batch_size(size_t val) {
      _settings.batch_size = val;
      return *this;
    }

    //! \brief Return the number of rules to accumulate before processing.
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
    //!
    //! \sa \ref run and \ref process_pending_rules.
    [[nodiscard]] size_t batch_size() const noexcept {
      return _settings.batch_size;
    }

    //! \brief Set the interval at which confluence is checked.
    //!
    //! The function \ref run periodically checks if
    //! the system is already confluent. This function can be used to
    //! set how frequently this happens, it is the number of new overlaps
    //! that should be considered before checking confluence. Setting this
    //! value too low can adversely affect the performance of
    //! \ref run.
    //!
    //! The default value is \c 4096, and should be set to
    //! \ref LIMIT_MAX if \ref run should never
    //! check if the system is already confluent.
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

    //! \brief Return the interval at which confluence is checked.
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

    //! \brief Return the maximum length of overlaps to be considered.
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
    //! This member function sets the (approximate) maximum number of rules
    //! that the system should contain. If this is number is exceeded in
    //! calls to \ref run or
    //! knuth_bendix_by_overlap_length, then they
    //! will terminate and the system may not be confluent.
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

    //! \brief Return the maximum number of rules.
    //!
    //! This member function returns the (approximate) maximum number of rules
    //! that the system should contain. If this is number is exceeded in
    //! calls to \ref run or \ref knuth_bendix::by_overlap_length, then they
    //! will terminate and the system may not be confluent.
    //!
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

    //! \brief Return the overlap policy.
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

    //! \brief Check if every letter of a word is in the presentation's
    //! alphabet.
    //!
    //! Check if every letter of a word is in the presentation's
    //! alphabet.
    //!
    //! \throws LibsemigroupsException if there is a letter of \p w not in the
    //! presentations alphabet.
    //!
    //! \param w word to validate.
    //!
    //! \sa Presentation::validate_word.
    // TODO(0) remove
    void validate_word(word_type const& w) const {
      ToString    to_string(presentation().alphabet());
      std::string s = to_string(w);
      presentation().validate_word(s.cbegin(), s.cend());
    }

    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      presentation().validate_word(first, last);
    }

    //! \brief Return the presentation defined by the rewriting system
    //!
    //! Return the presentation defined by the rewriting system
    //!
    //! \returns
    //! A const reference to the presentation, a value of type
    //! <tt>Presentation<std::string> const&</tt>.
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

    // TODO(0) add note about empty active rules after init and non-const-ness
    //! \brief Return the current number of active
    //! rules in the KnuthBendix instance.
    //!
    //! Return the current number of active rules in
    //! the KnuthBendix instance.
    //!
    //! \returns
    //! The current number of active rules, a value
    //! of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t number_of_active_rules() noexcept;

    //! \brief Return the current number of inactive
    //! rules in the KnuthBendix instance.
    //!
    //! Return the current number of inactive rules
    //! in the KnuthBendix instance.
    //!
    //! \returns
    //! The current number of inactive rules, a
    //! value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    [[nodiscard]] size_t number_of_inactive_rules() const noexcept {
      return _rewriter.number_of_inactive_rules();
    }

    //! \brief Return the number of rules that
    //! KnuthBendix has created
    //!
    //! Return the total number of Rule instances
    //! that have been created whilst whilst the
    //! Knuth-Bendix algorithm has been running.
    //! Note that this is not the sum of \ref
    //! number_of_active_rules and \ref
    //! number_of_inactive_rules, due to the
    //! re-initialisation of rules where possible.
    //!
    //! \returns
    //! The total number of rules, a value of type
    //! \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t total_rules() const noexcept {
      return _rewriter.stats().total_rules;
    }

    // TODO(0) What do we do about doc-ing this?
    //! Type of the rules in the system.
    using rule_type = std::pair<std::string, std::string>;

    // TODO(0) update the doc, now returns a Range
    // TODO(0) add note about empty active rules after
    // init
    //! \brief Return a copy of the active rules.
    //!
    //! This member function returns a vector consisting of the pairs of
    //! strings which represent the rules of the KnuthBendix instance. The \c
    //! first entry in every such pair is greater than the \c second according
    //! to the reduction ordering of the KnuthBendix instance. The rules are
    //! sorted according to the reduction ordering used by the rewriting system,
    //! on the first entry.
    //!
    //! \returns
    //! A copy of the currently active rules, a
    //! value of type
    //! \c std::vector<rule_type>.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the sum of the
    //! lengths of the words in rules of \p copy.
    [[nodiscard]] auto active_rules() {
      using rx::iterator_range;
      using rx::transform;
      if (_rewriter.number_of_active_rules() == 0
          && _rewriter.number_of_pending_rules() != 0) {
        _rewriter.process_pending_rules();
      }
      return iterator_range(_rewriter.begin(), _rewriter.end())
             | transform([this](auto const& rule) {
                 // TODO(1) remove allocation
                 detail::internal_string_type lhs
                     = detail::internal_string_type(*rule->lhs());
                 detail::internal_string_type rhs
                     = detail::internal_string_type(*rule->rhs());
                 internal_to_external_string(lhs);
                 internal_to_external_string(rhs);
                 return std::make_pair(lhs, rhs);
               });
    }

   private:
    // TODO(0) recycle this doc or delete
    // TODO add note about empty active rules after
    // init and non-const-ness
    //! \brief Rewrite a word in-place.
    //!
    //! The word \p w is rewritten in-place
    //! according to the current active rules in the
    //! KnuthBendix instance.
    //!
    //! \param w the word to rewrite.
    //!
    //! \returns
    //! The argument \p w after it has been
    //! rewritten.
    // TODO update doc
    void rewrite_inplace(std::string& w);

    // TODO(0) recycle this doc or delete
    // TODO add note about empty active rules after
    // init and non-const-ness
    //! \brief Rewrite a word.
    //!
    //! Rewrites a copy of the word \p w rewritten according to the current
    //! rules in the KnuthBendix instance.
    //!
    //! \param w the word to rewrite.
    //!
    //! \returns
    //! A copy of the argument \p w after it has
    //! been rewritten.
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

    // REVIEW None of these \sa's exist anymore. Should it be \ref
    // number_of_classes and \ref normal_forms?
    //! \brief Return the Gilman \ref WordGraph.
    //!
    //! Return the Gilman WordGraph of the system.
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
    //! \sa \ref number_of_normal_forms, \ref cbegin_normal_forms, and \ref
    //! cend_normal_forms.
    WordGraph<uint32_t> const& gilman_graph();

    //! \brief Return the node labels of the Gilman
    //! \ref WordGraph
    //!
    //! Return the node labels of the Gilman \ref WordGraph, corresponding to
    //! the unique prefixes of the left-hand sides of the rules of the rewriting
    //! system.
    //!
    //! \return The node labels of the Gilman \ref WordGraph, a value of type
    //! \c std::vector<std::string>.
    //!
    //! \sa \ref gilman_graph.
    [[nodiscard]] std::vector<std::string> const& gilman_graph_node_labels() {
      gilman_graph();  // to ensure that gilman_graph is initialised
      return _gilman_graph_node_labels;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - attributes - public
    //////////////////////////////////////////////////////////////////////////
    // TODO(0) recycle or delete
    //! \brief Check if two inputs are equivalent with respect to the system
    //!
    //! By first testing \c string equivalence, then by rewriting the inputs,
    //! then by running the Knuth-Bendix algorithm and rewriting the inputs with
    //! respect to the updated system again, check if \p u and \p v are
    //! equivalent.
    //!
    //! \param u, v the words to test the
    //! equivalence of.
    //!
    //! \returns \c true if \p u is equivalent to \p v, and \c false otherwise.
    //!
    //! \warning If the inputs don't rewrite to equivalent words with the
    //! initial rewriting rules, then the Knuth-Bendix algorithm is run. This
    //! terminates when the rewriting system is confluent, which may be never.
    //!
    //! \sa run.

    // TODO(0) recycle or delete
    //! \brief Check containment
    //!
    //! Check if the pair of words \p u and \p v is
    //! contained in within the congruence
    //! corresponding to rewriting system.
    //!
    //! \param u, v the words to check containment
    //! of.
    //!
    //! \returns \c true if the the pair consisting
    //! of \p u and \p v is contained within the
    //! congruence, and \c false otherwise.
    //!
    //! \sa \ref equal_to.

    // No in-place version just use rewrite instead,
    // this only exists so that run is called.

   private:
    void report_presentation(Presentation<std::string> const&) const;
    void report_before_run();
    void report_progress_from_thread(std::atomic_bool const&);
    void report_after_run();

    // TODO(0) remove this use the version in CongruenceInterface
    void throw_if_started() const;
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

    //////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////

    void               run_impl() override;
    void               run_real(std::atomic_bool&);
    bool               finished_impl() const override;
    [[nodiscard]] bool stop_running() const;
  };

  //! This friend function allows a KnuthBendix object to be left shifted
  //! into a std::ostream, such as std::cout. The currently active rules
  //! of the system are represented in the output.
  template <typename Rewriter, typename ReductionOrder>
  std::ostream& operator<<(std::ostream&,
                           KnuthBendix<Rewriter, ReductionOrder> const&);

  namespace detail {
    // TODO put in separate file
    template <typename Word, typename Rewriter, typename ReductionOrder>
    class KnuthBendixNormalFormRange : public Paths<uint32_t> {
      using Paths_ = Paths<uint32_t>;

      mutable Word                           _current;
      KnuthBendix<Rewriter, ReductionOrder>* _kb;

     public:
      using size_type   = typename Paths_::size_type;
      using output_type = Word const&;

      explicit KnuthBendixNormalFormRange(
          KnuthBendix<Rewriter, ReductionOrder>& kb)
          : Paths(kb.gilman_graph()), _current(), _kb(&kb) {
        // It's possible that the gilman graph is empty, so the call to
        // source_no_checks(0) is technically invalid, but nothing goes wrong,
        // so we just go with it. This is slightly smelly.
        Paths_::source_no_checks(0);
        if (!kb.presentation().contains_empty_word()) {
          Paths_::next();
        }
      }

      output_type get() const {
        word_type const& w = Paths_::get();
        _current.clear();
        for (auto c : w) {
          _current.push_back(_kb->presentation().letter_no_checks(c));
        }
        return _current;
      }

      KnuthBendixNormalFormRange& min(size_type val) noexcept {
        Paths_::min(val);
        return *this;
      }

      KnuthBendixNormalFormRange& max(size_type val) noexcept {
        Paths_::max(val);
        return *this;
      }

      using Paths_::at_end;
      using Paths_::count;
      using Paths_::max;
      using Paths_::min;
      using Paths_::next;
      using Paths_::size_hint;

      static constexpr bool is_finite     = true;  // this isn't always true!
      static constexpr bool is_idempotent = true;
    };  // class KnuthBendixNormalFormRange

  }  // namespace detail

  namespace knuth_bendix {

    //! \defgroup knuth_bendix_helpers_group Knuth-Bendix specific helpers
    //!
    //! TODO
    //! @{

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix specific helpers
    ////////////////////////////////////////////////////////////////////////

    // TODO Should this have a param?
    //! \brief Run the Knuth-Bendix algorithm by considering all overlaps of
    //! a given length.
    //!
    //! This function runs the Knuth-Bendix algorithm on the rewriting
    //! system represented by a KnuthBendix instance by considering all
    //! overlaps of a given length \f$n\f$ (according to the \ref
    //! options::overlap) before those overlaps of length \f$n + 1\f$.
    //!
    //! \complexity
    //! See warning.
    //!
    //! \warning This will terminate when the KnuthBendix instance is
    //! confluent, which might be never.
    //!
    //! \sa \ref run.
    template <typename Rewriter, typename ReductionOrder>
    void by_overlap_length(KnuthBendix<Rewriter, ReductionOrder>&);

    //! \brief Check if the all rules are reduced with respect to each other.
    //!
    //! This function is defined in \c knuth-bendix.hpp.
    //!
    //! \returns \c true if for each pair \f$(A, B)\f$ and \f$(C, D)\f$ of rules
    //! stored within the KnuthBendix instance, \f$C\f$ is neither a subword of
    //! \f$A\f$ nor \f$B\f$. Returns \c false otherwise.
    //!
    //! \tparam Rewriter type of the rewriting system to be used in the
    //! Knuth-Bendix algorithm.
    //! \tparam ReductionOrder type of the reduction ordering used by the
    //! Knuth-Bendix algorithm.
    //! \param kb the KnuthBendix instance defining the rules that are to be
    //! checked for being reduced.
    template <typename Rewriter, typename ReductionOrder>
    [[nodiscard]] bool is_reduced(KnuthBendix<Rewriter, ReductionOrder>& kb) {
      for (auto const& test_rule : kb.active_rules()) {
        auto const lhs = test_rule.first;
        for (auto const& rule : kb.active_rules()) {
          if (test_rule == rule) {
            continue;
          }

          if (rule.first.find(lhs) != detail::internal_string_type::npos
              || rule.second.find(lhs) != detail::internal_string_type::npos) {
            return false;
          }
        }
      }
      return true;
    }

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

    //! \brief Return a forward iterator pointing at the first normal form with
    //! length in a given range.
    //!
    //! If incremented, the iterator will point to the next least short-lex
    //! normal form (if it's less than \p max in length).  Iterators of the
    //! type returned by this function should only be compared with other
    //! iterators created from the same KnuthBendix instance.
    //!
    //! \param lphbt the alphabet to use for the normal forms
    //! \param min the minimum length of a normal form
    //! \param max one larger than the maximum length of a normal form.
    //!
    //! \returns
    //! A value of type \ref const_normal_form_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! Copying iterators of this type is relatively expensive.  As a
    //! consequence, prefix incrementing \c ++it the iterator \c it returned
    //! by \c cbegin_normal_forms is significantly cheaper than postfix
    //! incrementing \c it++.
    //!
    //! \warning
    //! If the finitely presented semigroup represented by \c this is
    //! infinite, then \p max should be chosen with some care.
    //!
    //! \sa \ref cend_normal_forms.
    // TODO update doc
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

    // Compute non-trivial classes in kb1!
    template <typename Rewriter,
              typename ReductionOrder,
              typename Word = std::string>
    [[nodiscard]] std::vector<std::vector<Word>>
    non_trivial_classes(KnuthBendix<Rewriter, ReductionOrder>& kb1,
                        KnuthBendix<Rewriter, ReductionOrder>& kb2);

    // TODO(0) move all of the functions from here to the end of the namespace
    // knuth_bendix to before the interface helpers when they are out of lined.

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - try_equal_to
    ////////////////////////////////////////////////////////////////////////

    // TODO Doc
    template <typename T>
    inline tril try_equal_to(Presentation<std::string>& p,
                             std::string const&         lhs,
                             std::string const&         rhs,
                             T t = std::chrono::seconds(1)) {
      constexpr static congruence_kind twosided = congruence_kind::twosided;

      // TODO validate lhs and rhs
      KnuthBendix         kb(twosided, p);
      std::string         lphbt = p.alphabet();
      std::vector<size_t> perm(lphbt.size(), 0);
      std::iota(perm.begin(), perm.end(), 0);

      do {
        detail::apply_permutation(lphbt, perm);

        p.alphabet(lphbt);
        p.validate();

        kb.init(twosided, p);
        // TODO(0) no checks
        if (reduce_no_run(kb, lhs) == reduce_no_run(kb, rhs)) {
          return tril::TRUE;
        }
        kb.run_for(t);
        // TODO(0) no checks
        if (reduce_no_run(kb, lhs) == reduce_no_run(kb, rhs)) {
          return tril::TRUE;
        } else if (kb.finished()) {
          return tril::FALSE;
        }
      } while (std::next_permutation(perm.begin(), perm.end()));
      return tril::unknown;
    }

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - redundant_rule
    ////////////////////////////////////////////////////////////////////////

    //! \brief Return an iterator pointing at the left hand side of a redundant
    //! rule.
    //!
    //! This function is defined in \c knuth-bendix.hpp.
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
    //! \tparam T type of the 2nd parameter (time to try running
    //! Knuth-Bendix). \param p the presentation \param t time to run
    //! KnuthBendix for every omitted rule
    //!
    //! \warning The progress of the Knuth-Bendix algorithm may differ between
    //! different calls to this function even if the parameters are identical.
    //! As such this is non-deterministic, and may produce different results
    //! with the same input.
    template <typename T>
    [[nodiscard]] auto redundant_rule(Presentation<std::string> const& p, T t) {
      constexpr static congruence_kind twosided = congruence_kind::twosided;

      p.validate();
      Presentation<std::string> q;
      q.alphabet(p.alphabet());
      q.contains_empty_word(p.contains_empty_word());
      KnuthBendix kb;

      for (auto omit = p.rules.crbegin(); omit != p.rules.crend(); omit += 2) {
        q.rules.clear();
        q.rules.insert(q.rules.end(), p.rules.crbegin(), omit);
        q.rules.insert(q.rules.end(), omit + 2, p.rules.crend());
        kb.init(twosided, q);
        kb.run_for(t);
        // TODO no_checks
        if (reduce_no_run(kb, *omit) == reduce_no_run(kb, *(omit + 1))) {
          return (omit + 1).base() - 1;
        }
      }
      return p.rules.cend();
    }

    //! \brief Return an iterator pointing at the left hand side of a redundant
    //! rule.
    //!
    //! This function is defined in \c knuth-bendix.hpp.
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
    //! \tparam W type of words in the Presentation
    //! \tparam T type of the 2nd parameter (time to try running
    //! Knuth-Bendix). \param p the presentation \param t time to run
    //! KnuthBendix for every omitted rule
    //!
    //! \warning The progress of the Knuth-Bendix algorithm may differ between
    //! different calls to this function even if the parameters are identical.
    //! As such this is non-deterministic, and may produce different results
    //! with the same input.
    template <typename W, typename T>
    [[nodiscard]] auto redundant_rule(Presentation<W> const& p, T t) {
      auto pp = to_presentation<std::string>(p);
      return p.rules.cbegin()
             + std::distance(pp.rules.cbegin(), redundant_rule(pp, t));
    }
    //! @}
  }  // namespace knuth_bendix

  // TODO to tpp file
  // TODO to to_presentation file
  template <typename Word, typename Rewriter, typename ReductionOrder>
  Presentation<Word>
  to_presentation(KnuthBendix<Rewriter, ReductionOrder>& kb) {
    if constexpr (std::is_same_v<Word, std::string>) {
      auto const&               p_orig = kb.presentation();
      Presentation<std::string> p;
      p.alphabet(p_orig.alphabet())
          .contains_empty_word(p_orig.contains_empty_word());

      for (auto const& rule : kb.active_rules()) {
        presentation::add_rule(p, rule.first, rule.second);
      }
      return p;
    } else {
      return to_presentation<Word>(to_presentation<std::string>(kb));
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // global functions - to_human_readable_repr
  ////////////////////////////////////////////////////////////////////////

  // TODO(0) What should the \param be?
  //! \brief Return a string representation of a KnuthBendix instance
  //!
  //! Return a string representation of a KnuthBendix instance, specifying the
  //! size of the underlying alphabet and the number of active rules.
  //!
  //! \returns The representation, a value of type \c std::string
  // TODO(0) rename to_human_readable_repr
  template <typename Rewriter, typename ReductionOrder>
  std::string repr(KnuthBendix<Rewriter, ReductionOrder>& kb) {
    using str = std::string;

    str conf;
    if (!kb.confluent_known()) {
      conf = "KnuthBendix";
    } else if (kb.confluent()) {
      conf = "confluent KnuthBendix";
    } else {
      conf = "non-confluent KnuthBendix";
    }
    str alphabet_size = std::to_string(kb.presentation().alphabet().size());
    str n_rules       = std::to_string(kb.number_of_active_rules());

    // TODO(JE) use fmt::format instead
    return str("<") + conf + " on " + alphabet_size + " letters with " + n_rules
           + " active rules>";
  }
}  // namespace libsemigroups

#include "knuth-bendix.tpp"

#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_HPP_
