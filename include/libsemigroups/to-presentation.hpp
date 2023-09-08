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

// TODO tpp file

#ifndef LIBSEMIGROUPS_TO_PRESENTATION_HPP_
#define LIBSEMIGROUPS_TO_PRESENTATION_HPP_

#include <algorithm>    // for transform
#include <string>       // for string
#include <type_traits>  // for is_base_of, enable_if_t
#include <vector>       // for vector

#include "froidure-pin-base.hpp"  // for FroidurePinBase::const_rule_i...
#include "presentation.hpp"       // for Presentation

namespace libsemigroups {

  //! Make presentation from a FroidurePin object.
  //!
  //! This function constructs and returns a Presentation object using the
  //! rules of a FroidurePin object, accessed via FroidurePin::cbegin_rules and
  //! FroidurePin::cend_rules.
  //!
  //! No enumeration of the argument \p fp is performed, so it might be the
  //! case that the resulting presentation does not define the same semigroup
  //! as \p fp. To ensure that the resulting presentation defines the same
  //! semigroup as \p fp, run FroidurePin::run (or any other function that
  //! fully enumerates \p fp) prior to calling this function.
  //!
  //!
  //! \tparam T the type of the presentation to construct (must be a type of
  //! Presentation).
  //! \param fp the FroidurePin object from which to obtain the rules.
  //!
  //! \returns An object of type \c T.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename WordOutput>  // TODO why not just to_presentation with no
                                  // template WordOutput, always use word_type,
                                  // and can be converted after if desirable
  Presentation<WordOutput> to_presentation(FroidurePinBase& fp) {
    Presentation<WordOutput> p;
    p.alphabet(fp.number_of_generators());
    for (auto it = fp.cbegin_rules(); it != fp.cend_rules(); ++it) {
      p.add_rule(it->first.cbegin(),
                 it->first.cend(),
                 it->second.cbegin(),
                 it->second.cend());
    }

    return p;
  }

  //! Make a presentation from a different type of presentation.
  //!
  //! Returns a presentation equivalent to the input presentation but of a
  //! different type (for example, can be used to convert from \string to \ref
  //! word_type). The second parameter specifies how to map the letters of one
  //! presentation to the other.
  //!
  //! \tparam S the type of the returned presentation, must be a type of
  //! Presentation
  //! \tparam W the type of the words in the input presentation
  //! \tparam F the type of a function from transforming letters
  //! \param p the input presentation
  //! \param f a function mapping `S::letter_type` to
  //! Presentation<W>::letter_type
  //!
  //! \returns A value of type \p S.
  //! \throws LibsemigroupsException if `p.validate()` throws.
  template <typename WordOutput,
            typename WordInput,
            typename Func,
            typename = std::enable_if_t<std::is_invocable_v<
                std::decay_t<Func>,
                typename Presentation<WordInput>::letter_type>>>
  // TODO add WordOutput != WordInput as for inverse
  // presentations below
  Presentation<WordOutput> to_presentation(Presentation<WordInput> const& p,
                                           Func&&                         f) {
    p.validate();
    // Must call p.validate otherwise p.index(val) might seg fault below

    Presentation<WordOutput> result;
    result.contains_empty_word(p.contains_empty_word());
    WordOutput new_alphabet;
    new_alphabet.resize(p.alphabet().size());
    std::transform(
        p.alphabet().cbegin(), p.alphabet().cend(), new_alphabet.begin(), f);
    result.alphabet(new_alphabet);
    WordOutput rel;
    for (auto it = p.rules.cbegin(); it != p.rules.cend(); ++it) {
      rel.resize(it->size());
      std::transform(it->cbegin(), it->cend(), rel.begin(), f);
      result.rules.push_back(rel);  // TODO std::move?
      rel.clear();
    }
    return result;
  }

  //! No doc
  // Really the doc is in docs/source/api/present-helper.rst since JDM
  // couldn't figure out how to use doxygenfunction in this case (since there
  // are multiple function templates with the same arguments, just different
  // type constraints).
  template <typename WordOutput,
            template <typename>
            class Presentation_,
            typename WordInput>
  auto to_presentation(Presentation_<WordInput> const& p)
      -> std::enable_if_t<!std::is_same_v<WordOutput, std::string>,
                          Presentation_<WordOutput>> {
    return to_presentation<WordOutput>(p,
                                       [&p](auto val) { return p.index(val); });
  }

  //! No doc
  template <typename WordOutput,
            template <typename>
            class Presentation_,
            typename WordInput>
  auto to_presentation(Presentation_<WordInput> const& p)
      -> std::enable_if_t<std::is_same_v<WordOutput, std::string>,
                          Presentation_<WordOutput>> {
    return to_presentation<WordOutput>(
        p, [&p](auto val) { return human_readable_char(p.index(val)); });
  }

  template <
      typename WordOutput,
      typename WordInput,
      typename Func,
      typename = std::enable_if_t<
          std::is_invocable_v<std::decay_t<Func>,
                              typename Presentation<WordInput>::letter_type>
          && !std::is_same_v<WordOutput, WordInput>>>
  InversePresentation<WordOutput>
  to_presentation(InversePresentation<WordInput> const& p, Func&& f) {
    WordOutput new_alphabet;
    new_alphabet.resize(p.alphabet().size());
    std::transform(
        p.alphabet().cbegin(), p.alphabet().cend(), new_alphabet.begin(), f);

    WordOutput new_inverses;
    new_inverses.resize(p.inverses().size());
    std::transform(
        p.inverses().cbegin(), p.inverses().cend(), new_inverses.begin(), f);

    InversePresentation<WordOutput> result;
    result.contains_empty_word(p.contains_empty_word());
    result.alphabet(new_alphabet);
    result.inverses(new_inverses);

    WordOutput rel;
    for (auto it = p.rules.cbegin(); it != p.rules.cend(); ++it) {
      rel.resize(it->size());
      std::transform(it->cbegin(), it->cend(), rel.begin(), f);
      result.rules.push_back(rel);  // TODO std::move?
      rel.clear();
    }
    return result;
  }

  template <typename Word>
  InversePresentation<Word>
  to_presentation(InversePresentation<Word> const& p) {
    return p;
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TO_PRESENTATION_HPP_
