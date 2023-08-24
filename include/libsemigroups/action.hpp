//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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
#include <stack>          // for stack
#include <type_traits>    // for is_trivially_default_construc...
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <vector>         // for vector

#include "adapters.hpp"    // for One
#include "constants.hpp"   // for UNDEFINED
#include "debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
#include "forest.hpp"      // for Forest
#include "gabow.hpp"       // for Gabow
#include "runner.hpp"      // for Runner
#include "word-graph.hpp"  // for WordGraph

#include "detail/bruidhinn-traits.hpp"  // for detail::BruidhinnTraits
#include "detail/report.hpp"            // for REPORT_DEFAULT

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
  //! The function \c run  finds points that can be obtained by acting on the
  //! seeds of \c this by the generators of \c this until no further points can
  //! be found, or \c stopped returns \c true.  This is achieved by performing
  //! a breadth first search.
  //!
  //! \tparam Element the type of the elements of the semigroup.
  //!
  //! \tparam Point the type of the points acted on.
  //!
  //! \tparam Func the type of the action of elements of type
  //! `Element` on points of type `Point`. This type should be a
  //! stateless trivially default constructible class with a call operator of
  //! signature `void(Point&, Point const&, Element const&)`
  //! which computes the action of the 3rd argument on the 2nd argument, and
  //! stores it in the 1st argument. See, for example,
  //! ImageLeftAction and ImageRightAction.
  //!
  //! \tparam Traits the type of a traits class with the requirements of
  //! \ref ActionTraits.
  //!
  //! \tparam LeftOrRight the libsemigroups::side of the action (i.e. if it is
  //! a left or a right action).
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
  //! \par Complexity
  //! The time complexity is \f$O(mn)\f$ where \f$m\f$ is the total
  //! number of points in the orbit and \f$n\f$ is the number of generators.
  template <typename Element,
            typename Point,
            typename Func,
            typename Traits,
            side LeftOrRight>
  class Action : public Runner, private detail::BruidhinnTraits<Point> {
    ////////////////////////////////////////////////////////////////////////
    // Action - typedefs - private
    ////////////////////////////////////////////////////////////////////////

    using internal_point_type =
        typename detail::BruidhinnTraits<Point>::internal_value_type;
    using internal_const_point_type =
        typename detail::BruidhinnTraits<Point>::internal_const_value_type;
    using internal_const_reference =
        typename detail::BruidhinnTraits<Point>::internal_const_reference;

    struct InternalEqualTo : private detail::BruidhinnTraits<Point> {
      using EqualTo = typename Traits::EqualTo;
      bool operator()(internal_const_point_type x,
                      internal_const_point_type y) const {
        return EqualTo()(this->to_external_const(x),
                         this->to_external_const(y));
      }
    };

    struct InternalHash : private detail::BruidhinnTraits<Point> {
      using Hash = typename Traits::Hash;
      size_t operator()(internal_const_point_type x) const {
        return Hash()(this->to_external_const(x));
      }
    };

    // Forward decls
    class MultiplierCache;
    struct Options;

    static_assert(
        std::is_const<internal_const_point_type>::value
            || std::is_const<typename std::remove_pointer<
                internal_const_point_type>::type>::value,
        "internal_const_element_type must be const or pointer to const");

   public:
    ////////////////////////////////////////////////////////////////////////
    // Action - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    //! The template parameter \p Element.
    //!
    //! Also the type of the elements of the semigroup.
    using element_type = Element;

    //! The template parameter \p Point.
    //!
    //! Also the type of the points acted on by this class.
    using point_type = Point;

    //! The type of a const reference to a \ref point_type.
    using const_reference_point_type =
        typename detail::BruidhinnTraits<Point>::const_reference;

    //! The type of a const pointer to a \ref point_type.
    using const_pointer_point_type =
        typename detail::BruidhinnTraits<Point>::const_pointer;

    //! The type of the index of a point.
    using index_type = size_t;

    //! Type of the action of \ref element_type on  \ref point_type.
    //!
    //! \sa ImageRightAction, ImageLeftAction
    using action_type = Func;

    ////////////////////////////////////////////////////////////////////////
    // Action - iterators - public
    ////////////////////////////////////////////////////////////////////////

    //! The type of a const iterator pointing to a \ref point_type.
    using const_iterator
        = detail::BruidhinnConstIterator<point_type,
                                         std::vector<internal_point_type>>;

   private:
    ////////////////////////////////////////////////////////////////////////
    // Action - internal types with call operators - private
    ////////////////////////////////////////////////////////////////////////

    using ActionOp = Func;
    using One      = typename Traits::One;
    using Product  = typename Traits::Product;
    using Swap     = typename Traits::Swap;

    static_assert(std::is_trivially_default_constructible<ActionOp>::value,
                  "the third template parameter Func is not trivially "
                  "default constructible");
    // TODO(later) more static assertions

    ////////////////////////////////////////////////////////////////////////
    // Action - product functions for left/right multiplication  - private
    ////////////////////////////////////////////////////////////////////////

    auto internal_product(element_type&       xy,
                          element_type const& x,
                          element_type const& y) {
      if constexpr (LeftOrRight == side::right) {
        Product()(xy, x, y);
      } else {
        Product()(xy, y, x);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Action - data members - private
    ////////////////////////////////////////////////////////////////////////

    std::vector<element_type> _gens;
    WordGraph<size_t>         _graph;
    std::unordered_map<internal_const_point_type,
                       size_t,
                       InternalHash,
                       InternalEqualTo>
                                     _map;
    Options                          _options;
    std::vector<internal_point_type> _orb;
    MultiplierCache                  _multipliers_from_scc_root;
    MultiplierCache                  _multipliers_to_scc_root;
    size_t                           _pos;
    Gabow<size_t>                    _scc;
    internal_point_type              _tmp_point;
    bool                             _tmp_point_init;

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
    Action();

    //! Default copy constructor.
    Action(Action const&) = default;

    //! Default move constructor.
    Action(Action&&) = default;

    //! Default copy assignment operator.
    Action& operator=(Action const&) = default;

    //! Default move assignment operator.
    Action& operator=(Action&&) = default;

    ~Action();

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
    Action& reserve(size_t val);

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
    Action& add_seed(const_reference_point_type seed);

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
    Action& add_generator(element_type gen) {
      _gens.push_back(gen);
      return *this;
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
    [[nodiscard]] index_type position(const_reference_point_type pt) const;

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
    [[nodiscard]] bool empty() const noexcept {
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
    [[nodiscard]] inline const_reference_point_type
    operator[](size_t pos) const noexcept {
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
    [[nodiscard]] inline const_reference_point_type at(size_t pos) const {
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
    [[nodiscard]] size_t size() {
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
    [[nodiscard]] size_t current_size() const noexcept {
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
    [[nodiscard]] const_iterator cbegin() const noexcept {
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
    [[nodiscard]] const_iterator cend() const noexcept {
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
    [[nodiscard]] bool cache_scc_multipliers() const noexcept {
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
    //! component containing \c at(pos), then after `Func()(res, r,
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
    [[nodiscard]] element_type multiplier_from_scc_root(index_type pos) {
      return multiplier_private<true>(
          _multipliers_from_scc_root, _scc.spanning_forest(), pos);
    }

    //! Returns a multiplier from a given index to a scc root.
    //!
    //! Returns an element \c x of the semigroup generated by the generators
    //! in the action such that after `Func()(res, at(pos), x)`
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
    [[nodiscard]] element_type multiplier_to_scc_root(index_type pos) {
      return multiplier_private<false>(
          _multipliers_to_scc_root, _scc.reverse_spanning_forest(), pos);
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
    [[nodiscard]] const_reference_point_type
    root_of_scc(const_reference_point_type x) {
      return root_of_scc(position(x));
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
    [[nodiscard]] const_reference_point_type root_of_scc(index_type pos) {
      return this->to_external_const(_orb[_scc.root_of(pos)]);
    }

    //! Returns the digraph of the completely enumerated action.
    //!
    //! \returns A const reference to an WordGraph<size_t>.
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
    [[nodiscard]] WordGraph<size_t> const& digraph() {
      run();
      return _graph;
    }

    [[nodiscard]] Gabow<size_t> const& scc() {
      run();
      return _scc;
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] bool finished_impl() const override {
      return (_pos == _orb.size()) && (_graph.out_degree() == _gens.size());
    }

    void run_impl() override;

    ////////////////////////////////////////////////////////////////////////
    // Action - member functions - private
    ////////////////////////////////////////////////////////////////////////

    template <bool Forward>
    element_type multiplier_private(MultiplierCache& mults,
                                    Forest const&    f,
                                    index_type       pos);

    void validate_index(index_type i) const;
    void validate_gens() const;
  };

  //! This is a traits class for use with Action, \ref LeftAction,
  //! and \ref RightAction.
  //!
  //! \tparam Element the type of the elements.
  //! \tparam Point the type of the points acted on.
  //!
  //! \sa Action.
  template <typename Element, typename Point>
  struct ActionTraits {
    //! \copydoc libsemigroups::Hash
    using Hash = ::libsemigroups::Hash<Point>;
    //! \copydoc libsemigroups::EqualTo
    using EqualTo = ::libsemigroups::EqualTo<Point>;
    //! \copydoc libsemigroups::Swap
    using Swap = ::libsemigroups::Swap<Element>;
    //! \copydoc libsemigroups::One
    using One = ::libsemigroups::One<Element>;
    //! \copydoc libsemigroups::Product
    using Product = ::libsemigroups::Product<Element>;
  };

  //! This class represents the right action of a semigroup on a set.
  //!
  //! \sa Action for further details.
  template <typename Element,
            typename Point,
            typename Func,
            typename Traits = ActionTraits<Element, Point>>
  using RightAction = Action<Element, Point, Func, Traits, side::right>;

  //! This class represents the left action of a semigroup on a set.
  //!
  //! \sa Action for further details.
  template <typename Element,
            typename Point,
            typename Func,
            typename Traits = ActionTraits<Element, Point>>
  using LeftAction = Action<Element, Point, Func, Traits, side::left>;

}  // namespace libsemigroups

#include "action.tpp"
#endif  // LIBSEMIGROUPS_ACTION_HPP_
