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

// This file contains the implementation for a class to manage cosets for a
// ToddCoxeter instance.

#include "libsemigroups/coset.hpp"

#include <cstddef>  // for size_t
#include <numeric>  // for iota

#include "libsemigroups/libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/report.hpp"               // for REPORT_DEBUG

////////////////////////////////////////////////////////////////////////////////
//
// We use these two vectors to implement a doubly-linked list of cosets. There
// are two types of coset, those that are "active" and those that are "free".
//
// If c is a coset, then
//   * _forwd[c] is the coset that comes after c in the list,
//     _forwd[the last coset in the list] = UNDEFINED
//
//   * _bckwd[c] is the coset that comes before c in the list,
//     _bckwd[_id_coset] = _id_coset
//
// If c is an active coset, then _ident[c] = c.
//
// If c is a free coset, then _ident[c] != c.
//
// We also store some special locations in the list:
//
//   * _id_coset:  the first coset, this never changes.
//
//   * _current:   is the coset which we are currently using for something in a
//     loop somewhere, the member functions of this class guarantee that
//     _current always points to an active coset, even if the value changes
//     during a function call.
//
//   * _current_la: is similar to _current, and can be used independently of
//     _current.
//
//   * _last_active_coset points to the final active coset in the list.
//
//   * _first_free_coset points to the first free coset in the list, if there
//     are any, or is set to UNDEFINED if there aren't any. Otherwise,
//     _first_free_coset == _forwd[_last_active_coset].
//
////////////////////////////////////////////////////////////////////////////////

namespace libsemigroups {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // Typedefs/aliases
    ////////////////////////////////////////////////////////////////////////

    using coset_type = CosetManager::coset_type;
    constexpr coset_type CosetManager::_id_coset;

    ////////////////////////////////////////////////////////////////////////
    // Helper free function
    ////////////////////////////////////////////////////////////////////////

    static inline coset_type ff(coset_type c, coset_type d, coset_type r) {
      return (r == c ? d : (r == d ? c : r));
      // return (r == c) * d + (r == d) * c + (r != c && r != d) * r;
    }

    ////////////////////////////////////////////////////////////////////////
    // CosetManager - constructors - public
    ////////////////////////////////////////////////////////////////////////

    CosetManager::CosetManager()
        :  // protected
          _current(0),
          _current_la(0),
          // private
          _active(1),
          _bckwd(1, 0),
          _cosets_killed(0),
          _defined(1),
          _first_free_coset(UNDEFINED),
          _forwd(1, static_cast<size_t>(UNDEFINED)),
          _ident(1, 0),
          _last_active_coset(0) {}

    ////////////////////////////////////////////////////////////////////////
    // CosetManager - member functions - protected
    ////////////////////////////////////////////////////////////////////////

    void CosetManager::add_active_cosets(size_t n) {
      if (n > (coset_capacity() - nr_cosets_active())) {
        size_t const m = n - (coset_capacity() - nr_cosets_active());
        add_free_cosets(m);
        // add_free_cosets adds new free cosets to the start of the free list
        _last_active_coset = _forwd.size() - 1;
        _first_free_coset  = _forwd[_last_active_coset];
        std::iota(_ident.begin() + (_ident.size() - m),
                  _ident.end(),
                  _ident.size() - m);
        _active += m;
        _defined += m;
        n -= m;
      }
      _active += n;
      _defined += n;
      for (; n > 0; --n) {
        _bckwd[_first_free_coset]  = _last_active_coset;
        _last_active_coset         = _first_free_coset;
        _first_free_coset          = _forwd[_last_active_coset];
        _ident[_last_active_coset] = _last_active_coset;
      }
    }

    void CosetManager::add_free_cosets(size_t n) {
      // We add n new free cosets at the end of the current list, and link them
      // in as follows:
      //
      // 0 <-> ... <-> _last_active_coset <-> old_capacity <-> new free coset 1
      //   <-> ... <-> new free coset n   <-> old_first_free_coset
      //   <-> remaining old free cosets
      size_t const     old_capacity         = _forwd.size();
      coset_type const old_first_free_coset = _first_free_coset;

      _forwd.resize(_forwd.size() + n, UNDEFINED);
      std::iota(
          _forwd.begin() + old_capacity, _forwd.end() - 1, old_capacity + 1);

      _bckwd.resize(_bckwd.size() + n, 0);
      std::iota(_bckwd.begin() + old_capacity + 1, _bckwd.end(), old_capacity);

      _ident.resize(_ident.size() + n, 0);

      _first_free_coset          = old_capacity;
      _forwd[_last_active_coset] = _first_free_coset;
      _bckwd[_first_free_coset]  = _last_active_coset;

      if (old_first_free_coset != UNDEFINED) {
        _forwd[_forwd.size() - 1]    = old_first_free_coset;
        _bckwd[old_first_free_coset] = _forwd.size() - 1;
      }
#ifdef LIBSEMIGROUPS_DEBUG
      debug_validate_forwd_bckwd();
#endif
    }

    void CosetManager::erase_free_cosets() {
#ifdef LIBSEMIGROUPS_DEBUG
      size_t sum = 0;
      for (coset_type c = _id_coset; c != _first_free_coset; c = _forwd[c]) {
        LIBSEMIGROUPS_ASSERT(c < nr_cosets_active());
        sum += c;
      }
      LIBSEMIGROUPS_ASSERT(
          sum == nr_cosets_active() * (nr_cosets_active() - 1) / 2);
      std::vector<coset_type> copy(_forwd.cbegin(),
                                   _forwd.cbegin() + nr_cosets_active());
      std::sort(copy.begin(), copy.end());
      auto it = std::unique(copy.begin(), copy.end());
      LIBSEMIGROUPS_ASSERT(it == copy.end());
#endif
      _first_free_coset = UNDEFINED;
      _forwd.erase(_forwd.begin() + nr_cosets_active(), _forwd.end());
      _forwd[_last_active_coset] = UNDEFINED;
      _forwd.shrink_to_fit();
      _bckwd.erase(_bckwd.begin() + nr_cosets_active(), _bckwd.end());
      _bckwd.shrink_to_fit();
      _ident.erase(_ident.begin() + nr_cosets_active(), _ident.end());
      _ident.shrink_to_fit();
#ifdef LIBSEMIGROUPS_DEBUG
      debug_validate_forwd_bckwd();
#endif
    }

    coset_type CosetManager::new_active_coset() {
      if (_first_free_coset == UNDEFINED) {
        // There are no free cosets to recycle: make new ones.
        // It seems to be marginally faster to make lots like this, than to
        // just make 1, in some examples, notably ToddCoxeter 040 (Walker 3).
        add_free_cosets(2 * coset_capacity());
      }
      add_active_cosets(1);
      return _last_active_coset;
    }

    void CosetManager::switch_cosets(coset_type const c, coset_type const d) {
      LIBSEMIGROUPS_ASSERT(is_active_coset(c) || is_active_coset(d));
      coset_type fc = _forwd[c], fd = _forwd[d], bc = _bckwd[c], bd = _bckwd[d];

      if (fc != d) {
        _forwd[d]  = fc;
        _bckwd[c]  = bd;
        _forwd[bd] = c;
        if (fc != UNDEFINED) {
          _bckwd[fc] = d;
        }
      } else {
        _forwd[d] = c;
        _bckwd[c] = d;
      }

      if (fd != c) {
        _forwd[c]  = fd;
        _bckwd[d]  = bc;
        _forwd[bc] = d;
        if (fd != UNDEFINED) {
          _bckwd[fd] = c;
        }
      } else {
        _forwd[c] = d;
        _bckwd[d] = c;
      }

      if (!is_active_coset(c)) {
        _ident[d] = 0;
        _ident[c] = c;
      } else if (!is_active_coset(d)) {
        _ident[c] = 0;
        _ident[d] = d;
      }

      _current           = ff(c, d, _current);
      _last_active_coset = ff(c, d, _last_active_coset);
      _first_free_coset  = ff(c, d, _first_free_coset);
      // This is never called from lookahead so we don't have to update
      // _current_la, also it might be that we are calling this after
      // everything is finished and so _current may not be active.

      LIBSEMIGROUPS_ASSERT(is_active_coset(_last_active_coset));
      LIBSEMIGROUPS_ASSERT(!is_active_coset(_first_free_coset));
    }

    ////////////////////////////////////////////////////////////////////////
    // CosetManager - member functions - private
    ////////////////////////////////////////////////////////////////////////

    void CosetManager::free_coset(coset_type const c) {
      LIBSEMIGROUPS_ASSERT(is_active_coset(c));
      // If any "controls" point to <c>, move them back one in the list
      LIBSEMIGROUPS_ASSERT(_current < _bckwd.size()
                           || _current == _first_free_coset);
      _current = (c == _current ? _bckwd[_current] : _current);
      LIBSEMIGROUPS_ASSERT(_current_la < _bckwd.size()
                           || _current_la == _first_free_coset);
      _current_la = (c == _current_la ? _bckwd[_current_la] : _current_la);

      if (c == _last_active_coset) {
        // Simply move the start of the free list back by 1
        LIBSEMIGROUPS_ASSERT(_last_active_coset < _bckwd.size());
        _last_active_coset = _bckwd[_last_active_coset];
      } else {
        LIBSEMIGROUPS_ASSERT(_forwd[c] != UNDEFINED);
        // Remove <c> from the active list
        _bckwd[_forwd[c]] = _bckwd[c];
        _forwd[_bckwd[c]] = _forwd[c];
        // Add <c> to the start of the free list
        _forwd[c] = _first_free_coset;
        LIBSEMIGROUPS_ASSERT(_last_active_coset < _forwd.size());
        if (_first_free_coset != UNDEFINED) {
          _bckwd[_first_free_coset] = c;
        }
        _forwd[_last_active_coset] = c;
      }
      _bckwd[c]         = _last_active_coset;
      _first_free_coset = c;
      _ident[c]         = _id_coset;
    }

#ifdef LIBSEMIGROUPS_DEBUG
    ////////////////////////////////////////////////////////////////////////
    // CosetManager - member functions (debug only) - protected
    ////////////////////////////////////////////////////////////////////////

    void CosetManager::debug_validate_forwd_bckwd() const {
      REPORT_DEBUG_DEFAULT("validating the doubly linked list of cosets...\n");
      LIBSEMIGROUPS_ASSERT(_forwd.size() == _bckwd.size());
      LIBSEMIGROUPS_ASSERT(_bckwd.size() == _ident.size());
      coset_type nr_cosets = 0;
      coset_type e         = _id_coset;
      while (e != _first_free_coset) {
        LIBSEMIGROUPS_ASSERT(e == _id_coset || _forwd[_bckwd[e]] == e);
        LIBSEMIGROUPS_ASSERT(_forwd[e] == _first_free_coset
                             || _bckwd[_forwd[e]] == e);
        nr_cosets++;
        LIBSEMIGROUPS_ASSERT(is_active_coset(_forwd[e])
                             || _forwd[e] == _first_free_coset);
        e = _forwd[e];
      }
      while (e != UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(!is_active_coset(e));
        LIBSEMIGROUPS_ASSERT(_forwd[_bckwd[e]] == e);
        LIBSEMIGROUPS_ASSERT(_forwd[e] == UNDEFINED || _bckwd[_forwd[e]] == e);
        nr_cosets++;
        e = _forwd[e];
      }
      LIBSEMIGROUPS_ASSERT(nr_cosets == _forwd.size());
      LIBSEMIGROUPS_ASSERT(nr_cosets == _bckwd.size());
      LIBSEMIGROUPS_ASSERT(nr_cosets == _ident.size());
      REPORT_DEBUG_DEFAULT("...doubly linked list of cosets ok\n");
    }
#endif
  }  // namespace detail
}  // namespace libsemigroups
