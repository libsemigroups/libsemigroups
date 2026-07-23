/*************************************************************************
ALGLIB 4.08.0 (source code generated 2026-06-08)
Copyright (c) Sergey Bochkanov (ALGLIB project).

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses
>>> END OF LICENSE >>>
*************************************************************************/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "minlp.h"
#include "stdafx.h"

// disable some irrelevant warnings
#if (AE_COMPILER == AE_MSVC) && !defined(AE_ALL_WARNINGS)
#pragma warning(disable : 4100)
#pragma warning(disable : 4127)
#pragma warning(disable : 4611)
#pragma warning(disable : 4702)
#pragma warning(disable : 4996)
#endif

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS IMPLEMENTATION OF C++ INTERFACE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib {

#if defined(AE_COMPILE_BBGD) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MIRBFVNS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINLPSOLVERS) || !defined(AE_PARTIAL_BUILD)
  /*************************************************************************
                  MIXED INTEGER NONLINEAR PROGRAMMING SOLVER

  DESCRIPTION:
  The  subroutine  minimizes a function  F(x)  of N arguments subject to any
  combination of:
  * box constraints
  * linear equality/inequality/range constraints CL<=Ax<=CU
  * nonlinear equality/inequality/range constraints HL<=Hi(x)<=HU
  * integrality constraints on some variables

  REQUIREMENTS:
  * F(), H() are continuously differentiable on the  feasible  set  and  its
    neighborhood
  * starting point X0, which can be infeasible

  INPUT PARAMETERS:
      N       -   problem dimension, N>0:
                  * if given, only leading N elements of X are used
                  * if not given, automatically determined from size ofX
      X       -   starting point, array[N]:
                  * it is better to set X to a feasible point
                  * but X can be infeasible, in which case algorithm will try
                    to find feasible point first, using X as initial
                    approximation.

  OUTPUT PARAMETERS:
      State   -   structure stores algorithm state

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolvercreate(const ae_int_t       n,
                         const real_1d_array& x,
                         minlpsolverstate&    state,
                         const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolvercreate(
        n, x.c_ptr(), state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

/*************************************************************************
                MIXED INTEGER NONLINEAR PROGRAMMING SOLVER

DESCRIPTION:
The  subroutine  minimizes a function  F(x)  of N arguments subject to any
combination of:
* box constraints
* linear equality/inequality/range constraints CL<=Ax<=CU
* nonlinear equality/inequality/range constraints HL<=Hi(x)<=HU
* integrality constraints on some variables

REQUIREMENTS:
* F(), H() are continuously differentiable on the  feasible  set  and  its
  neighborhood
* starting point X0, which can be infeasible

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 01.01.2025 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
  void minlpsolvercreate(const real_1d_array& x,
                         minlpsolverstate&    state,
                         const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    ae_int_t              n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump))
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolvercreate(
        n, x.c_ptr(), state.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }
#endif

  /*************************************************************************
  This function sets box constraints for the mixed integer optimizer.

  Box constraints are inactive by default.

  IMPORTANT: box constraints work in parallel with the integrality ones:
             * a variable marked as integral is considered  having no bounds
               until minlpsolversetbc() is called
             * a  variable  with  lower  and  upper bounds set is considered
               continuous   until    marked    as    integral    with    the
               minlpsolversetintkth() function.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      BndL    -   lower bounds, array[N].
                  If some (all) variables are unbounded, you may  specify  a
                  very small number or -INF, with the  latter  option  being
                  recommended.
      BndU    -   upper bounds, array[N].
                  If some (all) variables are unbounded, you may  specify  a
                  very large number or +INF, with the  latter  option  being
                  recommended.

  NOTE 1:  it is possible to specify  BndL[i]=BndU[i].  In  this  case  I-th
           variable will be "frozen" at X[i]=BndL[i]=BndU[i].

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetbc(minlpsolverstate&    state,
                        const real_1d_array& bndl,
                        const real_1d_array& bndu,
                        const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetbc(
        state.c_ptr(), bndl.c_ptr(), bndu.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function sets two-sided linear constraints AL <= A*x <= AU with dense
  constraint matrix A.

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      A       -   linear constraints, array[K,N]. Each row of  A  represents
                  one  constraint. One-sided  inequality   constraints, two-
                  sided inequality  constraints,  equality  constraints  are
                  supported (see below)
      AL, AU  -   lower and upper bounds, array[K];
                  * AL[i]=AU[i] => equality constraint Ai*x
                  * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                  * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                  * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                  * AL[i]=-INF, AU[i]=+INF => constraint is ignored
      K       -   number of equality/inequality constraints,  K>=0;  if  not
                  given, inferred from sizes of A, AL, AU.

    -- ALGLIB --
       Copyright 15.04.2024 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetlc2dense(minlpsolverstate&    state,
                              const real_2d_array& a,
                              const real_1d_array& al,
                              const real_1d_array& au,
                              const ae_int_t       k,
                              const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetlc2dense(state.c_ptr(),
                                        a.c_ptr(),
                                        al.c_ptr(),
                                        au.c_ptr(),
                                        k,
                                        &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU with dense
constraint matrix A.

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpsolvercreate() call.
    A       -   linear constraints, array[K,N]. Each row of  A  represents
                one  constraint. One-sided  inequality   constraints, two-
                sided inequality  constraints,  equality  constraints  are
                supported (see below)
    AL, AU  -   lower and upper bounds, array[K];
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number of equality/inequality constraints,  K>=0;  if  not
                given, inferred from sizes of A, AL, AU.

  -- ALGLIB --
     Copyright 15.04.2024 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
  void minlpsolversetlc2dense(minlpsolverstate&    state,
                              const real_2d_array& a,
                              const real_1d_array& al,
                              const real_1d_array& au,
                              const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    ae_int_t              k;
    if ((a.rows() != al.length()) || (a.rows() != au.length()))
      _ALGLIB_CPP_EXCEPTION("Error while calling 'minlpsolversetlc2dense': "
                            "looks like one of arguments has wrong size");
    k = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump))
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetlc2dense(state.c_ptr(),
                                        a.c_ptr(),
                                        al.c_ptr(),
                                        au.c_ptr(),
                                        k,
                                        &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }
#endif

  /*************************************************************************
  This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
  a sparse constraining matrix A. Recommended for large-scale problems.

  This  function  overwrites  linear  (non-box)  constraints set by previous
  calls (if such calls were made).

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      A       -   sparse matrix with size [K,N] (exactly!).
                  Each row of A represents one general linear constraint.
                  A can be stored in any sparse storage format.
      AL, AU  -   lower and upper bounds, array[K];
                  * AL[i]=AU[i] => equality constraint Ai*x
                  * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                  * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                  * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                  * AL[i]=-INF, AU[i]=+INF => constraint is ignored
      K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                  is specified, A, AL, AU are ignored.

    -- ALGLIB --
       Copyright 15.04.2024 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetlc2(minlpsolverstate&    state,
                         const sparsematrix&  a,
                         const real_1d_array& al,
                         const real_1d_array& au,
                         const ae_int_t       k,
                         const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetlc2(state.c_ptr(),
                                   a.c_ptr(),
                                   al.c_ptr(),
                                   au.c_ptr(),
                                   k,
                                   &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
  a mixed constraining matrix A including a sparse part (first SparseK rows)
  and a dense part (last DenseK rows). Recommended for large-scale problems.

  This  function  overwrites  linear  (non-box)  constraints set by previous
  calls (if such calls were made).

  This function may be useful if constraint matrix includes large number  of
  both types of rows - dense and sparse. If you have just a few sparse rows,
  you  may  represent  them  in  dense  format  without losing  performance.
  Similarly, if you have just a few dense rows, you may store them in sparse
  format with almost same performance.

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      SparseA -   sparse matrix with size [K,N] (exactly!).
                  Each row of A represents one general linear constraint.
                  A can be stored in any sparse storage format.
      SparseK -   number of sparse constraints, SparseK>=0
      DenseA  -   linear constraints, array[K,N], set of dense constraints.
                  Each row of A represents one general linear constraint.
      DenseK  -   number of dense constraints, DenseK>=0
      AL, AU  -   lower and upper bounds, array[SparseK+DenseK], with former
                  SparseK elements corresponding to sparse constraints,  and
                  latter DenseK elements corresponding to dense constraints;
                  * AL[i]=AU[i] => equality constraint Ai*x
                  * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                  * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                  * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                  * AL[i]=-INF, AU[i]=+INF => constraint is ignored
      K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                  is specified, A, AL, AU are ignored.

    -- ALGLIB --
       Copyright 15.04.2024 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetlc2mixed(minlpsolverstate&    state,
                              const sparsematrix&  sparsea,
                              const ae_int_t       ksparse,
                              const real_2d_array& densea,
                              const ae_int_t       kdense,
                              const real_1d_array& al,
                              const real_1d_array& au,
                              const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetlc2mixed(state.c_ptr(),
                                        sparsea.c_ptr(),
                                        ksparse,
                                        densea.c_ptr(),
                                        kdense,
                                        al.c_ptr(),
                                        au.c_ptr(),
                                        &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function appends a two-sided linear constraint AL <= A*x <= AU to the
  matrix of dense constraints.

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      A       -   linear constraint coefficient, array[N], right side is NOT
                  included.
      AL, AU  -   lower and upper bounds;
                  * AL=AU    => equality constraint Ai*x
                  * AL<AU    => two-sided constraint AL<=A*x<=AU
                  * AL=-INF  => one-sided constraint Ai*x<=AU
                  * AU=+INF  => one-sided constraint AL<=Ai*x
                  * AL=-INF, AU=+INF => constraint is ignored

    -- ALGLIB --
       Copyright 15.04.2024 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolveraddlc2dense(minlpsolverstate&    state,
                              const real_1d_array& a,
                              const double         al,
                              const double         au,
                              const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolveraddlc2dense(
        state.c_ptr(), a.c_ptr(), al, au, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function appends two-sided linear constraint  AL <= A*x <= AU  to the
  list of currently present sparse constraints.

  Constraint is passed in compressed format: as list of non-zero entries  of
  coefficient vector A. Such approach is more efficient than  dense  storage
  for highly sparse constraint vectors.

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      IdxA    -   array[NNZ], indexes of non-zero elements of A:
                  * can be unsorted
                  * can include duplicate indexes (corresponding entries  of
                    ValA[] will be summed)
      ValA    -   array[NNZ], values of non-zero elements of A
      NNZ     -   number of non-zero coefficients in A
      AL, AU  -   lower and upper bounds;
                  * AL=AU    => equality constraint A*x
                  * AL<AU    => two-sided constraint AL<=A*x<=AU
                  * AL=-INF  => one-sided constraint A*x<=AU
                  * AU=+INF  => one-sided constraint AL<=A*x
                  * AL=-INF, AU=+INF => constraint is ignored

    -- ALGLIB --
       Copyright 19.07.2018 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolveraddlc2(minlpsolverstate&       state,
                         const integer_1d_array& idxa,
                         const real_1d_array&    vala,
                         const ae_int_t          nnz,
                         const double            al,
                         const double            au,
                         const xparams           _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolveraddlc2(state.c_ptr(),
                                   idxa.c_ptr(),
                                   vala.c_ptr(),
                                   nnz,
                                   al,
                                   au,
                                   &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function appends two-sided linear constraint  AL <= A*x <= AU  to the
  list of currently present sparse constraints.

  Constraint vector A is  passed  as  a  dense  array  which  is  internally
  sparsified by this function.

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      DA      -   array[N], constraint vector
      AL, AU  -   lower and upper bounds;
                  * AL=AU    => equality constraint A*x
                  * AL<AU    => two-sided constraint AL<=A*x<=AU
                  * AL=-INF  => one-sided constraint A*x<=AU
                  * AU=+INF  => one-sided constraint AL<=A*x
                  * AL=-INF, AU=+INF => constraint is ignored

    -- ALGLIB --
       Copyright 19.07.2018 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolveraddlc2sparsefromdense(minlpsolverstate&    state,
                                        const real_1d_array& da,
                                        const double         al,
                                        const double         au,
                                        const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolveraddlc2sparsefromdense(
        state.c_ptr(), da.c_ptr(), al, au, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function sets two-sided nonlinear constraints for MINLP optimizer.

  In fact, this function sets  only  constraints  COUNT  and  their  BOUNDS.
  Constraints  themselves  (constraint  functions)   are   passed   to   the
  MINLPSolverOptimize() method as callbacks.

  MINLPSolverOptimize() method accepts a user-defined vector function F[] and
  its Jacobian J[], where:
  * first element of F[] and first row of J[] correspond to the target
  * subsequent NNLC components of F[] (and rows of J[]) correspond  to  two-
    sided nonlinear constraints NL<=C(x)<=NU, where
    * NL[i]=NU[i] => I-th row is an equality constraint Ci(x)=NL
    * NL[i]<NU[i] => I-th tow is a  two-sided constraint NL[i]<=Ci(x)<=NU[i]
    * NL[i]=-INF  => I-th row is an one-sided constraint Ci(x)<=NU[i]
    * NU[i]=+INF  => I-th row is an one-sided constraint NL[i]<=Ci(x)
    * NL[i]=-INF, NU[i]=+INF => constraint is ignored

  NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
        your problem has mixed constraints, you  may explicitly specify some
        of them as linear or box ones.
        It helps optimizer to handle them more efficiently.

  INPUT PARAMETERS:
      State   -   structure previously allocated with MINLPSolverCreate call.
      NL      -   array[NNLC], lower bounds, can contain -INF
      NU      -   array[NNLC], lower bounds, can contain +INF
      NNLC    -   constraints count, NNLC>=0

  NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
          possible that the algorithm will evaluate the function  outside of
          the feasible area!

  NOTE 2: algorithm scales variables  according  to the scale  specified by
          MINLPSolverSetScale()  function,  so it can handle problems with badly
          scaled variables (as long as we KNOW their scales).

          However,  there  is  no  way  to  automatically  scale   nonlinear
          constraints. Inappropriate scaling  of nonlinear  constraints  may
          ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
          is NOT the same as solving it with constraint "0.001*G0(x)=0".

          It means that YOU are  the  one who is responsible for the correct
          scaling of the nonlinear constraints Gi(x) and Hi(x). We recommend
          you to scale nonlinear constraints in such a way that the Jacobian
          rows have approximately unit magnitude  (for  problems  with  unit
          scale) or have magnitude approximately equal to 1/S[i] (where S is
          a scale set by MINLPSolverSetScale() function).

    -- ALGLIB --
       Copyright 05.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetnlc2(minlpsolverstate&    state,
                          const real_1d_array& nl,
                          const real_1d_array& nu,
                          const ae_int_t       nnlc,
                          const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetnlc2(
        state.c_ptr(), nl.c_ptr(), nu.c_ptr(), nnlc, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

/*************************************************************************
This function sets two-sided nonlinear constraints for MINLP optimizer.

In fact, this function sets  only  constraints  COUNT  and  their  BOUNDS.
Constraints  themselves  (constraint  functions)   are   passed   to   the
MINLPSolverOptimize() method as callbacks.

MINLPSolverOptimize() method accepts a user-defined vector function F[] and its
Jacobian J[], where:
* first element of F[] and first row of J[] correspond to the target
* subsequent NNLC components of F[] (and rows of J[]) correspond  to  two-
  sided nonlinear constraints NL<=C(x)<=NU, where
  * NL[i]=NU[i] => I-th row is an equality constraint Ci(x)=NL
  * NL[i]<NU[i] => I-th tow is a  two-sided constraint NL[i]<=Ci(x)<=NU[i]
  * NL[i]=-INF  => I-th row is an one-sided constraint Ci(x)<=NU[i]
  * NU[i]=+INF  => I-th row is an one-sided constraint NL[i]<=Ci(x)
  * NL[i]=-INF, NU[i]=+INF => constraint is ignored

NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
      your problem has mixed constraints, you  may explicitly specify some
      of them as linear or box ones.
      It helps optimizer to handle them more efficiently.

INPUT PARAMETERS:
    State   -   structure previously allocated with MINLPSolverCreate call.
    NL      -   array[NNLC], lower bounds, can contain -INF
    NU      -   array[NNLC], lower bounds, can contain +INF
    NNLC    -   constraints count, NNLC>=0

NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
        possible that the algorithm will evaluate the function  outside of
        the feasible area!

NOTE 2: algorithm scales variables  according  to the scale  specified by
        MINLPSolverSetScale()  function,  so it can handle problems with badly
        scaled variables (as long as we KNOW their scales).

        However,  there  is  no  way  to  automatically  scale   nonlinear
        constraints. Inappropriate scaling  of nonlinear  constraints  may
        ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
        is NOT the same as solving it with constraint "0.001*G0(x)=0".

        It means that YOU are  the  one who is responsible for the correct
        scaling of the nonlinear constraints Gi(x) and Hi(x). We recommend
        you to scale nonlinear constraints in such a way that the Jacobian
        rows have approximately unit magnitude  (for  problems  with  unit
        scale) or have magnitude approximately equal to 1/S[i] (where S is
        a scale set by MINLPSolverSetScale() function).

  -- ALGLIB --
     Copyright 05.01.2025 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
  void minlpsolversetnlc2(minlpsolverstate&    state,
                          const real_1d_array& nl,
                          const real_1d_array& nu,
                          const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    ae_int_t              nnlc;
    if ((nl.length() != nu.length()))
      _ALGLIB_CPP_EXCEPTION("Error while calling 'minlpsolversetnlc2': looks "
                            "like one of arguments has wrong size");
    nnlc = nl.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump))
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetnlc2(
        state.c_ptr(), nl.c_ptr(), nu.c_ptr(), nnlc, &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }
#endif

  /*************************************************************************
  This function APPENDS a two-sided nonlinear constraint to the list.

  In fact, this function adds constraint bounds.  A  constraints  itself  (a
  function) is passed to the MINLPSolverOptimize() method as a callback. See
  comments on  MINLPSolverSetNLC2()  for  more  information  about  callback
  structure.

  The function adds a two-sided nonlinear constraint NL<=C(x)<=NU, where
  * NL=NU => I-th row is an equality constraint Ci(x)=NL
  * NL<NU => I-th tow is a  two-sided constraint NL<=Ci(x)<=NU
  * NL=-INF  => I-th row is an one-sided constraint Ci(x)<=NU
  * NU=+INF  => I-th row is an one-sided constraint NL<=Ci(x)
  * NL=-INF, NU=+INF => constraint is ignored

  NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
        your problem has mixed constraints, you  may explicitly specify some
        of them as linear or box ones. It helps the optimizer to handle them
        more efficiently.

  INPUT PARAMETERS:
      State   -   structure previously allocated with MINLPSolverCreate call.
      NL      -   lower bound, can be -INF
      NU      -   upper bound, can be +INF

  NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
          possible that the algorithm will evaluate the function  outside of
          the feasible area!

  NOTE 2: algorithm scales variables  according  to the scale  specified by
          MINLPSolverSetScale()  function,  so it can handle problems with badly
          scaled variables (as long as we KNOW their scales).

          However,  there  is  no  way  to  automatically  scale   nonlinear
          constraints. Inappropriate scaling  of nonlinear  constraints  may
          ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
          is NOT the same as solving it with constraint "0.001*G0(x)=0".

          It means that YOU are  the  one who is responsible for the correct
          scaling of the nonlinear constraints Gi(x) and Hi(x). We recommend
          you to scale nonlinear constraints in such a way that the Jacobian
          rows have approximately unit magnitude  (for  problems  with  unit
          scale) or have magnitude approximately equal to 1/S[i] (where S is
          a scale set by MINLPSolverSetScale() function).

  NOTE 3: use addnlc2masked() in order to specify variable  mask.  Masks are
          essential  for  derivative-free  optimization because they provide
          important information about relevant and irrelevant variables.

    -- ALGLIB --
       Copyright 05.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolveraddnlc2(minlpsolverstate& state,
                          const double      nl,
                          const double      nu,
                          const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolveraddnlc2(state.c_ptr(), nl, nu, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function APPENDS a two-sided nonlinear constraint to the  list,  with
  the  variable   mask  being  specified  as  a  compressed  index  array. A
  variable mask is a set of variables actually appearing in the constraint.

  ----- ABOUT VARIABLE MASKS -----------------------------------------------

  Variable masks provide crucial information  for  derivative-free  solvers,
  greatly accelerating surrogate model construction. This  applies  to  both
  continuous and integral variables, with results for binary variables being
  more pronounced.

  Up to 2x improvement in convergence speed has been observed for sufficiently
  sparse MINLP problems.

  NOTE: In order to unleash the full potential of variable  masking,  it  is
        important to provide masks for objective as well  as  all  nonlinear
        constraints.

        Even partial  information  matters,  i.e.  if you are 100% sure that
        your black-box  function  does  not  depend  on  some variables, but
        unsure about other ones, mark surely irrelevant variables, and  tell
        the solver that other ones may be relevant.

  NOTE: the solver is may behave unpredictably  if  some  relevant  variable
        is not included into the mask. Most likely it will fail to converge,
        although it sometimes possible to converge  to  solution  even  with
        incorrectly specified mask.

  NOTE: minlpsolversetobjectivemask() can be used to set  variable  mask for
        the objective.

  NOTE: Masks  are  ignored  by  branch-and-bound-type  solvers  relying  on
        analytic gradients.

  ----- ABOUT NONLINEAR CONSTRAINTS ----------------------------------------

  In fact, this function adds constraint bounds.  A  constraint   itself  (a
  function) is passed to the MINLPSolverOptimize() method as a callback. See
  comments on  MINLPSolverSetNLC2()  for  more  information  about  callback
  structure.

  The function adds a two-sided nonlinear constraint NL<=C(x)<=NU, where
  * NL=NU => I-th row is an equality constraint Ci(x)=NL
  * NL<NU => I-th tow is a  two-sided constraint NL<=Ci(x)<=NU
  * NL=-INF  => I-th row is an one-sided constraint Ci(x)<=NU
  * NU=+INF  => I-th row is an one-sided constraint NL<=Ci(x)
  * NL=-INF, NU=+INF => constraint is ignored

  NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
        your problem has mixed constraints, you  may explicitly specify some
        of them as linear or box ones. It helps the optimizer to handle them
        more efficiently.

  INPUT PARAMETERS:
      State   -   structure previously allocated with MINLPSolverCreate call.
      NL      -   lower bound, can be -INF
      NU      -   upper bound, can be +INF
      VarIdx  -   array[NMSK], with potentially  unsorted  and  non-distinct
                  indexes (the function will sort and merge duplicates).  If
                  a variable index K appears in the list, it  means that the
                  constraint potentially depends  on  K-th  variable.  If  a
                  variable index K does NOT appear in  the  list,  it  means
                  that the constraint does NOT depend on K-th variable.
                  The array can have more than NMSK elements, in which  case
                  only leading NMSK will be used.
      NMSK    -   NMSK>=0, VarIdx[] size:
                  * NMSK>0 means that the constraint depends on up  to  NMSK
                    variables whose indexes are stored in VarIdx[]
                  * NMSK=0 means that the constraint is a constant function;
                    the solver may fail if it is not actually the case.

  NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
          possible that the algorithm will evaluate the function  outside of
          the feasible area!

  NOTE 2: algorithm scales variables  according  to the scale  specified by
          MINLPSolverSetScale()  function,  so it can handle problems with badly
          scaled variables (as long as we KNOW their scales).

          However,  there  is  no  way  to  automatically  scale   nonlinear
          constraints. Inappropriate scaling  of nonlinear  constraints  may
          ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
          is NOT the same as solving it with constraint "0.001*G0(x)=0".

          It means that YOU are  the  one who is responsible for the correct
          scaling of the nonlinear constraints Gi(x) and Hi(x). We recommend
          you to scale nonlinear constraints in such a way that the Jacobian
          rows have approximately unit magnitude  (for  problems  with  unit
          scale) or have magnitude approximately equal to 1/S[i] (where S is
          a scale set by MINLPSolverSetScale() function).

    -- ALGLIB --
       Copyright 05.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolveraddnlc2masked(minlpsolverstate&       state,
                                const double            nl,
                                const double            nu,
                                const integer_1d_array& varidx,
                                const ae_int_t          nmsk,
                                const xparams           _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolveraddnlc2masked(
        state.c_ptr(), nl, nu, varidx.c_ptr(), nmsk, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function sets stopping condition for the branch-and-bound  family  of
  solvers: a solver must when when the gap between primal and dual bounds is
  less than PDGap.

  The solver computes relative gap, equal to |Fprim-Fdual|/max(|Fprim|,1).

  This parameter is ignored by other types of solvers, e.g. MIVNS.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      PDGap   -   >=0, tolerance. Zero value means that some default value
                  is automatically selected.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetpdgap(minlpsolverstate& state,
                           const double      pdgap,
                           const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetpdgap(state.c_ptr(), pdgap, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function sets BBSYNC profile to "small tree".

  It means that we expect our problem to have a shallow B&B  tree  with  the
  number of nodes comparable to the integer variables count, or below.

  BBSYNC solver will run with simplified settings:
  * pseudocost branching is used

  INPUT PARAMETERS:
      State   -   structure that stores algorithm state

    -- ALGLIB --
       Copyright 01.12.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetbbsyncprofilesmalltree(minlpsolverstate& state,
                                            const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetbbsyncprofilesmalltree(state.c_ptr(),
                                                      &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function sets BBSYNC profile to "large tree".

  It means that we expect our problem to have a large  B&B  tree  with  much
  more than NInt (the integer variables count) nodes. However, we expect  it
  to be solvable within our computational budget (i.e. that we are  able  to
  explore the entire B&B tree).

  BBSYNC solver will run with heuristics that are  costly  to  power-up, but
  greatly improve performance on long distances:
  * reliability branching is used

  BBSYNC will not use  heuristics  that  increase  chance  of  finding  good
  solutions early at the cost of increasing total time to prove optimality.

  INPUT PARAMETERS:
      State   -   structure that stores algorithm state

    -- ALGLIB --
       Copyright 01.12.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetbbsyncprofilelargetree(minlpsolverstate& state,
                                            const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetbbsyncprofilelargetree(state.c_ptr(),
                                                      &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function tells branch-and-bound solvers to use nonlinear interior
  point method for continuous subproblems.

  This solver needs several times more function evaluations  than  SQP,  but
  has an order of magnitude smaller per-iteration linear algebra overhead.

  It is a recommended option for
  * large-scale problems, especially sparse ones
  * problems with many constraints (hence  high  linear  algebra  cost)  but
    relatively cheap objective/constraints evaluations


  INPUT PARAMETERS:
      State   -   structure that stores algorithm state
      MemLen  -   >=0, memory length for quasi-Newton update (similar to
                  LBFGS memory parameter):
                  * 0 means default value which may change in future versions;
                    presently it is 8.
                  * 8 is a good default value for moderately nonlinear tasks
                  * 32 is a good value for problems with more nonlinear
                    objective/constraints
                  * values larger than the variables count N  are  possible;
                    these will be silently truncated to N.

    -- ALGLIB --
       Copyright 01.02.2026 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetbbsyncsubsolveripm(minlpsolverstate& state,
                                        const ae_int_t    memlen,
                                        const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetbbsyncsubsolveripm(
        state.c_ptr(), memlen, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function tells branch-and-bound solvers to use SQP method for
  continuous subproblems.

  This solver needs several times less function evaluations  than  IPM,  but
  has an order of magnitude larger per-iteration  linear  algebra  overhead.
  Nevertheless, the solver is fully sparse-capable.

  It is a recommended option for:
  * problems with  relatively  expensive  objective/constraints  evaluations
    that outweigh additional expense  of  solving  QP  subproblems  at  each
    step

  INPUT PARAMETERS:
      State   -   structure that stores algorithm state

    -- ALGLIB --
       Copyright 01.02.2026 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetbbsyncsubsolversqp(minlpsolverstate& state,
                                        const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetbbsyncsubsolversqp(state.c_ptr(),
                                                  &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function sets tolerance for nonlinear constraints;  points  violating
  constraints by no more than CTol are considered feasible.

  Depending on the specific algorithm  used,  constraint  violation  may  be
  checked against  internally  scaled/normalized  constraints  (some  smooth
  solvers renormalize constraints in such a way that they have roughly  unit
  gradient magnitudes) or against raw constraint values:
  * BBSYNC renormalizes constraints prior to comparing them with CTol
  * MIRBF-VNS checks violation against raw constraint values

  IMPORTANT: one  should  be  careful  when choosing tolerances and stopping
             criteria.

             A solver stops  as  soon  as  stopping  criteria are triggered;
             a feasibility check is  performed  after  that.  If  too  loose
             stopping criteria are  used, the solver  may  fail  to  enforce
             constraints  with  sufficient  accuracy  and  fail to recognize
             solution as a feasible one.

             For example, stopping with EpsX=0.01 and checking CTol=0.000001
             will almost surely result in problems. Ideally, CTol should  be
             1-2 orders of magnitude more relaxed than stopping criteria.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      CTol    -   >0, tolerance.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetctol(minlpsolverstate& state,
                          const double      ctol,
                          const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetctol(state.c_ptr(), ctol, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This  function  tells  MINLP solver  to  use  an  objective-based stopping
  condition for an underlying subsolver, i.e. to stop subsolver if  relative
  change in objective between iterations is less than EpsF.

  Too tight EspF, as always, result in spending too much time in the solver.
  Zero value means that some default non-zero value will be used.

  Exact action of this condition as well as reaction  to  too  relaxed  EpsF
  depend on specific MINLP solver being used

  * BBSYNC. This condition controls SQP subsolver used to solve NLP (relaxed)
    subproblems arising during B&B  tree  search. Good  values are typically
    between 1E-6 and 1E-7.

    Too relaxed values may result in subproblems being  mistakenly  fathomed
    (feasible solutions not identified), too  large  constraint  violations,
    etc.

  * MIVNS. This condition controls RBF-based surrogate model subsolver  used
    to handle continuous variables. It is ignored for integer-only problems.

    The subsolver stops if total objective change in last  several  (between
    5 and 10) steps is less than EpsF. More than one step is used  to  check
    convergence because surrogate  model-based  solvers  usually  need  more
    stringent stopping criteria than SQP.

    Good values are relatively high, between 0.01 and 0.0001,  depending  on
    a  problem.  The  MIVNS  solver  is  designed to gracefully handle large
    values of EpsF - it will stop early, but it won't compromise feasibility
    (it will try to reduce constraint violations below CTol)  and  will  not
    drop promising integral nodes.

  INPUT PARAMETERS:
      State   -   solver structure
      EpsF    -   >0, stopping condition

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetsubsolverepsf(minlpsolverstate& state,
                                   const double      epsf,
                                   const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetsubsolverepsf(
        state.c_ptr(), epsf, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This  function  tells  MINLP solver to use a step-based stopping condition
  for an underlying subsolver, i.e. to stop subsolver  if  typical step size
  becomes less than EpsX.

  Too tight EspX, as always, result in spending too much time in the solver.
  Zero value means that some default non-zero value will be used.

  Exact action of this condition as well as reaction  to  too  relaxed  EpsX
  depend on specific MINLP solver being used

  * BBSYNC. This condition controls SQP subsolver used to solve NLP (relaxed)
    subproblems arising during B&B  tree  search. Good  values are typically
    between 1E-6 and 1E-7.

    Too relaxed values may result in subproblems being  mistakenly  fathomed
    (feasible solutions not identified), too  large  constraint  violations,
    etc.

  * MIVNS. This condition controls RBF-based surrogate model subsolver  used
    to handle continuous variables. It is ignored for integer-only problems.

    The subsolver stops if trust radius  for  a  surrogate  model  optimizer
    becomes less than EpsX.

    Good values are relatively high, between 0.01 and 0.0001,  depending  on
    a  problem.  The  MIVNS  solver  is  designed to gracefully handle large
    values of EpsX - it will stop early, but it won't compromise feasibility
    (it will try to reduce constraint violations below CTol)  and  will  not
    drop promising integral nodes.

  INPUT PARAMETERS:
      State   -   solver structure
      EpsX    -   >=0, stopping condition. Zero value means that some default
                  value will be used.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetsubsolverepsx(minlpsolverstate& state,
                                   const double      epsx,
                                   const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetsubsolverepsx(
        state.c_ptr(), epsx, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function controls adaptive internal parallelism, i.e. algorithm  used
  by  the  solver  to  adaptively  decide  whether parallel acceleration  of
  solver's internal calculations (B&B  code,  SQP,  parallel linear algebra)
  should be actually used or not.

  This  function  tells  the  solver  to  favor  parallelism,  i.e.  utilize
  multithreading (when allowed by the  user)  until  statistics  prove  that
  overhead from starting/stopping worker threads is too large.

  This way solver gets the best performance  on  problems  with  significant
  amount  of  internal  calculations  (large  QP/MIQP  subproblems,  lengthy
  surrogate model optimization sessions) from the very beginning. The  price
  is that problems with small solver overhead that does not justify internal
  parallelism (<1ms per iteration) will suffer slowdown for several  initial
  10-20 milliseconds until the solver proves that parallelism makes no sense

  Use  MINLPSolver.CautiousInternalParallelism()  to  avoid slowing down the
  solver on easy problems.

  NOTE: the internal parallelism is distinct from the callback  parallelism.
        The former is the ability to utilize parallelism to speed-up solvers
        own internal calculations,  while  the  latter  is  the  ability  to
        perform several callback evaluations at once. Aside from performance
        considerations, the internal parallelism is entirely transparent  to
        the user. The callback parallelism requries  the  user  to  write  a
        thread-safe, reentrant callback.

  NOTE: in order to use internal parallelism, adaptive or not, the user must
        activate it by   specifying  alglib::parallel  in  flags  or  global
        threading settings. ALGLIB for C++ must be compiled in the  OS-aware
        mode.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolverfavorinternalparallelism(minlpsolverstate& state,
                                           const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolverfavorinternalparallelism(state.c_ptr(),
                                                     &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function controls adaptive internal parallelism, i.e. algorithm  used
  by  the  solver  to  adaptively  decide  whether parallel acceleration  of
  solver's internal calculations (B&B  code,  SQP,  parallel linear algebra)
  should be actually used or not.

  This function tells the solver  to  do calculations in the single-threaded
  mode until statistics  prove  that  iteration  cost  justified  activating
  multithreading.

  This way solver does not suffer slow-down on problems with small iteration
  overhead (<1ms per iteration), at the cost of spending  initial  10-20  ms
  in the single-threaded  mode  even  on  difficult  problems  that  justify
  parallelism usage.

  Use  MINLPSolver.FavorInternalParallelism() to use parallelism until it is
  proven to be useless.

  NOTE: the internal parallelism is distinct from the callback  parallelism.
        The former is the ability to utilize parallelism to speed-up solvers
        own internal calculations,  while  the  latter  is  the  ability  to
        perform several callback evaluations at once. Aside from performance
        considerations, the internal parallelism is entirely transparent  to
        the user. The callback parallelism requries  the  user  to  write  a
        thread-safe, reentrant callback.

  NOTE: in order to use internal parallelism, adaptive or not, the user must
        activate it by   specifying  alglib::parallel  in  flags  or  global
        threading settings. ALGLIB for C++ must be compiled in the  OS-aware
        mode.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolvercautiousinternalparallelism(minlpsolverstate& state,
                                              const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolvercautiousinternalparallelism(state.c_ptr(),
                                                        &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function controls adaptive internal parallelism, i.e. algorithm  used
  by  the  solver  to  adaptively  decide  whether parallel acceleration  of
  solver's internal calculations (B&B  code,  SQP,  parallel linear algebra)
  should be actually used or not.

  This function tells the solver to do calculations exactly as prescribed by
  the user: in the parallel mode when alglib::parallel flag  is  passed,  in
  the single-threaded mode otherwise. The solver  does  not  analyze  actual
  running times to decide whether parallelism is justified or not.

  NOTE: the internal parallelism is distinct from the callback  parallelism.
        The former is the ability to utilize parallelism to speed-up solvers
        own internal calculations,  while  the  latter  is  the  ability  to
        perform several callback evaluations at once. Aside from performance
        considerations, the internal parallelism is entirely transparent  to
        the user. The callback parallelism requries  the  user  to  write  a
        thread-safe, reentrant callback.

  NOTE: in order to use internal parallelism, adaptive or not, the user must
        activate it by   specifying  alglib::parallel  in  flags  or  global
        threading settings. ALGLIB for C++ must be compiled in the  OS-aware
        mode.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolvernoadaptiveinternalparallelism(minlpsolverstate& state,
                                                const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolvernoadaptiveinternalparallelism(state.c_ptr(),
                                                          &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function marks K-th variable as an integral one.

  Unless box constraints are set for the variable, it is unconstrained (i.e.
  can take positive or  negative  values).  By  default  all  variables  are
  continuous.

  IMPORTANT: box constraints work in parallel with the integrality ones:
             * a variable marked as integral is considered  having no bounds
               until minlpsolversetbc() is called
             * a  variable  with  lower  and  upper bounds set is considered
               continuous   until    marked    as    integral    with    the
               minlpsolversetintkth() function.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      K       -   0<=K<N, variable index

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetintkth(minlpsolverstate& state,
                            const ae_int_t    k,
                            const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetintkth(state.c_ptr(), k, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function marks K-th variable as a linear one.

  A  linear  variable can appear in objective and all constraint types (box,
  linear and nonlinear), but the problem must be linear with respect to this
  variable.

  Knowning that some variables are linear  allows  the  solver  to  do avoid
  modelling nonlinearities associated with  these  variables  (corresponding
  rows/cols of a quasi-Newton Hessian will be zero), and, potentially, to do
  some otherwise unavailable reductions, decreasing linear algebra  overhead
  and improving convergence.

  By default all variables are nonlinear.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      K       -   0<=K<N, variable index

    -- ALGLIB --
       Copyright 01.03.2026 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolvermarkaslinearvar(minlpsolverstate& state,
                                  const ae_int_t    k,
                                  const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolvermarkaslinearvar(
        state.c_ptr(), k, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function sets variable  mask for the objective.  A variable  mask  is
  a set of variables actually appearing in the objective.

  If you want  to  set  variable  mask  for  a  nonlinear  constraint,   use
  addnlc2masked() or addnlc2maskeddense() to add  a constraint together with
  a constraint-specific mask.

  Variable masks provide crucial information  for  derivative-free  solvers,
  greatly accelerating surrogate model construction. This  applies  to  both
  continuous and integral variables, with results for binary variables being
  more pronounced.

  Up to 2x improvement in convergence speed has been observed for sufficiently
  sparse MINLP problems.

  NOTE: In order to unleash the full potential of variable  masking,  it  is
        important to provide masks for objective as well  as  all  nonlinear
        constraints.

        Even partial  information  matters,  i.e.  if you are 100% sure that
        your black-box  function  does  not  depend  on  some variables, but
        unsure about other ones, mark surely irrelevant variables, and  tell
        the solver that other ones may be relevant.

  NOTE: the solver is may behave unpredictably  if  some  relevant  variable
        is not included into the mask. Most likely it will fail to converge,
        although it sometimes possible to converge  to  solution  even  with
        incorrectly specified mask.

  NOTE: Masks  are  ignored  by  branch-and-bound-type  solvers  relying  on
        analytic gradients.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      ObjMask -   array[N],  I-th  element  is  False  if  I-th variable  is
                  irrelevant.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetobjectivemaskdense(minlpsolverstate&       state,
                                        const boolean_1d_array& objmask,
                                        const xparams           _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetobjectivemaskdense(
        state.c_ptr(), objmask.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function sets scaling coefficients for the mixed integer optimizer.

  ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
  size and gradient are scaled before comparison  with  tolerances).  Scales
  are also used by the finite difference variant of the optimizer - the step
  along I-th axis is equal to DiffStep*S[I]. Finally,  variable  scales  are
  used for preconditioning (i.e. to speed up the solver).

  The scale of the I-th variable is a translation invariant measure of:
  a) "how large" the variable is
  b) how large the step should be to make significant changes in the function

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      S       -   array[N], non-zero scaling coefficients
                  S[i] may be negative, sign doesn't matter.

    -- ALGLIB --
       Copyright 06.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetscale(minlpsolverstate&    state,
                           const real_1d_array& s,
                           const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetscale(
        state.c_ptr(), s.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function tell the solver to use BBSYNC (Branch&Bound with Synchronous
  processing) mixed-integer nonlinear programming algorithm.

  The BBSYNC algorithm is an NLP-based branch-and-bound method with integral
  and spatial splits, supporting both convex  and  nonconvex  problems.  The
  algorithm combines parallelism support with deterministic  behavior  (i.e.
  the same branching decisions are performed with every parallel run).

  Non-convex (multiextremal) problems can be solved with  multiple  restarts
  from random points, which are activated by minlpsolversetmultistarts()

  IMPORTANT: contrary to the popular  misconception,  MINLP  is  not  easily
             parallelizable. B&B trees often have  profiles  unsuitable  for
             parallel processing (too short and/or too linear).  Spatial  or
             integral splits add  some limited degree of parallelism (up  to
             2x in the very best case), but in practice it often results  in
             just a 1.5x speed-up at best due to imbalanced  leaf processing
             times.  Furthermore,  determinism  is   always  at   odds  with
             efficiency.

             Achieving good parallel speed-up requires some amount of tuning
             and having a 2x-3x speed-up is  already  a  good  result.  Only
             difficult long-running problems (here  'difficult'  means  that
             the value of rep.ntreenodes is at least several larger than the
             variables count) have good parallelism properties.

             On the other hand, setups using multiple  random  restarts  are
             obviously highly parallelizable.

  IMPORTANT: the commercial edition of ALGLIB can  accelerate  factorization
             phase of this function (this phase takes significant amounts of
             time when solving large problems) by using SIMD intrinsics or a
             performance  backend  library   (Intel   PARDISO   or   another
             platform-specific sparse factorization library).

             Specific speed-up due  to  performance  backend  usage  heavily
             depends  on  the  sparsity  pattern  of  constraints.  For some
             problem types performance backends provide great speed-up.  For
             other ones, ALGLIB's  own  sparse  factorization  code  is  the
             preferred option.

             See the ALGLIB Reference Manual for more information on how  to
             activate parallelism and backend support.


  INPUT PARAMETERS:
      State           -   structure that stores algorithm state

      GroupSize       -   >=1, group size. Up to GroupSize tree nodes can be
                          processed in the parallel manner.

                          Increasing  this   parameter   makes   the  solver
                          less efficient serially (it always tries  to  fill
                          the batch with nodes, even if there  is  a  chance
                          that most of them will be  discarded  later),  but
                          increases its parallel potential.

                          Parallel speed-up comes from two sources:
                          * callback parallelism (several  objective  values
                            are computed concurrently), which is significant
                            for problems with callbacks that take  more than
                            1ms per evaluation
                          * internal parallelism, i.e. ability to do parallel
                            sparse matrix factorization  and  other  solver-
                            related tasks
                          By  default,  the  solver  runs  serially even for
                          GroupSize>1. Both kinds of parallelism have to  be
                          activated by the user, see ALGLIB Reference Manual
                          for more information.

                          Recommended value, depending on callback cost  and
                          matrix factorization overhead, can be:
                          * 1 for 'easy' problems with cheap  callbacks  and
                            small dimensions; also for problems with  nearly
                            linear B&B trees.
                          * 2-3  for   problems   with  sufficiently  costly
                            callbacks (or sufficiently high  linear  algebra
                            overhead) that it makes sense to utilize limited
                            parallelism.
                          * cores count - for difficult problems  with  deep
                            and  wide   B&B trees  and  sufficiently  costly
                            callbacks (or sufficiently high  linear  algebra
                            overhead).

  NOTES: DETERMINISM

  Running with fixed GroupSize generally produces same results independently
  of whether parallelism is used or not. Changing  GroupSize  parameter  may
  change results in the following ways:

  * for problems that are solved to optimality  but have multiple solutions,
    different values of this parameter may  result  in  different  solutions
    being returned (but still with the same objective value)

  * while operating close to exhausting budget (either timeout or iterations
    limit), different GroupSize values may result in different  outcomes:  a
    solution being found, or budget being exhausted

  * finally, on difficult problems that are too hard to solve to  optimality
    but still allow finding primal feasible solutions changing GroupSize may
    result in different primal feasible solutions being returned.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetalgobbsync(minlpsolverstate& state,
                                const ae_int_t    groupsize,
                                const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetalgobbsync(
        state.c_ptr(), groupsize, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function  tell  the  solver  to  use  MIVNS  (Mixed-Integer  Variable
  Neighborhood Search) solver for  derivative-free  mixed-integer  nonlinear
  programming with expensive objective/constraints and non-relaxable integer
  variables.

  The solver is intended for moderately-sized problems, typically with  tens
  of variables.

  The algorithm has the following features:
  * it supports all-integer and mixed-integer problems with box, linear  and
    nonlinear equality and inequality  constraints
  * it makes no assumptions about problem convexity
  * it does not require derivative information. Although  it  still  assumes
    that objective/constraints are smooth wrt continuous variables, no  such
    assumptions are made regarding dependence on integer variables.
  * it efficiently uses limited computational budget and  scales  well  with
    larger budgets
  * it does not evaluate objective/constraints at points violating integrality
  * it also respects linear constraints in all intermediate points

  NOTE: In  particular,  if  your  task  uses integrality+sum-to-one set  of
        constraints to encode multiple choice options (e.g. [1,0,0], [0,1,0]
        or [0,0,1]), you can be sure that the algorithm will not ask for  an
        objective value at a point with fractional values like [0.1,0.5,0.4]
        or at one that is not a correct one-hot encoded value (e.g.  [1,1,0]
        which has two variables set to 1).

  The algorithm is intended for low-to-medium accuracy solution of otherwise
  intractable problems with expensive objective/constraints.

  It can solve any MINLP problem; however, it is optimized for the following
  problem classes:
  * limited variable count
  * expensive objective/constraints
  * nonrelaxable integer variables
  * no derivative information
  * problems where changes in integer variables lead to  structural  changes
    in the entire system. Speaking in other words, on  problems  where  each
    integer variable acts as an on/off or "choice"  switch  that  completely
    rewires the model - turning constraints, variables, or whole sub-systems
    on or off

  INPUT PARAMETERS:
      State           -   structure that stores algorithm state

      Budget          -   optimization  budget (function  evaluations).  The
                          solver will not stop  immediately  after  reaching
                          Budget evaluations, but  will  stop  shortly after
                          that (usually within 2N+1 evaluations). Zero value
                          means no limit.

      MaxNeighborhood -   stopping condition for the solver.  The  algorithm
                          will stop as soon as there are  no  points  better
                          than the current candidate in a neighborhood whose
                          size is equal to or exceeds MaxNeighborhood.  Zero
                          means no stopping condition.

                          Recommended neighborhood size is  proportional  to
                          the difference between integral variables count NI
                          and the number of linear equality  constraints  on
                          integral variables L (such constraints effectively
                          reduce problem dimensionality).

                          The very minimal value for binary problems is NI-L,
                          which means that the solution can not be  improved
                          by flipping one of variables between 0 and 1.  The
                          very minimal value for non-binary integral vars is
                          twice as much (because  now  each  point  has  two
                          neighbors per  variable).  However,  such  minimal
                          values often result in an early termination.

                          It is recommended to set this parameter to 5*N  or
                          10*N (ignoring LI) and to test how it  behaves  on
                          your problem.

      BatchSize           >=1,   recommended  batch  size  for  neighborhood
                          exploration.   Up   to  BatchSize  nodes  will  be
                          evaluated at any  moment,  thus  up  to  BatchSize
                          objective evaluations can be performed in parallel.

                          Increasing  this   parameter   makes   the  solver
                          slightly less efficient serially (it always  tries
                          to fill the batch with nodes, even if there  is  a
                          chance that most of them will be discarded later),
                          but greatly increases its parallel potential.

                          Recommended values depend on the cores  count  and
                          on the limitations  of  the  objective/constraints
                          callback:
                          * 1 for serial execution, callback that can not be
                            called  from   multiple   threads,   or   highly
                            parallelized  expensive  callback that keeps all
                            cores occupied
                          * small fixed value like 5  or  10,  if  you  need
                            reproducible behavior independent from the cores
                            count
                          * CORESCOUNT, 2*CORESCOUNT or some other  multiple
                            of CORESCOUNT, if you want to utilize parallelism
                            to the maximum extent

                          Parallel speed-up comes from two sources:
                          * callback parallelism (several  objective  values
                            are computed concurrently), which is significant
                            for problems with callbacks that take  more than
                            1ms per evaluation
                          * internal parallelism, i.e. ability to do parallel
                            sparse matrix factorization  and  other  solver-
                            related tasks
                          By  default,  the  solver  runs  serially even for
                          GroupSize>1. Both kinds of parallelism have to  be
                          activated by the user, see ALGLIB Reference Manual
                          for more information.

  NOTES: if no stopping criteria is specified (unlimited budget, no timeout,
         no  neighborhood  size  limit),  then  the  solver  will  run until
         enumerating all integer solutions.

  ===== ALGORITHM DESCRIPTION ==============================================

  A simplified description for an  all-integer  algorithm, omitting stopping
  criteria and various checks:

      MIVNS (ALL-INTEGER):
          1. Input: initial integral point, may be infeasible wrt  nonlinear
             constraints, but is feasible wrt linear  ones.  Enforce  linear
             feasibility, if needed.
          2. Generate initial neighborhood around the current point that  is
             equal to the point itself. The point is marked as explored.
          3. Scan  neighborhood  for  a  better  point  (one  that  is  less
             infeasible or has lower objective);  if  one  is found, make it
             current and goto #2
          4. Scan neighborhood for an unexplored point (one with no objective
             computed). If one if found, compute objective, mark the point as
             explored, goto #3
          5. If there are no unexplored or better points in the neighborhood,
             expand it: find a  point  that  was  not  used  for  expansion,
             compute up to 2N its nearest integral neighbors,  add  them  to
             the neighborhood and mark as unexplored. Goto #3.

      NOTE: A nearest integral neighbor is a nearest point that  differs  at
            least by +1 or -1 in one  of  integral  variables  and  that  is
            feasible with respect to box and  linear  constraints  (ignoring
            nonlinear ones). For problems  with  difficult  constraint  sets
            integral neighbors are found by solving MIQP subproblems.

  The algorithm above systematically scans neighborhood  of  a  point  until
  either better point is found, an entire integer grid is enumerated, or one
  of stopping conditions is met.

  A mixed-integer version of the algorithm is more complex:
  * it still sees optimization space as a set of integer  nodes,  each  node
    having a subspace of continuous variables associated with it
  * after starting to explore a node, the algorithm runs an  RBF  surrogate-
    based subsolver for the node. It manages a dedicated subsolver for  each
    node in a neighborhood and adaptively divides its  computational  budget
    between subsolvers, switching to a node as soon as its  subsolver  shows
    better results than its competitors.
  * the algorithm remembers all previously evaluated points and reuses  them
    as much as possible

  ===== ALGORITHM SCALING WITH VARIABLES COUNT N ===========================

  A 'neighborhood scan' is a minimum number of function evaluations   needed
  to perform at least minimal evaluation of the immediate  neighborhood. For
  an N-dimensional problem with NI  integer variables and NF continuous ones
  we have ~NI nodes in an immediate neighborhood, and each  node  needs  ~NF
  evalutations to build at least linear model of the objective.

  Thus, a MIVNS neighborhood scan will need  about NI*NF=NI*(N-NI)=NF*(N-NF)
  objective evaluations.

  It is important to note that MIVNS  does  not  share  information  between
  nodes because it assumes that objective landscape can  drastically  change
  when jumping from node to node. That's why we need  NI*NF instead of NI+NF
  objective values.

  In practice, when started not too far away from the minimum, we can expect
  to get some improvement in 5-10 scans, and to get significant progress  in
  50-100 scans.

  For problems with NF being small or NI  being  small  we  have  scan  cost
  being proportional to variables count N, which allows us to  achieve  good
  progress using between 5N and 100N objective values.  However,  when  both
  NI and NF are close to N/2,  a  scan  needs  ~N^2  objective  evaluations,
  which results in a much worse scaling behavior.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetalgomivns(minlpsolverstate& state,
                               const ae_int_t    budget,
                               const ae_int_t    maxneighborhood,
                               const ae_int_t    batchsize,
                               const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetalgomivns(
        state.c_ptr(), budget, maxneighborhood, batchsize, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function activates multiple random restarts (performed for each node,
  including root and child ones) that help to find global solutions to  non-
  convex problems.

  This parameter is used  by  branch-and-bound  solvers  and  is  presently
  ignored by derivative-free solvers.

  INPUT PARAMETERS:
      State           -   structure that stores algorithm state
      NMultistarts    -   >=1, number of random restarts:
                          * 1 means that no restarts performed, the solver
                            assumes convexity
                          * >=1 means that solver assumes non-convexity and
                            performs fixed amount of random restarts

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetmultistarts(minlpsolverstate& state,
                                 const ae_int_t    nmultistarts,
                                 const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversetmultistarts(
        state.c_ptr(), nmultistarts, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function activates timeout feature. The solver finishes after running
  for a specified amount of time (in seconds, fractions can  be  used)  with
  the best point so far.

  Depending on the situation, the following completion codes can be reported
  in rep.terminationtype:
  * -33 (failure), if timed out without finding a feasible point
  * 5 (partial success), if timed out after finding at least one feasible point

  The solver does not stop immediately after timeout was  triggered  because
  it needs some time for underlying subsolvers to react to  timeout  signal.
  Generally, about one additional subsolver iteration (which is usually  far
  less than one B&B split) will be performed prior to stopping.

  INPUT PARAMETERS:
      State           -   structure that stores algorithm state
      Timeout         -   >=0, timeout in seconds (floating point number):
                          * 0 means no timeout
                          * >=0 means stopping after specified number of
                            seconds.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversettimeout(minlpsolverstate& state,
                             const double      timeout,
                             const xparams     _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolversettimeout(
        state.c_ptr(), timeout, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This function provides reverse communication interface
  Reverse communication interface is not documented or recommended to use.
  See below for functions which provide better documented API
  *************************************************************************/
  bool minlpsolveriteration(minlpsolverstate& state, const xparams _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result
        = alglib_impl::minlpsolveriteration(state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
  }

  void minlpsolveroptimize(
      minlpsolverstate& state,
      void (*fvec)(const real_1d_array& x, real_1d_array& fi, void* ptr),
      void (*rep)(const real_1d_array& x, double func, void* ptr),
      void*         ptr,
      const xparams _xparams) {
    alglib_impl::minlpsolverstate& optimizer = *(state.c_ptr());
    alglib_impl::rcommv2_request   request(
        &optimizer.rcommv2, ptr, "minlpsolver");
    alglib_impl::rcommv2_callbacks callbacks;
    alglib_impl::rcommv2_buffers   buffers(&state.c_ptr()->rcommv2.tmpx1,
                                         &state.c_ptr()->rcommv2.tmpc1,
                                         &state.c_ptr()->rcommv2.tmpf1,
                                         &state.c_ptr()->rcommv2.tmpg1,
                                         &state.c_ptr()->rcommv2.tmpj1,
                                         &state.c_ptr()->rcommv2.tmps1);
    jmp_buf                        _break_jump;
    alglib_impl::ae_state          _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::ae_assert(
        fvec != NULL,
        "ALGLIB: error in 'minlpsolveroptimize()' (fvec is NULL)",
        &_alglib_env_state);
    callbacks.rep  = rep;
    callbacks.fvec = fvec;

    alglib_impl::minlpsolversetprotocolv2(state.c_ptr(), &_alglib_env_state);
    alglib_impl::_rcommstate_assign_handler(
        &state.c_ptr()->rcommv2,
        alglib_impl::rcommv2_request_cpphandler,
        &request,
        &callbacks,
        &buffers,
        NULL);
    while (
        alglib_impl::minlpsolveriteration(state.c_ptr(), &_alglib_env_state)) {
      _ALGLIB_CALLBACK_EXCEPTION_GUARD_BEGIN
      alglib_impl::ae_assert(ae_false,
                             "ALGLIB: critical error in 'minlpsolveroptimize' "
                             "(RCommV2 request escaped handler)",
                             &_alglib_env_state);
      _ALGLIB_CALLBACK_EXCEPTION_GUARD_END
    }
    alglib_impl::ae_state_clear(&_alglib_env_state);
  }

  void minlpsolveroptimize(minlpsolverstate& state,
                           void (*jac)(const real_1d_array& x,
                                       real_1d_array&       fi,
                                       real_2d_array&       jac,
                                       void*                ptr),
                           void (*rep)(const real_1d_array& x,
                                       double               func,
                                       void*                ptr),
                           void*         ptr,
                           const xparams _xparams) {
    alglib_impl::minlpsolverstate& optimizer = *(state.c_ptr());
    alglib_impl::rcommv2_request   request(
        &optimizer.rcommv2, ptr, "minlpsolver");
    alglib_impl::rcommv2_callbacks callbacks;
    alglib_impl::rcommv2_buffers   buffers(&state.c_ptr()->rcommv2.tmpx1,
                                         &state.c_ptr()->rcommv2.tmpc1,
                                         &state.c_ptr()->rcommv2.tmpf1,
                                         &state.c_ptr()->rcommv2.tmpg1,
                                         &state.c_ptr()->rcommv2.tmpj1,
                                         &state.c_ptr()->rcommv2.tmps1);
    jmp_buf                        _break_jump;
    alglib_impl::ae_state          _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::ae_assert(
        jac != NULL,
        "ALGLIB: error in 'minlpsolveroptimize()' (jac is NULL)",
        &_alglib_env_state);
    callbacks.rep = rep;
    callbacks.jac = jac;

    alglib_impl::minlpsolversetprotocolv2(state.c_ptr(), &_alglib_env_state);
    alglib_impl::_rcommstate_assign_handler(
        &state.c_ptr()->rcommv2,
        alglib_impl::rcommv2_request_cpphandler,
        &request,
        &callbacks,
        &buffers,
        NULL);
    while (
        alglib_impl::minlpsolveriteration(state.c_ptr(), &_alglib_env_state)) {
      _ALGLIB_CALLBACK_EXCEPTION_GUARD_BEGIN
      alglib_impl::ae_assert(ae_false,
                             "ALGLIB: critical error in 'minlpsolveroptimize' "
                             "(RCommV2 request escaped handler)",
                             &_alglib_env_state);
      _ALGLIB_CALLBACK_EXCEPTION_GUARD_END
    }
    alglib_impl::ae_state_clear(&_alglib_env_state);
  }

  void minlpsolveroptimize(minlpsolverstate& state,
                           void (*sjac)(const real_1d_array& x,
                                        real_1d_array&       fi,
                                        sparsematrix&        s,
                                        void*                ptr),
                           void (*rep)(const real_1d_array& x,
                                       double               func,
                                       void*                ptr),
                           void*         ptr,
                           const xparams _xparams) {
    alglib_impl::minlpsolverstate& optimizer = *(state.c_ptr());
    alglib_impl::rcommv2_request   request(
        &optimizer.rcommv2, ptr, "minlpsolver");
    alglib_impl::rcommv2_callbacks callbacks;
    alglib_impl::rcommv2_buffers   buffers(&state.c_ptr()->rcommv2.tmpx1,
                                         &state.c_ptr()->rcommv2.tmpc1,
                                         &state.c_ptr()->rcommv2.tmpf1,
                                         &state.c_ptr()->rcommv2.tmpg1,
                                         &state.c_ptr()->rcommv2.tmpj1,
                                         &state.c_ptr()->rcommv2.tmps1);
    jmp_buf                        _break_jump;
    alglib_impl::ae_state          _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::ae_assert(
        sjac != NULL,
        "ALGLIB: error in 'minlpsolveroptimize()' (sjac is NULL)",
        &_alglib_env_state);
    callbacks.rep  = rep;
    callbacks.sjac = sjac;

    alglib_impl::minlpsolversetprotocolv2s(state.c_ptr(), &_alglib_env_state);
    alglib_impl::_rcommstate_assign_handler(
        &state.c_ptr()->rcommv2,
        alglib_impl::rcommv2_request_cpphandler,
        &request,
        &callbacks,
        &buffers,
        NULL);
    while (
        alglib_impl::minlpsolveriteration(state.c_ptr(), &_alglib_env_state)) {
      _ALGLIB_CALLBACK_EXCEPTION_GUARD_BEGIN
      alglib_impl::ae_assert(ae_false,
                             "ALGLIB: critical error in 'minlpsolveroptimize' "
                             "(RCommV2 request escaped handler)",
                             &_alglib_env_state);
      _ALGLIB_CALLBACK_EXCEPTION_GUARD_END
    }
    alglib_impl::ae_state_clear(&_alglib_env_state);
  }

  /*************************************************************************
  This subroutine  restarts  algorithm  from  new  point.  All  optimization
  parameters (including constraints) are left unchanged.

  This  function  allows  to  solve multiple  optimization  problems  (which
  must have  same number of dimensions) without object reallocation penalty.

  INPUT PARAMETERS:
      State   -   structure previously allocated with MINLPSolverCreate call.
      X       -   new starting point.

    -- ALGLIB --
       Copyright 28.11.2010 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolverrestartfrom(minlpsolverstate&    state,
                              const real_1d_array& x,
                              const xparams        _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolverrestartfrom(
        state.c_ptr(), x.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  MINLPSolver results:  the  solution  found,  completion  codes  and additional
  information.

  INPUT PARAMETERS:
      Solver  -   solver

  OUTPUT PARAMETERS:
      X       -   array[N], solution
      Rep     -   optimization report, contains information about completion
                  code, constraint violation at the solution and so on.

                  rep.f contains objective value at the solution.

                  You   should   check   rep.terminationtype  in  order   to
                  distinguish successful termination from unsuccessful one.

                  More information about fields of this  structure  can  be
                  found in the comments on the minlpsolverreport datatype.

    -- ALGLIB --
       Copyright 18.01.2024 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolverresults(const minlpsolverstate& state,
                          real_1d_array&          x,
                          minlpsolverreport&      rep,
                          const xparams           _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolverresults(
        state.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  NLC results

  Buffered implementation of MINLPSolverResults() which uses pre-allocated
  buffer to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It
  is intended to be used in the inner cycles of performance critical algorithms
  where array reallocation penalty is too large to be ignored.

    -- ALGLIB --
       Copyright 28.11.2010 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolverresultsbuf(const minlpsolverstate& state,
                             real_1d_array&          x,
                             minlpsolverreport&      rep,
                             const xparams           _xparams) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
      return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if (_xparams.flags != (alglib_impl::ae_uint64_t) 0x0)
      ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::minlpsolverresultsbuf(
        state.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
  }

  /*************************************************************************
  This object stores nonlinear optimizer state.
  You should use functions provided by MinNLC subpackage to work  with  this
  object
  *************************************************************************/
  _minlpsolverstate_owner::_minlpsolverstate_owner() {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _state;

    alglib_impl::ae_state_init(&_state);
    if (setjmp(_break_jump)) {
      if (p_struct != NULL) {
        alglib_impl::_minlpsolverstate_destroy(p_struct);
        alglib_impl::ae_free(p_struct);
      }
      p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
      return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::minlpsolverstate*) alglib_impl::ae_malloc(
        sizeof(alglib_impl::minlpsolverstate), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::minlpsolverstate));
    alglib_impl::_minlpsolverstate_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
    is_attached = false;
  }

  _minlpsolverstate_owner::_minlpsolverstate_owner(
      alglib_impl::minlpsolverstate* attach_to) {
    p_struct    = attach_to;
    is_attached = true;
  }

  _minlpsolverstate_owner::_minlpsolverstate_owner(
      const _minlpsolverstate_owner& rhs) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _state;

    alglib_impl::ae_state_init(&_state);
    if (setjmp(_break_jump)) {
      if (p_struct != NULL) {
        alglib_impl::_minlpsolverstate_destroy(p_struct);
        alglib_impl::ae_free(p_struct);
      }
      p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
      return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct != NULL,
                           "ALGLIB: minlpsolverstate copy constructor failure "
                           "(source is not initialized)",
                           &_state);
    p_struct = (alglib_impl::minlpsolverstate*) alglib_impl::ae_malloc(
        sizeof(alglib_impl::minlpsolverstate), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::minlpsolverstate));
    alglib_impl::_minlpsolverstate_init_copy(
        p_struct,
        const_cast<alglib_impl::minlpsolverstate*>(rhs.p_struct),
        &_state,
        ae_false);
    ae_state_clear(&_state);
    is_attached = false;
  }

  _minlpsolverstate_owner&
  _minlpsolverstate_owner::operator=(const _minlpsolverstate_owner& rhs) {
    if (this == &rhs)
      return *this;
    jmp_buf               _break_jump;
    alglib_impl::ae_state _state;

    alglib_impl::ae_state_init(&_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
      return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct != NULL,
                           "ALGLIB: minlpsolverstate assignment constructor "
                           "failure (destination is not initialized)",
                           &_state);
    alglib_impl::ae_assert(rhs.p_struct != NULL,
                           "ALGLIB: minlpsolverstate assignment constructor "
                           "failure (source is not initialized)",
                           &_state);
    alglib_impl::ae_assert(
        !is_attached,
        "ALGLIB: minlpsolverstate assignment constructor failure (can not "
        "assign to the structure which is attached to something else)",
        &_state);
    alglib_impl::_minlpsolverstate_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::minlpsolverstate));
    alglib_impl::_minlpsolverstate_init_copy(
        p_struct,
        const_cast<alglib_impl::minlpsolverstate*>(rhs.p_struct),
        &_state,
        ae_false);
    ae_state_clear(&_state);
    return *this;
  }

  _minlpsolverstate_owner::~_minlpsolverstate_owner() {
    if (p_struct != NULL && !is_attached) {
      alglib_impl::_minlpsolverstate_destroy(p_struct);
      ae_free(p_struct);
    }
  }

  alglib_impl::minlpsolverstate* _minlpsolverstate_owner::c_ptr() {
    return p_struct;
  }

  const alglib_impl::minlpsolverstate* _minlpsolverstate_owner::c_ptr() const {
    return p_struct;
  }
  minlpsolverstate::minlpsolverstate() : _minlpsolverstate_owner() {}

  minlpsolverstate::minlpsolverstate(alglib_impl::minlpsolverstate* attach_to)
      : _minlpsolverstate_owner(attach_to) {}

  minlpsolverstate::minlpsolverstate(const minlpsolverstate& rhs)
      : _minlpsolverstate_owner(rhs) {}

  minlpsolverstate& minlpsolverstate::operator=(const minlpsolverstate& rhs) {
    if (this == &rhs)
      return *this;
    _minlpsolverstate_owner::operator=(rhs);
    return *this;
  }

  minlpsolverstate::~minlpsolverstate() {}

  /*************************************************************************
  This structure stores the optimization report.

  The following fields are set by all MINLP solvers:
  * f                         objective value at the solution
  * nfev                      number of value/gradient evaluations
  * terminationtype           termination type (see below)

  The BBGD solver additionally sets the following fields:
  * pdgap                     final primal-dual gap
  * ntreenodes                number of B&B tree nodes traversed
  * nsubproblems              total number of NLP relaxations solved; can be
                              larger than ntreenodes because of restarts
  * nnodesbeforefeasibility   number of nodes evaluated before finding first
                              integer feasible solution

  TERMINATION CODES

  TerminationType field contains completion code, which can be either FAILURE
  code or SUCCESS code.

  === FAILURE CODE ===
    -33   timed out, failed to find a feasible point within  time  limit  or
          iteration budget
    -8    internal integrity control detected  infinite  or  NAN  values  in
          function/gradient, recovery was impossible.  Abnormal  termination
          signaled.
    -4    the problem is likely to be unbounded;  for  MINLP  problems it is
          generally impossible to provide an unboundedness  certificate,  so
          only heuristics are possible, based on growth of |x| and  decrease
          of f compared to |f(x0)|.
    -3    integer infeasibility is signaled:
          * for convex problems: proved to be infeasible
          * for nonconvex problems: a primal feasible point  is  nonexistent
            or too difficult to find


  === SUCCESS CODE ===
     2    successful solution:
          * for BBGD - entire tree was scanned
          * for MIVNS - either entire  integer  grid  was  scanned,  or  the
            neighborhood size  based  condition  was  triggered  (in  future
            versions other criteria may be introduced)
     5    a primal feasible point was found, but time or iteration limit was
          exhausted but we  failed  to  find  a  better  one  or  prove  its
          optimality; the best point so far is returned.
  *************************************************************************/
  _minlpsolverreport_owner::_minlpsolverreport_owner() {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _state;

    alglib_impl::ae_state_init(&_state);
    if (setjmp(_break_jump)) {
      if (p_struct != NULL) {
        alglib_impl::_minlpsolverreport_destroy(p_struct);
        alglib_impl::ae_free(p_struct);
      }
      p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
      return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::minlpsolverreport*) alglib_impl::ae_malloc(
        sizeof(alglib_impl::minlpsolverreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::minlpsolverreport));
    alglib_impl::_minlpsolverreport_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
    is_attached = false;
  }

  _minlpsolverreport_owner::_minlpsolverreport_owner(
      alglib_impl::minlpsolverreport* attach_to) {
    p_struct    = attach_to;
    is_attached = true;
  }

  _minlpsolverreport_owner::_minlpsolverreport_owner(
      const _minlpsolverreport_owner& rhs) {
    jmp_buf               _break_jump;
    alglib_impl::ae_state _state;

    alglib_impl::ae_state_init(&_state);
    if (setjmp(_break_jump)) {
      if (p_struct != NULL) {
        alglib_impl::_minlpsolverreport_destroy(p_struct);
        alglib_impl::ae_free(p_struct);
      }
      p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
      return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct != NULL,
                           "ALGLIB: minlpsolverreport copy constructor failure "
                           "(source is not initialized)",
                           &_state);
    p_struct = (alglib_impl::minlpsolverreport*) alglib_impl::ae_malloc(
        sizeof(alglib_impl::minlpsolverreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::minlpsolverreport));
    alglib_impl::_minlpsolverreport_init_copy(
        p_struct,
        const_cast<alglib_impl::minlpsolverreport*>(rhs.p_struct),
        &_state,
        ae_false);
    ae_state_clear(&_state);
    is_attached = false;
  }

  _minlpsolverreport_owner&
  _minlpsolverreport_owner::operator=(const _minlpsolverreport_owner& rhs) {
    if (this == &rhs)
      return *this;
    jmp_buf               _break_jump;
    alglib_impl::ae_state _state;

    alglib_impl::ae_state_init(&_state);
    if (setjmp(_break_jump)) {
#if !defined(AE_NO_EXCEPTIONS)
      _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
      _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
      return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct != NULL,
                           "ALGLIB: minlpsolverreport assignment constructor "
                           "failure (destination is not initialized)",
                           &_state);
    alglib_impl::ae_assert(rhs.p_struct != NULL,
                           "ALGLIB: minlpsolverreport assignment constructor "
                           "failure (source is not initialized)",
                           &_state);
    alglib_impl::ae_assert(
        !is_attached,
        "ALGLIB: minlpsolverreport assignment constructor failure (can not "
        "assign to the structure which is attached to something else)",
        &_state);
    alglib_impl::_minlpsolverreport_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::minlpsolverreport));
    alglib_impl::_minlpsolverreport_init_copy(
        p_struct,
        const_cast<alglib_impl::minlpsolverreport*>(rhs.p_struct),
        &_state,
        ae_false);
    ae_state_clear(&_state);
    return *this;
  }

  _minlpsolverreport_owner::~_minlpsolverreport_owner() {
    if (p_struct != NULL && !is_attached) {
      alglib_impl::_minlpsolverreport_destroy(p_struct);
      ae_free(p_struct);
    }
  }

  alglib_impl::minlpsolverreport* _minlpsolverreport_owner::c_ptr() {
    return p_struct;
  }

  const alglib_impl::minlpsolverreport*
  _minlpsolverreport_owner::c_ptr() const {
    return p_struct;
  }
  minlpsolverreport::minlpsolverreport()
      : _minlpsolverreport_owner(),
        f(p_struct->f),
        nfev(p_struct->nfev),
        nsubproblems(p_struct->nsubproblems),
        ntreenodes(p_struct->ntreenodes),
        nnodesbeforefeasibility(p_struct->nnodesbeforefeasibility),
        terminationtype(p_struct->terminationtype),
        pdgap(p_struct->pdgap) {}

  minlpsolverreport::minlpsolverreport(
      alglib_impl::minlpsolverreport* attach_to)
      : _minlpsolverreport_owner(attach_to),
        f(p_struct->f),
        nfev(p_struct->nfev),
        nsubproblems(p_struct->nsubproblems),
        ntreenodes(p_struct->ntreenodes),
        nnodesbeforefeasibility(p_struct->nnodesbeforefeasibility),
        terminationtype(p_struct->terminationtype),
        pdgap(p_struct->pdgap) {}

  minlpsolverreport::minlpsolverreport(const minlpsolverreport& rhs)
      : _minlpsolverreport_owner(rhs),
        f(p_struct->f),
        nfev(p_struct->nfev),
        nsubproblems(p_struct->nsubproblems),
        ntreenodes(p_struct->ntreenodes),
        nnodesbeforefeasibility(p_struct->nnodesbeforefeasibility),
        terminationtype(p_struct->terminationtype),
        pdgap(p_struct->pdgap) {}

  minlpsolverreport&
  minlpsolverreport::operator=(const minlpsolverreport& rhs) {
    if (this == &rhs)
      return *this;
    _minlpsolverreport_owner::operator=(rhs);
    return *this;
  }

  minlpsolverreport::~minlpsolverreport() {}
#endif
}  // namespace alglib

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS IMPLEMENTATION OF COMPUTATIONAL CORE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl {
#if defined(AE_COMPILE_BBGD) || !defined(AE_PARTIAL_BUILD)
  static double   bbgd_unboundedf            = -1.0E200;
  static ae_int_t bbgd_laconicreportperiod   = 5000;
  static double   bbgd_safetyfactor          = 0.001;
  static double   bbgd_nonlinearitythreshold = 1.0E-14;
  static ae_int_t bbgd_backtracklimit        = 0;
  static double   bbgd_alphaint              = 0.01;
  static ae_int_t bbgd_ftundefined           = -1;
  static ae_int_t bbgd_ftroot                = 0;
  static ae_int_t bbgd_ftdynamic             = 2;
  static ae_int_t bbgd_stundefined           = -1;
  static ae_int_t bbgd_stfrontrunning        = 698;
  static ae_int_t bbgd_stfrontreadytorun     = 699;
  static ae_int_t bbgd_streadytorun          = 700;
  static ae_int_t bbgd_stwaitingforrcomm     = 701;
  static ae_int_t bbgd_stsolved              = 702;
  static ae_int_t bbgd_sttimeout             = 703;
  static ae_int_t bbgd_stwaitingforsync      = 704;
  static ae_int_t bbgd_stunbounded           = 705;
  static ae_int_t bbgd_rqsrcfront            = 1;
  static ae_int_t bbgd_rqsrcxc               = 2;
  static ae_int_t bbgd_divenever             = 0;
  static ae_int_t bbgd_diveuntilprimal       = 1;
  static ae_int_t bbgd_divealways            = 2;
  static ae_int_t bbgd_maxipmits             = 200;
  static ae_int_t bbgd_maxqprfsits           = 5;
  static void     bbgd_clearoutputs(bbgdstate* state, ae_state* _state);
  static void     bbgd_initinternal(ae_int_t                       n,
                                    /* Real    */ const ae_vector* x,
                                    ae_int_t                       solvermode,
                                    double                         diffstep,
                                    bbgdstate*                     state,
                                    ae_state*                      _state);
  static void     bbgd_reduceandappendrequestto(const minnlcstate* subsolver,
                                                bbgdstate*         state,
                                                ae_int_t*          requesttype,
                                                ae_int_t*          querysize,
                                                ae_int_t*          queryfuncs,
                                                ae_int_t*          queryvars,
                                                ae_int_t*          querydim,
                                                ae_int_t*          queryformulasize,
                                                /* Real    */ ae_vector* querydata,
                                                ae_state*                _state);
  static void
              bbgd_extractextendandforwardreplyto(const bbgdstate* state,
                                                  ae_int_t         requesttype,
                                                  ae_int_t         querysize,
                                                  ae_int_t         queryfuncs,
                                                  ae_int_t         queryvars,
                                                  ae_int_t         querydim,
                                                  ae_int_t         queryformulasize,
                                                  /* Real    */ const ae_vector* replyfi,
                                                  /* Real    */ const ae_vector* replydj,
                                                  const sparsematrix*            replysj,
                                                  ae_int_t*                      requestidx,
                                                  minnlcstate*                   subsolver,
                                                  ae_state*                      _state);
  static void bbgd_subproblemcopy(const bbgdsubproblem* src,
                                  ae_int_t              newid,
                                  bbgdsubproblem*       dst,
                                  ae_state*             _state);
  static void bbgd_subproblemcopyasunsolved(const bbgdsubproblem* src,
                                            ae_int_t              newid,
                                            bbgdsubproblem*       dst,
                                            ae_state*             _state);
  static void
                 bbgd_subproblemmergeinsolution(bbgdsubproblem*                subproblem,
                                                /* Real    */ const ae_vector* x,
                                                /* Real    */ const ae_vector* nlrep,
                                                double                         f,
                                                double                         earlyerror,
                                                double                         sclfeaserr,
                                                ae_bool                        isintfeas,
                                                ae_bool                        isearlystopped,
                                                ae_int_t                       terminationtype,
                                                ae_int_t                       its,
                                                ae_state*                      _state);
  static void    bbgd_subproblemrandomizex0(bbgdsubproblem* p,
                                            bbgdstate*      state,
                                            ae_state*       _state);
  static void    bbgd_subproblemappendcopytoarray(const bbgdsubproblem* p,
                                                  bbgdstate*            state,
                                                  ae_bool randomizeinitialpoint,
                                                  ae_obj_array* a,
                                                  ae_state*     _state);
  static ae_bool bbgd_subproblemcanfathom(const bbgdsubproblem* subproblem,
                                          const bbgdstate*      state,
                                          ae_state*             _state);
  static void    bbgd_frontinitundefined(bbgdfront* front,
                                         bbgdstate* state,
                                         ae_state*  _state);
  static void    bbgd_frontstartroot(bbgdfront*            front,
                                     const bbgdsubproblem* r,
                                     bbgdstate*            state,
                                     ae_state*             _state);
  static void    bbgd_frontstartdynamic(bbgdfront* front,
                                        bbgdstate* sstate,
                                        ae_state*  _state);
  static void    bbgd_frontrecomputedualbound(bbgdfront* front,
                                              bbgdstate* state,
                                              ae_state*  _state);
  static ae_bool bbgd_frontrun(bbgdfront* front,
                               bbgdstate* state,
                               ae_state*  _state);
  static ae_bool bbgd_frontruninternal(bbgdfront* front,
                                       bbgdstate* state,
                                       ae_state*  _state);
  static void    bbgd_frontparallelrunentries(bbgdfront* front,
                                              ae_int_t   job0,
                                              ae_int_t   job1,
                                              ae_bool    isrootcall,
                                              bbgdstate* state,
                                              ae_state*  _state);
  ae_bool        _trypexec_bbgd_frontparallelrunentries(bbgdfront* front,
                                                        ae_int_t   job0,
                                                        ae_int_t   job1,
                                                        ae_bool    isrootcall,
                                                        bbgdstate* state,
                                                        ae_state*  _state);
  static void    bbgd_frontrunkthentryjthsubsolver(bbgdfront* front,
                                                   ae_int_t   k,
                                                   ae_int_t   j,
                                                   bbgdstate* state,
                                                   ae_state*  _state);
  static void    bbgd_frontpackqueries(bbgdfront*               front,
                                       bbgdstate*               state,
                                       ae_int_t*                requesttype,
                                       ae_int_t*                querysize,
                                       ae_int_t*                queryfuncs,
                                       ae_int_t*                queryvars,
                                       ae_int_t*                querydim,
                                       ae_int_t*                queryformulasize,
                                       /* Real    */ ae_vector* querydata,
                                       ae_state*                _state);
  static void    bbgd_frontunpackreplies(bbgdstate* state,
                                         ae_int_t   requesttype,
                                         ae_int_t   querysize,
                                         ae_int_t   queryfuncs,
                                         ae_int_t   queryvars,
                                         ae_int_t   querydim,
                                         ae_int_t   queryformulasize,
                                         /* Real    */ const ae_vector* replyfi,
                                         /* Real    */ const ae_vector* replydj,
                                         const sparsematrix*            replysj,
                                         bbgdfront*                     front,
                                         ae_state*                      _state);
  static void    bbgd_entryprepareroot(bbgdfrontentry*       entry,
                                       const bbgdfront*      front,
                                       const bbgdsubproblem* rootsubproblem,
                                       bbgdstate*            state,
                                       ae_state*             _state);
  static ae_bool bbgd_entryprepareleafs(bbgdfrontentry*  entry,
                                        const bbgdfront* front,
                                        bbgdsubproblem*  s,
                                        bbgdstate*       state,
                                        ae_state*        _state);
  static void    bbgd_entrypreparex(bbgdfrontentry*  entry,
                                    const bbgdfront* front,
                                    bbgdstate*       state,
                                    ae_bool          isroot,
                                    ae_int_t         subproblemid,
                                    ae_state*        _state);
  static void    bbgd_entrypreparesubsolver(bbgdstate*            state,
                                            bbgdfront*            front,
                                            bbgdfrontentry*       entry,
                                            const bbgdsubproblem* subproblem,
                                            ae_bool               isroot,
                                            bbgdfrontsubsolver*   subsolver,
                                            ae_state*             _state);
  static ae_bool bbgd_subsolverrun(bbgdstate*          state,
                                   bbgdfront*          front,
                                   bbgdfrontentry*     entry,
                                   bbgdfrontsubsolver* subsolver,
                                   ae_state*           _state);
  static void    bbgd_entryaggregateandupdateglobalstats(bbgdfrontentry* entry,
                                                         bbgdstate*      state,
                                                         ae_state*       _state);
  static void    bbgd_entrydecideonfathoming(bbgdfrontentry* entry,
                                             bbgdstate*      state,
                                             ae_state*       _state);
  static void    bbgd_entrypushsolution(const bbgdfrontentry* entry,
                                        bbgdstate*            state,
                                        ae_bool*              setonupdate,
                                        ae_state*             _state);
  static ae_bool bbgd_entrytrypushanddive(bbgdfrontentry*  entry,
                                          const bbgdfront* front,
                                          bbgdstate*       state,
                                          ae_state*        _state);
  static void    bbgd_pushsubproblemsolution(const bbgdsubproblem* subproblem,
                                             bbgdstate*            state,
                                             ae_bool*              setonupdate,
                                             ae_state*             _state);
  static ae_int_t
              bbgd_qpquickpresolve(const bbgdfrontentry*          entry,
                                   bbgdfrontsubsolver*            subsolver,
                                   /* Real    */ const ae_vector* raws,
                                   /* Real    */ const ae_vector* rawxorigin,
                                   /* Real    */ const ae_vector* rawbndl,
                                   /* Real    */ const ae_vector* rawbndu,
                                   const sparsematrix*            rawa,
                                   ae_bool                        isupper,
                                   /* Real    */ const ae_vector* rawb,
                                   ae_int_t                       n,
                                   const sparsematrix*            rawsparsec,
                                   /* Real    */ const ae_vector* rawcl,
                                   /* Real    */ const ae_vector* rawcu,
                                   ae_int_t                       lccnt,
                                   /* Integer */ const ae_vector* qpordering,
                                   double                         eps,
                                   ae_state*                      _state);
  static void bbgd_solveqpnode(bbgdfrontentry*                entry,
                               bbgdfrontsubsolver*            subsolver,
                               bbgdstate*                     state,
                               /* Real    */ const ae_vector* x0,
                               /* Real    */ const ae_vector* bndl,
                               /* Real    */ const ae_vector* bndu,
                               ae_obj_array*                  subproblemarray,
                               ae_int_t                       itemidx,
                               ae_bool                        uselock,
                               ae_state*                      _state);
  static void bbgd_analyzeqpsolutionandenforceintegrality(
      bbgdfrontentry*                entry,
      /* Real    */ ae_vector*       xsol,
      /* Real    */ const ae_vector* nlrep,
      ae_int_t                       terminationtype,
      const bbgdstate*               state,
      bbgdsubproblem*                subproblem,
      ae_bool                        uselock,
      ae_bool*                       isintfeas,
      ae_state*                      _state);
  static void bbgd_analyzenlpsolutionandenforceintegrality(
      bbgdfrontentry*                entry,
      /* Real    */ ae_vector*       xsol,
      /* Real    */ const ae_vector* nlrep,
      ae_bool                        forbidrecognizingintegrality,
      const minnlcreport*            rep,
      double                         earlyerror,
      const bbgdstate*               state,
      ae_obj_array*                  subproblemarray,
      ae_int_t                       itemidx,
      ae_bool                        uselock,
      ae_state*                      _state);
  static void     bbgd_growheap(bbgdstate* state, ae_state* _state);
  static void     bbgd_growheapandpoptop(bbgdstate* state, ae_state* _state);
  static ae_int_t bbgd_subproblemheapgrow(ae_obj_array* subproblemheap,
                                          ae_int_t      offs,
                                          ae_int_t      heapsize,
                                          ae_int_t      appendcnt,
                                          ae_state*     _state);
  static ae_int_t bbgd_subproblemheappoptop(ae_obj_array* subproblemheap,
                                            ae_int_t      offs,
                                            ae_int_t      heapsize,
                                            ae_state*     _state);
  static void     bbgd_tracelaconicheader(bbgdstate* state, ae_state* _state);
  static void     bbgd_tracelaconic(bbgdstate* state, ae_state* _state);

#endif
#if defined(AE_COMPILE_MIRBFVNS) || !defined(AE_PARTIAL_BUILD)
  static ae_int_t mirbfvns_nodeunexplored          = 0;
  static ae_int_t mirbfvns_nodeinprogress          = 1;
  static ae_int_t mirbfvns_nodesolved              = 2;
  static ae_int_t mirbfvns_nodebad                 = 3;
  static ae_int_t mirbfvns_ncolstatus              = 0;
  static ae_int_t mirbfvns_ncolneighborbegin       = 1;
  static ae_int_t mirbfvns_ncolneighborend         = 2;
  static ae_int_t mirbfvns_ncolfbest               = 3;
  static ae_int_t mirbfvns_ncolhbest               = 4;
  static ae_int_t mirbfvns_ncolmxbest              = 5;
  static ae_int_t mirbfvns_ncollastaccepted        = 6;
  static ae_int_t mirbfvns_maxprimalcandforcut     = 10;
  static ae_int_t mirbfvns_softmaxnodescoeff       = 10;
  static ae_int_t mirbfvns_safetyboxforbbgd        = 5;
  static ae_int_t mirbfvns_rbfcloudsizemultiplier  = 4;
  static ae_int_t mirbfvns_rbfminimizeitsperphase  = 5;
  static double   mirbfvns_rbfsubsolverepsx        = 0.00001;
  static double   mirbfvns_eta2                    = 0.7;
  static double   mirbfvns_gammadec                = 0.5;
  static double   mirbfvns_gammadec2               = 0.66;
  static double   mirbfvns_gammadec3               = 0.05;
  static double   mirbfvns_gammainc                = 2.0;
  static double   mirbfvns_gammainc2               = 4.0;
  static double   mirbfvns_rbfpointunacceptablyfar = 10.0;
  static double   mirbfvns_rbfpointtooclose        = 0.01;
  static double   mirbfvns_rbfsktooshort           = 0.01;
  static double   mirbfvns_habovezero              = 50.0;
  static ae_int_t mirbfvns_maxipmits               = 200;
  static void     mirbfvns_clearoutputs(mirbfvnsstate* state, ae_state* _state);
  static void     mirbfvns_initinternal(ae_int_t                       n,
                                        /* Real    */ const ae_vector* x,
                                        ae_int_t                       solvermode,
                                        double                         diffstep,
                                        mirbfvnsstate*                 state,
                                        ae_state*                      _state);
  static ae_bool  mirbfvns_prepareinitialpoint(mirbfvnsstate*           state,
                                               /* Real    */ ae_vector* x,
                                               double*                  lcerr,
                                               ae_state*                _state);
  static void     mirbfvns_prepareevaluationbatch(mirbfvnsstate* state,
                                                  ae_state*      _state);
  static ae_int_t mirbfvns_expandneighborhood(mirbfvnsstate* state,
                                              ae_state*      _state);
  static void     mirbfvns_computeviolation2(const mirbfvnsstate*           state,
                                             /* Real    */ const ae_vector* x,
                                             /* Real    */ const ae_vector* fi,
                                             double*                        h,
                                             double*                        mx,
                                             ae_state*                      _state);
  static void     mirbfvns_findnearestintegralsubjecttocut(
          mirbfvnsstate*                 state,
          /* Real    */ const ae_vector* x0,
          /* Real    */ const ae_matrix* cutstable,
          /* Real    */ ae_matrix*       resultstable,
          /* Boolean */ ae_vector*       successflags,
          ae_int_t                       rowidx,
          ae_bool                        usesafetybox,
          ae_state*                      _state);
  static void mirbfvns_findnearestintegralsubjecttocutx(
      mirbfvnsstate*                 state,
      /* Real    */ const ae_vector* x0,
      /* Real    */ const ae_matrix* cutstable,
      /* Real    */ ae_matrix*       resultstable,
      /* Boolean */ ae_vector*       successflags,
      ae_int_t                       rowidx,
      ae_bool                        usesafetybox,
      mirbfvnstemporaries*           buf,
      ae_state*                      _state);
  static void mirbfvns_parallelfindnearestintegralsubjecttocut(
      mirbfvnsstate*                 state,
      /* Real    */ const ae_vector* x0,
      /* Real    */ const ae_matrix* cutstable,
      /* Real    */ ae_matrix*       resultstable,
      /* Boolean */ ae_vector*       successflags,
      ae_int_t                       r0,
      ae_int_t                       r1,
      ae_bool                        usesafetybox,
      ae_bool                        isroot,
      ae_bool                        tryparallelism,
      ae_state*                      _state);
  ae_bool _trypexec_mirbfvns_parallelfindnearestintegralsubjecttocut(
      mirbfvnsstate*                 state,
      /* Real    */ const ae_vector* x0,
      /* Real    */ const ae_matrix* cutstable,
      /* Real    */ ae_matrix*       resultstable,
      /* Boolean */ ae_vector*       successflags,
      ae_int_t                       r0,
      ae_int_t                       r1,
      ae_bool                        usesafetybox,
      ae_bool                        isroot,
      ae_bool                        tryparallelism,
      ae_state*                      _state);
  static void     mirbfvns_datasetinitempty(mirbfvnsdataset* dataset,
                                            mirbfvnsstate*   state,
                                            ae_state*        _state);
  static ae_int_t mirbfvns_datasetappendpoint(mirbfvnsdataset* dataset,
                                              /* Real    */ const ae_vector* x,
                                              /* Real    */ const ae_vector* fi,
                                              double                         h,
                                              double                         mx,
                                              ae_state* _state);
  static ae_int_t mirbfvns_gridcreate(mirbfvnsgrid*                  grid,
                                      mirbfvnsstate*                 state,
                                      /* Real    */ const ae_vector* x,
                                      /* Real    */ const ae_vector* fi,
                                      double                         h,
                                      double                         mx,
                                      ae_state*                      _state);
  static void     mirbfvns_gridappendpointtolist(mirbfvnsgrid* grid,
                                                 ae_int_t      pointidx,
                                                 ae_int_t      nodeidx,
                                                 ae_state*     _state);
  static ae_int_t mirbfvns_gridgetstatus(const mirbfvnsgrid*  grid,
                                         const mirbfvnsstate* state,
                                         ae_int_t             nodeidx,
                                         ae_state*            _state);
  static ae_bool  mirbfvns_gridneedsevals(mirbfvnsgrid*  grid,
                                          mirbfvnsstate* state,
                                          ae_int_t       nodeidx,
                                          ae_state*      _state);
  static ae_int_t
              mirbfvns_gridfindorcreatenode(mirbfvnsgrid*                  grid,
                                            mirbfvnsstate*                 state,
                                            /* Real    */ const ae_vector* x,
                                            ae_state*                      _state);
  static void mirbfvns_gridfindnodeslike(const mirbfvnsgrid*  grid,
                                         const mirbfvnsstate* state,
                                         ae_int_t             nodeidx,
                                         ae_bool              putfirst,
                                         /* Boolean */ const ae_vector* varmask,
                                         /* Integer */ ae_vector* nodeslist,
                                         ae_int_t*                nodescnt,
                                         ae_state*                _state);
  static void mirbfvns_gridappendnilsubsolver(mirbfvnsgrid* grid,
                                              ae_state*     _state);
  static void mirbfvns_gridinitnilsubsolver(mirbfvnsgrid*        grid,
                                            const mirbfvnsstate* state,
                                            ae_int_t             nodeidx,
                                            double               f,
                                            double               h,
                                            double               mx,
                                            ae_state*            _state);
  static ae_bool
                 mirbfvns_gridgetbestinneighborhood(mirbfvnsgrid*                  grid,
                                                    mirbfvnsstate*                 state,
                                                    /* Integer */ const ae_vector* neighbors,
                                                    ae_int_t  neighborscnt,
                                                    double*   fbest,
                                                    double*   hbest,
                                                    double*   mxbest,
                                                    ae_state* _state);
  static ae_bool mirbfvns_gridgetbestlastacceptedinunsolvedneighborhood(
      mirbfvnsgrid*                  grid,
      mirbfvnsstate*                 state,
      /* Integer */ const ae_vector* neighbors,
      ae_int_t                       neighborscnt,
      ae_int_t*                      nodeidx,
      double*                        fbest,
      double*                        hbest,
      double*                        mxbest,
      ae_state*                      _state);
  static void mirbfvns_gridexpandcutgenerateneighbors(
      mirbfvnsgrid*                  grid,
      mirbfvnsstate*                 state,
      /* Real    */ const ae_vector* xcentral,
      /* Real    */ const ae_vector* nodecut,
      /* Integer */ const ae_vector* excludelist,
      ae_int_t                       excludecnt,
      /* Integer */ ae_vector*       neighbornodes,
      /* Real    */ ae_matrix*       cutsapplied,
      /* Real    */ ae_matrix*       pointsfound,
      ae_int_t*                      nncnt,
      /* Boolean */ ae_vector*       tmpsuccessflags,
      ae_state*                      _state);
  static void
          mirbfvns_gridparallelproposelocaltrialpoint(const mirbfvnsgrid*  grid,
                                                      mirbfvnsgrid*        sharedgrid,
                                                      const mirbfvnsstate* state,
                                                      mirbfvnsstate*       sharedstate,
                                                      ae_int_t             r0,
                                                      ae_int_t             r1,
                                                      ae_bool              isroot,
                                                      ae_bool   tryparallelism,
                                                      ae_state* _state);
  ae_bool _trypexec_mirbfvns_gridparallelproposelocaltrialpoint(
      const mirbfvnsgrid*  grid,
      mirbfvnsgrid*        sharedgrid,
      const mirbfvnsstate* state,
      mirbfvnsstate*       sharedstate,
      ae_int_t             r0,
      ae_int_t             r1,
      ae_bool              isroot,
      ae_bool              tryparallelism,
      ae_state*            _state);
  static void
  mirbfvns_gridproposelocaltrialpointnomask(const mirbfvnsgrid*  grid,
                                            mirbfvnsgrid*        sharedgrid,
                                            const mirbfvnsstate* state,
                                            mirbfvnsstate*       sharedstate,
                                            ae_int_t             nodeidx,
                                            ae_int_t             rngseedtouse0,
                                            ae_int_t             rngseedtouse1,
                                            ae_int_t             evalbatchidx,
                                            ae_state*            _state);
  static void
  mirbfvns_gridproposelocaltrialpointmasked(const mirbfvnsgrid*  grid,
                                            mirbfvnsgrid*        sharedgrid,
                                            const mirbfvnsstate* state,
                                            mirbfvnsstate*       sharedstate,
                                            ae_int_t             nodeidx,
                                            ae_int_t             rngseedtouse0,
                                            ae_int_t             rngseedtouse1,
                                            ae_int_t             evalbatchidx,
                                            ae_state*            _state);
  static void
  mirbfvns_gridproposetrialpointwhenexploringfrom(const mirbfvnsgrid* grid,
                                                  mirbfvnsgrid* sharedgrid,
                                                  const mirbfvnsstate* state,
                                                  mirbfvnsstate* sharedstate,
                                                  ae_int_t       newnodeidx,
                                                  ae_int_t  explorefromnode,
                                                  ae_int_t  rngseedtouse0,
                                                  ae_int_t  rngseedtouse1,
                                                  ae_int_t  evalbatchidx,
                                                  ae_state* _state);
  static void
                 mirbfvns_gridsendtrialpointto(mirbfvnsgrid*                  grid,
                                               mirbfvnsstate*                 state,
                                               ae_int_t                       centralnodeidx,
                                               ae_int_t                       nodeidx,
                                               /* Real    */ const ae_vector* xtrial,
                                               /* Real    */ const ae_vector* replyfi,
                                               ae_state*                      _state);
  static void    mirbfvns_gridoffloadbestpoint(const mirbfvnsgrid*      grid,
                                               const mirbfvnsstate*     state,
                                               ae_int_t                 nodeidx,
                                               /* Real    */ ae_vector* x,
                                               ae_int_t*                pointidx,
                                               double*                  f,
                                               double*                  h,
                                               double*                  mx,
                                               ae_state*                _state);
  static ae_bool mirbfvns_isbetterpoint(double    f0,
                                        double    h0,
                                        double    mx0,
                                        double    f1,
                                        double    h1,
                                        double    mx1,
                                        double    ctol,
                                        ae_state* _state);
  static ae_bool mirbfvns_gridisbetter(mirbfvnsgrid*  grid,
                                       mirbfvnsstate* state,
                                       ae_int_t       baseidx,
                                       ae_int_t       candidx,
                                       ae_state*      _state);
  static double  mirbfvns_gridgetpointscountinnode(mirbfvnsgrid*  grid,
                                                   mirbfvnsstate* state,
                                                   ae_int_t       nodeidx,
                                                   ae_state*      _state);
  static double  mirbfvns_gridgetfbest(mirbfvnsgrid*  grid,
                                       mirbfvnsstate* state,
                                       ae_int_t       nodeidx,
                                       ae_state*      _state);
  static double  mirbfvns_gridgethbest(mirbfvnsgrid*  grid,
                                       mirbfvnsstate* state,
                                       ae_int_t       nodeidx,
                                       ae_state*      _state);
  static double  mirbfvns_gridgetmxbest(mirbfvnsgrid*  grid,
                                        mirbfvnsstate* state,
                                        ae_int_t       nodeidx,
                                        ae_state*      _state);
  static void
              mirbfvns_rbfminimizemodel(const mirbfmodel*              model,
                                        /* Real    */ const ae_vector* x0,
                                        /* Real    */ const ae_vector* bndl,
                                        /* Real    */ const ae_vector* bndu,
                                        /* Real    */ const ae_vector* trustregion,
                                        double                         trustradfactor,
                                        double                         ctol,
                                        ae_int_t                       maxitsperphase,
                                        ae_bool                        autoscalemodel,
                                        const sparsematrix*            c,
                                        /* Real    */ const ae_vector* cl,
                                        /* Real    */ const ae_vector* cu,
                                        ae_int_t                       lccnt,
                                        /* Real    */ const ae_vector* nl,
                                        /* Real    */ const ae_vector* nu,
                                        ae_int_t                       nnlc,
                                        ae_int_t                       n,
                                        rbfmmtemporaries*              buf,
                                        /* Real    */ ae_vector*       xn,
                                        /* Real    */ ae_vector*       sk,
                                        double*                        predf,
                                        double*                        predh,
                                        ae_int_t*                      subsolverits,
                                        ae_state*                      _state);
  static void mirbfvns_rbfinitmodel(/* Real    */ const ae_matrix* xf,
                                    /* Real    */ const ae_vector* multscale,
                                    ae_int_t                       nc,
                                    ae_int_t                       n,
                                    ae_int_t                       nf,
                                    mirbfmodel*                    model,
                                    ae_state*                      _state);
  static void
  mirbfvns_rbfinitemptysparsemodel(/* Real    */ const ae_vector* multscale,
                                   ae_int_t                       n,
                                   mirbfmodel*                    model,
                                   ae_state*                      _state);
  static void   mirbfvns_rbfappendconstantmodel(mirbfmodel* model,
                                                double      v,
                                                ae_state*   _state);
  static void   mirbfvns_rbfappendmodel(mirbfmodel*                    model,
                                        const mirbfmodel*              minimodel,
                                        /* Integer */ const ae_vector* mini2full,
                                        ae_state*                      _state);
  static void   mirbfvns_rbfaddlinearterm(mirbfmodel*                    model,
                                          /* Real    */ const ae_matrix* c,
                                          ae_state*                      _state);
  static void   mirbfvns_rbfcomputemodel(const mirbfmodel*              mmodel,
                                         /* Real    */ const ae_vector* x,
                                         /* Real    */ ae_vector*       f,
                                         ae_bool                        needf,
                                         /* Real    */ ae_vector*       g,
                                         ae_bool                        needg,
                                         ae_state*                      _state);
  static void   mirbfvns_rbfsolvecpdm(/* Real    */ const ae_matrix* a,
                                    /* Real    */ const ae_matrix* bb,
                                    ae_int_t                       ncenters,
                                    ae_int_t                       nrhs,
                                    ae_int_t                       nx,
                                    double                         lambdav,
                                    ae_bool                        iscpd,
                                    /* Real    */ ae_matrix*       ssol,
                                    ae_state*                      _state);
  static double mirbfvns_rdistinfrr(ae_int_t                       n,
                                    /* Real    */ const ae_matrix* a,
                                    ae_int_t                       i0,
                                    /* Real    */ const ae_matrix* b,
                                    ae_int_t                       i1,
                                    ae_state*                      _state);

#endif
#if defined(AE_COMPILE_MINLPSOLVERS) || !defined(AE_PARTIAL_BUILD)
  static void minlpsolvers_clearoutputs(minlpsolverstate* state,
                                        ae_state*         _state);
  static void minlpsolvers_initinternal(ae_int_t                       n,
                                        /* Real    */ const ae_vector* x,
                                        ae_int_t          solvermode,
                                        double            diffstep,
                                        minlpsolverstate* state,
                                        ae_state*         _state);

#endif

#if defined(AE_COMPILE_BBGD) || !defined(AE_PARTIAL_BUILD)

  /*************************************************************************
  BBGD solver initialization.
  --------------------------------------------------------------------------

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdcreatebuf(ae_int_t                       n,
                     /* Real    */ const ae_vector* bndl,
                     /* Real    */ const ae_vector* bndu,
                     /* Real    */ const ae_vector* s,
                     /* Real    */ const ae_vector* x0,
                     /* Boolean */ const ae_vector* isintegral,
                     /* Boolean */ const ae_vector* isbinary,
                     /* Boolean */ const ae_vector* islinear,
                     const sparsematrix*            sparsea,
                     /* Real    */ const ae_vector* al,
                     /* Real    */ const ae_vector* au,
                     /* Integer */ const ae_vector* lcsrcidx,
                     ae_int_t                       lccnt,
                     /* Real    */ const ae_vector* nl,
                     /* Real    */ const ae_vector* nu,
                     ae_int_t                       nnlc,
                     ae_bool                        issuesparserequests,
                     ae_int_t                       groupsize,
                     ae_int_t                       nmultistarts,
                     ae_int_t                       timeout,
                     ae_int_t                       tracelevel,
                     bbgdstate*                     state,
                     ae_state*                      _state) {
    ae_int_t i;

    ae_assert(n >= 1, "BBGDCreateBuf: N<1", _state);
    ae_assert(x0->cnt >= n, "BBGDCreateBuf: Length(X0)<N", _state);
    ae_assert(isfinitevector(x0, n, _state),
              "BBGDCreateBuf: X contains infinite or NaN values",
              _state);
    ae_assert(bndl->cnt >= n, "BBGDCreateBuf: Length(BndL)<N", _state);
    ae_assert(bndu->cnt >= n, "BBGDCreateBuf: Length(BndU)<N", _state);
    ae_assert(s->cnt >= n, "BBGDCreateBuf: Length(S)<N", _state);
    ae_assert(
        isintegral->cnt >= n, "BBGDCreateBuf: Length(IsIntegral)<N", _state);
    ae_assert(isbinary->cnt >= n, "BBGDCreateBuf: Length(IsBinary)<N", _state);
    ae_assert(islinear->cnt >= n, "BBGDCreateBuf: Length(IsLinear)<N", _state);
    ae_assert(nnlc >= 0, "BBGDCreateBuf: NNLC<0", _state);
    ae_assert(nl->cnt >= nnlc, "BBGDCreateBuf: Length(NL)<NNLC", _state);
    ae_assert(nu->cnt >= nnlc, "BBGDCreateBuf: Length(NU)<NNLC", _state);
    ae_assert(groupsize >= 1, "BBGDCreateBuf: GroupSize<1", _state);
    ae_assert(nmultistarts >= 1, "BBGDCreateBuf: NMultistarts<1", _state);
    ae_assert(timeout >= 0, "BBGDCreateBuf: Timeout<0", _state);
    ae_assert((tracelevel == 0 || tracelevel == 1) || tracelevel == 2,
              "BBGDCreateBuf: unexpected trace level",
              _state);
    bbgd_initinternal(n, x0, 0, 0.0, state, _state);
    state->issuesparserequests = issuesparserequests;
    state->forceserial         = ae_false;
    state->bbgdgroupsize       = groupsize;
    state->nmultistarts        = nmultistarts;
    state->timeout             = timeout;
    state->dotrace             = tracelevel == 2;
    state->dolaconictrace      = tracelevel == 1;
    state->doanytrace          = state->dotrace || state->dolaconictrace;
    for (i = 0; i <= n - 1; i++) {
      ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)
                    || ae_isneginf(bndl->ptr.p_double[i], _state),
                "BBGDCreateBuf: BndL contains NAN or +INF",
                _state);
      ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)
                    || ae_isposinf(bndu->ptr.p_double[i], _state),
                "BBGDCreateBuf: BndL contains NAN or -INF",
                _state);
      ae_assert(isintegral->ptr.p_bool[i] || !isbinary->ptr.p_bool[i],
                "BBGDCreateBuf: variable marked as binary but not integral",
                _state);
      ae_assert(ae_isfinite(s->ptr.p_double[i], _state),
                "BBGDCreateBuf: S contains infinite or NAN elements",
                _state);
      ae_assert(ae_fp_neq(s->ptr.p_double[i], (double) (0)),
                "BBGDCreateBuf: S contains zero elements",
                _state);
      state->bndl.ptr.p_double[i]  = bndl->ptr.p_double[i];
      state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
      state->bndu.ptr.p_double[i]  = bndu->ptr.p_double[i];
      state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
      state->isintegral.ptr.p_bool[i] = isintegral->ptr.p_bool[i];
      state->isbinary.ptr.p_bool[i]   = isbinary->ptr.p_bool[i];
      state->islinear.ptr.p_bool[i]   = islinear->ptr.p_bool[i];
      state->s.ptr.p_double[i]        = rcase2(isintegral->ptr.p_bool[i],
                                        1.0,
                                        ae_fabs(s->ptr.p_double[i], _state),
                                        _state);
    }
    state->lccnt = lccnt;
    if (lccnt > 0) {
      sparsecopybuf(sparsea, &state->rawa, _state);
      rcopyallocv(lccnt, al, &state->rawal, _state);
      rcopyallocv(lccnt, au, &state->rawau, _state);
      icopyallocv(lccnt, lcsrcidx, &state->lcsrcidx, _state);
    }
    state->nnlc = nnlc;
    rallocv(nnlc, &state->nl, _state);
    rallocv(nnlc, &state->nu, _state);
    for (i = 0; i <= nnlc - 1; i++) {
      ae_assert(ae_isfinite(nl->ptr.p_double[i], _state)
                    || ae_isneginf(nl->ptr.p_double[i], _state),
                "BBGDCreateBuf: NL[i] is +INF or NAN",
                _state);
      ae_assert(ae_isfinite(nu->ptr.p_double[i], _state)
                    || ae_isposinf(nu->ptr.p_double[i], _state),
                "BBGDCreateBuf: NU[i] is -INF or NAN",
                _state);
      state->nl.ptr.p_double[i] = nl->ptr.p_double[i];
      state->nu.ptr.p_double[i] = nu->ptr.p_double[i];
    }
  }

  /*************************************************************************
  Enforces serial processing (useful when BBGD is called as a part of larger
  multithreaded algorithm)

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdforceserial(bbgdstate* state, ae_state* _state) {
    state->forceserial = ae_true;
  }

  /*************************************************************************
  Set required primal-dual gap

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetpdgap(bbgdstate* state, double pdgap, ae_state* _state) {
    state->pdgap = pdgap;
  }

  /*************************************************************************
  Set tolerance for violation of nonlinear constraints

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetctol(bbgdstate* state, double ctol, ae_state* _state) {
    state->ctol = ctol;
  }

  /*************************************************************************
  Set subsolver stopping condition

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetepsf(bbgdstate* state, double epsf, ae_state* _state) {
    state->epsf = epsf;
  }

  /*************************************************************************
  Small tree profile

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetsmalltree(bbgdstate* state, ae_state* _state) {
    state->branchingtype   = 1;
    state->krel            = 1;
    state->kevalunreliable = 1;
    state->kevalreliable   = 1;
  }

  /*************************************************************************
  Large tree profile

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetlargetree(bbgdstate* state, ae_state* _state) {
    state->branchingtype   = 2;
    state->krel            = 1;
    state->kevalunreliable = state->n;
    state->kevalreliable   = 1;
  }

  /*************************************************************************
  Sets IPM subsolver

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetipm(bbgdstate* state, ae_int_t memlen, ae_state* _state) {
    state->subsolveralgo   = 0;
    state->subsolvermemlen = memlen;
  }

  /*************************************************************************
  Sets SQP subsolver

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetsqp(bbgdstate* state, ae_state* _state) {
    state->subsolveralgo = 1;
  }

  /*************************************************************************
  Sets diving strategy:
  * 0 for no diving
  * 1 for diving until finding first primal solution, then switching to best
    first
  * 2 for always diving

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetdiving(bbgdstate* state, ae_int_t divingmode, ae_state* _state) {
    if (divingmode == 0) {
      state->dodiving = bbgd_divenever;
      return;
    }
    if (divingmode == 1) {
      state->dodiving = bbgd_diveuntilprimal;
      return;
    }
    if (divingmode == 2) {
      state->dodiving = bbgd_divealways;
      return;
    }
    ae_assert(ae_false, "BBGDSetDiving: unexpected diving mode", _state);
  }

  /*************************************************************************
  Tells the solver to stop after finding MaxCand primal candidates (integral
  solutions that were accepted or fathomed)

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetmaxprimalcandidates(bbgdstate* state,
                                  ae_int_t   maxcand,
                                  ae_state*  _state) {
    state->maxprimalcandidates = maxcand;
  }

  /*************************************************************************
  Set soft max nodes (stop after this amount of nodes, if we have a primal
  solution)

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetsoftmaxnodes(bbgdstate* state,
                           ae_int_t   maxnodes,
                           ae_state*  _state) {
    state->softmaxnodes = maxnodes;
  }

  /*************************************************************************
  Set hard max nodes (stop after this amount of nodes, no matter primal
  solution status)

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsethardmaxnodes(bbgdstate* state,
                           ae_int_t   maxnodes,
                           ae_state*  _state) {
    state->hardmaxnodes = maxnodes;
  }

  /*************************************************************************
  Set subsolver stopping condition

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetepsx(bbgdstate* state, double epsx, ae_state* _state) {
    state->epsx = epsx;
  }

  /*************************************************************************
  Sets quadratic objective. If no nonlinear constraints  were given,  it  is
  guaranteed that no RCOMM requests will be issued during the optimization.

  The objective has the form 0.5*x'*A*x + b'*x + c0

  Sparse A can be stored in any format, but presently this function supports
  only matrices given by their lower triangle.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void bbgdsetquadraticobjective(bbgdstate*                     state,
                                 const sparsematrix*            a,
                                 ae_bool                        isupper,
                                 /* Real    */ const ae_vector* b,
                                 double                         c0,
                                 ae_state*                      _state) {
    ae_assert(!isupper,
              "BBGDSetQuadraticObjective: IsUpper=False is not implemented yet",
              _state);
    state->objtype = 1;
    sparsecopytocrs(a, &state->obja, _state);
    rcopyallocv(state->n, b, &state->objb, _state);
    state->objc0 = c0;
  }

  /*************************************************************************


    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  ae_bool bbgditeration(bbgdstate* state, ae_state* _state) {
    ae_int_t n;
    ae_int_t i;
    ae_int_t k;
    ae_bool  result;

    /*
     * Reverse communication preparations
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if (state->rcommv2.stage >= 0) {
      n = state->rcommv2.ia.ptr.p_int[0];
      i = state->rcommv2.ia.ptr.p_int[1];
      k = state->rcommv2.ia.ptr.p_int[2];
    } else {
      n = 359;
      i = -58;
      k = -919;
    }
    if (state->rcommv2.stage == 0) {
      goto lbl_0;
    }
    if (state->rcommv2.stage == 1) {
      goto lbl_1;
    }
    if (state->rcommv2.stage == 2) {
      goto lbl_2;
    }

    /*
     * Routine body
     */

    /*
     * Init
     */
    n = state->n;
    bbgd_clearoutputs(state, _state);
    ae_obj_array_clear(&state->bbsubproblems);
    state->bbsubproblemsheapsize      = 0;
    state->bbsubproblemsrecentlyadded = 0;
    rsetallocv(state->n, 1.0, &state->pseudocostsup, _state);
    rsetallocv(state->n, 1.0, &state->pseudocostsdown, _state);
    isetallocv(state->n, 0, &state->pseudocostscntup, _state);
    isetallocv(state->n, 0, &state->pseudocostscntdown, _state);
    state->globalpseudocostup             = 1.0;
    state->globalpseudocostdown           = 1.0;
    state->globalpseudocostcntup          = 0;
    state->globalpseudocostcntdown        = 0;
    state->globalsynchronizednfev         = 0;
    state->globalsynchronizednsubproblems = 0;
    ae_assert(state->objtype == 0 || state->objtype == 1,
              "BBGD: 661544 failed",
              _state);
    if (state->objtype == 1) {
      for (i = 0; i <= n - 1; i++) {
        if (state->obja.ridx.ptr.p_int[i] < state->obja.didx.ptr.p_int[i]) {
          ae_assert(
              ae_false,
              "BBGD: preordering for non-diagonal A is not implemented yet",
              _state);
        }
      }
      ipm2proposeordering(&state->dummyqpsubsolver,
                          n,
                          ae_true,
                          &state->hasbndl,
                          &state->hasbndu,
                          &state->rawa,
                          &state->rawal,
                          &state->rawau,
                          state->lccnt,
                          &state->qpordering,
                          _state);
    }
    state->usehandlersandsync = _rcommstate_has_handler(&state->rcommv2);

    /*
     * Initial synchronization interval
     */
    if (state->syncinterval == 0) {
      state->syncinterval = 5;
      if (state->subsolveralgo == 0) {
        state->syncinterval = 15;
      }
      if (state->subsolveralgo == 1) {
        state->syncinterval = 5;
      }
    }

    /*
     * Initialize globally shared information
     */
    state->nextleafid             = 0;
    state->hasprimalsolution      = ae_false;
    state->fprim                  = _state->v_posinf;
    state->timedout               = ae_false;
    state->unbounded              = ae_false;
    state->ffdual                 = _state->v_neginf;
    state->lastlaconicreportepoch = -999;
    bbgd_frontinitundefined(&state->front, state, _state);
    ae_shared_pool_set_seed_if_different(
        &state->sppool,
        &state->dummysubproblem,
        (ae_int_t) sizeof(state->dummysubproblem),
        (ae_copy_constructor) _bbgdsubproblem_init_copy,
        (ae_destructor) _bbgdsubproblem_destroy,
        _state);
    ae_shared_pool_set_seed_if_different(
        &state->subsolverspool,
        &state->dummysubsolver,
        (ae_int_t) sizeof(state->dummysubsolver),
        (ae_copy_constructor) _bbgdfrontsubsolver_init_copy,
        (ae_destructor) _bbgdfrontsubsolver_destroy,
        _state);

    /*
     * Prepare root subproblem, perform initial feasibility checks, solve it.
     * This part is the same for all BB algorithms.
     */
    stimerinit(&state->timerglobal, _state);
    stimerstart(&state->timerglobal, _state);
    state->rootsubproblem.leafid
        = weakatomicfetchadd(&state->nextleafid, 1, _state);
    state->rootsubproblem.branchbucket = -1;
    state->rootsubproblem.parentfdual  = ae_maxrealnumber;
    bcopyallocv(
        n, &state->islinear, &state->rootsubproblem.parentlinearity, _state);
    state->rootsubproblem.n = n;
    ae_assert(state->hasx0, "BBGD: integrity check 500655 failed", _state);
    rcopyallocv(n, &state->x0, &state->rootsubproblem.x0, _state);
    rcopyallocv(n, &state->bndl, &state->rootsubproblem.bndl, _state);
    rcopyallocv(n, &state->bndu, &state->rootsubproblem.bndu, _state);
    for (i = 0; i <= n - 1; i++) {
      if (state->isintegral.ptr.p_bool[i]) {
        if (ae_isfinite(state->rootsubproblem.bndl.ptr.p_double[i], _state)) {
          state->rootsubproblem.bndl.ptr.p_double[i] = (double) (ae_iceil(
              state->rootsubproblem.bndl.ptr.p_double[i] - state->ctol,
              _state));
        }
        if (ae_isfinite(state->rootsubproblem.bndu.ptr.p_double[i], _state)) {
          state->rootsubproblem.bndu.ptr.p_double[i] = (double) (ae_ifloor(
              state->rootsubproblem.bndu.ptr.p_double[i] + state->ctol,
              _state));
        }
      }
      if (state->isbinary.ptr.p_bool[i]) {
        if (ae_isneginf(state->rootsubproblem.bndl.ptr.p_double[i], _state)
            || ae_fp_less(state->rootsubproblem.bndl.ptr.p_double[i],
                          (double) (0))) {
          state->rootsubproblem.bndl.ptr.p_double[i] = (double) (0);
        }
        if (ae_isposinf(state->rootsubproblem.bndu.ptr.p_double[i], _state)
            || ae_fp_greater(state->rootsubproblem.bndu.ptr.p_double[i],
                             (double) (1))) {
          state->rootsubproblem.bndu.ptr.p_double[i] = (double) (1);
        }
      }
    }
    state->rootsubproblem.hasprimalsolution = ae_false;
    state->rootsubproblem.hasdualsolution   = ae_false;
    state->rootsubproblem.ncuttingplanes    = 0;
    bsetallocv(n, ae_true, &state->rootsubproblem.subproblemlinearity, _state);
    for (i = 0; i <= n - 1; i++) {
      if ((ae_isfinite(state->rootsubproblem.bndl.ptr.p_double[i], _state)
           && ae_isfinite(state->rootsubproblem.bndu.ptr.p_double[i], _state))
          && ae_fp_greater(state->rootsubproblem.bndl.ptr.p_double[i],
                           state->rootsubproblem.bndu.ptr.p_double[i]
                               + state->ctol)) {
        if (state->doanytrace) {
          ae_trace("> a combination of box and integrality constraints is "
                   "infeasible, stopping\n");
        }
        state->repterminationtype = -3;
        result                    = ae_false;
        return result;
      }
    }
    if (state->doanytrace) {
      ae_trace("> generated root node, starting to solve it\n");
    }
    bbgd_frontstartroot(&state->front, &state->rootsubproblem, state, _state);
  lbl_3:
    if (!bbgd_frontrun(&state->front, state, _state)) {
      goto lbl_4;
    }
    state->requestsource = bbgd_rqsrcfront;
    state->rcommv2.stage = 0;
    if (state->rcommv2.rcomm2_handler != NULL && state->rcommv2.requesttype != 0
        && state->rcommv2.requesttype <= _ALGLIB_MAX_RCOMMV2_REQUEST)
      state->rcommv2.rcomm2_handler(&state->rcommv2,
                                    state->rcommv2.handler_p0,
                                    state->rcommv2.handler_p1,
                                    state->rcommv2.handler_p2,
                                    state->rcommv2.handler_p3,
                                    _state);
    else
      goto lbl_rcomm;
  lbl_0:
    goto lbl_3;
  lbl_4:
    ae_assert((state->front.frontstatus == bbgd_stsolved
               || state->front.frontstatus == bbgd_sttimeout)
                  || state->front.frontstatus == bbgd_stunbounded,
              "BBGD: integrity check 184017 failed",
              _state);
    if (state->front.frontstatus != bbgd_stsolved) {
      goto lbl_5;
    }
    if (state->doanytrace) {
      ae_trace("> root subproblem solved in %0.0f ms\n",
               (double) (stimergetmsrunning(&state->timerglobal, _state)));
      ae_trace(">> primal (upper) bound is %0.12e\n", (double) (state->fprim));
      ae_trace(">> dual   (lower) bound is %0.12e\n", (double) (state->ffdual));
      ae_trace("> proceeding to branch-and-bound tree search\n");
    }
    if (state->dolaconictrace) {
      bbgd_tracelaconicheader(state, _state);
      bbgd_tracelaconic(state, _state);
      state->lastlaconicreportepoch
          = ae_ifloor(stimergetmsrunning(&state->timerglobal, _state)
                          / (double) bbgd_laconicreportperiod,
                      _state);
    }
    state->repterminationtype = 1;
    bbgd_frontstartdynamic(&state->front, state, _state);
  lbl_7:
    if (!bbgd_frontrun(&state->front, state, _state)) {
      goto lbl_8;
    }
    state->requestsource = bbgd_rqsrcfront;
    state->rcommv2.stage = 1;
    if (state->rcommv2.rcomm2_handler != NULL && state->rcommv2.requesttype != 0
        && state->rcommv2.requesttype <= _ALGLIB_MAX_RCOMMV2_REQUEST)
      state->rcommv2.rcomm2_handler(&state->rcommv2,
                                    state->rcommv2.handler_p0,
                                    state->rcommv2.handler_p1,
                                    state->rcommv2.handler_p2,
                                    state->rcommv2.handler_p3,
                                    _state);
    else
      goto lbl_rcomm;
  lbl_1:
    goto lbl_7;
  lbl_8:
    ae_assert((state->front.frontstatus == bbgd_stsolved
               || state->front.frontstatus == bbgd_sttimeout)
                  || state->front.frontstatus == bbgd_stunbounded,
              "BBGD: integrity check 826253 failed",
              _state);
    if (state->front.frontstatus == bbgd_sttimeout) {
      if (state->doanytrace) {
        ae_trace("> timeout was signaled, %0.0f ms passed\n",
                 (double) (stimergetmsrunning(&state->timerglobal, _state)));
      }
      state->timedout = ae_true;
    }
    if (state->front.frontstatus == bbgd_stunbounded) {
      if (state->doanytrace) {
        ae_trace("> unboundedness was signaled, the problem is likely to be "
                 "unbounded (suspiciously large |x| or -f)\n");
      }
      state->unbounded = ae_true;
    }
    goto lbl_6;
  lbl_5:
    if (state->front.frontstatus == bbgd_sttimeout) {
      if (state->doanytrace) {
        ae_trace("> timeout was signaled during solution of the root "
                 "subproblem, %0.0f ms passed\n",
                 (double) (stimergetmsrunning(&state->timerglobal, _state)));
      }
      state->timedout = ae_true;
    }
    if (state->front.frontstatus == bbgd_stunbounded) {
      if (state->doanytrace) {
        ae_trace("> unboundedness was signaled during solution of the root "
                 "subproblem\n");
      }
      state->unbounded = ae_true;
    }
  lbl_6:
    if (state->unbounded) {
      state->hasprimalsolution = ae_false;
    }

    /*
     * Write out solution
     */
    if (!state->hasprimalsolution) {
      goto lbl_9;
    }

    /*
     * A primal solution was found
     */
    ae_assert(state->repterminationtype > 0,
              "BBGD: integrity check 080232 failed",
              _state);
    ae_assert(state->objtype == 0 || state->objtype == 1,
              "BBGD: 778547 failed",
              _state);
    rcopyallocv(n, &state->xprim, &state->xc, _state);
    if (state->objtype != 0) {
      goto lbl_11;
    }
    state->requestsource = bbgd_rqsrcxc;
    state->rcommv2.stage = 2;
    if (state->rcommv2.rcomm2_handler != NULL && state->rcommv2.requesttype != 0
        && state->rcommv2.requesttype <= _ALGLIB_MAX_RCOMMV2_REQUEST)
      state->rcommv2.rcomm2_handler(&state->rcommv2,
                                    state->rcommv2.handler_p0,
                                    state->rcommv2.handler_p1,
                                    state->rcommv2.handler_p2,
                                    state->rcommv2.handler_p3,
                                    _state);
    else
      goto lbl_rcomm;
  lbl_2:
    goto lbl_12;
  lbl_11:
    state->repf = 0.5 * sparsevsmv(&state->obja, ae_false, &state->xc, _state)
                  + rdotv(n, &state->xc, &state->objb, _state) + state->objc0;
  lbl_12:
    state->reppdgap
        = ae_maxreal(state->fprim - state->ffdual, (double) (0), _state)
          / rmaxabs2(state->fprim, (double) (1), _state);
    state->repterminationtype = 1;
    if (state->timedout) {
      state->repterminationtype = 5;
    }
    if (state->doanytrace) {
      ae_trace(
          "> a primal solution is found: f=%0.9e, relative duality gap is "
          "%0.3e; %0.3fs passed\n",
          (double) (state->repf),
          (double) (state->reppdgap),
          (double) (0.001 * stimergetmsrunning(&state->timerglobal, _state)));
    }
    goto lbl_10;
  lbl_9:

    /*
     * The problem is infeasible
     */
    ae_assert((state->front.frontstatus == bbgd_stsolved
               || state->front.frontstatus == bbgd_sttimeout)
                  || state->front.frontstatus == bbgd_stunbounded,
              "BBGD: integrity check 280023 failed",
              _state);
    state->repterminationtype = -3;
    if (state->timedout) {
      state->repterminationtype = -33;
    }
    if (state->unbounded) {
      state->repterminationtype = -4;
    }
    if (state->doanytrace) {
      if (state->repterminationtype != -4) {
        ae_trace(
            "> the problem is infeasible (or feasible point is too difficult "
            "to find); %0.3fs passed\n",
            (double) (0.001 * stimergetmsrunning(&state->timerglobal, _state)));
      }
    }
    if (state->doanytrace) {
      if (state->repterminationtype == -4) {
        ae_trace(
            "> the problem is unbounded; %0.3fs passed\n",
            (double) (0.001 * stimergetmsrunning(&state->timerglobal, _state)));
      }
    }
  lbl_10:
    result = ae_false;
    return result;

    /*
     * Saving state
     */
  lbl_rcomm:
    result                         = ae_true;
    state->rcommv2.ia.ptr.p_int[0] = n;
    state->rcommv2.ia.ptr.p_int[1] = i;
    state->rcommv2.ia.ptr.p_int[2] = k;
    return result;
  }

  /*************************************************************************
  Checks whether request came from front (used for integrity checks)
  *************************************************************************/
  ae_bool bbgdisrequestfromfront(const bbgdstate* state, ae_state* _state) {
    ae_bool result;

    result = state->requestsource == bbgd_rqsrcfront;
    return result;
  }

  /*************************************************************************
  Produce RComm request in RCOMM-V2 format, according to the current request
  source
  *************************************************************************/
  void bbgdoffloadrcommrequest(bbgdstate*               state,
                               ae_int_t*                requesttype,
                               ae_int_t*                querysize,
                               ae_int_t*                queryfuncs,
                               ae_int_t*                queryvars,
                               ae_int_t*                querydim,
                               ae_int_t*                queryformulasize,
                               /* Real    */ ae_vector* querydata,
                               ae_state*                _state) {
    if (state->requestsource == bbgd_rqsrcfront) {
      *requesttype = 0;
      bbgd_frontpackqueries(&state->front,
                            state,
                            requesttype,
                            querysize,
                            queryfuncs,
                            queryvars,
                            querydim,
                            queryformulasize,
                            querydata,
                            _state);
      ae_assert(*querysize > 0, "BBGD: 074812 failed", _state);
      return;
    }
    if (state->requestsource == bbgd_rqsrcxc) {
      *requesttype = icase2(state->issuesparserequests, 1, 2, _state);
      *querysize   = 1;
      *queryfuncs  = 1 + state->nnlc;
      *queryvars   = state->n;
      *querydim    = 0;
      rcopyallocv(state->n, &state->xc, querydata, _state);
      return;
    }
    ae_assert(ae_false, "BBGD: integrity check 094519 failed", _state);
  }

  /*************************************************************************
  Process RComm reply in RCOMM-V2 format, according to the current request
  source
  *************************************************************************/
  void bbgdloadrcommreply(bbgdstate*                     state,
                          ae_int_t                       requesttype,
                          ae_int_t                       querysize,
                          ae_int_t                       queryfuncs,
                          ae_int_t                       queryvars,
                          ae_int_t                       querydim,
                          ae_int_t                       queryformulasize,
                          /* Real    */ const ae_vector* replyfi,
                          /* Real    */ const ae_vector* replydj,
                          const sparsematrix*            replysj,
                          ae_state*                      _state) {
    if (state->requestsource == bbgd_rqsrcfront) {
      bbgd_frontunpackreplies(state,
                              requesttype,
                              querysize,
                              queryfuncs,
                              queryvars,
                              querydim,
                              queryformulasize,
                              replyfi,
                              replydj,
                              replysj,
                              &state->front,
                              _state);
      return;
    }
    if (state->requestsource == bbgd_rqsrcxc) {
      state->repf = replyfi->ptr.p_double[0];
      return;
    }
    ae_assert(ae_false, "BBGD: integrity check 118522 failed", _state);
  }

  /*************************************************************************
  Clears output fields during initialization
  *************************************************************************/
  static void bbgd_clearoutputs(bbgdstate* state, ae_state* _state) {
    state->userterminationneeded      = ae_false;
    state->repnfev                    = 0;
    state->repterminationtype         = 0;
    state->repf                       = (double) (0);
    state->reppdgap                   = ae_maxrealnumber;
    state->repnsubproblems            = 0;
    state->repntreenodes              = 0;
    state->repnnodesbeforefeasibility = -1;
    state->repnprimalcandidates       = 0;
  }

  /*************************************************************************
  Internal initialization subroutine.
  Sets default NLC solver with default criteria.
  *************************************************************************/
  static void bbgd_initinternal(ae_int_t                       n,
                                /* Real    */ const ae_vector* x,
                                ae_int_t                       solvermode,
                                double                         diffstep,
                                bbgdstate*                     state,
                                ae_state*                      _state) {
    ae_frame  _frame_block;
    ae_int_t  i;
    ae_matrix c;
    ae_vector ct;

    ae_frame_make(_state, &_frame_block);
    memset(&c, 0, sizeof(c));
    memset(&ct, 0, sizeof(ct));
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ct, 0, DT_INT, _state, ae_true);

    state->convexityflag = 0;

    /*
     * Initialize other params
     */
    critinitdefault(&state->criteria, _state);
    state->timeout                     = 0;
    state->pdgap                       = 1.0E-6;
    state->ctol                        = 1.0E-5;
    state->n                           = n;
    state->epsx                        = 1.0E-7;
    state->epsf                        = 1.0E-7;
    state->nonrootmaxitslin            = 10;
    state->nonrootmaxitsconst          = 200;
    state->nonrootadditsforfeasibility = 5;
    state->nonrootmaxitsaboveaverage   = 0.0;
    state->nmultistarts                = 1;
    state->branchingtype               = 1;
    state->subsolveralgo               = 0;
    state->subsolvermemlen             = 0;
    state->krel                        = 1;
    state->kevalunreliable             = 1;
    state->kevalreliable               = 1;
    state->dodiving                    = bbgd_diveuntilprimal;
    state->pseudocostmu                = 0.15;
    state->pseudocostminfrac           = 0.001;
    state->pseudocostinfeaspenaly      = 25.0;
    state->nonconvexitygain            = (double) (100);
    state->diffstep                    = diffstep;
    state->userterminationneeded       = ae_false;
    state->maxsubsolvers
        = icase2(ae_is_trace_enabled("DBG.BBSYNC.ONESUBSOLVER"),
                 1,
                 4 * maxconcurrency(_state),
                 _state);
    state->softmaxnodes        = 0;
    state->hardmaxnodes        = 0;
    state->maxprimalcandidates = 0;
    state->syncinterval        = 0;
    bsetallocv(n, ae_false, &state->isintegral, _state);
    bsetallocv(n, ae_false, &state->isbinary, _state);
    bsetallocv(n, ae_false, &state->islinear, _state);
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->hasbndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->hasbndu, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->x0, n, _state);
    ae_vector_set_length(&state->xc, n, _state);
    for (i = 0; i <= n - 1; i++) {
      state->bndl.ptr.p_double[i]  = _state->v_neginf;
      state->hasbndl.ptr.p_bool[i] = ae_false;
      state->bndu.ptr.p_double[i]  = _state->v_posinf;
      state->hasbndu.ptr.p_bool[i] = ae_false;
      state->s.ptr.p_double[i]     = 1.0;
      state->x0.ptr.p_double[i]    = x->ptr.p_double[i];
      state->xc.ptr.p_double[i]    = x->ptr.p_double[i];
    }
    state->hasx0 = ae_true;

    /*
     * Objective
     */
    state->objtype = 0;

    /*
     * Constraints
     */
    state->lccnt = 0;
    state->nnlc  = 0;

    /*
     * Report fields
     */
    bbgd_clearoutputs(state, _state);

    /*
     * Other structures
     */
    hqrndseed(8543, 7455, &state->unsafeglobalrng, _state);

    /*
     * RComm
     */
    ae_vector_set_length(&state->rcommv2.ia, 2 + 1, _state);
    state->rcommv2.stage = -1;
    _rcommstate_clear_handler(&state->rcommv2);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Appends RComm-V2 request coming from the subsolver to a queue. Performs
  dimensional reduction, truncating slack variable.

  On the first call to this function State.RequestType must be zero.
  *************************************************************************/
  static void bbgd_reduceandappendrequestto(const minnlcstate* subsolver,
                                            bbgdstate*         state,
                                            ae_int_t*          requesttype,
                                            ae_int_t*          querysize,
                                            ae_int_t*          queryfuncs,
                                            ae_int_t*          queryvars,
                                            ae_int_t*          querydim,
                                            ae_int_t*          queryformulasize,
                                            /* Real    */ ae_vector* querydata,
                                            ae_state*                _state) {
    ae_int_t localrequesttype;
    ae_int_t n;

    /*
     * If our request is the first one in a queue, initialize aggregated
     * request. Otherwise, perform compatibility checks.
     */
    localrequesttype = subsolver->rcommv2.requesttype;
    ae_assert((((localrequesttype == 1 || localrequesttype == 2)
                || localrequesttype == 3)
               || localrequesttype == 4)
                  || localrequesttype == 5,
              "BBGD: subsolver sends unsupported request",
              _state);
    if (*requesttype == 0) {
      /*
       * Load query metric and perform integrity checks
       */
      *requesttype           = localrequesttype;
      state->lastrequesttype = localrequesttype;
      *querysize             = 0;
      ae_assert(subsolver->rcommv2.queryfuncs >= 1,
                "BBGD: integrity check 946245 failed",
                _state);
      ae_assert(subsolver->rcommv2.queryvars >= 1,
                "BBGD: integrity check 947246 failed",
                _state);
      ae_assert(subsolver->rcommv2.querydim == 0,
                "BBGD: integrity check 947247 failed",
                _state);
      *queryfuncs       = subsolver->rcommv2.queryfuncs;
      *queryvars        = subsolver->rcommv2.queryvars;
      *querydim         = 0;
      *queryformulasize = subsolver->rcommv2.queryformulasize;
    }
    ae_assert(*requesttype == localrequesttype,
              "BBGD: subsolvers send incompatible request types that can not "
              "be aggregated",
              _state);
    ae_assert(*queryfuncs == subsolver->rcommv2.queryfuncs,
              "BBGD: subsolvers send requests that have incompatible sizes and "
              "can not be aggregated",
              _state);
    ae_assert(*queryvars == subsolver->rcommv2.queryvars,
              "BBGD: subsolvers send requests that have incompatible sizes and "
              "can not be aggregated",
              _state);
    ae_assert(subsolver->rcommv2.querydim == 0,
              "BBGD: subsolver send request with QueryDim<>0, unexpected",
              _state);
    ae_assert((localrequesttype != 3 && localrequesttype != 5)
                  || *queryformulasize == subsolver->rcommv2.queryformulasize,
              "BBGD: subsolvers send requests that are incompatible due to "
              "different query formula sizes",
              _state);
    n = *queryvars;

    /*
     * Handle various request types
     */
    if (localrequesttype == 1) {
      /*
       * Query sparse Jacobian
       */
      rgrowv(*querysize * (*queryvars)
                 + subsolver->rcommv2.querysize * (*queryvars),
             querydata,
             _state);
      rcopyvx(subsolver->rcommv2.querysize * n,
              &subsolver->rcommv2.querydata,
              0,
              querydata,
              *querysize * (*queryvars),
              _state);
      *querysize = *querysize + subsolver->rcommv2.querysize;
      return;
    }
    if (localrequesttype == 2) {
      /*
       * Query dense Jacobian
       */
      rgrowv(*querysize * (*queryvars)
                 + subsolver->rcommv2.querysize * (*queryvars),
             querydata,
             _state);
      rcopyvx(subsolver->rcommv2.querysize * n,
              &subsolver->rcommv2.querydata,
              0,
              querydata,
              *querysize * (*queryvars),
              _state);
      *querysize = *querysize + subsolver->rcommv2.querysize;
      return;
    }
    ae_assert(
        ae_false, "ReduceAndAppendRequestTo: unsupported protocol", _state);
  }

  /*************************************************************************
  Extracts Subproblem.QuerySize replies, starting from RequestIdx-th one,
  from the aggregated reply, reformulates nonlinear objective as an additional
  constraint and extends the problem with a slack variable.
  *************************************************************************/
  static void
  bbgd_extractextendandforwardreplyto(const bbgdstate* state,
                                      ae_int_t         requesttype,
                                      ae_int_t         querysize,
                                      ae_int_t         queryfuncs,
                                      ae_int_t         queryvars,
                                      ae_int_t         querydim,
                                      ae_int_t         queryformulasize,
                                      /* Real    */ const ae_vector* replyfi,
                                      /* Real    */ const ae_vector* replydj,
                                      const sparsematrix*            replysj,
                                      ae_int_t*                      requestidx,
                                      minnlcstate*                   subsolver,
                                      ae_state*                      _state) {
    ae_int_t n;
    ae_int_t localrequesttype;
    ae_int_t fidstoffs;
    ae_int_t fisrcoffs;
    ae_int_t jacdstoffs;
    ae_int_t jacsrcoffs;

    /*
     * Compatibility checks.
     */
    localrequesttype = subsolver->rcommv2.requesttype;
    ae_assert(localrequesttype == state->lastrequesttype,
              "BBGD: integrity check 040003 failed",
              _state);
    ae_assert(subsolver->rcommv2.queryfuncs == queryfuncs,
              "BBGD: integrity check 041003 failed",
              _state);
    ae_assert(subsolver->rcommv2.queryvars == queryvars,
              "BBGD: integrity check 042003 failed",
              _state);
    ae_assert(querydim == 0, "BBGD: integrity check 043003 failed", _state);
    ae_assert(*requestidx + subsolver->rcommv2.querysize <= querysize,
              "BBGD: integrity check 044003 failed",
              _state);
    n = queryvars;

    /*
     * Handle various request types
     */
    if (state->lastrequesttype == 1) {
      /*
       * A sparse Jacobian is retrieved
       */
      fidstoffs  = 0;
      fisrcoffs  = *requestidx * queryfuncs;
      jacdstoffs = 0;
      jacsrcoffs = *requestidx * queryfuncs;
      rcopyvx(subsolver->rcommv2.querysize * queryfuncs,
              replyfi,
              fisrcoffs,
              &subsolver->rcommv2.replyfi,
              fidstoffs,
              _state);
      sparsecreatecrsfromcrsrangebuf(
          replysj,
          jacsrcoffs,
          jacsrcoffs + subsolver->rcommv2.querysize * queryfuncs,
          &subsolver->rcommv2.replysj,
          _state);
      *requestidx = *requestidx + subsolver->rcommv2.querysize;
      return;
    }
    if (state->lastrequesttype == 2) {
      /*
       * A dense Jacobian is retrieved
       */
      fidstoffs  = 0;
      fisrcoffs  = *requestidx * queryfuncs;
      jacdstoffs = 0;
      jacsrcoffs = *requestidx * queryvars * queryfuncs;
      rcopyvx(subsolver->rcommv2.querysize * queryfuncs,
              replyfi,
              fisrcoffs,
              &subsolver->rcommv2.replyfi,
              fidstoffs,
              _state);
      rcopyvx(n * subsolver->rcommv2.querysize * queryfuncs,
              replydj,
              jacsrcoffs,
              &subsolver->rcommv2.replydj,
              jacdstoffs,
              _state);
      *requestidx = *requestidx + subsolver->rcommv2.querysize;
      return;
    }
    ae_assert(ae_false,
              "ExtractExtendAndForwardReplyTo: unsupported protocol",
              _state);
  }

  /*************************************************************************
  Create a copy of subproblem with new ID (which can be equal to the original
  one, though).
  *************************************************************************/
  static void bbgd_subproblemcopy(const bbgdsubproblem* src,
                                  ae_int_t              newid,
                                  bbgdsubproblem*       dst,
                                  ae_state*             _state) {
    dst->leafid       = newid;
    dst->branchbucket = src->branchbucket;
    dst->parentfdual  = src->parentfdual;
    bcopyallocv(src->n, &src->parentlinearity, &dst->parentlinearity, _state);
    dst->branchvar = src->branchvar;
    dst->branchval = src->branchval;
    dst->n         = src->n;
    rcopyallocv(src->n, &src->x0, &dst->x0, _state);
    rcopyallocv(src->n, &src->bndl, &dst->bndl, _state);
    rcopyallocv(src->n, &src->bndu, &dst->bndu, _state);
    ae_assert(src->ncuttingplanes == 0,
              "BBGD: integrity check 346147 failed",
              _state);
    dst->ncuttingplanes    = src->ncuttingplanes;
    dst->hasprimalsolution = src->hasprimalsolution;
    dst->hasdualsolution   = src->hasdualsolution;
    if (src->hasprimalsolution) {
      rcopyallocv(src->n, &src->xprim, &dst->xprim, _state);
    }
    dst->fprim = src->fprim;
    dst->hprim = src->hprim;
    if (src->hasdualsolution) {
      rcopyallocv(src->n, &src->bestxdual, &dst->bestxdual, _state);
      rcopyallocv(src->n, &src->worstxdual, &dst->worstxdual, _state);
    }
    dst->bestfdual           = src->bestfdual;
    dst->bestfdualearlyerror = src->bestfdualearlyerror;
    dst->besthdual           = src->besthdual;
    dst->besttt              = src->besttt;
    dst->bestits             = src->bestits;
    dst->worstfdual          = src->worstfdual;
    dst->worsthdual          = src->worsthdual;
    dst->bestdualisintfeas   = src->bestdualisintfeas;
    dst->dualbound           = src->dualbound;
    dst->earlystopped        = src->earlystopped;
    dst->donotfathom         = src->donotfathom;
    bcopyallocv(
        src->n, &src->subproblemlinearity, &dst->subproblemlinearity, _state);
  }

  /*************************************************************************
  Create a copy of subproblem with new ID, in an unsolved state
  *************************************************************************/
  static void bbgd_subproblemcopyasunsolved(const bbgdsubproblem* src,
                                            ae_int_t              newid,
                                            bbgdsubproblem*       dst,
                                            ae_state*             _state) {
    bbgd_subproblemcopy(src, newid, dst, _state);
    dst->hasprimalsolution   = ae_false;
    dst->hasdualsolution     = ae_false;
    dst->bestdualisintfeas   = ae_false;
    dst->fprim               = _state->v_posinf;
    dst->hprim               = _state->v_posinf;
    dst->bestfdual           = _state->v_posinf;
    dst->bestfdualearlyerror = (double) (0);
    dst->besthdual           = _state->v_posinf;
    dst->besttt              = 0;
    dst->bestits             = 0;
    dst->worstfdual          = _state->v_posinf;
    dst->worsthdual          = _state->v_posinf;
    dst->dualbound           = _state->v_posinf;
    dst->earlystopped        = ae_false;
    dst->donotfathom         = ae_false;
    bsetallocv(src->n, ae_true, &dst->subproblemlinearity, _state);
  }

  /*************************************************************************
  "Merges in" a feasible solution, updating primal and dual points as well as
  dual bound.

  This function can be applied to two kinds of solutions:
  * fully converged solutions found to be feasible
  * partially converged solutions subject to early stopping and splitting

      X               -   solution
      NLREP           -   array[N], nonlinearity report returned by MinNLC
                          (when reports are inactive, a vector of 1's is
                          returned which is a good default value). This report
                          is ALWAYS present
      S               -   subproblem to update, must be initialized by
                          SubproblemCopyAsUnsolved() at some moment in past
      F               -   objective value
      EarlyError      -   an upper estimate of error due to early stopping
                          of an IPM solver; zero for normal convergence
      SclFeasErr      -   scaled feasibility error; musy be zero for problems
                          that stopped early due to integral variables
                          converging to non-integral values
      IsIntFeas       -   if true, solution is integer feasible. Must be zero
                          for problems that stopped early due to integral
  variables converging to non-integral values IsEarlyStopped  -   if True, the
  solution was obtained by early stopping and the dual bound is not reliable
  *************************************************************************/
  static void
  bbgd_subproblemmergeinsolution(bbgdsubproblem*                subproblem,
                                 /* Real    */ const ae_vector* x,
                                 /* Real    */ const ae_vector* nlrep,
                                 double                         f,
                                 double                         earlyerror,
                                 double                         sclfeaserr,
                                 ae_bool                        isintfeas,
                                 ae_bool                        isearlystopped,
                                 ae_int_t                       terminationtype,
                                 ae_int_t                       its,
                                 ae_state*                      _state) {
    ae_int_t i;
    double   bestworstspread;

    ae_assert(!(isintfeas && ae_fp_greater(earlyerror, (double) (0))),
              "BBGD: 491152 failed",
              _state);
    if (!subproblem->hasdualsolution
        || ae_fp_less(f - earlyerror,
                      subproblem->bestfdual
                          - subproblem->bestfdualearlyerror)) {
      rcopyallocv(subproblem->n, x, &subproblem->bestxdual, _state);
      subproblem->bestfdual           = f;
      subproblem->bestfdualearlyerror = earlyerror;
      subproblem->besthdual           = sclfeaserr;
      subproblem->besttt              = terminationtype;
      subproblem->bestits             = its;
      subproblem->bestdualisintfeas   = isintfeas;
      subproblem->earlystopped        = isearlystopped;
      subproblem->donotfathom         = ae_false;
    }
    if (!subproblem->hasdualsolution
        || ae_fp_greater(f, subproblem->worstfdual)) {
      rcopyallocv(subproblem->n, x, &subproblem->worstxdual, _state);
      subproblem->worstfdual = f;
      subproblem->worsthdual = sclfeaserr;
    }
    subproblem->hasdualsolution = ae_true;
    if (isintfeas
        && (!subproblem->hasprimalsolution
            || ae_fp_less(f, subproblem->fprim))) {
      ae_assert(
          ae_fp_eq(earlyerror, (double) (0)), "BBGD: 511154 failed", _state);
      subproblem->hasprimalsolution = ae_true;
      rcopyallocv(subproblem->n, x, &subproblem->xprim, _state);
      subproblem->fprim = f;
      subproblem->hprim = sclfeaserr;
    }
    bestworstspread
        = ae_fabs(subproblem->bestfdual - subproblem->worstfdual, _state);
    subproblem->dualbound = subproblem->bestfdual
                            - subproblem->bestfdualearlyerror
                            - bbgd_safetyfactor * bestworstspread;
    for (i = 0; i <= subproblem->n - 1; i++) {
      subproblem->subproblemlinearity.ptr.p_bool[i]
          = subproblem->subproblemlinearity.ptr.p_bool[i]
            && ae_fp_less_eq(nlrep->ptr.p_double[i],
                             bbgd_nonlinearitythreshold);
    }
  }

  /*************************************************************************
  Randomize initial point of a subproblem
  *************************************************************************/
  static void bbgd_subproblemrandomizex0(bbgdsubproblem* p,
                                         bbgdstate*      state,
                                         ae_state*       _state) {
    ae_int_t i;
    double   vs;

    ae_assert(p->n == state->n, "BBGD: integrity check 797204 failed", _state);
    vs = ae_pow((double) (2),
                0.5 * hqrndnormal(&state->unsafeglobalrng, _state),
                _state);
    for (i = 0; i <= state->n - 1; i++) {
      if (ae_isfinite(p->bndl.ptr.p_double[i], _state)
          && ae_isfinite(p->bndu.ptr.p_double[i], _state)) {
        p->x0.ptr.p_double[i]
            = p->bndl.ptr.p_double[i]
              + (p->bndu.ptr.p_double[i] - p->bndl.ptr.p_double[i])
                    * hqrnduniformr(&state->unsafeglobalrng, _state);
        p->x0.ptr.p_double[i] = ae_maxreal(
            p->x0.ptr.p_double[i], p->bndl.ptr.p_double[i], _state);
        p->x0.ptr.p_double[i] = ae_minreal(
            p->x0.ptr.p_double[i], p->bndu.ptr.p_double[i], _state);
        continue;
      }
      if (ae_isfinite(p->bndl.ptr.p_double[i], _state)) {
        p->x0.ptr.p_double[i]
            = ae_maxreal(p->x0.ptr.p_double[i], p->bndl.ptr.p_double[i], _state)
              + vs * hqrndnormal(&state->unsafeglobalrng, _state);
        p->x0.ptr.p_double[i] = ae_maxreal(
            p->x0.ptr.p_double[i], p->bndl.ptr.p_double[i], _state);
        continue;
      }
      if (ae_isfinite(p->bndu.ptr.p_double[i], _state)) {
        p->x0.ptr.p_double[i]
            = ae_minreal(p->x0.ptr.p_double[i], p->bndu.ptr.p_double[i], _state)
              + vs * hqrndnormal(&state->unsafeglobalrng, _state);
        p->x0.ptr.p_double[i] = ae_minreal(
            p->x0.ptr.p_double[i], p->bndu.ptr.p_double[i], _state);
        continue;
      }
      p->x0.ptr.p_double[i]
          = p->x0.ptr.p_double[i]
            + vs * hqrndnormal(&state->unsafeglobalrng, _state);
    }
  }

  /*************************************************************************
  Randomize initial point of a subproblem
  *************************************************************************/
  static void bbgd_subproblemappendcopytoarray(const bbgdsubproblem* p,
                                               bbgdstate*            state,
                                               ae_bool randomizeinitialpoint,
                                               ae_obj_array* a,
                                               ae_state*     _state) {
    ae_frame        _frame_block;
    bbgdsubproblem* subproblem;
    ae_smart_ptr    _subproblem;

    ae_frame_make(_state, &_frame_block);
    memset(&_subproblem, 0, sizeof(_subproblem));
    ae_smart_ptr_init(
        &_subproblem, (void**) &subproblem, ae_false, _state, ae_true);

    ae_shared_pool_retrieve(&state->sppool, &_subproblem, _state);
    bbgd_subproblemcopy(p, p->leafid, subproblem, _state);
    if (randomizeinitialpoint) {
      bbgd_subproblemrandomizex0(subproblem, state, _state);
    }
    ae_obj_array_append_transfer(a, &_subproblem, _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Decides whether subproblem can be fathomed due to:
  * infeasibility
  * primal bound
  *************************************************************************/
  static ae_bool bbgd_subproblemcanfathom(const bbgdsubproblem* subproblem,
                                          const bbgdstate*      state,
                                          ae_state*             _state) {
    ae_bool result;

    if (state->dotrace) {
      ae_trace(">> analyzing %8dP: ", (int) (subproblem->leafid));
    }
    if (!subproblem->hasdualsolution) {
      if (state->dotrace) {
        ae_trace("infeasible (err=%0.2e, tt=%0d, its=%0d), fathomed\n",
                 (double) (subproblem->besthdual),
                 (int) (subproblem->besttt),
                 (int) (subproblem->bestits));
      }
      result = ae_true;
      return result;
    }
    if (state->dotrace) {
      ae_trace("(bestfdual=%0.12e, tt=%0d, its=%0d, dualbound=%0.12e",
               (double) (subproblem->bestfdual),
               (int) (subproblem->besttt),
               (int) (subproblem->bestits),
               (double) (subproblem->dualbound));
      if (subproblem->earlystopped) {
        ae_trace(", early stopped");
      }
      ae_trace(", fprim=%0.12e)", (double) (subproblem->fprim));
    }
    if ((state->hasprimalsolution && !subproblem->donotfathom)
        && ae_fp_greater_eq(
            subproblem->dualbound,
            state->fprim
                - state->pdgap
                      * rmaxabs2(state->fprim, (double) (1), _state))) {
      if (state->dotrace) {
        ae_trace(", fathomed\n");
      }
      result = ae_true;
      return result;
    }
    if (state->dotrace) {
      ae_trace("\n");
    }
    result = ae_false;
    return result;
  }

  /*************************************************************************
  Initialize front in an undefined state. Ideally, it should be called  once
  per entire optimization session.
  *************************************************************************/
  static void bbgd_frontinitundefined(bbgdfront* front,
                                      bbgdstate* state,
                                      ae_state*  _state) {
    ae_obj_array_clear(&front->entries);
    front->frontmode       = bbgd_ftundefined;
    front->frontstatus     = bbgd_stundefined;
    front->popmostrecent   = ae_false;
    front->backtrackbudget = bbgd_backtracklimit;
    ae_shared_pool_set_seed_if_different(
        &front->entrypool,
        &state->dummyentry,
        (ae_int_t) sizeof(state->dummyentry),
        (ae_copy_constructor) _bbgdfrontentry_init_copy,
        (ae_destructor) _bbgdfrontentry_destroy,
        _state);
  }

  /*************************************************************************
  Having an initialized front, configures it to solve a root subproblem.
  *************************************************************************/
  static void bbgd_frontstartroot(bbgdfront*            front,
                                  const bbgdsubproblem* r,
                                  bbgdstate*            state,
                                  ae_state*             _state) {
    ae_frame        _frame_block;
    bbgdfrontentry* e;
    ae_smart_ptr    _e;

    ae_frame_make(_state, &_frame_block);
    memset(&_e, 0, sizeof(_e));
    ae_smart_ptr_init(&_e, (void**) &e, ae_false, _state, ae_true);

    ae_assert(state->nmultistarts >= 1,
              "BBGD: integrity check 832130 failed",
              _state);
    front->frontmode   = bbgd_ftroot;
    front->frontstatus = bbgd_stfrontreadytorun;
    front->frontsize   = 1;
    while (ae_obj_array_get_length(&front->entries) < front->frontsize) {
      ae_shared_pool_retrieve(&front->entrypool, &_e, _state);
      ae_obj_array_append_transfer(&front->entries, &_e, _state);
    }
    while (ae_obj_array_get_length(&front->entries) > front->frontsize) {
      ae_obj_array_pop_transfer(&front->entries, &_e, _state);
      ae_shared_pool_recycle(&front->entrypool, &_e, _state);
    }
    ae_obj_array_get(&front->entries, 0, &_e, _state);
    bbgd_entryprepareroot(e, front, r, state, _state);
    front->rstate.stage = -1;
    _rcommstate_clear_handler(&front->rstate);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Starts synchronous dynamic front.

  Basically, it creates an empty front that will be dynamically populated by
  FrontRun().

  This function always succeedes.
  *************************************************************************/
  static void bbgd_frontstartdynamic(bbgdfront* front,
                                     bbgdstate* sstate,
                                     ae_state*  _state) {
    ae_frame        _frame_block;
    bbgdfrontentry* e;
    ae_smart_ptr    _e;

    ae_frame_make(_state, &_frame_block);
    memset(&_e, 0, sizeof(_e));
    ae_smart_ptr_init(&_e, (void**) &e, ae_false, _state, ae_true);

    front->frontmode   = bbgd_ftdynamic;
    front->frontstatus = bbgd_stfrontreadytorun;
    front->frontsize   = 0;
    while (ae_obj_array_get_length(&front->entries) > front->frontsize) {
      ae_obj_array_pop_transfer(&front->entries, &_e, _state);
      ae_shared_pool_recycle(&front->entrypool, &_e, _state);
    }
    front->rstate.stage = -1;
    _rcommstate_clear_handler(&front->rstate);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Recomputes State.FFDual using current heap and front entries being processed.
  Works only with dynamic fronts.
  *************************************************************************/
  static void bbgd_frontrecomputedualbound(bbgdfront* front,
                                           bbgdstate* state,
                                           ae_state*  _state) {
    ae_frame        _frame_block;
    bbgdfrontentry* e;
    ae_smart_ptr    _e;
    bbgdsubproblem* p;
    ae_smart_ptr    _p;
    ae_int_t        i;
    ae_bool         hasnofathom;

    ae_frame_make(_state, &_frame_block);
    memset(&_e, 0, sizeof(_e));
    memset(&_p, 0, sizeof(_p));
    ae_smart_ptr_init(&_e, (void**) &e, ae_false, _state, ae_true);
    ae_smart_ptr_init(&_p, (void**) &p, ae_false, _state, ae_true);

    ae_assert(front->frontmode == bbgd_ftroot
                  || front->frontmode == bbgd_ftdynamic,
              "BBGD: 647012 failed",
              _state);
    hasnofathom   = ae_false;
    state->ffdual = ae_maxrealnumber;
    if (state->hasprimalsolution) {
      state->ffdual = ae_minreal(state->ffdual, state->fprim, _state);
    }
    for (i = 0; i <= front->frontsize - 1; i++) {
      ae_obj_array_get(&front->entries, i, &_e, _state);
      ae_assert(e->parentsubproblem.hasdualsolution
                    || front->frontmode == bbgd_ftroot,
                "BBGD: 775356 failed",
                _state);
      if (front->frontmode != bbgd_ftroot
          && e->parentsubproblem.hasdualsolution) {
        state->ffdual
            = ae_minreal(state->ffdual, e->parentsubproblem.dualbound, _state);
        hasnofathom = hasnofathom || e->parentsubproblem.donotfathom;
      }
    }
    if (ae_obj_array_get_length(&state->bbsubproblems) > 0) {
      bbgd_growheap(state, _state);
      ae_obj_array_get(&state->bbsubproblems, 0, &_p, _state);
      ae_assert(
          p->hasdualsolution, "BBGD: integrity check 810337 failed", _state);
      state->ffdual = ae_minreal(state->ffdual, p->dualbound, _state);
      hasnofathom   = hasnofathom || p->donotfathom;
    }
    if (ae_fp_eq(state->ffdual, ae_maxrealnumber)) {
      state->ffdual = -ae_maxrealnumber;
    }
    if (hasnofathom && state->hasprimalsolution) {
      state->ffdual
          = ae_minreal(state->ffdual,
                       state->fprim
                           - (double) 10 * state->pdgap
                                 * rmaxabs2(state->fprim, (double) (1), _state),
                       _state);
    }
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Run front
  *************************************************************************/
  static ae_bool bbgd_frontrun(bbgdfront* front,
                               bbgdstate* state,
                               ae_state*  _state) {
    ae_bool result;

    /*
     * Reverse communication preparations
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if (front->rstate.stage >= 0) {
    } else {
    }
    if (front->rstate.stage == 0) {
      goto lbl_0;
    }

    /*
     * Routine body
     */
    ae_assert(front->frontstatus == bbgd_stfrontreadytorun,
              "BBGD: integrity check 249321 failed",
              _state);
    front->frontstatus = bbgd_stfrontrunning;
  lbl_1:
    if (ae_false) {
      goto lbl_2;
    }
    if (!bbgd_frontruninternal(front, state, _state)) {
      goto lbl_2;
    }
    front->rstate.stage = 0;
    if (front->rstate.rcomm2_handler != NULL && front->rstate.requesttype != 0
        && front->rstate.requesttype <= _ALGLIB_MAX_RCOMMV2_REQUEST)
      front->rstate.rcomm2_handler(&front->rstate,
                                   front->rstate.handler_p0,
                                   front->rstate.handler_p1,
                                   front->rstate.handler_p2,
                                   front->rstate.handler_p3,
                                   _state);
    else
      goto lbl_rcomm;
  lbl_0:
    goto lbl_1;
  lbl_2:
    result = ae_false;
    return result;

    /*
     * Saving state
     */
  lbl_rcomm:
    result = ae_true;
    return result;
  }

  /*************************************************************************
  Run front (internal function), returns False when the front finished its
  job.
  *************************************************************************/
  static ae_bool bbgd_frontruninternal(bbgdfront* front,
                                       bbgdstate* state,
                                       ae_state*  _state) {
    ae_frame            _frame_block;
    bbgdfrontentry*     e;
    ae_smart_ptr        _e;
    bbgdsubproblem*     p;
    ae_smart_ptr        _p;
    bbgdfrontsubsolver* subsolver;
    ae_smart_ptr        _subsolver;
    ae_int_t            i;
    ae_int_t            j;
    ae_int_t            jobscnt;
    ae_int_t            waitingforrcommcnt;
    ae_int_t            waitingforsynccnt;
    ae_bool             bdummy;
    ae_bool             continuediving;
    ae_bool             handled;
    ae_bool             someentriessolved;
    ae_bool             result;

    ae_frame_make(_state, &_frame_block);
    memset(&_e, 0, sizeof(_e));
    memset(&_p, 0, sizeof(_p));
    memset(&_subsolver, 0, sizeof(_subsolver));
    ae_smart_ptr_init(&_e, (void**) &e, ae_false, _state, ae_true);
    ae_smart_ptr_init(&_p, (void**) &p, ae_false, _state, ae_true);
    ae_smart_ptr_init(
        &_subsolver, (void**) &subsolver, ae_false, _state, ae_true);

    result = ae_true;

    /*
     * Root front
     */
    if (front->frontmode == bbgd_ftroot) {
      /*
       * Phase 0: integrity check. At the entry the front must have single entry
       * in stReadyToRun or stWaitingForRComm state. All subsolvers, if present,
       * must also be stWaitingForRComm
       */
      ae_assert(front->frontsize == 1
                    && ae_obj_array_get_length(&state->bbsubproblems) == 0,
                "BBGD: 909109 failed",
                _state);
      ae_obj_array_get(&front->entries, 0, &_e, _state);
      waitingforrcommcnt = 0;
      waitingforsynccnt  = 0;
      for (j = 0; j <= ae_obj_array_get_length(&e->subsolvers) - 1; j++) {
        ae_obj_array_get(&e->subsolvers, j, &_subsolver, _state);
        ae_assert((subsolver->subsolverstatus == bbgd_stwaitingforrcomm
                   || subsolver->subsolverstatus == bbgd_stwaitingforsync)
                      || subsolver->subsolverstatus == bbgd_streadytorun,
                  "BBGD: 915110 failed",
                  _state);
        if (subsolver->subsolverstatus == bbgd_stwaitingforrcomm) {
          waitingforrcommcnt = waitingforrcommcnt + 1;
        }
        if (subsolver->subsolverstatus == bbgd_stwaitingforsync) {
          waitingforsynccnt = waitingforsynccnt + 1;
        }
      }
      ae_assert(((e->entrystatus == bbgd_streadytorun
                  && waitingforrcommcnt + waitingforsynccnt == 0)
                 || ((e->entrystatus == bbgd_stwaitingforrcomm
                      && waitingforrcommcnt > 0)
                     && waitingforsynccnt == 0))
                    || ((e->entrystatus == bbgd_stwaitingforsync
                         && waitingforrcommcnt == 0)
                        && waitingforsynccnt > 0),
                "BBGD: 919110 failed",
                _state);

      /*
       * Internal loop: repeat until front size at the end of the loop is
       * non-zero
       */
      do {
        /*
         * Prepare
         */
        bbgd_frontrecomputedualbound(front, state, _state);
        ae_obj_array_get(&front->entries, 0, &_e, _state);

        /*
         * Activate subsolvers until we hit MaxSubsolvers limit
         */
        while (ae_obj_array_get_length(&e->spqueue) > 0
               && ae_obj_array_get_length(&e->subsolvers) < e->maxsubsolvers) {
          ae_obj_array_pop_transfer(&e->spqueue, &_p, _state);
          ae_shared_pool_retrieve(&state->subsolverspool, &_subsolver, _state);
          bbgd_entrypreparesubsolver(
              state, front, e, p, ae_true, subsolver, _state);
          ae_obj_array_append_transfer(&e->subsolvers, &_subsolver, _state);
          ae_shared_pool_recycle(&state->sppool, &_p, _state);
        }

        /*
         * Parallel call to FrontRunKthEntry()
         */
        jobscnt = 0;
        for (j = 0; j <= ae_obj_array_get_length(&e->subsolvers) - 1; j++) {
          igrowappendv(jobscnt + 1, &front->jobs, j, _state);
          jobscnt = jobscnt + 1;
        }
        bbgd_frontparallelrunentries(front, 0, jobscnt, ae_true, state, _state);

        /*
         * Analyze solution: signal timeout, check that all entries are stSolved
         * or stWaitingForRComm or stWaitingForSync, first-phase process solved
         * entries (update global stats).
         */
        e->entrystatus = icase2(ae_obj_array_get_length(&e->spqueue) > 0,
                                bbgd_streadytorun,
                                bbgd_stsolved,
                                _state);
        j              = 0;
        while (j < ae_obj_array_get_length(&e->subsolvers)) {
          ae_obj_array_get(&e->subsolvers, j, &_subsolver, _state);
          if (subsolver->subsolverstatus == bbgd_stunbounded) {
            e->entrystatus     = bbgd_stunbounded;
            front->frontstatus = bbgd_stunbounded;
            result             = ae_false;
            ae_frame_leave(_state);
            return result;
          }
          if (subsolver->subsolverstatus == bbgd_sttimeout) {
            e->entrystatus     = icase2(e->entrystatus != bbgd_stunbounded,
                                    bbgd_sttimeout,
                                    e->entrystatus,
                                    _state);
            front->frontstatus = icase2(front->frontstatus != bbgd_stunbounded,
                                        bbgd_sttimeout,
                                        front->frontstatus,
                                        _state);
            result             = ae_false;
            ae_frame_leave(_state);
            return result;
          }
          if (subsolver->subsolverstatus == bbgd_stwaitingforrcomm) {
            ae_assert(e->entrystatus != bbgd_stwaitingforsync,
                      "BBGD: integrity check 858018 failed",
                      _state);
            e->entrystatus = bbgd_stwaitingforrcomm;
            j              = j + 1;
            continue;
          }
          if (subsolver->subsolverstatus == bbgd_stwaitingforsync) {
            ae_assert(e->entrystatus != bbgd_stwaitingforrcomm,
                      "BBGD: integrity check 858018 failed",
                      _state);
            e->entrystatus = bbgd_stwaitingforsync;
            j              = j + 1;
            continue;
          }
          ae_assert(subsolver->subsolverstatus == bbgd_stsolved,
                    "BBGD: integrity check 976115 failed",
                    _state);
          if (j != ae_obj_array_get_length(&e->subsolvers) - 1) {
            ae_obj_array_swap(&e->subsolvers,
                              j,
                              ae_obj_array_get_length(&e->subsolvers) - 1,
                              _state);
          }
          ae_obj_array_pop_transfer(&e->subsolvers, &_subsolver, _state);
          ae_shared_pool_recycle(&state->subsolverspool, &_subsolver, _state);
        }
        if ((e->entrystatus != bbgd_streadytorun
             && e->entrystatus != bbgd_stwaitingforrcomm)
            && e->entrystatus != bbgd_stwaitingforsync) {
          ae_assert(e->entrystatus == bbgd_stsolved,
                    "BBGD: integrity check 670157 failed",
                    _state);
          bbgd_entryaggregateandupdateglobalstats(e, state, _state);
          if (state->hasprimalsolution
              && state->repnnodesbeforefeasibility < 0) {
            state->repnnodesbeforefeasibility = state->repntreenodes;
          }
          bbgd_entrydecideonfathoming(e, state, _state);
        }

        /*
         * Push solutions to the heap, check stopping criteria for PDGap,
         * recompute dual bound.
         *
         * After this phase is done either:
         * a) the front is empty (in which case we repeat the loop), or
         * b) there are entries, with all of them being stWaitingForRComm or
         * stReadyToRun, in which case we exit in order for RComm request to be
         * processed by the caller
         */
        if ((e->entrystatus != bbgd_streadytorun
             && e->entrystatus != bbgd_stwaitingforrcomm)
            && e->entrystatus != bbgd_stwaitingforsync) {
          ae_assert(e->entrystatus == bbgd_stsolved,
                    "BBGD: integrity check 000116 failed",
                    _state);
          bbgd_entrypushsolution(e, state, &bdummy, _state);
          ae_obj_array_pop_transfer(&front->entries, &_e, _state);
          ae_shared_pool_recycle(&front->entrypool, &_e, _state);
          front->frontsize = front->frontsize - 1;
          bbgd_frontrecomputedualbound(front, state, _state);
          if (state->dotrace) {
            ae_trace("> root problem solved\n");
          }
          result             = ae_false;
          front->frontstatus = bbgd_stsolved;
          ae_frame_leave(_state);
          return result;
        }
        bbgd_frontrecomputedualbound(front, state, _state);
        if (state->hasprimalsolution
            && ae_fp_greater_eq(
                state->ffdual,
                state->fprim
                    - state->pdgap
                          * rmaxabs2(state->fprim, (double) (1), _state))) {
          if (state->dotrace) {
            ae_trace("> relative duality gap decreased below %0.2e, stopping\n",
                     (double) (state->pdgap));
          }
          result             = ae_false;
          front->frontstatus = bbgd_stsolved;
          ae_frame_leave(_state);
          return result;
        }
        if ((state->softmaxnodes > 0 && state->hasprimalsolution)
            && state->repntreenodes >= state->softmaxnodes) {
          if (state->dotrace) {
            ae_trace("> soft max nodes triggered (stop if have primal "
                     "solution), stopping\n");
          }
          result             = ae_false;
          front->frontstatus = bbgd_sttimeout;
          ae_frame_leave(_state);
          return result;
        }
        if (state->hardmaxnodes > 0
            && state->repntreenodes >= state->hardmaxnodes) {
          if (state->dotrace) {
            ae_trace("> hard max nodes triggered (stop independently of primal "
                     "solution status), stopping\n");
          }
          result             = ae_false;
          front->frontstatus = bbgd_sttimeout;
          ae_frame_leave(_state);
          return result;
        }
        if ((state->maxprimalcandidates > 0 && state->hasprimalsolution)
            && state->repnprimalcandidates >= state->maxprimalcandidates) {
          if (state->dotrace) {
            ae_trace("> maximum number of primal candidates tried (more than "
                     "%0d), stopping\n",
                     (int) (state->maxprimalcandidates));
          }
          result             = ae_false;
          front->frontstatus = bbgd_sttimeout;
          ae_frame_leave(_state);
          return result;
        }

        /*
         * Count entries that wait for RComm; exit if RComm is needed. Continue
         * iteration if all entries are stReadyToRun, we will generate RComm
         * requests at the next round.
         */
        waitingforrcommcnt
            = icase2(e->entrystatus == bbgd_stwaitingforrcomm, 1, 0, _state);
      } while (waitingforrcommcnt <= 0);
      ae_frame_leave(_state);
      return result;
    }

    /*
     * Dynamic front
     */
    if (front->frontmode == bbgd_ftdynamic) {
      /*
       * Phase 0: integrity check. At the entry the front must have only
       * stReadyToRun or stWaitingForRComm entries (or be empty). All
       * subsolvers, if present, must also be stWaitingForRComm
       */
      for (i = 0; i <= front->frontsize - 1; i++) {
        ae_obj_array_get(&front->entries, i, &_e, _state);
        waitingforrcommcnt = 0;
        waitingforsynccnt  = 0;
        for (j = 0; j <= ae_obj_array_get_length(&e->subsolvers) - 1; j++) {
          ae_obj_array_get(&e->subsolvers, j, &_subsolver, _state);
          ae_assert((subsolver->subsolverstatus == bbgd_stwaitingforrcomm
                     || subsolver->subsolverstatus == bbgd_stwaitingforsync)
                        || subsolver->subsolverstatus == bbgd_streadytorun,
                    "BBGD: 713006 failed",
                    _state);
          if (subsolver->subsolverstatus == bbgd_stwaitingforrcomm) {
            waitingforrcommcnt = waitingforrcommcnt + 1;
          }
          if (subsolver->subsolverstatus == bbgd_stwaitingforsync) {
            waitingforsynccnt = waitingforsynccnt + 1;
          }
        }
        ae_assert(((e->entrystatus == bbgd_streadytorun
                    && waitingforrcommcnt + waitingforsynccnt == 0)
                   || ((e->entrystatus == bbgd_stwaitingforrcomm
                        && waitingforrcommcnt > 0)
                       && waitingforsynccnt == 0))
                      || ((e->entrystatus == bbgd_stwaitingforsync
                           && waitingforrcommcnt == 0)
                          && waitingforsynccnt > 0),
                  "BBGD: 665242 failed",
                  _state);
      }

      /*
       * Internal loop: repeat until front size at the end of the loop is
       * non-zero
       */
      do {
        /*
         * Append entries from the BB heap until the front is full (or the heap
         * is empty). If the front is empty after this phase it means that we
         * are done.
         */
        while (ae_obj_array_get_length(&state->bbsubproblems) > 0
               && front->frontsize < state->bbgdgroupsize) {
          bbgd_growheapandpoptop(state, _state);
          ae_obj_array_pop_transfer(&state->bbsubproblems, &_p, _state);
          ae_assert(p->hasdualsolution,
                    "BBGD: integrity check 687259 failed",
                    _state);
          if ((state->hasprimalsolution && !p->donotfathom)
              && ae_fp_greater_eq(
                  p->dualbound,
                  state->fprim
                      - state->pdgap
                            * rmaxabs2(state->fprim, (double) (1), _state))) {
            if (state->dotrace) {
              ae_trace("> fathomed %8dP during tree search (p.bestfdual=%0.2e, "
                       "p.dual_bound=%0.2e, global.fprim=%0.2e)\n",
                       (int) (p->leafid),
                       (double) (p->bestfdual),
                       (double) (p->dualbound),
                       (double) (state->fprim));
            }
            ae_shared_pool_recycle(&state->sppool, &_p, _state);
            continue;
          }
          ae_shared_pool_retrieve(&front->entrypool, &_e, _state);
          if (!bbgd_entryprepareleafs(e, front, p, state, _state)) {
            /*
             * Looks like the subproblem we extracted does not need splitting.
             * Next one, please.
             */
            if (state->dotrace) {
              ae_trace("> subproblem %8dP does not need integral or spatial "
                       "branching, skipping\n",
                       (int) (p->leafid));
            }
            ae_shared_pool_recycle(&front->entrypool, &_e, _state);
            ae_shared_pool_recycle(&state->sppool, &_p, _state);
            continue;
          }
          ae_obj_array_append_transfer(&front->entries, &_e, _state);
          front->frontsize = front->frontsize + 1;
          ae_shared_pool_recycle(&state->sppool, &_p, _state);
        }
        bbgd_frontrecomputedualbound(front, state, _state);
        if (front->frontsize == 0) {
          if (state->dolaconictrace) {
            bbgd_tracelaconic(state, _state);
          }
          if (state->doanytrace) {
            ae_trace(
                "> B&B tree has no subproblems that can be split, stopping\n");
          }
          result             = ae_false;
          front->frontstatus = bbgd_stsolved;
          ae_frame_leave(_state);
          return result;
        }

        /*
         * Activate subsolvers in each entry until we hit MaxSubsolvers limit
         */
        for (i = 0; i <= front->frontsize - 1; i++) {
          ae_obj_array_get(&front->entries, i, &_e, _state);
          while (ae_obj_array_get_length(&e->spqueue) > 0
                 && ae_obj_array_get_length(&e->subsolvers)
                        < e->maxsubsolvers) {
            ae_obj_array_pop_transfer(&e->spqueue, &_p, _state);
            ae_shared_pool_retrieve(
                &state->subsolverspool, &_subsolver, _state);
            bbgd_entrypreparesubsolver(
                state, front, e, p, ae_false, subsolver, _state);
            ae_obj_array_append_transfer(&e->subsolvers, &_subsolver, _state);
            ae_shared_pool_recycle(&state->sppool, &_p, _state);
          }
        }

        /*
         * Parallel call to FrontRunKthEntry()
         */
        jobscnt = 0;
        for (i = 0; i <= front->frontsize - 1; i++) {
          ae_obj_array_get(&front->entries, i, &_e, _state);
          for (j = 0; j <= ae_obj_array_get_length(&e->subsolvers) - 1; j++) {
            igrowappendv(
                jobscnt + 1, &front->jobs, i + j * front->frontsize, _state);
            jobscnt = jobscnt + 1;
          }
        }
        bbgd_frontparallelrunentries(front, 0, jobscnt, ae_true, state, _state);

        /*
         * Analyze solution: signal timeout, check that all entries are stSolved
         * or stWaitingForRComm, first-phase process solved entries (update
         * global stats).
         */
        for (i = 0; i <= front->frontsize - 1; i++) {
          ae_obj_array_get(&front->entries, i, &_e, _state);
          e->entrystatus = icase2(ae_obj_array_get_length(&e->spqueue) > 0,
                                  bbgd_streadytorun,
                                  bbgd_stsolved,
                                  _state);
          j              = 0;
          while (j < ae_obj_array_get_length(&e->subsolvers)) {
            ae_obj_array_get(&e->subsolvers, j, &_subsolver, _state);
            if (subsolver->subsolverstatus == bbgd_stunbounded) {
              if (state->dolaconictrace) {
                bbgd_tracelaconic(state, _state);
              }
              e->entrystatus     = bbgd_stunbounded;
              front->frontstatus = bbgd_stunbounded;
              result             = ae_false;
              ae_frame_leave(_state);
              return result;
            }
            if (subsolver->subsolverstatus == bbgd_sttimeout) {
              if (state->dolaconictrace) {
                bbgd_tracelaconic(state, _state);
              }
              e->entrystatus = icase2(e->entrystatus != bbgd_stunbounded,
                                      bbgd_sttimeout,
                                      bbgd_stunbounded,
                                      _state);
              front->frontstatus
                  = icase2(front->frontstatus != bbgd_stunbounded,
                           bbgd_sttimeout,
                           bbgd_stunbounded,
                           _state);
              result = ae_false;
              ae_frame_leave(_state);
              return result;
            }
            if (subsolver->subsolverstatus == bbgd_stwaitingforrcomm) {
              ae_assert(e->entrystatus != bbgd_stwaitingforsync,
                        "BBGD: integrity check 071024 failed",
                        _state);
              e->entrystatus = bbgd_stwaitingforrcomm;
              j              = j + 1;
              continue;
            }
            if (subsolver->subsolverstatus == bbgd_stwaitingforsync) {
              ae_assert(e->entrystatus != bbgd_stwaitingforrcomm,
                        "BBGD: integrity check 078024 failed",
                        _state);
              e->entrystatus = bbgd_stwaitingforsync;
              j              = j + 1;
              continue;
            }
            ae_assert(subsolver->subsolverstatus == bbgd_stsolved,
                      "BBGD: integrity check 840056 failed",
                      _state);
            if (j != ae_obj_array_get_length(&e->subsolvers) - 1) {
              ae_obj_array_swap(&e->subsolvers,
                                j,
                                ae_obj_array_get_length(&e->subsolvers) - 1,
                                _state);
            }
            ae_obj_array_pop_transfer(&e->subsolvers, &_subsolver, _state);
            ae_shared_pool_recycle(&state->subsolverspool, &_subsolver, _state);
          }
          if ((e->entrystatus == bbgd_streadytorun
               || e->entrystatus == bbgd_stwaitingforrcomm)
              || e->entrystatus == bbgd_stwaitingforsync) {
            continue;
          }
          ae_assert(e->entrystatus == bbgd_stsolved,
                    "BBGD: integrity check 670157 failed",
                    _state);
          bbgd_entryaggregateandupdateglobalstats(e, state, _state);
        }
        if (state->hasprimalsolution && state->repnnodesbeforefeasibility < 0) {
          state->repnnodesbeforefeasibility = state->repntreenodes;
        }

        /*
         * Push solutions to the heap, check stopping criteria for PDGap,
         * recompute dual bound.
         *
         * After this phase is done either:
         * a) the front is empty (in which case we repeat the loop), or
         * b) there are entries, with all of them being stWaitingForRComm or
         * stReadyToRun, in which case we exit in order for RComm request to be
         * processed by the caller
         */
        i                 = 0;
        someentriessolved = ae_false;
        while (i < front->frontsize) {
          /*
           * Analyze I-th entry, skip if ready to run or waiting for RComm
           */
          ae_obj_array_get(&front->entries, i, &_e, _state);
          if ((e->entrystatus == bbgd_streadytorun
               || e->entrystatus == bbgd_stwaitingforrcomm)
              || e->entrystatus == bbgd_stwaitingforsync) {
            i = i + 1;
            continue;
          }
          ae_assert(e->entrystatus == bbgd_stsolved,
                    "BBGD: integrity check 670158 failed",
                    _state);
          someentriessolved = ae_true;

          /*
           * Process entry by either pushing both leaves to the heap or by
           * perfoming a diving (the better leaf is continued, the worse one is
           * pushed to the heap)
           */
          bbgd_entrydecideonfathoming(e, state, _state);
          continuediving = ae_false;
          handled        = ae_false;
          if (state->dodiving == bbgd_divealways) {
            continuediving = bbgd_entrytrypushanddive(e, front, state, _state);
            handled        = ae_true;
          }
          if (state->dodiving == bbgd_diveuntilprimal
              && !state->hasprimalsolution) {
            continuediving = bbgd_entrytrypushanddive(e, front, state, _state);
            handled        = ae_true;
          }
          if (!handled) {
            bbgd_entrypushsolution(e, state, &bdummy, _state);
          }
          if (!continuediving) {
            /*
             * No diving, push the solution and remove the entry
             */
            if (i != front->frontsize - 1) {
              ae_obj_array_swap(
                  &front->entries, i, front->frontsize - 1, _state);
            }
            ae_obj_array_pop_transfer(&front->entries, &_e, _state);
            front->frontsize = front->frontsize - 1;
            ae_shared_pool_recycle(&front->entrypool, &_e, _state);
          }
        }
        bbgd_frontrecomputedualbound(front, state, _state);
        if (front->frontsize == 0
            && ae_obj_array_get_length(&state->bbsubproblems) == 0) {
          if (state->dolaconictrace) {
            bbgd_tracelaconic(state, _state);
          }
          if (state->doanytrace) {
            ae_trace(
                "> B&B tree has no subproblems that can be split, stopping\n");
          }
          result             = ae_false;
          front->frontstatus = bbgd_stsolved;
          ae_frame_leave(_state);
          return result;
        }
        if (someentriessolved && state->dotrace) {
          ae_trace(">> global dual bound was recomputed as %0.12e, global "
                   "primal bound is %0.12e\n",
                   (double) (state->ffdual),
                   (double) (state->fprim));
        }
        if (state->hasprimalsolution
            && ae_fp_greater_eq(
                state->ffdual,
                state->fprim
                    - state->pdgap
                          * rmaxabs2(state->fprim, (double) (1), _state))) {
          if (state->dolaconictrace) {
            bbgd_tracelaconic(state, _state);
          }
          if (state->doanytrace) {
            ae_trace("> relative duality gap decreased below %0.2e, stopping\n",
                     (double) (state->pdgap));
          }
          result             = ae_false;
          front->frontstatus = bbgd_stsolved;
          ae_frame_leave(_state);
          return result;
        }
        if ((state->softmaxnodes > 0 && state->hasprimalsolution)
            && state->repntreenodes >= state->softmaxnodes) {
          if (state->dolaconictrace) {
            bbgd_tracelaconic(state, _state);
          }
          if (state->doanytrace) {
            ae_trace("> soft max nodes triggered (stop if have primal "
                     "solution), stopping\n");
          }
          result             = ae_false;
          front->frontstatus = bbgd_sttimeout;
          ae_frame_leave(_state);
          return result;
        }
        if (state->hardmaxnodes > 0
            && state->repntreenodes >= state->hardmaxnodes) {
          if (state->dolaconictrace) {
            bbgd_tracelaconic(state, _state);
          }
          if (state->doanytrace) {
            ae_trace("> hard max nodes triggered (stop independently of primal "
                     "solution status), stopping\n");
          }
          result             = ae_false;
          front->frontstatus = bbgd_sttimeout;
          ae_frame_leave(_state);
          return result;
        }
        if ((state->maxprimalcandidates > 0 && state->hasprimalsolution)
            && state->repnprimalcandidates >= state->maxprimalcandidates) {
          if (state->dolaconictrace) {
            bbgd_tracelaconic(state, _state);
          }
          if (state->doanytrace) {
            ae_trace("> maximum number of primal candidates tried (more than "
                     "%0d), stopping\n",
                     (int) (state->maxprimalcandidates));
          }
          result             = ae_false;
          front->frontstatus = bbgd_sttimeout;
          ae_frame_leave(_state);
          return result;
        }

        /*
         * Trace
         */
        if (state->dolaconictrace
            && ae_ifloor(stimergetmsrunning(&state->timerglobal, _state)
                             / (double) bbgd_laconicreportperiod,
                         _state)
                   > state->lastlaconicreportepoch) {
          bbgd_tracelaconic(state, _state);
          state->lastlaconicreportepoch
              = ae_ifloor(stimergetmsrunning(&state->timerglobal, _state)
                              / (double) bbgd_laconicreportperiod,
                          _state);
        }

        /*
         * Count entries that wait for RComm; exit if RComm is needed. Continue
         * iteration if all entries are stReadyToRun, we will generate RComm
         * requests at the next round.
         */
        waitingforrcommcnt = 0;
        for (i = 0; i <= front->frontsize - 1; i++) {
          ae_obj_array_get(&front->entries, i, &_e, _state);
          if (e->entrystatus == bbgd_stwaitingforrcomm) {
            waitingforrcommcnt = waitingforrcommcnt + 1;
          }
        }
      } while (waitingforrcommcnt <= 0);
      ae_frame_leave(_state);
      return result;
    }

    /*
     * Unexpected front type
     */
    ae_assert(ae_false, "BBGD: 592133 failed (unexpected front type)", _state);
    ae_frame_leave(_state);
    return result;
  }

  /*************************************************************************
  Call EntryRun in parallel.

  Job0 and Job1 denote a range [Job0,Job1) in Front.Jobs to process,
  IsRootCall must be True when called (used to distinguish recursive calls
  from root ones).
  *************************************************************************/
  static void bbgd_frontparallelrunentries(bbgdfront* front,
                                           ae_int_t   job0,
                                           ae_int_t   job1,
                                           ae_bool    isrootcall,
                                           bbgdstate* state,
                                           ae_state*  _state) {
    ae_int_t jobmid;

    ae_assert(job1 > job0, "BBGD: 551121 failed", _state);
    if (job1 - job0 == 1) {
      bbgd_frontrunkthentryjthsubsolver(
          front,
          front->jobs.ptr.p_int[job0] % front->frontsize,
          front->jobs.ptr.p_int[job0] / front->frontsize,
          state,
          _state);
      return;
    }
    if (isrootcall && !state->forceserial) {
      if (_trypexec_bbgd_frontparallelrunentries(
              front, job0, job1, isrootcall, state, _state)) {
        return;
      }
    }
    jobmid = job0 + (job1 - job0) / 2;
    bbgd_frontparallelrunentries(front, job0, jobmid, ae_false, state, _state);
    bbgd_frontparallelrunentries(front, jobmid, job1, ae_false, state, _state);
  }

  /*************************************************************************
  Serial stub for GPL edition.
  *************************************************************************/
  ae_bool _trypexec_bbgd_frontparallelrunentries(bbgdfront* front,
                                                 ae_int_t   job0,
                                                 ae_int_t   job1,
                                                 ae_bool    isrootcall,
                                                 bbgdstate* state,
                                                 ae_state*  _state) {
    return ae_false;
  }

  /*************************************************************************
  Run k-th entry of the front.
  *************************************************************************/
  static void bbgd_frontrunkthentryjthsubsolver(bbgdfront* front,
                                                ae_int_t   k,
                                                ae_int_t   j,
                                                bbgdstate* state,
                                                ae_state*  _state) {
    ae_frame            _frame_block;
    bbgdfrontentry*     e;
    ae_smart_ptr        _e;
    bbgdfrontsubsolver* s;
    ae_smart_ptr        _s;

    ae_frame_make(_state, &_frame_block);
    memset(&_e, 0, sizeof(_e));
    memset(&_s, 0, sizeof(_s));
    ae_smart_ptr_init(&_e, (void**) &e, ae_false, _state, ae_true);
    ae_smart_ptr_init(&_s, (void**) &s, ae_false, _state, ae_true);

    ae_obj_array_get(&front->entries, k, &_e, _state);
    if (front->frontmode == bbgd_ftroot || front->frontmode == bbgd_ftdynamic) {
      ae_obj_array_get(&e->subsolvers, j, &_s, _state);
      ae_assert((s->subsolverstatus == bbgd_streadytorun
                 || s->subsolverstatus == bbgd_stwaitingforrcomm)
                    || s->subsolverstatus == bbgd_stwaitingforsync,
                "BBGD: 979201 failed",
                _state);
      bbgd_subsolverrun(state, front, e, s, _state);
      ae_frame_leave(_state);
      return;
    }
    ae_assert(ae_false, "BBGD: 963109 failed", _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Pack requests coming from the front elements into the grand RComm request
  *************************************************************************/
  static void bbgd_frontpackqueries(bbgdfront*               front,
                                    bbgdstate*               state,
                                    ae_int_t*                requesttype,
                                    ae_int_t*                querysize,
                                    ae_int_t*                queryfuncs,
                                    ae_int_t*                queryvars,
                                    ae_int_t*                querydim,
                                    ae_int_t*                queryformulasize,
                                    /* Real    */ ae_vector* querydata,
                                    ae_state*                _state) {
    ae_frame            _frame_block;
    bbgdfrontentry*     e;
    ae_smart_ptr        _e;
    bbgdfrontsubsolver* subsolver;
    ae_smart_ptr        _subsolver;
    ae_int_t            i;
    ae_int_t            j;

    ae_frame_make(_state, &_frame_block);
    memset(&_e, 0, sizeof(_e));
    memset(&_subsolver, 0, sizeof(_subsolver));
    ae_smart_ptr_init(&_e, (void**) &e, ae_false, _state, ae_true);
    ae_smart_ptr_init(
        &_subsolver, (void**) &subsolver, ae_false, _state, ae_true);

    for (i = 0; i <= front->frontsize - 1; i++) {
      ae_obj_array_get(&front->entries, i, &_e, _state);
      ae_assert((((e->entrystatus == bbgd_streadytorun
                   || e->entrystatus == bbgd_stwaitingforrcomm)
                  || e->entrystatus == bbgd_stsolved)
                 || e->entrystatus == bbgd_sttimeout)
                    || e->entrystatus == bbgd_stunbounded,
                "BBGD: integrity check 304325 failed",
                _state);
      if (e->entrystatus == bbgd_stwaitingforrcomm) {
        if (front->frontmode == bbgd_ftroot
            || front->frontmode == bbgd_ftdynamic) {
          for (j = 0; j <= ae_obj_array_get_length(&e->subsolvers) - 1; j++) {
            ae_obj_array_get(&e->subsolvers, j, &_subsolver, _state);
            ae_assert(subsolver->subsolverstatus == bbgd_stwaitingforrcomm,
                      "BBGD: 021235 failed",
                      _state);
            bbgd_reduceandappendrequestto(&subsolver->nlpsubsolver,
                                          state,
                                          requesttype,
                                          querysize,
                                          queryfuncs,
                                          queryvars,
                                          querydim,
                                          queryformulasize,
                                          querydata,
                                          _state);
          }
          continue;
        }
        ae_assert(ae_false, "BBGD: 026236 failed", _state);
      }
    }
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Unpack RComm replies and distribute them to front entries
  *************************************************************************/
  static void bbgd_frontunpackreplies(bbgdstate* state,
                                      ae_int_t   requesttype,
                                      ae_int_t   querysize,
                                      ae_int_t   queryfuncs,
                                      ae_int_t   queryvars,
                                      ae_int_t   querydim,
                                      ae_int_t   queryformulasize,
                                      /* Real    */ const ae_vector* replyfi,
                                      /* Real    */ const ae_vector* replydj,
                                      const sparsematrix*            replysj,
                                      bbgdfront*                     front,
                                      ae_state*                      _state) {
    ae_frame            _frame_block;
    ae_int_t            i;
    ae_int_t            j;
    ae_int_t            offs;
    bbgdfrontentry*     e;
    ae_smart_ptr        _e;
    bbgdfrontsubsolver* subsolver;
    ae_smart_ptr        _subsolver;

    ae_frame_make(_state, &_frame_block);
    memset(&_e, 0, sizeof(_e));
    memset(&_subsolver, 0, sizeof(_subsolver));
    ae_smart_ptr_init(&_e, (void**) &e, ae_false, _state, ae_true);
    ae_smart_ptr_init(
        &_subsolver, (void**) &subsolver, ae_false, _state, ae_true);

    offs = 0;
    for (i = 0; i <= front->frontsize - 1; i++) {
      ae_obj_array_get(&front->entries, i, &_e, _state);
      ae_assert((((e->entrystatus == bbgd_streadytorun
                   || e->entrystatus == bbgd_stwaitingforrcomm)
                  || e->entrystatus == bbgd_stsolved)
                 || e->entrystatus == bbgd_sttimeout)
                    || e->entrystatus == bbgd_stunbounded,
                "BBGD: integrity check 304325 failed",
                _state);
      if (e->entrystatus == bbgd_stwaitingforrcomm) {
        if (front->frontmode == bbgd_ftroot
            || front->frontmode == bbgd_ftdynamic) {
          for (j = 0; j <= ae_obj_array_get_length(&e->subsolvers) - 1; j++) {
            ae_obj_array_get(&e->subsolvers, j, &_subsolver, _state);
            ae_assert(subsolver->subsolverstatus == bbgd_stwaitingforrcomm,
                      "BBGD: 070237 failed",
                      _state);
            bbgd_extractextendandforwardreplyto(state,
                                                requesttype,
                                                querysize,
                                                queryfuncs,
                                                queryvars,
                                                querydim,
                                                queryformulasize,
                                                replyfi,
                                                replydj,
                                                replysj,
                                                &offs,
                                                &subsolver->nlpsubsolver,
                                                _state);
          }
          continue;
        }
        ae_assert(ae_false, "BBGD: 071236 failed", _state);
      }
    }
    ae_assert(offs == querysize, "BBGD: integrity check 606236 failed", _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Prepare subsolver for the front entry. Sets timers if timeout was specified.
  *************************************************************************/
  static void bbgd_entryprepareroot(bbgdfrontentry*       entry,
                                    const bbgdfront*      front,
                                    const bbgdsubproblem* rootsubproblem,
                                    bbgdstate*            state,
                                    ae_state*             _state) {
    ae_frame        _frame_block;
    ae_int_t        restartidx;
    bbgdsubproblem* subproblem;
    ae_smart_ptr    _subproblem;

    ae_frame_make(_state, &_frame_block);
    memset(&_subproblem, 0, sizeof(_subproblem));
    ae_smart_ptr_init(
        &_subproblem, (void**) &subproblem, ae_false, _state, ae_true);

    bbgd_entrypreparex(
        entry, front, state, ae_true, rootsubproblem->leafid, _state);
    bbgd_subproblemcopyasunsolved(rootsubproblem,
                                  rootsubproblem->leafid,
                                  &entry->parentsubproblem,
                                  _state);
    bbgd_subproblemcopyasunsolved(
        rootsubproblem, rootsubproblem->leafid, &entry->rootproblem, _state);
    ae_shared_pool_retrieve(&state->sppool, &_subproblem, _state);
    bbgd_subproblemcopyasunsolved(
        &entry->rootproblem, entry->rootproblem.leafid, subproblem, _state);
    ae_obj_array_append_transfer(&entry->solutions, &_subproblem, _state);
    for (restartidx = 0; restartidx <= state->nmultistarts - 1; restartidx++) {
      ae_shared_pool_retrieve(&state->sppool, &_subproblem, _state);
      bbgd_subproblemcopyasunsolved(
          &entry->rootproblem, entry->rootproblem.leafid, subproblem, _state);
      if (restartidx > 0) {
        bbgd_subproblemrandomizex0(subproblem, state, _state);
      }
      ae_obj_array_append_transfer(&entry->spqueue, &_subproblem, _state);
    }
    entry->maxsubsolvers = ae_minint(
        ae_obj_array_get_length(&entry->spqueue), state->maxsubsolvers, _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Prepare subsolver for the front entry. Sets timers if timeout was specified.
  *************************************************************************/
  static ae_bool bbgd_entryprepareleafs(bbgdfrontentry*  entry,
                                        const bbgdfront* front,
                                        bbgdsubproblem*  s,
                                        bbgdstate*       state,
                                        ae_state*        _state) {
    ae_bool  done;
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t choiceidx;
    ae_int_t branchidx;
    double   v;
    double   vcostup;
    double   vcostdown;
    double   vscore;
    double   vmid;
    double   branchscore;
    double   maxinterr;
    ae_int_t leaf0;
    ae_int_t leaf1;
    ae_int_t restartidx;
    ae_int_t cntreliable;
    ae_int_t cntunreliable;
    ae_int_t cntchosen;
    ae_bool  isreliable;
    ae_bool  result;

    done = ae_false;
    n    = s->n;
    ae_assert(
        s->hasdualsolution, "BBGD: integrity check 391031 failed", _state);

    /*
     * Entry initialization
     */
    bbgd_entrypreparex(entry, front, state, ae_false, s->leafid, _state);
    bbgd_subproblemcopy(s, s->leafid, &entry->parentsubproblem, _state);

    /*
     * Our first attempt to split: split subproblems with significant
     * integrality errors.
     */
    if (!done && !s->bestdualisintfeas) {
      /*
       * Evaluate variables potential for branching
       */
      ae_assert((state->branchingtype == 0 || state->branchingtype == 1)
                    || state->branchingtype == 2,
                "BBGD: 167923 failed",
                _state);
      maxinterr = (double) (0);
      for (i = 0; i <= n - 1; i++) {
        if (state->isintegral.ptr.p_bool[i]) {
          v = s->bestxdual.ptr.p_double[i]
              - (double) ae_ifloor(s->bestxdual.ptr.p_double[i], _state);
          maxinterr = ae_maxreal(
              maxinterr, ae_minreal(v, (double) 1 - v, _state), _state);
        }
      }
      ae_assert(ae_fp_less_eq(bbgd_alphaint, 0.95),
                "BBGD: integrity check 943151 failed",
                _state);
      iallocv(n, &entry->tmpreliablebranchidx, _state);
      rallocv(n, &entry->tmpreliablebranchscore, _state);
      iallocv(n, &entry->tmpunreliablebranchidx, _state);
      rallocv(n, &entry->tmpunreliablebranchscore, _state);
      cntreliable   = 0;
      cntunreliable = 0;
      for (i = 0; i <= n - 1; i++) {
        /*
         * Skip non-integral variables and variables with integrality error
         * below fraction of MaxIntErr
         */
        if (!state->isintegral.ptr.p_bool[i]) {
          continue;
        }
        v = s->bestxdual.ptr.p_double[i]
            - (double) ae_ifloor(s->bestxdual.ptr.p_double[i], _state);
        if (ae_fp_less(ae_minreal(v, (double) 1 - v, _state),
                       bbgd_alphaint * maxinterr)) {
          continue;
        }

        /*
         * Evaluate variable potential, add to one of lists, deterministically
         * reshuffle lists to avoid resolving ties the same way
         */
        vscore     = ae_minreal(v, (double) 1 - v, _state);
        isreliable = ae_true;
        if (state->branchingtype == 1 || state->branchingtype == 2) {
          vcostup   = state->globalpseudocostup;
          vcostdown = state->globalpseudocostdown;
          if (state->pseudocostscntup.ptr.p_int[i] >= state->krel) {
            vcostup = state->pseudocostsup.ptr.p_double[i];
          } else {
            isreliable = ae_false;
          }
          if (state->pseudocostscntdown.ptr.p_int[i] >= state->krel) {
            vcostdown = state->pseudocostsdown.ptr.p_double[i];
          } else {
            isreliable = ae_false;
          }
          vscore = ((double) 1 - state->pseudocostmu)
                       * ae_minreal(
                           v * vcostdown, ((double) 1 - v) * vcostup, _state)
                   + state->pseudocostmu
                         * ae_maxreal(
                             v * vcostdown, ((double) 1 - v) * vcostup, _state);
        }
        ae_assert(ae_fp_greater_eq(vscore, (double) (0)),
                  "BBGD: 230640 failed",
                  _state);
        if (isreliable) {
          igrowappendv(
              cntreliable + 1, &entry->tmpreliablebranchidx, i, _state);
          rgrowappendv(
              cntreliable + 1, &entry->tmpreliablebranchscore, -vscore, _state);
          cntreliable = cntreliable + 1;
        } else {
          igrowappendv(
              cntunreliable + 1, &entry->tmpunreliablebranchidx, i, _state);
          rgrowappendv(cntunreliable + 1,
                       &entry->tmpunreliablebranchscore,
                       -vscore,
                       _state);
          cntunreliable = cntunreliable + 1;
        }
      }
      ae_assert(cntreliable + cntunreliable > 0,
                "BBGD: integrity check 982152 failed",
                _state);
      for (i = 0; i <= cntreliable - 2; i++) {
        j = i + hqrnduniformi(&entry->entryrng, cntreliable - i, _state);
        if (j != i) {
          swapelements(&entry->tmpreliablebranchscore, i, j, _state);
          swapelementsi(&entry->tmpreliablebranchidx, i, j, _state);
        }
      }
      for (i = 0; i <= cntunreliable - 2; i++) {
        j = i + hqrnduniformi(&entry->entryrng, cntunreliable - i, _state);
        if (j != i) {
          swapelements(&entry->tmpunreliablebranchscore, i, j, _state);
          swapelementsi(&entry->tmpunreliablebranchidx, i, j, _state);
        }
      }
      tagsortmiddleri(&entry->tmpreliablebranchscore,
                      &entry->tmpreliablebranchidx,
                      0,
                      cntreliable,
                      _state);
      tagsortmiddleri(&entry->tmpunreliablebranchscore,
                      &entry->tmpunreliablebranchidx,
                      0,
                      cntunreliable,
                      _state);

      /*
       * Depending on branch strategy, generate a list of candidates for the
       * evaluation
       */
      iallocv(n, &entry->tmpchosenbranchidx, _state);
      rallocv(n, &entry->tmpchosenbranchscore, _state);
      cntchosen = 0;
      if (state->branchingtype == 0 || state->branchingtype == 1) {
        if (cntreliable > 0
            && (cntchosen == 0
                || ae_fp_greater(
                    -entry->tmpreliablebranchscore.ptr.p_double[0],
                    entry->tmpchosenbranchscore.ptr.p_double[0]))) {
          entry->tmpchosenbranchidx.ptr.p_int[0]
              = entry->tmpreliablebranchidx.ptr.p_int[0];
          entry->tmpchosenbranchscore.ptr.p_double[0]
              = -entry->tmpreliablebranchscore.ptr.p_double[0];
          cntchosen = 1;
        }
        if (cntunreliable > 0
            && (cntchosen == 0
                || ae_fp_greater(
                    -entry->tmpunreliablebranchscore.ptr.p_double[0],
                    entry->tmpchosenbranchscore.ptr.p_double[0]))) {
          entry->tmpchosenbranchidx.ptr.p_int[0]
              = entry->tmpunreliablebranchidx.ptr.p_int[0];
          entry->tmpchosenbranchscore.ptr.p_double[0]
              = -entry->tmpunreliablebranchscore.ptr.p_double[0];
          cntchosen = 1;
        }
      }
      if (state->branchingtype == 2) {
        for (i = 0;
             i <= ae_minint(cntreliable, state->kevalreliable, _state) - 1;
             i++) {
          igrowappendv(cntchosen + 1,
                       &entry->tmpchosenbranchidx,
                       entry->tmpreliablebranchidx.ptr.p_int[i],
                       _state);
          rgrowappendv(cntchosen + 1,
                       &entry->tmpchosenbranchscore,
                       -entry->tmpreliablebranchscore.ptr.p_double[i],
                       _state);
          cntchosen = cntchosen + 1;
        }
        for (i = 0;
             i <= ae_minint(cntunreliable, state->kevalunreliable, _state) - 1;
             i++) {
          igrowappendv(cntchosen + 1,
                       &entry->tmpchosenbranchidx,
                       entry->tmpunreliablebranchidx.ptr.p_int[i],
                       _state);
          rgrowappendv(cntchosen + 1,
                       &entry->tmpchosenbranchscore,
                       -entry->tmpunreliablebranchscore.ptr.p_double[i],
                       _state);
          cntchosen = cntchosen + 1;
        }
      }
      ae_assert(cntchosen > 0, "BBGD: 269713 failed", _state);

      /*
       * Append new subproblems to the group
       */
      for (choiceidx = 0; choiceidx <= cntchosen - 1; choiceidx++) {
        branchidx = entry->tmpchosenbranchidx.ptr.p_int[choiceidx];
        leaf0     = weakatomicfetchadd(&state->nextleafid, 1, _state);
        leaf1     = weakatomicfetchadd(&state->nextleafid, 1, _state);
        if (state->dotrace) {
          ae_trace("> branching %8dP on var %8d:",
                   (int) (s->leafid),
                   (int) (branchidx));
        }
        bbgd_subproblemcopyasunsolved(s, leaf0, &entry->tmpsubproblem, _state);
        entry->tmpsubproblem.branchbucket = 2 * choiceidx + 0;
        entry->tmpsubproblem.parentfdual  = s->bestfdual;
        bcopyv(n,
               &s->subproblemlinearity,
               &entry->tmpsubproblem.parentlinearity,
               _state);
        entry->tmpsubproblem.branchvar = branchidx;
        entry->tmpsubproblem.branchval = s->bestxdual.ptr.p_double[branchidx];
        rcopyv(n, &s->bestxdual, &entry->tmpsubproblem.x0, _state);
        entry->tmpsubproblem.x0.ptr.p_double[branchidx] = (double) (ae_ifloor(
            s->bestxdual.ptr.p_double[branchidx], _state));
        entry->tmpsubproblem.bndu.ptr.p_double[branchidx]
            = entry->tmpsubproblem.x0.ptr.p_double[branchidx];
        bbgd_subproblemappendcopytoarray(
            &entry->tmpsubproblem, state, ae_false, &entry->solutions, _state);
        for (restartidx = 0; restartidx <= state->nmultistarts - 1;
             restartidx++) {
          bbgd_subproblemappendcopytoarray(&entry->tmpsubproblem,
                                           state,
                                           restartidx > 0,
                                           &entry->spqueue,
                                           _state);
        }
        if (state->dotrace) {
          ae_trace(
              " creating %8dP (x<=%0.2e)",
              (int) (entry->tmpsubproblem.leafid),
              (double) (entry->tmpsubproblem.bndu.ptr.p_double[branchidx]));
        }
        bbgd_subproblemcopyasunsolved(s, leaf1, &entry->tmpsubproblem, _state);
        entry->tmpsubproblem.branchbucket = 2 * choiceidx + 1;
        entry->tmpsubproblem.parentfdual  = s->bestfdual;
        bcopyv(n,
               &s->subproblemlinearity,
               &entry->tmpsubproblem.parentlinearity,
               _state);
        entry->tmpsubproblem.branchvar = branchidx;
        entry->tmpsubproblem.branchval = s->bestxdual.ptr.p_double[branchidx];
        rcopyv(n, &s->bestxdual, &entry->tmpsubproblem.x0, _state);
        entry->tmpsubproblem.x0.ptr.p_double[branchidx]
            = (double) (ae_iceil(s->bestxdual.ptr.p_double[branchidx], _state));
        entry->tmpsubproblem.bndl.ptr.p_double[branchidx]
            = entry->tmpsubproblem.x0.ptr.p_double[branchidx];
        bbgd_subproblemappendcopytoarray(
            &entry->tmpsubproblem, state, ae_false, &entry->solutions, _state);
        for (restartidx = 0; restartidx <= state->nmultistarts - 1;
             restartidx++) {
          bbgd_subproblemappendcopytoarray(&entry->tmpsubproblem,
                                           state,
                                           restartidx > 0,
                                           &entry->spqueue,
                                           _state);
        }
        if (state->dotrace) {
          ae_trace(
              " and %8dP (x>=%0.2e)",
              (int) (entry->tmpsubproblem.leafid),
              (double) (entry->tmpsubproblem.bndl.ptr.p_double[branchidx]));
        }
        if (state->dotrace) {
          ae_trace("\n");
        }
      }
      entry->maxsubsolvers = ae_minint(ae_obj_array_get_length(&entry->spqueue),
                                       state->maxsubsolvers,
                                       _state);

      /*
       * Splitting on integer variable is done.
       */
      done = ae_true;
    }

    /*
     * Another option spatial splitting between worst and dual solutions (used
     * for non-convex problems with multistarts)
     */
    if ((!done && s->hasdualsolution)
        && ae_fp_greater(ae_fabs(s->bestfdual - s->worstfdual, _state),
                         state->pdgap
                             * rmaxabs2(state->ffdual, (double) (1), _state))) {
      /*
       * Select variable with the highest distance between worst and dual
       * solutions
       */
      branchidx   = -1;
      branchscore = (double) (0);
      vmid        = (double) (0);
      for (i = 0; i <= n - 1; i++) {
        vscore = ae_fabs(s->bestxdual.ptr.p_double[i]
                             - s->worstxdual.ptr.p_double[i],
                         _state)
                 / state->s.ptr.p_double[i];
        if (branchidx < 0 || ae_fp_greater(vscore, branchscore)) {
          branchidx   = i;
          branchscore = vscore;
          vmid        = 0.5
                 * (s->bestxdual.ptr.p_double[i]
                    + s->worstxdual.ptr.p_double[i]);
        }
      }
      ae_assert(branchidx >= 0, "BBGD: integrity check 137104 failed", _state);
      if (ae_fp_greater(branchscore, (double) (0))) {
        /*
         * Append new subproblems to the group
         */
        if (state->dotrace) {
          ae_trace(
              "> spatially branching %8dP on var %8d (|x_worst-x_dual|=%0.2e):",
              (int) (s->leafid),
              (int) (branchidx),
              (double) (ae_fabs(s->bestxdual.ptr.p_double[branchidx]
                                    - s->worstxdual.ptr.p_double[branchidx],
                                _state)));
        }
        leaf0 = weakatomicfetchadd(&state->nextleafid, 1, _state);
        leaf1 = weakatomicfetchadd(&state->nextleafid, 1, _state);
        bbgd_subproblemcopyasunsolved(s, leaf0, &entry->tmpsubproblem, _state);
        entry->tmpsubproblem.branchbucket = 0;
        entry->tmpsubproblem.parentfdual  = s->bestfdual;
        bcopyv(n,
               &s->subproblemlinearity,
               &entry->tmpsubproblem.parentlinearity,
               _state);
        entry->tmpsubproblem.branchvar = branchidx;
        entry->tmpsubproblem.branchval = vmid;
        if (ae_fp_less_eq(s->bestxdual.ptr.p_double[branchidx], vmid)) {
          rcopyv(n, &s->bestxdual, &entry->tmpsubproblem.x0, _state);
        } else {
          rcopyv(n, &s->worstxdual, &entry->tmpsubproblem.x0, _state);
        }
        entry->tmpsubproblem.bndu.ptr.p_double[branchidx]
            = rcase2(state->isintegral.ptr.p_bool[i],
                     (double) (ae_ifloor(vmid, _state)),
                     vmid,
                     _state);
        bbgd_subproblemappendcopytoarray(
            &entry->tmpsubproblem, state, ae_false, &entry->solutions, _state);
        for (restartidx = 0; restartidx <= state->nmultistarts - 1;
             restartidx++) {
          bbgd_subproblemappendcopytoarray(&entry->tmpsubproblem,
                                           state,
                                           restartidx > 0,
                                           &entry->spqueue,
                                           _state);
        }
        if (state->dotrace) {
          ae_trace(
              " creating %8dP (x<=%0.2e)",
              (int) (entry->tmpsubproblem.leafid),
              (double) (entry->tmpsubproblem.bndu.ptr.p_double[branchidx]));
        }
        bbgd_subproblemcopyasunsolved(s, leaf1, &entry->tmpsubproblem, _state);
        entry->tmpsubproblem.branchbucket = 1;
        entry->tmpsubproblem.parentfdual  = s->bestfdual;
        bcopyv(n,
               &s->subproblemlinearity,
               &entry->tmpsubproblem.parentlinearity,
               _state);
        entry->tmpsubproblem.branchvar = branchidx;
        entry->tmpsubproblem.branchval = vmid;
        if (ae_fp_greater_eq(s->bestxdual.ptr.p_double[branchidx], vmid)) {
          rcopyv(n, &s->bestxdual, &entry->tmpsubproblem.x0, _state);
        } else {
          rcopyv(n, &s->worstxdual, &entry->tmpsubproblem.x0, _state);
        }
        entry->tmpsubproblem.bndl.ptr.p_double[branchidx]
            = rcase2(state->isintegral.ptr.p_bool[i],
                     (double) (ae_ifloor(vmid, _state) + 1),
                     vmid,
                     _state);
        bbgd_subproblemappendcopytoarray(
            &entry->tmpsubproblem, state, ae_false, &entry->solutions, _state);
        for (restartidx = 0; restartidx <= state->nmultistarts - 1;
             restartidx++) {
          bbgd_subproblemappendcopytoarray(&entry->tmpsubproblem,
                                           state,
                                           restartidx > 0,
                                           &entry->spqueue,
                                           _state);
        }
        if (state->dotrace) {
          ae_trace(
              " and %8dP (x>=%0.2e)\n",
              (int) (entry->tmpsubproblem.leafid),
              (double) (entry->tmpsubproblem.bndl.ptr.p_double[branchidx]));
        }
        entry->maxsubsolvers
            = ae_minint(ae_obj_array_get_length(&entry->spqueue),
                        state->maxsubsolvers,
                        _state);

        /*
         * Splitting on integer variable is done.
         */
        done = ae_true;
      }
    }

    /*
     * Done or not done
     */
    result = done;
    return result;
  }

  /*************************************************************************
  Prepares the front entry generating ready to run instance with empty
  subproblems queue, empty solutions list, and maxsubsolvers=1.

  The caller must generate subproblems as necessary for that node type, set
  up initial state of the solutions list and set maxsubsolvers as it sees
  fit.

  Sets timers if timeout was specified.

  Uses subproblem ID to deterministically seed entry-local RNG
  *************************************************************************/
  static void bbgd_entrypreparex(bbgdfrontentry*  entry,
                                 const bbgdfront* front,
                                 bbgdstate*       state,
                                 ae_bool          isroot,
                                 ae_int_t         subproblemid,
                                 ae_state*        _state) {
    ae_frame            _frame_block;
    bbgdsubproblem*     subproblem;
    ae_smart_ptr        _subproblem;
    bbgdfrontsubsolver* subsolver;
    ae_smart_ptr        _subsolver;

    ae_frame_make(_state, &_frame_block);
    memset(&_subproblem, 0, sizeof(_subproblem));
    memset(&_subsolver, 0, sizeof(_subsolver));
    ae_smart_ptr_init(
        &_subproblem, (void**) &subproblem, ae_false, _state, ae_true);
    ae_smart_ptr_init(
        &_subsolver, (void**) &subsolver, ae_false, _state, ae_true);

    ae_assert(front->frontmode == bbgd_ftroot
                  || front->frontmode == bbgd_ftdynamic,
              "BBGD: 776046 failed",
              _state);
    entry->isrootentry                  = isroot;
    entry->entrystatus                  = bbgd_streadytorun;
    entry->addstatussolutionsaggregated = ae_false;
    entry->addstatusdecisionsmade       = ae_false;
    entry->fathomroot                   = ae_true;
    entry->fathomchild0                 = ae_true;
    entry->fathomchild1                 = ae_true;
    entry->entrynfev                    = 0;
    entry->entrynsubproblems            = 0;
    ae_weak_store_release(&entry->entrylock, 0);
    entry->hastimeout = state->timeout > 0;
    if (entry->hastimeout) {
      entry->timeout
          = ae_round((double) state->timeout
                         - stimergetmsrunning(&state->timerglobal, _state),
                     _state);
      stimerinit(&entry->timerlocal, _state);
      stimerstart(&entry->timerlocal, _state);
    }
    hqrndseed(
        1 + subproblemid, 1 + 17 * subproblemid, &entry->entryrng, _state);

    /*
     * Generate subproblem queue
     */
    while (ae_obj_array_get_length(&entry->spqueue) > 0) {
      ae_obj_array_pop_transfer(&entry->spqueue, &_subproblem, _state);
      ae_shared_pool_recycle(&state->sppool, &_subproblem, _state);
    }
    entry->maxsubsolvers = 1;

    /*
     * Clear subsolver list
     */
    while (ae_obj_array_get_length(&entry->subsolvers) > 0) {
      ae_obj_array_pop_transfer(&entry->subsolvers, &_subsolver, _state);
      ae_shared_pool_recycle(&state->subsolverspool, &_subsolver, _state);
    }

    /*
     * Clear solutions list
     */
    while (ae_obj_array_get_length(&entry->solutions) > 0) {
      ae_obj_array_pop_transfer(&entry->solutions, &_subproblem, _state);
      ae_shared_pool_recycle(&state->sppool, &_subproblem, _state);
    }
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Prepare subsolver for the front entry and specific subproblem to solve.
  *************************************************************************/
  static void bbgd_entrypreparesubsolver(bbgdstate*            state,
                                         bbgdfront*            front,
                                         bbgdfrontentry*       entry,
                                         const bbgdsubproblem* subproblem,
                                         ae_bool               isroot,
                                         bbgdfrontsubsolver*   subsolver,
                                         ae_state*             _state) {
    ae_assert((front->frontmode == bbgd_ftroot && isroot)
                  || (front->frontmode == bbgd_ftdynamic && !isroot),
              "BBGD: 415212 failed",
              _state);
    ae_vector_set_length(&subsolver->rstate.ia, 2 + 1, _state);
    ae_vector_set_length(&subsolver->rstate.ba, 2 + 1, _state);
    subsolver->rstate.stage = -1;
    _rcommstate_clear_handler(&subsolver->rstate);
    bbgd_subproblemcopy(
        subproblem, subproblem->leafid, &subsolver->subproblem, _state);
    subsolver->subsolverstatus = bbgd_streadytorun;
  }

  /*************************************************************************
  Run subproblem solver
  *************************************************************************/
  static ae_bool bbgd_subsolverrun(bbgdstate*          state,
                                   bbgdfront*          front,
                                   bbgdfrontentry*     entry,
                                   bbgdfrontsubsolver* subsolver,
                                   ae_state*           _state) {
    ae_int_t i;
    ae_int_t terminationtype;
    ae_int_t repnfevinternal;
    ae_bool  uselock;
    ae_bool  forbidrecognizingintegrality;
    ae_bool  done;
    ae_bool  result;

    /*
     * Reverse communication preparations
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if (subsolver->rstate.stage >= 0) {
      i                            = subsolver->rstate.ia.ptr.p_int[0];
      terminationtype              = subsolver->rstate.ia.ptr.p_int[1];
      repnfevinternal              = subsolver->rstate.ia.ptr.p_int[2];
      uselock                      = subsolver->rstate.ba.ptr.p_bool[0];
      forbidrecognizingintegrality = subsolver->rstate.ba.ptr.p_bool[1];
      done                         = subsolver->rstate.ba.ptr.p_bool[2];
    } else {
      i                            = -909;
      terminationtype              = 81;
      repnfevinternal              = 255;
      uselock                      = ae_false;
      forbidrecognizingintegrality = ae_false;
      done                         = ae_true;
    }
    if (subsolver->rstate.stage == 0) {
      goto lbl_0;
    }
    if (subsolver->rstate.stage == 1) {
      goto lbl_1;
    }

    /*
     * Routine body
     */

    /*
     * Init
     */
    repnfevinternal              = 0;
    uselock                      = ae_true;
    forbidrecognizingintegrality = ae_true;
    ae_assert(subsolver->subsolverstatus == bbgd_streadytorun
                  && ((front->frontmode == bbgd_ftdynamic
                       && subsolver->subproblem.branchbucket >= 0)
                      || (front->frontmode == bbgd_ftroot
                          && subsolver->subproblem.branchbucket == -1)),
              "BBGD: integrity check 589220 failed",
              _state);

    /*
     * Handle various objective types
     */
    done = ae_false;
    if (state->objtype == 1 && state->nnlc == 0) {
      bbgd_solveqpnode(entry,
                       subsolver,
                       state,
                       &subsolver->subproblem.x0,
                       &subsolver->subproblem.bndl,
                       &subsolver->subproblem.bndu,
                       &entry->solutions,
                       ae_maxint(subsolver->subproblem.branchbucket, 0, _state),
                       uselock,
                       _state);
      weakatomicfetchadd(&state->repnsubproblems, 1, _state);
      if (entry->hastimeout
          && ae_fp_greater(stimergetmsrunning(&entry->timerlocal, _state),
                           (double) (entry->timeout))) {
        subsolver->subsolverstatus = bbgd_sttimeout;
        result                     = ae_false;
        return result;
      }
      subsolver->subsolverstatus = bbgd_stsolved;
      done                       = ae_true;
    }
    if (done) {
      goto lbl_2;
    }

    /*
     * Generic NLP subproblem is solved.
     */
    minnlccreatebuf(subsolver->subproblem.n,
                    &subsolver->subproblem.x0,
                    &subsolver->nlpsubsolver,
                    _state);
    minnlcsetscale(&subsolver->nlpsubsolver, &state->s, _state);
    minnlcsetbc(&subsolver->nlpsubsolver,
                &subsolver->subproblem.bndl,
                &subsolver->subproblem.bndu,
                _state);
    minnlcsetlc2(&subsolver->nlpsubsolver,
                 &state->rawa,
                 &state->rawal,
                 &state->rawau,
                 state->lccnt,
                 _state);
    minnlcsetnlc2(
        &subsolver->nlpsubsolver, &state->nl, &state->nu, state->nnlc, _state);
    for (i = 0; i <= state->n - 1; i++) {
      if (state->islinear.ptr.p_bool[i]
          || subsolver->subproblem.parentlinearity.ptr.p_bool[i]) {
        minnlcmarkaslinearvar(&subsolver->nlpsubsolver, i, _state);
      }
    }
    if (state->issuesparserequests) {
      minnlcsetprotocolv2s(&subsolver->nlpsubsolver, _state);
    } else {
      minnlcsetprotocolv2(&subsolver->nlpsubsolver, _state);
    }
    if (subsolver->subproblem.branchbucket >= 0) {
      i = state->nonrootmaxitsconst
          + state->nonrootmaxitslin * subsolver->subproblem.n;
      if (state->globalsynchronizednsubproblems > 0) {
        i = ae_maxint(
            i,
            ae_round(state->nonrootmaxitsaboveaverage
                         * (double) state->globalsynchronizednfev
                         / (double) state->globalsynchronizednsubproblems,
                     _state),
            _state);
      }
      minnlcsetcond3(
          &subsolver->nlpsubsolver, state->epsf, state->epsx, i, _state);
      minnlcsetfsqpadditsforctol(&subsolver->nlpsubsolver,
                                 state->nonrootadditsforfeasibility,
                                 state->ctol,
                                 _state);
    }
    minnlcsetearlystopping(&subsolver->nlpsubsolver,
                           &state->isintegral,
                           subsolver->subproblem.parentfdual,
                           state->fprim,
                           100 + 5 * state->n,
                           1.0E-4,
                           _state);
    if (state->subsolveralgo == 0) {
      minnlcsetalgonlpipm(
          &subsolver->nlpsubsolver, state->subsolvermemlen, _state);
    }
    if (state->subsolveralgo == 1) {
      minnlcsetalgosqp(&subsolver->nlpsubsolver, _state);
    }
    minnlcsetpowerfulpresolvernomults(&subsolver->nlpsubsolver, _state);
    minnlcsetnonlinearityreports(&subsolver->nlpsubsolver, ae_true, _state);

    /*
     * Solve NLP relaxation
     */
  lbl_4:
    if (!minnlciteration(&subsolver->nlpsubsolver, _state)) {
      goto lbl_5;
    }

    /*
     * Handle RComm request
     */
    if (subsolver->nlpsubsolver.rcommv2.requesttype == -1) {
      goto lbl_6;
    }
    if (!state->usehandlersandsync) {
      goto lbl_8;
    }

    /*
     * Handle RComm request with handler
     */
    if (!_rcommstate_apply_handler_to(
            &state->rcommv2, &subsolver->nlpsubsolver.rcommv2, _state)) {
      ae_assert(ae_false, "BBSYNC: 784245 failed", _state);
    }
    repnfevinternal = repnfevinternal + 1;
    if (!(state->syncinterval > 0
          && repnfevinternal % state->syncinterval == 0)) {
      goto lbl_10;
    }
    subsolver->subsolverstatus = bbgd_stwaitingforsync;
    subsolver->rstate.stage    = 0;
    if (subsolver->rstate.rcomm2_handler != NULL
        && subsolver->rstate.requesttype != 0
        && subsolver->rstate.requesttype <= _ALGLIB_MAX_RCOMMV2_REQUEST)
      subsolver->rstate.rcomm2_handler(&subsolver->rstate,
                                       subsolver->rstate.handler_p0,
                                       subsolver->rstate.handler_p1,
                                       subsolver->rstate.handler_p2,
                                       subsolver->rstate.handler_p3,
                                       _state);
    else
      goto lbl_rcomm;
  lbl_0:
  lbl_10:
    goto lbl_9;
  lbl_8:

    /*
     * Forward RComm request to parent
     */
    subsolver->subsolverstatus = bbgd_stwaitingforrcomm;
    subsolver->rstate.stage    = 1;
    if (subsolver->rstate.rcomm2_handler != NULL
        && subsolver->rstate.requesttype != 0
        && subsolver->rstate.requesttype <= _ALGLIB_MAX_RCOMMV2_REQUEST)
      subsolver->rstate.rcomm2_handler(&subsolver->rstate,
                                       subsolver->rstate.handler_p0,
                                       subsolver->rstate.handler_p1,
                                       subsolver->rstate.handler_p2,
                                       subsolver->rstate.handler_p3,
                                       _state);
    else
      goto lbl_rcomm;
  lbl_1:
  lbl_9:
  lbl_6:

    /*
     * Check for timeout
     */
    if (entry->hastimeout
        && ae_fp_greater(stimergetmsrunning(&entry->timerlocal, _state),
                         (double) (entry->timeout))) {
      weakatomicfetchadd(&state->repnfev, repnfevinternal, _state);
      weakatomicfetchadd(&state->repnsubproblems, 1, _state);
      subsolver->subsolverstatus = bbgd_sttimeout;
      result                     = ae_false;
      return result;
    }
    goto lbl_4;
  lbl_5:
    minnlcresultsbuf(
        &subsolver->nlpsubsolver, &subsolver->xsol, &subsolver->nlprep, _state);
    weakatomicfetchadd(&state->repnfev, repnfevinternal, _state);
    weakatomicfetchadd(&state->repnsubproblems, 1, _state);
    weakatomicfetchadd(&entry->entrynfev, repnfevinternal, _state);
    weakatomicfetchadd(&entry->entrynsubproblems, 1, _state);

    /*
     * Analyze solution
     */
    ae_assert(!done, "BBGD: 010612 failed", _state);
    if (ae_fp_less_eq(subsolver->nlprep.f, bbgd_unboundedf)) {
      subsolver->nlprep.terminationtype = -4;
    }
    if (subsolver->nlprep.terminationtype == -4) {
      subsolver->subsolverstatus = bbgd_stunbounded;
      done                       = ae_true;
    } else {
      /*
       * Analyze bounded solution
       */
      bbgd_analyzenlpsolutionandenforceintegrality(
          entry,
          &subsolver->xsol,
          &subsolver->nlpsubsolver.repnonlinearityreport,
          !forbidrecognizingintegrality,
          &subsolver->nlprep,
          subsolver->nlpsubsolver.repearlyerrorestimate,
          state,
          &entry->solutions,
          ae_maxint(subsolver->subproblem.branchbucket, 0, _state),
          uselock,
          _state);
      subsolver->subsolverstatus = bbgd_stsolved;
      done                       = ae_true;
    }
  lbl_2:
    ae_assert(done, "BBGD: integrity check 659230 failed", _state);
    result = ae_false;
    return result;

    /*
     * Saving state
     */
  lbl_rcomm:
    result                             = ae_true;
    subsolver->rstate.ia.ptr.p_int[0]  = i;
    subsolver->rstate.ia.ptr.p_int[1]  = terminationtype;
    subsolver->rstate.ia.ptr.p_int[2]  = repnfevinternal;
    subsolver->rstate.ba.ptr.p_bool[0] = uselock;
    subsolver->rstate.ba.ptr.p_bool[1] = forbidrecognizingintegrality;
    subsolver->rstate.ba.ptr.p_bool[2] = done;
    return result;
  }

  /*************************************************************************
  Aggregates data from the Solutions[] array into RootProblem or
  ChildSubproblem0/ChildSubproblem1, depending on entry type. Updates global
  statistics (pseudocosts etc).


  Uses entry data to update global statistics in State:
  * primal bound
  * pseudocosts

  This function must be called on all solved entries, prior to calling
  EntryDecideOnFathoming() for all solved entries.

  update should be performed prior to feeding data from the entry  into
  State with EntryPushSolution() or EntryTryPushAndDive().
  *************************************************************************/
  static void bbgd_entryaggregateandupdateglobalstats(bbgdfrontentry* entry,
                                                      bbgdstate*      state,
                                                      ae_state*       _state) {
    ae_frame        _frame_block;
    ae_int_t        i;
    ae_int_t        k;
    ae_int_t        solcnt;
    double          v;
    double          vfrac;
    double          vup;
    double          vdown;
    ae_int_t        cntsimplebranch;
    ae_int_t        cnttighten;
    ae_int_t        infeassolidx;
    ae_int_t        bestbranch;
    ae_int_t        besttighten;
    ae_int_t        bestbranchsolidx;
    ae_int_t        besttightensolidx;
    double          bestbranchscore;
    double          besttightenlowerscore;
    ae_bool         isinfeasible;
    double          vscore;
    double          vrnddn;
    double          vrndup;
    bbgdsubproblem* sol;
    ae_smart_ptr    _sol;
    bbgdsubproblem* sol0;
    ae_smart_ptr    _sol0;
    bbgdsubproblem* sol1;
    ae_smart_ptr    _sol1;

    ae_frame_make(_state, &_frame_block);
    memset(&_sol, 0, sizeof(_sol));
    memset(&_sol0, 0, sizeof(_sol0));
    memset(&_sol1, 0, sizeof(_sol1));
    ae_smart_ptr_init(&_sol, (void**) &sol, ae_false, _state, ae_true);
    ae_smart_ptr_init(&_sol0, (void**) &sol0, ae_false, _state, ae_true);
    ae_smart_ptr_init(&_sol1, (void**) &sol1, ae_false, _state, ae_true);

    ae_assert(entry->entrystatus == bbgd_stsolved
                  && !entry->addstatussolutionsaggregated,
              "BBGD: integrity check 828957 failed",
              _state);
    solcnt = ae_obj_array_get_length(&entry->solutions);

    /*
     * Update primal bound
     */
    for (i = 0; i <= solcnt - 1; i++) {
      ae_obj_array_get(&entry->solutions, i, &_sol, _state);
      if (sol->hasprimalsolution) {
        ae_assert(
            sol->n == state->n, "BBGD: integrity check 832958 failed", _state);
        if (!state->hasprimalsolution || ae_fp_less(sol->fprim, state->fprim)) {
          rcopyallocv(sol->n, &sol->xprim, &state->xprim, _state);
          state->fprim             = sol->fprim;
          state->hprim             = sol->hprim;
          state->hasprimalsolution = ae_true;
        }
        state->repnprimalcandidates = state->repnprimalcandidates + 1;
      }
    }

    /*
     * Update pseudocosts:
     * * per-variable ones are always updated, for feasible and infeasible
     * problems (in the latter case the global cost times penalty is used)
     * * global pseudocosts are updated only when subproblem is feasible (to
     * avoid updating them with their own value times penalty)
     */
    if (!entry->isrootentry) {
      for (i = 0; i <= solcnt - 1; i++) {
        ae_obj_array_get(&entry->solutions, i, &_sol, _state);
        if (state->isintegral.ptr.p_bool[sol->branchvar]) {
          ae_assert(sol->branchbucket >= 0 && sol->branchbucket < solcnt,
                    "BBGD: 546152 failed",
                    _state);
          if (sol->branchbucket % 2 == 0) {
            k     = sol->branchvar;
            v     = sol->branchval;
            vfrac = v - (double) ae_ifloor(v, _state);
            vdown = (double) (0);
            if (sol->hasdualsolution) {
              vdown = ae_maxreal(sol->bestfdual - sol->parentfdual
                                     - sol->bestfdualearlyerror
                                     - state->epsf
                                           * rmaxabs3(sol->bestfdual,
                                                      sol->parentfdual,
                                                      (double) (1),
                                                      _state),
                                 (double) (0),
                                 _state)
                      / ae_maxreal(vfrac, ae_machineepsilon, _state);
            }
            if (!sol->hasdualsolution && state->globalpseudocostcntdown > 0) {
              vdown
                  = state->pseudocostinfeaspenaly * state->globalpseudocostdown;
            }
            if (ae_fp_greater_eq(vdown, (double) (0))
                && ae_fp_greater(vfrac, state->pseudocostminfrac)) {
              state->pseudocostsdown.ptr.p_double[k]
                  = (state->pseudocostsdown.ptr.p_double[k]
                         * (double) state->pseudocostscntdown.ptr.p_int[k]
                     + vdown)
                    / (double) (state->pseudocostscntdown.ptr.p_int[k] + 1);
              state->pseudocostscntdown.ptr.p_int[k]
                  = state->pseudocostscntdown.ptr.p_int[k] + 1;
            }
            if ((ae_fp_greater_eq(vdown, (double) (0))
                 && ae_fp_greater(vfrac, state->pseudocostminfrac))
                && sol->hasdualsolution) {
              state->globalpseudocostdown
                  = (state->globalpseudocostdown
                         * (double) state->globalpseudocostcntdown
                     + vdown)
                    / (double) (state->globalpseudocostcntdown + 1);
              state->globalpseudocostcntdown
                  = state->globalpseudocostcntdown + 1;
            }
          }
          if (sol->branchbucket % 2 == 1) {
            k     = sol->branchvar;
            v     = sol->branchval;
            vfrac = (double) ae_iceil(v, _state) - v;
            vup   = (double) (0);
            if (sol->hasdualsolution) {
              vup = ae_maxreal(sol->bestfdual - sol->parentfdual
                                   - sol->bestfdualearlyerror
                                   - state->epsf
                                         * rmaxabs3(sol->bestfdual,
                                                    sol->parentfdual,
                                                    (double) (1),
                                                    _state),
                               (double) (0),
                               _state)
                    / ae_maxreal(vfrac, ae_machineepsilon, _state);
            }
            if (!sol->hasdualsolution && state->globalpseudocostcntup > 0) {
              vup = state->pseudocostinfeaspenaly * state->globalpseudocostup;
            }
            if (ae_fp_greater_eq(vup, (double) (0))
                && ae_fp_greater(vfrac, state->pseudocostminfrac)) {
              state->pseudocostsup.ptr.p_double[k]
                  = (state->pseudocostsup.ptr.p_double[k]
                         * (double) state->pseudocostscntup.ptr.p_int[k]
                     + vup)
                    / (double) (state->pseudocostscntup.ptr.p_int[k] + 1);
              state->pseudocostscntup.ptr.p_int[k]
                  = state->pseudocostscntup.ptr.p_int[k] + 1;
            }
            if ((ae_fp_greater_eq(vup, (double) (0))
                 && ae_fp_greater(vfrac, state->pseudocostminfrac))
                && sol->hasdualsolution) {
              state->globalpseudocostup
                  = (state->globalpseudocostup
                         * (double) state->globalpseudocostcntup
                     + vup)
                    / (double) (state->globalpseudocostcntup + 1);
              state->globalpseudocostcntup = state->globalpseudocostcntup + 1;
            }
          }
        }
      }
    }

    /*
     * Select the most promising branching across ones proposed.
     * Upload to RootProblem/ChildSubproblem0/ChildSubproblem1
     */
    if (entry->isrootentry) {
      /*
       * Root problem being solved, upload results to RootProblem
       */
      ae_assert(solcnt == 1, "BBGD: 587200 failed", _state);
      ae_obj_array_get(&entry->solutions, 0, &_sol, _state);
      bbgd_subproblemcopy(sol, sol->leafid, &entry->rootproblem, _state);
    } else {
      /*
       * Branching is over, analyze results
       */
      ae_assert(solcnt % 2 == 0, "BBGD: 599208 failed", _state);
      ae_assert(state->convexityflag >= 0, "BBGD: 701838 failed", _state);
      cntsimplebranch       = 0;
      cnttighten            = 0;
      isinfeasible          = ae_false;
      infeassolidx          = -1;
      bestbranch            = -1;
      bestbranchsolidx      = -1;
      bestbranchscore       = (double) (0);
      besttighten           = -1;
      besttightensolidx     = -1;
      besttightenlowerscore = (double) (0);
      for (i = 0; i <= solcnt / 2 - 1; i++) {
        ae_obj_array_get(&entry->solutions, 2 * i + 0, &_sol0, _state);
        ae_obj_array_get(&entry->solutions, 2 * i + 1, &_sol1, _state);
        ae_assert(sol0->branchbucket >= 0 && sol0->branchbucket < solcnt,
                  "BBGD: 715531 failed",
                  _state);
        ae_assert(sol1->branchbucket == sol0->branchbucket + 1,
                  "BBGD: 715532 failed",
                  _state);
        ae_assert(
            sol0->branchvar == sol1->branchvar, "BBGD: 715533 failed", _state);
        if (!sol0->hasdualsolution && !sol1->hasdualsolution) {
          /*
           * Branching on the variable produced two infeasible problems: good
           * opportunity to prune the entire branch!
           */
          infeassolidx = i;
          isinfeasible = ae_true;
          continue;
        }
        if (sol0->hasdualsolution && sol1->hasdualsolution) {
          /*
           * Both leafs are feasible, compute score for the branching decision.
           *
           * The score handles both convex problems
           * (Sol0.BestFDual>Sol0.ParentFDual) and non-convex ones, where
           * introducing a branching constraint suddenly moves us to a better
           * extremum. Such branchings are given higher priority by using
           * NonconvexityGain>1.
           */
          vrnddn = ae_maxreal(sol0->bestfdual - sol0->parentfdual,
                              state->nonconvexitygain
                                  * (sol0->parentfdual - sol0->bestfdual),
                              _state);
          vrndup = ae_maxreal(sol1->bestfdual - sol1->parentfdual,
                              state->nonconvexitygain
                                  * (sol1->parentfdual - sol1->bestfdual),
                              _state);
          vscore = ((double) 1 - state->pseudocostmu)
                       * ae_minreal(vrnddn, vrndup, _state)
                   + state->pseudocostmu * ae_maxreal(vrnddn, vrndup, _state);
          if (bestbranch < 0 || ae_fp_greater(vscore, bestbranchscore)) {
            bestbranch       = sol0->branchvar;
            bestbranchsolidx = i;
            bestbranchscore  = vscore;
          }
          cntsimplebranch = cntsimplebranch + 1;
          continue;
        }

        /*
         * Branching produced one feasible and one infeasible problem: we can
         * tighten a variable.
         *
         * Non-convexity is handled similarly to the 'both leafs are feasible'
         * case.
         */
        if (sol0->hasdualsolution) {
          vscore = ae_maxreal(sol0->bestfdual - sol0->parentfdual,
                              state->nonconvexitygain
                                  * (sol0->parentfdual - sol0->bestfdual),
                              _state);
          if (besttighten < 0 || ae_fp_greater(vscore, besttightenlowerscore)) {
            besttighten           = sol0->branchvar;
            besttightensolidx     = i;
            besttightenlowerscore = vscore;
          }
        } else {
          vscore = ae_maxreal(sol1->bestfdual - sol1->parentfdual,
                              state->nonconvexitygain
                                  * (sol1->parentfdual - sol1->bestfdual),
                              _state);
          if (besttighten < 0 || ae_fp_greater(vscore, besttightenlowerscore)) {
            besttighten           = sol1->branchvar;
            besttightensolidx     = i;
            besttightenlowerscore = vscore;
          }
        }
        cnttighten = cnttighten + 1;
      }
      ae_assert(state->convexityflag >= 0, "BBGD: 783600", _state);
      if (isinfeasible) {
        /*
         * The problem is infeasible
         */
        ae_obj_array_get(
            &entry->solutions, 2 * infeassolidx + 0, &_sol0, _state);
        ae_obj_array_get(
            &entry->solutions, 2 * infeassolidx + 1, &_sol1, _state);
        bbgd_subproblemcopy(
            sol0, sol0->leafid, &entry->childsubproblem0, _state);
        bbgd_subproblemcopy(
            sol1, sol1->leafid, &entry->childsubproblem1, _state);
      } else {
        if (cnttighten > 0) {
          /*
           * At least one of variables can be tightened
           */
          ae_obj_array_get(
              &entry->solutions, 2 * besttightensolidx + 0, &_sol0, _state);
          ae_obj_array_get(
              &entry->solutions, 2 * besttightensolidx + 1, &_sol1, _state);
          bbgd_subproblemcopy(
              sol0, sol0->leafid, &entry->childsubproblem0, _state);
          bbgd_subproblemcopy(
              sol1, sol1->leafid, &entry->childsubproblem1, _state);
        } else {
          /*
           * Basic split
           */
          ae_assert(cntsimplebranch > 0, "BBGD: 809607", _state);
          ae_obj_array_get(
              &entry->solutions, 2 * bestbranchsolidx + 0, &_sol0, _state);
          ae_obj_array_get(
              &entry->solutions, 2 * bestbranchsolidx + 1, &_sol1, _state);
          bbgd_subproblemcopy(
              sol0, sol0->leafid, &entry->childsubproblem0, _state);
          bbgd_subproblemcopy(
              sol1, sol1->leafid, &entry->childsubproblem1, _state);
        }
      }
    }
    entry->addstatussolutionsaggregated = ae_true;

    /*
     * Update subproblem counts and synchronized global NFEV/NSubproblems (used
     * to deterministically decide on solver stopping criteria)
     */
    state->globalsynchronizednfev
        = state->globalsynchronizednfev + entry->entrynfev;
    state->globalsynchronizednsubproblems
        = state->globalsynchronizednsubproblems + entry->entrynsubproblems;
    state->repntreenodes
        = state->repntreenodes + icase2(entry->isrootentry, 1, 2, _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Decides on fathoming RootSubproblem or Child0/1, depending on IsRootEntry.

  Sets corresponding flags, but does not push solution and does not perform
  diving.

  This function must be called on all solved entries after calling
  EntryAggregateAndUpdateGlobalStats(), prior to  to feeding data from the
  entry into State with EntryPushSolution() or EntryTryPushAndDive().
  *************************************************************************/
  static void bbgd_entrydecideonfathoming(bbgdfrontentry* entry,
                                          bbgdstate*      state,
                                          ae_state*       _state) {
    ae_assert((entry->entrystatus == bbgd_stsolved
               && entry->addstatussolutionsaggregated)
                  && !entry->addstatusdecisionsmade,
              "BBGD: integrity check 902210 failed",
              _state);
    if (entry->isrootentry) {
      entry->fathomroot
          = bbgd_subproblemcanfathom(&entry->rootproblem, state, _state);
    } else {
      entry->fathomchild0
          = bbgd_subproblemcanfathom(&entry->childsubproblem0, state, _state);
      entry->fathomchild1
          = bbgd_subproblemcanfathom(&entry->childsubproblem1, state, _state);
    }
    entry->addstatusdecisionsmade = ae_true;
  }

  /*************************************************************************
  Feeding solution from the entry to the end of the State.bbSubproblems[]
  array. This function does not regrow the sorted part of bbSubproblems (heap).

  If at least one subproblem was added, the flag variable is set to true.
  It is left unchanged otherwise.

  This function merely adds subproblems to the heap, it is assumed that they
  were already scanned for primal solutions, pseudocosts, etc.
  *************************************************************************/
  static void bbgd_entrypushsolution(const bbgdfrontentry* entry,
                                     bbgdstate*            state,
                                     ae_bool*              setonupdate,
                                     ae_state*             _state) {
    ae_assert((entry->entrystatus == bbgd_stsolved
               && entry->addstatussolutionsaggregated)
                  && entry->addstatusdecisionsmade,
              "BBGD: integrity check 863011 failed",
              _state);
    if (entry->isrootentry) {
      if (!entry->fathomroot) {
        bbgd_pushsubproblemsolution(
            &entry->rootproblem, state, setonupdate, _state);
      }
    } else {
      if (!entry->fathomchild0) {
        bbgd_pushsubproblemsolution(
            &entry->childsubproblem0, state, setonupdate, _state);
      }
      if (!entry->fathomchild1) {
        bbgd_pushsubproblemsolution(
            &entry->childsubproblem1, state, setonupdate, _state);
      }
    }
  }

  /*************************************************************************
  Tries to perform diving by pushing the worst leaf to the heap (if feasible
  and not fathomed) and configuring the entry to process the best leaf.

  If unsuccessful due to both leafs being infeasible/fathomed, returns
  False and does not change entry state or the heap. There is no need to call
  EntryPushSolution() in this case.

  If successful, the entry status is set to stReadyToRun and True is returned.

  This function merely adds subproblems to the heap, it is assumed that they
  were already scanned for primal solutions, pseudocosts, etc.
  *************************************************************************/
  static ae_bool bbgd_entrytrypushanddive(bbgdfrontentry*  entry,
                                          const bbgdfront* front,
                                          bbgdstate*       state,
                                          ae_state*        _state) {
    ae_frame        _frame_block;
    ae_int_t        eligibleleafidx;
    double          eligibleleafdual;
    ae_bool         iseligibleleaf;
    bbgdsubproblem* eligiblep;
    ae_smart_ptr    _eligiblep;
    ae_bool         bdummy;
    ae_bool         result;

    ae_frame_make(_state, &_frame_block);
    memset(&_eligiblep, 0, sizeof(_eligiblep));
    ae_smart_ptr_init(
        &_eligiblep, (void**) &eligiblep, ae_false, _state, ae_true);

    ae_assert(((entry->entrystatus == bbgd_stsolved
                && entry->addstatussolutionsaggregated)
               && entry->addstatusdecisionsmade)
                  && !entry->isrootentry,
              "BBGD: integrity check 905205 failed",
              _state);
    eligibleleafidx  = -1;
    eligibleleafdual = ae_maxrealnumber;
    ae_shared_pool_retrieve(&state->sppool, &_eligiblep, _state);

    /*
     * Analyze leaf 0
     */
    iseligibleleaf = !entry->fathomchild0
                     && (eligibleleafidx < 0
                         || ae_fp_less(entry->childsubproblem0.dualbound,
                                       eligibleleafdual));
    if (iseligibleleaf) {
      eligibleleafidx  = 0;
      eligibleleafdual = entry->childsubproblem0.dualbound;
      bbgd_subproblemcopy(&entry->childsubproblem0,
                          entry->childsubproblem0.leafid,
                          eligiblep,
                          _state);
    }

    /*
     * Analyze leaf 1
     */
    iseligibleleaf = !entry->fathomchild1
                     && (eligibleleafidx < 0
                         || ae_fp_less(entry->childsubproblem1.dualbound,
                                       eligibleleafdual));
    if (iseligibleleaf) {
      eligibleleafidx  = 1;
      eligibleleafdual = entry->childsubproblem1.dualbound;
      bbgd_subproblemcopy(&entry->childsubproblem1,
                          entry->childsubproblem1.leafid,
                          eligiblep,
                          _state);
    }

    /*
     * Exit if no leaf is eligible
     */
    if (eligibleleafidx < 0) {
      if (!entry->fathomchild0) {
        bbgd_pushsubproblemsolution(
            &entry->childsubproblem0, state, &bdummy, _state);
      }
      if (!entry->fathomchild1) {
        bbgd_pushsubproblemsolution(
            &entry->childsubproblem1, state, &bdummy, _state);
      }
      if (state->dotrace) {
        ae_trace("> no eligible leaves to continue diving, retrieving problem "
                 "from the heap\n");
      }
      ae_shared_pool_recycle(&state->sppool, &_eligiblep, _state);
      result = ae_false;
      ae_frame_leave(_state);
      return result;
    }
    if (state->dotrace) {
      ae_trace("> diving into leaf %0d (subproblem %8dP)\n",
               (int) (eligibleleafidx),
               (int) (eligiblep->leafid));
    }
    result = ae_true;

    /*
     * Process eligible leaf, try to push ineligible one
     *
     * NOTE: this code has one small inefficiency - if eligible leaf does not
     *       need splitting (and hence diving), it decides to break the diving
     *       even if another one could be processed instead.
     */
    if (eligibleleafidx == 0) {
      if (!entry->fathomchild1) {
        bbgd_pushsubproblemsolution(
            &entry->childsubproblem1, state, &bdummy, _state);
      }
    } else {
      if (!entry->fathomchild0) {
        bbgd_pushsubproblemsolution(
            &entry->childsubproblem0, state, &bdummy, _state);
      }
    }
    if (!bbgd_entryprepareleafs(entry, front, eligiblep, state, _state)) {
      if (eligibleleafidx == 0) {
        if (!entry->fathomchild0) {
          bbgd_pushsubproblemsolution(
              &entry->childsubproblem0, state, &bdummy, _state);
        }
      } else {
        if (!entry->fathomchild1) {
          bbgd_pushsubproblemsolution(
              &entry->childsubproblem1, state, &bdummy, _state);
        }
      }
      result = ae_false;
    }
    ae_shared_pool_recycle(&state->sppool, &_eligiblep, _state);
    ae_frame_leave(_state);
    return result;
  }

  /*************************************************************************
  Feeding solution of the subproblem to the end of the State.bbSubproblems[]
  array. This function does not regrow the sorted part of bbSubproblems (heap).

  Intended to be used by EntryPushSolution().
  *************************************************************************/
  static void bbgd_pushsubproblemsolution(const bbgdsubproblem* subproblem,
                                          bbgdstate*            state,
                                          ae_bool*              setonupdate,
                                          ae_state*             _state) {
    ae_frame        _frame_block;
    bbgdsubproblem* p;
    ae_smart_ptr    _p;

    ae_frame_make(_state, &_frame_block);
    memset(&_p, 0, sizeof(_p));
    ae_smart_ptr_init(&_p, (void**) &p, ae_false, _state, ae_true);

    ae_assert(subproblem->hasdualsolution, "BBGD: 096224 failed", _state);
    ae_shared_pool_retrieve(&state->sppool, &_p, _state);
    bbgd_subproblemcopy(subproblem, subproblem->leafid, p, _state);
    ae_obj_array_append_transfer(&state->bbsubproblems, &_p, _state);
    *setonupdate = ae_true;
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Quick lightweight presolve for a QP subproblem. Saves presolved problem to
  the Entry fields.

  Returns termination type: 0 for success, negative for failure (infeasibility
  detected)
  *************************************************************************/
  static ae_int_t
  bbgd_qpquickpresolve(const bbgdfrontentry*          entry,
                       bbgdfrontsubsolver*            subsolver,
                       /* Real    */ const ae_vector* raws,
                       /* Real    */ const ae_vector* rawxorigin,
                       /* Real    */ const ae_vector* rawbndl,
                       /* Real    */ const ae_vector* rawbndu,
                       const sparsematrix*            rawa,
                       ae_bool                        isupper,
                       /* Real    */ const ae_vector* rawb,
                       ae_int_t                       n,
                       const sparsematrix*            rawsparsec,
                       /* Real    */ const ae_vector* rawcl,
                       /* Real    */ const ae_vector* rawcu,
                       ae_int_t                       lccnt,
                       /* Integer */ const ae_vector* qpordering,
                       double                         eps,
                       ae_state*                      _state) {
    ae_int_t npsv;
    ae_int_t offs;
    ae_int_t dstrow;
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t ic;
    ae_int_t jc;
    double   v;
    double   vi;
    double   vf;
    double   cmin;
    double   cmax;
    double   vscl;
    ae_int_t result;

    ae_assert(
        !isupper, "QPQuickPresolve: IsUpper=True is not implemented", _state);
    result = 0;

    /*
     * Copy box constraints to Subsolver.psvRawBndL/U
     */
    rcopyallocv(n, rawbndl, &subsolver->psvrawbndl, _state);
    rcopyallocv(n, rawbndu, &subsolver->psvrawbndu, _state);

    /*
     * Analyze constraints to find forcing ones that fix variables at their
     * bounds
     */
    for (i = 0; i <= lccnt - 1; i++) {
      j0 = rawsparsec->ridx.ptr.p_int[i];
      j1 = rawsparsec->ridx.ptr.p_int[i + 1] - 1;

      /*
       * Compute row normalzation factor for scaled coordinates
       */
      vscl = 0.0;
      for (jj = j0; jj <= j1; jj++) {
        v = rawsparsec->vals.ptr.p_double[jj]
            * raws->ptr.p_double[rawsparsec->idx.ptr.p_int[jj]];
        vscl = vscl + v * v;
      }
      vscl = (double) 1 / coalesce(ae_sqrt(vscl, _state), (double) (1), _state);

      /*
       * Compute minimum and maximum row values
       */
      cmin = (double) (0);
      cmax = (double) (0);
      for (jj = j0; jj <= j1; jj++) {
        j = rawsparsec->idx.ptr.p_int[jj];
        v = rawsparsec->vals.ptr.p_double[jj];
        if (v > 0.0) {
          cmin = cmin + v * subsolver->psvrawbndl.ptr.p_double[j];
          cmax = cmax + v * subsolver->psvrawbndu.ptr.p_double[j];
        }
        if (v < 0.0) {
          cmin = cmin + v * subsolver->psvrawbndu.ptr.p_double[j];
          cmax = cmax + v * subsolver->psvrawbndl.ptr.p_double[j];
        }
      }
      if (ae_isfinite(cmax, _state)
          && ae_isfinite(rawcl->ptr.p_double[i], _state)) {
        if (ae_fp_less(cmax, rawcl->ptr.p_double[i] - vscl * eps)) {
          /*
           * constraint is infeasible beyond Eps
           */
          result = -3;
          return result;
        }
        if (ae_fp_less(cmax, rawcl->ptr.p_double[i] + vscl * eps)) {
          /*
           * Constraint fixes its variables at values maximizing constraint
           * value
           */
          for (jj = j0; jj <= j1; jj++) {
            j = rawsparsec->idx.ptr.p_int[jj];
            v = rawsparsec->vals.ptr.p_double[jj];
            if (v > 0.0) {
              subsolver->psvrawbndl.ptr.p_double[j]
                  = subsolver->psvrawbndu.ptr.p_double[j];
            }
            if (v < 0.0) {
              subsolver->psvrawbndu.ptr.p_double[j]
                  = subsolver->psvrawbndl.ptr.p_double[j];
            }
          }
          continue;
        }
      }
      if (ae_isfinite(cmin, _state)
          && ae_isfinite(rawcu->ptr.p_double[i], _state)) {
        if (ae_fp_greater(cmin, rawcu->ptr.p_double[i] + vscl * eps)) {
          /*
           * constraint is infeasible beyond Eps
           */
          result = -3;
          return result;
        }
        if (ae_fp_greater(cmin, rawcu->ptr.p_double[i] - vscl * eps)) {
          /*
           * Constraint fixes its variables at values minimizing constraint
           * value
           */
          for (jj = j0; jj <= j1; jj++) {
            j = rawsparsec->idx.ptr.p_int[jj];
            v = rawsparsec->vals.ptr.p_double[jj];
            if (v > 0.0) {
              subsolver->psvrawbndu.ptr.p_double[j]
                  = subsolver->psvrawbndl.ptr.p_double[j];
            }
            if (v < 0.0) {
              subsolver->psvrawbndl.ptr.p_double[j]
                  = subsolver->psvrawbndu.ptr.p_double[j];
            }
          }
          continue;
        }
      }
    }

    /*
     * Analyze fixed vars, compress S, Origin, variable bounds and linear term
     */
    npsv = 0;
    isetallocv(n + lccnt, -1, &subsolver->psvpackxyperm, _state);
    isetallocv(n + lccnt, -1, &subsolver->psvunpackxyperm, _state);
    rallocv(n, &subsolver->psvs, _state);
    rallocv(n, &subsolver->psvxorigin, _state);
    rallocv(n, &subsolver->psvbndl, _state);
    rallocv(n, &subsolver->psvbndu, _state);
    rallocv(n, &subsolver->psvb, _state);
    rallocv(n, &subsolver->psvfixvals, _state);
    for (i = 0; i <= n - 1; i++) {
      if ((ae_isfinite(subsolver->psvrawbndl.ptr.p_double[i], _state)
           && ae_isfinite(subsolver->psvrawbndu.ptr.p_double[i], _state))
          && subsolver->psvrawbndl.ptr.p_double[i]
                 > subsolver->psvrawbndu.ptr.p_double[i]) {
        result = -3;
        return result;
      }
      if ((!ae_isfinite(subsolver->psvrawbndl.ptr.p_double[i], _state)
           || !ae_isfinite(subsolver->psvrawbndu.ptr.p_double[i], _state))
          || subsolver->psvrawbndl.ptr.p_double[i]
                 < subsolver->psvrawbndu.ptr.p_double[i]) {
        subsolver->psvunpackxyperm.ptr.p_int[npsv] = i;
        subsolver->psvpackxyperm.ptr.p_int[i]      = npsv;
        subsolver->psvs.ptr.p_double[npsv]         = raws->ptr.p_double[i];
        subsolver->psvxorigin.ptr.p_double[npsv] = rawxorigin->ptr.p_double[i];
        subsolver->psvbndl.ptr.p_double[npsv]
            = subsolver->psvrawbndl.ptr.p_double[i];
        subsolver->psvbndu.ptr.p_double[npsv]
            = subsolver->psvrawbndu.ptr.p_double[i];
        subsolver->psvb.ptr.p_double[npsv] = rawb->ptr.p_double[i];
        npsv                               = npsv + 1;
      } else {
        subsolver->psvfixvals.ptr.p_double[i]
            = subsolver->psvrawbndl.ptr.p_double[i];
      }
    }
    subsolver->npsv = npsv;
    if (npsv == 0) {
      return result;
    }

    /*
     * Compress quadratic term
     */
    subsolver->psva.n = npsv;
    subsolver->psva.m = npsv;
    iallocv(npsv + 1, &subsolver->psva.ridx, _state);
    offs                              = 0;
    dstrow                            = 0;
    subsolver->psva.ridx.ptr.p_int[0] = 0;
    for (i = 0; i <= n - 1; i++) {
      igrowv(offs + npsv, &subsolver->psva.idx, _state);
      rgrowv(offs + npsv, &subsolver->psva.vals, _state);
      j0 = rawa->ridx.ptr.p_int[i];
      j1 = rawa->uidx.ptr.p_int[i] - 1;
      ic = subsolver->psvpackxyperm.ptr.p_int[i];
      if (ic < 0) {
        /*
         * I-th variable is fixed: products with this variable either update the
         * linear term or the constant one (ignored for presolve).
         *
         * The diagonal term is implicitly ignored (JC>=0 is always evaluated to
         * false).
         */
        vi = subsolver->psvfixvals.ptr.p_double[i]
             - rawxorigin->ptr.p_double[i];
        for (jj = j0; jj <= j1; jj++) {
          j  = rawa->idx.ptr.p_int[jj];
          v  = rawa->vals.ptr.p_double[jj];
          jc = subsolver->psvpackxyperm.ptr.p_int[j];
          if (jc >= 0) {
            subsolver->psvb.ptr.p_double[jc]
                = subsolver->psvb.ptr.p_double[jc] + v * vi;
          }
        }
      } else {
        /*
         * I-th variable is non-fixed: products with this variable either update
         * the quadratic term or the linear one
         */
        for (jj = j0; jj <= j1; jj++) {
          j  = rawa->idx.ptr.p_int[jj];
          v  = rawa->vals.ptr.p_double[jj];
          jc = subsolver->psvpackxyperm.ptr.p_int[j];
          if (jc >= 0) {
            subsolver->psva.idx.ptr.p_int[offs]     = jc;
            subsolver->psva.vals.ptr.p_double[offs] = v;
            offs                                    = offs + 1;
          } else {
            subsolver->psvb.ptr.p_double[ic]
                = subsolver->psvb.ptr.p_double[ic]
                  + v
                        * (subsolver->psvfixvals.ptr.p_double[j]
                           - rawxorigin->ptr.p_double[j]);
          }
        }
        subsolver->psva.ridx.ptr.p_int[dstrow + 1] = offs;
        dstrow                                     = dstrow + 1;
      }
    }
    sparsecreatecrsinplace(&subsolver->psva, _state);

    /*
     * Compress linear constraints
     */
    subsolver->psvlccnt = 0;
    if (lccnt > 0) {
      rsetallocv(lccnt, _state->v_neginf, &subsolver->psvcl, _state);
      rsetallocv(lccnt, _state->v_posinf, &subsolver->psvcu, _state);
      subsolver->psvsparsec.m = 0;
      subsolver->psvsparsec.n = npsv;
      iallocv(lccnt + 1, &subsolver->psvsparsec.ridx, _state);
      subsolver->psvsparsec.ridx.ptr.p_int[0] = 0;
      for (i = 0; i <= lccnt - 1; i++) {
        vf   = 0.0;
        offs = subsolver->psvsparsec.ridx.ptr.p_int[subsolver->psvlccnt];
        igrowv(offs + npsv, &subsolver->psvsparsec.idx, _state);
        rgrowv(offs + npsv, &subsolver->psvsparsec.vals, _state);
        j0 = rawsparsec->ridx.ptr.p_int[i];
        j1 = rawsparsec->ridx.ptr.p_int[i + 1] - 1;
        for (jj = j0; jj <= j1; jj++) {
          j  = rawsparsec->idx.ptr.p_int[jj];
          v  = rawsparsec->vals.ptr.p_double[jj];
          jc = subsolver->psvpackxyperm.ptr.p_int[j];
          if (jc >= 0) {
            subsolver->psvsparsec.idx.ptr.p_int[offs]     = jc;
            subsolver->psvsparsec.vals.ptr.p_double[offs] = v;
            offs                                          = offs + 1;
          } else {
            vf = vf + subsolver->psvfixvals.ptr.p_double[j] * v;
          }
        }
        if (offs == subsolver->psvsparsec.ridx.ptr.p_int[subsolver->psvlccnt]) {
          continue;
        }
        subsolver->psvsparsec.ridx.ptr.p_int[subsolver->psvlccnt + 1] = offs;
        if (ae_isfinite(rawcl->ptr.p_double[i], _state)) {
          subsolver->psvcl.ptr.p_double[subsolver->psvlccnt]
              = rawcl->ptr.p_double[i] - vf;
        }
        if (ae_isfinite(rawcu->ptr.p_double[i], _state)) {
          subsolver->psvcu.ptr.p_double[subsolver->psvlccnt]
              = rawcu->ptr.p_double[i] - vf;
        }
        subsolver->psvpackxyperm.ptr.p_int[n + i] = npsv + subsolver->psvlccnt;
        subsolver->psvunpackxyperm.ptr.p_int[npsv + subsolver->psvlccnt]
            = n + i;
        subsolver->psvlccnt     = subsolver->psvlccnt + 1;
        subsolver->psvsparsec.m = subsolver->psvsparsec.m + 1;
      }
      sparsecreatecrsinplace(&subsolver->psvsparsec, _state);
    }

    /*
     * Compress QP ordering:
     * * first, compute inverse of State.qpOrdering[] in Subsolver.tmpI[]
     * * map Subsolver.tmpI[] elements to packed indexes
     * Subsolver.psvPackXYPerm[]
     * * compress Subsolver.tmpI[], skipping -1's (dropped vars/constraints)
     * * invert Subsolver.tmpI[], storing result into Subsolver.psvQPOrdering[]
     */
    iallocv(n + lccnt, &subsolver->tmpi, _state);
    for (i = 0; i <= n + lccnt - 1; i++) {
      subsolver->tmpi.ptr.p_int[qpordering->ptr.p_int[i]] = i;
    }
    for (i = 0; i <= n + lccnt - 1; i++) {
      subsolver->tmpi.ptr.p_int[i]
          = subsolver->psvpackxyperm.ptr.p_int[subsolver->tmpi.ptr.p_int[i]];
    }
    offs = 0;
    for (i = 0; i <= n + lccnt - 1; i++) {
      if (subsolver->tmpi.ptr.p_int[i] >= 0) {
        subsolver->tmpi.ptr.p_int[offs] = subsolver->tmpi.ptr.p_int[i];
        offs                            = offs + 1;
      }
    }
    ae_assert(
        offs == npsv + subsolver->psvlccnt, "BBGD: 534739 failed", _state);
    iallocv(npsv + subsolver->psvlccnt, &subsolver->psvqpordering, _state);
    for (i = 0; i <= npsv + subsolver->psvlccnt - 1; i++) {
      subsolver->psvqpordering.ptr.p_int[subsolver->tmpi.ptr.p_int[i]] = i;
    }
    return result;
  }

  /*************************************************************************
  Solve QP subproblem given by its bounds and initial point. Internally
  applies iterative refinement to produce highly accurate solutions (essential
  for proper functioning of the B&B solver).

  The solution is loaded into SubproblemArray[itemIdx], with the array object
  storing instances of BBGDSubproblem. Usual rules of updating competing
  solutions (BestXDual, WorstXDual) are followed.

  If UseLock=True, then subproblem is updated by acquiring Entry.EntryLock
  *************************************************************************/
  static void bbgd_solveqpnode(bbgdfrontentry*                entry,
                               bbgdfrontsubsolver*            subsolver,
                               bbgdstate*                     state,
                               /* Real    */ const ae_vector* x0,
                               /* Real    */ const ae_vector* bndl,
                               /* Real    */ const ae_vector* bndu,
                               ae_obj_array*                  subproblemarray,
                               ae_int_t                       itemidx,
                               ae_bool                        uselock,
                               ae_state*                      _state) {
    ae_frame        _frame_block;
    ae_int_t        i;
    ae_int_t        n;
    ae_int_t        terminationtype;
    ae_int_t        tmpterminationtype;
    ae_int_t        k;
    ae_int_t        maxidx;
    double          fsol;
    double          hsol;
    double          mxsol;
    double          fcand;
    double          hcand;
    double          mxcand;
    double          stpnrm;
    double          v;
    double          trustrad;
    ae_bool         applytrustrad;
    ae_bool         isintfeasible;
    bbgdsubproblem* subproblemtoupdate;
    ae_smart_ptr    _subproblemtoupdate;

    ae_frame_make(_state, &_frame_block);
    memset(&_subproblemtoupdate, 0, sizeof(_subproblemtoupdate));
    ae_smart_ptr_init(&_subproblemtoupdate,
                      (void**) &subproblemtoupdate,
                      ae_false,
                      _state,
                      ae_true);

    ae_assert(state->objtype == 1 && state->nnlc == 0,
              "BBGD: integrity check 330714 failed",
              _state);
    n = state->n;
    rallocv(ae_maxint(n, state->lccnt, _state), &subsolver->tmp0, _state);
    ae_obj_array_get(subproblemarray, itemidx, &_subproblemtoupdate, _state);

    /*
     * Quick exit for infeasible with respect to box constraints
     */
    for (i = 0; i <= n - 1; i++) {
      if ((ae_isfinite(bndl->ptr.p_double[i], _state)
           && ae_isfinite(bndu->ptr.p_double[i], _state))
          && ae_fp_greater(bndl->ptr.p_double[i], bndu->ptr.p_double[i])) {
        ae_frame_leave(_state);
        return;
      }
    }

    /*
     * Initial state: box constrain proposed X0
     */
    rcopyallocv(n, x0, &subsolver->xsol, _state);
    for (i = 0; i <= n - 1; i++) {
      if (ae_isfinite(bndl->ptr.p_double[i], _state)) {
        subsolver->xsol.ptr.p_double[i] = ae_maxreal(
            bndl->ptr.p_double[i], subsolver->xsol.ptr.p_double[i], _state);
      }
      if (ae_isfinite(bndu->ptr.p_double[i], _state)) {
        subsolver->xsol.ptr.p_double[i] = ae_minreal(
            bndu->ptr.p_double[i], subsolver->xsol.ptr.p_double[i], _state);
      }
    }
    fsol = 0.5 * sparsevsmv(&state->obja, ae_false, &subsolver->xsol, _state)
           + rdotv(n, &subsolver->xsol, &state->objb, _state) + state->objc0;
    unscaleandchecklc2violation(&state->s,
                                &state->rawa,
                                &state->rawal,
                                &state->rawau,
                                &state->lcsrcidx,
                                state->lccnt,
                                &subsolver->xsol,
                                &hsol,
                                &mxsol,
                                &maxidx,
                                _state);
    for (i = 0; i <= n - 1; i++) {
      if (ae_isfinite(bndl->ptr.p_double[i], _state)) {
        v = ae_maxreal(bndl->ptr.p_double[i] - subsolver->xsol.ptr.p_double[i],
                       0.0,
                       _state);
        hsol  = hsol + v;
        mxsol = ae_maxreal(mxsol, v, _state);
      }
      if (ae_isfinite(bndu->ptr.p_double[i], _state)) {
        v = ae_maxreal(subsolver->xsol.ptr.p_double[i] - bndu->ptr.p_double[i],
                       0.0,
                       _state);
        hsol  = hsol + v;
        mxsol = ae_maxreal(mxsol, v, _state);
      }
    }

    /*
     * Perform several refinement iterations, then analyze candidate
     */
    terminationtype = 0;
    applytrustrad   = ae_false;
    for (k = 0; k <= bbgd_maxqprfsits - 1; k++) {
      /*
       * Reformulate raw problem
       *
       *     min[0.5x'Ax+b'x+c]    subject to    AL<=Ax<=AU, BndL<=x<=BndU
       *
       * as an SQP-style problem
       *
       *     min[0.5y'Ay+(A*xsol+b)'y+fsol]    subject to
       * AL-A*xsol<=Ay<=AU-A*xsol, BndL-xsol<=y<=BndU-xsol
       *
       *  with y=x-xsol.
       */
      rcopyallocv(n, bndl, &subsolver->wrkbndl, _state);
      rcopyallocv(n, bndu, &subsolver->wrkbndu, _state);
      if (applytrustrad) {
        for (i = 0; i <= n - 1; i++) {
          subsolver->wrkbndl.ptr.p_double[i]
              = ae_maxreal(subsolver->wrkbndl.ptr.p_double[i],
                           subsolver->xsol.ptr.p_double[i]
                               - trustrad * state->s.ptr.p_double[i],
                           _state);
          subsolver->wrkbndu.ptr.p_double[i]
              = ae_minreal(subsolver->wrkbndu.ptr.p_double[i],
                           subsolver->xsol.ptr.p_double[i]
                               + trustrad * state->s.ptr.p_double[i],
                           _state);
        }
      }
      sparsesmv(
          &state->obja, ae_false, &subsolver->xsol, &subsolver->wrkb, _state);
      raddv(n, 1.0, &state->objb, &subsolver->wrkb, _state);
      rcopyallocv(n, &state->s, &subsolver->wrks, _state);
      if (applytrustrad) {
        rmulv(n, ae_minreal(trustrad, 1.0, _state), &subsolver->wrks, _state);
      }

      /*
       * Solve SQP subproblem
       */
      tmpterminationtype = bbgd_qpquickpresolve(entry,
                                                subsolver,
                                                &state->s,
                                                &subsolver->xsol,
                                                &subsolver->wrkbndl,
                                                &subsolver->wrkbndu,
                                                &state->obja,
                                                ae_false,
                                                &subsolver->wrkb,
                                                n,
                                                &state->rawa,
                                                &state->rawal,
                                                &state->rawau,
                                                state->lccnt,
                                                &state->qpordering,
                                                state->epsx,
                                                _state);
      if (tmpterminationtype >= 0) {
        if (subsolver->npsv > 0) {
          ipm2init(&subsolver->qpsubsolver,
                   &subsolver->psvs,
                   &subsolver->psvxorigin,
                   subsolver->npsv,
                   &state->densedummy2,
                   &subsolver->psva,
                   1,
                   ae_false,
                   &state->densedummy2,
                   &subsolver->tmp0,
                   0,
                   &subsolver->psvb,
                   0.0,
                   &subsolver->psvbndl,
                   &subsolver->psvbndu,
                   &subsolver->psvsparsec,
                   subsolver->psvlccnt,
                   &state->densedummy2,
                   0,
                   &subsolver->psvcl,
                   &subsolver->psvcu,
                   ae_false,
                   ae_false,
                   _state);
          ipm2setcond(&subsolver->qpsubsolver,
                      state->epsx,
                      state->epsx,
                      state->epsx,
                      _state);
          ipm2setmaxits(&subsolver->qpsubsolver, bbgd_maxipmits, _state);
          ipm2setordering(
              &subsolver->qpsubsolver, &subsolver->psvqpordering, _state);
          ipm2optimize(&subsolver->qpsubsolver,
                       ae_true,
                       &subsolver->tmp1,
                       &subsolver->tmp2,
                       &subsolver->tmp3,
                       &tmpterminationtype,
                       _state);
        }
        rcopyallocv(n, &subsolver->psvfixvals, &subsolver->tmp0, _state);
        for (i = 0; i <= subsolver->npsv - 1; i++) {
          subsolver->tmp0.ptr.p_double[subsolver->psvunpackxyperm.ptr.p_int[i]]
              = subsolver->tmp1.ptr.p_double[i];
        }
      } else {
        /*
         * Presolver signalled infeasibility, stop
         */
        break;
      }
      terminationtype = icoalesce(terminationtype, tmpterminationtype, _state);

      /*
       * Modify trust radius
       */
      rcopyallocv(n, &subsolver->tmp0, &subsolver->tmp1, _state);
      raddv(n, -1.0, &subsolver->xsol, &subsolver->tmp1, _state);
      stpnrm = rsclnrminf(n, &subsolver->tmp1, &state->s, _state);
      if (!applytrustrad) {
        applytrustrad = ae_true;
        trustrad      = 1.0E20;
      }
      trustrad = ae_minreal(trustrad, stpnrm, _state);

      /*
       * Evaluate proposed point using Markov filter
       */
      fcand = 0.5 * sparsevsmv(&state->obja, ae_false, &subsolver->tmp0, _state)
              + rdotv(n, &subsolver->tmp0, &state->objb, _state) + state->objc0;
      unscaleandchecklc2violation(&state->s,
                                  &state->rawa,
                                  &state->rawal,
                                  &state->rawau,
                                  &state->lcsrcidx,
                                  state->lccnt,
                                  &subsolver->tmp0,
                                  &hcand,
                                  &mxcand,
                                  &maxidx,
                                  _state);
      for (i = 0; i <= n - 1; i++) {
        if (ae_isfinite(bndl->ptr.p_double[i], _state)) {
          v      = ae_maxreal(bndl->ptr.p_double[i]
                             - subsolver->tmp0.ptr.p_double[i],
                         0.0,
                         _state);
          hcand  = hcand + v;
          mxcand = ae_maxreal(mxcand, v, _state);
        }
        if (ae_isfinite(bndu->ptr.p_double[i], _state)) {
          v      = ae_maxreal(subsolver->tmp0.ptr.p_double[i]
                             - bndu->ptr.p_double[i],
                         0.0,
                         _state);
          hcand  = hcand + v;
          mxcand = ae_maxreal(mxcand, v, _state);
        }
      }
      if (ae_fp_greater_eq(fcand, fsol) && ae_fp_greater_eq(hcand, hsol)) {
        break;
      }
      rcopyv(n, &subsolver->tmp0, &subsolver->xsol, _state);
      fsol            = fcand;
      hsol            = hcand;
      mxsol           = mxcand;
      terminationtype = tmpterminationtype;
      if (ae_fp_less_eq(stpnrm, state->epsx)) {
        break;
      }
    }
    for (i = 0; i <= n - 1; i++) {
      if (ae_isfinite(bndl->ptr.p_double[i], _state)) {
        subsolver->xsol.ptr.p_double[i] = ae_maxreal(
            bndl->ptr.p_double[i], subsolver->xsol.ptr.p_double[i], _state);
      }
      if (ae_isfinite(bndu->ptr.p_double[i], _state)) {
        subsolver->xsol.ptr.p_double[i] = ae_minreal(
            bndu->ptr.p_double[i], subsolver->xsol.ptr.p_double[i], _state);
      }
    }
    rcopyallocv(n, &subsolver->xsol, &subsolver->tmp0, _state);
    rsetallocv(n, 1.0, &subsolver->tmp1, _state);
    bbgd_analyzeqpsolutionandenforceintegrality(entry,
                                                &subsolver->tmp0,
                                                &subsolver->tmp1,
                                                terminationtype,
                                                state,
                                                subproblemtoupdate,
                                                uselock,
                                                &isintfeasible,
                                                _state);

    /*
     * Apply rounding heuristic to solutions that are box/linearly feasible, but
     * not integer feasible
     */
    if (!isintfeasible && ae_fp_less_eq(mxsol, state->ctol)) {
      /*
       * Round the solution
       */
      for (i = 0; i <= n - 1; i++) {
        if (state->isintegral.ptr.p_bool[i]) {
          subsolver->xsol.ptr.p_double[i]
              = (double) (ae_round(subsolver->xsol.ptr.p_double[i], _state));
          if (ae_isfinite(bndl->ptr.p_double[i], _state)) {
            subsolver->xsol.ptr.p_double[i] = ae_maxreal(
                bndl->ptr.p_double[i], subsolver->xsol.ptr.p_double[i], _state);
          }
          if (ae_isfinite(bndu->ptr.p_double[i], _state)) {
            subsolver->xsol.ptr.p_double[i] = ae_minreal(
                bndu->ptr.p_double[i], subsolver->xsol.ptr.p_double[i], _state);
          }
        }
      }

      /*
       * Reformulate raw problem as one centered around XSol and having fixed
       * integer variables
       */
      rcopyallocv(n, bndl, &subsolver->wrkbndl, _state);
      rcopyallocv(n, bndu, &subsolver->wrkbndu, _state);
      for (i = 0; i <= n - 1; i++) {
        if (state->isintegral.ptr.p_bool[i]) {
          subsolver->wrkbndl.ptr.p_double[i] = subsolver->xsol.ptr.p_double[i];
          subsolver->wrkbndu.ptr.p_double[i] = subsolver->xsol.ptr.p_double[i];
        }
      }
      sparsesmv(
          &state->obja, ae_false, &subsolver->xsol, &subsolver->wrkb, _state);
      raddv(n, 1.0, &state->objb, &subsolver->wrkb, _state);

      /*
       * Solve SQP subproblem
       */
      tmpterminationtype = bbgd_qpquickpresolve(entry,
                                                subsolver,
                                                &state->s,
                                                &subsolver->xsol,
                                                &subsolver->wrkbndl,
                                                &subsolver->wrkbndu,
                                                &state->obja,
                                                ae_false,
                                                &subsolver->wrkb,
                                                n,
                                                &state->rawa,
                                                &state->rawal,
                                                &state->rawau,
                                                state->lccnt,
                                                &state->qpordering,
                                                state->epsx,
                                                _state);
      if (tmpterminationtype >= 0) {
        terminationtype = 1;
        if (subsolver->npsv > 0) {
          ipm2init(&subsolver->qpsubsolver,
                   &subsolver->psvs,
                   &subsolver->psvxorigin,
                   subsolver->npsv,
                   &state->densedummy2,
                   &subsolver->psva,
                   1,
                   ae_false,
                   &state->densedummy2,
                   &subsolver->tmp0,
                   0,
                   &subsolver->psvb,
                   0.0,
                   &subsolver->psvbndl,
                   &subsolver->psvbndu,
                   &subsolver->psvsparsec,
                   subsolver->psvlccnt,
                   &state->densedummy2,
                   0,
                   &subsolver->psvcl,
                   &subsolver->psvcu,
                   ae_false,
                   ae_false,
                   _state);
          ipm2setcond(&subsolver->qpsubsolver,
                      state->epsx,
                      state->epsx,
                      state->epsx,
                      _state);
          ipm2setmaxits(&subsolver->qpsubsolver, bbgd_maxipmits, _state);
          ipm2setordering(
              &subsolver->qpsubsolver, &subsolver->psvqpordering, _state);
          ipm2optimize(&subsolver->qpsubsolver,
                       ae_true,
                       &subsolver->tmp1,
                       &subsolver->tmp2,
                       &subsolver->tmp3,
                       &terminationtype,
                       _state);
        }
        rcopyallocv(n, &subsolver->psvfixvals, &subsolver->tmp0, _state);
        for (i = 0; i <= subsolver->npsv - 1; i++) {
          subsolver->tmp0.ptr.p_double[subsolver->psvunpackxyperm.ptr.p_int[i]]
              = subsolver->tmp1.ptr.p_double[i];
        }
        if (terminationtype > 0) {
          rsetallocv(n, 1.0, &subsolver->tmp3, _state);
          bbgd_analyzeqpsolutionandenforceintegrality(entry,
                                                      &subsolver->tmp0,
                                                      &subsolver->tmp3,
                                                      terminationtype,
                                                      state,
                                                      subproblemtoupdate,
                                                      uselock,
                                                      &isintfeasible,
                                                      _state);
        }
      }
    }
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Analyze solution of a QP relaxation, and send it to the subproblem,
  updating its best and worst primal/dual solutions.

  Can modify XSol.

  NLREP is a compatibility parameter similar to nonlinearity report that is
  used by nonlinear version of BBGD, it must be filled by 1's or some other
  positive values bounded away from zero.

  If UseLock=True, then Subproblem is accessed by acquiring Entry.EntryLock
  *************************************************************************/
  static void bbgd_analyzeqpsolutionandenforceintegrality(
      bbgdfrontentry*                entry,
      /* Real    */ ae_vector*       xsol,
      /* Real    */ const ae_vector* nlrep,
      ae_int_t                       terminationtype,
      const bbgdstate*               state,
      bbgdsubproblem*                subproblem,
      ae_bool                        uselock,
      ae_bool*                       isintfeas,
      ae_state*                      _state) {
    ae_int_t i;
    ae_int_t n;
    double   sumerr;
    double   maxerr;
    ae_int_t maxidx;
    double   f;

    *isintfeas = ae_false;

    ae_assert(state->objtype == 1,
              "BBGD: objType<>1 in AnalyzeQPSolutionAndEnforceIntegrality()",
              _state);
    n = subproblem->n;
    unscaleandchecklc2violation(&state->s,
                                &state->rawa,
                                &state->rawal,
                                &state->rawau,
                                &state->lcsrcidx,
                                state->lccnt,
                                xsol,
                                &sumerr,
                                &maxerr,
                                &maxidx,
                                _state);
    maxerr = maxerr
             / ae_maxreal(
                 rsclnrminf(n, xsol, &state->s, _state), (double) (1), _state);
    if (terminationtype > 0 && ae_fp_less_eq(maxerr, state->ctol)) {
      ae_assert(terminationtype != 6, "BBGD: 382157 failed", _state);

      /*
       * Analyze integrality
       */
      *isintfeas = ae_true;
      for (i = 0; i <= n - 1; i++) {
        if (state->isintegral.ptr.p_bool[i]) {
          *isintfeas
              = *isintfeas
                && ae_fp_less_eq(ae_fabs(xsol->ptr.p_double[i]
                                             - (double) ae_round(
                                                 xsol->ptr.p_double[i], _state),
                                         _state),
                                 state->ctol);
        }
      }
      if (*isintfeas) {
        for (i = 0; i <= n - 1; i++) {
          if (state->isintegral.ptr.p_bool[i]) {
            xsol->ptr.p_double[i]
                = (double) (ae_round(xsol->ptr.p_double[i], _state));
          }
        }
      }
      f = 0.5 * sparsevsmv(&state->obja, ae_false, xsol, _state)
          + rdotv(n, xsol, &state->objb, _state) + state->objc0;
      unscaleandchecklc2violation(&state->s,
                                  &state->rawa,
                                  &state->rawal,
                                  &state->rawau,
                                  &state->lcsrcidx,
                                  state->lccnt,
                                  xsol,
                                  &sumerr,
                                  &maxerr,
                                  &maxidx,
                                  _state);
      maxerr = maxerr
               / ae_maxreal(rsclnrminf(n, xsol, &state->s, _state),
                            (double) (1),
                            _state);

      /*
       * Update primal and dual solutions.
       * Use locks to protect access.
       */
      if (uselock) {
        weakatomicacquirelock(&entry->entrylock, 0, 1, _state);
      }
      bbgd_subproblemmergeinsolution(subproblem,
                                     xsol,
                                     nlrep,
                                     f,
                                     0.0,
                                     maxerr,
                                     *isintfeas,
                                     ae_false,
                                     0,
                                     0,
                                     _state);
      if (uselock) {
        ae_weak_atomic_release_lock(&entry->entrylock, 1, 0);
      }
    } else {
      if (uselock) {
        weakatomicacquirelock(&entry->entrylock, 0, 1, _state);
      }
      subproblem->besthdual = ae_minreal(subproblem->besthdual, maxerr, _state);
      *isintfeas            = ae_false;
      if (uselock) {
        ae_weak_atomic_release_lock(&entry->entrylock, 1, 0);
      }
    }
  }

  /*************************************************************************
  Analyze solution of an NLP relaxation, and send it to the subproblem array.
  The solution is loaded into SubproblemArray[itemIdx], with the array object
  storing instances of BBGDSubproblem. Usual rules of updating competing
  solutions (BestXDual, WorstXDual) are followed.

  Can modify XSol.

  NLREP is nonlinearity report as returned by MINNLC
  *************************************************************************/
  static void bbgd_analyzenlpsolutionandenforceintegrality(
      bbgdfrontentry*                entry,
      /* Real    */ ae_vector*       xsol,
      /* Real    */ const ae_vector* nlrep,
      ae_bool                        forbidrecognizingintegrality,
      const minnlcreport*            rep,
      double                         earlyerror,
      const bbgdstate*               state,
      ae_obj_array*                  subproblemarray,
      ae_int_t                       itemidx,
      ae_bool                        uselock,
      ae_state*                      _state) {
    ae_frame        _frame_block;
    ae_int_t        i;
    ae_int_t        n;
    ae_bool         isintfeas;
    bbgdsubproblem* subproblem;
    ae_smart_ptr    _subproblem;

    ae_frame_make(_state, &_frame_block);
    memset(&_subproblem, 0, sizeof(_subproblem));
    ae_smart_ptr_init(
        &_subproblem, (void**) &subproblem, ae_false, _state, ae_true);

    ae_obj_array_get(subproblemarray, itemidx, &_subproblem, _state);
    n = subproblem->n;

    /*
     * An early stopping solution
     */
    if (rep->terminationtype == 22) {
      if (uselock) {
        weakatomicacquirelock(&entry->entrylock, 0, 1, _state);
      }
      bbgd_subproblemmergeinsolution(subproblem,
                                     xsol,
                                     nlrep,
                                     rep->f,
                                     earlyerror,
                                     0.0,
                                     ae_false,
                                     ae_true,
                                     rep->terminationtype,
                                     rep->iterationscount,
                                     _state);
      if (uselock) {
        ae_weak_atomic_release_lock(&entry->entrylock, 1, 0);
      }
      ae_frame_leave(_state);
      return;
    }

    /*
     * A feasible solution
     */
    if (rep->terminationtype > 0
        && ae_fp_less_eq(rep->sclfeaserr, state->ctol)) {
      ae_assert(rep->terminationtype != 22, "BBGD: 920254 failed", _state);

      /*
       * Analyze integrality
       */
      isintfeas = ae_true;
      for (i = 0; i <= n - 1; i++) {
        if (state->isintegral.ptr.p_bool[i]) {
          isintfeas
              = isintfeas
                && ae_fp_less_eq(ae_fabs(xsol->ptr.p_double[i]
                                             - (double) ae_round(
                                                 xsol->ptr.p_double[i], _state),
                                         _state),
                                 state->ctol);
        }
      }
      if (isintfeas) {
        for (i = 0; i <= n - 1; i++) {
          if (state->isintegral.ptr.p_bool[i]) {
            xsol->ptr.p_double[i]
                = (double) (ae_round(xsol->ptr.p_double[i], _state));
          }
        }
      }

      /*
       * Update primal and dual solutions
       * Use locks to protect access.
       */
      if (uselock) {
        weakatomicacquirelock(&entry->entrylock, 0, 1, _state);
      }
      bbgd_subproblemmergeinsolution(subproblem,
                                     xsol,
                                     nlrep,
                                     rep->f,
                                     earlyerror,
                                     rep->sclfeaserr,
                                     isintfeas && !forbidrecognizingintegrality,
                                     ae_false,
                                     rep->terminationtype,
                                     rep->iterationscount,
                                     _state);
      if (uselock) {
        ae_weak_atomic_release_lock(&entry->entrylock, 1, 0);
      }
      ae_frame_leave(_state);
      return;
    }

    /*
     * Bad solution. Use locks to protect access.
     */
    if (uselock) {
      weakatomicacquirelock(&entry->entrylock, 0, 1, _state);
    }
    if (ae_fp_greater(subproblem->besthdual, rep->sclfeaserr)) {
      subproblem->besthdual = rep->sclfeaserr;
      subproblem->besttt    = rep->terminationtype;
      subproblem->bestits   = rep->iterationscount;
    }
    if (uselock) {
      ae_weak_atomic_release_lock(&entry->entrylock, 1, 0);
    }
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Add sall subproblems that are outside of the heap to the heap.

  When the function finishes, we have
  bbSubproblemsHeapSize=len(bbSubproblems)-1.
  *************************************************************************/
  static void bbgd_growheap(bbgdstate* state, ae_state* _state) {
    ae_int_t cnt;

    cnt = ae_obj_array_get_length(&state->bbsubproblems);
    ae_assert(state->bbsubproblemsheapsize >= 0
                  && state->bbsubproblemsheapsize <= cnt,
              "BBGD: integrity check 181334 failed",
              _state);
    if (cnt > 0) {
      state->bbsubproblemsheapsize
          = bbgd_subproblemheapgrow(&state->bbsubproblems,
                                    0,
                                    state->bbsubproblemsheapsize,
                                    cnt - state->bbsubproblemsheapsize,
                                    _state);
    }
  }

  /*************************************************************************
  Adds all subproblems that are outside of the heap to the heap, then remove
  one on top of the heap and move it to the end of the array. The heap is
  rebuilt after that.

  When the function finishes, we have
  bbSubproblemsHeapSize=len(bbSubproblems)-1.
  *************************************************************************/
  static void bbgd_growheapandpoptop(bbgdstate* state, ae_state* _state) {
    ae_int_t cnt;

    cnt = ae_obj_array_get_length(&state->bbsubproblems);
    ae_assert(cnt > 0, "BBGD: integrity check 040311 failed", _state);
    ae_assert(state->bbsubproblemsheapsize >= 0
                  && state->bbsubproblemsheapsize <= cnt,
              "BBGD: integrity check 040312 failed",
              _state);
    state->bbsubproblemsheapsize
        = bbgd_subproblemheapgrow(&state->bbsubproblems,
                                  0,
                                  state->bbsubproblemsheapsize,
                                  cnt - state->bbsubproblemsheapsize,
                                  _state);
    state->bbsubproblemsheapsize = bbgd_subproblemheappoptop(
        &state->bbsubproblems, 0, state->bbsubproblemsheapsize, _state);
  }

  /*************************************************************************
  Grows subproblem heap having size HeapSize elements, located starting from
  element Offs of the SubproblemHeap array,  by  adding  AppendCnt  elements
  located immediately after the heap part.

  Returns new heap size.
  *************************************************************************/
  static ae_int_t bbgd_subproblemheapgrow(ae_obj_array* subproblemheap,
                                          ae_int_t      offs,
                                          ae_int_t      heapsize,
                                          ae_int_t      appendcnt,
                                          ae_state*     _state) {
    ae_frame        _frame_block;
    bbgdsubproblem* pchild;
    ae_smart_ptr    _pchild;
    bbgdsubproblem* pparent;
    ae_smart_ptr    _pparent;
    ae_int_t        ichild;
    ae_int_t        iparent;
    ae_int_t        newheapsize;
    ae_int_t        result;

    ae_frame_make(_state, &_frame_block);
    memset(&_pchild, 0, sizeof(_pchild));
    memset(&_pparent, 0, sizeof(_pparent));
    ae_smart_ptr_init(&_pchild, (void**) &pchild, ae_false, _state, ae_true);
    ae_smart_ptr_init(&_pparent, (void**) &pparent, ae_false, _state, ae_true);

    ae_assert(heapsize >= 0 && appendcnt >= 0,
              "BBGD: integrity check 984505 failed",
              _state);
    ae_assert(ae_obj_array_get_length(subproblemheap)
                  >= offs + heapsize + appendcnt,
              "BBGD: integrity check 985506 failed",
              _state);

    /*
     * Grow heap until all elements are in the heap
     */
    newheapsize = heapsize + appendcnt;
    while (heapsize < newheapsize) {
      ichild = heapsize;
      while (ichild > 0) {
        iparent = (ichild - 1) / 2;
        ae_obj_array_get(subproblemheap, offs + ichild, &_pchild, _state);
        ae_obj_array_get(subproblemheap, offs + iparent, &_pparent, _state);
        if (ae_fp_less_eq(pparent->dualbound, pchild->dualbound)) {
          break;
        }
        ae_obj_array_swap(
            subproblemheap, offs + ichild, offs + iparent, _state);
        ichild = iparent;
      }
      heapsize = heapsize + 1;
    }
    result = newheapsize;
    ae_frame_leave(_state);
    return result;
  }

  /*************************************************************************
  Removes subproblem on top of the heap having size HeapSize elements, located
  starting from the element Offs of the SubproblemHeap array, and moves it
  to the end of the array. The heap is resorted.

  Returns new heap size.
  *************************************************************************/
  static ae_int_t bbgd_subproblemheappoptop(ae_obj_array* subproblemheap,
                                            ae_int_t      offs,
                                            ae_int_t      heapsize,
                                            ae_state*     _state) {
    ae_frame        _frame_block;
    bbgdsubproblem* pchild;
    ae_smart_ptr    _pchild;
    bbgdsubproblem* pchild2;
    ae_smart_ptr    _pchild2;
    bbgdsubproblem* pparent;
    ae_smart_ptr    _pparent;
    ae_int_t        ichild;
    ae_int_t        ichild2;
    ae_int_t        iparent;
    ae_int_t        result;

    ae_frame_make(_state, &_frame_block);
    memset(&_pchild, 0, sizeof(_pchild));
    memset(&_pchild2, 0, sizeof(_pchild2));
    memset(&_pparent, 0, sizeof(_pparent));
    ae_smart_ptr_init(&_pchild, (void**) &pchild, ae_false, _state, ae_true);
    ae_smart_ptr_init(&_pchild2, (void**) &pchild2, ae_false, _state, ae_true);
    ae_smart_ptr_init(&_pparent, (void**) &pparent, ae_false, _state, ae_true);

    ae_assert(heapsize >= 1, "BBGD: integrity check 023510 failed", _state);
    ae_assert(ae_obj_array_get_length(subproblemheap) >= offs + heapsize,
              "BBGD: integrity check 024510 failed",
              _state);

    /*
     * Pop top
     */
    ae_obj_array_swap(subproblemheap, offs + 0, offs + heapsize - 1, _state);
    heapsize = heapsize - 1;
    iparent  = 0;
    ichild   = 1;
    ichild2  = 2;
    while (ichild < heapsize) {
      ae_obj_array_get(subproblemheap, offs + iparent, &_pparent, _state);
      ae_obj_array_get(subproblemheap, offs + ichild, &_pchild, _state);

      /*
       * only one child.
       *
       * swap and terminate (because this child
       * has no siblings due to heap structure)
       */
      if (ichild2 >= heapsize) {
        if (ae_fp_greater(pparent->dualbound, pchild->dualbound)) {
          ae_obj_array_swap(
              subproblemheap, offs + iparent, offs + ichild, _state);
        }
        break;
      }

      /*
       * Two children
       */
      ae_obj_array_get(subproblemheap, offs + ichild2, &_pchild2, _state);
      if (ae_fp_less(pchild->dualbound, pchild2->dualbound)) {
        if (ae_fp_greater(pparent->dualbound, pchild->dualbound)) {
          ae_obj_array_swap(
              subproblemheap, offs + iparent, offs + ichild, _state);
          iparent = ichild;
        } else {
          break;
        }
      } else {
        if (ae_fp_greater(pparent->dualbound, pchild2->dualbound)) {
          ae_obj_array_swap(
              subproblemheap, offs + iparent, offs + ichild2, _state);
          iparent = ichild2;
        } else {
          break;
        }
      }
      ichild  = 2 * iparent + 1;
      ichild2 = 2 * iparent + 2;
    }
    result = heapsize;
    ae_frame_leave(_state);
    return result;
  }

  static void bbgd_tracelaconicheader(bbgdstate* state, ae_state* _state) {
    ae_trace("                                                       Nodes "
             "statistics  \n");
    ae_trace("       Time         Dual bound       Primal bound      solved    "
             " in heap\n");
  }

  static void bbgd_tracelaconic(bbgdstate* state, ae_state* _state) {
    ae_trace(
        "%10.3fs",
        (double) (0.001 * stimergetmsrunning(&state->timerglobal, _state)));
    ae_trace("    %15.6e", (double) (state->ffdual));
    if (state->hasprimalsolution) {
      ae_trace("    %15.6e", (double) (state->fprim));
    } else {
      ae_trace("                INF");
    }
    ae_trace("    %8d", (int) (state->repnsubproblems));
    ae_trace("    %8d", (int) (ae_obj_array_get_length(&state->bbsubproblems)));
    ae_trace("\n");
  }

  void _bbgdsubproblem_init(void*     _p,
                            ae_state* _state,
                            ae_bool   make_automatic) {
    bbgdsubproblem* p = (bbgdsubproblem*) _p;
    ae_touch_ptr((void*) p);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->parentlinearity, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->xprim, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bestxdual, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->worstxdual, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->subproblemlinearity, 0, DT_BOOL, _state, make_automatic);
  }

  void _bbgdsubproblem_init_copy(void*       _dst,
                                 const void* _src,
                                 ae_state*   _state,
                                 ae_bool     make_automatic) {
    bbgdsubproblem*       dst = (bbgdsubproblem*) _dst;
    const bbgdsubproblem* src = (const bbgdsubproblem*) _src;
    dst->leafid               = src->leafid;
    dst->n                    = src->n;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    dst->branchbucket = src->branchbucket;
    dst->parentfdual  = src->parentfdual;
    ae_vector_init_copy(
        &dst->parentlinearity, &src->parentlinearity, _state, make_automatic);
    dst->branchvar         = src->branchvar;
    dst->branchval         = src->branchval;
    dst->ncuttingplanes    = src->ncuttingplanes;
    dst->hasprimalsolution = src->hasprimalsolution;
    ae_vector_init_copy(&dst->xprim, &src->xprim, _state, make_automatic);
    dst->fprim           = src->fprim;
    dst->hprim           = src->hprim;
    dst->hasdualsolution = src->hasdualsolution;
    ae_vector_init_copy(
        &dst->bestxdual, &src->bestxdual, _state, make_automatic);
    dst->bestfdual           = src->bestfdual;
    dst->bestfdualearlyerror = src->bestfdualearlyerror;
    dst->besthdual           = src->besthdual;
    dst->besttt              = src->besttt;
    dst->bestits             = src->bestits;
    ae_vector_init_copy(
        &dst->worstxdual, &src->worstxdual, _state, make_automatic);
    dst->worstfdual        = src->worstfdual;
    dst->worsthdual        = src->worsthdual;
    dst->bestdualisintfeas = src->bestdualisintfeas;
    dst->dualbound         = src->dualbound;
    dst->earlystopped      = src->earlystopped;
    dst->donotfathom       = src->donotfathom;
    ae_vector_init_copy(&dst->subproblemlinearity,
                        &src->subproblemlinearity,
                        _state,
                        make_automatic);
  }

  void _bbgdsubproblem_clear(void* _p) {
    bbgdsubproblem* p = (bbgdsubproblem*) _p;
    ae_touch_ptr((void*) p);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->parentlinearity);
    ae_vector_clear(&p->xprim);
    ae_vector_clear(&p->bestxdual);
    ae_vector_clear(&p->worstxdual);
    ae_vector_clear(&p->subproblemlinearity);
  }

  void _bbgdsubproblem_destroy(void* _p) {
    bbgdsubproblem* p = (bbgdsubproblem*) _p;
    ae_touch_ptr((void*) p);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->parentlinearity);
    ae_vector_destroy(&p->xprim);
    ae_vector_destroy(&p->bestxdual);
    ae_vector_destroy(&p->worstxdual);
    ae_vector_destroy(&p->subproblemlinearity);
  }

  void _bbgdfrontsubsolver_init(void*     _p,
                                ae_state* _state,
                                ae_bool   make_automatic) {
    bbgdfrontsubsolver* p = (bbgdfrontsubsolver*) _p;
    ae_touch_ptr((void*) p);
    _bbgdsubproblem_init(&p->subproblem, _state, make_automatic);
    _minnlcstate_init(&p->nlpsubsolver, _state, make_automatic);
    _ipm2state_init(&p->qpsubsolver, _state, make_automatic);
    _minnlcreport_init(&p->nlprep, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    ae_vector_init(&p->xsol, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp3, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpi, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->wrks, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->psvpackxyperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->psvunpackxyperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->psvs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->psvxorigin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->psvbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->psvbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->psvb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->psvfixvals, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->psvrawbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->psvrawbndu, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->psva, _state, make_automatic);
    _sparsematrix_init(&p->psvsparsec, _state, make_automatic);
    ae_vector_init(&p->psvcl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->psvcu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->psvqpordering, 0, DT_INT, _state, make_automatic);
  }

  void _bbgdfrontsubsolver_init_copy(void*       _dst,
                                     const void* _src,
                                     ae_state*   _state,
                                     ae_bool     make_automatic) {
    bbgdfrontsubsolver*       dst = (bbgdfrontsubsolver*) _dst;
    const bbgdfrontsubsolver* src = (const bbgdfrontsubsolver*) _src;
    dst->subsolverstatus          = src->subsolverstatus;
    _bbgdsubproblem_init_copy(
        &dst->subproblem, &src->subproblem, _state, make_automatic);
    _minnlcstate_init_copy(
        &dst->nlpsubsolver, &src->nlpsubsolver, _state, make_automatic);
    _ipm2state_init_copy(
        &dst->qpsubsolver, &src->qpsubsolver, _state, make_automatic);
    _minnlcreport_init_copy(&dst->nlprep, &src->nlprep, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    ae_vector_init_copy(&dst->xsol, &src->xsol, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp3, &src->tmp3, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpi, &src->tmpi, _state, make_automatic);
    ae_vector_init_copy(&dst->wrks, &src->wrks, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkbndl, &src->wrkbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkbndu, &src->wrkbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkb, &src->wrkb, _state, make_automatic);
    ae_vector_init_copy(
        &dst->psvpackxyperm, &src->psvpackxyperm, _state, make_automatic);
    ae_vector_init_copy(
        &dst->psvunpackxyperm, &src->psvunpackxyperm, _state, make_automatic);
    ae_vector_init_copy(&dst->psvs, &src->psvs, _state, make_automatic);
    ae_vector_init_copy(
        &dst->psvxorigin, &src->psvxorigin, _state, make_automatic);
    ae_vector_init_copy(&dst->psvbndl, &src->psvbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->psvbndu, &src->psvbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->psvb, &src->psvb, _state, make_automatic);
    ae_vector_init_copy(
        &dst->psvfixvals, &src->psvfixvals, _state, make_automatic);
    ae_vector_init_copy(
        &dst->psvrawbndl, &src->psvrawbndl, _state, make_automatic);
    ae_vector_init_copy(
        &dst->psvrawbndu, &src->psvrawbndu, _state, make_automatic);
    dst->npsv = src->npsv;
    _sparsematrix_init_copy(&dst->psva, &src->psva, _state, make_automatic);
    _sparsematrix_init_copy(
        &dst->psvsparsec, &src->psvsparsec, _state, make_automatic);
    ae_vector_init_copy(&dst->psvcl, &src->psvcl, _state, make_automatic);
    ae_vector_init_copy(&dst->psvcu, &src->psvcu, _state, make_automatic);
    dst->psvlccnt = src->psvlccnt;
    ae_vector_init_copy(
        &dst->psvqpordering, &src->psvqpordering, _state, make_automatic);
  }

  void _bbgdfrontsubsolver_clear(void* _p) {
    bbgdfrontsubsolver* p = (bbgdfrontsubsolver*) _p;
    ae_touch_ptr((void*) p);
    _bbgdsubproblem_clear(&p->subproblem);
    _minnlcstate_clear(&p->nlpsubsolver);
    _ipm2state_clear(&p->qpsubsolver);
    _minnlcreport_clear(&p->nlprep);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->xsol);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->tmp2);
    ae_vector_clear(&p->tmp3);
    ae_vector_clear(&p->tmpi);
    ae_vector_clear(&p->wrks);
    ae_vector_clear(&p->wrkbndl);
    ae_vector_clear(&p->wrkbndu);
    ae_vector_clear(&p->wrkb);
    ae_vector_clear(&p->psvpackxyperm);
    ae_vector_clear(&p->psvunpackxyperm);
    ae_vector_clear(&p->psvs);
    ae_vector_clear(&p->psvxorigin);
    ae_vector_clear(&p->psvbndl);
    ae_vector_clear(&p->psvbndu);
    ae_vector_clear(&p->psvb);
    ae_vector_clear(&p->psvfixvals);
    ae_vector_clear(&p->psvrawbndl);
    ae_vector_clear(&p->psvrawbndu);
    _sparsematrix_clear(&p->psva);
    _sparsematrix_clear(&p->psvsparsec);
    ae_vector_clear(&p->psvcl);
    ae_vector_clear(&p->psvcu);
    ae_vector_clear(&p->psvqpordering);
  }

  void _bbgdfrontsubsolver_destroy(void* _p) {
    bbgdfrontsubsolver* p = (bbgdfrontsubsolver*) _p;
    ae_touch_ptr((void*) p);
    _bbgdsubproblem_destroy(&p->subproblem);
    _minnlcstate_destroy(&p->nlpsubsolver);
    _ipm2state_destroy(&p->qpsubsolver);
    _minnlcreport_destroy(&p->nlprep);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->xsol);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->tmp2);
    ae_vector_destroy(&p->tmp3);
    ae_vector_destroy(&p->tmpi);
    ae_vector_destroy(&p->wrks);
    ae_vector_destroy(&p->wrkbndl);
    ae_vector_destroy(&p->wrkbndu);
    ae_vector_destroy(&p->wrkb);
    ae_vector_destroy(&p->psvpackxyperm);
    ae_vector_destroy(&p->psvunpackxyperm);
    ae_vector_destroy(&p->psvs);
    ae_vector_destroy(&p->psvxorigin);
    ae_vector_destroy(&p->psvbndl);
    ae_vector_destroy(&p->psvbndu);
    ae_vector_destroy(&p->psvb);
    ae_vector_destroy(&p->psvfixvals);
    ae_vector_destroy(&p->psvrawbndl);
    ae_vector_destroy(&p->psvrawbndu);
    _sparsematrix_destroy(&p->psva);
    _sparsematrix_destroy(&p->psvsparsec);
    ae_vector_destroy(&p->psvcl);
    ae_vector_destroy(&p->psvcu);
    ae_vector_destroy(&p->psvqpordering);
  }

  void _bbgdfrontentry_init(void*     _p,
                            ae_state* _state,
                            ae_bool   make_automatic) {
    bbgdfrontentry* p = (bbgdfrontentry*) _p;
    ae_touch_ptr((void*) p);
    _stimer_init(&p->timerlocal, _state, make_automatic);
    _bbgdsubproblem_init(&p->parentsubproblem, _state, make_automatic);
    _bbgdsubproblem_init(&p->rootproblem, _state, make_automatic);
    _bbgdsubproblem_init(&p->childsubproblem0, _state, make_automatic);
    _bbgdsubproblem_init(&p->childsubproblem1, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    _hqrndstate_init(&p->entryrng, _state, make_automatic);
    ae_obj_array_init(&p->subsolvers, _state, make_automatic);
    ae_obj_array_init(&p->spqueue, _state, make_automatic);
    ae_obj_array_init(&p->solutions, _state, make_automatic);
    _bbgdsubproblem_init(&p->tmpsubproblem, _state, make_automatic);
    ae_vector_init(&p->tmpreliablebranchidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(
        &p->tmpreliablebranchscore, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(
        &p->tmpunreliablebranchidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(
        &p->tmpunreliablebranchscore, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpchosenbranchidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(
        &p->tmpchosenbranchscore, 0, DT_REAL, _state, make_automatic);
  }

  void _bbgdfrontentry_init_copy(void*       _dst,
                                 const void* _src,
                                 ae_state*   _state,
                                 ae_bool     make_automatic) {
    bbgdfrontentry*       dst         = (bbgdfrontentry*) _dst;
    const bbgdfrontentry* src         = (const bbgdfrontentry*) _src;
    dst->entrystatus                  = src->entrystatus;
    dst->addstatussolutionsaggregated = src->addstatussolutionsaggregated;
    dst->addstatusdecisionsmade       = src->addstatusdecisionsmade;
    dst->entrylock                    = src->entrylock;
    dst->isrootentry                  = src->isrootentry;
    dst->maxsubsolvers                = src->maxsubsolvers;
    dst->hastimeout                   = src->hastimeout;
    dst->timeout                      = src->timeout;
    _stimer_init_copy(
        &dst->timerlocal, &src->timerlocal, _state, make_automatic);
    _bbgdsubproblem_init_copy(
        &dst->parentsubproblem, &src->parentsubproblem, _state, make_automatic);
    _bbgdsubproblem_init_copy(
        &dst->rootproblem, &src->rootproblem, _state, make_automatic);
    _bbgdsubproblem_init_copy(
        &dst->childsubproblem0, &src->childsubproblem0, _state, make_automatic);
    _bbgdsubproblem_init_copy(
        &dst->childsubproblem1, &src->childsubproblem1, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    _hqrndstate_init_copy(
        &dst->entryrng, &src->entryrng, _state, make_automatic);
    dst->entrynfev         = src->entrynfev;
    dst->entrynsubproblems = src->entrynsubproblems;
    dst->fathomroot        = src->fathomroot;
    dst->fathomchild0      = src->fathomchild0;
    dst->fathomchild1      = src->fathomchild1;
    ae_obj_array_init_copy(
        &dst->subsolvers, &src->subsolvers, _state, make_automatic);
    ae_obj_array_init_copy(
        &dst->spqueue, &src->spqueue, _state, make_automatic);
    ae_obj_array_init_copy(
        &dst->solutions, &src->solutions, _state, make_automatic);
    _bbgdsubproblem_init_copy(
        &dst->tmpsubproblem, &src->tmpsubproblem, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpreliablebranchidx,
                        &src->tmpreliablebranchidx,
                        _state,
                        make_automatic);
    ae_vector_init_copy(&dst->tmpreliablebranchscore,
                        &src->tmpreliablebranchscore,
                        _state,
                        make_automatic);
    ae_vector_init_copy(&dst->tmpunreliablebranchidx,
                        &src->tmpunreliablebranchidx,
                        _state,
                        make_automatic);
    ae_vector_init_copy(&dst->tmpunreliablebranchscore,
                        &src->tmpunreliablebranchscore,
                        _state,
                        make_automatic);
    ae_vector_init_copy(&dst->tmpchosenbranchidx,
                        &src->tmpchosenbranchidx,
                        _state,
                        make_automatic);
    ae_vector_init_copy(&dst->tmpchosenbranchscore,
                        &src->tmpchosenbranchscore,
                        _state,
                        make_automatic);
  }

  void _bbgdfrontentry_clear(void* _p) {
    bbgdfrontentry* p = (bbgdfrontentry*) _p;
    ae_touch_ptr((void*) p);
    _stimer_clear(&p->timerlocal);
    _bbgdsubproblem_clear(&p->parentsubproblem);
    _bbgdsubproblem_clear(&p->rootproblem);
    _bbgdsubproblem_clear(&p->childsubproblem0);
    _bbgdsubproblem_clear(&p->childsubproblem1);
    _rcommstate_clear(&p->rstate);
    _hqrndstate_clear(&p->entryrng);
    ae_obj_array_clear(&p->subsolvers);
    ae_obj_array_clear(&p->spqueue);
    ae_obj_array_clear(&p->solutions);
    _bbgdsubproblem_clear(&p->tmpsubproblem);
    ae_vector_clear(&p->tmpreliablebranchidx);
    ae_vector_clear(&p->tmpreliablebranchscore);
    ae_vector_clear(&p->tmpunreliablebranchidx);
    ae_vector_clear(&p->tmpunreliablebranchscore);
    ae_vector_clear(&p->tmpchosenbranchidx);
    ae_vector_clear(&p->tmpchosenbranchscore);
  }

  void _bbgdfrontentry_destroy(void* _p) {
    bbgdfrontentry* p = (bbgdfrontentry*) _p;
    ae_touch_ptr((void*) p);
    _stimer_destroy(&p->timerlocal);
    _bbgdsubproblem_destroy(&p->parentsubproblem);
    _bbgdsubproblem_destroy(&p->rootproblem);
    _bbgdsubproblem_destroy(&p->childsubproblem0);
    _bbgdsubproblem_destroy(&p->childsubproblem1);
    _rcommstate_destroy(&p->rstate);
    _hqrndstate_destroy(&p->entryrng);
    ae_obj_array_destroy(&p->subsolvers);
    ae_obj_array_destroy(&p->spqueue);
    ae_obj_array_destroy(&p->solutions);
    _bbgdsubproblem_destroy(&p->tmpsubproblem);
    ae_vector_destroy(&p->tmpreliablebranchidx);
    ae_vector_destroy(&p->tmpreliablebranchscore);
    ae_vector_destroy(&p->tmpunreliablebranchidx);
    ae_vector_destroy(&p->tmpunreliablebranchscore);
    ae_vector_destroy(&p->tmpchosenbranchidx);
    ae_vector_destroy(&p->tmpchosenbranchscore);
  }

  void _bbgdfront_init(void* _p, ae_state* _state, ae_bool make_automatic) {
    bbgdfront* p = (bbgdfront*) _p;
    ae_touch_ptr((void*) p);
    ae_obj_array_init(&p->entries, _state, make_automatic);
    ae_shared_pool_init(&p->entrypool, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    ae_vector_init(&p->jobs, 0, DT_INT, _state, make_automatic);
  }

  void _bbgdfront_init_copy(void*       _dst,
                            const void* _src,
                            ae_state*   _state,
                            ae_bool     make_automatic) {
    bbgdfront*       dst = (bbgdfront*) _dst;
    const bbgdfront* src = (const bbgdfront*) _src;
    dst->frontmode       = src->frontmode;
    dst->frontstatus     = src->frontstatus;
    dst->popmostrecent   = src->popmostrecent;
    dst->backtrackbudget = src->backtrackbudget;
    dst->frontsize       = src->frontsize;
    ae_obj_array_init_copy(
        &dst->entries, &src->entries, _state, make_automatic);
    ae_shared_pool_init_copy(
        &dst->entrypool, &src->entrypool, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    ae_vector_init_copy(&dst->jobs, &src->jobs, _state, make_automatic);
  }

  void _bbgdfront_clear(void* _p) {
    bbgdfront* p = (bbgdfront*) _p;
    ae_touch_ptr((void*) p);
    ae_obj_array_clear(&p->entries);
    ae_shared_pool_clear(&p->entrypool);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->jobs);
  }

  void _bbgdfront_destroy(void* _p) {
    bbgdfront* p = (bbgdfront*) _p;
    ae_touch_ptr((void*) p);
    ae_obj_array_destroy(&p->entries);
    ae_shared_pool_destroy(&p->entrypool);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->jobs);
  }

  void _bbgdstate_init(void* _p, ae_state* _state, ae_bool make_automatic) {
    bbgdstate* p = (bbgdstate*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_init(&p->criteria, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->isintegral, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->isbinary, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->islinear, 0, DT_BOOL, _state, make_automatic);
    _sparsematrix_init(&p->obja, _state, make_automatic);
    ae_vector_init(&p->objb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->qpordering, 0, DT_INT, _state, make_automatic);
    _sparsematrix_init(&p->rawa, _state, make_automatic);
    ae_vector_init(&p->rawal, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawau, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lcsrcidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->nl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xc, 0, DT_REAL, _state, make_automatic);
    _stimer_init(&p->timerglobal, _state, make_automatic);
    ae_vector_init(&p->xprim, 0, DT_REAL, _state, make_automatic);
    _bbgdsubproblem_init(&p->rootsubproblem, _state, make_automatic);
    ae_obj_array_init(&p->bbsubproblems, _state, make_automatic);
    _bbgdfront_init(&p->front, _state, make_automatic);
    ae_shared_pool_init(&p->sppool, _state, make_automatic);
    ae_shared_pool_init(&p->subsolverspool, _state, make_automatic);
    ae_vector_init(&p->pseudocostsup, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->pseudocostsdown, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->pseudocostscntup, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->pseudocostscntdown, 0, DT_INT, _state, make_automatic);
    _hqrndstate_init(&p->unsafeglobalrng, _state, make_automatic);
    _rcommstate_init(&p->rcommv2, _state, make_automatic);
    _bbgdsubproblem_init(&p->dummysubproblem, _state, make_automatic);
    _bbgdfrontsubsolver_init(&p->dummysubsolver, _state, make_automatic);
    _ipm2state_init(&p->dummyqpsubsolver, _state, make_automatic);
    _bbgdfrontentry_init(&p->dummyentry, _state, make_automatic);
    ae_matrix_init(&p->densedummy2, 0, 0, DT_REAL, _state, make_automatic);
  }

  void _bbgdstate_init_copy(void*       _dst,
                            const void* _src,
                            ae_state*   _state,
                            ae_bool     make_automatic) {
    bbgdstate*       dst = (bbgdstate*) _dst;
    const bbgdstate* src = (const bbgdstate*) _src;
    dst->n               = src->n;
    _nlpstoppingcriteria_init_copy(
        &dst->criteria, &src->criteria, _state, make_automatic);
    dst->diffstep                    = src->diffstep;
    dst->convexityflag               = src->convexityflag;
    dst->nonconvexitygain            = src->nonconvexitygain;
    dst->pdgap                       = src->pdgap;
    dst->ctol                        = src->ctol;
    dst->epsx                        = src->epsx;
    dst->epsf                        = src->epsf;
    dst->subsolveralgo               = src->subsolveralgo;
    dst->subsolvermemlen             = src->subsolvermemlen;
    dst->nonrootmaxitslin            = src->nonrootmaxitslin;
    dst->nonrootmaxitsconst          = src->nonrootmaxitsconst;
    dst->nonrootmaxitsaboveaverage   = src->nonrootmaxitsaboveaverage;
    dst->nonrootadditsforfeasibility = src->nonrootadditsforfeasibility;
    dst->pseudocostmu                = src->pseudocostmu;
    dst->pseudocostminfrac           = src->pseudocostminfrac;
    dst->pseudocostinfeaspenaly      = src->pseudocostinfeaspenaly;
    dst->nmultistarts                = src->nmultistarts;
    dst->branchingtype               = src->branchingtype;
    dst->krel                        = src->krel;
    dst->kevalunreliable             = src->kevalunreliable;
    dst->kevalreliable               = src->kevalreliable;
    dst->dodiving                    = src->dodiving;
    dst->timeout                     = src->timeout;
    dst->bbgdgroupsize               = src->bbgdgroupsize;
    dst->maxsubsolvers               = src->maxsubsolvers;
    dst->issuesparserequests         = src->issuesparserequests;
    dst->forceserial                 = src->forceserial;
    dst->softmaxnodes                = src->softmaxnodes;
    dst->hardmaxnodes                = src->hardmaxnodes;
    dst->maxprimalcandidates         = src->maxprimalcandidates;
    dst->syncinterval                = src->syncinterval;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(
        &dst->isintegral, &src->isintegral, _state, make_automatic);
    ae_vector_init_copy(&dst->isbinary, &src->isbinary, _state, make_automatic);
    ae_vector_init_copy(&dst->islinear, &src->islinear, _state, make_automatic);
    dst->objtype = src->objtype;
    _sparsematrix_init_copy(&dst->obja, &src->obja, _state, make_automatic);
    ae_vector_init_copy(&dst->objb, &src->objb, _state, make_automatic);
    dst->objc0 = src->objc0;
    ae_vector_init_copy(
        &dst->qpordering, &src->qpordering, _state, make_automatic);
    _sparsematrix_init_copy(&dst->rawa, &src->rawa, _state, make_automatic);
    ae_vector_init_copy(&dst->rawal, &src->rawal, _state, make_automatic);
    ae_vector_init_copy(&dst->rawau, &src->rawau, _state, make_automatic);
    ae_vector_init_copy(&dst->lcsrcidx, &src->lcsrcidx, _state, make_automatic);
    dst->lccnt = src->lccnt;
    dst->nnlc  = src->nnlc;
    ae_vector_init_copy(&dst->nl, &src->nl, _state, make_automatic);
    ae_vector_init_copy(&dst->nu, &src->nu, _state, make_automatic);
    dst->hasx0 = src->hasx0;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    dst->userterminationneeded = src->userterminationneeded;
    ae_vector_init_copy(&dst->xc, &src->xc, _state, make_automatic);
    dst->repnfev                    = src->repnfev;
    dst->repnsubproblems            = src->repnsubproblems;
    dst->repntreenodes              = src->repntreenodes;
    dst->repnnodesbeforefeasibility = src->repnnodesbeforefeasibility;
    dst->repnprimalcandidates       = src->repnprimalcandidates;
    dst->repterminationtype         = src->repterminationtype;
    dst->repf                       = src->repf;
    dst->reppdgap                   = src->reppdgap;
    _stimer_init_copy(
        &dst->timerglobal, &src->timerglobal, _state, make_automatic);
    dst->lastlaconicreportepoch = src->lastlaconicreportepoch;
    dst->dotrace                = src->dotrace;
    dst->dolaconictrace         = src->dolaconictrace;
    dst->doanytrace             = src->doanytrace;
    dst->nextleafid             = src->nextleafid;
    dst->hasprimalsolution      = src->hasprimalsolution;
    ae_vector_init_copy(&dst->xprim, &src->xprim, _state, make_automatic);
    dst->fprim     = src->fprim;
    dst->hprim     = src->hprim;
    dst->ffdual    = src->ffdual;
    dst->timedout  = src->timedout;
    dst->unbounded = src->unbounded;
    _bbgdsubproblem_init_copy(
        &dst->rootsubproblem, &src->rootsubproblem, _state, make_automatic);
    ae_obj_array_init_copy(
        &dst->bbsubproblems, &src->bbsubproblems, _state, make_automatic);
    dst->bbsubproblemsheapsize      = src->bbsubproblemsheapsize;
    dst->bbsubproblemsrecentlyadded = src->bbsubproblemsrecentlyadded;
    _bbgdfront_init_copy(&dst->front, &src->front, _state, make_automatic);
    ae_shared_pool_init_copy(
        &dst->sppool, &src->sppool, _state, make_automatic);
    ae_shared_pool_init_copy(
        &dst->subsolverspool, &src->subsolverspool, _state, make_automatic);
    ae_vector_init_copy(
        &dst->pseudocostsup, &src->pseudocostsup, _state, make_automatic);
    ae_vector_init_copy(
        &dst->pseudocostsdown, &src->pseudocostsdown, _state, make_automatic);
    ae_vector_init_copy(
        &dst->pseudocostscntup, &src->pseudocostscntup, _state, make_automatic);
    ae_vector_init_copy(&dst->pseudocostscntdown,
                        &src->pseudocostscntdown,
                        _state,
                        make_automatic);
    dst->globalpseudocostup             = src->globalpseudocostup;
    dst->globalpseudocostdown           = src->globalpseudocostdown;
    dst->globalpseudocostcntup          = src->globalpseudocostcntup;
    dst->globalpseudocostcntdown        = src->globalpseudocostcntdown;
    dst->globalsynchronizednfev         = src->globalsynchronizednfev;
    dst->globalsynchronizednsubproblems = src->globalsynchronizednsubproblems;
    _hqrndstate_init_copy(
        &dst->unsafeglobalrng, &src->unsafeglobalrng, _state, make_automatic);
    dst->requestsource   = src->requestsource;
    dst->lastrequesttype = src->lastrequesttype;
    _rcommstate_init_copy(&dst->rcommv2, &src->rcommv2, _state, make_automatic);
    dst->usehandlersandsync = src->usehandlersandsync;
    _bbgdsubproblem_init_copy(
        &dst->dummysubproblem, &src->dummysubproblem, _state, make_automatic);
    _bbgdfrontsubsolver_init_copy(
        &dst->dummysubsolver, &src->dummysubsolver, _state, make_automatic);
    _ipm2state_init_copy(
        &dst->dummyqpsubsolver, &src->dummyqpsubsolver, _state, make_automatic);
    _bbgdfrontentry_init_copy(
        &dst->dummyentry, &src->dummyentry, _state, make_automatic);
    ae_matrix_init_copy(
        &dst->densedummy2, &src->densedummy2, _state, make_automatic);
  }

  void _bbgdstate_clear(void* _p) {
    bbgdstate* p = (bbgdstate*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_clear(&p->criteria);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->isintegral);
    ae_vector_clear(&p->isbinary);
    ae_vector_clear(&p->islinear);
    _sparsematrix_clear(&p->obja);
    ae_vector_clear(&p->objb);
    ae_vector_clear(&p->qpordering);
    _sparsematrix_clear(&p->rawa);
    ae_vector_clear(&p->rawal);
    ae_vector_clear(&p->rawau);
    ae_vector_clear(&p->lcsrcidx);
    ae_vector_clear(&p->nl);
    ae_vector_clear(&p->nu);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->xc);
    _stimer_clear(&p->timerglobal);
    ae_vector_clear(&p->xprim);
    _bbgdsubproblem_clear(&p->rootsubproblem);
    ae_obj_array_clear(&p->bbsubproblems);
    _bbgdfront_clear(&p->front);
    ae_shared_pool_clear(&p->sppool);
    ae_shared_pool_clear(&p->subsolverspool);
    ae_vector_clear(&p->pseudocostsup);
    ae_vector_clear(&p->pseudocostsdown);
    ae_vector_clear(&p->pseudocostscntup);
    ae_vector_clear(&p->pseudocostscntdown);
    _hqrndstate_clear(&p->unsafeglobalrng);
    _rcommstate_clear(&p->rcommv2);
    _bbgdsubproblem_clear(&p->dummysubproblem);
    _bbgdfrontsubsolver_clear(&p->dummysubsolver);
    _ipm2state_clear(&p->dummyqpsubsolver);
    _bbgdfrontentry_clear(&p->dummyentry);
    ae_matrix_clear(&p->densedummy2);
  }

  void _bbgdstate_destroy(void* _p) {
    bbgdstate* p = (bbgdstate*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_destroy(&p->criteria);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->isintegral);
    ae_vector_destroy(&p->isbinary);
    ae_vector_destroy(&p->islinear);
    _sparsematrix_destroy(&p->obja);
    ae_vector_destroy(&p->objb);
    ae_vector_destroy(&p->qpordering);
    _sparsematrix_destroy(&p->rawa);
    ae_vector_destroy(&p->rawal);
    ae_vector_destroy(&p->rawau);
    ae_vector_destroy(&p->lcsrcidx);
    ae_vector_destroy(&p->nl);
    ae_vector_destroy(&p->nu);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->xc);
    _stimer_destroy(&p->timerglobal);
    ae_vector_destroy(&p->xprim);
    _bbgdsubproblem_destroy(&p->rootsubproblem);
    ae_obj_array_destroy(&p->bbsubproblems);
    _bbgdfront_destroy(&p->front);
    ae_shared_pool_destroy(&p->sppool);
    ae_shared_pool_destroy(&p->subsolverspool);
    ae_vector_destroy(&p->pseudocostsup);
    ae_vector_destroy(&p->pseudocostsdown);
    ae_vector_destroy(&p->pseudocostscntup);
    ae_vector_destroy(&p->pseudocostscntdown);
    _hqrndstate_destroy(&p->unsafeglobalrng);
    _rcommstate_destroy(&p->rcommv2);
    _bbgdsubproblem_destroy(&p->dummysubproblem);
    _bbgdfrontsubsolver_destroy(&p->dummysubsolver);
    _ipm2state_destroy(&p->dummyqpsubsolver);
    _bbgdfrontentry_destroy(&p->dummyentry);
    ae_matrix_destroy(&p->densedummy2);
  }

#endif
#if defined(AE_COMPILE_MIRBFVNS) || !defined(AE_PARTIAL_BUILD)

  /*************************************************************************
  MIRBFVNS solver initialization.
  --------------------------------------------------------------------------

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void mirbfvnscreatebuf(ae_int_t                       n,
                         /* Real    */ const ae_vector* bndl,
                         /* Real    */ const ae_vector* bndu,
                         /* Real    */ const ae_vector* s,
                         /* Real    */ const ae_vector* x0,
                         /* Boolean */ const ae_vector* isintegral,
                         /* Boolean */ const ae_vector* isbinary,
                         const sparsematrix*            sparsea,
                         /* Real    */ const ae_vector* al,
                         /* Real    */ const ae_vector* au,
                         /* Integer */ const ae_vector* lcsrcidx,
                         ae_int_t                       lccnt,
                         /* Real    */ const ae_vector* nl,
                         /* Real    */ const ae_vector* nu,
                         ae_int_t                       nnlc,
                         ae_int_t                       algomode,
                         ae_int_t                       budget,
                         ae_int_t                       maxneighborhood,
                         ae_int_t                       batchsize,
                         ae_int_t                       timeout,
                         ae_int_t                       tracelevel,
                         mirbfvnsstate*                 state,
                         ae_state*                      _state) {
    ae_int_t i;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jj;

    ae_assert(n >= 1, "MIRBFVNSCreateBuf: N<1", _state);
    ae_assert(x0->cnt >= n, "MIRBFVNSCreateBuf: Length(X0)<N", _state);
    ae_assert(isfinitevector(x0, n, _state),
              "MIRBFVNSCreateBuf: X contains infinite or NaN values",
              _state);
    ae_assert(bndl->cnt >= n, "MIRBFVNSCreateBuf: Length(BndL)<N", _state);
    ae_assert(bndu->cnt >= n, "MIRBFVNSCreateBuf: Length(BndU)<N", _state);
    ae_assert(s->cnt >= n, "MIRBFVNSCreateBuf: Length(S)<N", _state);
    ae_assert(isintegral->cnt >= n,
              "MIRBFVNSCreateBuf: Length(IsIntegral)<N",
              _state);
    ae_assert(
        isbinary->cnt >= n, "MIRBFVNSCreateBuf: Length(IsBinary)<N", _state);
    ae_assert(nnlc >= 0, "MIRBFVNSCreateBuf: NNLC<0", _state);
    ae_assert(nl->cnt >= nnlc, "MIRBFVNSCreateBuf: Length(NL)<NNLC", _state);
    ae_assert(nu->cnt >= nnlc, "MIRBFVNSCreateBuf: Length(NU)<NNLC", _state);
    ae_assert(budget >= 0, "MIRBFVNSCreateBuf: Length(NU)<NNLC", _state);
    ae_assert(timeout >= 0, "MIRBFVNSCreateBuf: Timeout<0", _state);
    ae_assert(((tracelevel == 0 || tracelevel == 1) || tracelevel == 2)
                  || tracelevel == 3,
              "MIRBFVNSCreateBuf: unexpected trace level",
              _state);
    ae_assert(algomode == 0 || algomode == 1,
              "MIRBFVNSCreateBuf: unexpected AlgoMode",
              _state);
    mirbfvns_initinternal(n, x0, 0, 0.0, state, _state);
    state->algomode                  = algomode;
    state->expandneighborhoodonstart = ae_true;
    state->retrylastcut              = ae_true;
    state->budget                    = budget;
    state->maxneighborhood           = maxneighborhood;
    state->batchsize                 = batchsize;
    state->timeout                   = timeout;
    state->dotrace                   = tracelevel >= 2;
    state->doextratrace              = tracelevel >= 3;
    state->dolaconictrace            = tracelevel == 1;
    state->doanytrace                = state->dotrace || state->dolaconictrace;
    for (i = 0; i <= n - 1; i++) {
      ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)
                    || ae_isneginf(bndl->ptr.p_double[i], _state),
                "MIRBFVNSCreateBuf: BndL contains NAN or +INF",
                _state);
      ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)
                    || ae_isposinf(bndu->ptr.p_double[i], _state),
                "MIRBFVNSCreateBuf: BndL contains NAN or -INF",
                _state);
      ae_assert(isintegral->ptr.p_bool[i] || !isbinary->ptr.p_bool[i],
                "MIRBFVNSCreateBuf: variable marked as binary but not integral",
                _state);
      ae_assert(ae_isfinite(s->ptr.p_double[i], _state),
                "MIRBFVNSCreateBuf: S contains infinite or NAN elements",
                _state);
      ae_assert(ae_fp_neq(s->ptr.p_double[i], (double) (0)),
                "MIRBFVNSCreateBuf: S contains zero elements",
                _state);
      state->bndl.ptr.p_double[i]  = bndl->ptr.p_double[i];
      state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
      state->bndu.ptr.p_double[i]  = bndu->ptr.p_double[i];
      state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
      state->isintegral.ptr.p_bool[i] = isintegral->ptr.p_bool[i];
      state->isbinary.ptr.p_bool[i]   = isbinary->ptr.p_bool[i];
      state->s.ptr.p_double[i]        = ae_fabs(s->ptr.p_double[i], _state);
    }
    state->lccnt                      = lccnt;
    state->haslinearlyconstrainedints = ae_false;
    if (lccnt > 0) {
      sparsecopytocrsbuf(sparsea, &state->rawa, _state);
      rcopyallocv(lccnt, al, &state->rawal, _state);
      rcopyallocv(lccnt, au, &state->rawau, _state);
      icopyallocv(lccnt, lcsrcidx, &state->lcsrcidx, _state);
      for (i = 0; i <= lccnt - 1; i++) {
        j0 = state->rawa.ridx.ptr.p_int[i];
        j1 = state->rawa.ridx.ptr.p_int[i + 1] - 1;
        for (jj = j0; jj <= j1; jj++) {
          state->haslinearlyconstrainedints
              = state->haslinearlyconstrainedints
                || isintegral->ptr.p_bool[state->rawa.idx.ptr.p_int[jj]];
        }
      }
    }
    state->nnlc = nnlc;
    rallocv(nnlc, &state->nl, _state);
    rallocv(nnlc, &state->nu, _state);
    for (i = 0; i <= nnlc - 1; i++) {
      ae_assert(ae_isfinite(nl->ptr.p_double[i], _state)
                    || ae_isneginf(nl->ptr.p_double[i], _state),
                "MIRBFVNSCreateBuf: NL[i] is +INF or NAN",
                _state);
      ae_assert(ae_isfinite(nu->ptr.p_double[i], _state)
                    || ae_isposinf(nu->ptr.p_double[i], _state),
                "MIRBFVNSCreateBuf: NU[i] is -INF or NAN",
                _state);
      state->nl.ptr.p_double[i] = nl->ptr.p_double[i];
      state->nu.ptr.p_double[i] = nu->ptr.p_double[i];
    }
  }

  /*************************************************************************
  Set tolerance for violation of nonlinear constraints

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void mirbfvnssetctol(mirbfvnsstate* state, double ctol, ae_state* _state) {
    state->ctol = ctol;
  }

  /*************************************************************************
  Set subsolver stopping condition

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void mirbfvnssetepsf(mirbfvnsstate* state, double epsf, ae_state* _state) {
    state->epsf = epsf;
  }

  /*************************************************************************
  Set subsolver stopping condition

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void mirbfvnssetepsx(mirbfvnsstate* state, double epsx, ae_state* _state) {
    state->epsx = epsx;
  }

  /*************************************************************************
  Set variable mask

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void mirbfvnssetvariablemask(mirbfvnsstate*                 state,
                               /* Boolean */ const ae_vector* hasmask,
                               const sparsematrix*            mask,
                               ae_state*                      _state) {
    state->nomask = ae_false;
    bcopyallocv(1 + state->nnlc, hasmask, &state->hasmask, _state);
    sparsecopybuf(mask, &state->varmask, _state);
  }

  /*************************************************************************
  Set adaptive internal parallelism:
  * +1 for 'favor parallelism', turn off when proved that serial is better
  *  0 for 'cautious parallelism', start serially, use SMP when proved that SMP
  is better
  * -1 for 'no adaptiveness', always start SMP when allowed to do so

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void mirbfvnssetadaptiveinternalparallelism(mirbfvnsstate* state,
                                              ae_int_t       smpmode,
                                              ae_state*      _state) {
    state->adaptiveinternalparallelism = smpmode;
  }

  /*************************************************************************


    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  ae_bool mirbfvnsiteration(mirbfvnsstate* state, ae_state* _state) {
    ae_int_t n;
    ae_int_t nnlc;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t newneighbors;
    ae_int_t offs;
    double   v;
    double   v0;
    double   v1;
    double   lcerr;
    ae_bool  bflag;
    ae_bool  result;

    /*
     * Reverse communication preparations
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if (state->rstate.stage >= 0) {
      n            = state->rstate.ia.ptr.p_int[0];
      nnlc         = state->rstate.ia.ptr.p_int[1];
      i            = state->rstate.ia.ptr.p_int[2];
      j            = state->rstate.ia.ptr.p_int[3];
      k            = state->rstate.ia.ptr.p_int[4];
      newneighbors = state->rstate.ia.ptr.p_int[5];
      offs         = state->rstate.ia.ptr.p_int[6];
      bflag        = state->rstate.ba.ptr.p_bool[0];
      v            = state->rstate.ra.ptr.p_double[0];
      v0           = state->rstate.ra.ptr.p_double[1];
      v1           = state->rstate.ra.ptr.p_double[2];
      lcerr        = state->rstate.ra.ptr.p_double[3];
    } else {
      n            = 359;
      nnlc         = -58;
      i            = -919;
      j            = -909;
      k            = 81;
      newneighbors = 255;
      offs         = 74;
      bflag        = ae_false;
      v            = 809.0;
      v0           = 205.0;
      v1           = -838.0;
      lcerr        = 939.0;
    }
    if (state->rstate.stage == 0) {
      goto lbl_0;
    }
    if (state->rstate.stage == 1) {
      goto lbl_1;
    }

    /*
     * Routine body
     */
    n    = state->n;
    nnlc = state->nnlc;
    stimerinit(&state->timerglobal, _state);
    stimerinit(&state->timerprepareneighbors, _state);
    stimerinit(&state->timerproposetrial, _state);
    stimerstart(&state->timerglobal, _state);
    mirbfvns_clearoutputs(state, _state);
    ae_shared_pool_set_seed(
        &state->tmppool,
        &state->dummytmp,
        (ae_int_t) sizeof(state->dummytmp),
        (ae_copy_constructor) _mirbfvnstemporaries_init_copy,
        (ae_destructor) _mirbfvnstemporaries_destroy,
        _state);
    ae_nxpool_alloc(
        &state->rpool, imax3(n, state->lccnt, 1 + nnlc, _state), _state);
    rsetallocv(n, 0.0, &state->maskint, _state);
    rsetallocv(n, 0.0, &state->maskfrac, _state);
    isetallocv(n, -1, &state->idxint, _state);
    isetallocv(n, -1, &state->idxfrac, _state);
    state->nfrac = 0;
    state->nint  = 0;
    for (i = 0; i <= n - 1; i++) {
      if (state->isintegral.ptr.p_bool[i]) {
        state->maskint.ptr.p_double[i]       = 1.0;
        state->idxint.ptr.p_int[state->nint] = i;
        state->nint                          = state->nint + 1;
      } else {
        state->maskfrac.ptr.p_double[i]        = 1.0;
        state->idxfrac.ptr.p_int[state->nfrac] = i;
        state->nfrac                           = state->nfrac + 1;
      }
    }
    rcopyallocv(n, &state->x0, &state->xc, _state);
    rallocv(n, &state->tmpx1, _state);
    rallocv(1 + state->nnlc, &state->tmpf1, _state);
    state->prepareevaluationbatchparallelism
        = state->adaptiveinternalparallelism == 1
          || state->adaptiveinternalparallelism == -1;
    state->expandcutgenerateneighborsparallelism
        = state->adaptiveinternalparallelism == 1
          || state->adaptiveinternalparallelism == -1;
    if (state->doanytrace) {
      ae_trace("> preparing initial point\n");
    }
    rsetallocv(
        n, -ae_sqrt(ae_maxrealnumber, _state), &state->finitebndl, _state);
    rsetallocv(
        n, ae_sqrt(ae_maxrealnumber, _state), &state->finitebndu, _state);
    for (i = 0; i <= n - 1; i++) {
      if (state->hasbndl.ptr.p_bool[i] && state->hasbndu.ptr.p_bool[i]) {
        if (ae_fp_less(state->bndu.ptr.p_double[i],
                       state->bndl.ptr.p_double[i])) {
          if (state->doanytrace) {
            ae_trace(">> error: box constraint %0d is infeasible (bndU<bndL)\n",
                     (int) (i));
          }
          state->repterminationtype = -3;
          result                    = ae_false;
          return result;
        }
        if (state->isintegral.ptr.p_bool[i]
            && ae_fp_less(
                (double) (ae_ifloor(state->bndu.ptr.p_double[i], _state)),
                state->bndl.ptr.p_double[i])) {
          if (state->doanytrace) {
            ae_trace(">> error: box constraint %0d is incompatible with "
                     "integrality constraints\n",
                     (int) (i));
          }
          state->repterminationtype = -3;
          result                    = ae_false;
          return result;
        }
      }
      if (state->isintegral.ptr.p_bool[i]) {
        if (state->hasbndl.ptr.p_bool[i]
            && ae_fp_neq(
                state->bndl.ptr.p_double[i],
                (double) (ae_round(state->bndl.ptr.p_double[i], _state)))) {
          state->bndl.ptr.p_double[i]
              = (double) (ae_iceil(state->bndl.ptr.p_double[i], _state));
        }
        if (state->hasbndu.ptr.p_bool[i]
            && ae_fp_neq(
                state->bndu.ptr.p_double[i],
                (double) (ae_round(state->bndu.ptr.p_double[i], _state)))) {
          state->bndu.ptr.p_double[i]
              = (double) (ae_ifloor(state->bndu.ptr.p_double[i], _state));
        }
      }
      if (state->isbinary.ptr.p_bool[i]) {
        if (ae_isneginf(state->bndl.ptr.p_double[i], _state)
            || ae_fp_less(state->bndl.ptr.p_double[i], (double) (0))) {
          state->bndl.ptr.p_double[i] = (double) (0);
        }
        if (ae_isposinf(state->bndu.ptr.p_double[i], _state)
            || ae_fp_greater(state->bndu.ptr.p_double[i], (double) (1))) {
          state->bndu.ptr.p_double[i] = (double) (1);
        }
      }
      state->hasbndl.ptr.p_bool[i]
          = state->hasbndl.ptr.p_bool[i]
            || ae_isfinite(state->bndl.ptr.p_double[i], _state);
      state->hasbndu.ptr.p_bool[i]
          = state->hasbndu.ptr.p_bool[i]
            || ae_isfinite(state->bndu.ptr.p_double[i], _state);
      if (state->hasbndl.ptr.p_bool[i]) {
        state->finitebndl.ptr.p_double[i] = state->bndl.ptr.p_double[i];
      }
      if (state->hasbndu.ptr.p_bool[i]) {
        state->finitebndu.ptr.p_double[i] = state->bndu.ptr.p_double[i];
      }
    }
    bflag = mirbfvns_prepareinitialpoint(state, &state->xc, &lcerr, _state);
    if (!bflag || ae_fp_greater(lcerr, state->ctol)) {
      if (state->doanytrace) {
        ae_trace(">> error: box, linear and integrality constraints together "
                 "are inconsistent; declaring infeasibility\n");
      }
      state->repterminationtype = -3;
      result                    = ae_false;
      return result;
    }
    rcopyallocv(n, &state->xc, &state->querydata, _state);
    rallocv(1 + nnlc, &state->replyfi, _state);
    state->requesttype  = 4;
    state->querysize    = 1;
    state->queryfuncs   = 1 + nnlc;
    state->queryvars    = n;
    state->querydim     = 0;
    state->rstate.stage = 0;
    if (state->rstate.rcomm2_handler != NULL && state->rstate.requesttype != 0
        && state->rstate.requesttype <= _ALGLIB_MAX_RCOMMV2_REQUEST)
      state->rstate.rcomm2_handler(&state->rstate,
                                   state->rstate.handler_p0,
                                   state->rstate.handler_p1,
                                   state->rstate.handler_p2,
                                   state->rstate.handler_p3,
                                   _state);
    else
      goto lbl_rcomm;
  lbl_0:
    state->repnfev = state->repnfev + 1;
    if (!isfinitevector(&state->replyfi, 1 + nnlc, _state)) {
      if (state->doanytrace) {
        ae_trace(
            ">> error: the initial point has objective or one of nonlinear "
            "constraints equal to NAN/INF\n>> unable to restore, stop\n");
      }
      state->repterminationtype = -3;
      result                    = ae_false;
      return result;
    }
    state->fc = state->replyfi.ptr.p_double[0];
    mirbfvns_computeviolation2(
        state, &state->xc, &state->replyfi, &state->hc, &state->mxc, _state);
    mirbfvns_datasetinitempty(&state->dataset, state, _state);
    state->nodec = mirbfvns_gridcreate(&state->grid,
                                       state,
                                       &state->xc,
                                       &state->replyfi,
                                       state->hc,
                                       state->mxc,
                                       _state);
    rsetallocv(n, 0.0, &state->nodecproducedbycut, _state);
    if (state->doanytrace) {
      ae_trace(">> done; the initial grid node N%0d is created\n",
               (int) (state->nodec));
    }
    state->repterminationtype = 0;
    state->outofbudget        = ae_false;
  lbl_2:
    if (ae_false) {
      goto lbl_3;
    }
    isetallocv(1, state->nodec, &state->xcneighbors, _state);
    isetallocv(1, state->nodec, &state->xcreachedfrom, _state);
    bsetallocv(1, ae_false, &state->xcqueryflags, _state);
    rgrowrowsfixedcolsm(1, n, &state->xcreachedbycut, _state);
    rsetr(n, 0.0, &state->xcreachedbycut, 0, _state);
    state->xcneighborscnt         = 1;
    state->xcpriorityneighborscnt = 1;
    if (state->expandneighborhoodonstart) {
      mirbfvns_expandneighborhood(state, _state);
    }
    if (state->dotrace) {
      ae_trace("> starting search from node %0d", (int) (state->nodec));
      if (state->xcneighborscnt > 1) {
        ae_trace(" with expanded neighborhood of %0d nodes",
                 (int) (state->xcneighborscnt - 1));
      }
      ae_trace(" (F=%0.6e,H=%0.6e)\n",
               (double) (mirbfvns_gridgetfbest(
                   &state->grid, state, state->nodec, _state)),
               (double) (mirbfvns_gridgethbest(
                   &state->grid, state, state->nodec, _state)));
    }
  lbl_4:
    if (ae_false) {
      goto lbl_5;
    }
    k = state->nodec;
    j = 0;
    for (i = 0; i <= state->xcneighborscnt - 1; i++) {
      if (mirbfvns_gridisbetter(&state->grid,
                                state,
                                k,
                                state->xcneighbors.ptr.p_int[i],
                                _state)) {
        k = state->xcneighbors.ptr.p_int[i];
        j = i;
      }
    }
    if (k != state->nodec) {
      if (state->dotrace) {
        ae_trace(
            ">> found better neighbor, switching to node %0d "
            "(F=%0.6e,H=%0.6e)\n",
            (int) (k),
            (double) (mirbfvns_gridgetfbest(&state->grid, state, k, _state)),
            (double) (mirbfvns_gridgethbest(&state->grid, state, k, _state)));
      }
      state->nodec = k;
      rcopyrv(n, &state->xcreachedbycut, j, &state->nodecproducedbycut, _state);
      goto lbl_5;
    }
    if (state->budget > 0 && state->repnfev >= state->budget) {
      if (state->doanytrace) {
        ae_trace("> iteration budget exhausted, stopping\n");
      }
      state->repterminationtype = 5;
      state->outofbudget        = ae_true;
      goto lbl_5;
    }
    if (state->timeout > 0
        && ae_fp_greater(stimergetmsrunning(&state->timerglobal, _state),
                         (double) (state->timeout))) {
      if (state->doanytrace) {
        ae_trace("> time budget exhausted, stopping\n");
      }
      state->repterminationtype = 5;
      state->outofbudget        = ae_true;
      goto lbl_5;
    }
    mirbfvns_prepareevaluationbatch(state, _state);
    if (state->evalbatchsize <= 0) {
      goto lbl_6;
    }
    state->requesttype = 4;
    state->querysize   = state->evalbatchsize;
    state->queryfuncs  = 1 + nnlc;
    state->queryvars   = n;
    state->querydim    = 0;
    rallocv(n * state->evalbatchsize, &state->querydata, _state);
    offs = 0;
    for (i = 0; i <= state->evalbatchsize - 1; i++) {
      for (k = 0; k <= n - 1; k++) {
        state->querydata.ptr.p_double[offs + k]
            = state->evalbatchpoints.ptr.pp_double[i][k];
      }
      offs = offs + n;
    }
    rallocv((1 + nnlc) * state->evalbatchsize, &state->replyfi, _state);
    state->rstate.stage = 1;
    if (state->rstate.rcomm2_handler != NULL && state->rstate.requesttype != 0
        && state->rstate.requesttype <= _ALGLIB_MAX_RCOMMV2_REQUEST)
      state->rstate.rcomm2_handler(&state->rstate,
                                   state->rstate.handler_p0,
                                   state->rstate.handler_p1,
                                   state->rstate.handler_p2,
                                   state->rstate.handler_p3,
                                   _state);
    else
      goto lbl_rcomm;
  lbl_1:
    state->repnfev = state->repnfev + state->evalbatchsize;
    rallocv(n, &state->xtrial, _state);
    rallocv(1 + nnlc, &state->trialfi, _state);
    for (i = 0; i <= state->evalbatchsize - 1; i++) {
      rcopyrv(n, &state->evalbatchpoints, i, &state->xtrial, _state);
      rcopyvx(1 + nnlc,
              &state->replyfi,
              i * (1 + nnlc),
              &state->trialfi,
              0,
              _state);
      mirbfvns_gridsendtrialpointto(&state->grid,
                                    state,
                                    state->nodec,
                                    state->evalbatchnodeidx.ptr.p_int[i],
                                    &state->xtrial,
                                    &state->trialfi,
                                    _state);
    }
    if (state->dotrace) {
      if (mirbfvns_gridgetbestlastacceptedinunsolvedneighborhood(
              &state->grid,
              state,
              &state->xcneighbors,
              state->xcneighborscnt,
              &k,
              &v,
              &v0,
              &v1,
              _state)) {
        ae_trace(">> improving neighborhood, %3d evaluations, best unsolved "
                 "node is %6d: f=%15.6e, sum(viol)=%0.2e, max(viol)=%0.2e\n",
                 (int) (state->evalbatchsize),
                 (int) (k),
                 (double) (v),
                 (double) (v0),
                 (double) (v1));
      } else {
        mirbfvns_gridgetbestinneighborhood(&state->grid,
                                           state,
                                           &state->xcneighbors,
                                           state->xcneighborscnt,
                                           &v,
                                           &v0,
                                           &v1,
                                           _state);
        ae_trace(">> improving neighborhood, %3d evaluations, all nodes are  "
                 "solved,  best:  f=%15.6e, sum(viol)=%0.2e, max(viol)=%0.2e\n",
                 (int) (state->evalbatchsize),
                 (double) (v),
                 (double) (v0),
                 (double) (v1));
      }
    }
    if (state->evalbatchsize < state->batchsize
        && (state->maxneighborhood == 0
            || state->xcneighborscnt - 1 < state->maxneighborhood)) {
      newneighbors = mirbfvns_expandneighborhood(state, _state);
      if (state->dotrace) {
        ae_trace(
            "> evaluation batch is not fully used (%0d out of %0d), expanding "
            "neighborhood: %0d new neighbor(s), |neighborhood|=%0d\n",
            (int) (state->evalbatchsize),
            (int) (state->batchsize),
            (int) (newneighbors),
            (int) (state->xcneighborscnt));
      }
    }
    goto lbl_4;
  lbl_6:
    if (state->maxneighborhood > 0
        && state->xcneighborscnt - 1 >= state->maxneighborhood) {
      if (state->doanytrace) {
        ae_trace("> the neighborhood size exceeds limit (%0d+1), stopping\n",
                 (int) (state->maxneighborhood));
      }
      state->repterminationtype = 2;
      goto lbl_5;
    }
    newneighbors = mirbfvns_expandneighborhood(state, _state);
    if (newneighbors == 0) {
      if (state->doanytrace) {
        ae_trace("> the integer grid was completely scanned, stopping\n");
      }
      state->repterminationtype = 1;
      goto lbl_5;
    }
    if (state->dotrace) {
      ae_trace(">> expanding neighborhood, %0d new neighbor(s), "
               "|neighborhood|=%0d\n",
               (int) (newneighbors),
               (int) (state->xcneighborscnt));
    }
    goto lbl_4;
  lbl_5:
    if (state->repterminationtype != 0) {
      goto lbl_3;
    }
    goto lbl_2;
  lbl_3:
    mirbfvns_gridoffloadbestpoint(&state->grid,
                                  state,
                                  state->nodec,
                                  &state->xc,
                                  &k,
                                  &state->fc,
                                  &state->hc,
                                  &state->mxc,
                                  _state);
    if (ae_fp_greater(state->mxc, state->ctol)) {
      state->repterminationtype = icase2(state->outofbudget, -33, -3, _state);
    }
    stimerstop(&state->timerglobal, _state);
    if (state->doanytrace) {
      ae_trace("\n=== STOPPED "
               "==============================================================="
               "=========================\n");
      ae_trace("raw target:     %20.12e\n", (double) (state->fc));
      ae_trace("max.violation:  %20.12e\n", (double) (state->mxc));
      ae_trace("evaluations:    %6d\n", (int) (state->repnfev));
      ae_trace("subsolver its:  %6d\n", (int) (state->repsubsolverits));
      ae_trace("integral nodes: %6d\n", (int) (state->grid.nnodes));
      ae_trace("total time:     %10.1f ms (wall-clock)\n",
               (double) (stimergetms(&state->timerglobal, _state)));
      ae_trace("\nDetailed time (wall-clock):\n");
      ae_trace("gen neighbors:  %10.1f ms (wall-clock)\n",
               (double) (stimergetms(&state->timerprepareneighbors, _state)));
      ae_trace("propose trial:  %10.1f ms (wall-clock)\n",
               (double) (stimergetms(&state->timerproposetrial, _state)));
      ae_trace("\nAdvanced statistics:\n");
      ae_trace("> neighborhood-generating cuts:\n");
      ae_trace("avg.time:       %0.1f ms\n",
               (double) ((double) state->cuttimems
                         / (double) ae_maxint(state->cutcnt, 1, _state)));
      ae_trace("count:          %0d\n", (int) (state->cutcnt));
      ae_trace(">> sequential and potentially parallel rounds:\n");
      ae_trace("sequential:     %0d\n", (int) (state->dbgsequentialcutrounds));
      ae_trace("parallel:       %0d\n",
               (int) (state->dbgpotentiallyparallelcutrounds));
      ae_trace("> integer node explorations and continuous subspace searches "
               "(callback time not included):\n");
      ae_trace(">> initial exploration:\n");
      ae_trace("avg.time:       %0.1f ms\n",
               (double) ((double) state->explorativetrialtimems
                         / (double) ae_maxint(
                             state->explorativetrialcnt, 1, _state)));
      ae_trace("count:          %0d\n", (int) (state->explorativetrialcnt));
      ae_trace(">> random sampling around initial point:\n");
      ae_trace("avg.time:       %0.1f ms\n",
               (double) ((double) state->localtrialsamplingtimems
                         / (double) ae_maxint(
                             state->localtrialsamplingcnt, 1, _state)));
      ae_trace("count:          %0d\n", (int) (state->localtrialsamplingcnt));
      ae_trace(">> surrogate model optimizations:\n");
      ae_trace(
          "avg.time:       %0.1f ms\n",
          (double) ((double) state->localtrialrbftimems
                    / (double) ae_maxint(state->localtrialrbfcnt, 1, _state)));
      ae_trace("count:          %0d\n", (int) (state->localtrialrbfcnt));
      ae_trace(">> sequential and potentially parallel batches:\n");
      ae_trace("sequential:     %0d\n", (int) (state->dbgsequentialbatches));
      ae_trace("parallel:       %0d\n",
               (int) (state->dbgpotentiallyparallelbatches));
    }
    result = ae_false;
    return result;

    /*
     * Saving state
     */
  lbl_rcomm:
    result                           = ae_true;
    state->rstate.ia.ptr.p_int[0]    = n;
    state->rstate.ia.ptr.p_int[1]    = nnlc;
    state->rstate.ia.ptr.p_int[2]    = i;
    state->rstate.ia.ptr.p_int[3]    = j;
    state->rstate.ia.ptr.p_int[4]    = k;
    state->rstate.ia.ptr.p_int[5]    = newneighbors;
    state->rstate.ia.ptr.p_int[6]    = offs;
    state->rstate.ba.ptr.p_bool[0]   = bflag;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = v0;
    state->rstate.ra.ptr.p_double[2] = v1;
    state->rstate.ra.ptr.p_double[3] = lcerr;
    return result;
  }

  /*************************************************************************
  Clears output fields during initialization
  *************************************************************************/
  static void mirbfvns_clearoutputs(mirbfvnsstate* state, ae_state* _state) {
    state->userterminationneeded = ae_false;
    state->repnfev               = 0;
    state->repsubsolverits       = 0;
    state->repiterationscount    = 0;
    state->repterminationtype    = 0;
  }

  /*************************************************************************
  Internal initialization subroutine.
  Sets default NLC solver with default criteria.
  *************************************************************************/
  static void mirbfvns_initinternal(ae_int_t                       n,
                                    /* Real    */ const ae_vector* x,
                                    ae_int_t                       solvermode,
                                    double                         diffstep,
                                    mirbfvnsstate*                 state,
                                    ae_state*                      _state) {
    ae_frame  _frame_block;
    ae_int_t  i;
    ae_matrix c;
    ae_vector ct;

    ae_frame_make(_state, &_frame_block);
    memset(&c, 0, sizeof(c));
    memset(&ct, 0, sizeof(ct));
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ct, 0, DT_INT, _state, ae_true);

    state->convexityflag = 0;
    critinitdefault(&state->criteria, _state);
    state->adaptiveinternalparallelism = 0;
    state->timeout                     = 0;
    state->ctol                        = 1.0E-5;
    state->epsf                        = 1.0E-5;
    state->epsx                        = 1.0E-5;
    state->quickepsf                   = 0.01;
    state->n                           = n;
    state->userterminationneeded       = ae_false;
    bsetallocv(n, ae_false, &state->isintegral, _state);
    bsetallocv(n, ae_false, &state->isbinary, _state);
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->hasbndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->hasbndu, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->x0, n, _state);
    ae_vector_set_length(&state->xc, n, _state);
    for (i = 0; i <= n - 1; i++) {
      state->bndl.ptr.p_double[i]  = _state->v_neginf;
      state->hasbndl.ptr.p_bool[i] = ae_false;
      state->bndu.ptr.p_double[i]  = _state->v_posinf;
      state->hasbndu.ptr.p_bool[i] = ae_false;
      state->s.ptr.p_double[i]     = 1.0;
      state->x0.ptr.p_double[i]    = x->ptr.p_double[i];
      state->xc.ptr.p_double[i]    = x->ptr.p_double[i];
    }
    state->hasx0  = ae_true;
    state->lccnt  = 0;
    state->nnlc   = 0;
    state->nomask = ae_true;
    mirbfvns_clearoutputs(state, _state);
    state->explorativetrialcnt             = 0;
    state->explorativetrialtimems          = 0;
    state->localtrialsamplingcnt           = 0;
    state->localtrialsamplingtimems        = 0;
    state->localtrialrbfcnt                = 0;
    state->localtrialrbftimems             = 0;
    state->cutcnt                          = 0;
    state->cuttimems                       = 0;
    state->dbgpotentiallyparallelbatches   = 0;
    state->dbgsequentialbatches            = 0;
    state->dbgpotentiallyparallelcutrounds = 0;
    state->dbgsequentialcutrounds          = 0;
    hqrndseed(8543, 7455, &state->unsafeglobalrng, _state);
    ae_vector_set_length(&state->rstate.ia, 6 + 1, _state);
    ae_vector_set_length(&state->rstate.ba, 0 + 1, _state);
    ae_vector_set_length(&state->rstate.ra, 3 + 1, _state);
    state->rstate.stage = -1;
    _rcommstate_clear_handler(&state->rstate);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Prepare initial point that is feasible with respect to integrality, box
  and linear constraints (but may potentially violate nonlinear ones).

  X contains current approximation that is replaced by a point satisfying
  constraints. Error in constraints is returned as a result.

  If it is impossible to satisfy box, integrality and linear constraints
  simultaneously, an integer+box feasible point is returned, and LCErr is
  set to an error at this point.

  Box and integrality constraints are assumed to be compatible.

  Returns True on success, False on failure.
  *************************************************************************/
  static ae_bool mirbfvns_prepareinitialpoint(mirbfvnsstate*           state,
                                              /* Real    */ ae_vector* x,
                                              double*                  lcerr,
                                              ae_state*                _state) {
    ae_int_t n;
    ae_int_t i;
    ae_bool  result;

    *lcerr = 0.0;

    n      = state->n;
    result = ae_true;
    *lcerr = (double) (0);
    for (i = 0; i <= n - 1; i++) {
      if (state->hasbndl.ptr.p_bool[i]) {
        x->ptr.p_double[i] = ae_maxreal(
            x->ptr.p_double[i], state->bndl.ptr.p_double[i], _state);
      }
      if (state->hasbndu.ptr.p_bool[i]) {
        x->ptr.p_double[i] = ae_minreal(
            x->ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
      }
      if (state->isintegral.ptr.p_bool[i]) {
        x->ptr.p_double[i] = (double) (ae_round(x->ptr.p_double[i], _state));
      }
    }
    if (!state->haslinearlyconstrainedints) {
      return result;
    }
    rsetallocm(1, n, 0.0, &state->xucuts, _state);
    rsetallocm(1, n, 0.0, &state->xupoints, _state);
    bsetallocv(1, ae_false, &state->xuflags, _state);
    mirbfvns_findnearestintegralsubjecttocut(state,
                                             x,
                                             &state->xucuts,
                                             &state->xupoints,
                                             &state->xuflags,
                                             0,
                                             ae_false,
                                             _state);
    result = state->xuflags.ptr.p_bool[0];
    *lcerr = (double) (0);
    if (result) {
      rcopyrv(n, &state->xupoints, 0, x, _state);
    }
    return result;
  }

  /*************************************************************************
  Given current neighborhood (one stored in  State.XCNeighbors[]),  prepares
  up to State.BatchSize evaluation requests, depending on neighbor priorities,
  statuses (unexplored or in progress) and other factors.

  The evaluation batch is stored into:
  * EvalBatchSize, >=0
  * EvalBatchPoints, array[EvalBatchSize,N]
  * EvalBatchNodeIdx, array[EvalBatchSize], node index in the grid
  * EvalBatchNeighborIdx, array[EvalBatchSize], node index in XCNeighbors[]
  array
  *************************************************************************/
  static void mirbfvns_prepareevaluationbatch(mirbfvnsstate* state,
                                              ae_state*      _state) {
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t cntu;
    ae_int_t st;
    ae_int_t n;
    ae_bool  addednodec;
    ae_int_t expectedexplorationcnt;
    ae_int_t expectedsamplingcnt;
    ae_int_t expectedrbfcnt;
    double   avgexplorationtime;
    double   avgsamplingtime;
    double   avgrbftime;
    double   expectedexplorationtime;
    double   expectedsamplingtime;
    double   expectedrbftime;
    ae_bool  sufficienttime;
    ae_bool  sufficientcount;

    n                    = state->n;
    state->evalbatchsize = 0;
    addednodec           = ae_false;
    ae_assert(
        state->nodec == state->xcneighbors.ptr.p_int[0]
            && mirbfvns_gridgetstatus(
                   &state->grid, state, state->xcneighbors.ptr.p_int[0], _state)
                   != mirbfvns_nodeunexplored,
        "MIRBFVNS: 989642 failed",
        _state);
    for (i = 0; i <= state->xcpriorityneighborscnt - 1; i++) {
      if (state->evalbatchsize < state->batchsize
          && mirbfvns_gridgetstatus(
                 &state->grid, state, state->xcneighbors.ptr.p_int[i], _state)
                 == mirbfvns_nodeunexplored) {
        igrowappendv(state->evalbatchsize + 1,
                     &state->evalbatchnodeidx,
                     state->xcneighbors.ptr.p_int[i],
                     _state);
        igrowappendv(
            state->evalbatchsize + 1, &state->evalbatchneighboridx, i, _state);
        state->evalbatchsize = state->evalbatchsize + 1;
      }
    }
    if (state->evalbatchsize < state->batchsize) {
      cntu = 0;
      for (i = state->xcpriorityneighborscnt; i <= state->xcneighborscnt - 1;
           i++) {
        if (mirbfvns_gridgetstatus(
                &state->grid, state, state->xcneighbors.ptr.p_int[i], _state)
            == mirbfvns_nodeunexplored) {
          igrowappendv(cntu + 1,
                       &state->tmpeb0,
                       state->xcneighbors.ptr.p_int[i],
                       _state);
          igrowappendv(cntu + 1, &state->tmpeb2, i, _state);
          cntu = cntu + 1;
        }
      }
      for (i = 0; i <= cntu - 2; i++) {
        j = i + hqrnduniformi(&state->unsafeglobalrng, cntu - i, _state);
        k = state->tmpeb0.ptr.p_int[i];
        state->tmpeb0.ptr.p_int[i] = state->tmpeb0.ptr.p_int[j];
        state->tmpeb0.ptr.p_int[j] = k;
        k                          = state->tmpeb2.ptr.p_int[i];
        state->tmpeb2.ptr.p_int[i] = state->tmpeb2.ptr.p_int[j];
        state->tmpeb2.ptr.p_int[j] = k;
      }
      i = 0;
      while (state->evalbatchsize < state->batchsize && i < cntu) {
        igrowappendv(state->evalbatchsize + 1,
                     &state->evalbatchnodeidx,
                     state->tmpeb0.ptr.p_int[i],
                     _state);
        igrowappendv(state->evalbatchsize + 1,
                     &state->evalbatchneighboridx,
                     state->tmpeb2.ptr.p_int[i],
                     _state);
        state->evalbatchsize = state->evalbatchsize + 1;
        i                    = i + 1;
      }
    }
    ae_assert(state->nodec == state->xcneighbors.ptr.p_int[0]
                  && !ilinearsearchispresent(&state->evalbatchnodeidx,
                                             0,
                                             state->evalbatchsize,
                                             state->nodec,
                                             _state),
              "MIRBFVNS: 023353 failed",
              _state);
    if (mirbfvns_gridgetstatus(&state->grid, state, state->nodec, _state)
        == mirbfvns_nodeinprogress) {
      if (state->evalbatchsize < state->batchsize
          && (state->batchsize > 1
              || ae_fp_less(hqrnduniformr(&state->unsafeglobalrng, _state),
                            0.5))) {
        igrowappendv(state->evalbatchsize + 1,
                     &state->evalbatchnodeidx,
                     state->nodec,
                     _state);
        igrowappendv(
            state->evalbatchsize + 1, &state->evalbatchneighboridx, 0, _state);
        state->evalbatchsize = state->evalbatchsize + 1;
        addednodec           = ae_true;
      }
    }
    if (state->evalbatchsize < state->batchsize) {
      cntu = 0;
      for (i = icase2(addednodec, 1, 0, _state); i <= state->xcneighborscnt - 1;
           i++) {
        st = mirbfvns_gridgetstatus(
            &state->grid, state, state->xcneighbors.ptr.p_int[i], _state);
        if ((st == mirbfvns_nodeunexplored || st == mirbfvns_nodesolved)
            || st == mirbfvns_nodebad) {
          continue;
        }
        if (st == mirbfvns_nodeinprogress) {
          igrowappendv(cntu + 1,
                       &state->tmpeb0,
                       state->xcneighbors.ptr.p_int[i],
                       _state);
          igrowappendv(cntu + 1, &state->tmpeb2, i, _state);
          cntu = cntu + 1;
          continue;
        }
        ae_assert(ae_false, "MIRBFVNS: 047402 failed", _state);
      }
      for (i = 0; i <= cntu - 2; i++) {
        j = i + hqrnduniformi(&state->unsafeglobalrng, cntu - i, _state);
        k = state->tmpeb0.ptr.p_int[i];
        state->tmpeb0.ptr.p_int[i] = state->tmpeb0.ptr.p_int[j];
        state->tmpeb0.ptr.p_int[j] = k;
        k                          = state->tmpeb2.ptr.p_int[i];
        state->tmpeb2.ptr.p_int[i] = state->tmpeb2.ptr.p_int[j];
        state->tmpeb2.ptr.p_int[j] = k;
      }
      i = 0;
      while (state->evalbatchsize < state->batchsize && i < cntu) {
        igrowappendv(state->evalbatchsize + 1,
                     &state->evalbatchnodeidx,
                     state->tmpeb0.ptr.p_int[i],
                     _state);
        igrowappendv(state->evalbatchsize + 1,
                     &state->evalbatchneighboridx,
                     state->tmpeb2.ptr.p_int[i],
                     _state);
        state->evalbatchsize = state->evalbatchsize + 1;
        i                    = i + 1;
      }
    }
    if (state->evalbatchsize > 0) {
      rgrowrowsfixedcolsm(
          state->evalbatchsize, n, &state->evalbatchpoints, _state);
      iallocv(2 * state->evalbatchsize, &state->tmpeb0, _state);
      avgexplorationtime = (double) state->explorativetrialtimems
                           / coalesce((double) (state->explorativetrialcnt),
                                      (double) (1),
                                      _state);
      avgsamplingtime = (double) state->localtrialsamplingtimems
                        / coalesce((double) (state->localtrialsamplingcnt),
                                   (double) (1),
                                   _state);
      avgrbftime = (double) state->localtrialrbftimems
                   / coalesce((double) (state->localtrialrbfcnt),
                              (double) (1),
                              _state);
      expectedexplorationtime = (double) (0);
      expectedexplorationcnt  = 0;
      expectedsamplingtime    = (double) (0);
      expectedsamplingcnt     = 0;
      expectedrbftime         = (double) (0);
      expectedrbfcnt          = 0;
      for (i = 0; i <= state->evalbatchsize - 1; i++) {
        ae_assert(state->evalbatchnodeidx.ptr.p_int[i]
                      == state->xcneighbors.ptr
                             .p_int[state->evalbatchneighboridx.ptr.p_int[i]],
                  "MIRBFVNS: 089649",
                  _state);
        state->tmpeb0.ptr.p_int[2 * i + 0]
            = hqrnduniformi(&state->unsafeglobalrng, 1000000, _state);
        state->tmpeb0.ptr.p_int[2 * i + 1]
            = hqrnduniformi(&state->unsafeglobalrng, 1000000, _state);
        if (mirbfvns_gridgetstatus(&state->grid,
                                   state,
                                   state->evalbatchnodeidx.ptr.p_int[i],
                                   _state)
            == mirbfvns_nodeunexplored) {
          expectedexplorationtime
              = expectedexplorationtime + avgexplorationtime;
          expectedexplorationcnt = expectedexplorationcnt + 1;
        } else {
          if (ae_fp_less(mirbfvns_gridgetpointscountinnode(
                             &state->grid,
                             state,
                             state->evalbatchnodeidx.ptr.p_int[i],
                             _state),
                         (double) (state->nfrac + 1))) {
            expectedsamplingtime = expectedsamplingtime + avgsamplingtime;
            expectedsamplingcnt  = expectedsamplingcnt + 1;
          } else {
            expectedrbftime = expectedrbftime + avgrbftime;
            expectedrbfcnt  = expectedrbfcnt + 1;
          }
        }
      }
      sufficienttime  = ae_fp_greater((double) (state->explorativetrialtimems
                                               + state->localtrialsamplingtimems
                                               + state->localtrialrbftimems),
                                     adaptiveparallelismtimerequired(_state));
      sufficientcount = ae_fp_greater((double) (state->explorativetrialcnt
                                                + state->localtrialsamplingcnt
                                                + state->localtrialrbfcnt),
                                      adaptiveparallelismcountrequired(_state));
      if (state->adaptiveinternalparallelism >= 0
          && (sufficienttime || sufficientcount)) {
        state->prepareevaluationbatchparallelism = ae_false;
        state->prepareevaluationbatchparallelism
            = state->prepareevaluationbatchparallelism
              || (ae_fp_greater_eq(expectedexplorationtime,
                                   workerstartthresholdms(_state))
                  && expectedexplorationcnt >= 2);
        state->prepareevaluationbatchparallelism
            = state->prepareevaluationbatchparallelism
              || (ae_fp_greater_eq(expectedsamplingtime,
                                   workerstartthresholdms(_state))
                  && expectedsamplingcnt >= 2);
        state->prepareevaluationbatchparallelism
            = state->prepareevaluationbatchparallelism
              || (ae_fp_greater_eq(expectedrbftime,
                                   workerstartthresholdms(_state))
                  && expectedrbfcnt >= 2);
      }
      stimerstartcond(&state->timerproposetrial, state->doanytrace, _state);
      mirbfvns_gridparallelproposelocaltrialpoint(
          &state->grid,
          &state->grid,
          state,
          state,
          0,
          state->evalbatchsize,
          ae_true,
          state->prepareevaluationbatchparallelism,
          _state);
      stimerstopcond(&state->timerproposetrial, state->doanytrace, _state);
      if (state->prepareevaluationbatchparallelism) {
        state->dbgpotentiallyparallelbatches
            = state->dbgpotentiallyparallelbatches + 1;
      } else {
        state->dbgsequentialbatches = state->dbgsequentialbatches + 1;
      }
    }
  }

  /*************************************************************************
  Expands current neighborhood (one stored in State.XCNeighbors[]) with  new
  neighbors

  The function returns the number of added neighbors. Zero means that the
  entire integer grid was scanned.

  Changes XCNeighbors[], XCReachedFrom[], XCNeighborsCnt, XCQueryFlags[].

  Uses XUNeighbors for temporary storage.
  *************************************************************************/
  static ae_int_t mirbfvns_expandneighborhood(mirbfvnsstate* state,
                                              ae_state*      _state) {
    ae_frame  _frame_block;
    ae_int_t  n;
    ae_int_t  i;
    ae_int_t  nncnt;
    ae_int_t  unmarked;
    ae_int_t  unmarkedi;
    ae_int_t  unsolvedcnt;
    ae_int_t  newunsolvedcnt;
    ae_int_t  st;
    ae_vector xc;
    ae_vector unmarkedcut;
    ae_int_t  idummy;
    double    fc;
    double    hc;
    double    mxc;
    ae_int_t  result;

    ae_frame_make(_state, &_frame_block);
    memset(&xc, 0, sizeof(xc));
    memset(&unmarkedcut, 0, sizeof(unmarkedcut));
    ae_vector_init(&xc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&unmarkedcut, 0, DT_REAL, _state, ae_true);

    n = state->n;
    ae_nxpool_retrieve(&state->rpool, &xc, _state);
    ae_nxpool_retrieve(&state->rpool, &unmarkedcut, _state);
    ae_assert(xc.cnt >= n && unmarkedcut.cnt >= n, "MIRBFVNS: 944420", _state);
    result      = 0;
    unsolvedcnt = 0;
    while (unsolvedcnt < state->batchsize) {
      unmarked  = -1;
      unmarkedi = -1;
      for (i = 0; i <= state->xcneighborscnt - 1; i++) {
        if (!state->xcqueryflags.ptr.p_bool[i]) {
          st = mirbfvns_gridgetstatus(
              &state->grid, state, state->xcneighbors.ptr.p_int[i], _state);
          if (st == mirbfvns_nodebad || st == mirbfvns_nodeunexplored) {
            continue;
          }
          ae_assert(st == mirbfvns_nodesolved || st == mirbfvns_nodeinprogress,
                    "MIRBFVNS: 964543",
                    _state);
          if (unmarked < 0
              || mirbfvns_gridisbetter(&state->grid,
                                       state,
                                       unmarked,
                                       state->xcneighbors.ptr.p_int[i],
                                       _state)) {
            unmarked  = state->xcneighbors.ptr.p_int[i];
            unmarkedi = i;
          }
        }
      }
      if (unmarked < 0) {
        break;
      }
      mirbfvns_gridoffloadbestpoint(&state->grid,
                                    state,
                                    state->nodec,
                                    &xc,
                                    &idummy,
                                    &fc,
                                    &hc,
                                    &mxc,
                                    _state);
      rcopyrv(n, &state->xcreachedbycut, unmarkedi, &unmarkedcut, _state);
      mirbfvns_gridexpandcutgenerateneighbors(&state->grid,
                                              state,
                                              &xc,
                                              &unmarkedcut,
                                              &state->xcneighbors,
                                              state->xcneighborscnt,
                                              &state->xuneighbors,
                                              &state->xucuts,
                                              &state->xupoints,
                                              &nncnt,
                                              &state->xuflags,
                                              _state);
      if (nncnt == 0) {
        state->xcqueryflags.ptr.p_bool[unmarkedi] = ae_true;
        continue;
      }
      newunsolvedcnt = 0;
      for (i = 0; i <= nncnt - 1; i++) {
        if (mirbfvns_gridneedsevals(
                &state->grid, state, state->xuneighbors.ptr.p_int[i], _state)) {
          newunsolvedcnt = newunsolvedcnt + 1;
        }
      }
      if (result != 0) {
        if (state->maxneighborhood > 0
            && state->xcneighborscnt - 1 + nncnt >= state->maxneighborhood) {
          break;
        }
      }
      state->xcqueryflags.ptr.p_bool[unmarkedi] = ae_true;
      for (i = 0; i <= nncnt - 1; i++) {
        igrowappendv(state->xcneighborscnt + 1,
                     &state->xcneighbors,
                     state->xuneighbors.ptr.p_int[i],
                     _state);
        igrowappendv(
            state->xcneighborscnt + 1, &state->xcreachedfrom, unmarked, _state);
        bgrowappendv(
            state->xcneighborscnt + 1, &state->xcqueryflags, ae_false, _state);
        rgrowrowsfixedcolsm(
            state->xcneighborscnt + 1, n, &state->xcreachedbycut, _state);
        rcopyrr(n,
                &state->xucuts,
                i,
                &state->xcreachedbycut,
                state->xcneighborscnt,
                _state);
        state->xcneighborscnt = state->xcneighborscnt + 1;
      }
      result      = result + nncnt;
      unsolvedcnt = unsolvedcnt + newunsolvedcnt;
    }
    ae_nxpool_recycle(&state->rpool, &xc, _state);
    ae_nxpool_recycle(&state->rpool, &unmarkedcut, _state);
    ae_frame_leave(_state);
    return result;
  }

  /*************************************************************************
  Sum of violations of linear and nonlinear constraints. Linear ones are
  scaled
  *************************************************************************/
  static void mirbfvns_computeviolation2(const mirbfvnsstate*           state,
                                         /* Real    */ const ae_vector* x,
                                         /* Real    */ const ae_vector* fi,
                                         double*                        h,
                                         double*                        mx,
                                         ae_state* _state) {
    ae_int_t i;
    double   v;
    double   vmx;
    double   vs;

    *h  = 0.0;
    *mx = 0.0;

    unscaleandchecklc2violation(&state->s,
                                &state->rawa,
                                &state->rawal,
                                &state->rawau,
                                &state->lcsrcidx,
                                state->lccnt,
                                x,
                                &vs,
                                &vmx,
                                &i,
                                _state);
    *h  = vs;
    *mx = vmx;
    for (i = 0; i <= state->nnlc - 1; i++) {
      if (ae_isfinite(state->nl.ptr.p_double[i], _state)) {
        v = ae_maxreal(
            state->nl.ptr.p_double[i] - fi->ptr.p_double[1 + i], 0.0, _state);
        *h  = *h + v;
        *mx = ae_maxreal(*mx, v, _state);
      }
      if (ae_isfinite(state->nu.ptr.p_double[i], _state)) {
        v = ae_maxreal(
            fi->ptr.p_double[1 + i] - state->nu.ptr.p_double[i], 0.0, _state);
        *h  = *h + v;
        *mx = ae_maxreal(*mx, v, _state);
      }
    }
  }

  /*************************************************************************
  Finds integer feasible point satisfying linear constraints that is nearest
  to X0, subject to integral cuts given by CutsTable[RowIdx,*]:
  * CutsTable[RowIdx,i]>0 means that the lower bound on variable i is set to
  X0[i]+CutsTable[RowIdx,i]
  * CutsTable[RowIdx,i]<0 means that the upper bound on variable i is set to
  X0[i]+CutsTable[RowIdx,i]
  * CutsTable[RowIdx,i]=0 means that bounds on variable i are unchanged.
  * CutsTable[RowIdx,i] must be integer and must be zero for fractional
  variables X0 is assumed to be feasible with respect to at least box
  constraints.

  This function does not modify the grid, merely solves MILP subproblem and
  returns its solution. If no feasible point satisfying cut can be found,
  False is returned and XN is left in an undefined state.

  XN must be preallocated array long enough to store the result
  *************************************************************************/
  static void mirbfvns_findnearestintegralsubjecttocut(
      mirbfvnsstate*                 state,
      /* Real    */ const ae_vector* x0,
      /* Real    */ const ae_matrix* cutstable,
      /* Real    */ ae_matrix*       resultstable,
      /* Boolean */ ae_vector*       successflags,
      ae_int_t                       rowidx,
      ae_bool                        usesafetybox,
      ae_state*                      _state) {
    ae_frame             _frame_block;
    ae_int_t             n;
    ae_int_t             vidx;
    mirbfvnstemporaries* buf;
    ae_smart_ptr         _buf;
    ae_bool              updatestats;

    ae_frame_make(_state, &_frame_block);
    memset(&_buf, 0, sizeof(_buf));
    ae_smart_ptr_init(&_buf, (void**) &buf, ae_false, _state, ae_true);

    n           = state->n;
    updatestats = state->doanytrace || state->adaptiveinternalparallelism >= 0;
    for (vidx = 0; vidx <= n - 1; vidx++) {
      ae_assert(
          (cutstable->ptr.pp_double[rowidx][vidx]
               == (double) ae_round(cutstable->ptr.pp_double[rowidx][vidx],
                                    _state)
           && (state->isintegral.ptr.p_bool[vidx]
               || cutstable->ptr.pp_double[rowidx][vidx] == (double) 0))
              && (!state->isintegral.ptr.p_bool[vidx]
                  || x0->ptr.p_double[vidx]
                         == (double) ae_round(x0->ptr.p_double[vidx], _state)),
          "MIRBFVNS: 075558",
          _state);
    }
    successflags->ptr.p_bool[rowidx] = ae_true;
    for (vidx = 0; vidx <= n - 1; vidx++) {
      resultstable->ptr.pp_double[rowidx][vidx]
          = x0->ptr.p_double[vidx] + cutstable->ptr.pp_double[rowidx][vidx];
      if ((state->hasbndl.ptr.p_bool[vidx]
           && cutstable->ptr.pp_double[rowidx][vidx] < (double) 0)
          && resultstable->ptr.pp_double[rowidx][vidx]
                 < state->bndl.ptr.p_double[vidx]) {
        successflags->ptr.p_bool[rowidx] = ae_false;
      }
      if ((state->hasbndu.ptr.p_bool[vidx]
           && cutstable->ptr.pp_double[rowidx][vidx] > (double) 0)
          && resultstable->ptr.pp_double[rowidx][vidx]
                 > state->bndu.ptr.p_double[vidx]) {
        successflags->ptr.p_bool[rowidx] = ae_false;
      }
    }
    if (successflags->ptr.p_bool[rowidx] && state->haslinearlyconstrainedints) {
      ae_shared_pool_retrieve(&state->tmppool, &_buf, _state);
      stimerinit(&buf->localtimer, _state);
      stimerstartcond(&buf->localtimer, updatestats, _state);
      mirbfvns_findnearestintegralsubjecttocutx(state,
                                                x0,
                                                cutstable,
                                                resultstable,
                                                successflags,
                                                rowidx,
                                                usesafetybox,
                                                buf,
                                                _state);
      stimerstopcond(&buf->localtimer, updatestats, _state);
      if (updatestats) {
        weakatomicfetchadd(&state->cutcnt, 1, _state);
        weakatomicfetchadd(&state->cuttimems,
                           stimergetmsint(&buf->localtimer, _state),
                           _state);
      }
      ae_shared_pool_recycle(&state->tmppool, &_buf, _state);
    } else {
      if (updatestats) {
        weakatomicfetchadd(&state->cutcnt, 1, _state);
      }
    }
    ae_frame_leave(_state);
  }

  /*************************************************************************
  A version of FindNearestIntegralSubjectToCut() internally called by the
  function when we have a linearly constrained problem. A workhorse for
  diffucult problems.
  *************************************************************************/
  static void mirbfvns_findnearestintegralsubjecttocutx(
      mirbfvnsstate*                 state,
      /* Real    */ const ae_vector* x0,
      /* Real    */ const ae_matrix* cutstable,
      /* Real    */ ae_matrix*       resultstable,
      /* Boolean */ ae_vector*       successflags,
      ae_int_t                       rowidx,
      ae_bool                        usesafetybox,
      mirbfvnstemporaries*           buf,
      ae_state*                      _state) {
    ae_int_t n;
    ae_int_t i;
    double   v;
    ae_bool  bbgdsparserequests;
    ae_int_t bbgdgroupsize;
    ae_int_t nmultistarts;
    ae_int_t timeout;
    double   smallcoeff;

    n = state->n;
    rcopyallocv(n, &state->bndl, &buf->wrkbndl, _state);
    rcopyallocv(n, &state->bndu, &buf->wrkbndu, _state);
    for (i = 0; i <= n - 1; i++) {
      if (cutstable->ptr.pp_double[rowidx][i] > (double) 0) {
        buf->wrkbndl.ptr.p_double[i]
            = x0->ptr.p_double[i] + cutstable->ptr.pp_double[rowidx][i];
      }
      if (cutstable->ptr.pp_double[rowidx][i] < (double) 0) {
        buf->wrkbndu.ptr.p_double[i]
            = x0->ptr.p_double[i] + cutstable->ptr.pp_double[rowidx][i];
      }
      if (state->isintegral.ptr.p_bool[i] && usesafetybox) {
        v = x0->ptr.p_double[i] - (double) mirbfvns_safetyboxforbbgd;
        if (!ae_isfinite(buf->wrkbndl.ptr.p_double[i], _state)
            || ae_fp_less(buf->wrkbndl.ptr.p_double[i], v)) {
          buf->wrkbndl.ptr.p_double[i] = v;
        }
        v = x0->ptr.p_double[i] + (double) mirbfvns_safetyboxforbbgd;
        if (!ae_isfinite(buf->wrkbndu.ptr.p_double[i], _state)
            || ae_fp_greater(buf->wrkbndu.ptr.p_double[i], v)) {
          buf->wrkbndu.ptr.p_double[i] = v;
        }
      }
    }
    buf->diaga.n = n;
    buf->diaga.m = n;
    iallocv(n + 1, &buf->diaga.ridx, _state);
    iallocv(n, &buf->diaga.idx, _state);
    rallocv(n, &buf->diaga.vals, _state);
    rallocv(n, &buf->linb, _state);
    for (i = 0; i <= n - 1; i++) {
      buf->diaga.ridx.ptr.p_int[i] = i;
      buf->diaga.idx.ptr.p_int[i]  = i;
      if (state->isintegral.ptr.p_bool[i]) {
        buf->diaga.vals.ptr.p_double[i] = 1.0;
        buf->linb.ptr.p_double[i]
            = -(x0->ptr.p_double[i] + cutstable->ptr.pp_double[rowidx][i]);
      } else {
        smallcoeff
            = 1.0E-5
              / ae_maxreal(state->s.ptr.p_double[i] * state->s.ptr.p_double[i],
                           (double) (1),
                           _state);
        buf->diaga.vals.ptr.p_double[i] = smallcoeff;
        buf->linb.ptr.p_double[i]       = -x0->ptr.p_double[i] * smallcoeff;
      }
    }
    buf->diaga.ridx.ptr.p_int[n] = n;
    sparsecreatecrsinplace(&buf->diaga, _state);
    bsetallocv(n, ae_false, &buf->tmpb, _state);
    bbgdsparserequests = ae_true;
    bbgdgroupsize      = 1;
    nmultistarts       = 1;
    timeout            = 0;
    bbgdcreatebuf(n,
                  &buf->wrkbndl,
                  &buf->wrkbndu,
                  &state->s,
                  x0,
                  &state->isintegral,
                  &state->isbinary,
                  &buf->tmpb,
                  &state->rawa,
                  &state->rawal,
                  &state->rawau,
                  &state->lcsrcidx,
                  state->lccnt,
                  &state->nl,
                  &state->nu,
                  0,
                  bbgdsparserequests,
                  bbgdgroupsize,
                  nmultistarts,
                  timeout,
                  0,
                  &buf->bbgdsubsolver,
                  _state);
    bbgdsetctol(&buf->bbgdsubsolver, state->ctol, _state);
    bbgdsetquadraticobjective(
        &buf->bbgdsubsolver, &buf->diaga, ae_false, &buf->linb, 0.0, _state);
    bbgdforceserial(&buf->bbgdsubsolver, _state);
    bbgdsetdiving(&buf->bbgdsubsolver, 2, _state);
    bbgdsetmaxprimalcandidates(
        &buf->bbgdsubsolver, mirbfvns_maxprimalcandforcut, _state);
    bbgdsetsoftmaxnodes(
        &buf->bbgdsubsolver, mirbfvns_softmaxnodescoeff * n, _state);
    while (bbgditeration(&buf->bbgdsubsolver, _state)) {
      ae_assert(ae_false,
                "MIRBFVNS: unexpected V2 request by BBGD working in MIQP mode",
                _state);
    }
    rcopyvr(n, &buf->bbgdsubsolver.xc, resultstable, rowidx, _state);
    successflags->ptr.p_bool[rowidx]
        = buf->bbgdsubsolver.repterminationtype > 0;
  }

  /*************************************************************************
  Parallel version of FindNearestIntegralSubjectToCut().

  The half-range [R0,R1) of cuts from CutsTable[] is processed.

  IsRoot must be true on initial call (recursive calls set it to False).
  TryParallelism controls whether parallel processing is used or not.
  *************************************************************************/
  static void mirbfvns_parallelfindnearestintegralsubjecttocut(
      mirbfvnsstate*                 state,
      /* Real    */ const ae_vector* x0,
      /* Real    */ const ae_matrix* cutstable,
      /* Real    */ ae_matrix*       resultstable,
      /* Boolean */ ae_vector*       successflags,
      ae_int_t                       r0,
      ae_int_t                       r1,
      ae_bool                        usesafetybox,
      ae_bool                        isroot,
      ae_bool                        tryparallelism,
      ae_state*                      _state) {
    ae_int_t rmid;

    if (r1 <= r0) {
      return;
    }
    if ((isroot && tryparallelism) && r1 - r0 >= 2) {
      if (_trypexec_mirbfvns_parallelfindnearestintegralsubjecttocut(
              state,
              x0,
              cutstable,
              resultstable,
              successflags,
              r0,
              r1,
              usesafetybox,
              isroot,
              tryparallelism,
              _state)) {
        return;
      }
    }
    if (r1 == r0 + 1) {
      mirbfvns_findnearestintegralsubjecttocut(state,
                                               x0,
                                               cutstable,
                                               resultstable,
                                               successflags,
                                               r0,
                                               usesafetybox,
                                               _state);
      return;
    }
    ae_assert(r1 > r0 + 1, "MIRBFVNS: 705014 failed", _state);
    rmid = r0 + (r1 - r0) / 2;
    mirbfvns_parallelfindnearestintegralsubjecttocut(state,
                                                     x0,
                                                     cutstable,
                                                     resultstable,
                                                     successflags,
                                                     r0,
                                                     rmid,
                                                     usesafetybox,
                                                     ae_false,
                                                     tryparallelism,
                                                     _state);
    mirbfvns_parallelfindnearestintegralsubjecttocut(state,
                                                     x0,
                                                     cutstable,
                                                     resultstable,
                                                     successflags,
                                                     rmid,
                                                     r1,
                                                     usesafetybox,
                                                     ae_false,
                                                     tryparallelism,
                                                     _state);
  }

  /*************************************************************************
  Serial stub for GPL edition.
  *************************************************************************/
  ae_bool _trypexec_mirbfvns_parallelfindnearestintegralsubjecttocut(
      mirbfvnsstate*                 state,
      /* Real    */ const ae_vector* x0,
      /* Real    */ const ae_matrix* cutstable,
      /* Real    */ ae_matrix*       resultstable,
      /* Boolean */ ae_vector*       successflags,
      ae_int_t                       r0,
      ae_int_t                       r1,
      ae_bool                        usesafetybox,
      ae_bool                        isroot,
      ae_bool                        tryparallelism,
      ae_state*                      _state) {
    return ae_false;
  }

  /*************************************************************************
  Initializes dataset in an empty state
  *************************************************************************/
  static void mirbfvns_datasetinitempty(mirbfvnsdataset* dataset,
                                        mirbfvnsstate*   state,
                                        ae_state*        _state) {
    dataset->npoints = 0;
    dataset->nvars   = state->n;
    dataset->nnlc    = state->nnlc;
    rgrowrowsfixedcolsm(
        1, dataset->nvars + 1 + dataset->nnlc + 2, &dataset->pointinfo, _state);
  }

  /*************************************************************************
  Appends point to the dataset and returns its index
  *************************************************************************/
  static ae_int_t mirbfvns_datasetappendpoint(mirbfvnsdataset* dataset,
                                              /* Real    */ const ae_vector* x,
                                              /* Real    */ const ae_vector* fi,
                                              double                         h,
                                              double                         mx,
                                              ae_state* _state) {
    ae_int_t i;
    ae_int_t n;
    ae_int_t nnlc;
    ae_int_t rowidx;
    ae_int_t result;

    rgrowrowsfixedcolsm(dataset->npoints + 1,
                        dataset->nvars + 1 + dataset->nnlc + 2,
                        &dataset->pointinfo,
                        _state);
    rowidx = dataset->npoints;
    n      = dataset->nvars;
    nnlc   = dataset->nnlc;
    for (i = 0; i <= n - 1; i++) {
      dataset->pointinfo.ptr.pp_double[rowidx][i] = x->ptr.p_double[i];
    }
    for (i = 0; i <= nnlc; i++) {
      dataset->pointinfo.ptr.pp_double[rowidx][n + i] = fi->ptr.p_double[i];
    }
    dataset->pointinfo.ptr.pp_double[rowidx][n + 1 + nnlc + 0] = h;
    dataset->pointinfo.ptr.pp_double[rowidx][n + 1 + nnlc + 1] = mx;
    dataset->npoints = dataset->npoints + 1;
    result           = rowidx;
    return result;
  }

  /*************************************************************************
  Initializes an integer grid using an initial point. Returns a node index,
  which is likely to be zero in all implementations.

  Params:
      X           point
      Fi  objective/constraints
      H   sum of constraint violations
      MX  maximum of constraint violations
  *************************************************************************/
  static ae_int_t mirbfvns_gridcreate(mirbfvnsgrid*                  grid,
                                      mirbfvnsstate*                 state,
                                      /* Real    */ const ae_vector* x,
                                      /* Real    */ const ae_vector* fi,
                                      double                         h,
                                      double                         mx,
                                      ae_state*                      _state) {
    ae_int_t n;
    ae_int_t i;
    ae_int_t pointidx;
    ae_int_t result;

    n = state->n;
    for (i = 0; i <= n - 1; i++) {
      if (state->isintegral.ptr.p_bool[i]
          && (double) ae_round(x->ptr.p_double[i], _state)
                 != x->ptr.p_double[i]) {
        ae_assert(ae_false, "MIRBFVNS: 886456 failed", _state);
      }
    }
    pointidx
        = mirbfvns_datasetappendpoint(&state->dataset, x, fi, h, mx, _state);
    grid->nnodes   = 1;
    grid->naddcols = 7;
    rgrowrowsfixedcolsm(1, n + grid->naddcols, &grid->nodesinfo, _state);
    rcopyvr(n, x, &grid->nodesinfo, 0, _state);
    rmergemulvr(n, &state->maskint, &grid->nodesinfo, 0, _state);
    grid->nodesinfo.ptr.pp_double[0][n + mirbfvns_ncolstatus]
        = (double) (icase2(state->nfrac == 0,
                           mirbfvns_nodesolved,
                           mirbfvns_nodeinprogress,
                           _state));
    grid->nodesinfo.ptr.pp_double[0][n + mirbfvns_ncolneighborbegin]
        = (double) (-1);
    grid->nodesinfo.ptr.pp_double[0][n + mirbfvns_ncolneighborend]
        = (double) (-1);
    grid->nodesinfo.ptr.pp_double[0][n + mirbfvns_ncolfbest]
        = fi->ptr.p_double[0];
    grid->nodesinfo.ptr.pp_double[0][n + mirbfvns_ncolhbest]  = h;
    grid->nodesinfo.ptr.pp_double[0][n + mirbfvns_ncolmxbest] = mx;
    grid->nodesinfo.ptr.pp_double[0][n + mirbfvns_ncollastaccepted]
        = (double) (pointidx);
    result             = 0;
    grid->ptlistlength = 0;
    iallocv(2, &grid->ptlistheads, _state);
    grid->ptlistheads.ptr.p_int[0] = -1;
    grid->ptlistheads.ptr.p_int[1] = 0;
    mirbfvns_gridappendpointtolist(grid, pointidx, 0, _state);
    ae_obj_array_clear(&grid->subsolvers);
    if (state->nfrac > 0) {
      mirbfvns_gridappendnilsubsolver(grid, _state);
      mirbfvns_gridinitnilsubsolver(
          grid, state, 0, fi->ptr.p_double[0], h, mx, _state);
    }
    return result;
  }

  /*************************************************************************
  Appends point index to the per-node points list
  *************************************************************************/
  static void mirbfvns_gridappendpointtolist(mirbfvnsgrid* grid,
                                             ae_int_t      pointidx,
                                             ae_int_t      nodeidx,
                                             ae_state*     _state) {
    ae_int_t nextentry;
    ae_int_t listsize;

    nextentry = grid->ptlistheads.ptr.p_int[2 * nodeidx + 0];
    listsize  = grid->ptlistheads.ptr.p_int[2 * nodeidx + 1];
    igrowappendv(
        2 * grid->ptlistlength + 1, &grid->ptlistdata, pointidx, _state);
    igrowappendv(
        2 * grid->ptlistlength + 2, &grid->ptlistdata, nextentry, _state);
    grid->ptlistheads.ptr.p_int[2 * nodeidx + 0] = grid->ptlistlength;
    grid->ptlistheads.ptr.p_int[2 * nodeidx + 1] = listsize + 1;
    grid->ptlistlength                           = grid->ptlistlength + 1;
  }

  /*************************************************************************
  Return node status by its index
  *************************************************************************/
  static ae_int_t mirbfvns_gridgetstatus(const mirbfvnsgrid*  grid,
                                         const mirbfvnsstate* state,
                                         ae_int_t             nodeidx,
                                         ae_state*            _state) {
    ae_int_t result;

    ae_assert(
        nodeidx >= 0 && nodeidx < grid->nnodes, "MIRBFVNS: 905114", _state);
    result = ae_round(
        grid->nodesinfo.ptr.pp_double[nodeidx][state->n + mirbfvns_ncolstatus],
        _state);
    return result;
  }

  /*************************************************************************
  Return true of the node needs further evaluations (unexplored or in progress).
  False is returned when no evaluations are needed (solved or bad).
  *************************************************************************/
  static ae_bool mirbfvns_gridneedsevals(mirbfvnsgrid*  grid,
                                         mirbfvnsstate* state,
                                         ae_int_t       nodeidx,
                                         ae_state*      _state) {
    double  k;
    ae_bool result;

    ae_assert(
        nodeidx >= 0 && nodeidx < grid->nnodes, "MIRBFVNS: 039445", _state);
    k = grid->nodesinfo.ptr.pp_double[nodeidx][state->n + mirbfvns_ncolstatus];
    ae_assert(((ae_fp_eq(k, (double) (mirbfvns_nodeunexplored))
                || ae_fp_eq(k, (double) (mirbfvns_nodeinprogress)))
               || ae_fp_eq(k, (double) (mirbfvns_nodesolved)))
                  || ae_fp_eq(k, (double) (mirbfvns_nodebad)),
              "MIRBFVNS: 935449",
              _state);
    result = ae_fp_eq(k, (double) (mirbfvns_nodeunexplored))
             || ae_fp_eq(k, (double) (mirbfvns_nodeinprogress));
    return result;
  }

  /*************************************************************************
  Scans integer grid for a node corresponding to a point. If no node is found,
  creates a new one in a nodeUnexplored state.
  *************************************************************************/
  static ae_int_t
  mirbfvns_gridfindorcreatenode(mirbfvnsgrid*                  grid,
                                mirbfvnsstate*                 state,
                                /* Real    */ const ae_vector* x,
                                ae_state*                      _state) {
    ae_frame  _frame_block;
    ae_int_t  n;
    ae_int_t  i;
    ae_int_t  j;
    ae_vector xm;
    ae_bool   isequal;
    ae_int_t  result;

    ae_frame_make(_state, &_frame_block);
    memset(&xm, 0, sizeof(xm));
    ae_vector_init(&xm, 0, DT_REAL, _state, ae_true);

    n = state->n;
    for (i = 0; i <= n - 1; i++) {
      if (state->isintegral.ptr.p_bool[i]
          && (double) ae_round(x->ptr.p_double[i], _state)
                 != x->ptr.p_double[i]) {
        ae_assert(ae_false, "MIRBFVNS: 932513 failed", _state);
      }
    }
    ae_nxpool_retrieve(&state->rpool, &xm, _state);
    ae_assert(xm.cnt >= n, "MIRBFVNS: 932514", _state);
    rcopyv(n, x, &xm, _state);
    rmergemulv(n, &state->maskint, &xm, _state);
    for (i = 0; i <= grid->nnodes - 1; i++) {
      isequal = ae_true;
      for (j = 0; j <= n - 1; j++) {
        if (!(xm.ptr.p_double[j] == grid->nodesinfo.ptr.pp_double[i][j])) {
          isequal = ae_false;
          break;
        }
      }
      if (isequal) {
        result = i;
        ae_frame_leave(_state);
        return result;
      }
    }
    rgrowrowsfixedcolsm(
        grid->nnodes + 1, n + grid->naddcols, &grid->nodesinfo, _state);
    rcopyvr(n, &xm, &grid->nodesinfo, grid->nnodes, _state);
    grid->nodesinfo.ptr.pp_double[grid->nnodes][n + mirbfvns_ncolstatus]
        = (double) (mirbfvns_nodeunexplored);
    grid->nodesinfo.ptr.pp_double[grid->nnodes][n + mirbfvns_ncolneighborbegin]
        = (double) (-1);
    grid->nodesinfo.ptr.pp_double[grid->nnodes][n + mirbfvns_ncolneighborend]
        = (double) (-1);
    grid->nodesinfo.ptr.pp_double[grid->nnodes][n + mirbfvns_ncolfbest]
        = ae_maxrealnumber;
    grid->nodesinfo.ptr.pp_double[grid->nnodes][n + mirbfvns_ncolhbest]
        = ae_maxrealnumber;
    grid->nodesinfo.ptr.pp_double[grid->nnodes][n + mirbfvns_ncolmxbest]
        = ae_maxrealnumber;
    grid->nodesinfo.ptr.pp_double[grid->nnodes][n + mirbfvns_ncollastaccepted]
        = (double) (-1);
    igrowappendv(2 * grid->nnodes + 1, &grid->ptlistheads, -1, _state);
    igrowappendv(2 * grid->nnodes + 2, &grid->ptlistheads, 0, _state);
    grid->nnodes = grid->nnodes + 1;
    result       = grid->nnodes - 1;
    if (state->nfrac > 0) {
      mirbfvns_gridappendnilsubsolver(grid, _state);
    }
    if (state->doextratrace) {
      ae_trace("[%6d] >>> CREATING NODE: variables mask is [",
               (int) (grid->nnodes - 1));
      tracerowautoprec(&grid->nodesinfo, grid->nnodes - 1, 0, n, _state);
      ae_trace("]\n");
    }
    ae_nxpool_recycle(&state->rpool, &xm, _state);
    ae_frame_leave(_state);
    return result;
  }

  /*************************************************************************
  Scans integer grid for nodes similar to #NodeIdx.

  Here 'similar' means that:
  * there is a mask of relevant variables given by VarMask[], where True
    means that the variable is marked as a relevant
  * a node has all relevant integer variables equal to that of #NodeIdx
    (fractional ones and irrelevant integer ones are ignored)

  If PutFirst is True, then #NodeIdx is output first in the list.

  Node indexes are stored to nodeList[] array that is resized as needed,
  results count is stores to nodesCnt
  *************************************************************************/
  static void mirbfvns_gridfindnodeslike(const mirbfvnsgrid*  grid,
                                         const mirbfvnsstate* state,
                                         ae_int_t             nodeidx,
                                         ae_bool              putfirst,
                                         /* Boolean */ const ae_vector* varmask,
                                         /* Integer */ ae_vector* nodeslist,
                                         ae_int_t*                nodescnt,
                                         ae_state*                _state) {
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_bool  isequal;

    *nodescnt = 0;

    n         = state->n;
    *nodescnt = 0;
    if (putfirst) {
      igrowappendv(*nodescnt + 1, nodeslist, nodeidx, _state);
      *nodescnt = *nodescnt + 1;
    }
    for (i = 0; i <= grid->nnodes - 1; i++) {
      if (putfirst && i == nodeidx) {
        continue;
      }
      isequal = ae_true;
      for (j = 0; j <= n - 1; j++) {
        if ((state->isintegral.ptr.p_bool[j] && varmask->ptr.p_bool[j])
            && !(grid->nodesinfo.ptr.pp_double[nodeidx][j]
                 == grid->nodesinfo.ptr.pp_double[i][j])) {
          isequal = ae_false;
          break;
        }
      }
      if (isequal) {
        igrowappendv(*nodescnt + 1, nodeslist, i, _state);
        *nodescnt = *nodescnt + 1;
      }
    }
  }

  /*************************************************************************
  Appends nil subsolver to the end of the subsolver list; no integrity checks,
  internal function used by grid.
  *************************************************************************/
  static void mirbfvns_gridappendnilsubsolver(mirbfvnsgrid* grid,
                                              ae_state*     _state) {
    ae_frame               _frame_block;
    mirbfvnsnodesubsolver* dummy;
    ae_smart_ptr           _dummy;

    ae_frame_make(_state, &_frame_block);
    memset(&_dummy, 0, sizeof(_dummy));
    ae_smart_ptr_init(&_dummy, (void**) &dummy, ae_false, _state, ae_true);

    ae_obj_array_append_transfer(&grid->subsolvers, &_dummy, _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Replaces nil subsolver with the newly initialized one. Assumes NFrac>0.
  No integrity checks, internal function used by grid.
  *************************************************************************/
  static void mirbfvns_gridinitnilsubsolver(mirbfvnsgrid*        grid,
                                            const mirbfvnsstate* state,
                                            ae_int_t             nodeidx,
                                            double               f,
                                            double               h,
                                            double               mx,
                                            ae_state*            _state) {
    ae_frame               _frame_block;
    mirbfvnsnodesubsolver* subsolver;
    ae_smart_ptr           _subsolver;

    ae_frame_make(_state, &_frame_block);
    memset(&_subsolver, 0, sizeof(_subsolver));
    ae_smart_ptr_init(
        &_subsolver, (void**) &subsolver, ae_false, _state, ae_true);

    ae_obj_array_get(&grid->subsolvers, nodeidx, &_subsolver, _state);
    ae_assert(!(subsolver != NULL), "MIRBFVNS: 231513", _state);
    subsolver = (mirbfvnsnodesubsolver*) ae_malloc(
        sizeof(mirbfvnsnodesubsolver),
        _state); /* note: using subsolver as a temporary prior to assigning its
                    value to _subsolver */
    memset(subsolver, 0, sizeof(mirbfvnsnodesubsolver));
    _mirbfvnsnodesubsolver_init(subsolver, _state, ae_false);
    ae_smart_ptr_assign(&_subsolver,
                        subsolver,
                        ae_true,
                        ae_true,
                        (ae_int_t) sizeof(mirbfvnsnodesubsolver),
                        _mirbfvnsnodesubsolver_init_copy,
                        _mirbfvnsnodesubsolver_destroy);
    subsolver->trustrad            = 1.0;
    subsolver->sufficientcloudsize = ae_false;
    subsolver->maxh       = (double) 10 * ae_maxreal((double) (10), h, _state);
    subsolver->historymax = iboundval(state->nfrac + 1, 10, 10, _state);
    rsetallocv(
        subsolver->historymax, 1.0E20, &subsolver->successfhistory, _state);
    rsetallocv(
        subsolver->historymax, 1.0E20, &subsolver->successhhistory, _state);
    ae_obj_array_set_transfer(&grid->subsolvers, nodeidx, &_subsolver, _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Returns information about best point in a neighborhood. Mostly used for
  debug purposes. If no neighbors are present, returns False and dummy values
  (MaxRealNumber).

  Returns objective, sum(violation) and max(violation) at the best point.
  *************************************************************************/
  static ae_bool
  mirbfvns_gridgetbestinneighborhood(mirbfvnsgrid*                  grid,
                                     mirbfvnsstate*                 state,
                                     /* Integer */ const ae_vector* neighbors,
                                     ae_int_t  neighborscnt,
                                     double*   fbest,
                                     double*   hbest,
                                     double*   mxbest,
                                     ae_state* _state) {
    ae_frame  _frame_block;
    ae_int_t  n;
    ae_int_t  i;
    ae_int_t  idummy;
    ae_vector x0;
    double    f;
    double    h;
    double    mx;
    ae_bool   result;

    ae_frame_make(_state, &_frame_block);
    memset(&x0, 0, sizeof(x0));
    *fbest  = 0.0;
    *hbest  = 0.0;
    *mxbest = 0.0;
    ae_vector_init(&x0, 0, DT_REAL, _state, ae_true);

    n = state->n;
    ae_nxpool_retrieve(&state->rpool, &x0, _state);
    ae_assert(x0.cnt >= n, "MIRBFVNS: 212354", _state);
    *fbest  = ae_maxrealnumber;
    *hbest  = ae_maxrealnumber;
    *mxbest = ae_maxrealnumber;
    result  = ae_false;
    for (i = 0; i <= neighborscnt - 1; i++) {
      if (mirbfvns_gridgetstatus(grid, state, neighbors->ptr.p_int[i], _state)
          != mirbfvns_nodeunexplored) {
        mirbfvns_gridoffloadbestpoint(grid,
                                      state,
                                      neighbors->ptr.p_int[i],
                                      &x0,
                                      &idummy,
                                      &f,
                                      &h,
                                      &mx,
                                      _state);
        if (!result
            || mirbfvns_isbetterpoint(
                *fbest, *hbest, *mxbest, f, h, mx, state->ctol, _state)) {
          *fbest  = f;
          *hbest  = h;
          *mxbest = mx;
          result  = ae_true;
        }
      }
    }
    ae_nxpool_recycle(&state->rpool, &x0, _state);
    ae_frame_leave(_state);
    return result;
  }

  /*************************************************************************
  Returns information about best last accepted point in unsolved nodes in a
  neighborhood.

  Mostly used for debug purposes. If no neighbors are present, returns False
  and dummy values (MaxRealNumber, -1 for node index).

  Returns objective, sum(violation) and max(violation) at the best point as
  well as node index.
  *************************************************************************/
  static ae_bool mirbfvns_gridgetbestlastacceptedinunsolvedneighborhood(
      mirbfvnsgrid*                  grid,
      mirbfvnsstate*                 state,
      /* Integer */ const ae_vector* neighbors,
      ae_int_t                       neighborscnt,
      ae_int_t*                      nodeidx,
      double*                        fbest,
      double*                        hbest,
      double*                        mxbest,
      ae_state*                      _state) {
    ae_int_t n;
    ae_int_t nnlc;
    ae_int_t i;
    ae_int_t st;
    ae_int_t pointidx;
    double   f;
    double   h;
    double   mx;
    ae_bool  result;

    *nodeidx = 0;
    *fbest   = 0.0;
    *hbest   = 0.0;
    *mxbest  = 0.0;

    n        = state->n;
    nnlc     = state->nnlc;
    *nodeidx = -1;
    *fbest   = ae_maxrealnumber;
    *hbest   = ae_maxrealnumber;
    *mxbest  = ae_maxrealnumber;
    result   = ae_false;
    for (i = 0; i <= neighborscnt - 1; i++) {
      st = ae_round(
          grid->nodesinfo.ptr.pp_double[neighbors->ptr.p_int[i]]
                                       [state->n + mirbfvns_ncolstatus],
          _state);
      ae_assert(
          ((st == mirbfvns_nodeunexplored || st == mirbfvns_nodeinprogress)
           || st == mirbfvns_nodesolved)
              || st == mirbfvns_nodebad,
          "MIRBFVNS: 311248",
          _state);
      if (st != mirbfvns_nodeinprogress) {
        continue;
      }
      pointidx = ae_round(
          grid->nodesinfo.ptr.pp_double[neighbors->ptr.p_int[i]]
                                       [n + mirbfvns_ncollastaccepted],
          _state);
      f  = state->dataset.pointinfo.ptr.pp_double[pointidx][n];
      h  = state->dataset.pointinfo.ptr.pp_double[pointidx][n + 1 + nnlc + 0];
      mx = state->dataset.pointinfo.ptr.pp_double[pointidx][n + 1 + nnlc + 1];
      if (!result
          || mirbfvns_isbetterpoint(
              *fbest, *hbest, *mxbest, f, h, mx, state->ctol, _state)) {
        *fbest   = f;
        *hbest   = h;
        *mxbest  = mx;
        *nodeidx = neighbors->ptr.p_int[i];
        result   = ae_true;
      }
    }
    return result;
  }

  /*************************************************************************
  Informally speaking, this function returns a list of node indexes that
  correspond to up to 2N neighbors of a given node, excluding nodes in a
  user-specified list.

  More precisely, the node is given by a central point of a neighborhood plus
  a cut that resulted in reaching a node. The actual node is a sum of XCentral
  and NodeCut.

  This function 'expands' the cut by generating many derivative cuts, querying
  for nearest neighbors corresponding to these cuts, and generating a list
  of both cuts and neighbors.

  This function creates new nodes with status=nodeUnexplored, if necessary
  *************************************************************************/
  static void mirbfvns_gridexpandcutgenerateneighbors(
      mirbfvnsgrid*                  grid,
      mirbfvnsstate*                 state,
      /* Real    */ const ae_vector* xcentral,
      /* Real    */ const ae_vector* nodecut,
      /* Integer */ const ae_vector* excludelist,
      ae_int_t                       excludecnt,
      /* Integer */ ae_vector*       neighbornodes,
      /* Real    */ ae_matrix*       cutsapplied,
      /* Real    */ ae_matrix*       pointsfound,
      ae_int_t*                      nncnt,
      /* Boolean */ ae_vector*       tmpsuccessflags,
      ae_state*                      _state) {
    ae_frame  _frame_block;
    ae_int_t  n;
    ae_int_t  vidx;
    ae_int_t  sidx;
    ae_int_t  candcnt;
    ae_int_t  candidx;
    ae_int_t  newnodeidx;
    double    vshift;
    ae_vector xm;
    ae_bool   sufficienttime;
    ae_bool   sufficientcount;

    ae_frame_make(_state, &_frame_block);
    memset(&xm, 0, sizeof(xm));
    ae_vector_init(&xm, 0, DT_REAL, _state, ae_true);

    n = state->n;
    ae_nxpool_retrieve(&state->rpool, &xm, _state);
    ae_assert(xm.cnt >= n, "MIRBFVNS: 937448", _state);
    rgrowrowsfixedcolsm(2 * n, n, cutsapplied, _state);
    rgrowrowsfixedcolsm(2 * n, n, pointsfound, _state);
    bsetallocv(2 * n, ae_false, tmpsuccessflags, _state);
    candcnt = 0;
    for (vidx = 0; vidx <= n - 1; vidx++) {
      if (!state->isintegral.ptr.p_bool[vidx]) {
        continue;
      }
      for (sidx = 0; sidx <= 1; sidx++) {
        vshift = (double) (1 - 2 * sidx);
        if (nodecut->ptr.p_double[vidx] > (double) 0 && vshift < (double) 0) {
          continue;
        }
        if (nodecut->ptr.p_double[vidx] < (double) 0 && vshift > (double) 0) {
          continue;
        }
        if (state->hasbndl.ptr.p_bool[vidx]
            && xcentral->ptr.p_double[vidx] + nodecut->ptr.p_double[vidx]
                       + vshift
                   < state->bndl.ptr.p_double[vidx]) {
          continue;
        }
        if (state->hasbndu.ptr.p_bool[vidx]
            && xcentral->ptr.p_double[vidx] + nodecut->ptr.p_double[vidx]
                       + vshift
                   > state->bndu.ptr.p_double[vidx]) {
          continue;
        }
        rcopyvr(n, nodecut, cutsapplied, candcnt, _state);
        cutsapplied->ptr.pp_double[candcnt][vidx]
            = cutsapplied->ptr.pp_double[candcnt][vidx] + vshift;
        candcnt = candcnt + 1;
      }
    }
    sufficienttime  = ae_fp_greater_eq((double) (state->cuttimems),
                                      adaptiveparallelismtimerequired(_state));
    sufficientcount = ae_fp_greater_eq(
        (double) (state->cutcnt), adaptiveparallelismcountrequired(_state));
    if (state->adaptiveinternalparallelism >= 0
        && (sufficienttime || sufficientcount)) {
      state->expandcutgenerateneighborsparallelism
          = ae_fp_greater_eq(
                (double) state->cuttimems
                    / coalesce((double) (state->cutcnt), (double) (1), _state)
                    * (double) candcnt,
                workerstartthresholdms(_state))
            && candcnt >= 2;
    }
    stimerstartcond(&state->timerprepareneighbors, state->doanytrace, _state);
    mirbfvns_parallelfindnearestintegralsubjecttocut(
        state,
        xcentral,
        cutsapplied,
        pointsfound,
        tmpsuccessflags,
        0,
        candcnt,
        ae_true,
        ae_true,
        state->expandcutgenerateneighborsparallelism,
        _state);
    stimerstopcond(&state->timerprepareneighbors, state->doanytrace, _state);
    if (state->expandcutgenerateneighborsparallelism) {
      state->dbgpotentiallyparallelcutrounds
          = state->dbgpotentiallyparallelcutrounds + 1;
    } else {
      state->dbgsequentialcutrounds = state->dbgsequentialcutrounds + 1;
    }
    *nncnt = 0;
    for (candidx = 0; candidx <= candcnt - 1; candidx++) {
      if (!tmpsuccessflags->ptr.p_bool[candidx]) {
        continue;
      }
      rcopyrv(n, pointsfound, candidx, &xm, _state);
      newnodeidx = mirbfvns_gridfindorcreatenode(grid, state, &xm, _state);
      if (!ilinearsearchispresent(
              excludelist, 0, excludecnt, newnodeidx, _state)
          && !ilinearsearchispresent(
              neighbornodes, 0, *nncnt, newnodeidx, _state)) {
        igrowappendv(*nncnt + 1, neighbornodes, newnodeidx, _state);
        if (candidx > (*nncnt)) {
          rcopyrr(n, cutsapplied, candidx, cutsapplied, *nncnt, _state);
          rcopyrr(n, pointsfound, candidx, pointsfound, *nncnt, _state);
        }
        *nncnt = *nncnt + 1;
      }
    }
    ae_nxpool_recycle(&state->rpool, &xm, _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Parallel version of the gridProposeLocalTrialPoint().

  For each I in half-range [R0,R1) it loads:
  * node index from State.EvalBatchNodeIdx[I]
  * seeds from State.tmpEB0[2*I+0] and State.tmpEB0[2*I+1]
  and calls gridProposeLocalTrialPoint in parallel manner.

  IsRoot must be true on initial call (recursive calls set it to False).
  TryParallelism controls whether parallel processing is used or not.
  *************************************************************************/
  static void
  mirbfvns_gridparallelproposelocaltrialpoint(const mirbfvnsgrid*  grid,
                                              mirbfvnsgrid*        sharedgrid,
                                              const mirbfvnsstate* state,
                                              mirbfvnsstate*       sharedstate,
                                              ae_int_t             r0,
                                              ae_int_t             r1,
                                              ae_bool              isroot,
                                              ae_bool   tryparallelism,
                                              ae_state* _state) {
    ae_int_t rmid;

    if (r1 <= r0) {
      return;
    }
    if ((isroot && tryparallelism) && r1 - r0 >= 2) {
      if (_trypexec_mirbfvns_gridparallelproposelocaltrialpoint(grid,
                                                                sharedgrid,
                                                                state,
                                                                sharedstate,
                                                                r0,
                                                                r1,
                                                                isroot,
                                                                tryparallelism,
                                                                _state)) {
        return;
      }
    }
    if (r1 == r0 + 1) {
      if (mirbfvns_gridgetstatus(&state->grid,
                                 state,
                                 state->evalbatchnodeidx.ptr.p_int[r0],
                                 _state)
          == mirbfvns_nodeunexplored) {
        mirbfvns_gridproposetrialpointwhenexploringfrom(
            grid,
            sharedgrid,
            state,
            sharedstate,
            state->evalbatchnodeidx.ptr.p_int[r0],
            state->xcreachedfrom.ptr
                .p_int[state->evalbatchneighboridx.ptr.p_int[r0]],
            state->tmpeb0.ptr.p_int[2 * r0 + 0],
            state->tmpeb0.ptr.p_int[2 * r0 + 1],
            r0,
            _state);
        return;
      }
      if (state->nomask) {
        mirbfvns_gridproposelocaltrialpointnomask(
            grid,
            sharedgrid,
            state,
            sharedstate,
            state->evalbatchnodeidx.ptr.p_int[r0],
            state->tmpeb0.ptr.p_int[2 * r0 + 0],
            state->tmpeb0.ptr.p_int[2 * r0 + 1],
            r0,
            _state);
      } else {
        mirbfvns_gridproposelocaltrialpointmasked(
            grid,
            sharedgrid,
            state,
            sharedstate,
            state->evalbatchnodeidx.ptr.p_int[r0],
            state->tmpeb0.ptr.p_int[2 * r0 + 0],
            state->tmpeb0.ptr.p_int[2 * r0 + 1],
            r0,
            _state);
      }
      return;
    }
    ae_assert(r1 > r0 + 1, "MIRBFVNS: 190337 failed", _state);
    rmid = r0 + (r1 - r0) / 2;
    mirbfvns_gridparallelproposelocaltrialpoint(grid,
                                                sharedgrid,
                                                state,
                                                sharedstate,
                                                r0,
                                                rmid,
                                                ae_false,
                                                tryparallelism,
                                                _state);
    mirbfvns_gridparallelproposelocaltrialpoint(grid,
                                                sharedgrid,
                                                state,
                                                sharedstate,
                                                rmid,
                                                r1,
                                                ae_false,
                                                tryparallelism,
                                                _state);
  }

  /*************************************************************************
  Serial stub for GPL edition.
  *************************************************************************/
  ae_bool _trypexec_mirbfvns_gridparallelproposelocaltrialpoint(
      const mirbfvnsgrid*  grid,
      mirbfvnsgrid*        sharedgrid,
      const mirbfvnsstate* state,
      mirbfvnsstate*       sharedstate,
      ae_int_t             r0,
      ae_int_t             r1,
      ae_bool              isroot,
      ae_bool              tryparallelism,
      ae_state*            _state) {
    return ae_false;
  }

  /*************************************************************************
  Having node with  status=nodeInProgress,  proposes  trial  point  for  the
  exploration. Raises an exception for nodes with statuses different from
  nodeInProgress, e.g. unexplored.

  Use gridProposeTrialPointWhenExploringFrom() to start exploration of an
  unexplored node.

  Grid and State are passed twice: first as a constant reference, second as
  a shared one. The idea is to separate potentially thread-unsafe accesses
  from read-only ones that are safe to do.

  The result is written into SharedState.EvalBatchPoints[], row EvalBatchIdx.
  *************************************************************************/
  static void
  mirbfvns_gridproposelocaltrialpointnomask(const mirbfvnsgrid*  grid,
                                            mirbfvnsgrid*        sharedgrid,
                                            const mirbfvnsstate* state,
                                            mirbfvnsstate*       sharedstate,
                                            ae_int_t             nodeidx,
                                            ae_int_t             rngseedtouse0,
                                            ae_int_t             rngseedtouse1,
                                            ae_int_t             evalbatchidx,
                                            ae_state*            _state) {
    ae_frame               _frame_block;
    mirbfvnsnodesubsolver* subsolver;
    ae_smart_ptr           _subsolver;
    mirbfvnstemporaries*   buf;
    ae_smart_ptr           _buf;
    double                 f;
    double                 h;
    ae_int_t               nfrac;
    ae_int_t               fulln;
    ae_int_t               nnlc;
    ae_int_t               k;
    ae_int_t               i;
    ae_int_t               j;
    ae_int_t               jj;
    ae_int_t               j0;
    ae_int_t               j1;
    ae_int_t               offs;
    ae_int_t               ortbasissize;
    ae_int_t               nextlistpos;
    ae_int_t               npoints;
    ae_int_t               candidx;
    ae_int_t               lastacceptedidx;
    ae_int_t               subsolverits;
    double                 v;
    double                 v0;
    double                 v1;
    double                 mindistinf;
    double                 vmax;
    ae_bool                updatestats;

    ae_frame_make(_state, &_frame_block);
    memset(&_subsolver, 0, sizeof(_subsolver));
    memset(&_buf, 0, sizeof(_buf));
    ae_smart_ptr_init(
        &_subsolver, (void**) &subsolver, ae_false, _state, ae_true);
    ae_smart_ptr_init(&_buf, (void**) &buf, ae_false, _state, ae_true);

    fulln       = state->n;
    nfrac       = state->nfrac;
    nnlc        = state->nnlc;
    updatestats = state->doanytrace || state->adaptiveinternalparallelism >= 0;
    ae_assert(state->nomask, "MIRBFVNS: 331952 failed", _state);
    ae_assert(ae_fp_less_eq(mirbfvns_rbfpointtooclose, mirbfvns_rbfsktooshort)
                  && ae_fp_greater(mirbfvns_rbfsktooshort, (double) (0)),
              "MIRBFVNS: integrity check 498747 for control parameters failed",
              _state);
    ae_assert(
        nodeidx >= 0 && nodeidx < grid->nnodes, "MIRBFVNS: 075437", _state);
    ae_assert(mirbfvns_gridgetstatus(grid, state, nodeidx, _state)
                  == mirbfvns_nodeinprogress,
              "MIRBFVNS: 076438",
              _state);
    ae_assert(state->nfrac > 0, "MIRBFVNS: 086602", _state);
    ae_obj_array_get(&sharedgrid->subsolvers, nodeidx, &_subsolver, _state);
    ae_assert(subsolver != NULL, "MIRBFVNS: 266533", _state);
    ae_shared_pool_retrieve(&sharedstate->tmppool, &_buf, _state);
    stimerinit(&buf->localtimer, _state);
    stimerstartcond(&buf->localtimer, updatestats, _state);
    hqrndseed(rngseedtouse0, rngseedtouse1, &buf->localrng, _state);
    isetallocv(fulln, -1, &buf->mapfull2compact, _state);
    rallocv(nfrac, &buf->glbbndl, _state);
    rallocv(nfrac, &buf->glbbndu, _state);
    rallocv(nfrac, &buf->glbs, _state);
    rsetallocv(nfrac, subsolver->trustrad, &buf->glbvtrustregion, _state);
    for (i = 0; i <= nfrac - 1; i++) {
      j                            = state->idxfrac.ptr.p_int[i];
      buf->glbs.ptr.p_double[i]    = state->s.ptr.p_double[j];
      buf->glbbndl.ptr.p_double[i] = state->bndl.ptr.p_double[j];
      buf->glbbndu.ptr.p_double[i] = state->bndu.ptr.p_double[j];
      buf->glbvtrustregion.ptr.p_double[i]
          = buf->glbvtrustregion.ptr.p_double[i] * buf->glbs.ptr.p_double[i];
      buf->mapfull2compact.ptr.p_int[j] = i;
    }
    npoints = 1;
    rgrowrowsfixedcolsm(npoints, nfrac + 1 + nnlc, &buf->glbxf, _state);
    rgrowrowsfixedcolsm(npoints, nfrac, &buf->glbsx, _state);
    lastacceptedidx
        = ae_round(grid->nodesinfo.ptr
                       .pp_double[nodeidx][fulln + mirbfvns_ncollastaccepted],
                   _state);
    rallocv(nfrac, &buf->glbx0, _state);
    for (j = 0; j <= nfrac - 1; j++) {
      buf->glbx0.ptr.p_double[j]
          = state->dataset.pointinfo.ptr
                .pp_double[lastacceptedidx][state->idxfrac.ptr.p_int[j]];
      buf->glbxf.ptr.pp_double[0][j] = buf->glbx0.ptr.p_double[j];
      buf->glbsx.ptr.pp_double[0][j]
          = buf->glbx0.ptr.p_double[j] / buf->glbs.ptr.p_double[j];
    }
    for (j = 0; j <= nnlc; j++) {
      buf->glbxf.ptr.pp_double[0][nfrac + j]
          = state->dataset.pointinfo.ptr.pp_double[lastacceptedidx][fulln + j];
    }
    rallocv(fulln, &buf->fullx0, _state);
    rcopyrv(fulln,
            &state->dataset.pointinfo,
            lastacceptedidx,
            &buf->fullx0,
            _state);
    f = state->dataset.pointinfo.ptr.pp_double[lastacceptedidx][fulln];
    h = state->dataset.pointinfo.ptr
            .pp_double[lastacceptedidx][fulln + 1 + nnlc + 0];
    subsolver->basef = f;
    subsolver->baseh = h;
    nextlistpos      = grid->ptlistheads.ptr.p_int[2 * nodeidx + 0];
    ae_assert(nextlistpos >= 0
                  && grid->ptlistheads.ptr.p_int[2 * nodeidx + 1] > 0,
              "MIRBFVNS: 352426",
              _state);
    while (nextlistpos >= 0
           && npoints < mirbfvns_rbfcloudsizemultiplier * nfrac + 1) {
      candidx     = grid->ptlistdata.ptr.p_int[2 * nextlistpos + 0];
      nextlistpos = grid->ptlistdata.ptr.p_int[2 * nextlistpos + 1];
      if (candidx == lastacceptedidx) {
        continue;
      }
      rgrowrowsfixedcolsm(npoints + 1, nfrac + 1 + nnlc, &buf->glbxf, _state);
      rgrowrowsfixedcolsm(npoints + 1, nfrac, &buf->glbsx, _state);
      for (j = 0; j <= nfrac - 1; j++) {
        buf->glbxf.ptr.pp_double[npoints][j]
            = state->dataset.pointinfo.ptr
                  .pp_double[candidx][state->idxfrac.ptr.p_int[j]];
        buf->glbsx.ptr.pp_double[npoints][j]
            = buf->glbxf.ptr.pp_double[npoints][j] / buf->glbs.ptr.p_double[j];
      }
      for (j = 0; j <= nnlc; j++) {
        buf->glbxf.ptr.pp_double[npoints][nfrac + j]
            = state->dataset.pointinfo.ptr.pp_double[candidx][fulln + j];
      }
      if (ae_fp_greater(
              mirbfvns_rdistinfrr(
                  nfrac, &buf->glbsx, npoints, &buf->glbsx, 0, _state),
              mirbfvns_rbfpointunacceptablyfar * subsolver->trustrad)) {
        continue;
      }
      mindistinf = ae_maxrealnumber;
      for (i = 0; i <= npoints - 1; i++) {
        mindistinf = ae_minreal(
            mindistinf,
            mirbfvns_rdistinfrr(
                nfrac, &buf->glbsx, npoints, &buf->glbsx, i, _state),
            _state);
      }
      if (ae_fp_less(mindistinf,
                     mirbfvns_rbfpointtooclose * subsolver->trustrad)) {
        continue;
      }
      npoints = npoints + 1;
    }
    subsolver->sufficientcloudsize = npoints >= nfrac + 1;
    if (!subsolver->sufficientcloudsize) {
      rallocv(nfrac, &buf->glbtmp0, _state);
      rallocv(nfrac, &buf->glbtmp1, _state);
      rallocv(nfrac, &buf->glbtmp2, _state);
      rallocm(npoints - 1, nfrac, &buf->ortdeltas, _state);
      ortbasissize = 0;
      for (i = 0; i <= npoints - 2; i++) {
        rcopyrv(nfrac, &buf->glbxf, i, &buf->glbtmp0, _state);
        raddrv(nfrac,
               (double) (-1),
               &buf->glbxf,
               npoints - 1,
               &buf->glbtmp0,
               _state);
        v0 = ae_sqrt(rdotv2(nfrac, &buf->glbtmp0, _state), _state);
        if (ae_fp_eq(v0, (double) (0))) {
          continue;
        }
        rowwisegramschmidt(&buf->ortdeltas,
                           ortbasissize,
                           nfrac,
                           &buf->glbtmp0,
                           &buf->glbtmp0,
                           ae_false,
                           _state);
        v1 = ae_sqrt(rdotv2(nfrac, &buf->glbtmp0, _state), _state);
        if (ae_fp_eq(v1, (double) (0))) {
          continue;
        }
        rmulv(nfrac, (double) 1 / v1, &buf->glbtmp0, _state);
        rcopyvr(nfrac, &buf->glbtmp0, &buf->ortdeltas, ortbasissize, _state);
        ortbasissize = ortbasissize + 1;
      }
      rsetv(nfrac, 0.0, &buf->glbtmp1, _state);
      vmax = (double) (-1);
      for (k = 0; k <= 4; k++) {
        for (i = 0; i <= nfrac - 1; i++) {
          v0 = buf->glbx0.ptr.p_double[i]
               - buf->glbvtrustregion.ptr.p_double[i];
          v1 = buf->glbx0.ptr.p_double[i]
               + buf->glbvtrustregion.ptr.p_double[i];
          if (ae_isfinite(buf->glbbndl.ptr.p_double[i], _state)) {
            v0 = ae_maxreal(v0, buf->glbbndl.ptr.p_double[i], _state);
          }
          if (ae_isfinite(buf->glbbndu.ptr.p_double[i], _state)) {
            v1 = ae_minreal(v1, buf->glbbndu.ptr.p_double[i], _state);
          }
          buf->glbtmp0.ptr.p_double[i]
              = boundval(v0 + (v1 - v0) * hqrnduniformr(&buf->localrng, _state),
                         v0,
                         v1,
                         _state);
        }
        rcopyv(nfrac, &buf->glbtmp0, &buf->glbtmp2, _state);
        rowwisegramschmidt(&buf->ortdeltas,
                           ortbasissize,
                           nfrac,
                           &buf->glbtmp2,
                           &buf->glbtmp2,
                           ae_false,
                           _state);
        v = rdotv2(nfrac, &buf->glbtmp2, _state);
        if (ae_fp_greater_eq(v, vmax)) {
          rcopyv(nfrac, &buf->glbtmp0, &buf->glbtmp1, _state);
          vmax = v;
        }
      }
      subsolver->skrellen = (double) (0);
      for (i = 0; i <= nfrac - 1; i++) {
        v = buf->glbtmp1.ptr.p_double[i];
        sharedstate->evalbatchpoints.ptr
            .pp_double[evalbatchidx][state->idxfrac.ptr.p_int[i]]
            = v;
        subsolver->skrellen
            = ae_maxreal(subsolver->skrellen,
                         ae_fabs(v - buf->glbx0.ptr.p_double[i], _state)
                             / buf->glbvtrustregion.ptr.p_double[i],
                         _state);
      }
      for (i = 0; i <= state->nint - 1; i++) {
        sharedstate->evalbatchpoints.ptr
            .pp_double[evalbatchidx][state->idxint.ptr.p_int[i]]
            = grid->nodesinfo.ptr
                  .pp_double[nodeidx][state->idxint.ptr.p_int[i]];
      }
      stimerstopcond(&buf->localtimer, updatestats, _state);
      if (updatestats) {
        weakatomicfetchadd(&sharedstate->localtrialsamplingcnt, 1, _state);
        weakatomicfetchadd(&sharedstate->localtrialsamplingtimems,
                           stimergetmsint(&buf->localtimer, _state),
                           _state);
      }
    } else {
      hqrndnormalm(
          &buf->localrng, 1 + nnlc, nfrac + 1, &buf->glbrandomprior, _state);
      for (i = 0; i <= nnlc; i++) {
        rmergedivvr(nfrac, &buf->glbs, &buf->glbrandomprior, i, _state);
      }
      rallocv(1 + nnlc, &buf->glbprioratx0, _state);
      rgemv(1 + nnlc,
            nfrac,
            1.0,
            &buf->glbrandomprior,
            0,
            &buf->glbx0,
            0.0,
            &buf->glbprioratx0,
            _state);
      rsetc(1 + nnlc, 0.0, &buf->glbrandomprior, nfrac, _state);
      for (i = 0; i <= npoints - 1; i++) {
        for (j = 0; j <= nnlc; j++) {
          buf->glbxf.ptr.pp_double[i][nfrac + j]
              = buf->glbxf.ptr.pp_double[i][nfrac + j]
                - (rdotrr(
                       nfrac, &buf->glbrandomprior, j, &buf->glbxf, i, _state)
                   - buf->glbprioratx0.ptr.p_double[j]);
        }
      }
      if (state->lccnt > 0) {
        ae_assert(sparseiscrs(&state->rawa, _state),
                  "MIRBFVNS: 095629 failed",
                  _state);
        rsetallocv(state->lccnt, _state->v_neginf, &buf->glbal, _state);
        rsetallocv(state->lccnt, _state->v_posinf, &buf->glbau, _state);
        buf->glba.m = state->lccnt;
        buf->glba.n = nfrac;
        iallocv(state->lccnt + 1, &buf->glba.ridx, _state);
        buf->glba.ridx.ptr.p_int[0] = 0;
        for (i = 0; i <= state->lccnt - 1; i++) {
          v    = 0.0;
          offs = buf->glba.ridx.ptr.p_int[i];
          igrowv(offs + nfrac, &buf->glba.idx, _state);
          rgrowv(offs + nfrac, &buf->glba.vals, _state);
          j0 = state->rawa.ridx.ptr.p_int[i];
          j1 = state->rawa.ridx.ptr.p_int[i + 1] - 1;
          for (jj = j0; jj <= j1; jj++) {
            j = state->rawa.idx.ptr.p_int[jj];
            if (!state->isintegral.ptr.p_bool[j]) {
              buf->glba.idx.ptr.p_int[offs] = buf->mapfull2compact.ptr.p_int[j];
              buf->glba.vals.ptr.p_double[offs]
                  = state->rawa.vals.ptr.p_double[jj];
              offs = offs + 1;
            } else {
              v = v
                  + buf->fullx0.ptr.p_double[j]
                        * state->rawa.vals.ptr.p_double[jj];
            }
          }
          buf->glba.ridx.ptr.p_int[i + 1] = offs;
          if (ae_isfinite(state->rawal.ptr.p_double[i], _state)) {
            buf->glbal.ptr.p_double[i] = state->rawal.ptr.p_double[i] - v;
          }
          if (ae_isfinite(state->rawau.ptr.p_double[i], _state)) {
            buf->glbau.ptr.p_double[i] = state->rawau.ptr.p_double[i] - v;
          }
        }
        sparsecreatecrsinplace(&buf->glba, _state);
      }
      rsetallocv(
          nfrac, (double) 1 / subsolver->trustrad, &buf->glbmultscale, _state);
      rmergedivv(nfrac, &buf->glbs, &buf->glbmultscale, _state);
      mirbfvns_rbfinitmodel(&buf->glbxf,
                            &buf->glbmultscale,
                            npoints,
                            nfrac,
                            1 + nnlc,
                            &buf->glbmodel,
                            _state);
      mirbfvns_rbfaddlinearterm(&buf->glbmodel, &buf->glbrandomprior, _state);
      rallocv(nfrac, &buf->glbxtrial, _state);
      mirbfvns_rbfminimizemodel(&buf->glbmodel,
                                &buf->glbx0,
                                &buf->glbbndl,
                                &buf->glbbndu,
                                &buf->glbvtrustregion,
                                subsolver->trustrad,
                                state->ctol,
                                mirbfvns_rbfminimizeitsperphase,
                                ae_false,
                                &buf->glba,
                                &buf->glbal,
                                &buf->glbau,
                                state->lccnt,
                                &state->nl,
                                &state->nu,
                                state->nnlc,
                                nfrac,
                                &buf->mmbuf,
                                &buf->glbxtrial,
                                &buf->glbsk,
                                &subsolver->predf,
                                &subsolver->predh,
                                &subsolverits,
                                _state);
      subsolver->skrellen = (double) (0);
      for (i = 0; i <= nfrac - 1; i++) {
        subsolver->skrellen
            = ae_maxreal(subsolver->skrellen,
                         ae_fabs(buf->glbsk.ptr.p_double[i]
                                     / buf->glbvtrustregion.ptr.p_double[i],
                                 _state),
                         _state);
        sharedstate->evalbatchpoints.ptr
            .pp_double[evalbatchidx][state->idxfrac.ptr.p_int[i]]
            = buf->glbxtrial.ptr.p_double[i];
      }
      for (i = 0; i <= state->nint - 1; i++) {
        sharedstate->evalbatchpoints.ptr
            .pp_double[evalbatchidx][state->idxint.ptr.p_int[i]]
            = grid->nodesinfo.ptr
                  .pp_double[nodeidx][state->idxint.ptr.p_int[i]];
      }
      stimerstopcond(&buf->localtimer, updatestats, _state);
      weakatomicfetchadd(&sharedstate->repsubsolverits, subsolverits, _state);
      if (updatestats) {
        weakatomicfetchadd(&sharedstate->localtrialrbfcnt, 1, _state);
        weakatomicfetchadd(&sharedstate->localtrialrbftimems,
                           stimergetmsint(&buf->localtimer, _state),
                           _state);
      }
    }
    ae_shared_pool_recycle(&sharedstate->tmppool, &_buf, _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Having node with  status=nodeInProgress,  proposes  trial  point  for  the
  exploration. Raises an exception for nodes with statuses different from
  nodeInProgress, e.g. unexplored.

  Use gridProposeTrialPointWhenExploringFrom() to start exploration of an
  unexplored node.

  Grid and State are passed twice: first as a constant reference, second as
  a shared one. The idea is to separate potentially thread-unsafe accesses
  from read-only ones that are safe to do.

  The result is written into SharedState.EvalBatchPoints[], row EvalBatchIdx.
  *************************************************************************/
  static void
  mirbfvns_gridproposelocaltrialpointmasked(const mirbfvnsgrid*  grid,
                                            mirbfvnsgrid*        sharedgrid,
                                            const mirbfvnsstate* state,
                                            mirbfvnsstate*       sharedstate,
                                            ae_int_t             nodeidx,
                                            ae_int_t             rngseedtouse0,
                                            ae_int_t             rngseedtouse1,
                                            ae_int_t             evalbatchidx,
                                            ae_state*            _state) {
    ae_frame               _frame_block;
    mirbfvnsnodesubsolver* subsolver;
    ae_smart_ptr           _subsolver;
    mirbfvnstemporaries*   buf;
    ae_smart_ptr           _buf;
    double                 f;
    double                 h;
    ae_int_t               fidx;
    ae_int_t               curnfrac;
    ae_int_t               fulln;
    ae_int_t               nnlc;
    ae_int_t               k;
    ae_int_t               i;
    ae_int_t               j;
    ae_int_t               jj;
    ae_int_t               j0;
    ae_int_t               j1;
    ae_int_t               offs;
    ae_int_t               nodescnt;
    ae_int_t               nextlistpos;
    ae_int_t               npoints;
    ae_int_t               candidx;
    ae_int_t               lastacceptedidx;
    ae_int_t               subsolverits;
    double                 v;
    double                 v0;
    double                 v1;
    double                 mindistinf;
    double                 prioratx0;
    ae_bool                updatestats;

    ae_frame_make(_state, &_frame_block);
    memset(&_subsolver, 0, sizeof(_subsolver));
    memset(&_buf, 0, sizeof(_buf));
    ae_smart_ptr_init(
        &_subsolver, (void**) &subsolver, ae_false, _state, ae_true);
    ae_smart_ptr_init(&_buf, (void**) &buf, ae_false, _state, ae_true);

    fulln       = state->n;
    nnlc        = state->nnlc;
    updatestats = state->dotrace || state->adaptiveinternalparallelism >= 0;
    ae_assert(!state->nomask, "MIRBFVNS: 676607 failed", _state);
    ae_assert(ae_fp_less_eq(mirbfvns_rbfpointtooclose, mirbfvns_rbfsktooshort)
                  && ae_fp_greater(mirbfvns_rbfsktooshort, (double) (0)),
              "MIRBFVNS: integrity check 498747 for control parameters failed",
              _state);
    ae_assert(
        nodeidx >= 0 && nodeidx < grid->nnodes, "MIRBFVNS: 075437", _state);
    ae_assert(mirbfvns_gridgetstatus(grid, state, nodeidx, _state)
                  == mirbfvns_nodeinprogress,
              "MIRBFVNS: 076438",
              _state);
    ae_assert(state->nfrac > 0, "MIRBFVNS: 086602", _state);
    ae_obj_array_get(&sharedgrid->subsolvers, nodeidx, &_subsolver, _state);
    ae_assert(subsolver != NULL, "MIRBFVNS: 266533", _state);
    ae_shared_pool_retrieve(&sharedstate->tmppool, &_buf, _state);
    stimerinit(&buf->localtimer, _state);
    stimerstartcond(&buf->localtimer, updatestats, _state);
    hqrndseed(rngseedtouse0, rngseedtouse1, &buf->localrng, _state);
    lastacceptedidx
        = ae_round(grid->nodesinfo.ptr
                       .pp_double[nodeidx][fulln + mirbfvns_ncollastaccepted],
                   _state);
    rallocv(fulln, &buf->fullx0, _state);
    rcopyrv(fulln,
            &state->dataset.pointinfo,
            lastacceptedidx,
            &buf->fullx0,
            _state);
    f = state->dataset.pointinfo.ptr.pp_double[lastacceptedidx][fulln];
    h = state->dataset.pointinfo.ptr
            .pp_double[lastacceptedidx][fulln + 1 + nnlc + 0];
    subsolver->basef = f;
    subsolver->baseh = h;
    isetallocv(fulln, -1, &buf->mapfull2compact, _state);
    rallocv(state->nfrac, &buf->glbbndl, _state);
    rallocv(state->nfrac, &buf->glbbndu, _state);
    rallocv(state->nfrac, &buf->glbs, _state);
    rallocv(state->nfrac, &buf->glbx0, _state);
    rsetallocv(
        state->nfrac, subsolver->trustrad, &buf->glbvtrustregion, _state);
    for (i = 0; i <= state->nfrac - 1; i++) {
      j                            = state->idxfrac.ptr.p_int[i];
      buf->glbs.ptr.p_double[i]    = state->s.ptr.p_double[j];
      buf->glbbndl.ptr.p_double[i] = state->bndl.ptr.p_double[j];
      buf->glbbndu.ptr.p_double[i] = state->bndu.ptr.p_double[j];
      buf->glbvtrustregion.ptr.p_double[i]
          = buf->glbvtrustregion.ptr.p_double[i] * buf->glbs.ptr.p_double[i];
      buf->glbx0.ptr.p_double[i]
          = state->dataset.pointinfo.ptr
                .pp_double[lastacceptedidx][state->idxfrac.ptr.p_int[i]];
      buf->mapfull2compact.ptr.p_int[j] = i;
    }
    rsetallocv(state->nfrac,
               (double) 1 / subsolver->trustrad,
               &buf->glbmultscale,
               _state);
    rmergedivv(state->nfrac, &buf->glbs, &buf->glbmultscale, _state);
    if (state->lccnt > 0) {
      ae_assert(
          sparseiscrs(&state->rawa, _state), "MIRBFVNS: 095629 failed", _state);
      rsetallocv(state->lccnt, _state->v_neginf, &buf->glbal, _state);
      rsetallocv(state->lccnt, _state->v_posinf, &buf->glbau, _state);
      buf->glba.m = state->lccnt;
      buf->glba.n = state->nfrac;
      iallocv(state->lccnt + 1, &buf->glba.ridx, _state);
      buf->glba.ridx.ptr.p_int[0] = 0;
      for (i = 0; i <= state->lccnt - 1; i++) {
        v    = 0.0;
        offs = buf->glba.ridx.ptr.p_int[i];
        igrowv(offs + state->nfrac, &buf->glba.idx, _state);
        rgrowv(offs + state->nfrac, &buf->glba.vals, _state);
        j0 = state->rawa.ridx.ptr.p_int[i];
        j1 = state->rawa.ridx.ptr.p_int[i + 1] - 1;
        for (jj = j0; jj <= j1; jj++) {
          j = state->rawa.idx.ptr.p_int[jj];
          if (!state->isintegral.ptr.p_bool[j]) {
            buf->glba.idx.ptr.p_int[offs] = buf->mapfull2compact.ptr.p_int[j];
            buf->glba.vals.ptr.p_double[offs]
                = state->rawa.vals.ptr.p_double[jj];
            offs = offs + 1;
          } else {
            v = v
                + buf->fullx0.ptr.p_double[j]
                      * state->rawa.vals.ptr.p_double[jj];
          }
        }
        buf->glba.ridx.ptr.p_int[i + 1] = offs;
        if (ae_isfinite(state->rawal.ptr.p_double[i], _state)) {
          buf->glbal.ptr.p_double[i] = state->rawal.ptr.p_double[i] - v;
        }
        if (ae_isfinite(state->rawau.ptr.p_double[i], _state)) {
          buf->glbau.ptr.p_double[i] = state->rawau.ptr.p_double[i] - v;
        }
      }
      sparsecreatecrsinplace(&buf->glba, _state);
    }
    mirbfvns_rbfinitemptysparsemodel(
        &buf->glbmultscale, state->nfrac, &buf->glbmodel, _state);
    subsolver->sufficientcloudsize = ae_true;
    for (fidx = 0; fidx <= nnlc; fidx++) {
      if (state->hasmask.ptr.p_bool[fidx]) {
        bsetallocv(fulln, ae_false, &buf->glbmask, _state);
        j0 = state->varmask.ridx.ptr.p_int[fidx];
        j1 = state->varmask.ridx.ptr.p_int[fidx + 1] - 1;
        for (jj = j0; jj <= j1; jj++) {
          buf->glbmask.ptr.p_bool[state->varmask.idx.ptr.p_int[jj]] = ae_true;
        }
      } else {
        bsetallocv(fulln, ae_true, &buf->glbmask, _state);
      }
      iallocv(state->nfrac, &buf->lclidxfrac, _state);
      iallocv(state->nfrac, &buf->lcl2glb, _state);
      rallocv(state->nfrac, &buf->lcls, _state);
      rallocv(state->nfrac, &buf->lclmultscale, _state);
      curnfrac = 0;
      for (i = 0; i <= state->nfrac - 1; i++) {
        if (buf->glbmask.ptr.p_bool[state->idxfrac.ptr.p_int[i]]) {
          buf->lclidxfrac.ptr.p_int[curnfrac] = state->idxfrac.ptr.p_int[i];
          buf->lcl2glb.ptr.p_int[curnfrac]    = i;
          buf->lcls.ptr.p_double[curnfrac]    = buf->glbs.ptr.p_double[i];
          buf->lclmultscale.ptr.p_double[curnfrac]
              = buf->glbmultscale.ptr.p_double[i];
          curnfrac = curnfrac + 1;
        }
      }
      if (curnfrac == 0) {
        mirbfvns_rbfappendconstantmodel(
            &buf->glbmodel,
            state->dataset.pointinfo.ptr
                .pp_double[lastacceptedidx][fulln + fidx],
            _state);
        continue;
      }
      npoints = 1;
      rgrowrowsfixedcolsm(npoints, curnfrac + 1, &buf->lclxf, _state);
      rgrowrowsfixedcolsm(npoints, curnfrac, &buf->lclsx, _state);
      for (j = 0; j <= curnfrac - 1; j++) {
        buf->lclxf.ptr.pp_double[0][j]
            = state->dataset.pointinfo.ptr
                  .pp_double[lastacceptedidx][buf->lclidxfrac.ptr.p_int[j]];
        buf->lclsx.ptr.pp_double[0][j]
            = buf->lclxf.ptr.pp_double[0][j]
              / state->s.ptr.p_double[buf->lclidxfrac.ptr.p_int[j]];
      }
      buf->lclxf.ptr.pp_double[0][curnfrac]
          = state->dataset.pointinfo.ptr
                .pp_double[lastacceptedidx][fulln + fidx];
      mirbfvns_gridfindnodeslike(grid,
                                 state,
                                 nodeidx,
                                 ae_true,
                                 &buf->glbmask,
                                 &buf->nodeslist,
                                 &nodescnt,
                                 _state);
      ae_assert(nodescnt >= 1 && buf->nodeslist.ptr.p_int[0] == nodeidx,
                "MIRBFVNS: 773613 failed",
                _state);
      for (k = 0; k <= nodescnt - 1; k++) {
        if (grid->ptlistheads.ptr.p_int[2 * buf->nodeslist.ptr.p_int[k] + 1]
            == 0) {
          continue;
        }
        nextlistpos
            = grid->ptlistheads.ptr.p_int[2 * buf->nodeslist.ptr.p_int[k] + 0];
        ae_assert(nextlistpos >= 0
                      && grid->ptlistheads.ptr
                                 .p_int[2 * buf->nodeslist.ptr.p_int[k] + 1]
                             > 0,
                  "MIRBFVNS: 904319",
                  _state);
        while (nextlistpos >= 0
               && npoints < mirbfvns_rbfcloudsizemultiplier * curnfrac + 1) {
          candidx     = grid->ptlistdata.ptr.p_int[2 * nextlistpos + 0];
          nextlistpos = grid->ptlistdata.ptr.p_int[2 * nextlistpos + 1];
          rgrowrowsfixedcolsm(npoints + 1, curnfrac + 1, &buf->lclxf, _state);
          rgrowrowsfixedcolsm(npoints + 1, curnfrac, &buf->lclsx, _state);
          for (j = 0; j <= curnfrac - 1; j++) {
            buf->lclxf.ptr.pp_double[npoints][j]
                = state->dataset.pointinfo.ptr
                      .pp_double[candidx][buf->lclidxfrac.ptr.p_int[j]];
            buf->lclsx.ptr.pp_double[npoints][j]
                = buf->lclxf.ptr.pp_double[npoints][j]
                  / state->s.ptr.p_double[buf->lclidxfrac.ptr.p_int[j]];
          }
          buf->lclxf.ptr.pp_double[npoints][curnfrac]
              = state->dataset.pointinfo.ptr.pp_double[candidx][fulln + fidx];
          if (ae_fp_greater(
                  mirbfvns_rdistinfrr(
                      curnfrac, &buf->lclsx, npoints, &buf->lclsx, 0, _state),
                  mirbfvns_rbfpointunacceptablyfar * subsolver->trustrad)) {
            continue;
          }
          mindistinf = ae_maxrealnumber;
          for (i = 0; i <= npoints - 1; i++) {
            mindistinf = ae_minreal(
                mindistinf,
                mirbfvns_rdistinfrr(
                    curnfrac, &buf->lclsx, npoints, &buf->lclsx, i, _state),
                _state);
          }
          if (ae_fp_less(mindistinf,
                         mirbfvns_rbfpointtooclose * subsolver->trustrad)) {
            continue;
          }
          npoints = npoints + 1;
        }
      }
      subsolver->sufficientcloudsize
          = subsolver->sufficientcloudsize && npoints >= curnfrac + 1;
      hqrndnormalm(
          &buf->localrng, 1, curnfrac + 1, &buf->lclrandomprior, _state);
      rmergedivvr(curnfrac, &buf->lcls, &buf->lclrandomprior, 0, _state);
      prioratx0
          = rdotrr(curnfrac, &buf->lclxf, 0, &buf->lclrandomprior, 0, _state);
      buf->lclrandomprior.ptr.pp_double[0][curnfrac] = 0.0;
      for (i = 0; i <= npoints - 1; i++) {
        buf->lclxf.ptr.pp_double[i][curnfrac]
            = buf->lclxf.ptr.pp_double[i][curnfrac]
              - (rdotrr(
                     curnfrac, &buf->lclrandomprior, 0, &buf->lclxf, i, _state)
                 - prioratx0);
      }
      mirbfvns_rbfinitmodel(&buf->lclxf,
                            &buf->lclmultscale,
                            npoints,
                            curnfrac,
                            1,
                            &buf->tmpmodel,
                            _state);
      mirbfvns_rbfaddlinearterm(&buf->tmpmodel, &buf->lclrandomprior, _state);
      mirbfvns_rbfappendmodel(
          &buf->glbmodel, &buf->tmpmodel, &buf->lcl2glb, _state);
    }
    if (!subsolver->sufficientcloudsize) {
      subsolver->skrellen = (double) (0);
      for (i = 0; i <= state->nfrac - 1; i++) {
        v0 = buf->glbx0.ptr.p_double[i] - buf->glbvtrustregion.ptr.p_double[i];
        v1 = buf->glbx0.ptr.p_double[i] + buf->glbvtrustregion.ptr.p_double[i];
        if (ae_isfinite(buf->glbbndl.ptr.p_double[i], _state)) {
          v0 = ae_maxreal(v0, buf->glbbndl.ptr.p_double[i], _state);
        }
        if (ae_isfinite(buf->glbbndu.ptr.p_double[i], _state)) {
          v1 = ae_minreal(v1, buf->glbbndu.ptr.p_double[i], _state);
        }
        v = boundval(v0 + (v1 - v0) * hqrnduniformr(&buf->localrng, _state),
                     v0,
                     v1,
                     _state);
        sharedstate->evalbatchpoints.ptr
            .pp_double[evalbatchidx][state->idxfrac.ptr.p_int[i]]
            = v;
        subsolver->skrellen
            = ae_maxreal(subsolver->skrellen,
                         ae_fabs(v - buf->glbx0.ptr.p_double[i], _state)
                             / buf->glbvtrustregion.ptr.p_double[i],
                         _state);
      }
      for (i = 0; i <= state->nint - 1; i++) {
        sharedstate->evalbatchpoints.ptr
            .pp_double[evalbatchidx][state->idxint.ptr.p_int[i]]
            = grid->nodesinfo.ptr
                  .pp_double[nodeidx][state->idxint.ptr.p_int[i]];
      }
      stimerstopcond(&buf->localtimer, updatestats, _state);
      if (updatestats) {
        weakatomicfetchadd(&sharedstate->localtrialsamplingcnt, 1, _state);
        weakatomicfetchadd(&sharedstate->localtrialsamplingtimems,
                           stimergetmsint(&buf->localtimer, _state),
                           _state);
      }
    } else {
      rallocv(state->nfrac, &buf->glbxtrial, _state);
      mirbfvns_rbfminimizemodel(&buf->glbmodel,
                                &buf->glbx0,
                                &buf->glbbndl,
                                &buf->glbbndu,
                                &buf->glbvtrustregion,
                                subsolver->trustrad,
                                state->ctol,
                                mirbfvns_rbfminimizeitsperphase,
                                ae_false,
                                &buf->glba,
                                &buf->glbal,
                                &buf->glbau,
                                state->lccnt,
                                &state->nl,
                                &state->nu,
                                state->nnlc,
                                state->nfrac,
                                &buf->mmbuf,
                                &buf->glbxtrial,
                                &buf->glbsk,
                                &subsolver->predf,
                                &subsolver->predh,
                                &subsolverits,
                                _state);
      subsolver->skrellen = (double) (0);
      for (i = 0; i <= state->nfrac - 1; i++) {
        subsolver->skrellen
            = ae_maxreal(subsolver->skrellen,
                         ae_fabs(buf->glbsk.ptr.p_double[i]
                                     / buf->glbvtrustregion.ptr.p_double[i],
                                 _state),
                         _state);
        sharedstate->evalbatchpoints.ptr
            .pp_double[evalbatchidx][state->idxfrac.ptr.p_int[i]]
            = buf->glbxtrial.ptr.p_double[i];
      }
      for (i = 0; i <= state->nint - 1; i++) {
        sharedstate->evalbatchpoints.ptr
            .pp_double[evalbatchidx][state->idxint.ptr.p_int[i]]
            = grid->nodesinfo.ptr
                  .pp_double[nodeidx][state->idxint.ptr.p_int[i]];
      }
      stimerstopcond(&buf->localtimer, updatestats, _state);
      weakatomicfetchadd(&sharedstate->repsubsolverits, subsolverits, _state);
      if (updatestats) {
        weakatomicfetchadd(&sharedstate->localtrialrbfcnt, 1, _state);
        weakatomicfetchadd(&sharedstate->localtrialrbftimems,
                           stimergetmsint(&buf->localtimer, _state),
                           _state);
      }
    }
    ae_shared_pool_recycle(&sharedstate->tmppool, &_buf, _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Propose initial values of fractional variables that are used to start
  exploration of node #NewNodeIdx from #ExploreFromNode.

  The function loads XTrial with fixed and fractional variables. Values of
  fractional variables are derived from that at #ExploreFromNode.
  *************************************************************************/
  static void
  mirbfvns_gridproposetrialpointwhenexploringfrom(const mirbfvnsgrid* grid,
                                                  mirbfvnsgrid* sharedgrid,
                                                  const mirbfvnsstate* state,
                                                  mirbfvnsstate* sharedstate,
                                                  ae_int_t       newnodeidx,
                                                  ae_int_t  explorefromnode,
                                                  ae_int_t  rngseedtouse0,
                                                  ae_int_t  rngseedtouse1,
                                                  ae_int_t  evalbatchidx,
                                                  ae_state* _state) {
    ae_frame             _frame_block;
    double               f;
    double               h;
    double               mx;
    ae_int_t             fulln;
    ae_int_t             nfrac;
    ae_int_t             i;
    ae_int_t             j;
    ae_int_t             jj;
    ae_int_t             j0;
    ae_int_t             j1;
    ae_int_t             offs;
    ae_int_t             bestidx;
    double               v;
    mirbfvnstemporaries* buf;
    ae_smart_ptr         _buf;
    ae_bool              updatestats;
    ae_int_t             terminationtype;

    ae_frame_make(_state, &_frame_block);
    memset(&_buf, 0, sizeof(_buf));
    ae_smart_ptr_init(&_buf, (void**) &buf, ae_false, _state, ae_true);

    fulln       = state->n;
    nfrac       = state->nfrac;
    updatestats = state->doanytrace || state->adaptiveinternalparallelism >= 0;
    ae_assert(newnodeidx >= 0 && newnodeidx < grid->nnodes,
              "MIRBFVNS: 087602",
              _state);
    ae_assert(explorefromnode >= 0 && explorefromnode < grid->nnodes,
              "MIRBFVNS: 088602",
              _state);
    ae_shared_pool_retrieve(&sharedstate->tmppool, &_buf, _state);
    stimerinit(&buf->localtimer, _state);
    stimerstartcond(&buf->localtimer, updatestats, _state);
    hqrndseed(rngseedtouse0, rngseedtouse1, &buf->localrng, _state);
    if (state->nfrac == 0) {
      rcopyrr(fulln,
              &grid->nodesinfo,
              newnodeidx,
              &sharedstate->evalbatchpoints,
              evalbatchidx,
              _state);
      weakatomicfetchadd(&sharedstate->explorativetrialcnt, 1, _state);
      ae_shared_pool_recycle(&sharedstate->tmppool, &_buf, _state);
      ae_frame_leave(_state);
      return;
    }
    mirbfvns_gridoffloadbestpoint(grid,
                                  state,
                                  explorefromnode,
                                  &buf->fullx0,
                                  &bestidx,
                                  &f,
                                  &h,
                                  &mx,
                                  _state);
    for (j = 0; j <= fulln - 1; j++) {
      buf->fullx0.ptr.p_double[j]
          = state->maskfrac.ptr.p_double[j] * buf->fullx0.ptr.p_double[j]
            + state->maskint.ptr.p_double[j]
                  * grid->nodesinfo.ptr.pp_double[newnodeidx][j];
    }
    rcopyvr(fulln,
            &buf->fullx0,
            &sharedstate->evalbatchpoints,
            evalbatchidx,
            _state);
    if (state->lccnt == 0) {
      ae_shared_pool_recycle(&sharedstate->tmppool, &_buf, _state);
      ae_frame_leave(_state);
      return;
    }
    isetallocv(fulln, -1, &buf->mapfull2compact, _state);
    rallocv(nfrac, &buf->glbbndl, _state);
    rallocv(nfrac, &buf->glbbndu, _state);
    rallocv(nfrac, &buf->glbs, _state);
    rallocv(nfrac, &buf->glbx0, _state);
    for (i = 0; i <= nfrac - 1; i++) {
      j                                 = state->idxfrac.ptr.p_int[i];
      buf->glbs.ptr.p_double[i]         = state->s.ptr.p_double[j];
      buf->glbx0.ptr.p_double[i]        = buf->fullx0.ptr.p_double[j];
      buf->glbbndl.ptr.p_double[i]      = state->bndl.ptr.p_double[j];
      buf->glbbndu.ptr.p_double[i]      = state->bndu.ptr.p_double[j];
      buf->mapfull2compact.ptr.p_int[j] = i;
    }
    buf->diaga.n = nfrac;
    buf->diaga.m = nfrac;
    iallocv(nfrac + 1, &buf->diaga.ridx, _state);
    iallocv(nfrac, &buf->diaga.idx, _state);
    rallocv(nfrac, &buf->diaga.vals, _state);
    for (i = 0; i <= nfrac - 1; i++) {
      buf->diaga.ridx.ptr.p_int[i] = i;
      buf->diaga.idx.ptr.p_int[i]  = i;
      buf->diaga.vals.ptr.p_double[i]
          = 1.0 / (state->s.ptr.p_double[i] * state->s.ptr.p_double[i]);
    }
    buf->diaga.ridx.ptr.p_int[nfrac] = nfrac;
    sparsecreatecrsinplace(&buf->diaga, _state);
    rsetallocv(nfrac, 0.0, &buf->linb, _state);
    ae_assert(
        sparseiscrs(&state->rawa, _state), "MIRBFVNS: 095629 failed", _state);
    rsetallocv(state->lccnt, _state->v_neginf, &buf->glbal, _state);
    rsetallocv(state->lccnt, _state->v_posinf, &buf->glbau, _state);
    buf->glba.m = state->lccnt;
    buf->glba.n = nfrac;
    iallocv(state->lccnt + 1, &buf->glba.ridx, _state);
    buf->glba.ridx.ptr.p_int[0] = 0;
    for (i = 0; i <= state->lccnt - 1; i++) {
      v    = 0.0;
      offs = buf->glba.ridx.ptr.p_int[i];
      igrowv(offs + nfrac, &buf->glba.idx, _state);
      rgrowv(offs + nfrac, &buf->glba.vals, _state);
      j0 = state->rawa.ridx.ptr.p_int[i];
      j1 = state->rawa.ridx.ptr.p_int[i + 1] - 1;
      for (jj = j0; jj <= j1; jj++) {
        j = state->rawa.idx.ptr.p_int[jj];
        if (!state->isintegral.ptr.p_bool[j]) {
          buf->glba.idx.ptr.p_int[offs]     = buf->mapfull2compact.ptr.p_int[j];
          buf->glba.vals.ptr.p_double[offs] = state->rawa.vals.ptr.p_double[jj];
          offs                              = offs + 1;
        } else {
          v = v
              + buf->fullx0.ptr.p_double[j] * state->rawa.vals.ptr.p_double[jj];
        }
      }
      buf->glba.ridx.ptr.p_int[i + 1] = offs;
      if (ae_isfinite(state->rawal.ptr.p_double[i], _state)) {
        buf->glbal.ptr.p_double[i] = state->rawal.ptr.p_double[i] - v;
      }
      if (ae_isfinite(state->rawau.ptr.p_double[i], _state)) {
        buf->glbau.ptr.p_double[i] = state->rawau.ptr.p_double[i] - v;
      }
    }
    sparsecreatecrsinplace(&buf->glba, _state);
    ipm2init(&buf->qpsubsolver,
             &buf->glbs,
             &buf->glbx0,
             nfrac,
             &state->densedummy2,
             &buf->diaga,
             1,
             ae_false,
             &state->densedummy2,
             &buf->glbtmp0,
             0,
             &buf->linb,
             0.0,
             &buf->glbbndl,
             &buf->glbbndu,
             &buf->glba,
             state->lccnt,
             &state->densedummy2,
             0,
             &buf->glbal,
             &buf->glbau,
             ae_false,
             ae_false,
             _state);
    ipm2setcond(
        &buf->qpsubsolver, state->epsx, state->epsx, state->epsx, _state);
    ipm2setmaxits(&buf->qpsubsolver, mirbfvns_maxipmits, _state);
    ipm2optimize(&buf->qpsubsolver,
                 ae_true,
                 &buf->glbtmp0,
                 &buf->glbtmp1,
                 &buf->glbtmp2,
                 &terminationtype,
                 _state);
    if (terminationtype > 0) {
      for (i = 0; i <= nfrac - 1; i++) {
        buf->fullx0.ptr.p_double[state->idxfrac.ptr.p_int[i]]
            = buf->glbtmp0.ptr.p_double[i];
      }
    }
    rcopyvr(fulln,
            &buf->fullx0,
            &sharedstate->evalbatchpoints,
            evalbatchidx,
            _state);
    stimerstopcond(&buf->localtimer, updatestats, _state);
    if (updatestats) {
      weakatomicfetchadd(&sharedstate->explorativetrialcnt, 1, _state);
      weakatomicfetchadd(&sharedstate->explorativetrialtimems,
                         stimergetmsint(&buf->localtimer, _state),
                         _state);
    }
    ae_shared_pool_recycle(&sharedstate->tmppool, &_buf, _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Having trial point corresponding to node #NodeIdx (it is not checked that
  the point actually belongs to the node) and objective/constraint values
  at that point, send information about trial to the node.

  Processing may involve updating node status (from unexplored to explored or
  solved), but generally does not involve re-running internal subsolver
  *************************************************************************/
  static void
  mirbfvns_gridsendtrialpointto(mirbfvnsgrid*                  grid,
                                mirbfvnsstate*                 state,
                                ae_int_t                       centralnodeidx,
                                ae_int_t                       nodeidx,
                                /* Real    */ const ae_vector* xtrial,
                                /* Real    */ const ae_vector* replyfi,
                                ae_state*                      _state) {
    ae_frame               _frame_block;
    ae_int_t               n;
    ae_int_t               pointidx;
    ae_int_t               st;
    ae_int_t               i;
    double                 h;
    double                 mx;
    double                 v0;
    double                 v1;
    double                 nodefbest;
    double                 nodehbest;
    double                 nodemxbest;
    double                 centralfbest;
    double                 centralhbest;
    mirbfvnsnodesubsolver* subsolver;
    ae_smart_ptr           _subsolver;
    double                 preddeltaf;
    double                 preddeltah;
    ae_bool                acceptablebymarkovfilter;
    ae_bool                sufficientdecreasef;
    ae_bool                sufficientdecreaseh;

    ae_frame_make(_state, &_frame_block);
    memset(&_subsolver, 0, sizeof(_subsolver));
    ae_smart_ptr_init(
        &_subsolver, (void**) &subsolver, ae_false, _state, ae_true);

    n = state->n;
    ae_assert(
        nodeidx >= 0 && nodeidx < grid->nnodes, "MIRBFVNS: 116651", _state);
    if (!isfinitevector(replyfi, 1 + state->nnlc, _state)) {
      st = mirbfvns_gridgetstatus(grid, state, nodeidx, _state);
      if (st == mirbfvns_nodeunexplored) {
        grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolstatus]
            = (double) (mirbfvns_nodebad);
      } else {
        ae_assert(st == mirbfvns_nodeinprogress && state->nfrac > 0,
                  "MIRBFVNS: 175604",
                  _state);
        ae_obj_array_get(&grid->subsolvers, nodeidx, &_subsolver, _state);
        ae_assert(subsolver != NULL, "MIRBFVNS: 412607", _state);
        if (state->doextratrace) {
          ae_trace(
              ">>> infinities detected, decreasing trust radius for node %0d\n",
              (int) (nodeidx));
        }
        subsolver->trustrad = 0.5 * subsolver->trustrad;
        if (ae_fp_less_eq(subsolver->trustrad, state->epsx)) {
          grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolstatus]
              = (double) (mirbfvns_nodesolved);
        }
      }
      ae_frame_leave(_state);
      return;
    }
    mirbfvns_computeviolation2(state, xtrial, replyfi, &h, &mx, _state);
    pointidx = mirbfvns_datasetappendpoint(
        &state->dataset, xtrial, replyfi, h, mx, _state);
    mirbfvns_gridappendpointtolist(grid, pointidx, nodeidx, _state);
    st = mirbfvns_gridgetstatus(grid, state, nodeidx, _state);
    if (st == mirbfvns_nodeunexplored) {
      grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolstatus]
          = (double) (icase2(state->nfrac == 0,
                             mirbfvns_nodesolved,
                             mirbfvns_nodeinprogress,
                             _state));
      grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolfbest]
          = replyfi->ptr.p_double[0];
      grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolhbest]  = h;
      grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolmxbest] = mx;
      grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncollastaccepted]
          = (double) (pointidx);
      if (state->nfrac > 0) {
        mirbfvns_gridinitnilsubsolver(
            grid, state, nodeidx, replyfi->ptr.p_double[0], h, mx, _state);
      }
    } else {
      ae_assert(st == mirbfvns_nodeinprogress && state->nfrac > 0,
                "MIRBFVNS: 117652",
                _state);
      if (mirbfvns_isbetterpoint(
              grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolfbest],
              grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolhbest],
              grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolmxbest],
              replyfi->ptr.p_double[0],
              h,
              mx,
              state->ctol,
              _state)) {
        grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolfbest]
            = replyfi->ptr.p_double[0];
        grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolhbest]  = h;
        grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolmxbest] = mx;
      }
      ae_obj_array_get(&grid->subsolvers, nodeidx, &_subsolver, _state);
      ae_assert(subsolver != NULL, "MIRBFVNS: 412607", _state);
      if (subsolver->sufficientcloudsize) {
        preddeltaf = subsolver->predf - subsolver->basef;
        preddeltah = subsolver->predh - subsolver->baseh;
        sufficientdecreasef
            = ae_fp_less(preddeltaf, (double) (0))
              && ae_fp_less(replyfi->ptr.p_double[0],
                            subsolver->basef + mirbfvns_eta2 * preddeltaf);
        sufficientdecreaseh
            = ae_fp_less(preddeltah, (double) (0))
              && ae_fp_less(h, subsolver->baseh + mirbfvns_eta2 * preddeltah);
        acceptablebymarkovfilter = ae_fp_less_eq(h, subsolver->maxh);
        acceptablebymarkovfilter
            = acceptablebymarkovfilter
              && (ae_fp_less(replyfi->ptr.p_double[0], subsolver->basef)
                  || ae_fp_less(h, subsolver->baseh));
        acceptablebymarkovfilter
            = acceptablebymarkovfilter
              && (sufficientdecreasef || sufficientdecreaseh);
        if (acceptablebymarkovfilter) {
          if (ae_fp_greater(subsolver->skrellen, mirbfvns_rbfsktooshort)) {
            if (ae_fp_less(subsolver->predf, subsolver->basef)) {
              if (ae_fp_greater((subsolver->basef - replyfi->ptr.p_double[0])
                                    / (subsolver->basef - subsolver->predf),
                                mirbfvns_eta2)) {
                subsolver->trustrad = ae_minreal(
                    mirbfvns_gammainc * subsolver->trustrad,
                    mirbfvns_gammainc2
                        * (subsolver->skrellen * subsolver->trustrad),
                    _state);
              } else {
                subsolver->trustrad
                    = ae_maxreal(mirbfvns_gammadec,
                                 mirbfvns_gammadec2 * subsolver->skrellen,
                                 _state)
                      * subsolver->trustrad;
              }
              if (state->doextratrace) {
                ae_trace("[%6d] >>> acceptable, predicted f-step, "
                         "predDeltaF=%0.2e  ratio=%0.2e    predDeltaH=%0.2e  "
                         "ratio=%0.2e, trustRad:=%0.2e\n",
                         (int) (nodeidx),
                         (double) (-(subsolver->basef - subsolver->predf)),
                         (double) ((subsolver->basef - replyfi->ptr.p_double[0])
                                   / (subsolver->basef - subsolver->predf)),
                         (double) (-(subsolver->baseh - subsolver->predh)),
                         (double) ((subsolver->baseh - h)
                                   / (subsolver->baseh - subsolver->predh)),
                         (double) (subsolver->trustrad));
              }
            } else {
              if (ae_fp_less(subsolver->predh, subsolver->baseh)
                  && ae_fp_greater((subsolver->baseh - h)
                                       / (subsolver->baseh - subsolver->predh),
                                   mirbfvns_eta2)) {
                subsolver->trustrad = ae_minreal(
                    mirbfvns_gammainc * subsolver->trustrad,
                    mirbfvns_gammainc2
                        * (subsolver->skrellen * subsolver->trustrad),
                    _state);
              } else {
                subsolver->trustrad
                    = ae_maxreal(mirbfvns_gammadec,
                                 mirbfvns_gammadec2 * subsolver->skrellen,
                                 _state)
                      * subsolver->trustrad;
              }
              if (state->doextratrace) {
                ae_trace("[%6d] >>> acceptable, predicted h-step, "
                         "predDeltaF=%0.2e  ratio=%0.2e    predDeltaH=%0.2e  "
                         "ratio=%0.2e, trustRad:=%0.2e\n",
                         (int) (nodeidx),
                         (double) (-(subsolver->basef - subsolver->predf)),
                         (double) ((subsolver->basef - replyfi->ptr.p_double[0])
                                   / (subsolver->basef - subsolver->predf)),
                         (double) (-(subsolver->baseh - subsolver->predh)),
                         (double) ((subsolver->baseh - h)
                                   / (subsolver->baseh - subsolver->predh)),
                         (double) (subsolver->trustrad));
              }
            }
          } else {
            subsolver->trustrad
                = ae_minreal((double) 10 * mirbfvns_rbfsktooshort, 0.1, _state)
                  * subsolver->trustrad;
            if (state->doextratrace) {
              ae_trace("[%6d] >>> acceptable, Sk is too short, decreasing "
                       "trustRad:=%0.2e\n",
                       (int) (nodeidx),
                       (double) (subsolver->trustrad));
            }
          }
          grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncollastaccepted]
              = (double) (pointidx);
          for (i = 0; i <= subsolver->historymax - 2; i++) {
            subsolver->successfhistory.ptr.p_double[i]
                = subsolver->successfhistory.ptr.p_double[i + 1];
          }
          subsolver->successfhistory.ptr.p_double[subsolver->historymax - 1]
              = ae_fabs(replyfi->ptr.p_double[0] - subsolver->basef, _state);
          for (i = 0; i <= subsolver->historymax - 2; i++) {
            subsolver->successhhistory.ptr.p_double[i]
                = subsolver->successhhistory.ptr.p_double[i + 1];
          }
          subsolver->successhhistory.ptr.p_double[subsolver->historymax - 1]
              = ae_fabs(h - subsolver->baseh, _state);
        } else {
          if (state->doextratrace) {
            ae_trace("[%6d] >>> unacceptable, DeltaF=%0.2e, DeltaH=%0.2e\n",
                     (int) (nodeidx),
                     (double) (-(subsolver->basef - replyfi->ptr.p_double[0])),
                     (double) (-(subsolver->baseh - h)));
          }
          subsolver->trustrad
              = ae_maxreal(mirbfvns_gammadec3,
                           mirbfvns_gammadec2 * subsolver->skrellen,
                           _state)
                * subsolver->trustrad;
        }
      }
      centralfbest = mirbfvns_gridgetfbest(grid, state, centralnodeidx, _state);
      centralhbest = mirbfvns_gridgethbest(grid, state, centralnodeidx, _state);
      nodefbest    = mirbfvns_gridgetfbest(grid, state, nodeidx, _state);
      nodehbest    = mirbfvns_gridgethbest(grid, state, nodeidx, _state);
      nodemxbest   = mirbfvns_gridgetmxbest(grid, state, nodeidx, _state);
      v0           = (double) (0);
      v1           = (double) (0);
      for (i = 0; i <= subsolver->historymax - 1; i++) {
        v0 = v0 + subsolver->successfhistory.ptr.p_double[i];
        v1 = v1 + subsolver->successhhistory.ptr.p_double[i];
      }
      if (ae_fp_less_eq(subsolver->trustrad, state->epsx)) {
        if (state->doextratrace) {
          ae_trace("[%6d] >>> STOP: trust radius is small (%0.2e), marking as "
                   "solved (fbest=%0.6e,maxv=%0.6e)\n",
                   (int) (nodeidx),
                   (double) (subsolver->trustrad),
                   (double) (nodefbest),
                   (double) (nodemxbest));
        }
        grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolstatus]
            = (double) (mirbfvns_nodesolved);
      }
      if (ae_fp_less_eq(nodemxbest, state->ctol)) {
        if (nodeidx == centralnodeidx) {
          if (ae_fp_less(ae_fabs(v0, _state) * mirbfvns_habovezero,
                         state->epsf * rmaxabs2(nodefbest, 1.0, _state))) {
            if (state->doextratrace) {
              ae_trace(
                  "[%6d] >>> STOP: central node feasible, objective change is "
                  "small, marking as solved (fbest=%0.6e,maxv=%0.6e)\n",
                  (int) (nodeidx),
                  (double) (nodefbest),
                  (double) (nodemxbest));
            }
            grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolstatus]
                = (double) (mirbfvns_nodesolved);
          }
        } else {
          if (ae_fp_greater(nodefbest
                                - ae_fabs(v0, _state) * mirbfvns_habovezero,
                            centralfbest)
              && ae_fp_less(
                  ae_fabs(v0, _state) * mirbfvns_habovezero,
                  ae_maxreal(state->quickepsf, (double) 2 * state->epsf, _state)
                      * rmaxabs2(nodefbest, 1.0, _state))) {
            if (state->doextratrace) {
              ae_trace("[%6d] >>> STOP: neighbor node feasible and worse than "
                       "central one, objective change is small, marking as "
                       "solved (fbest=%0.6e,maxv=%0.6e)\n",
                       (int) (nodeidx),
                       (double) (nodefbest),
                       (double) (nodemxbest));
            }
            grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolstatus]
                = (double) (mirbfvns_nodesolved);
          }
        }
      } else {
        if (nodeidx == centralnodeidx) {
          if (ae_fp_greater(nodehbest
                                - ae_fabs(v1, _state) * mirbfvns_habovezero,
                            (double) (0))
              && ae_fp_less(ae_fabs(v1, _state) * mirbfvns_habovezero,
                            state->epsf * rmaxabs2(nodehbest, 1.0, _state))) {
            if (state->doextratrace) {
              ae_trace("[%6d] >>> STOP: central node infeasible, constraint "
                       "violation converged, marking as solved "
                       "(fbest=%0.6e,maxv=%0.6e)\n",
                       (int) (nodeidx),
                       (double) (nodefbest),
                       (double) (nodemxbest));
            }
            grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolstatus]
                = (double) (mirbfvns_nodesolved);
          }
        } else {
          if (ae_fp_greater(nodehbest
                                - ae_fabs(v1, _state) * mirbfvns_habovezero,
                            centralhbest)
              && ae_fp_less(
                  ae_fabs(v1, _state) * mirbfvns_habovezero,
                  ae_maxreal(state->quickepsf, (double) 2 * state->epsf, _state)
                      * rmaxabs2(nodehbest, 1.0, _state))) {
            if (state->doextratrace) {
              ae_trace("[%6d] >>> STOP: neighbor node infeasible and worse "
                       "than central one, constraint violation converged, "
                       "marking as solved (fbest=%0.6e,maxv=%0.6e)\n",
                       (int) (nodeidx),
                       (double) (nodefbest),
                       (double) (nodemxbest));
            }
            grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolstatus]
                = (double) (mirbfvns_nodesolved);
          }
        }
      }
    }
    ae_frame_leave(_state);
  }

  /*************************************************************************
  Offloads best point for the grid node
  *************************************************************************/
  static void mirbfvns_gridoffloadbestpoint(const mirbfvnsgrid*      grid,
                                            const mirbfvnsstate*     state,
                                            ae_int_t                 nodeidx,
                                            /* Real    */ ae_vector* x,
                                            ae_int_t*                pointidx,
                                            double*                  f,
                                            double*                  h,
                                            double*                  mx,
                                            ae_state*                _state) {
    ae_int_t n;
    ae_int_t st;
    ae_int_t candidx;
    ae_bool  firstpoint;
    ae_int_t nextlistpos;
    double   f1;
    double   h1;
    double   mx1;

    *pointidx = 0;
    *f        = 0.0;
    *h        = 0.0;
    *mx       = 0.0;

    n = state->n;
    ae_assert(
        nodeidx >= 0 && nodeidx < grid->nnodes, "MIRBFVNS: 116651", _state);
    st = ae_round(
        grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolstatus],
        _state);
    ae_assert(st == mirbfvns_nodesolved || st == mirbfvns_nodeinprogress,
              "MIRBFVNS: 171713",
              _state);
    ae_assert(grid->ptlistheads.ptr.p_int[2 * nodeidx + 0] >= 0
                  && grid->ptlistheads.ptr.p_int[2 * nodeidx + 1] > 0,
              "MIRBFVNS: 171714",
              _state);
    rallocv(n, x, _state);
    *f          = ae_maxrealnumber;
    *h          = ae_maxrealnumber;
    *mx         = ae_maxrealnumber;
    firstpoint  = ae_true;
    nextlistpos = grid->ptlistheads.ptr.p_int[2 * nodeidx + 0];
    *pointidx   = -1;
    while (nextlistpos >= 0) {
      candidx     = grid->ptlistdata.ptr.p_int[2 * nextlistpos + 0];
      nextlistpos = grid->ptlistdata.ptr.p_int[2 * nextlistpos + 1];
      f1          = state->dataset.pointinfo.ptr.pp_double[candidx][n];
      h1          = state->dataset.pointinfo.ptr
               .pp_double[candidx][n + 1 + state->nnlc + 0];
      mx1 = state->dataset.pointinfo.ptr
                .pp_double[candidx][n + 1 + state->nnlc + 1];
      if (firstpoint
          || mirbfvns_isbetterpoint(
              *f, *h, *mx, f1, h1, mx1, state->ctol, _state)) {
        rcopyrv(n, &state->dataset.pointinfo, candidx, x, _state);
        *pointidx = candidx;
        *f        = f1;
        *h        = h1;
        *mx       = mx1;
      }
      firstpoint = ae_false;
    }
  }

  /*************************************************************************
  Compares two points, with (objective,sumviolation,maxviolation)=(F,H,MX),
  and returns true if the second one is better. CTol is used to differentiate
  between feasible and infeasible points.

  The base can not be in 'bad' or 'unexplored' state
  *************************************************************************/
  static ae_bool mirbfvns_isbetterpoint(double    f0,
                                        double    h0,
                                        double    mx0,
                                        double    f1,
                                        double    h1,
                                        double    mx1,
                                        double    ctol,
                                        ae_state* _state) {
    ae_bool result;

    result = ae_false;
    if (mx0 <= ctol && mx1 <= ctol) {
      result = ae_fp_less(f1, f0);
    }
    if (mx0 <= ctol && mx1 > ctol) {
      result = ae_false;
    }
    if (mx0 > ctol && mx1 <= ctol) {
      result = ae_true;
    }
    if (mx0 > ctol && mx1 > ctol) {
      result = ae_fp_less(h1, h0 - ctol);
    }
    return result;
  }

  /*************************************************************************
  Compares two grid nodes, returns if the second one is better.

  The base can not be in 'bad' or 'unexplored' state
  *************************************************************************/
  static ae_bool mirbfvns_gridisbetter(mirbfvnsgrid*  grid,
                                       mirbfvnsstate* state,
                                       ae_int_t       baseidx,
                                       ae_int_t       candidx,
                                       ae_state*      _state) {
    ae_int_t n;
    ae_int_t st0;
    ae_int_t st1;
    double   f0;
    double   f1;
    double   h0;
    double   h1;
    double   mx0;
    double   mx1;
    ae_bool  result;

    n      = state->n;
    result = ae_false;
    ae_assert(((baseidx >= 0 && baseidx < grid->nnodes) && candidx >= 0)
                  && candidx < grid->nnodes,
              "MIRBFVNS: 204716",
              _state);
    st0 = ae_round(
        grid->nodesinfo.ptr.pp_double[baseidx][n + mirbfvns_ncolstatus],
        _state);
    st1 = ae_round(
        grid->nodesinfo.ptr.pp_double[candidx][n + mirbfvns_ncolstatus],
        _state);
    ae_assert(
        (((st0 == mirbfvns_nodeinprogress || st0 == mirbfvns_nodesolved)
          || st0 == mirbfvns_nodebad)
         || st0 == mirbfvns_nodeunexplored)
            && (((st1 == mirbfvns_nodeinprogress || st1 == mirbfvns_nodesolved)
                 || st1 == mirbfvns_nodebad)
                || st1 == mirbfvns_nodeunexplored),
        "MIRBFVNS: 209730",
        _state);
    if (st0 == mirbfvns_nodebad || st0 == mirbfvns_nodeunexplored) {
      result = st1 != mirbfvns_nodebad && st1 != mirbfvns_nodeunexplored;
      return result;
    }
    if (st1 == mirbfvns_nodebad || st1 == mirbfvns_nodeunexplored) {
      return result;
    }
    f0  = grid->nodesinfo.ptr.pp_double[baseidx][n + mirbfvns_ncolfbest];
    f1  = grid->nodesinfo.ptr.pp_double[candidx][n + mirbfvns_ncolfbest];
    h0  = grid->nodesinfo.ptr.pp_double[baseidx][n + mirbfvns_ncolhbest];
    h1  = grid->nodesinfo.ptr.pp_double[candidx][n + mirbfvns_ncolhbest];
    mx0 = grid->nodesinfo.ptr.pp_double[baseidx][n + mirbfvns_ncolmxbest];
    mx1 = grid->nodesinfo.ptr.pp_double[candidx][n + mirbfvns_ncolmxbest];
    result
        = mirbfvns_isbetterpoint(f0, h0, mx0, f1, h1, mx1, state->ctol, _state);
    return result;
  }

  /*************************************************************************
  Returns best objective for a node
  *************************************************************************/
  static double mirbfvns_gridgetpointscountinnode(mirbfvnsgrid*  grid,
                                                  mirbfvnsstate* state,
                                                  ae_int_t       nodeidx,
                                                  ae_state*      _state) {
    double result;

    ae_assert(
        nodeidx >= 0 && nodeidx < grid->nnodes, "MIRBFVNS: 271747", _state);
    result = (double) (grid->ptlistheads.ptr.p_int[2 * nodeidx + 1]);
    return result;
  }

  /*************************************************************************
  Returns best objective for a node
  *************************************************************************/
  static double mirbfvns_gridgetfbest(mirbfvnsgrid*  grid,
                                      mirbfvnsstate* state,
                                      ae_int_t       nodeidx,
                                      ae_state*      _state) {
    ae_int_t n;
    double   result;

    n = state->n;
    ae_assert(
        nodeidx >= 0 && nodeidx < grid->nnodes, "MIRBFVNS: 271747", _state);
    result = grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolfbest];
    return result;
  }

  /*************************************************************************
  Returns best sum of violations for a node
  *************************************************************************/
  static double mirbfvns_gridgethbest(mirbfvnsgrid*  grid,
                                      mirbfvnsstate* state,
                                      ae_int_t       nodeidx,
                                      ae_state*      _state) {
    ae_int_t n;
    double   result;

    n = state->n;
    ae_assert(
        nodeidx >= 0 && nodeidx < grid->nnodes, "MIRBFVNS: 271748", _state);
    result = grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolhbest];
    return result;
  }

  /*************************************************************************
  Returns best max of violations for a node
  *************************************************************************/
  static double mirbfvns_gridgetmxbest(mirbfvnsgrid*  grid,
                                       mirbfvnsstate* state,
                                       ae_int_t       nodeidx,
                                       ae_state*      _state) {
    ae_int_t n;
    double   result;

    n = state->n;
    ae_assert(
        nodeidx >= 0 && nodeidx < grid->nnodes, "MIRBFVNS: 031236", _state);
    result = grid->nodesinfo.ptr.pp_double[nodeidx][n + mirbfvns_ncolmxbest];
    return result;
  }

  /*************************************************************************
  This function performs minimization of the RBF model of objective/constraints
  and returns minimum as well as predicted values at the minimum

    -- ALGLIB --
       Copyright 15.10.2024 by Bochkanov Sergey
  *************************************************************************/
  static void
  mirbfvns_rbfminimizemodel(const mirbfmodel*              model,
                            /* Real    */ const ae_vector* x0,
                            /* Real    */ const ae_vector* bndl,
                            /* Real    */ const ae_vector* bndu,
                            /* Real    */ const ae_vector* trustregion,
                            double                         trustradfactor,
                            double                         ctol,
                            ae_int_t                       maxitsperphase,
                            ae_bool                        autoscalemodel,
                            const sparsematrix*            c,
                            /* Real    */ const ae_vector* cl,
                            /* Real    */ const ae_vector* cu,
                            ae_int_t                       lccnt,
                            /* Real    */ const ae_vector* nl,
                            /* Real    */ const ae_vector* nu,
                            ae_int_t                       nnlc,
                            ae_int_t                       n,
                            rbfmmtemporaries*              buf,
                            /* Real    */ ae_vector*       xn,
                            /* Real    */ ae_vector*       sk,
                            double*                        predf,
                            double*                        predh,
                            ae_int_t*                      subsolverits,
                            ae_state*                      _state) {
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t nnz;
    ae_int_t nx;
    double   multiplyby;
    double   v;
    double   vax;
    double   predsum;
    ae_bool  usedensebfgs;

    *predf        = 0.0;
    *predh        = 0.0;
    *subsolverits = 0;

    nx         = n + 2 * lccnt + 2 * nnlc;
    multiplyby = 1.0;
    critinitdefault(&buf->crit, _state);
    critsetcondv1(
        &buf->crit, 0.0, mirbfvns_rbfsubsolverepsx, maxitsperphase, _state);
    *subsolverits = 0;
    rallocv(n, xn, _state);
    rallocv(n, sk, _state);
    usedensebfgs = ae_false;
    rallocv(nx, &buf->bndlx, _state);
    rallocv(nx, &buf->bndux, _state);
    rallocv(nx, &buf->x0x, _state);
    rallocv(nx, &buf->sx, _state);
    for (i = 0; i <= n - 1; i++) {
      buf->bndlx.ptr.p_double[i]
          = x0->ptr.p_double[i] - trustregion->ptr.p_double[i];
      buf->bndux.ptr.p_double[i]
          = x0->ptr.p_double[i] + trustregion->ptr.p_double[i];
      if (ae_isfinite(bndl->ptr.p_double[i], _state)
          && ae_fp_greater(bndl->ptr.p_double[i], buf->bndlx.ptr.p_double[i])) {
        buf->bndlx.ptr.p_double[i] = bndl->ptr.p_double[i];
      }
      if (ae_isfinite(bndu->ptr.p_double[i], _state)
          && ae_fp_less(bndu->ptr.p_double[i], buf->bndux.ptr.p_double[i])) {
        buf->bndux.ptr.p_double[i] = bndu->ptr.p_double[i];
      }
      buf->x0x.ptr.p_double[i] = x0->ptr.p_double[i];
      buf->sx.ptr.p_double[i]  = trustregion->ptr.p_double[i];
    }
    for (i = n; i <= nx - 1; i++) {
      buf->bndlx.ptr.p_double[i] = 0.0;
      buf->bndux.ptr.p_double[i] = _state->v_posinf;
      buf->sx.ptr.p_double[i]    = trustradfactor * multiplyby;
      buf->x0x.ptr.p_double[i]   = 0.0;
    }
    rallocv(lccnt + 1, &buf->clx, _state);
    rallocv(lccnt + 1, &buf->cux, _state);
    sparsecreatecrsemptybuf(nx, &buf->cx, _state);
    iallocv(nx, &buf->tmpi, _state);
    rallocv(nx, &buf->tmp0, _state);
    for (i = 0; i <= lccnt - 1; i++) {
      nnz = 0;
      vax = (double) (0);
      for (jj = c->ridx.ptr.p_int[i]; jj <= c->ridx.ptr.p_int[i + 1] - 1;
           jj++) {
        j                           = c->idx.ptr.p_int[jj];
        v                           = c->vals.ptr.p_double[jj];
        buf->tmpi.ptr.p_int[nnz]    = j;
        buf->tmp0.ptr.p_double[nnz] = v;
        vax                         = vax + v * x0->ptr.p_double[j];
        nnz                         = nnz + 1;
      }
      buf->clx.ptr.p_double[i] = cl->ptr.p_double[i];
      if (ae_isfinite(cl->ptr.p_double[i], _state)) {
        buf->tmpi.ptr.p_int[nnz]    = n + 2 * i + 0;
        buf->tmp0.ptr.p_double[nnz] = 1.0;
        buf->x0x.ptr.p_double[n + 2 * i + 0]
            = ae_maxreal(cl->ptr.p_double[i] - vax, 0.0, _state);
        nnz = nnz + 1;
      }
      buf->cux.ptr.p_double[i] = cu->ptr.p_double[i];
      if (ae_isfinite(cu->ptr.p_double[i], _state)) {
        buf->tmpi.ptr.p_int[nnz]    = n + 2 * i + 1;
        buf->tmp0.ptr.p_double[nnz] = -1.0;
        buf->x0x.ptr.p_double[n + 2 * i + 1]
            = ae_maxreal(vax - cu->ptr.p_double[i], 0.0, _state);
        nnz = nnz + 1;
      }
      ae_assert(nnz <= nx, "RBF4OPT: integrity check 800519 failed", _state);
      sparseappendcompressedrow(&buf->cx, &buf->tmpi, &buf->tmp0, nnz, _state);
    }
    iallocv(lccnt, &buf->tmpi, _state);
    for (i = 0; i <= lccnt - 1; i++) {
      buf->tmpi.ptr.p_int[i] = i;
    }
    rallocv(1 + nnlc, &buf->tmp0, _state);
    rallocv((1 + nnlc) * n, &buf->tmp1, _state);
    mirbfvns_rbfcomputemodel(
        model, &buf->x0x, &buf->tmp0, ae_true, &buf->tmp1, ae_true, _state);
    rsetallocv(1 + nnlc, 1.0, &buf->scalingfactors, _state);
    if (autoscalemodel) {
      for (i = 0; i <= nnlc; i++) {
        v = 0.0;
        for (j = 0; j <= n - 1; j++) {
          v = ae_maxreal(
              v, ae_fabs(buf->tmp1.ptr.p_double[i * n + j], _state), _state);
        }
        buf->scalingfactors.ptr.p_double[i]
            = (double) 1 / ae_maxreal(v, (double) (1), _state);
      }
    }
    rcopyallocv(nnlc, nl, &buf->tmpnl, _state);
    rcopyallocv(nnlc, nu, &buf->tmpnu, _state);
    for (i = 0; i <= nnlc - 1; i++) {
      if (ae_isfinite(nl->ptr.p_double[i], _state)) {
        buf->tmpnl.ptr.p_double[i] = buf->tmpnl.ptr.p_double[i]
                                     * buf->scalingfactors.ptr.p_double[i + 1];
        buf->x0x.ptr.p_double[n + 2 * lccnt + 2 * i + 0]
            = ae_maxreal(buf->tmpnl.ptr.p_double[i]
                             - buf->scalingfactors.ptr.p_double[i + 1]
                                   * buf->tmp0.ptr.p_double[1 + i],
                         0.0,
                         _state);
      }
      if (ae_isfinite(nu->ptr.p_double[i], _state)) {
        buf->tmpnu.ptr.p_double[i] = buf->tmpnu.ptr.p_double[i]
                                     * buf->scalingfactors.ptr.p_double[i + 1];
        buf->x0x.ptr.p_double[n + 2 * lccnt + 2 * i + 1]
            = ae_maxreal(buf->scalingfactors.ptr.p_double[i + 1]
                                 * buf->tmp0.ptr.p_double[1 + i]
                             - buf->tmpnu.ptr.p_double[i],
                         0.0,
                         _state);
      }
    }
    predsum = (double) (0);
    if (lccnt + nnlc > 0) {
      minfsqpinitbuf(&buf->bndlx,
                     &buf->bndux,
                     &buf->sx,
                     &buf->x0x,
                     nx,
                     &buf->cx,
                     &buf->clx,
                     &buf->cux,
                     &buf->tmpi,
                     lccnt,
                     &buf->tmpnl,
                     &buf->tmpnu,
                     nnlc,
                     &buf->crit,
                     usedensebfgs,
                     &buf->fsqpsolver,
                     _state);
      minfsqpsetinittrustrad(&buf->fsqpsolver, 1.0, _state);
      smoothnessmonitorinit(
          &buf->smonitor, &buf->sx, nx, 1 + nnlc, ae_false, _state);
      while (minfsqpiteration(
          &buf->fsqpsolver, &buf->smonitor, ae_false, _state)) {
        if (buf->fsqpsolver.xupdated) {
          continue;
        }
        if (buf->fsqpsolver.needfisj) {
          rallocv(nx, &buf->tmp0, _state);
          rallocv((1 + nnlc) * n, &buf->tmp1, _state);
          rcopyv(nx, &buf->fsqpsolver.x, &buf->tmp0, _state);
          rmergemulv(nx, &buf->sx, &buf->tmp0, _state);
          rmergemaxv(nx, &buf->bndlx, &buf->tmp0, _state);
          rmergeminv(nx, &buf->bndux, &buf->tmp0, _state);
          mirbfvns_rbfcomputemodel(model,
                                   &buf->tmp0,
                                   &buf->fsqpsolver.fi,
                                   ae_true,
                                   &buf->tmp1,
                                   ae_true,
                                   _state);
          rsetallocv((1 + nnlc) * nx, 0.0, &buf->tmp2, _state);
          buf->fsqpsolver.fi.ptr.p_double[0] = (double) (0);
          for (j = n; j <= nx - 1; j++) {
            buf->fsqpsolver.fi.ptr.p_double[0]
                = buf->fsqpsolver.fi.ptr.p_double[0]
                  + buf->tmp0.ptr.p_double[j];
            buf->tmp2.ptr.p_double[j] = 1.0;
          }
          for (i = 1; i <= nnlc; i++) {
            buf->fsqpsolver.fi.ptr.p_double[i]
                = buf->fsqpsolver.fi.ptr.p_double[i]
                  * buf->scalingfactors.ptr.p_double[i];
            rcopyvx(n, &buf->tmp1, i * n, &buf->tmp2, i * nx, _state);
            rmulvx(n,
                   buf->scalingfactors.ptr.p_double[i],
                   &buf->tmp2,
                   i * nx,
                   _state);
            if (ae_isfinite(nl->ptr.p_double[i - 1], _state)) {
              j = n + 2 * lccnt + 2 * (i - 1) + 0;
              buf->fsqpsolver.fi.ptr.p_double[i]
                  = buf->fsqpsolver.fi.ptr.p_double[i]
                    + buf->tmp0.ptr.p_double[j];
              buf->tmp2.ptr.p_double[i * nx + j] = 1.0;
            }
            if (ae_isfinite(nu->ptr.p_double[i - 1], _state)) {
              j = n + 2 * lccnt + 2 * (i - 1) + 1;
              buf->fsqpsolver.fi.ptr.p_double[i]
                  = buf->fsqpsolver.fi.ptr.p_double[i]
                    - buf->tmp0.ptr.p_double[j];
              buf->tmp2.ptr.p_double[i * nx + j] = -1.0;
            }
          }
          sparsecreatecrsfromdensev(
              &buf->tmp2, 1 + nnlc, nx, &buf->fsqpsolver.sj, _state);
          sparsemultiplycolsby(&buf->fsqpsolver.sj, &buf->sx, _state);
          continue;
        }
        ae_assert(ae_false, "RBF4OPT: integrity check 858514 failed", _state);
      }
      ae_assert(buf->fsqpsolver.repterminationtype > 0,
                "RBF4OPT: integrity check 860514 failed",
                _state);
      *subsolverits = *subsolverits + buf->fsqpsolver.repiterationscount;
      rcopyv(nx, &buf->fsqpsolver.stepk.x, &buf->x0x, _state);
      rmergemulv(nx, &buf->sx, &buf->x0x, _state);
      rmergemaxv(nx, &buf->bndlx, &buf->x0x, _state);
      rmergeminv(nx, &buf->bndux, &buf->x0x, _state);
      predsum = (double) (0);
      if (lccnt > 0) {
        sparsemv(c, &buf->x0x, &buf->tmp0, _state);
        for (i = 0; i <= lccnt - 1; i++) {
          if (ae_isfinite(cl->ptr.p_double[i], _state)) {
            predsum
                = predsum
                  + ae_maxreal(cl->ptr.p_double[i] - buf->tmp0.ptr.p_double[i],
                               0.0,
                               _state);
          }
          if (ae_isfinite(cu->ptr.p_double[i], _state)) {
            predsum
                = predsum
                  + ae_maxreal(buf->tmp0.ptr.p_double[i] - cu->ptr.p_double[i],
                               0.0,
                               _state);
          }
        }
      }
      rallocv(1 + nnlc, &buf->tmp1, _state);
      mirbfvns_rbfcomputemodel(
          model, &buf->x0x, &buf->tmp1, ae_true, &buf->tmp0, ae_false, _state);
      for (i = 0; i <= nnlc - 1; i++) {
        if (ae_isfinite(nl->ptr.p_double[i], _state)) {
          predsum = predsum
                    + ae_maxreal(
                        buf->tmpnl.ptr.p_double[i]
                            - buf->tmp1.ptr.p_double[1 + i]
                                  * buf->scalingfactors.ptr.p_double[1 + i],
                        0.0,
                        _state);
        }
        if (ae_isfinite(nu->ptr.p_double[i], _state)) {
          predsum
              = predsum
                + ae_maxreal(buf->tmp1.ptr.p_double[1 + i]
                                     * buf->scalingfactors.ptr.p_double[1 + i]
                                 - buf->tmpnu.ptr.p_double[i],
                             0.0,
                             _state);
        }
      }
    }
    iallocv(nx, &buf->tmpi, _state);
    rallocv(nx, &buf->tmp0, _state);
    nnz = 0;
    for (i = n; i <= nx - 1; i++) {
      buf->tmpi.ptr.p_int[nnz]    = i;
      buf->tmp0.ptr.p_double[nnz] = 1.0;
      nnz                         = nnz + 1;
    }
    ae_assert(buf->clx.cnt >= lccnt + 1 && buf->cux.cnt >= lccnt + 1,
              "RBF4OPT: integrity check 889517 failed",
              _state);
    sparseappendcompressedrow(&buf->cx, &buf->tmpi, &buf->tmp0, nnz, _state);
    buf->clx.ptr.p_double[lccnt] = _state->v_neginf;
    buf->cux.ptr.p_double[lccnt] = ae_maxreal(predsum, 0.1 * ctol, _state);
    iallocv(lccnt + 1, &buf->tmpi, _state);
    for (i = 0; i <= lccnt; i++) {
      buf->tmpi.ptr.p_int[i] = i;
    }
    minfsqpinitbuf(&buf->bndlx,
                   &buf->bndux,
                   &buf->sx,
                   &buf->x0x,
                   nx,
                   &buf->cx,
                   &buf->clx,
                   &buf->cux,
                   &buf->tmpi,
                   lccnt + 1,
                   &buf->tmpnl,
                   &buf->tmpnu,
                   nnlc,
                   &buf->crit,
                   usedensebfgs,
                   &buf->fsqpsolver,
                   _state);
    minfsqpsetinittrustrad(&buf->fsqpsolver, 1.0, _state);
    smoothnessmonitorinit(
        &buf->smonitor, &buf->sx, nx, 1 + nnlc, ae_false, _state);
    while (
        minfsqpiteration(&buf->fsqpsolver, &buf->smonitor, ae_false, _state)) {
      if (buf->fsqpsolver.xupdated) {
        continue;
      }
      if (buf->fsqpsolver.needfisj) {
        rallocv(nx, &buf->tmp0, _state);
        rallocv((1 + nnlc) * n, &buf->tmp1, _state);
        rcopyv(nx, &buf->fsqpsolver.x, &buf->tmp0, _state);
        rmergemulv(nx, &buf->sx, &buf->tmp0, _state);
        rmergemaxv(nx, &buf->bndlx, &buf->tmp0, _state);
        rmergeminv(nx, &buf->bndux, &buf->tmp0, _state);
        mirbfvns_rbfcomputemodel(model,
                                 &buf->tmp0,
                                 &buf->fsqpsolver.fi,
                                 ae_true,
                                 &buf->tmp1,
                                 ae_true,
                                 _state);
        rsetallocv((1 + nnlc) * nx, 0.0, &buf->tmp2, _state);
        buf->fsqpsolver.fi.ptr.p_double[0]
            = buf->fsqpsolver.fi.ptr.p_double[0]
              * buf->scalingfactors.ptr.p_double[0];
        rcopyvx(n, &buf->tmp1, 0, &buf->tmp2, 0, _state);
        rmulvx(n, buf->scalingfactors.ptr.p_double[0], &buf->tmp2, 0, _state);
        for (i = 1; i <= nnlc; i++) {
          buf->fsqpsolver.fi.ptr.p_double[i]
              = buf->fsqpsolver.fi.ptr.p_double[i]
                * buf->scalingfactors.ptr.p_double[i];
          rcopyvx(n, &buf->tmp1, i * n, &buf->tmp2, i * nx, _state);
          rmulvx(n,
                 buf->scalingfactors.ptr.p_double[i],
                 &buf->tmp2,
                 i * nx,
                 _state);
          if (ae_isfinite(nl->ptr.p_double[i - 1], _state)) {
            j = n + 2 * lccnt + 2 * (i - 1) + 0;
            buf->fsqpsolver.fi.ptr.p_double[i]
                = buf->fsqpsolver.fi.ptr.p_double[i]
                  + buf->tmp0.ptr.p_double[j];
            buf->tmp2.ptr.p_double[i * nx + j] = 1.0;
          }
          if (ae_isfinite(nu->ptr.p_double[i - 1], _state)) {
            j = n + 2 * lccnt + 2 * (i - 1) + 1;
            buf->fsqpsolver.fi.ptr.p_double[i]
                = buf->fsqpsolver.fi.ptr.p_double[i]
                  - buf->tmp0.ptr.p_double[j];
            buf->tmp2.ptr.p_double[i * nx + j] = -1.0;
          }
        }
        sparsecreatecrsfromdensev(
            &buf->tmp2, 1 + nnlc, nx, &buf->fsqpsolver.sj, _state);
        sparsemultiplycolsby(&buf->fsqpsolver.sj, &buf->sx, _state);
        continue;
      }
      ae_assert(ae_false, "DFGM: integrity check 261738 failed", _state);
    }
    *subsolverits = *subsolverits + buf->fsqpsolver.repiterationscount;
    rcopyv(n, &buf->fsqpsolver.stepk.x, &buf->x0x, _state);
    rmergemulv(nx, &buf->sx, &buf->x0x, _state);
    rmergemaxv(nx, &buf->bndlx, &buf->x0x, _state);
    rmergeminv(nx, &buf->bndux, &buf->x0x, _state);
    rcopyv(n, &buf->x0x, xn, _state);
    rallocv(1 + nnlc, &buf->tmp1, _state);
    rcopyv(n, xn, sk, _state);
    raddv(n, -1.0, x0, sk, _state);
    mirbfvns_rbfcomputemodel(
        model, xn, &buf->tmp1, ae_true, &buf->tmp0, ae_false, _state);
    *predf = buf->tmp1.ptr.p_double[0];
    *predh = (double) (0);
    if (lccnt > 0) {
      sparsemv(c, xn, &buf->tmp0, _state);
      for (i = 0; i <= lccnt - 1; i++) {
        if (ae_isfinite(cl->ptr.p_double[i], _state)) {
          *predh = *predh
                   + ae_maxreal(cl->ptr.p_double[i] - buf->tmp0.ptr.p_double[i],
                                0.0,
                                _state);
        }
        if (ae_isfinite(cu->ptr.p_double[i], _state)) {
          *predh = *predh
                   + ae_maxreal(buf->tmp0.ptr.p_double[i] - cu->ptr.p_double[i],
                                0.0,
                                _state);
        }
      }
    }
    for (i = 0; i <= nnlc - 1; i++) {
      if (ae_isfinite(nl->ptr.p_double[i], _state)) {
        *predh
            = *predh
              + ae_maxreal(nl->ptr.p_double[i] - buf->tmp1.ptr.p_double[1 + i],
                           0.0,
                           _state);
      }
      if (ae_isfinite(nu->ptr.p_double[i], _state)) {
        *predh
            = *predh
              + ae_maxreal(buf->tmp1.ptr.p_double[1 + i] - nu->ptr.p_double[i],
                           0.0,
                           _state);
      }
    }
  }

  /*************************************************************************
  This function performs initial construction of an RBF model.

    -- ALGLIB --
       Copyright 15.10.2024 by Bochkanov Sergey
  *************************************************************************/
  static void mirbfvns_rbfinitmodel(/* Real    */ const ae_matrix* xf,
                                    /* Real    */ const ae_vector* multscale,
                                    ae_int_t                       nc,
                                    ae_int_t                       n,
                                    ae_int_t                       nf,
                                    mirbfmodel*                    model,
                                    ae_state*                      _state) {
    ae_frame  _frame_block;
    ae_int_t  i;
    ae_int_t  j;
    ae_int_t  k;
    ae_int_t  fidx;
    double    v;
    ae_matrix rbfsys;
    ae_vector sol;
    ae_vector rhs;
    ae_vector sol2;
    ae_matrix rrhs;
    ae_matrix ssol;

    ae_frame_make(_state, &_frame_block);
    memset(&rbfsys, 0, sizeof(rbfsys));
    memset(&sol, 0, sizeof(sol));
    memset(&rhs, 0, sizeof(rhs));
    memset(&sol2, 0, sizeof(sol2));
    memset(&rrhs, 0, sizeof(rrhs));
    memset(&ssol, 0, sizeof(ssol));
    ae_matrix_init(&rbfsys, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sol, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&rhs, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sol2, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&rrhs, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&ssol, 0, 0, DT_REAL, _state, ae_true);

    ae_assert((nc >= 1 && n >= 1) && nf >= 1, "RBF4OPT: 234603 failed", _state);
    rsetallocm(nc + n + 1, nc + n + 1, 0.0, &rbfsys, _state);
    for (i = 0; i <= nc - 1; i++) {
      for (j = 0; j <= i; j++) {
        v = (double) (0);
        for (k = 0; k <= n - 1; k++) {
          v = v
              + ae_sqr((xf->ptr.pp_double[i][k] - xf->ptr.pp_double[j][k])
                           * multscale->ptr.p_double[k],
                       _state);
        }
        v                          = v * ae_sqrt(v, _state);
        rbfsys.ptr.pp_double[i][j] = v;
        rbfsys.ptr.pp_double[j][i] = v;
      }
    }
    for (i = 0; i <= n - 1; i++) {
      for (j = 0; j <= nc - 1; j++) {
        rbfsys.ptr.pp_double[nc + i][j]
            = (xf->ptr.pp_double[j][i] - xf->ptr.pp_double[0][i])
              * multscale->ptr.p_double[i];
        rbfsys.ptr.pp_double[j][nc + i] = rbfsys.ptr.pp_double[nc + i][j];
      }
    }
    for (j = 0; j <= nc - 1; j++) {
      rbfsys.ptr.pp_double[nc + n][j] = 1.0;
      rbfsys.ptr.pp_double[j][nc + n] = 1.0;
    }
    rallocv(nf, &model->vmodelbase, _state);
    rallocv(nf, &model->vmodelscale, _state);
    for (j = 0; j <= nf - 1; j++) {
      model->vmodelbase.ptr.p_double[j] = xf->ptr.pp_double[0][n + j];
      v                                 = (double) (0);
      for (i = 0; i <= nc - 1; i++) {
        v = v
            + ae_sqr(xf->ptr.pp_double[i][n + j]
                         - model->vmodelbase.ptr.p_double[j],
                     _state);
      }
      model->vmodelscale.ptr.p_double[j]
          = ae_sqrt(coalesce(v, (double) (1), _state) / (double) nc, _state);
    }
    model->isdense = ae_true;
    model->n       = n;
    model->nc      = nc;
    model->nf      = nf;
    rgrowrowsfixedcolsm(nc, n, &model->centers, _state);
    rcopym(nc, n, xf, &model->centers, _state);
    rallocm(nf, n, &model->mx0, _state);
    for (i = 0; i <= nf - 1; i++) {
      rcopyrr(n, xf, 0, &model->mx0, i, _state);
    }
    rcopyallocv(n, multscale, &model->multscale, _state);
    rsetallocm(nf, nc + n + 1, 0.0, &rrhs, _state);
    rallocm(nf, nc, &model->crbf, _state);
    rallocm(nf, n + 1, &model->clinear, _state);
    for (fidx = 0; fidx <= nf - 1; fidx++) {
      for (i = 0; i <= nc - 1; i++) {
        rrhs.ptr.pp_double[fidx][i] = (xf->ptr.pp_double[i][n + fidx]
                                       - model->vmodelbase.ptr.p_double[fidx])
                                      / model->vmodelscale.ptr.p_double[fidx];
      }
    }
    mirbfvns_rbfsolvecpdm(
        &rbfsys, &rrhs, nc, nf, n, 0.0, ae_true, &ssol, _state);
    rcopym(nf, nc, &ssol, &model->crbf, _state);
    for (fidx = 0; fidx <= nf - 1; fidx++) {
      for (i = 0; i <= n - 1; i++) {
        model->clinear.ptr.pp_double[fidx][i]
            = ssol.ptr.pp_double[fidx][nc + i];
      }
      model->clinear.ptr.pp_double[fidx][n] = ssol.ptr.pp_double[fidx][nc + n];
    }
    ae_frame_leave(_state);
  }

  /*************************************************************************
  This function performs initial construction of an empty sparse RBF model.

    -- ALGLIB --
       Copyright 15.10.2024 by Bochkanov Sergey
  *************************************************************************/
  static void
  mirbfvns_rbfinitemptysparsemodel(/* Real    */ const ae_vector* multscale,
                                   ae_int_t                       n,
                                   mirbfmodel*                    model,
                                   ae_state*                      _state) {
    ae_assert(n >= 1, "RBF4OPT: 980221 failed", _state);
    model->isdense = ae_false;
    model->n       = n;
    model->nf      = 0;
    rcopyallocv(n, multscale, &model->multscale, _state);
    igrowappendv(1, &model->cridx, 0, _state);
    sparsecreatecrsemptybuf(n, &model->spcenters, _state);
  }

  /*************************************************************************
  This function appends a constant model to a sparse RBF model.

  The sparse model is given by Model parameter, the constant model is given
  by its value V.

    -- ALGLIB --
       Copyright 15.10.2024 by Bochkanov Sergey
  *************************************************************************/
  static void mirbfvns_rbfappendconstantmodel(mirbfmodel* model,
                                              double      v,
                                              ae_state*   _state) {
    ae_int_t n;
    ae_int_t nf;
    ae_int_t offs;

    nf = model->nf;
    n  = model->n;
    ae_assert(!model->isdense, "RBF4OPT: 102335 failed", _state);
    rgrowappendv(nf + 1, &model->vmodelbase, v, _state);
    rgrowappendv(nf + 1, &model->vmodelscale, 0.0, _state);
    rgrowrowsfixedcolsm(nf + 1, n + 1, &model->clinear, _state);
    rgrowrowsfixedcolsm(nf + 1, n, &model->mx0, _state);
    rsetr(n + 1, 0.0, &model->clinear, nf, _state);
    rsetr(n, 0.0, &model->mx0, nf, _state);
    offs = model->cridx.ptr.p_int[model->nf];
    ae_assert(offs == model->spcenters.m, "RBF4OPT: 097316 failed", _state);
    igrowappendv(nf + 2, &model->cridx, offs, _state);
    model->nf = nf + 1;
  }

  /*************************************************************************
  This function appends a dense RBF model to a potentially larger sparse one.

  The sparse model is given by Model parameter, the dense model is given by
  miniModel parameter, with miniModel.N<=Model.N. The mini2full[] array
  maps reduced subspace indexes in [0,miniModel.N) to a full space.

  The dense model must have NF=1.

  The function assumes (but does not check) that Model and miniModel have
  the same Model.multScale[]

    -- ALGLIB --
       Copyright 15.10.2024 by Bochkanov Sergey
  *************************************************************************/
  static void mirbfvns_rbfappendmodel(mirbfmodel*                    model,
                                      const mirbfmodel*              minimodel,
                                      /* Integer */ const ae_vector* mini2full,
                                      ae_state*                      _state) {
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    ae_int_t mn;
    ae_int_t nf;
    ae_int_t offs;

    nf = model->nf;
    n  = model->n;
    mn = minimodel->n;
    ae_assert(minimodel->isdense && !model->isdense,
              "RBF4OPT: 061252 failed",
              _state);
    ae_assert(minimodel->n <= model->n, "RBF4OPT: 061253 failed", _state);
    ae_assert(minimodel->nf == 1, "RBF4OPT: 061254 failed", _state);
    rgrowappendv(nf + 1,
                 &model->vmodelbase,
                 minimodel->vmodelbase.ptr.p_double[0],
                 _state);
    rgrowappendv(nf + 1,
                 &model->vmodelscale,
                 minimodel->vmodelscale.ptr.p_double[0],
                 _state);
    rgrowrowsfixedcolsm(nf + 1, n + 1, &model->clinear, _state);
    rgrowrowsfixedcolsm(nf + 1, n, &model->mx0, _state);
    rsetr(n, 0.0, &model->clinear, nf, _state);
    rsetr(n, 0.0, &model->mx0, nf, _state);
    for (i = 0; i <= mn - 1; i++) {
      model->clinear.ptr.pp_double[nf][mini2full->ptr.p_int[i]]
          = minimodel->clinear.ptr.pp_double[0][i];
      model->mx0.ptr.pp_double[nf][mini2full->ptr.p_int[i]]
          = minimodel->mx0.ptr.pp_double[0][i];
    }
    model->clinear.ptr.pp_double[nf][n]
        = minimodel->clinear.ptr.pp_double[0][minimodel->n];
    offs = model->cridx.ptr.p_int[model->nf];
    ae_assert(offs == model->spcenters.m, "RBF4OPT: 097316 failed", _state);
    for (i = 0; i <= minimodel->nc - 1; i++) {
      sparseappendemptyrow(&model->spcenters, _state);
      for (j = 0; j <= mn - 1; j++) {
        sparseappendelement(&model->spcenters,
                            mini2full->ptr.p_int[j],
                            minimodel->centers.ptr.pp_double[i][j],
                            _state);
      }
      rgrowappendv(offs + 1,
                   &model->spcoeffs,
                   minimodel->crbf.ptr.pp_double[0][i],
                   _state);
      offs = offs + 1;
    }
    igrowappendv(nf + 2, &model->cridx, offs, _state);
    model->nf = nf + 1;
  }

  /*************************************************************************
  This function modifies RBF model by adding linear function to its linear
  term.

      C       array[NF,N+1], one row per function, N columns for coefficients
              before the x[i]-x0[i] term, one column for the constant term.

    -- ALGLIB --
       Copyright 15.10.2024 by Bochkanov Sergey
  *************************************************************************/
  static void mirbfvns_rbfaddlinearterm(mirbfmodel*                    model,
                                        /* Real    */ const ae_matrix* c,
                                        ae_state*                      _state) {
    ae_int_t n;
    ae_int_t k;
    ae_int_t fidx;
    double   v;

    ae_assert(model->isdense, "RBF4OPT: 133304", _state);
    n = model->n;
    for (fidx = 0; fidx <= model->nf - 1; fidx++) {
      for (k = 0; k <= n - 1; k++) {
        v = model->vmodelscale.ptr.p_double[fidx]
            * model->multscale.ptr.p_double[k];
        if (v != (double) 0) {
          model->clinear.ptr.pp_double[fidx][k]
              = model->clinear.ptr.pp_double[fidx][k]
                + c->ptr.pp_double[fidx][k] / v;
        }
      }
      if (model->vmodelscale.ptr.p_double[fidx] != (double) 0) {
        model->clinear.ptr.pp_double[fidx][n]
            = model->clinear.ptr.pp_double[fidx][n]
              + c->ptr.pp_double[fidx][n]
                    / model->vmodelscale.ptr.p_double[fidx];
      }
    }
  }

  /*************************************************************************
  This function computes RBF model at the required point. May return model
  value and its gradient.

    -- ALGLIB --
       Copyright 15.10.2024 by Bochkanov Sergey
  *************************************************************************/
  static void mirbfvns_rbfcomputemodel(const mirbfmodel*              mmodel,
                                       /* Real    */ const ae_vector* x,
                                       /* Real    */ ae_vector*       f,
                                       ae_bool                        needf,
                                       /* Real    */ ae_vector*       g,
                                       ae_bool                        needg,
                                       ae_state*                      _state) {
    ae_int_t n;
    ae_int_t j;
    ae_int_t k;
    ae_int_t fidx;
    ae_int_t cc;
    ae_int_t c0;
    ae_int_t c1;
    ae_int_t kk;
    ae_int_t k0;
    ae_int_t k1;
    double   v;
    double   r;
    double   vf;
    double   vc;

    n = mmodel->n;
    ae_assert(!needf || f->cnt >= mmodel->nf,
              "RBF4OPT: integrity check 419111 failed",
              _state);
    ae_assert(!needg || g->cnt >= mmodel->nf * n,
              "RBF4OPT: integrity check 419112 failed",
              _state);
    if (needf) {
      rsetv(mmodel->nf, 0.0, f, _state);
    }
    if (needg) {
      rsetv(mmodel->nf * n, 0.0, g, _state);
    }
    if (mmodel->isdense) {
      for (fidx = 0; fidx <= mmodel->nf - 1; fidx++) {
        vf = (double) (0);
        for (j = 0; j <= mmodel->nc - 1; j++) {
          r = (double) (0);
          for (k = 0; k <= n - 1; k++) {
            v = (x->ptr.p_double[k] - mmodel->centers.ptr.pp_double[j][k])
                * mmodel->multscale.ptr.p_double[k];
            r = r + v * v;
          }
          r  = ae_sqrt(r, _state);
          vf = vf + mmodel->crbf.ptr.pp_double[fidx][j] * (r * r * r);
          if (needg) {
            for (k = 0; k <= n - 1; k++) {
              g->ptr.p_double[fidx * n + k]
                  = g->ptr.p_double[fidx * n + k]
                    + mmodel->crbf.ptr.pp_double[fidx][j] * (double) 3 * r
                          * (x->ptr.p_double[k]
                             - mmodel->centers.ptr.pp_double[j][k])
                          * mmodel->multscale.ptr.p_double[k]
                          * mmodel->multscale.ptr.p_double[k];
            }
          }
        }
        for (k = 0; k <= n - 1; k++) {
          vf = vf
               + mmodel->clinear.ptr.pp_double[fidx][k]
                     * (x->ptr.p_double[k] - mmodel->mx0.ptr.pp_double[fidx][k])
                     * mmodel->multscale.ptr.p_double[k];
          if (needg) {
            g->ptr.p_double[fidx * n + k]
                = g->ptr.p_double[fidx * n + k]
                  + mmodel->clinear.ptr.pp_double[fidx][k]
                        * mmodel->multscale.ptr.p_double[k];
          }
        }
        vf = vf + mmodel->clinear.ptr.pp_double[fidx][n];
        if (needf) {
          f->ptr.p_double[fidx] = vf * mmodel->vmodelscale.ptr.p_double[fidx]
                                  + mmodel->vmodelbase.ptr.p_double[fidx];
        }
        if (needg) {
          rmulvx(
              n, mmodel->vmodelscale.ptr.p_double[fidx], g, fidx * n, _state);
        }
      }
    } else {
      for (fidx = 0; fidx <= mmodel->nf - 1; fidx++) {
        vf = (double) (0);
        c0 = mmodel->cridx.ptr.p_int[fidx];
        c1 = mmodel->cridx.ptr.p_int[fidx + 1] - 1;
        for (cc = c0; cc <= c1; cc++) {
          r  = (double) (0);
          k0 = mmodel->spcenters.ridx.ptr.p_int[cc];
          k1 = mmodel->spcenters.ridx.ptr.p_int[cc + 1] - 1;
          for (kk = k0; kk <= k1; kk++) {
            k = mmodel->spcenters.idx.ptr.p_int[kk];
            v = (x->ptr.p_double[k] - mmodel->spcenters.vals.ptr.p_double[kk])
                * mmodel->multscale.ptr.p_double[k];
            r = r + v * v;
          }
          r  = ae_sqrt(r, _state);
          vc = mmodel->spcoeffs.ptr.p_double[cc];
          vf = vf + vc * (r * r * r);
          if (needg) {
            for (kk = k0; kk <= k1; kk++) {
              k = mmodel->spcenters.idx.ptr.p_int[kk];
              g->ptr.p_double[fidx * n + k]
                  = g->ptr.p_double[fidx * n + k]
                    + vc * (double) 3 * r
                          * (x->ptr.p_double[k]
                             - mmodel->spcenters.vals.ptr.p_double[kk])
                          * mmodel->multscale.ptr.p_double[k]
                          * mmodel->multscale.ptr.p_double[k];
            }
          }
        }
        for (k = 0; k <= n - 1; k++) {
          vf = vf
               + mmodel->clinear.ptr.pp_double[fidx][k]
                     * (x->ptr.p_double[k] - mmodel->mx0.ptr.pp_double[fidx][k])
                     * mmodel->multscale.ptr.p_double[k];
          if (needg) {
            g->ptr.p_double[fidx * n + k]
                = g->ptr.p_double[fidx * n + k]
                  + mmodel->clinear.ptr.pp_double[fidx][k]
                        * mmodel->multscale.ptr.p_double[k];
          }
        }
        vf = vf + mmodel->clinear.ptr.pp_double[fidx][n];
        if (needf) {
          f->ptr.p_double[fidx] = vf * mmodel->vmodelscale.ptr.p_double[fidx]
                                  + mmodel->vmodelbase.ptr.p_double[fidx];
        }
        if (needg) {
          rmulvx(
              n, mmodel->vmodelscale.ptr.p_double[fidx], g, fidx * n, _state);
        }
      }
    }
  }

  /*************************************************************************
  This function solves RBF  system using conditionally positive definiteness
  if possible.

  INPUT PARAMETERS:
      A           array[NCenters+NX,NCenters], basis function matrix and
                  linear polynomial values
      B           array[NCenters], target values
      NCenters    centers count
      NX          space dimensionality
      LambdaV     smoothing parameter, LambdaV>=0
      isCPD       whether basis is conditionally positive definite or not


    -- ALGLIB --
       Copyright 15.10.2024 by Bochkanov Sergey
  *************************************************************************/
  static void mirbfvns_rbfsolvecpdm(/* Real    */ const ae_matrix* a,
                                    /* Real    */ const ae_matrix* bb,
                                    ae_int_t                       ncenters,
                                    ae_int_t                       nrhs,
                                    ae_int_t                       nx,
                                    double                         lambdav,
                                    ae_bool                        iscpd,
                                    /* Real    */ ae_matrix*       ssol,
                                    ae_state*                      _state) {
    ae_frame  _frame_block;
    ae_int_t  ncoeff;
    ae_int_t  i;
    ae_int_t  j;
    ae_int_t  k;
    double    v;
    double    vv;
    double    mx;
    double    reg;
    ae_int_t  ortbasissize;
    ae_matrix q;
    ae_matrix q1;
    ae_matrix r;
    ae_vector c;
    ae_vector y;
    ae_vector z;
    ae_vector ortbasismap;
    ae_vector choltmp;

    ae_frame_make(_state, &_frame_block);
    memset(&q, 0, sizeof(q));
    memset(&q1, 0, sizeof(q1));
    memset(&r, 0, sizeof(r));
    memset(&c, 0, sizeof(c));
    memset(&y, 0, sizeof(y));
    memset(&z, 0, sizeof(z));
    memset(&ortbasismap, 0, sizeof(ortbasismap));
    memset(&choltmp, 0, sizeof(choltmp));
    ae_matrix_init(&q, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&q1, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&r, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&c, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&z, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ortbasismap, 0, DT_INT, _state, ae_true);
    ae_vector_init(&choltmp, 0, DT_REAL, _state, ae_true);

    ae_assert(ae_fp_greater_eq(lambdav, (double) (0)),
              "DFGM: integrity check 854519 failed",
              _state);
    ae_assert(iscpd, "DFGM: integrity check 855520 failed", _state);
    ncoeff = ncenters + nx + 1;
    reg    = ae_sqrt(ae_machineepsilon, _state);
    rallocm(nx + 1, nx + 1, &r, _state);
    rallocm(nx + 1, ncenters, &q1, _state);
    iallocv(nx + 1, &ortbasismap, _state);
    rsetr(ncenters,
          (double) 1 / ae_sqrt((double) (ncenters), _state),
          &q1,
          0,
          _state);
    r.ptr.pp_double[0][0]    = ae_sqrt((double) (ncenters), _state);
    ortbasismap.ptr.p_int[0] = nx;
    ortbasissize             = 1;
    rallocv(ncenters, &z, _state);
    for (k = 0; k <= nx - 1; k++) {
      for (j = 0; j <= ncenters - 1; j++) {
        z.ptr.p_double[j] = a->ptr.pp_double[ncenters + k][j];
      }
      v = ae_sqrt(rdotv2(ncenters, &z, _state), _state);
      rowwisegramschmidt(&q1, ortbasissize, ncenters, &z, &y, ae_true, _state);
      vv = ae_sqrt(rdotv2(ncenters, &z, _state), _state);
      if (ae_fp_greater(
              vv, ae_sqrt(ae_machineepsilon, _state) * (v + (double) 1))) {
        rcopymulvr(ncenters, (double) 1 / vv, &z, &q1, ortbasissize, _state);
        rcopyvc(ortbasissize, &y, &r, ortbasissize, _state);
        r.ptr.pp_double[ortbasissize][ortbasissize] = vv;
        ortbasismap.ptr.p_int[ortbasissize]         = k;
        ortbasissize                                = ortbasissize + 1;
      }
    }
    rsetallocm(ncenters, ncenters, 0.0, &q, _state);
    for (i = 0; i <= ncenters - 1; i++) {
      rcopyrr(ncenters, a, i, &q, i, _state);
    }
    rallocm(nrhs, ncoeff, ssol, _state);
    rcopym(nrhs, ncenters, bb, ssol, _state);
    for (i = 0; i <= ncenters - 1; i++) {
      q.ptr.pp_double[i][i] = q.ptr.pp_double[i][i] + lambdav;
    }
    rallocv(ncenters, &z, _state);
    for (i = 0; i <= ncenters - 1; i++) {
      rcopyrv(ncenters, &q, i, &z, _state);
      rowwisegramschmidt(&q1, ortbasissize, ncenters, &z, &y, ae_false, _state);
      rcopyvr(ncenters, &z, &q, i, _state);
    }
    for (i = 0; i <= ncenters - 1; i++) {
      rcopycv(ncenters, &q, i, &z, _state);
      rowwisegramschmidt(&q1, ortbasissize, ncenters, &z, &y, ae_false, _state);
      rcopyvc(ncenters, &z, &q, i, _state);
    }
    for (i = 0; i <= nrhs - 1; i++) {
      rcopyrv(ncenters, ssol, i, &z, _state);
      rowwisegramschmidt(&q1, ortbasissize, ncenters, &z, &y, ae_false, _state);
      rcopyvr(ncenters, &z, ssol, i, _state);
    }
    mx = 1.0;
    for (i = 0; i <= ncenters - 1; i++) {
      mx = ae_maxreal(mx, ae_fabs(q.ptr.pp_double[i][i], _state), _state);
    }
    for (i = 0; i <= ncenters - 1; i++) {
      rcopyrv(ncenters, &q, i, &z, _state);
      for (j = 0; j <= ortbasissize - 1; j++) {
        raddrv(ncenters, mx * q1.ptr.pp_double[j][i], &q1, j, &z, _state);
      }
      rcopyvr(ncenters, &z, &q, i, _state);
    }
    for (i = 0; i <= ncenters - 1; i++) {
      q.ptr.pp_double[i][i] = q.ptr.pp_double[i][i] + reg * mx;
    }
    if (!spdmatrixcholeskyrec(&q, 0, ncenters, ae_false, &choltmp, _state)) {
      ae_assert(ae_false,
                "GENMOD: RBF solver failed due to extreme degeneracy",
                _state);
    }
    rmatrixrighttrsm(
        nrhs, ncenters, &q, 0, 0, ae_false, ae_false, 1, ssol, 0, 0, _state);
    rmatrixrighttrsm(
        nrhs, ncenters, &q, 0, 0, ae_false, ae_false, 0, ssol, 0, 0, _state);
    rallocv(ncenters, &z, _state);
    rallocv(ncenters, &c, _state);
    for (i = 0; i <= nrhs - 1; i++) {
      rcopyrv(ncenters, bb, i, &z, _state);
      rcopyrv(ncenters, ssol, i, &c, _state);
      rgemv(ncenters, ncenters, -1.0, a, 0, &c, 1.0, &z, _state);
      rowwisegramschmidt(&q1, ortbasissize, ncenters, &z, &y, ae_true, _state);
      rmatrixtrsv(ortbasissize, &r, 0, 0, ae_true, ae_false, 0, &y, 0, _state);
      for (j = 0; j <= nx; j++) {
        ssol->ptr.pp_double[i][ncenters + j] = 0.0;
      }
      for (j = 0; j <= ortbasissize - 1; j++) {
        ssol->ptr.pp_double[i][ncenters + ortbasismap.ptr.p_int[j]]
            = y.ptr.p_double[j];
      }
    }
    ae_frame_leave(_state);
  }

  static double mirbfvns_rdistinfrr(ae_int_t                       n,
                                    /* Real    */ const ae_matrix* a,
                                    ae_int_t                       i0,
                                    /* Real    */ const ae_matrix* b,
                                    ae_int_t                       i1,
                                    ae_state*                      _state) {
    ae_int_t i;
    double   v;
    double   result;

    result = (double) (0);
    for (i = 0; i <= n - 1; i++) {
      v      = a->ptr.pp_double[i0][i] - b->ptr.pp_double[i1][i];
      result = ae_maxreal(result, ae_fabs(v, _state), _state);
    }
    return result;
  }

  void _mirbfmodel_init(void* _p, ae_state* _state, ae_bool make_automatic) {
    mirbfmodel* p = (mirbfmodel*) _p;
    ae_touch_ptr((void*) p);
    ae_vector_init(&p->vmodelbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->vmodelscale, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->multscale, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->clinear, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->mx0, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->centers, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->crbf, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cridx, 0, DT_INT, _state, make_automatic);
    _sparsematrix_init(&p->spcenters, _state, make_automatic);
    ae_vector_init(&p->spcoeffs, 0, DT_REAL, _state, make_automatic);
  }

  void _mirbfmodel_init_copy(void*       _dst,
                             const void* _src,
                             ae_state*   _state,
                             ae_bool     make_automatic) {
    mirbfmodel*       dst = (mirbfmodel*) _dst;
    const mirbfmodel* src = (const mirbfmodel*) _src;
    dst->isdense          = src->isdense;
    dst->n                = src->n;
    dst->nf               = src->nf;
    ae_vector_init_copy(
        &dst->vmodelbase, &src->vmodelbase, _state, make_automatic);
    ae_vector_init_copy(
        &dst->vmodelscale, &src->vmodelscale, _state, make_automatic);
    ae_vector_init_copy(
        &dst->multscale, &src->multscale, _state, make_automatic);
    ae_matrix_init_copy(&dst->clinear, &src->clinear, _state, make_automatic);
    ae_matrix_init_copy(&dst->mx0, &src->mx0, _state, make_automatic);
    dst->nc = src->nc;
    ae_matrix_init_copy(&dst->centers, &src->centers, _state, make_automatic);
    ae_matrix_init_copy(&dst->crbf, &src->crbf, _state, make_automatic);
    ae_vector_init_copy(&dst->cridx, &src->cridx, _state, make_automatic);
    _sparsematrix_init_copy(
        &dst->spcenters, &src->spcenters, _state, make_automatic);
    ae_vector_init_copy(&dst->spcoeffs, &src->spcoeffs, _state, make_automatic);
  }

  void _mirbfmodel_clear(void* _p) {
    mirbfmodel* p = (mirbfmodel*) _p;
    ae_touch_ptr((void*) p);
    ae_vector_clear(&p->vmodelbase);
    ae_vector_clear(&p->vmodelscale);
    ae_vector_clear(&p->multscale);
    ae_matrix_clear(&p->clinear);
    ae_matrix_clear(&p->mx0);
    ae_matrix_clear(&p->centers);
    ae_matrix_clear(&p->crbf);
    ae_vector_clear(&p->cridx);
    _sparsematrix_clear(&p->spcenters);
    ae_vector_clear(&p->spcoeffs);
  }

  void _mirbfmodel_destroy(void* _p) {
    mirbfmodel* p = (mirbfmodel*) _p;
    ae_touch_ptr((void*) p);
    ae_vector_destroy(&p->vmodelbase);
    ae_vector_destroy(&p->vmodelscale);
    ae_vector_destroy(&p->multscale);
    ae_matrix_destroy(&p->clinear);
    ae_matrix_destroy(&p->mx0);
    ae_matrix_destroy(&p->centers);
    ae_matrix_destroy(&p->crbf);
    ae_vector_destroy(&p->cridx);
    _sparsematrix_destroy(&p->spcenters);
    ae_vector_destroy(&p->spcoeffs);
  }

  void _mirbfvnsnodesubsolver_init(void*     _p,
                                   ae_state* _state,
                                   ae_bool   make_automatic) {
    mirbfvnsnodesubsolver* p = (mirbfvnsnodesubsolver*) _p;
    ae_touch_ptr((void*) p);
    ae_vector_init(&p->successfhistory, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->successhhistory, 0, DT_REAL, _state, make_automatic);
  }

  void _mirbfvnsnodesubsolver_init_copy(void*       _dst,
                                        const void* _src,
                                        ae_state*   _state,
                                        ae_bool     make_automatic) {
    mirbfvnsnodesubsolver*       dst = (mirbfvnsnodesubsolver*) _dst;
    const mirbfvnsnodesubsolver* src = (const mirbfvnsnodesubsolver*) _src;
    dst->trustrad                    = src->trustrad;
    dst->sufficientcloudsize         = src->sufficientcloudsize;
    dst->basef                       = src->basef;
    dst->baseh                       = src->baseh;
    dst->predf                       = src->predf;
    dst->predh                       = src->predh;
    dst->skrellen                    = src->skrellen;
    dst->maxh                        = src->maxh;
    ae_vector_init_copy(
        &dst->successfhistory, &src->successfhistory, _state, make_automatic);
    ae_vector_init_copy(
        &dst->successhhistory, &src->successhhistory, _state, make_automatic);
    dst->historymax = src->historymax;
  }

  void _mirbfvnsnodesubsolver_clear(void* _p) {
    mirbfvnsnodesubsolver* p = (mirbfvnsnodesubsolver*) _p;
    ae_touch_ptr((void*) p);
    ae_vector_clear(&p->successfhistory);
    ae_vector_clear(&p->successhhistory);
  }

  void _mirbfvnsnodesubsolver_destroy(void* _p) {
    mirbfvnsnodesubsolver* p = (mirbfvnsnodesubsolver*) _p;
    ae_touch_ptr((void*) p);
    ae_vector_destroy(&p->successfhistory);
    ae_vector_destroy(&p->successhhistory);
  }

  void _rbfmmtemporaries_init(void*     _p,
                              ae_state* _state,
                              ae_bool   make_automatic) {
    rbfmmtemporaries* p = (rbfmmtemporaries*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_init(&p->crit, _state, make_automatic);
    ae_vector_init(&p->bndlx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndux, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x0x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->scalingfactors, 0, DT_REAL, _state, make_automatic);
    _minfsqpstate_init(&p->fsqpsolver, _state, make_automatic);
    _smoothnessmonitor_init(&p->smonitor, _state, make_automatic);
    _sparsematrix_init(&p->cx, _state, make_automatic);
    ae_vector_init(&p->clx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cux, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpnl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpnu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpi, 0, DT_INT, _state, make_automatic);
  }

  void _rbfmmtemporaries_init_copy(void*       _dst,
                                   const void* _src,
                                   ae_state*   _state,
                                   ae_bool     make_automatic) {
    rbfmmtemporaries*       dst = (rbfmmtemporaries*) _dst;
    const rbfmmtemporaries* src = (const rbfmmtemporaries*) _src;
    _nlpstoppingcriteria_init_copy(
        &dst->crit, &src->crit, _state, make_automatic);
    ae_vector_init_copy(&dst->bndlx, &src->bndlx, _state, make_automatic);
    ae_vector_init_copy(&dst->bndux, &src->bndux, _state, make_automatic);
    ae_vector_init_copy(&dst->x0x, &src->x0x, _state, make_automatic);
    ae_vector_init_copy(&dst->sx, &src->sx, _state, make_automatic);
    ae_vector_init_copy(
        &dst->scalingfactors, &src->scalingfactors, _state, make_automatic);
    _minfsqpstate_init_copy(
        &dst->fsqpsolver, &src->fsqpsolver, _state, make_automatic);
    _smoothnessmonitor_init_copy(
        &dst->smonitor, &src->smonitor, _state, make_automatic);
    _sparsematrix_init_copy(&dst->cx, &src->cx, _state, make_automatic);
    ae_vector_init_copy(&dst->clx, &src->clx, _state, make_automatic);
    ae_vector_init_copy(&dst->cux, &src->cux, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpnl, &src->tmpnl, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpnu, &src->tmpnu, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpi, &src->tmpi, _state, make_automatic);
  }

  void _rbfmmtemporaries_clear(void* _p) {
    rbfmmtemporaries* p = (rbfmmtemporaries*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_clear(&p->crit);
    ae_vector_clear(&p->bndlx);
    ae_vector_clear(&p->bndux);
    ae_vector_clear(&p->x0x);
    ae_vector_clear(&p->sx);
    ae_vector_clear(&p->scalingfactors);
    _minfsqpstate_clear(&p->fsqpsolver);
    _smoothnessmonitor_clear(&p->smonitor);
    _sparsematrix_clear(&p->cx);
    ae_vector_clear(&p->clx);
    ae_vector_clear(&p->cux);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->tmp2);
    ae_vector_clear(&p->tmpnl);
    ae_vector_clear(&p->tmpnu);
    ae_vector_clear(&p->tmpi);
  }

  void _rbfmmtemporaries_destroy(void* _p) {
    rbfmmtemporaries* p = (rbfmmtemporaries*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_destroy(&p->crit);
    ae_vector_destroy(&p->bndlx);
    ae_vector_destroy(&p->bndux);
    ae_vector_destroy(&p->x0x);
    ae_vector_destroy(&p->sx);
    ae_vector_destroy(&p->scalingfactors);
    _minfsqpstate_destroy(&p->fsqpsolver);
    _smoothnessmonitor_destroy(&p->smonitor);
    _sparsematrix_destroy(&p->cx);
    ae_vector_destroy(&p->clx);
    ae_vector_destroy(&p->cux);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->tmp2);
    ae_vector_destroy(&p->tmpnl);
    ae_vector_destroy(&p->tmpnu);
    ae_vector_destroy(&p->tmpi);
  }

  void _mirbfvnstemporaries_init(void*     _p,
                                 ae_state* _state,
                                 ae_bool   make_automatic) {
    mirbfvnstemporaries* p = (mirbfvnstemporaries*) _p;
    ae_touch_ptr((void*) p);
    _stimer_init(&p->localtimer, _state, make_automatic);
    _hqrndstate_init(&p->localrng, _state, make_automatic);
    ae_vector_init(&p->glbbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fullx0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbx0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbtmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbtmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbtmp2, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->glbxf, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->glbsx, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->ortdeltas, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbmultscale, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbvtrustregion, 0, DT_REAL, _state, make_automatic);
    _mirbfmodel_init(&p->glbmodel, _state, make_automatic);
    _rbfmmtemporaries_init(&p->buf, _state, make_automatic);
    ae_vector_init(&p->glbsk, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->glbrandomprior, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbprioratx0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbxtrial, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->mapfull2compact, 0, DT_INT, _state, make_automatic);
    _sparsematrix_init(&p->glba, _state, make_automatic);
    ae_vector_init(&p->glbal, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbau, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->glbmask, 0, DT_BOOL, _state, make_automatic);
    _rbfmmtemporaries_init(&p->mmbuf, _state, make_automatic);
    ae_vector_init(&p->lclidxfrac, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->lcl2glb, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->lclxf, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->lclsx, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nodeslist, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->lclrandomprior, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lclmultscale, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lcls, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lclx0, 0, DT_REAL, _state, make_automatic);
    _mirbfmodel_init(&p->tmpmodel, _state, make_automatic);
    _ipm2state_init(&p->qpsubsolver, _state, make_automatic);
    _bbgdstate_init(&p->bbgdsubsolver, _state, make_automatic);
    ae_vector_init(&p->wrkbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpb, 0, DT_BOOL, _state, make_automatic);
    _sparsematrix_init(&p->diaga, _state, make_automatic);
    ae_vector_init(&p->linb, 0, DT_REAL, _state, make_automatic);
  }

  void _mirbfvnstemporaries_init_copy(void*       _dst,
                                      const void* _src,
                                      ae_state*   _state,
                                      ae_bool     make_automatic) {
    mirbfvnstemporaries*       dst = (mirbfvnstemporaries*) _dst;
    const mirbfvnstemporaries* src = (const mirbfvnstemporaries*) _src;
    _stimer_init_copy(
        &dst->localtimer, &src->localtimer, _state, make_automatic);
    _hqrndstate_init_copy(
        &dst->localrng, &src->localrng, _state, make_automatic);
    ae_vector_init_copy(&dst->glbbndl, &src->glbbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->glbbndu, &src->glbbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->fullx0, &src->fullx0, _state, make_automatic);
    ae_vector_init_copy(&dst->glbx0, &src->glbx0, _state, make_automatic);
    ae_vector_init_copy(&dst->glbtmp0, &src->glbtmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->glbtmp1, &src->glbtmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->glbtmp2, &src->glbtmp2, _state, make_automatic);
    ae_matrix_init_copy(&dst->glbxf, &src->glbxf, _state, make_automatic);
    ae_matrix_init_copy(&dst->glbsx, &src->glbsx, _state, make_automatic);
    ae_matrix_init_copy(
        &dst->ortdeltas, &src->ortdeltas, _state, make_automatic);
    ae_vector_init_copy(
        &dst->glbmultscale, &src->glbmultscale, _state, make_automatic);
    ae_vector_init_copy(
        &dst->glbvtrustregion, &src->glbvtrustregion, _state, make_automatic);
    _mirbfmodel_init_copy(
        &dst->glbmodel, &src->glbmodel, _state, make_automatic);
    _rbfmmtemporaries_init_copy(&dst->buf, &src->buf, _state, make_automatic);
    ae_vector_init_copy(&dst->glbsk, &src->glbsk, _state, make_automatic);
    ae_matrix_init_copy(
        &dst->glbrandomprior, &src->glbrandomprior, _state, make_automatic);
    ae_vector_init_copy(
        &dst->glbprioratx0, &src->glbprioratx0, _state, make_automatic);
    ae_vector_init_copy(
        &dst->glbxtrial, &src->glbxtrial, _state, make_automatic);
    ae_vector_init_copy(&dst->glbs, &src->glbs, _state, make_automatic);
    ae_vector_init_copy(
        &dst->mapfull2compact, &src->mapfull2compact, _state, make_automatic);
    _sparsematrix_init_copy(&dst->glba, &src->glba, _state, make_automatic);
    ae_vector_init_copy(&dst->glbal, &src->glbal, _state, make_automatic);
    ae_vector_init_copy(&dst->glbau, &src->glbau, _state, make_automatic);
    ae_vector_init_copy(&dst->glbmask, &src->glbmask, _state, make_automatic);
    _rbfmmtemporaries_init_copy(
        &dst->mmbuf, &src->mmbuf, _state, make_automatic);
    ae_vector_init_copy(
        &dst->lclidxfrac, &src->lclidxfrac, _state, make_automatic);
    ae_vector_init_copy(&dst->lcl2glb, &src->lcl2glb, _state, make_automatic);
    ae_matrix_init_copy(&dst->lclxf, &src->lclxf, _state, make_automatic);
    ae_matrix_init_copy(&dst->lclsx, &src->lclsx, _state, make_automatic);
    ae_vector_init_copy(
        &dst->nodeslist, &src->nodeslist, _state, make_automatic);
    ae_matrix_init_copy(
        &dst->lclrandomprior, &src->lclrandomprior, _state, make_automatic);
    ae_vector_init_copy(
        &dst->lclmultscale, &src->lclmultscale, _state, make_automatic);
    ae_vector_init_copy(&dst->lcls, &src->lcls, _state, make_automatic);
    ae_vector_init_copy(&dst->lclx0, &src->lclx0, _state, make_automatic);
    _mirbfmodel_init_copy(
        &dst->tmpmodel, &src->tmpmodel, _state, make_automatic);
    _ipm2state_init_copy(
        &dst->qpsubsolver, &src->qpsubsolver, _state, make_automatic);
    _bbgdstate_init_copy(
        &dst->bbgdsubsolver, &src->bbgdsubsolver, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkbndl, &src->wrkbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkbndu, &src->wrkbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpb, &src->tmpb, _state, make_automatic);
    _sparsematrix_init_copy(&dst->diaga, &src->diaga, _state, make_automatic);
    ae_vector_init_copy(&dst->linb, &src->linb, _state, make_automatic);
  }

  void _mirbfvnstemporaries_clear(void* _p) {
    mirbfvnstemporaries* p = (mirbfvnstemporaries*) _p;
    ae_touch_ptr((void*) p);
    _stimer_clear(&p->localtimer);
    _hqrndstate_clear(&p->localrng);
    ae_vector_clear(&p->glbbndl);
    ae_vector_clear(&p->glbbndu);
    ae_vector_clear(&p->fullx0);
    ae_vector_clear(&p->glbx0);
    ae_vector_clear(&p->glbtmp0);
    ae_vector_clear(&p->glbtmp1);
    ae_vector_clear(&p->glbtmp2);
    ae_matrix_clear(&p->glbxf);
    ae_matrix_clear(&p->glbsx);
    ae_matrix_clear(&p->ortdeltas);
    ae_vector_clear(&p->glbmultscale);
    ae_vector_clear(&p->glbvtrustregion);
    _mirbfmodel_clear(&p->glbmodel);
    _rbfmmtemporaries_clear(&p->buf);
    ae_vector_clear(&p->glbsk);
    ae_matrix_clear(&p->glbrandomprior);
    ae_vector_clear(&p->glbprioratx0);
    ae_vector_clear(&p->glbxtrial);
    ae_vector_clear(&p->glbs);
    ae_vector_clear(&p->mapfull2compact);
    _sparsematrix_clear(&p->glba);
    ae_vector_clear(&p->glbal);
    ae_vector_clear(&p->glbau);
    ae_vector_clear(&p->glbmask);
    _rbfmmtemporaries_clear(&p->mmbuf);
    ae_vector_clear(&p->lclidxfrac);
    ae_vector_clear(&p->lcl2glb);
    ae_matrix_clear(&p->lclxf);
    ae_matrix_clear(&p->lclsx);
    ae_vector_clear(&p->nodeslist);
    ae_matrix_clear(&p->lclrandomprior);
    ae_vector_clear(&p->lclmultscale);
    ae_vector_clear(&p->lcls);
    ae_vector_clear(&p->lclx0);
    _mirbfmodel_clear(&p->tmpmodel);
    _ipm2state_clear(&p->qpsubsolver);
    _bbgdstate_clear(&p->bbgdsubsolver);
    ae_vector_clear(&p->wrkbndl);
    ae_vector_clear(&p->wrkbndu);
    ae_vector_clear(&p->tmpb);
    _sparsematrix_clear(&p->diaga);
    ae_vector_clear(&p->linb);
  }

  void _mirbfvnstemporaries_destroy(void* _p) {
    mirbfvnstemporaries* p = (mirbfvnstemporaries*) _p;
    ae_touch_ptr((void*) p);
    _stimer_destroy(&p->localtimer);
    _hqrndstate_destroy(&p->localrng);
    ae_vector_destroy(&p->glbbndl);
    ae_vector_destroy(&p->glbbndu);
    ae_vector_destroy(&p->fullx0);
    ae_vector_destroy(&p->glbx0);
    ae_vector_destroy(&p->glbtmp0);
    ae_vector_destroy(&p->glbtmp1);
    ae_vector_destroy(&p->glbtmp2);
    ae_matrix_destroy(&p->glbxf);
    ae_matrix_destroy(&p->glbsx);
    ae_matrix_destroy(&p->ortdeltas);
    ae_vector_destroy(&p->glbmultscale);
    ae_vector_destroy(&p->glbvtrustregion);
    _mirbfmodel_destroy(&p->glbmodel);
    _rbfmmtemporaries_destroy(&p->buf);
    ae_vector_destroy(&p->glbsk);
    ae_matrix_destroy(&p->glbrandomprior);
    ae_vector_destroy(&p->glbprioratx0);
    ae_vector_destroy(&p->glbxtrial);
    ae_vector_destroy(&p->glbs);
    ae_vector_destroy(&p->mapfull2compact);
    _sparsematrix_destroy(&p->glba);
    ae_vector_destroy(&p->glbal);
    ae_vector_destroy(&p->glbau);
    ae_vector_destroy(&p->glbmask);
    _rbfmmtemporaries_destroy(&p->mmbuf);
    ae_vector_destroy(&p->lclidxfrac);
    ae_vector_destroy(&p->lcl2glb);
    ae_matrix_destroy(&p->lclxf);
    ae_matrix_destroy(&p->lclsx);
    ae_vector_destroy(&p->nodeslist);
    ae_matrix_destroy(&p->lclrandomprior);
    ae_vector_destroy(&p->lclmultscale);
    ae_vector_destroy(&p->lcls);
    ae_vector_destroy(&p->lclx0);
    _mirbfmodel_destroy(&p->tmpmodel);
    _ipm2state_destroy(&p->qpsubsolver);
    _bbgdstate_destroy(&p->bbgdsubsolver);
    ae_vector_destroy(&p->wrkbndl);
    ae_vector_destroy(&p->wrkbndu);
    ae_vector_destroy(&p->tmpb);
    _sparsematrix_destroy(&p->diaga);
    ae_vector_destroy(&p->linb);
  }

  void _mirbfvnsgrid_init(void* _p, ae_state* _state, ae_bool make_automatic) {
    mirbfvnsgrid* p = (mirbfvnsgrid*) _p;
    ae_touch_ptr((void*) p);
    ae_matrix_init(&p->nodesinfo, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ptlistheads, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->ptlistdata, 0, DT_INT, _state, make_automatic);
    ae_obj_array_init(&p->subsolvers, _state, make_automatic);
  }

  void _mirbfvnsgrid_init_copy(void*       _dst,
                               const void* _src,
                               ae_state*   _state,
                               ae_bool     make_automatic) {
    mirbfvnsgrid*       dst = (mirbfvnsgrid*) _dst;
    const mirbfvnsgrid* src = (const mirbfvnsgrid*) _src;
    dst->nnodes             = src->nnodes;
    ae_matrix_init_copy(
        &dst->nodesinfo, &src->nodesinfo, _state, make_automatic);
    dst->ptlistlength = src->ptlistlength;
    ae_vector_init_copy(
        &dst->ptlistheads, &src->ptlistheads, _state, make_automatic);
    ae_vector_init_copy(
        &dst->ptlistdata, &src->ptlistdata, _state, make_automatic);
    ae_obj_array_init_copy(
        &dst->subsolvers, &src->subsolvers, _state, make_automatic);
    dst->naddcols = src->naddcols;
  }

  void _mirbfvnsgrid_clear(void* _p) {
    mirbfvnsgrid* p = (mirbfvnsgrid*) _p;
    ae_touch_ptr((void*) p);
    ae_matrix_clear(&p->nodesinfo);
    ae_vector_clear(&p->ptlistheads);
    ae_vector_clear(&p->ptlistdata);
    ae_obj_array_clear(&p->subsolvers);
  }

  void _mirbfvnsgrid_destroy(void* _p) {
    mirbfvnsgrid* p = (mirbfvnsgrid*) _p;
    ae_touch_ptr((void*) p);
    ae_matrix_destroy(&p->nodesinfo);
    ae_vector_destroy(&p->ptlistheads);
    ae_vector_destroy(&p->ptlistdata);
    ae_obj_array_destroy(&p->subsolvers);
  }

  void _mirbfvnsdataset_init(void*     _p,
                             ae_state* _state,
                             ae_bool   make_automatic) {
    mirbfvnsdataset* p = (mirbfvnsdataset*) _p;
    ae_touch_ptr((void*) p);
    ae_matrix_init(&p->pointinfo, 0, 0, DT_REAL, _state, make_automatic);
  }

  void _mirbfvnsdataset_init_copy(void*       _dst,
                                  const void* _src,
                                  ae_state*   _state,
                                  ae_bool     make_automatic) {
    mirbfvnsdataset*       dst = (mirbfvnsdataset*) _dst;
    const mirbfvnsdataset* src = (const mirbfvnsdataset*) _src;
    dst->npoints               = src->npoints;
    dst->nvars                 = src->nvars;
    dst->nnlc                  = src->nnlc;
    ae_matrix_init_copy(
        &dst->pointinfo, &src->pointinfo, _state, make_automatic);
  }

  void _mirbfvnsdataset_clear(void* _p) {
    mirbfvnsdataset* p = (mirbfvnsdataset*) _p;
    ae_touch_ptr((void*) p);
    ae_matrix_clear(&p->pointinfo);
  }

  void _mirbfvnsdataset_destroy(void* _p) {
    mirbfvnsdataset* p = (mirbfvnsdataset*) _p;
    ae_touch_ptr((void*) p);
    ae_matrix_destroy(&p->pointinfo);
  }

  void _mirbfvnsstate_init(void* _p, ae_state* _state, ae_bool make_automatic) {
    mirbfvnsstate* p = (mirbfvnsstate*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_init(&p->criteria, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->finitebndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->finitebndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->isintegral, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->isbinary, 0, DT_BOOL, _state, make_automatic);
    _sparsematrix_init(&p->rawa, _state, make_automatic);
    ae_vector_init(&p->rawal, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawau, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lcsrcidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->nl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasmask, 0, DT_BOOL, _state, make_automatic);
    _sparsematrix_init(&p->varmask, _state, make_automatic);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->reportx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->querydata, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replyfi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replydj, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->replysj, _state, make_automatic);
    ae_vector_init(&p->tmpx1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpc1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpf1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpg1, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpj1, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->tmps1, _state, make_automatic);
    _stimer_init(&p->timerglobal, _state, make_automatic);
    _stimer_init(&p->timerprepareneighbors, _state, make_automatic);
    _stimer_init(&p->timerproposetrial, _state, make_automatic);
    ae_vector_init(&p->xc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nodecproducedbycut, 0, DT_REAL, _state, make_automatic);
    _mirbfvnsgrid_init(&p->grid, _state, make_automatic);
    _mirbfvnsdataset_init(&p->dataset, _state, make_automatic);
    ae_vector_init(&p->xcneighbors, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->xcreachedfrom, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->xcreachedbycut, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xcqueryflags, 0, DT_BOOL, _state, make_automatic);
    ae_matrix_init(&p->evalbatchpoints, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->evalbatchnodeidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->evalbatchneighboridx, 0, DT_INT, _state, make_automatic);
    _hqrndstate_init(&p->unsafeglobalrng, _state, make_automatic);
    ae_vector_init(&p->maskint, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->maskfrac, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->idxint, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->idxfrac, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->xuneighbors, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->xucuts, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->xupoints, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xuflags, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->xtrial, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->trialfi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpeb0, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmpeb1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpeb2, 0, DT_INT, _state, make_automatic);
    _mirbfvnstemporaries_init(&p->dummytmp, _state, make_automatic);
    ae_matrix_init(&p->densedummy2, 0, 0, DT_REAL, _state, make_automatic);
    ae_nxpool_init(&p->rpool, DT_REAL, _state, make_automatic);
    ae_shared_pool_init(&p->tmppool, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
  }

  void _mirbfvnsstate_init_copy(void*       _dst,
                                const void* _src,
                                ae_state*   _state,
                                ae_bool     make_automatic) {
    mirbfvnsstate*       dst = (mirbfvnsstate*) _dst;
    const mirbfvnsstate* src = (const mirbfvnsstate*) _src;
    dst->n                   = src->n;
    _nlpstoppingcriteria_init_copy(
        &dst->criteria, &src->criteria, _state, make_automatic);
    dst->algomode                    = src->algomode;
    dst->budget                      = src->budget;
    dst->maxneighborhood             = src->maxneighborhood;
    dst->batchsize                   = src->batchsize;
    dst->expandneighborhoodonstart   = src->expandneighborhoodonstart;
    dst->retrylastcut                = src->retrylastcut;
    dst->convexityflag               = src->convexityflag;
    dst->ctol                        = src->ctol;
    dst->epsf                        = src->epsf;
    dst->quickepsf                   = src->quickepsf;
    dst->epsx                        = src->epsx;
    dst->adaptiveinternalparallelism = src->adaptiveinternalparallelism;
    dst->timeout                     = src->timeout;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(
        &dst->finitebndl, &src->finitebndl, _state, make_automatic);
    ae_vector_init_copy(
        &dst->finitebndu, &src->finitebndu, _state, make_automatic);
    ae_vector_init_copy(
        &dst->isintegral, &src->isintegral, _state, make_automatic);
    ae_vector_init_copy(&dst->isbinary, &src->isbinary, _state, make_automatic);
    _sparsematrix_init_copy(&dst->rawa, &src->rawa, _state, make_automatic);
    ae_vector_init_copy(&dst->rawal, &src->rawal, _state, make_automatic);
    ae_vector_init_copy(&dst->rawau, &src->rawau, _state, make_automatic);
    ae_vector_init_copy(&dst->lcsrcidx, &src->lcsrcidx, _state, make_automatic);
    dst->lccnt                      = src->lccnt;
    dst->haslinearlyconstrainedints = src->haslinearlyconstrainedints;
    dst->nnlc                       = src->nnlc;
    ae_vector_init_copy(&dst->nl, &src->nl, _state, make_automatic);
    ae_vector_init_copy(&dst->nu, &src->nu, _state, make_automatic);
    dst->nomask = src->nomask;
    ae_vector_init_copy(&dst->hasmask, &src->hasmask, _state, make_automatic);
    _sparsematrix_init_copy(
        &dst->varmask, &src->varmask, _state, make_automatic);
    dst->hasx0 = src->hasx0;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    dst->requesttype = src->requesttype;
    ae_vector_init_copy(&dst->reportx, &src->reportx, _state, make_automatic);
    dst->reportf          = src->reportf;
    dst->querysize        = src->querysize;
    dst->queryfuncs       = src->queryfuncs;
    dst->queryvars        = src->queryvars;
    dst->querydim         = src->querydim;
    dst->queryformulasize = src->queryformulasize;
    ae_vector_init_copy(
        &dst->querydata, &src->querydata, _state, make_automatic);
    ae_vector_init_copy(&dst->replyfi, &src->replyfi, _state, make_automatic);
    ae_vector_init_copy(&dst->replydj, &src->replydj, _state, make_automatic);
    _sparsematrix_init_copy(
        &dst->replysj, &src->replysj, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpx1, &src->tmpx1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpc1, &src->tmpc1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpf1, &src->tmpf1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpg1, &src->tmpg1, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpj1, &src->tmpj1, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmps1, &src->tmps1, _state, make_automatic);
    dst->userterminationneeded = src->userterminationneeded;
    dst->repnfev               = src->repnfev;
    dst->repsubsolverits       = src->repsubsolverits;
    dst->repiterationscount    = src->repiterationscount;
    dst->repterminationtype    = src->repterminationtype;
    _stimer_init_copy(
        &dst->timerglobal, &src->timerglobal, _state, make_automatic);
    _stimer_init_copy(&dst->timerprepareneighbors,
                      &src->timerprepareneighbors,
                      _state,
                      make_automatic);
    _stimer_init_copy(&dst->timerproposetrial,
                      &src->timerproposetrial,
                      _state,
                      make_automatic);
    dst->explorativetrialcnt             = src->explorativetrialcnt;
    dst->explorativetrialtimems          = src->explorativetrialtimems;
    dst->localtrialsamplingcnt           = src->localtrialsamplingcnt;
    dst->localtrialsamplingtimems        = src->localtrialsamplingtimems;
    dst->localtrialrbfcnt                = src->localtrialrbfcnt;
    dst->localtrialrbftimems             = src->localtrialrbftimems;
    dst->cutcnt                          = src->cutcnt;
    dst->cuttimems                       = src->cuttimems;
    dst->dbgpotentiallyparallelbatches   = src->dbgpotentiallyparallelbatches;
    dst->dbgsequentialbatches            = src->dbgsequentialbatches;
    dst->dbgpotentiallyparallelcutrounds = src->dbgpotentiallyparallelcutrounds;
    dst->dbgsequentialcutrounds          = src->dbgsequentialcutrounds;
    dst->prepareevaluationbatchparallelism
        = src->prepareevaluationbatchparallelism;
    dst->expandcutgenerateneighborsparallelism
        = src->expandcutgenerateneighborsparallelism;
    dst->doanytrace     = src->doanytrace;
    dst->dotrace        = src->dotrace;
    dst->doextratrace   = src->doextratrace;
    dst->dolaconictrace = src->dolaconictrace;
    ae_vector_init_copy(&dst->xc, &src->xc, _state, make_automatic);
    dst->fc    = src->fc;
    dst->mxc   = src->mxc;
    dst->hc    = src->hc;
    dst->nodec = src->nodec;
    ae_vector_init_copy(&dst->nodecproducedbycut,
                        &src->nodecproducedbycut,
                        _state,
                        make_automatic);
    _mirbfvnsgrid_init_copy(&dst->grid, &src->grid, _state, make_automatic);
    _mirbfvnsdataset_init_copy(
        &dst->dataset, &src->dataset, _state, make_automatic);
    dst->nfrac = src->nfrac;
    dst->nint  = src->nint;
    ae_vector_init_copy(
        &dst->xcneighbors, &src->xcneighbors, _state, make_automatic);
    ae_vector_init_copy(
        &dst->xcreachedfrom, &src->xcreachedfrom, _state, make_automatic);
    ae_matrix_init_copy(
        &dst->xcreachedbycut, &src->xcreachedbycut, _state, make_automatic);
    ae_vector_init_copy(
        &dst->xcqueryflags, &src->xcqueryflags, _state, make_automatic);
    dst->xcneighborscnt         = src->xcneighborscnt;
    dst->xcpriorityneighborscnt = src->xcpriorityneighborscnt;
    dst->evalbatchsize          = src->evalbatchsize;
    ae_matrix_init_copy(
        &dst->evalbatchpoints, &src->evalbatchpoints, _state, make_automatic);
    ae_vector_init_copy(
        &dst->evalbatchnodeidx, &src->evalbatchnodeidx, _state, make_automatic);
    ae_vector_init_copy(&dst->evalbatchneighboridx,
                        &src->evalbatchneighboridx,
                        _state,
                        make_automatic);
    dst->outofbudget = src->outofbudget;
    _hqrndstate_init_copy(
        &dst->unsafeglobalrng, &src->unsafeglobalrng, _state, make_automatic);
    ae_vector_init_copy(&dst->maskint, &src->maskint, _state, make_automatic);
    ae_vector_init_copy(&dst->maskfrac, &src->maskfrac, _state, make_automatic);
    ae_vector_init_copy(&dst->idxint, &src->idxint, _state, make_automatic);
    ae_vector_init_copy(&dst->idxfrac, &src->idxfrac, _state, make_automatic);
    ae_vector_init_copy(
        &dst->xuneighbors, &src->xuneighbors, _state, make_automatic);
    ae_matrix_init_copy(&dst->xucuts, &src->xucuts, _state, make_automatic);
    ae_matrix_init_copy(&dst->xupoints, &src->xupoints, _state, make_automatic);
    ae_vector_init_copy(&dst->xuflags, &src->xuflags, _state, make_automatic);
    ae_vector_init_copy(&dst->xtrial, &src->xtrial, _state, make_automatic);
    ae_vector_init_copy(&dst->trialfi, &src->trialfi, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpeb0, &src->tmpeb0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpeb1, &src->tmpeb1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpeb2, &src->tmpeb2, _state, make_automatic);
    _mirbfvnstemporaries_init_copy(
        &dst->dummytmp, &src->dummytmp, _state, make_automatic);
    ae_matrix_init_copy(
        &dst->densedummy2, &src->densedummy2, _state, make_automatic);
    ae_nxpool_init_copy(&dst->rpool, &src->rpool, _state, make_automatic);
    ae_shared_pool_init_copy(
        &dst->tmppool, &src->tmppool, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
  }

  void _mirbfvnsstate_clear(void* _p) {
    mirbfvnsstate* p = (mirbfvnsstate*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_clear(&p->criteria);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->finitebndl);
    ae_vector_clear(&p->finitebndu);
    ae_vector_clear(&p->isintegral);
    ae_vector_clear(&p->isbinary);
    _sparsematrix_clear(&p->rawa);
    ae_vector_clear(&p->rawal);
    ae_vector_clear(&p->rawau);
    ae_vector_clear(&p->lcsrcidx);
    ae_vector_clear(&p->nl);
    ae_vector_clear(&p->nu);
    ae_vector_clear(&p->hasmask);
    _sparsematrix_clear(&p->varmask);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->reportx);
    ae_vector_clear(&p->querydata);
    ae_vector_clear(&p->replyfi);
    ae_vector_clear(&p->replydj);
    _sparsematrix_clear(&p->replysj);
    ae_vector_clear(&p->tmpx1);
    ae_vector_clear(&p->tmpc1);
    ae_vector_clear(&p->tmpf1);
    ae_vector_clear(&p->tmpg1);
    ae_matrix_clear(&p->tmpj1);
    _sparsematrix_clear(&p->tmps1);
    _stimer_clear(&p->timerglobal);
    _stimer_clear(&p->timerprepareneighbors);
    _stimer_clear(&p->timerproposetrial);
    ae_vector_clear(&p->xc);
    ae_vector_clear(&p->nodecproducedbycut);
    _mirbfvnsgrid_clear(&p->grid);
    _mirbfvnsdataset_clear(&p->dataset);
    ae_vector_clear(&p->xcneighbors);
    ae_vector_clear(&p->xcreachedfrom);
    ae_matrix_clear(&p->xcreachedbycut);
    ae_vector_clear(&p->xcqueryflags);
    ae_matrix_clear(&p->evalbatchpoints);
    ae_vector_clear(&p->evalbatchnodeidx);
    ae_vector_clear(&p->evalbatchneighboridx);
    _hqrndstate_clear(&p->unsafeglobalrng);
    ae_vector_clear(&p->maskint);
    ae_vector_clear(&p->maskfrac);
    ae_vector_clear(&p->idxint);
    ae_vector_clear(&p->idxfrac);
    ae_vector_clear(&p->xuneighbors);
    ae_matrix_clear(&p->xucuts);
    ae_matrix_clear(&p->xupoints);
    ae_vector_clear(&p->xuflags);
    ae_vector_clear(&p->xtrial);
    ae_vector_clear(&p->trialfi);
    ae_vector_clear(&p->tmpeb0);
    ae_vector_clear(&p->tmpeb1);
    ae_vector_clear(&p->tmpeb2);
    _mirbfvnstemporaries_clear(&p->dummytmp);
    ae_matrix_clear(&p->densedummy2);
    ae_nxpool_clear(&p->rpool);
    ae_shared_pool_clear(&p->tmppool);
    _rcommstate_clear(&p->rstate);
  }

  void _mirbfvnsstate_destroy(void* _p) {
    mirbfvnsstate* p = (mirbfvnsstate*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_destroy(&p->criteria);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->finitebndl);
    ae_vector_destroy(&p->finitebndu);
    ae_vector_destroy(&p->isintegral);
    ae_vector_destroy(&p->isbinary);
    _sparsematrix_destroy(&p->rawa);
    ae_vector_destroy(&p->rawal);
    ae_vector_destroy(&p->rawau);
    ae_vector_destroy(&p->lcsrcidx);
    ae_vector_destroy(&p->nl);
    ae_vector_destroy(&p->nu);
    ae_vector_destroy(&p->hasmask);
    _sparsematrix_destroy(&p->varmask);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->reportx);
    ae_vector_destroy(&p->querydata);
    ae_vector_destroy(&p->replyfi);
    ae_vector_destroy(&p->replydj);
    _sparsematrix_destroy(&p->replysj);
    ae_vector_destroy(&p->tmpx1);
    ae_vector_destroy(&p->tmpc1);
    ae_vector_destroy(&p->tmpf1);
    ae_vector_destroy(&p->tmpg1);
    ae_matrix_destroy(&p->tmpj1);
    _sparsematrix_destroy(&p->tmps1);
    _stimer_destroy(&p->timerglobal);
    _stimer_destroy(&p->timerprepareneighbors);
    _stimer_destroy(&p->timerproposetrial);
    ae_vector_destroy(&p->xc);
    ae_vector_destroy(&p->nodecproducedbycut);
    _mirbfvnsgrid_destroy(&p->grid);
    _mirbfvnsdataset_destroy(&p->dataset);
    ae_vector_destroy(&p->xcneighbors);
    ae_vector_destroy(&p->xcreachedfrom);
    ae_matrix_destroy(&p->xcreachedbycut);
    ae_vector_destroy(&p->xcqueryflags);
    ae_matrix_destroy(&p->evalbatchpoints);
    ae_vector_destroy(&p->evalbatchnodeidx);
    ae_vector_destroy(&p->evalbatchneighboridx);
    _hqrndstate_destroy(&p->unsafeglobalrng);
    ae_vector_destroy(&p->maskint);
    ae_vector_destroy(&p->maskfrac);
    ae_vector_destroy(&p->idxint);
    ae_vector_destroy(&p->idxfrac);
    ae_vector_destroy(&p->xuneighbors);
    ae_matrix_destroy(&p->xucuts);
    ae_matrix_destroy(&p->xupoints);
    ae_vector_destroy(&p->xuflags);
    ae_vector_destroy(&p->xtrial);
    ae_vector_destroy(&p->trialfi);
    ae_vector_destroy(&p->tmpeb0);
    ae_vector_destroy(&p->tmpeb1);
    ae_vector_destroy(&p->tmpeb2);
    _mirbfvnstemporaries_destroy(&p->dummytmp);
    ae_matrix_destroy(&p->densedummy2);
    ae_nxpool_destroy(&p->rpool);
    ae_shared_pool_destroy(&p->tmppool);
    _rcommstate_destroy(&p->rstate);
  }

#endif
#if defined(AE_COMPILE_MINLPSOLVERS) || !defined(AE_PARTIAL_BUILD)

  /*************************************************************************
                  MIXED INTEGER NONLINEAR PROGRAMMING SOLVER

  DESCRIPTION:
  The  subroutine  minimizes a function  F(x)  of N arguments subject to any
  combination of:
  * box constraints
  * linear equality/inequality/range constraints CL<=Ax<=CU
  * nonlinear equality/inequality/range constraints HL<=Hi(x)<=HU
  * integrality constraints on some variables

  REQUIREMENTS:
  * F(), H() are continuously differentiable on the  feasible  set  and  its
    neighborhood
  * starting point X0, which can be infeasible

  INPUT PARAMETERS:
      N       -   problem dimension, N>0:
                  * if given, only leading N elements of X are used
                  * if not given, automatically determined from size ofX
      X       -   starting point, array[N]:
                  * it is better to set X to a feasible point
                  * but X can be infeasible, in which case algorithm will try
                    to find feasible point first, using X as initial
                    approximation.

  OUTPUT PARAMETERS:
      State   -   structure stores algorithm state

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolvercreate(ae_int_t                       n,
                         /* Real    */ const ae_vector* x,
                         minlpsolverstate*              state,
                         ae_state*                      _state) {
    _minlpsolverstate_clear(state);

    ae_assert(n >= 1, "MINLPSolverCreate: N<1", _state);
    ae_assert(x->cnt >= n, "MINLPSolverCreate: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state),
              "MINLPSolverCreate: X contains infinite or NaN values",
              _state);
    minlpsolvers_initinternal(n, x, 0, 0.0, state, _state);
  }

  /*************************************************************************
  This function sets box constraints for the mixed integer optimizer.

  Box constraints are inactive by default.

  IMPORTANT: box constraints work in parallel with the integrality ones:
             * a variable marked as integral is considered  having no bounds
               until minlpsolversetbc() is called
             * a  variable  with  lower  and  upper bounds set is considered
               continuous   until    marked    as    integral    with    the
               minlpsolversetintkth() function.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      BndL    -   lower bounds, array[N].
                  If some (all) variables are unbounded, you may  specify  a
                  very small number or -INF, with the  latter  option  being
                  recommended.
      BndU    -   upper bounds, array[N].
                  If some (all) variables are unbounded, you may  specify  a
                  very large number or +INF, with the  latter  option  being
                  recommended.

  NOTE 1:  it is possible to specify  BndL[i]=BndU[i].  In  this  case  I-th
           variable will be "frozen" at X[i]=BndL[i]=BndU[i].

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetbc(minlpsolverstate*              state,
                        /* Real    */ const ae_vector* bndl,
                        /* Real    */ const ae_vector* bndu,
                        ae_state*                      _state) {
    ae_int_t i;
    ae_int_t n;

    n = state->n;
    ae_assert(bndl->cnt >= n, "MINLPSolverSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt >= n, "MINLPSolverSetBC: Length(BndU)<N", _state);
    for (i = 0; i <= n - 1; i++) {
      ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)
                    || ae_isneginf(bndl->ptr.p_double[i], _state),
                "MINLPSolverSetBC: BndL contains NAN or +INF",
                _state);
      ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)
                    || ae_isposinf(bndu->ptr.p_double[i], _state),
                "MINLPSolverSetBC: BndL contains NAN or -INF",
                _state);
      state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
      state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
    }
  }

  /*************************************************************************
  This function sets two-sided linear constraints AL <= A*x <= AU with dense
  constraint matrix A.

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      A       -   linear constraints, array[K,N]. Each row of  A  represents
                  one  constraint. One-sided  inequality   constraints, two-
                  sided inequality  constraints,  equality  constraints  are
                  supported (see below)
      AL, AU  -   lower and upper bounds, array[K];
                  * AL[i]=AU[i] => equality constraint Ai*x
                  * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                  * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                  * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                  * AL[i]=-INF, AU[i]=+INF => constraint is ignored
      K       -   number of equality/inequality constraints,  K>=0;  if  not
                  given, inferred from sizes of A, AL, AU.

    -- ALGLIB --
       Copyright 15.04.2024 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetlc2dense(minlpsolverstate*              state,
                              /* Real    */ const ae_matrix* a,
                              /* Real    */ const ae_vector* al,
                              /* Real    */ const ae_vector* au,
                              ae_int_t                       k,
                              ae_state*                      _state) {
    xlcsetlc2mixed(&state->xlc, &state->rcommv2.tmps1, 0, a, k, al, au, _state);
  }

  /*************************************************************************
  This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
  a sparse constraining matrix A. Recommended for large-scale problems.

  This  function  overwrites  linear  (non-box)  constraints set by previous
  calls (if such calls were made).

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      A       -   sparse matrix with size [K,N] (exactly!).
                  Each row of A represents one general linear constraint.
                  A can be stored in any sparse storage format.
      AL, AU  -   lower and upper bounds, array[K];
                  * AL[i]=AU[i] => equality constraint Ai*x
                  * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                  * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                  * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                  * AL[i]=-INF, AU[i]=+INF => constraint is ignored
      K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                  is specified, A, AL, AU are ignored.

    -- ALGLIB --
       Copyright 15.04.2024 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetlc2(minlpsolverstate*              state,
                         const sparsematrix*            a,
                         /* Real    */ const ae_vector* al,
                         /* Real    */ const ae_vector* au,
                         ae_int_t                       k,
                         ae_state*                      _state) {
    xlcsetlc2mixed(&state->xlc, a, k, &state->rcommv2.tmpj1, 0, al, au, _state);
  }

  /*************************************************************************
  This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
  a mixed constraining matrix A including a sparse part (first SparseK rows)
  and a dense part (last DenseK rows). Recommended for large-scale problems.

  This  function  overwrites  linear  (non-box)  constraints set by previous
  calls (if such calls were made).

  This function may be useful if constraint matrix includes large number  of
  both types of rows - dense and sparse. If you have just a few sparse rows,
  you  may  represent  them  in  dense  format  without losing  performance.
  Similarly, if you have just a few dense rows, you may store them in sparse
  format with almost same performance.

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      SparseA -   sparse matrix with size [K,N] (exactly!).
                  Each row of A represents one general linear constraint.
                  A can be stored in any sparse storage format.
      SparseK -   number of sparse constraints, SparseK>=0
      DenseA  -   linear constraints, array[K,N], set of dense constraints.
                  Each row of A represents one general linear constraint.
      DenseK  -   number of dense constraints, DenseK>=0
      AL, AU  -   lower and upper bounds, array[SparseK+DenseK], with former
                  SparseK elements corresponding to sparse constraints,  and
                  latter DenseK elements corresponding to dense constraints;
                  * AL[i]=AU[i] => equality constraint Ai*x
                  * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                  * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                  * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                  * AL[i]=-INF, AU[i]=+INF => constraint is ignored
      K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                  is specified, A, AL, AU are ignored.

    -- ALGLIB --
       Copyright 15.04.2024 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetlc2mixed(minlpsolverstate*              state,
                              const sparsematrix*            sparsea,
                              ae_int_t                       ksparse,
                              /* Real    */ const ae_matrix* densea,
                              ae_int_t                       kdense,
                              /* Real    */ const ae_vector* al,
                              /* Real    */ const ae_vector* au,
                              ae_state*                      _state) {
    xlcsetlc2mixed(
        &state->xlc, sparsea, ksparse, densea, kdense, al, au, _state);
  }

  /*************************************************************************
  This function appends a two-sided linear constraint AL <= A*x <= AU to the
  matrix of dense constraints.

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      A       -   linear constraint coefficient, array[N], right side is NOT
                  included.
      AL, AU  -   lower and upper bounds;
                  * AL=AU    => equality constraint Ai*x
                  * AL<AU    => two-sided constraint AL<=A*x<=AU
                  * AL=-INF  => one-sided constraint Ai*x<=AU
                  * AU=+INF  => one-sided constraint AL<=Ai*x
                  * AL=-INF, AU=+INF => constraint is ignored

    -- ALGLIB --
       Copyright 15.04.2024 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolveraddlc2dense(minlpsolverstate*              state,
                              /* Real    */ const ae_vector* a,
                              double                         al,
                              double                         au,
                              ae_state*                      _state) {
    xlcaddlc2dense(&state->xlc, a, al, au, _state);
  }

  /*************************************************************************
  This function appends two-sided linear constraint  AL <= A*x <= AU  to the
  list of currently present sparse constraints.

  Constraint is passed in compressed format: as list of non-zero entries  of
  coefficient vector A. Such approach is more efficient than  dense  storage
  for highly sparse constraint vectors.

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      IdxA    -   array[NNZ], indexes of non-zero elements of A:
                  * can be unsorted
                  * can include duplicate indexes (corresponding entries  of
                    ValA[] will be summed)
      ValA    -   array[NNZ], values of non-zero elements of A
      NNZ     -   number of non-zero coefficients in A
      AL, AU  -   lower and upper bounds;
                  * AL=AU    => equality constraint A*x
                  * AL<AU    => two-sided constraint AL<=A*x<=AU
                  * AL=-INF  => one-sided constraint A*x<=AU
                  * AU=+INF  => one-sided constraint AL<=A*x
                  * AL=-INF, AU=+INF => constraint is ignored

    -- ALGLIB --
       Copyright 19.07.2018 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolveraddlc2(minlpsolverstate*              state,
                         /* Integer */ const ae_vector* idxa,
                         /* Real    */ const ae_vector* vala,
                         ae_int_t                       nnz,
                         double                         al,
                         double                         au,
                         ae_state*                      _state) {
    xlcaddlc2(&state->xlc, idxa, vala, nnz, al, au, _state);
  }

  /*************************************************************************
  This function appends two-sided linear constraint  AL <= A*x <= AU  to the
  list of currently present sparse constraints.

  Constraint vector A is  passed  as  a  dense  array  which  is  internally
  sparsified by this function.

  INPUT PARAMETERS:
      State   -   structure previously allocated with minlpsolvercreate() call.
      DA      -   array[N], constraint vector
      AL, AU  -   lower and upper bounds;
                  * AL=AU    => equality constraint A*x
                  * AL<AU    => two-sided constraint AL<=A*x<=AU
                  * AL=-INF  => one-sided constraint A*x<=AU
                  * AU=+INF  => one-sided constraint AL<=A*x
                  * AL=-INF, AU=+INF => constraint is ignored

    -- ALGLIB --
       Copyright 19.07.2018 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolveraddlc2sparsefromdense(minlpsolverstate*              state,
                                        /* Real    */ const ae_vector* da,
                                        double                         al,
                                        double                         au,
                                        ae_state*                      _state) {
    xlcaddlc2sparsefromdense(&state->xlc, da, al, au, _state);
  }

  /*************************************************************************
  This function sets two-sided nonlinear constraints for MINLP optimizer.

  In fact, this function sets  only  constraints  COUNT  and  their  BOUNDS.
  Constraints  themselves  (constraint  functions)   are   passed   to   the
  MINLPSolverOptimize() method as callbacks.

  MINLPSolverOptimize() method accepts a user-defined vector function F[] and
  its Jacobian J[], where:
  * first element of F[] and first row of J[] correspond to the target
  * subsequent NNLC components of F[] (and rows of J[]) correspond  to  two-
    sided nonlinear constraints NL<=C(x)<=NU, where
    * NL[i]=NU[i] => I-th row is an equality constraint Ci(x)=NL
    * NL[i]<NU[i] => I-th tow is a  two-sided constraint NL[i]<=Ci(x)<=NU[i]
    * NL[i]=-INF  => I-th row is an one-sided constraint Ci(x)<=NU[i]
    * NU[i]=+INF  => I-th row is an one-sided constraint NL[i]<=Ci(x)
    * NL[i]=-INF, NU[i]=+INF => constraint is ignored

  NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
        your problem has mixed constraints, you  may explicitly specify some
        of them as linear or box ones.
        It helps optimizer to handle them more efficiently.

  INPUT PARAMETERS:
      State   -   structure previously allocated with MINLPSolverCreate call.
      NL      -   array[NNLC], lower bounds, can contain -INF
      NU      -   array[NNLC], lower bounds, can contain +INF
      NNLC    -   constraints count, NNLC>=0

  NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
          possible that the algorithm will evaluate the function  outside of
          the feasible area!

  NOTE 2: algorithm scales variables  according  to the scale  specified by
          MINLPSolverSetScale()  function,  so it can handle problems with badly
          scaled variables (as long as we KNOW their scales).

          However,  there  is  no  way  to  automatically  scale   nonlinear
          constraints. Inappropriate scaling  of nonlinear  constraints  may
          ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
          is NOT the same as solving it with constraint "0.001*G0(x)=0".

          It means that YOU are  the  one who is responsible for the correct
          scaling of the nonlinear constraints Gi(x) and Hi(x). We recommend
          you to scale nonlinear constraints in such a way that the Jacobian
          rows have approximately unit magnitude  (for  problems  with  unit
          scale) or have magnitude approximately equal to 1/S[i] (where S is
          a scale set by MINLPSolverSetScale() function).

    -- ALGLIB --
       Copyright 05.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetnlc2(minlpsolverstate*              state,
                          /* Real    */ const ae_vector* nl,
                          /* Real    */ const ae_vector* nu,
                          ae_int_t                       nnlc,
                          ae_state*                      _state) {
    ae_int_t i;

    ae_assert(nnlc >= 0, "MINLPSolverSetNLC2: NNLC<0", _state);
    ae_assert(nl->cnt >= nnlc, "MINLPSolverSetNLC2: Length(NL)<NNLC", _state);
    ae_assert(nu->cnt >= nnlc, "MINLPSolverSetNLC2: Length(NU)<NNLC", _state);
    state->nnlc = nnlc;
    bsetallocv(nnlc, ae_false, &state->hasnlcmask, _state);
    sparsecreatecrsemptybuf(state->n, &state->nlcmask, _state);
    rallocv(nnlc, &state->nl, _state);
    rallocv(nnlc, &state->nu, _state);
    for (i = 0; i <= nnlc - 1; i++) {
      ae_assert(ae_isfinite(nl->ptr.p_double[i], _state)
                    || ae_isneginf(nl->ptr.p_double[i], _state),
                "MINLPSolverSetNLC2: NL[i] is +INF or NAN",
                _state);
      ae_assert(ae_isfinite(nu->ptr.p_double[i], _state)
                    || ae_isposinf(nu->ptr.p_double[i], _state),
                "MINLPSolverSetNLC2: NU[i] is -INF or NAN",
                _state);
      state->nl.ptr.p_double[i] = nl->ptr.p_double[i];
      state->nu.ptr.p_double[i] = nu->ptr.p_double[i];
      sparseappendemptyrow(&state->nlcmask, _state);
    }
  }

  /*************************************************************************
  This function APPENDS a two-sided nonlinear constraint to the list.

  In fact, this function adds constraint bounds.  A  constraints  itself  (a
  function) is passed to the MINLPSolverOptimize() method as a callback. See
  comments on  MINLPSolverSetNLC2()  for  more  information  about  callback
  structure.

  The function adds a two-sided nonlinear constraint NL<=C(x)<=NU, where
  * NL=NU => I-th row is an equality constraint Ci(x)=NL
  * NL<NU => I-th tow is a  two-sided constraint NL<=Ci(x)<=NU
  * NL=-INF  => I-th row is an one-sided constraint Ci(x)<=NU
  * NU=+INF  => I-th row is an one-sided constraint NL<=Ci(x)
  * NL=-INF, NU=+INF => constraint is ignored

  NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
        your problem has mixed constraints, you  may explicitly specify some
        of them as linear or box ones. It helps the optimizer to handle them
        more efficiently.

  INPUT PARAMETERS:
      State   -   structure previously allocated with MINLPSolverCreate call.
      NL      -   lower bound, can be -INF
      NU      -   upper bound, can be +INF

  NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
          possible that the algorithm will evaluate the function  outside of
          the feasible area!

  NOTE 2: algorithm scales variables  according  to the scale  specified by
          MINLPSolverSetScale()  function,  so it can handle problems with badly
          scaled variables (as long as we KNOW their scales).

          However,  there  is  no  way  to  automatically  scale   nonlinear
          constraints. Inappropriate scaling  of nonlinear  constraints  may
          ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
          is NOT the same as solving it with constraint "0.001*G0(x)=0".

          It means that YOU are  the  one who is responsible for the correct
          scaling of the nonlinear constraints Gi(x) and Hi(x). We recommend
          you to scale nonlinear constraints in such a way that the Jacobian
          rows have approximately unit magnitude  (for  problems  with  unit
          scale) or have magnitude approximately equal to 1/S[i] (where S is
          a scale set by MINLPSolverSetScale() function).

  NOTE 3: use addnlc2masked() in order to specify variable  mask.  Masks are
          essential  for  derivative-free  optimization because they provide
          important information about relevant and irrelevant variables.

    -- ALGLIB --
       Copyright 05.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolveraddnlc2(minlpsolverstate* state,
                          double            nl,
                          double            nu,
                          ae_state*         _state) {
    ae_assert(ae_isfinite(nl, _state) || ae_isneginf(nl, _state),
              "MINLPSolverAddNLC2: NL is +INF or NAN",
              _state);
    ae_assert(ae_isfinite(nu, _state) || ae_isposinf(nu, _state),
              "MINLPSolverAddNLC2: NU is -INF or NAN",
              _state);
    rgrowappendv(state->nnlc + 1, &state->nl, nl, _state);
    rgrowappendv(state->nnlc + 1, &state->nu, nu, _state);
    bgrowappendv(state->nnlc + 1, &state->hasnlcmask, ae_false, _state);
    sparseappendemptyrow(&state->nlcmask, _state);
    state->nnlc = state->nnlc + 1;
  }

  /*************************************************************************
  This function APPENDS a two-sided nonlinear constraint to the  list,  with
  the  variable   mask  being  specified  as  a  compressed  index  array. A
  variable mask is a set of variables actually appearing in the constraint.

  ----- ABOUT VARIABLE MASKS -----------------------------------------------

  Variable masks provide crucial information  for  derivative-free  solvers,
  greatly accelerating surrogate model construction. This  applies  to  both
  continuous and integral variables, with results for binary variables being
  more pronounced.

  Up to 2x improvement in convergence speed has been observed for sufficiently
  sparse MINLP problems.

  NOTE: In order to unleash the full potential of variable  masking,  it  is
        important to provide masks for objective as well  as  all  nonlinear
        constraints.

        Even partial  information  matters,  i.e.  if you are 100% sure that
        your black-box  function  does  not  depend  on  some variables, but
        unsure about other ones, mark surely irrelevant variables, and  tell
        the solver that other ones may be relevant.

  NOTE: the solver is may behave unpredictably  if  some  relevant  variable
        is not included into the mask. Most likely it will fail to converge,
        although it sometimes possible to converge  to  solution  even  with
        incorrectly specified mask.

  NOTE: minlpsolversetobjectivemask() can be used to set  variable  mask for
        the objective.

  NOTE: Masks  are  ignored  by  branch-and-bound-type  solvers  relying  on
        analytic gradients.

  ----- ABOUT NONLINEAR CONSTRAINTS ----------------------------------------

  In fact, this function adds constraint bounds.  A  constraint   itself  (a
  function) is passed to the MINLPSolverOptimize() method as a callback. See
  comments on  MINLPSolverSetNLC2()  for  more  information  about  callback
  structure.

  The function adds a two-sided nonlinear constraint NL<=C(x)<=NU, where
  * NL=NU => I-th row is an equality constraint Ci(x)=NL
  * NL<NU => I-th tow is a  two-sided constraint NL<=Ci(x)<=NU
  * NL=-INF  => I-th row is an one-sided constraint Ci(x)<=NU
  * NU=+INF  => I-th row is an one-sided constraint NL<=Ci(x)
  * NL=-INF, NU=+INF => constraint is ignored

  NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
        your problem has mixed constraints, you  may explicitly specify some
        of them as linear or box ones. It helps the optimizer to handle them
        more efficiently.

  INPUT PARAMETERS:
      State   -   structure previously allocated with MINLPSolverCreate call.
      NL      -   lower bound, can be -INF
      NU      -   upper bound, can be +INF
      VarIdx  -   array[NMSK], with potentially  unsorted  and  non-distinct
                  indexes (the function will sort and merge duplicates).  If
                  a variable index K appears in the list, it  means that the
                  constraint potentially depends  on  K-th  variable.  If  a
                  variable index K does NOT appear in  the  list,  it  means
                  that the constraint does NOT depend on K-th variable.
                  The array can have more than NMSK elements, in which  case
                  only leading NMSK will be used.
      NMSK    -   NMSK>=0, VarIdx[] size:
                  * NMSK>0 means that the constraint depends on up  to  NMSK
                    variables whose indexes are stored in VarIdx[]
                  * NMSK=0 means that the constraint is a constant function;
                    the solver may fail if it is not actually the case.

  NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
          possible that the algorithm will evaluate the function  outside of
          the feasible area!

  NOTE 2: algorithm scales variables  according  to the scale  specified by
          MINLPSolverSetScale()  function,  so it can handle problems with badly
          scaled variables (as long as we KNOW their scales).

          However,  there  is  no  way  to  automatically  scale   nonlinear
          constraints. Inappropriate scaling  of nonlinear  constraints  may
          ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
          is NOT the same as solving it with constraint "0.001*G0(x)=0".

          It means that YOU are  the  one who is responsible for the correct
          scaling of the nonlinear constraints Gi(x) and Hi(x). We recommend
          you to scale nonlinear constraints in such a way that the Jacobian
          rows have approximately unit magnitude  (for  problems  with  unit
          scale) or have magnitude approximately equal to 1/S[i] (where S is
          a scale set by MINLPSolverSetScale() function).

    -- ALGLIB --
       Copyright 05.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolveraddnlc2masked(minlpsolverstate*              state,
                                double                         nl,
                                double                         nu,
                                /* Integer */ const ae_vector* varidx,
                                ae_int_t                       nmsk,
                                ae_state*                      _state) {
    ae_assert(ae_isfinite(nl, _state) || ae_isneginf(nl, _state),
              "MINLPSolverAddNLC2Masked: NL is +INF or NAN",
              _state);
    ae_assert(ae_isfinite(nu, _state) || ae_isposinf(nu, _state),
              "MINLPSolverAddNLC2Masked: NU is -INF or NAN",
              _state);
    ae_assert(nmsk >= 0, "MINLPSolverAddNLC2Masked: NMSK<0", _state);
    ae_assert(varidx->cnt >= nmsk,
              "MINLPSolverAddNLC2Masked: len(VarIdx)<NMSK",
              _state);
    rgrowappendv(state->nnlc + 1, &state->nl, nl, _state);
    rgrowappendv(state->nnlc + 1, &state->nu, nu, _state);
    bgrowappendv(state->nnlc + 1, &state->hasnlcmask, ae_true, _state);
    rsetallocv(nmsk, 1.0, &state->rdummy, _state);
    sparseappendcompressedrow(
        &state->nlcmask, varidx, &state->rdummy, nmsk, _state);
    state->nnlc = state->nnlc + 1;
  }

  /*************************************************************************
  This function sets stopping condition for the branch-and-bound  family  of
  solvers: a solver must when when the gap between primal and dual bounds is
  less than PDGap.

  The solver computes relative gap, equal to |Fprim-Fdual|/max(|Fprim|,1).

  This parameter is ignored by other types of solvers, e.g. MIVNS.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      PDGap   -   >=0, tolerance. Zero value means that some default value
                  is automatically selected.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetpdgap(minlpsolverstate* state,
                           double            pdgap,
                           ae_state*         _state) {
    ae_assert(ae_isfinite(pdgap, _state),
              "MINLPSolverSetPDGap: PDGap is not finite",
              _state);
    ae_assert(ae_fp_greater_eq(pdgap, (double) (0)),
              "MINLPSolverSetPDGap: PDGap<0",
              _state);
    state->pdgap = pdgap;
  }

  /*************************************************************************
  This function sets BBSYNC profile to "small tree".

  It means that we expect our problem to have a shallow B&B  tree  with  the
  number of nodes comparable to the integer variables count, or below.

  BBSYNC solver will run with simplified settings:
  * pseudocost branching is used

  INPUT PARAMETERS:
      State   -   structure that stores algorithm state

    -- ALGLIB --
       Copyright 01.12.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetbbsyncprofilesmalltree(minlpsolverstate* state,
                                            ae_state*         _state) {
    state->bbsyncprofile = 0;
  }

  /*************************************************************************
  This function sets BBSYNC profile to "large tree".

  It means that we expect our problem to have a large  B&B  tree  with  much
  more than NInt (the integer variables count) nodes. However, we expect  it
  to be solvable within our computational budget (i.e. that we are  able  to
  explore the entire B&B tree).

  BBSYNC solver will run with heuristics that are  costly  to  power-up, but
  greatly improve performance on long distances:
  * reliability branching is used

  BBSYNC will not use  heuristics  that  increase  chance  of  finding  good
  solutions early at the cost of increasing total time to prove optimality.

  INPUT PARAMETERS:
      State   -   structure that stores algorithm state

    -- ALGLIB --
       Copyright 01.12.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetbbsyncprofilelargetree(minlpsolverstate* state,
                                            ae_state*         _state) {
    state->bbsyncprofile = 1;
  }

  /*************************************************************************
  This function tells branch-and-bound solvers to use nonlinear interior
  point method for continuous subproblems.

  This solver needs several times more function evaluations  than  SQP,  but
  has an order of magnitude smaller per-iteration linear algebra overhead.

  It is a recommended option for
  * large-scale problems, especially sparse ones
  * problems with many constraints (hence  high  linear  algebra  cost)  but
    relatively cheap objective/constraints evaluations


  INPUT PARAMETERS:
      State   -   structure that stores algorithm state
      MemLen  -   >=0, memory length for quasi-Newton update (similar to
                  LBFGS memory parameter):
                  * 0 means default value which may change in future versions;
                    presently it is 8.
                  * 8 is a good default value for moderately nonlinear tasks
                  * 32 is a good value for problems with more nonlinear
                    objective/constraints
                  * values larger than the variables count N  are  possible;
                    these will be silently truncated to N.

    -- ALGLIB --
       Copyright 01.02.2026 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetbbsyncsubsolveripm(minlpsolverstate* state,
                                        ae_int_t          memlen,
                                        ae_state*         _state) {
    ae_assert(
        memlen >= 0, "MINLPSolverSetBBSYNCSubsolverIPM: MemLen<0", _state);
    state->bbsyncsubsolver       = 0;
    state->bbsyncsubsolvermemlen = memlen;
  }

  /*************************************************************************
  This function tells branch-and-bound solvers to use SQP method for
  continuous subproblems.

  This solver needs several times less function evaluations  than  IPM,  but
  has an order of magnitude larger per-iteration  linear  algebra  overhead.
  Nevertheless, the solver is fully sparse-capable.

  It is a recommended option for:
  * problems with  relatively  expensive  objective/constraints  evaluations
    that outweigh additional expense  of  solving  QP  subproblems  at  each
    step

  INPUT PARAMETERS:
      State   -   structure that stores algorithm state

    -- ALGLIB --
       Copyright 01.02.2026 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetbbsyncsubsolversqp(minlpsolverstate* state,
                                        ae_state*         _state) {
    state->bbsyncsubsolver = 1;
  }

  /*************************************************************************
  This function sets tolerance for nonlinear constraints;  points  violating
  constraints by no more than CTol are considered feasible.

  Depending on the specific algorithm  used,  constraint  violation  may  be
  checked against  internally  scaled/normalized  constraints  (some  smooth
  solvers renormalize constraints in such a way that they have roughly  unit
  gradient magnitudes) or against raw constraint values:
  * BBSYNC renormalizes constraints prior to comparing them with CTol
  * MIRBF-VNS checks violation against raw constraint values

  IMPORTANT: one  should  be  careful  when choosing tolerances and stopping
             criteria.

             A solver stops  as  soon  as  stopping  criteria are triggered;
             a feasibility check is  performed  after  that.  If  too  loose
             stopping criteria are  used, the solver  may  fail  to  enforce
             constraints  with  sufficient  accuracy  and  fail to recognize
             solution as a feasible one.

             For example, stopping with EpsX=0.01 and checking CTol=0.000001
             will almost surely result in problems. Ideally, CTol should  be
             1-2 orders of magnitude more relaxed than stopping criteria.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      CTol    -   >0, tolerance.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetctol(minlpsolverstate* state,
                          double            ctol,
                          ae_state*         _state) {
    ae_assert(ae_isfinite(ctol, _state),
              "MINLPSolverSetCTol: CTol is not finite",
              _state);
    ae_assert(ae_fp_greater(ctol, (double) (0)),
              "MINLPSolverSetCTol: CTol<=0",
              _state);
    state->ctol = ctol;
  }

  /*************************************************************************
  This  function  tells  MINLP solver  to  use  an  objective-based stopping
  condition for an underlying subsolver, i.e. to stop subsolver if  relative
  change in objective between iterations is less than EpsF.

  Too tight EspF, as always, result in spending too much time in the solver.
  Zero value means that some default non-zero value will be used.

  Exact action of this condition as well as reaction  to  too  relaxed  EpsF
  depend on specific MINLP solver being used

  * BBSYNC. This condition controls SQP subsolver used to solve NLP (relaxed)
    subproblems arising during B&B  tree  search. Good  values are typically
    between 1E-6 and 1E-7.

    Too relaxed values may result in subproblems being  mistakenly  fathomed
    (feasible solutions not identified), too  large  constraint  violations,
    etc.

  * MIVNS. This condition controls RBF-based surrogate model subsolver  used
    to handle continuous variables. It is ignored for integer-only problems.

    The subsolver stops if total objective change in last  several  (between
    5 and 10) steps is less than EpsF. More than one step is used  to  check
    convergence because surrogate  model-based  solvers  usually  need  more
    stringent stopping criteria than SQP.

    Good values are relatively high, between 0.01 and 0.0001,  depending  on
    a  problem.  The  MIVNS  solver  is  designed to gracefully handle large
    values of EpsF - it will stop early, but it won't compromise feasibility
    (it will try to reduce constraint violations below CTol)  and  will  not
    drop promising integral nodes.

  INPUT PARAMETERS:
      State   -   solver structure
      EpsF    -   >0, stopping condition

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetsubsolverepsf(minlpsolverstate* state,
                                   double            epsf,
                                   ae_state*         _state) {
    ae_assert(ae_isfinite(epsf, _state),
              "MINLPSolverSetSubsolverEpsF: EpsF is not finite",
              _state);
    ae_assert(ae_fp_greater_eq(epsf, (double) (0)),
              "MINLPSolverSetSubsolverEpsF: EpsF<0",
              _state);
    state->subsolverepsf = epsf;
  }

  /*************************************************************************
  This  function  tells  MINLP solver to use a step-based stopping condition
  for an underlying subsolver, i.e. to stop subsolver  if  typical step size
  becomes less than EpsX.

  Too tight EspX, as always, result in spending too much time in the solver.
  Zero value means that some default non-zero value will be used.

  Exact action of this condition as well as reaction  to  too  relaxed  EpsX
  depend on specific MINLP solver being used

  * BBSYNC. This condition controls SQP subsolver used to solve NLP (relaxed)
    subproblems arising during B&B  tree  search. Good  values are typically
    between 1E-6 and 1E-7.

    Too relaxed values may result in subproblems being  mistakenly  fathomed
    (feasible solutions not identified), too  large  constraint  violations,
    etc.

  * MIVNS. This condition controls RBF-based surrogate model subsolver  used
    to handle continuous variables. It is ignored for integer-only problems.

    The subsolver stops if trust radius  for  a  surrogate  model  optimizer
    becomes less than EpsX.

    Good values are relatively high, between 0.01 and 0.0001,  depending  on
    a  problem.  The  MIVNS  solver  is  designed to gracefully handle large
    values of EpsX - it will stop early, but it won't compromise feasibility
    (it will try to reduce constraint violations below CTol)  and  will  not
    drop promising integral nodes.

  INPUT PARAMETERS:
      State   -   solver structure
      EpsX    -   >=0, stopping condition. Zero value means that some default
                  value will be used.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetsubsolverepsx(minlpsolverstate* state,
                                   double            epsx,
                                   ae_state*         _state) {
    ae_assert(ae_isfinite(epsx, _state),
              "MINLPSolverSetSubsolverEpsX: EpsX is not finite",
              _state);
    ae_assert(ae_fp_greater_eq(epsx, (double) (0)),
              "MINLPSolverSetSubsolverEpsX: EpsX<0",
              _state);
    state->subsolverepsx = epsx;
  }

  /*************************************************************************
  This function controls adaptive internal parallelism, i.e. algorithm  used
  by  the  solver  to  adaptively  decide  whether parallel acceleration  of
  solver's internal calculations (B&B  code,  SQP,  parallel linear algebra)
  should be actually used or not.

  This  function  tells  the  solver  to  favor  parallelism,  i.e.  utilize
  multithreading (when allowed by the  user)  until  statistics  prove  that
  overhead from starting/stopping worker threads is too large.

  This way solver gets the best performance  on  problems  with  significant
  amount  of  internal  calculations  (large  QP/MIQP  subproblems,  lengthy
  surrogate model optimization sessions) from the very beginning. The  price
  is that problems with small solver overhead that does not justify internal
  parallelism (<1ms per iteration) will suffer slowdown for several  initial
  10-20 milliseconds until the solver proves that parallelism makes no sense

  Use  MINLPSolver.CautiousInternalParallelism()  to  avoid slowing down the
  solver on easy problems.

  NOTE: the internal parallelism is distinct from the callback  parallelism.
        The former is the ability to utilize parallelism to speed-up solvers
        own internal calculations,  while  the  latter  is  the  ability  to
        perform several callback evaluations at once. Aside from performance
        considerations, the internal parallelism is entirely transparent  to
        the user. The callback parallelism requries  the  user  to  write  a
        thread-safe, reentrant callback.

  NOTE: in order to use internal parallelism, adaptive or not, the user must
        activate it by   specifying  alglib::parallel  in  flags  or  global
        threading settings. ALGLIB for C++ must be compiled in the  OS-aware
        mode.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolverfavorinternalparallelism(minlpsolverstate* state,
                                           ae_state*         _state) {
    state->adaptiveinternalparallelism = 1;
  }

  /*************************************************************************
  This function controls adaptive internal parallelism, i.e. algorithm  used
  by  the  solver  to  adaptively  decide  whether parallel acceleration  of
  solver's internal calculations (B&B  code,  SQP,  parallel linear algebra)
  should be actually used or not.

  This function tells the solver  to  do calculations in the single-threaded
  mode until statistics  prove  that  iteration  cost  justified  activating
  multithreading.

  This way solver does not suffer slow-down on problems with small iteration
  overhead (<1ms per iteration), at the cost of spending  initial  10-20  ms
  in the single-threaded  mode  even  on  difficult  problems  that  justify
  parallelism usage.

  Use  MINLPSolver.FavorInternalParallelism() to use parallelism until it is
  proven to be useless.

  NOTE: the internal parallelism is distinct from the callback  parallelism.
        The former is the ability to utilize parallelism to speed-up solvers
        own internal calculations,  while  the  latter  is  the  ability  to
        perform several callback evaluations at once. Aside from performance
        considerations, the internal parallelism is entirely transparent  to
        the user. The callback parallelism requries  the  user  to  write  a
        thread-safe, reentrant callback.

  NOTE: in order to use internal parallelism, adaptive or not, the user must
        activate it by   specifying  alglib::parallel  in  flags  or  global
        threading settings. ALGLIB for C++ must be compiled in the  OS-aware
        mode.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolvercautiousinternalparallelism(minlpsolverstate* state,
                                              ae_state*         _state) {
    state->adaptiveinternalparallelism = 0;
  }

  /*************************************************************************
  This function controls adaptive internal parallelism, i.e. algorithm  used
  by  the  solver  to  adaptively  decide  whether parallel acceleration  of
  solver's internal calculations (B&B  code,  SQP,  parallel linear algebra)
  should be actually used or not.

  This function tells the solver to do calculations exactly as prescribed by
  the user: in the parallel mode when alglib::parallel flag  is  passed,  in
  the single-threaded mode otherwise. The solver  does  not  analyze  actual
  running times to decide whether parallelism is justified or not.

  NOTE: the internal parallelism is distinct from the callback  parallelism.
        The former is the ability to utilize parallelism to speed-up solvers
        own internal calculations,  while  the  latter  is  the  ability  to
        perform several callback evaluations at once. Aside from performance
        considerations, the internal parallelism is entirely transparent  to
        the user. The callback parallelism requries  the  user  to  write  a
        thread-safe, reentrant callback.

  NOTE: in order to use internal parallelism, adaptive or not, the user must
        activate it by   specifying  alglib::parallel  in  flags  or  global
        threading settings. ALGLIB for C++ must be compiled in the  OS-aware
        mode.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolvernoadaptiveinternalparallelism(minlpsolverstate* state,
                                                ae_state*         _state) {
    state->adaptiveinternalparallelism = -1;
  }

  /*************************************************************************
  This function marks K-th variable as an integral one.

  Unless box constraints are set for the variable, it is unconstrained (i.e.
  can take positive or  negative  values).  By  default  all  variables  are
  continuous.

  IMPORTANT: box constraints work in parallel with the integrality ones:
             * a variable marked as integral is considered  having no bounds
               until minlpsolversetbc() is called
             * a  variable  with  lower  and  upper bounds set is considered
               continuous   until    marked    as    integral    with    the
               minlpsolversetintkth() function.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      K       -   0<=K<N, variable index

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetintkth(minlpsolverstate* state,
                            ae_int_t          k,
                            ae_state*         _state) {
    ae_assert(k >= 0 && k < state->n,
              "MINLPSolverSetIntKth: K is outside of [0,N)",
              _state);
    state->isintegral.ptr.p_bool[k] = ae_true;
    state->isbinary.ptr.p_bool[k]   = ae_false;
  }

  /*************************************************************************
  This function marks K-th variable as a linear one.

  A  linear  variable can appear in objective and all constraint types (box,
  linear and nonlinear), but the problem must be linear with respect to this
  variable.

  Knowning that some variables are linear  allows  the  solver  to  do avoid
  modelling nonlinearities associated with  these  variables  (corresponding
  rows/cols of a quasi-Newton Hessian will be zero), and, potentially, to do
  some otherwise unavailable reductions, decreasing linear algebra  overhead
  and improving convergence.

  By default all variables are nonlinear.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      K       -   0<=K<N, variable index

    -- ALGLIB --
       Copyright 01.03.2026 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolvermarkaslinearvar(minlpsolverstate* state,
                                  ae_int_t          k,
                                  ae_state*         _state) {
    ae_assert(k >= 0 && k < state->n,
              "MINLPSolverMarkAsLinearVar: K is outside of [0,N)",
              _state);
    state->islinear.ptr.p_bool[k] = ae_true;
  }

  /*************************************************************************
  This function sets variable  mask for the objective.  A variable  mask  is
  a set of variables actually appearing in the objective.

  If you want  to  set  variable  mask  for  a  nonlinear  constraint,   use
  addnlc2masked() or addnlc2maskeddense() to add  a constraint together with
  a constraint-specific mask.

  Variable masks provide crucial information  for  derivative-free  solvers,
  greatly accelerating surrogate model construction. This  applies  to  both
  continuous and integral variables, with results for binary variables being
  more pronounced.

  Up to 2x improvement in convergence speed has been observed for sufficiently
  sparse MINLP problems.

  NOTE: In order to unleash the full potential of variable  masking,  it  is
        important to provide masks for objective as well  as  all  nonlinear
        constraints.

        Even partial  information  matters,  i.e.  if you are 100% sure that
        your black-box  function  does  not  depend  on  some variables, but
        unsure about other ones, mark surely irrelevant variables, and  tell
        the solver that other ones may be relevant.

  NOTE: the solver is may behave unpredictably  if  some  relevant  variable
        is not included into the mask. Most likely it will fail to converge,
        although it sometimes possible to converge  to  solution  even  with
        incorrectly specified mask.

  NOTE: Masks  are  ignored  by  branch-and-bound-type  solvers  relying  on
        analytic gradients.

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      ObjMask -   array[N],  I-th  element  is  False  if  I-th variable  is
                  irrelevant.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetobjectivemaskdense(minlpsolverstate*              state,
                                        /* Boolean */ const ae_vector* _objmask,
                                        ae_state*                      _state) {
    ae_frame  _frame_block;
    ae_vector objmask;
    ae_int_t  i;

    ae_frame_make(_state, &_frame_block);
    memset(&objmask, 0, sizeof(objmask));
    ae_vector_init_copy(&objmask, _objmask, _state, ae_true);

    ae_assert(objmask.cnt >= state->n,
              "MINLPSolverSetObjectiveMaskDense: len(ObjMask)<N",
              _state);
    state->hasobjmask = ae_false;
    for (i = 0; i <= state->n - 1; i++) {
      state->hasobjmask = state->hasobjmask || !objmask.ptr.p_bool[i];
    }
    bcopyallocv(state->n, &objmask, &state->objmask, _state);
    ae_frame_leave(_state);
  }

  /*************************************************************************
  This function sets scaling coefficients for the mixed integer optimizer.

  ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
  size and gradient are scaled before comparison  with  tolerances).  Scales
  are also used by the finite difference variant of the optimizer - the step
  along I-th axis is equal to DiffStep*S[I]. Finally,  variable  scales  are
  used for preconditioning (i.e. to speed up the solver).

  The scale of the I-th variable is a translation invariant measure of:
  a) "how large" the variable is
  b) how large the step should be to make significant changes in the function

  INPUT PARAMETERS:
      State   -   structure stores algorithm state
      S       -   array[N], non-zero scaling coefficients
                  S[i] may be negative, sign doesn't matter.

    -- ALGLIB --
       Copyright 06.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetscale(minlpsolverstate*              state,
                           /* Real    */ const ae_vector* s,
                           ae_state*                      _state) {
    ae_int_t i;

    ae_assert(s->cnt >= state->n, "MINLPSolver: Length(S)<N", _state);
    for (i = 0; i <= state->n - 1; i++) {
      ae_assert(ae_isfinite(s->ptr.p_double[i], _state),
                "MINLPSolver: S contains infinite or NAN elements",
                _state);
      ae_assert(ae_fp_neq(s->ptr.p_double[i], (double) (0)),
                "MINLPSolver: S contains zero elements",
                _state);
      state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
  }

  /*************************************************************************
  This function tell the solver to use BBSYNC (Branch&Bound with Synchronous
  processing) mixed-integer nonlinear programming algorithm.

  The BBSYNC algorithm is an NLP-based branch-and-bound method with integral
  and spatial splits, supporting both convex  and  nonconvex  problems.  The
  algorithm combines parallelism support with deterministic  behavior  (i.e.
  the same branching decisions are performed with every parallel run).

  Non-convex (multiextremal) problems can be solved with  multiple  restarts
  from random points, which are activated by minlpsolversetmultistarts()

  IMPORTANT: contrary to the popular  misconception,  MINLP  is  not  easily
             parallelizable. B&B trees often have  profiles  unsuitable  for
             parallel processing (too short and/or too linear).  Spatial  or
             integral splits add  some limited degree of parallelism (up  to
             2x in the very best case), but in practice it often results  in
             just a 1.5x speed-up at best due to imbalanced  leaf processing
             times.  Furthermore,  determinism  is   always  at   odds  with
             efficiency.

             Achieving good parallel speed-up requires some amount of tuning
             and having a 2x-3x speed-up is  already  a  good  result.  Only
             difficult long-running problems (here  'difficult'  means  that
             the value of rep.ntreenodes is at least several larger than the
             variables count) have good parallelism properties.

             On the other hand, setups using multiple  random  restarts  are
             obviously highly parallelizable.

  IMPORTANT: the commercial edition of ALGLIB can  accelerate  factorization
             phase of this function (this phase takes significant amounts of
             time when solving large problems) by using SIMD intrinsics or a
             performance  backend  library   (Intel   PARDISO   or   another
             platform-specific sparse factorization library).

             Specific speed-up due  to  performance  backend  usage  heavily
             depends  on  the  sparsity  pattern  of  constraints.  For some
             problem types performance backends provide great speed-up.  For
             other ones, ALGLIB's  own  sparse  factorization  code  is  the
             preferred option.

             See the ALGLIB Reference Manual for more information on how  to
             activate parallelism and backend support.


  INPUT PARAMETERS:
      State           -   structure that stores algorithm state

      GroupSize       -   >=1, group size. Up to GroupSize tree nodes can be
                          processed in the parallel manner.

                          Increasing  this   parameter   makes   the  solver
                          less efficient serially (it always tries  to  fill
                          the batch with nodes, even if there  is  a  chance
                          that most of them will be  discarded  later),  but
                          increases its parallel potential.

                          Parallel speed-up comes from two sources:
                          * callback parallelism (several  objective  values
                            are computed concurrently), which is significant
                            for problems with callbacks that take  more than
                            1ms per evaluation
                          * internal parallelism, i.e. ability to do parallel
                            sparse matrix factorization  and  other  solver-
                            related tasks
                          By  default,  the  solver  runs  serially even for
                          GroupSize>1. Both kinds of parallelism have to  be
                          activated by the user, see ALGLIB Reference Manual
                          for more information.

                          Recommended value, depending on callback cost  and
                          matrix factorization overhead, can be:
                          * 1 for 'easy' problems with cheap  callbacks  and
                            small dimensions; also for problems with  nearly
                            linear B&B trees.
                          * 2-3  for   problems   with  sufficiently  costly
                            callbacks (or sufficiently high  linear  algebra
                            overhead) that it makes sense to utilize limited
                            parallelism.
                          * cores count - for difficult problems  with  deep
                            and  wide   B&B trees  and  sufficiently  costly
                            callbacks (or sufficiently high  linear  algebra
                            overhead).

  NOTES: DETERMINISM

  Running with fixed GroupSize generally produces same results independently
  of whether parallelism is used or not. Changing  GroupSize  parameter  may
  change results in the following ways:

  * for problems that are solved to optimality  but have multiple solutions,
    different values of this parameter may  result  in  different  solutions
    being returned (but still with the same objective value)

  * while operating close to exhausting budget (either timeout or iterations
    limit), different GroupSize values may result in different  outcomes:  a
    solution being found, or budget being exhausted

  * finally, on difficult problems that are too hard to solve to  optimality
    but still allow finding primal feasible solutions changing GroupSize may
    result in different primal feasible solutions being returned.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetalgobbsync(minlpsolverstate* state,
                                ae_int_t          groupsize,
                                ae_state*         _state) {
    ae_assert(groupsize >= 1, "MINLPSolverSetAlgoBBSYNC: GroupSize<1", _state);
    state->algoidx       = 0;
    state->bbgdgroupsize = groupsize;
  }

  /*************************************************************************
  This function  tell  the  solver  to  use  MIVNS  (Mixed-Integer  Variable
  Neighborhood Search) solver for  derivative-free  mixed-integer  nonlinear
  programming with expensive objective/constraints and non-relaxable integer
  variables.

  The solver is intended for moderately-sized problems, typically with  tens
  of variables.

  The algorithm has the following features:
  * it supports all-integer and mixed-integer problems with box, linear  and
    nonlinear equality and inequality  constraints
  * it makes no assumptions about problem convexity
  * it does not require derivative information. Although  it  still  assumes
    that objective/constraints are smooth wrt continuous variables, no  such
    assumptions are made regarding dependence on integer variables.
  * it efficiently uses limited computational budget and  scales  well  with
    larger budgets
  * it does not evaluate objective/constraints at points violating integrality
  * it also respects linear constraints in all intermediate points

  NOTE: In  particular,  if  your  task  uses integrality+sum-to-one set  of
        constraints to encode multiple choice options (e.g. [1,0,0], [0,1,0]
        or [0,0,1]), you can be sure that the algorithm will not ask for  an
        objective value at a point with fractional values like [0.1,0.5,0.4]
        or at one that is not a correct one-hot encoded value (e.g.  [1,1,0]
        which has two variables set to 1).

  The algorithm is intended for low-to-medium accuracy solution of otherwise
  intractable problems with expensive objective/constraints.

  It can solve any MINLP problem; however, it is optimized for the following
  problem classes:
  * limited variable count
  * expensive objective/constraints
  * nonrelaxable integer variables
  * no derivative information
  * problems where changes in integer variables lead to  structural  changes
    in the entire system. Speaking in other words, on  problems  where  each
    integer variable acts as an on/off or "choice"  switch  that  completely
    rewires the model - turning constraints, variables, or whole sub-systems
    on or off

  INPUT PARAMETERS:
      State           -   structure that stores algorithm state

      Budget          -   optimization  budget (function  evaluations).  The
                          solver will not stop  immediately  after  reaching
                          Budget evaluations, but  will  stop  shortly after
                          that (usually within 2N+1 evaluations). Zero value
                          means no limit.

      MaxNeighborhood -   stopping condition for the solver.  The  algorithm
                          will stop as soon as there are  no  points  better
                          than the current candidate in a neighborhood whose
                          size is equal to or exceeds MaxNeighborhood.  Zero
                          means no stopping condition.

                          Recommended neighborhood size is  proportional  to
                          the difference between integral variables count NI
                          and the number of linear equality  constraints  on
                          integral variables L (such constraints effectively
                          reduce problem dimensionality).

                          The very minimal value for binary problems is NI-L,
                          which means that the solution can not be  improved
                          by flipping one of variables between 0 and 1.  The
                          very minimal value for non-binary integral vars is
                          twice as much (because  now  each  point  has  two
                          neighbors per  variable).  However,  such  minimal
                          values often result in an early termination.

                          It is recommended to set this parameter to 5*N  or
                          10*N (ignoring LI) and to test how it  behaves  on
                          your problem.

      BatchSize           >=1,   recommended  batch  size  for  neighborhood
                          exploration.   Up   to  BatchSize  nodes  will  be
                          evaluated at any  moment,  thus  up  to  BatchSize
                          objective evaluations can be performed in parallel.

                          Increasing  this   parameter   makes   the  solver
                          slightly less efficient serially (it always  tries
                          to fill the batch with nodes, even if there  is  a
                          chance that most of them will be discarded later),
                          but greatly increases its parallel potential.

                          Recommended values depend on the cores  count  and
                          on the limitations  of  the  objective/constraints
                          callback:
                          * 1 for serial execution, callback that can not be
                            called  from   multiple   threads,   or   highly
                            parallelized  expensive  callback that keeps all
                            cores occupied
                          * small fixed value like 5  or  10,  if  you  need
                            reproducible behavior independent from the cores
                            count
                          * CORESCOUNT, 2*CORESCOUNT or some other  multiple
                            of CORESCOUNT, if you want to utilize parallelism
                            to the maximum extent

                          Parallel speed-up comes from two sources:
                          * callback parallelism (several  objective  values
                            are computed concurrently), which is significant
                            for problems with callbacks that take  more than
                            1ms per evaluation
                          * internal parallelism, i.e. ability to do parallel
                            sparse matrix factorization  and  other  solver-
                            related tasks
                          By  default,  the  solver  runs  serially even for
                          GroupSize>1. Both kinds of parallelism have to  be
                          activated by the user, see ALGLIB Reference Manual
                          for more information.

  NOTES: if no stopping criteria is specified (unlimited budget, no timeout,
         no  neighborhood  size  limit),  then  the  solver  will  run until
         enumerating all integer solutions.

  ===== ALGORITHM DESCRIPTION ==============================================

  A simplified description for an  all-integer  algorithm, omitting stopping
  criteria and various checks:

      MIVNS (ALL-INTEGER):
          1. Input: initial integral point, may be infeasible wrt  nonlinear
             constraints, but is feasible wrt linear  ones.  Enforce  linear
             feasibility, if needed.
          2. Generate initial neighborhood around the current point that  is
             equal to the point itself. The point is marked as explored.
          3. Scan  neighborhood  for  a  better  point  (one  that  is  less
             infeasible or has lower objective);  if  one  is found, make it
             current and goto #2
          4. Scan neighborhood for an unexplored point (one with no objective
             computed). If one if found, compute objective, mark the point as
             explored, goto #3
          5. If there are no unexplored or better points in the neighborhood,
             expand it: find a  point  that  was  not  used  for  expansion,
             compute up to 2N its nearest integral neighbors,  add  them  to
             the neighborhood and mark as unexplored. Goto #3.

      NOTE: A nearest integral neighbor is a nearest point that  differs  at
            least by +1 or -1 in one  of  integral  variables  and  that  is
            feasible with respect to box and  linear  constraints  (ignoring
            nonlinear ones). For problems  with  difficult  constraint  sets
            integral neighbors are found by solving MIQP subproblems.

  The algorithm above systematically scans neighborhood  of  a  point  until
  either better point is found, an entire integer grid is enumerated, or one
  of stopping conditions is met.

  A mixed-integer version of the algorithm is more complex:
  * it still sees optimization space as a set of integer  nodes,  each  node
    having a subspace of continuous variables associated with it
  * after starting to explore a node, the algorithm runs an  RBF  surrogate-
    based subsolver for the node. It manages a dedicated subsolver for  each
    node in a neighborhood and adaptively divides its  computational  budget
    between subsolvers, switching to a node as soon as its  subsolver  shows
    better results than its competitors.
  * the algorithm remembers all previously evaluated points and reuses  them
    as much as possible

  ===== ALGORITHM SCALING WITH VARIABLES COUNT N ===========================

  A 'neighborhood scan' is a minimum number of function evaluations   needed
  to perform at least minimal evaluation of the immediate  neighborhood. For
  an N-dimensional problem with NI  integer variables and NF continuous ones
  we have ~NI nodes in an immediate neighborhood, and each  node  needs  ~NF
  evalutations to build at least linear model of the objective.

  Thus, a MIVNS neighborhood scan will need  about NI*NF=NI*(N-NI)=NF*(N-NF)
  objective evaluations.

  It is important to note that MIVNS  does  not  share  information  between
  nodes because it assumes that objective landscape can  drastically  change
  when jumping from node to node. That's why we need  NI*NF instead of NI+NF
  objective values.

  In practice, when started not too far away from the minimum, we can expect
  to get some improvement in 5-10 scans, and to get significant progress  in
  50-100 scans.

  For problems with NF being small or NI  being  small  we  have  scan  cost
  being proportional to variables count N, which allows us to  achieve  good
  progress using between 5N and 100N objective values.  However,  when  both
  NI and NF are close to N/2,  a  scan  needs  ~N^2  objective  evaluations,
  which results in a much worse scaling behavior.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetalgomivns(minlpsolverstate* state,
                               ae_int_t          budget,
                               ae_int_t          maxneighborhood,
                               ae_int_t          batchsize,
                               ae_state*         _state) {
    ae_assert(budget >= 0, "MINLPSolverSetAlgoMIVNS: Budget<0", _state);
    ae_assert(maxneighborhood >= 0,
              "MINLPSolverSetAlgoMIVNS: MaxNeighborhood<0",
              _state);
    ae_assert(batchsize >= 1, "MINLPSolverSetAlgoMIVNS: BatchSize<1", _state);
    state->algoidx                 = 1;
    state->mirbfvnsalgo            = 0;
    state->mirbfvnsbudget          = budget;
    state->mirbfvnsmaxneighborhood = maxneighborhood;
    state->mirbfvnsbatchsize       = batchsize;
  }

  /*************************************************************************
  This function activates multiple random restarts (performed for each node,
  including root and child ones) that help to find global solutions to  non-
  convex problems.

  This parameter is used  by  branch-and-bound  solvers  and  is  presently
  ignored by derivative-free solvers.

  INPUT PARAMETERS:
      State           -   structure that stores algorithm state
      NMultistarts    -   >=1, number of random restarts:
                          * 1 means that no restarts performed, the solver
                            assumes convexity
                          * >=1 means that solver assumes non-convexity and
                            performs fixed amount of random restarts

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversetmultistarts(minlpsolverstate* state,
                                 ae_int_t          nmultistarts,
                                 ae_state*         _state) {
    ae_assert(
        nmultistarts >= 1, "MINLPSolverSetMultistarts: NMultistarts<1", _state);
    state->nmultistarts = nmultistarts;
  }

  /*************************************************************************
  This function activates timeout feature. The solver finishes after running
  for a specified amount of time (in seconds, fractions can  be  used)  with
  the best point so far.

  Depending on the situation, the following completion codes can be reported
  in rep.terminationtype:
  * -33 (failure), if timed out without finding a feasible point
  * 5 (partial success), if timed out after finding at least one feasible point

  The solver does not stop immediately after timeout was  triggered  because
  it needs some time for underlying subsolvers to react to  timeout  signal.
  Generally, about one additional subsolver iteration (which is usually  far
  less than one B&B split) will be performed prior to stopping.

  INPUT PARAMETERS:
      State           -   structure that stores algorithm state
      Timeout         -   >=0, timeout in seconds (floating point number):
                          * 0 means no timeout
                          * >=0 means stopping after specified number of
                            seconds.

    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolversettimeout(minlpsolverstate* state,
                             double            timeout,
                             ae_state*         _state) {
    ae_assert(ae_isfinite(timeout, _state)
                  && ae_fp_greater_eq(timeout, (double) (0)),
              "MINLPSolverSetTimeout: Timeout<0 or is infinite/NAN",
              _state);
    state->timeout = ae_iceil((double) 1000 * timeout, _state);
  }

  /*************************************************************************


    -- ALGLIB --
       Copyright 01.01.2025 by Bochkanov Sergey
  *************************************************************************/
  ae_bool minlpsolveriteration(minlpsolverstate* state, ae_state* _state) {
    ae_int_t n;
    ae_int_t i;
    ae_int_t k;
    ae_int_t originalrequesttype;
    ae_bool  done;
    ae_bool  densejac;
    ae_bool  b;
    ae_bool  result;

    /*
     * Reverse communication preparations
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if (state->rcommv2.stage >= 0) {
      n                   = state->rcommv2.ia.ptr.p_int[0];
      i                   = state->rcommv2.ia.ptr.p_int[1];
      k                   = state->rcommv2.ia.ptr.p_int[2];
      originalrequesttype = state->rcommv2.ia.ptr.p_int[3];
      done                = state->rcommv2.ba.ptr.p_bool[0];
      densejac            = state->rcommv2.ba.ptr.p_bool[1];
      b                   = state->rcommv2.ba.ptr.p_bool[2];
    } else {
      n                   = 359;
      i                   = -58;
      k                   = -919;
      originalrequesttype = -909;
      done                = ae_true;
      densejac            = ae_true;
      b                   = ae_false;
    }
    if (state->rcommv2.stage == 0) {
      goto lbl_0;
    }
    if (state->rcommv2.stage == 1) {
      goto lbl_1;
    }

    /*
     * Routine body
     */
    ae_assert(
        state->hasx0, "MINLPSolver: integrity check 500655 failed", _state);
    n = state->n;
    minlpsolvers_clearoutputs(state, _state);
    xlcconverttosparse(&state->xlc, _state);
    state->tracelevel = 0;
    if (ae_is_trace_enabled("MINLP")) {
      state->tracelevel = 2;
    }
    if (ae_is_trace_enabled("MINLP.LACONIC")) {
      state->tracelevel = 1;
    }

    /*
     * Initial trace messages
     */
    if (state->tracelevel == 1) {
      ae_trace(
          "------------------------------------------------------------\n");
      ae_trace("> Problem parameters:\n");
      k = 0;
      for (i = 0; i <= n - 1; i++) {
        if (state->isintegral.ptr.p_bool[i]) {
          k = k + 1;
        }
      }
      ae_trace(
          "* %0d variables, including %0d integral\n", (int) (n), (int) (k));
      k = 0;
      for (i = 0; i <= n - 1; i++) {
        if (state->islinear.ptr.p_bool[i]) {
          k = k + 1;
        }
      }
      ae_trace("* %0d variables are linear\n", (int) (k));
      ae_trace("* %0d constraints, including %0d linear and %0d nonlinear\n",
               (int) (state->xlc.nsparse + state->xlc.ndense + state->nnlc),
               (int) (state->xlc.nsparse + state->xlc.ndense),
               (int) (state->nnlc));
      if (state->algoidx == 0) {
        ae_trace("> BBSYNC solver parameters:\n");
        ae_trace("* batch size %0d\n", (int) (state->bbgdgroupsize));
        if (state->nmultistarts > 1) {
          ae_trace("* %0d random restarts\n", (int) (state->nmultistarts));
        }
        if (state->timeout > 0) {
          ae_trace("* timeout is %0.1fs\n",
                   (double) (0.001 * (double) state->timeout));
        } else {
          ae_trace("* no timeout\n");
        }
      }
      if (state->algoidx == 1) {
        ae_trace("> printing MIVNS solver parameters:\n");
        if (state->mirbfvnsbudget > 0) {
          ae_trace("Budget        = %6d\n", (int) (state->mirbfvnsbudget));
        } else {
          ae_trace("Budget        =    inf\n");
        }
        if (state->mirbfvnsmaxneighborhood > 0) {
          ae_trace("MaxNeighbors  = %6d\n",
                   (int) (state->mirbfvnsmaxneighborhood));
        } else {
          ae_trace("MaxNeighbors  =    inf\n");
        }
        ae_trace("BatchSize     = %6d\n", (int) (state->mirbfvnsbatchsize));
        if (state->timeout > 0) {
          ae_trace("Timeout       = %0.1fs\n",
                   (double) (0.001 * (double) state->timeout));
        } else {
          ae_trace("Timeout       = none\n");
        }
      }
      ae_trace(
          "------------------------------------------------------------\n");
      if (state->algoidx == 0) {
        ae_trace("> Starting BBSYNC solver\n");
      }
    }
    if (state->tracelevel >= 2) {
      ae_trace("\n\n");
      ae_trace("///////////////////////////////////////////////////////////////"
               "/////////////////////////////////////\n");
      ae_trace("//  MINLP SOLVER STARTED                                       "
               "                                   //\n");
      ae_trace("///////////////////////////////////////////////////////////////"
               "/////////////////////////////////////\n");
      ae_trace("N             = %6d\n", (int) (n));
      ae_trace("cntLC         = %6d\n",
               (int) (state->xlc.nsparse + state->xlc.ndense));
      ae_trace("cntNLC        = %6d\n", (int) (state->nnlc));
      k = 0;
      for (i = 0; i <= n - 1; i++) {
        if (state->isintegral.ptr.p_bool[i]) {
          k = k + 1;
        }
      }
      ae_trace("nIntegral     = %6d vars", (int) (k));
      k = 0;
      for (i = 0; i <= n - 1; i++) {
        if (state->isbinary.ptr.p_bool[i]) {
          k = k + 1;
        }
      }
      if (k > 0) {
        ae_trace(" (incl. %0d binary ones)", (int) (k));
      }
      ae_trace("\n");
      k = 0;
      for (i = 0; i <= n - 1; i++) {
        if (state->islinear.ptr.p_bool[i]) {
          k = k + 1;
        }
      }
      ae_trace("nLinear       = %6d vars\n", (int) (k));
      if (state->algoidx == 0) {
        ae_trace("> printing BBSYNC solver parameters:\n");
        ae_trace("GroupSize     = %6d\n", (int) (state->bbgdgroupsize));
        ae_trace("Multistarts   = %6d\n", (int) (state->nmultistarts));
        if (state->timeout > 0) {
          ae_trace("Timeout       = %0.1fs\n",
                   (double) (0.001 * (double) state->timeout));
        } else {
          ae_trace("Timeout       = none\n");
        }
      }
      if (state->algoidx == 1) {
        ae_trace("> printing MIVNS solver parameters:\n");
        if (state->mirbfvnsbudget > 0) {
          ae_trace("Budget        = %6d\n", (int) (state->mirbfvnsbudget));
        } else {
          ae_trace("Budget        =    inf\n");
        }
        if (state->mirbfvnsmaxneighborhood > 0) {
          ae_trace("MaxNeighbors  = %6d\n",
                   (int) (state->mirbfvnsmaxneighborhood));
        } else {
          ae_trace("MaxNeighbors  =    inf\n");
        }
        ae_trace("BatchSize     = %6d\n", (int) (state->mirbfvnsbatchsize));
        if (state->timeout > 0) {
          ae_trace("Timeout       = %0.1fs\n",
                   (double) (0.001 * (double) state->timeout));
        } else {
          ae_trace("Timeout       = none\n");
        }
      }
      ae_trace("\n");
    }

    /*
     * Init the solver
     */
    done = ae_false;
    if (state->algoidx == 0) {
      if (!(state->bbgdsubsolver != NULL)) {
        state->bbgdsubsolver = (bbgdstate*) ae_malloc(
            sizeof(bbgdstate),
            _state); /* note: using state->bbgdsubsolver as a temporary prior to
                        assigning its value to _state->bbgdsubsolver */
        memset(state->bbgdsubsolver, 0, sizeof(bbgdstate));
        _bbgdstate_init(state->bbgdsubsolver, _state, ae_false);
        ae_smart_ptr_assign(&state->_bbgdsubsolver,
                            state->bbgdsubsolver,
                            ae_true,
                            ae_true,
                            (ae_int_t) sizeof(bbgdstate),
                            _bbgdstate_init_copy,
                            _bbgdstate_destroy);
      }
      bbgdcreatebuf(n,
                    &state->bndl,
                    &state->bndu,
                    &state->s,
                    &state->x0,
                    &state->isintegral,
                    &state->isbinary,
                    &state->islinear,
                    &state->xlc.effsparsea,
                    &state->xlc.effal,
                    &state->xlc.effau,
                    &state->xlc.lcsrcidx,
                    state->xlc.nsparse + state->xlc.ndense,
                    &state->nl,
                    &state->nu,
                    state->nnlc,
                    state->issuesparserequests,
                    state->bbgdgroupsize,
                    state->nmultistarts,
                    state->timeout,
                    state->tracelevel,
                    state->bbgdsubsolver,
                    _state);
      if (ae_fp_greater(state->pdgap, (double) (0))) {
        bbgdsetpdgap(state->bbgdsubsolver, state->pdgap, _state);
      }
      if (ae_fp_greater(state->ctol, (double) (0))) {
        bbgdsetctol(state->bbgdsubsolver, state->ctol, _state);
      }
      if (ae_fp_greater(state->subsolverepsx, (double) (0))) {
        bbgdsetepsx(state->bbgdsubsolver, state->subsolverepsx, _state);
      }
      if (ae_fp_greater(state->subsolverepsf, (double) (0))) {
        bbgdsetepsf(state->bbgdsubsolver, state->subsolverepsf, _state);
      }
      if (state->bbsyncprofile == 0) {
        bbgdsetsmalltree(state->bbgdsubsolver, _state);
      }
      if (state->bbsyncprofile == 1) {
        bbgdsetlargetree(state->bbgdsubsolver, _state);
      }
      if (state->bbsyncsubsolver == 0) {
        bbgdsetipm(state->bbgdsubsolver, state->bbsyncsubsolvermemlen, _state);
      }
      if (state->bbsyncsubsolver == 1) {
        bbgdsetsqp(state->bbgdsubsolver, _state);
      }
      _rcommstate_forward_handler(&state->rcommv2,
                                  &state->bbgdsubsolver->rcommv2);
      done = ae_true;
    }
    if (state->algoidx == 1) {
      if (!(state->mirbfvnssubsolver != NULL)) {
        state->mirbfvnssubsolver = (mirbfvnsstate*) ae_malloc(
            sizeof(mirbfvnsstate),
            _state); /* note: using state->mirbfvnssubsolver as a temporary
                        prior to assigning its value to
                        _state->mirbfvnssubsolver */
        memset(state->mirbfvnssubsolver, 0, sizeof(mirbfvnsstate));
        _mirbfvnsstate_init(state->mirbfvnssubsolver, _state, ae_false);
        ae_smart_ptr_assign(&state->_mirbfvnssubsolver,
                            state->mirbfvnssubsolver,
                            ae_true,
                            ae_true,
                            (ae_int_t) sizeof(mirbfvnsstate),
                            _mirbfvnsstate_init_copy,
                            _mirbfvnsstate_destroy);
      }
      mirbfvnscreatebuf(n,
                        &state->bndl,
                        &state->bndu,
                        &state->s,
                        &state->x0,
                        &state->isintegral,
                        &state->isbinary,
                        &state->xlc.effsparsea,
                        &state->xlc.effal,
                        &state->xlc.effau,
                        &state->xlc.lcsrcidx,
                        state->xlc.nsparse + state->xlc.ndense,
                        &state->nl,
                        &state->nu,
                        state->nnlc,
                        state->mirbfvnsalgo,
                        state->mirbfvnsbudget,
                        state->mirbfvnsmaxneighborhood,
                        state->mirbfvnsbatchsize,
                        state->timeout,
                        state->tracelevel,
                        state->mirbfvnssubsolver,
                        _state);
      mirbfvnssetadaptiveinternalparallelism(
          state->mirbfvnssubsolver, state->adaptiveinternalparallelism, _state);
      if (ae_fp_greater(state->ctol, (double) (0))) {
        mirbfvnssetctol(state->mirbfvnssubsolver, state->ctol, _state);
      }
      if (ae_fp_greater(state->subsolverepsf, (double) (0))) {
        mirbfvnssetepsf(state->mirbfvnssubsolver, state->subsolverepsf, _state);
      }
      if (ae_fp_greater(state->subsolverepsx, (double) (0))) {
        mirbfvnssetepsx(state->mirbfvnssubsolver, state->subsolverepsx, _state);
      }
      b = state->hasobjmask;
      for (i = 0; i <= state->nnlc - 1; i++) {
        b = b || state->hasnlcmask.ptr.p_bool[i];
      }
      if (b) {
        ballocv(1 + state->nnlc, &state->tmpb1, _state);
        sparsecreatecrsemptybuf(n, &state->tmpsparse, _state);
        sparseappendemptyrow(&state->tmpsparse, _state);
        state->tmpb1.ptr.p_bool[0] = state->hasobjmask;
        if (state->hasobjmask) {
          for (i = 0; i <= n - 1; i++) {
            if (state->objmask.ptr.p_bool[i]) {
              sparseappendelement(&state->tmpsparse, i, 1.0, _state);
            }
          }
        }
        if (state->nnlc > 0) {
          sparseappendmatrix(&state->tmpsparse, &state->nlcmask, _state);
        }
        for (i = 0; i <= state->nnlc - 1; i++) {
          state->tmpb1.ptr.p_bool[1 + i] = state->hasnlcmask.ptr.p_bool[i];
        }
        mirbfvnssetvariablemask(
            state->mirbfvnssubsolver, &state->tmpb1, &state->tmpsparse, _state);
      }
      done = ae_true;
    }
    ae_assert(done, "MINLPSolvers: 891649 failed", _state);

    /*
     * Run the solver
     */
    done = ae_false;
    if (state->algoidx != 0) {
      goto lbl_2;
    }
  lbl_4:
    if (!bbgditeration(state->bbgdsubsolver, _state)) {
      goto lbl_5;
    }
    ae_assert(!_rcommstate_has_handler(&state->rcommv2)
                  || !bbgdisrequestfromfront(state->bbgdsubsolver, _state),
              "MINLPSOLVERS: integrity check 964031 failed (front-issued "
              "RCommV2 request escaped handler passed to BBSYNC)",
              _state);

    /*
     * Offload request
     */
    bbgdoffloadrcommrequest(state->bbgdsubsolver,
                            &originalrequesttype,
                            &state->rcommv2.querysize,
                            &state->rcommv2.queryfuncs,
                            &state->rcommv2.queryvars,
                            &state->rcommv2.querydim,
                            &state->rcommv2.queryformulasize,
                            &state->rcommv2.querydata,
                            _state);
    ae_assert((originalrequesttype == 1 && state->issuesparserequests)
                  || (originalrequesttype == 2 && !state->issuesparserequests),
              "MINLPSOLVERS: integrity check 328345 failed",
              _state);
    state->rcommv2.requesttype = originalrequesttype;

    /*
     * Initialize temporaries and prepare place for reply
     */
    densejac
        = (state->rcommv2.requesttype == 2 || state->rcommv2.requesttype == 3)
          || state->rcommv2.requesttype == 5;
    rallocv(n, &state->rcommv2.tmpg1, _state);
    rallocv(n, &state->rcommv2.tmpx1, _state);
    rallocv(1 + state->nnlc, &state->rcommv2.tmpf1, _state);
    if (densejac) {
      rallocm(1 + state->nnlc, n, &state->rcommv2.tmpj1, _state);
      rallocv(state->rcommv2.queryfuncs * state->rcommv2.queryvars
                  * state->rcommv2.querysize,
              &state->rcommv2.replydj,
              _state);
    }
    rallocv(state->rcommv2.queryfuncs * state->rcommv2.querysize,
            &state->rcommv2.replyfi,
            _state);

    /*
     * RComm and copy back
     */
    state->rcommv2.stage = 0;
    if (state->rcommv2.rcomm2_handler != NULL && state->rcommv2.requesttype != 0
        && state->rcommv2.requesttype <= _ALGLIB_MAX_RCOMMV2_REQUEST)
      state->rcommv2.rcomm2_handler(&state->rcommv2,
                                    state->rcommv2.handler_p0,
                                    state->rcommv2.handler_p1,
                                    state->rcommv2.handler_p2,
                                    state->rcommv2.handler_p3,
                                    _state);
    else
      goto lbl_rcomm;
  lbl_0:
    bbgdloadrcommreply(state->bbgdsubsolver,
                       originalrequesttype,
                       state->rcommv2.querysize,
                       state->rcommv2.queryfuncs,
                       state->rcommv2.queryvars,
                       state->rcommv2.querydim,
                       state->rcommv2.queryformulasize,
                       &state->rcommv2.replyfi,
                       &state->rcommv2.replydj,
                       &state->rcommv2.replysj,
                       _state);
    goto lbl_4;
  lbl_5:
    done = ae_true;
  lbl_2:
    if (state->algoidx != 1) {
      goto lbl_6;
    }
  lbl_8:
    if (!mirbfvnsiteration(state->mirbfvnssubsolver, _state)) {
      goto lbl_9;
    }

    /*
     * Offload request
     */
    ae_assert(state->mirbfvnssubsolver->requesttype == 4,
              "MINLPSOLVERS: 993231 failed",
              _state);
    state->rcommv2.requesttype = state->mirbfvnssubsolver->requesttype;
    state->rcommv2.querysize   = state->mirbfvnssubsolver->querysize;
    state->rcommv2.queryfuncs  = state->mirbfvnssubsolver->queryfuncs;
    state->rcommv2.queryvars   = state->mirbfvnssubsolver->queryvars;
    state->rcommv2.querydim    = state->mirbfvnssubsolver->querydim;
    rcopyallocv(state->rcommv2.querysize
                    * (state->rcommv2.queryvars + state->rcommv2.querydim),
                &state->mirbfvnssubsolver->querydata,
                &state->rcommv2.querydata,
                _state);

    /*
     * Initialize temporaries and prepare place for reply
     */
    rallocv(n, &state->rcommv2.tmpx1, _state);
    rallocv(1 + state->nnlc, &state->rcommv2.tmpf1, _state);
    rallocv(state->rcommv2.queryfuncs * state->rcommv2.querysize,
            &state->rcommv2.replyfi,
            _state);

    /*
     * RComm and copy back
     */
    state->rcommv2.stage = 1;
    if (state->rcommv2.rcomm2_handler != NULL && state->rcommv2.requesttype != 0
        && state->rcommv2.requesttype <= _ALGLIB_MAX_RCOMMV2_REQUEST)
      state->rcommv2.rcomm2_handler(&state->rcommv2,
                                    state->rcommv2.handler_p0,
                                    state->rcommv2.handler_p1,
                                    state->rcommv2.handler_p2,
                                    state->rcommv2.handler_p3,
                                    _state);
    else
      goto lbl_rcomm;
  lbl_1:
    rcopyv(state->rcommv2.querysize * state->rcommv2.queryfuncs,
           &state->rcommv2.replyfi,
           &state->mirbfvnssubsolver->replyfi,
           _state);
    goto lbl_8;
  lbl_9:
    done = ae_true;
  lbl_6:
    ae_assert(done, "MINLPSolvers: 926649 failed", _state);

    /*
     * Save results
     */
    done = ae_false;
    if (state->algoidx == 0) {
      rcopyallocv(n, &state->bbgdsubsolver->xc, &state->xc, _state);
      state->repnfev         = state->bbgdsubsolver->repnfev;
      state->repnsubproblems = state->bbgdsubsolver->repnsubproblems;
      state->repntreenodes   = state->bbgdsubsolver->repntreenodes;
      state->repnnodesbeforefeasibility
          = state->bbgdsubsolver->repnnodesbeforefeasibility;
      state->repterminationtype = state->bbgdsubsolver->repterminationtype;
      state->repf               = state->bbgdsubsolver->repf;
      state->reppdgap           = state->bbgdsubsolver->reppdgap;
      done                      = ae_true;
    }
    if (state->algoidx == 1) {
      rcopyallocv(n, &state->mirbfvnssubsolver->xc, &state->xc, _state);
      state->repnfev                    = state->mirbfvnssubsolver->repnfev;
      state->repnsubproblems            = 0;
      state->repntreenodes              = 0;
      state->repnnodesbeforefeasibility = 0;
      state->repterminationtype = state->mirbfvnssubsolver->repterminationtype;
      state->repf               = state->mirbfvnssubsolver->fc;
      state->reppdgap           = (double) (0);
      done                      = ae_true;
    }
    ae_assert(done, "MINLPSolvers: 944650 failed", _state);
    result = ae_false;
    return result;

    /*
     * Saving state
     */
  lbl_rcomm:
    result                          = ae_true;
    state->rcommv2.ia.ptr.p_int[0]  = n;
    state->rcommv2.ia.ptr.p_int[1]  = i;
    state->rcommv2.ia.ptr.p_int[2]  = k;
    state->rcommv2.ia.ptr.p_int[3]  = originalrequesttype;
    state->rcommv2.ba.ptr.p_bool[0] = done;
    state->rcommv2.ba.ptr.p_bool[1] = densejac;
    state->rcommv2.ba.ptr.p_bool[2] = b;
    return result;
  }

  /*************************************************************************
  This subroutine  restarts  algorithm  from  new  point.  All  optimization
  parameters (including constraints) are left unchanged.

  This  function  allows  to  solve multiple  optimization  problems  (which
  must have  same number of dimensions) without object reallocation penalty.

  INPUT PARAMETERS:
      State   -   structure previously allocated with MINLPSolverCreate call.
      X       -   new starting point.

    -- ALGLIB --
       Copyright 28.11.2010 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolverrestartfrom(minlpsolverstate*              state,
                              /* Real    */ const ae_vector* x,
                              ae_state*                      _state) {
    ae_int_t n;

    n = state->n;

    /*
     * First, check for errors in the inputs
     */
    ae_assert(x->cnt >= n, "MINLPSolverRestartFrom: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state),
              "MINLPSolverRestartFrom: X contains infinite or NaN values!",
              _state);

    /*
     * Set XC
     */
    rcopyv(n, x, &state->x0, _state);
    state->hasx0 = ae_true;

    /*
     * prepare RComm facilities
     */
    ae_vector_set_length(&state->rcommv2.ia, 3 + 1, _state);
    ae_vector_set_length(&state->rcommv2.ba, 2 + 1, _state);
    state->rcommv2.stage = -1;
    _rcommstate_clear_handler(&state->rcommv2);
    minlpsolvers_clearoutputs(state, _state);
  }

  /*************************************************************************
  MINLPSolver results:  the  solution  found,  completion  codes  and additional
  information.

  INPUT PARAMETERS:
      Solver  -   solver

  OUTPUT PARAMETERS:
      X       -   array[N], solution
      Rep     -   optimization report, contains information about completion
                  code, constraint violation at the solution and so on.

                  rep.f contains objective value at the solution.

                  You   should   check   rep.terminationtype  in  order   to
                  distinguish successful termination from unsuccessful one.

                  More information about fields of this  structure  can  be
                  found in the comments on the minlpsolverreport datatype.

    -- ALGLIB --
       Copyright 18.01.2024 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolverresults(const minlpsolverstate*  state,
                          /* Real    */ ae_vector* x,
                          minlpsolverreport*       rep,
                          ae_state*                _state) {
    ae_vector_clear(x);
    _minlpsolverreport_clear(rep);

    minlpsolverresultsbuf(state, x, rep, _state);
  }

  /*************************************************************************
  NLC results

  Buffered implementation of MINLPSolverResults() which uses pre-allocated
  buffer to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It
  is intended to be used in the inner cycles of performance critical algorithms
  where array reallocation penalty is too large to be ignored.

    -- ALGLIB --
       Copyright 28.11.2010 by Bochkanov Sergey
  *************************************************************************/
  void minlpsolverresultsbuf(const minlpsolverstate*  state,
                             /* Real    */ ae_vector* x,
                             minlpsolverreport*       rep,
                             ae_state*                _state) {
    rep->f                       = state->repf;
    rep->nfev                    = state->repnfev;
    rep->nsubproblems            = state->repnsubproblems;
    rep->ntreenodes              = state->repntreenodes;
    rep->nnodesbeforefeasibility = state->repnnodesbeforefeasibility;
    rep->terminationtype         = state->repterminationtype;
    rep->pdgap                   = state->reppdgap;
    if (state->repterminationtype > 0) {
      rcopyallocv(state->n, &state->xc, x, _state);
    } else {
      rsetallocv(state->n, _state->v_nan, x, _state);
    }
  }

  /*************************************************************************
  Set V2 reverse communication protocol with dense requests
  *************************************************************************/
  void minlpsolversetprotocolv2(minlpsolverstate* state, ae_state* _state) {
    state->protocolversion     = 2;
    state->issuesparserequests = ae_false;
    ae_vector_set_length(&state->rcommv2.ia, 3 + 1, _state);
    ae_vector_set_length(&state->rcommv2.ba, 2 + 1, _state);
    state->rcommv2.stage = -1;
    _rcommstate_clear_handler(&state->rcommv2);
    _rcommstate_clear_handler(&state->rcommv2);
  }

  /*************************************************************************
  Set V2 reverse communication protocol with sparse requests
  *************************************************************************/
  void minlpsolversetprotocolv2s(minlpsolverstate* state, ae_state* _state) {
    state->protocolversion     = 2;
    state->issuesparserequests = ae_true;
    ae_vector_set_length(&state->rcommv2.ia, 3 + 1, _state);
    ae_vector_set_length(&state->rcommv2.ba, 2 + 1, _state);
    state->rcommv2.stage = -1;
    _rcommstate_clear_handler(&state->rcommv2);
    _rcommstate_clear_handler(&state->rcommv2);
  }

  /*************************************************************************
  Clears output fields during initialization
  *************************************************************************/
  static void minlpsolvers_clearoutputs(minlpsolverstate* state,
                                        ae_state*         _state) {
    state->userterminationneeded      = ae_false;
    state->repnfev                    = 0;
    state->repterminationtype         = 0;
    state->repf                       = (double) (0);
    state->reppdgap                   = ae_maxrealnumber;
    state->repnsubproblems            = 0;
    state->repntreenodes              = 0;
    state->repnnodesbeforefeasibility = -1;
  }

  /*************************************************************************
  Internal initialization subroutine.
  Sets default NLC solver with default criteria.
  *************************************************************************/
  static void minlpsolvers_initinternal(ae_int_t                       n,
                                        /* Real    */ const ae_vector* x,
                                        ae_int_t          solvermode,
                                        double            diffstep,
                                        minlpsolverstate* state,
                                        ae_state*         _state) {
    ae_frame  _frame_block;
    ae_int_t  i;
    ae_matrix c;
    ae_vector ct;

    ae_frame_make(_state, &_frame_block);
    memset(&c, 0, sizeof(c));
    memset(&ct, 0, sizeof(ct));
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ct, 0, DT_INT, _state, ae_true);

    state->protocolversion     = 2;
    state->issuesparserequests = ae_false;
    state->convexityflag       = 0;

    /*
     * Initialize other params
     */
    critinitdefault(&state->criteria, _state);
    state->timeout               = 0;
    state->pdgap                 = (double) (0);
    state->ctol                  = (double) (0);
    state->n                     = n;
    state->subsolverepsx         = (double) (0);
    state->subsolverepsf         = (double) (0);
    state->nmultistarts          = 1;
    state->diffstep              = diffstep;
    state->userterminationneeded = ae_false;
    bsetallocv(n, ae_false, &state->isintegral, _state);
    bsetallocv(n, ae_false, &state->isbinary, _state);
    bsetallocv(n, ae_false, &state->islinear, _state);
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->x0, n, _state);
    ae_vector_set_length(&state->xc, n, _state);
    for (i = 0; i <= n - 1; i++) {
      state->bndl.ptr.p_double[i] = _state->v_neginf;
      state->bndu.ptr.p_double[i] = _state->v_posinf;
      state->s.ptr.p_double[i]    = 1.0;
      state->x0.ptr.p_double[i]   = x->ptr.p_double[i];
      state->xc.ptr.p_double[i]   = x->ptr.p_double[i];
    }
    state->hasx0      = ae_true;
    state->hasobjmask = ae_false;

    /*
     * Constraints
     */
    xlcinit(n, &state->xlc, _state);
    sparsecreatecrsemptybuf(n, &state->nlcmask, _state);
    state->nnlc = 0;

    /*
     * Report fields
     */
    minlpsolvers_clearoutputs(state, _state);

    /*
     * RComm
     */
    ae_vector_set_length(&state->rcommv2.ia, 3 + 1, _state);
    ae_vector_set_length(&state->rcommv2.ba, 2 + 1, _state);
    state->rcommv2.stage = -1;
    _rcommstate_clear_handler(&state->rcommv2);

    /*
     * Final setup
     */
    minlpsolversetbbsyncprofilelargetree(state, _state);
    minlpsolvercautiousinternalparallelism(state, _state);
    minlpsolversetalgobbsync(state, 1, _state);
    minlpsolversetbbsyncsubsolveripm(state, 0, _state);
    ae_frame_leave(_state);
  }

  void _minlpsolverstate_init(void*     _p,
                              ae_state* _state,
                              ae_bool   make_automatic) {
    minlpsolverstate* p = (minlpsolverstate*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_init(&p->criteria, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->isintegral, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->islinear, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->isbinary, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->objmask, 0, DT_BOOL, _state, make_automatic);
    _xlinearconstraints_init(&p->xlc, _state, make_automatic);
    _sparsematrix_init(&p->nlcmask, _state, make_automatic);
    ae_vector_init(&p->nl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasnlcmask, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xc, 0, DT_REAL, _state, make_automatic);
    ae_smart_ptr_init(&p->_bbgdsubsolver,
                      (void**) &p->bbgdsubsolver,
                      ae_true,
                      _state,
                      make_automatic);
    ae_smart_ptr_init(&p->_mirbfvnssubsolver,
                      (void**) &p->mirbfvnssubsolver,
                      ae_true,
                      _state,
                      make_automatic);
    ae_vector_init(&p->rdummy, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpb1, 0, DT_BOOL, _state, make_automatic);
    _sparsematrix_init(&p->tmpsparse, _state, make_automatic);
    _rcommstate_init(&p->rcommv2, _state, make_automatic);
  }

  void _minlpsolverstate_init_copy(void*       _dst,
                                   const void* _src,
                                   ae_state*   _state,
                                   ae_bool     make_automatic) {
    minlpsolverstate*       dst = (minlpsolverstate*) _dst;
    const minlpsolverstate* src = (const minlpsolverstate*) _src;
    dst->n                      = src->n;
    dst->algoidx                = src->algoidx;
    _nlpstoppingcriteria_init_copy(
        &dst->criteria, &src->criteria, _state, make_automatic);
    dst->diffstep                    = src->diffstep;
    dst->convexityflag               = src->convexityflag;
    dst->pdgap                       = src->pdgap;
    dst->ctol                        = src->ctol;
    dst->subsolverepsx               = src->subsolverepsx;
    dst->subsolverepsf               = src->subsolverepsf;
    dst->nmultistarts                = src->nmultistarts;
    dst->timeout                     = src->timeout;
    dst->bbgdgroupsize               = src->bbgdgroupsize;
    dst->bbsyncsubsolver             = src->bbsyncsubsolver;
    dst->bbsyncsubsolvermemlen       = src->bbsyncsubsolvermemlen;
    dst->mirbfvnsbudget              = src->mirbfvnsbudget;
    dst->mirbfvnsmaxneighborhood     = src->mirbfvnsmaxneighborhood;
    dst->mirbfvnsbatchsize           = src->mirbfvnsbatchsize;
    dst->mirbfvnsalgo                = src->mirbfvnsalgo;
    dst->adaptiveinternalparallelism = src->adaptiveinternalparallelism;
    dst->bbsyncprofile               = src->bbsyncprofile;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(
        &dst->isintegral, &src->isintegral, _state, make_automatic);
    ae_vector_init_copy(&dst->islinear, &src->islinear, _state, make_automatic);
    ae_vector_init_copy(&dst->isbinary, &src->isbinary, _state, make_automatic);
    dst->hasobjmask = src->hasobjmask;
    ae_vector_init_copy(&dst->objmask, &src->objmask, _state, make_automatic);
    _xlinearconstraints_init_copy(&dst->xlc, &src->xlc, _state, make_automatic);
    _sparsematrix_init_copy(
        &dst->nlcmask, &src->nlcmask, _state, make_automatic);
    dst->nnlc = src->nnlc;
    ae_vector_init_copy(&dst->nl, &src->nl, _state, make_automatic);
    ae_vector_init_copy(&dst->nu, &src->nu, _state, make_automatic);
    ae_vector_init_copy(
        &dst->hasnlcmask, &src->hasnlcmask, _state, make_automatic);
    dst->hasx0 = src->hasx0;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    dst->protocolversion       = src->protocolversion;
    dst->issuesparserequests   = src->issuesparserequests;
    dst->userterminationneeded = src->userterminationneeded;
    ae_vector_init_copy(&dst->xc, &src->xc, _state, make_automatic);
    dst->repnfev                    = src->repnfev;
    dst->repnsubproblems            = src->repnsubproblems;
    dst->repntreenodes              = src->repntreenodes;
    dst->repnnodesbeforefeasibility = src->repnnodesbeforefeasibility;
    dst->repterminationtype         = src->repterminationtype;
    dst->repf                       = src->repf;
    dst->reppdgap                   = src->reppdgap;
    dst->tracelevel                 = src->tracelevel;
    ae_smart_ptr_init(&dst->_bbgdsubsolver,
                      (void**) &dst->bbgdsubsolver,
                      ae_true,
                      _state,
                      make_automatic);
    if (src->bbgdsubsolver != NULL) {
      dst->bbgdsubsolver = (bbgdstate*) ae_malloc(
          sizeof(bbgdstate),
          _state); /* note: using bbgdsubsolver as a temporary prior to
                      assigning its value to _bbgdsubsolver */
      memset(dst->bbgdsubsolver, 0, sizeof(bbgdstate));
      _bbgdstate_init_copy(
          dst->bbgdsubsolver, src->bbgdsubsolver, _state, ae_false);
      ae_smart_ptr_assign(&dst->_bbgdsubsolver,
                          dst->bbgdsubsolver,
                          ae_true,
                          ae_true,
                          (ae_int_t) sizeof(bbgdstate),
                          _bbgdstate_init_copy,
                          _bbgdstate_destroy);
    }
    ae_smart_ptr_init(&dst->_mirbfvnssubsolver,
                      (void**) &dst->mirbfvnssubsolver,
                      ae_true,
                      _state,
                      make_automatic);
    if (src->mirbfvnssubsolver != NULL) {
      dst->mirbfvnssubsolver = (mirbfvnsstate*) ae_malloc(
          sizeof(mirbfvnsstate),
          _state); /* note: using mirbfvnssubsolver as a temporary prior to
                      assigning its value to _mirbfvnssubsolver */
      memset(dst->mirbfvnssubsolver, 0, sizeof(mirbfvnsstate));
      _mirbfvnsstate_init_copy(
          dst->mirbfvnssubsolver, src->mirbfvnssubsolver, _state, ae_false);
      ae_smart_ptr_assign(&dst->_mirbfvnssubsolver,
                          dst->mirbfvnssubsolver,
                          ae_true,
                          ae_true,
                          (ae_int_t) sizeof(mirbfvnsstate),
                          _mirbfvnsstate_init_copy,
                          _mirbfvnsstate_destroy);
    }
    ae_vector_init_copy(&dst->rdummy, &src->rdummy, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpb1, &src->tmpb1, _state, make_automatic);
    _sparsematrix_init_copy(
        &dst->tmpsparse, &src->tmpsparse, _state, make_automatic);
    _rcommstate_init_copy(&dst->rcommv2, &src->rcommv2, _state, make_automatic);
  }

  void _minlpsolverstate_clear(void* _p) {
    minlpsolverstate* p = (minlpsolverstate*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_clear(&p->criteria);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->isintegral);
    ae_vector_clear(&p->islinear);
    ae_vector_clear(&p->isbinary);
    ae_vector_clear(&p->objmask);
    _xlinearconstraints_clear(&p->xlc);
    _sparsematrix_clear(&p->nlcmask);
    ae_vector_clear(&p->nl);
    ae_vector_clear(&p->nu);
    ae_vector_clear(&p->hasnlcmask);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->xc);
    ae_smart_ptr_clear(&p->_bbgdsubsolver);
    ae_smart_ptr_clear(&p->_mirbfvnssubsolver);
    ae_vector_clear(&p->rdummy);
    ae_vector_clear(&p->tmpb1);
    _sparsematrix_clear(&p->tmpsparse);
    _rcommstate_clear(&p->rcommv2);
  }

  void _minlpsolverstate_destroy(void* _p) {
    minlpsolverstate* p = (minlpsolverstate*) _p;
    ae_touch_ptr((void*) p);
    _nlpstoppingcriteria_destroy(&p->criteria);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->isintegral);
    ae_vector_destroy(&p->islinear);
    ae_vector_destroy(&p->isbinary);
    ae_vector_destroy(&p->objmask);
    _xlinearconstraints_destroy(&p->xlc);
    _sparsematrix_destroy(&p->nlcmask);
    ae_vector_destroy(&p->nl);
    ae_vector_destroy(&p->nu);
    ae_vector_destroy(&p->hasnlcmask);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->xc);
    ae_smart_ptr_destroy(&p->_bbgdsubsolver);
    ae_smart_ptr_destroy(&p->_mirbfvnssubsolver);
    ae_vector_destroy(&p->rdummy);
    ae_vector_destroy(&p->tmpb1);
    _sparsematrix_destroy(&p->tmpsparse);
    _rcommstate_destroy(&p->rcommv2);
  }

  void _minlpsolverreport_init(void*     _p,
                               ae_state* _state,
                               ae_bool   make_automatic) {
    minlpsolverreport* p = (minlpsolverreport*) _p;
    ae_touch_ptr((void*) p);
  }

  void _minlpsolverreport_init_copy(void*       _dst,
                                    const void* _src,
                                    ae_state*   _state,
                                    ae_bool     make_automatic) {
    minlpsolverreport*       dst = (minlpsolverreport*) _dst;
    const minlpsolverreport* src = (const minlpsolverreport*) _src;
    dst->f                       = src->f;
    dst->nfev                    = src->nfev;
    dst->nsubproblems            = src->nsubproblems;
    dst->ntreenodes              = src->ntreenodes;
    dst->nnodesbeforefeasibility = src->nnodesbeforefeasibility;
    dst->terminationtype         = src->terminationtype;
    dst->pdgap                   = src->pdgap;
  }

  void _minlpsolverreport_clear(void* _p) {
    minlpsolverreport* p = (minlpsolverreport*) _p;
    ae_touch_ptr((void*) p);
  }

  void _minlpsolverreport_destroy(void* _p) {
    minlpsolverreport* p = (minlpsolverreport*) _p;
    ae_touch_ptr((void*) p);
  }

#endif

}  // namespace alglib_impl
