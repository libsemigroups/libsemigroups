//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
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

namespace libsemigroups {

  // The following deduction guides can only really be used in this file,
  // because we don't expose the type of the elements in FroidurePin objects
  // arising from Kambites, KnuthBendix, or ToddCoxeter

  template <typename Word>
  FroidurePin(Kambites<Word> const&) -> FroidurePin<detail::KE<Word>>;

  template <typename Word, typename Rewriter, typename ReductionOrder>
  FroidurePin(KnuthBendix<Word, Rewriter, ReductionOrder> const&)
      -> FroidurePin<detail::KBE<KnuthBendix<Word, Rewriter, ReductionOrder>>>;

  FroidurePin(detail::ToddCoxeterImpl const&)->FroidurePin<detail::TCE>;

  ////////////////////////////////////////////////////////////////////////
  // Congruence
  ////////////////////////////////////////////////////////////////////////

  template <template <typename...> typename Thing, typename Word>
  auto to(Congruence<Word>& cong)
      -> std::enable_if_t<std::is_same_v<Thing<int>, FroidurePin<int>>,
                          std::unique_ptr<FroidurePinBase>> {
    cong.run();
    if (cong.template has<Kambites<Word>>()) {
      // TODO(2) there an issue here that if the Kambites clause isn't first,
      // then we incorrectly start running the other algos here, which run
      // forever. Probably something goes wrong that the other runners don't
      // get deleted if Kambites wins, since it's run first.
      auto fp = to<FroidurePin>(*cong.template get<Kambites<Word>>());
      return std::make_unique<decltype(fp)>(std::move(fp));
    } else if (cong.template has<ToddCoxeter<Word>>()) {
      auto fp = to<FroidurePin>(*cong.template get<ToddCoxeter<Word>>());
      return std::make_unique<decltype(fp)>(std::move(fp));
    } else if (cong.template has<KnuthBendix<Word>>()) {
      auto fp = to<FroidurePin>(*cong.template get<KnuthBendix<Word>>());
      return std::make_unique<decltype(fp)>(std::move(fp));
    }
    LIBSEMIGROUPS_EXCEPTION("It is not possible to construct a FroidurePin "
                            "object from the 1st argument (Congruence)");
  }

  ////////////////////////////////////////////////////////////////////////
  // Kambites
  ////////////////////////////////////////////////////////////////////////

  template <template <typename...> typename Thing, typename Word>
  auto to(Kambites<Word>& k) -> std::enable_if_t<
      std::is_same_v<Thing<detail::KE<Word>>, FroidurePin<detail::KE<Word>>>,
      FroidurePin<detail::KE<Word>>> {
    if (k.small_overlap_class() < 4) {
      LIBSEMIGROUPS_EXCEPTION(
          "the small overlap class of the argument must be >= 4, found {}",
          k.small_overlap_class());
    }

    FroidurePin result(k);

    size_t const n = k.presentation().alphabet().size();
    for (size_t i = 0; i < n; ++i) {
      result.add_generator(detail::KE<Word>(k, i));
    }
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // KnuthBendix
  ////////////////////////////////////////////////////////////////////////

  template <template <typename...> typename Thing,
            typename Word,
            typename Rewriter,
            typename ReductionOrder>
  auto to(KnuthBendix<Word, Rewriter, ReductionOrder>& kb) -> std::enable_if_t<
      std::is_same_v<Thing<int>, FroidurePin<int>>,
      FroidurePin<detail::KBE<KnuthBendix<Word, Rewriter, ReductionOrder>>>> {
    size_t const n = kb.presentation().alphabet().size();

    if (n == 0) {
      LIBSEMIGROUPS_EXCEPTION("Cannot create a FroidurePin object from a "
                              "KnuthBendix object with empty alphabet");
    } else if (kb.kind() != congruence_kind::twosided) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument must be a 2-sided congruence, found a {} congruence",
          kb.kind());
    }
    kb.run();
    // kb.rewriter().process_pending_rules();

    FroidurePin result(kb);
    using KBE = typename decltype(result)::element_type;
    for (auto const& a : kb.presentation().alphabet()) {
      result.add_generator(KBE(kb, a));
    }
    if (kb.presentation().contains_empty_word()) {
      result.add_generator(KBE(kb, Word()));
    }
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // Konieczny
  ////////////////////////////////////////////////////////////////////////

  template <template <typename...> typename Thing,
            typename Element,
            typename Traits>
  auto to(Konieczny<Element, Traits> const& k)
      -> std::enable_if_t<std::is_same_v<Thing<int>, FroidurePin<int>>,
                          FroidurePin<Element>> {
    return FroidurePin(k.cbegin_generators(), k.cend_generators());
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter
  ////////////////////////////////////////////////////////////////////////

  template <template <typename...> typename Thing>
  auto to(detail::ToddCoxeterImpl& tc) -> std::enable_if_t<
      std::is_same_v<Thing<detail::TCE>, FroidurePin<detail::TCE>>,
      FroidurePin<detail::TCE>> {
    using word_graph_type = typename detail::ToddCoxeterImpl::word_graph_type;

    if (tc.kind() != congruence_kind::twosided) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument must be a two-sided congruence, found a {} congruence",
          tc.kind());
    }

    tc.run();
    tc.shrink_to_fit();
    // Ensure class indices and letters are equal!
    auto wg        = std::make_shared<word_graph_type>(tc.current_word_graph());
    size_t const n = tc.current_word_graph().out_degree();
    size_t       m = n;
    for (letter_type a = 0; a < m;) {
      if (wg->target_no_checks(0, a) != a + 1) {
        wg->remove_label(a);
        m--;
      } else {
        ++a;
      }
    }
    using TCE = detail::TCE;
    FroidurePin<TCE> result(wg);
    for (size_t i = 0; i < n; ++i) {
      // We use _word_graph.target_no_checks instead of just i, because there
      // might be more generators than cosets.
      result.add_generator(TCE(tc.current_word_graph().target_no_checks(0, i)));
    }
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // WordGraph
  ////////////////////////////////////////////////////////////////////////

  template <typename Result, typename Node>
  std::enable_if_t<
      std::is_same_v<FroidurePin<typename Result::element_type>, Result>,
      Result>
  to(WordGraph<Node> const& wg, size_t first, size_t last) {
    using Element = typename Result::element_type;

    if (first >= last) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 2nd argument (first node) must be strictly less than "
          "the 3rd argument (last node), found {} >= {}",
          first,
          last);
    } else if (first > wg.number_of_nodes()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 2nd argument (first node) must be at most the out-degree of the "
          "1st argument (WordGraph), found {} > {}",
          first,
          wg.out_degree());
    } else if (last > wg.number_of_nodes()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 3rd argument (last node) must be at most the out-degree of the "
          "1st argument (WordGraph), found {} > {}",
          last,
          wg.out_degree());
    }

    LIBSEMIGROUPS_ASSERT(wg.out_degree() > 0);
    FroidurePin<Element> result;
    Element              x(last - first);
    // Each label corresponds to a generator of S
    for (Node a = 0; a < wg.out_degree(); ++a) {
      for (size_t s = first; s < last; ++s) {
        auto t = wg.target_no_checks(s, a);
        if (t != UNDEFINED) {
          x[s - first] = t - first;
        } else {
          x[s - first] = UNDEFINED;
        }
      }
      // The next loop is required because if element_type is a fixed degree
      // type, such as Transf<5> for example, but first = last = 0, then the
      // degree of x is still 5 not last - first = 0.
      for (size_t s = last - first; s < x.degree(); ++s) {
        x[s] = s;
      }
      throw_if_image_value_out_of_range(x);
      result.add_generator(x);
    }
    return result;
  }
}  // namespace libsemigroups
