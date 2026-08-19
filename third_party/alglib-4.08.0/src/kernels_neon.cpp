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
#include "stdafx.h"

//
// Must be defined before we include kernel header
//
#define _ALGLIB_IMPL_DEFINES
#define _ALGLIB_INTEGRITY_CHECKS_ONCE

#include "kernels_neon.h"

// disable some irrelevant warnings
#if (AE_COMPILER==AE_MSVC) && !defined(AE_ALL_WARNINGS)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#pragma warning(disable:4611)
#pragma warning(disable:4702)
#pragma warning(disable:4996)
#endif

namespace alglib_impl
{



#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_NEON_INTRINSICS)

static double rabs_neon_scalar(double x)
{
    return x>=0.0 ? x : -x;
}

static double rdot_neon(ae_int_t n,
     const double* px,
     const double* py,
     ae_state *_state)
{
    ae_int_t i;
    const ae_int_t neonlen = n>>1;
    const ae_int_t unroll_len = (neonlen>>2)<<2;
    const float64x2_t* __restrict pX = (const float64x2_t*)px;
    const float64x2_t* __restrict pY = (const float64x2_t*)py;
    float64x2_t acc;

    (void)_state;

    if( unroll_len>=4 )
    {
        float64x2_t acc0 = vmulq_f64(pX[0], pY[0]);
        float64x2_t acc1 = vmulq_f64(pX[1], pY[1]);
        float64x2_t acc2 = vmulq_f64(pX[2], pY[2]);
        float64x2_t acc3 = vmulq_f64(pX[3], pY[3]);
        for(i=4; i<unroll_len; i+=4)
        {
            acc0 = vaddq_f64(vmulq_f64(pX[i+0], pY[i+0]), acc0);
            acc1 = vaddq_f64(vmulq_f64(pX[i+1], pY[i+1]), acc1);
            acc2 = vaddq_f64(vmulq_f64(pX[i+2], pY[i+2]), acc2);
            acc3 = vaddq_f64(vmulq_f64(pX[i+3], pY[i+3]), acc3);
        }
        switch(neonlen-unroll_len)
        {
        case 3:
            acc2 = vaddq_f64(vmulq_f64(pX[i+2], pY[i+2]), acc2);
        case 2:
            acc1 = vaddq_f64(vmulq_f64(pX[i+1], pY[i+1]), acc1);
        case 1:
            acc0 = vaddq_f64(vmulq_f64(pX[i+0], pY[i+0]), acc0);
        default:
            break;
        }
        acc = vaddq_f64(vaddq_f64(acc0, acc1), vaddq_f64(acc2, acc3));
    }
    else
    {
        switch(neonlen)
        {
        case 0:
            acc = vdupq_n_f64(0.0);
            break;
        case 1:
            acc = vmulq_f64(pX[0], pY[0]);
            break;
        case 2:
            acc = vaddq_f64(vmulq_f64(pX[0], pY[0]), vmulq_f64(pX[1], pY[1]));
            break;
        default:
            acc = vaddq_f64(
                vaddq_f64(vmulq_f64(pX[0], pY[0]), vmulq_f64(pX[1], pY[1])),
                vmulq_f64(pX[2], pY[2]));
            break;
        }
    }

    {
        double scalar = vgetq_lane_f64(acc, 0) + vgetq_lane_f64(acc, 1);
        const ae_int_t tail = neonlen<<1;
        if( n-tail )
            scalar += px[tail]*py[tail];
        return scalar;
    }
}

static double rdotv2_neon_impl(ae_int_t n,
     const double* px,
     ae_state *_state)
{
    ae_int_t i;
    const ae_int_t neonlen = n>>1;
    const ae_int_t unroll_len = (neonlen>>2)<<2;
    const float64x2_t* __restrict pX = (const float64x2_t*)px;
    float64x2_t acc;

    (void)_state;

    if( unroll_len>=4 )
    {
        float64x2_t acc0 = vmulq_f64(pX[0], pX[0]);
        float64x2_t acc1 = vmulq_f64(pX[1], pX[1]);
        float64x2_t acc2 = vmulq_f64(pX[2], pX[2]);
        float64x2_t acc3 = vmulq_f64(pX[3], pX[3]);
        for(i=4; i<unroll_len; i+=4)
        {
            acc0 = vaddq_f64(vmulq_f64(pX[i+0], pX[i+0]), acc0);
            acc1 = vaddq_f64(vmulq_f64(pX[i+1], pX[i+1]), acc1);
            acc2 = vaddq_f64(vmulq_f64(pX[i+2], pX[i+2]), acc2);
            acc3 = vaddq_f64(vmulq_f64(pX[i+3], pX[i+3]), acc3);
        }
        switch(neonlen-unroll_len)
        {
        case 3:
            acc2 = vaddq_f64(vmulq_f64(pX[i+2], pX[i+2]), acc2);
        case 2:
            acc1 = vaddq_f64(vmulq_f64(pX[i+1], pX[i+1]), acc1);
        case 1:
            acc0 = vaddq_f64(vmulq_f64(pX[i+0], pX[i+0]), acc0);
        default:
            break;
        }
        acc = vaddq_f64(vaddq_f64(acc0, acc1), vaddq_f64(acc2, acc3));
    }
    else
    {
        switch(neonlen)
        {
        case 0:
            acc = vdupq_n_f64(0.0);
            break;
        case 1:
            acc = vmulq_f64(pX[0], pX[0]);
            break;
        case 2:
            acc = vaddq_f64(vmulq_f64(pX[0], pX[0]), vmulq_f64(pX[1], pX[1]));
            break;
        default:
            acc = vaddq_f64(
                vaddq_f64(vmulq_f64(pX[0], pX[0]), vmulq_f64(pX[1], pX[1])),
                vmulq_f64(pX[2], pX[2]));
            break;
        }
    }

    {
        double scalar = vgetq_lane_f64(acc, 0) + vgetq_lane_f64(acc, 1);
        const ae_int_t tail = neonlen<<1;
        if( n-tail )
            scalar += px[tail]*px[tail];
        return scalar;
    }
}

double rdotv_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_state *_state)
{
    if( n==0 )
        return 0;
    return rdot_neon(n, x->ptr.p_double, y->ptr.p_double, _state);
}

double rdotvr_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_state *_state)
{
    if( n==0 )
        return 0;
    return rdot_neon(n, x->ptr.p_double, a->ptr.pp_double[i], _state);
}

double rdotrr_neon(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_state *_state)
{
    if( n==0 )
        return 0;
    return rdot_neon(n, a->ptr.pp_double[ia], b->ptr.pp_double[ib], _state);
}

double rdotv2_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return 0;
    return rdotv2_neon_impl(n, x->ptr.p_double, _state);
}

static void rcopyv_neon_aligned(ae_int_t n,
     const double* __restrict x,
     double* __restrict y)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
        vst1q_f64(y+i, vld1q_f64(x+i));
    if( n-vec_len )
        y[vec_len] = x[vec_len];
}

void rcopyv_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    if( n==0 )
        return;
    rcopyv_neon_aligned(n, x->ptr.p_double, y->ptr.p_double);
}

void rcopyvr_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state)
{
    if( n==0 )
        return;
    rcopyv_neon_aligned(n, x->ptr.p_double, a->ptr.pp_double[i]);
}

void rcopyrv_neon(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rcopyv_neon_aligned(n, a->ptr.pp_double[i], x->ptr.p_double);
}

void rcopyrr_neon(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_matrix* b,
     ae_int_t k,
     ae_state *_state)
{
    if( n==0 )
        return;
    rcopyv_neon_aligned(n, a->ptr.pp_double[i], b->ptr.pp_double[k]);
}

static void rcopymulv_neon_aligned(ae_int_t n,
     double v,
     const double* __restrict x,
     double* __restrict y)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;
    const float64x2_t neonv = vdupq_n_f64(v);

    for(i=0; i<vec_len; i+=2)
        vst1q_f64(y+i, vmulq_f64(neonv, vld1q_f64(x+i)));
    if( n-vec_len )
        y[vec_len] = v*x[vec_len];
}

void rcopymulv_neon(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    if( n==0 )
        return;
    rcopymulv_neon_aligned(n, v, x->ptr.p_double, y->ptr.p_double);
}

void rcopymulvr_neon(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t ridx,
     ae_state *_state)
{
    if( n==0 )
        return;
    rcopymulv_neon_aligned(n, v, x->ptr.p_double, y->ptr.pp_double[ridx]);
}

void rcopyrcpv_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;
    const double* __restrict px = x->ptr.p_double;
    double* __restrict py = y->ptr.p_double;
    const ae_int_t vec_len = (n>>1)<<1;
    const float64x2_t neonone = vdupq_n_f64(1.0);

    for(i=0; i<vec_len; i+=2)
        vst1q_f64(py+i, vdivq_f64(neonone, vld1q_f64(px+i)));
    if( n-vec_len )
        py[vec_len] = 1.0/px[vec_len];
}

void rcopyrcpvx_neon(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state)
{
    ae_int_t i;
    const double* __restrict px = x->ptr.p_double+offsx;
    double* __restrict py = y->ptr.p_double+offsy;
    const ae_int_t vec_len = (n>>1)<<1;
    const float64x2_t neonone = vdupq_n_f64(1.0);

    for(i=0; i<vec_len; i+=2)
        vst1q_f64(py+i, vdivq_f64(neonone, vld1q_f64(px+i)));
    if( n-vec_len )
        py[vec_len] = 1.0/px[vec_len];
}

static void rcopyvx_neon_xaligned(ae_int_t n,
     const double* __restrict x,
     double* __restrict y,
     ae_state *_state)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
        vst1q_f64(y+i, vld1q_f64(x+i));
    if( n-vec_len )
        y[i] = x[i];
}

static void rcopyvx_neon_impl(ae_int_t n,
     const double* __restrict x,
     double* __restrict y,
     ae_state *_state)
{
    if((((ptrdiff_t)x) & 15) == 0)
    {
        rcopyvx_neon_xaligned(n, x, y, _state);
        return;
    }
    y[0] = x[0];
    rcopyvx_neon_xaligned(n-1, x+1, y+1, _state);
}

void rcopyvx_neon(ae_int_t n,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* y,
     ae_int_t offsy,
     ae_state *_state)
{
    if( n==0 )
        return;
    rcopyvx_neon_impl(n, x->ptr.p_double+offsx, y->ptr.p_double+offsy, _state);
}

static void icopyvx_neon_xaligned(ae_int_t n,
     const ae_int_t* __restrict x,
     ae_int_t* __restrict y,
     ae_state* __restrict _state)
{
    ae_int_t i = 0;

    (void)_state;

    if( sizeof(ae_int_t)==8 )
    {
        const ae_int_t vec_len = (n>>1)<<1;
        for(; i<vec_len; i+=2)
            vst1q_s64((int64_t*)(y+i), vld1q_s64((const int64_t*)(x+i)));
    }
    else
    {
        const ae_int_t vec_len = (n>>2)<<2;
        for(; i<vec_len; i+=4)
            vst1q_s32((int32_t*)(y+i), vld1q_s32((const int32_t*)(x+i)));
    }
    for(; i<n; i++)
        y[i] = x[i];
}

static void icopyvx_neon_impl(ae_int_t n,
     const ae_int_t* __restrict x,
     ae_int_t* __restrict y,
     ae_state* __restrict _state)
{
    const ptrdiff_t unal = ((ptrdiff_t)x) & 15;

    if( n<=8 )
    {
        ae_int_t i;
        for(i=0; i<=n-1; i++)
            y[i] = x[i];
        return;
    }
    if( unal==0 )
    {
        icopyvx_neon_xaligned(n, x, y, _state);
        return;
    }

    {
        const ae_int_t offset = 16-unal;
        memmove(y, x, offset);
        {
            const ae_int_t n_done = offset/sizeof(ae_int_t);
            icopyvx_neon_xaligned(n-n_done, x+n_done, y+n_done, _state);
        }
    }
}

void icopyvx_neon(ae_int_t n,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* y,
     ae_int_t offsy,
     ae_state *_state)
{
    if( n==0 )
        return;
    icopyvx_neon_impl(n, x->ptr.p_int+offsx, y->ptr.p_int+offsy, _state);
}

static void icopyv_neon_aligned(ae_int_t n,
     const ae_int_t* __restrict x,
     ae_int_t* __restrict y)
{
    ae_int_t i;
    const ae_int_t nbytes = n*(ae_int_t)sizeof(ae_int_t);
    const ae_int_t vecbytes = (nbytes>>4)<<4;
    const uint8_t* __restrict src = (const uint8_t*)x;
    uint8_t* __restrict dst = (uint8_t*)y;

    for(i=0; i<vecbytes; i+=16)
        vst1q_u8(dst+i, vld1q_u8(src+i));
    for(; i<nbytes; i++)
        dst[i] = src[i];
}

void icopyv_neon(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state)
{
    if( n==0 )
        return;
    icopyv_neon_aligned(n, x->ptr.p_int, y->ptr.p_int);
}

static void bcopyv_neon_aligned(ae_int_t n,
     const ae_bool* __restrict x,
     ae_bool* __restrict y)
{
    ae_int_t i;
    const ae_int_t vecbytes = (n>>4)<<4;
    const uint8_t* __restrict src = (const uint8_t*)x;
    uint8_t* __restrict dst = (uint8_t*)y;

    for(i=0; i<vecbytes; i+=16)
        vst1q_u8(dst+i, vld1q_u8(src+i));
    for(; i<n; i++)
        dst[i] = src[i];
}

void bcopyv_neon(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state)
{
    if( n==0 )
        return;
    bcopyv_neon_aligned(n, x->ptr.p_bool, y->ptr.p_bool);
}

static void rsetv_neon_aligned(ae_int_t n,
     double v,
     double* __restrict x)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;
    const float64x2_t neonv = vdupq_n_f64(v);

    for(i=0; i<vec_len; i+=2)
        vst1q_f64(x+i, neonv);
    if( n-vec_len )
        x[vec_len] = v;
}

void rsetv_neon(ae_int_t n,
     double v,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rsetv_neon_aligned(n, v, x->ptr.p_double);
}

void rsetr_neon(ae_int_t n,
     double v,
     ae_matrix* a,
     ae_int_t i,
     ae_state *_state)
{
    if( n==0 )
        return;
    rsetv_neon_aligned(n, v, a->ptr.pp_double[i]);
}

static void rsetvx_neon_impl(ae_int_t n,
     double v,
     double* __restrict x,
     const ae_state* __restrict _state)
{
    if( n<=4 )
    {
        ae_int_t j;
        for(j=0; j<=n-1; j++)
            x[j] = v;
        return;
    }
    if((((ptrdiff_t)x) & 15) == 0)
    {
        rsetv_neon_aligned(n, v, x);
        return;
    }
    x[0] = v;
    rsetv_neon_aligned(n-1, v, x+1);
}

void rsetvx_neon(ae_int_t n,
     double v,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    if( n==0 )
        return;
    rsetvx_neon_impl(n, v, x->ptr.p_double+offsx, _state);
}

static void isetv_neon_aligned(ae_int_t n,
     ae_int_t v,
     ae_int_t* __restrict x)
{
    ae_int_t i = 0;
    if( sizeof(ae_int_t)==8 )
    {
        const ae_int_t vec_len = (n>>1)<<1;
        const int64x2_t neonv = vdupq_n_s64((int64_t)v);
        for(; i<vec_len; i+=2)
            vst1q_s64((int64_t*)(x+i), neonv);
        if( n-vec_len )
            x[vec_len] = v;
        return;
    }
    else
    {
        const ae_int_t vec_len = (n>>2)<<2;
        const int32x4_t neonv = vdupq_n_s32((int32_t)v);
        for(; i<vec_len; i+=4)
            vst1q_s32((int32_t*)(x+i), neonv);
        for(; i<n; i++)
            x[i] = v;
    }
}

void isetv_neon(ae_int_t n, ae_int_t v, ae_vector* x, ae_state *_state)
{
    if( n==0 )
        return;
    isetv_neon_aligned(n, v, x->ptr.p_int);
}

static void bsetv_neon_aligned(ae_int_t n,
     ae_bool v,
     ae_bool* __restrict x)
{
    ae_int_t i;
    const ae_int_t vecbytes = (n>>4)<<4;
    const uint8x16_t neonv = vdupq_n_u8((uint8_t)v);

    for(i=0; i<vecbytes; i+=16)
        vst1q_u8((uint8_t*)x+i, neonv);
    memset(x+vecbytes, v, n-vecbytes);
}

void bsetv_neon(ae_int_t n, ae_bool v, ae_vector* x, ae_state *_state)
{
    if( n==0 )
        return;
    bsetv_neon_aligned(n, v, x->ptr.p_bool);
}

static void rmulv_neon_aligned(ae_int_t n,
     double v,
     double* __restrict x)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;
    const float64x2_t neonv = vdupq_n_f64(v);

    for(i=0; i<vec_len; i+=2)
        vst1q_f64(x+i, vmulq_f64(neonv, vld1q_f64(x+i)));
    if( n-vec_len )
        x[vec_len] *= v;
}

void rmulv_neon(ae_int_t n,
     double v,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmulv_neon_aligned(n, v, x->ptr.p_double);
}

void rmulr_neon(ae_int_t n,
     double v,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmulv_neon_aligned(n, v, x->ptr.pp_double[rowidx]);
}

static void rsqrtv_neon_aligned(ae_int_t n,
     double* __restrict x)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
        vst1q_f64(x+i, vsqrtq_f64(vld1q_f64(x+i)));
    for(; i<n; i++)
        x[i] = sqrt(x[i]);
}

void rsqrtv_neon(ae_int_t n,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rsqrtv_neon_aligned(n, x->ptr.p_double);
}

void rsqrtr_neon(ae_int_t n,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    if( n==0 )
        return;
    rsqrtv_neon_aligned(n, x->ptr.pp_double[rowidx]);
}

static void rmulvx_neon_impl(ae_int_t n,
     double v,
     double* __restrict x,
     const ae_state* __restrict _state)
{
    (void)_state;
    if( n<=4 )
    {
        ae_int_t i;
        for(i=0; i<=n-1; i++)
            x[i] *= v;
        return;
    }
    if((((ptrdiff_t)x) & 15) == 0)
    {
        rmulv_neon_aligned(n, v, x);
        return;
    }
    x[0] *= v;
    rmulv_neon_aligned(n-1, v, x+1);
}

void rmulvx_neon(ae_int_t n,
     double v,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmulvx_neon_impl(n, v, x->ptr.p_double+offsx, _state);
}

static void raddv_neon_aligned(ae_int_t n,
     double alpha,
     const double* __restrict y,
     double* __restrict x)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;
    const float64x2_t alphav = vdupq_n_f64(alpha);

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(x+i);
        const float64x2_t yv = vld1q_f64(y+i);
        vst1q_f64(x+i, vfmaq_f64(xv, yv, alphav));
    }
    for(; i<n; i++)
        x[i] += alpha*y[i];
}

void raddv_neon(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    raddv_neon_aligned(n, alpha, y->ptr.p_double, x->ptr.p_double);
}

void raddvr_neon(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    if( n==0 )
        return;
    raddv_neon_aligned(n, alpha, y->ptr.p_double, x->ptr.pp_double[rowidx]);
}

void raddrv_neon(ae_int_t n,
     double alpha,
     const ae_matrix* y,
     ae_int_t ridx,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    raddv_neon_aligned(n, alpha, y->ptr.pp_double[ridx], x->ptr.p_double);
}

void raddrr_neon(ae_int_t n,
     double alpha,
     const ae_matrix* y,
     ae_int_t ridxsrc,
     ae_matrix* x,
     ae_int_t ridxdst,
     ae_state *_state)
{
    if( n==0 )
        return;
    raddv_neon_aligned(n, alpha, y->ptr.pp_double[ridxsrc], x->ptr.pp_double[ridxdst]);
}

static void raddvx_neon_xaligned(ae_int_t n,
     double alpha,
     const double* __restrict y,
     double* __restrict x,
     ae_state* __restrict _state)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;
    const float64x2_t alphav = vdupq_n_f64(alpha);

    (void)_state;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(x+i);
        const float64x2_t yv = vld1q_f64(y+i);
        vst1q_f64(x+i, vfmaq_f64(xv, yv, alphav));
    }
    for(; i<n; i++)
        x[i] += alpha*y[i];
}

static void raddvx_neon_impl(ae_int_t n,
     double alpha,
     const double* __restrict y,
     double* __restrict x,
     ae_state* __restrict _state)
{
    if( n<=4 )
    {
        ae_int_t i;
        for(i=0; i<=n-1; i++)
            x[i] += alpha*y[i];
        return;
    }
    if((((ptrdiff_t)x) & 15) == 0)
    {
        raddvx_neon_xaligned(n, alpha, y, x, _state);
        return;
    }
    x[0] += alpha*y[0];
    raddvx_neon_xaligned(n-1, alpha, y+1, x+1, _state);
}

void raddvx_neon(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_int_t offsy,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    if( n==0 )
        return;
    raddvx_neon_impl(n, alpha, y->ptr.p_double+offsy, x->ptr.p_double+offsx, _state);
}

static void rmuladdv_neon_aligned(ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     double* __restrict x)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(x+i);
        const float64x2_t yv = vld1q_f64(y+i);
        const float64x2_t zv = vld1q_f64(z+i);
        vst1q_f64(x+i, vfmaq_f64(xv, yv, zv));
    }
    for(; i<n; i++)
        x[i] += y[i]*z[i];
}

void rmuladdv_neon(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmuladdv_neon_aligned(n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double);
}

void rmuladdvx_neon(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    ae_int_t i;
    const double* __restrict py = y->ptr.p_double+offsy;
    const double* __restrict pz = z->ptr.p_double+offsz;
    double* __restrict px = x->ptr.p_double+offsx;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(px+i);
        const float64x2_t yv = vld1q_f64(py+i);
        const float64x2_t zv = vld1q_f64(pz+i);
        vst1q_f64(px+i, vfmaq_f64(xv, yv, zv));
    }
    for(; i<n; i++)
        px[i] += py[i]*pz[i];
}

static void rnegmuladdv_neon_aligned(ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     double* __restrict x)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(x+i);
        const float64x2_t yv = vld1q_f64(y+i);
        const float64x2_t zv = vld1q_f64(z+i);
        vst1q_f64(x+i, vfmsq_f64(xv, yv, zv));
    }
    for(; i<n; i++)
        x[i] -= y[i]*z[i];
}

void rnegmuladdv_neon(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rnegmuladdv_neon_aligned(n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double);
}

void rnegmuladdvx_neon(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    ae_int_t i;
    const double* __restrict py = y->ptr.p_double+offsy;
    const double* __restrict pz = z->ptr.p_double+offsz;
    double* __restrict px = x->ptr.p_double+offsx;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(px+i);
        const float64x2_t yv = vld1q_f64(py+i);
        const float64x2_t zv = vld1q_f64(pz+i);
        vst1q_f64(px+i, vfmsq_f64(xv, yv, zv));
    }
    for(; i<n; i++)
        px[i] -= py[i]*pz[i];
}

static void rcopymuladdv_neon_aligned(ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     const double* __restrict x,
     double* __restrict r)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(x+i);
        const float64x2_t yv = vld1q_f64(y+i);
        const float64x2_t zv = vld1q_f64(z+i);
        vst1q_f64(r+i, vfmaq_f64(xv, yv, zv));
    }
    for(; i<n; i++)
        r[i] = x[i]+y[i]*z[i];
}

void rcopymuladdv_neon(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     const ae_vector* x,
     ae_vector* r,
     ae_state *_state)
{
    if( n==0 )
        return;
    rcopymuladdv_neon_aligned(n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double, r->ptr.p_double);
}

void rcopymuladdvx_neon(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* r,
     ae_int_t offsr,
     ae_state *_state)
{
    ae_int_t i;
    const double* __restrict py = y->ptr.p_double+offsy;
    const double* __restrict pz = z->ptr.p_double+offsz;
    const double* __restrict px = x->ptr.p_double+offsx;
    double* __restrict pr = r->ptr.p_double+offsr;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(px+i);
        const float64x2_t yv = vld1q_f64(py+i);
        const float64x2_t zv = vld1q_f64(pz+i);
        vst1q_f64(pr+i, vfmaq_f64(xv, yv, zv));
    }
    for(; i<n; i++)
        pr[i] = px[i]+py[i]*pz[i];
}

static void rcopynegmuladdv_neon_aligned(ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     const double* __restrict x,
     double* __restrict r)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(x+i);
        const float64x2_t yv = vld1q_f64(y+i);
        const float64x2_t zv = vld1q_f64(z+i);
        vst1q_f64(r+i, vfmsq_f64(xv, yv, zv));
    }
    for(; i<n; i++)
        r[i] = x[i]-y[i]*z[i];
}

void rcopynegmuladdv_neon(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     const ae_vector* x,
     ae_vector* r,
     ae_state *_state)
{
    if( n==0 )
        return;
    rcopynegmuladdv_neon_aligned(n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double, r->ptr.p_double);
}

void rcopynegmuladdvx_neon(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* r,
     ae_int_t offsr,
     ae_state *_state)
{
    ae_int_t i;
    const double* __restrict py = y->ptr.p_double+offsy;
    const double* __restrict pz = z->ptr.p_double+offsz;
    const double* __restrict px = x->ptr.p_double+offsx;
    double* __restrict pr = r->ptr.p_double+offsr;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(px+i);
        const float64x2_t yv = vld1q_f64(py+i);
        const float64x2_t zv = vld1q_f64(pz+i);
        vst1q_f64(pr+i, vfmsq_f64(xv, yv, zv));
    }
    for(; i<n; i++)
        pr[i] = px[i]-py[i]*pz[i];
}

static void rmergemulv_neon_aligned(ae_int_t n,
     const double* __restrict y,
     double* __restrict x)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(x+i);
        const float64x2_t yv = vld1q_f64(y+i);
        vst1q_f64(x+i, vmulq_f64(xv, yv));
    }
    if( n-vec_len )
        x[vec_len] *= y[vec_len];
}

void rmergemulv_neon(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergemulv_neon_aligned(n, y->ptr.p_double, x->ptr.p_double);
}

void rmergemulvx_neon(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    ae_int_t i;
    const double* __restrict py = y->ptr.p_double+offsy;
    double* __restrict px = x->ptr.p_double+offsx;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(px+i);
        const float64x2_t yv = vld1q_f64(py+i);
        vst1q_f64(px+i, vmulq_f64(xv, yv));
    }
    if( n-vec_len )
        px[vec_len] *= py[vec_len];
}

void rmergemulvr_neon(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergemulv_neon_aligned(n, y->ptr.p_double, x->ptr.pp_double[rowidx]);
}

void rmergemulrv_neon(ae_int_t n,
     const ae_matrix* y,
     ae_int_t rowidx,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergemulv_neon_aligned(n, y->ptr.pp_double[rowidx], x->ptr.p_double);
}

static void rmergedivv_neon_aligned(ae_int_t n,
     const double* __restrict y,
     double* __restrict x)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(x+i);
        const float64x2_t yv = vld1q_f64(y+i);
        vst1q_f64(x+i, vdivq_f64(xv, yv));
    }
    if( n-vec_len )
        x[vec_len] /= y[vec_len];
}

void rmergedivv_neon(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergedivv_neon_aligned(n, y->ptr.p_double, x->ptr.p_double);
}

void rmergedivvr_neon(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergedivv_neon_aligned(n, y->ptr.p_double, x->ptr.pp_double[rowidx]);
}

void rmergedivrv_neon(ae_int_t n,
     const ae_matrix* y,
     ae_int_t rowidx,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergedivv_neon_aligned(n, y->ptr.pp_double[rowidx], x->ptr.p_double);
}

static void rmergemaxv_neon_aligned(ae_int_t n,
     const double* __restrict y,
     double* __restrict x,
     ae_state* __restrict _state)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    (void)_state;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(x+i);
        const float64x2_t yv = vld1q_f64(y+i);
        vst1q_f64(x+i, vmaxq_f64(xv, yv));
    }
    if( n-vec_len )
        x[vec_len] = x[vec_len]>y[vec_len] ? x[vec_len] : y[vec_len];
}

void rmergemaxv_neon(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergemaxv_neon_aligned(n, y->ptr.p_double, x->ptr.p_double, _state);
}

void rmergemaxvr_neon(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergemaxv_neon_aligned(n, y->ptr.p_double, x->ptr.pp_double[rowidx], _state);
}

void rmergemaxrv_neon(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_vector* y,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergemaxv_neon_aligned(n, x->ptr.pp_double[rowidx], y->ptr.p_double, _state);
}

static void rmergeminv_neon_aligned(ae_int_t n,
     const double* __restrict y,
     double* __restrict x,
     ae_state* __restrict _state)
{
    ae_int_t i;
    const ae_int_t vec_len = (n>>1)<<1;

    (void)_state;

    for(i=0; i<vec_len; i+=2)
    {
        const float64x2_t xv = vld1q_f64(x+i);
        const float64x2_t yv = vld1q_f64(y+i);
        vst1q_f64(x+i, vminq_f64(xv, yv));
    }
    if( n-vec_len )
        x[vec_len] = x[vec_len]>y[vec_len] ? y[vec_len] : x[vec_len];
}

void rmergeminv_neon(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergeminv_neon_aligned(n, y->ptr.p_double, x->ptr.p_double, _state);
}

void rmergeminvr_neon(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergeminv_neon_aligned(n, y->ptr.p_double, x->ptr.pp_double[rowidx], _state);
}

void rmergeminrv_neon(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_vector* y,
     ae_state *_state)
{
    if( n==0 )
        return;
    rmergeminv_neon_aligned(n, x->ptr.pp_double[rowidx], y->ptr.p_double, _state);
}

static double rmaxv_neon_aligned(ae_int_t n,
     const double* __restrict x,
     ae_state* __restrict _state)
{
    ae_int_t i;

    (void)_state;

    if( n<=4 )
    {
        double result;
        if( n==0 )
            return 0.0;
        result = x[0];
        for(i=1; i<n; i++)
        {
            if( x[i]>result )
                result = x[i];
        }
        return result;
    }

    const ae_int_t vec_len = (n>>1)<<1;
    float64x2_t cur_max = vld1q_f64(x);
    for(i=2; i<vec_len; i+=2)
        cur_max = vmaxq_f64(cur_max, vld1q_f64(x+i));

    {
        double result = vgetq_lane_f64(cur_max, 0);
        double lane1 = vgetq_lane_f64(cur_max, 1);
        if( lane1>result )
            result = lane1;
        if( n-vec_len && x[vec_len]>result )
            result = x[vec_len];
        return result;
    }
}

double rmaxv_neon(ae_int_t n, const ae_vector* x, ae_state *_state)
{
    if( n==0 )
        return 0.0;
    return rmaxv_neon_aligned(n, x->ptr.p_double, _state);
}

double rmaxr_neon(ae_int_t n, const ae_matrix* x, ae_int_t rowidx, ae_state *_state)
{
    if( n==0 )
        return 0.0;
    return rmaxv_neon_aligned(n, x->ptr.pp_double[rowidx], _state);
}

static double rmaxabsv_neon_aligned(ae_int_t n,
     const double* __restrict x,
     ae_state* __restrict _state)
{
    ae_int_t i;

    (void)_state;

    if( n<=4 )
    {
        double result = 0.0;
        for(i=0; i<n; i++)
        {
            double v = rabs_neon_scalar(x[i]);
            if( v>result )
                result = v;
        }
        return result;
    }

    const ae_int_t vec_len = (n>>1)<<1;
    uint64x2_t sign_mask = vdupq_n_u64(0x7fffffffffffffffULL);
    float64x2_t cur_max = vreinterpretq_f64_u64(vandq_u64(vreinterpretq_u64_f64(vld1q_f64(x)), sign_mask));
    for(i=2; i<vec_len; i+=2)
    {
        float64x2_t av = vreinterpretq_f64_u64(vandq_u64(vreinterpretq_u64_f64(vld1q_f64(x+i)), sign_mask));
        cur_max = vmaxq_f64(cur_max, av);
    }

    {
        double result = vgetq_lane_f64(cur_max, 0);
        double lane1 = vgetq_lane_f64(cur_max, 1);
        if( lane1>result )
            result = lane1;
        if( n-vec_len )
        {
            double tail = rabs_neon_scalar(x[vec_len]);
            if( tail>result )
                result = tail;
        }
        return result;
    }
}

double rmaxabsv_neon(ae_int_t n, const ae_vector* x, ae_state *_state)
{
    if( n==0 )
        return 0.0;
    return rmaxabsv_neon_aligned(n, x->ptr.p_double, _state);
}

double rmaxabsr_neon(ae_int_t n, const ae_matrix* x, ae_int_t rowidx, ae_state *_state)
{
    if( n==0 )
        return 0.0;
    return rmaxabsv_neon_aligned(n, x->ptr.pp_double[rowidx], _state);
}

/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_NEON_INTRINSICS */
#endif


}

