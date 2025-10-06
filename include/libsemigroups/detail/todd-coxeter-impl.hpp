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

// This file contains a declaration of a class for performing the Todd-Coxeter
// algorithm for semigroups and monoids.
//
// TODO(1)
// * re-implement reserve
// * remove preferred_defs from FelschGraph etc (except where they are really
// needed)? Or possibly reintroduce PrefDefs here

#ifndef LIBSEMIGROUPS_DETAIL_TODD_COXETER_IMPL_HPP_
#define LIBSEMIGROUPS_DETAIL_TODD_COXETER_IMPL_HPP_

#include <chrono>    // for nanoseconds
#include <cstddef>   // for size_t
#include <cstdint>   // for uint32_t
#include <iterator>  // for bidirect...
#include <memory>    // for unique_ptr
#include <numeric>   // for iota
#include <string>    // for string
#include <utility>   // for move, pair
#include <vector>    // for vector

#include "libsemigroups/constants.hpp"     // for operator!=
#include "libsemigroups/debug.hpp"         // for LIBSEMIG...
#include "libsemigroups/forest.hpp"        // for Forest
#include "libsemigroups/order.hpp"         // for Order
#include "libsemigroups/presentation.hpp"  // for Presenta...
#include "libsemigroups/ranges.hpp"        // for operator|
#include "libsemigroups/runner.hpp"        // for Reporter
#include "libsemigroups/types.hpp"         // for word_type
#include "libsemigroups/word-graph.hpp"    // for WordGraph

#include "cong-common-class.hpp"        // for Congruen...
#include "felsch-graph.hpp"             // for FelschGraph
#include "node-managed-graph.hpp"       // for NodeMana...
#include "node-manager.hpp"             // for NodeMana...
#include "word-graph-with-sources.hpp"  // for WordGrap...

////////////////////////////////////////////////////////////////////////
// This file is organised as follows:
// 0.  ToddCoxeterImpl - member types - public
// 1.  ToddCoxeterImpl - nested classes - private
// 2.  ToddCoxeterImpl - data members - private
// 3.  ToddCoxeterImpl - constructors + initializers - public
// 4.  ToddCoxeterImpl - interface requirements - add_generating_pair
// 5.  ToddCoxeterImpl - interface requirements - number_of_classes
// 6.  ToddCoxeterImpl - interface requirements - contains
// 7.  ToddCoxeterImpl - interface requirements - reduce
// 8.  ToddCoxeterImpl - settings - public
// 9.  ToddCoxeterImpl - accessors - public
// 10. ToddCoxeterImpl - modifiers - public
// 11. ToddCoxeterImpl - word -> index
// 12. ToddCoxeterImpl - index -> word
// 13. Runner          - pure virtual member functions - private
// 14. ToddCoxeterImpl - member functions - private
////////////////////////////////////////////////////////////////////////

namespace libsemigroups {

  // NOTE: groups are defined here because the order they are declared is the
  // order they appear in the output doc.

  //! \defgroup todd_coxeter_class_mem_types_group Member types
  //! \ingroup todd_coxeter_class_group
  //!
  //! \brief Public member types.
  //!
  //! This page contains the documentation of the public member types of a
  //! \ref_todd_coxeter instance.

  //! \defgroup todd_coxeter_class_init_group Constructors + initializers
  //! \ingroup todd_coxeter_class_group
  //!
  //! \brief Construct or re-initialize a \ref_todd_coxeter
  //! instance (public member function).
  //!
  //! This page documents the constructors and initialisers for the
  //! \ref_todd_coxeter class.
  //!
  //! Every constructor (except the move + copy constructors, and the move
  //! and copy assignment operators) has a matching `init` function with the
  //! same signature that can be used to re-initialize a \ref_todd_coxeter
  //! instance as if it had just been constructed; but without necessarily
  //! releasing any previous allocated memory.

  //! \defgroup todd_coxeter_class_settings_group Settings
  //! \ingroup todd_coxeter_class_group
  //!
  //! \brief Settings that control the behaviour of a
  //! \ref todd_coxeter_class_group  instance.
  //!
  //! This page contains information about the member functions of the
  //! \ref_todd_coxeter that control various settings that influence the
  //! congruence enumeration process.
  //!
  //! There are a fairly large number of settings, they can profoundly
  //! alter the run time of a congruence enumeration process, but it is hard
  //! to predict what settings will work best for any particular input.
  //!
  //! See also \ref Runner for further settings.

  //! \defgroup todd_coxeter_class_intf_group Common member functions
  //! \ingroup todd_coxeter_class_group
  //!
  //! \brief Documentation of common member functions of \ref_congruence,
  //! \ref_kambites, \ref_knuth_bendix, and \ref_todd_coxeter.
  //!
  //! This page contains documentation of the member functions of
  //! \ref_todd_coxeter that are implemented in all of the classes
  //! \ref_congruence, \ref_kambites, \ref_knuth_bendix, and \ref_todd_coxeter.

  //! \defgroup todd_coxeter_class_accessors_group Accessors
  //!
  //! \ingroup todd_coxeter_class_group
  //!
  //! \brief Member functions that can be used to access the state of a
  //! \ref_todd_coxeter instance.
  //!
  //! This page contains the documentation of the various member
  //! functions of the \ref_todd_coxeter
  //! class that can be used to access the state of an instance.
  //!
  //! Those functions with the prefix `current_` do not perform any
  //! further enumeration.

  //! \defgroup todd_coxeter_class_mod_group Modifiers
  //! \ingroup todd_coxeter_class_group
  //!
  //! \brief Member functions that can be used to modify the state of a
  //! \ref_todd_coxeter instance.
  //!
  //! This page contains documentation of the member functions of
  //! \ref_todd_coxeter that can be used to modify the state of a
  //! \ref_todd_coxeter instance. In other words, for modifying the WordGraph
  //! that is the output of the algorithm in a way that preserves it up to
  //! isomorphism.

  namespace detail {
    class ToddCoxeterImpl : public CongruenceCommon,
                            public FelschGraphSettings<ToddCoxeterImpl> {
      using FelschGraphSettings_ = FelschGraphSettings<ToddCoxeterImpl>;

     public:
      ////////////////////////////////////////////////////////////////////////
      // 0. ToddCoxeterImpl - member types - public
      ////////////////////////////////////////////////////////////////////////

      struct options : public FelschGraphSettings_::options {
        enum class strategy { hlt, felsch, CR, R_over_C, Cr, Rc };

        enum class lookahead_extent { full, partial };

        enum class lookahead_style { hlt, felsch };

        enum class def_policy : uint8_t {
          no_stack_if_no_space,
          purge_from_top,
          purge_all,
          discard_all_if_no_space,
          unlimited
        };
      };  // struct options

      enum class state : uint8_t { none, hlt, felsch, lookahead };

      using node_type        = typename WordGraph<uint32_t>::node_type;
      using index_type       = node_type;
      using label_type       = typename WordGraph<uint32_t>::label_type;
      using native_word_type = word_type;

     private:
      ////////////////////////////////////////////////////////////////////////
      // 1. ToddCoxeterImpl - nested classes - private
      ////////////////////////////////////////////////////////////////////////

      struct Settings;

      class SettingsGuard;
      friend class SettingsGuard;

      struct NonAtomicStats {
        using time_point = std::chrono::high_resolution_clock::time_point;

        // Data
        time_point               create_or_init_time;
        std::chrono::nanoseconds all_runs_time;

        std::chrono::nanoseconds all_hlt_phases_time;
        std::chrono::nanoseconds all_felsch_phases_time;
        std::chrono::nanoseconds all_lookahead_phases_time;

        uint64_t all_num_hlt_phases;
        uint64_t all_num_felsch_phases;
        uint64_t all_num_lookahead_phases;

        uint64_t   run_index;
        time_point run_start_time;

        uint64_t run_edges_active_at_start;
        uint64_t run_nodes_active_at_start;

        std::chrono::nanoseconds run_hlt_phases_time;
        std::chrono::nanoseconds run_felsch_phases_time;
        std::chrono::nanoseconds run_lookahead_phases_time;

        uint64_t run_num_hlt_phases;
        uint64_t run_num_felsch_phases;
        uint64_t run_num_lookahead_phases;

        uint64_t   phase_index;
        uint64_t   phase_edges_active_at_start;
        float      phase_complete_at_start;
        uint64_t   phase_nodes_defined_at_start;
        uint64_t   phase_nodes_killed_at_start;
        uint64_t   phase_nodes_active_at_start;
        time_point phase_start_time;

        mutable uint64_t report_index;
        mutable uint64_t report_edges_active_prev;
        mutable float    report_complete_prev;
        mutable uint64_t report_nodes_defined_prev;
        mutable uint64_t report_nodes_killed_prev;
        mutable uint64_t report_nodes_active_prev;

        // Constructors + initializers

        NonAtomicStats() {
          init();
        }

        NonAtomicStats& init();

        NonAtomicStats(NonAtomicStats const&)            = default;
        NonAtomicStats(NonAtomicStats&&)                 = default;
        NonAtomicStats& operator=(NonAtomicStats const&) = default;
        NonAtomicStats& operator=(NonAtomicStats&&)      = default;
      };

      struct Stats : public NonAtomicStats {
        // Data
        std::atomic_uint64_t lookahead_nodes_killed;
        std::atomic_uint64_t lookahead_position;

        // Constructors + initialisers

        Stats()
            : NonAtomicStats(),
              lookahead_nodes_killed(),
              lookahead_position() {}

        Stats& init() {
          NonAtomicStats::init();
          return *this;
        }

        Stats(Stats const& that)
            : NonAtomicStats(that),
              lookahead_nodes_killed(that.lookahead_nodes_killed.load()),
              lookahead_position(that.lookahead_position.load()) {}

        Stats(Stats&& that)
            : NonAtomicStats(std::move(that)),
              lookahead_nodes_killed(that.lookahead_nodes_killed.load()),
              lookahead_position(that.lookahead_position.load()) {}

        Stats& operator=(Stats const& that) {
          NonAtomicStats::operator=(that);
          lookahead_nodes_killed = that.lookahead_nodes_killed.load();
          lookahead_position     = that.lookahead_position.load();
          return *this;
        }

        Stats& operator=(Stats&& that) {
          NonAtomicStats::operator=(std::move(that));
          lookahead_nodes_killed = that.lookahead_nodes_killed.load();
          lookahead_position     = that.lookahead_position.load();
          return *this;
        }
      };

      void stats_run_start() {
        _stats.run_start_time = std::chrono::high_resolution_clock::now();

        _stats.run_nodes_active_at_start
            = current_word_graph().number_of_nodes_active();
        _stats.run_edges_active_at_start
            = current_word_graph().number_of_edges_active();
        _stats.run_num_hlt_phases       = 0;
        _stats.run_num_felsch_phases    = 0;
        _stats.run_num_lookahead_phases = 0;

        _stats.run_hlt_phases_time       = std::chrono::nanoseconds(0);
        _stats.run_felsch_phases_time    = std::chrono::nanoseconds(0);
        _stats.run_lookahead_phases_time = std::chrono::nanoseconds(0);

        _stats.phase_index = 0;
      }

      void stats_run_stop() {
        _stats.run_index++;

        _stats.all_runs_time += delta(_stats.run_start_time);
        _stats.all_num_hlt_phases += _stats.run_num_hlt_phases;
        _stats.all_num_felsch_phases += _stats.run_num_felsch_phases;
        _stats.all_num_lookahead_phases += _stats.run_num_lookahead_phases;

        _stats.all_hlt_phases_time += _stats.run_hlt_phases_time;
        _stats.all_felsch_phases_time += _stats.run_felsch_phases_time;
        _stats.all_lookahead_phases_time += _stats.run_lookahead_phases_time;
      }

      void stats_phase_start() {
        _stats.phase_start_time = std::chrono::high_resolution_clock::now();
        _stats.report_index     = 0;

        _stats.phase_nodes_active_at_start
            = current_word_graph().number_of_nodes_active();
        _stats.phase_nodes_killed_at_start
            = current_word_graph().number_of_nodes_killed();
        _stats.phase_nodes_defined_at_start
            = current_word_graph().number_of_nodes_defined();

        _stats.phase_edges_active_at_start
            = current_word_graph().number_of_edges_active();
        _stats.phase_complete_at_start
            = complete(current_word_graph().number_of_edges_active());
      }

      void stats_phase_stop() {
        _stats.phase_index++;

        switch (_state) {
          case state::none: {
            break;
          }
          case state::hlt: {
            _stats.run_num_hlt_phases++;
            _stats.run_hlt_phases_time += delta(_stats.phase_start_time);
            break;
          }
          case state::felsch: {
            _stats.run_num_felsch_phases++;
            _stats.run_felsch_phases_time += delta(_stats.phase_start_time);
            break;
          }
          case state::lookahead: {
            _stats.run_num_lookahead_phases++;
            _stats.run_lookahead_phases_time += delta(_stats.phase_start_time);
            break;
          }
          default: {
            break;
          }
        }
      }

      void stats_report_stop() const {
        _stats.report_index++;
      }

      // Simple struct that allows the "receivers" value to be set to "val" but
      // only when the object goes out of scope. Useful in reporting when
      // we want to do something with an old value, then update the data member
      // of Stats.
      class DeferSet {
        uint64_t& _receiver;
        uint64_t  _val;

       public:
        DeferSet(uint64_t& receiver, uint64_t val)
            : _receiver(receiver), _val(val) {}

        operator uint64_t() {
          return _val;
        }

        ~DeferSet() {
          _receiver = _val;
        }
      };

     public:
      ////////////////////////////////////////////////////////////////////////
      // ?. ToddCoxeterImpl - nested classes - public
      ////////////////////////////////////////////////////////////////////////

      class Definitions {
        using Definition = std::pair<node_type, label_type>;

       private:
        bool                    _any_skipped;
        std::vector<Definition> _definitions;
        ToddCoxeterImpl const*  _tc;

       public:
        Definitions() : _any_skipped(false), _definitions(), _tc(nullptr) {}
        // TODO(1) init()

        Definitions(Definitions const&)                 = default;
        Definitions(Definitions&&)                      = default;
        Definitions& operator=(Definitions const& that) = default;
        Definitions& operator=(Definitions&&)           = default;

        // TODO(1) corresponding constructor
        void init(ToddCoxeterImpl const* tc) {
          _any_skipped = false;
          _definitions.clear();
          _tc = tc;
        }

        void emplace_back(node_type c, label_type x);

        [[nodiscard]] bool any_skipped() const noexcept {
          return _any_skipped;
        }

        [[nodiscard]] bool empty() const noexcept {
          return _definitions.empty();
        }

        void pop(Definition& d) {
          d = std::move(_definitions.back());
          _definitions.pop_back();
        }

        void clear() {
          _definitions.clear();
        }

       private:
        bool is_active_node(node_type n) const noexcept {
          return _tc->current_word_graph().is_active_node(n);
        }
      };  // class Definitions

      class Graph
          : public FelschGraph<NodeManagedGraph<node_type>, Definitions> {
        using FelschGraph_
            = FelschGraph<NodeManagedGraph<node_type>, Definitions>;

       public:
        Graph()                        = default;
        Graph(Graph const&)            = default;
        Graph(Graph&&)                 = default;
        Graph& operator=(Graph const&) = default;
        Graph& operator=(Graph&&)      = default;

        Graph& operator=(WordGraph<node_type> const& wg) {
          FelschGraph_::operator=(wg);
          return *this;
        }

        using FelschGraph_::presentation;
        using FelschGraph_::target_no_checks;

        Graph& init();
        Graph& init(Presentation<word_type>&& p);

        Graph& init(Presentation<word_type> const& p,
                    WordGraph<node_type> const&    wg) {
          FelschGraph_::operator=(wg);
          FelschGraph_::presentation_no_checks(p);
          return *this;
        }

        Graph& presentation_no_checks(Presentation<word_type> const& p);

        void process_definitions();

        template <bool RegDefs>
        void push_definition_hlt(node_type const& c,
                                 word_type const& u,
                                 word_type const& v);

        template <typename Iterator>
        void make_compatible(ToddCoxeterImpl* tc,
                             node_type&       current,
                             Iterator         first,
                             Iterator         last,
                             bool             stop_early);

       private:
        void report_lookahead_stop_early(ToddCoxeterImpl* tc,
                                         size_t           expected,
                                         size_t           killed_last_interval);
      };  // class Graph

     private:
      ////////////////////////////////////////////////////////////////////////
      // 2. ToddCoxeterImpl - data members - private
      ////////////////////////////////////////////////////////////////////////

      bool                                   _finished;
      Forest                                 _forest;
      std::vector<std::unique_ptr<Settings>> _settings_stack;
      Order                                  _standardized;
      // _state must be atomic to avoid the situation where the ticker
      // function is called concurrently with a new lookahead starting
      std::atomic<state> _state;
      Stats              _stats;
      bool               _ticker_running;
      Graph              _word_graph;

     public:
      using word_graph_type = Graph;

      ////////////////////////////////////////////////////////////////////////
      // 3. ToddCoxeterImpl - constructors + initializers - public
      ////////////////////////////////////////////////////////////////////////

      ToddCoxeterImpl();
      ToddCoxeterImpl& init();

      ToddCoxeterImpl(ToddCoxeterImpl const& that);
      ToddCoxeterImpl(ToddCoxeterImpl&&);
      ToddCoxeterImpl& operator=(ToddCoxeterImpl const&);
      ToddCoxeterImpl& operator=(ToddCoxeterImpl&&);

      ~ToddCoxeterImpl();

      ToddCoxeterImpl(congruence_kind knd, Presentation<word_type>&& p);
      ToddCoxeterImpl& init(congruence_kind knd, Presentation<word_type>&& p);
      ToddCoxeterImpl(congruence_kind knd, Presentation<word_type> const& p);
      ToddCoxeterImpl& init(congruence_kind                knd,
                            Presentation<word_type> const& p);

      // TODO(1) a "to" function variant that throws if wg is not valid see
      // below
      template <typename Node>
      ToddCoxeterImpl(congruence_kind knd, WordGraph<Node> const& wg)
          : ToddCoxeterImpl() {
        LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
        init(knd, wg);
      }

      template <typename Node>
      ToddCoxeterImpl& init(congruence_kind knd, WordGraph<Node> const& wg);

      // TODO(1) rvalue ref WordGraph init + constructor

      ToddCoxeterImpl(congruence_kind knd, ToddCoxeterImpl const& tc);

      ToddCoxeterImpl& init(congruence_kind knd, ToddCoxeterImpl const& tc);

      ToddCoxeterImpl& presentation_no_checks(Presentation<word_type> const& p);

      // This is a constructor and not a helper so that everything that takes
      // a presentation has the same constructors, regardless of what they use
      // inside.

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
      // Used in Sims
      // TODO(1) could this and the next function be removed, and replaced
      // with something else?
      template <typename Node>
      ToddCoxeterImpl(congruence_kind                knd,
                      Presentation<word_type> const& p,
                      WordGraph<Node> const&         wg) {
        init(knd, p, wg);
      }

      // TODO(1) a to_todd_coxeter variant that throws if p is not valid
      template <typename Node>
      ToddCoxeterImpl& init(congruence_kind                knd,
                            Presentation<word_type> const& p,
                            WordGraph<Node> const&         wg);
#endif

      template <typename Iterator1, typename Iterator2>
      void throw_if_letter_not_in_alphabet(Iterator1 first,
                                           Iterator2 last) const {
        internal_presentation().throw_if_letter_not_in_alphabet(first, last);
      }

      ////////////////////////////////////////////////////////////////////////
      // 4. ToddCoxeterImpl - interface requirements - add_generating_pair
      ////////////////////////////////////////////////////////////////////////

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      ToddCoxeterImpl& add_generating_pair_no_checks(Iterator1 first1,
                                                     Iterator2 last1,
                                                     Iterator3 first2,
                                                     Iterator4 last2) {
        return CongruenceCommon::add_internal_generating_pair_no_checks<
            ToddCoxeterImpl>(first1, last1, first2, last2);
      }

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      ToddCoxeterImpl& add_generating_pair(Iterator1 first1,
                                           Iterator2 last1,
                                           Iterator3 first2,
                                           Iterator4 last2) {
        return CongruenceCommon::add_generating_pair<ToddCoxeterImpl>(
            first1, last1, first2, last2);
      }

      ////////////////////////////////////////////////////////////////////////
      // 5. ToddCoxeterImpl - interface requirements - number_of_classes
      ////////////////////////////////////////////////////////////////////////

      //! \ingroup todd_coxeter_class_intf_group
      //!
      //! \brief Compute the number of classes in the congruence.
      //!
      //! This function computes the number of classes in the congruence
      //! represented by a \ref_todd_coxeter instance by running the
      //! congruence enumeration until it terminates.
      //!
      //! \returns The number of congruences classes of a \ref_todd_coxeter
      //! instance if this number is finite, or \ref POSITIVE_INFINITY in some
      //! cases if this number is not finite.
      //!
      //! \cong_common_warn_undecidable{Todd-Coxeter}
      [[nodiscard]] uint64_t number_of_classes();

      ////////////////////////////////////////////////////////////////////////
      // 6. ToddCoxeterImpl - interface requirements - contains
      ////////////////////////////////////////////////////////////////////////

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      tril currently_contains_no_checks(Iterator1 first1,
                                        Iterator2 last1,
                                        Iterator3 first2,
                                        Iterator4 last2) const;

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      tril currently_contains(Iterator1 first1,
                              Iterator2 last1,
                              Iterator3 first2,
                              Iterator4 last2) const {
        return CongruenceCommon::currently_contains<ToddCoxeterImpl>(
            first1, last1, first2, last2);
      }

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      bool contains_no_checks(Iterator1 first1,
                              Iterator2 last1,
                              Iterator3 first2,
                              Iterator4 last2);

      template <typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename Iterator4>
      bool contains(Iterator1 first1,
                    Iterator2 last1,
                    Iterator3 first2,
                    Iterator4 last2);

      ////////////////////////////////////////////////////////////////////////
      // 7. ToddCoxeterImpl - interface requirements - reduce
      ////////////////////////////////////////////////////////////////////////

      template <typename OutputIterator,
                typename InputIterator1,
                typename InputIterator2>
      OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                             InputIterator1 first,
                                             InputIterator2 last) const;

      template <typename OutputIterator,
                typename InputIterator1,
                typename InputIterator2>
      OutputIterator reduce_no_run(OutputIterator d_first,
                                   InputIterator1 first,
                                   InputIterator2 last) const {
        return CongruenceCommon::reduce_no_run<ToddCoxeterImpl>(
            d_first, first, last);
      }

      template <typename OutputIterator,
                typename InputIterator1,
                typename InputIterator2>
      OutputIterator reduce_no_checks(OutputIterator d_first,
                                      InputIterator1 first,
                                      InputIterator2 last) {
        return CongruenceCommon::reduce_no_checks<ToddCoxeterImpl>(
            d_first, first, last);
      }

      template <typename OutputIterator,
                typename InputIterator1,
                typename InputIterator2>
      OutputIterator reduce(OutputIterator d_first,
                            InputIterator1 first,
                            InputIterator2 last) {
        return CongruenceCommon::reduce<ToddCoxeterImpl>(d_first, first, last);
      }

      ////////////////////////////////////////////////////////////////////////
      // 8. ToddCoxeterImpl - settings - public
      ////////////////////////////////////////////////////////////////////////

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
      // This is documented in Reporter, so we don't duplicate the doc here.
      template <typename Time>
      [[deprecated]] void report_every(Time val) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        CongruenceCommon::report_every(val);
#pragma GCC diagnostic pop
      }

      [[deprecated]] [[nodiscard]] nanoseconds report_every() const noexcept {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        return Reporter::report_every();
#pragma GCC diagnostic pop
      }
#endif

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the maximum number of definitions in the stack.
      //!
      //! This setting specifies the maximum number of definitions that can
      //! be in the stack at any given time. What happens if there are the
      //! maximum number of definitions in the stack and a new definition is
      //! generated is governed by \ref ToddCoxeter::options::def_policy.
      //!
      //! The default value of this setting is \c 2'000.
      //!
      //! \param val the maximum size of the definition stack.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& def_max(size_t val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the setting for the maximum number
      //! of definitions.
      //!
      //! \returns The current value of the setting, a value of type
      //! `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t def_max() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the definition policy.
      //!
      //! This function can be used to specify how to handle definitions.
      //! For details see \ref ToddCoxeter::options::def_policy.
      //!
      //! The default value of this setting is
      //! \ref ToddCoxeter::options::def_policy::no_stack_if_no_space.
      //!
      //! \param val the policy to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      ToddCoxeterImpl& def_policy(options::def_policy val);

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the definition policy.
      //!
      //! This function returns the current value of the definition policy
      //! which specifies how to handle definitions. For details see
      //! \ref ToddCoxeter::options::def_policy.
      //!
      //! \returns The current value of the setting, a value of type
      //! \ref ToddCoxeter::options::def_policy.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] options::def_policy def_policy() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the number of Felsch style definitions in
      //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
      //!
      //! This function can be used to set the approximate number of nodes
      //! defined in Felsch style in each phase of the
      //! [ACE](https://staff.itee.uq.edu.au/havas/) style strategies:
      //! * \ref ToddCoxeter::options::strategy::CR;
      //! * \ref ToddCoxeter::options::strategy::R_over_C;
      //! * \ref ToddCoxeter::options::strategy::R_over_C;
      //! * \ref ToddCoxeter::options::strategy::Cr; and
      //! * \ref ToddCoxeter::options::strategy::Rc.
      //!
      //! If the strategy is not one of those listed above, then this
      //! setting is ignored.
      //!
      //! The default value of this setting is \c 100'000.
      //!
      //! \param val the value to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \p val is \c 0.
      ToddCoxeterImpl& f_defs(size_t val);

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the number of Felsch style definitions in
      //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
      //!
      //! This function returns the approx number of Felsch style
      //! definitions in each phase of the
      //! [ACE](https://staff.itee.uq.edu.au/havas/) style strategies:
      //! * \ref ToddCoxeter::options::strategy::CR;
      //! * \ref ToddCoxeter::options::strategy::R_over_C;
      //! * \ref ToddCoxeter::options::strategy::R_over_C;
      //! * \ref ToddCoxeter::options::strategy::Cr; and
      //! * \ref ToddCoxeter::options::strategy::Rc.
      //!
      //! If the strategy is not one of those listed above, then this
      //! setting is ignored.
      //!
      //! The default value of this setting is \c 100'000.
      //!
      //! \returns The current value of the setting, a value of type
      //! `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t f_defs() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the number of HLT style definitions in
      //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
      //!
      //! This function can be used to set the approximate number nodes
      //! defined in HLT style in each phase of the
      //! [ACE](https://staff.itee.uq.edu.au/havas/) style strategies:
      //! * \ref ToddCoxeter::options::strategy::CR;
      //! * \ref ToddCoxeter::options::strategy::R_over_C;
      //! * \ref ToddCoxeter::options::strategy::R_over_C;
      //! * \ref ToddCoxeter::options::strategy::Cr; and
      //! * \ref ToddCoxeter::options::strategy::Rc.
      //!
      //! If the strategy is not one of those listed above, then this
      //! setting is ignored.
      //!
      //! The default value of this setting is \c 200'000.
      //!
      //! \param val the value to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \p val is \c 0.
      ToddCoxeterImpl& hlt_defs(size_t val);

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the number of HLT style definitions in
      //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
      //!
      //! This function returns the approx number of HLT style definitions
      //! in each phase of the [ACE](https://staff.itee.uq.edu.au/havas/)
      //! style strategies:
      //! * \ref ToddCoxeter::options::strategy::CR;
      //! * \ref ToddCoxeter::options::strategy::R_over_C;
      //! * \ref ToddCoxeter::options::strategy::R_over_C;
      //! * \ref ToddCoxeter::options::strategy::Cr; and
      //! * \ref ToddCoxeter::options::strategy::Rc.
      //!
      //! If the strategy is not one of those listed above, then this
      //! setting is ignored.
      //!
      //! The default value of this setting is \c 100'000.
      //!
      //! \returns The current value of the setting, a value of type
      //! `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t hlt_defs() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the size of a large collapse.
      //!
      //! This function can be used to set what should be considered a
      //! \"large\" collapse.
      //!
      //! By default when processing coincidences nodes are merged in the
      //! word graph one pair at a time, and the in-neighbours of the
      //! surviving node are updated at the same time. If the number of
      //! coincidences is large, then it might be that a pair of nodes are
      //! merged at one step, then the surviving node is merged with another
      //! node at a future step, and this may happen many many times. This
      //! results in the in-neighbours of the surviving nodes being
      //! repeatedly traversed, which can result in a significant
      //! performance penalty. It can be beneficial to stop updating the
      //! in-neighbours as nodes are merged, and to just rebuild the entire
      //! in-neighbours data structure by traversing the entire word graph
      //! after all coincidences have been processed. This is beneficial if
      //! the number of surviving nodes is relatively small in comparison to
      //! the number of nodes merged. The purpose of this setting is to
      //! specify what should be considered a \"large\" collapse, or more
      //! precisely, what number of coincidences in the stack will trigger a
      //! change from updating the in-neighbours one-by-one to traversing
      //! the entire graph once after all coincidences have been processed.
      //!
      //! The default value of this setting is \c 100'000.
      //!
      //! \param val the value to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& large_collapse(size_t val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current size of a large collapse.
      //!
      //! This function can be used to get what is currently considered a
      //! \"large\" collapse. See \ref large_collapse(size_t) for the
      //! meaning of this setting.
      //!
      //! The default value of this setting is \c 100'000.
      //!
      //! \returns The current value of the setting, a value of type
      //! `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t large_collapse() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the lookahead extent.
      //!
      //! This function can be used to specify the extent of any lookaheads
      //! that might take place in a congruence enumeration. The possible
      //! values are ToddCoxeter::options::lookahead_extent::partial or
      //! ToddCoxeter::options::lookahead_extent::full.
      //!
      //! The default value of this setting is
      //! \ref ToddCoxeter::options::lookahead_extent::partial.
      //!
      //! \param val the extent.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& lookahead_extent(options::lookahead_extent val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the lookahead extent.
      //!
      //! This function returns the current value of the lookahead extent
      //! setting.
      //!
      //! The default value of this setting is
      //! \ref ToddCoxeter::options::lookahead_extent::partial.
      //!
      //! \returns The current lookahead extent.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] options::lookahead_extent lookahead_extent() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the lookahead growth factor.
      //!
      //! This setting determines by what factor the number of nodes
      //! required to trigger a lookahead grows. More specifically, at the
      //! end of any lookahead if the number of active nodes already exceeds
      //! the value of
      //! \ref lookahead_next or the number of nodes killed during the
      //! lookahead is less than the number of active nodes divided by
      //! \ref lookahead_growth_threshold, then the value of
      //! \ref lookahead_next is increased by a multiple of \p val.
      //!
      //! The default value is of this setting is `2.0`.
      //!
      //! \param val the value indicating the lookahead growth factor.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \p val is less than `1.0`.
      ToddCoxeterImpl& lookahead_growth_factor(float val);

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the lookahead growth factor.
      //!
      //! This function returns the current value of the lookahead growth
      //! factor. See lookahead_growth_factor(float) for a full explanation
      //! of this setting.
      //!
      //! \returns The lookahead growth factor.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] float lookahead_growth_factor() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the lookahead growth threshold.
      //!
      //! This setting determines the threshold for the number of nodes
      //! required to trigger a lookahead. More specifically, at the end of
      //! any lookahead if the number of active nodes already exceeds the
      //! value of \ref lookahead_next or the number of nodes killed during
      //! the lookahead is less than the number of active nodes divided by
      //! \ref lookahead_growth_threshold, then the value of
      //! \ref lookahead_next is increased.
      //!
      //! The default value is `4`.
      //!
      //! \param val the value indicating the lookahead growth threshold.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& lookahead_growth_threshold(size_t val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the lookahead growth threshold.
      //!
      //! This function returns the current value of the lookahead growth
      //! threshold. See \ref lookahead_growth_threshold for a full
      //! description of this setting.
      //!
      //! \returns A value of type `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t lookahead_growth_threshold() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the minimum value of \ref lookahead_next.
      //!
      //! After a lookahead is performed the value of \ref lookahead_next is
      //! modified depending on the outcome of the current lookahead. If the
      //! return value of \ref lookahead_next is too small or too large,
      //! then the value is adjusted according to
      //! \ref lookahead_growth_factor and
      //! \ref lookahead_growth_threshold. This setting specified the
      //! minimum possible value for lookahead_next().
      //!
      //! The default value is \c 10'000.
      //!
      //! \param val value indicating the minimum value of
      //! \ref lookahead_next.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& lookahead_min(size_t val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the minimum lookahead setting.
      //!
      //! This function returns the current value of the minimum lookahead.
      //! See \ref lookahead_min(size_t) for a full description
      //! of this setting.
      //!
      //! The default value is \c 10'000.
      //!
      //! \returns The current value of the minimum lookahead.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t lookahead_min() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the threshold that will trigger a lookahead.
      //!
      //! If the number of active nodes exceeds the value set by this
      //! function, then a lookahead of style \ref lookahead_style and
      //! extent \ref lookahead_extent will be triggered.
      //!
      //! The default value is 5 million.
      //!
      //! \param val value indicating the initial threshold.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& lookahead_next(size_t val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the lookahead next setting.
      //!
      //! This function returns the current value of the lookahead next
      //! setting. See \ref lookahead_next(size_t) for a full description of
      //! this setting.
      //!
      //! \returns The number of active nodes that will trigger the next
      //! lookahead.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t lookahead_next() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the lookahead stop early interval.
      //!
      //! During any lookaheads that are performed, it is periodically
      //! checked what proportion of the active nodes have been killed since
      //! the previous such check. This function can be used to set the
      //! interval between these checks. The purpose of this setting is to
      //! allow lookaheads to be stopped early if the number of nodes being
      //! killed is too small (for example, if \f$<1%\f$ of nodes were
      //! killed in the previous second, then we might want to stop the
      //! lookahead early, since lookaheads take some time but may not
      //! result in many nodes being killed).
      //!
      //! The default value is 1 second.
      //!
      //! \param val the new value for the interval.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl&
      lookahead_stop_early_interval(std::chrono::nanoseconds val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the lookahead stop early interval.
      //!
      //! This function returns the current value of the lookahead stop
      //! early interval. See
      //! \ref lookahead_stop_early_interval(std::chrono::nanoseconds) for a
      //! full description of this setting.
      //!
      //! \returns The length of the interval in nanoseconds.
      //!
      //! \exceptions
      //! \noexcept
      std::chrono::nanoseconds lookahead_stop_early_interval() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the lookahead stop early ratio.
      //!
      //! During any lookaheads that are performed, it is periodically
      //! checked what proportion of the active nodes have been killed since
      //! the previous such check. This function can be used to set the
      //! minimum proportion of the active nodes that must be killed every
      //! \ref lookahead_stop_early_interval to avoid the lookahead being
      //! stopped early. The purpose of this setting is to allow lookaheads
      //! to be stopped early if the number of nodes being killed is too
      //! small (for example, if no nodes were killed in the previous
      //! second, then we might want to stop the lookahead early, since
      //! lookaheads take some time but may not result in many nodes being
      //! killed).
      //!
      //! \param val the proportion of active nodes.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \p val is not in the interval
      //! \f$[0, 1)\f$.
      ToddCoxeterImpl& lookahead_stop_early_ratio(float val);

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the lookahead stop early ratio.
      //!
      //! This function returns the current value of the lookahead stop
      //! early ratio. See \ref lookahead_stop_early_ratio(float) for a full
      //! description of this setting.
      //!
      //! \returns The ratio.
      //!
      //! \exceptions
      //! \noexcept
      float lookahead_stop_early_ratio() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set the style of lookahead.
      //!
      //! This function can be used to set the style of any lookaheads that
      //! are performed during the congruence enumeration. The possible
      //! values are \ref ToddCoxeter::options::lookahead_style::hlt and
      //! \ref ToddCoxeter::options::lookahead_style::felsch.
      //!
      //! The default value of this setting is
      //! \ref ToddCoxeter::options::lookahead_style::hlt.
      //!
      //! \param val the style of lookahead to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& lookahead_style(options::lookahead_style val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the lookahead style.
      //!
      //! This function returns the current value of the lookahead style.
      //! See \ref lookahead_style for a full description of this setting.
      //!
      //! \returns The current lookahead style.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] options::lookahead_style lookahead_style() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Specify the minimum number of classes that may permit any
      //! enumeration early stop.
      //!
      //! This function can be used to set a lower bound for the number of
      //! classes of the congruence represented by a  \ref_todd_coxeter
      //! instance. If the number of active nodes becomes at least the value
      //! of the argument, and the word graph is complete (
      //! \ref word_graph::is_complete returns \c true), then the enumeration
      //! is terminated. When the given bound is equal to the number of
      //! classes, this may prevent following the paths labelled by relations
      //! at many nodes when there is no possibility of finding coincidences.
      //!
      //! The default value is \ref UNDEFINED.
      //!
      //! \param val value indicating the lower bound.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& lower_bound(size_t val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the lower bound.
      //!
      //! This function returns the current value of the lower bound.
      //! See \ref lower_bound(size_t) for a full description of this
      //! setting.
      //!
      //! \returns The current lower bound.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] size_t lower_bound() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set whether or not to process definitions during HLT.
      //!
      //! If the argument of this function is \c true and the HLT strategy
      //! is being used, then definitions are processed during any
      //! enumeration.
      //!
      //! The default value is \c false.
      //!
      //! \param val value indicating whether or not to process deductions.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& save(bool val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the save setting.
      //!
      //! This function returns the current value of the save setting.
      //! See \ref save(bool) for a full description of this setting.
      //!
      //! \returns The current value.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] bool save() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Specify the congruence enumeration strategy.
      //!
      //! The strategy used during the enumeration can be specified using
      //! this function.
      //!
      //! The default value is \ref ToddCoxeter::options::strategy::hlt.
      //!
      //! \param val value indicating which strategy to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& strategy(options::strategy val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the strategy setting.
      //!
      //! This function returns the current value of the strategy setting.
      //! See \ref strategy for a full description of this setting.
      //!
      //! \returns The current value.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] options::strategy strategy() const noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Set whether or not to perform an HLT-style push of the
      //! defining relations at the identity.
      //!
      //! If a  \ref_todd_coxeter instance is defined over a finitely
      //! presented semigroup or monoid and the Felsch strategy is being used,
      //! it can be useful to follow all the paths from the identity labelled
      //! by the underlying relations. The setting specifies whether or not to
      //! do this.
      //!
      //! The default value of this setting is \c false.
      //!
      //! \param val the boolean value.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeterImpl& use_relations_in_extra(bool val) noexcept;

      //! \ingroup todd_coxeter_class_settings_group
      //! \brief Get the current value of the \"use relations in extra\"
      //! setting.
      //!
      //! This function returns the current value of the "use relations in
      //! extra" setting. See \ref use_relations_in_extra(bool) for a fuller
      //! description of this setting.
      //!
      //! \returns The current value.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] bool use_relations_in_extra() const noexcept;

#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
      //! \ingroup todd_coxeter_class_settings_group
      //!
      //! \brief Set the value of the definition version setting.
      //!
      //! There are two versions of definition processing represented by the
      //! values \ref ToddCoxeter::options::def_version::one and
      //! \ref ToddCoxeter::options::def_version::two. The first version is
      //! simpler, but may involve following the same path that leads nowhere
      //! multiple times. The second version is more complex, and attempts to
      //! avoid following the same path multiple times if it is found to lead
      //! nowhere once.
      //!
      //! \param val the version to use.
      //!
      //! \returns A reference to `*this`.
      ToddCoxeterImpl& def_version(options::def_version val);

      //! \ingroup todd_coxeter_class_settings_group
      //!
      //! \brief Get the current value of the definition version setting.
      //!
      //! This function returns the current version of the definition
      //! version setting.
      //!
      //! \returns The current value of the setting, a value of type
      //! \ref ToddCoxeter::options::def_version.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] options::def_version def_version() const noexcept;
#else
      using FelschGraphSettings_::def_version;
      using FelschGraphSettings_::settings;
#endif

      ////////////////////////////////////////////////////////////////////////
      // 9. ToddCoxeterImpl - accessors - public
      ////////////////////////////////////////////////////////////////////////

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
      [[nodiscard]] Presentation<word_type> const&
      internal_presentation() const noexcept {
        return _word_graph.presentation();
      }
#endif

      //! \ingroup todd_coxeter_class_accessors_group
      //!
      //! \brief Return the number of nodes in the active part of the current
      //! word graph.
      //!
      //! This function returns the number of nodes in the active part of the
      //! \ref current_word_graph. Recall that \ref current_word_graph can grow
      //! and shrink drastically during a congruence enumeration. As
      //! such to avoid unnecessary memory allocations, where possible, the
      //! nodes in the \ref current_word_graph are "recycled" leading to the
      //! situation where some of the nodes in \ref current_word_graph are
      //! "active" and others are "inactive". In other words, the "active" nodes
      //! correspond to the part of the word graph that actually represents the
      //! classes of the congruence we are trying to enumerate; and the
      //! "inactive" nodes are only there to be "recycled" into "active" nodes
      //! if they are required later on.
      //!
      //! \exceptions
      //! \noexcept
      // This isn't really necessary in C++, but in the python bindings we don't
      // bind ToddCoxeter::Graph and so we expose this here.
      [[nodiscard]] uint64_t number_of_nodes_active() const noexcept {
        return current_word_graph().number_of_nodes_active();
      }

      //! \ingroup todd_coxeter_class_accessors_group
      //!
      //! \brief Return the number of edges in the active part of the current
      //! word graph.
      //!
      //! This function returns the number of edges in the active part of the
      //! \ref current_word_graph. Recall that \ref current_word_graph can grow
      //! and shrink drastically during a congruence enumeration. As
      //! such to avoid unnecessary memory allocations, where possible, the
      //! nodes in the \ref current_word_graph are "recycled" leading to the
      //! situation where some of the nodes in \ref current_word_graph are
      //! "active" and others are "inactive". In other words, the "active" nodes
      //! correspond to the part of the word graph that actually represents the
      //! classes of the congruence we are trying to enumerate; and the
      //! "inactive" nodes are only there to be "recycled" into "active" nodes
      //! if they are required later on.
      //!
      //! \exceptions
      //! \noexcept
      // This isn't really necessary in C++, but in the python bindings we don't
      // bind ToddCoxeter::Graph and so we expose this here.
      [[nodiscard]] uint64_t number_of_edges_active() const noexcept {
        return current_word_graph().number_of_edges_active();
      }

      //! \ingroup todd_coxeter_class_accessors_group
      //!
      //! \brief Return the proportion of edges defined in the active part of
      //! the word graph.
      //!
      //! This function returns the proportion (as a float) of the edges
      //! defined. This value is  \ref number_of_edges_active divided by
      //! \ref number_of_nodes_active multiplied by \ref WordGraph::out_degree
      //! applied to \ref current_word_graph.
      //!
      //! \returns The proportion of edges defined in the active part of
      //! \ref current_word_graph.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] float complete() const noexcept {
        return complete(current_word_graph().number_of_edges_active());
      }

      // [[nodiscard]] bool empty() const {
      //   return (internal_presentation().rules.empty() &&
      //   generating_pairs().empty()
      //           && current_word_graph().number_of_nodes_active() == 1);
      //   // FIXME(1) there's an issue where the word graph can have 0
      //   nodes but
      //   1
      //   // active node.
      // }

      //! \ingroup todd_coxeter_class_accessors_group
      //! \brief Get the current word graph.
      //!
      //! In some sense, the purpose of the Todd-Coxeter algorithm is to
      //! produce a WordGraph of the action of a set of generators on the
      //! classes of a congruence. This function can be used to obtain a
      //! reference to that WordGraph as it currently exists within a
      //! \ref_todd_coxeter instance. This function does not trigger any
      //! enumeration.
      //!
      //! The WordGraph returned by this function may be in a rather
      //! complicated state. No guarantees are given: about the values of
      //! the active nodes (i.e. they may be any non-negative integers in
      //! any order); that the number of nodes (including those that are
      //! inactive) should coincide with the number of active nodes; that
      //! the graph is complete; or that the graph is compatible with the
      //! relations of the \ref presentation or with the
      //! \ref ToddCoxeter::generating_pairs.
      //!
      //! The functions \ref standardize(Order) and \ref shrink_to_fit can
      //! be used to modify the returned word graph in-place to have
      //! (possibly) more reasonable characteristics.
      //!
      //! \returns A const reference to the underlying WordGraph.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \sa number_of_nodes_active
      //! \sa number_of_edges_active
      word_graph_type const& current_word_graph() const noexcept {
        return _word_graph;
      }

      //! \ingroup todd_coxeter_class_accessors_group
      //! \brief Get the word graph after performing a full congruence
      //! enumeration.
      //!
      //! In some sense, the purpose of the Todd-Coxeter algorithm is to
      //! produce a WordGraph of the action of a set of generators on the
      //! classes of a congruence. This function can be used to obtain a
      //! reference to that WordGraph instance. This function triggers a
      //! full enumeration.
      //!
      //! The WordGraph returned by this function may be in a rather
      //! complicated state. The active nodes (and nodes) will be \f$\{0,
      //! \ldots, n - 1\}\f$ where \f$n\f$ is the number of classes in the
      //! congruence if \ref presentation contains the empty word; or the
      //! number of classes plus one if \ref presentation does not contain
      //! the empty word. The returned WordGraph is also short-lex
      //! standardized. The returned WordGraph will usually be complete and
      //! compatible with the relations of the \ref presentation and with
      //! the \ref ToddCoxeter::generating_pairs. The WordGraph may not be
      //! complete or compatible for some values of the settings. For example,
      //! if the setting \ref lower_bound is used but is not the same as the
      //! number of classes in the congruence, then the WordGraph returned by
      //! this function may not be compatible with the relations of
      //! \ref presentation or \ref ToddCoxeter::generating_pairs.
      //!
      //! \returns A const reference to the underlying WordGraph.
      word_graph_type const& word_graph();

      //! \ingroup todd_coxeter_class_accessors_group
      //! \brief Get the current possible spanning tree of the underlying
      //! word graph.
      //!
      //! This function returns a const reference to the current value of a
      //! possible spanning tree (a \ref Forest) for the underlying
      //! WordGraph (returned by \ref current_word_graph). This spanning
      //! tree is only populated during calls to \ref standardize and as
      //! such might contain nothing, or a spanning tree of a previous value
      //! of \ref current_word_graph. Some care should be used with the
      //! return value of this function, and it might be better to use the
      //! function \ref spanning_tree, which has none of these limitation.
      //!
      //! If \ref finished returns \c true, and \ref standardize(Order) has
      //! been called prior to a call to this function, then the returned
      //! \ref Forest will represent a valid spanning tree for the WordGraph
      //! returned by \ref current_word_graph or \ref word_graph.
      //!
      //! \returns A const reference to a possible spanning tree of the
      //! underlying \ref WordGraph.
      //!
      //! \exceptions
      //! \noexcept
      Forest const& current_spanning_tree() const noexcept {
        return _forest;
      }

      //! \ingroup todd_coxeter_class_accessors_group
      //! \brief Get the spanning tree of the underlying word graph.
      //!
      //! This function returns a const reference to a spanning tree (a
      //! \ref Forest) for the underlying WordGraph (returned by
      //! \ref word_graph) with the nodes appearing in short-lex order. This
      //! function triggers a full congruence enumeration.
      //!
      //! \returns A const reference to a spanning tree of the underlying
      //! \ref WordGraph.
      Forest const& spanning_tree();

      //! \ingroup todd_coxeter_class_accessors_group
      //! \brief Get the current standardization order of the underlying
      //! word graph.
      //!
      //! This function returns the standardization order currently used in
      //! the underlying word graph. The return value of this function will
      //! be the argument of the most recent call to
      //! \ref standardize(Order); or Order::none.
      //!
      //! The return value of this function indicates the following:
      //! * Order::none implies that no standardization has been performed
      //! and:
      //!   - the return value of \ref ToddCoxeter::reduce will essentially
      //!   arbitrary;
      //!   - the return values of \ref todd_coxeter::normal_forms will be
      //!   essentially arbitrary;
      //!   - the classes of the congruence will be indexed in an arbitrary
      //!   order;
      //! * Order::shortlex implies that:
      //!   - the return value of \ref ToddCoxeter::reduce will be the
      //!   short-lex least word belonging to a given congruence class;
      //!   - the return values of \ref todd_coxeter::normal_forms will be
      //!   in short-lex order;
      //!   - the classes of the congruence will be indexed in short-lex order
      //!   on the short-lex least word;
      //! * Order::lex implies that:
      //!   - the return values of \ref todd_coxeter::normal_forms will be
      //!   ordered lexicographically.
      //!   - the return values of \ref ToddCoxeter::reduce and the indexes of
      //!   class are essentially arbitrary because there is not necessarily a
      //!   lexicographically least word in every class;
      //! * Order::recursive implies that:
      //!   - the return value of \ref ToddCoxeter::reduce will be the
      //!   recursive path least word belonging to a given congruence class;
      //!   - the return values of \ref todd_coxeter::normal_forms will be
      //!   in recursive path order;
      //!   - the classes of the congruence will be indexed in recursive
      //!   path order on the recursive path least word.
      //!
      //! \returns The current standardization order.
      //!
      //! \exceptions
      //! \noexcept
      inline Order standardization_order() const noexcept {
        return _standardized;
      }

      //! \ingroup todd_coxeter_class_accessors_group
      //! \brief Check if the word graph is currently standardized with
      //! respect to a given order.
      //!
      //! This function returns \c true if the \ref current_word_graph has
      //! been standardized with respect to the order \p val; and \c false
      //! if not.
      //!
      //! \param val the \ref Order to check for.
      //!
      //! \returns Whether or not the current word graph is standardized
      //! with respect to a given order.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      bool is_standardized(Order val) const;

      //! \ingroup todd_coxeter_class_accessors_group
      //! \brief Check if the word graph is currently standardized with
      //! respect to any order.
      //!
      //! This function returns \c true if the \ref current_word_graph has
      //! been standardized with respect to the any \ref Order other than
      //! Order::none.
      //!
      //! \returns Whether or not the current word graph is standardized
      //! with respect to any order.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      bool is_standardized() const;

      //! \ingroup todd_coxeter_class_accessors_group
      //! \brief Return the number of large collapses that have occurred.
      //!
      //! This function returns the number of "large" collapses that have
      //! occurred in the graph during any run of a ToddCoxeter instance. What
      //! qualifies as a "large" collapse can be specified using
      //! \ref ToddCoxeter::large_collapse.
      //!
      //! \returns The number of large collapses.
      //!
      //! \exceptions
      //! \noexcept
      [[nodiscard]] uint64_t number_of_large_collapses() const noexcept {
        return _word_graph.stats().num_large_collapses;
      }

      ////////////////////////////////////////////////////////////////////////
      // 10. ToddCoxeterImpl - modifiers - public
      ////////////////////////////////////////////////////////////////////////

      //! \ingroup todd_coxeter_class_mod_group
      //! \brief Shrink the underlying word graph to remove all dead nodes.
      //!
      //! This function triggers a full enumeration, and standardization,
      //! and removes from \ref word_graph any dead nodes.
      //!
      //! If \ref finished returns \c false, then this function does
      //! nothing.
      void shrink_to_fit();

      //! \ingroup todd_coxeter_class_mod_group
      //! \brief Standardize the \ref current_word_graph.
      //!
      //! This function standardizes the return value of
      //! \ref current_word_graph, and does not trigger any enumeration. See
      //! \ref standardization_order for a full description. The return value
      //! of this function indicates whether or not the
      //! \ref current_word_graph was modified. In other words, if this function
      //! returns \c true, then the word graph was not previously standardized
      //! with respect to \p val, and was modified by calling this function if
      //! \c false is returned, then the word graph was previously
      //! standardized with respect to \p val (although this might not have
      //! been known), and was not modified by calling this function.
      //!
      //! \param val the order of the standardization.
      //!
      //! \returns Whether or not the word graph was modified by the
      //! standardization.
      //!
      //! \note If \p val is Order::none, then this function does nothing.
      //!
      //! \sa \ref word_graph::standardize
      //! \sa \ref current_spanning_tree.
      bool standardize(Order val);

      //! \ingroup todd_coxeter_class_mod_group
      //! \brief Perform a lookahead.
      //!
      //! This function can be used to explicitly perform a lookahead. The
      //! style and extent of this lookahead are controlled by the settings
      //! \ref lookahead_style and \ref lookahead_extent.
      //!
      //! If the argument \p stop_early is \c true, then the settings
      //! \ref lookahead_stop_early_interval and
      //! \ref lookahead_stop_early_ratio are used to determine whether or not
      //! the lookahead should be aborted early. If \p stop_early is \c false,
      //! then these settings are ignored.
      //!
      //! \param stop_early whether or not to consider stopping the
      //! lookahead early if too few nodes are killed.
      void perform_lookahead(bool stop_early);

      ////////////////////////////////////////////////////////////////////////
      // 11. ToddCoxeterImpl - word -> index
      ////////////////////////////////////////////////////////////////////////

      //! \defgroup todd_coxeter_class_word_index_group Word to class index
      //! \ingroup todd_coxeter_class_group
      //!
      //! \brief Member functions for converting a word into the index of a
      //! class in a \ref_todd_coxeter
      //! instance.
      //!
      //! This page contains documentation for the member functions of
      //! \ref todd_coxeter_class_group  that can be used to
      //! convert a word into the index of congruence class.
      //!
      //! \sa \ref todd_coxeter_class_index_word_group for the inverses of
      //! the functions described on this page.
      //!
      //! @{

      //! \brief Returns the current index of the class containing a word.
      //!
      //! This function returns the current index of the class containing
      //! the word described by the iterators \p first and \p last. No
      //! enumeration is triggered by calls to this function. Unless
      //! \ref finished returns \c true, the index returned by this function
      //! is essentially arbitrary, and can only really be used to check
      //! whether or not two words are currently known to belong to the
      //! congruence.
      //!
      //! The returned index is obtained by following the path in
      //! \ref current_word_graph from node \c 0 labelled by the word given by
      //! the arguments \p first and \p last. If there is no such path, then
      //! \ref UNDEFINED is returned.
      //!
      //! \tparam Iterator1 the type of first argument \p first.
      //! \tparam Iterator2 the type of second argument \p last.
      //!
      //! \param first iterator pointing at the first letter of the word.
      //! \param last iterator pointing one beyond the last letter of the
      //! word.
      //!
      //! \returns The current index of the class containing the word.
      //!
      //! \cong_common_warn_assume_letters_in_bounds
      //!
      // NOTE: the graph contains one more node than there are element
      // if the underlying presentation does not contain the empty word
      template <typename Iterator1, typename Iterator2>
      index_type current_index_of_no_checks(Iterator1 first,
                                            Iterator2 last) const;

      //! \brief Returns the current index of the class containing a word.
      //!
      //! This function returns the current index of the class containing
      //! the word described by the iterators \p first and \p last. No
      //! enumeration is triggered by calls to this function. Unless
      //! \ref finished returns \c true, the index returned by this function
      //! is essentially arbitrary, and can only really be used to check
      //! whether or not two words are currently known to belong to the
      //! congruence.
      //!
      //! The returned index is obtained by following the path in
      //! \ref current_word_graph from node \c 0 labelled by the word given by
      //! the arguments \p first and \p last. If there is no such path, then
      //! \ref UNDEFINED is returned.
      //!
      //! \tparam Iterator1 the type of first argument \p first.
      //! \tparam Iterator2 the type of second argument \p last.
      //!
      //! \param first iterator pointing at the first letter of the word.
      //! \param last iterator pointing one beyond the last letter of the
      //! word.
      //!
      //! \returns The current index of the class containing the word.
      //!
      //! \cong_common_throws_if_letters_out_of_bounds
      template <typename Iterator1, typename Iterator2>
      index_type current_index_of(Iterator1 first, Iterator2 last) const {
        throw_if_letter_not_in_alphabet(first, last);
        return current_index_of_no_checks(first, last);
      }

      //! \brief Returns the index of the class containing a word.
      //!
      //! This function returns the index of the class containing the word
      //! described by the iterators \p first and \p last. A full
      //! enumeration is triggered by calls to this function. If the
      //! \ref current_word_graph has not already been standardized, then this
      //! function first standardizes it with respect to Order::shortlex;
      //! otherwise the existing standardization order is used.
      //!
      //! The returned index is obtained by following the path in
      //! \ref current_word_graph from node \c 0 labelled by the word given by
      //! the arguments \p first and \p last. Since a full enumeration is
      //! triggered by calls to this function, the word graph is complete,
      //! and so the return value is never \ref UNDEFINED.
      //!
      //! \tparam Iterator1 the type of first argument \p first.
      //! \tparam Iterator2 the type of second argument \p last.
      //!
      //! \param first iterator pointing at the first letter of the word.
      //! \param last iterator pointing one beyond the last letter of the
      //! word.
      //!
      //! \returns The index of the class containing the word.
      //!
      //! \cong_common_warn_assume_letters_in_bounds
      template <typename Iterator1, typename Iterator2>
      index_type index_of_no_checks(Iterator1 first, Iterator2 last);

      //! \brief Returns the index of the class containing a word.
      //!
      //! This function returns the index of the class containing the word
      //! described by the iterators \p first and \p last. A full
      //! enumeration is triggered by calls to this function. If the
      //! \ref current_word_graph has not already been standardized, then this
      //! function first standardizes it with respect to Order::shortlex;
      //! otherwise the existing standardization order is used.
      //!
      //! The returned index is obtained by following the path in
      //! \ref current_word_graph from node \c 0 labelled by the word given by
      //! the arguments \p first and \p last. Since a full enumeration is
      //! triggered by calls to this function, the word graph is complete,
      //! and so the return value is never \ref UNDEFINED.
      //!
      //! \tparam Iterator1 the type of first argument \p first.
      //! \tparam Iterator2 the type of second argument \p last.
      //!
      //! \param first iterator pointing at the first letter of the word.
      //! \param last iterator pointing one beyond the last letter of the
      //! word.
      //!
      //! \returns The index of the class containing the word.
      //!
      //! \cong_common_throws_if_letters_out_of_bounds
      template <typename Iterator1, typename Iterator2>
      index_type index_of(Iterator1 first, Iterator2 last) {
        throw_if_letter_not_in_alphabet(first, last);
        return index_of_no_checks(first, last);
      }

      //! @}

      ////////////////////////////////////////////////////////////////////////
      // 12. ToddCoxeterImpl - index -> word
      ////////////////////////////////////////////////////////////////////////

      //! \defgroup todd_coxeter_class_index_word_group Class index to word
      //! \ingroup todd_coxeter_class_group
      //!
      //! \brief Member functions for converting the index of a class
      //! into a word in a \ref_todd_coxeter
      //! instance.
      //!
      //! This page contains documentation for the member functions of
      //! \ref_todd_coxeter that can be used to convert the index of a
      //! congruence class to a representative word belonging to that
      //! class.
      //!
      //! \sa \ref todd_coxeter_class_word_index_group for the inverses of
      //! the functions described on this page.
      //!
      //! @{

      //! \brief Insert a current word representing a class with given index
      //! into an output iterator.
      //!
      //! This function appends a current word representing the class with
      //! index
      //! \p i to the output iterator \p d_first. No enumeration is
      //! triggered by calls to this function, but \ref current_word_graph
      //! is standardized (using Order::shortlex) if it is not already
      //! standardized.
      //!
      //! The word appended to \p d_first is obtained by following a path in
      //! \ref current_spanning_tree from the node corresponding to index
      //! \c i back to the root of that tree.
      //!
      //! \tparam OutputIterator the type of the first argument.
      //!
      //! \param d_first output iterator pointing at the first letter of the
      //! destination.
      //! \param i the index of the class.
      //!
      //! \returns An output iterator pointing one beyond the last letter of
      //! the output word.
      //!
      //! \warning This function does not check its arguments. In
      //! particular, it is assumed that \p i is a valid index of a current
      //! class.
      //!
      // NOTE THAT: the graph contains one more node than there are element
      // if the underlying presentation does not contain the empty word
      template <typename OutputIterator>
      OutputIterator current_word_of_no_checks(OutputIterator d_first,
                                               index_type     i) const;

      //! \brief Insert a current word representing a class with given index
      //! into an output iterator.
      //!
      //! This function appends a current word representing the class with
      //! index
      //! \p i to the output iterator \p d_first. No enumeration is
      //! triggered by calls to this function, but \ref current_word_graph
      //! is standardized (using Order::shortlex) if it is not already
      //! standardized.
      //!
      //! The word appended to \p d_first is obtained by following a path in
      //! \ref current_spanning_tree from the node corresponding to index
      //! \c i back to the root of that tree.
      //!
      //! \tparam OutputIterator the type of the first argument.
      //!
      //! \param d_first output iterator pointing at the first letter of the
      //! destination.
      //! \param i the index of the class.
      //!
      //! \returns An output iterator pointing one beyond the last letter of
      //! the output word.
      //!
      //! \throws LibsemigroupsException if \p i is out of bounds.
      //!
      template <typename OutputIterator>
      OutputIterator current_word_of(OutputIterator d_first,
                                     index_type     i) const;

      //! \brief Insert the word representing a class with given index into
      //! an output iterator.
      //!
      //! This function appends the word representing the class with index
      //! \p i to the output iterator \p d_first. A full enumeration is
      //! triggered by calls to this function, and \ref current_word_graph
      //! is standardized.
      //!
      //! The word appended to \p d_first is obtained by following a path in
      //! \ref current_spanning_tree from the node corresponding to index
      //! \c i back to the root of that tree.
      //!
      //! \tparam OutputIterator the type of the first argument.
      //!
      //! \param d_first output iterator pointing at the first letter of the
      //! destination.
      //! \param i the index of the class.
      //!
      //! \returns An output iterator pointing one beyond the last letter of
      //! the output word.
      //!
      //! \throws LibsemigroupsException if \p i is out of bounds.
      //!
      template <typename Iterator>
      Iterator word_of_no_checks(Iterator d_first, index_type i) {
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        return current_word_of_no_checks(d_first, i);
      }

      //! \brief Insert the word representing a class with given index into
      //! an output iterator.
      //!
      //! This function appends the word representing the class with index
      //! \p i to the output iterator \p d_first. A full enumeration is
      //! triggered by calls to this function, and \ref current_word_graph
      //! is standardized.
      //!
      //! The word appended to \p d_first is obtained by following a path in
      //! \ref current_spanning_tree from the node corresponding to index
      //! \c i back to the root of that tree.
      //!
      //! \tparam OutputIterator the type of the first argument.
      //!
      //! \param d_first output iterator pointing at the first letter of the
      //! destination.
      //! \param i the index of the class.
      //!
      //! \returns An output iterator pointing one beyond the last letter of
      //! the output word.
      //!
      //! \warning This function does not check its arguments. In
      //! particular, it is assumed that \p i is a valid index of a current
      //! class.
      template <typename Iterator>
      Iterator word_of(Iterator d_first, index_type i) {
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        return current_word_of(d_first, i);
      }

      //! @}

     private:
      ////////////////////////////////////////////////////////////////////////
      // 13. Runner - pure virtual member functions - private
      ////////////////////////////////////////////////////////////////////////

      void really_run_impl();

      void run_impl() override;

      [[nodiscard]] bool finished_impl() const override {
        return _finished;
      }

      ////////////////////////////////////////////////////////////////////////
      // 14. ToddCoxeterImpl - member functions - private
      ////////////////////////////////////////////////////////////////////////

      void copy_settings_into_graph();

      // This function has prefix tc_ because there's already a settings
      // function in a base class
      Settings&       tc_settings();
      Settings const& tc_settings() const;

      void reset_settings_stack();

      [[nodiscard]] bool any_change() const {
        return _stats.run_nodes_active_at_start
               != current_word_graph().number_of_nodes_active();
      }

      // We take both values here, although we could compute them, so that in
      // report_progress_from_thread we do not report at one point in time with
      // some value, then within the same function call (if the graph is
      // changing a lot) report with another value
      [[nodiscard]] float complete(uint64_t num_nodes,
                                   int64_t  num_edges) const noexcept {
        return static_cast<float>(num_edges)
               / (static_cast<uint64_t>(num_nodes)
                  * current_word_graph().out_degree());
      }

      [[nodiscard]] float complete(int64_t num_edges) const noexcept {
        return complete(current_word_graph().number_of_nodes_active(),
                        num_edges);
      }

      [[nodiscard]] bool lookahead_stop_early(
          bool                                            stop_early,
          std::chrono::high_resolution_clock::time_point& last_stop_early_check,
          uint64_t& killed_at_prev_interval);

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeterImpl - main strategies - private
      ////////////////////////////////////////////////////////////////////////

      void init_run();
      void finalise_run();

      void felsch();
      void hlt();
      void CR_style();
      void R_over_C_style();
      void Cr_style();
      void Rc_style();

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeterImpl - reporting - private
      ////////////////////////////////////////////////////////////////////////

      void report_after_phase() const;
      void report_after_lookahead(size_t old_lookahead_next) const;
      void report_after_run() const;
      void report_before_phase(std::string_view = "") const;
      void report_before_lookahead() const;
      void report_before_run() const;
      void report_lookahead_stop_early(size_t expected,
                                       size_t killed_last_interval);
      void report_presentation() const;
      void report_progress_from_thread(bool divider) const;
      void report_times() const;

      void add_timing_row(ReportCell<5>& rc) const;

      // The 2nd (and 3rd) arguments for the next 2 functions are required
      // because we need the values at a fixed point in time (due to
      // multi-threaded reporting).
      void add_nodes_rows(ReportCell<5>& rc, uint64_t num_active_nodes) const;
      void add_edges_rows(ReportCell<5>& rc,
                          uint64_t       num_active_nodes,
                          uint64_t       num_active_edges) const;
      void add_lookahead_row(ReportCell<5>& rc) const;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeterImpl - lookahead - private
      ////////////////////////////////////////////////////////////////////////

      static constexpr bool StopEarly      = true;
      static constexpr bool DoNotStopEarly = false;

      void hlt_lookahead(bool stop_early);
      void felsch_lookahead(bool stop_early);
    };  // class ToddCoxeterImpl

  }  // namespace detail

}  // namespace libsemigroups

#include "todd-coxeter-impl.tpp"
#endif  // LIBSEMIGROUPS_DETAIL_TODD_COXETER_IMPL_HPP_
