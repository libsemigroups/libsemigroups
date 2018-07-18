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

// This file contains an implementation of the Todd-Coxeter algorithm for
// semigroups.

// FIXME there doesn't seem to be any reason to store _relations as pairs,
// maybe better to store it as a flat vector, this might reduce code
// duplication too.
//
// TODO:
// - allow addition of identity (i.e. via empty string)
// - allow inverses

#include "todd-coxeter.h"

#include <algorithm>
#include <unordered_map>

#include "libsemigroups-debug.h"
#include "libsemigroups-exception.h"
#include "report.h"
#include "semigroups.h"
#include "tce.h"

namespace libsemigroups {

  namespace congruence {
    ////////////////////////////////////////////////////////////////////////////
    // Private Constructor
    ////////////////////////////////////////////////////////////////////////////

    // Split this ToddCoxeter into two the original one (just used to compute
    // congruences of an fp semigroup), and the other one for finding
    // congruences over SemigroupBase*.

    ToddCoxeter::ToddCoxeter(congruence_t type)
        : Interface(type),
          _active(1),
          _base(nullptr),
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
          _non_trivial_classes(),
          _nrgens(UNDEFINED),
          _pack(120000),
          _policy(none),
          _prefilled(false),
          _quotient_semigroup(nullptr),
          _stop_packing(false),
          _type(type) {}

    ////////////////////////////////////////////////////////////////////////////
    // Public Constructors
    ////////////////////////////////////////////////////////////////////////////

    ToddCoxeter::ToddCoxeter(congruence_t type, SemigroupBase* S, policy p)
        : ToddCoxeter(type) {
      _base   = S;
      _policy = p;
      LIBSEMIGROUPS_ASSERT(validate_relations());
      // TODO assertion should be an exception
    }

    ToddCoxeter::ToddCoxeter(congruence_t                   type,
                             SemigroupBase*                 S,
                             std::vector<relation_t> const& genpairs,
                             policy                         p)
        : ToddCoxeter(type, S, p) {
      _extra = genpairs;
    }

    // ToddCoxeter::ToddCoxeter(ToddCoxeter const&
    // copy)
    //     : ToddCoxeter(copy.type(),
    //                   copy.nr_generators(),
    //                   std::vector<relation_t>(copy._relations),
    //                   std::vector<relation_t>(copy._extra)) {
    //   // FIXME init_relations will be called on this at some point, and if it
    //   // was already called on copy, and we are a left congruence, then this
    //   // will reverse the relations again.
    // }

    // ToddCoxeter::ToddCoxeter(
    //     ToddCoxeter const*   copy,
    //     std::vector<relation_t> const& extra)
    //     : ToddCoxeter(*copy) {
    //   _extra.reserve(_extra.size() + extra.size());
    //   _extra.insert(_extra.end(), extra.begin(), extra.end());
    //   LIBSEMIGROUPS_ASSERT(validate_relations());
    // }

    ToddCoxeter::~ToddCoxeter() {
      delete _quotient_semigroup;
    }

    //////////////////////////////////////////////////////////////////////////
    // Overridden public pure virtual methods from congruence::Interface
    //////////////////////////////////////////////////////////////////////////

    class_index_t ToddCoxeter::word_to_class_index(word_t const& w) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      class_index_t c = const_word_to_class_index(w);
      // c is in the range 1, ..., _active because 0 represents the identity
      // coset, and does not correspond to an element.
      LIBSEMIGROUPS_ASSERT(c < _active || c == UNDEFINED);
      return c;
    }

    size_t ToddCoxeter::nr_classes() {
      if (is_quotient_obviously_infinite()) {
        return INFTY;
      } else {
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        return _active - 1;
      }
    }

    void ToddCoxeter::add_pair(word_t lhs, word_t rhs) {
      LIBSEMIGROUPS_ASSERT(_nrgens != UNDEFINED);
      LIBSEMIGROUPS_ASSERT(validate_word(lhs));
      LIBSEMIGROUPS_ASSERT(validate_word(rhs));
      LIBSEMIGROUPS_ASSERT(!_init_done);
      _extra.push_back(std::make_pair(lhs, rhs));
      reset_quotient_semigroup();
    }

    SemigroupBase* ToddCoxeter::quotient_semigroup() {
      if (type() != TWOSIDED) {
        throw LibsemigroupsException("The congruence must be two-sided");
      }
      if (_quotient_semigroup == nullptr) {
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        Semigroup<TCE>* Q = new Semigroup<TCE>({TCE(this, _table.get(0, 0))});
        for (size_t i = 1; i < _nrgens; ++i) {
          // We use _table.get(0, i) instead of just i, because there might be
          // more generators than cosets.
          Q->add_generator(TCE(this, _table.get(0, i)));
        }
        _quotient_semigroup = Q;
      }
      return _quotient_semigroup;
    }

    std::vector<std::vector<word_t>>::const_iterator
    ToddCoxeter::cbegin_non_trivial_classes() {
      init_non_trivial_classes();
      return _non_trivial_classes.cbegin();
    }

    std::vector<std::vector<word_t>>::const_iterator
    ToddCoxeter::cend_non_trivial_classes() {
      init_non_trivial_classes();
      return _non_trivial_classes.cend();
    }

    size_t ToddCoxeter::nr_non_trivial_classes() {
      init_non_trivial_classes();
      return _non_trivial_classes.size();
    }

    bool ToddCoxeter::is_quotient_obviously_infinite() const {
      // TODO exceptions instead of assertions
      LIBSEMIGROUPS_ASSERT(_nrgens != UNDEFINED);
      if (_policy != none) {
        // _policy != none means we were created from a SemigroupBase*, which
        // means that this is infinite if and only if the SemigroupBase* is
        // infinite too, which is not obvious (or even possible to check at
        // present).
        return false;
      } else if (_nrgens > _relations.size() + _extra.size()) {
        return true;
      }

      auto is_letter_in_word = [](word_t const& w, size_t gen) {
        return (std::find(w.cbegin(), w.cend(), gen) != w.cend());
      };

      // Does there exist a generator which appears in no relation?
      for (size_t gen = 0; gen < _nrgens; ++gen) {
        bool found = false;
        for (auto it = _relations.cbegin(); it < _relations.cend() && !found;
             ++it) {
          found = is_letter_in_word((*it).first, gen)
                  || is_letter_in_word((*it).second, gen);
        }
        for (auto it = _extra.cbegin(); it < _extra.cend() && !found; ++it) {
          found = is_letter_in_word((*it).first, gen)
                  || is_letter_in_word((*it).second, gen);
        }
        if (!found) {
          return true;  // We found a generator not in any relation.
        }
      }
      // Otherwise, every generator occurs at least once in a relation.
      return false;
      // TODO: check that for every generator there exists a word in one of the
      // relations consisting solely of that generator, otherwise the order of
      // that generator is infinite.
      // TODO: check if the number of occurrences of a given letter is constant
      // on both sides of every relation, if so then again that letter has
      // infinite order.
    }

    //////////////////////////////////////////////////////////////////////////
    // NOTES on the data structures used by Todd-Coxeter
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
    //
    // We also store some special locations in the list:
    //   _current is the coset to which we are currently applying relations.
    //   _current_no_add is used instead of _current if we are in a packing
    //   phase.
    //   _last points to the final active coset in the list.
    //   _next points to the first free coset in the list.
    // Hence usually _next == _last + 1.
    //
    // COSET TABLES:
    //
    // We use these three tables to store all a coset's images and preimages.
    //   _table[c][i] is coset c's image under generator i.
    //   _preim_init[c][i] is ONE of coset c's preimages under generator i.
    //   _preim_next[c][i] is a coset that has THE SAME IMAGE as coset c
    //   (under i)
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
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Initialisation methods
    //////////////////////////////////////////////////////////////////////////

    // Private
    void ToddCoxeter::prefill(SemigroupBase* S) {
      LIBSEMIGROUPS_ASSERT(!_init_done);
      LIBSEMIGROUPS_ASSERT(_policy == use_cayley_graph);
      LIBSEMIGROUPS_ASSERT(_table.nr_rows() == 1);
      LIBSEMIGROUPS_ASSERT(_table.nr_cols() == S->nrgens());
      LIBSEMIGROUPS_ASSERT(S->nrgens() == _nrgens);
      _table.add_rows(S->size());
      for (size_t i = 0; i < _nrgens; i++) {
        _table.set(0, i, S->letter_to_pos(i) + 1);
      }
      if (type() == LEFT) {
        for (size_t row = 0; row < S->size(); ++row) {
          for (size_t col = 0; col < _nrgens; ++col) {
            _table.set(row + 1, col, S->left(row, col) + 1);
          }
        }
      } else {
        for (size_t row = 0; row < S->size(); ++row) {
          for (size_t col = 0; col < _nrgens; ++col) {
            _table.set(row + 1, col, S->right(row, col) + 1);
          }
        }
      }
      init_after_prefill();
    }

    // Public
    void ToddCoxeter::prefill(RecVec<class_index_t> const& table) {
      LIBSEMIGROUPS_ASSERT(!_init_done);
      LIBSEMIGROUPS_ASSERT(_policy == none);
      LIBSEMIGROUPS_ASSERT(_base == nullptr);
      LIBSEMIGROUPS_ASSERT(table.nr_rows() > 0);
      LIBSEMIGROUPS_ASSERT(table.nr_cols() == _nrgens);
      LIBSEMIGROUPS_ASSERT(_relations.empty());
      LIBSEMIGROUPS_ASSERT(_table.nr_rows() == 1);
      _table = table;
      LIBSEMIGROUPS_ASSERT(validate_table());
      // TODO Suppose that "table" is the right/left Cayley graph of a Semigroup
      // and add a row at the start for coset 0. See [todd-coxeter][21]. This
      // would make this method more useable.
      init_after_prefill();
    }

    // Private
    void ToddCoxeter::init_after_prefill() {
      LIBSEMIGROUPS_ASSERT(_table.nr_cols() == _nrgens);
      LIBSEMIGROUPS_ASSERT(_table.nr_rows() > 1);
      LIBSEMIGROUPS_ASSERT(!_init_done);
      LIBSEMIGROUPS_ASSERT(_relations.empty());
      _prefilled = true;
      _active    = _table.nr_rows();
      _id_coset  = 0;

      _forwd.reserve(_active);
      _bckwd.reserve(_active);

      for (size_t i = 1; i < _active; i++) {
        _forwd.push_back(i + 1);
        _bckwd.push_back(i - 1);
      }

      _forwd[0]           = 1;
      _forwd[_active - 1] = UNDEFINED;

      _last = _active - 1;

      _preim_init.add_rows(_table.nr_rows());
      _preim_next.add_rows(_table.nr_rows());

      for (class_index_t c = 0; c < _active; c++) {
        for (size_t i = 0; i < _nrgens; i++) {
          class_index_t b = _table.get(c, i);
          _preim_next.set(c, i, _preim_init.get(b, i));
          _preim_init.set(b, i, c);
        }
      }
      _defined = _active;
    }

    // Private
    void ToddCoxeter::use_relations_or_cayley_graph() {
      if (_base != nullptr) {
        switch (_policy) {
          case use_relations:
            relations(_base, [this](word_t lhs, word_t rhs) -> void {
              _relations.push_back(make_pair(lhs, rhs));
            });
            LIBSEMIGROUPS_ASSERT(validate_relations());
            break;
          case use_cayley_graph:
            prefill(_base);
            LIBSEMIGROUPS_ASSERT(validate_table());
            break;
          case none:
            // TODO exception
            LIBSEMIGROUPS_ASSERT(false);
        }
      }
    }

    // Private: do not call this directly, use init() instead!
    void ToddCoxeter::init_relations() {
      // This should not have been run before
      LIBSEMIGROUPS_ASSERT(!_init_done);

      // Add the relations/Cayley graph from _base if any.
      use_relations_or_cayley_graph();

      switch (type()) {
        case LEFT:
          for (relation_t& rel : _extra) {
            std::reverse(rel.first.begin(), rel.first.end());
            std::reverse(rel.second.begin(), rel.second.end());
          }
          for (relation_t& rel : _relations) {
            std::reverse(rel.first.begin(), rel.first.end());
            std::reverse(rel.second.begin(), rel.second.end());
          }
          break;
        case RIGHT:  // do nothing
          break;
        case TWOSIDED:
          if (!_extra.empty()) {
            _relations.insert(_relations.end(), _extra.cbegin(), _extra.cend());
            _extra.clear();
            // Don't reset_isomorphic_non_fp_semigroup here since although we
            // are changing the relations, we are not changing the semigroup
            // over which this is defined.
          }
          break;
        default:
          LIBSEMIGROUPS_ASSERT(false);
      }
    }

    // Private
    void ToddCoxeter::init() {
      if (!_init_done) {
        init_relations();
        // Apply each "extra" relation to the first coset only
        for (relation_t const& rel : _extra) {
          trace(_id_coset, rel);  // Allow new cosets
        }
        _init_done = true;
      }
    }

    // Private
    void ToddCoxeter::init_non_trivial_classes() {
      // FIXME if not is proper quotient also!! If this is the case, then
      // we might end up enumerating an infinite semigroup in the loop below.
      if (!_non_trivial_classes.empty()) {
        // There are no non-trivial classes, or we already found them.
        return;
      } else if (_base == nullptr) {
        throw LibsemigroupsException("There's no base semigroup in which to "
                                     "find the non-trivial classes");
      }

      _non_trivial_classes.assign(_table.nr_rows(), std::vector<word_t>());

      word_t w;
      for (size_t pos = 0; pos < _base->size(); ++pos) {
        _base->factorisation(w, pos);
        LIBSEMIGROUPS_ASSERT(word_to_class_index(w) < _table.nr_rows());
        _non_trivial_classes[word_to_class_index(w)].push_back(w);
      }

      _non_trivial_classes.erase(
          std::remove_if(_non_trivial_classes.begin(),
                         _non_trivial_classes.end(),
                         [this](std::vector<word_t> const& klass) -> bool {
                           return klass.size() <= 1;
                         }),
          _non_trivial_classes.end());
    }

    //////////////////////////////////////////////////////////////////////////
    // Overridden virtual methods from Runner
    //////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::run() {
      if (finished()) {
        return;
      }
      Timer timer;
      init();
      if (is_quotient_obviously_infinite()) {
        throw LibsemigroupsException(
            "this is infinite and Todd-Coxeter will never terminate");
      }

      while (!dead() && !timed_out() && _current != _next) {
        // Apply each relation to the "_current" coset
        for (relation_t const& rel : _relations) {
          trace(_current, rel);  // Allow new cosets
        }

        // If the number of active cosets is too high, start a packing phase
        if (_active > _pack) {
          REPORT(_defined << " defined, " << _forwd.size() << " max, "
                          << _active << " active, "
                          << (_defined - _active) - _cosets_killed
                          << " killed, "
                          << "current " << _current);
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
          } while (!dead() && _current_no_add != _next && !_stop_packing);

          REPORT("Entering lookahead complete " << oldactive - _active
                                                << " killed");

          _pack += _pack / 10;  // Raise packing threshold 10%
          _stop_packing   = false;
          _current_no_add = UNDEFINED;
        }

        // Move onto the next coset
        _current = _forwd[_current];

        // Quit loop when we reach an inactive coset
      }

      // Final report
      REPORT(_defined << " cosets defined,"
                      << " maximum " << _forwd.size() << ", " << _active
                      << " survived");
      REPORT("elapsed time = " << timer);
      if (dead()) {
        REPORT("killed!");
      } else if (timed_out()) {
        REPORT("timed out!");
      } else {
        LIBSEMIGROUPS_ASSERT(_current == _next);
        REPORT("finished!");
        set_finished();
        compress();
      }
      // No return value: all info is now stored in the class
    }

    //////////////////////////////////////////////////////////////////////////
    // ToddCoxeter specific methods
    //////////////////////////////////////////////////////////////////////////
    // TODO check order
    bool ToddCoxeter::empty() const {
      return _relations.empty() && _extra.empty()
             && (_table.empty()
                 || (_table.nr_rows() == 1
                     && std::all_of(_table.cbegin_row(0),
                                    _table.cend_row(0),
                                    [](class_index_t x) -> bool {
                                      return x == UNDEFINED;
                                    })));
    }

    void ToddCoxeter::reset_isomorphic_non_fp_semigroup() {
      if (_delete_isomorphic_non_fp_semigroup) {
        delete _isomorphic_non_fp_semigroup;
      }
      _delete_isomorphic_non_fp_semigroup = true;
      _isomorphic_non_fp_semigroup        = nullptr;
    }

    word_t ToddCoxeter::string_to_word(std::string const& s) const {
      word_t w;
      w.reserve(s.size());
      for (char const& c : s) {
        w.push_back(char_to_uint(c));
      }
      return w;
    }

    std::string ToddCoxeter::word_to_string(word_t const& w) const {
      std::string s;
      s.reserve(w.size());
      for (size_t const& l : w) {
        s.push_back(uint_to_char(l));
      }
      return s;
    }

    class_index_t ToddCoxeter::right(class_index_t i, letter_t j) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      return _table.get(i, j);
    }

    void ToddCoxeter::set_pack(size_t val) {
      _pack = val;
    }

    //////////////////////////////////////////////////////////////////////////
    // Other private methods
    //////////////////////////////////////////////////////////////////////////

    // Create a new active coset for coset c to map to under generator a
    void ToddCoxeter::new_coset(class_index_t const& c, letter_t const& a) {
      _active++;
      _defined++;

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
      for (letter_t i = 0; i < _nrgens; i++) {
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
    void ToddCoxeter::identify_cosets(class_index_t lhs, class_index_t rhs) {
      // Note that _lhs_stack and _rhs_stack may not be empty, if this was
      // killed before and has been restarted.
      // TODO add assertion that lhs and rhs are valid values

      // Make sure lhs < rhs
      if (lhs == rhs) {
        return;
      } else if (rhs < lhs) {
        class_index_t tmp = lhs;
        lhs               = rhs;
        rhs               = tmp;
      }

      // TODO Replace the following line with "while (!_killed)" and simply run
      // identify_cosets if TC::run is called when _lhs_stack or _rhs_stack is
      // not empty.
      while (true) {
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

          for (letter_t i = 0; i < _nrgens; i++) {
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
      LIBSEMIGROUPS_ASSERT(_lhs_stack.empty() && _rhs_stack.empty());
    }

    // Take the two words of the relation <rel>, apply them both to the coset
    // <c>, and identify the two results.  If <add> is true (the default) then
    // new cosets will be created whenever necessary; if false, then we are
    // "packing", and this function will not create any new cosets.
    void ToddCoxeter::trace(class_index_t const& c,
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
      if (report()) {
        REPORT(_defined << " defined, " << _forwd.size() << " max, " << _active
                        << " active, " << (_defined - _active) - _cosets_killed
                        << " killed, "
                        << "current " << (add ? _current : _current_no_add))
        // If we are killing cosets too slowly, then stop packing
        if ((_defined - _active) - _cosets_killed < 100) {
          _stop_packing = true;
        }
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

    // compress the table
    void ToddCoxeter::compress() {
      if (dead()) {
        return;
      }
      LIBSEMIGROUPS_ASSERT(finished());
      if (_active == _table.nr_rows()) {
        return;
      }

      RecVec<class_index_t> table(_nrgens, _active);

      class_index_t pos = _id_coset;
      // old number to new numbers lookup
      std::unordered_map<class_index_t, class_index_t> lookup;
      size_t                                           next_index = 0;

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
        for (size_t i = 0; i < _nrgens; i++) {
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

    // TODO Should throw exception
    bool ToddCoxeter::validate_relations() const {
      for (auto const& rel : _relations) {
        if (!validate_word(rel.first) || !validate_word(rel.second)) {
          return false;
        }
      }
      for (auto const& rel : _extra) {
        if (!validate_word(rel.first) || !validate_word(rel.second)) {
          return false;
        }
      }
      return true;
    }

    // TODO Should throw exception
    bool ToddCoxeter::validate_table() const {
      if (!std::all_of(_table.cbegin(), _table.cend(), [this](class_index_t c) {
            return c > 0 && (c == UNDEFINED || c < _table.nr_rows());
          })) {
        return false;
      }
      return true;
    }

    class_index_t
    ToddCoxeter::const_word_to_class_index(word_t const& w) const {
      validate_word(w);
      class_index_t c = _id_coset;

      if (type() == LEFT) {
        // Iterate in reverse order
        for (auto rit = w.crbegin(); rit != w.crend() && c != UNDEFINED;
             ++rit) {
          c = _table.get(c, *rit);
        }
      } else {
        // Iterate in sequential order
        for (auto it = w.cbegin(); it != w.cend() && c != UNDEFINED; ++it) {
          c = _table.get(c, *it);
        }
      }
      return (c == UNDEFINED ? c : c - 1);
    }

    //////////////////////////////////////////////////////////////////////////
    // ToddCoxeter class
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Overridden virtual methods from fpsemigroup::Interface
    //
    // All methods in this section refer to the fp semigroup defined by
    // _relations (or _table if prefilled), and not to the quotient of that
    // semigroup by _extra (unless _extra is empty upon construction).
    //////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::set_nr_generators(size_t nr) {
      // TODO exceptions instead of assertions
      LIBSEMIGROUPS_ASSERT(_nrgens == UNDEFINED);
      _nrgens = nr;
      if (_alphabet == NO_ALPHABET) {
        std::string lphbt = "";
        for (size_t i = 0; i < _nrgens; ++i) {
          lphbt += static_cast<char>(i + 1);
        }
        set_alphabet(lphbt);
      }
      _preim_init = RecVec<class_index_t>(nr, 1, UNDEFINED),
      _preim_next = RecVec<class_index_t>(nr, 1, UNDEFINED),
      _table      = RecVec<class_index_t>(nr, 1, UNDEFINED);
    }

    size_t ToddCoxeter::nr_generators() const {
      return _nrgens;
    }

    void ToddCoxeter::set_alphabet(std::string lphbt) {
      // TODO exceptions instead of assertions
      LIBSEMIGROUPS_ASSERT(_alphabet == NO_ALPHABET);
      _alphabet = lphbt;
      _alphabet_map.reserve(_alphabet.size());
      for (size_t i = 0; i < _alphabet.size(); ++i) {
        _alphabet_map.emplace(std::make_pair(_alphabet[i], i));
      }
      if (_nrgens == UNDEFINED) {
        set_nr_generators(lphbt.size());
      }
    }

    // Private
    char ToddCoxeter::uint_to_char(size_t i) const {
      LIBSEMIGROUPS_ASSERT(i < _alphabet.size());
      return _alphabet[i];
    }

    // Private
    size_t ToddCoxeter::char_to_uint(char c) const {
      LIBSEMIGROUPS_ASSERT(_alphabet_map.find(c) != _alphabet_map.end());
      return (*_alphabet_map.find(c)).second;
    }

    // Private
    // TODO make void
    bool ToddCoxeter::validate_word(word_t const& w) const {
      LIBSEMIGROUPS_ASSERT(_nrgens != UNDEFINED);
      if (!std::all_of(
              w.cbegin(), w.cend(), [this](size_t i) { return i < _nrgens; })) {
        throw std::runtime_error("invalid word");
        // TODO more info
      }
      return true;
    }

    // Private
    bool ToddCoxeter::validate_word(std::string const& w) const {
      LIBSEMIGROUPS_ASSERT(_alphabet != NO_ALPHABET);
      return std::all_of(w.cbegin(), w.cend(), [this](char c) {
        return _alphabet_map.find(c) != _alphabet_map.end();
      });
    }

    // Private - for use by FpSemigroup
    void ToddCoxeter::set_isomorphic_non_fp_semigroup(SemigroupBase* S) {
      LIBSEMIGROUPS_ASSERT(_isomorphic_non_fp_semigroup == nullptr);
      LIBSEMIGROUPS_ASSERT(_policy == none);
      LIBSEMIGROUPS_ASSERT(empty());
      _policy                             = use_relations;
      _delete_isomorphic_non_fp_semigroup = false;
      _isomorphic_non_fp_semigroup        = S;
      if (_nrgens == UNDEFINED) {
        set_nr_generators(S->nrgens());
      }
      LIBSEMIGROUPS_ASSERT(S->nrgens() == _nrgens);
    }

    void ToddCoxeter::internal_add_relation(word_t lhs, word_t rhs) {
      // TODO exceptions instead of assertions
      LIBSEMIGROUPS_ASSERT(_nrgens != UNDEFINED);
      LIBSEMIGROUPS_ASSERT(validate_word(lhs));
      LIBSEMIGROUPS_ASSERT(validate_word(rhs));
      LIBSEMIGROUPS_ASSERT(!_init_done);
      _relations.push_back(std::make_pair(lhs, rhs));
    }

    void ToddCoxeter::add_relation(word_t lhs, word_t rhs) {
      // Add the relations from _isomorphic_non_fp_semigroup if any, and then
      // reset _isomorphic_non_fp_semigroup, since it is no longer valid.
      use_relations_or_cayley_graph();
      internal_add_relation(lhs, rhs);
      reset_isomorphic_non_fp_semigroup();
      reset_quotient_semigroup();
    }

    void ToddCoxeter::add_relation(std::string lhs, std::string rhs) {
      add_relation(string_to_word(lhs), string_to_word(rhs));
    }

    bool ToddCoxeter::is_obviously_finite() const {
      // TODO exception instead of assertion
      LIBSEMIGROUPS_ASSERT(_nrgens != UNDEFINED);
      return _prefilled
             || (_isomorphic_non_fp_semigroup != nullptr
                 && _isomorphic_non_fp_semigroup->is_done());
      // 1. _prefilled means that either we were created from a SemigroupBase*
      // with _policy = use_cayley_graph and we successfully prefilled the
      // table, or we manually prefilled the table.  In this case the semigroup
      // defined by _relations must be finite.
      //
      // 2. _isomorphic_non_fp_semigroup being defined and fully enumerated
      // means it is finite.
    }

  }  // namespace congruence

  namespace fpsemigroup {
    ToddCoxeter::ToddCoxeter() : _tcc(new congruence::ToddCoxeter(TWOSIDED)) {}

    void ToddCoxeter::run() {
      _tcc->run();
    }

    std::string const& ToddCoxeter::alphabet() const {
      return _alphabet;
    }
    SemigroupBase* ToddCoxeter::isomorphic_non_fp_semigroup() {
      if (_isomorphic_non_fp_semigroup != nullptr) {
        // This might be the case if this was constructed from a SemigroupBase*
        return _isomorphic_non_fp_semigroup;
      } else if (!_is_proper_quotient) {
        // This ToddCoxeter instance represents an fp semigroup, and so we can
        // compute the isomorphic non-fp semigroup.
        return quotient_semigroup();
      }
      throw std::runtime_error("this defines a proper quotient, cannot find "
                               "an isomorphic non-fp semigroup");
    }

    bool ToddCoxeter::has_isomorphic_non_fp_semigroup() {
      return _isomorphic_non_fp_semigroup != nullptr;
    }

    bool ToddCoxeter::equal_to(word_t const& u, word_t const& v) {
      if (u == v) {
        return true;
      } else if (!_prefilled && _relations.empty() && _extra.empty()) {
        // This defines the free semigroup
        return false;
      } else if (!_is_proper_quotient) {
        return word_to_class_index(u) == word_to_class_index(v);
      }
      throw std::runtime_error(
          "this defines a proper quotient, cannot test equality of words");
    }

    bool ToddCoxeter::equal_to(std::string const& u, std::string const& v) {
      if (u == v) {
        return true;
      } else if (!_prefilled && _relations.empty() && _extra.empty()) {
        // This defines the free semigroup
        return false;
      }
      return equal_to(string_to_word(u), string_to_word(v));
    }

    word_t ToddCoxeter::normal_form(word_t const& w) {
      // isomorphic_non_fp_semigroup throws if we cannot do this
      auto S = static_cast<Semigroup<TCE>*>(isomorphic_non_fp_semigroup());
      S->enumerate();
      word_t out;
      S->factorisation(out, S->word_to_pos(w));
      return out;
    }

    std::string ToddCoxeter::normal_form(std::string const& w) {
      return word_to_string(normal_form(string_to_word(w)));
    }

    // bool ToddCoxeter::is_obviously_infinite() const {
    //  return !is_obviously_finite() && is_quotient_obviously_infinite();
    //}

    // size_t ToddCoxeter::size() {
    //  return nr_classes();
    // }
  }  // namespace fpsemigroup
}  // namespace libsemigroups
