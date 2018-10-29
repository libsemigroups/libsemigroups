//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

//! \file
//!
//! This file contains a generic implementation of a class Orb which represents
//! the action of a semigroup on a set.

// TODO(now):
// 1. A reserve method.
// 2. Check that all of the methods actually work now (i.e. they have not been
//    revised).

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
#include "runner.hpp"
#include "timer.hpp"
#include "traits.hpp"

namespace libsemigroups {

  enum class Side { LEFT = 0, RIGHT = 1 };

  template <typename TElementType,
            typename TPointType,
            typename TActionType
            = ::libsemigroups::right_action<TElementType, TPointType>,
            Side TLeftOrRight    = Side::RIGHT,
            typename TPointHash  = internal::hash<TPointType>,
            typename TPointEqual = internal::equal_to<TPointType>,
            typename TTraits
            = TraitsHashEqual<TPointType, TPointHash, TPointEqual>>
  class Orb : public internal::Runner, private TTraits {
    ////////////////////////////////////////////////////////////////////////
    // Orb - typedefs - private
    ////////////////////////////////////////////////////////////////////////

    using internal_point_type = typename TTraits::internal_element_type;
    using internal_const_point_type =
        typename TTraits::internal_const_element_type;
    using internal_const_reference = typename TTraits::internal_const_reference;

    using internal_equal_to = typename TTraits::internal_equal_to;
    using internal_hash     = typename TTraits::internal_hash;

    static_assert(
        std::is_const<internal_const_point_type>::value
            || std::is_const<typename std::remove_pointer<
                   internal_const_point_type>::type>::value,
        "internal_const_element_type must be const or pointer to const");

   public:
    ////////////////////////////////////////////////////////////////////////
    // Orb - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    using element_type               = TElementType;
    using point_type                 = TPointType;
    using const_reference_point_type = typename TTraits::const_reference;

    using index_type          = size_t;
    using scc_index_type      = ActionDigraph<size_t>::scc_index_type;
    using const_iterator_scc  = ActionDigraph<size_t>::const_iterator_scc;
    using const_iterator_sccs = ActionDigraph<size_t>::const_iterator_sccs;
    using const_iterator_scc_roots
        = ActionDigraph<size_t>::const_iterator_scc_roots;
    using const_iterator = typename std::vector<point_type>::const_iterator;

   private:
    ////////////////////////////////////////////////////////////////////////
    // Orb - internal types with call operators - private
    ////////////////////////////////////////////////////////////////////////

    // gcc apparently requires the extra qualification on the aliases below
    using action  = TActionType;
    using one     = ::libsemigroups::one<element_type>;
    using product = ::libsemigroups::product<element_type>;
    using swap    = ::libsemigroups::swap<element_type>;

    static_assert(std::is_trivially_default_constructible<action>::value,
                  "the third template parameter TActionType is not trivially "
                  "default constructible");

    ////////////////////////////////////////////////////////////////////////
    // Orb - product functions for left/right multiplication  - private
    ////////////////////////////////////////////////////////////////////////

    template <typename SFINAE = void>
    auto internal_product(element_type&       xy,
                          element_type const& x,
                          element_type const& y) ->
        typename std::enable_if<Side::RIGHT == TLeftOrRight, SFINAE>::type {
      product()(xy, x, y);
    }

    template <typename SFINAE = void>
    auto internal_product(element_type&       xy,
                          element_type const& x,
                          element_type const& y) ->
        typename std::enable_if<Side::LEFT == TLeftOrRight, SFINAE>::type {
      product()(xy, y, x);
    }

   public:
    ////////////////////////////////////////////////////////////////////////
    // Orb - constructor + destructor - public
    ////////////////////////////////////////////////////////////////////////

    Orb()
        : _gens(),
          _graph(),
          _map(),
          _orb(),
          _pos(0),
          _tmp_point(),
          _tmp_point_init(false) { }

    ~Orb() {
      if (_tmp_point_init) {
        this->internal_free(_tmp_point);
      }
      for (auto pt : _orb) {
        this->internal_free(pt);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Orb - initialisation member functions - public
    ////////////////////////////////////////////////////////////////////////

    void reserve(size_t val) {
      _graph.reserve(val, _gens.size());
      _map.reserve(val);
      _orb.reserve(val);
    }

    void add_seed(const_reference_point_type seed) {
      auto internal_seed = this->internal_copy(this->to_internal_const(seed));
      if (!_tmp_point_init) {
        _tmp_point_init = true;
        _tmp_point      = this->internal_copy(internal_seed);
      }
      _map.emplace(internal_seed, _orb.size());
      _orb.push_back(internal_seed);
      _graph.add_nodes(1);
      set_finished(false);
    }

    void add_generator(element_type gen) {
      _gens.push_back(gen);
      set_finished(false);
    }

    ////////////////////////////////////////////////////////////////////////
    // Orb - main method - public
    ////////////////////////////////////////////////////////////////////////

    void enumerate() {
      run();
    }

    void run() {
      if (finished()) {
        return;
      }
      size_t old_nr_gens = _graph.out_degree();
      _graph.add_to_out_degree(_gens.size() - _graph.out_degree());
      if (started() && old_nr_gens < _gens.size()) {
        // Generators were added after the last call to run
        for (size_t i = 0; i < _pos; i++) {
          for (size_t j = old_nr_gens; j < _gens.size(); ++j) {
            action()(this->to_external(_tmp_point),
                     this->to_external_const(_orb[i]),
                     _gens[j]);
            auto it = _map.find(_tmp_point);
            if (it == _map.end()) {
              _graph.add_nodes(1);
              _graph.add_edge(i, _orb.size(), j);
              _orb.push_back(this->internal_copy(_tmp_point));
              _map.emplace(_orb.back(), _orb.size() - 1);
            } else {
              _graph.add_edge(i, (*it).second, j);
            }
          }
        }
      }
      set_started(true);

      for (; _pos < _orb.size() && !stopped(); ++_pos) {
        for (size_t j = 0; j < _gens.size(); ++j) {
          action()(this->to_external(_tmp_point),
                   this->to_external_const(_orb[_pos]),
                   _gens[j]);
          auto it = _map.find(_tmp_point);
          if (it == _map.end()) {
            _graph.add_nodes(1);
            _graph.add_edge(_pos, _orb.size(), j);
            _orb.push_back(this->internal_copy(_tmp_point));
            _map.emplace(_orb.back(), _orb.size() - 1);
          } else {
            _graph.add_edge(_pos, (*it).second, j);
          }
        }
        if (report()) {
          REPORT("found ", _orb.size(), " points, so far");
        }
      }
      if (_pos == _orb.size()) {
        set_finished(true);
      }
      report_why_we_stopped();
    }

    ////////////////////////////////////////////////////////////////////////
    // Orb - public member functions
    ////////////////////////////////////////////////////////////////////////

    index_type position(const_reference_point_type pt) {
      auto it = _map.find(this->to_internal_const(pt));
      if (it != _map.end()) {
        return (*it).second;
      } else {
        return UNDEFINED;
      }
    }

    // FIXME this isn't correct (the return type is wrong
    typename std::vector<point_type>::const_iterator find(point_type pt) {
      auto it = _map.find(pt);
      if (it != _map.end()) {
        return _orb.cbegin() + (*it).second;
      } else {
        return _orb.cend();
      }
    }

    bool empty() const {
      return _orb.empty();
    }

    // FIXME return type should be const&
    inline point_type operator[](size_t pos) const {
      LIBSEMIGROUPS_ASSERT(pos < _orb.size());
      return _orb[pos];
    }

    // FIXME return type should be const&
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
      if (pos >= current_size()) {
        (
            "index out of range, expected value in [0, "
            + internal::to_string(current_size()) + ") but found "
            + internal::to_string(pos));
      }
      element_type out = one()();  // TODO Not general enough
      element_type tmp = one()();  // TODO Not general enough
      while (_graph.spanning_forest().parent(pos) != UNDEFINED) {
        swap()(tmp, out);
        internal_product(out, _gens[_graph.spanning_forest().label(pos)], tmp);
        pos = _graph.spanning_forest().parent(pos);
      }
      return out;
    }

    element_type multiplier_to_scc_root(index_type pos) {
      if (pos >= current_size()) {
        (
            "index out of range, expected value in [0, "
            + internal::to_string(current_size()) + ") but found "
            + internal::to_string(pos));
      }
      element_type out = one()();  // TODO Not general enough
      element_type tmp = one()();  // TODO Not general enough
      while (_graph.reverse_spanning_forest().parent(pos) != UNDEFINED) {
        swap()(tmp, out);
        internal_product(
            out, tmp, _gens[_graph.reverse_spanning_forest().label(pos)]);
        pos = _graph.reverse_spanning_forest().parent(pos);
      }
      return out;
    }

    const_iterator cbegin() const {
      return _orb.cbegin();
    }

    const_iterator cend() const {
      return _orb.cend();
    }

    const_iterator_scc cbegin_scc(scc_index_type i) {
      return _graph.cbegin_scc(i);
    }

    const_iterator_scc cend_scc(scc_index_type i) {
      return _graph.cend_scc(i);
    }

    const_iterator_sccs cbegin_sccs() {
      return _graph.cbegin_sccs();
    }

    const_iterator_sccs cend_sccs() {
      return _graph.cend_sccs();
    }

    point_type root_of_scc(point_type x) {
      return _orb[_graph.root_of_scc(position(x))];
    }

    point_type root_of_scc(index_type pos) {
      return _orb[_graph.root_of_scc(pos)];
    }

    const_iterator_scc_roots cbegin_scc_roots() {
      return _graph.cbegin_scc_roots();
    }

    const_iterator_scc_roots cend_scc_roots() {
      return _graph.cend_scc_roots();
    }

    size_t nr_scc() const {
      return _graph.nr_scc();
    }

    ActionDigraph<size_t> const& action_digraph() {
      enumerate();
      return _graph;
    }

   private:
    std::vector<element_type> _gens;
    ActionDigraph<size_t>     _graph;
    std::unordered_map<internal_point_type,
                       size_t,
                       internal_hash,
                       internal_equal_to>
                                     _map;
    std::vector<internal_point_type> _orb;
    size_t                           _pos;
    internal_point_type              _tmp_point;
    bool                             _tmp_point_init;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_ORB_HPP_
