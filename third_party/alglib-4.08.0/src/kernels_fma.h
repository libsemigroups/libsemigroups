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
#ifndef _kernels_fma_h
#define _kernels_fma_h

#include "ap.h"

#define AE_USE_CPP



namespace alglib_impl
{
#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_AVX2_INTRINSICS)

double rdotv_fma(const ae_int_t n,
    /* Real    */ const ae_vector* x,
    /* Real    */ const ae_vector* y,
    ae_state *_state);
double rdotvr_fma(const ae_int_t n,
    /* Real    */ const ae_vector* x,
    /* Real    */ const ae_matrix* a,
    ae_int_t i,
    ae_state *_state);
double rdotrr_fma(const ae_int_t n,
    /* Real    */ const ae_matrix* a,
    ae_int_t ia,
    /* Real    */ const ae_matrix* b,
    ae_int_t ib,
    ae_state *_state);
double rdotv2_fma(const ae_int_t n,
    /* Real    */ const ae_vector* x,
    ae_state *_state);
void raddv_fma(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void raddvr_fma(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void raddrv_fma(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void raddrr_fma(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridxsrc,
     /* Real    */ ae_matrix* x,
     ae_int_t ridxdst,
     ae_state *_state);
void raddvx_fma(ae_int_t n, double alpha, const ae_vector* y, ae_int_t offsy, ae_vector* x, ae_int_t offsx, ae_state *_state);
void rmuladdv_fma(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmuladdvx_fma(const ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t offsy,
     /* Real    */ const ae_vector* z,
     ae_int_t offsz,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state* _state);
void rnegmuladdv_fma(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rnegmuladdvx_fma(const ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t offsy,
     /* Real    */ const ae_vector* z,
     ae_int_t offsz,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state* _state);
void rcopymuladdv_fma(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* r,
     ae_state *_state);
void rcopymuladdvx_fma(const ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t offsy,
     /* Real    */ const ae_vector* z,
     ae_int_t offsz,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* r,
     ae_int_t offsr,
     ae_state* _state);
void rcopynegmuladdv_fma(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* r,
     ae_state *_state);
void rcopynegmuladdvx_fma(const ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t offsy,
     /* Real    */ const ae_vector* z,
     ae_int_t offsz,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* r,
     ae_int_t offsr,
     ae_state* _state);
void rgemv_straight_fma(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a,
    const double* __restrict x, double* __restrict y, ae_state* _state);
void rgemv_transposed_fma(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a,
    const double* __restrict x, double* __restrict y, ae_state* _state);
void rgemvx_straight_fma(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x,
    double* __restrict y, ae_state* _state);
void rgemvx_transposed_fma(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x, double* __restrict y,
    ae_state* _state);

void ablasf_dotblkh_fma(
    const double *src_a,
    const double *src_b,
    ae_int_t round_length,
    ae_int_t block_size,
    ae_int_t micro_size,
    double *dst,
    ae_int_t dst_stride);
void spchol_propagatefwd_fma(/* Real    */ const ae_vector* x,
     ae_int_t cols0,
     ae_int_t blocksize,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t rbase,
     ae_int_t offdiagsize,
     /* Real    */ const ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t sstride,
     /* Real    */ ae_vector* simdbuf,
     ae_int_t simdwidth,
     ae_state *_state);
ae_bool spchol_updatekernelabc4_fma(ae_vector* _rowstorage,
     ae_int_t offss,
     ae_int_t twidth,
     ae_int_t offsu,
     ae_int_t uheight,
     ae_int_t urank,
     ae_int_t urowstride,
     ae_int_t uwidth,
     const ae_vector* _diagd,
     ae_int_t offsd,
     const ae_vector* _raw2smap,
     const ae_vector* _superrowidx,
     ae_int_t urbase,
     ae_state *_state);
ae_bool spchol_updatekernel4444_fma(
     double*  rowstorage,
     ae_int_t offss,
     ae_int_t sheight,
     ae_int_t offsu,
     ae_int_t uheight,
     const double*  diagd,
     ae_int_t offsd,
     const ae_int_t* raw2smap,
     const ae_int_t* superrowidx,
     ae_int_t urbase,
     ae_state *_state);


/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_AVX2_INTRINSICS */
#endif

}

#endif

