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
#include "detail/report.hpp"            // for report_default

namespace libsemigroups {
  //! \defgroup action_group Actions
  //! This page contains an overview of the functionality in
  //! `libsemigroups` for finding actions of semigroups, or groups, on sets.
  //! The notion of an "action" in the context of `libsemigroups` is analogous
  //! to the notion of an orbit of a group.
  //!
  //! You are unlikely to want to use Action directly, but rather via the more
  //! convenient aliases \ref RightAction and \ref LeftAction. To use
  //! \ref RightAction and \ref LeftAction with custom types, actions, and so
  //! on, see ActionTraits.
  //!
  //! See also ImageLeftAction and ImageRightAction.
  //!
  //! \code
  //! using namespace libsemigroups;
  //! RightAction<PPerm<16>, PPerm<16>, ImageRightAction<PPerm<16>, PPerm<16>>>
  //! o;
  //! o.add_seed(PPerm<16>::one(16));
  //! o.add_generator(
  //!    PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  //!              {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
  //!              16));
  //! o.add_generator(
  //!    PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  //!              {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  //!              16));
  //! o.add_generator(
  //!    PPerm<16>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  //!              {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
  //!              16));
  //! o.add_generator(
  //!    PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
  //!              {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  //!              16));
  //! o.reserve(70000);
  //! o.size();  //-> 65536
  //! \endcode

  //! \ingroup types_group
  //! \brief Enum class indicating the handedness or side of an action.
  //!
  //! The values in this enum can be used as a template parameter for the Action
  //! class to specify whether the action of the Action  is a left or right
  //! action.
  //!
  //! \sa action_group
  enum class side {
    //! This value indicates that the action in an Action instance should be a
    //! left action.
    left,
    //! This value indicates that the action in an Action instance should be a
    //! right action.
    right
  };

  //! \ingroup action_group
  //!
  //! \brief Class for generating the action of a semigroup.
  //!
  //! Defined in `action.hpp`.
  //!
  //! This page contains details of the Action class in `libsemigroups` for
  //! finding actions of semigroups, or groups, on sets.  The notion of an
  //! "action" in the context of `libsemigroups` is analogous to the notion
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
  //! which computes the action of its 3rd argument on its 2nd argument, and
  //! stores it in its 1st argument. See, for example,
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
  //! auto rg = ReportGuard(true);
  //! RightAction<PPerm<16>, PPerm<16>, ImageRightAction<PPerm<16>, PPerm<16>>>
  //! o;
  //! o.add_seed(PPerm<16>::one(16));
  //! o.add_generator(
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
  //! o.size(); //-> 65536
  //! o.scc().number_of_components(); //-> 17
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

    using Degree = typename Traits::Degree;

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

    //! \brief The template parameter \p Element.
    //!
    //! The template parameter \p Element, which is the type of the elements of
    //! the semigroup whose action is represented by an Action instance.
    using element_type = Element;

    //! \brief The template parameter \p Point.
    //!
    //! The template parameter \p Point, which is the type of the points of
    //! acted on in an Action instance.
    using point_type = Point;

    //! \brief The type of the index of a point.
    //!
    //! Type of indices for values in an Action instance.
    //!
    //! \sa \ref operator[] and \ref at.
    using index_type = size_t;

    //! \brief Type of the action of \ref element_type on  \ref point_type.
    //!
    //! The type of the action of elements of type `Element` on points of type
    //! `Point`. This type should be a stateless trivially default constructible
    //! class with a call operator of signature
    //! `void(Point&, Point const&, Element const&)` which computes the action
    //! of its 3rd argument on its 2nd argument, and stores it in its 1st
    //! argument.
    //!
    //! \sa ImageRightAction, ImageLeftAction
    using action_type = Func;

    //! \brief Insertion operator.
    //!
    //! This member function allows Action objects to be inserted into an
    //! std::ostringstream.
    template <typename Elment,
              typename Pint,
              typename Fnc,
              typename Trits,
              side LftOrRight>
    // TODO make this a free function, and non-friend
    friend std::ostringstream&
    operator<<(std::ostringstream&                                 os,
               Action<Elment, Pint, Fnc, Trits, LftOrRight> const& action);

    //! \brief Insertion operator.
    //!
    //! This member function allows Action objects to be inserted into a
    //! std::ostream.
    // TODO make this a free function, and non-friend
    friend std::ostream& operator<<(std::ostream& os, Action const& action) {
      os << detail::to_string(action);
      return os;
    }

    ////////////////////////////////////////////////////////////////////////
    // Action - iterators - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief The type of a const iterator pointing to a \ref point_type.
    //!
    //! The type of a const iterator pointing to the \ref point_type values
    //! contained in an Action object.
    using const_iterator
        = detail::BruidhinnConstIterator<point_type,
                                         std::vector<internal_point_type>>;

    //! \brief The type of a const reference to a \ref point_type.
    //!
    //! The type of a const reference to the \ref point_type values
    //! contained in an Action object.
    using const_reference_point_type =
        typename detail::BruidhinnTraits<Point>::const_reference;

    //! \brief The type of a const pointer to a \ref point_type.
    //!
    //! The type of a const pointer to the \ref point_type values
    //! contained in an Action object.
    using const_pointer_point_type =
        typename detail::BruidhinnTraits<Point>::const_pointer;

   private:
    ////////////////////////////////////////////////////////////////////////
    // Action - internal types with call operators - private
    ////////////////////////////////////////////////////////////////////////

    using ActionOp = Func;
    using One      = typename Traits::One;
    using Product  = typename Traits::Product;
    using Swap     = typename Traits::Swap;

    static_assert(std::is_trivially_default_constructible<ActionOp>::value,
                  "the 3rd template parameter Func is not trivially "
                  "default constructible");
    // TODO(2) more static assertions

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

    // TODO(1) change size_t -> uint32_t
    std::vector<element_type> _gens;
    WordGraph<uint32_t>       _graph;
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
    Gabow<uint32_t>                  _scc;
    internal_point_type              _tmp_point;
    bool                             _tmp_point_init;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Action - constructor + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Default constructor.
    //!
    //! A constructor that creates an uninitialized Action instance representing
    //! a left or right action.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    Action();

    //! \brief Initialize an existing Action object.
    //!
    //! This function puts a Action object back into the same state as if it
    //! had been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Action& init();

    //! Default copy constructor.
    Action(Action const& that) : Action() {
      *this = that;
    }

    //! Default move constructor.
    Action(Action&& that) : Action() {
      *this = std::move(that);
    }

    //! Default copy assignment operator.
    Action& operator=(Action const&);

    //! Default move assignment operator.
    Action& operator=(Action&&);

    ~Action();

    ////////////////////////////////////////////////////////////////////////
    // Action - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Increase the capacity to a value that is greater or equal to
    //! \p val.
    //!
    //! Increase the capacity to a value that is greater or equal to \p val.
    //!
    //! \param val new capacity of an action instance.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws std::length_error if \p val is too large.
    //!
    //! \complexity
    //! At most linear in the size() of the Action.
    Action& reserve(size_t val);

    //! \brief Add a seed to the action.
    //!
    //! A *seed* is just a starting point for the action, it will belong to the
    //! action, as will every point that can be obtained from the seed by
    //! acting with the generators of the action.
    //!
    //! \param seed the seed to add.
    //!
    //! \returns A reference to `*this`
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    Action& add_seed(const_reference_point_type seed);

    //! \brief Add a generator to the action.
    //!
    //! An Action instance represents the action of the semigroup generated by
    //! the elements added via this member function.
    //!
    //! \param gen the generator to add.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if the degree of \p gen is not the same
    //! as the existing generators (if any).
    //!
    //! \complexity
    //! Constant.
    Action& add_generator(element_type const& gen) {
      throw_if_bad_degree(gen);
      return add_generator_no_checks(gen);
    }

    //! \brief Add a generator to the action.
    //!
    //! An Action instance represents the action of the semigroup generated by
    //! the elements added via this member function.
    //!
    //! \param gen the generator to add.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    Action& add_generator_no_checks(element_type const& gen) {
      _gens.push_back(gen);
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // Action - member functions: position, empty, size, etc - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns the number of generators.
    //!
    //! \returns The number of generators.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t number_of_generators() const noexcept {
      return _gens.size();
    }

    //! \brief Returns a const reference to the vector of generators.
    //!
    //! \returns A value of type `std::vector<Element>`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] std::vector<Element> const& generators() const noexcept {
      return _gens;
    }

    //! \brief Returns the position of a point in the so far discovered
    //! points.
    //!
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

    //! \brief Checks if the Action contains any points.
    //!
    //! \returns \c true if the action contains no points (including seeds),
    //! and \c false if not.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] bool empty() const noexcept {
      return _orb.empty();
    }

    //! \brief Returns a const reference to the point in a given position.
    //!
    //! \param pos the index of a point.
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
    operator[](index_type pos) const noexcept {
      LIBSEMIGROUPS_ASSERT(pos < _orb.size());
      return this->to_external_const(_orb[pos]);
    }

    //! \brief Returns a const reference to the point in a given position.
    //!
    //! \param pos the index of a point.
    //!
    //! \returns
    //! A \ref const_reference_point_type to the point in position
    //! \p pos of the currently enumerated points.
    //!
    //! \throws
    //! std::out_of_range if `!(pos < current_size())`.
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] inline const_reference_point_type at(index_type pos) const {
      return this->to_external_const(_orb.at(pos));
    }

    //! \brief Returns the size of the fully enumerated action.
    //!
    //! Returns the size of the fully enumerated action.
    //!
    //! \returns The size of the action, a value of type \c size_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! The time complexity is \f$O(mn)\f$ where \f$m\f$ is the total number of
    //! points in the orbit and \f$n\f$ is the number of generators.
    [[nodiscard]] size_t size() {
      run();
      return _orb.size();
    }

    //! \brief Returns the number of points found so far.
    //!
    //! Returns the number of points found so far.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t current_size() const noexcept {
      return _orb.size();
    }

    //! \brief Returns a \ref const_iterator (random access
    //! iterator) pointing at the first point.
    //!
    //!  This function returns a \ref const_iterator to the first point in the
    //!  action (if any). No enumeration is triggered by calling this function.
    //!
    //! \returns
    //! A const iterator to the first point.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] const_iterator cbegin() const noexcept {
      return const_iterator(_orb.cbegin());
    }

    //! \copydoc cbegin
    [[nodiscard]] const_iterator begin() const noexcept {
      return cbegin();
    }

    //! \brief Returns a range object containing the current points in the
    //! action.
    //!
    //! Returns a range object containing the current points in the
    //! action.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    // TODO(1) add a reference to the ranges page when it exists
    [[nodiscard]] auto range() const noexcept {
      return rx::iterator_range(_orb.cbegin(), _orb.cend())
             | rx::transform([this](auto const& pt) {
                 return this->to_external_const(pt);
               });
    }

    //! \brief Returns a \ref const_iterator (random access iterator) pointing
    //! one past the last point.
    //!
    //! This function returns a \ref const_iterator pointing one beyond the
    //! last point in the action (if any). No enumeration is triggered by
    //! calling this function.
    //!
    //! \returns
    //! A const iterator to one past the end.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] const_iterator cend() const noexcept {
      return const_iterator(_orb.cend());
    }

    //! \copydoc cend
    [[nodiscard]] const_iterator end() const noexcept {
      return cend();
    }

    ////////////////////////////////////////////////////////////////////////
    // Action - member functions: strongly connected components - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns whether or not we are caching scc multipliers.
    //!
    //! If the returned value of this function is \c true, then the values
    //! returned by multiplier_from_scc_root() and multiplier_to_scc_root() are
    //! cached, and not recomputed every time one of these functions is called.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] bool cache_scc_multipliers() const noexcept {
      return _options._cache_scc_multipliers;
    }

    //! \brief Set whether or not to cache scc multipliers.
    //!
    //! If the parameter \p val is \c true, then the values returned by
    //! multiplier_from_scc_root() and multiplier_to_scc_root() are cached, and
    //! not recomputed every time one of these functions is called.
    //!
    //! \param val the value.
    //!
    //! \returns
    //! A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Action& cache_scc_multipliers(bool val) noexcept {
      _options._cache_scc_multipliers = val;
      return *this;
    }

    //! \brief Returns a multiplier from a scc root to a given index.
    //!
    //! Returns an element \c x of the semigroup generated by the generators
    //! in the action such that if \c r is the root of the strongly connected
    //! component containing \c at(pos), then after
    //! `Func()(res, r, x)` the point \c res equals \c at(pos).
    //!
    //! \param pos a position in the action.
    //!
    //! \returns An element of type \ref element_type.
    //!
    //! \throws LibsemigroupsException if there are no generators yet added
    //! or the index \p pos is out of range.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    [[nodiscard]] element_type multiplier_from_scc_root(index_type pos) {
      return multiplier_private<true>(
          _multipliers_from_scc_root, _scc.spanning_forest(), pos);
    }

    //! \brief Returns a multiplier from a given index to a scc root.
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
    //! \throws LibsemigroupsException if there are no generators yet added
    //! or the index \p pos is out of range.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    [[nodiscard]] element_type multiplier_to_scc_root(index_type pos) {
      return multiplier_private<false>(
          _multipliers_to_scc_root, _scc.reverse_spanning_forest(), pos);
    }

    //! \brief Returns a const reference to the root point of a strongly
    //! connected component.
    //!
    //! Returns a const reference to the root point of a strongly
    //! connected component.
    //!
    //! \param x the point whose root we want to find.
    //!
    //! \returns A value of type \ref const_reference_point_type.
    //!
    //! \throws LibsemigroupsException if the point \p x does not belong to
    //! the action.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    [[nodiscard]] const_reference_point_type
    root_of_scc(const_reference_point_type x) {
      // TODO(2) this could be a helper
      return root_of_scc(position(x));
    }

    //! \brief Returns a const reference to the root point of a strongly
    //! connected component.
    //!
    //! Returns a const reference to the root point of a strongly
    //! connected component.
    //!
    //! \param pos the index of the point in the action whose root we want to
    //! find.
    //!
    //! \returns A value of type \ref const_reference_point_type.
    //!
    //! \throws LibsemigroupsException if the index \p pos is out of range.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    [[nodiscard]] const_reference_point_type root_of_scc(index_type pos) {
      return this->to_external_const(_orb[_scc.root_of(pos)]);
    }

    //! Returns the word graph of the completely enumerated action.
    //!
    //! \returns A const reference to a WordGraph<uint32_t>.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    [[nodiscard]] WordGraph<uint32_t> const& word_graph() {
      run();
      return _graph;
    }

    //! \brief Returns a reference to a Gabow object for strongly connected
    //! components.
    //!
    //! Returns a reference to a Gabow object for strongly connected
    //! components.
    //!
    //! \returns A const reference to a Gabow object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the complexity of multiplying
    //! elements of type \ref element_type and \f$n\f$ is the size of the fully
    //! enumerated orbit.
    [[nodiscard]] Gabow<uint32_t> const& scc() {
      run();
      return _scc;
    }

    //! \brief Apply the template parameter Func in-place.
    //!
    //! This static function applies the template parameter \c Func to the
    //! point \p pt and element \p x and modifies \p result in-place to hold
    //! the result.
    //!
    //! \param result the point to hold the result.
    //! \param pt the point to apply the action function to.
    //! \param x the element to apply the action function to.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // These functions allow the action operation (defined by Func) to be called
    // on an instance of Action. This isn't very helpful in C++ code, but is in
    // the python bindings, where figuring out the type of Func is a bit fiddly.
    static void apply(point_type&         result,
                      point_type const&   pt,
                      element_type const& x) {
      ActionOp()(result, pt, x);
    }

    //! \brief Apply the template parameter Func.
    //!
    //! This static function applies the template parameter \c Func to the
    //! point \p pt and element \p x and returns the result in a newly
    //! constructed \c point_type.
    //!
    //! \param pt the point to apply the action function to.
    //! \param x the element to apply the action function to.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    [[nodiscard]] static point_type apply(point_type const&   pt,
                                          element_type const& x) {
      point_type result;
      apply(result, pt, x);
      return result;
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

    void throw_if_index_out_of_range(index_type i) const;
    void throw_if_no_generators() const;
    void throw_if_bad_degree(element_type const&) const;
  };  // class Action

  //! \ingroup action_group
  //! This is a traits class for use with Action, \ref LeftAction,
  //! and \ref RightAction.
  //!
  //! \tparam Element the type of the elements.
  //! \tparam Point the type of the points acted on.
  //!
  //! \sa Action.
  template <typename Element, typename Point>
  struct ActionTraits {
    //! \copydoc libsemigroups::Degree
    using Degree = ::libsemigroups::Degree<Element>;
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

  //! \ingroup action_group
  //! This class represents the right action of a semigroup on a set.
  //!
  //! \sa Action for further details.
  template <typename Element,
            typename Point,
            typename Func   = ImageRightAction<Element, Point>,
            typename Traits = ActionTraits<Element, Point>>
  using RightAction = Action<Element, Point, Func, Traits, side::right>;

  //! \ingroup action_group
  //! This class represents the left action of a semigroup on a set.
  //!
  //! \sa Action for further details.
  template <typename Element,
            typename Point,
            typename Func   = ImageLeftAction<Element, Point>,
            typename Traits = ActionTraits<Element, Point>>
  using LeftAction = Action<Element, Point, Func, Traits, side::left>;

  //! \ingroup action_group
  //!
  //! \brief Return a human readable representation of an Action object.
  //!
  //! Defined in `action.hpp`.
  //!
  //! This function returns a human readable representation of an Action
  //! object.
  //!
  //! \param action the Action.
  //!
  //! \returns A std::string containing the representation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Element,
            typename Point,
            typename Func,
            typename Traits,
            side LeftOrRight>
  std::string to_human_readable_repr(
      Action<Element, Point, Func, Traits, LeftOrRight> const& action);
}  // namespace libsemigroups

#include "action.tpp"
#endif  // LIBSEMIGROUPS_ACTION_HPP_
