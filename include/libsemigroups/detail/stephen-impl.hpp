//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2026 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_DETAIL_STEPHEN_IMPL_HPP_
#define LIBSEMIGROUPS_DETAIL_STEPHEN_IMPL_HPP_

#include <cmath>        // for pow
#include <cstddef>      // for size_t
#include <cstdint>      // for uint32_t
#include <memory>       // for shared_ptr
#include <string>       // for basic_st...
#include <string_view>  // for basic_st...
#include <tuple>        // for tie
#include <type_traits>  // for is_same_v
#include <utility>      // for make_pair
#include <vector>       // for vector

#include "libsemigroups/constants.hpp"  // for Max, UNDEFINED
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/is_specialization_of.hpp"  // for is_specialization_of
#include "libsemigroups/paths.hpp"                 // for Paths
#include "libsemigroups/presentation.hpp"          // for Presentation
#include "libsemigroups/runner.hpp"                // for Runner
#include "libsemigroups/types.hpp"                 // for word_type
#include "libsemigroups/word-graph-helpers.hpp"    // for word_graph_no_run
#include "libsemigroups/word-graph.hpp"            // for WordGraph

#include "node-managed-graph.hpp"       // for NodeMana...
#include "node-manager.hpp"             // for NodeManager
#include "report.hpp"                   // for report_n...
#include "string.hpp"                   // for group_di...
#include "timer.hpp"                    // for string_time
#include "value-guard.hpp"              // for ValueGuard
#include "word-graph-with-sources.hpp"  // for WordGrap...

// TODO(2)
// * update so that run_for, run_until work properly (at present basically
//   run_impl starts again from scratch every time)
// * minimal rep (as per Reinis) (named normal_form?)
// * invert() - just swap the initial and accept states and re-standardize
// * idempotent() - just make the accept state = initial state.
// * class_of for inverse StephenImpl (i.e. all walks in the graph through all
// nodes) (not sure how to do this just yet). This is different than
// words_accepted see Corollary 3.2 in StephenImpl's "Presentations of inverse
// monoids" paper (not thesis).
// * canonical_form (as per Howie's book)

namespace libsemigroups {
  namespace detail {

    template <typename PresentationType>
    class StephenImpl : public Runner {
      static_assert(
          std::is_same_v<Presentation<word_type>, PresentationType>
              || std::is_same_v<InversePresentation<word_type>,
                                PresentationType>,
          "the template parameter PresentationType must be "
          "Presentation<word_type> or InversePresentation<word_type>");

     public:
      using presentation_type = PresentationType;

      using word_graph_type = WordGraph<uint32_t>;

      using node_type = word_graph_type::node_type;

     private:
      class StephenGraph;  // forward decl

      // Data members
      node_type                         _accept_state;
      bool                              _finished;
      bool                              _is_word_set;
      std::shared_ptr<PresentationType> _internal_presentation;
      word_type                         _internal_word;
      bool                              _ticker_running;
      StephenGraph                      _word_graph;

     public:
      StephenImpl();

      StephenImpl& init();

      // Not noexcept because allocated memory
      explicit StephenImpl(PresentationType const& p) : StephenImpl() {
        init(p);
      }

      explicit StephenImpl(PresentationType&& p) : StephenImpl() {
        init(std::move(p));
      }

      explicit StephenImpl(std::shared_ptr<PresentationType> const& ptr)
          : StephenImpl() {
        init(ptr);
      }

      StephenImpl(StephenImpl const& that) = default;

      StephenImpl(StephenImpl&&) = default;

      StephenImpl& operator=(StephenImpl const&) = default;

      StephenImpl& operator=(StephenImpl&&) = default;

      ~StephenImpl();

      StephenImpl& init(PresentationType const& p) {
        return init(std::make_shared<PresentationType>(p));
      }

      StephenImpl& init(PresentationType&& p) {
        return init(std::make_shared<PresentationType>(std::move(p)));
      }

      StephenImpl& init(std::shared_ptr<PresentationType> const& ptr);

      [[nodiscard]] presentation_type const&
      internal_presentation() const noexcept {
        return *_internal_presentation;
      }

      template <typename Iterator1, typename Iterator2>
      StephenImpl& set_internal_word(Iterator1 first, Iterator2 last) {
        internal_presentation().throw_if_letter_not_in_alphabet(first, last);
        return set_internal_word_no_checks(first, last);
      }

      template <typename Iterator1, typename Iterator2>
      StephenImpl& set_internal_word_no_checks(Iterator1 first, Iterator2 last);

      [[nodiscard]] bool is_word_set() const noexcept {
        return _is_word_set;
      }

      [[nodiscard]] word_type const& internal_word() const {
        throw_if_not_ready();
        return _internal_word;
      }

      [[deprecated("Use word_graph_no_run() "
                   "instead!")]] [[nodiscard]] word_graph_type const&
      word_graph() const {
        return word_graph_no_run();
      }

      [[nodiscard]] word_graph_type const& word_graph_no_run() const {
        throw_if_not_ready();
        return _word_graph;
      }

      // Throws if run throws, also this is not in the helper namespace because
      // we cache the return value.
      [[nodiscard]] node_type accept_state();

      [[nodiscard]] static constexpr node_type initial_state() noexcept {
        return 0;
      }

      void operator*=(StephenImpl<PresentationType>& that);

      void append_no_checks(StephenImpl<PresentationType>& that);

     protected:
      void throw_if_not_ready() const;
      void throw_if_presentation_empty(presentation_type const& p) const;

     private:
      ////////////////////////////////////////////////////////////////////////
      // Reporting
      ////////////////////////////////////////////////////////////////////////

      void report_after_run() const;
      void report_before_run() const;
      void report_progress_from_thread() const;

      void run_impl() override;

      [[nodiscard]] bool finished_impl() const noexcept override {
        return _finished;
      }

      void standardize() {
        v4::word_graph::standardize(_word_graph);
        _word_graph.induced_subgraph_no_checks(
            0, _word_graph.number_of_nodes_active());
      }
    };  // class StephenImpl

    namespace stephen {

      template <typename PresentationType, typename Iterator>
      [[nodiscard]] bool accepts(StephenImpl<PresentationType>& s,
                                 Iterator                       first,
                                 Iterator                       last);

      template <typename PresentationType, typename Iterator>
      [[nodiscard]] bool is_left_factor(StephenImpl<PresentationType>& s,
                                        Iterator                       first,
                                        Iterator                       last);

      template <typename PresentationType>
      [[nodiscard]] auto words_accepted(StephenImpl<PresentationType>& s) {
        s.run();
        Paths paths(s.word_graph_no_run());
        return paths.source(s.initial_state()).target(s.accept_state());
      }

      template <typename PresentationType>
      [[nodiscard]] auto left_factors(StephenImpl<PresentationType>& s) {
        s.run();
        Paths paths(s.word_graph_no_run());
        return paths.source(s.initial_state());
      }
    }  // namespace stephen
  }    // namespace detail
}  // namespace libsemigroups

#include "stephen-impl.tpp"
#endif  // LIBSEMIGROUPS_DETAIL_STEPHEN_IMPL_HPP_
