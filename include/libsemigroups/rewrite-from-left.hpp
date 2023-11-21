//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 Joe Edwards + James D. Mitchell
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

#ifndef LIBSEMIGROUPS_REWRITE_FROM_LEFT_HPP_
#define LIBSEMIGROUPS_REWRITE_FROM_LEFT_HPP_

#include <set>  // for set

#include "rewriter-base.hpp"  //for Rewriter

namespace libsemigroups {
  class RewriteFromLeft : public Rewriter {
    std::set<RuleLookup> _set_rules;

   public:
    using Rewriter::confluent;
    using Rules::stats;

    RewriteFromLeft() = default;

    // Rules(Rules const& that);
    // Rules(Rules&& that);
    RewriteFromLeft& operator=(RewriteFromLeft const&);

    // TODO the other constructors

    ~RewriteFromLeft() = default;  // TODO out-of-line this

    RewriteFromLeft& init();

    void rewrite(internal_string_type& u) const;

    [[nodiscard]] bool confluent() const;

    // TODO private?
    void add_rule(Rule* rule);
    void reduce();

    using Rewriter::add_rule;
    // template <typename StringLike>
    // void add_rule(StringLike const& lhs, StringLike const& rhs) {
    //   if (Rewriter::add_rule(lhs, rhs)) {
    //     clear_stack();
    //   }
    // }

   private:
    void rewrite(Rule* rule) const;
    // void     clear_stack();
    iterator erase_from_active_rules(iterator);
  };
}  // namespace libsemigroups
#endif