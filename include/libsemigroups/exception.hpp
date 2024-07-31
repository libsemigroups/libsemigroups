//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_EXCEPTION_HPP_
#define LIBSEMIGROUPS_EXCEPTION_HPP_

#include <exception>
#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string

#include "detail/fmt.hpp"
#include "detail/formatters.hpp"

namespace libsemigroups {
  //! \ingroup misc_group
  //!
  //! \brief Exception class deriving from std::runtime_error.
  //!
  //! Defined in ``exception.hpp``.
  //!
  //! This page contains the documentation for the class LibsemigroupsException.
  //! All exceptions thrown by ``libsemigroups`` are LibsemigroupsException
  //! instances.
  struct LibsemigroupsException : public std::runtime_error {
    //! \brief Create an exception
    //!
    //! Create an exception with filename, line number, function name, and
    //! message.
    //!
    //! \param fname the name of the file throwing the exception.
    //! \param linenum the line number where the exception is thrown.
    //! \param funcname the name of the function where the exception is thrown.
    //! \param msg the message of the exception being thrown.
    LibsemigroupsException(std::string const& fname,
                           int                linenum,
                           std::string const& funcname,
                           std::string const& msg);

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    LibsemigroupsException(LibsemigroupsException const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    LibsemigroupsException(LibsemigroupsException&&) = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    LibsemigroupsException& operator=(LibsemigroupsException const&) = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    LibsemigroupsException& operator=(LibsemigroupsException&&) = default;

    ~LibsemigroupsException() = default;
  };
}  // namespace libsemigroups

//! \ingroup misc_group
//!
//! \brief Throw a LibsemigroupsException
//!
//! Throw a `LibsemigroupsException` that reports the file name, line number and
//! function that this macro is contained within, and any additional reporting
//! information passed to the macro.
#define LIBSEMIGROUPS_EXCEPTION(...)                             \
  {                                                              \
    throw LibsemigroupsException(                                \
        __FILE__, __LINE__, __func__, fmt::format(__VA_ARGS__)); \
  }

#endif  // LIBSEMIGROUPS_EXCEPTION_HPP_
