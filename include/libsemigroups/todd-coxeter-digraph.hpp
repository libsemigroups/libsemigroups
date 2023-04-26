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

// This file contains the declaration of the NodeManagedDigraph class, used
// by Stephen and (by ToddCoxeter in the future).

#ifndef LIBSEMIGROUPS_TODD_COXETER_DIGRAPH_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_DIGRAPH_HPP_

#include <cstddef>  // for size_t
#include <cstdint>  // for uint32_t
#include <vector>   // for vector

#include <cstddef>      // for size_t
#include <cstdint>      // for uint32_t
#include <stack>        // for stack
#include <type_traits>  // for is_base_of
#include <utility>      // for pair
#include <vector>       // for vector

#include "word-graph-with-sources.hpp"  // for WordGraphWithSources
#include "felsch-graph.hpp"
#include "node-manager.hpp"  // for NodeManager
#include "present.hpp"       // for Presentation, Presentation<>:...
#include "report.hpp"        // for REPORT_DEFAULT
#include "runner.hpp"
#include "timer.hpp"       // for Timer
#include "types.hpp"       // for word_type
#include "word-graph.hpp"  // for WordGraph

namespace libsemigroups {
  // TODO merge this with NodeManager
  template <typename BaseDigraph>
  class NodeManagedDigraph
      : public BaseDigraph,
        public detail::NodeManager<typename BaseDigraph::node_type>,
        public Runner {
   public:
    using node_type         = typename BaseDigraph::node_type;
    using base_digraph_type = BaseDigraph;

    static_assert(
        std::is_base_of<WordGraphWithSources<node_type>, BaseDigraph>::value,
        "the template parameter BaseDigraph must be derived from "
        "WordGraphWithSources<node_type>");

   protected:
    using NodeManager_ = detail::NodeManager<node_type>;
    using Coincidence  = std::pair<node_type, node_type>;
    using Coincidences = std::stack<Coincidence>;

    struct Settings;  // forward decl TODO is this really used?
    struct Stats;     // forward decl

    Coincidences _coinc;

   private:
    std::string   _prefix;
    Settings      _settings;
    mutable Stats _stats;

   public:
    using BaseDigraph::BaseDigraph;
    using BaseDigraph::target_no_checks;
    using BaseDigraph::out_degree;

    NodeManagedDigraph()                                     = default;
    NodeManagedDigraph(NodeManagedDigraph const&)            = default;
    NodeManagedDigraph(NodeManagedDigraph&&)                 = default;
    NodeManagedDigraph& operator=(NodeManagedDigraph const&) = default;
    NodeManagedDigraph& operator=(NodeManagedDigraph&&)      = default;

    void init() {
      _coinc    = decltype(_coinc)();
      _settings = Settings();
      _stats    = Stats();
    }

    // TODO corresponding init + rvalue ref version
    // TODO this should be marked explicit but then compilation fails
    template <typename N>
    NodeManagedDigraph(WordGraph<N> const& ad)
        : BaseDigraph(ad), NodeManager_() {
      // NodeManager always has one node active
      NodeManager_::add_active_nodes(WordGraph<node_type>::number_of_nodes()
                                     - 1);
    }

    bool operator==(WordGraph<node_type> const& that) const {
      return static_cast<WordGraph<node_type> const&>(*this) == that;
    }

    NodeManagedDigraph& set_prefix(std::string const& val) {
      _prefix = val;
      return *this;
    }

    NodeManagedDigraph& large_collapse(size_t val) noexcept {
      _settings.large_collapse = val;
      return *this;
    }

    size_t large_collapse() const noexcept {
      return _settings.large_collapse;
    }

    node_type& cursor() {
      return NodeManager_::_current;
    }

    node_type& lookahead_cursor() {
      return NodeManager_::_current_la;
    }

    void reserve(size_t n);

    template <bool RegisterDefs = false>
    std::pair<bool, node_type>
    complete_path(node_type                 c,
                  word_type::const_iterator first,
                  word_type::const_iterator last) noexcept;

    void coincide_nodes(node_type x, node_type y) {
      _coinc.emplace(x, y);
    }

    void report_coincidences() const {
      report_default("{}: coincidences {}\n", _prefix, _coinc.size());
    }

    void report_active_nodes() const;

    template <bool RegisterDefs>
    void process_coincidences();

    // Need by standardize
    void swap_nodes(node_type c, node_type d);

    Stats& stats() noexcept {
      return _stats;
    }

    void stats_check_point() const;

    node_type new_node();

    void permute_nodes_no_checks(std::vector<node_type> const& p,
                                 std::vector<node_type> const& q) {
      BaseDigraph::permute_nodes_no_checks(
          p, q, this->number_of_nodes_active());
      NodeManager_::apply_permutation(p);
    }

    // TODO to t/cpp file
    node_type random_active_node() const {
      static std::random_device rd;
      static std::mt19937       g(rd());

      std::uniform_int_distribution<> d(
          0, NodeManager_::number_of_nodes_active() - 1);
      auto r = NodeManager_::active_nodes();
      rx::advance_by(r, d(g));
      return r.get();
    }

   protected:
    struct CollectCoincidences {
      explicit CollectCoincidences(Coincidences& c) : _coinc(c) {}

      bool operator()(node_type x, node_type y) {
        _coinc.emplace(x, y);
        return true;
      }

      Coincidences& _coinc;
    };

   private:
    void run_impl() {}
    bool finished_impl() const {
      return false;
    }
  };
}  // namespace libsemigroups

#include "todd-coxeter-digraph.tpp"

namespace libsemigroups {
  // TODO move to stephen or its own file.
  class StephenDigraph : public NodeManagedDigraph<WordGraphWithSources<size_t>> {
    using BaseDigraph         = WordGraphWithSources<size_t>;
    using NodeManagedDigraph_ = NodeManagedDigraph<BaseDigraph>;
    using NodeManager_        = typename NodeManagedDigraph_::NodeManager_;

   public:
    StephenDigraph& init(Presentation<word_type> const& p) {
      NodeManager_::clear();
      BaseDigraph::init(NodeManager_::node_capacity(), p.alphabet().size());
      return *this;
    }

    StephenDigraph& init(Presentation<word_type>&& p) {
      NodeManager_::clear();
      BaseDigraph::init(NodeManager_::node_capacity(), p.alphabet().size());
      return *this;
    }
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TODD_COXETER_DIGRAPH_HPP_
