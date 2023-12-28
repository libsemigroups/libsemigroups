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
// congruence" algorithm for semigroups and monoid.

#include "libsemigroups/sims.hpp"

#include <algorithm>   // for max, find_if, fill
#include <chrono>      // for duration, durat...
#include <functional>  // for ref
#include <memory>      // for unique_ptr, mak...
#include <string>      // for operator+, basi...
#include <thread>      // for thread, yield
#include <utility>     // for swap

#include "libsemigroups/constants.hpp"        // for operator!=, ope...
#include "libsemigroups/debug.hpp"            // for LIBSEMIGROUPS_A...
#include "libsemigroups/exception.hpp"        // for LIBSEMIGROUPS_E...
#include "libsemigroups/felsch-graph.hpp"     // for FelschGraph
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/presentation.hpp"     // for Presentation
#include "libsemigroups/to-froidure-pin.hpp"  // for to_froidure_pin
#include "libsemigroups/transf.hpp"           // for Transf, validate
#include "libsemigroups/types.hpp"            // for congruence_kind
#include "libsemigroups/word-graph.hpp"       // for follow_path_no_...

#include "libsemigroups/detail/fmt.hpp"     // for buffer::append
#include "libsemigroups/detail/report.hpp"  // for report_default
#include "libsemigroups/detail/stl.hpp"     // for JoinThreads
#include "libsemigroups/detail/string.hpp"  // for group_digits

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // SimsStats
  ////////////////////////////////////////////////////////////////////////

  SimsStats::SimsStats()
      : count_last(),
        count_now(),
        max_pending(),
        total_pending_last(),
        total_pending_now() {
    stats_zero();
  }

  SimsStats& SimsStats::stats_zero() {
    count_last         = 0;
    count_now          = 0;
    max_pending        = 0;
    total_pending_last = 0;
    total_pending_now  = 0;
    return *this;
  }

  SimsStats& SimsStats::init_from(SimsStats const& that) {
    count_last         = that.count_last;
    count_now          = that.count_now.load();
    max_pending        = that.max_pending.load();
    total_pending_last = that.total_pending_last;
    total_pending_now  = that.total_pending_now.load();
    return *this;
  }

  template <typename Sims1or2>
  Sims1or2& detail::SimsBase<Sims1or2>::init() {
    if constexpr (std::is_same_v<Sims1or2, Sims1>) {
      report_prefix("Sims1");
    } else {
      report_prefix("Sims2");
    }
    SimsSettings<Sims1or2>::init();
    return static_cast<Sims1or2&>(*this);
  }

  template <typename Sims1or2>
  detail::SimsBase<Sims1or2>::SimsBase() {
    init();
  }

  template <typename Sims1or2>
  detail::SimsBase<Sims1or2>::iterator::iterator(Sims1or2 const* s, size_type n)
      : iterator_base(s, n) {
    if (this->_felsch_graph.number_of_active_nodes() == 0) {
      return;
    }
    init(n);
    ++(*this);
    // The increment above is required so that when dereferencing any
    // instance of this type we obtain a valid word graph (o/w the value
    // pointed to here is empty).
  }

  template class detail::SimsBase<Sims1>;
  template class detail::SimsBase<Sims2>;

  Sims1& Sims1::init() {
    return SimsBase::init();
  }

  class Sims2::iterator_base::RuleContainer {
   private:
    std::vector<word_type> _words;
    // _used_slots[i] is the length of _words when we have i edges
    std::vector<size_t> _used_slots;

   public:
    RuleContainer()                                = default;
    RuleContainer(RuleContainer const&)            = default;
    RuleContainer(RuleContainer&&)                 = default;
    RuleContainer& operator=(RuleContainer const&) = default;
    RuleContainer& operator=(RuleContainer&&)      = default;

    ~RuleContainer() = default;

    void resize(size_t m) {
      // TODO should we use resize?
      _used_slots.assign(m, UNDEFINED);
      if (m > 0) {
        _used_slots[0] = 0;
      }
      _words.assign(m, word_type());
    }

    word_type& next_rule_word(size_t num_edges) {
      return _words[used_slots(num_edges)++];
    }

    auto begin(size_t) const noexcept {
      return _words.begin();
    }

    auto end(size_t num_edges) noexcept {
      return _words.begin() + used_slots(num_edges);
    }

    void backtrack(size_t num_edges) {
      std::fill(_used_slots.begin() + num_edges,
                _used_slots.end(),
                size_t(UNDEFINED));
      if (num_edges == 0) {
        _used_slots[0] = 0;
      }
    }

   private:
    size_t& used_slots(size_t num_edges) {
      LIBSEMIGROUPS_ASSERT(num_edges < _used_slots.size());
      if (_used_slots[0] == UNDEFINED) {
        _used_slots[0] = 0;
      }
      size_t i = num_edges;
      while (_used_slots[i] == UNDEFINED) {
        --i;
      }
      LIBSEMIGROUPS_ASSERT(i < _used_slots.size());
      LIBSEMIGROUPS_ASSERT(_used_slots[i] != UNDEFINED);
      for (size_t j = i + 1; j <= num_edges; ++j) {
        _used_slots[j] = _used_slots[i];
      }
      LIBSEMIGROUPS_ASSERT(_used_slots[num_edges] <= _words.size());
      return _used_slots[num_edges];
    }
  };

  ///////////////////////////////////////////////////////////////////////////////
  // iterator_base nested class
  ///////////////////////////////////////////////////////////////////////////////

  void Sims2::iterator_base::partial_copy_for_steal_from(
      Sims2::iterator_base const& that) {
    SimsBase::IteratorBase::partial_copy_for_steal_from(that);
    *_2_sided_include = *that._2_sided_include;
    _2_sided_words    = that._2_sided_words;
  }

  Sims2::iterator_base::iterator_base(Sims2 const* s, size_type n)
      : SimsBase::IteratorBase(s, n),
        // protected
        _2_sided_include(new RuleContainer()),
        _2_sided_words() {
    // TODO could be slightly less space allocated here
    size_t const m = SimsBase::IteratorBase::maximum_number_of_classes();
    Presentation<word_type> const& p = this->_felsch_graph.presentation();
    _2_sided_include->resize(2 * m * p.alphabet().size());
    _2_sided_words.assign(n, word_type());
  }

  Sims2::iterator_base::iterator_base(Sims2::iterator_base const& that)
      : SimsBase::IteratorBase(that),
        _2_sided_include(new RuleContainer(*that._2_sided_include)),
        _2_sided_words(that._2_sided_words) {}

  Sims2::iterator_base::iterator_base(Sims2::iterator_base&& that)
      : SimsBase::IteratorBase(std::move(that)),  // TODO std::move correct?
        _2_sided_include(std::move(that._2_sided_include)),
        _2_sided_words(std::move(that._2_sided_words)) {}

  typename Sims2::iterator_base&
  Sims2::iterator_base::operator=(Sims2::iterator_base const& that) {
    SimsBase::IteratorBase::operator=(that);
    *_2_sided_include = *that._2_sided_include;
    _2_sided_words    = that._2_sided_words;

    return *this;
  }

  typename Sims2::iterator_base&
  Sims2::iterator_base::operator=(Sims2::iterator_base&& that) {
    SimsBase::IteratorBase::operator=(std::move(that));  // TODO std::move ok?
    _2_sided_include = std::move(that._2_sided_include);
    _2_sided_words   = std::move(that._2_sided_words);
    return *this;
  }

  void Sims2::iterator_base::swap(Sims2::iterator_base& that) noexcept {
    SimsBase::IteratorBase::swap(that);
    std::swap(_2_sided_include, that._2_sided_include);
    std::swap(_2_sided_words, that._2_sided_words);
  }

  Sims2::iterator_base::~iterator_base() = default;

  bool Sims2::iterator_base::try_define(PendingDef const& current) {
    LIBSEMIGROUPS_ASSERT(current.target < current.num_nodes);
    LIBSEMIGROUPS_ASSERT(current.num_nodes <= maximum_number_of_classes());

    if (!SimsBase::IteratorBase::try_define(current)) {
      return false;
    }

    std::lock_guard lg(_mtx);
    _2_sided_include->backtrack(current.num_edges);

    if (current.target_is_new_node) {
      LIBSEMIGROUPS_ASSERT(current.target < _2_sided_words.size());
      LIBSEMIGROUPS_ASSERT(current.source < _2_sided_words.size());
      _2_sided_words[current.target] = _2_sided_words[current.source];
      _2_sided_words[current.target].push_back(current.generator);
    } else {
      auto const& e = _felsch_graph.definitions()[current.num_edges];
      word_type&  u = _2_sided_include->next_rule_word(current.num_edges);
      u.assign(_2_sided_words[e.first].cbegin(),
               _2_sided_words[e.first].cend());
      u.push_back(e.second);
      word_type&       v = _2_sided_include->next_rule_word(current.num_edges);
      word_type const& w
          = _2_sided_words[_felsch_graph.target_no_checks(e.first, e.second)];
      v.assign(w.begin(), w.end());
    }

    size_type start = current.num_edges;
    while (start < _felsch_graph.definitions().size()) {
      auto first = _2_sided_include->begin(current.num_edges);
      auto last  = _2_sided_include->end(current.num_edges);
      start      = _felsch_graph.definitions().size();
      if (!felsch_graph::make_compatible<RegisterDefs>(
              _felsch_graph,
              0,
              _felsch_graph.number_of_active_nodes(),
              first,
              last)
          || !_felsch_graph.process_definitions(start)) {
        return false;
      }
    }
    return true;
  }

  ////////////////////////////////////////////////////////////////////////
  // Sims1
  ////////////////////////////////////////////////////////////////////////

  Sims1& Sims1::kind(congruence_kind ck) {
    if (ck == congruence_kind::left && kind() != ck) {
      presentation::reverse(_presentation);
      reverse(_include);
      reverse(_exclude);
    }
    _kind = ck;
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // RepOrc helper class
  ////////////////////////////////////////////////////////////////////////

  RepOrc& RepOrc::init() {
    _min  = 0;
    _max  = 0;
    _size = 0;
    return *this;
  }

  Sims1::word_graph_type RepOrc::word_graph() const {
    using word_graph_type = typename Sims1::word_graph_type;
    using node_type       = typename word_graph_type::node_type;

    report_no_prefix("{:+<80}\n", "");
    report_default(
        "RepOrc: Searching for a faithful rep. o.r.c. on [{}, {}) points\n",
        _min,
        _max + 1);
    if (_min > _max || _max == 0) {
      report_no_prefix("{:+<80}\n", "");
      report_default(
          "RepOrc: No faithful rep. o.r.c. exists in [{}, {}) = \u2205\n",
          _min,
          _max + 1);
      return word_graph_type(0, 0);
    }

    SuppressReportFor suppressor("FroidurePin");

    std::atomic_uint64_t count(0);

    auto hook = [&](word_graph_type const& x) {
      ++count;
      if (x.number_of_active_nodes() >= _min) {
        auto first = (presentation().contains_empty_word() ? 0 : 1);
        auto S     = to_froidure_pin<Transf<0, node_type>>(
            x, first, x.number_of_active_nodes());
        // It'd be nice to reuse S here, but this is tricky because hook
        // maybe called in multiple threads, and so we can't easily do this.
        if (presentation().contains_empty_word()) {
          auto one = S.generator(0).identity();
          if (!S.contains(one)) {
            S.add_generator(one);
          }
        }
        LIBSEMIGROUPS_ASSERT(S.size() <= _size);
        if (S.size() == _size) {
          return true;
        }
      }
      return false;
    };

    auto result = Sims1(congruence_kind::right)
                      .settings_copy_from(*this)
                      .find_if(_max, hook);

    if (result.number_of_active_nodes() == 0) {
      report_default(
          "RepOrc: No faithful rep. o.r.c. on [{}, {}) points found\n",
          _min,
          _max + 1);
      result.induced_subgraph_no_checks(0, 0);
    } else {
      report_default("RepOrc: Found a faithful rep. o.r.c. on {} points\n",
                     presentation().contains_empty_word()
                         ? result.number_of_active_nodes()
                         : result.number_of_active_nodes() - 1);
      if (presentation().contains_empty_word()) {
        result.induced_subgraph_no_checks(0, result.number_of_active_nodes());
      } else {
        result.induced_subgraph_no_checks(1, result.number_of_active_nodes());
        result.number_of_active_nodes(result.number_of_active_nodes() - 1);
      }
    }
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // MinimalRepOrc helper class
  ////////////////////////////////////////////////////////////////////////

  // An alternative to the approach used below would be to do a sort of
  // binary search for a minimal representation. It seems that in most
  // examples that I've tried, this actually finds the minimal rep close to
  // the start of the search. The binary search approach would only really
  // be useful if the rep found at the start of the search was much larger
  // than the minimal one, and that the minimal one would not be found until
  // late in the search through all the reps with [1, best rep so far). It
  // seems that in most examples, binary search will involve checking many
  // of the same graphs repeatedly, i.e. if we check [1, size = 1000) find a
  // rep on 57 points, then check [1, 57 / 2) find nothing, then check
  // [57/2, 57) and find nothing, then the search through [57/2, 57)
  // actually iterates through all the digraphs with [1, 57 / 2) again (just
  // doesn't construct a FroidurePin object for these). So, it seems to be
  // best to just search through the digraphs with [1, 57) nodes once.
  //
  // TODO(later) perhaps find minimal 2-sided congruences first (or try to)
  // and then run MinimalRepOrc for right congruences excluding all the
  // generating pairs from the minimal 2-sided congruences. Also with this
  // approach FroidurePin wouldn't be required in RepOrc. This might not work,
  // given that the minimal rc might contain some pairs from minimal 2-sided
  // congs, just not all of them.
  Sims1::word_graph_type MinimalRepOrc::word_graph() const {
    auto cr = RepOrc(*this);

    size_t hi   = (presentation().contains_empty_word() ? _size : _size + 1);
    auto   best = cr.min_nodes(1).max_nodes(hi).target_size(_size).word_graph();

    if (best.number_of_nodes() < 1) {
      stats_copy_from(cr.stats());
      return best;
    }

    hi        = best.number_of_nodes();
    auto next = cr.max_nodes(hi - 1).word_graph();
    while (next.number_of_nodes() != 0) {
      hi   = next.number_of_nodes();
      best = std::move(next);
      next = cr.max_nodes(hi - 1).word_graph();
    }
    stats_copy_from(cr.stats());
    return best;
  }

}  // namespace libsemigroups
