//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-23 James D. Mitchell
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

// This file contains a declaration of a class for performing the Todd-Coxeter
// algorithm for semigroups and monoids.
//
// TODO:
// 1) Seems like not stacking deductions in coincidences when doing Felsch
//    makes some examples much quicker (like [003]) while being mostly correct
//    too (one example that requires this is partition_monoid(4, East))

#ifndef LIBSEMIGROUPS_TODD_COXETER_NEW_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_NEW_HPP_

#include "cong-intf-new.hpp"
#include "felsch-digraph.hpp"        // for FelschDigraph
#include "make-present.hpp"          // for make
#include "obvinf.hpp"                // for is_obviously_infinite
#include "order.hpp"                 // for order
#include "present.hpp"               // for Presentation
#include "todd-coxeter-digraph.hpp"  // for ToddCoxeterDigraph
#include "types.hpp"                 // for word_type

namespace libsemigroups {
  class ToddCoxeter : public v3::CongruenceInterface,
                      public FelschDigraphSettings<ToddCoxeter> {
   public:
    struct Stats;  // forward decl

    struct options {
      enum class strategy { hlt, felsch };
      enum class lookahead_extent { full, partial };
      enum class lookahead_style { hlt, felsch };
    };

   private:
    struct Settings {
      bool                     use_relations_in_extra = false;
      options::lookahead_style _lookahead_style = options::lookahead_style::hlt;
      options::lookahead_extent _lookahead_extent
          = options::lookahead_extent::partial;
      // TODO uncomment
      // options::lookahead    lookahead
      //    = options::lookahead::partial | options::lookahead::hlt;
      float             lookahead_growth_factor    = 2.0;
      size_t            lookahead_growth_threshold = 4;
      size_t            lower_bound                = UNDEFINED;
      size_t            max_preferred_defs         = 256;
      size_t            lookahead_min              = 10'000;
      size_t            lookahead_next             = 5'000'000;
      bool              restandardize              = false;
      bool              save                       = false;
      bool              standardize                = false;
      options::strategy strategy                   = options::strategy::hlt;
    };
    using FelschDigraphSettings_ = FelschDigraphSettings<ToddCoxeter>;

   public:
    using digraph_type = ToddCoxeterDigraph<FelschDigraph<word_type, uint32_t>>;
    using node_type    = typename digraph_type::node_type;

   private:
    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - data - private
    ////////////////////////////////////////////////////////////////////////

    // std::stack<Settings*>                _setting_stack;
    // Stats                                _stats;
    bool         _finished;
    Forest       _forest;
    Settings     _settings;
    order        _standardized;
    digraph_type _word_graph;

   public:
    // Private constructor
    ToddCoxeter(congruence_kind knd);
    void init(congruence_kind knd);

    ToddCoxeter(congruence_kind knd, Presentation<word_type>&& p);
    void init(congruence_kind knd, Presentation<word_type>&& p);

    ToddCoxeter(congruence_kind knd, Presentation<word_type> const& p);
    void init(congruence_kind knd, Presentation<word_type> const& p);

    // This is a constructor and not a helper so that everything that takes a
    // presentation has the same constructors, regardless of what they use
    // inside.
    // TODO init version
    template <typename T>
    ToddCoxeter(congruence_kind knd, Presentation<T> const& p)
        : ToddCoxeter(knd, make<Presentation<word_type>>(p)) {}

    template <typename T>
    void init(congruence_kind knd, Presentation<T> const& p) {
      init(knd, make<Presentation<word_type>>(p));
    }

    // TODO init version
    template <typename N>
    ToddCoxeter(congruence_kind knd, ActionDigraph<N> const& ad)
        : ToddCoxeter(knd) {
      // TODO is this the right way to init _word_graph?
      _word_graph = ad;
      _word_graph.presentation().alphabet(ad.out_degree());
    }

    // TODO init version
    ToddCoxeter(congruence_kind knd, ToddCoxeter const& tc);

    // TODO init version
    // ToddCoxeter() = default;
    // TODO probably do want a default constructor
    // ToddCoxeter()
    //     : v3::CongruenceInterface(),
    //       _finished(false),
    //       _forest(),
    //       _standardized(order::none),
    //       _word_graph() {}
    ToddCoxeter(ToddCoxeter const& that)       = default;
    ToddCoxeter(ToddCoxeter&&)                 = default;
    ToddCoxeter& operator=(ToddCoxeter const&) = default;
    ToddCoxeter& operator=(ToddCoxeter&&)      = default;

    ~ToddCoxeter() = default;

    using FelschDigraphSettings_::definition_policy;
    using FelschDigraphSettings_::definition_version;
    using FelschDigraphSettings_::max_definitions;
    using FelschDigraphSettings_::settings;

    Presentation<word_type> const& presentation() const noexcept {
      return _word_graph.presentation();
    }

    ////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////
    ToddCoxeter& strategy(options::strategy val);

    //! The current strategy for enumeration.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns The current strategy, a value of type options::strategy.
    //!
    //! \exceptions
    //! \noexcept
    options::strategy strategy() const noexcept;

    //! Set the style of lookahead to use in HLT.
    //!
    //! If the strategy is not HLT, then the value of this setting is
    //! ignored.
    //!
    //! The default value is options::lookahead::partial |
    //! options::lookahead::hlt. The other
    //! possible value are documented in options::lookahead.
    //!
    //! \param val value indicating whether to perform a full or partial
    //! lookahead in HLT or Felsch style.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_style(options::lookahead_style val) noexcept {
      _settings._lookahead_style = val;
      return *this;
    }

    ToddCoxeter& lookahead_extent(options::lookahead_extent val) noexcept {
      _settings._lookahead_extent = val;
      return *this;
    }

    //! The current value of the setting for lookaheads.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type options::lookahead.
    //!
    //! \exceptions
    //! \noexcept
    options::lookahead_style lookahead_style() const noexcept {
      return _settings._lookahead_style;
    }

    options::lookahead_extent lookahead_extent() const noexcept {
      return _settings._lookahead_extent;
    }

    //! Set the threshold that will trigger a lookahead in HLT.
    //!
    //! If the number of cosets active exceeds the value set by this
    //! function, then a lookahead, of the type set using the function
    //! \ref lookahead, is triggered. This only applies when using
    //! the HLT strategy.
    //!
    //! The default value is 5 million.
    //!
    //! \param val value indicating the initial threshold.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_next(size_t val) noexcept;

    //! The current value of the next lookahead setting.
    //!
    //! \parameters
    //! None
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_next(size_t)
    size_t lookahead_next() const noexcept;

    //! Set the minimum value of lookahead_next().
    //!
    //! After a lookahead is performed the value of lookahead_next() is
    //! modified depending on the outcome of the current lookahead. If the
    //! return value of lookahead_next() is too small or too large, then the
    //! value is adjusted according to lookahead_growth_factor() and
    //! lookahead_growth_threshold(). This setting specified the minimum
    //! possible value for lookahead_next().
    //!
    //! The default value is \c 10'000.
    //!
    //! \param val value indicating the minimum value of lookahead_next().
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_min(size_t val) noexcept;

    //! The current value of the minimum lookahead setting.
    //!
    //! \parameters
    //! None
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_min(size_t)
    size_t lookahead_min() const noexcept;

    //! Set the lookahead growth factor.
    //!
    //! This setting determines by what factor the number of nodes required
    //! to trigger a lookahead grows. More specifically, at the end of any
    //! lookahead if the number of active nodes already exceeds the value of
    //! lookahead_next() or the number of nodes killed during the lookahead
    //! is less than the number of active nodes divided by
    //! lookahead_growth_threshold(), then the value of
    //! ToddCoxeter::next_lookhead is increased by a multiple of the \p value.
    //!
    //! \param val the value indicating the lookahead growth factor.
    //! The default value is ``2.0``.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is less than ``1.0``.
    ToddCoxeter& lookahead_growth_factor(float val);

    //! The current value of the lookahead growth factor.
    //!
    //! \parameters
    //! None
    //!
    //! \returns A value of type `float`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_growth_factor(float)
    float lookahead_growth_factor() const noexcept;

    //! Set the lookahead growth threshold.
    //!
    //! This setting determines by what threshold for changing the number of
    //! nodes required to trigger a lookahead grows. More specifically, at
    //! the end of any lookahead if the number of active nodes already
    //! exceeds the value of lookahead_next() or the number of nodes killed
    //! during the lookahead is less than the number of active nodes divided
    //! by \ref lookahead_growth_threshold, then the value of
    //! ToddCoxeter::next_lookhead() is increased.
    //!
    //! The default value is ``4``.
    //!
    //! \param val the value indicating the lookahead growth threshold.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_growth_threshold(size_t val) noexcept;

    //! The current value of the lookahead growth threshold.
    //!
    //! \parameters
    //! None
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_growth_threshold()
    size_t lookahead_growth_threshold() const noexcept;

    ////////////////////////////////////////////////////////////////////////
    // Settings end
    ////////////////////////////////////////////////////////////////////////

    digraph_type const& word_graph() const noexcept {
      return _word_graph;
    }

    Forest const& spanning_tree() const noexcept {
      return _forest;
    }

    void shrink_to_fit();

    // Returns true if anything changed
    bool standardize(order val);

    inline bool is_standardized(order val) const {
      // TODO this is probably not always valid
      return val == _standardized
             && _forest.number_of_nodes()
                    == word_graph().number_of_nodes_active();
    }

    inline bool is_standardized() const {
      // TODO this is probably not always valid, i.e. if we are standardized,
      // then grow, then collapse, but end up with the same number of nodes
      // again.
      return _standardized != order::none
             && _forest.number_of_nodes()
                    == word_graph().number_of_nodes_active();
    }

    inline order standardization_order() const noexcept {
      return _standardized;
    }

    bool contains(word_type const& lhs, word_type const& rhs) override;

   private:
    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    void run_impl() override {
      if (is_obviously_infinite(*this)) {
        LIBSEMIGROUPS_EXCEPTION(
            "there are infinitely many classes in the congruence and "
            "Todd-Coxeter will never terminate");
      }
      init_run();

      if (strategy() == options::strategy::felsch) {
        felsch();
      } else if (strategy() == options::strategy::hlt) {
        hlt();
      }

      finalise_run();

      /*else if (strategy() == options::strategy::random) {
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
      }*/
    }

    bool finished_impl() const override {
      return _finished;
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    word_type class_index_to_word_impl(class_index_type i) override;

    size_t number_of_classes_impl() override;

    class_index_type word_to_class_index_impl(word_type const& w) override;

    class_index_type
    const_word_to_class_index(word_type const& w) const override;

    void validate_word(word_type const& w) const override;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (main strategies) - private
    ////////////////////////////////////////////////////////////////////////

    void init_run();
    void finalise_run();

    void felsch();
    void hlt();

    void report_active_nodes() const;
    void report_next_lookahead(size_t old_value) const;
    void report_nodes_killed(int64_t number) const;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - member functions (lookahead) - private
    ////////////////////////////////////////////////////////////////////////

    // TODO(refactor) move to digraph (largest_compatible_quotient or similar)
    void perform_lookahead();
    // TODO(refactor) move to digraph
    size_t hlt_lookahead();
    // TODO(refactor) move to digraph
    size_t felsch_lookahead();
  };

  namespace detail {
    using node_type = typename ToddCoxeter::node_type;
    struct NormalFormIteratorTraits
        : ConstIteratorTraits<IntegralRange<node_type>> {
      using value_type      = word_type;
      using const_reference = value_type const;
      using reference       = value_type;
      using const_pointer   = value_type const*;
      using pointer         = value_type*;

      using state_type = ToddCoxeter*;

      struct Deref {
        value_type operator()(state_type                               tc,
                              IntegralRange<node_type>::const_iterator it) {
          // It might seem better to just use forest::cbegin_paths, but we can't
          // because tc.kind() (runtime) determines whether or not the paths
          // should be reversed.
          return tc->class_index_to_word(*it);
        }
      };

      struct AddressOf {
        pointer operator()(state_type,
                           IntegralRange<node_type>::const_iterator) {
          LIBSEMIGROUPS_ASSERT(false);
          return nullptr;
        }
      };
    };
  }  // namespace detail

  namespace todd_coxeter {
    using node_type = typename ToddCoxeter::node_type;

    inline auto cbegin_class(ToddCoxeter& tc,
                             node_type    n,
                             size_t       min = 0,
                             size_t       max = POSITIVE_INFINITY) {
      // TODO don't add 1 if tc contains empty word?
      return tc.word_graph().cbegin_pstislo(0, n + 1, min, max);
    }

    inline auto cbegin_class(ToddCoxeter&     tc,
                             word_type const& w,
                             size_t           min = 0,
                             size_t           max = POSITIVE_INFINITY) {
      // TODO don't add 1 if tc contains empty word
      return tc.word_graph().cbegin_pstislo(
          0, tc.word_to_class_index(w) + 1, min, max);
    }

    inline auto cend_class(ToddCoxeter& tc) {
      return tc.word_graph().cend_pstislo();
    }

    // TODO -> size_of_class?
    inline auto number_of_words(ToddCoxeter const& tc, node_type i) {
      return tc.word_graph().number_of_paths(0, i + 1, 0, POSITIVE_INFINITY);
    }

    //! The type of a const iterator pointing to a normal form.
    //!
    //! Iterators of this type point to a \ref word_type.
    //!
    //! \sa cbegin_normal_forms, cend_normal_forms.
    // TODO(refactor): redo the doc
    using normal_form_iterator
        = detail::ConstIteratorStateful<detail::NormalFormIteratorTraits>;

    //! Returns a \ref normal_form_iterator pointing at the first normal
    //! form.
    //!
    //! Returns a const iterator pointing to the normal form of the first
    //! class of the congruence represented by an instance of ToddCoxeter.
    //! The order of the classes, and the normal form, that is returned are
    //! controlled by standardize(order).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref normal_form_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(refactor): redo the doc
    inline normal_form_iterator cbegin_normal_forms(ToddCoxeter& tc) {
      auto range = IntegralRange<node_type>(0, tc.number_of_classes());
      // TODO use rx::range instead
      return normal_form_iterator(&tc, range.cbegin());
    }

    //! Returns a \ref normal_form_iterator pointing one past the last normal
    //! form.
    //!
    //! Returns a const iterator one past the normal form of the last class
    //! of the congruence represented by an instance of ToddCoxeter. The
    //! order of the classes, and the normal form, that is returned are
    //! controlled by standardize(order).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref normal_form_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(refactor): redo the doc
    inline normal_form_iterator cend_normal_forms(ToddCoxeter& tc) {
      auto range = IntegralRange<node_type>(0, tc.number_of_classes());
      return normal_form_iterator(&tc, range.cend());
    }

    inline word_type normal_form(ToddCoxeter& tc, word_type const& w) {
      return tc.class_index_to_word(tc.word_to_class_index(w));
    }

    template <typename It>
    std::vector<std::vector<word_type>> partition(ToddCoxeter& tc,
                                                  It           first,
                                                  It           last) {
      using return_type = std::vector<std::vector<word_type>>;

      return_type result(tc.number_of_classes(), std::vector<word_type>());

      for (auto it = first; it != last; ++it) {
        LIBSEMIGROUPS_ASSERT(tc.word_to_class_index(*it) < result.size());
        result[tc.word_to_class_index(*it)].push_back(*it);
      }
      return result;
    }

    template <typename It>
    std::vector<std::vector<word_type>> non_trivial_classes(ToddCoxeter& tc,
                                                            It           first,
                                                            It           last) {
      auto result = partition(tc, first, last);

      result.erase(
          std::remove_if(result.begin(),
                         result.end(),
                         [](auto const& x) -> bool { return x.size() <= 1; }),
          result.end());
      return result;
    }
  }  // namespace todd_coxeter

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TODD_COXETER_HPP_