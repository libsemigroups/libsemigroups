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
  class Sims2 : public detail::SimsBase<Sims2> {
    using SimsBase = detail::SimsBase<Sims2>;
    friend SimsBase;

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
      SimsSettings<Sims2>::init();
      return *this;
    }

    template <typename Word>
    explicit Sims2(Presentation<Word> p) : Sims2() {
      presentation(p);
    }

   private:
    struct PendingDef : public SimsBase::PendingDefBase {
      PendingDef() = default;

      PendingDef(node_type   s,
                 letter_type g,
                 node_type   t,
                 size_type   e,
                 size_type   n,
                 bool        tin) noexcept
          : SimsBase::PendingDefBase(s, g, t, e, n, tin),
            target_is_new_node(tin) {}
      bool target_is_new_node;
    };

    // This class collects some common aspects of the iterator and
    // thread_iterator nested classes. The mutex does nothing for <iterator>
    // and is an actual std::mutex for <thread_iterator>.
    class iterator_base : public SimsBase::IteratorBase {
      class RuleContainer;

     public:
      using const_reference = SimsBase::IteratorBase::const_reference;
      using const_pointer   = SimsBase::IteratorBase::const_pointer;

     private:
      std::unique_ptr<RuleContainer> _2_sided_include;
      std::vector<word_type>         _2_sided_words;

     protected:
      using SimsBase::IteratorBase::init;
      using SimsBase::IteratorBase::try_pop;

      // We could use the copy constructor, but there's no point in copying
      // anything except the FelschGraph and so we only copy that.
      void partial_copy_for_steal_from(iterator_base const& that);

      // Try to make the definition represented by PendingDef, returns false if
      // it wasn't possible, and true if it was.
      //! No doc
      [[nodiscard]] bool try_define(PendingDef const&);

      iterator_base(Sims2 const* s, size_type n);

     public:
      iterator_base() = default;

      iterator_base(iterator_base const& that);
      iterator_base(iterator_base&& that);
      iterator_base& operator=(iterator_base const& that);
      iterator_base& operator=(iterator_base&& that);
      ~iterator_base();

      using SimsBase::IteratorBase::operator==;
      using SimsBase::IteratorBase::operator!=;
      using SimsBase::IteratorBase::operator*;
      using SimsBase::IteratorBase::operator->;

      //! No doc
      void swap(iterator_base& that) noexcept;

      using SimsBase::IteratorBase::stats;
    };  // class iterator_base

   public:
    using SimsSettings::cbegin_long_rules;
    using SimsSettings::exclude;
    using SimsSettings::include;
    using SimsSettings::number_of_threads;
    using SimsSettings::presentation;

    using SimsBase::cbegin;
    using SimsBase::cend;
    using SimsBase::find_if;
    using SimsBase::for_each;
    using SimsBase::number_of_congruences;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SIMS2_HPP_
