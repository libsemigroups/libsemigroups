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

// This file contains the declaration of the ToddCoxeterDigraph class, used
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

#include "digraph-with-sources.hpp"  // for DigraphWithSources
#include "digraph.hpp"               // for ActionDigraph
#include "node-manager.hpp"          // for NodeManager
#include "present.hpp"               // for Presentation, Presentation<>:...
#include "report.hpp"                // for REPORT_DEFAULT
#include "types.hpp"                 // for word_type

namespace libsemigroups {
  namespace detail {

    template <typename BaseDigraph>
    class ToddCoxeterDigraph : public BaseDigraph, public NodeManager {
      static_assert(
          std::is_base_of<DigraphWithSources<size_t>, BaseDigraph>::value,
          "the template parameter BaseDigraph must be derived from "
          "DigraphWithSources<size_t>");

     public:
      using node_type = typename BaseDigraph::node_type;

     private:
      using Coincidence  = std::pair<node_type, node_type>;
      using Coincidences = std::stack<Coincidence>;

      struct Settings;  // forward decl
      struct Stats;     // forward decl

      Coincidences _coinc;
      Settings     _settings;
      Stats        _stats;

     public:
      using BaseDigraph::BaseDigraph;
      using BaseDigraph::out_degree;
      using BaseDigraph::unsafe_neighbor;

      ToddCoxeterDigraph()                                     = default;
      ToddCoxeterDigraph(ToddCoxeterDigraph const&)            = default;
      ToddCoxeterDigraph(ToddCoxeterDigraph&&)                 = default;
      ToddCoxeterDigraph& operator=(ToddCoxeterDigraph const&) = default;
      ToddCoxeterDigraph& operator=(ToddCoxeterDigraph&&)      = default;

      ToddCoxeterDigraph& init(Presentation<word_type> const& p) {
        NodeManager::clear();
        BaseDigraph::init(node_capacity(), p.alphabet().size());
        return *this;
      }

      ToddCoxeterDigraph init(Presentation<word_type>&& p) {
        NodeManager::clear();
        BaseDigraph::init(node_capacity(), p.alphabet().size());
        return *this;
      }

      bool operator==(ActionDigraph<uint32_t> const& that) const {
        return static_cast<ActionDigraph<uint32_t>>(*this) == that;
      }

      ToddCoxeterDigraph& large_collapse(size_t val) noexcept {
        _settings.large_collapse = val;
        return *this;
      }

      size_t large_collapse() const noexcept {
        return _settings.large_collapse;
      }

      node_type& cursor() {
        return _current;
      }

      void reserve(size_t n);

      std::pair<bool, node_type>
      complete_path(node_type                 c,
                    word_type::const_iterator first,
                    word_type::const_iterator last) noexcept;

      void coincide_nodes(node_type x, node_type y) {
        _coinc.emplace(x, y);
      }

      void process_coincidences();

      void swap_nodes(node_type c, node_type d);

      Stats& stats() noexcept {
        return _stats;
      }

      void stats_check_point();

     private:
      node_type new_node();
    };

  }  // namespace detail
}  // namespace libsemigroups

#include "todd-coxeter-digraph.tpp"

#endif  // LIBSEMIGROUPS_TODD_COXETER_DIGRAPH_HPP_
