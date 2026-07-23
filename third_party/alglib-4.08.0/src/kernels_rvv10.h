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
#ifndef _kernels_rvv10_h
#define _kernels_rvv10_h

#include "ap.h"

#define AE_USE_CPP



namespace alglib_impl
{
#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_RVV10_INTRINSICS)

double rdotv_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_state *_state);
double rdotvr_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
double rdotrr_rvv10(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_state *_state);
double rdotv2_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void rcopyv_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopyvr_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
void rcopyrv_rvv10(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rcopyrr_rvv10(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_matrix* b,
     ae_int_t k,
     ae_state *_state);
void rcopymulv_rvv10(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopymulvr_rvv10(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t ridx,
     ae_state *_state);
void rcopyrcpv_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopyrcpvx_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
void rcopyvx_rvv10(ae_int_t n,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
void icopyvx_rvv10(ae_int_t n,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
void icopyv_rvv10(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state);
void bcopyv_rvv10(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state);
void rsetv_rvv10(ae_int_t n,
     double v,
     ae_vector* x,
     ae_state *_state);
void rsetr_rvv10(ae_int_t n,
     double v,
     ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
void rsetvx_rvv10(ae_int_t n,
     double v,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void isetv_rvv10(ae_int_t n, ae_int_t v, ae_vector* x, ae_state *_state);
void bsetv_rvv10(ae_int_t n, ae_bool v, ae_vector* x, ae_state *_state);
void rmulv_rvv10(ae_int_t n,
     double v,
     ae_vector* x,
     ae_state *_state);
void rmulr_rvv10(ae_int_t n,
     double v,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rsqrtv_rvv10(ae_int_t n,
     ae_vector* x,
     ae_state *_state);
void rsqrtr_rvv10(ae_int_t n,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmulvx_rvv10(ae_int_t n,
     double v,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void raddv_rvv10(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state);
void raddvr_rvv10(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void raddrv_rvv10(ae_int_t n,
     double alpha,
     const ae_matrix* y,
     ae_int_t ridx,
     ae_vector* x,
     ae_state *_state);
void raddrr_rvv10(ae_int_t n,
     double alpha,
     const ae_matrix* y,
     ae_int_t ridxsrc,
     ae_matrix* x,
     ae_int_t ridxdst,
     ae_state *_state);
void raddvx_rvv10(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_int_t offsy,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rmuladdv_rvv10(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     ae_vector* x,
     ae_state *_state);
void rmuladdvx_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rnegmuladdv_rvv10(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     ae_vector* x,
     ae_state *_state);
void rnegmuladdvx_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rcopymuladdv_rvv10(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     const ae_vector* x,
     ae_vector* r,
     ae_state *_state);
void rcopymuladdvx_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* r,
     ae_int_t offsr,
     ae_state *_state);
void rcopynegmuladdv_rvv10(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     const ae_vector* x,
     ae_vector* r,
     ae_state *_state);
void rcopynegmuladdvx_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* r,
     ae_int_t offsr,
     ae_state *_state);
void rmergemulv_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state);
void rmergemulvx_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rmergemulvr_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergemulrv_rvv10(ae_int_t n,
     const ae_matrix* y,
     ae_int_t rowidx,
     ae_vector* x,
     ae_state *_state);
void rmergedivv_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state);
void rmergedivvr_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergedivrv_rvv10(ae_int_t n,
     const ae_matrix* y,
     ae_int_t rowidx,
     ae_vector* x,
     ae_state *_state);
void rmergemaxv_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state);
void rmergemaxvr_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergemaxrv_rvv10(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_vector* y,
     ae_state *_state);
void rmergeminv_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state);
void rmergeminvr_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergeminrv_rvv10(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_vector* y,
     ae_state *_state);
double rmaxv_rvv10(ae_int_t n,
     const ae_vector* x,
     ae_state *_state);
double rmaxr_rvv10(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
double rmaxabsv_rvv10(ae_int_t n,
     const ae_vector* x,
     ae_state *_state);
double rmaxabsr_rvv10(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);

/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_RVV10_INTRINSICS */
#endif

}

#endif

