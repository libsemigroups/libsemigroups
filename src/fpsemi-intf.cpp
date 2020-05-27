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

// This file contains a base class for f.p. semigroup like classes.

#include "libsemigroups/fpsemi-intf.hpp"

#include <algorithm>  // for sort
#include <string>     // for std::string

#include "libsemigroups/froidure-pin-base.hpp"     // for FroidurePinBase
#include "libsemigroups/libsemigroups-config.hpp"  // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/report.hpp"                   // for REPORT_DEFAULT ...
#include "libsemigroups/string.hpp"                   // for detail::to_string

namespace libsemigroups {

  char to_human_readable(char c) noexcept {
    if (c > 95) {
      return c;
    } else {
#ifndef LIBSEMIGROUPS_DEBUG
      return c + 96;
#else
      return c;
#endif
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - constructor + destructor - public
  //////////////////////////////////////////////////////////////////////////////

  FpSemigroupInterface::FpSemigroupInterface()
      : Runner(),
        // Non-mutable
        _alphabet(),
        _alphabet_map(),
        _identity_defined(false),
        _identity(),
        _inverses(),
        _rules(),
        // Mutable
        _froidure_pin(nullptr),
        _is_obviously_finite(false),
        _is_obviously_infinite(false) {}

  ////////////////////////////////////////////////////////////////////////////
  // Runner - pure virtual overridden function - public
  ////////////////////////////////////////////////////////////////////////////

  void FpSemigroupInterface::before_run() {
    if (alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("no alphabet specified!");
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - non-pure virtual member functions - public
  //////////////////////////////////////////////////////////////////////////////

  bool FpSemigroupInterface::equal_to(word_type const& u, word_type const& v) {
    validate_word(u);
    validate_word(v);
    return equal_to(word_to_string(u), word_to_string(v));
  }

  word_type FpSemigroupInterface::normal_form(word_type const& w) {
    validate_word(w);
    return string_to_word(normal_form(word_to_string(w)));
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - non-virtual member functions - public
  //////////////////////////////////////////////////////////////////////////////

  void FpSemigroupInterface::set_alphabet(std::string const& lphbt) {
    if (!_alphabet.empty()) {
      LIBSEMIGROUPS_EXCEPTION("the alphabet cannot be set more than once");
    } else if (lphbt.empty()) {
      LIBSEMIGROUPS_EXCEPTION("the alphabet must be non-empty");
    }
    for (size_t i = 0; i < lphbt.size(); ++i) {
      if (_alphabet_map.find(lphbt[i]) != _alphabet_map.end()) {
        _alphabet_map.clear();  // Strong exception guarantee
        LIBSEMIGROUPS_EXCEPTION(
            "invalid alphabet, it contains the duplicate letter "
            + detail::to_string(lphbt[i]));
      }
      _alphabet_map.emplace(lphbt[i], i);
    }
    _alphabet = lphbt;
    set_alphabet_impl(lphbt);
    reset();
  }

  void FpSemigroupInterface::set_alphabet(size_t const nr_letters) {
    if (!_alphabet.empty()) {
      LIBSEMIGROUPS_EXCEPTION("the alphabet cannot be set more than once");
    } else if (nr_letters == 0) {
      LIBSEMIGROUPS_EXCEPTION("the alphabet must be non-empty");
    } else if (nr_letters > 256) {
      LIBSEMIGROUPS_EXCEPTION("the alphabet must contain at most 256 letters");
    }
    for (size_t i = 0; i < nr_letters; ++i) {
#ifdef LIBSEMIGROUPS_DEBUG
      _alphabet += static_cast<char>(i + 97);
#else
      _alphabet += static_cast<char>(i + 1);
#endif
      _alphabet_map.emplace(_alphabet[i], i);
    }
    set_alphabet_impl(nr_letters);
    reset();
  }

  void FpSemigroupInterface::add_rules(FroidurePinBase& S) {
    if (!_alphabet.empty() && _alphabet.size() != S.nr_generators()) {
      LIBSEMIGROUPS_EXCEPTION("incompatible number of generators, found "
                              + detail::to_string(S.nr_generators())
                              + ", should be at most "
                              + detail::to_string(_alphabet.size()));
    }
    add_rules_impl(S);
    reset();
  }

  // Note that this can be called repeatedly, and that's fine.
  void FpSemigroupInterface::set_identity(std::string const& id) {
    if (validate_identity_impl(id)) {
      _identity = id[0];
      for (auto l : alphabet()) {
        if (l == id[0]) {
          add_rule(id + id, id);
        } else {
          add_rule(detail::to_string(l) + id, detail::to_string(l));
          add_rule(id + detail::to_string(l), detail::to_string(l));
        }
      }
    }
    _identity_defined = true;
  }

  std::string const& FpSemigroupInterface::identity() const {
    if (_identity_defined) {
      return _identity;
    } else {
      LIBSEMIGROUPS_EXCEPTION("no identity has been defined");
    }
  }

  void FpSemigroupInterface::set_inverses(std::string const& inv) {
    if (!_inverses.empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "inverses already defined, cannot define inverses more than once");
    } else if (_alphabet.empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "no alphabet has been defined, define an alphabet first");
    } else if (!_identity_defined) {
      LIBSEMIGROUPS_EXCEPTION(
          "no identity has been defined, define an identity first");
    } else if (_alphabet.size() != inv.size()) {
      LIBSEMIGROUPS_EXCEPTION("invalid inverses, expected "
                              + detail::to_string(_alphabet.size())
                              + " but found " + detail::to_string(inv.size()));
    }

    validate_word(inv);

    std::string cpy = inv;
    std::sort(cpy.begin(), cpy.end());
    for (auto it = cpy.cbegin(); it < cpy.cend() - 1; ++it) {
      if (*it == *(it + 1)) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid inverses, it contains the duplicate letter "
            + detail::to_string(*it));
      }
    }

    _inverses = inv;

    for (size_t i = 0; i < _alphabet.size(); ++i) {
      add_rule(std::string(1, _alphabet[i]) + _inverses[i], _identity);
      add_rule(std::string(1, _inverses[i]) + _alphabet[i], _identity);
    }
  }

  std::string const& FpSemigroupInterface::inverses() const {
    if (!_inverses.empty()) {
      return _inverses;
    } else {
      LIBSEMIGROUPS_EXCEPTION("no inverses have been defined");
    }
  }

  word_type FpSemigroupInterface::string_to_word(std::string const& s) const {
    word_type w;
    w.reserve(s.size());
    for (char const& c : s) {
      w.push_back(char_to_uint(c));
    }
    return w;
  }

  std::string FpSemigroupInterface::word_to_string(word_type const& w) const {
    validate_word(w);
    std::string s;
    s.reserve(w.size());
    for (letter_type const& l : w) {
      s.push_back(uint_to_char(l));
    }
    return s;
  }

  bool FpSemigroupInterface::is_obviously_infinite() {
    REPORT_DEFAULT("checking if the semigroup is obviously infinite . . .\n");
    // If has_parent_froidure_pin(), then that is either finite (and so this
    // is not obviously infinite), or infinite, which is undecidable in
    // general, so we leave the answer to this question to
    // is_quotient_obviously_infinite_impl in the derived class.
    if (alphabet().empty()) {
      // If nr_generators() is undefined, then there is no quotient yet,
      // and so it is not obviously infinite, or anything!
      REPORT_VERBOSE_DEFAULT("not obviously infinite (no alphabet defined)\n");
      return false;
    } else if (has_froidure_pin() && froidure_pin()->finished()) {
      // If the isomorphic FroidurePin is fully enumerated, it must be
      // finite, and hence this is not (obviously) infinite.
      REPORT_VERBOSE_DEFAULT("not obviously infinite (finite)\n");
      return false;
    } else if (is_obviously_infinite_impl()) {
      // The derived class of FpSemigroupInterface knows the quotient is
      // infinite
      return true;
    }
    return false;
  }

  bool FpSemigroupInterface::is_obviously_finite() {
    if (alphabet().empty()) {
      // If the alphabet is empty, then so is the fp semigrorup and so it is
      // obviously finite.
      REPORT_VERBOSE_DEFAULT("obviously finite (no alphabet defined)\n");
      return true;
    } else if (has_froidure_pin() && froidure_pin()->finished()) {
      // If the isomorphic FroidurePin is fully enumerated, it must be
      // finite, and hence this is (obviously) finite.
      REPORT_VERBOSE_DEFAULT("obviously finite (finite)\n");
      return true;
    } else if (is_obviously_finite_impl()) {
      // The derived class of FpSemigroupInterface knows it is
      // finite
      return true;
    }
    return false;
  }

  std::string FpSemigroupInterface::to_gap_string() {
    auto to_gap_word = [](std::string const& w) -> std::string {
      if (w.empty()) {
        return "One(free)";
      }
      std::string out;
      for (auto it = w.cbegin(); it < w.cend() - 1; ++it) {
        out += to_human_readable(*it);
        out += " * ";
      }
      out += to_human_readable(w.back());
      return out;
    };

    std::string out = "free := FreeMonoid(";
    for (auto x : alphabet()) {
      out += std::string("\"") + to_human_readable(x) + "\"";
      if (x != alphabet().back()) {
        out += ", ";
      }
    }
    out += ");\n";
    out += "AssignGeneratorVariables(free);\n";
    out += "rules := [\n";
    for (auto it = cbegin_rules(); it < cend_rules(); ++it) {
      out += "          [";
      out += to_gap_word(it->first);
      out += ", ";
      out += to_gap_word(it->second);
      if (it != cend_rules() - 1) {
        out += "],\n";
      } else {
        out += "]\n";
      }
    }
    out += "         ];\n";
    out += "S := free / rules;\n";
    return out;
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - non-virtual member functions - protected
  //////////////////////////////////////////////////////////////////////////////

  void FpSemigroupInterface::validate_letter(char c) const {
    if (_alphabet.empty()) {
      LIBSEMIGROUPS_EXCEPTION("no alphabet has been defined");
    } else if (_alphabet_map.find(c) == _alphabet_map.end()) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid letter %c, valid letters are \"%s\"", c, _alphabet);
    }
  }

  void FpSemigroupInterface::validate_letter(letter_type c) const {
    if (_alphabet.empty()) {
      LIBSEMIGROUPS_EXCEPTION("no alphabet has been defined");
    } else if (c >= _alphabet.size()) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid letter %d, the valid range is [0, %d)", c, _alphabet.size());
    }
  }

  // void FpSemigroupInterface::validate_relation(std::string const& l,
  //                                              std::string const& r) const
  //                                              {
  //   validate_word(l);
  //   validate_word(r);
  // }

  // void FpSemigroupInterface::validate_relation(
  //     std::pair<std::string, std::string> const& p) const {
  //   validate_relation(p.first, p.second);
  // }

  // void FpSemigroupInterface::validate_relation(word_type const& l,
  //                                              word_type const& r) const {
  //   validate_word(l);
  //   validate_word(r);
  // }

  // void FpSemigroupInterface::validate_relation(relation_type const& r)
  // const {
  //   validate_relation(r.first, r.second);
  // }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - non-pure virtual member functions - private
  //////////////////////////////////////////////////////////////////////////////

  void FpSemigroupInterface::set_alphabet_impl(std::string const&) {
    // do nothing
  }

  void FpSemigroupInterface::set_alphabet_impl(size_t) {
    // do nothing
  }

  void FpSemigroupInterface::add_rules_impl(FroidurePinBase& S) {
    S.run();
    for (auto it = S.cbegin_rules(); it != S.cend_rules(); ++it) {
      add_rule(*it);
    }
  }

  bool FpSemigroupInterface::is_obviously_finite_impl() {
    return false;
  }

  void FpSemigroupInterface::validate_word_impl(std::string const& w) const {
    if (w.empty()) {
      LIBSEMIGROUPS_EXCEPTION("invalid word, found the empty word but "
                              "words must be non-empty");
    }
  }

  void FpSemigroupInterface::validate_word_impl(word_type const& w) const {
    if (w.empty()) {
      LIBSEMIGROUPS_EXCEPTION("invalid word, found the empty word but "
                              "words must be non-empty");
    }
  }

  bool
  FpSemigroupInterface::validate_identity_impl(std::string const& id) const {
    if (id.length() != 1) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid identity, expected 1 letter, found %d letters", id.length())
    }
    validate_letter(id[0]);
    return true;
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - non-virtual member functions - private
  //////////////////////////////////////////////////////////////////////////////

  void FpSemigroupInterface::reset() noexcept {
    // set_finished(false);
    _froidure_pin          = nullptr;
    _is_obviously_finite   = false;
    _is_obviously_infinite = false;
  }

  void FpSemigroupInterface::add_rule_private(std::string&& u,
                                              std::string&& v) {
    if (started()) {
      LIBSEMIGROUPS_EXCEPTION("cannot add further rules at this stage");
      // Note that there is actually nothing that prevents us from adding
      // rules to KnuthBendix (i.e. it is setup so that it can be run
      // (partially or fully) and then more rules can be added and everything
      // is valid. We add this restriction to simplify things in the first
      // instance.
    }
    validate_word(u);
    validate_word(v);
    if (u == v) {
      return;
    }
    _rules.emplace_back(u, v);
    add_rule_impl(_rules.back().first, _rules.back().second);
    reset();
  }
}  // namespace libsemigroups
