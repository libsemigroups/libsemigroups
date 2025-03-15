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

// This file contains a base class for congruence-like classes.

#ifndef LIBSEMIGROUPS_DETAIL_CONG_COMMON_CLASS_HPP_
#define LIBSEMIGROUPS_DETAIL_CONG_COMMON_CLASS_HPP_

#include <algorithm>  // for equal
#include <cstddef>    // for size_t
#include <vector>     // for vector

#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/runner.hpp"     // for Runner
#include "libsemigroups/types.hpp"      // for word_type, tril

namespace libsemigroups {
  namespace detail {

    class CongruenceCommon : public Runner {
      /////////////////////////////////////////////////////////////////////////
      // CongruenceCommon - data members - private
      /////////////////////////////////////////////////////////////////////////

      // TODO(1) given that each of ToddCoxeter<>, KnuthBendix<>, and Kambites<>
      // now contain their generating pairs, it's not clear that we need to
      // store them here as well.
      std::vector<word_type> _internal_generating_pairs;
      congruence_kind        _type;

     protected:
      ////////////////////////////////////////////////////////////////////////////
      // CongruenceCommon - constructors + destructor - protected
      ////////////////////////////////////////////////////////////////////////////

      // Constructors + initializers are protected to prevent construction of
      // useless CongruenceCommon objects
      CongruenceCommon() = default;

      CongruenceCommon& init() {
        _internal_generating_pairs.clear();
        Runner::init();
        return *this;
      }

      explicit CongruenceCommon(congruence_kind type) : CongruenceCommon() {
        init(type);
      }

      CongruenceCommon& init(congruence_kind type) {
        init();
        _type = type;
        return *this;
      }

      CongruenceCommon(CongruenceCommon const&);
      CongruenceCommon(CongruenceCommon&&);
      CongruenceCommon& operator=(CongruenceCommon const&);
      CongruenceCommon& operator=(CongruenceCommon&&);

      ////////////////////////////////////////////////////////////////////////////
      // CongruenceCommon - validation - protected
      ////////////////////////////////////////////////////////////////////////////

      template <typename Subclass, typename Iterator1, typename Iterator2>
      void throw_if_letter_not_in_alphabet(Iterator1 first,
                                           Iterator2 last) const {
        static_cast<Subclass const*>(this)->throw_if_letter_not_in_alphabet(
            first, last);
      }

     public:
      ~CongruenceCommon();

      ////////////////////////////////////////////////////////////////////////////
      // CongruenceCommon - public member functions
      ////////////////////////////////////////////////////////////////////////////

      // Documented in todd-coxeter-class.hpp etc
      [[nodiscard]] size_t number_of_generating_pairs() const noexcept {
        return _internal_generating_pairs.size() / 2;
      }

      // no doc
      [[nodiscard]] std::vector<word_type> const&
      internal_generating_pairs() const noexcept {
        return _internal_generating_pairs;
      }

      // Documented in todd-coxeter-class.hpp etc
      [[nodiscard]] congruence_kind kind() const noexcept {
        return _type;
      }

     protected:
      // This is protected so that it is not possible to change the kind
      // arbitrarily.
      CongruenceCommon& kind(congruence_kind knd) {
        _type = knd;
        return *this;
      }

      ////////////////////////////////////////////////////////////////////////
      // CongruenceCommon - add_internal_generating_pair
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
      // CongruenceCommon - contains
      ////////////////////////////////////////////////////////////////////////

      // currently_contains_no_checks must be implemented in the derived class.

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
        throw_if_letter_not_in_alphabet<Subclass>(first1, last1);
        throw_if_letter_not_in_alphabet<Subclass>(first2, last2);
        return contains_no_checks<Subclass>(first1, last1, first2, last2);
      }

      ////////////////////////////////////////////////////////////////////////
      // CongruenceCommon - reduce
      ////////////////////////////////////////////////////////////////////////

      // reduce_no_run_no_checks must be implemented in the derived class

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
    };  // class CongruenceCommon
  }     // namespace detail
}  // namespace libsemigroups

#include "cong-common-class.tpp"
#endif  // LIBSEMIGROUPS_DETAIL_CONG_COMMON_CLASS_HPP_
