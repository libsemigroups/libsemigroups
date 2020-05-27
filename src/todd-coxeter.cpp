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

// This file contains an implementation of the Todd-Coxeter algorithm for
// semigroups.

#include "libsemigroups/todd-coxeter.hpp"

#include <algorithm>  // for reverse
#include <chrono>     // for nanoseconds etc
#include <cstddef>    // for size_t
#include <memory>     // for shared_ptr
#include <numeric>    // for iota
#include <random>     // for mt19937
#include <string>     // for operator+, basic_string
#include <utility>    // for pair

#ifdef LIBSEMIGROUPS_DEBUG
#include <set>  // for set
#endif

#include "libsemigroups/cong-intf.hpp"          // for CongruenceInterface
#include "libsemigroups/coset.hpp"              // for CosetManager
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"       // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"       // for fpsemigroup::KnuthBendix
#include "libsemigroups/libsemigroups-config.hpp"  // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/libsemigroups-debug.hpp"   // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/obvinf.hpp"                   // for IsObviouslyInfinite
#include "libsemigroups/report.hpp"                   // for REPORT
#include "libsemigroups/stl.hpp"                      // for apply_permutation
#include "libsemigroups/tce.hpp"                      // for TCE
#include "libsemigroups/timer.hpp"                    // for detail::Timer
#include "libsemigroups/types.hpp"                    // for letter_type

// TODO(later)
//
// 1. Explore whether row-filling is useful when performing HLT. I think the
//    just means making sure that there are no undefined values in the row of
//    the current coset, this is an option from ACE.
//
// 2. Allow there to be a limit to the number of deductions that are stacked.
//    this is an option from ACE. There are 4 options as described:
//
//    https://magma.maths.usyd.edu.au/magma/handbook/text/833
//
// 3. Explore whether deductions can be useful in HLT.
//
// 4. Make make_deductions_dfs non-recursive, this will likely only be an issue
//    for presentations with extremely long relations.
//
// 5. Use path compression in _ident, or other techniques from union-find, see:
//
//      https://www.boost.org/doc/libs/1_70_0/boost/pending/disjoint_sets.hpp
//
// 6. Wreath product standardize mem fn.
//
// 7. ACE stacks deductions when processing coincidences, we don't

////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Reporting macros
////////////////////////////////////////////////////////////////////////

#define TODD_COXETER_REPORT_COSETS()                                \
  REPORT_DEFAULT("%d defined, %d max, %d active, %d killed (%s)\n", \
                 nr_cosets_defined(),                               \
                 coset_capacity(),                                  \
                 nr_cosets_active(),                                \
                 nr_cosets_killed(),                                \
                 __func__);

#define TODD_COXETER_REPORT_SWITCH(t, r)                                 \
  REPORT_VERBOSE_DEFAULT("switching %*d %s and %*d %s\n",                \
                         detail::to_string(nr_cosets_active()).length()  \
                             - detail::to_string(t).length() + 1,        \
                         t,                                              \
                         (is_active_coset(t) ? "(active)" : "(free)  "), \
                         detail::to_string(nr_cosets_active()).length()  \
                             - detail::to_string(r).length() + 1,        \
                         r,                                              \
                         (is_active_coset(r) ? "(active)" : "(free)  "));

#ifdef LIBSEMIGROUPS_DEBUG
#define TODD_COXETER_REPORT_OK() REPORT_DEBUG(" ok\n").flush_right().flush();
#else
#define TODD_COXETER_REPORT_OK()
#endif

// Helper functions
namespace {
  using class_index_type = libsemigroups::CongruenceInterface::class_index_type;
  using word_type        = libsemigroups::word_type;

  void sort_generating_pairs(std::vector<class_index_type>& perm,
                             std::vector<word_type>&        vec) {
    // Apply the permutation (adapted from
    // stl.hpp:apply_permutation)
    size_t const n = perm.size();
    for (size_t i = 0; i < n; ++i) {
      size_t current = i;
      while (i != perm[current]) {
        size_t next = perm[current];
        std::swap(vec[2 * current], vec[2 * next]);
        std::swap(vec[2 * current + 1], vec[2 * next + 1]);
        perm[current] = current;
        current       = next;
      }
      perm[current] = current;
    }
  }

  void sort_generating_pairs(
      std::function<bool(word_type const&, word_type const&)> func,
      std::vector<class_index_type>&                          perm,
      std::vector<word_type>&                                 vec) {
    // Sort each relation so that the lhs is greater than the rhs according
    // to func.
    for (auto it = vec.begin(); it < vec.end(); it += 2) {
      if (func(*it, *(it + 1))) {
        std::swap(*it, *(it + 1));
      }
    }

    // Create a permutation of the even indexed entries in vec
    perm.resize(vec.size() / 2);
    std::iota(perm.begin(), perm.end(), 0);
    std::sort(perm.begin(),
              perm.end(),
              [&func, &vec](class_index_type x, class_index_type y) -> bool {
                return func(vec[2 * x], vec[2 * y]);
              });
    sort_generating_pairs(perm, vec);
  }
}  // namespace

namespace libsemigroups {
  namespace congruence {
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////

    using coset_type  = congruence::ToddCoxeter::coset_type;
    using Coincidence = std::pair<coset_type, coset_type>;
    using Deduction   = std::pair<coset_type, letter_type>;

    ////////////////////////////////////////////////////////////////////////
    // Helper structs
    ////////////////////////////////////////////////////////////////////////

    struct StackDeductions {
      inline void operator()(std::stack<Deduction>& stck,
                             coset_type             c,
                             letter_type            a) const noexcept {
        stck.emplace(c, a);
      }
    };

    struct DoNotStackDeductions {
      inline void operator()(std::stack<Deduction>&,
                             coset_type,
                             letter_type) const noexcept {}
    };

    struct ProcessCoincidences {
      template <typename TStackDeductions>
      void operator()(congruence::ToddCoxeter* tc) const noexcept {
        tc->process_coincidences<TStackDeductions>();
      }
    };

    struct DoNotProcessCoincidences {
      template <typename TStackDeductions>
      void operator()(congruence::ToddCoxeter*) const noexcept {}
    };

    ////////////////////////////////////////////////////////////////////////
    // Helper free functions
    ////////////////////////////////////////////////////////////////////////

    static inline coset_type ff(coset_type c, coset_type d, coset_type r) {
      return (r == c ? d : (r == d ? c : r));
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - inner classes - private
    ////////////////////////////////////////////////////////////////////////

    struct ToddCoxeter::Settings {
      Settings()
          :
#ifdef LIBSEMIGROUPS_DEBUG
            enable_debug_verify_no_missing_deductions(true),
#endif
            lookahead(policy::lookahead::partial),
            lower_bound(UNDEFINED),
            next_lookahead(5000000),
            froidure_pin(policy::froidure_pin::none),
            random_interval(200000000),
            save(false),
            standardize(false),
            strategy(policy::strategy::hlt) {
      }

      Settings(Settings const& copy) = default;

#ifdef LIBSEMIGROUPS_DEBUG
      bool enable_debug_verify_no_missing_deductions;
#endif
      policy::lookahead        lookahead;
      size_t                   lower_bound;
      size_t                   next_lookahead;
      policy::froidure_pin     froidure_pin;
      std::chrono::nanoseconds random_interval;
      bool                     save;
      bool                     standardize;
      policy::strategy         strategy;
    };

    class ToddCoxeter::FelschTree {
     public:
      using index_type     = size_t;
      using state_type     = size_t;
      using const_iterator = std::vector<index_type>::const_iterator;
      static constexpr state_type initial_state = 0;
      static constexpr state_type final_state   = UNDEFINED;

      explicit FelschTree(ToddCoxeter const* tc)
          : _automata(tc->nr_generators(), 1, final_state),
            _index(1, std::vector<index_type>({})),
            _parent(1, state_type(UNDEFINED)) {}

      FelschTree(FelschTree const&) = default;

      void add_relations(std::vector<word_type> const& rels) {
        size_t nr_words = 0;
        LIBSEMIGROUPS_ASSERT(rels.size() % 2 == 0);
        for (auto const& w : rels) {
          // For every prefix [w.cbegin(), last)
          for (auto last = w.cend(); last > w.cbegin(); --last) {
            // For every suffix [first, last) of the prefix [w.cbegin(), last)
            for (auto first = w.cbegin(); first < last; ++first) {
              // Find the maximal suffix of [first, last) that corresponds to
              // an existing state . . .
              auto       it = last - 1;
              state_type s  = initial_state;
              while (_automata.get(s, *it) != final_state && it > first) {
                s = _automata.get(s, *it);
                --it;
              }
              if (_automata.get(s, *it) == final_state) {
                // [it + 1, last) is the maximal suffix of [first, last) that
                // corresponds to the existing state s
                size_t nr_states = _automata.nr_rows();
                _automata.add_rows((it + 1) - first);
                _index.resize(_index.size() + ((it + 1) - first), {});
                _parent.resize(_parent.size() + ((it + 1) - first), UNDEFINED);
                while (it >= first) {
                  // Add [it, last) as a new state
                  _automata.set(s, *it, nr_states);
                  _parent[nr_states] = s;
                  s                  = nr_states;
                  nr_states++;
                  it--;
                }
              }
            }
            // Find the state corresponding to the prefix [w.cbegin(), last)
            auto       it = last - 1;
            state_type s  = initial_state;
            while (it >= w.cbegin()) {
              s = _automata.get(s, *it);
              LIBSEMIGROUPS_ASSERT(s != final_state);
              --it;
            }
            index_type m = ((nr_words % 2) == 0 ? nr_words : nr_words - 1);
            if (!std::binary_search(_index[s].cbegin(), _index[s].cend(), m)) {
              _index[s].push_back(m);
            }
          }
          nr_words++;
        }
      }

      void push_back(letter_type x) {
        _current_state = _automata.get(initial_state, x);
      }

      bool push_front(letter_type x) {
        LIBSEMIGROUPS_ASSERT(x < _automata.nr_cols());
        if (_automata.get(_current_state, x) != final_state) {
          _current_state = _automata.get(_current_state, x);
          return true;
        } else {
          return false;
        }
      }

      void pop_front() {
        _current_state = _parent[_current_state];
      }

      const_iterator cbegin() const {
        LIBSEMIGROUPS_ASSERT(_current_state != final_state);
        return _index[_current_state].cbegin();
      }

      const_iterator cend() const {
        LIBSEMIGROUPS_ASSERT(_current_state != final_state);
        return _index[_current_state].cend();
      }

     private:
      using StateTable = detail::DynamicArray2<state_type>;
      StateTable                           _automata;
      state_type                           _current_state;
      std::vector<std::vector<index_type>> _index;
      std::vector<state_type>              _parent;
    };

    struct ToddCoxeter::TreeNode {
      TreeNode() : parent(UNDEFINED), gen(UNDEFINED) {}
      TreeNode(coset_type p, letter_type g) : parent(p), gen(g) {}
      coset_type  parent;
      letter_type gen;
    };

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - constructors and destructor - public
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeter::ToddCoxeter(congruence_type type)
        : CongruenceInterface(type),
          CosetManager(),
          _coinc(),
          _deduct(),
          _extra(),
          _felsch_tree(nullptr),
          _nr_pairs_added_earlier(0),
          _prefilled(false),
          _preim_init(0, 0, UNDEFINED),
          _preim_next(0, 0, UNDEFINED),
          _relations(),
          _settings(new Settings()),
          _standardized(order::none),
          _state(state::constructed),
          _table(0, 0, UNDEFINED),
          _tree(nullptr) {}

    ToddCoxeter::ToddCoxeter(ToddCoxeter const& copy)
        : CongruenceInterface(copy),
          CosetManager(copy),
          _coinc(copy._coinc),
          _deduct(copy._deduct),
          _extra(copy._extra),
          _felsch_tree(nullptr),
          _nr_pairs_added_earlier(copy._nr_pairs_added_earlier),
          _prefilled(copy._prefilled),
          _preim_init(copy._preim_init),
          _preim_next(copy._preim_next),
          _relations(copy._relations),
          _settings(detail::make_unique<Settings>(*copy._settings)),
          _standardized(copy._standardized),
          _state(copy._state),
          _table(copy._table),
          _tree(nullptr) {
      if (copy._felsch_tree != nullptr) {
        _felsch_tree = detail::make_unique<FelschTree>(*copy._felsch_tree);
      }
      if (copy._tree != nullptr) {
        _tree = detail::make_unique<Tree>(*copy._tree);
      }
    }

    ToddCoxeter::ToddCoxeter(congruence_type                  type,
                             std::shared_ptr<FroidurePinBase> S,
                             policy::froidure_pin             p)
        : ToddCoxeter(type) {
      _settings->froidure_pin = p;
      set_parent_froidure_pin(S);
      set_nr_generators(S->nr_generators());
    }

    // Construct a ToddCoxeter object representing a congruence over the
    // semigroup defined by copy (the quotient that is).
    ToddCoxeter::ToddCoxeter(congruence_type typ, ToddCoxeter& copy)
        : ToddCoxeter(typ) {
      if (copy.kind() != congruence_type::twosided && typ != copy.kind()) {
        LIBSEMIGROUPS_EXCEPTION("incompatible types of congruence, found ("
                                + congruence_type_to_string(copy.kind()) + " / "
                                + congruence_type_to_string(typ)
                                + ") but only (left / left), (right / "
                                  "right), (two-sided / *) are valid");
      }
      copy_relations_for_quotient(copy);
    }

    ToddCoxeter::ToddCoxeter(congruence_type           typ,
                             fpsemigroup::ToddCoxeter& copy)
        : ToddCoxeter(typ) {
      set_parent_froidure_pin(copy);
      if (copy.finished()) {
        set_nr_generators(copy.froidure_pin()->nr_generators());
        _settings->froidure_pin = policy::froidure_pin::use_cayley_graph;

      } else {
        copy_relations_for_quotient(copy.congruence());
        _settings->froidure_pin = policy::froidure_pin::use_relations;
      }
    }

    ToddCoxeter::ToddCoxeter(congruence_type typ, fpsemigroup::KnuthBendix& kb)
        : ToddCoxeter(typ) {
      set_nr_generators(kb.alphabet().size());
      // TODO(later) use active rules when available
      for (auto it = kb.cbegin_rules(); it < kb.cend_rules(); ++it) {
        add_pair(kb.string_to_word(it->first), kb.string_to_word(it->second));
      }
      // FIXME something goes horribly wrong if the next line is above the for
      // loop above.
      set_parent_froidure_pin(kb);
      if (kb.finished() && kb.is_obviously_finite()) {
        LIBSEMIGROUPS_ASSERT(_settings->froidure_pin
                             == policy::froidure_pin::none);
        _settings->froidure_pin = policy::froidure_pin::use_cayley_graph;
      }
    }

    ToddCoxeter::~ToddCoxeter() = default;

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

    bool ToddCoxeter::contains(word_type const& lhs, word_type const& rhs) {
      validate_word(lhs);
      validate_word(rhs);
      init();
      if (!_prefilled && _relations.empty() && _extra.empty()) {
        // This defines the free semigroup
        return lhs == rhs;
      }
      return CongruenceInterface::contains(lhs, rhs);
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (init + settings) - public
    ////////////////////////////////////////////////////////////////////////

    // Init
    void ToddCoxeter::prefill(table_type const& table) {
      prefill_and_validate(table, true);
      init_preimages_from_table();
    }

    // Settings
    ToddCoxeter&
    ToddCoxeter::froidure_pin_policy(policy::froidure_pin x) noexcept {
      _settings->froidure_pin = x;
      return *this;
    }

    ToddCoxeter::policy::froidure_pin ToddCoxeter::froidure_pin_policy() const
        noexcept {
      return _settings->froidure_pin;
    }

    ToddCoxeter& ToddCoxeter::lookahead(policy::lookahead x) noexcept {
      _settings->lookahead = x;
      return *this;
    }

    ToddCoxeter& ToddCoxeter::lower_bound(size_t n) noexcept {
      _settings->lower_bound = n;
      return *this;
    }

    ToddCoxeter& ToddCoxeter::next_lookahead(size_t n) noexcept {
      _settings->next_lookahead = n;
      return *this;
    }

    ToddCoxeter& ToddCoxeter::standardize(bool x) noexcept {
      _settings->standardize = x;
      return *this;
    }

    ToddCoxeter& ToddCoxeter::save(bool x) {
      if ((_prefilled
           || (has_parent_froidure_pin()
               && parent_froidure_pin()->is_finite() == tril::TRUE
               && (_settings->froidure_pin == policy::froidure_pin::none
                   || _settings->froidure_pin
                          == policy::froidure_pin::use_cayley_graph)))
          && x) {
        LIBSEMIGROUPS_EXCEPTION("cannot use the save setting with a "
                                "prefilled ToddCoxeter instance");
      }
      _settings->save = x;
      return *this;
    }

    ToddCoxeter& ToddCoxeter::strategy(policy::strategy x) {
      if ((_prefilled
           || (has_parent_froidure_pin()
               && parent_froidure_pin()->is_finite() == tril::TRUE
               && (_settings->froidure_pin == policy::froidure_pin::none
                   || _settings->froidure_pin
                          == policy::froidure_pin::use_cayley_graph)))
          && x == policy::strategy::felsch) {
        LIBSEMIGROUPS_EXCEPTION("cannot use the Felsch strategy with a "
                                "prefilled ToddCoxeter instance");
      }
      _settings->strategy = x;
      return *this;
    }

    ToddCoxeter::policy::strategy ToddCoxeter::strategy() const noexcept {
      return _settings->strategy;
    }

    ToddCoxeter&
    ToddCoxeter::random_interval(std::chrono::nanoseconds x) noexcept {
      _settings->random_interval = x;
      return *this;
    }

    ToddCoxeter& ToddCoxeter::sort_generating_pairs(
        std::function<bool(word_type const&, word_type const&)> func) {
      if (started()) {
        LIBSEMIGROUPS_EXCEPTION(
            "Cannot sort relations, the coset enumeration has started!")
      }
      init();
      std::vector<class_index_type> perm;
      ::sort_generating_pairs(func, perm, _relations);
      ::sort_generating_pairs(func, perm, _extra);
      return *this;
    }

    ToddCoxeter& ToddCoxeter::random_shuffle_generating_pairs() {
      if (started()) {
        LIBSEMIGROUPS_EXCEPTION(
            "Cannot shuffle relations, the coset enumeration has started!")
      }
      init();
      std::vector<class_index_type> perm(0, _relations.size());
      std::iota(perm.begin(), perm.end(), 0);
      std::random_device rd;
      std::mt19937       g(rd());
      std::shuffle(perm.begin(), perm.end(), g);
      ::sort_generating_pairs(perm, _relations);
      ::sort_generating_pairs(perm, _extra);
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (container-like) - public
    ////////////////////////////////////////////////////////////////////////

    bool ToddCoxeter::empty() const {
      return _relations.empty() && _extra.empty() && nr_cosets_active() == 1;
    }

    void ToddCoxeter::reserve(size_t n) {
      size_t m = coset_capacity();
      if (n > m) {
        m = n - m;
        _table.add_rows(m);
        _preim_init.add_rows(m);
        _preim_next.add_rows(m);
        add_free_cosets(m);
      }
    }

    void ToddCoxeter::shrink_to_fit() {
      if (!finished()) {
        return;
      }
      if (!is_standardized()) {
        standardize(order::shortlex);
      }

      _table.shrink_rows_to(nr_cosets_active());
      // Cannot delete _preim_init or _preim_next because they are required by
      // standardize
      _preim_init.shrink_rows_to(nr_cosets_active());
      _preim_next.shrink_rows_to(nr_cosets_active());
      _relations.clear();
      _relations.shrink_to_fit();
      _extra.clear();
      _extra.shrink_to_fit();
      erase_free_cosets();
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (state) - public
    ////////////////////////////////////////////////////////////////////////

    bool ToddCoxeter::complete() const noexcept {
      size_t const n = nr_generators();
      coset_type   c = _id_coset;
      while (c != first_free_coset()) {
        for (size_t a = 0; a < n; ++a) {
          if (_table.get(c, a) == UNDEFINED) {
            return false;
          }
        }
        c = next_active_coset(c);
      }
      return true;
    }

    bool ToddCoxeter::compatible() const noexcept {
      coset_type c = _id_coset;
      while (c != first_free_coset()) {
        for (auto it = _relations.cbegin(); it < _relations.cend(); it += 2) {
          coset_type x = tau(c, (*it).cbegin(), (*it).cend());
          LIBSEMIGROUPS_ASSERT(is_active_coset(x) || x == UNDEFINED);
          coset_type y = tau(c, (*(it + 1)).cbegin(), (*(it + 1)).cend());
          LIBSEMIGROUPS_ASSERT(is_active_coset(y) || y == UNDEFINED);
          if (x != y) {
            return false;
          }
        }
        c = next_active_coset(c);
      }
      return true;
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (standardization) - public
    ////////////////////////////////////////////////////////////////////////

    bool ToddCoxeter::is_standardized() const noexcept {
      return _standardized != order::none;
    }

    void ToddCoxeter::standardize(order rdr) {
      if (_standardized == rdr || empty()) {
        return;
      }
      switch (rdr) {
        case order::shortlex:
          init_standardize();
          shortlex_standardize();
          break;
        case order::lex:
          init_standardize();
          lex_standardize();
          break;
        case order::recursive:
          init_standardize();
          recursive_standardize();
          break;
        case order::none: {
        }
      }
      if (finished()) {
        _standardized = rdr;
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    word_type ToddCoxeter::class_index_to_word_impl(coset_type i) {
      run();
      if (!is_standardized()) {
        standardize(order::shortlex);
      }
      LIBSEMIGROUPS_ASSERT(finished());
      word_type w;
      TreeNode  tn = (*_tree)[i + 1];
      while (tn.parent != UNDEFINED) {
        w.push_back(tn.gen);
        tn = (*_tree)[tn.parent];
      }
      if (kind() != congruence_type::left) {
        std::reverse(w.begin(), w.end());
      }
      return w;
    }

    size_t ToddCoxeter::nr_classes_impl() {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      return nr_cosets_active() - 1;
    }

    std::shared_ptr<FroidurePinBase> ToddCoxeter::quotient_impl() {
      using detail::TCE;
      run();
      standardize(order::shortlex);
      shrink_to_fit();
      // Ensure class indices and letters are equal!
      auto   table = std::make_shared<table_type>(_table);
      size_t n     = nr_generators();
      for (letter_type a = 0; a < n;) {
        if (table->get(0, a) != a + 1) {
          table->erase_column(a);
          n--;
        } else {
          ++a;
        }
      }
      auto ptr = std::make_shared<
          FroidurePin<TCE, FroidurePinTraits<TCE, table_type>>>(table);
      for (size_t i = 0; i < nr_generators(); ++i) {
        // We use table.get(0, i) instead of just i, because there might be
        // more generators than cosets.
        ptr->add_generator(TCE(_table.get(0, i)));
      }
      return ptr;
    }

    void ToddCoxeter::run_impl() {
      if (is_quotient_obviously_infinite()) {
        LIBSEMIGROUPS_EXCEPTION(
            "there are infinitely many classes in the congruence and "
            "Todd-Coxeter will never terminate");
      }
      if (_settings->lower_bound != UNDEFINED) {
        size_t const bound     = _settings->lower_bound;
        _settings->lower_bound = UNDEFINED;
        run_until([this, &bound]() -> bool {
          return (nr_cosets_active() == bound) && complete();
        });
      } else if (_settings->strategy == policy::strategy::felsch) {
        felsch();
      } else if (_settings->strategy == policy::strategy::hlt) {
        hlt();
      } else if (_settings->strategy == policy::strategy::random) {
        sims();
      }
    }

    bool ToddCoxeter::finished_impl() const {
      return _state == state::finished;
    }

    coset_type ToddCoxeter::word_to_class_index_impl(word_type const& w) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      if (!is_standardized()) {
        standardize(order::shortlex);
      }
      coset_type c = const_word_to_class_index(w);
      // c is in the range 1, ..., nr_cosets_active() because 0 represents the
      // identity coset, and does not correspond to an element.
      return c;
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    coset_type
    ToddCoxeter::const_word_to_class_index(word_type const& w) const {
      validate_word(w);
      coset_type c = _id_coset;

      if (kind() == congruence_type::left) {
        c = tau(c, w.crbegin(), w.crend());
      } else {
        c = tau(c, w.cbegin(), w.cend());
      }
      return (c == UNDEFINED ? c : c - 1);
    }

    bool ToddCoxeter::is_quotient_obviously_finite_impl() {
      if (finished()) {
        return true;
      }
      init();
      return _prefilled;
      // _prefilled means that either we were created from a FroidurePinBase*
      // with _settings->froidure_pin = use_cayley_graph and we successfully
      // prefilled the table, or we manually prefilled the table.  In this case
      // the semigroup defined by _relations must be finite.
    }

    bool ToddCoxeter::is_quotient_obviously_infinite_impl() {
      if (finished()) {
        return false;
      }
      init();
      if (_prefilled) {
        return false;
      } else if (nr_generators() > _relations.size() + _extra.size()) {
        return true;
      }
      detail::IsObviouslyInfinite<letter_type, word_type> ioi(nr_generators());
      ioi.add_rules(_relations.cbegin(), _relations.cend());
      ioi.add_rules(_extra.cbegin(), _extra.cend());
      return ioi.result();
    }

    void ToddCoxeter::set_nr_generators_impl(size_t n) {
      // TODO(later) add columns to make it up to n?
      _preim_init = table_type(n, 1, UNDEFINED);
      _preim_next = table_type(n, 1, UNDEFINED);
      _table      = table_type(n, 1, UNDEFINED);
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (validation) - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::validate_table(table_type const& table,
                                     size_t const      first,
                                     size_t const      last) const {
      REPORT_DEBUG_DEFAULT("validating coset table...\n");
      if (nr_generators() == UNDEFINED) {
        LIBSEMIGROUPS_EXCEPTION("no generators have been defined");
      } else if (table.nr_cols() != nr_generators()) {
        LIBSEMIGROUPS_EXCEPTION("invalid table, expected %d columns, found %d",
                                nr_generators(),
                                table.nr_cols());
      }
      if (last - first <= 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid table, expected at least 1 rows, found %d",
            table.nr_rows());
      }
      for (size_t i = first; i < last; ++i) {
        for (size_t j = 0; j < table.nr_cols(); ++j) {
          coset_type c = table.get(i, j);
          if (c < first || c >= last) {
            LIBSEMIGROUPS_EXCEPTION(
                "invalid table, expected entries in the range [%d, %d), found "
                "%d in row %d, column %d",
                i,
                j,
                first,
                last,
                c);
          }
        }
      }
      REPORT_DEBUG_DEFAULT("...coset table ok\n");
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (initialisation) - private
    ////////////////////////////////////////////////////////////////////////

    //! Copy all _relations and _extra from copy into _relations of this
    void ToddCoxeter::copy_relations_for_quotient(ToddCoxeter& copy) {
      REPORT_DEBUG_DEFAULT("copying relations for quotient...\n");
      LIBSEMIGROUPS_ASSERT(empty());
      copy.init();
      set_nr_generators(copy.nr_generators());
      _state     = state::initialized;
      _relations = copy._relations;
      _relations.insert(
          _relations.end(), copy._extra.cbegin(), copy._extra.cend());
      if (kind() == congruence_type::left
          && copy.kind() != congruence_type::left) {
        for (auto& w : _relations) {
          std::reverse(w.begin(), w.end());
        }
      }
      _nr_pairs_added_earlier = 0;
    }

    void ToddCoxeter::init() {
      if (_state == state::constructed) {
        REPORT_DEBUG_DEFAULT("initializing...\n");
        // Add the relations/Cayley graph from parent() if any.
        if (has_parent_froidure_pin()
            && parent_froidure_pin()->is_finite() == tril::TRUE) {
          if (_settings->froidure_pin == policy::froidure_pin::use_cayley_graph
              || _settings->froidure_pin == policy::froidure_pin::none) {
            REPORT_DEBUG_DEFAULT("using Cayley graph...\n");
            LIBSEMIGROUPS_ASSERT(_relations.empty());
            prefill(*parent_froidure_pin());
#ifdef LIBSEMIGROUPS_DEBUG
            // This is a check of program logic, since we use parent() to fill
            // the table, so we only validate in debug mode.
            validate_table(_table, 1, parent_froidure_pin()->size() + 1);
#endif
          } else {
            REPORT_DEBUG_DEFAULT("using presentation...\n");
            LIBSEMIGROUPS_ASSERT(_settings->froidure_pin
                                 == policy::froidure_pin::use_relations);
            auto fp = parent_froidure_pin();
            fp->run();
            for (auto it = fp->cbegin_rules(); it != fp->cend_rules(); ++it) {
              reverse_if_necessary_and_push_back(it->first, _relations);
              reverse_if_necessary_and_push_back(it->second, _relations);
            }
#ifdef LIBSEMIGROUPS_DEBUG
            // This is a check of program logic, since we use parent() to
            // obtain the relations, so we only validate in debug mode.
            for (auto const& rel : _relations) {
              validate_word(rel);
            }
            // We don't add anything to _extra here so no need to check.
#endif
          }
        }
        _state = state::initialized;
      }

      // Get new generating pairs (if any) from CongruenceInterface (if any)
      auto it = cbegin_generating_pairs() + _nr_pairs_added_earlier;
      if (kind() != congruence_type::twosided) {
        for (; it < cend_generating_pairs(); ++it) {
          reverse_if_necessary_and_push_back(it->first, _extra);
          reverse_if_necessary_and_push_back(it->second, _extra);
        }
      } else {
        for (; it < cend_generating_pairs(); ++it) {
          reverse_if_necessary_and_push_back(it->first, _relations);
          reverse_if_necessary_and_push_back(it->second, _relations);
        }
      }
      _nr_pairs_added_earlier
          = cend_generating_pairs() - cbegin_generating_pairs();
    }

    void ToddCoxeter::init_felsch_tree() {
      LIBSEMIGROUPS_ASSERT(_state >= state::initialized);
      if (_felsch_tree == nullptr) {
        REPORT_DEFAULT("initializing the Felsch tree...\n");
        detail::Timer tmr;
        _felsch_tree = detail::make_unique<FelschTree>(this);
        _felsch_tree->add_relations(_relations);
        REPORT_TIME(tmr);
      }
    }

    void ToddCoxeter::init_preimages_from_table() {
      REPORT_DEBUG("initializing preimages...\n");
      LIBSEMIGROUPS_ASSERT(_table.nr_cols() == nr_generators());
      LIBSEMIGROUPS_ASSERT(_table.nr_rows() >= nr_cosets_active());
      LIBSEMIGROUPS_ASSERT(_prefilled);
      LIBSEMIGROUPS_ASSERT(_state == state::constructed);
      LIBSEMIGROUPS_ASSERT(_relations.empty());

      for (coset_type c = 0; c < nr_cosets_active(); c++) {
        for (size_t i = 0; i < nr_generators(); i++) {
          coset_type b = _table.get(c, i);
          _preim_next.set(c, i, _preim_init.get(b, i));
          _preim_init.set(b, i, c);
        }
      }
    }

    void ToddCoxeter::prefill(FroidurePinBase& S) {
      REPORT_DEBUG_DEFAULT("prefilling the coset table from FroidurePin...\n");
      LIBSEMIGROUPS_ASSERT(_state == state::constructed);
      LIBSEMIGROUPS_ASSERT(
          _settings->froidure_pin == policy::froidure_pin::use_cayley_graph
          || _settings->froidure_pin == policy::froidure_pin::none);
      LIBSEMIGROUPS_ASSERT(S.nr_generators() == nr_generators());
      if (kind() == congruence_type::left) {
        prefill_and_validate(S.left_cayley_graph(), false);
      } else {
        prefill_and_validate(S.right_cayley_graph(), false);
      }
      for (size_t i = 0; i < nr_generators(); i++) {
        _table.set(0, i, S.letter_to_pos(i) + 1);
      }
      init_preimages_from_table();
    }

    void ToddCoxeter::prefill_and_validate(table_type const& table,
                                           bool              validate) {
      if (_settings->strategy == policy::strategy::felsch) {
        LIBSEMIGROUPS_EXCEPTION(
            "it is not possible to prefill when using the Felsch strategy");
      }
      if (!empty()) {
        LIBSEMIGROUPS_EXCEPTION("cannot prefill a non-empty instance")
      }
      if (validate) {
        validate_table(table, 0, table.nr_rows());
      }

      REPORT_DEBUG("prefilling the coset table...\n");
      _prefilled = true;
      size_t m   = table.nr_rows() + 1;
      _table.add_rows(m - _table.nr_rows());
      for (size_t i = 0; i < _table.nr_cols(); i++) {
        _table.set(0, i, i + 1);
      }
      for (size_t row = 0; row < _table.nr_rows() - 1; ++row) {
        for (size_t col = 0; col < _table.nr_cols(); ++col) {
          _table.set(row + 1, col, table.get(row, col) + 1);
        }
      }
      add_active_cosets(m - nr_cosets_active());
      _preim_init.add_rows(m - _preim_init.nr_rows());
      _preim_next.add_rows(m - _preim_next.nr_rows());
    }

    void
    ToddCoxeter::reverse_if_necessary_and_push_back(word_type               w,
                                                    std::vector<word_type>& v) {
      if (kind() == congruence_type::left) {
        std::reverse(w.begin(), w.end());
      }
      v.push_back(std::move(w));
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (cosets) - private
    ////////////////////////////////////////////////////////////////////////

    coset_type ToddCoxeter::new_coset() {
      if (!has_free_cosets()) {
        reserve(2 * coset_capacity());
        return new_active_coset();
      } else {
        coset_type const c = new_active_coset();
        // Clear the new coset's row in each table
        for (letter_type i = 0; i < nr_generators(); i++) {
          _table.set(c, i, UNDEFINED);
          _preim_init.set(c, i, UNDEFINED);
        }
        return c;
      }
    }

    void ToddCoxeter::remove_preimage(coset_type const  cx,
                                      letter_type const x,
                                      coset_type const  d) {
      LIBSEMIGROUPS_ASSERT(is_active_coset(cx));
      LIBSEMIGROUPS_ASSERT(is_valid_coset(d));
      coset_type e = _preim_init.get(cx, x);
      if (e == d) {
        _preim_init.set(cx, x, _preim_next.get(d, x));
      } else {
        while (_preim_next.get(e, x) != d) {
          e = _preim_next.get(e, x);
        }
        LIBSEMIGROUPS_ASSERT(_preim_next.get(e, x) == d);
        _preim_next.set(e, x, _preim_next.get(d, x));
      }
    }

    // Perform a DFS in _felsch_tree
    void ToddCoxeter::make_deductions_dfs(coset_type const c) {
      for (auto it = _felsch_tree->cbegin(); it < _felsch_tree->cend(); ++it) {
        push_definition_felsch<StackDeductions, DoNotProcessCoincidences>(
            c, _relations[*it], _relations[*it + 1]);
      }

      size_t const n = nr_generators();
      for (size_t x = 0; x < n; ++x) {
        if (_felsch_tree->push_front(x)) {
          coset_type e = _preim_init.get(c, x);
          while (e != UNDEFINED) {
            make_deductions_dfs(e);
            e = _preim_next.get(e, x);
          }
          _felsch_tree->pop_front();
        }
      }
    }

    void ToddCoxeter::process_deductions() {
      LIBSEMIGROUPS_ASSERT(!_prefilled);
#ifdef LIBSEMIGROUPS_VERBOSE
      if (!_deduct.empty()) {
        REPORT_VERBOSE_DEFAULT("processing %llu deductions . . .\n",
                               _deduct.size());
      }
#endif
      while (!_deduct.empty()) {
        auto d = _deduct.top();
        _deduct.pop();
        if (is_active_coset(d.first)) {
          _felsch_tree->push_back(d.second);
          make_deductions_dfs(d.first);
          process_coincidences<StackDeductions>();
        }
      }
      process_coincidences<StackDeductions>();
      if (!_deduct.empty()) {
        process_deductions();
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (main strategies) - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::felsch() {
      REPORT_DEFAULT("performing Felsch %s standardization...\n",
                     _settings->standardize ? "with" : "without");
      detail::Timer tmr;
      init();
      coset_type   t = 0;
      size_t const n = nr_generators();
      // Can only initialise _felsch_tree here because we require _relations
      // to be complete.
      init_felsch_tree();
      if (_state == state::initialized) {
        LIBSEMIGROUPS_ASSERT(_settings->strategy == policy::strategy::felsch);
        for (auto it = _extra.cbegin(); it < _extra.cend(); it += 2) {
          push_definition_hlt<StackDeductions, ProcessCoincidences>(
              _id_coset, *it, *(it + 1));
        }
        if (_settings->standardize) {
          for (letter_type x = 0; x < n; ++x) {
            standardize_immediate(_id_coset, t, x);
          }
        }
        if (!_prefilled) {
          if (_relations.empty()) {
            _felsch_tree->add_relations(_extra);
            _extra.swap(_relations);
          }
          process_deductions();
          // process_deductions doesn't work if the table is prefilled
        }
      } else if (_state == state::hlt) {
        _current = _id_coset;
      }
      _state = state::felsch;
      while (_current != first_free_coset() && !stopped()) {
        for (letter_type a = 0; a < n; ++a) {
          if (_table.get(_current, a) == UNDEFINED) {
            define<StackDeductions>(_current, a, new_coset());
            process_deductions();
#ifdef LIBSEMIGROUPS_DEBUG
            if (_settings->enable_debug_verify_no_missing_deductions) {
              debug_verify_no_missing_deductions();
            }
#endif
          }
          if (_settings->standardize) {
            standardize_immediate(_current, t, a);
          }
        }
        if (report()) {
          TODD_COXETER_REPORT_COSETS()
        }
        _current = next_active_coset(_current);
      }
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      LIBSEMIGROUPS_ASSERT(_deduct.empty());
      if (!stopped()) {
        LIBSEMIGROUPS_ASSERT(_current == first_free_coset());
        _state = state::finished;
      }
      TODD_COXETER_REPORT_COSETS()
      REPORT_TIME(tmr);
      report_why_we_stopped();
    }

    // Walker's Strategy 1 = HLT = ACE style-R
    void ToddCoxeter::hlt() {
      REPORT_DEFAULT("performing HLT %s standardization, %s lookahead, and%s"
                     "deduction processing...\n",
                     _settings->standardize ? "with" : "without",
                     _settings->lookahead == policy::lookahead::partial
                         ? "partial"
                         : "full",
                     _settings->save ? " " : " no ");
      detail::Timer tmr;
      init();

      coset_type t = 0;
      if (_state == state::initialized) {
        for (auto it = _extra.cbegin(); it < _extra.cend(); it += 2) {
          push_definition_hlt<DoNotStackDeductions, ProcessCoincidences>(
              _id_coset, *it, *(it + 1));
        }
        if (_settings->standardize) {
          size_t const n = nr_generators();
          for (letter_type x = 0; x < n; ++x) {
            standardize_immediate(_id_coset, t, x);
          }
        }
        if (!_prefilled) {
          if (_relations.empty()) {
            _extra.swap(_relations);
          }
        }
      } else if (_state == state::felsch) {
        _current = _id_coset;
      }
      _state = state::hlt;

      if (_settings->save) {
        init_felsch_tree();
      }
      // size_t const n = nr_generators();
      while (_current != first_free_coset() && !stopped()) {
        if (!_settings->save) {
          for (auto it = _relations.cbegin(); it < _relations.cend(); it += 2) {
            push_definition_hlt<DoNotStackDeductions, ProcessCoincidences>(
                _current, *it, *(it + 1));
          }
          // Row filling
          // for (letter_type x = 0; x < n; ++x) {
          //   if (tau(_current, x) == UNDEFINED) {
          //     define<DoNotStackDeductions>(_current, x, new_coset());
          //   }
          // }
        } else {
          for (auto it = _relations.cbegin(); it < _relations.cend(); it += 2) {
            push_definition_hlt<StackDeductions, DoNotProcessCoincidences>(
                _current, *it, *(it + 1));
            process_deductions();
          }
          // Row filling
          // for (letter_type x = 0; x < n; ++x) {
          //   if (tau(_current, x) == UNDEFINED) {
          //     define<StackDeductions>(_current, x, new_coset());
          //   }
          // }
        }
        if (nr_cosets_active() > _settings->next_lookahead) {
          perform_lookahead();
        }
        if (_settings->standardize) {
          size_t const n = nr_generators();
          for (letter_type x = 0; x < n; ++x) {
            standardize_immediate(_current, t, x);
          }
        }
        if (report()) {
          TODD_COXETER_REPORT_COSETS()
        }
        _current = next_active_coset(_current);
      }
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      LIBSEMIGROUPS_ASSERT(_deduct.empty());
      if (!stopped()) {
        LIBSEMIGROUPS_ASSERT(_current == first_free_coset());
        _state = state::finished;
      }
      TODD_COXETER_REPORT_COSETS();
      REPORT_TIME(tmr);
      report_why_we_stopped();
    }

    // This is not exactly Sim's TEN_CE, since all of the variants of
    // Todd-Coxeter represented in TEN_CE (that apply to semigroups/monoids)
    // are already accounted for in the above.
    void ToddCoxeter::sims() {
      REPORT_DEFAULT("performing random Sims' TEN_CE strategy...\n");
      using int_dist_type
          = std::uniform_int_distribution<std::mt19937::result_type>;
      static int_dist_type dist(0, 9);
      static std::mt19937  mt;

      static constexpr std::array<bool, 8> full
          = {true, true, true, true, false, false, false, false};
      static constexpr std::array<bool, 10> stand
          = {true, true, false, false, true, true, false, false, true, false};
      static constexpr std::array<bool, 8> save_it
          = {true, false, true, false, true, false, true, false};

      static const std::string line = std::string(79, '#') + '\n';
#ifdef LIBSEMIGROUPS_DEBUG
      // Don't check for missing deductions, because when changing from HLT to
      // Felsch and back repeatedly, there can be missing deductions after
      // deduction processing in Felsch (caused by not pushing all relations
      // through all cosets in HLT).
      _settings->enable_debug_verify_no_missing_deductions = false;
#endif
      while (!finished()) {
        size_t m = dist(mt);
        if (m < 8) {
          strategy(policy::strategy::hlt);
          lookahead(
              (full[m] ? policy::lookahead::full : policy::lookahead::partial));
          try {
            save(save_it[m]);
          } catch (...) {
            // It isn't always possible to use the save option (when this is
            // created from a Cayley table, for instance), and
            // ToddCoxeter::save throws if this is the case.
          }
        } else {
          try {
            strategy(policy::strategy::felsch);
          } catch (...) {
            // It isn't always possible to use the Felsch strategy (when this
            // is created from a Cayley table, for instance), and
            // ToddCoxeter::save throws if this is the case.
          }
        }
        standardize(stand[m]);

        REPORT(line).prefix().color(fmt::color::dim_gray).flush();
        // Second param, means don't lock, since we already locked in run_impl
        // above.
        run_for(_settings->random_interval);
      }
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      LIBSEMIGROUPS_ASSERT(_deduct.empty());
      // The next 2 lines are necessary because if we are changing from HLT to
      // Felsch repeatedly, we can be in the situation where the table is
      // complete but not compatible. Test [ToddCoxeter][048] is a good example
      // of this.
      lookahead(policy::lookahead::full);
      perform_lookahead();
    }

    // TODO(later) we could use deduction processing here instead of this,
    // where appropriate?
    void ToddCoxeter::perform_lookahead() {
      state const old_state = _state;
      _state                = state::lookahead;
      if (_settings->lookahead == policy::lookahead::partial) {
        REPORT_DEFAULT("performing partial lookahead...\n");
        // Start lookahead from the coset after _current
        _current_la = next_active_coset(_current);
      } else {
        LIBSEMIGROUPS_ASSERT(_settings->lookahead == policy::lookahead::full);
        REPORT_DEFAULT("performing full lookahead...\n");
        // Start from the first coset
        _current_la = _id_coset;
      }
      TODD_COXETER_REPORT_COSETS()

      size_t nr_killed = nr_cosets_killed();
      while (_current_la != first_free_coset()
             // when running the random sims method the state is finished at
             // this point, and so stopped() == true, but we anyway want to
             // perform a full lookahead, which is why "_state ==
             // state::finished" is in the next line.
             && (old_state == state::finished || !stopped())) {
        for (auto it = _relations.cbegin(); it < _relations.cend(); it += 2) {
          push_definition_felsch<DoNotStackDeductions, ProcessCoincidences>(
              _current_la, *it, *(it + 1));
        }
        _current_la = next_active_coset(_current_la);
        if (report()) {
          TODD_COXETER_REPORT_COSETS()
        }
      }
      nr_killed = nr_cosets_killed() - nr_killed;
      if (nr_cosets_active() > _settings->next_lookahead
          || nr_killed < (nr_cosets_active() / 4)) {
        _settings->next_lookahead *= 2;
      }
      REPORT_DEFAULT("%2d cosets killed\n", nr_killed);
      _state = old_state;
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (standardize) - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::init_standardize() {
      if (_tree == nullptr) {
        _tree = detail::make_unique<std::vector<TreeNode>>(nr_cosets_active(),
                                                           TreeNode());
      } else {
        _tree->resize(nr_cosets_active());
      }
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
    }

    // Returns true if t is incremented (i.e. it's the first time we are
    // seeing and t as a coset in a standardization) and false otherwise.
    bool ToddCoxeter::standardize_immediate(coset_type const  s,
                                            coset_type&       t,
                                            letter_type const x) {
      LIBSEMIGROUPS_ASSERT(!finished());
      coset_type const r = _table.get(s, x);
      if (r != UNDEFINED) {
        if (r > t) {
          t++;
          if (r > t) {
            swap(t, r);
          }
          return true;
        }
      }
      return false;
    }

    bool ToddCoxeter::standardize_deferred(std::vector<coset_type>& p,
                                           std::vector<coset_type>& q,
                                           coset_type const         s,
                                           coset_type&              t,
                                           letter_type const        x) {
      // p : new -> old and q : old -> new
      coset_type r = _table.get(p[s], x);
      if (r != UNDEFINED) {
        r = q[r];  // new
        if (r > t) {
          t++;
          if (r > t) {
            TODD_COXETER_REPORT_SWITCH(r, t);
            std::swap(p[t], p[r]);
            std::swap(q[p[t]], q[p[r]]);
          }
          (*_tree)[t].parent = (s == t ? r : s);
          (*_tree)[t].gen    = x;
          return true;
        }
      }
      return false;
    }

    void ToddCoxeter::lex_standardize() {
      REPORT_DEFAULT("standardizing (lex)... ");
      detail::Timer tmr;

      coset_type              s = 0;
      coset_type              t = 0;
      letter_type             x = 0;
      size_t const            n = nr_generators();
      std::vector<coset_type> p(coset_capacity(), 0);
      std::iota(p.begin(), p.end(), 0);
      std::vector<coset_type> q(coset_capacity(), 0);
      std::iota(q.begin(), q.end(), 0);

      // Perform a DFS through the _table
      while (s <= t) {
        if (standardize_deferred(p, q, s, t, x)) {
          s = t;
          x = 0;
          continue;
        }
        x++;
        if (x == n) {  // backtrack
          x = (*_tree)[s].gen;
          s = (*_tree)[s].parent;
        }
      }
      apply_permutation(p, q);

      REPORT("%d\n", tmr.string()).prefix().flush_right().flush();
#ifdef LIBSEMIGROUPS_DEBUG
      debug_validate_forwd_bckwd();
      debug_validate_table();
      // debug_validate_preimages();
#endif
    }

    void ToddCoxeter::shortlex_standardize() {
      REPORT_DEFAULT("standardizing (shortlex)... ");
      detail::Timer           tmr;
      coset_type              t = 0;
      size_t const            n = nr_generators();
      std::vector<coset_type> p(coset_capacity(), 0);
      std::iota(p.begin(), p.end(), 0);
      std::vector<coset_type> q(coset_capacity(), 0);
      std::iota(q.begin(), q.end(), 0);

      for (coset_type s = 0; s <= t; ++s) {
        for (letter_type x = 0; x < n; ++x) {
          standardize_deferred(p, q, s, t, x);
        }
      }
      apply_permutation(p, q);
      REPORT("%s\n", tmr.string()).prefix().flush_right().flush();
#ifdef LIBSEMIGROUPS_DEBUG
      debug_validate_forwd_bckwd();
      debug_validate_table();
      // debug_validate_preimages();
#endif
    }

    // This is how the recursive words up to a given length M, and on an
    // arbitrary finite alphabet are generated.  On a single letter alphabet,
    // this order is just increasing powers of the only generator:
    //
    //   a < aa < aaa < aaaa < ... < aa...a (M times)
    //
    // With an n-letter alphabet A = {a_1, a_2, ..., a_n}, suppose we have
    // already obtained all of the words W_{n - 1} containing {a_1, ..., a_{n -
    // 1}}.  Every word in W_{n - 1} is less than any word containing a_n, and
    // the least word greater than every word in W_{n - 1} is a_n. Words greater
    // than a_n are obtain in the follow way, where:
    //
    // x: is the maximum word in W_{n - 1}, this is constant, in the description
    //    that follows.
    // u: the first word obtained in point (1), the first time it is applied
    //    after (2) has been applied, starting with u = a_{n - 1}.
    // v: a word with one fewer letters than u, starting with the empty word.
    // w: a word such that w < u, also starting with the empty word.
    //
    // 1. If v < x, then v is replaced by the next word in the order. If |uv| <=
    //    M, then the next word is uv. Otherwise, goto 1.
    //
    // 2. If v = x, then and there exists a word w' in the set of words obtained
    //    so far such that w' > w and |w'| <= M - 1, then replace w with w',
    //    replace u by wa_n, replace v by the empty word, and the next word is
    //    wa_n.
    //
    //    If no such word w' exists, then we have enumerated all the required
    //    words, and we can stop.
    //
    // For example, if A = {a, b} and M = 4, then the initial elements in the
    // order are:
    //
    //   e < a < aa < aaa < aaaa (e is the empty word)
    //
    // Set b > aaaa. At this point, x = aaaa, u = b, v = e, w = e, and so
    // (1) applies, v <- a, and since |uv| = ba <= 4 = M, the next word is
    // ba.  Repeatedly applying (1), until it fails to hold, we obtain the
    // following:
    //
    //   aaaa < b < ba < baa < baaa
    //
    // After defining baa < baaa, x = aaaa, u = b, v = aaaa, and w = e. Hence v
    // = x, and so (2) applies. The next w' in the set of words so far
    // enumerated is a, and |a| = 1 <= 3 = M - 1, and so w <- a, u <- ab, v <-
    // e, and the next word is ab. We repeatedly apply (1), until it fails, to
    // obtain
    //
    //   baaa < ab < aba < abaa
    //
    // At which point u = b, v = aaaa = x, and w = a. Hence (2) applies, w <-
    // aa, v <- e, u <- aab, and the next word is: aab. And so on ...

    // TODO(later): improve this, it is currently very slow.
    void ToddCoxeter::recursive_standardize() {
      REPORT_DEFAULT("standardizing (recursive)... ");
      detail::Timer          tmr;
      std::vector<word_type> out;
      size_t const           n = nr_generators();
      letter_type            a = 0;
      coset_type             s = 0;
      coset_type             t = 0;

      std::vector<coset_type> p(coset_capacity(), 0);
      std::iota(p.begin(), p.end(), 0);
      std::vector<coset_type> q(coset_capacity(), 0);
      std::iota(q.begin(), q.end(), 0);

      while (s <= t) {
        if (standardize_deferred(p, q, s, t, 0)) {
          out.push_back(word_type(t, a));
        }
        s++;
      }
      a++;
      bool new_generator = true;
      int  x, u, w;
      while (a < n && t < nr_cosets_active() - 1) {
        if (new_generator) {
          w = -1;  // -1 is the empty word
          if (standardize_deferred(p, q, 0, t, a)) {
            out.push_back({a});
          }
          x             = out.size() - 1;
          u             = out.size() - 1;
          new_generator = false;
        }

        coset_type const uu = tau(0, out[u].begin(), out[u].end());
        if (uu != UNDEFINED) {
          for (int v = 0; v < x; v++) {
            coset_type const uuv = tau(uu, out[v].begin(), out[v].end() - 1);
            if (uuv != UNDEFINED) {
              s = q[uuv];
              if (standardize_deferred(p, q, s, t, out[v].back())) {
                word_type nxt = out[u];
                nxt.insert(nxt.end(), out[v].begin(), out[v].end());
                out.push_back(std::move(nxt));
              }
            }
          }
        }
        w++;
        if (static_cast<size_t>(w) < out.size()) {
          coset_type const ww = tau(0, out[w].begin(), out[w].end());
          if (ww != UNDEFINED) {
            s = q[ww];
            if (standardize_deferred(p, q, s, t, a)) {
              u             = out.size();
              word_type nxt = out[w];
              nxt.push_back(a);
              out.push_back(std::move(nxt));
            }
          }
        } else {
          a++;
          new_generator = true;
        }
      }
      apply_permutation(p, q);
      REPORT("%d\n", tmr.string()).prefix().flush_right().flush();
#ifdef LIBSEMIGROUPS_DEBUG
      debug_validate_forwd_bckwd();
      debug_validate_table();
      debug_validate_preimages();
#endif
    }

    // The permutation q must map the active cosets to the [0, ..
    // , nr_cosets_active())
    void ToddCoxeter::apply_permutation(std::vector<coset_type>& p,
                                        std::vector<coset_type>& q) {
      // p : new -> old, q = p ^ -1
#ifdef LIBSEMIGROUPS_DEBUG
      for (size_t c = 0; c < q.size(); ++c) {
        LIBSEMIGROUPS_ASSERT(
            (is_active_coset(c) && q[c] < nr_cosets_active())
            || (!is_active_coset(c) && q[c] >= nr_cosets_active()));
        LIBSEMIGROUPS_ASSERT(p[q[c]] == c);
        LIBSEMIGROUPS_ASSERT(q[p[c]] == c);
      }
#endif
      {
        coset_type   c = _id_coset;
        size_t const n = nr_generators();
        // Permute all the values in the _table, and pre-images, that relate
        // to active cosets
        while (c < nr_cosets_active()) {
          for (letter_type x = 0; x < n; ++x) {
            coset_type i = _table.get(p[c], x);
            _table.set(p[c], x, (i == UNDEFINED ? i : q[i]));
            i = _preim_init.get(p[c], x);
            _preim_init.set(p[c], x, (i == UNDEFINED ? i : q[i]));
            i = _preim_next.get(p[c], x);
            _preim_next.set(p[c], x, (i == UNDEFINED ? i : q[i]));
          }
          c++;
        }
        // Permute the rows themselves
        _table.apply_row_permutation(p);
        _preim_init.apply_row_permutation(p);
        _preim_next.apply_row_permutation(p);
      }
      {
        // Permute the cosets in the CosetManager using p . . .
        size_t const n = p.size();
        for (coset_type i = 0; i < n; ++i) {
          coset_type current = i;
          while (i != p[current]) {
            size_t next = p[current];
            switch_cosets(current, next);
            p[current] = current;
            current    = next;
          }
          p[current] = current;
        }
      }
    }

    // Based on the procedure SWITCH in Sims' book, p193
    // Swaps an active coset and another coset in the table.
    void ToddCoxeter::swap(coset_type const c, coset_type const d) {
      TODD_COXETER_REPORT_SWITCH(c, d)

      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      LIBSEMIGROUPS_ASSERT(c != _id_coset);
      LIBSEMIGROUPS_ASSERT(d != _id_coset);
      LIBSEMIGROUPS_ASSERT(c != d);
      LIBSEMIGROUPS_ASSERT(is_valid_coset(c));
      LIBSEMIGROUPS_ASSERT(is_valid_coset(d));
      LIBSEMIGROUPS_ASSERT(is_active_coset(c) || is_active_coset(d));

      size_t const n = nr_generators();
      for (letter_type x = 0; x < n; ++x) {
        coset_type cx = _table.get(c, x);
        coset_type dx = _table.get(d, x);

        if (is_active_coset(c)) {
          // Replace c <-- d in the coset table _table
          coset_type e = _preim_init.get(c, x);
          while (e != UNDEFINED) {
            LIBSEMIGROUPS_ASSERT(_table.get(e, x) == c);
            _table.set(e, x, d);
            e = _preim_next.get(e, x);
          }
          _table.set(c, x, ff(c, d, cx));
        }
        if (is_active_coset(d)) {
          // Replace d <-- c in the coset table _table
          coset_type e = _preim_init.get(d, x);
          while (e != UNDEFINED) {
            _table.set(e, x, c);
            e = _preim_next.get(e, x);
          }
          _table.set(d, x, ff(c, d, dx));
        }
        if (is_active_coset(c) && is_active_coset(d) && cx == dx
            && cx != UNDEFINED) {
          // Swap c <--> d in preimages of cx = dx
          size_t     found = 0;
          coset_type e     = _preim_init.get(cx, x);
          if (e == c) {
            _preim_init.set(cx, x, d);
            found++;
          } else if (e == d) {
            _preim_init.set(cx, x, c);
            found++;
          }
          while (e != UNDEFINED && found < 2) {
            LIBSEMIGROUPS_ASSERT(ff(c, d, _table.get(e, x)) == cx);
            coset_type f = _preim_next.get(e, x);
            if (f == c) {
              _preim_next.set(e, x, d);
              found++;
            } else if (f == d) {
              _preim_next.set(e, x, c);
              found++;
            }
            e = f;
          }
        } else {
          if (is_active_coset(c) && cx != UNDEFINED) {
            // Replace c <-- d in preimages of cx, and d is not a preimage of
            // cx under x
            coset_type e = _preim_init.get(cx, x);
            if (e == c) {
              _preim_init.set(cx, x, d);
              e = UNDEFINED;  // To prevent going into the next loop
            }
            while (e != UNDEFINED) {
              LIBSEMIGROUPS_ASSERT(ff(c, d, _table.get(e, x)) == cx);
              coset_type f = _preim_next.get(e, x);
              if (f == c) {
                _preim_next.set(e, x, d);
                break;
              }
              e = f;
            }
          }
          if (is_active_coset(d) && dx != UNDEFINED) {
            // Replace d <-- c in preimages of dx, and c is not a preimage of
            // dx under x
            coset_type e = _preim_init.get(dx, x);
            if (e == d) {
              _preim_init.set(dx, x, c);
              e = UNDEFINED;  // To prevent going into the next loop
            }
            while (e != UNDEFINED) {
              LIBSEMIGROUPS_ASSERT(ff(c, d, _table.get(e, x)) == dx);
              coset_type f = _preim_next.get(e, x);
              if (f == d) {
                _preim_next.set(e, x, c);
                break;
              }
              e = f;
            }
          }
        }
        _table.swap(c, x, d, x);
        _preim_init.swap(c, x, d, x);
        _preim_next.swap(c, x, d, x);
      }
      switch_cosets(c, d);
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (debug) - private
    ////////////////////////////////////////////////////////////////////////

#ifdef LIBSEMIGROUPS_DEBUG
    // Validates the coset table.
    void ToddCoxeter::debug_validate_table() const {
      REPORT_DEBUG_DEFAULT("validating the coset table... ");
      size_t const n = nr_generators();
      coset_type   c = _id_coset;
      while (c != first_free_coset()) {
        if (!is_active_coset(c)) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid table, coset %d is both free and active!", c);
        }
        for (letter_type x = 0; x < n; ++x) {
          if (_table.get(c, x) != UNDEFINED
              && !is_active_coset(_table.get(c, x))) {
            LIBSEMIGROUPS_EXCEPTION("invalid table, _table.get(%d, %d) = %d"
                                    " is not an active coset or UNDEFINED!",
                                    c,
                                    x,
                                    _table.get(c, x));
          }
        }
        c = next_active_coset(c);
      }
      TODD_COXETER_REPORT_OK();
    }

    // Validates the preimages, this is very expensive.
    void ToddCoxeter::debug_validate_preimages() const {
      REPORT_DEBUG_DEFAULT("validating preimages... ");
      size_t const n = nr_generators();
      coset_type   c = _id_coset;
      while (c != first_free_coset()) {
        for (letter_type x = 0; x < n; ++x) {
          coset_type           e = _preim_init.get(c, x);
          std::set<coset_type> stored_preimages;
          while (e != UNDEFINED) {
            if (!is_active_coset(e)) {
              LIBSEMIGROUPS_EXCEPTION("invalid preimage e = %d of c = %d, e = "
                                      "%d is not an active coset or UNDEFINED!",
                                      e,
                                      c,
                                      e);
            }
            if (!stored_preimages.insert(e).second) {
              LIBSEMIGROUPS_EXCEPTION(
                  "duplicate preimage e = %d of c = %d under x = %d!", e, c, x);
            }
            if (_table.get(e, x) != c) {
              LIBSEMIGROUPS_EXCEPTION(
                  "invalid preimage, _table.get(%d, %d) != %d but found e = %d "
                  "in the preimages of %d under x = %d",
                  e,
                  x,
                  c,
                  e,
                  c,
                  x);
            }
            e = _preim_next.get(e, x);
          }
          // Check there are no missing preimages!
          coset_type d = 0;
          while (d != first_free_coset()) {
            if (d != c && _table.get(d, x) == c
                && stored_preimages.insert(d).second) {
              LIBSEMIGROUPS_EXCEPTION(
                  "missing preimage, _table.get(%d, %d) == %d but d = %d "
                  "wasn't found in the stored preimages",
                  d,
                  x,
                  c,
                  d);
            }
            d = next_active_coset(d);
          }
        }
        c = next_active_coset(c);
      }
      TODD_COXETER_REPORT_OK();
    }

    void ToddCoxeter::debug_verify_no_missing_deductions() const {
      REPORT_DEBUG_DEFAULT("verifying no missing deductions or "
                           "coincidences...");
      if (!_deduct.empty()) {
        LIBSEMIGROUPS_EXCEPTION("the deduction stack is not empty");
      }
      if (!_coinc.empty()) {
        LIBSEMIGROUPS_EXCEPTION("the coincidence stack is not empty");
      }
      for (coset_type c = _id_coset; c != first_free_coset();
           c            = next_active_coset(c)) {
        for (auto it = _relations.cbegin(); it < _relations.cend(); it += 2) {
          word_type const& u = *it;
          word_type const& v = *(it + 1);
          LIBSEMIGROUPS_ASSERT(is_valid_coset(c));
          LIBSEMIGROUPS_ASSERT(!u.empty());
          LIBSEMIGROUPS_ASSERT(!v.empty());
          coset_type x = tau(c, u.cbegin(), u.cend() - 1);
          if (x == UNDEFINED) {
            goto end;
          }
          LIBSEMIGROUPS_ASSERT(is_valid_coset(x));
          coset_type y = tau(c, v.cbegin(), v.cend() - 1);
          if (y == UNDEFINED) {
            goto end;
          }
          LIBSEMIGROUPS_ASSERT(is_valid_coset(y));
          letter_type a  = u.back();
          letter_type b  = v.back();
          coset_type  xx = _table.get(x, a);
          coset_type  yy = _table.get(y, b);
          if (xx == UNDEFINED && yy != UNDEFINED) {
            LIBSEMIGROUPS_EXCEPTION("missing deduction tau(%d, %d) = %d when "
                                    "pushing coset %d through %s = %s",
                                    x,
                                    a,
                                    yy,
                                    c,
                                    detail::to_string(u),
                                    detail::to_string(v));
          } else if (xx != UNDEFINED && yy == UNDEFINED) {
            // tau(y, b) <- xx
            LIBSEMIGROUPS_EXCEPTION("missing deduction tau(%d, %d) = %d when "
                                    "pushing coset %d through %s = %s",
                                    y,
                                    b,
                                    xx,
                                    c,
                                    detail::to_string(u),
                                    detail::to_string(v));
          } else if (xx != UNDEFINED && yy != UNDEFINED) {
            // tau(x, a) and tau(y, b) are defined
            if (xx != yy) {
              LIBSEMIGROUPS_EXCEPTION("missing coincidence %d = %d when "
                                      "pushing coset %d through %s = %s",
                                      xx,
                                      yy,
                                      c,
                                      detail::to_string(u),
                                      detail::to_string(v));
            }
          }
        }
      }
    end:
      TODD_COXETER_REPORT_OK();
    }

#endif
  }  // namespace congruence
}  // namespace libsemigroups
