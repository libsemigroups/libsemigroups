//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains a declaration of a class for performing the "low-index
// congruence" algorithm for 2-sided congruences of semigroups and monoids.

// This file and the class Sims2 exist because iterating through 2-sided
// congruences is fundamentally different that iterating through 1-sided
// congruences. In more words, iterating through 2-sided congruences requires
// some more steps than iterating through 1-sided congruences. It might have
// been more pleasing to allow Sims1 objects (maybe appropriately renamed) to
// accept congruence_kind::twosided as their "kind". However, this would either
// have required:
// 1. run time checks in the iterator_base::try_define function (and probably
//    elsewhere too) if we were enumerating 1-sided or 2-sided congruences.
// 2. making iterator_base_1_sided and iterator_base_2_sided, and then
//    templating iterator and thread_iterator to use the appropriate type
//    depending on the value of "kind()".
//
// The disadvantage of 1 is that it would likely be slower, since try_define is
// the critical function for Sims1. It also makes the code more complicated,
// and breaks backwards compatibility. The disadvantage of 2 is that the return
// type of "cbegin" and "cend" depends on whether we are iterating through 1- or
// 2-sided congruences. In other words, number 2 above doesn't actually work.
// Hence we've opted for just having a separate class for low-index 2-sided
// congruences. This is also cleaner since we don't have to touch the impl of
// Sims1 (much) and is more backwards compatible.

#ifndef LIBSEMIGROUPS_SIMS2_HPP_
#define LIBSEMIGROUPS_SIMS2_HPP_

#include "sims1.hpp"

namespace libsemigroups {

  namespace detail {
    struct Rule {
      size_t source;
      size_t generator;
      size_t target;
    };

    class RuleContainer {
     private:
      std::vector<Rule> _2_sided_include;
      // _used_slots[i] is the length of _2_sided_include when we have i edges
      std::vector<size_t> _used_slots;

     public:
      RuleContainer()                                = default;
      RuleContainer(RuleContainer const&)            = default;
      RuleContainer(RuleContainer&&)                 = default;
      RuleContainer& operator=(RuleContainer const&) = default;
      RuleContainer& operator=(RuleContainer&&)      = default;

      ~RuleContainer() = default;

      void resize(size_t m) {
        // TODO should we use resize?
        _used_slots.assign(m, UNDEFINED);
        if (m > 0) {
          _used_slots[0] = 0;
        }
        _2_sided_include.assign(m, {});
      }

      Rule& next_rule(size_t num_edges) {
        return _2_sided_include[used_slots(num_edges)++];
      }

      auto begin(size_t) const noexcept {
        return _2_sided_include.begin();
      }

      auto end(size_t num_edges) noexcept {
        return _2_sided_include.begin() + used_slots(num_edges);
      }

      void backtrack(size_t num_edges) {
        std::fill(_used_slots.begin() + num_edges,
                  _used_slots.end(),
                  size_t(UNDEFINED));
        if (num_edges == 0) {
          _used_slots[0] = 0;
        }
      }

     private:
      size_t& used_slots(size_t num_edges) {
        LIBSEMIGROUPS_ASSERT(num_edges < _used_slots.size());
        if (_used_slots[0] == UNDEFINED) {
          _used_slots[0] = 0;
        }
        size_t i = num_edges;
        while (_used_slots[i] == UNDEFINED) {
          --i;
        }
        LIBSEMIGROUPS_ASSERT(i < _used_slots.size());
        LIBSEMIGROUPS_ASSERT(_used_slots[i] != UNDEFINED);
        for (size_t j = i + 1; j <= num_edges; ++j) {
          _used_slots[j] = _used_slots[i];
        }
        LIBSEMIGROUPS_ASSERT(_used_slots[num_edges] <= _2_sided_include.size());
        return _used_slots[num_edges];
      }
    };
  }  // namespace detail

  class Sims2 : public SimsBase<Sims2> {
   public:
    using node_type       = SimsBase::node_type;
    using label_type      = SimsBase::label_type;
    using letter_type     = SimsBase::letter_type;
    using size_type       = SimsBase::size_type;
    using word_graph_type = SimsBase::word_graph_type;

    Sims2()                        = default;
    Sims2(Sims2 const& other)      = default;
    Sims2(Sims2&&)                 = default;
    Sims2& operator=(Sims2 const&) = default;
    Sims2& operator=(Sims2&&)      = default;
    ~Sims2()                       = default;

    Sims2& init() {
      Sims1Settings<Sims2>::init();
      return *this;
    }

    template <typename Word>
    Sims2(Presentation<Word> p) : Sims2() {
      presentation(p);
    }

   private:
    using PendingDef = Sims1::PendingDef;

    // This class collects some common aspects of the iterator and
    // thread_iterator nested classes. The mutex does nothing for <iterator>
    // and is an actual std::mutex for <thread_iterator>.
    class iterator_base : public Sims1::iterator_base<Sims2> {
     public:
      using const_reference = Sims1::iterator_base<Sims2>::const_reference;
      using const_pointer   = Sims1::iterator_base<Sims2>::const_pointer;

     protected:
      detail::RuleContainer  _2_sided_include;
      std::vector<word_type> _2_sided_words;

      using Sims1::iterator_base<Sims2>::init;
      using Sims1::iterator_base<Sims2>::try_pop;

      // We could use the copy constructor, but there's no point in copying
      // anything except the FelschGraph and so we only copy that.
      // TODO rename
      void copy_felsch_graph(iterator_base const& that) {
        Sims1::iterator_base<Sims2>::copy_felsch_graph(that);
        _2_sided_include = that._2_sided_include;
        _2_sided_words   = that._2_sided_words;
      }

      // Try to make the definition represented by PendingDef, returns false if
      // it wasn't possible, and true if it was.
      //! No doc
      [[nodiscard]] bool try_define(PendingDef const&);

     public:
      iterator_base(Sims2 const* s, size_type n);

      // None of the constructors are noexcept because the corresponding
      // constructors for Presentation aren't currently

      iterator_base() = default;

      iterator_base(iterator_base const& that);
      iterator_base(iterator_base&& that);
      iterator_base& operator=(iterator_base const& that);
      iterator_base& operator=(iterator_base&& that);
      ~iterator_base();

      using Sims1::iterator_base<Sims2>::operator==;
      using Sims1::iterator_base<Sims2>::operator!=;
      using Sims1::iterator_base<Sims2>::operator*;
      using Sims1::iterator_base<Sims2>::operator->;

      //! No doc
      void swap(iterator_base& that) noexcept;

      using Sims1::iterator_base<Sims2>::stats;
    };  // class iterator_base

   public:
    using iterator        = Sims1::Iterator<iterator_base>;
    using thread_iterator = Sims1::ThreadIterator<iterator_base>;
    using thread_runner   = Sims1::ThreadRunner<thread_iterator>;

    using Sims1Settings::cbegin_long_rules;
    using Sims1Settings::exclude;
    using Sims1Settings::include;
    using Sims1Settings::number_of_threads;
    using Sims1Settings::presentation;

    [[nodiscard]] iterator cbegin(size_type n) const {
      return SimsBase<Sims2>::cbegin<iterator>(n);
    }

    [[nodiscard]] iterator cend(size_type n) const {
      return SimsBase<Sims2>::cend<iterator>(n);
    }

    using SimsBase<Sims2>::number_of_congruences;
    using SimsBase<Sims2>::for_each;
    using SimsBase<Sims2>::find_if;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SIMS2_HPP_
