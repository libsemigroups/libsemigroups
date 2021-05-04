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

#ifndef LIBSEMIGROUPS_COSET_HPP_
#define LIBSEMIGROUPS_COSET_HPP_

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

      //! Type of cosets stored in the table.
      using coset_type = size_t;

      ////////////////////////////////////////////////////////////////////////
      // CosetManager - constructors + destructor - public
      ////////////////////////////////////////////////////////////////////////

      CosetManager();

      CosetManager(CosetManager const&) = default;
      CosetManager(CosetManager&&)      = default;

      CosetManager& operator=(CosetManager const&) = delete;
      CosetManager& operator=(CosetManager&&) = delete;

      ~CosetManager();

      ////////////////////////////////////////////////////////////////////////
      // CosetManager - member functions - public
      ////////////////////////////////////////////////////////////////////////

      //! Returns the capacity of the coset table.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      // std::vector::size is noexcept
      inline size_t coset_capacity() const noexcept {
        return _forwd.size();
      }

      //! Returns the first free coset.
      //!
      //! \returns A value of type CosetManager::coset_type
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      inline coset_type first_free_coset() const noexcept {
        return _first_free_coset;
      }

      //! Check if there are any free cosets.
      //!
      //! \returns A value of type \c bool.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      inline bool has_free_cosets() const noexcept {
        return _first_free_coset != UNDEFINED;
      }

      //! Check if the given coset is active or not.
      //!
      //! \param c the coset to check.
      //!
      //! \returns A value of type \c bool.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! Constant
      // not noexcept since std::vector::operator[] isn't.
      inline bool is_active_coset(coset_type const c) const {
        LIBSEMIGROUPS_ASSERT(c < _ident.size() || c == UNDEFINED);
        return c != UNDEFINED && _ident[c] == c;
      }

      //! Check if the given coset is valid.
      //!
      //! \param c the coset to check.
      //!
      //! \returns A value of type \c bool.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      inline bool is_valid_coset(coset_type const c) const noexcept {
        return c < _forwd.size();
      }

      //! Returns the next active coset after the given coset.
      //!
      //! \param c the coset.
      //!
      //! \returns A value of type CosetManager::coset_type
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! Constant
      // not noexcept since std::vector::operator[] isn't.
      inline coset_type next_active_coset(coset_type const c) const {
        LIBSEMIGROUPS_ASSERT(is_active_coset(c));
        return _forwd[c];
      }

      //! Returns the number of active cosets.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      inline size_t nr_cosets_active() const noexcept {
        return _active;
      }

      //! Returns the total number of cosets defined so far.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      inline size_t nr_cosets_defined() const noexcept {
        return _defined;
      }

      //! Returns the total number of cosets that have been killed so far.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      inline size_t nr_cosets_killed() const noexcept {
        return _cosets_killed;
      }

     protected:
      ////////////////////////////////////////////////////////////////////////
      // CosetManager - member functions - protected
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because free_coset isn't, and std::vector::operator[]
      // isn't.
      //! No doc
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
      //! No doc
      inline coset_type find_coset(coset_type c) const {
        LIBSEMIGROUPS_ASSERT(is_valid_coset(c));
        while (_ident[c] != c) {
          c = _ident[c];
        }
        LIBSEMIGROUPS_ASSERT(is_active_coset(c));
        return c;
      }

      //! No doc
      void add_active_cosets(size_t);
      //! No doc
      void add_free_cosets(size_t);
      //! No doc
      void erase_free_cosets();
      //! No doc
      coset_type new_active_coset();
      //! No doc
      void switch_cosets(coset_type const, coset_type const);

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

#endif  // LIBSEMIGROUPS_COSET_HPP_
