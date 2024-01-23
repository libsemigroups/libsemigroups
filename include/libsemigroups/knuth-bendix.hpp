//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 James D. Mitchell
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

// TODO:
// * noexcept
// * fix doc
// * template Rules
// * template Order
// * Separate rule container from Rules

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
#include "order.hpp"            // for ShortLexCompare
#include "paths.hpp"            // for Paths
#include "presentation.hpp"     // for Presentation
#include "ranges.hpp"           // for operator<<
#include "rewriters.hpp"        // for RewriteTrie
#include "runner.hpp"           // for Runner
#include "to-presentation.hpp"  // for to_presentation
#include "types.hpp"            // for word_type
#include "word-graph.hpp"       // for WordGraph
#include "words.hpp"            // for to_strings

#include "detail/multi-string-view.hpp"  // for MultiStringView
#include "detail/report.hpp"             // for Reporter, REPORT_DEFAULT, REP...
#include "detail/string.hpp"             // for is_prefix, maximum_common_prefix

#include "ranges.hpp"  // for iterator_range

namespace libsemigroups {
  // Forward declarations
  namespace detail {
    template <typename KnuthBendix_>
    class KBE;
  }  // namespace detail

  //! Defined in ``knuth-bendix.hpp``.
  //!
  //! On this page we describe the functionality relating to the Knuth-Bendix
  //! algorithm for semigroups and monoids that is available in
  //! ``libsemigroups``. This page contains details of the member functions
  //! of the class KnuthBendix.
  //!
  //! This class is used to represent a
  //! [string rewriting system](https://w.wiki/9Re)
  //! defining a finitely presented monoid or semigroup.
  //!
  //! \par Example
  //! \code
  //! KnuthBendix kb;
  //! kb.set_alphabet("abc");
  //!
  //! kb.add_rule("aaaa", "a");
  //! kb.add_rule("bbbb", "b");
  //! kb.add_rule("cccc", "c");
  //! kb.add_rule("abab", "aaa");
  //! kb.add_rule("bcbc", "bbb");
  //!
  //! !kb.confluent();       // true
  //! kb.run();
  //! kb.number_of_active_rules();  // 31
  //! kb.confluent();        // true
  //! \endcode
  template <typename Rewriter       = RewriteTrie,
            typename ReductionOrder = ShortLexCompare>
  class KnuthBendix : public CongruenceInterface {
    // defined in detail/kbe.hpp
    friend class ::libsemigroups::detail::KBE<KnuthBendix>;

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - typedefs/aliases - private
    ////////////////////////////////////////////////////////////////////////

    using external_string_type = std::string;
    using internal_string_type = std::string;
    using external_char_type   = char;
    using internal_char_type   = char;

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - nested subclasses - private
    ////////////////////////////////////////////////////////////////////////

    // Overlap measures
    struct OverlapMeasure {
      virtual size_t operator()(Rule const*,
                                Rule const*,
                                internal_string_type::const_iterator const&)
          = 0;
      virtual ~OverlapMeasure() {}
    };

    struct ABC;
    struct AB_BC;
    struct MAX_AB_BC;

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - friend declarations - private
    //////////////////////////////////////////////////////////////////////////

   public:
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
      //! \sa overlap_policy(options::overlap)
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
    } _settings;

    // TODO remove mutable
    mutable struct Stats {
      using time_point = std::chrono::high_resolution_clock::time_point;
      Stats() noexcept;
      Stats& init() noexcept;

      Stats(Stats const&) noexcept            = default;
      Stats(Stats&&) noexcept                 = default;
      Stats& operator=(Stats const&) noexcept = default;
      Stats& operator=(Stats&&) noexcept      = default;

      size_t                                   max_stack_depth;
      size_t                                   max_word_length;
      size_t                                   max_active_word_length;
      size_t                                   max_active_rules;
      size_t                                   min_length_lhs_rule;
      size_t                                   prev_active_rules;
      size_t                                   prev_inactive_rules;
      size_t                                   prev_total_rules;
      size_t                                   total_rules;
      std::unordered_set<internal_string_type> unique_lhs_rules;
    } _stats;

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - data - private
    ////////////////////////////////////////////////////////////////////////

    Rewriter                  _rewriter;
    bool                      _gen_pairs_initted;
    WordGraph<size_t>         _gilman_graph;
    std::vector<std::string>  _gilman_graph_node_labels;
    bool                      _internal_is_same_as_external;
    OverlapMeasure*           _overlap_measure;
    Presentation<std::string> _presentation;

   public:
    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - constructors and destructor - public
    //////////////////////////////////////////////////////////////////////////

    //! \brief Default constructor.
    //!
    //! Constructs a KnuthBendix instance with no rules, and the short-lex
    //! reduction ordering.
    //!
    //! \parameters
    //! (None)
    //!
    //! \complexity
    //! Constant.
    explicit KnuthBendix(congruence_kind knd);

    //! Remove the presentation and rewriter data
    //!
    //! This function clears the rewriter, presentation, settings and stats from
    //! the KnuthBendix object, putting it back into the state it would be in if
    //! it was newly constructed.
    //!
    //! \param (None)
    //!
    //! \returns
    //! A reference to `this`.
    KnuthBendix& init(congruence_kind knd);

    //! Copy constructor.
    //!
    //! \param copy the KnuthBendix instance to copy.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
    //! rules of \p copy.
    KnuthBendix(KnuthBendix const& copy);

    // TODO doc
    KnuthBendix(KnuthBendix&&);

    // TODO doc
    KnuthBendix& operator=(KnuthBendix const&);

    // TODO doc
    KnuthBendix& operator=(KnuthBendix&&);

    ~KnuthBendix();

    // TODO doc
    KnuthBendix(congruence_kind knd, Presentation<std::string> const& p)
        : KnuthBendix(knd) {
      private_init(knd, p, false);  // false means don't call init, since we
                                    // just called it from KnuthBendix()
    }

    // TODO doc
    KnuthBendix& init(congruence_kind knd, Presentation<std::string> const& p);

    // TODO doc
    KnuthBendix(congruence_kind knd, Presentation<std::string>&& p)
        : KnuthBendix(knd) {
      private_init(knd,
                   std::move(p),
                   false);  // false means don't call init, since we just
                            // called it from KnuthBendix()
    }

    // TODO doc
    KnuthBendix& init(congruence_kind knd, Presentation<std::string>&& p);

    // TODO doc
    template <typename Word>
    explicit KnuthBendix(congruence_kind knd, Presentation<Word> const& p)
        : KnuthBendix(knd, to_presentation<std::string>(p)) {}

    // TODO doc
    template <typename Word>
    explicit KnuthBendix(congruence_kind knd, Presentation<Word>&& p)
        : KnuthBendix(knd, to_presentation<std::string>(p)) {}

    // TODO doc
    template <typename Word>
    KnuthBendix& init(congruence_kind knd, Presentation<Word> const& p) {
      init(knd, to_presentation<std::string>(p));
      return *this;
    }

    // TODO doc
    template <typename Word>
    KnuthBendix& init(congruence_kind knd, Presentation<Word>&& p) {
      init(knd, to_presentation<std::string>(p));
      return *this;
    }

   private:
    KnuthBendix& private_init(congruence_kind                  knd,
                              Presentation<std::string> const& p,
                              bool                             call_init);
    KnuthBendix& private_init(congruence_kind             knd,
                              Presentation<std::string>&& p,
                              bool                        call_init);

    void init_from_generating_pairs();
    void init_from_presentation();

   public:
    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - setters for optional parameters - public
    //////////////////////////////////////////////////////////////////////////

    //! Set the number of rules to accumulate before they are processed.
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
    //! A reference to \c *this
    //!
    //! \complexity
    //! Constant.
    KnuthBendix& batch_size(size_t val) {
      _settings.batch_size = val;
      return *this;
    }

    //! Return the number of rules to accumulate before they are processed.
    //!
    //! This function can be used to return the number of pending rules that
    //! must accumulate before they are reduced, processed, and added to the
    //! system.
    //!
    //! The default value is \c 128.
    //!
    //! \returns
    //! The batch size, a value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    [[nodiscard]] size_t batch_size() const noexcept {
      return _settings.batch_size;
    }

    //! Set the interval at which confluence is checked.
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

    // TODO doc
    [[nodiscard]] size_t check_confluence_interval() const noexcept {
      return _settings.check_confluence_interval;
    }

    //! Set the maximum length of overlaps to be considered.
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

    // TODO doc
    [[nodiscard]] size_t max_overlap() const noexcept {
      return _settings.max_overlap;
    }

    //! Set the maximum number of rules.
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

    // TODO doc
    [[nodiscard]] size_t max_rules() const noexcept {
      return _settings.max_rules;
    }

    //! Set the overlap policy.
    //!
    //! This function can be used to determine the way that the length
    //! of an overlap of two words in the system is measured.
    //!
    //! \param val the maximum number of rules.
    //!
    //! \returns
    //! A reference to \c *this.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa options::overlap.
    KnuthBendix& overlap_policy(typename options::overlap val);

    // TODO doc
    [[nodiscard]] typename options::overlap overlap_policy() const noexcept {
      return _settings.overlap_policy;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - member functions for rules and rewriting - public
    //////////////////////////////////////////////////////////////////////////

    // TODO doc
    void validate_word(word_type const& w) const override {
      std::string s = to_string(presentation(), w);
      return presentation().validate_word(s.cbegin(), s.cend());
    }

    // TODO doc
    [[nodiscard]] Presentation<std::string> const&
    presentation() const noexcept {
      return _presentation;
    }

    // TODO doc
    // TODO required?
    KnuthBendix& presentation(Presentation<std::string> const& p) {
      throw_if_started();
      return private_init(kind(), p, false);
    }

    // TODO doc
    // TODO required?
    KnuthBendix& presentation(Presentation<std::string>&& p) {
      throw_if_started();
      return private_init(kind(), std::move(p), false);
    }

    // TODO doc
    // TODO required?
    template <typename Word>
    KnuthBendix& presentation(Presentation<Word> const& p) {
      throw_if_started();
      return private_init(kind(), to_presentation<std::string>(p), false);
    }

    // TODO doc
    // TODO required?
    template <typename Word>
    KnuthBendix& presentation(Presentation<Word>&& p) {
      throw_if_started();
      return private_init(kind(), to_presentation<std::string>(p), false);
    }

    //! Returns the current number of active rules in the KnuthBendix
    //! instance.
    //!
    //! \returns
    //! The current number of active rules, a value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    [[nodiscard]] size_t number_of_active_rules() const noexcept;

    // TODO doc
    [[nodiscard]] size_t number_of_inactive_rules() const noexcept {
      return _rewriter.number_of_inactive_rules();
    }

    [[nodiscard]] size_t total_rules() const noexcept {
      return _rewriter.stats().total_rules;
    }

    //! Returns a copy of the active rules.
    //!
    //! This member function returns a vector consisting of the pairs of
    //! strings which represent the rules of the KnuthBendix instance. The \c
    //! first entry in every such pair is greater than the \c second according
    //! to the reduction ordering of the KnuthBendix instance. The rules are
    //! sorted according to the reduction ordering used by the rewriting
    //! system, on the first entry.
    //!
    //! \returns
    //! A copy of the currently active rules, a value of type
    //! `std::vector<rule_type>`.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
    //! rules of \p copy.
    //!
    //! \parameters
    //! (None)
    using rule_type = std::pair<std::string, std::string>;
    // TODO update the doc, now returns a Range
    [[nodiscard]] auto active_rules() const {
      using rx::iterator_range;
      using rx::transform;
      return iterator_range(_rewriter.begin(), _rewriter.end())
             | transform([this](auto const& rule) {
                 // TODO remove allocation
                 internal_string_type lhs = internal_string_type(*rule->lhs());
                 internal_string_type rhs = internal_string_type(*rule->rhs());
                 internal_to_external_string(lhs);
                 internal_to_external_string(rhs);
                 if (this->kind() == congruence_kind::left) {
                   std::reverse(lhs.begin(), lhs.end());
                   std::reverse(rhs.begin(), rhs.end());
                 }
                 return std::make_pair(lhs, rhs);
               });
    }

    //! Rewrite a word in-place.
    //!
    //! The word \p w is rewritten in-place according to the current active
    //! rules in the KnuthBendix instance.
    //!
    //! \param w the word to rewrite.
    //!
    //! \returns
    //! The argument \p w after it has been rewritten.
    // TODO update doc
    void rewrite_inplace(std::string& w) const;

    //! Rewrite a word.
    //!
    //! Rewrites a copy of the word \p w rewritten according to the current
    //! rules in the KnuthBendix instance.
    //!
    //! \param w the word to rewrite.
    //!
    //! \returns
    //! A copy of the argument \p w after it has been rewritten.
    [[nodiscard]] std::string rewrite(std::string w) const {
      rewrite_inplace(w);
      return w;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - main member functions - public
    //////////////////////////////////////////////////////////////////////////
    //! Doc
    void pre_run();

    //! Check confluence of the current rules.
    //!
    //! \returns \c true if the KnuthBendix instance is
    //! [confluent](https://w.wiki/9DA) and \c false if it is not.
    //!
    //! \parameters
    //! (None)
    [[nodiscard]] bool confluent() const;

    //! Check if the current system knows the state of confluence of the current
    //! rules.
    //!
    //! \returns \c true if the confluence of the rules in the KnuthBendix
    //! instance is known, and \c false if it is not.
    //!
    //! \parameters
    //! (None)
    [[nodiscard]] bool confluent_known() const noexcept;

    //! Returns the Gilman digraph.
    //!
    //! \returns A const reference to a \ref WordGraph.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This will terminate when the KnuthBendix instance is
    //! reduced and confluent, which might be never.
    //!
    //! \sa \ref number_of_normal_forms,
    //! \ref cbegin_normal_forms, and \ref cend_normal_forms./
    //!
    //! \parameters
    //! (None)
    WordGraph<size_t> const& gilman_graph();

    [[nodiscard]] std::vector<std::string> const& gilman_graph_node_labels() {
      gilman_graph();  // to ensure that gilman_graph is initialised
      return _gilman_graph_node_labels;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - attributes - public
    //////////////////////////////////////////////////////////////////////////

    //! \copydoc FpSemigroupInterface::size TODO copy the doc
    //!
    //! \note If \c this has been run until finished, then this function can
    //! determine the size of the semigroup represented by \c this even if
    //! it is infinite. Moreover, the complexity of this function is at
    //! worst \f$O(mn)\f$ where \f$m\f$ is the number of letters in the
    //! alphabet, and \f$n\f$ is the number of nodes in the \ref
    //! gilman_graph.
    [[nodiscard]] uint64_t number_of_classes() override;

    // TODO doc
    [[nodiscard]] bool equal_to(std::string const&, std::string const&);

    // TODO doc
    [[nodiscard]] bool contains(word_type const& u,
                                word_type const& v) override {
      return equal_to(to_string(presentation(), u),
                      to_string(presentation(), v));
    }

    // TODO doc
    [[nodiscard]] bool contains(std::initializer_list<letter_type> u,
                                std::initializer_list<letter_type> v) {
      return contains(word_type(u), word_type(v));
    }

    // No in-place version just use rewrite instead, this only exists so that
    // run is called.
    // TODO required?
    [[nodiscard]] std::string normal_form(std::string const& w);

   private:
    void report_presentation(Presentation<std::string> const&) const;
    void report_before_run() const;
    void report_progress_from_thread(std::atomic_bool const&) const;
    void report_after_run() const;

    void throw_if_started() const;
    void stats_check_point() const;

    [[nodiscard]] static internal_char_type uint_to_internal_char(size_t a);
    [[nodiscard]] static size_t internal_char_to_uint(internal_char_type c);

    [[nodiscard]] static internal_string_type uint_to_internal_string(size_t i);

    [[nodiscard]] static word_type
    internal_string_to_word(internal_string_type const& s);

    [[nodiscard]] internal_char_type
    external_to_internal_char(external_char_type c) const;
    [[nodiscard]] external_char_type
    internal_to_external_char(internal_char_type a) const;

    void external_to_internal_string(external_string_type& w) const;
    void internal_to_external_string(internal_string_type& w) const;

    void add_octo(external_string_type& w) const;
    void rm_octo(external_string_type& w) const;

    void add_rule_impl(std::string const& p, std::string const& q);

    void overlap(Rule const* u, Rule const* v);

    [[nodiscard]] size_t max_active_word_length() const;

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

  KnuthBendix(congruence_kind)->KnuthBendix<>;

  namespace knuth_bendix {

    //! Run the Knuth-Bendix by considering all overlaps of a given length.
    //!
    //! This function runs the Knuth-Bendix algorithm on the rewriting
    //! system represented by a KnuthBendix instance by considering all
    //! overlaps of a given length \f$n\f$ (according to the \ref
    //! options::overlap) before those overlaps of length \f$n + 1\f$.
    //!
    //! \returns
    //! (None)
    //!
    //! \complexity
    //! See warning.
    //!
    //! \warning This will terminate when the KnuthBendix instance is
    //! confluent, which might be never.
    //!
    //! \sa \ref run.
    //!
    //! \parameters
    //! (None)
    template <typename Rewriter, typename ReductionOrder>
    void by_overlap_length(KnuthBendix<Rewriter, ReductionOrder>&);

    //! Returns a forward iterator pointing at the first normal form with
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
    //! \sa
    //! \ref cend_normal_forms.
    // TODO update doc
    template <typename Rewriter, typename ReductionOrder>
    [[nodiscard]] inline auto
    normal_forms(KnuthBendix<Rewriter, ReductionOrder>& kb) {
      using rx::      operator|;
      ReversiblePaths paths(kb.gilman_graph());
      paths.from(0).reverse(kb.kind() == congruence_kind::left);
      return paths;
    }

    // Compute non-trivial classes in kb1!
    template <typename Rewriter, typename ReductionOrder>
    [[nodiscard]] std::vector<std::vector<std::string>>
    non_trivial_classes(KnuthBendix<Rewriter, ReductionOrder>& kb1,
                        KnuthBendix<Rewriter, ReductionOrder>& kb2);

    //! Return an iterator pointing at the left hand side of a redundant rule.
    //!
    //! This function is defined in ``knuth-bendix.hpp``.
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
    //! pointing to `p.cend()` is returned.
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
      KnuthBendix kb(twosided);

      for (auto omit = p.rules.crbegin(); omit != p.rules.crend(); omit += 2) {
        q.rules.clear();
        q.rules.insert(q.rules.end(), p.rules.crbegin(), omit);
        q.rules.insert(q.rules.end(), omit + 2, p.rules.crend());
        kb.init(twosided, q);
        kb.run_for(t);
        if (kb.rewrite(*omit) == kb.rewrite(*(omit + 1))) {
          return (omit + 1).base() - 1;
        }
      }
      return p.rules.cend();
    }

    //! Check if the all rules are reduced with respect to each other.
    //!
    //! This function is defined in ``knuth-bendix.hpp``.
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

          if (rule.first.find(lhs) != internal_string_type::npos
              || rule.second.find(lhs) != internal_string_type::npos) {
            return false;
          }
        }
      }
      return true;
    }

    template <typename T>
    inline tril try_equal_to(Presentation<std::string>& p,
                             std::string const&         lhs,
                             std::string const&         rhs,
                             T t = std::chrono::seconds(1)) {
      constexpr static congruence_kind twosided = congruence_kind::twosided;

      KnuthBendix         kb(twosided, p);
      std::string         lphbt = p.alphabet();
      std::vector<size_t> perm(lphbt.size(), 0);
      std::iota(perm.begin(), perm.end(), 0);

      do {
        detail::apply_permutation(lphbt, perm);

        p.alphabet(lphbt);
        p.validate();

        kb.init(twosided, p);
        if (kb.rewrite(lhs) == kb.rewrite(rhs)) {
          return tril::TRUE;
        }
        kb.run_for(t);
        if (kb.rewrite(lhs) == kb.rewrite(rhs)) {
          return tril::TRUE;
        } else if (kb.finished()) {
          return tril::FALSE;
        }

      } while (std::next_permutation(perm.begin(), perm.end()));
      return tril::unknown;
    }

    //! Return an iterator pointing at the left hand side of a redundant rule.
    //!
    //! This function is defined in ``knuth-bendix.hpp``.
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
    //! pointing to `p.cend()` is returned.
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
  }  // namespace knuth_bendix

  template <typename Rewriter, typename ReductionOrder, typename Range>
  [[nodiscard]] std::vector<std::vector<std::string>>
  partition(KnuthBendix<Rewriter, ReductionOrder>& kb, Range r) {
    static_assert(
        std::is_same_v<std::decay_t<typename Range::output_type>, std::string>);
    using return_type = std::vector<std::vector<std::string>>;
    using rx::operator|;

    if (!r.is_finite) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd argument (a range) must be finite, "
                              "found an infinite range");
    }

    return_type result;

    std::unordered_map<std::string, size_t> map;
    size_t                                  index = 0;

    while (!r.at_end()) {
      auto next = r.get();
      if (kb.presentation().contains_empty_word() || !next.empty()) {
        auto next_nf        = kb.rewrite(next);
        auto [it, inserted] = map.emplace(next_nf, index);
        if (inserted) {
          result.emplace_back();
          index++;
        }
        size_t index_of_next_nf = it->second;
        result[index_of_next_nf].push_back(next);
      }
      r.next();
    }
    return result;
  }

  // TODO to tpp file
  template <typename Word, typename Rewriter, typename ReductionOrder>
  Presentation<Word>
  to_presentation(KnuthBendix<Rewriter, ReductionOrder> const& kb) {
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

}  // namespace libsemigroups

#include "knuth-bendix.tpp"

#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_HPP_
