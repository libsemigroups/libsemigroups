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
#ifndef _kernels_sse2_h
#define _kernels_sse2_h

#include "ap.h"

#define AE_USE_CPP



namespace alglib_impl
{
#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_SSE2_INTRINSICS)

double rdotv_sse2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_state *_state);
double rdotvr_sse2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
double rdotrr_sse2(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_state *_state);
double rdotv2_sse2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void rcopyv_sse2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopyvr_sse2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
void rcopyrv_sse2(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rcopyrr_sse2(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_matrix* b,
     ae_int_t k,
     ae_state *_state);
void rcopymulv_sse2(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopymulvr_sse2(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t ridx,
     ae_state *_state);
void icopyv_sse2(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state);
void bcopyv_sse2(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state);
void rsetv_sse2(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rsetr_sse2(ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
void rsetvx_sse2(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void isetv_sse2(ae_int_t n, ae_int_t v, ae_vector* x, ae_state *_state);
void bsetv_sse2(ae_int_t n, ae_bool v, ae_vector* x, ae_state *_state);
void rmulv_sse2(ae_int_t n, double v, ae_vector* x, ae_state *_state);
void rmulr_sse2(ae_int_t n, double v, ae_matrix* x, ae_int_t rowidx, ae_state *_state);
void rmulvx_sse2(ae_int_t n, double v, ae_vector* x, ae_int_t offsx, ae_state *_state);
void raddv_sse2(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void raddvr_sse2(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void raddrv_sse2(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void raddrr_sse2(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridxsrc,
     /* Real    */ ae_matrix* x,
     ae_int_t ridxdst,
     ae_state *_state);
void raddvx_sse2(ae_int_t n, double alpha,
    const ae_vector* y, ae_int_t offsy, ae_vector* x, ae_int_t offsx, ae_state *_state);
void rmergemulv_sse2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergemulvr_sse2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergemulrv_sse2(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergemaxv_sse2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergemaxvr_sse2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergemaxrv_sse2(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergeminv_sse2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergeminvr_sse2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergeminrv_sse2(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
double rmaxv_sse2(ae_int_t n, /* Real    */ const ae_vector* x, ae_state *_state);
double rmaxr_sse2(ae_int_t n, /* Real    */ const ae_matrix* x, ae_int_t rowidx, ae_state *_state);
double rmaxabsv_sse2(ae_int_t n, /* Real    */ const ae_vector* x, ae_state *_state);
double rmaxabsr_sse2(ae_int_t n, /* Real    */ const ae_matrix* x, ae_int_t rowidx, ae_state *_state);
void rcopyvx_sse2(ae_int_t n, const ae_vector* x, ae_int_t offsx,
    ae_vector* y, ae_int_t offsy, ae_state *_state);
void icopyvx_sse2(ae_int_t n, const ae_vector* x, ae_int_t offsx,
                ae_vector* y, ae_int_t offsy, ae_state *_state);
/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_SSE2_INTRINSICS */
#endif

}

#endif

