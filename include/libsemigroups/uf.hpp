//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Michael Torpey
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

// This file defines UF, a class used to make an equivalence relation on
// the integers {1 .. n}, using the UNION-FIND METHOD: new pairs can be added
// and the appropriate classes combined quickly.

#ifndef LIBSEMIGROUPS_INCLUDE_UF_HPP_
#define LIBSEMIGROUPS_INCLUDE_UF_HPP_

#include <cstddef>  // for size_t
#include <vector>   // for vector

namespace libsemigroups {
  namespace detail {
    class UF final {
     public:
      using table_type  = std::vector<size_t>;
      using blocks_type = std::vector<table_type*>;

      // Copy constructor
      UF(UF const& copy);
      UF& operator=(UF const& copy) = delete;

      // Constructor by table
      explicit UF(table_type const& table);

      // Constructor by size
      explicit UF(size_t size);

      // Destructor
      ~UF();

      // Getters
      size_t      get_size();
      table_type* get_table();

      void join(UF const& uf);
      void join(UF* uf) {
        join(*uf);
      }

      // get_blocks
      blocks_type* get_blocks();

      // find
      size_t find(size_t i);

      // union
      void unite(size_t i, size_t j);

      // flatten
      void flatten();

      // add_entry
      void add_entry();

      // nr_blocks
      size_t nr_blocks();

      void   reset_next_rep();
      size_t next_rep();

     private:
      size_t       _size;
      table_type*  _table;
      blocks_type* _blocks;
      bool         _haschanged;
      size_t       _next_rep;
    };
  }  // namespace detail
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_UF_HPP_
