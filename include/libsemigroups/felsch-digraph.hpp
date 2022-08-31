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

// This file contains a declaration of a class called FelschDigraph which is
// used by the classes Sims1 and ToddCoxeter.

#ifndef LIBSEMIGROUPS_FELSCH_DIGRAPH_HPP_
#define LIBSEMIGROUPS_FELSCH_DIGRAPH_HPP_

#include <cstddef>

#include "digraph-with-sources.hpp"
#include "felsch-tree.hpp"
#include "present.hpp"

namespace libsemigroups {
  template <typename W, typename N>
  class FelschDigraph : public DigraphWithSources<N> {
   public:
    using node_type    = N;
    using word_type    = W;
    using letter_type  = typename W::value_type;
    using digraph_type = DigraphWithSources<node_type>;
    using size_type    = typename digraph_type::size_type;

   private:
    using Definition  = std::pair<node_type, letter_type>;
    using Definitions = std::vector<Definition>;

    Definitions             _definitions;
    detail::FelschTree      _felsch_tree;
    Presentation<word_type> _presentation;

   public:
    using DigraphWithSources<node_type>::DigraphWithSources;

    FelschDigraph(Presentation<word_type> const& p, size_type n);

    FelschDigraph()                     = default;
    FelschDigraph(FelschDigraph const&) = default;
    FelschDigraph(FelschDigraph&&)      = default;
    FelschDigraph& operator=(FelschDigraph const&) = default;
    FelschDigraph& operator=(FelschDigraph&&) = default;

    bool def_edge(node_type c, letter_type x, node_type d) noexcept;

    // Returns true if no contradictions are found.
    bool process_definitions(size_t start);

    size_type number_of_edges() const noexcept {
      return _definitions.size();
    }

    void reduce_number_of_edges_to(size_type n);

    bool compatible(node_type        c,
                    word_type const& u,
                    word_type const& v) noexcept;

    bool operator==(FelschDigraph const& that) const {
      size_type const m = this->number_of_active_nodes();
      size_type const n = that.number_of_active_nodes();
      return (m == 0 && n == 0)
             || (m == n && this->ActionDigraph<node_type>::operator==(that));
    }

   private:
    bool process_definitions_dfs_v1(node_type c);

    inline bool compatible(node_type const& c, size_t i) noexcept {
      auto j = (i % 2 == 0 ? i + 1 : i - 1);
      return compatible(c, _presentation.rules[i], _presentation.rules[j]);
    }
  };

  namespace felsch_digraph {
    template <typename W, typename N>
    bool compatible(FelschDigraph<W, N>&                    fd,
                    typename FelschDigraph<W, N>::node_type first_node,
                    typename FelschDigraph<W, N>::node_type last_node,
                    typename std::vector<W>::const_iterator first_rule,
                    typename std::vector<W>::const_iterator last_rule) noexcept;

    template <typename W, typename N>
    bool compatible(FelschDigraph<W, N>&                    fd,
                    typename FelschDigraph<W, N>::node_type node,
                    typename std::vector<W>::const_iterator first_rule,
                    typename std::vector<W>::const_iterator last_rule) noexcept;

  }  // namespace felsch_digraph
}  // namespace libsemigroups

#include "felsch-digraph.tpp"

#endif  // LIBSEMIGROUPS_FELSCH_DIGRAPH_HPP_
