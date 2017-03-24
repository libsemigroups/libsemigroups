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

#ifndef LIBSEMIGROUPS_RWSE_H_
#define LIBSEMIGROUPS_RWSE_H_

#include <algorithm>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "elements.h"
#include "rws.h"

namespace libsemigroups {

  // Non-abstract
  //
  // This is a subclass of <Element> and is really just a wrapper for an
  // <rws_word_t>.
  class RWSE : public Element {
   private:
    RWSE(RWS* rws, rws_word_t* w, bool reduce, size_t hv)
        : Element(hv, Element::elm_t::RWSE), _rws(rws), _rws_word(w) {
      if (reduce) {
        rws_word_t buf;
        _rws->rewrite(_rws_word, buf);
      }
    }

   public:
    // 2 parameters (pointers)
    // @rws a rewriting system
    // @w   a pointer to an <rws_word_t>
    //
    // Constructs a RWSE which is essentially the word <w>, whose
    // multiplication with other RWSEs is defined with respect to the rewriting
    // system <rws>.
    //
    // The <rws_word_t> <w> is not copied, and should be deleted using
    // <ElementWithVectorData::really_delete>.
    //
    // The rewriting <rws> is not copied either, but it is the responsibility
    // of the caller to delete this.
    RWSE(RWS* rws, rws_word_t* w) : RWSE(rws, w, true, Element::UNDEFINED) {}

    // 2 parameters (refs)
    // @rws a rewriting system
    // @w   a const reference to a <rws_word_t>
    //
    // Constructs a RWSE which is essentially the word <w>, whose
    // multiplication with other RWSEs is defined with respect to the rewriting
    // system <rws>.
    //
    // The <rws_word_t> <w> is copied, and should be deleted using
    // <ElementWithVectorData::really_delete>.
    RWSE(RWS& rws, rws_word_t const& w) : RWSE(&rws, new rws_word_t(w)) {}

    // 2 parameters (ref, letter)
    // @rws a rewriting system
    // @a   a letter <letter_t>
    //
    // Constructs a RWSE which is essentially the word with single letter <a>,
    // whose multiplication with other RWSEs is defined with respect to the
    // rewriting system <rws>.
    RWSE(RWS& rws, letter_t a)
        : RWSE(&rws, new rws_word_t(RWS::letter_to_rws_word(a))) {}

    // 2 parameters (ref, word)
    // @rws a rewriting system
    // @w   a word <word_t>
    //
    // Constructs a RWSE which is essentially an <rws_word_t> word
    // corresponding to the <word_t> <w>, whose multiplication with other RWSEs
    // is defined with respect to the rewriting system <rws>.
    RWSE(RWS& rws, word_t w)
        : RWSE(&rws, new rws_word_t(RWS::word_to_rws_word(w))) {}

    // const
    // @that RWSE
    //
    // This method checks the mathematical equality of two RWSE, in other
    // words whether or not they represent that the same reduced word of the
    // rewriting system they are defined over.
    //
    // @return **true** if **this** equals <that>.
    bool operator==(Element const& that) const override {
      return *(static_cast<RWSE const&>(that)._rws_word) == *(this->_rws_word);
    }

    // const
    // @that Compare **this** and <that>.
    //
    // This defines a total order on RWSEs that is the short-lex order on all
    // words.
    //
    // @return **true** if **this** is less than <that> and **false** if it is
    // not.
    bool operator<(const Element& that) const override;

    // const
    // @increase_deg_by this is not used.
    //
    // See <Element::really_copy>.
    //
    // @return A pointer to a copy of **this**.
    Element* really_copy(size_t increase_deg_by) const override;

    // non-const
    // @x a pointer to an element.
    //
    // This method copies the element pointed to by <x> into **this** by
    // changing **this** in-place.
    void copy(Element const* x) override;

    // non-const
    //
    // See <Element::really_delete>.
    //
    // Deletes the underlying <rws_word_t> that this object wraps.
    void really_delete() override {
      delete _rws_word;
    }

    // const
    //
    // See <Element::complexity> for more details.
    //
    // @return <Semigroup::LIMIT_MAX>.
    size_t complexity() const override {
      return Semigroup::LIMIT_MAX;
    }

    // const
    //
    // See <Element::degree> for more details.
    //
    // @return the integer 0
    size_t degree() const override {
      return 0;  // TODO(JDM): Ok?
    }

    // const
    //
    // See <Element::identity>.
    //
    // @return a new RWSE wrapping the empty word and over the same rewriting
    // system as **this**.
    Element* identity() const override {
      return new RWSE(_rws, new rws_word_t());
    }

    // const
    //
    // Calculates a hash value for this object which is cached; see
    // <Element::hash_value>.
    void cache_hash_value() const override {
      this->_hash_value = std::hash<rws_word_t>()(*_rws_word);
    }

    // non-const
    // @x RWSE
    // @y RWSE
    // @thread_id the id number of the thread calling the method
    //
    // See <Element::redefine>.
    //
    // Redefine **this** to be a reduced word with respect to the rewriting
    // system of <x> and <y> which is equivalent to the concatenation of <x>
    // and <y>.  This method asserts that <x> and <y> have the same rewriting
    // system.
    //
    // Note that if different threads call this method with the same value of
    // <thread_id> then bad things will happen.
    void redefine(Element const* x,
                  Element const* y,
                  size_t const&  thread_id) override;
    // const
    //
    // @return a pointer to the <rws_word_t> used to create **this**.
    rws_word_t* get_rws_word() const {
      return _rws_word;
    }

   private:
    // TODO const!
    RWS*                           _rws;
    rws_word_t*                    _rws_word;
    static std::vector<rws_word_t> _buf;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_RWSE_H_
