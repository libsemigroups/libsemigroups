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

// TODO(now)
// 1. allow inverses
// 2. add constructor from RWS, see comments in somewhere in here?

// TODO(later)
// 1. there doesn't seem to be any reason to store _relations as pairs, maybe
//    better to store it as a flat vector, this might reduce code duplication
//    too.

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

#include "todd-coxeter.hpp"

#include <algorithm>      // for reverse, max_element
#include <string>         // for operator+, basic_string
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair

#include "internal/libsemigroups-config.hpp"     // for LIBSEMIGROUPS_DEBUG
#include "internal/libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "internal/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "internal/report.hpp"                   // for REPORT
#include "internal/stl.hpp"                      // for to_string
#include "internal/timer.hpp"                    // for Timer

#include "froidure-pin-base.hpp"  // for FroidurePinBase
#include "froidure-pin.hpp"       // for FroidurePin
#include "knuth-bendix.hpp"       // for fpsemigroup::KnuthBendix
#include "obvinf.hpp"             // for IsObviouslyInfinite
#include "tce.hpp"                // for TCE

namespace libsemigroups {

  namespace congruence {
    using signed_class_index_type = int64_t;
    using class_index_type        = CongBase::class_index_type;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - constructors and destructor - public
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeter::ToddCoxeter(congruence_type type)
        : CongBase(type),
          _active(1),
          _bckwd(1, 0),
          _class_index_to_letter(),
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
          _relations_are_reversed(false),
          _relations(),
          _rhs_stack(),
          _stop_packing(false),
          _table(0, 0, UNDEFINED) {}

    ToddCoxeter::ToddCoxeter(congruence_type type, FroidurePinBase& S, policy p)
        : ToddCoxeter(type) {
      _policy = p;
      set_parent_semigroup(S);
      set_nr_generators(S.nr_generators());
    }

    // Construct a ToddCoxeter object representing a congruence over the
    // semigroup defined by copy (the quotient that is).
    ToddCoxeter::ToddCoxeter(congruence_type typ, ToddCoxeter const& copy)
        : ToddCoxeter(typ) {
      if (copy.type() != congruence_type::TWOSIDED && typ != copy.type()) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "incompatible types of congruence, found ("
            + congruence_type_to_string(copy.type()) + " / "
            + congruence_type_to_string(typ)
            + ") but only (left / left), (right / "
              "right), (two-sided / *) are valid");
      }
      LIBSEMIGROUPS_ASSERT(!_relations_are_reversed
                           || typ == congruence_type::LEFT);
      set_nr_generators(copy.nr_generators());
      _relations_are_reversed = copy._relations_are_reversed;
      _relations              = copy._relations;
      _relations.insert(
          _relations.end(), copy._extra.cbegin(), copy._extra.cend());
    }

    ToddCoxeter::ToddCoxeter(congruence_type           typ,
                             fpsemigroup::ToddCoxeter& copy)
        : ToddCoxeter(typ, copy.congruence()) {
      LIBSEMIGROUPS_ASSERT(!has_parent_semigroup());
      if (copy.finished()) {
        set_parent_semigroup(copy.isomorphic_non_fp_semigroup());
        LIBSEMIGROUPS_ASSERT(_policy == policy::none);
        _policy = policy::use_relations;
        // FIXME assertion failure if we use_cayley_graph
        // TODO(now) should be use_cayley_graph
      }
    }

    ToddCoxeter::ToddCoxeter(congruence_type typ, fpsemigroup::KnuthBendix& kb)
        : ToddCoxeter(typ) {
      set_nr_generators(kb.alphabet().size());
      for (auto it = kb.cbegin_rules(); it < kb.cend_rules(); ++it) {
        add_pair(kb.string_to_word(it->first), kb.string_to_word(it->second));
      }
      if (kb.finished()) {
        set_parent_semigroup(kb.isomorphic_non_fp_semigroup());
        LIBSEMIGROUPS_ASSERT(_policy == policy::none);
        _policy = policy::use_cayley_graph;
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::run() {
      if (stopped()) {
        return;
      } else if (is_quotient_obviously_infinite()) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "there are infinitely many classes in the congruence and "
            "Todd-Coxeter will never terminate");
      }

      Timer timer;
      init();

      while (_current != _next && !dead() && !timed_out()) {
        // Apply each relation to the "_current" coset
        for (relation_type const& rel : _relations) {
          trace(_current, rel);  // Allow new cosets
        }
        if (type() == congruence_type::TWOSIDED
            || (_relations.empty() && !_prefilled)) {
          for (relation_type const& rel : _extra) {
            trace(_current, rel);  // Allow new cosets
          }
        }

        // If the number of active cosets is too high, start a packing phase
        if (_active > _pack) {
          REPORT(_defined,
                 " defined, ",
                 _forwd.size(),
                 " max, ",
                 _active,
                 " active, ",
                 (_defined - _active) - _cosets_killed,
                 " killed, ",
                 "current ",
                 _current);
          REPORT("entering lookahead phase . . .");
          _cosets_killed = _defined - _active;

          size_t oldactive = _active;       // Keep this for stats
          _current_no_add  = _current + 1;  // Start packing from _current

          do {
            // Apply every relation to the "_current_no_add" coset
            for (relation_type const& rel : _relations) {
              trace(_current_no_add, rel, false);  // Don't allow new cosets
            }
            if (type() == congruence_type::TWOSIDED
                || (_relations.empty() && !_prefilled)) {
              for (relation_type const& rel : _extra) {
                trace(_current, rel);  // Allow new cosets
              }
            }
            _current_no_add = _forwd[_current_no_add];

            // Quit loop if we reach an inactive coset OR we get a "stop"
            // signal
          } while (!_stop_packing && _current_no_add != _next && !dead()
                   && !timed_out());

          REPORT("lookahead complete ", oldactive - _active, " killed");

          _pack += _pack / 10;  // Raise packing threshold 10%
          _stop_packing   = false;
          _current_no_add = UNDEFINED;
        }

        // Move onto the next coset
        _current = _forwd[_current];

        // Quit loop when we reach an inactive coset
      }

      if (!dead() && !timed_out()) {
        LIBSEMIGROUPS_ASSERT(_current == _next);
        set_finished(true);
        compress();
        class_index_type max
            = *std::max_element(_table.cbegin_row(0), _table.cend_row(0));
        _class_index_to_letter.resize(max + 1, UNDEFINED);
        for (letter_type i = 0; i < nr_generators(); ++i) {
          _class_index_to_letter[_table.get(0, i)] = i;
        }
      }

      // Final report
      REPORT(_defined,
             " cosets defined,",
             " maximum ",
             _forwd.size(),
             ", ",
             _active,
             " survived");
      REPORT("elapsed time = ", timer);
      report_why_we_stopped();
      // No return value: all info is now stored in the class
    }

    ////////////////////////////////////////////////////////////////////////
    // CongBase - pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////

    size_t ToddCoxeter::nr_classes() {
      if (is_quotient_obviously_infinite()) {
        return POSITIVE_INFINITY;
      } else {
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        return _active - 1;
      }
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
      if (i >= nr_classes()) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "invalid class index, should be in the range [0, "
            + to_string(nr_classes()) + "), not " + to_string(i));
      }

      // FIXME(now) quotient_semigroup throws if this is not 2-sided
      // but we could return an answer to this question anyway.
      auto fp = static_cast<FroidurePin<TCE>&>(quotient_semigroup());
      return fp.minimal_factorisation(TCE(this, i + 1));
    }

    ////////////////////////////////////////////////////////////////////////
    // CongBase - non-pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////

    bool ToddCoxeter::contains(word_type const& lhs, word_type const& rhs) {
      if (lhs == rhs) {
        return true;
      } else if (!_prefilled && _relations.empty() && _extra.empty()) {
        // This defines the free semigroup
        return false;
      }
      return CongBase::contains(lhs, rhs);
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - methods - public
    ////////////////////////////////////////////////////////////////////////

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

    letter_type ToddCoxeter::class_index_to_letter(class_index_type x) {
      run();
      LIBSEMIGROUPS_ASSERT(x < _class_index_to_letter.size());
      LIBSEMIGROUPS_ASSERT(_class_index_to_letter[x] != UNDEFINED);
      return _class_index_to_letter[x];
    }

    ToddCoxeter::policy ToddCoxeter::get_policy() const noexcept {
      return _policy;
    }

    class_index_type ToddCoxeter::table(class_index_type i, letter_type j) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      return _table.get(i, j);
    }

    void ToddCoxeter::set_pack(size_t val) {
      _pack = val;
    }

    ////////////////////////////////////////////////////////////////////////
    // CongBase - pure virtual methods - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::add_pair_impl(word_type const& u, word_type const& v) {
      word_type U = u;
      word_type V = v;
      if (_relations_are_reversed) {
        std::reverse(U.begin(), U.end());
        std::reverse(V.begin(), V.end());
      }
      _extra.emplace_back(std::move(U), std::move(V));
    }

    internal::owned_ptr<FroidurePinBase> ToddCoxeter::quotient_impl() {
      if (type() != congruence_type::TWOSIDED) {
        throw LIBSEMIGROUPS_EXCEPTION("the congruence must be two-sided");
      }
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      // TODO replace with 0-parameter constructor when available
      std::vector<TCE> gens;
      for (size_t i = 0; i < nr_generators(); ++i) {
        // We use _table.get(0, i) instead of just i, because there might be
        // more generators than cosets.
        gens.emplace_back(this, _table.get(0, i));
      }
      return internal::owned_ptr<FroidurePinBase>(new FroidurePin<TCE>(gens),
                                                  this);
    }

    ////////////////////////////////////////////////////////////////////////
    // CongBase - non-pure virtual methods - private
    ////////////////////////////////////////////////////////////////////////

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

    void ToddCoxeter::set_nr_generators_impl(size_t n) {
      _preim_init = RecVec<class_index_type>(n, 1, UNDEFINED),
      _preim_next = RecVec<class_index_type>(n, 1, UNDEFINED),
      _table      = RecVec<class_index_type>(n, 1, UNDEFINED);
    }

    bool ToddCoxeter::is_quotient_obviously_infinite_impl() {
      init();
      if (_prefilled) {
        return false;
      } else if (nr_generators() > _relations.size() + _extra.size()) {
        return true;
      }
      internal::IsObviouslyInfinite<letter_type, word_type> ioi(
          nr_generators());
      ioi.add_rules(_relations.cbegin(), _relations.cend());
      ioi.add_rules(_extra.cbegin(), _extra.cend());
      return ioi.result();
    }

    bool ToddCoxeter::is_quotient_obviously_finite_impl() {
      init();
      return _prefilled;
      // _prefilled means that either we were created from a FroidurePinBase*
      // with _policy = use_cayley_graph and we successfully prefilled the
      // table, or we manually prefilled the table.  In this case the semigroup
      // defined by _relations must be finite.
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - methods (validation) - private
    ////////////////////////////////////////////////////////////////////////

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

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - methods (initialisation) - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::init() {
      if (!_init_done) {
        // Add the relations/Cayley graph from parent() if any.
        use_relations_or_cayley_graph();
        init_relations();
        _init_done = true;
        // The following is here to avoid doing it repeatedly in repeated
        // calls to run(). Apply each "extra" relation to the first coset only
        for (auto it = _extra.cbegin(); it < _extra.cend() && !dead(); ++it) {
          trace(_id_coset, *it);  // Allow new cosets
        }
      } else {
        // This is required in case we called add_pair since the last time
        // init() was run.
        init_relations();
      }
    }

    void ToddCoxeter::init_after_prefill() {
      LIBSEMIGROUPS_ASSERT(_table.nr_cols() == nr_generators());
      LIBSEMIGROUPS_ASSERT(_table.nr_rows() > 1);
      LIBSEMIGROUPS_ASSERT(!_init_done);
      LIBSEMIGROUPS_ASSERT(_relations.empty());
      _prefilled = true;
      validate_table();
      _active   = _table.nr_rows();
      _id_coset = 0;

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

    void ToddCoxeter::init_relations() {
      switch (type()) {
        case congruence_type::LEFT:
          if (!_relations_are_reversed) {
            _relations_are_reversed = true;
            for (relation_type& rel : _extra) {
              std::reverse(rel.first.begin(), rel.first.end());
              std::reverse(rel.second.begin(), rel.second.end());
            }
            for (relation_type& rel : _relations) {
              std::reverse(rel.first.begin(), rel.first.end());
              std::reverse(rel.second.begin(), rel.second.end());
            }
          }
          break;
        case congruence_type::RIGHT:
          // intentional fall through
        case congruence_type::TWOSIDED:  // do nothing
          break;
        default:
          LIBSEMIGROUPS_ASSERT(false);
      }
    }

    void ToddCoxeter::prefill(FroidurePinBase& S) {
      LIBSEMIGROUPS_ASSERT(!_init_done);
      LIBSEMIGROUPS_ASSERT(_policy == policy::use_cayley_graph);
      LIBSEMIGROUPS_ASSERT(_table.nr_rows() == 1);
      LIBSEMIGROUPS_ASSERT(_table.nr_cols() == S.nr_generators());
      LIBSEMIGROUPS_ASSERT(S.nr_generators() == nr_generators());
      _table.add_rows(S.size());
      for (size_t i = 0; i < nr_generators(); i++) {
        _table.set(0, i, S.letter_to_pos(i) + 1);
      }
      if (type() == congruence_type::LEFT) {
        for (size_t row = 0; row < S.size(); ++row) {
          for (size_t col = 0; col < nr_generators(); ++col) {
            _table.set(row + 1, col, S.left(row, col) + 1);
          }
        }
      } else {
        for (size_t row = 0; row < S.size(); ++row) {
          for (size_t col = 0; col < nr_generators(); ++col) {
            _table.set(row + 1, col, S.right(row, col) + 1);
          }
        }
      }
      init_after_prefill();
    }

    void ToddCoxeter::use_relations_or_cayley_graph() {
      // This should not have been run before
      LIBSEMIGROUPS_ASSERT(!_init_done);
      if (has_parent_semigroup()) {
        switch (_policy) {
          case policy::none:
            _policy = policy::use_cayley_graph;
            // Intentional fall through
          case policy::use_cayley_graph:
            prefill(parent_semigroup());
#ifdef LIBSEMIGROUPS_DEBUG
            // This is a check of program logic, since we use parent() to fill
            // the table, so we only validate in debug mode.
            validate_table();
#endif
            _relations.clear();  // FIXME don't put anything into _relations in
                                 // this case
            break;
          case policy::use_relations:
            relations(
                parent_semigroup(),
                [this](word_type const& lhs, word_type const& rhs) -> void {
                  _relations.emplace_back(lhs, rhs);
                });
#ifdef LIBSEMIGROUPS_DEBUG
            // This is a check of program logic, since we use parent() to
            // obtain the relations, so we only validate in debug mode.
            for (auto const& rel : _relations) {
              validate_relation(rel);
            }
            // We don't add anything to _extra here so no need to check.
#endif
            break;
        }
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - methods (other) - private
    ////////////////////////////////////////////////////////////////////////

    // Compress the table
    void ToddCoxeter::compress() {
      if (dead() || _active == _table.nr_rows()) {
        return;
      }
      RecVec<class_index_type> table(nr_generators(), _active);

      class_index_type pos = _id_coset;
      // old number to new numbers lookup
      std::unordered_map<class_index_type, class_index_type> lookup;
      size_t                                                 next_index = 0;

      while (!dead() && pos != _next) {
        size_t curr_index;
        auto   it = lookup.find(pos);
        if (it == lookup.end()) {
          lookup.emplace(pos, next_index);
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
            lookup.emplace(val, next_index);
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
      LIBSEMIGROUPS_ASSERT(lhs != UNDEFINED);
      LIBSEMIGROUPS_ASSERT(rhs != UNDEFINED);

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
        REPORT(_defined,
               " defined, ",
               _forwd.size(),
               " max, ",
               _active,
               " active, ",
               (_defined - _active) - _cosets_killed,
               " killed, ",
               "current ",
               (add ? _current : _current_no_add));
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
}  // namespace libsemigroups
