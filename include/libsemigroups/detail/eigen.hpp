//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 James D. Mitchell
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

#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_EIGEN_ENABLED

#ifndef LIBSEMIGROUPS_DETAIL_EIGEN_HPP_
#define LIBSEMIGROUPS_DETAIL_EIGEN_HPP_
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wswitch-default"
#endif
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Winline"
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include <Eigen/Core>
#include <Eigen/QR>
#include <unsupported/Eigen/MatrixFunctions>
#undef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#pragma GCC diagnostic pop
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif
#endif  // LIBSEMIGROUPS_DETAIL_EIGEN_HPP_
