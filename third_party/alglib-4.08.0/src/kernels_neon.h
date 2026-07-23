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
#ifndef _kernels_neon_h
#define _kernels_neon_h

#include "ap.h"

#define AE_USE_CPP



namespace alglib_impl
{
#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_NEON_INTRINSICS)

double rdotv_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_state *_state);
double rdotvr_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
double rdotrr_neon(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_state *_state);
double rdotv2_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void rcopyv_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopyvr_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
void rcopyrv_neon(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rcopyrr_neon(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_matrix* b,
     ae_int_t k,
     ae_state *_state);
void rcopymulv_neon(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopymulvr_neon(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t ridx,
     ae_state *_state);
void rcopyrcpv_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopyrcpvx_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
void rcopyvx_neon(ae_int_t n,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
void icopyvx_neon(ae_int_t n,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
void icopyv_neon(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state);
void bcopyv_neon(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state);
void rsetv_neon(ae_int_t n,
     double v,
     ae_vector* x,
     ae_state *_state);
void rsetr_neon(ae_int_t n,
     double v,
     ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
void rsetvx_neon(ae_int_t n,
     double v,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void isetv_neon(ae_int_t n, ae_int_t v, ae_vector* x, ae_state *_state);
void bsetv_neon(ae_int_t n, ae_bool v, ae_vector* x, ae_state *_state);
void rmulv_neon(ae_int_t n,
     double v,
     ae_vector* x,
     ae_state *_state);
void rmulr_neon(ae_int_t n,
     double v,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rsqrtv_neon(ae_int_t n,
     ae_vector* x,
     ae_state *_state);
void rsqrtr_neon(ae_int_t n,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmulvx_neon(ae_int_t n,
     double v,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void raddv_neon(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state);
void raddvr_neon(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void raddrv_neon(ae_int_t n,
     double alpha,
     const ae_matrix* y,
     ae_int_t ridx,
     ae_vector* x,
     ae_state *_state);
void raddrr_neon(ae_int_t n,
     double alpha,
     const ae_matrix* y,
     ae_int_t ridxsrc,
     ae_matrix* x,
     ae_int_t ridxdst,
     ae_state *_state);
void raddvx_neon(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_int_t offsy,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rmuladdv_neon(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     ae_vector* x,
     ae_state *_state);
void rmuladdvx_neon(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rnegmuladdv_neon(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     ae_vector* x,
     ae_state *_state);
void rnegmuladdvx_neon(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rcopymuladdv_neon(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     const ae_vector* x,
     ae_vector* r,
     ae_state *_state);
void rcopymuladdvx_neon(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* r,
     ae_int_t offsr,
     ae_state *_state);
void rcopynegmuladdv_neon(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     const ae_vector* x,
     ae_vector* r,
     ae_state *_state);
void rcopynegmuladdvx_neon(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* r,
     ae_int_t offsr,
     ae_state *_state);
void rmergemulv_neon(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state);
void rmergemulvx_neon(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rmergemulvr_neon(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergemulrv_neon(ae_int_t n,
     const ae_matrix* y,
     ae_int_t rowidx,
     ae_vector* x,
     ae_state *_state);
void rmergedivv_neon(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state);
void rmergedivvr_neon(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergedivrv_neon(ae_int_t n,
     const ae_matrix* y,
     ae_int_t rowidx,
     ae_vector* x,
     ae_state *_state);
void rmergemaxv_neon(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state);
void rmergemaxvr_neon(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergemaxrv_neon(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_vector* y,
     ae_state *_state);
void rmergeminv_neon(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state);
void rmergeminvr_neon(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergeminrv_neon(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_vector* y,
     ae_state *_state);
double rmaxv_neon(ae_int_t n, const ae_vector* x, ae_state *_state);
double rmaxr_neon(ae_int_t n, const ae_matrix* x, ae_int_t rowidx, ae_state *_state);
double rmaxabsv_neon(ae_int_t n, const ae_vector* x, ae_state *_state);
double rmaxabsr_neon(ae_int_t n, const ae_matrix* x, ae_int_t rowidx, ae_state *_state);

/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_NEON_INTRINSICS */
#endif

}

#endif

