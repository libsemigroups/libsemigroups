//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

// Private streaming parser and buffered writer for KBMAG records.

#ifndef LIBSEMIGROUPS_DETAIL_KBMAG_HPP_
#define LIBSEMIGROUPS_DETAIL_KBMAG_HPP_

#include <cstddef>        // for size_t
#include <cstdint>        // for uint64_t
#include <iosfwd>         // for istream, ostream
#include <string>         // for string
#include <string_view>    // for string_view
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

namespace libsemigroups {
  namespace detail {
    class KbmagReader {
     public:
      explicit KbmagReader(std::istream& input);

      uint64_t number_of_nodes() const {
        return _number_of_nodes;
      }
      uint64_t out_degree() const {
        return _out_degree;
      }
      std::vector<uint64_t> const& read_row();
      void                         finish();
      [[noreturn]] void            fail(std::string const& reason) const;

     private:
      enum class kind { identifier, number, string, punctuation, end };

      int         get();
      int         peek();
      void        next();
      bool        is(std::string_view value) const;
      bool        accept(std::string_view value);
      void        expect(std::string_view value);
      uint64_t    number(std::string_view description,
                         bool             allow_negative = false);
      std::string string();
      std::string field(std::unordered_set<std::string>& seen);
      void        skip_value();
      uint64_t    read_set(std::string const& name);
      void        read_flags();
      void        read_initial();
      void        read_accepting();
      void        read_table_field(std::string const& name);
      void        require_header() const;

      std::istream&                   _input;
      size_t                          _line         = 1;
      size_t                          _column       = 1;
      size_t                          _token_line   = 1;
      size_t                          _token_column = 1;
      kind                            _kind         = kind::end;
      std::string                     _token;
      std::unordered_set<std::string> _fields;
      std::unordered_set<std::string> _table_fields;
      uint64_t                        _number_of_nodes      = 0;
      uint64_t                        _out_degree           = 0;
      uint64_t                        _source               = 0;
      uint64_t                        _default_target       = 0;
      uint64_t                        _expected_transitions = 0;
      uint64_t                        _transitions          = 0;
      bool                            _sparse               = false;
      std::vector<uint64_t>           _row;
      std::vector<bool>               _seen;
    };

    class KbmagWriter {
     public:
      explicit KbmagWriter(std::ostream& output);
      void write(std::string_view text);
      void write(uint64_t value);
      void flush();

     private:
      std::ostream& _output;
      std::string   _buffer;
    };
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_KBMAG_HPP_
