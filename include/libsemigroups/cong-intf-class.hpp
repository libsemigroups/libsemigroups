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

// This file contains a base class for congruence-like classes.

#ifndef LIBSEMIGROUPS_CONG_INTF_CLASS_HPP_
#define LIBSEMIGROUPS_CONG_INTF_CLASS_HPP_

#include <algorithm>  // for equal
#include <cstddef>    // for size_t
#include <vector>     // for vector

#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "runner.hpp"     // for Runner
#include "types.hpp"      // for word_type, tril

namespace libsemigroups {

  //! \defgroup cong_all_group Congruences
  //!
  //! This page contains links to the documentation for the classes Congruence
  //! and CongruenceInterface, and to helper functions for all of the classes in
  //! `libsemigroups` that are derived from CongruenceInterface.
  //! These classes are:
  //! * \ref Congruence
  //! * \ref Kambites
  //! * \ref KnuthBendixBase
  //! * \ref todd_coxeter_class_group "ToddCoxeterBase"

  //! \defgroup cong_all_classes_group Classes
  //! \ingroup cong_all_group
  //!
  //! \brief Documentation for the classes CongruenceInterface and Congruence.
  //!
  //! This page contains links to the documentation for the classes \ref
  //! CongruenceInterface and \ref Congruence.

  //! \ingroup cong_all_classes_group
  //!
  //! \brief Class collecting common aspects of classes representing
  //! congruences.
  //!
  //! Defined in `cong-intf.hpp`.
  //!
  //! Every class for representing a congruence in `libsemigroups` is derived
  //! from CongruenceInterface, which holds the member functions and data that
  //! are common to all its derived classes.
  //!
  //! These classes are:
  //! * \ref Congruence
  //! * \ref Kambites
  //! * \ref KnuthBendixBase
  //! * \ref todd_coxeter_class_group "ToddCoxeterBase"
  class CongruenceInterface : public Runner {
    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - data members - private
    /////////////////////////////////////////////////////////////////////////

    std::vector<word_type> _internal_generating_pairs;
    congruence_kind        _type;

   protected:
    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - constructors + destructor - protected
    ////////////////////////////////////////////////////////////////////////////

    // Constructors + initializers are protected to prevent construction of
    // useless CongruenceInterface objects
    CongruenceInterface() = default;

    CongruenceInterface& init() {
      _internal_generating_pairs.clear();
      Runner::init();
      return *this;
    }

    explicit CongruenceInterface(congruence_kind type) : CongruenceInterface() {
      init(type);
    }

    CongruenceInterface& init(congruence_kind type) {
      init();
      _type = type;
      return *this;
    }

    CongruenceInterface(CongruenceInterface const&);
    CongruenceInterface(CongruenceInterface&&);
    CongruenceInterface& operator=(CongruenceInterface const&);
    CongruenceInterface& operator=(CongruenceInterface&&);

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - validation - protected
    ////////////////////////////////////////////////////////////////////////////

    template <typename Subclass, typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      static_cast<Subclass const*>(this)->throw_if_letter_out_of_bounds(first,
                                                                        last);
    }

   public:
    ~CongruenceInterface();
    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - public member functions
    ////////////////////////////////////////////////////////////////////////////

    //! \brief Returns the number of generating pairs.
    //!
    //! This function returns the number of generating pairs, which is the size
    //! of \ref generating_pairs divided by \f$2\f$.
    //!
    //! \returns
    //! The number of generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t number_of_generating_pairs() const noexcept {
      return _internal_generating_pairs.size() / 2;
    }

    //! \brief Returns a const reference to the std::vector of generating
    //! pairs.
    //!
    //! This function returns the generating pairs of the congruence
    //! represented by any derived class of a CongruenceInterface. This is
    //! always a std::vector of \ref word_type, regardless of the
    //! type of the presentation used by the implementation in the derived
    //! class.
    //!
    //! \returns
    //! A const reference to the generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] std::vector<word_type> const&
    internal_generating_pairs() const noexcept {
      return _internal_generating_pairs;
    }

    //! \brief The kind of the congruence (1- or 2-sided).
    //!
    //! This function returns the kind of the congruence represented by a
    //! derived class of CongruenceInterface. See \ref congruence_kind for
    //! details.
    //!
    //! \return The kind of the congruence (1- or 2-sided).
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] congruence_kind kind() const noexcept {
      return _type;
    }

   protected:
    // This is protected so that it is not possible to change the kind
    // arbitrarily.
    CongruenceInterface& kind(congruence_kind knd) {
      _type = knd;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - add_internal_generating_pair
    ////////////////////////////////////////////////////////////////////////

    // The functions in this section are used as aliases in the derived
    // classes, and so can be protected here.
    template <typename Subclass,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    Subclass& add_internal_generating_pair_no_checks(Iterator1 first1,
                                                     Iterator2 last1,
                                                     Iterator3 first2,
                                                     Iterator4 last2);

    template <typename Subclass,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    Subclass& add_generating_pair(Iterator1 first1,
                                  Iterator2 last1,
                                  Iterator3 first2,
                                  Iterator4 last2);

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - contains
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] tril currently_contains(Iterator1 first1,
                                          Iterator2 last1,
                                          Iterator3 first2,
                                          Iterator4 last2) const;
    template <typename Subclass,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] bool contains_no_checks(Iterator1 first1,
                                          Iterator2 last1,
                                          Iterator3 first2,
                                          Iterator4 last2);

    template <typename Subclass,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] bool contains(Iterator1 first1,
                                Iterator2 last1,
                                Iterator3 first2,
                                Iterator4 last2) {
      throw_if_letter_out_of_bounds<Subclass>(first1, last1);
      throw_if_letter_out_of_bounds<Subclass>(first2, last2);
      return contains_no_checks<Subclass>(first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - reduce
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass,
              typename OutputIterator,
              typename Iterator1,
              typename Iterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 Iterator1      first,
                                 Iterator2      last) const;

    template <typename Subclass,
              typename OutputIterator,
              typename Iterator1,
              typename Iterator2>
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    Iterator1      first,
                                    Iterator2      last) {
      run();
      return static_cast<Subclass const&>(*this).reduce_no_run_no_checks(
          d_first, first, last);
    }

    template <typename Subclass,
              typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last);

   private:
    void throw_if_started() const;
  };  // class CongruenceInterface
}  // namespace libsemigroups

#include "cong-intf-class.tpp"
#endif  // LIBSEMIGROUPS_CONG_INTF_CLASS_HPP_
