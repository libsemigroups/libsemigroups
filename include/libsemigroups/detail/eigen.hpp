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

// This file includes relevant headers from eigen and suppresses the compiler
// warnings emitted from it.

#ifndef LIBSEMIGROUPS_DETAIL_EIGEN_HPP_
#define LIBSEMIGROUPS_DETAIL_EIGEN_HPP_
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wshadow"
#endif
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Winline"
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include <Eigen/Core>
#include <unsupported/Eigen/MatrixFunctions>
#undef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#pragma GCC diagnostic pop
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif
#endif  // LIBSEMIGROUPS_DETAIL_EIGEN_HPP_
