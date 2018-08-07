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

// TODO:
// 1. there doesn't seem to be any reason to store _relations as pairs, maybe
//    better to store it as a flat vector, this might reduce code duplication
//    too.
// 2. allow addition of identity (i.e. via empty string)
// 3. allow inverses
// 4. add constructor from RWS, see comments in somewhere in here??

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

#include "todd-coxeter.h"

#include <algorithm>
#include <unordered_map>

#include "internal/libsemigroups-debug.h"
#include "internal/libsemigroups-exception.h"
#include "internal/report.h"
#include "internal/stl.h"

#include "semigroup.h"
#include "tce.h"

namespace libsemigroups {

  namespace congruence {
    using signed_class_index_type = int64_t;
    using class_index_type        = CongIntf::class_index_type;

    /////////////////////////////////
    // Constructors and destructor //
    /////////////////////////////////

    ToddCoxeter::ToddCoxeter(congruence_type type)
        : CongIntf(type),
          _active(1),
          _bckwd(1, 0),
          _cosets_killed(0),
          _current(0),
          _current_no_add(UNDEFINED),
          _defined(1),
          _extra(),
          _forwd(1, static_cast<size_t>(UNDEFINED)),
          _id_coset(0),
          _init_done(false),
          _last(0),
          _lhs_stack(),
          _next(UNDEFINED),
          _pack(120000),
          _policy(policy::none),
          _prefilled(false),
          _preim_init(0, 0, UNDEFINED),
          _preim_next(0, 0, UNDEFINED),
          _relations(),
          _rhs_stack(),
          _stop_packing(false),
          _table(0, 0, UNDEFINED) {}

    ToddCoxeter::ToddCoxeter(congruence_type type, SemigroupBase* S, policy p)
        : ToddCoxeter(type) {
      _policy = p;
      set_parent(S);
      set_nr_generators(S->nrgens());
    }

    ToddCoxeter::ToddCoxeter(congruence_type                   type,
                             SemigroupBase*                    S,
                             std::vector<relation_type> const& genpairs,
                             policy                            p)
        : ToddCoxeter(type, S, p) {
      _extra = genpairs;
    }

    // TODO make this constructor private??
    ToddCoxeter::ToddCoxeter(congruence_type                   type,
                             size_t                            nrgens,
                             std::vector<relation_type> const& relations,
                             std::vector<relation_type> const& extra)
        : ToddCoxeter(type) {
      set_nr_generators(nrgens);
      _relations = relations;
      _extra     = extra;
      validate_relations();
    }

    ToddCoxeter::ToddCoxeter(ToddCoxeter const& copy)
        : ToddCoxeter(copy.type(),
                      copy.nr_generators(),
                      std::vector<relation_type>(copy._relations),
                      std::vector<relation_type>(copy._extra)) {
      // FIXME init_relations will be called on this at some point, and if it
      // was already called on copy, and we are a left congruence, then this
      // will reverse the relations again.
    }

    ToddCoxeter::ToddCoxeter(fpsemigroup::ToddCoxeter const& copy)
        : ToddCoxeter(*copy.congruence()) {}

    ToddCoxeter::~ToddCoxeter() {
      reset_quotient();
    }

    ////////////////////////////////////////////
    // Overridden virtual methods from Runner //
    ////////////////////////////////////////////

    void ToddCoxeter::run() {
      if (finished()) {
        return;
      }
      Timer timer;
      init();
      if (is_quotient_obviously_infinite()) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "there are infinitely many classes in the congruence and "
            "Todd-Coxeter will never terminate");
      }

      while (!dead() && !timed_out() && _current != _next) {
        // Apply each relation to the "_current" coset
        for (relation_type const& rel : _relations) {
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
            for (relation_type const& rel : _relations) {
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

    //////////////////////////////////////////////////////////
    // Overridden public pure virtual methods from CongIntf //
    //////////////////////////////////////////////////////////
    // FIXME should be word_type const&
    void ToddCoxeter::add_pair(word_type lhs, word_type rhs) {
      // TODO exception not assertion
      LIBSEMIGROUPS_ASSERT(nr_generators() != UNDEFINED);
      validate_word(lhs);
      validate_word(rhs);
      if (_init_done) {
        // TODO allow adding of pairs here
        throw LIBSEMIGROUPS_EXCEPTION("can't add pair at this point");
      }
      _extra.emplace_back(lhs, rhs);
      reset_quotient();
    }

    size_t ToddCoxeter::nr_classes() {
      if (is_quotient_obviously_infinite()) {
        return POSITIVE_INFINITY;
      } else {
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        return _active - 1;
      }
    }

    SemigroupBase* ToddCoxeter::quotient_semigroup() {
      if (type() != congruence_type::TWOSIDED) {
        throw LIBSEMIGROUPS_EXCEPTION("the congruence must be two-sided");
      } else if (!has_quotient()) {
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        // TODO replace with 0-parameter constructor when available
        Semigroup<TCE>* Q = new Semigroup<TCE>({TCE(this, _table.get(0, 0))});
        for (size_t i = 1; i < nr_generators(); ++i) {
          // We use _table.get(0, i) instead of just i, because there might be
          // more generators than cosets.
          Q->add_generator(TCE(this, _table.get(0, i)));
        }
        set_quotient(Q);
      }
      return quotient();
    }

    class_index_type ToddCoxeter::word_to_class_index(word_type const& w) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      class_index_type c = const_word_to_class_index(w);
      // c is in the range 1, ..., _active because 0 represents the identity
      // coset, and does not correspond to an element.
      LIBSEMIGROUPS_ASSERT(c < _active || c == UNDEFINED);
      return c;
    }

    word_type ToddCoxeter::class_index_to_word(class_index_type i) {
      // TODO check arg
      // quotient_semigroup throws if we cannot do this
      // TODO check the comment in the previous line is still accurate
      auto S = static_cast<Semigroup<TCE>*>(quotient_semigroup());
      S->enumerate();
      word_type out;
      S->minimal_factorisation(out, S->position(TCE(this, i + 1)));
      return out;  // TODO std::move?
    }

    //////////////////////////////////////////////////////////////////////////
    // Overridden public non-pure virtual methods from CongIntf
    //////////////////////////////////////////////////////////////////////////

    bool ToddCoxeter::contains(word_type const& lhs, word_type const& rhs) {
      if (lhs == rhs) {
        return true;
      } else if (!_prefilled && _relations.empty() && _extra.empty()) {
        // This defines the free semigroup
        return false;
      }
      return CongIntf::contains(lhs, rhs);
    }

    bool ToddCoxeter::is_quotient_obviously_infinite() {
      LIBSEMIGROUPS_ASSERT(nr_generators() != UNDEFINED);
      if (_policy != policy::none) {
        // _policy != none means we were created from a SemigroupBase*, which
        // means that this is infinite if and only if the SemigroupBase* is
        // infinite too, which is not obvious (or even possible to check at
        // present).
        return false;
      } else if (_prefilled) {
        return false;
      }
      init();
      if (nr_generators() > _relations.size() + _extra.size()) {
        return true;
      }
      auto is_letter_in_word = [](word_type const& w, size_t gen) {
        return (std::find(w.cbegin(), w.cend(), gen) != w.cend());
      };

      // Does there exist a generator which appears in no relation?
      for (size_t gen = 0; gen < nr_generators(); ++gen) {
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

    bool ToddCoxeter::is_quotient_obviously_finite() {
      return _prefilled || (has_quotient() && quotient()->is_done());
      // 1. _prefilled means that either we were created from a SemigroupBase*
      // with _policy = use_cayley_graph and we successfully prefilled the
      // table, or we manually prefilled the table.  In this case the semigroup
      // defined by _relations must be finite.
      //
      // 2. _isomorphic_non_fp_semigroup being defined and fully enumerated
      // means it is finite.
    }

    void ToddCoxeter::set_nr_generators(size_t n) {
      CongIntf::set_nr_generators(n);
      _preim_init = RecVec<class_index_type>(n, 1, UNDEFINED),
      _preim_next = RecVec<class_index_type>(n, 1, UNDEFINED),
      _table      = RecVec<class_index_type>(n, 1, UNDEFINED);
    }

    ////////////////////
    // Public methods //
    ////////////////////

    bool ToddCoxeter::empty() const {
      return _relations.empty() && _extra.empty()
             && (_table.empty()
                 || (_table.nr_rows() == 1
                     && std::all_of(_table.cbegin_row(0),
                                    _table.cend_row(0),
                                    [](class_index_type x) -> bool {
                                      return x == UNDEFINED;
                                    })));
    }

    ToddCoxeter::policy ToddCoxeter::get_policy() const noexcept {
      return _policy;
    }

    void ToddCoxeter::prefill(RecVec<class_index_type> const& table) {
      // TODO assertions -> exceptions
      LIBSEMIGROUPS_ASSERT(!_init_done);
      LIBSEMIGROUPS_ASSERT(_policy == policy::none);
      LIBSEMIGROUPS_ASSERT(!has_parent());
      LIBSEMIGROUPS_ASSERT(table.nr_rows() > 0);
      LIBSEMIGROUPS_ASSERT(table.nr_cols() == nr_generators());
      LIBSEMIGROUPS_ASSERT(_relations.empty());
      LIBSEMIGROUPS_ASSERT(_table.nr_rows() == 1);
      _table = table;
      validate_table();
      // TODO Suppose that "table" is the right/left Cayley graph of a Semigroup
      // and add a row at the start for coset 0. See [todd-coxeter][21]. This
      // would make this method more useable.
      init_after_prefill();
    }

    class_index_type ToddCoxeter::right(class_index_type i, letter_type j) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      return _table.get(i, j);
    }

    void ToddCoxeter::set_pack(size_t val) {
      _pack = val;
    }

    ///////////////////////////////////////////////////////////
    // Overridden private pure virtual methods from CongIntf //
    ///////////////////////////////////////////////////////////

    class_index_type
    ToddCoxeter::const_word_to_class_index(word_type const& w) const {
      validate_word(w);
      class_index_type c = _id_coset;

      if (type() == congruence_type::LEFT) {
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

    //////////////////////////////////
    // Private methods - validation //
    //////////////////////////////////

    // TODO This method should go into cong-intf
    void ToddCoxeter::validate_relations() const {
      for (auto const& rel : _relations) {
        validate_word(rel.first);
        validate_word(rel.second);
      }
      for (auto const& rel : _extra) {
        validate_word(rel.first);
        validate_word(rel.second);
      }
    }

    void ToddCoxeter::validate_table() const {
      for (size_t i = 0; i < _table.nr_rows(); ++i) {
        for (size_t j = 0; j < _table.nr_cols(); ++j) {
          class_index_type c = _table.get(i, j);
          if (c == 0 || (c != UNDEFINED && c >= _table.nr_rows())) {
            throw LIBSEMIGROUPS_EXCEPTION(
                "invalid table, the entry in row " + to_string(i)
                + " and column " + to_string(j) + " should be in the range [1, "
                + to_string(_table.nr_rows()) + ") or UNDEFINED, but is "
                + to_string(c));
          }
        }
      }
    }

    // TODO This method should go into cong-intf
    void ToddCoxeter::validate_word(word_type const& w) const {
      LIBSEMIGROUPS_ASSERT(nr_generators() != UNDEFINED);
      for (auto const& l : w) {
        if (l >= nr_generators()) {
          throw LIBSEMIGROUPS_EXCEPTION("invalid word, found " + to_string(l)
                                        + " should be at most "
                                        + to_string(nr_generators()));
        }
      }
    }

    //////////////////////////////////////
    // Private methods - initialisation //
    //////////////////////////////////////

    void ToddCoxeter::init() {
      if (!_init_done) {
        init_relations();
        // Apply each "extra" relation to the first coset only
        for (relation_type const& rel : _extra) {
          trace(_id_coset, rel);  // Allow new cosets
        }
        _init_done = true;
      }
    }

    void ToddCoxeter::init_after_prefill() {
      LIBSEMIGROUPS_ASSERT(_table.nr_cols() == nr_generators());
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

      for (class_index_type c = 0; c < _active; c++) {
        for (size_t i = 0; i < nr_generators(); i++) {
          class_index_type b = _table.get(c, i);
          _preim_next.set(c, i, _preim_init.get(b, i));
          _preim_init.set(b, i, c);
        }
      }
      _defined = _active;
    }

    // Private: do not call this directly, use init() instead!
    void ToddCoxeter::init_relations() {
      // This should not have been run before
      LIBSEMIGROUPS_ASSERT(!_init_done);

      // Add the relations/Cayley graph from parent() if any.
      use_relations_or_cayley_graph();

      switch (type()) {
        case congruence_type::LEFT:
          for (relation_type& rel : _extra) {
            std::reverse(rel.first.begin(), rel.first.end());
            std::reverse(rel.second.begin(), rel.second.end());
          }
          for (relation_type& rel : _relations) {
            std::reverse(rel.first.begin(), rel.first.end());
            std::reverse(rel.second.begin(), rel.second.end());
          }
          break;
        case congruence_type::RIGHT:  // do nothing
          break;
        case congruence_type::TWOSIDED:
          if (!_extra.empty()) {
            _relations.insert(_relations.end(), _extra.cbegin(), _extra.cend());
            _extra.clear();
            // Don't reset_quotient_semigroup here since although we are
            // changing the relations, we are not changing the semigroup over
            // which this congruence is defined.
          }
          break;
        default:
          LIBSEMIGROUPS_ASSERT(false);
      }
    }

    void ToddCoxeter::prefill(SemigroupBase* S) {
      LIBSEMIGROUPS_ASSERT(!_init_done);
      LIBSEMIGROUPS_ASSERT(_policy == policy::use_cayley_graph);
      LIBSEMIGROUPS_ASSERT(_table.nr_rows() == 1);
      LIBSEMIGROUPS_ASSERT(_table.nr_cols() == S->nrgens());
      LIBSEMIGROUPS_ASSERT(S->nrgens() == nr_generators());
      _table.add_rows(S->size());
      for (size_t i = 0; i < nr_generators(); i++) {
        _table.set(0, i, S->letter_to_pos(i) + 1);
      }
      if (type() == congruence_type::LEFT) {
        for (size_t row = 0; row < S->size(); ++row) {
          for (size_t col = 0; col < nr_generators(); ++col) {
            _table.set(row + 1, col, S->left(row, col) + 1);
          }
        }
      } else {
        for (size_t row = 0; row < S->size(); ++row) {
          for (size_t col = 0; col < nr_generators(); ++col) {
            _table.set(row + 1, col, S->right(row, col) + 1);
          }
        }
      }
      init_after_prefill();
    }

    void ToddCoxeter::use_relations_or_cayley_graph() {
      // FIXME nothing stops this from being called multiple times
      if (has_parent()) {
        switch (_policy) {
          case policy::use_relations:
            relations(parent(), [this](word_type lhs, word_type rhs) -> void {
              _relations.emplace_back(lhs, rhs);
            });
#ifdef LIBSEMIGROUPS_DEBUG
            // This is a check of program logic, since we use parent() to
            // obtain the relations, so we only validate in debug mode.
            validate_relations();
#endif
            break;
          case policy::use_cayley_graph:
            prefill(parent());
#ifdef LIBSEMIGROUPS_DEBUG
            // This is a check of program logic, since we use parent() to fill
            // the table, so we only validate in debug mode.
            validate_table();
#endif
            break;
          case policy::none:
            // So that this doesn't fail silently
            throw INTERNAL_EXCEPTION;
        }
      }
    }

    /////////////////////////////
    // Private methods - other //
    /////////////////////////////

    // Compress the table
    void ToddCoxeter::compress() {
      if (dead()) {
        return;
      }
      LIBSEMIGROUPS_ASSERT(finished());
      if (_active == _table.nr_rows()) {
        return;
      }

      RecVec<class_index_type> table(nr_generators(), _active);

      class_index_type pos = _id_coset;
      // old number to new numbers lookup
      std::unordered_map<class_index_type, class_index_type> lookup;
      size_t                                                 next_index = 0;

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
        for (size_t i = 0; i < nr_generators(); i++) {
          class_index_type val = _table.get(pos, i);
          it                   = lookup.find(val);
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

    // Create a new active coset for coset c to map to under generator a
    void ToddCoxeter::new_coset(class_index_type const& c,
                                letter_type const&      a) {
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
      for (letter_type i = 0; i < nr_generators(); i++) {
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
    void ToddCoxeter::identify_cosets(class_index_type lhs,
                                      class_index_type rhs) {
      // Note that _lhs_stack and _rhs_stack may not be empty, if this was
      // killed before and has been restarted.
      // TODO add assertion that lhs and rhs are valid values

      // Make sure lhs < rhs
      if (lhs == rhs) {
        return;
      } else if (rhs < lhs) {
        class_index_type tmp = lhs;
        lhs                  = rhs;
        rhs                  = tmp;
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
          _bckwd[rhs] = -static_cast<signed_class_index_type>(lhs);

          for (letter_type i = 0; i < nr_generators(); i++) {
            // Let <v> be the first PREIMAGE of <rhs>
            class_index_type v = _preim_init.get(rhs, i);
            while (v != UNDEFINED) {
              _table.set(v, i, lhs);  // Replace <rhs> by <lhs> in the table
              class_index_type u
                  = _preim_next.get(v, i);  // Get <rhs>'s next preimage
              _preim_next.set(v, i, _preim_init.get(lhs, i));
              _preim_init.set(lhs, i, v);
              // v is now a preimage of <lhs>, not <rhs>
              v = u;  // Let <v> be <rhs>'s next preimage, and repeat
            }

            // Now let <v> be the IMAGE of <rhs>
            v = _table.get(rhs, i);
            if (v != UNDEFINED) {
              class_index_type u = _preim_init.get(v, i);
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
    void ToddCoxeter::trace(class_index_type const& c,
                            relation_type const&    rel,
                            bool                    add) {
      class_index_type lhs = c;
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

      class_index_type rhs = c;
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

      letter_type      a = rel.first.back();
      letter_type      b = rel.second.back();
      class_index_type u = _table.get(lhs, a);
      class_index_type v = _table.get(rhs, b);
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
  }  // namespace congruence
  /*
    namespace fpsemigroup {
      //////////////////
      // Constructors //
      //////////////////

      ToddCoxeter::ToddCoxeter()
          : _nr_rules(0),
            _tcc(libsemigroups::make_unique<congruence::ToddCoxeter>(
                congruence_type::TWOSIDED)) {}

      ToddCoxeter::ToddCoxeter(std::string const& lphbt) : ToddCoxeter() {
        set_alphabet(lphbt);
      }

      ToddCoxeter::ToddCoxeter(SemigroupBase* S) : ToddCoxeter() {
        set_alphabet(S->nrgens());
        add_rules(S);
        _nr_rules += S->nrrules();
        // TODO something like the following
        // if (S->nr_rules() == this->nr_rules()) {
        //   set_isomorphic_non_fp_semigroup(S);
        // }
      }

      // TODO move to FpSemiIntf?
      ToddCoxeter::ToddCoxeter(SemigroupBase& S) : ToddCoxeter(&S) {}

      void ToddCoxeter::run() {
        _tcc->run();
      }

      /////////////////////////////////////////////////////
      // Overridden pure virtual methods from FpSemiIntf //
      /////////////////////////////////////////////////////

      void ToddCoxeter::add_rule(std::string const& lhs, std::string const& rhs)
    { if (!is_alphabet_defined()) { throw
    LIBSEMIGROUPS_EXCEPTION("ToddCoxeter::add_rule: cannot add rules " "before
    an alphabet is defined");
        }
        // We perform these checks here because string_to_word fails if lhs/rhs
        // are not valid, and string_to_word does not checks.
        validate_word(lhs);
        validate_word(rhs);
        _nr_rules++;
        _tcc->add_pair(string_to_word(lhs), string_to_word(rhs));
      }

      bool ToddCoxeter::is_obviously_finite() {
        return _tcc->is_quotient_obviously_finite();
      }

      bool ToddCoxeter::is_obviously_infinite() {
        return _tcc->is_quotient_obviously_infinite();
      }

      size_t ToddCoxeter::size() {
        return _tcc->nr_classes();
      }

      bool ToddCoxeter::equal_to(std::string const& lhs, std::string const& rhs)
    { return _tcc->contains(string_to_word(lhs), string_to_word(rhs));
      }

      // TODO improve the many copies etc in:
      // string -> word_type -> class_index_type -> word_type -> string
      std::string ToddCoxeter::normal_form(std::string const& w) {
        // TODO use the other normal_form method
        // FIXME there's an off by one error in the output of string_to_word..
        word_type ww = string_to_word(w);
        std::for_each(ww.begin(), ww.end(), [](size_t& i) -> void { ++i; });
        return word_to_string(
            _tcc->class_index_to_word(_tcc->word_to_class_index(ww)));
      }

      SemigroupBase* ToddCoxeter::isomorphic_non_fp_semigroup() {
        // _tcc handles changes to this that effect the quotient.
        return _tcc->quotient_semigroup();
      }

      size_t ToddCoxeter::nr_rules() const noexcept {
        return _nr_rules;
      }

      /////////////////////////////////////////////////////////
      // Overridden non-pure virtual methods from FpSemiIntf //
     /////////////////////////////////////////////////////////

      // We override FpSemiIntf::add_rule to avoid unnecessary conversion from
      // word_type -> string.
      void ToddCoxeter::add_rule(word_type const& lhs, word_type const& rhs) {
        if (lhs.empty() || rhs.empty()) {
          throw LIBSEMIGROUPS_EXCEPTION(
              "ToddCoxeter::add_rule: rules must be non-empty");
        }
        validate_word(lhs);
        validate_word(rhs);
        _tcc->add_pair(lhs, rhs);
      }

      // We override FpSemiIntf::equal_to to avoid unnecessary conversion from
      // word_type -> string.
      bool ToddCoxeter::equal_to(word_type const& lhs, word_type const& rhs) {
        return _tcc->contains(lhs, rhs);
      }

      // We override FpSemiIntf::normal_form to avoid unnecessary conversion
    from
      // word_type -> string.
      word_type ToddCoxeter::normal_form(word_type const& w) {
        return _tcc->class_index_to_word(_tcc->word_to_class_index(w));
      }

      // We override FpSemiIntf::set_alphabet so that we can set the number of
      // generators in _tcc.
      void ToddCoxeter::set_alphabet(std::string const& lphbet) {
        FpSemiIntf::set_alphabet(lphbet);
        _tcc->set_nr_generators(lphbet.size());
      }

      // We override FpSemiIntf::set_alphabet so that we can set the number of
      // generators in _tcc.
      void ToddCoxeter::set_alphabet(size_t nr_letters) {
        FpSemiIntf::set_alphabet(nr_letters);
        _tcc->set_nr_generators(nr_letters);
      }
    }  // namespace fpsemigroup*/
}  // namespace libsemigroups
