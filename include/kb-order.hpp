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

#ifndef LIBSEMIGROUPS_SRC_RWS_ORDER_H_
#define LIBSEMIGROUPS_SRC_RWS_ORDER_H_

#include <functional>
#include <string>

// TODO
// - cpp file

namespace libsemigroups {
  //! This class provides a call operator which can be used to compare
  //! libsemigroups::rws_word_t.
  //!
  //! A *reduction ordering* is a linear order \f$\prec\f$ with no infinite
  //! descending chains and where \f$u \prec v\f$ for some
  //! libsemigroups::rws_word_t \f$u\f$ and \f$v\f$ implies that
  //! \f$ aub \prec avb\f$ for all  libsemigroups::rws_word_t \f$a\f$ and
  //! \f$b\f$.
  class ReductionOrdering {
   public:
    //! A constructor.
    //!
    //! This constructs a reduction ordering object whose call operator uses
    //! the function \p func to compare libsemigroups::rws_word_t's. It is the
    //! responsibility of the caller to verify that \p func specifies a
    //! reduction ordering.
    explicit ReductionOrdering(ReductionOrdering const* ro)
        : _func(ro->_func) {}

    explicit ReductionOrdering(
        std::function<bool(std::string const*, std::string const*)> func)
        : _func(func) {}

    //! Returns \c true if the word pointed to by \p p is greater than the
    //! word pointed to by \p q in the reduction ordering.
    size_t operator()(std::string const* p, std::string const* q) const {
      return _func(p, q);
    }

    //! Returns \c true if the word \p p is greater than the word
    //! \p q in the reduction ordering.
    size_t operator()(std::string const& p, std::string const& q) const {
      return _func(&p, &q);
    }

   private:
    std::function<bool(std::string const*, std::string const*)> _func;
  };

  //! This class implements the shortlex reduction ordering derived from
  //! an ordering on libsemigroups::rws_letter_t's.
  class SHORTLEX : public ReductionOrdering {
   public:
    //! Constructs a short-lex reduction ordering object derived from the
    //! order of on libsemigroups::rws_letter_t's given by the operator <.
    SHORTLEX()
        : ReductionOrdering([](std::string const* p, std::string const* q) {
            return (p->size() > q->size()
                    || (p->size() == q->size() && *p > *q));
          }) {}
  };

  // The ordering used here is recursive path ordering (based on
  // that described in the book "Confluent String Rewriting" by Matthias
  // Jantzen, Defn 1.2.14, page 24).
  //
  // The ordering is as follows:
  // let u, v be elements of X* u >= v iff one of the following conditions is
  // fulfilled;
  // 1) u = v
  // OR
  // u = u'a, v = v'b for some a,b elements of X, u',v' elements of X* and
  // then:
  // 2) a = b and u' >= v'
  // OR
  // 3) a > b and u  > v'
  // OR
  // 4) b > a and u'> v
  //
  // 1 or 0 = false
  // 2      = true
  /*class RECURSIVE : public ReductionOrdering {
   public:
    RECURSIVE()
        : ReductionOrdering([](std::string const* Q, std::string const* P) {
            bool lastmoved = false;
            auto it_P      = P->crbegin();
            auto it_Q      = Q->crbegin();
            while (true) {
              if (it_P == P->crend()) {
                return (it_Q == Q->crend() ? lastmoved : true);
              } else if (it_Q == Q->crend()) {
                return false;
              }
              if (*it_P == *it_Q) {
                ++it_P;
                ++it_Q;
              } else if (*it_P < *it_Q) {
                ++it_P;
                lastmoved = false;
              } else {
                ++it_Q;
                lastmoved = true;
              }
            }
          }) {}
  };*/

  // TODO add more reduction orderings

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_RWS_ORDER_H_
