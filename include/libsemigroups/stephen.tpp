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

  }  // namespace stephen

  namespace detail {
    template <typename PresentationType>
    Dot stephen_dot(Stephen<PresentationType>&                  s,
                    typename PresentationType::word_type const& alphabet,
                    size_t                                      radius,
                    bool use_inverse_literals) {
      using Word = typename PresentationType::word_type;

      if constexpr (is_specialization_of_v<PresentationType, Presentation>) {
        LIBSEMIGROUPS_ASSERT(!use_inverse_literals);
      }

      static std::string_view const empty  = "\u03B5";
      static std::string_view const inv    = "\u207B\u00B9";
      std::string                   font   = "STIX Two Text";
      static std::string_view const italic = " Italic";
      if (std::is_same_v<Word, std::string>) {
        font += italic;
      }

      auto const& p    = s.presentation();
      auto const& wg   = s.word_graph_no_run();
      auto const  root = s.initial_state();

      throw_if_duplicates(alphabet.begin(), alphabet.end(), "alphabet");
      // Check that alphabet contains only letters in p.alphabet()
      auto invalid_it = std::find_if_not(
          alphabet.cbegin(), alphabet.cend(), [&p](auto letter) {
            return p.alphabet_v4().contains(letter);
          });
      if (invalid_it != alphabet.cend()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the alphabet {} contains invalid letters, expected values in {} "
            "but found {} in position {}",
            to_printable(alphabet),
            to_printable(p.alphabet()),
            *invalid_it,
            std::distance(alphabet.cbegin(), invalid_it));
      }

      {
        // Check that we can render generators as single characters properly.
        // It's okay if there are letters in p.alphabet() that can't be
        // rendered, as long as all of the letters in "alphabet" are
        // renderable.
        auto it = std::find_if_not(
            alphabet.begin(), alphabet.end(), [](auto letter) {
              if constexpr (std::is_same_v<Word, std::string>) {
                return std::isprint(letter) || letter < 10;
              } else {
                return letter < 10;
              }
            });
        // Too large generators like "10" will be rendered as "10", making
        // words like "110" impossible to parse.
        if (it != alphabet.end()) {
          LIBSEMIGROUPS_EXCEPTION(
              "the alphabet {} contains letters that may render ambiguously, "
              "expected a printable char or a value <= 9 but found {} in "
              "position {}",
              to_printable(alphabet),
              to_printable(*it),
              std::distance(alphabet.begin(), it));
        }
      }

      if (Dot::colors.size() < alphabet.size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the alphabet contains too many letters, expected at most {} (= "
            "Dot::colors.size()), found {}",
            Dot::colors.size(),
            alphabet.size());
      }

      Dot result;
      result.add_node("initial").add_attr("style", "invis");
      result.add_node("accept").add_attr("style", "invis");

      auto to_print = [](auto letter) {
        if (!std::isprint(letter)) {
          return static_cast<char>('0' + letter);
        }
        return static_cast<char>(letter);
      };

      // Strictly speaking it isn't necessary to get both "nodes" and "f" below,
      // but we do because it's convenient and this is for drawing a picture and
      // so shouldn't be done when the number of nodes is large.
      auto nodes = v4::word_graph::nodes_reachable_from(wg, root, radius);
      auto f     = v4::word_graph::spanning_tree_no_checks(wg, root, radius);
      for (auto n : nodes) {
        std::string label;
        f.path_from_root_no_checks(std::back_inserter(label), n);
        auto& node = result.add_node(n).add_attr("shape", "box");
        if (n == root) {
          node.add_attr("label", empty);
          node.add_attr("fontname", "STIX Two Text Italic");
        } else {
          for (auto it = label.begin(); it != label.end(); ++it) {
            // At this point label consists of indices, not letters!
            auto const letter = p.letter_no_checks(*it);
            if constexpr (is_specialization_of_v<PresentationType,
                                                 InversePresentation>) {
              if (use_inverse_literals
                  && std::find(alphabet.begin(), alphabet.end(), letter)
                         == alphabet.end()) {
                size_t pos = std::distance(label.begin(), it);
                *it        = to_print(p.inverse(letter));
                // Stupid cygwin is not standard compliant, and label.insert has
                // return type void, so we can't just use it = label.insert(...)
                // below like normal people. Hence the shenanigans with "pos"
                label.insert(it + 1, inv.begin(), inv.end());
                it = label.begin() + pos + inv.size();
                continue;
              }
            }
            *it = to_print(letter);
          }
          node.add_attr("label", std::move(label));
          node.add_attr("fontname", font);
        }
      }

      result.add_edge("initial", s.initial_state());
      if (s.finished()) {
        // We check this because accept_state() calls run() and we don't want
        // to modify "s".
        result.add_edge(s.accept_state(), "accept");
      }

      for (auto src : s.word_graph_no_run().nodes()) {
        for (auto letter : alphabet) {
          auto const index = p.index_no_checks(letter);
          auto const tgt   = wg.target(src, index);
          if (result.is_node(src) && tgt != UNDEFINED && result.is_node(tgt)) {
            result.add_edge(src, tgt)
                .add_attr("color", result.colors[index])
                // The next line uses std::string since if we just pass
                // to_print(letter) it's interpreted as an integer.
                .add_attr("label", std::string(1, to_print(letter)))
                .add_attr("fontname", font);
          }
        }
      }
      return result;
    }
  }  // namespace detail

  namespace stephen {
    template <typename Word>
    Dot dot(Stephen<Presentation<Word>>& s, size_t radius) {
      return detail::stephen_dot(s, s.presentation().alphabet(), radius, false);
    }

    template <typename Word>
    Dot dot(Stephen<Presentation<Word>>& s,
            Word const&                  alphabet,
            size_t                       radius) {
      return detail::stephen_dot(s, alphabet, radius, false);
    }

    template <typename Word>
    Dot dot(Stephen<InversePresentation<Word>>& s,
            size_t                              radius,
            bool                                use_inverse_literals) {
      auto alphabet
          = presentation::inverse_alphabet_no_checks(s.presentation());
      return detail::stephen_dot(s, alphabet, radius, use_inverse_literals);
    }

    template <typename Word>
    Dot dot(Stephen<InversePresentation<Word>>& s,
            Word const&                         alphabet,
            size_t                              radius,
            bool                                use_inverse_literals) {
      return detail::stephen_dot(s, alphabet, radius, use_inverse_literals);
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
