//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_MAKE_PRESENT_HPP_
#define LIBSEMIGROUPS_MAKE_PRESENT_HPP_

#include <algorithm>    // for transform
#include <string>       // for string
#include <type_traits>  // for is_base_of, enable_if_t
#include <vector>       // for vector

#include "froidure-pin-base.hpp"  // for FroidurePinBase::const_rule_i...
#include "present.hpp"            // for Presentation
#include "word.hpp"               // for word_to_string

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
  template <typename T,
            typename
            = std::enable_if_t<std::is_base_of<PresentationBase, T>::value>>
  T make(FroidurePinBase& fp) {
    T p;
    p.alphabet(fp.number_of_generators());
    for (auto it = fp.cbegin_rules(); it != fp.cend_rules(); ++it) {
      p.add_rule(it->first.cbegin(),
                 it->first.cend(),
                 it->second.cbegin(),
                 it->second.cend());
    }

    return p;
  }

  //! Make presentation with strings from a FroidurePin object
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
  //! \tparam T the type of the presentation to construct (must be a type of
  //! Presentation).
  //! \param fp the FroidurePin object from which to obtain the rules.
  //! \param alphabet the alphabet of the presentation to be constructed.
  //!
  //! \returns An object of type \c T.
  //!
  //! \throws LibsemigroupsException if the length of \p alphabet is not equal
  //! to `fp.number_of_generators()`.
  //! \throws LibsemigroupsException if `Presentation::alphabet(alphabet)`
  //! throws.
  Presentation<std::string> make(FroidurePinBase&   fp,
                                 std::string const& alphabet);

  //! Make a presentation from a different type of presentation.
  //!
  //! Returns a presentation equivalent to the input presentation but of a
  //! different type (for example, can be used to convert from \string to \ref
  //! word_type). The second parameter specifies how to map the letters of one
  //! presentation to the other.
  //!
  //! \tparam S the type of the returned presentation, must be a type of
  //! Presentation and distinct from \p T
  //! \tparam T the type of the input presentation, must be a type of
  //! Presentation and distinct from \p S
  //! \tparam F the type of a function from transforming letters
  //! \param p the input presentation
  //! \param f a function mapping S::letter_type to T::letter_type
  //!
  //! \returns A value of type \p S.
  //! \throws LibsemigroupsException if `p.validate()` throws.
  template <typename S,
            typename T,
            typename F,
            typename
            = std::enable_if_t<std::is_base_of<PresentationBase, S>::value
                               && std::is_base_of<PresentationBase, T>::value>>
  S make(T const& p, F&& f) {
    using s_word_type = typename S::word_type;
    p.validate();
    // Must call p.validate otherwise p.index(val) might seg fault below

    S result;
    result.contains_empty_word(p.contains_empty_word());
    s_word_type new_alphabet;
    new_alphabet.resize(p.alphabet().size());
    std::transform(
        p.alphabet().cbegin(), p.alphabet().cend(), new_alphabet.begin(), f);
    result.alphabet(new_alphabet);
    s_word_type rel;
    for (auto it = p.rules.cbegin(); it != p.rules.cend(); ++it) {
      rel.resize(it->size());
      std::transform(it->cbegin(), it->cend(), rel.begin(), f);
      result.rules.push_back(rel);
      rel.clear();
    }
    return result;
  }

  //! Make a presentation from a different type of presentation.
  //!
  //! Returns a presentation equivalent to the input presentation but of a
  //! different type (for example, can be used to convert from
  //! \string to \ref word_type).
  //!
  //! The alphabet of the returned presentation is \f$\{0, \ldots, n - 1\}\f$
  //! where \f$n\f$ is the size of the alphabet of the input presentation.
  //!
  //! \tparam S the type of the returned presentation, must be a type of
  //! Presentation and distinct from \p T
  //! \tparam T the type of the input presentation, must be a type of
  //! Presentation and distinct from \p S
  //!
  //! \param p the input presentation
  //!
  //! \returns A value of type \p S.
  //! \throws LibsemigroupsException if `p.validate()` throws.
  template <typename S,
            typename T,
            typename
            = std::enable_if_t<std::is_base_of<PresentationBase, S>::value
                               && std::is_base_of<PresentationBase, T>::value>>
  S make(T const& p) {
    return make<S>(p, [&p](auto val) { return p.index(val); });
  }

  //! Make a string presentation from a different type of presentation.
  //!
  //! Returns a presentation equivalent to the input presentation but of a
  //! different type (for example, can be used to convert from \string to \ref
  //! word_type).
  //!
  //! The alphabet of the returned presentation is the second parameter \p
  //! alphabet where \f$n\f$ is the size of the alphabet of the input
  //! presentation.
  //!
  //! \tparam T the type of the input presentation, must be a type of
  //! Presentation and distinct from \p S
  //!
  //! \param p the input presentation
  //! \param alphabet the output presentations alphabet
  //!
  //! \returns A value of type Presentation<std::string>.
  //!
  //! \throws LibsemigroupsException if `p.validate()` throws.
  template <typename T,
            typename
            = std::enable_if_t<std::is_base_of<PresentationBase, T>::value>>
  // The first parameter is passed by value intentionally since
  // normalize_alphabet rewrites the presentation
  Presentation<std::string> make(T p, std::string const& alphabet) {
    // to ensure that alphabet is 0, .., n - 1.
    presentation::normalize_alphabet(p);
    return make<Presentation<std::string>>(
        p, [&alphabet](auto val) { return alphabet[val]; });
  }

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_MAKE_PRESENT_HPP_
