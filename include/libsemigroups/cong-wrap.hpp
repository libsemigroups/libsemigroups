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

// This file contains wrappers for converting an CongruenceInterface ->
// FpSemigroupInterface, the resulting FpSemigroupInterface delegates everything
// to CongruenceInterface which it wraps.

#ifndef LIBSEMIGROUPS_CONG_WRAP_HPP_
#define LIBSEMIGROUPS_CONG_WRAP_HPP_

#include <cstddef>  // for size_t
#include <memory>   // for unique_ptr, shared_ptr
#include <string>   // for string

#include "cong-intf.hpp"          // for congruence_type, con...
#include "fpsemi-intf.hpp"        // for FpSemigroupInterface
#include "froidure-pin-base.hpp"  // for FroidurePinBase
#include "stl.hpp"                // for detail::make_unique

namespace libsemigroups {
  //! Defined in ``cong-wrap.hpp``.
  //!
  //! This class wraps a derived class of CongruenceInterface of type
  //! \p T so that it has the FpSemigroupInterface.
  template <typename T>
  class CongruenceWrapper final : public FpSemigroupInterface {
    static_assert(
        std::is_base_of<CongruenceInterface, T>::value,
        "the template parameter T must be derived from CongruenceInterface");

   public:
    //! The type of the wrapped congruence, also the template parameter \c T.
    using wrapped_type = T;

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceWrapper - constructors - public
    ////////////////////////////////////////////////////////////////////////////

    //! Default constructor.
    //!
    //! \exceptions
    //! Throws if the constructor of the \c wrapped_type that \c this wraps
    //! throws.
    //!
    //! \par Parameters
    //! (None)
    CongruenceWrapper()
        : _wrapped_cong(
            detail::make_unique<wrapped_type>(congruence_type::twosided)) {}

    //! Constructor from a shared_ptr to a FroidurePinBase.
    //!
    //! \param fp the FroidurePinBase isomorphic to the  finitely presented
    //! semigroup represented by \c this.
    //!
    //! \exceptions
    //! Throws if the constructor of the \c wrapped_type that \c this wraps
    //! throws.
    //!
    //! \note
    //! The FroidurePinBase pointed to by the parameter \p fp is not copied by
    //! this constructor.
    explicit CongruenceWrapper(std::shared_ptr<FroidurePinBase> fp)
        : _wrapped_cong(
            detail::make_unique<wrapped_type>(congruence_type::twosided, fp)) {
      set_alphabet(fp->nr_generators());
      // TODO(later): Adding the rules from fp in the constructor is not
      // always desirable, since it might trigger an infinite computation in
      // fp.  This seems to work right in all current tests though, and since
      // there is no place currently to store fp, I decided to just add_rules
      // here for the sake of simplicity. Note that if add_rules(*fp) were in
      // run, or run_impl, then even if it is an infinite computation, it
      // would get killed, whereas it will just run forever in the
      // constructor.
      add_rules(*fp);
    }

    //! Constructor from a const reference to a FroidurePinBase
    //!
    //! \tparam S a derived class of FroidurePinBase
    //!
    //! \param fp the \p S isomorphic to the finitely presented
    //! semigroup represented by \c this.
    //!
    //! \exceptions
    //! Throws if the constructor of the \c wrapped_type that \c this wraps
    //! throws.
    //!
    //! \warning
    //! The parameter \p fp is copied by this constructor, this might be
    //! expensive, use CongruenceWrapper(std::shared_ptr<FroidurePinBase>) to
    //! avoid this copy.
    // WARNING the argument is copied, might be expensive, use a shared_ptr
    // to avoid the copy!
    template <typename S>
    explicit CongruenceWrapper(S const& fp)
        : CongruenceWrapper(static_cast<std::shared_ptr<FroidurePinBase>>(
            std::make_shared<S>(fp))) {
      static_assert(std::is_base_of<FroidurePinBase, S>::value,
                    "the template parameter must be a derived class of "
                    "FroidurePinBase");
    }

    //! Default copy constructor.
    CongruenceWrapper(CongruenceWrapper const& other)
        : FpSemigroupInterface(),
          _wrapped_cong(detail::make_unique<T>(*other._wrapped_cong)) {}

    //! Deleted
    CongruenceWrapper(CongruenceWrapper&&) = delete;

    //! Deleted
    CongruenceWrapper& operator=(CongruenceWrapper const&) = delete;

    //! Deleted
    CongruenceWrapper& operator=(CongruenceWrapper&&) = delete;

    ~CongruenceWrapper() = default;

    ////////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////////

    size_t size() override {
      run();  // to ensure the state is correct
      return _wrapped_cong->nr_classes();
    }

    bool equal_to(std::string const& lhs, std::string const& rhs) override {
      validate_word(lhs);
      validate_word(rhs);
      return _wrapped_cong->contains(string_to_word(lhs), string_to_word(rhs));
    }

    // TODO(later) improve the many copies etc in:
    // string -> word_type -> class_index_type -> word_type -> string
    std::string normal_form(std::string const& w) override {
      return word_to_string(_wrapped_cong->class_index_to_word(
          _wrapped_cong->word_to_class_index(string_to_word(w))));
    }

    ////////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////////

    // We override FpSemigroupInterface::equal_to to avoid unnecessary
    // conversion from word_type -> string.
    bool equal_to(word_type const& lhs, word_type const& rhs) override {
      return _wrapped_cong->contains(lhs, rhs);
    }

    // We override FpSemigroupInterface::normal_form to avoid unnecessary
    // conversion from word_type -> string.
    word_type normal_form(word_type const& w) override {
      return _wrapped_cong->class_index_to_word(
          _wrapped_cong->word_to_class_index(w));
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceWrapper - member functions - public
    ////////////////////////////////////////////////////////////////////////////

    //! Get a reference to the underlying congruence.
    //!
    //! \returns
    //! A reference to the underlying congruence of \c this, a value of type
    //! CongruenceWrapper::wrapped_type.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    wrapped_type& congruence() const {
      return *_wrapped_cong.get();
    }

   private:
    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////

    void run_impl() override {
      _wrapped_cong->run_until(
          [this]() -> bool { return dead() || timed_out(); });
    }

    bool finished_impl() const override {
      return _wrapped_cong->finished();
    }

    void add_rule_impl(std::string const& u, std::string const& v) override {
      // This is only ever called if u and v are valid
      _wrapped_cong->add_pair(string_to_word(u), string_to_word(v));
    }

    std::shared_ptr<FroidurePinBase> froidure_pin_impl() override {
      return _wrapped_cong->quotient_froidure_pin();
    }

    bool is_obviously_infinite_impl() override {
      return _wrapped_cong->is_quotient_obviously_infinite();
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////

    void set_alphabet_impl(std::string const& lphbt) override {
      _wrapped_cong->set_nr_generators(lphbt.size());
    }

    void set_alphabet_impl(size_t nr_letters) override {
      _wrapped_cong->set_nr_generators(nr_letters);
    }

    // TODO(later) We override FpSemigroupInterface::add_rule_impl to avoid
    // unnecessary conversion from word_type -> string.
    // void add_rule_impl(word_type const& u, word_type const& v) override {
    //   // This is only ever called if u and v are valid
    //   _wrapped_cong->add_pair(u, v);
    // }

    void add_rules_impl(FroidurePinBase& S) override {
      S.run();
      for (auto it = S.cbegin_rules(); it != S.cend_rules(); ++it) {
        add_rule(*it);
      }
    }

    bool is_obviously_finite_impl() override {
      return _wrapped_cong->is_quotient_obviously_finite();
    }

    //////////////////////////////////////////////////////////////////////////
    // CongruenceWrapper - data - private
    //////////////////////////////////////////////////////////////////////////

    std::unique_ptr<wrapped_type> _wrapped_cong;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_CONG_WRAP_HPP_
