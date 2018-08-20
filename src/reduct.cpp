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

#include "reduct.hpp"

namespace libsemigroups {

  ReductionOrdering::ReductionOrdering(ReductionOrdering const* ro)
      : _func(ro->_func) {}

  ReductionOrdering::ReductionOrdering(
      std::function<bool(std::string const*, std::string const*)> func)
      : _func(func) {}

  size_t ReductionOrdering::operator()(std::string const* p,
                                       std::string const* q) const {
    return _func(p, q);
  }

  size_t ReductionOrdering::operator()(std::string const& p,
                                       std::string const& q) const {
    return _func(&p, &q);
  }

  SHORTLEX::SHORTLEX()
      : ReductionOrdering([](std::string const* p, std::string const* q) {
          return (p->size() > q->size() || (p->size() == q->size() && *p > *q));
        }) {}

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

}  // namespace libsemigroups
