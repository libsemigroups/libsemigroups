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

#ifndef LIBSEMIGROUPS_INCLUDE_REDUCT_HPP_
#define LIBSEMIGROUPS_INCLUDE_REDUCT_HPP_

#include <functional>
#include <string>

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
    explicit ReductionOrdering(ReductionOrdering const*);

    explicit ReductionOrdering(
        std::function<bool(std::string const*, std::string const*)>);

    //! Returns \c true if the word pointed to by \p p is greater than the
    //! word pointed to by \p q in the reduction ordering.
    size_t operator()(std::string const*, std::string const*) const;

    //! Returns \c true if the word \p p is greater than the word
    //! \p q in the reduction ordering.
    size_t operator()(std::string const&, std::string const&) const;

   private:
    std::function<bool(std::string const*, std::string const*)> _func;
  };

  //! This class implements the shortlex reduction ordering derived from
  //! an ordering on libsemigroups::rws_letter_t's.
  class SHORTLEX : public ReductionOrdering {
   public:
    //! Constructs a short-lex reduction ordering object derived from the
    //! order of on libsemigroups::rws_letter_t's given by the operator <.
    SHORTLEX();
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_REDUCT_HPP_
