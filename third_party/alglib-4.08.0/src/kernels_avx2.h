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
#ifndef _kernels_avx2_h
#define _kernels_avx2_h

#include "ap.h"

#define AE_USE_CPP



namespace alglib_impl
{
#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_AVX2_INTRINSICS)

double rdotv_avx2(const ae_int_t n,
    /* Real    */ const ae_vector* x,
    /* Real    */ const ae_vector* y,
    ae_state *_state);
double rdotvr_avx2(const ae_int_t n,
    /* Real    */ const ae_vector* x,
    /* Real    */ const ae_matrix* a,
    ae_int_t i,
    ae_state *_state);
double rdotrr_avx2(const ae_int_t n,
    /* Real    */ const ae_matrix* a,
    ae_int_t ia,
    /* Real    */ const ae_matrix* b,
    ae_int_t ib,
    ae_state *_state);
double rdotv2_avx2(const ae_int_t n,
    /* Real    */ const ae_vector* x,
    ae_state *_state);
void rcopyv_avx2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopyvr_avx2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
void rcopyrv_avx2(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rcopyrr_avx2(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_matrix* b,
     ae_int_t k,
     ae_state *_state);
void rcopymulv_avx2(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopymulvr_avx2(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t ridx,
     ae_state *_state);
void rcopyrcpv_avx2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopyrcpvx_avx2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
void icopyv_avx2(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state);
void bcopyv_avx2(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state);
void rsetv_avx2(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rsetr_avx2(ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
void rsetvx_avx2(ae_int_t n, double v, ae_vector* x, ae_int_t offsx, ae_state *_state);
void isetv_avx2(ae_int_t n, ae_int_t v, ae_vector* x, ae_state *_state);
void bsetv_avx2(ae_int_t n, ae_bool v, ae_vector* x, ae_state *_state);
void rmulv_avx2(ae_int_t n, double v, ae_vector* x, ae_state *_state);
void rmulr_avx2(ae_int_t n, double v, ae_matrix* x, ae_int_t rowidx, ae_state *_state);
void rsqrtv_avx2(ae_int_t n, ae_vector* x, ae_state *_state);
void rsqrtr_avx2(ae_int_t n, ae_matrix* x, ae_int_t rowidx, ae_state *_state);
void rmulvx_avx2(ae_int_t n, double v, ae_vector* x, ae_int_t offsx, ae_state *_state);
void raddv_avx2(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void raddvr_avx2(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void raddrv_avx2(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void raddrr_avx2(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridxsrc,
     /* Real    */ ae_matrix* x,
     ae_int_t ridxdst,
     ae_state *_state);
void raddvx_avx2(ae_int_t n, double alpha, const ae_vector* y,
    ae_int_t offsy, ae_vector* x, ae_int_t offsx, ae_state *_state);
void rmuladdvx_avx2(const ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t offsy,
     /* Real    */ const ae_vector* z,
     ae_int_t offsz,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rcopynegmuladdvx_avx2(const ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t offsy,
     /* Real    */ const ae_vector* z,
     ae_int_t offsz,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* r,
     ae_int_t offsr,
     ae_state *_state);
void rmergemulv_avx2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergemulvr_avx2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergemulrv_avx2(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergemulvx_avx2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t offsy,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rmergedivv_avx2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergedivvr_avx2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergedivrv_avx2(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergemaxv_avx2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergemaxvr_avx2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergemaxrv_avx2(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergeminv_avx2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergeminvr_avx2(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergeminrv_avx2(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
double rmaxv_avx2(ae_int_t n, /* Real    */ const ae_vector* x, ae_state *_state);
double rmaxr_avx2(ae_int_t n, /* Real    */ const ae_matrix* x, ae_int_t rowidx, ae_state *_state);
double rmaxabsv_avx2(ae_int_t n, /* Real    */ const ae_vector* x, ae_state *_state);
double rmaxabsr_avx2(ae_int_t n, /* Real    */ const ae_matrix* x, ae_int_t rowidx, ae_state *_state);
void rcopyvx_avx2(ae_int_t n, const ae_vector* x, ae_int_t offsx,
    ae_vector* y, ae_int_t offsy, ae_state *_state);
void icopyvx_avx2(ae_int_t n, const ae_vector* x, ae_int_t offsx,
                ae_vector* y, ae_int_t offsy, ae_state *_state);

void rgemv_straight_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a,
    const double* __restrict x, double* __restrict y, ae_state* _state);
void rgemv_transposed_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a,
    const double* __restrict x, double* __restrict y, ae_state* _state);
void rgemvx_straight_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x,
    double* __restrict y, ae_state* _state);
void rgemvx_transposed_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x, double* __restrict y,
    ae_state* _state);

ae_int_t ablasf_packblkh_avx2(
    const double *src,
    ae_int_t src_stride,
    ae_int_t op,
    ae_int_t opsrc_length,
    ae_int_t opsrc_width,
    double   *dst,
    ae_int_t block_size,
    ae_int_t micro_size);
ae_int_t ablasf_packblkh32_avx2(
    const double *src,
    ae_int_t src_stride,
    ae_int_t op,
    ae_int_t ignore_opsrc_length,
    ae_int_t opsrc_width,
    double   *dst,
    ae_int_t ignore_block_size,
    ae_int_t micro_size);
void ablasf_dotblkh_avx2(
    const double *src_a,
    const double *src_b,
    ae_int_t round_length,
    ae_int_t block_size,
    ae_int_t micro_size,
    double *dst,
    ae_int_t dst_stride);
void ablasf_daxpby_avx2(
    ae_int_t     n,
    double       alpha,
    const double *src,
    double       beta,
    double       *dst);
ae_bool spchol_updatekernelabc4_avx2(ae_vector* _rowstorage,
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
ae_bool spchol_updatekernel4444_avx2(
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
ae_bool rbfv3farfields_bhpaneleval1fastkernel16_avx2(double d0,
     double d1,
     double d2,
     const double* pnma,
     const double* pnmb,
     const double* pmmcdiag,
     const double* ynma,
     const double* tblrmodmn,
     double* f,
     double* invpowrpplus1,
     ae_state *_state);
ae_bool rbfv3farfields_bhpanelevalfastkernel16_avx2(double d0,
     double d1,
     double d2,
     ae_int_t ny,
     const double* pnma,
     const double* pnmb,
     const double* pmmcdiag,
     const double* ynma,
     const double* tblrmodmn,
     double* f,
     double* invpowrpplus1,
     ae_state *_state);

/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_AVX2_INTRINSICS */
#endif

}

#endif

