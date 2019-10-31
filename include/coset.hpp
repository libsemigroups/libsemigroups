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

// This file contains the declaration for a class to manage cosets for a
// ToddCoxeter instance.

#ifndef LIBSEMIGROUPS_INCLUDE_COSET_HPP_
#define LIBSEMIGROUPS_INCLUDE_COSET_HPP_

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "constants.hpp"            // for UNDEFINED
#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT/DEBUG

namespace libsemigroups {
  namespace detail {
    class CosetManager {
     public:
      ////////////////////////////////////////////////////////////////////////
      // CosetManager - typedefs - public
      ////////////////////////////////////////////////////////////////////////

      using coset_type = size_t;

      ////////////////////////////////////////////////////////////////////////
      // CosetManager - constructors + destructor - public
      ////////////////////////////////////////////////////////////////////////

      CosetManager();

      CosetManager(CosetManager const&) = default;
      CosetManager(CosetManager&&)      = default;

      CosetManager& operator=(CosetManager const&) = delete;
      CosetManager& operator=(CosetManager&&) = delete;

      ~CosetManager() = default;

      ////////////////////////////////////////////////////////////////////////
      // CosetManager - member functions - public
      ////////////////////////////////////////////////////////////////////////

      // std::vector::size is noexcept
      inline size_t coset_capacity() const noexcept {
        return _forwd.size();
      }

      inline coset_type first_free_coset() const noexcept {
        return _first_free_coset;
      }

      inline bool has_free_cosets() const noexcept {
        return _first_free_coset != UNDEFINED;
      }

      // not noexcept since std::vector::operator[] isn't.
      inline bool is_active_coset(coset_type const c) const {
        LIBSEMIGROUPS_ASSERT(c < _ident.size() || c == UNDEFINED);
        return c != UNDEFINED && _ident[c] == c;
      }

      inline bool is_valid_coset(coset_type const c) const noexcept {
        return c < _forwd.size();
      }

      // not noexcept since std::vector::operator[] isn't.
      inline coset_type next_active_coset(coset_type const c) const {
        LIBSEMIGROUPS_ASSERT(is_active_coset(c));
        return _forwd[c];
      }

      inline size_t nr_cosets_active() const noexcept {
        return _active;
      }

      inline size_t nr_cosets_defined() const noexcept {
        return _defined;
      }

      inline size_t nr_cosets_killed() const noexcept {
        return _cosets_killed;
      }

     protected:
      ////////////////////////////////////////////////////////////////////////
      // CosetManager - member functions - protected
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because free_coset isn't, and std::vector::operator[]
      // isn't.
      inline void union_cosets(coset_type const min, coset_type const max) {
        LIBSEMIGROUPS_ASSERT(is_active_coset(min));
        LIBSEMIGROUPS_ASSERT(is_active_coset(max));
        LIBSEMIGROUPS_ASSERT(max > min);
        _active--;
        _cosets_killed++;
        free_coset(max);
        // Leave a "forwarding address" so we know what <max> was identified
        // with
        _ident[max] = min;
      }

      // not noexcept since std::vector::operator[] isn't.
      inline coset_type find_coset(coset_type c) const {
        LIBSEMIGROUPS_ASSERT(is_valid_coset(c));
        while (_ident[c] != c) {
          c = _ident[c];
        }
        LIBSEMIGROUPS_ASSERT(is_active_coset(c));
        return c;
      }

      void       add_active_cosets(size_t);
      void       add_free_cosets(size_t);
      void       erase_free_cosets();
      coset_type new_active_coset();
      void       switch_cosets(coset_type const, coset_type const);

      ////////////////////////////////////////////////////////////////////////
      // CosetManager - data - protected
      ////////////////////////////////////////////////////////////////////////

      static constexpr coset_type _id_coset = 0;
      coset_type                  _current;
      coset_type                  _current_la;

     private:
      ////////////////////////////////////////////////////////////////////////
      // CosetManager - member functions - private
      ////////////////////////////////////////////////////////////////////////

      // not noexcept since std::vector::operator[] isn't.
      void free_coset(coset_type const);

      ////////////////////////////////////////////////////////////////////////
      // CosetManager - data - private
      ////////////////////////////////////////////////////////////////////////

      size_t                  _active;
      std::vector<coset_type> _bckwd;
      size_t                  _cosets_killed;
      size_t                  _defined;
      coset_type              _first_free_coset;
      std::vector<coset_type> _forwd;
      std::vector<coset_type> _ident;
      coset_type              _last_active_coset;

#ifdef LIBSEMIGROUPS_DEBUG

     protected:
      ////////////////////////////////////////////////////////////////////////
      // CosetManager - member functions (debug only) - protected
      ////////////////////////////////////////////////////////////////////////
      void debug_validate_forwd_bckwd() const;
#endif
    };

  }  // namespace detail
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_COSET_HPP_
