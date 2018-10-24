//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

// This file contains ...

#ifndef LIBSEMIGROUPS_INCLUDE_ORB_HPP_
#define LIBSEMIGROUPS_INCLUDE_ORB_HPP_

#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "adapters.hpp"
#include "constants.hpp"
#include "digraph.hpp"
#include "timer.hpp"

namespace libsemigroups {
  // TODO make subclass of Runner
  enum class LeftOrRight {LEFT = 0, RIGHT = 1};

  template <typename TPointType,
            typename TElementType,
            LeftOrRight TLeftOrRight = LeftOrRight::RIGHT>
  class Orb {
   public:
    using element_type = TElementType;
    using point_type   = TPointType;
    using index_type   = size_t;
    using scc_index_type = ActionDigraph<size_t>::scc_index_type;
    using const_iterator_scc = ActionDigraph<size_t>::const_iterator_scc;

   private:
    // gcc apparently requires the extra qualification on the aliases below
    using action  = ::libsemigroups::action<element_type, point_type>;
    using one     = ::libsemigroups::one<element_type>;
    using product = ::libsemigroups::product<element_type>;
    using swap    = ::libsemigroups::swap<element_type>;

    template <typename SFINAE = void>
    auto internal_product(element_type&       xy,
                          element_type const& x,
                          element_type const& y)
        -> typename std::enable_if<LeftOrRight::RIGHT == TLeftOrRight, SFINAE>::type {
      product()(xy, x, y);
    }

    template <typename SFINAE = void>
    auto internal_product(element_type&       xy,
                          element_type const& x,
                          element_type const& y)
        -> typename std::enable_if<LeftOrRight::LEFT == TLeftOrRight, SFINAE>::type {
      product()(xy, y, x);
    }

   public:
    Orb() : _enumerated(false), _gens(), _graph(0), _map(), _orb() {}

    void add_seed(point_type seed) {
      _map.insert(std::make_pair(seed, _orb.size()));
      _orb.push_back(seed);
      _enumerated = false;
    }

    void add_generator(element_type gen) {
      // TODO: what if partial enumerated?
      _gens.push_back(gen);
      _enumerated = false;
    }

    void enumerate() {
      if (_enumerated) {
        return;
      }
      _graph = ActionDigraph<size_t>(_gens.size(), _orb.size());
      for (size_t i = 0; i < _orb.size(); i++) {
        for (size_t j = 0; j < _gens.size(); ++j) {
          point_type pt = action()(_orb[i], _gens[j]);
          auto  it = _map.find(pt);
          if (it == _map.end()) {
            _map.emplace(pt, _orb.size());
            _graph.add_nodes(1);
            _graph.add_edge(i, _orb.size(), j);
            _orb.push_back(pt);
          } else {
            _graph.add_edge(i, (*it).second, j);
          }
        }
      }
      _enumerated = true;
    }

    bool finished() const {
      return _enumerated;
    }

    size_t position(point_type pt) {
      auto it = _map.find(pt);
      if (it != _map.end()) {
        return (*it).second;
      } else {
        return UNDEFINED;
      }
    }

    typename std::vector<point_type>::const_iterator find(point_type pt) {
      auto it = _map.find(pt);
      if (it != _map.end()) {
        return _orb.cbegin() + (*it).second;
      } else {
        return _orb.cend();
      }
    }

    virtual void reserve(size_t n) {
      _map.reserve(n);
      _orb.reserve(n);
    }

    bool empty() const {
      return _orb.empty();
    }

    inline point_type operator[](size_t pos) const {
      LIBSEMIGROUPS_ASSERT(pos < _orb.size());
      return _orb[pos];
    }

    inline point_type at(size_t pos) const {
      return _orb.at(pos);
    }

    size_t size() {
      enumerate();
      return _orb.size();
    }

    size_t current_size() const {
      return _orb.size();
    }

    element_type multiplier_from_scc_root(index_type pos) {
      element_type out = one()(); // TODO Not general enough
      while (_graph.spanning_forest()->parent(pos) != UNDEFINED) {
        out = internal_product(_gens[_graph.spanning_forest()->label(pos)], out);
        pos = _graph.spanning_forest()->parent(pos);
      }
      return out;
    }

    element_type multiplier_to_scc_root(index_type pos) {
      if (pos >= current_size()) {
        throw LIBSEMIGROUPS_EXCEPTION("index out of range, expected value in [0, " +
                                      internal::to_string(current_size()) + ") but found "
                                      + internal::to_string(pos));
      }
      element_type out = one()(); // TODO Not general enough
      element_type tmp = one()(); // TODO Not general enough
      for (auto it = _graph.cbegin_path_to_root(pos);
           it < _graph.cbegin_path_to_root(pos);
           ++it) {
        LIBSEMIGROUPS_ASSERT(*it < _gens.size());
        swap()(tmp, out);
        internal_product(out, tmp, _gens[*it]);
      }
      return out;
    }

    const_iterator_scc cbegin_scc(scc_index_type i) {
      return _graph.cbegin_scc(i);
    }

    const_iterator_scc cend_scc(scc_index_type i) {
      return _graph.cend_scc(i);
    }

   private:
    bool                                   _enumerated;
    std::vector<element_type>              _gens;
    ActionDigraph<size_t>                  _graph;
    std::unordered_map<point_type, size_t> _map;
    std::vector<point_type>                _orb;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_ORB_HPP_
