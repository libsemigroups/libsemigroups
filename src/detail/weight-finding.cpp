//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

// This file contains the implementation of a function that takes in 2d array of
// constraints and returns an integer solution, if such a solution exists

#include "libsemigroups/detail/weight-finding.hpp"

#include "minlp.h"

#include <iostream>

#include "libsemigroups/exception.hpp"  // for LIBSEMIGRUOPS_EXCEPTION

namespace libsemigroups::detail {
  // Specify the objective function and its associated Jacobian. This is the
  // function we are attempting to minimise, and arbitrarily define it to be
  // the function with the constant value 1.
  // TODO: experiment with different objectives
  void objective(const alglib::real_1d_array& x,
                 alglib::real_1d_array&       fi,
                 alglib::real_2d_array&       jac,
                 void*                        ptr) {
    // ptr is a necessary parameter as this function's API is prescribed by
    // <minlpsolveroptimize>
    (void) ptr;

    size_t const n = x.length();
    fi[0]          = 0;
    for (size_t i = 0; i < n; ++i) {
      // fi[0] += x[i];
      // jac[0][i] = 1;
      jac[0][i] = 0;
    }
  }

  std::optional<std::vector<size_t>>
  get_weights(DynamicArray2<int> const& coefficients,
              std::vector<bool> const&  is_strict) {
    if (coefficients.number_of_rows() != is_strict.size()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the number of rows of the first argument must be the same as the "
          "size of the second argument, found {} and {} respectively",
          coefficients.number_of_rows(),
          is_strict.size());
    }

    // Create the solver and reporting object
    alglib::minlpsolverstate  solver;
    alglib::minlpsolverstate  solver2;
    alglib::minlpsolverreport rep;
    alglib::real_1d_array     result;

    size_t const num_variables   = coefficients.number_of_cols();
    size_t const num_constraints = coefficients.number_of_rows();

    // Initial guess
    alglib::real_1d_array x0;
    x0.setlength(num_variables);
    for (size_t i = 0; i < num_variables; ++i) {
      x0[i] = 1;
    }
    alglib::minlpsolvercreate(num_variables, x0, solver);

    x0.setlength(1);
    for (size_t i = 0; i < 1; ++i) {
      x0[i] = 1000;
    }
    alglib::minlpsolvercreate(1, x0, solver2);

    // Bounds on the variables
    alglib::real_1d_array variable_lower_bounds;
    alglib::real_1d_array variable_upper_bounds;
    variable_lower_bounds.setlength(num_variables);
    variable_upper_bounds.setlength(num_variables);

    // TODO(1): Experiment with different upper bounds
    for (size_t i = 0; i < num_variables; ++i) {
      variable_lower_bounds[i] = 1;
      variable_upper_bounds[i] = alglib::fp_posinf;
      alglib::minlpsolversetintkth(solver, i);
      alglib::minlpsolvermarkaslinearvar(solver, i);
    }
    alglib::minlpsolversetbc(
        solver, variable_lower_bounds, variable_upper_bounds);

    // TODO(1): This for loop is the only part that really depends on the
    // parameters. Refactor the function to avoid this, and make the solver
    // reusable with different constraints.
    for (size_t i = 0; i < num_constraints; ++i) {
      // Add constraints
      alglib::real_1d_array row;
      row.setlength(num_variables);
      for (size_t j = 0; j < num_variables; ++j) {
        row[j] = coefficients.get(i, j);
      }
      // The third argument below is the lower bound of the constraint. If
      // a_1 * x_1 + ... a_n * x_n > 0,
      // then we should add the constraint
      // a_1 * x_1 + ... a_n * x_n >= 1.
      alglib::minlpsolveraddlc2sparsefromdense(
          solver, row, is_strict[i] ? 1 : 0, alglib::fp_posinf);
    }

    // Tell the solver to use BBSYNC (Branch & Bound with Synchronous
    // processing) mixed-integer nonlinear programming algorithm. The second
    // parameter being set to 1 indicates that the algorithm shouldn't try and
    // parallelise computation. This is recommended when the diminsion is
    // small, and also when using the free version of the software, as we are.
    alglib::minlpsolversetalgobbsync(solver, 1);

    // Our problems our convex, so we don't need multiple starts.
    alglib::minlpsolversetmultistarts(solver, 1);

    // Tell the solver that we expect our problem to have a shallow B&B  tree
    // with  the number of nodes comparable to the integer variables count, or
    // below.
    // TODO(2): Experiment with largetree
    alglib::minlpsolversetbbsyncprofilesmalltree(solver);
    // alglib::minlpsolversetbbsyncprofilelargetree(solver);

    // Tell the solver what to try and minimize
    alglib::minlpsolveroptimize(solver, objective);

    // Run the solver
    alglib::minlpsolverresults(solver, result, rep);
    std::cout << "\nNumber of evaluations: " << rep.nfev << std::endl;

    // The documentation says that a terminationtype of 2 corresponds to a
    // solution being found, but the implementation doesn't agree with this.
    // It may be the case that there is a value greater than 0 that represents
    // failure, but this was the best JDE could do.
    if (rep.terminationtype > 0) {
      std::vector<size_t> weights;
      weights.assign(result.getcontent(), result.getcontent() + num_variables);
      return weights;
    }
    return {};
  }
}  // namespace libsemigroups::detail
