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

#include "tce.hpp"

#include "cong-intf.hpp"            // for CongruenceInterface::class_index_type
#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "todd-coxeter.hpp"         // for congruence::ToddCoxeter

namespace libsemigroups {
  using ToddCoxeter = congruence::ToddCoxeter;
  using order       = ToddCoxeter::order;
  using CosetTable  = detail::TCE::CosetTable;

  namespace congruence {
    // Friend function to extract a copy of the final coset table,
    // standardized by short-lex and with redundant generators removed.
    CosetTable* table(ToddCoxeter* tc) {
      tc->run();
      tc->standardize(order::shortlex);
      tc->shrink_to_fit();
      // Ensure class indices and letters are equal!
      auto   table = new CosetTable(tc->_table);
      size_t n     = tc->nr_generators();
      for (letter_type a = 0; a < n;) {
        if (table->get(0, a) != a + 1) {
          table->erase_column(a);
          n--;
        } else {
          ++a;
        }
      }
      return table;
    }
  }  // namespace congruence

  namespace detail {
    static_assert(std::is_same<TCE::class_index_type,
                               CongruenceInterface::class_index_type>::value,
                  "class_index_type is incorrect");

    using class_index_type = TCE::class_index_type;
    TCE::TCE(ToddCoxeter* tc) noexcept : _table(table(tc)), _index(UNDEFINED) {}

    TCE::TCE(TCE const& x, class_index_type i) noexcept
        : _table(x._table), _index(i) {}

    bool TCE::operator==(TCE const& that) const noexcept {
      LIBSEMIGROUPS_ASSERT(_table == that._table);
      return _index == that._index;
    }

    bool TCE::operator<(TCE const& that) const noexcept {
      LIBSEMIGROUPS_ASSERT(_table == that._table);
      return _index < that._index;
    }

    // Only works when "that" is a generator!!
    TCE TCE::operator*(TCE const& that) const {
      LIBSEMIGROUPS_ASSERT(that._index - 1 < _table->nr_cols());
      return TCE(that, _table->get(_index, that._index - 1));
    }

    TCE TCE::one() const noexcept {
      return TCE(*this, 0);
    }

    std::ostringstream& operator<<(std::ostringstream& os, TCE const& x) {
      os << "TCE(" << x._index << ")";
      return os;
    }

  }  // namespace detail
  std::ostream& operator<<(std::ostream&                     os,
                           libsemigroups::detail::TCE const& tc) {
    os << detail::to_string(tc);
    return os;
  }

}  // namespace libsemigroups
