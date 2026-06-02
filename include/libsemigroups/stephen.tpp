//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2026 James D. Mitchell
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

// This file contains the implementation of the Stephen class template.

namespace libsemigroups {

  template <typename PresentationType>
  Stephen<PresentationType>::Stephen()
      : StephenImpl_(), _presentation(), _word() {}

  template <typename PresentationType>
  Stephen<PresentationType>& Stephen<PresentationType>::init() {
    StephenImpl_::init();
    _presentation.init();
    _word.clear();
    return *this;
  }

  template <typename PresentationType>
  Stephen<PresentationType>&
  Stephen<PresentationType>::init(PresentationType&& p) {
    // Throw this here because o/w "to" fails below
    if (p.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("the presentation must not have 0 generators");
    }
    if constexpr (!std::is_same_v<typename PresentationType::word_type,
                                  word_type>) {
      // to<Presentation> throws in the next line if p isn't valid
      StephenImpl_::init(v4::to<internal_presentation_type>(p));
      _presentation = std::move(p);
    } else {
      // PresentationType = Presentation<word_type> or
      // InversePresentation<word_type>
      p.throw_if_bad_alphabet_or_rules();
      _presentation = p;  // copy p in to _presentation
      presentation::normalize_alphabet(p);
      StephenImpl_::init(std::move(p));
    }
    _word.clear();
    return *this;
  }

  template <typename PresentationType>
  Stephen<PresentationType>&
  Stephen<PresentationType>::init(PresentationType const& p) {
    // Call the init overload above
    return init(PresentationType(p));
  }

  template <typename PresentationType>
  Stephen<PresentationType>::~Stephen() = default;

  ////////////////////////////////////////////////////////////////////////
  // Public
  ////////////////////////////////////////////////////////////////////////

  template <typename PresentationType>
  template <typename Iterator1, typename Iterator2>
  Stephen<PresentationType>&
  Stephen<PresentationType>::set_word_no_checks(Iterator1 first,
                                                Iterator2 last) {
    StephenImpl_::set_internal_word_no_checks(detail::citow(this, first),
                                              detail::citow(this, last));
    _word.assign(first, last);
    return *this;
  }

  template <typename PresentationType>
  void Stephen<PresentationType>::operator*=(Stephen<PresentationType>& that) {
    // It's possible for distinct presentations to have the same normalized
    // presentation and so we do this check here.
    if (this->presentation() != that.presentation()) {
      LIBSEMIGROUPS_EXCEPTION(
          "this.presentation() must equal that.presentation() when appending "
          "Stephen instances")
    }
    _word.insert(_word.end(), that._word.cbegin(), that._word.cend());
    StephenImpl_::operator*=(that);
  }

  namespace stephen {
    template <typename PresentationType>
    bool accepts_no_checks(Stephen<PresentationType>&                  s,
                           typename PresentationType::word_type const& w) {
      // TODO rename the function detail::stephen::accepts_no_checks
      return detail::stephen::accepts(
          s, detail::citow(&s, w.begin()), detail::citow(&s, w.end()));
    }

    template <typename PresentationType>
    bool accepts(Stephen<PresentationType>&                  s,
                 typename PresentationType::word_type const& w) {
      if (!w.empty()) {
        // Here we always allow w to be empty, but the following line throws if
        // s.presentation() does not contain the empty word and w is empty.
        s.presentation().throw_if_letter_not_in_alphabet(w.begin(), w.end());
      }
      return accepts_no_checks(s, w);
    }

    template <typename PresentationType>
    bool
    is_left_factor_no_checks(Stephen<PresentationType>&                  s,
                             typename PresentationType::word_type const& w) {
      // TODO rename the function detail::stephen::is_left_factor_no_checks
      return detail::stephen::is_left_factor(
          s, detail::citow(&s, w.begin()), detail::citow(&s, w.end()));
    }

    template <typename PresentationType>
    bool is_left_factor(Stephen<PresentationType>&                  s,
                        typename PresentationType::word_type const& w) {
      if (!w.empty()) {
        // Here we always allow w to be empty, but the following line throws if
        // s.presentation() does not contain the empty word and w is empty.
        s.presentation().throw_if_letter_not_in_alphabet(w.begin(), w.end());
      }
      return is_left_factor_no_checks(s, w);
    }

    template <typename PresentationType>
    Dot dot(Stephen<PresentationType>& s) {
      Dot result;
      result.kind(Dot::Kind::digraph);
      result.add_node("initial").add_attr("style", "invis");
      result.add_node("accept").add_attr("style", "invis");
      for (auto n : s.word_graph_no_run().nodes()) {
        result.add_node(n).add_attr("shape", "box");
      }
      result.add_edge("initial", s.initial_state());
      result.add_edge(s.accept_state(), "accept");

      size_t max_letters = s.presentation().alphabet().size();
      if constexpr (is_specialization_of_v<PresentationType,
                                           InversePresentation>) {
        max_letters /= 2;
      }

      for (auto n : s.word_graph_no_run().nodes()) {
        for (size_t a = 0; a < max_letters; ++a) {
          auto m = s.word_graph_no_run().target(n, a);
          if (m != UNDEFINED) {
            result.add_edge(n, m)
                .add_attr("color", result.colors[a])
                .add_attr("label", a)
                .add_attr("minlen", 2);
          }
        }
      }
      return result;
    }
  }  // namespace stephen

  template <typename PresentationType>
  bool operator==(Stephen<PresentationType> const& x,
                  Stephen<PresentationType> const& y) {
    if (x.presentation() != y.presentation()) {
      LIBSEMIGROUPS_EXCEPTION("the presentations of the arguments must be "
                              "equal when comparing Stephen instances")
    }
    return equal_to_no_checks(x, y);
  }

  template <typename PresentationType>
  std::string to_human_readable_repr(Stephen<PresentationType> const& x) {
    using detail::group_digits;
    if (!x.is_word_set()) {
      return fmt::format("<Stephen object over {} with no word set>",
                         to_human_readable_repr(x.presentation()));
    }
    return fmt::format(
        "<Stephen object over {} for {} with {} "
        "nodes and {} edges>",
        to_human_readable_repr(x.presentation()),
        x.word().size() < 10
            ? fmt::format("word {}", x.word())
            : fmt::format("{} letter word", group_digits(x.word().size())),
        group_digits(x.word_graph_no_run().number_of_active_nodes()),
        group_digits(x.word_graph_no_run().number_of_edges()));
  }

}  // namespace libsemigroups
