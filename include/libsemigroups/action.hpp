//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains a generic implementation of a class Action which
// represents the action of a semigroup on a set.

#ifndef LIBSEMIGROUPS_ACTION_HPP_
#define LIBSEMIGROUPS_ACTION_HPP_

#include <cstddef>        // for size_t
#include <type_traits>    // for is_trivially_default_construc...
#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

#include "adapters.hpp"          // for One
#include "bruidhinn-traits.hpp"  // for detail::BruidhinnTraits
#include "debug.hpp"             // for LIBSEMIGROUPS_ASSERT
#include "digraph.hpp"           // for ActionDigraph
#include "exception.hpp"         // for LIBSEMIGROUPS_EXCEPTION
#include "report.hpp"            // for REPORT_DEFAULT
#include "runner.hpp"            // for Runner

namespace libsemigroups {
  //! The values in this enum can be used as a template parameter for the Action
  //! class to specify whether the action of the Action  is a left or right
  //! action.
  enum class side {
    //! This value indicates that the action in an Action instance should be a
    //! left action.
    left,
    //! This value indicates that the action in an Action instance should be a
    //! right action.
    right
  };

  //! Defined in ``action.hpp``.
  //!
  //! This page contains details of the Action class in ``libsemigroups`` for
  //! finding actions of semigroups, or groups, on sets.  The notion of an
  //! "action" in the context of ``libsemigroups`` is analogous to the notion
  //! of an orbit of a group.
  //!
  //! You are unlikely to want to use this class directly directly, but rather
  //! via the more convenient aliases \ref RightAction and
  //! \ref LeftAction.
  //!
  //! The function \c run  finds points
  //! that can be obtained by acting on the seeds of \c this by the generators
  //! of \c this until no further points can be found, or \c stopped
  //! returns \c true.  This is achieved by performing a breadth first search.
  //!
  //! \tparam TElementType the type of the elements of the semigroup.
  //!
  //! \tparam TPointType the type of the points acted on.
  //!
  //! \tparam TActionType the type of the action of elements of type
  //! `TElementType` on points of type `TPointType`. This type should be a
  //! stateless trivially default constructible class with a call operator of
  //! signature `void(TPointType&, TPointType const&, TElementType const&)`
  //! which computes the action of the 3rd argument on the 2nd argument, and
  //! stores it in the 1st argument. See, for example,
  //! ImageLeftAction and ImageRightAction.
  //!
  //! \tparam TTraits the type of a traits class with the requirements of
  //! \ref ActionTraits.
  //!
  //! \tparam TLeftOrRight the libsemigroups::side of the action (i.e. if it is
  //! is a left or a right action).
  //!
  //! \sa libsemigroups::side, ActionTraits, LeftAction, and RightAction.
  //!
  //! \par Example
  //! \code
  //! using namespace libsemigroups;
  //! auto rg = ReportGuard(REPORT);
  //! RightAction<PPerm<16>, PPerm<16>, ImageRightAction<PPerm<16>, PPerm<16>>>
  //! o; o.add_seed(PPerm<16>::identity(16)); o.add_generator(
  //!     PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  //!               {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
  //!               16));
  //! o.add_generator(
  //!     PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  //!               {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  //!               16));
  //! o.add_generator(
  //!     PPerm<16>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  //!               {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
  //!               16));
  //! o.add_generator(
  //!     PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
  //!               {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  //!               16));
  //! o.reserve(70000);
  //! o.size(); // 65536
  //! o.digraph().number_of_scc(); // 17
  //! \endcode
  //!
  //! \complexity
  //! The time complexity is \f$O(mn)\f$ where \f$m\f$ is the total
  //! number of points in the orbit and \f$n\f$ is the number of generators.
  template <typename TElementType,
            typename TPointType,
            typename TActionType,
            typename TTraits,
            side TLeftOrRight>
  class Action : public Runner, private detail::BruidhinnTraits<TPointType> {
    ////////////////////////////////////////////////////////////////////////
    // Action - typedefs - private
    ////////////////////////////////////////////////////////////////////////

    using internal_point_type =
        typename detail::BruidhinnTraits<TPointType>::internal_value_type;
    using internal_const_point_type =
        typename detail::BruidhinnTraits<TPointType>::internal_const_value_type;
    using internal_const_reference =
        typename detail::BruidhinnTraits<TPointType>::internal_const_reference;

    struct InternalEqualTo : private detail::BruidhinnTraits<TPointType> {
      using EqualTo = typename TTraits::EqualTo;
      bool operator()(internal_const_point_type x,
                      internal_const_point_type y) const {
        return EqualTo()(this->to_external_const(x),
                         this->to_external_const(y));
      }
    };

    struct InternalHash : private detail::BruidhinnTraits<TPointType> {
      using Hash = typename TTraits::Hash;
      size_t operator()(internal_const_point_type x) const {
        return Hash()(this->to_external_const(x));
      }
    };

    static_assert(
        std::is_const<internal_const_point_type>::value
            || std::is_const<typename std::remove_pointer<
                internal_const_point_type>::type>::value,
        "internal_const_element_type must be const or pointer to const");

   public:
    ////////////////////////////////////////////////////////////////////////
    // Action - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    //! The template parameter \p TElementType.
    //!
    //! Also the type of the elements of the semigroup.
    using element_type = TElementType;

    //! The template parameter \p TPointType.
    //!
    //! Also the type of the points acted on by this class.
    using point_type = TPointType;

    //! The type of a const reference to a \ref point_type.
    using const_reference_point_type =
        typename detail::BruidhinnTraits<TPointType>::const_reference;

    //! The type of a const pointer to a \ref point_type.
    using const_pointer_point_type =
        typename detail::BruidhinnTraits<TPointType>::const_pointer;

    //! The type of the index of a point.
    using index_type = size_t;

    //! The type of the index of a strongly connected component.
    //!
    //! \sa ActionDigraph::scc_index_type
    using scc_index_type = ActionDigraph<size_t>::scc_index_type;

    //! Type of the action of \ref element_type on \ref point_type.
    //!
    //! \sa ImageRightAction, ImageLeftAction
    using action_type = TActionType;

    ////////////////////////////////////////////////////////////////////////
    // Action - iterators - public
    ////////////////////////////////////////////////////////////////////////

    //! The type of a const iterator pointing to a single strongly
    //! connected component (scc).
    using const_iterator_scc = ActionDigraph<size_t>::const_iterator_scc;

    //! The type of a const iterator pointing to a strongly
    //! connected component (scc).
    using const_iterator_sccs = ActionDigraph<size_t>::const_iterator_sccs;

    //! The type of a const iterator pointing to a representative of a
    //! strongly connected component (scc).
    using const_iterator_scc_roots
        = ActionDigraph<size_t>::const_iterator_scc_roots;

    //! The type of a const iterator pointing to a \ref point_type.
    using const_iterator
        = detail::BruidhinnConstIterator<point_type,
                                         std::vector<internal_point_type>>;

   private:
    ////////////////////////////////////////////////////////////////////////
    // Action - internal types with call operators - private
    ////////////////////////////////////////////////////////////////////////

    using ActionOp = TActionType;
    using One      = typename TTraits::One;
    using Product  = typename TTraits::Product;
    using Swap     = typename TTraits::Swap;

    static_assert(std::is_trivially_default_constructible<ActionOp>::value,
                  "the third template parameter TActionType is not trivially "
                  "default constructible");
    // TODO(later) more static assertions

    ////////////////////////////////////////////////////////////////////////
    // Action - product functions for left/right multiplication  - private
    ////////////////////////////////////////////////////////////////////////

    template <typename TSfinae = void>
    auto internal_product(element_type&       xy,
                          element_type const& x,
                          element_type const& y)
        -> std::enable_if_t<side::right == TLeftOrRight, TSfinae> {
      Product()(xy, x, y);
    }

    template <typename TSfinae = void>
    auto internal_product(element_type&       xy,
                          element_type const& x,
                          element_type const& y)
        -> std::enable_if_t<side::left == TLeftOrRight, TSfinae> {
      Product()(xy, y, x);
    }

   public:
    ////////////////////////////////////////////////////////////////////////
    // Action - constructor + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! Default constructor.
    //!
    //! A constructor that creates an Action instance representing a
    //! left or right action.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    Action()
        : _gens(),
          _graph(),
          _map(),
          _options(),
          _orb(),
          _pos(0),
          _tmp_point(),
          _tmp_point_init(false) {}

    //! Default copy constructor.
    Action(Action const&) = default;

    //! Default move constructor.
    Action(Action&&) = default;

    //! Default copy assignment operator.
    Action& operator=(Action const&) = default;

    //! Default move assignment operator.
    Action& operator=(Action&&) = default;

    ~Action() {
      if (_tmp_point_init) {
        this->internal_free(_tmp_point);
      }
      for (auto pt : _orb) {
        this->internal_free(pt);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Action - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    //! Increase the capacity to a value that is greater or equal to \p val.
    //!
    //! \param val new capacity of an action instance.
    //!
    //! \returns (None)
    //!
    //! \throws std::length_error if \p val is too large.
    //!
    //! \throws std::bad_alloc or any exception thrown by the allocators of
    //! private data members.
    //!
    //! \complexity
    //! At most linear in the size() of the Action.
    void reserve(size_t val) {
      _graph.reserve(val, _gens.size());
      _map.reserve(val);
      _orb.reserve(val);
    }

    //! Add a seed to the action.
    //!
    //! A *seed* is just a starting point for the action, it will belong to the
    //! action, as will every point that can be obtained from the seed by
    //! acting with the generators of the action.
    //!
    //! \param seed the seed to add.
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At most linear in the size() of the action.
    void add_seed(const_reference_point_type seed) {
      auto internal_seed = this->internal_copy(this->to_internal_const(seed));
      if (!_tmp_point_init) {
        _tmp_point_init = true;
        _tmp_point      = this->internal_copy(internal_seed);
      }
      _map.emplace(internal_seed, _orb.size());
      _orb.push_back(internal_seed);
      _graph.add_nodes(1);
    }

    //! Add a generator to the action. An Action instance represents the
    //! action of the semigroup generated by the elements added via this
    //! member function.
    //!
    //! \param gen the generator to add.
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At most linear in the size() of the action.
    void add_generator(element_type gen) {
      _gens.push_back(gen);
    }

    ////////////////////////////////////////////////////////////////////////
    // Action - member functions: position, empty, size, etc - public
    ////////////////////////////////////////////////////////////////////////

    //! Returns the position of a point in the so far discovered
    //! points.
    //!
    //! \param pt the point whose position is sought.
    //!
    //! \returns The index of \p pt in \c this or \ref UNDEFINED.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    index_type position(const_reference_point_type pt) const {
      auto it = _map.find(this->to_internal_const(pt));
      if (it != _map.end()) {
        return (*it).second;
      } else {
        return UNDEFINED;
      }
    }

    //! Checks if the Action contains any points.
    //!
    //! \returns \c true if the action contains no points (including seeds),
    //! and \c false if not.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    bool empty() const noexcept {
      return _orb.empty();
    }

    //! Returns a const reference to the point in a given position.
    //!
    //! \param pos the index of an element.
    //!
    //! \returns
    //! A \ref const_reference_point_type to the point in position
    //! \p pos of the currently enumerated points.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    inline const_reference_point_type operator[](size_t pos) const noexcept {
      LIBSEMIGROUPS_ASSERT(pos < _orb.size());
      return this->to_external_const(_orb[pos]);
    }

    //! Returns a const reference to the point in a given position.
    //!
    //! \param pos the index of an element.
    //!
    //! \returns
    //! A \ref const_reference_point_type to the point in position \p
    //! pos of the currently enumerated points.
    //!
    //! \throws
    //! std::out_of_range if `!(pos < current_size())`.
    //!
    //! \complexity
    //! Constant.
    inline const_reference_point_type at(size_t pos) const {
      return this->to_external_const(_orb.at(pos));
    }

    //! Returns the size of the fully enumerated action.
    //!
    //! \returns The size of the action, a value of type \c size_t.
    //!
    //! \complexity
    //! The time complexity is \f$O(mn)\f$ where \f$m\f$ is the total number of
    //! points in the orbit and \f$n\f$ is the number of generators.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Parameters
    //! (None)
    size_t size() {
      run();
      return _orb.size();
    }

    //! Returns the number of points found so far.
    //!
    //! \returns
    //! A value of \c size_t.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \par Parameters
    //! (None)
    size_t current_size() const noexcept {
      return _orb.size();
    }

    //! Returns a \ref const_iterator (random access
    //! iterator) pointing at the first point.
    //!
    //! \returns
    //! A const iterator to the first point.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \par Parameters
    //! (None)
    const_iterator cbegin() const noexcept {
      return const_iterator(_orb.cbegin());
    }

    //! Returns a \ref const_iterator (random access
    //! iterator) pointing one past the last point.
    //!
    //! \returns
    //! A const iterator to one past the end.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \par Parameters
    //! (None)
    const_iterator cend() const noexcept {
      return const_iterator(_orb.cend());
    }

    ////////////////////////////////////////////////////////////////////////
    // Action - member functions: strongly connected components - public
    ////////////////////////////////////////////////////////////////////////

    //! Returns whether or not we are caching scc multipliers.
    //!
    //! If the returned value of this function is \c true, then the values
    //! returned by multiplier_from_scc_root() and multiplier_to_scc_root() are
    //! cached, and not recomputed every time one of these functions is called.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \par Parameters
    //! (None)
    bool cache_scc_multipliers() const noexcept {
      return _options._cache_scc_multipliers;
    }

    //! Set whether or not to cache scc multipliers.
    //!
    //! If the parameter \p val is \c true, then the values returned by
    //! multiplier_from_scc_root() and multiplier_to_scc_root() are cached, and
    //! not recomputed every time one of these functions is called.
    //!
    //! \param val the value.
    //!
    //! \returns
    //! A reference to \c this.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    Action& cache_scc_multipliers(bool val) noexcept {
      _options._cache_scc_multipliers = val;
      return *this;
    }

    //! Returns a multiplier from a scc root to a given index.
    //!
    //! Returns an element \c x of the semigroup generated by the generators
    //! in the action such that if \c r is the root of the strongly connected
    //! component containing \c at(pos), then after `TActionType()(res, r,
    //! x)` the point \c res equals \c at(pos).
    //!
    //! \param pos a position in the action.
    //!
    //! \returns An element of type \ref element_type.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    //!
    //! \throws LibsemigroupsException if there are no generators yet added
    //! or the index \p pos is out of range.
    element_type multiplier_from_scc_root(index_type pos) {
      validate_gens();
      validate_index(pos);
      if (cache_scc_multipliers()) {
        if (_multipliers_from_scc_root.defined(pos)) {
          return _multipliers_from_scc_root[pos];
        }
        _multipliers_from_scc_root.init(_graph.number_of_nodes(), _gens[0]);
        index_type             i = pos;
        std::stack<index_type> visited;
        while (!_multipliers_from_scc_root.defined(i)
               && _graph.spanning_forest().parent(i) != UNDEFINED) {
          visited.push(i);
          _multipliers_from_scc_root[i]
              = _gens[_graph.spanning_forest().label(i)];
          i = _graph.spanning_forest().parent(i);
        }
        if (visited.empty()) {
          // if pos is the scc root, then this can happen
          _multipliers_from_scc_root.set_defined(pos);
        } else {
          element_type tmp = One()(_gens[0]);
          while (i != pos) {
            index_type j = visited.top();
            visited.pop();
            Swap()(tmp, _multipliers_from_scc_root[j]);
            internal_product(_multipliers_from_scc_root[j],
                             _multipliers_from_scc_root[i],
                             tmp);
            _multipliers_from_scc_root.set_defined(j);
            i = j;
          }
        }
        return _multipliers_from_scc_root[pos];
      } else {
        element_type out = One()(_gens[0]);
        element_type tmp = One()(_gens[0]);
        while (_graph.spanning_forest().parent(pos) != UNDEFINED) {
          Swap()(tmp, out);
          internal_product(
              out, _gens[_graph.spanning_forest().label(pos)], tmp);
          pos = _graph.spanning_forest().parent(pos);
        }
        return out;
      }
    }

    //! Returns a multiplier from a given index to a scc root.
    //!
    //! Returns an element \c x of the semigroup generated by the generators
    //! in the action such that after `TActionType()(res, at(pos), x)`
    //! the point \c res is the root of the strongly connected component
    //! containing \c at(pos).
    //!
    //! \param pos a position in the action.
    //!
    //! \returns An element of type \ref element_type.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    //!
    //! \throws LibsemigroupsException if there are no generators yet added
    //! or the index \p pos is out of range.
    element_type multiplier_to_scc_root(index_type pos) {
      validate_gens();
      validate_index(pos);
      if (cache_scc_multipliers()) {
        if (_multipliers_to_scc_root.defined(pos)) {
          return _multipliers_to_scc_root[pos];
        }
        _multipliers_to_scc_root.init(_graph.number_of_nodes(), _gens[0]);
        index_type             i = pos;
        std::stack<index_type> visited;
        while (!_multipliers_to_scc_root.defined(i)
               && _graph.reverse_spanning_forest().parent(i) != UNDEFINED) {
          visited.push(i);
          _multipliers_to_scc_root[i]
              = _gens[_graph.reverse_spanning_forest().label(i)];
          i = _graph.reverse_spanning_forest().parent(i);
        }
        if (visited.empty()) {
          // if pos is the scc root, then this can happen
          _multipliers_to_scc_root.set_defined(pos);
        } else {
          element_type tmp = One()(_gens[0]);
          while (i != pos) {
            index_type j = visited.top();
            visited.pop();
            Swap()(tmp, _multipliers_to_scc_root[j]);
            internal_product(
                _multipliers_to_scc_root[j], tmp, _multipliers_to_scc_root[i]);
            _multipliers_to_scc_root.set_defined(j);
            i = j;
          }
        }
        return _multipliers_to_scc_root[pos];
      } else {
        element_type out = One()(_gens[0]);
        element_type tmp = One()(_gens[0]);
        while (_graph.reverse_spanning_forest().parent(pos) != UNDEFINED) {
          Swap()(tmp, out);
          internal_product(
              out, tmp, _gens[_graph.reverse_spanning_forest().label(pos)]);
          pos = _graph.reverse_spanning_forest().parent(pos);
        }
        return out;
      }
    }

    //! Returns a const reference to the root point of a strongly connected
    //! component.
    //!
    //! \param x the point whose root we want to find.
    //!
    //! \returns A value of type \ref const_reference_point_type.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    //!
    //! \throws LibsemigroupsException if the point \p x does not belong to
    //! the action.
    const_reference_point_type root_of_scc(const_reference_point_type x) {
      return this->to_external_const(_orb[_graph.root_of_scc(position(x))]);
    }

    //! Returns a const reference to the root point of a strongly connected
    //! component.
    //!
    //! \param pos the index of the point in the action whose root we want to
    //! find.
    //!
    //! \returns A value of type \ref const_reference_point_type.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    //!
    //! \throws LibsemigroupsException if the index \p pos is out of range.
    const_reference_point_type root_of_scc(index_type pos) {
      return this->to_external_const(_orb[_graph.root_of_scc(pos)]);
    }

    //! Returns the digraph of the completely enumerated action.
    //!
    //! \returns A const reference to an ActionDigraph<size_t>.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Parameters
    //! (None)
    ActionDigraph<size_t> const& digraph() {
      run();
      return _graph;
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    bool finished_impl() const override {
      return (_pos == _orb.size()) && (_graph.out_degree() == _gens.size());
    }

    void run_impl() override {
      size_t old_nr_gens = _graph.out_degree();
      _graph.add_to_out_degree(_gens.size() - _graph.out_degree());
      if (started() && old_nr_gens < _gens.size()) {
        // Generators were added after the last call to run
        for (size_t i = 0; i < _pos; i++) {
          for (size_t j = old_nr_gens; j < _gens.size(); ++j) {
            ActionOp()(this->to_external(_tmp_point),
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

      for (; _pos < _orb.size() && !stopped(); ++_pos) {
        for (size_t j = 0; j < _gens.size(); ++j) {
          ActionOp()(this->to_external(_tmp_point),
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
          REPORT_DEFAULT("found %d points, so far\n", _orb.size());
        }
      }
      report_why_we_stopped();
    }

    ////////////////////////////////////////////////////////////////////////
    // Action - member functions - private
    ////////////////////////////////////////////////////////////////////////

    void validate_index(index_type i) const {
      if (i > _orb.size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "index out of range, expected value in [0, %d) but found %d",
            current_size(),
            i);
      }
    }

    void validate_gens() const {
      if (_gens.empty()) {
        LIBSEMIGROUPS_EXCEPTION("no generators defined, this methods cannot be "
                                "used until at least one generator is added")
      }
    }

    class MultiplierCache {
     public:
      element_type& operator[](index_type i) {
        return _multipliers[i].second;
      }

      bool defined(index_type i) const {
        return (i < _multipliers.size() ? _multipliers[i].first : false);
      }

      bool set_defined(index_type i) {
        LIBSEMIGROUPS_ASSERT(i < _multipliers.size());
        return _multipliers[i].first = true;
      }

      void init(index_type N, element_type const& sample) {
        if (N > _multipliers.size()) {
          _multipliers.resize(N, {false, One()(sample)});
        }
      }

     private:
      std::vector<std::pair<bool, element_type>> _multipliers;
    };
    ////////////////////////////////////////////////////////////////////////
    // Action - data members - private
    ////////////////////////////////////////////////////////////////////////

    std::vector<element_type> _gens;
    ActionDigraph<size_t>     _graph;
    std::unordered_map<internal_const_point_type,
                       size_t,
                       InternalHash,
                       InternalEqualTo>
        _map;
    struct Options {
      Options() : _cache_scc_multipliers(false) {}
      Options(Options const&) = default;
      Options(Options&&)      = default;
      Options& operator=(Options const&) = default;
      Options& operator=(Options&&) = default;

      bool _cache_scc_multipliers;
    } _options;
    std::vector<internal_point_type> _orb;
    MultiplierCache                  _multipliers_from_scc_root;
    MultiplierCache                  _multipliers_to_scc_root;
    size_t                           _pos;
    internal_point_type              _tmp_point;
    bool                             _tmp_point_init;
  };

  //! This is a traits class for use with Action, \ref LeftAction,
  //! and \ref RightAction.
  //!
  //! \tparam TElementType the type of the elements.
  //! \tparam TPointType the type of the points acted on.
  //!
  //! \sa Action.
  template <typename TElementType, typename TPointType>
  struct ActionTraits {
    //! \copydoc libsemigroups::Hash
    using Hash = ::libsemigroups::Hash<TPointType>;
    //! \copydoc libsemigroups::EqualTo
    using EqualTo = ::libsemigroups::EqualTo<TPointType>;
    //! \copydoc libsemigroups::Swap
    using Swap = ::libsemigroups::Swap<TElementType>;
    //! \copydoc libsemigroups::One
    using One = ::libsemigroups::One<TElementType>;
    //! \copydoc libsemigroups::Product
    using Product = ::libsemigroups::Product<TElementType>;
  };

  //! This class represents the right action of a semigroup on a set.
  //!
  //! \sa Action for further details.
  template <typename TElementType,
            typename TPointType,
            typename TActionType,
            typename TTraits = ActionTraits<TElementType, TPointType>>
  using RightAction
      = Action<TElementType, TPointType, TActionType, TTraits, side::right>;

  //! This class represents the left action of a semigroup on a set.
  //!
  //! \sa Action for further details.
  template <typename TElementType,
            typename TPointType,
            typename TActionType,
            typename TTraits = ActionTraits<TElementType, TPointType>>
  using LeftAction
      = Action<TElementType, TPointType, TActionType, TTraits, side::left>;

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_ACTION_HPP_
