//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

// This file contains implementations for the private inner class of Congruence
// called TC, which is a subclass of Congruence::DATA.  This class is for
// performing Todd-Coxeter.

#include "tc.h"

#include <algorithm>

#define TC_KILLED                      \
  if (_killed) {                       \
    if (!_already_reported_killed) {   \
      _already_reported_killed = true; \
      REPORT("killed")                 \
    }                                  \
    _stop_packing = true;              \
    _steps        = 1;                 \
  }

namespace libsemigroups {

  // COSET LISTS:
  //
  // We use these two arrays to simulate a doubly-linked list of active
  // cosets
  // (the "active list") with deleted cosets attached to the end (the "free
  // list").  If c is an active coset:
  //   _forwd[c] is the coset that comes after c in the list.
  //   _bckwd[c] is the coset that comes before c in the list.
  // If c is a free coset (has been deleted) the backward reference is not
  // needed, and so instead, _bckwd[c] is set to the coset c was identified
  // with.  To indicate this alternative use of the list, the entry is
  // negated
  // (_backwd[c] == -3 indicates that c was identified with coset 3).

  // We also store some special locations in the list:
  //   _current is the coset to which we are currently applying relations.
  //   _current_no_add is used instead of _current if we are in a packing
  //   phase.
  //   _last points to the final active coset in the list.
  //   _next points to the first free coset in the list.
  // Hence usually _next == _last + 1.

  // COSET TABLES:
  //
  // We use these three tables to store all a coset's images and preimages.
  //   _table[c][i] is coset c's image under generator i.
  //   _preim_init[c][i] is ONE of coset c's preimages under generator i.
  //   _preim_next[c][i] is a coset that has THE SAME IMAGE as coset c
  //   (under
  //   i)
  //
  // Hence to find all the preimages of c under i:
  //   - Let u = _preim_init[c][i] ONCE.
  //   - Let u = _preim_next[u][i] REPEATEDLY until it becomes UNDEFINED.
  // Each u is one preimage.
  //
  // To add v, a new preimage of c under i:
  //   - Set _preim_next[v][i] to point to the current _preim_init[c][i].
  //   - Then change _preim_init[c][i] to point to v.
  // Now the new preimage and all the old preimages are stored.

  Congruence::TC::TC(Congruence& cong)
      : DATA(cong, 1000, 2000000),
        _active(1),
        _already_reported_killed(false),
        _bckwd(1, 0),
        _cosets_killed(0),
        _current(0),
        _current_no_add(UNDEFINED),
        _defined(1),
        _extra(),
        _forwd(1, UNDEFINED),
        _id_coset(0),
        _init_done(false),
        _last(0),
        _next(UNDEFINED),
        _pack(120000),
        _prefilled(false),
        _preim_init(cong._nrgens, 1, UNDEFINED),
        _preim_next(cong._nrgens, 1, UNDEFINED),
        _stop_packing(false),
        _table(cong._nrgens, 1, UNDEFINED),
        _tc_done(false) {}

  void Congruence::TC::init() {
    if (!_init_done) {
      // This is the first run
      init_tc_relations();
      // Apply each "extra" relation to the first coset only
      for (relation_t const& rel : _extra) {
        trace(_id_coset, rel);  // Allow new cosets
      }
    }
    _init_done = true;
  }

  void Congruence::TC::prefill() {
    Semigroup<>* semigroup = _cong._semigroup;
    if (semigroup == nullptr) {
      return;
    }
    _table.add_rows(semigroup->size());
    for (size_t i = 0; i < _cong._nrgens; i++) {
      _table.set(0, i, semigroup->letter_to_pos(i) + 1);
    }
    TC_KILLED
    if (_cong._type == LEFT) {
      for (size_t row = 0; row < semigroup->size(); ++row) {
        for (size_t col = 0; col < _cong._nrgens; ++col) {
          _table.set(row + 1, col, semigroup->left(row, col) + 1);
        }
      }
    } else {
      for (size_t row = 0; row < semigroup->size(); ++row) {
        for (size_t col = 0; col < _cong._nrgens; ++col) {
          _table.set(row + 1, col, semigroup->right(row, col) + 1);
        }
      }
    }
    TC_KILLED
    init_after_prefill();
  }

  void Congruence::TC::prefill(RecVec<class_index_t>& table) {
    // TODO(JDM) check table is valid
    LIBSEMIGROUPS_ASSERT(table.nr_cols() == _cong._nrgens);
    LIBSEMIGROUPS_ASSERT(table.nr_rows() > 0);

    _table = table;
    init_after_prefill();
  }

  void Congruence::TC::init_after_prefill() {
    _prefilled = true;
    _active    = _table.nr_rows();
    _id_coset  = 0;

    _forwd.reserve(_active);
    _bckwd.reserve(_active);

    for (size_t i = 1; i < _active; i++) {
      _forwd.push_back(i + 1);
      _bckwd.push_back(i - 1);
    }

    TC_KILLED

    _forwd[0]           = 1;
    _forwd[_active - 1] = UNDEFINED;

    _last = _active - 1;

    _preim_init.add_rows(_table.nr_rows());
    _preim_next.add_rows(_table.nr_rows());

    for (class_index_t c = 0; c < _active; c++) {
      for (letter_t i = 0; i < _cong._nrgens; i++) {
        class_index_t b = _table.get(c, i);
        _preim_next.set(c, i, _preim_init.get(b, i));
        _preim_init.set(b, i, c);
      }
      // TC_KILLED?
    }
    _defined = _active;
  }

  void Congruence::TC::init_tc_relations() {
    // This should not have been run before
    LIBSEMIGROUPS_ASSERT(!_init_done);

    // Handle _extra first!
    switch (_cong._type) {
      case LEFT:
        _extra.insert(_extra.end(), _cong._extra.begin(), _cong._extra.end());
        for (relation_t& rel : _extra) {
          std::reverse(rel.first.begin(), rel.first.end());
          std::reverse(rel.second.begin(), rel.second.end());
        }
        break;
      case RIGHT:               // do nothing
        _extra = _cong._extra;  // FIXME avoid copying here!
        break;
      case TWOSIDED:
        _relations.insert(
            _relations.end(), _cong._extra.begin(), _cong._extra.end());
        break;
      default:
        LIBSEMIGROUPS_ASSERT(false);
    }

    if (_prefilled) {
      // The information in the Congruence relations is already present in the
      // table since we prefilled it.
      return;
    }

    // Initialise the relations in the enclosing Congruence object. We do not
    // call relations() here so that we can pass _killed.

    _cong.init_relations(_cong._semigroup, _killed);

    // Must insert at _relations.end() since it might be non-empty
    _relations.insert(
        _relations.end(), _cong._relations.begin(), _cong._relations.end());
    // FIXME avoid copying in the RIGHT case
    switch (_cong._type) {
      case RIGHT:
      // intentional fall through
      case TWOSIDED:
        break;
      case LEFT:
        for (relation_t& rel : _relations) {
          std::reverse(rel.first.begin(), rel.first.end());
          std::reverse(rel.second.begin(), rel.second.end());
        }
        break;
      default:
        LIBSEMIGROUPS_ASSERT(false);
    }
  }

  // compress the table
  void Congruence::TC::compress() {
    LIBSEMIGROUPS_ASSERT(is_done());
    if (_active == _table.nr_rows()) {
      return;
    }

    RecVec<class_index_t> table(_cong._nrgens, _active);

    class_index_t pos = _id_coset;
    // old number to new numbers lookup
    std::unordered_map<class_index_t, class_index_t> lookup;
    size_t next_index = 0;

    while (pos != _next) {
      size_t curr_index;
      auto   it = lookup.find(pos);
      if (it == lookup.end()) {
        lookup.insert(std::make_pair(pos, next_index));
        curr_index = next_index;
        next_index++;
      } else {
        curr_index = it->second;
      }

      // copy row
      for (size_t i = 0; i < _cong._nrgens; i++) {
        class_index_t val = _table.get(pos, i);
        it                = lookup.find(val);
        if (it == lookup.end()) {
          lookup.insert(std::make_pair(val, next_index));
          val = next_index;
          next_index++;
        } else {
          val = it->second;
        }
        table.set(curr_index, i, val);
      }
      pos = _forwd[pos];
    }

    _table = table;
  }

  Congruence::class_index_t
  Congruence::TC::word_to_class_index(word_t const& w) {
    class_index_t c = _id_coset;
    if (_cong._type == LEFT) {
      // Iterate in reverse order
      for (auto rit = w.crbegin(); rit != w.crend() && c != UNDEFINED; ++rit) {
        c = _table.get(c, *rit);
      }
    } else {
      // Iterate in sequential order
      for (auto it = w.cbegin(); it != w.cend() && c != UNDEFINED; ++it) {
        c = _table.get(c, *it);
      }
    }
    // c in {1 .. n} (where 0 is the id coset)
    LIBSEMIGROUPS_ASSERT(c < _active || c == UNDEFINED);
    // Convert to {0 .. n-1}
    return (c == UNDEFINED ? c : c - 1);
  }

  Congruence::DATA::result_t Congruence::TC::current_equals(word_t const& w1,
                                                            word_t const& w2) {
    if (!is_done() && is_killed()) {
      // This cannot be reliably tested since it relies on a race condition:
      // if this has been killed since the start of the function, then we return
      // immediately to run_until with an inconclusive answer.  run_until will
      // then quit, and allow the winning DATA to answer the equality test.
      return result_t::UNKNOWN;
    }

    init();

    class_index_t c1 = word_to_class_index(w1);
    class_index_t c2 = word_to_class_index(w2);

    if (c1 == UNDEFINED || c2 == UNDEFINED) {
      return result_t::UNKNOWN;
    }

    // c in {1 .. n} (where 0 is the id coset)
    LIBSEMIGROUPS_ASSERT(c1 < _active);
    LIBSEMIGROUPS_ASSERT(c2 < _active);
    if (c1 == c2) {
      return result_t::TRUE;
    } else if (is_done()) {
      return result_t::FALSE;
    } else {
      return result_t::UNKNOWN;
    }
  }

  // Create a new active coset for coset c to map to under generator a
  void Congruence::TC::new_coset(class_index_t const& c, letter_t const& a) {
    TC_KILLED

    _active++;
    _defined++;
    _report_next++;

    if (_next == UNDEFINED) {
      // There are no free cosets to recycle: make a new one
      _next         = _active - 1;
      _forwd[_last] = _next;
      _forwd.push_back(UNDEFINED);
      _bckwd.push_back(_last);
      _table.add_rows(1);
      _preim_init.add_rows(1);
      _preim_next.add_rows(1);
    } else {
      _bckwd[_next] = _last;
    }

    // Mark one more coset as active
    _last = _next;
    _next = _forwd[_last];

    // Clear the new coset's row in each table
    for (letter_t i = 0; i < _cong._nrgens; i++) {
      _table.set(_last, i, UNDEFINED);
      _preim_init.set(_last, i, UNDEFINED);
    }

    // Set the new coset as the image of c under a
    _table.set(c, a, _last);

    // Set c as the one preimage of the new coset
    _preim_init.set(_last, a, c);
    _preim_next.set(c, a, UNDEFINED);
  }

  // Identify lhs with rhs, and process any further coincidences
  void Congruence::TC::identify_cosets(class_index_t lhs, class_index_t rhs) {
    TC_KILLED

    // Note that _lhs_stack and _rhs_stack may not be empty, if this was killed
    // before and has been restarted.

    // Make sure lhs < rhs
    if (lhs == rhs) {
      return;
    } else if (rhs < lhs) {
      class_index_t tmp = lhs;
      lhs               = rhs;
      rhs               = tmp;
    }

    while (!_killed) {
      // If <lhs> is not active, use the coset it was identified with
      while (_bckwd[lhs] < 0) {
        lhs = -_bckwd[lhs];
      }
      // Same with <rhs>
      while (_bckwd[rhs] < 0) {
        rhs = -_bckwd[rhs];
      }

      if (lhs != rhs) {
        _active--;
        // If any "controls" point to <rhs>, move them back one in the list
        if (rhs == _current) {
          _current = _bckwd[_current];
        }
        if (rhs == _current_no_add) {
          _current_no_add = _bckwd[_current_no_add];
        }

        LIBSEMIGROUPS_ASSERT(rhs != _next);
        if (rhs == _last) {
          // Simply move the start of the free list back by 1
          _last = _bckwd[_last];
        } else {
          // Remove <rhs> from the active list
          _bckwd[_forwd[rhs]] = _bckwd[rhs];
          _forwd[_bckwd[rhs]] = _forwd[rhs];
          // Add <rhs> to the start of the free list
          _forwd[rhs]   = _next;
          _forwd[_last] = rhs;
        }
        _next = rhs;

        // Leave a "forwarding address" so we know what <rhs> was identified
        // with
        _bckwd[rhs] = -static_cast<signed_class_index_t>(lhs);

        for (letter_t i = 0; i < _cong._nrgens; i++) {
          // Let <v> be the first PREIMAGE of <rhs>
          class_index_t v = _preim_init.get(rhs, i);
          while (v != UNDEFINED) {
            _table.set(v, i, lhs);  // Replace <rhs> by <lhs> in the table
            class_index_t u
                = _preim_next.get(v, i);  // Get <rhs>'s next preimage
            _preim_next.set(v, i, _preim_init.get(lhs, i));
            _preim_init.set(lhs, i, v);
            // v is now a preimage of <lhs>, not <rhs>
            v = u;  // Let <v> be <rhs>'s next preimage, and repeat
          }

          // Now let <v> be the IMAGE of <rhs>
          v = _table.get(rhs, i);
          if (v != UNDEFINED) {
            class_index_t u = _preim_init.get(v, i);
            LIBSEMIGROUPS_ASSERT(u != UNDEFINED);
            if (u == rhs) {
              // Remove <rhs> from the start of the list of <v>'s preimages
              _preim_init.set(v, i, _preim_next.get(rhs, i));
            } else {
              // Go through all <v>'s preimages until we find <rhs>
              while (_preim_next.get(u, i) != rhs) {
                u = _preim_next.get(u, i);
              }
              // Remove <rhs> from the list of <v>'s preimages
              _preim_next.set(u, i, _preim_next.get(rhs, i));
            }

            // Let <u> be the image of <lhs>, and ensure <u> = <v>
            u = _table.get(lhs, i);
            if (u == UNDEFINED) {
              _table.set(lhs, i, v);
              _preim_next.set(lhs, i, _preim_init.get(v, i));
              _preim_init.set(v, i, lhs);
            } else {
              // Add (u,v) to the stack of pairs to be identified
              _lhs_stack.push(std::min(u, v));
              _rhs_stack.push(std::max(u, v));
            }
          }
        }
      }
      if (_lhs_stack.empty()) {
        LIBSEMIGROUPS_ASSERT(_rhs_stack.empty());
        break;
      }
      // Get the next pair to be identified
      lhs = _lhs_stack.top();
      _lhs_stack.pop();
      rhs = _rhs_stack.top();
      _rhs_stack.pop();
    }

    LIBSEMIGROUPS_ASSERT((_lhs_stack.empty() && _rhs_stack.empty()) || _killed);
  }

  // Take the two words of the relation <rel>, apply them both to the coset
  // <c>, and identify the two results.  If <add> is true (the default) then
  // new cosets will be created whenever necessary; if false, then we are
  // "packing", and this function will not create any new cosets.
  void Congruence::TC::trace(class_index_t const& c,
                             relation_t const&    rel,
                             bool                 add) {
    class_index_t lhs = c;
    for (auto it = rel.first.cbegin(); it < rel.first.cend() - 1; it++) {
      if (_table.get(lhs, *it) != UNDEFINED) {
        lhs = _table.get(lhs, *it);
      } else if (add) {
        new_coset(lhs, *it);
        lhs = _last;
      } else {
        return;
      }
    }
    // <lhs> is the image of <c> under <rel>[1] (minus the last letter)

    class_index_t rhs = c;
    for (auto it = rel.second.cbegin(); it < rel.second.cend() - 1; it++) {
      if (_table.get(rhs, *it) != UNDEFINED) {
        rhs = _table.get(rhs, *it);
      } else if (add) {
        new_coset(rhs, *it);
        rhs = _last;
      } else {
        return;
      }
    }
    // <rhs> is the image of <c> under <rel>[2] (minus the last letter)

    // Statistics and packing
    _report_next++;
    if (_report_next > _report_interval) {
      REPORT(_defined << " defined, " << _forwd.size() << " max, " << _active
                      << " active, "
                      << (_defined - _active) - _cosets_killed
                      << " killed, "
                      << "current "
                      << (add ? _current : _current_no_add))
      // If we are killing cosets too slowly, then stop packing
      if ((_defined - _active) - _cosets_killed < 100) {
        _stop_packing = true;
      }
      _report_next   = 0;
      _cosets_killed = _defined - _active;
    }

    letter_t      a = rel.first.back();
    letter_t      b = rel.second.back();
    class_index_t u = _table.get(lhs, a);
    class_index_t v = _table.get(rhs, b);
    // u = lhs^a = c^rel[1]
    // v = rhs^b = c^rel[2]

    // We must now ensure lhs^a == rhs^b.
    if (u == UNDEFINED && v == UNDEFINED) {
      if (add) {
        // Create a new coset and set both lhs^a and rhs^b to it
        new_coset(lhs, a);
        _table.set(rhs, b, _last);
        if (a == b) {
          _preim_next.set(lhs, a, rhs);
          _preim_next.set(rhs, a, UNDEFINED);
        } else {
          _preim_init.set(_last, b, rhs);
          _preim_next.set(rhs, b, UNDEFINED);
        }
      } else {
        return;  // Packing phase: do nothing
      }
    } else if (u == UNDEFINED && v != UNDEFINED) {
      // Set lhs^a to v
      _table.set(lhs, a, v);
      _preim_next.set(lhs, a, _preim_init.get(v, a));
      _preim_init.set(v, a, lhs);
    } else if (u != UNDEFINED && v == UNDEFINED) {
      // Set rhs^b to u
      _table.set(rhs, b, u);
      _preim_next.set(rhs, b, _preim_init.get(u, b));
      _preim_init.set(u, b, rhs);
    } else {
      // lhs^a and rhs^b are both defined
      identify_cosets(u, v);
    }
  }

  // Apply the Todd-Coxeter algorithm until the coset table is complete.
  void Congruence::TC::run() {
    while (!is_done() && !is_killed()) {
      run(Congruence::LIMIT_MAX);
      TC_KILLED
    }
  }

  // Apply the Todd-Coxeter algorithm for the specified number of iterations
  void Congruence::TC::run(size_t steps) {
    _steps = steps;

    init();

    if (_tc_done) {
      return;
    }

    // Run a batch
    REPORT("number of steps: " << _steps);
    do {
      // Apply each relation to the "_current" coset
      for (relation_t const& rel : _relations) {
        trace(_current, rel);  // Allow new cosets
      }

      // If the number of active cosets is too high, start a packing phase
      if (_active > _pack) {
        REPORT(_defined << " defined, " << _forwd.size() << " max, " << _active
                        << " active, "
                        << (_defined - _active) - _cosets_killed
                        << " killed, "
                        << "current "
                        << _current);
        REPORT("Entering lookahead phase . . .");
        _cosets_killed = _defined - _active;

        size_t oldactive = _active;       // Keep this for stats
        _current_no_add  = _current + 1;  // Start packing from _current

        do {
          // Apply every relation to the "_current_no_add" coset
          for (relation_t const& rel : _relations) {
            trace(_current_no_add, rel, false);  // Don't allow new cosets
          }
          _current_no_add = _forwd[_current_no_add];

          // Quit loop if we reach an inactive coset OR we get a "stop" signal
          TC_KILLED
        } while (_current_no_add != _next && !_stop_packing);

        REPORT("Entering lookahead complete " << oldactive - _active
                                              << " killed");

        _pack += _pack / 10;  // Raise packing threshold 10%
        _stop_packing   = false;
        _current_no_add = UNDEFINED;
      }

      // Move onto the next coset
      _current = _forwd[_current];

      // Quit loop when we reach an inactive coset
      TC_KILLED
    } while (_current != _next && --_steps > 0);

    // Final report
    REPORT("stopping with " << _defined << " cosets defined,"
                            << " maximum "
                            << _forwd.size()
                            << ", "
                            << _active
                            << " survived");
    if (_current == _next) {
      _tc_done = true;
      compress();
      REPORT("finished!");
    }

    // No return value: all info is now stored in the class
  }

}  // namespace libsemigroups
