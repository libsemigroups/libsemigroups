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

// This file contains the declaration of the class KBE, which can be used as
// the element_type for a FroidurePin instance. This class essentially wraps a
// reduced word of a KnuthBendix instance.

// For technical reasons this is implemented in src/kbe-impl.hpp

#ifndef LIBSEMIGROUPS_INCLUDE_KBE_HPP_
#define LIBSEMIGROUPS_INCLUDE_KBE_HPP_

#include <cstddef>  // for size_t
#include <string>   // for string
#include <utility>  // for hash

#include "adapters.hpp"      // for One
#include "element.hpp"       // for Element
#include "froidure-pin.hpp"  // for FroidurePin
#include "knuth-bendix.hpp"  // for fpsemigroup::KnuthBendix
#include "types.hpp"         // for word_type, letter_type

namespace libsemigroups {
  namespace detail {
    // Subclass of Element that wraps an libsemigroups::internal_string_type.
    //
    // This class is used to wrap libsemigroups::internal_string_type into an
    // Element so that it is possible to use them as generators for a
    // FroidurePin object.
    class KBE final : public Element {
      using KnuthBendix          = fpsemigroup::KnuthBendix;
      using internal_string_type = std::string;

     private:
      KBE(KnuthBendix*, internal_string_type, bool);

     public:
      // Constructs a KBE which does not represent a word or have an
      // associated rewriting system.
      KBE() = default;

      KBE(KBE const&) = default;
      KBE(KBE&&)      = default;
      KBE& operator=(KBE const&) = default;
      KBE& operator=(KBE&&) = default;
      ~KBE()                = default;

      // Constructor from a rewriting system and a word.
      //
      // Constructs a KBE which represents the empty word, where multiplication
      // with other KBE's is defined with respect to the rewriting system \p
      // kb.
      //
      // The rewriting system \p kb is not copied, and it is the
      // responsibility of the caller to delete it.
      explicit KBE(KnuthBendix*);

      // Constructor from a rewriting system and a word.
      //
      // Constructs a KBE which is essentially the word \p w, whose
      // multiplication with other KBE's is defined with respect to the
      // rewriting system \p kb.
      //
      // The rewriting system \p kb is not copied, and it is the
      // responsibility of the caller to delete it.
      KBE(KnuthBendix*, internal_string_type const&);

      // Constructor from a rewriting system and a letter.
      //
      // Calls KBE::KBE with KnuthBendix::uint_to_kb_word of \p a.
      KBE(KnuthBendix*, letter_type const&);

      // Constructor from a rewriting system and a letter.
      //
      // Calls KBE::KBE with KnuthBendix::uint_to_kb_word of \p a.
      KBE(KnuthBendix&, letter_type const&);

      // Constructor from a rewriting system and a word.
      //
      // Calls KBE::KBE with KnuthBendix::word_to_kb_word of \p w.
      KBE(KnuthBendix*, word_type const&);

      // Constructor from a rewriting system and a word.
      //
      // Calls KBE::KBE with KnuthBendix::word_to_kb_word of \p w.
      KBE(KnuthBendix&, word_type const&);

      // Returns \c true if \c this equals \p that.
      //
      // This member function checks the mathematical equality of two KBE, in
      // other words whether or not they represent that the same reduced word of
      // the rewriting system they are defined over.
      bool operator==(Element const&) const override;

      // Returns \c true if \c this is less than that and \c false if it is
      // not.
      //
      // This defines a total order on KBEs that is the short-lex order on all
      // words.
      bool operator<(Element const&) const override;

      // Copy \p x into \c this.
      //
      // This member function copies the KBE pointed to by \p x into \c this by
      // changing \c this in-place.
      void swap(Element&) override;

      // Returns the approximate time complexity of multiplying two
      // KBE's.
      //
      // \sa Element::complexity.
      //
      // Returns KBE::LIMIT_MAX since the complexity of multiplying words
      // in a rewriting system is higher than the cost of tracing a path in the
      // left or right Cayley graph of a FroidurePin.
      size_t complexity() const override;

      // Returns the degree of an KBE.
      //
      // \sa Element::degree.
      //
      // Returns the integer 0 since the notion of degree is not really
      // meaningful in this context.
      size_t degree() const override;

      // Return the identity KBE.
      //
      // \sa Element::identity.
      //
      // Returns a new KBE wrapping the empty word and over the same rewriting
      // system as \c this.
      KBE identity() const;

      // Returns an independent copy of \c this.
      //
      // Returns a pointer to a copy of \c this, which is not linked to \c this
      // in memory.
      KBE* heap_copy() const override;

      // Returns a new copy of the identity KBE.
      //
      // Returns a pointer to a copy of \c this->identity(), which is not
      // linked to any other copy in memory.
      KBE* heap_identity() const override;

      // Calculates a hash value for this object which is cached.
      //
      // \sa Element::hash_value and Element::cache_hash_value.
      void cache_hash_value() const override;

      // Multiply \p x and \p y and stores the result in \c this.
      //
      // Redefine \c this to be a reduced word with respect to the rewriting
      // system of \p x and \p y which is equivalent to the concatenation of
      // \p x and \p y. This member function asserts that \p x and \p y have the
      // same rewriting system.
      //
      // The parameter \p thread_id is required since some temporary storage is
      // required to find the product of \p x and \p y.  Note that if different
      // threads call this member function with the same value of \p thread_id
      // then bad things will happen.
      void redefine(Element const&, Element const&, size_t = 0) override;

      // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
      //       Element* empty_key() const override;
      // #endif

      operator word_type() const;
      operator std::string() const;

     private:
      KnuthBendix*         _kb;
      internal_string_type _kb_word;
    };
  }  // namespace detail

  template <>
  struct One<detail::KBE*> {
    detail::KBE* operator()(detail::KBE const*);
  };

  // Specialization of the adapter libsemigroups::complexity for detail::KBE.
  template <>
  struct Complexity<detail::KBE> {
    // Returns LIMIT_MAX,
    constexpr size_t operator()(detail::KBE const&) const noexcept {
      return LIMIT_MAX;
    }
  };
}  // namespace libsemigroups

namespace std {
  // Provides a call operator returning a hash value for an Element
  // via a pointer.
  //
  // This struct provides a call operator for obtaining a hash value for the
  // Element from a const Element pointer. This is used by various member
  // functions of the FroidurePin class.
  template <>
  struct hash<libsemigroups::detail::KBE> {
    // Hashes a detail::KBE given by const detail::KBE reference.
    size_t operator()(libsemigroups::detail::KBE const& x) const {
      return x.hash_value();
    }
  };

  // Provides a call operator for comparing KBEs via references.
  //
  // This struct provides a call operator for comparing const detail::KBE
  // references (by comparing the detail::KBE objects they point to). This is
  // used by various member functions of the FroidurePin class.
  template <>
  struct equal_to<libsemigroups::detail::KBE> {
    // Tests equality of two const detail::KBE references via equality of the
    // KBEs.
    bool operator()(libsemigroups::detail::KBE const& x,
                    libsemigroups::detail::KBE const& y) const {
      return x == y;
    }
  };
}  // namespace std

namespace libsemigroups {
  // Returns a libsemigroups::word_type which evaluates to \p x.
  //
  // Specialises the factorisation member function for FroidurePin's of KBE's so
  // that it just returns the word inside the KBE.
  template <>
  word_type
  FroidurePin<detail::KBE, FroidurePinTraits<detail::KBE>>::factorisation(
      detail::KBE const& x);

  template <>
  tril FroidurePin<detail::KBE, FroidurePinTraits<detail::KBE>>::is_finite();
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_KBE_HPP_
