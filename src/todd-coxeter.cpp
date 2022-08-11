//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-21 James D. Mitchell
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

////////////////////////////////////////////////////////////////////////////////
// TODO(later)
//
// * Make process_deductions_dfs non-recursive, this will likely only be an
//   issue for presentations with extremely long relations.
//
// * Wreath product standardize mem fn.
//
////////////////////////////////////////////////////////////////////////////////
//
// Notes
//
// 1. It's suggested in Sims' book in the implementation appendix that using
//    the actual index of the coset referenced rather than the "row", to
//    avoid multiplication in get. I implemented this in 2021 and it made no
//    difference whatsoever, so I backed the changes.
//
// 2. In 2021, I couldn't figure out a low-cost way of implementing the "fill"
//    factor. The idea is that we should check that a certain percentage of the
//    entries in the table are filled before we use preferred definitions.
//    This doesn't seem to be necessary in any of the examples or tests, i.e.
//    the resulting enumerations are valid and terminate if we just always use
//    the preferred definitions and don't try to keep a particular percentage
//    of the entries in the table filled. It's possible that not using the
//    preferred definitions when the table is not filled enough, would make the
//    enumeration terminate faster, but it didn't seem worth the extra cost or
//    complexity of implementing the fill factor.
//
// 3. In 2021, I tried in felsch_lookahead to default to hlt_lookahead if there
//    are more paths (nodes) in the Felsch tree than the number of active
//    cosets times the number of relations, but found no examples where this
//    was invoked. So, I scrapped this too.
//
// 4. In 2021, I tried putting the options save() + standardize() outside the
//    main loop (+ duplicating a lot of the code) in ToddCoxeter::hlt and this
//    produced no measurable difference to the performance, so I backed these
//    changes out too.

#include "libsemigroups/todd-coxeter.hpp"

#include <algorithm>      // for reverse
#include <array>          // for array
#include <chrono>         // for nanoseconds etc
#include <cstddef>        // for size_t
#include <iterator>       // for distance
#include <limits>         // for numeric_limits
#include <memory>         // for shared_ptr
#include <numeric>        // for iota, accumulate
#include <ostream>        // for operator<<
#include <queue>          // for queue
#include <random>         // for mt19937
#include <string>         // for operator+, basic_string
#include <tuple>          // for tie
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair

#ifdef LIBSEMIGROUPS_DEBUG
#include <set>  // for set
#endif

#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_DEBUG

#include "libsemigroups/adapters.hpp"           // for Hash
#include "libsemigroups/cong-intf.hpp"          // for CongruenceInterface
#include "libsemigroups/coset.hpp"              // for CosetManager
#include "libsemigroups/debug.hpp"              // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/digraph-helper.hpp"     // for follow_path_nc, last_...
#include "libsemigroups/exception.hpp"          // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/felsch-tree.hpp"        // for FelschTree
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"       // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"       // for fpsemigroup::KnuthBendix
#include "libsemigroups/obvinf.hpp"             // for IsObviouslyInfinite
#include "libsemigroups/report.hpp"             // for PrintTable, Reporter
#include "libsemigroups/string.hpp"             // for to_string
#include "libsemigroups/suffix-tree.hpp"        // for SuffixTree
#include "libsemigroups/tce.hpp"                // for TCE
#include "libsemigroups/timer.hpp"              // for detail::Timer
#include "libsemigroups/types.hpp"              // for letter_type
#include "libsemigroups/uf.hpp"                 // for Duf

// This file is organised as follows:
//
// 1.  Helper functions, types etc in anonymous namespace
// 2.  ToddCoxeter - inner classes - private
// 3.  ToddCoxeter - constructors and destructor - public
// 4.  CongruenceInterface - non-pure virtual member functions - public
// 5.  CongruenceInterface - non-pure virtual member functions - private
// 6.  CongruenceInterface - pure virtual member functions - private
// 7.  ToddCoxeter - member functions (init + settings) - public
// 8.  ToddCoxeter - member functions (sorting gen. pairs etc) - public
// 9.  ToddCoxeter - member functions (container-like) - public
// 10. ToddCoxeter - member functions (state) - public
// 11. ToddCoxeter - member functions (standardization) - public
// 12. ToddCoxeter - member functions (attributes) - public
// 13. ToddCoxeter - member functions (reporting + stats) - public
// 14. ToddCoxeter - member functions (reporting + stats) - private
// 15. ToddCoxeter - member functions (validation) - private
// 16. ToddCoxeter - member functions (initialisation + settings) - private
// 17. ToddCoxeter - member functions (cosets) - private
// 18. ToddCoxeter - member functions (main strategies) - private
// 19. ToddCoxeter - member functions (deduction processing) - private
// 20. ToddCoxeter - member functions (lookahead) - private
// 21. ToddCoxeter - member functions (standardize) - private
// 22. ToddCoxeter - member functions (debug) - private

////////////////////////////////////////////////////////////////////////
// 1. Helper functions, types etc in anonymous namespace
////////////////////////////////////////////////////////////////////////

using coset_type = libsemigroups::detail::CosetManager::coset_type;

namespace {
  using class_index_type = libsemigroups::CongruenceInterface::class_index_type;
  using word_type        = libsemigroups::word_type;

  using Perm = typename libsemigroups::detail::CosetManager::Perm;
  template <typename T>
  using EqualTo = typename libsemigroups::EqualTo<T>;
  template <typename T>
  using Hash = typename libsemigroups::Hash<T>;

  void reverse_if_necessary_and_push_back(
      libsemigroups::congruence::ToddCoxeter const* tc,
      word_type                                     w,
      std::vector<word_type>&                       v) {
    if (tc->kind() == libsemigroups::congruence_kind::left) {
      std::reverse(w.begin(), w.end());
    }
    v.push_back(std::move(w));
  }

  void sort_generating_pairs(Perm& perm, std::vector<word_type>& vec) {
    // Apply the permutation (adapted from stl.hpp:apply_permutation)
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
      Perm&                                                   perm,
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

  // Attempts to reduce the length of the words in "words" by finding the
  // equivalence relation on the union of "wrt" and "words" generated by the
  // pairs in "wrt".  If A = {u_1, u_2, ..., u_n} are the distinct words in an
  // equivalence class on "words" and u_1 is the short-lex minimum word in the
  // class, then the words in "words" involving the words in A are replaced by:
  // u_1 = u_2, u_1 = u_3, ..., u_1 = u_n.
  class PairsSimplifier {
   public:
    PairsSimplifier() = default;

    PairsSimplifier& add(std::vector<word_type> const& words) {
      LIBSEMIGROUPS_ASSERT(words.size() % 2 == 0);
      _duf.resize(_duf.size() + words.size());
      // Create equivalence relation of the equal relations
      for (size_t i = 0; i < words.size(); ++i) {
        if (i % 2 == 0) {
          _duf.unite(i, i + 1);
        }
        auto const&              current_word = words[i];
        decltype(_map)::iterator it;
        bool                     inserted;
        std::tie(it, inserted) = _map.emplace(current_word, i);
        if (!inserted) {
          _duf.unite(it->second, i);
        }
      }
      return *this;
    }

    bool trivial() const {
      return (_duf.number_of_blocks() == _map.size());
    }

    void apply(std::vector<word_type>& words) {
      using libsemigroups::shortlex_compare;
      LIBSEMIGROUPS_ASSERT(words.size() % 2 == 0);
      // Class index -> index of min. length words in wrt + words
      std::unordered_map<size_t, word_type> mins;
      if (words.empty()) {
        return;
      }

      // Find index of minimum length word in every class
      for (auto const& word : words) {
        auto                     i = _map.find(word)->second;
        auto                     j = _duf.find(i);
        decltype(mins)::iterator it;
        bool                     inserted;
        std::tie(it, inserted) = mins.emplace(j, word);
        auto const& min_word   = it->second;
        if (!inserted && shortlex_compare(word, min_word)) {
          it->second = word;
        }
      }

      std::vector<word_type> result;
      for (auto it = _map.cbegin(); it != _map.cend(); ++it) {
        auto const& word     = it->first;
        auto const& index    = it->second;
        auto const& min_word = mins.find(_duf.find(index))->second;
        if (min_word != word) {
          result.push_back(min_word);
          result.push_back(word);
        }
      }
      std::swap(words, result);
    }

   private:
    libsemigroups::detail::Duf<> _duf;
    std::unordered_map<word_type, size_t, Hash<word_type>, EqualTo<word_type>>
        _map;
  };

  struct Noop {
    template <typename... Args>
    void operator()(Args...) const noexcept {}

    template <typename T>
    void operator()(libsemigroups::congruence::ToddCoxeter*) const noexcept {}
  };

  using DoNotProcessCoincidences = Noop;
  using NoPreferredDefs          = Noop;
  using DoNotStackDeductions     = Noop;

}  // namespace

namespace libsemigroups {

  using detail::FelschTree;

  namespace congruence {

    ////////////////////////////////////////////////////////////////////////
    // 2. ToddCoxeter - inner classes - private
    ////////////////////////////////////////////////////////////////////////

    // A class for managing Deductions (recently changed or defined values of
    // the form std::pair(coset_type, letter_type).
    class ToddCoxeter::Deductions {
     public:
      explicit Deductions(ToddCoxeter* tc)
          : _any_skipped(false), _deduct(), _tc(tc) {}

      Deduction pop() {
        auto res = _deduct.top();
        _deduct.pop();
        return res;
      }

      void emplace(coset_type c, letter_type x) {
        using opt = options::deductions;
        if (_deduct.size() < _tc->max_deductions()) {
          _deduct.emplace(c, x);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
          _tc->_stats.max_deduct = std::max(
              _tc->_stats.max_deduct, static_cast<uint64_t>(_deduct.size()));
#endif
        } else {
          _any_skipped = true;
          if (_tc->deduction_policy() & opt::purge_from_top) {
            while (!_deduct.empty()
                   && !_tc->is_active_coset(_deduct.top().first)) {
              _deduct.pop();
            }
          } else if (_tc->deduction_policy() & opt::purge_all) {
            std::stack<Deduction> copy;
            while (!_deduct.empty()) {
              auto d = pop();
              if (_tc->is_active_coset(d.first)) {
                copy.push(d);
              }
            }
            std::swap(copy, _deduct);
          } else if (_tc->deduction_policy() & opt::discard_all_if_no_space) {
            clear();
          }
        }
      }

      bool any_skipped() const noexcept {
        return _any_skipped;
      }

      // noexcept correct?
      bool empty() const noexcept {
        return _deduct.empty();
      }

      size_t size() const noexcept {
        return _deduct.size();
      }

      void clear() {
        if (!_deduct.empty()) {
          _any_skipped = true;
          std::stack<Deduction> copy;
          std::swap(copy, _deduct);
        }
      }

     private:
      bool                  _any_skipped;
      std::stack<Deduction> _deduct;
      ToddCoxeter*          _tc;
    };

    struct ToddCoxeter::Settings {
      bool                use_relations_in_extra = false;
      size_t              deduct_max             = 2'000;
      options::deductions deduct_policy
          = options::deductions::no_stack_if_no_space | options::deductions::v2;
      size_t                f_defs         = 100'000;
      options::froidure_pin froidure_pin   = options::froidure_pin::none;
      size_t                hlt_defs       = 200'000;
      size_t                large_collapse = 100'000;
      options::lookahead    lookahead
          = options::lookahead::partial | options::lookahead::hlt;
      float                   lookahead_growth_factor    = 2.0;
      size_t                  lookahead_growth_threshold = 4;
      size_t                  lower_bound                = UNDEFINED;
      size_t                  max_preferred_defs         = 256;
      size_t                  min_lookahead              = 10'000;
      size_t                  next_lookahead             = 5'000'000;
      options::preferred_defs preferred_defs
          = options::preferred_defs::deferred;
      std::chrono::nanoseconds random_interval = std::chrono::milliseconds(200);
      bool                     restandardize   = false;
      bool                     save            = false;
      bool                     standardize     = false;
      options::strategy        strategy        = options::strategy::hlt;
    };

    class ToddCoxeter::PreferredDefs {
     public:
      explicit PreferredDefs(ToddCoxeter* tc) : _defs(), _tc(tc) {}

      bool empty() const noexcept {
        return _defs.empty();
      }

      Deduction pop() {
        Deduction d = _defs.front();
        _defs.pop();
        return d;
      }

      void emplace(coset_type c, letter_type x) {
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        _tc->_stats.total_preferred_defs++;
#endif
        _defs.emplace(c, x);
        if (_defs.size() > _tc->max_preferred_defs()) {
          _defs.pop();
        }
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        _tc->_stats.max_preferred_defs
            = std::max(_tc->_stats.max_preferred_defs, uint64_t(_defs.size()));
#endif
      }

      void purge_from_top() {
        while (!_defs.empty()
               && (!_tc->is_active_coset(_defs.front().first)
                   || _tc->_word_graph.unsafe_neighbor(_defs.front().first,
                                                       _defs.front().second)
                          != UNDEFINED)) {
          _defs.pop();
        }
      }

     private:
      std::queue<Deduction> _defs;
      ToddCoxeter*          _tc;
    };

    struct ToddCoxeter::TreeNode {
      TreeNode() : parent(UNDEFINED), gen(UNDEFINED) {}
      TreeNode(coset_type p, letter_type g) : parent(p), gen(g) {}
      coset_type  parent;
      letter_type gen;
    };

    struct ToddCoxeter::QueuePreferredDefs {
      void operator()(ToddCoxeter* tc,
                      coset_type   x,
                      letter_type  a,
                      coset_type   y,
                      letter_type  b) {
        tc->_preferred_defs->emplace(x, a);
        tc->_preferred_defs->emplace(y, b);
      }
    };

    struct ToddCoxeter::StackDeductions {
      inline void operator()(ToddCoxeter::Deductions* stck,
                             coset_type               c,
                             letter_type              a) const noexcept {
        stck->emplace(c, a);
      }
    };

    template <typename TStackDeduct>
    struct ToddCoxeter::ImmediateDef {
      void operator()(ToddCoxeter* tc,
                      coset_type   x,
                      letter_type  a,
                      coset_type   y,
                      letter_type  b) const {
        coset_type d = tc->new_coset();
        tc->def_edge<TStackDeduct>(x, a, d);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        tc->_stats.tc2_good_appl++;
        if (tc->strategy() == options::strategy::hlt) {
          tc->_stats.tc1_hlt_appl++;
        } else {
          tc->_stats.tc1_f_appl++;
        }
#endif

        if (a != b || x != y) {
          tc->def_edge<TStackDeduct>(y, b, d);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
          tc->_stats.tc2_good_appl++;
#endif
        }
      }
    };

    ////////////////////////////////////////////////////////////////////////
    // 3. ToddCoxeter - constructors and destructor - public
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeter::ToddCoxeter(congruence_kind type)
        : CongruenceInterface(type),
          CosetManager(),
          _coinc(),
          _deduct(new Deductions(this)),
          _extra(),
          _felsch_tree(nullptr),
          _nr_pairs_added_earlier(0),
          _prefilled(false),
          _preferred_defs(new PreferredDefs(this)),
          _relations(),
          _settings(new Settings()),
          _standard_max(0),
          _standardized(order::none),
          _state(state::constructed),
          _tree(nullptr),
          _word_graph() {}

    ToddCoxeter::ToddCoxeter(ToddCoxeter const& copy)
        : CongruenceInterface(copy),
          CosetManager(copy),
          _coinc(copy._coinc),
          _deduct(std::make_unique<Deductions>(*copy._deduct)),
          _extra(copy._extra),
          _felsch_tree(nullptr),
          _nr_pairs_added_earlier(copy._nr_pairs_added_earlier),
          _prefilled(copy._prefilled),
          _preferred_defs(
              std::make_unique<PreferredDefs>(*copy._preferred_defs)),
          _relations(copy._relations),
          _settings(std::make_unique<Settings>(*copy._settings)),
          _standard_max(copy._standard_max),
          _standardized(copy._standardized),
          _state(copy._state),
          _tree(nullptr),
          _word_graph(copy._word_graph) {
      if (copy._felsch_tree != nullptr) {
        _felsch_tree = std::make_unique<FelschTree>(*copy._felsch_tree);
      }
      if (copy._tree != nullptr) {
        _tree = std::make_unique<Tree>(*copy._tree);
      }
    }

    ToddCoxeter::ToddCoxeter(congruence_kind                  type,
                             std::shared_ptr<FroidurePinBase> S,
                             options::froidure_pin            p)
        : ToddCoxeter(type) {
      froidure_pin_policy(p);
      set_parent_froidure_pin(S);
      set_number_of_generators(S->number_of_generators());
    }

    // Construct a ToddCoxeter object representing a congruence over the
    // semigroup defined by copy (the quotient that is).
    ToddCoxeter::ToddCoxeter(congruence_kind typ, ToddCoxeter& copy)
        : ToddCoxeter(typ) {
      if (copy.kind() != congruence_kind::twosided && typ != copy.kind()) {
        LIBSEMIGROUPS_EXCEPTION("incompatible types of congruence, found ("
                                + congruence_kind_to_string(copy.kind()) + " / "
                                + congruence_kind_to_string(typ)
                                + ") but only (left / left), (right / "
                                  "right), (two-sided / *) are valid");
      }
      copy_relations_for_quotient(copy);
    }

    ToddCoxeter::ToddCoxeter(congruence_kind           typ,
                             fpsemigroup::ToddCoxeter& copy)
        : ToddCoxeter(typ) {
      set_parent_froidure_pin(copy);
      if (copy.finished()) {
        set_number_of_generators(copy.froidure_pin()->number_of_generators());
        froidure_pin_policy(options::froidure_pin::use_cayley_graph);
      } else {
        copy_relations_for_quotient(copy.congruence());
        froidure_pin_policy(options::froidure_pin::use_relations);
      }
    }

    ToddCoxeter::ToddCoxeter(congruence_kind typ, fpsemigroup::KnuthBendix& kb)
        : ToddCoxeter(typ) {
      set_number_of_generators(kb.alphabet().size());
      // TODO(later) use active rules when available
      for (auto it = kb.cbegin_rules(); it < kb.cend_rules(); ++it) {
        add_pair(kb.string_to_word(it->first), kb.string_to_word(it->second));
      }
      // Note that something goes horribly wrong if the next line is above the
      // for loop above.
      set_parent_froidure_pin(kb);
      if (kb.finished() && kb.is_obviously_finite()) {
        LIBSEMIGROUPS_ASSERT(froidure_pin_policy()
                             == options::froidure_pin::none);
        froidure_pin_policy(options::froidure_pin::use_cayley_graph);
      }
    }

    ToddCoxeter::~ToddCoxeter() {
      while (!_setting_stack.empty()) {
        pop_settings();
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // 4. CongruenceInterface - non-pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

    bool ToddCoxeter::contains(word_type const& lhs, word_type const& rhs) {
      validate_word(lhs);
      validate_word(rhs);
      init_generating_pairs();
      if (empty()) {
        // Note that it's possible to be not _prefilled, and have _relations,
        // and _extra empty, because shrink_to_fit clears _relations and
        // _extra. That's why we use empty() here instead of checking
        // _prefilled && _relations.empty() && _extra.empty(), as used to be
        // the case. This defines the free semigroup
        return lhs == rhs;
      }
      return CongruenceInterface::contains(lhs, rhs);
    }

    ////////////////////////////////////////////////////////////////////////
    // 5. CongruenceInterface - non-pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    class_index_type
    ToddCoxeter::const_word_to_class_index(word_type const& w) const {
      validate_word(w);
      coset_type c = _id_coset;

      if (kind() == congruence_kind::left) {
        c = action_digraph_helper::follow_path_nc(
            _word_graph, c, w.crbegin(), w.crend());
      } else {
        c = action_digraph_helper::follow_path_nc(
            _word_graph, c, w.cbegin(), w.cend());
      }
      return (c == UNDEFINED ? UNDEFINED
                             : static_cast<class_index_type>(c - 1));
    }

    bool ToddCoxeter::is_quotient_obviously_finite_impl() {
      if (finished()) {
        return true;
      }
      init_generating_pairs();
      return _prefilled;
      // _prefilled means that either we were created from a FroidurePinBase*
      // with froidure_pin_policy() == use_cayley_graph and we successfully
      // prefilled the table, or we manually prefilled the table.  In this
      // case the semigroup defined by _relations must be finite.
    }

    bool ToddCoxeter::is_quotient_obviously_infinite_impl() {
      if (finished()) {
        return false;
      }
      init_generating_pairs();
      if (_prefilled) {
        return false;
      } else if (number_of_generators() > _relations.size() + _extra.size()) {
        return true;
      }
      detail::IsObviouslyInfinite ioi(number_of_generators());
      ioi.add_rules(_relations.cbegin(), _relations.cend());
      ioi.add_rules(_extra.cbegin(), _extra.cend());
      return ioi.result();
    }

    void ToddCoxeter::set_number_of_generators_impl(size_t n) {
      LIBSEMIGROUPS_ASSERT(_word_graph.out_degree() == 0);
      LIBSEMIGROUPS_ASSERT(_word_graph.number_of_nodes() == 0);
      _word_graph.add_nodes(1);
      _word_graph.add_to_out_degree(n);
    }

    void ToddCoxeter::add_generators_impl(size_t n) {
      LIBSEMIGROUPS_ASSERT(_word_graph.out_degree() != 0);
      LIBSEMIGROUPS_ASSERT(_word_graph.number_of_nodes() == 1);
      _word_graph.add_to_out_degree(n);
    }

    ////////////////////////////////////////////////////////////////////////
    // 5. CongruenceInterface - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    word_type ToddCoxeter::class_index_to_word_impl(class_index_type i) {
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
      if (kind() != congruence_kind::left) {
        std::reverse(w.begin(), w.end());
      }
      return w;
    }

    size_t ToddCoxeter::number_of_classes_impl() {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      return number_of_cosets_active() - 1;
    }

    std::shared_ptr<FroidurePinBase> ToddCoxeter::quotient_impl() {
      using detail::TCE;
      run();
      standardize(order::shortlex);
      shrink_to_fit();
      // Ensure class indices and letters are equal!
      auto   table = std::make_shared<table_type>(_word_graph.table());
      size_t n     = number_of_generators();
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
      for (size_t i = 0; i < number_of_generators(); ++i) {
        // We use table.get(0, i) instead of just i, because there might be
        // more generators than cosets.
        ptr->add_generator(TCE(_word_graph.unsafe_neighbor(0, i)));
      }
      return ptr;
    }

    void ToddCoxeter::run_impl() {
      if (is_quotient_obviously_infinite()) {
        LIBSEMIGROUPS_EXCEPTION(
            "there are infinitely many classes in the congruence and "
            "Todd-Coxeter will never terminate");
      }

      if (strategy() == options::strategy::felsch) {
        felsch();
      } else if (strategy() == options::strategy::hlt) {
        hlt();
      } else if (strategy() == options::strategy::random) {
        if (running_for()) {
          LIBSEMIGROUPS_EXCEPTION(
              "the strategy \"%s\" is incompatible with run_for!",
              detail::to_string(strategy()).c_str());
        }
        random();
      } else {
        if (running_until()) {
          LIBSEMIGROUPS_EXCEPTION(
              "the strategy \"%s\" is incompatible with run_until!",
              detail::to_string(strategy()).c_str());
        }

        if (strategy() == options::strategy::CR) {
          CR_style();
        } else if (strategy() == options::strategy::R_over_C) {
          R_over_C_style();
        } else if (strategy() == options::strategy::Cr) {
          Cr_style();
        } else if (strategy() == options::strategy::Rc) {
          Rc_style();
        }
      }
    }

    bool ToddCoxeter::finished_impl() const {
      return _state == state::finished;
    }

    class_index_type ToddCoxeter::word_to_class_index_impl(word_type const& w) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      if (!is_standardized()) {
        standardize(order::shortlex);
      }
      return const_word_to_class_index(w);
      // c is in the range 1, ..., number_of_cosets_active() because 0
      // represents the identity coset, and does not correspond to an element.
    }

    ////////////////////////////////////////////////////////////////////////
    // 7. ToddCoxeter - member functions (init_generating_pairs + settings) -
    // public
    ////////////////////////////////////////////////////////////////////////

    // Settings
    ToddCoxeter&
    ToddCoxeter::froidure_pin_policy(options::froidure_pin x) noexcept {
      _settings->froidure_pin = x;
      return *this;
    }

    ToddCoxeter::options::froidure_pin
    ToddCoxeter::froidure_pin_policy() const noexcept {
      return _settings->froidure_pin;
    }

    ToddCoxeter& ToddCoxeter::lookahead(options::lookahead x) noexcept {
      if (!(x & options::lookahead::felsch) && !(x & options::lookahead::hlt)) {
        x = x | options::lookahead::hlt;
      }
      _settings->lookahead = x;
      return *this;
    }

    ToddCoxeter::options::lookahead ToddCoxeter::lookahead() const noexcept {
      return _settings->lookahead;
    }

    ToddCoxeter& ToddCoxeter::lower_bound(size_t n) noexcept {
      _settings->lower_bound = n;
      return *this;
    }

    size_t ToddCoxeter::lower_bound() const noexcept {
      return _settings->lower_bound;
    }

    ToddCoxeter& ToddCoxeter::next_lookahead(size_t n) noexcept {
      _settings->next_lookahead = n;
      return *this;
    }

    size_t ToddCoxeter::next_lookahead() const noexcept {
      return _settings->next_lookahead;
    }

    ToddCoxeter& ToddCoxeter::min_lookahead(size_t n) noexcept {
      _settings->min_lookahead = n;
      return *this;
    }

    size_t ToddCoxeter::min_lookahead() const noexcept {
      return _settings->min_lookahead;
    }

    ToddCoxeter& ToddCoxeter::standardize(bool x) noexcept {
      _settings->standardize = x;
      return *this;
    }

    bool ToddCoxeter::standardize() const noexcept {
      return _settings->standardize;
    }

    ToddCoxeter& ToddCoxeter::save(bool x) {
      if ((_prefilled
           || (has_parent_froidure_pin()
               && parent_froidure_pin()->is_finite() == tril::TRUE
               && (_settings->froidure_pin == options::froidure_pin::none
                   || _settings->froidure_pin
                          == options::froidure_pin::use_cayley_graph)))
          && x) {
        LIBSEMIGROUPS_EXCEPTION("cannot use the save setting with a "
                                "prefilled ToddCoxeter instance");
      }
      _settings->save = x;
      return *this;
    }

    bool ToddCoxeter::save() const noexcept {
      return _settings->save;
    }

    ToddCoxeter& ToddCoxeter::strategy(options::strategy x) {
      if ((_prefilled
           || (has_parent_froidure_pin()
               && parent_froidure_pin()->is_finite() == tril::TRUE
               && (_settings->froidure_pin == options::froidure_pin::none
                   || _settings->froidure_pin
                          == options::froidure_pin::use_cayley_graph)))
          && x == options::strategy::felsch) {
        LIBSEMIGROUPS_EXCEPTION("cannot use the Felsch strategy with a "
                                "prefilled ToddCoxeter instance");
      }
      _settings->strategy = x;
      return *this;
    }

    ToddCoxeter::options::strategy ToddCoxeter::strategy() const noexcept {
      return _settings->strategy;
    }

    ToddCoxeter&
    ToddCoxeter::random_interval(std::chrono::nanoseconds x) noexcept {
      _settings->random_interval = x;
      return *this;
    }

    std::chrono::nanoseconds ToddCoxeter::random_interval() const noexcept {
      return _settings->random_interval;
    }

    ToddCoxeter& ToddCoxeter::use_relations_in_extra(bool val) noexcept {
      _settings->use_relations_in_extra = val;
      return *this;
    }

    bool ToddCoxeter::use_relations_in_extra() const noexcept {
      return _settings->use_relations_in_extra;
    }

    ToddCoxeter& ToddCoxeter::deduction_policy(options::deductions val) {
      using int_type = std::underlying_type_t<ToddCoxeter::options::deductions>;
      if (static_cast<int_type>(val) % 2 == 0
          || static_cast<int_type>(val) < 4) {
        LIBSEMIGROUPS_EXCEPTION("invalid option %s!",
                                detail::to_string(val).c_str())
      }
      _settings->deduct_policy = val;
      if (val & options::deductions::unlimited) {
        _settings->deduct_max = POSITIVE_INFINITY;
      }
      return *this;
    }

    ToddCoxeter::options::deductions
    ToddCoxeter::deduction_policy() const noexcept {
      return _settings->deduct_policy;
    }

    ToddCoxeter& ToddCoxeter::max_deductions(size_t val) noexcept {
      _settings->deduct_max = val;
      return *this;
    }

    size_t ToddCoxeter::max_deductions() const noexcept {
      return _settings->deduct_max;
    }

    ToddCoxeter&
    ToddCoxeter::preferred_defs(options::preferred_defs val) noexcept {
      if (val == options::preferred_defs::none) {
        // Don't call preferred_defs() here because ends up in infinite loop.
        _settings->max_preferred_defs = 0;
      }
      _settings->preferred_defs = val;
      return *this;
    }

    ToddCoxeter::options::preferred_defs
    ToddCoxeter::preferred_defs() const noexcept {
      return _settings->preferred_defs;
    }

    ToddCoxeter& ToddCoxeter::restandardize(bool val) noexcept {
      _settings->restandardize = val;
      return *this;
    }

    bool ToddCoxeter::restandardize() const noexcept {
      return _settings->restandardize;
    }

    ToddCoxeter& ToddCoxeter::max_preferred_defs(size_t val) noexcept {
      if (val == 0) {
        // Don't call preferred_defs() here because ends up in infinite loop.
        _settings->preferred_defs = options::preferred_defs::none;
      }
      _settings->max_preferred_defs = val;
      return *this;
    }

    size_t ToddCoxeter::max_preferred_defs() const noexcept {
      return _settings->max_preferred_defs;
    }

    ToddCoxeter& ToddCoxeter::hlt_defs(size_t val) {
      if (val < length_of_generating_pairs()) {
        LIBSEMIGROUPS_EXCEPTION("Expected a value >= %llu, found %llu!",
                                uint64_t(length_of_generating_pairs()),
                                uint64_t(val));
      }
      _settings->hlt_defs = val;
      return *this;
    }

    size_t ToddCoxeter::hlt_defs() const noexcept {
      return _settings->hlt_defs;
    }

    ToddCoxeter& ToddCoxeter::f_defs(size_t val) {
      if (val == 0) {
        LIBSEMIGROUPS_EXCEPTION("Expected a value != 0!");
      }
      _settings->f_defs = val;
      return *this;
    }

    size_t ToddCoxeter::f_defs() const noexcept {
      return _settings->f_defs;
    }

    ToddCoxeter& ToddCoxeter::lookahead_growth_factor(float val) {
      if (val < 1.0) {
        LIBSEMIGROUPS_EXCEPTION("Expected a value >= 1.0, found %f", val);
      }
      _settings->lookahead_growth_factor = val;
      return *this;
    }

    float ToddCoxeter::lookahead_growth_factor() const noexcept {
      return _settings->lookahead_growth_factor;
    }

    ToddCoxeter& ToddCoxeter::lookahead_growth_threshold(size_t val) noexcept {
      _settings->lookahead_growth_threshold = val;
      return *this;
    }

    size_t ToddCoxeter::lookahead_growth_threshold() const noexcept {
      return _settings->lookahead_growth_threshold;
    }

    ToddCoxeter& ToddCoxeter::large_collapse(size_t val) noexcept {
      _settings->large_collapse = val;
      return *this;
    }

    size_t ToddCoxeter::large_collapse() const noexcept {
      return _settings->large_collapse;
    }

    // Private settings!

    void ToddCoxeter::push_settings() {
      Settings* prev_settings = _settings.get();
      _settings.release();
      _setting_stack.push(prev_settings);
      _settings = std::make_unique<Settings>(*prev_settings);
    }

    void ToddCoxeter::pop_settings() {
      if (!_setting_stack.empty()) {
        _settings.reset(_setting_stack.top());
        _setting_stack.pop();
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // 8. ToddCoxeter - member functions (sorting gen. pairs etc) - public
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeter& ToddCoxeter::sort_generating_pairs(
        std::function<bool(word_type const&, word_type const&)> func) {
      if (started()) {
        LIBSEMIGROUPS_EXCEPTION(
            "Cannot sort relations, the enumeration has started!")
      }
      init_generating_pairs();
      Perm perm;
      ::sort_generating_pairs(func, perm, _relations);
      ::sort_generating_pairs(func, perm, _extra);
      return *this;
    }

    ToddCoxeter& ToddCoxeter::random_shuffle_generating_pairs() {
      if (started()) {
        LIBSEMIGROUPS_EXCEPTION("Cannot shuffle the generating pairs, the "
                                "enumeration has started!")
      }
      init_generating_pairs();
      Perm perm(0, _relations.size());
      std::iota(perm.begin(), perm.end(), 0);
      std::random_device rd;
      std::mt19937       g(rd());
      std::shuffle(perm.begin(), perm.end(), g);
      ::sort_generating_pairs(perm, _relations);
      ::sort_generating_pairs(perm, _extra);
      return *this;
    }

    ToddCoxeter& ToddCoxeter::remove_duplicate_generating_pairs() {
      if (started()) {
        LIBSEMIGROUPS_EXCEPTION("Cannot remove duplicate generating pairs, the "
                                "enumeration has started!")
      }
      init_generating_pairs();
      std::unordered_set<relation_type, Hash<relation_type>> relations_set;
      for (size_t i = 0; i < _relations.size(); i += 2) {
        if (shortlex_compare(_relations[i], _relations[i + 1])) {
          relations_set.emplace(_relations[i], _relations[i + 1]);
        } else {
          relations_set.emplace(_relations[i + 1], _relations[i]);
        }
      }
      _relations.clear();
      for (auto const& p : relations_set) {
        _relations.push_back(p.first);
        _relations.push_back(p.second);
      }
      std::unordered_set<relation_type, Hash<relation_type>> extra_set;

      for (size_t i = 0; i < _extra.size(); i += 2) {
        if (shortlex_compare(_extra[i], _extra[i + 1])) {
          if (relations_set.emplace(_extra[i], _extra[i + 1]).second) {
            extra_set.emplace(_extra[i], _extra[i + 1]);
          }
        } else {
          if (relations_set.emplace(_extra[i + 1], _extra[i]).second) {
            extra_set.emplace(_extra[i + 1], _extra[i]);
          }
        }
      }
      _extra.clear();
      for (auto const& p : extra_set) {
        _extra.push_back(p.first);
        _extra.push_back(p.second);
      }
      return *this;
    }

    ToddCoxeter& ToddCoxeter::simplify(size_t n) {
      init_generating_pairs();
      if (started()) {
        LIBSEMIGROUPS_EXCEPTION("the enumeration has started, it is no longer "
                                "possible to change the generating pairs!");
      } else if (_prefilled) {
        // There's no actual reason that we cannot do this here, just reducing
        // complexity of introducing this feature. But it would mean making a
        // fundamental change away from assuming that _relations is empty if
        // _prefilled is true. So, I am opting not to do this now.
        LIBSEMIGROUPS_EXCEPTION("the table has been prefilled, it is no longer "
                                "possible to change the generating pairs!");
      }
      if (_felsch_tree != nullptr) {
        _felsch_tree = nullptr;
      }
      PairsSimplifier p;
      // Replace words in _relations with min. equiv. word in _relations
      p.add(_relations).apply(_relations);
      p.add(_extra).apply(_extra);
      remove_duplicate_generating_pairs();
      // Reduce the length of the presentation by introducing new generators,
      // until either we reach n, or we cannot reduce the presentation any
      // further
      for (size_t i = 0; i < n; ++i) {
        if (!reduce_length_once()) {
          return *this;
        }
      }
      return *this;
    }

    bool ToddCoxeter::reduce_length_once() {
      LIBSEMIGROUPS_ASSERT(_felsch_tree == nullptr);
      using detail::SuffixTree;
      using const_iterator_word = typename detail::DFSHelper::const_iterator;

      if (_relations.empty() && _extra.empty()) {
        return false;
      }
      SuffixTree st;
      detail::suffix_tree_helper::add_words(st, _relations);
      detail::suffix_tree_helper::add_words(st, _extra);

      detail::DFSHelper   helper(st);
      const_iterator_word first, last;
      // Get the best word [first, last) so that replacing every
      // non-overlapping occurrence of [first, last) in _relations and
      // _extra with a new generator "x", and adding "x = [first, last)" as
      // a relation reduces the length of the presentation as much as
      // possible.
      std::tie(first, last) = st.dfs(helper);
      if (first == last) {
        return false;  // no change
      }
      letter_type const x = number_of_generators();
      add_generators(1);
      auto replace_subword = [&first, &last, &x](word_type& word) {
        auto it = std::search(word.begin(), word.end(), first, last);
        while (it != word.end()) {
          // found [first, last)
          *it      = x;
          auto pos = it - word.begin();
          word.erase(it + 1, it + (last - first));  // it not valid
          it = std::search(word.begin() + pos + 1, word.end(), first, last);
        }
      };
      std::for_each(_relations.begin(), _relations.end(), replace_subword);
      _relations.emplace_back(word_type({x}));
      _relations.emplace_back(first, last);
      std::for_each(_extra.begin(), _extra.end(), replace_subword);
      return true;
    }

    ////////////////////////////////////////////////////////////////////////
    // 9. ToddCoxeter - member functions (container-like) - public
    ////////////////////////////////////////////////////////////////////////

    bool ToddCoxeter::empty() const {
      return _relations.empty() && _extra.empty()
             && number_of_cosets_active() == 1;
    }

    void ToddCoxeter::reserve(size_t n) {
      size_t m = coset_capacity();
      if (n > m) {
        m = n - m;
        _word_graph.add_nodes(m);
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

      _word_graph.shrink_to_fit(number_of_cosets_active());
      _relations.clear();
      _relations.shrink_to_fit();
      _extra.clear();
      _extra.shrink_to_fit();
      erase_free_cosets();
    }

    ////////////////////////////////////////////////////////////////////////
    // 10. ToddCoxeter - member functions (state) - public
    ////////////////////////////////////////////////////////////////////////

    bool ToddCoxeter::complete() const noexcept {
      size_t const n = number_of_generators();
      coset_type   c = _id_coset;
      while (c != first_free_coset()) {
        for (size_t a = 0; a < n; ++a) {
          if (_word_graph.unsafe_neighbor(c, a) == UNDEFINED) {
            return false;
          }
        }
        c = next_active_coset(c);
      }
      return true;
    }

    template <typename T>
    bool ToddCoxeter::compatible(coset_type c, T first, T last) const {
      for (auto it = first; it < last; it += 2) {
        coset_type x = action_digraph_helper::follow_path_nc(
            _word_graph, c, (*it).cbegin(), (*it).cend());
        LIBSEMIGROUPS_ASSERT(is_active_coset(x) || x == UNDEFINED);
        coset_type y = action_digraph_helper::follow_path_nc(
            _word_graph, c, (*(it + 1)).cbegin(), (*(it + 1)).cend());
        LIBSEMIGROUPS_ASSERT(is_active_coset(y) || y == UNDEFINED);
        if (x != y || x == UNDEFINED) {
          return false;
        }
      }
      return true;
    }

    bool ToddCoxeter::compatible() const noexcept {
      coset_type c = _id_coset;
      if (!compatible(c, _extra.cbegin(), _extra.cend())) {
        return false;
      }
      while (c != first_free_coset()) {
        if (!compatible(c, _relations.cbegin(), _relations.cend())) {
          return false;
        }
        c = next_active_coset(c);
      }
      return true;
    }

    size_t ToddCoxeter::length_of_generating_pairs() {
      init_generating_pairs();
      auto   op = [](size_t val, word_type const& x) { return val + x.size(); };
      size_t N  = std::accumulate(
          _relations.cbegin(), _relations.cend(), size_t(0), op);
      N += std::accumulate(_extra.cbegin(), _extra.cend(), size_t(0), op);
      return N;
    }

    size_t ToddCoxeter::felsch_tree_height() {
      init_generating_pairs();
      init_run();
      init_felsch_tree();
      return _felsch_tree->height();
    }

    size_t ToddCoxeter::felsch_tree_number_of_nodes() {
      init_generating_pairs();
      init_run();
      init_felsch_tree();
      return _felsch_tree->number_of_nodes();
    }

    ////////////////////////////////////////////////////////////////////////
    // 11. ToddCoxeter - member functions (standardization) - public
    ////////////////////////////////////////////////////////////////////////

    bool ToddCoxeter::is_standardized() const noexcept {
      return _standardized != order::none;
    }

    ToddCoxeter::order ToddCoxeter::standardization_order() const noexcept {
      return _standardized;
    }

    bool ToddCoxeter::standardize(order rdr) {
      if (_standardized == rdr || empty()) {
        return false;
      }
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      LIBSEMIGROUPS_ASSERT(_deduct->empty());
      bool result = false;
      switch (rdr) {
        case order::shortlex:
          init_standardize();
          result = shortlex_standardize();
          break;
        case order::lex:
          init_standardize();
          result = lex_standardize();
          break;
        case order::recursive:
          init_standardize();
          result = recursive_standardize();
          break;
        case order::none:
        default:
          break;
      }
      if (finished()) {
        _standardized = rdr;
      } else {
        _standard_max = number_of_cosets_active();
      }
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // 12. ToddCoxeter - member functions (attributes) - public
    ////////////////////////////////////////////////////////////////////////

    tril ToddCoxeter::is_non_trivial(size_t                    tries,
                                     std::chrono::milliseconds try_for,
                                     float                     threshold) {
      if (is_quotient_obviously_infinite()) {
        return tril::TRUE;
      } else if (finished()) {
        return number_of_classes() == 1 ? tril::FALSE : tril::TRUE;
      }

      detail::Timer             tmr;
      static std::random_device rd;
      static std::mt19937       g(rd());
      for (size_t try_ = 0; try_ < tries; ++try_) {
        REPORT_DEFAULT(
            "trying to show non-triviality: %d / %d\n", try_ + 1, tries);
        ToddCoxeter tc(*this);
        tc.init_felsch_tree();
        tc.standardize(true);
        tc.save(true);
        while (!tc.finished()) {
          tc.run_for(try_for);
          size_t limit = tc.number_of_cosets_active();
          while (tc.number_of_cosets_active() >= threshold * limit
                 && !tc.finished()) {
            std::uniform_int_distribution<> d(0,
                                              tc.number_of_cosets_active() - 1);
            size_t                          N  = d(g);
            auto                            c1 = tc._id_coset;
            for (size_t i = 0; i < N; ++i) {
              c1 = tc.next_active_coset(c1);
            }
            N       = d(g);
            auto c2 = tc._id_coset;
            for (size_t i = 0; i < N; ++i) {
              c2 = tc.next_active_coset(c2);
            }
            tc._coinc.emplace(c1, c2);
            tc.process_coincidences(stack_deductions::yes);
            tc.process_deductions();
            tc.run_for(try_for);
          }
        }
        if (tc.number_of_classes() > 1) {
          REPORT_DEFAULT("successfully showed non-triviality!\n");
          report_time(__func__, tmr);
          return tril::TRUE;
        }
      }
      REPORT_DEFAULT("failed to show non-triviality!\n");
      report_time(__func__, tmr);
      return tril::unknown;
    }

    ////////////////////////////////////////////////////////////////////////
    // 13. ToddCoxeter - member functions (reporting + stats) - public
    ////////////////////////////////////////////////////////////////////////

    std::string ToddCoxeter::stats_string() const {
      detail::PrintTable pt;
      pt.header("Summary of statistics");
      pt("Number of generators:", "%'14llu", uint64_t(number_of_generators()));
      pt("Number of relations:", "%'14llu", uint64_t(_relations.size() / 2));
      pt("Number of generating pairs:", "%'14llu", uint64_t(_extra.size() / 2));
      pt("Total length of generating pairs:",
         "%'14llu",
         uint64_t(
             const_cast<ToddCoxeter*>(this)->length_of_generating_pairs()));
      pt.divider();

      pt("cosets defined (hlt)", "%'14llu", uint64_t(_stats.tc1_hlt_appl));
      pt("cosets defined (felsch)", "%'14llu", uint64_t(_stats.tc1_f_appl));
      pt("total cosets defined",
         "%'14llu",
         uint64_t(number_of_cosets_defined() - 2));
      pt("coset capacity", "%'14llu", uint64_t(coset_capacity()));
      pt("cosets killed", "%'14llu", uint64_t(number_of_cosets_killed()));
      pt("killed / defined",
         "%13f%%",
         static_cast<float>(100 * number_of_cosets_killed())
             / number_of_cosets_defined());
      pt("defined / capacity",
         "%13f%%",
         static_cast<float>(100 * number_of_cosets_defined())
             / coset_capacity());

#ifdef LIBSEMIGROUPS_ENABLE_STATS
      pt.divider();
      pt("relations pushed (good)", "%'14llu", uint64_t(_stats.tc2_good_appl));
      pt("relations pushed (total)", "%'14llu", uint64_t(_stats.tc2_appl));
      pt("relations pushed (% good)",
         "%13f%%",
         static_cast<double>(100 * _stats.tc2_good_appl) / _stats.tc2_appl);

      pt.divider();
      pt("calls to process_coincidences", "%'14llu", uint64_t(_stats.tc3_appl));
      pt("maximum coincidences", "%'14llu", uint64_t(_stats.max_coinc));
      pt("total active coincidences",
         "%'14llu",
         uint64_t(_stats.nr_active_coinc));
      pt("total coincidences", "%'14llu", uint64_t(_stats.total_coinc));

      pt.divider();
      pt("calls to process_deductions",
         "%'14llu",
         uint64_t(_stats.total_deduct));
      pt("maximum deductions", "%'14llu", uint64_t(_stats.max_deduct));
      pt("total active deductions",
         "%'14llu",
         uint64_t(_stats.nr_active_deduct));
      pt("total deductions", "%'14llu", uint64_t(_stats.total_deduct));

      pt.divider();
      pt("maximum preferred defs",
         "%'14llu",
         uint64_t(_stats.max_preferred_defs));
      pt("total active preferred defs",
         "%'14llu",
         uint64_t(_stats.nr_active_preferred_defs));
      pt("total preferred defs",
         "%'14llu",
         uint64_t(_stats.total_preferred_defs));

      pt.divider();
      pt("calls to lookahead (hlt)",
         "%'14llu",
         uint64_t(_stats.hlt_lookahead_calls));
      pt("calls to lookahead (felsch)",
         "%'14llu",
         uint64_t(_stats.f_lookahead_calls));
#endif
      pt.footer("End of summary");
      return pt.emit();
    }

    std::string ToddCoxeter::settings_string() const {
      detail::PrintTable pt;
      pt.header("Summary of settings (Todd-Coxeter algorithm)");
      pt("deduction_policy:", deduction_policy());
      pt("f_defs:", "%'14llu", uint64_t(f_defs()));
      pt("froidure_pin_policy:", froidure_pin_policy());
      pt("hlt_defs:", "%'14llu", uint64_t(hlt_defs()));
      pt("lookahead:", lookahead());
      pt("lookahead_growth_factor:", "%14f", lookahead_growth_factor());
      pt("lookahead_growth_threshold:",
         "%14llu",
         uint64_t(lookahead_growth_threshold()));

      if (lower_bound() == UNDEFINED) {
        pt("lower_bound:", "\u221E");
      } else {
        pt("lower_bound:", "%'14llu", uint64_t(lower_bound()));
      }
      pt("max_deductions:", "%'14llu", uint64_t(max_deductions()));
      pt("max_preferred_defs:", "%'14llu", uint64_t(max_preferred_defs()));
      pt("next_lookahead:", "%'14llu", uint64_t(next_lookahead()));
      pt("preferred_defs:", preferred_defs());
      pt("random_interval:", detail::Timer::string(random_interval()));
      pt("save: ", save() ? "true" : "false");
      pt("standardize:", standardize() ? "true" : "false");
      pt("strategy: ", strategy());
      pt("use_relations_in_extra:",
         use_relations_in_extra() ? "true" : "false");
      pt.footer("End of summary");
      return pt.emit();
    }

    std::string ToddCoxeter::to_gap_string() {
      std::string const alphabet
          = "abcdefghijklmnopqrstuvwxyzABCDFGHIJKLMNOPQRSTUVWY";
      if (number_of_generators() > 49) {
        LIBSEMIGROUPS_EXCEPTION("expected at most 49 generators, found %llu!",
                                uint64_t(number_of_generators()));
      }
      init_generating_pairs();

      auto to_gap_word = [&alphabet](word_type const& w) -> std::string {
        std::string out;
        for (auto it = w.cbegin(); it < w.cend() - 1; ++it) {
          out += alphabet[*it];
          out += " * ";
        }
        out += alphabet[w.back()];
        return out;
      };

      std::string out = "free := FreeSemigroup(";
      for (size_t i = 0; i < number_of_generators(); ++i) {
        out += std::string("\"") + alphabet[i] + "\"";
        if (i != number_of_generators() - 1) {
          out += ", ";
        }
      }
      out += ");\n";
      out += "DoAssignGenVars(GeneratorsOfSemigroup(free));\n";
      out += "rules := [\n";
      for (auto it = _relations.cbegin(); it < _relations.cend(); it += 2) {
        out += "          [";
        out += to_gap_word(*it);
        out += ", ";
        out += to_gap_word(*(it + 1));
        if (it != _relations.cend() - 2) {
          out += "],\n";
        } else {
          out += "]\n";
        }
      }
      for (auto it = _extra.cbegin(); it < _extra.cend(); it += 2) {
        out += "          [";
        out += to_gap_word(*it);
        out += ", ";
        out += to_gap_word(*(it + 1));
        if (it != _extra.cend() - 2) {
          out += "],\n";
        } else {
          out += "]\n";
        }
      }
      out += "         ];\n";
      out += "S := free / rules;\n";
      return out;
    }

    ////////////////////////////////////////////////////////////////////////
    // 14. ToddCoxeter - member functions (reporting + stats) - private
    ////////////////////////////////////////////////////////////////////////

    namespace {
      std::string fmt_line() {
#if defined(LIBSEMIGROUPS_FMT_ENABLED) && defined(LIBSEMIGROUPS_ENABLE_STATS)
        return "\t{:12L} {:+12L} ({})\n";
#elif defined(LIBSEMIGROUPS_FMT_ENABLED) && !defined(LIBSEMIGROUPS_ENABLE_STATS)
        return "\t{:12L} {:>12} ({})\n";
#elif !defined(LIBSEMIGROUPS_FMT_ENABLED) && defined(LIBSEMIGROUPS_ENABLE_STATS)
        return "\t%'12llu %+'12lld (%s)\n";
#elif !defined(LIBSEMIGROUPS_FMT_ENABLED) \
    && !defined(LIBSEMIGROUPS_ENABLE_STATS)
        return "\t%'12llu %12s (%s)\n";
#endif
      }
    }  // namespace

#ifdef LIBSEMIGROUPS_ENABLE_STATS
    void ToddCoxeter::report_coincidences(char const* fnam) {
      if (report::should_report()) {
        REPORT_DEFAULT(FORMAT("coincidences:" + fmt_line(),
                              _coinc.size(),
                              int64_t(_coinc.size() - _stats.prev_coincidences),
                              fnam));
        _stats.prev_coincidences = _coinc.size();
      }
    }
#else
    void ToddCoxeter::report_coincidences(char const* fnam) {
      REPORT_DEFAULT(
          FORMAT("coincidences:" + fmt_line(), _coinc.size(), "", fnam));
    }
#endif

#ifdef LIBSEMIGROUPS_ENABLE_STATS
    void ToddCoxeter::report_active_cosets(char const* fnam) {
      if (report::should_report()) {
        REPORT_DEFAULT(FORMAT(
            "active cosets:" + fmt_line(),
            number_of_cosets_active(),
            int64_t(number_of_cosets_active() - _stats.prev_active_cosets),
            fnam));
        _stats.prev_active_cosets = number_of_cosets_active();
      }
    }
#else
    void ToddCoxeter::report_active_cosets(char const* fnam) {
      REPORT_DEFAULT(FORMAT(
          "active cosets:" + fmt_line(), number_of_cosets_active(), "", fnam));
    }
#endif

    void ToddCoxeter::report_cosets_killed(char const* fnam, int64_t N) const {
      if (report::should_report()) {
#ifdef LIBSEMIGROUPS_FMT_ENABLED
        std::string fmt = "\t{:>12} {:+12L} ({})\n";
#else
        std::string fmt = "\t%12s %+'12lld (%s)\n";
#endif
        REPORT_DEFAULT(FORMAT("cosets killed:" + fmt, "", -1 * N, fnam));
      }
    }

    void ToddCoxeter::report_inc_lookahead(char const* fnam,
                                           size_t      new_value) const {
      if (report::should_report()) {
#if defined(LIBSEMIGROUPS_FMT_ENABLED)
        std::string fmt = "\t{:12L} {:+12L} ({})\n";
#else
        std::string fmt = "\t%'12llu %+'12lld (%s)\n";
#endif
        REPORT_DEFAULT(FORMAT("lookahead at:" + fmt,
                              new_value,
                              int64_t(new_value - next_lookahead()),
                              fnam));
      }
    }

    void ToddCoxeter::report_time(char const* fnam, detail::Timer& t) const {
      if (report::should_report()) {
        auto   tt    = t.string();
        size_t width = 12;
        // Check if we contain a \mu
        if (tt.find("\u03BC") != std::string::npos) {
          width = 13;
        }
#ifdef LIBSEMIGROUPS_FMT_ENABLED
        std::string fmt = "\t{:>" + std::to_string(width) + "} {:>{}} ({})\n";
        REPORT_DEFAULT(FORMAT("elapsed time:" + fmt, tt.c_str(), "", 12, fnam));
#else
        std::string fmt = "\t%" + std::to_string(width) + "s %*s (%s)\n";
        REPORT_DEFAULT(FORMAT("elapsed time:" + fmt, tt.c_str(), 12, "", fnam));
#endif
      }
    }

    // Cannot test this
    void ToddCoxeter::report_at_coset(char const* fnam, size_t N) const {
      if (report::should_report()) {
#ifdef LIBSEMIGROUPS_FMT_ENABLED
        std::string fmt = "\t{:12L} {:12L} ({})\n";
#else
        std::string fmt = "\t%'12llu %'12lld (%s)\n";
#endif
        REPORT_DEFAULT(
            FORMAT("at coset:" + fmt, N, number_of_cosets_active(), fnam));
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // 15. ToddCoxeter - member functions (validation) - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::validate_table(table_type const& table,
                                     size_t const      first,
                                     size_t const      last) const {
      REPORT_DEBUG_DEFAULT("validating coset table...\n");
      if (number_of_generators() == UNDEFINED) {
        LIBSEMIGROUPS_EXCEPTION("no generators have been defined");
      } else if (table.number_of_cols() != number_of_generators()) {
        LIBSEMIGROUPS_EXCEPTION("invalid table, expected %d columns, found %d",
                                number_of_generators(),
                                table.number_of_cols());
      } else if (last - first == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid table, expected at least 1 rows, found 0!");
      }
      for (size_t i = first; i < last; ++i) {
        for (size_t j = 0; j < table.number_of_cols(); ++j) {
          coset_type c = table.get(i, j);
          if (c < first || c >= last) {
            LIBSEMIGROUPS_EXCEPTION("invalid table, expected entries in the "
                                    "range [%d, %d), found "
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
    // 16. ToddCoxeter - member functions (initialisation) - private
    ////////////////////////////////////////////////////////////////////////

    //! Copy all _relations and _extra from copy into _relations of this
    void ToddCoxeter::copy_relations_for_quotient(ToddCoxeter& copy) {
      REPORT_DEBUG_DEFAULT("copying relations for quotient...\n");
      LIBSEMIGROUPS_ASSERT(empty());
      copy.init_generating_pairs();
      if (copy.number_of_generators() == UNDEFINED) {
        return;
      }
      set_number_of_generators(copy.number_of_generators());
      _state     = state::relation_extra_initialized;
      _relations = copy._relations;
      _relations.insert(
          _relations.end(), copy._extra.cbegin(), copy._extra.cend());
      if (kind() == congruence_kind::left
          && copy.kind() != congruence_kind::left) {
        for (auto& w : _relations) {
          std::reverse(w.begin(), w.end());
        }
      }
      _nr_pairs_added_earlier = 0;
    }

    void ToddCoxeter::init_generating_pairs() {
      if (_state == state::constructed) {
        REPORT_DEBUG_DEFAULT("initializing...\n");
        // Add the relations/Cayley graph from parent() if any.
        if (has_parent_froidure_pin()
            && parent_froidure_pin()->is_finite() == tril::TRUE) {
          if (froidure_pin_policy() == options::froidure_pin::use_cayley_graph
              || froidure_pin_policy() == options::froidure_pin::none) {
            REPORT_DEBUG_DEFAULT("using Cayley graph...\n");
            LIBSEMIGROUPS_ASSERT(_relations.empty());
            prefill(*parent_froidure_pin());
#ifdef LIBSEMIGROUPS_DEBUG
            // This is a check of program logic, since we use parent() to fill
            // the table, so we only validate in debug mode.
            validate_table(
                _word_graph.table(), 1, parent_froidure_pin()->size() + 1);
#endif
          } else {
            REPORT_DEBUG_DEFAULT("using presentation...\n");
            LIBSEMIGROUPS_ASSERT(froidure_pin_policy()
                                 == options::froidure_pin::use_relations);
            auto fp = parent_froidure_pin();
            fp->run();
            for (auto it = fp->cbegin_rules(); it != fp->cend_rules(); ++it) {
              reverse_if_necessary_and_push_back(this, it->first, _relations);
              reverse_if_necessary_and_push_back(this, it->second, _relations);
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
        _state = state::relation_extra_initialized;
      }

      // Get new generating pairs (if any) from CongruenceInterface (if any)
      auto it = cbegin_generating_pairs() + _nr_pairs_added_earlier;
      if (kind() != congruence_kind::twosided) {
        for (; it < cend_generating_pairs(); ++it) {
          reverse_if_necessary_and_push_back(this, it->first, _extra);
          reverse_if_necessary_and_push_back(this, it->second, _extra);
        }
      } else {
        for (; it < cend_generating_pairs(); ++it) {
          reverse_if_necessary_and_push_back(this, it->first, _relations);
          reverse_if_necessary_and_push_back(this, it->second, _relations);
        }
      }
      _nr_pairs_added_earlier
          = cend_generating_pairs() - cbegin_generating_pairs();
    }

    void ToddCoxeter::init_felsch_tree() {
      LIBSEMIGROUPS_ASSERT(_state >= state::relation_extra_initialized);
      if (_felsch_tree == nullptr) {
        REPORT_DEFAULT("initializing the Felsch tree...\n");
        detail::Timer tmr;
        _felsch_tree = std::make_unique<FelschTree>(number_of_generators());
        _felsch_tree->add_relations(_relations.cbegin(), _relations.cend());
        REPORT_DEFAULT("Felsch tree has %llu nodes + height %llu\n",
                       _felsch_tree->number_of_nodes(),
                       _felsch_tree->height());
        report_time(__func__, tmr);
      }
    }

    // Initialization
    void ToddCoxeter::prefill(table_type const&             table,
                              std::function<size_t(size_t)> generator_to_row) {
      prefill_and_validate(table, true, generator_to_row);
    }

    void ToddCoxeter::prefill(FroidurePinBase& S) {
      REPORT_DEBUG_DEFAULT("prefilling the coset table from FroidurePin...\n");
      LIBSEMIGROUPS_ASSERT(_state == state::constructed);
      LIBSEMIGROUPS_ASSERT(
          froidure_pin_policy() == options::froidure_pin::use_cayley_graph
          || froidure_pin_policy() == options::froidure_pin::none);
      LIBSEMIGROUPS_ASSERT(S.number_of_generators() == number_of_generators());
      auto generator_to_row = [&S](size_t i) { return S.current_position(i); };
      if (kind() == congruence_kind::left) {
        prefill_and_validate(S.left_cayley_graph(), false, generator_to_row);
      } else {
        prefill_and_validate(S.right_cayley_graph(), false, generator_to_row);
      }
    }

    void ToddCoxeter::prefill_and_validate(
        table_type const&             table,
        bool                          validate,
        std::function<size_t(size_t)> generator_to_row) {
      if (strategy() == options::strategy::felsch) {
        LIBSEMIGROUPS_EXCEPTION(
            "it is not possible to prefill when using the Felsch strategy");
      } else if (!empty()) {
        LIBSEMIGROUPS_EXCEPTION("cannot prefill a non-empty instance")
      }
      if (validate) {
        validate_table(table, 0, table.number_of_rows());
      }

      REPORT_DEBUG("prefilling the coset table...\n");
      _prefilled = true;
      size_t m   = table.number_of_rows() + 1;
      add_active_cosets(m - number_of_cosets_active());
      _word_graph.add_nodes(m - _word_graph.number_of_nodes());
      for (size_t i = 0; i < _word_graph.out_degree(); i++) {
        def_edge<DoNotStackDeductions>(0, i, generator_to_row(i) + 1);
      }
      for (size_t row = 0; row < _word_graph.number_of_nodes() - 1; ++row) {
        for (size_t col = 0; col < _word_graph.out_degree(); ++col) {
          def_edge<DoNotStackDeductions>(row + 1, col, table.get(row, col) + 1);
        }
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // 17. ToddCoxeter - member functions (cosets) - private
    ////////////////////////////////////////////////////////////////////////

    coset_type ToddCoxeter::new_coset() {
      if (has_free_cosets()) {
        coset_type const c = new_active_coset();
        // Clear the new coset's row in each table
        _word_graph.clear_sources_and_targets(c);
        return c;
      } else {
        reserve(2 * coset_capacity());
        return new_active_coset();
      }
    }

    template <typename TStackDeduct>
    coset_type ToddCoxeter::def_edges(coset_type                c,
                                      word_type::const_iterator first,
                                      word_type::const_iterator last) noexcept {
      word_type::const_iterator it;

      std::tie(c, it) = action_digraph_helper::last_node_on_path_nc(
          _word_graph, c, first, last);
      _stats.tc1_hlt_appl += std::distance(it, last);
      for (; it < last; ++it) {
        LIBSEMIGROUPS_ASSERT(_word_graph.unsafe_neighbor(c, *it) == UNDEFINED);
        coset_type d = new_coset();
        def_edge<TStackDeduct>(c, *it, d);
        c = d;
      }
      return c;
    }

    template <typename TStackDeduct, typename TProcessCoincide>
    void ToddCoxeter::push_definition_hlt(coset_type const& c,
                                          word_type const&  u,
                                          word_type const&  v) noexcept {
      LIBSEMIGROUPS_ASSERT(is_active_coset(c));
      LIBSEMIGROUPS_ASSERT(!u.empty());
      LIBSEMIGROUPS_ASSERT(!v.empty());
      coset_type const x = def_edges<TStackDeduct>(c, u.cbegin(), u.cend() - 1);
      coset_type const y = def_edges<TStackDeduct>(c, v.cbegin(), v.cend() - 1);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
      _stats.tc2_appl += 2;
#endif

      push_definition<TStackDeduct,
                      TProcessCoincide,
                      ImmediateDef<TStackDeduct>>(x, u.back(), y, v.back());
    }

    template <typename TStackDeduct,
              typename TProcessCoincide,
              typename TPreferredDef>
    void ToddCoxeter::push_definition_felsch(coset_type const& c,
                                             word_type const&  u,
                                             word_type const&  v) noexcept {
      LIBSEMIGROUPS_ASSERT(is_active_coset(c));
      LIBSEMIGROUPS_ASSERT(!u.empty());
      LIBSEMIGROUPS_ASSERT(!v.empty());
      coset_type x = action_digraph_helper::follow_path_nc(
          _word_graph, c, u.cbegin(), u.cend() - 1);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
      _stats.tc2_appl++;
#endif
      if (x == UNDEFINED) {
        return;
      }
      coset_type y = action_digraph_helper::follow_path_nc(
          _word_graph, c, v.cbegin(), v.cend() - 1);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
      _stats.tc2_appl++;
#endif
      if (y == UNDEFINED) {
        return;
      }
      push_definition<TStackDeduct, TProcessCoincide, TPreferredDef>(
          x, u.back(), y, v.back());
    }

    template <typename TStackDeduct,
              typename TProcessCoincide,
              typename TPreferredDef>
    void ToddCoxeter::push_definition(coset_type  x,
                                      letter_type a,
                                      coset_type  y,
                                      letter_type b) {
      LIBSEMIGROUPS_ASSERT(is_valid_coset(x));
      LIBSEMIGROUPS_ASSERT(is_valid_coset(y));
      LIBSEMIGROUPS_ASSERT(a < number_of_generators());
      LIBSEMIGROUPS_ASSERT(b < number_of_generators());

      coset_type const xa = _word_graph.unsafe_neighbor(x, a);
      coset_type const yb = _word_graph.unsafe_neighbor(y, b);

      if (xa == UNDEFINED && yb != UNDEFINED) {
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        _stats.tc2_good_appl++;
#endif
        def_edge<TStackDeduct>(x, a, yb);
      } else if (xa != UNDEFINED && yb == UNDEFINED) {
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        _stats.tc2_good_appl++;
#endif
        def_edge<TStackDeduct>(y, b, xa);
      } else if (xa != UNDEFINED && yb != UNDEFINED && xa != yb) {
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        _stats.tc2_good_appl++;
#endif
        _coinc.emplace(xa, yb);
        TProcessCoincide()(this);
      } else if (xa == UNDEFINED && yb == UNDEFINED) {
        // We discover that we are one letter away from being able
        // to follow the paths labelled u and v from some node. I.e.
        // u = u_1a and v = v_1b and u_1 and v_1 label (c, d)- and
        // (c, e)-paths but u and v don't label any paths starting
        // at c (i.e there are no edges labelled a incident to d nor
        // labelled b incident to e. This would make the (d, a) and
        // (e, b) "preferred" definitions, or make an immediate definition or
        // do nothing.
        TPreferredDef()(this, x, a, y, b);
      }
    }

    void ToddCoxeter::process_coincidences(stack_deductions val) {
      if (_coinc.empty()) {
        return;
      }
#ifdef LIBSEMIGROUPS_ENABLE_STATS
      _stats.tc3_appl++;
#endif
      std::function<void(coset_type, letter_type)> new_edge_func;
      if (val == stack_deductions::no) {
        new_edge_func = [](coset_type, letter_type) {};
      } else {
        new_edge_func = [this](coset_type c, letter_type x) {
          this->_deduct->emplace(c, x);
        };
      }
      auto incompat_func
          = [this](coset_type c, coset_type d) { this->_coinc.emplace(c, d); };
      while (!_coinc.empty() && _coinc.size() < large_collapse()) {
        Coincidence c = _coinc.top();
        _coinc.pop();
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        _stats.total_coinc++;
#endif
        coset_type min = find_coset(c.first);
        coset_type max = find_coset(c.second);
        if (min != max) {
#ifdef LIBSEMIGROUPS_ENABLE_STATS
          _stats.nr_active_coinc++;
#endif
          if (min > max) {
            std::swap(min, max);
          }
          union_cosets(min, max);
          _word_graph.merge_nodes(min, max, new_edge_func, incompat_func);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
          _stats.max_coinc = std::max(_stats.max_coinc,
                                      static_cast<uint64_t>(_coinc.size()));
#endif
        }
      }

      if (report() || !_coinc.empty()) {
        if (!_coinc.empty()) {
          REPORT_DEFAULT("large collapse detected!\n");
          report_coincidences(__func__);
        }
        report_active_cosets(__func__);
      }

      bool large_collapse = !_coinc.empty();

      while (!_coinc.empty()) {
        Coincidence c = _coinc.top();
        _coinc.pop();
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        _stats.total_coinc++;
#endif
        coset_type min = find_coset(c.first);
        coset_type max = find_coset(c.second);
        if (min != max) {
#ifdef LIBSEMIGROUPS_ENABLE_STATS
          _stats.nr_active_coinc++;
#endif
          if (min > max) {
            std::swap(min, max);
          }
          union_cosets(min, max);
          for (letter_type i = 0; i < number_of_generators(); ++i) {
            coset_type const v = _word_graph.unsafe_neighbor(max, i);
            if (v != UNDEFINED) {
              coset_type const u = _word_graph.unsafe_neighbor(min, i);
              if (u == UNDEFINED) {
                _word_graph.ActionDigraph<coset_type>::add_edge_nc(min, v, i);
              } else if (u != v) {
                // Add (u,v) to the stack of pairs to be identified
                _coinc.emplace(u, v);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
                _stats.max_coinc = std::max(
                    _stats.max_coinc, static_cast<uint64_t>(_coinc.size()));
#endif
              }
            }
          }
        }
        if (report()) {
          report_coincidences(__func__);
          report_active_cosets(__func__);
        }
      }
      if (large_collapse) {
        {
          // Remove all sources of all remaining active cosets
          auto c = _id_coset;
          while (c != first_free_coset()) {
            _word_graph.clear_sources(c);
            c = next_active_coset(c);
          }
        }
        {
          REPORT_DEFAULT("Rebuilding table sources...\n");
          auto   c = _id_coset;
          size_t m = 0;

          while (c != first_free_coset()) {
            m++;
            for (letter_type x = 0; x < number_of_generators(); ++x) {
              auto cx = _word_graph.unsafe_neighbor(c, x);
              if (cx != UNDEFINED) {
                auto d = find_coset(cx);
                if (cx != d) {
                  new_edge_func(c, x);
                  _word_graph.ActionDigraph<coset_type>::add_edge_nc(c, d, x);
                }
                // Must readd the source, even if we don't need to reset
                // the target or stack the deduction
                _word_graph.add_source(d, x, c);
                LIBSEMIGROUPS_ASSERT(is_active_coset(d));
              }
            }
            c = next_active_coset(c);
            if (report()) {
              report_at_coset(__func__, m);
            }
          }
        }
      }
      if (report() || large_collapse) {
        report_active_cosets(__func__);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // 18. ToddCoxeter - member functions (main strategies) - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::init_run() {
      LIBSEMIGROUPS_ASSERT(_state >= state::relation_extra_initialized);
      if (_state == state::relation_extra_initialized) {
        if (save() || strategy() == options::strategy::felsch) {
          for (auto it = _extra.cbegin(); it < _extra.cend(); it += 2) {
            push_definition_hlt<StackDeductions,
                                ProcessCoincidences<stack_deductions::yes>>(
                _id_coset, *it, *(it + 1));
          }
        } else {
          for (auto it = _extra.cbegin(); it < _extra.cend(); it += 2) {
            push_definition_hlt<DoNotStackDeductions,
                                ProcessCoincidences<stack_deductions::no>>(
                _id_coset, *it, *(it + 1));
          }
        }
        if (strategy() == options::strategy::felsch
            && use_relations_in_extra()) {
          for (auto it = _relations.cbegin(); it < _relations.cend(); it += 2) {
            push_definition_hlt<StackDeductions,
                                ProcessCoincidences<stack_deductions::yes>>(
                _id_coset, *it, *(it + 1));
          }
        }
        if (!_prefilled && _relations.empty()) {
          _extra.swap(_relations);
        }
        if (save() || strategy() == options::strategy::felsch) {
          init_felsch_tree();
          process_deductions();
        }
        if (standardize()) {
          // There are no deductions so we don't have to keep track of whether
          // or not we have made any changes here.
          for (letter_type x = 0; x < number_of_generators(); ++x) {
            standardize_immediate(_id_coset, x);
          }
        }
      } else {
        if (standardize() && restandardize()) {
          // We have previously run HLT or Felsch, and now we are running with
          // immediate standardization. So, in case we weren't standardizing
          // previously, we call standardize.
          if (standardize(order::shortlex)) {
            _deduct->clear();
          }
        }
        if (save() || strategy() == options::strategy::felsch) {
          // If we previously ran hlt, and are now running (felsch or hlt +
          // save), then the state will not be relation_extra_initialized and
          // felsch_tree will not be initialized.
          init_felsch_tree();
        }
      }
    }

    void ToddCoxeter::finalise_run(detail::Timer& tmr) {
      LIBSEMIGROUPS_ASSERT(_state == state::hlt || _state == state::felsch);
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      if (!stopped()) {
        if (_deduct->any_skipped()) {
          if (number_of_cosets_active() != lower_bound() + 1 || !complete()) {
            push_settings();
            lookahead(options::lookahead::full | options::lookahead::hlt);
            perform_lookahead();
            pop_settings();
          }
        }
        // LIBSEMIGROUPS_ASSERT(complete());
        // LIBSEMIGROUPS_ASSERT(compatible());
        _state = state::finished;
      } else {
        report_active_cosets(__func__);
        report_why_we_stopped();
      }
      report_time(__func__, tmr);
      if (finished()) {
        // Can't set standardize here, because we haven't initialised _tree,
        // even if we are using standard_immediate. Also until we are finished
        // it is probably a terrible idea to populate _tree, because it will
        // be much larger than required in many cases, and we also have to
        // manage its size etc. Also standardize_immediate doesn't currently
        // guarantee that the table is actually standardized by the end of the
        // run.
        REPORT_DEFAULT("SUCCESS!\n");
      }
    }

    void ToddCoxeter::felsch() {
      REPORT_DEFAULT("performing Felsch...\n");
      detail::Timer tmr;
      init_generating_pairs();
      init_run();
      _state         = state::felsch;
      size_t const n = number_of_generators();
      while (_current != first_free_coset() && !stopped()) {
        if (preferred_defs() == options::preferred_defs::deferred) {
          _preferred_defs->purge_from_top();
          while (!_preferred_defs->empty()) {
            Deduction d = _preferred_defs->pop();
            LIBSEMIGROUPS_ASSERT(is_active_coset(d.first));
            LIBSEMIGROUPS_ASSERT(_word_graph.unsafe_neighbor(d.first, d.second)
                                 == UNDEFINED);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
            _stats.nr_active_preferred_defs++;
#endif
            _stats.tc1_f_appl++;
            def_edge<StackDeductions>(d.first, d.second, new_coset());
            if (standardize()) {
              standardize_immediate(d.first, d.second);
            }
            process_deductions();
            // Note that process_deductions and process_coincidences both can
            // made definitions, but don't do any standardization, so it is
            // possible that the table is not actually standardized at this
            // point. This is okay because we anyway don't record that the
            // table is standardized, and so if standardize(order) is called
            // later then it performs an actual standardization at that point.
            // As such standardize_immediate is only really an attempt to keep
            // the table somewhat standardized, not genuinely standardized.
            _preferred_defs->purge_from_top();
          }
        }
        for (letter_type a = 0; a < n; ++a) {
          if (_word_graph.unsafe_neighbor(_current, a) == UNDEFINED) {
            _stats.tc1_f_appl++;
            def_edge<StackDeductions>(_current, a, new_coset());
            if (standardize()) {
              standardize_immediate(_current, a);
            }
            process_deductions();
          }
        }

        if (report()) {
          report_active_cosets(__func__);
        }
        _current = next_active_coset(_current);
      }
      finalise_run(tmr);
    }

    // Walker's Strategy 1 = HLT = ACE style-R
    void ToddCoxeter::hlt() {
      REPORT_DEFAULT("performing HLT...\n");
      detail::Timer tmr;
      init_generating_pairs();

      init_run();
      _state = state::hlt;

      bool do_pop_settings = false;
      if (save() && preferred_defs() == options::preferred_defs::deferred) {
        push_settings();
        do_pop_settings = true;
        // The call to process_deductions in the main loop below could
        // potentially accummulate large numbers of preferred definitions in
        // the queue if the preferred_defs() setting is
        // options::preferred_defs::deferred, so we change it.
        preferred_defs(options::preferred_defs::none);
      }
      while (_current != first_free_coset() && !stopped()) {
        if (!save()) {
          for (auto it = _relations.cbegin(); it < _relations.cend(); it += 2) {
            push_definition_hlt<DoNotStackDeductions,
                                ProcessCoincidences<stack_deductions::no>>(
                _current, *it, *(it + 1));
          }
        } else {
          for (auto it = _relations.cbegin(); it < _relations.cend(); it += 2) {
            push_definition_hlt<StackDeductions, DoNotProcessCoincidences>(
                _current, *it, *(it + 1));
            process_deductions();
            // See the comments in ToddCoxeter::felsch about the meaning of
            // standardize_immediate.
          }
        }
        if (standardize()) {
          bool any_changes = false;
          for (letter_type x = 0; x < number_of_generators(); ++x) {
            any_changes |= standardize_immediate(_current, x);
          }
          if (any_changes) {
            _deduct->clear();
          }
        }
        if ((!save() || _deduct->any_skipped())
            && number_of_cosets_active() > next_lookahead()) {
          // If save() == true and no deductions were skipped, then we have
          // already run process_deductions, and so there's no point in doing
          // a lookahead.
          perform_lookahead();
        }
        if (report()) {
          report_active_cosets(__func__);
        }
        _current = next_active_coset(_current);
      }
      finalise_run(tmr);
      if (do_pop_settings) {
        pop_settings();
      }
    }

    void ToddCoxeter::random() {
      push_settings();
      REPORT_DEFAULT("performing random strategy...\n");
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
      while (!finished()) {
        size_t prev_num_cosets = number_of_cosets_active();
        auto   prev_strategy   = strategy();
        size_t m               = dist(mt);
        if (m < 8) {
          strategy(options::strategy::hlt);
          lookahead((full[m] ? options::lookahead::full
                             : options::lookahead::partial));
          try {
            save(save_it[m]);
          } catch (...) {
            // It isn't always possible to use the save option (when this is
            // created from a Cayley table, for instance), and
            // ToddCoxeter::save throws if this is the case.
          }
        } else {
          try {
            strategy(options::strategy::felsch);
          } catch (...) {
            strategy(options::strategy::hlt);
            // It isn't always possible to use the Felsch strategy (when this
            // is created from a Cayley table, for instance), and
            // ToddCoxeter::save throws if this is the case.
          }
        }
        standardize(stand[m]);

        REPORT(line).prefix().flush();
        // Some tests show that this is better to do when using this strategy
        if (prev_strategy != strategy()) {
          _current = _id_coset;
        }
        run_for(_settings->random_interval);
        if (prev_num_cosets == number_of_cosets_active()) {
          lookahead(options::lookahead::full | options::lookahead::hlt);
          perform_lookahead();
        }
      }
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      lookahead(options::lookahead::full | options::lookahead::hlt);
      perform_lookahead();
      pop_settings();
    }

    void ToddCoxeter::CR_style() {
      size_t N = length_of_generating_pairs();
      push_settings();
      while (!finished()) {
        strategy(options::strategy::felsch);
        auto M = _stats.tc1_f_appl;
        run_until([this, &M]() -> bool {
          return this->_stats.tc1_f_appl >= (this->f_defs() + M);
        });
        if (finished()) {
          break;
        }
        strategy(options::strategy::hlt);
        M = _stats.tc1_hlt_appl;
        run_until([this, &M, &N]() -> bool {
          return this->_stats.tc1_hlt_appl >= ((this->hlt_defs() / N) + M);
        });
      }
      lookahead(options::lookahead::full | options::lookahead::hlt);
      perform_lookahead();
      pop_settings();
    }

    void ToddCoxeter::R_over_C_style() {
      push_settings();
      strategy(options::strategy::hlt);
      run_until([this]() -> bool {
        return this->number_of_cosets_active() >= this->next_lookahead();
      });
      if (lookahead() & options::lookahead::hlt) {
        lookahead(options::lookahead::full | options::lookahead::hlt);
      } else {
        lookahead(options::lookahead::full | options::lookahead::felsch);
      }
      perform_lookahead();
      CR_style();
      pop_settings();
    }

    void ToddCoxeter::Cr_style() {
      push_settings();
      strategy(options::strategy::felsch);
      auto M = _stats.tc1_f_appl;
      run_until([this, &M]() -> bool {
        return this->_stats.tc1_f_appl >= (this->f_defs() + M);
      });
      strategy(options::strategy::hlt);
      M        = _stats.tc1_hlt_appl;
      size_t N = length_of_generating_pairs();
      run_until([this, &M, &N]() -> bool {
        return this->_stats.tc1_hlt_appl >= ((this->hlt_defs() / N) + M);
      });
      strategy(options::strategy::felsch);
      run();
      lookahead(options::lookahead::full | options::lookahead::hlt);
      perform_lookahead();
      pop_settings();
    }

    void ToddCoxeter::Rc_style() {
      push_settings();
      strategy(options::strategy::hlt);
      auto   M = _stats.tc1_hlt_appl;
      size_t N = length_of_generating_pairs();
      run_until([this, &M, &N]() -> bool {
        return this->_stats.tc1_hlt_appl >= ((this->hlt_defs() / N) + M);
      });
      strategy(options::strategy::felsch);
      M = _stats.tc1_f_appl;
      run_until([this, &M]() -> bool {
        return this->_stats.tc1_f_appl >= (this->f_defs() + M);
      });
      strategy(options::strategy::hlt);
      run();
      lookahead(options::lookahead::full | options::lookahead::hlt);
      perform_lookahead();
      pop_settings();
    }

    ////////////////////////////////////////////////////////////////////////
    // 19. ToddCoxeter - member functions (deduction processing) - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::process_deductions() {
      LIBSEMIGROUPS_ASSERT(!_prefilled);
      if (deduction_policy() & options::deductions::v2) {
        switch (preferred_defs()) {
          case options::preferred_defs::none:
            process_deductions_v2<NoPreferredDefs>();
            break;
          case options::preferred_defs::immediate_no_stack:
            process_deductions_v2<ImmediateDef<DoNotStackDeductions>>();
            break;
          case options::preferred_defs::immediate_yes_stack:
            process_deductions_v2<ImmediateDef<StackDeductions>>();
            break;
          case options::preferred_defs::deferred:
            process_deductions_v2<QueuePreferredDefs>();
          default:
            break;
        }
      } else {
        LIBSEMIGROUPS_ASSERT(deduction_policy() & options::deductions::v1);
        switch (preferred_defs()) {
          case options::preferred_defs::none:
            process_deductions_v1<NoPreferredDefs>();
            break;
          case options::preferred_defs::immediate_no_stack:
            process_deductions_v1<ImmediateDef<DoNotStackDeductions>>();
            break;
          case options::preferred_defs::immediate_yes_stack:
            process_deductions_v1<ImmediateDef<StackDeductions>>();
            break;
          case options::preferred_defs::deferred:
            process_deductions_v1<QueuePreferredDefs>();
          default:
            break;
        }
      }
    }

    // Version 1
    template <typename TPreferredDefs>
    void ToddCoxeter::process_deductions_v1() {
      LIBSEMIGROUPS_ASSERT(_felsch_tree != nullptr);
      LIBSEMIGROUPS_ASSERT(!_prefilled);

      size_t report_check = 100'000;
      while (!_deduct->empty()) {
        auto d = _deduct->pop();
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        _stats.total_deduct++;
#endif
        if (is_active_coset(d.first)) {
#ifdef LIBSEMIGROUPS_ENABLE_STATS
          _stats.nr_active_deduct++;
#endif
          report_check--;
          _felsch_tree->push_back(d.second);
          process_deductions_dfs_v1<TPreferredDefs>(d.first);
          process_coincidences(stack_deductions::yes);
          // The report_check bit in the next line stops us from calling
          // report() too often, which can bring a time penalty.
          if (report_check == 0 && report()) {
            report_check = 100'000;
            report_active_cosets(__func__);
          }
        }
        if (_deduct->empty()) {
          process_coincidences(stack_deductions::yes);
        }
      }
      process_coincidences(stack_deductions::yes);
    }

    template <typename TPreferredDefs>
    void ToddCoxeter::process_deductions_dfs_v1(coset_type c) {
      for (auto it = _felsch_tree->cbegin(); it < _felsch_tree->cend(); ++it) {
        push_definition_felsch<StackDeductions,
                               DoNotProcessCoincidences,
                               TPreferredDefs>(c, *it);
      }

      size_t const n = number_of_generators();
      for (size_t x = 0; x < n; ++x) {
        if (_felsch_tree->push_front(x)) {
          coset_type e = _word_graph.first_source(c, x);
          while (e != UNDEFINED) {
            process_deductions_dfs_v1<TPreferredDefs>(e);
            e = _word_graph.next_source(e, x);
          }
          _felsch_tree->pop_front();
        }
      }
    }

    // Version 2
    template <typename TPreferredDefs>
    void ToddCoxeter::process_deductions_v2() {
      LIBSEMIGROUPS_ASSERT(_felsch_tree != nullptr);
      LIBSEMIGROUPS_ASSERT(!_prefilled);

      size_t report_check = 100'000;
      while (!_deduct->empty()) {
        auto d = _deduct->pop();
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        _stats.total_deduct++;
#endif
        if (is_active_coset(d.first)) {
#ifdef LIBSEMIGROUPS_ENABLE_STATS
          _stats.nr_active_deduct++;
#endif
          report_check--;
          _felsch_tree->push_back(d.second);
          for (auto it = _felsch_tree->cbegin(); it < _felsch_tree->cend();
               ++it) {
            // Using anything other than NoPreferredDefs here seems to be bad
            // in test case "ACE --- perf602p5 - Felsch", maybe this is a good
            // example where the fill factor would be useful?
            push_definition_felsch<StackDeductions,
                                   DoNotProcessCoincidences,
                                   NoPreferredDefs>(d.first, *it);
          }
          process_deductions_dfs_v2<TPreferredDefs>(d.first, d.first);
          process_coincidences(stack_deductions::yes);
          // The report_check bit in the next line stops us from calling
          // report() too often, which can bring a time penalty.
          if (report_check == 0 && report()) {
            report_check = 100'000;
            report_active_cosets(__func__);
          }
        }
        if (_deduct->empty()) {
          process_coincidences(stack_deductions::yes);
        }
      }
      process_coincidences(stack_deductions::yes);
    }

    template <typename TPreferredDefs>
    void ToddCoxeter::process_deductions_dfs_v2(coset_type root, coset_type c) {
      size_t const n = number_of_generators();
      for (size_t x = 0; x < n; ++x) {
        coset_type e = _word_graph.first_source(c, x);
        if (e != UNDEFINED && _felsch_tree->push_front(x)) {
          // We only need to push the side (the good side) of the relation
          // that corresponds to the prefix in the tree through one preimage,
          // because pushing it through any preimage leads to the same place
          // (this is how the preimages/tree works!). If that place is more
          // than one away from the end of the relation, then we don't have to
          // do anything further, i.e. no more pushes of any other preimage or
          // any pushes involving the other side of the relation. Do the
          // pushing of the good side once here and pass the result to the dfs
          // function. Update the pushing above to only do the other (bad)
          // side of the relations.
          for (auto it = _felsch_tree->cbegin(); it < _felsch_tree->cend();
               ++it) {
            auto        i = *it;  // good
            auto        j = (i % 2 == 0 ? *it + 1 : *it - 1);
            auto const& u = _relations[i];
            auto const& v = _relations[j];
            LIBSEMIGROUPS_ASSERT(action_digraph_helper::follow_path_nc(
                                     _word_graph,
                                     _word_graph.first_source(c, x),
                                     u.cbegin(),
                                     u.cbegin() + _felsch_tree->length() - 1)
                                 == root);
            // Start the push through not at the preimage, but at the original
            // node definition we are processing; again because we know that
            // all paths lead to this node (again by the definition of the
            // search).
            coset_type y = action_digraph_helper::follow_path_nc(
                _word_graph,
                root,
                u.cbegin() + _felsch_tree->length() - 1,
                u.cend() - 1);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
            _stats.tc2_appl++;
#endif
            if (y == UNDEFINED) {
              continue;
            }
            e = _word_graph.first_source(c, x);
            while (e != UNDEFINED) {
              coset_type z = action_digraph_helper::follow_path_nc(
                  _word_graph, e, v.cbegin(), v.cend() - 1);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
              _stats.tc2_appl++;
#endif
              if (z != UNDEFINED) {
                push_definition<StackDeductions,
                                DoNotProcessCoincidences,
                                TPreferredDefs>(y, u.back(), z, v.back());
              }
              e = _word_graph.next_source(e, x);
            }
          }
          e = _word_graph.first_source(c, x);
          while (e != UNDEFINED) {
            process_deductions_dfs_v2<TPreferredDefs>(root, e);
            e = _word_graph.next_source(e, x);
          }
          _felsch_tree->pop_front();
        }
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // 20. ToddCoxeter - member functions (lookahead) - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::perform_lookahead() {
      detail::Timer t;
      state const   old_state = _state;
      _state                  = state::lookahead;
      if (lookahead() & options::lookahead::partial) {
        REPORT_DEFAULT("performing partial lookahead...\n");
        // Start lookahead from the coset after _current
        _current_la = next_active_coset(_current);
      } else {
        LIBSEMIGROUPS_ASSERT(_settings->lookahead & options::lookahead::full);
        REPORT_DEFAULT("performing full lookahead...\n");
        // Start from the first coset
        _current_la = _id_coset;
      }
      size_t number_of_killed = 0;
      if (lookahead() & options::lookahead::hlt) {
        number_of_killed = hlt_lookahead(old_state);
      } else {
        LIBSEMIGROUPS_ASSERT(lookahead() & options::lookahead::felsch);
        number_of_killed = felsch_lookahead(old_state);
      }

      report_cosets_killed(__func__, number_of_killed);
      if (number_of_cosets_active()
              < (next_lookahead() / lookahead_growth_factor())
          && next_lookahead() > min_lookahead()) {
        // If the next_lookahead is much bigger than the current number of
        // cosets, then reduce the next lookahead.
        report_inc_lookahead(
            __func__, lookahead_growth_factor() * number_of_cosets_active());
        next_lookahead(lookahead_growth_factor() * number_of_cosets_active());
      } else if (number_of_cosets_active() > next_lookahead()
                 || number_of_killed < (number_of_cosets_active()
                                        / lookahead_growth_threshold())) {
        // Otherwise, if we already exceed the next_lookahead or too few
        // cosets were killed, then increase the next lookahead.
        report_inc_lookahead(__func__,
                             next_lookahead() * lookahead_growth_factor());
        _settings->next_lookahead *= lookahead_growth_factor();
      }
      report_time(__func__, t);
      _state = old_state;
    }

    size_t ToddCoxeter::hlt_lookahead(state const& old_state) {
      report_active_cosets(__func__);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
      _stats.hlt_lookahead_calls++;
#endif

      size_t const old_number_of_killed = number_of_cosets_killed();
      while (_current_la != first_free_coset()
             // when running certain strategies the state is finished at this
             // point, and so stopped() == true, but we anyway want to perform
             // a full lookahead, which is why "_state == state::finished" is
             // in the next line.
             && (old_state == state::finished || !stopped())) {
        for (auto it = _relations.cbegin(); it < _relations.cend(); it += 2) {
          push_definition_felsch<DoNotStackDeductions,
                                 ProcessCoincidences<stack_deductions::no>,
                                 NoPreferredDefs>(
              // Using NoPreferredDefs is just a (more or less) arbitrary
              // choice, could allow the other choices here too (which works,
              // but didn't seem to be very useful).
              _current_la,
              *it,
              *(it + 1));
        }
        _current_la = next_active_coset(_current_la);

        if (report()) {
          report_active_cosets(__func__);
        }
      }
      return number_of_cosets_killed() - old_number_of_killed;
    }

    size_t ToddCoxeter::felsch_lookahead(state const& old_state) {
      report_active_cosets(__func__);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
      _stats.f_lookahead_calls++;
#endif
      size_t const old_number_of_killed = number_of_cosets_killed();
      init_felsch_tree();
      while (_current_la != first_free_coset()
             // when running certain strategies the state is finished at
             // this point, and so stopped() == true, but we anyway want to
             // perform a full lookahead, which is why "_state ==
             // state::finished" is in the next line.
             && (old_state == state::finished || !stopped())) {
        for (size_t a = 0; a < number_of_generators(); ++a) {
          _deduct->emplace(_current_la, a);
        }
        process_deductions();
        _current_la = next_active_coset(_current_la);
        if (report()) {
          report_active_cosets(__func__);
        }
      }
      return number_of_cosets_killed() - old_number_of_killed;
    }

    ////////////////////////////////////////////////////////////////////////
    // 21. ToddCoxeter - member functions (standardize) - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeter::init_standardize() {
      if (_tree == nullptr) {
        _tree = std::make_unique<std::vector<TreeNode>>(
            number_of_cosets_active(), TreeNode());
      } else {
        _tree->resize(number_of_cosets_active());
      }
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      LIBSEMIGROUPS_ASSERT(_deduct->empty());
    }

    bool ToddCoxeter::standardize_immediate(coset_type s, letter_type x) {
      LIBSEMIGROUPS_ASSERT(is_active_coset(s));
      LIBSEMIGROUPS_ASSERT(!finished());
      LIBSEMIGROUPS_ASSERT(_coinc.empty());

      coset_type const r = _word_graph.unsafe_neighbor(s, x);
      LIBSEMIGROUPS_ASSERT(r == UNDEFINED || is_active_coset(r));
      if (r != UNDEFINED) {
        if (r > _standard_max) {
          _standard_max++;
          if (r > _standard_max) {
            swap_cosets(_standard_max, r);
            LIBSEMIGROUPS_ASSERT(is_active_coset(_standard_max));
            return true;
          }
        }
      }
      return false;
    }

    bool ToddCoxeter::standardize_deferred(Perm&             p,
                                           Perm&             q,
                                           coset_type const  s,
                                           coset_type&       t,
                                           letter_type const x) {
      // p : new -> old and q : old -> new
      coset_type r = _word_graph.unsafe_neighbor(p[s], x);
      if (r != UNDEFINED) {
        r = q[r];  // new
        if (r > t) {
          t++;
          if (r > t) {
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

    bool ToddCoxeter::lex_standardize() {
      REPORT_DEFAULT("standardizing:\t%*s(%s)\n", 26, "", __func__);
      detail::Timer tmr;

      coset_type   s = 0;
      coset_type   t = 0;
      letter_type  x = 0;
      size_t const n = number_of_generators();
      Perm         p(coset_capacity(), 0);
      std::iota(p.begin(), p.end(), 0);
      Perm q(coset_capacity(), 0);
      std::iota(q.begin(), q.end(), 0);
      bool result = false;

      // Perform a DFS through the _word_graph.table()
      while (s <= t) {
        if (standardize_deferred(p, q, s, t, x)) {
          result = true;
          s      = t;
          x      = 0;
          continue;
        }
        x++;
        if (x == n) {  // backtrack
          x = (*_tree)[s].gen;
          s = (*_tree)[s].parent;
        }
      }
      apply_permutation(p, q);

      report_time(__func__, tmr);
#ifdef LIBSEMIGROUPS_DEBUG
      debug_validate_forwd_bckwd();
      debug_validate_table();
      // debug_validate_word_graph();
#endif
      return result;
    }

    bool ToddCoxeter::shortlex_standardize() {
      REPORT_DEFAULT("standardizing:\t%*s(%s)\n", 26, "", __func__);
      detail::Timer tmr;
      coset_type    t = 0;
      size_t const  n = number_of_generators();
      Perm          p(coset_capacity(), 0);
      std::iota(p.begin(), p.end(), 0);
      Perm q(coset_capacity(), 0);
      std::iota(q.begin(), q.end(), 0);
      bool result = false;

      for (coset_type s = 0; s <= t; ++s) {
        for (letter_type x = 0; x < n; ++x) {
          result |= standardize_deferred(p, q, s, t, x);
        }
      }
      apply_permutation(p, q);
      report_time(__func__, tmr);
#ifdef LIBSEMIGROUPS_DEBUG
      debug_validate_forwd_bckwd();
      debug_validate_table();
      // debug_validate_word_graph();
#endif
      return result;
    }

    // This is how the recursive words up to a given length M, and on an
    // arbitrary finite alphabet are generated.  On a single letter alphabet,
    // this order is just increasing powers of the only generator:
    //
    //   a < aa < aaa < aaaa < ... < aa...a (M times)
    //
    // With an n-letter alphabet A = {a_1, a_2, ..., a_n}, suppose we have
    // already obtained all of the words W_{n - 1} containing {a_1, ..., a_{n
    // - 1}}.  Every word in W_{n - 1} is less than any word containing a_n,
    // and the least word greater than every word in W_{n - 1} is a_n. Words
    // greater than a_n are obtain in the follow way, where:
    //
    // x: is the maximum word in W_{n - 1}, this is constant, in the
    // description
    //    that follows.
    // u: the first word obtained in point (1), the first time it is applied
    //    after (2) has been applied, starting with u = a_{n - 1}.
    // v: a word with one fewer letters than u, starting with the empty
    // word. w: a word such that w < u, also starting with the empty word.
    //
    // (a) If v < x, then v is replaced by the next word in the order. If |uv|
    //     <= M, then the next word is uv. Otherwise, goto a.
    //
    // (b) If v = x, then and there exists a word w' in the set of words
    //     obtained so far such that w' > w and |w'| <= M - 1, then replace
    //     w with w', replace u by wa_n, replace v by the empty word, and
    //     the next word is wa_n.
    //
    //     If no such word w' exists, then we have enumerated all the
    //     required words, and we can stop.
    //
    // For example, if A = {a, b} and M = 4, then the initial elements in
    // the order are:
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
    // After defining baa < baaa, x = aaaa, u = b, v = aaaa, and w = e.
    // Hence v = x, and so (2) applies. The next w' in the set of words so
    // far enumerated is a, and |a| = 1 <= 3 = M - 1, and so w <- a, u <-
    // ab, v <- e, and the next word is ab. We repeatedly apply (1), until
    // it fails, to obtain
    //
    //   baaa < ab < aba < abaa
    //
    // At which point u = b, v = aaaa = x, and w = a. Hence (2) applies, w
    // <- aa, v <- e, u <- aab, and the next word is: aab. And so on ...

    // TODO(later): improve this, it is currently very slow.
    bool ToddCoxeter::recursive_standardize() {
      REPORT_DEFAULT("standardizing (recursive)...\n");
      detail::Timer          tmr;
      std::vector<word_type> out;
      size_t const           n = number_of_generators();
      letter_type            a = 0;
      coset_type             s = 0;
      coset_type             t = 0;

      Perm p(coset_capacity(), 0);
      std::iota(p.begin(), p.end(), 0);
      Perm q(coset_capacity(), 0);
      std::iota(q.begin(), q.end(), 0);
      bool result = false;

      while (s <= t) {
        if (standardize_deferred(p, q, s, t, 0)) {
          out.push_back(word_type(t, a));
          result = true;
        }
        s++;
      }
      a++;
      bool new_generator = true;
      int  x, u, w;
      while (a < n && t < number_of_cosets_active() - 1) {
        if (new_generator) {
          w = -1;  // -1 is the empty word
          if (standardize_deferred(p, q, 0, t, a)) {
            result = true;
            out.push_back({a});
          }
          x             = out.size() - 1;
          u             = out.size() - 1;
          new_generator = false;
        }

        coset_type const uu = action_digraph_helper::follow_path_nc(
            _word_graph, 0, out[u].begin(), out[u].end());
        if (uu != UNDEFINED) {
          for (int v = 0; v < x; v++) {
            coset_type const uuv = action_digraph_helper::follow_path_nc(
                _word_graph, uu, out[v].begin(), out[v].end() - 1);
            if (uuv != UNDEFINED) {
              s = q[uuv];
              if (standardize_deferred(p, q, s, t, out[v].back())) {
                result        = true;
                word_type nxt = out[u];
                nxt.insert(nxt.end(), out[v].begin(), out[v].end());
                out.push_back(std::move(nxt));
              }
            }
          }
        }
        w++;
        if (static_cast<size_t>(w) < out.size()) {
          coset_type const ww = action_digraph_helper::follow_path_nc(
              _word_graph, 0, out[w].begin(), out[w].end());
          if (ww != UNDEFINED) {
            s = q[ww];
            if (standardize_deferred(p, q, s, t, a)) {
              result        = true;
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
      report_time(__func__, tmr);
#ifdef LIBSEMIGROUPS_DEBUG
      debug_validate_forwd_bckwd();
      debug_validate_table();
      // debug_validate_word_graph();
#endif
      return result;
    }

    // The permutation q must map the active cosets to the [0, ..
    // , number_of_cosets_active())
    void ToddCoxeter::apply_permutation(Perm& p, Perm& q) {
      // p : new -> old, q = p ^ -1
#ifdef LIBSEMIGROUPS_DEBUG
      for (size_t c = 0; c < q.size(); ++c) {
        LIBSEMIGROUPS_ASSERT(
            (is_active_coset(c) && q[c] < number_of_cosets_active())
            || (!is_active_coset(c) && q[c] >= number_of_cosets_active()));
        LIBSEMIGROUPS_ASSERT(p[q[c]] == c);
        LIBSEMIGROUPS_ASSERT(q[p[c]] == c);
      }
#endif
      _word_graph.permute_nodes_nc(p, q, number_of_cosets_active());
      CosetManager::apply_permutation(p);
    }

    // Based on the procedure SWITCH in Sims' book, p193
    // Swaps an active coset and another coset in the table.
    void ToddCoxeter::swap_cosets(coset_type c, coset_type d) {
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      LIBSEMIGROUPS_ASSERT(c != _id_coset);
      LIBSEMIGROUPS_ASSERT(d != _id_coset);
      LIBSEMIGROUPS_ASSERT(c != d);
      LIBSEMIGROUPS_ASSERT(is_valid_coset(c));
      LIBSEMIGROUPS_ASSERT(is_valid_coset(d));
      if (is_active_coset(c) && is_active_coset(d)) {
        _word_graph.swap_nodes(c, d);
      } else if (is_active_coset(c)) {
        _word_graph.rename_node(c, d);
      } else {
        LIBSEMIGROUPS_ASSERT(is_active_coset(d));
        _word_graph.rename_node(d, c);
      }
      switch_cosets(c, d);
    }

    ////////////////////////////////////////////////////////////////////////
    // 22. ToddCoxeter - member functions (debug) - private
    ////////////////////////////////////////////////////////////////////////

#ifdef LIBSEMIGROUPS_DEBUG
    // Validates the coset table.
    void ToddCoxeter::debug_validate_table() const {
      REPORT_DEBUG_DEFAULT("validating the coset table... ");
      size_t const n = number_of_generators();
      coset_type   c = _id_coset;
      while (c != first_free_coset()) {
        if (!is_active_coset(c)) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid table, coset %d is both free and active!", c);
        }
        for (letter_type x = 0; x < n; ++x) {
          if (_word_graph.unsafe_neighbor(c, x) != UNDEFINED
              && !is_active_coset(_word_graph.unsafe_neighbor(c, x))) {
            LIBSEMIGROUPS_EXCEPTION(
                "invalid table, _word_graph.unsafe_neighbor(%d, %d) = %d"
                " is not an active coset or UNDEFINED!",
                c,
                x,
                _word_graph.unsafe_neighbor(c, x));
          }
        }
        c = next_active_coset(c);
      }
    }

    // Validates _word_graph, this is very expensive.
    void ToddCoxeter::debug_validate_word_graph() const {
      size_t const n = number_of_generators();
      coset_type   c = _id_coset;
      while (c != first_free_coset()) {
        for (letter_type x = 0; x < n; ++x) {
          coset_type cx = _word_graph.unsafe_neighbor(c, x);
          if (cx != UNDEFINED && !is_active_coset(cx)) {
            LIBSEMIGROUPS_EXCEPTION(
                "invalid table, _word_graph.unsafe_neighbor(%d, %d) = %d"
                " is not an active coset or UNDEFINED!",
                c,
                x,
                cx);
          }
        }
        c = next_active_coset(c);
      }
      c = _id_coset;
      while (c != first_free_coset()) {
        for (letter_type x = 0; x < n; ++x) {
          coset_type           e = _word_graph.first_source(c, x);
          std::set<coset_type> stored_preimages;
          while (e != UNDEFINED) {
            if (!stored_preimages.insert(e).second) {
              LIBSEMIGROUPS_EXCEPTION("duplicate preimage e = %llu of c = %llu "
                                      "under generator x = %llu",
                                      e,
                                      c,
                                      x);
            }
            if (!is_active_coset(e)) {
              LIBSEMIGROUPS_EXCEPTION(
                  "dead coset preimage e = %llu of c = %llu "
                  "under generator x = %llu",
                  e,
                  c,
                  x);
            }
            if (_word_graph.unsafe_neighbor(e, x) != c) {
              LIBSEMIGROUPS_EXCEPTION(
                  "invalid preimage, unsafe_neighbor(%d, %d) = %d != %d "
                  "butfound e = %d in the preimages of c = %d under "
                  "generator "
                  "x = %d",
                  e,
                  x,
                  _word_graph.unsafe_neighbor(e, x),
                  c,
                  e,
                  c,
                  x);
            }
            e = _word_graph.next_source(e, x);
          }
          // Check there are no missing preimages!
          e = _id_coset;
          while (e != first_free_coset()) {
            if (_word_graph.unsafe_neighbor(e, x) == c
                && stored_preimages.insert(e).second) {
              LIBSEMIGROUPS_EXCEPTION(
                  "missing preimage, _word_graph.unsafe_neighbor(%d, %d) == "
                  "%d but e = %d wasn't found in the stored preimages",
                  e,
                  x,
                  c,
                  e);
            }
            e = next_active_coset(e);
          }
        }
        c = next_active_coset(c);
      }
    }

    void ToddCoxeter::debug_verify_no_missing_deductions() const {
      if (!_deduct->empty()) {
        LIBSEMIGROUPS_EXCEPTION("the deduction stack is not empty");
      } else if (!_coinc.empty()) {
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
          coset_type x = action_digraph_helper::follow_path_nc(
              _word_graph, c, u.cbegin(), u.cend() - 1);
          if (x == UNDEFINED) {
            return;
          }
          LIBSEMIGROUPS_ASSERT(is_valid_coset(x));
          coset_type y = action_digraph_helper::follow_path_nc(
              _word_graph, c, v.cbegin(), v.cend() - 1);
          if (y == UNDEFINED) {
            return;
          }
          LIBSEMIGROUPS_ASSERT(is_valid_coset(y));
          letter_type a  = u.back();
          letter_type b  = v.back();
          coset_type  xx = _word_graph.unsafe_neighbor(x, a);
          coset_type  yy = _word_graph.unsafe_neighbor(y, b);
          if (xx == UNDEFINED && yy != UNDEFINED) {
            LIBSEMIGROUPS_EXCEPTION("missing deduction tau(%d, %d) = %d when "
                                    "pushing coset %d through %s = %s",
                                    x,
                                    a,
                                    yy,
                                    c,
                                    detail::to_string(u).c_str(),
                                    detail::to_string(v).c_str());
          } else if (xx != UNDEFINED && yy == UNDEFINED) {
            // _table(y, b) <- xx
            LIBSEMIGROUPS_EXCEPTION("missing deduction tau(%d, %d) = %d when "
                                    "pushing coset %d through %s = %s",
                                    y,
                                    b,
                                    xx,
                                    c,
                                    detail::to_string(u).c_str(),
                                    detail::to_string(v).c_str());
          } else if (xx != UNDEFINED && yy != UNDEFINED) {
            // _table(x, a) and _table(y, b) are defined
            if (xx != yy) {
              LIBSEMIGROUPS_EXCEPTION("missing coincidence %d = %d when "
                                      "pushing coset %d through %s = %s",
                                      xx,
                                      yy,
                                      c,
                                      detail::to_string(u).c_str(),
                                      detail::to_string(v).c_str());
            }
          }
        }
      }
    }

#endif

    ////////////////////////////////////////////////////////////////////////
    // Combining options
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeter::options::deductions
    operator|(ToddCoxeter::options::deductions const& opt1,
              ToddCoxeter::options::deductions const& opt2) {
      using int_type = std::underlying_type_t<ToddCoxeter::options::deductions>;
      auto int1      = static_cast<int_type>(opt1);
      auto int2      = static_cast<int_type>(opt2);
      if ((int1 < 4 && int2 < 4) || (int1 >= 4 && int2 >= 4)) {
        LIBSEMIGROUPS_EXCEPTION("invalid operands %s and %s for operator|",
                                detail::to_string(opt1).c_str(),
                                detail::to_string(opt2).c_str());
      }
      return ToddCoxeter::options::deductions(int1 | int2);
    }

    bool operator&(ToddCoxeter::options::deductions const& opt1,
                   ToddCoxeter::options::deductions const& opt2) {
      using int_type = std::underlying_type_t<ToddCoxeter::options::deductions>;
      auto int1      = static_cast<int_type>(opt1);
      auto int2      = static_cast<int_type>(opt2);
      static int_type constexpr mask1 = 3;
      static int_type constexpr mask2
          = std::numeric_limits<int_type>::max() - 3;
      if (int1 < 4 || int2 < 4) {
        return (int1 & mask1) == (int2 & mask1);
      } else {
        return (int1 & mask2) == (int2 & mask2);
      }
    }

    ToddCoxeter::options::lookahead
    operator|(ToddCoxeter::options::lookahead const& opt1,
              ToddCoxeter::options::lookahead const& opt2) {
      using int_type = std::underlying_type_t<ToddCoxeter::options::lookahead>;
      auto int1      = static_cast<int_type>(opt1);
      auto int2      = static_cast<int_type>(opt2);
      if ((int1 <= 2 && int2 <= 2) || (int1 > 2 && int2 > 2)) {
        LIBSEMIGROUPS_EXCEPTION("invalid operands %s and %s for operator|",
                                detail::to_string(opt1).c_str(),
                                detail::to_string(opt2).c_str());
      }

      return ToddCoxeter::options::lookahead(int1 | int2);
    }

    bool operator&(ToddCoxeter::options::lookahead const& opt1,
                   ToddCoxeter::options::lookahead const& opt2) {
      using int_type = std::underlying_type_t<ToddCoxeter::options::lookahead>;
      auto int1      = static_cast<int_type>(opt1);
      auto int2      = static_cast<int_type>(opt2);
      static int_type constexpr mask1 = 3;
      static int_type constexpr mask2
          = std::numeric_limits<int_type>::max() - 3;

      if (int1 <= 2 || int2 <= 2) {
        return (int1 & mask1) == (int2 & mask1);
      } else {
        return (int1 & mask2) == (int2 & mask2);
      }
    }

    // Silence compiler warnings in g++-11
    std::ostringstream& operator<<(std::ostringstream&                   os,
                                   ToddCoxeter::options::strategy const& val);

    std::ostringstream& operator<<(std::ostringstream&                   os,
                                   ToddCoxeter::options::strategy const& val) {
      using strategy = typename ToddCoxeter::options::strategy;
      switch (val) {
        case strategy::hlt:
          os << "HLT";
          break;
        case strategy::felsch:
          os << "Felsch";
          break;
        case strategy::random:
          os << "random";
          break;
        case strategy::CR:
          os << "CR";
          break;
        case strategy::R_over_C:
          os << "R/C";
          break;
        case strategy::Cr:
          os << "Cr";
          break;
        case strategy::Rc:
          os << "Rc";
          break;
        default:
          os << "unknown";
          break;
      }
      return os;
    }

    // Silence compiler warnings in g++-11
    std::ostringstream& operator<<(std::ostringstream&                     os,
                                   ToddCoxeter::options::deductions const& val);

    std::ostringstream&
    operator<<(std::ostringstream&                     os,
               ToddCoxeter::options::deductions const& val) {
      using deductions = typename ToddCoxeter::options::deductions;
      if (val & deductions::v1) {
        os << "v1 + ";
      } else if (val & deductions::v2) {
        os << "v2 + ";
      } else {
        os << "not set + ";
      }

      if (val & deductions::no_stack_if_no_space) {
        os << "no_stack_if_no_space";
      } else if (val & deductions::purge_from_top) {
        os << "purge_from_top";
      } else if (val & deductions::purge_all) {
        os << "purge_all";
      } else if (val & deductions::discard_all_if_no_space) {
        os << "discard_all_if_no_space";
      } else if (val & deductions::unlimited) {
        os << "unlimited";
      } else {
        os << "not set";
      }
      return os;
    }

    // Silence compiler warnings in g++-11
    std::ostringstream& operator<<(std::ostringstream&                    os,
                                   ToddCoxeter::options::lookahead const& val);

    std::ostringstream& operator<<(std::ostringstream&                    os,
                                   ToddCoxeter::options::lookahead const& val) {
      using lookahead = typename ToddCoxeter::options::lookahead;
      if (lookahead::partial & val) {
        os << "partial ";
      } else if (lookahead::full & val) {
        os << "full ";
      } else {
        os << "not set + ";
      }

      if (lookahead::hlt & val) {
        os << "HLT";
      } else if (lookahead::felsch & val) {
        os << "Felsch";
      } else {
        os << "not set + ";
      }
      return os;
    }

    // Silence compiler warnings in g++-11
    std::ostringstream&
    operator<<(std::ostringstream&                       os,
               ToddCoxeter::options::froidure_pin const& val);

    std::ostringstream&
    operator<<(std::ostringstream&                       os,
               ToddCoxeter::options::froidure_pin const& val) {
      using froidure_pin = typename ToddCoxeter::options::froidure_pin;
      switch (val) {
        case froidure_pin::none:
          os << "none";
          break;
        case froidure_pin::use_relations:
          os << "use_relations";
          break;
        case froidure_pin::use_cayley_graph:
          os << "use_cayley_graph";
          break;
        default:
          os << "unknown";
          break;
      }
      return os;
    }

    // Silence compiler warnings in g++-11
    std::ostringstream&
    operator<<(std::ostringstream&                         os,
               ToddCoxeter::options::preferred_defs const& val);

    std::ostringstream&
    operator<<(std::ostringstream&                         os,
               ToddCoxeter::options::preferred_defs const& val) {
      using preferred_defs = typename ToddCoxeter::options::preferred_defs;
      switch (val) {
        case preferred_defs::none:
          os << "none";
          break;
        case preferred_defs::immediate_no_stack:
          os << "immediate + no deduction stacked";
          break;
        case preferred_defs::immediate_yes_stack:
          os << "immediate + deduction stacked";
          break;
        case preferred_defs::deferred:
          os << "deferred";
          break;
        default:
          os << "unknown";
          break;
      }
      return os;
    }
  }  // namespace congruence
}  // namespace libsemigroups
