//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_SRC_RWSE_H_
#define LIBSEMIGROUPS_SRC_RWSE_H_

#include <algorithm>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "elements.h"
#include "rws.h"

namespace libsemigroups {

  //! Subclass of Element that wraps an libsemigroups::rws_word_t.
  //!
  //! This class is used to wrap libsemigroups::rws_word_t into an Element so
  //! that it is possible to use them as generators for a Semigroup object.
  class RWSE : public Element {
    using rws_word_t = RWS::rws_word_t;

   private:
    RWSE(RWS* rws, rws_word_t* w, bool reduce)
        : Element(), _rws(rws), _rws_word(w) {
      if (reduce) {
        _rws->rewrite(_rws_word);
      }
    }

   public:
    //! Constructor from a rewriting system and a word.
    //!
    //! Constructs a RWSE which is essentially the word \p w, whose
    //! multiplication with other RWSE's is defined with respect to the
    //! rewriting system \p rws.
    //!
    //! The rws_word_t w is not copied, and should be deleted using
    //! ElementWithVectorData::really_delete.
    //!
    //! The rewriting system \p rws is not copied either, and it is the
    //! responsibility of the caller to delete it.
    RWSE(RWS* rws, rws_word_t* w) : RWSE(rws, w, true) {
      LIBSEMIGROUPS_ASSERT(w != nullptr);
    }

    //! Constructor from a rewriting system and a word.
    //!
    //! Constructs a RWSE which is essentially the word \p w, whose
    //! multiplication with other RWSE's is defined with respect to the
    //! rewriting system \p rws.
    //!
    //! The rws_word_t \p w is copied, but the rewriting system \p rws is
    //! not.
    RWSE(RWS& rws, rws_word_t const& w) : RWSE(&rws, new rws_word_t(w)) {}

    //! Constructor from a rewriting system and a letter.
    //!
    //! Calls RWSE::RWSE with RWS::uint_to_rws_word of \p a.
    RWSE(RWS& rws, letter_t const& a) : RWSE(&rws, RWS::uint_to_rws_word(a)) {}

    //! Constructor from a rewriting system and a word.
    //!
    //! Calls RWSE::RWSE with RWS::word_to_rws_word of \p w.
    RWSE(RWS& rws, word_t const& w) : RWSE(&rws, RWS::word_to_rws_word(w)) {}

    //! Returns \c true if \c this equals \p that.
    //!
    //! This method checks the mathematical equality of two RWSE, in other
    //! words whether or not they represent that the same reduced word of the
    //! rewriting system they are defined over.
    bool operator==(Element const& that) const override {
      LIBSEMIGROUPS_ASSERT(_rws_word != nullptr);
      LIBSEMIGROUPS_ASSERT(static_cast<RWSE const&>(that)._rws_word != nullptr);
      return *(static_cast<RWSE const&>(that)._rws_word) == *(this->_rws_word);
    }

    //! Returns \c true if \c this is less than that and \c false if it is
    //! not.
    //!
    //! This defines a total order on RWSEs that is the short-lex order on all
    //! words.
    // TODO should use the reduction ordering of RWS.
    bool operator<(const Element& that) const override;

    //! Returns a pointer to a copy of \c this.
    //!
    //! The parameter \p increase_deg_by is not used
    //!
    //! \sa Element::really_copy.
    Element* really_copy(size_t increase_deg_by) const override;

    //! Copy \p x into \c this.
    //!
    //! This method copies the RWSE pointed to by \p x into \c this by
    //! changing \c this in-place.
    void copy(Element const* x) override;
    void swap(Element* x) override;

    //! Deletes the underlying rws_word_t that this object wraps.
    //!
    //! \sa Element::really_delete.
    void really_delete() override {
      delete _rws_word;
      _rws_word = nullptr;
    }

    //! Returns the approximate time complexity of multiplying two
    //! RWSE's.
    //!
    //! \sa Element::complexity.
    //!
    //! Returns Semigroup::LIMIT_MAX since the complexity of multiplying words
    //! in a rewriting system is higher than the cost of tracing a path in the
    //! left or right Cayley graph of a Semigroup.
    size_t complexity() const override {
      return Semigroup<>::LIMIT_MAX;
    }

    //! Returns the degree of an RWSE.
    //!
    //! \sa Element::degree.
    //!
    //! Returns the integer 0 since the notion of degree is not really
    //! meaningful in this context.
    size_t degree() const override {
      return 0;
    }

    //! Return the identity RWSE.
    //!
    //! \sa Element::identity.
    //!
    //! Returns a new RWSE wrapping the empty word and over the same rewriting
    //! system as \c this.
    Element* identity() const override {
      return new RWSE(_rws, new rws_word_t());
    }

    //! Calculates a hash value for this object which is cached.
    //!
    //! \sa Element::hash_value and Element::cache_hash_value.
    void cache_hash_value() const override {
      LIBSEMIGROUPS_ASSERT(_rws_word != nullptr);
      this->_hash_value = std::hash<rws_word_t>()(*_rws_word);
    }

    //! Multiply \p x and \p y and stores the result in \c this.
    //!
    //! Redefine \c this to be a reduced word with respect to the rewriting
    //! system of \p x and \p y which is equivalent to the concatenation of
    //! \p x and \p y. This method asserts that \p x and \p y have the same
    //! rewriting system.
    //!
    //! The parameter \p thread_id is required since some temporary storage is
    //! required to find the product of \p x and \p y.  Note that if different
    //! threads call this method with the same value of \p thread_id then bad
    //! things will happen.
    void redefine(Element const* x, Element const* y) override;

    //! Returns a pointer to the rws_word_t used to create \c this.
    rws_word_t const* get_rws_word() const {
      return _rws_word;
    }

   private:
    // TODO const!
    RWS*        _rws;
    rws_word_t* _rws_word;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SRC_RWSE_H_
