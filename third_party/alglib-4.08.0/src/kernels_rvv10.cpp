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

#include "kernels_rvv10.h"

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

/*
 * Include RVV headers
 */
#if defined(AE_USE_CPP)
} /* namespace alglib_impl */
#endif
#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_RVV10_INTRINSICS)
#include <stdint.h>
#include <riscv_vector.h>
#endif
#if defined(AE_USE_CPP)
namespace alglib_impl {
#endif


#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_RVV10_INTRINSICS)

static double rdot_rvv10(ae_int_t n,
     const double* __restrict px,
     const double* __restrict py,
     ae_state *_state)
{
    ae_int_t i;
    size_t vlmax;
    vfloat64m1_t acc;
    vfloat64m1_t sum;

    (void)_state;

    vlmax = __riscv_vsetvlmax_e64m1();
    acc = __riscv_vfmv_v_f_f64m1(0.0, vlmax);
    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(px+i, vl);
        const vfloat64m1_t yv = __riscv_vle64_v_f64m1(py+i, vl);

        acc = __riscv_vfmacc_vv_f64m1(acc, xv, yv, vl);
        i += (ae_int_t)vl;
    }
    sum = __riscv_vfmv_v_f_f64m1(0.0, vlmax);
    sum = __riscv_vfredusum_vs_f64m1_f64m1(acc, sum, vlmax);
    return __riscv_vfmv_f_s_f64m1_f64(sum);
}

static double rdotv2_rvv10_impl(ae_int_t n,
     const double* __restrict px,
     ae_state *_state)
{
    ae_int_t i;
    size_t vlmax;
    vfloat64m1_t acc;
    vfloat64m1_t sum;

    (void)_state;

    vlmax = __riscv_vsetvlmax_e64m1();
    acc = __riscv_vfmv_v_f_f64m1(0.0, vlmax);
    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(px+i, vl);

        acc = __riscv_vfmacc_vv_f64m1(acc, xv, xv, vl);
        i += (ae_int_t)vl;
    }
    sum = __riscv_vfmv_v_f_f64m1(0.0, vlmax);
    sum = __riscv_vfredusum_vs_f64m1_f64m1(acc, sum, vlmax);
    return __riscv_vfmv_f_s_f64m1_f64(sum);
}

double rdotv_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_state *_state)
{
    if( n==0 )
        return 0.0;
    return rdot_rvv10(n, x->ptr.p_double, y->ptr.p_double, _state);
}

double rdotvr_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_state *_state)
{
    if( n==0 )
        return 0.0;
    return rdot_rvv10(n, x->ptr.p_double, a->ptr.pp_double[i], _state);
}

double rdotrr_rvv10(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_state *_state)
{
    if( n==0 )
        return 0.0;
    return rdot_rvv10(n, a->ptr.pp_double[ia], b->ptr.pp_double[ib], _state);
}

double rdotv2_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    if( n==0 )
        return 0.0;
    return rdotv2_rvv10_impl(n, x->ptr.p_double, _state);
}

static void rcopyv_rvv10_impl(ae_int_t n,
     const double* __restrict x,
     double* __restrict y)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        __riscv_vse64_v_f64m1(y+i, xv, vl);
        i += (ae_int_t)vl;
    }
}

static void rcopymulv_rvv10_impl(ae_int_t n,
     double v,
     const double* __restrict x,
     double* __restrict y)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        __riscv_vse64_v_f64m1(y+i, __riscv_vfmul_vf_f64m1(xv, v, vl), vl);
        i += (ae_int_t)vl;
    }
}

static void rcopyrcpv_rvv10_impl(ae_int_t n,
     const double* __restrict x,
     double* __restrict y)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        const vfloat64m1_t one = __riscv_vfmv_v_f_f64m1(1.0, vl);
        __riscv_vse64_v_f64m1(y+i, __riscv_vfdiv_vv_f64m1(one, xv, vl), vl);
        i += (ae_int_t)vl;
    }
}

static void rvv10_copy_bytes(ae_int_t nbytes,
     const void* __restrict x,
     void* __restrict y)
{
    ae_int_t i;
    const uint8_t* __restrict px = (const uint8_t*)x;
    uint8_t* __restrict py = (uint8_t*)y;

    for(i=0; i<nbytes; )
    {
        const size_t vl = __riscv_vsetvl_e8m8((size_t)(nbytes-i));
        const vuint8m8_t xv = __riscv_vle8_v_u8m8(px+i, vl);
        __riscv_vse8_v_u8m8(py+i, xv, vl);
        i += (ae_int_t)vl;
    }
}

static void rsetv_rvv10_impl(ae_int_t n,
     double v,
     double* __restrict x)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t xv = __riscv_vfmv_v_f_f64m1(v, vl);
        __riscv_vse64_v_f64m1(x+i, xv, vl);
        i += (ae_int_t)vl;
    }
}

static void isetv_rvv10_impl(ae_int_t n,
     ae_int_t v,
     ae_int_t* __restrict x)
{
    ae_int_t i;

    if( sizeof(ae_int_t)==8 )
    {
        for(i=0; i<n; )
        {
            const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
            const vint64m1_t xv = __riscv_vmv_v_x_i64m1((int64_t)v, vl);
            __riscv_vse64_v_i64m1((int64_t*)(x+i), xv, vl);
            i += (ae_int_t)vl;
        }
    }
    else
    {
        for(i=0; i<n; )
        {
            const size_t vl = __riscv_vsetvl_e32m1((size_t)(n-i));
            const vint32m1_t xv = __riscv_vmv_v_x_i32m1((int32_t)v, vl);
            __riscv_vse32_v_i32m1((int32_t*)(x+i), xv, vl);
            i += (ae_int_t)vl;
        }
    }
}

static void bsetv_rvv10_impl(ae_int_t n,
     ae_bool v,
     ae_bool* __restrict x)
{
    ae_int_t i;
    const ae_int_t nbytes = n*(ae_int_t)sizeof(ae_bool);
    uint8_t* __restrict px = (uint8_t*)x;

    for(i=0; i<nbytes; )
    {
        const size_t vl = __riscv_vsetvl_e8m8((size_t)(nbytes-i));
        const vuint8m8_t xv = __riscv_vmv_v_x_u8m8((uint8_t)v, vl);
        __riscv_vse8_v_u8m8(px+i, xv, vl);
        i += (ae_int_t)vl;
    }
}

static void rmulv_rvv10_impl(ae_int_t n,
     double v,
     double* __restrict x)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        __riscv_vse64_v_f64m1(x+i, __riscv_vfmul_vf_f64m1(xv, v, vl), vl);
        i += (ae_int_t)vl;
    }
}

static void rsqrtv_rvv10_impl(ae_int_t n,
     double* __restrict x)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        __riscv_vse64_v_f64m1(x+i, __riscv_vfsqrt_v_f64m1(xv, vl), vl);
        i += (ae_int_t)vl;
    }
}

static void raddv_rvv10_impl(ae_int_t n,
     double alpha,
     const double* __restrict y,
     double* __restrict x)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t yv = __riscv_vle64_v_f64m1(y+i, vl);
        vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);

        xv = __riscv_vfmacc_vf_f64m1(xv, alpha, yv, vl);
        __riscv_vse64_v_f64m1(x+i, xv, vl);
        i += (ae_int_t)vl;
    }
}

static void rmuladdv_rvv10_impl(ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     double* __restrict x)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t yv = __riscv_vle64_v_f64m1(y+i, vl);
        const vfloat64m1_t zv = __riscv_vle64_v_f64m1(z+i, vl);
        vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);

        xv = __riscv_vfmacc_vv_f64m1(xv, yv, zv, vl);
        __riscv_vse64_v_f64m1(x+i, xv, vl);
        i += (ae_int_t)vl;
    }
}

static void rnegmuladdv_rvv10_impl(ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     double* __restrict x)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t yv = __riscv_vle64_v_f64m1(y+i, vl);
        const vfloat64m1_t zv = __riscv_vle64_v_f64m1(z+i, vl);
        vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);

        xv = __riscv_vfnmsac_vv_f64m1(xv, yv, zv, vl);
        __riscv_vse64_v_f64m1(x+i, xv, vl);
        i += (ae_int_t)vl;
    }
}

static void rcopymuladdv_rvv10_impl(ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     const double* __restrict x,
     double* __restrict r)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t yv = __riscv_vle64_v_f64m1(y+i, vl);
        const vfloat64m1_t zv = __riscv_vle64_v_f64m1(z+i, vl);
        vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);

        xv = __riscv_vfmacc_vv_f64m1(xv, yv, zv, vl);
        __riscv_vse64_v_f64m1(r+i, xv, vl);
        i += (ae_int_t)vl;
    }
}

static void rcopynegmuladdv_rvv10_impl(ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     const double* __restrict x,
     double* __restrict r)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t yv = __riscv_vle64_v_f64m1(y+i, vl);
        const vfloat64m1_t zv = __riscv_vle64_v_f64m1(z+i, vl);
        vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);

        xv = __riscv_vfnmsac_vv_f64m1(xv, yv, zv, vl);
        __riscv_vse64_v_f64m1(r+i, xv, vl);
        i += (ae_int_t)vl;
    }
}

static void rmergemulv_rvv10_impl(ae_int_t n,
     const double* __restrict y,
     double* __restrict x)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t yv = __riscv_vle64_v_f64m1(y+i, vl);
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        __riscv_vse64_v_f64m1(x+i, __riscv_vfmul_vv_f64m1(xv, yv, vl), vl);
        i += (ae_int_t)vl;
    }
}

static void rmergedivv_rvv10_impl(ae_int_t n,
     const double* __restrict y,
     double* __restrict x)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t yv = __riscv_vle64_v_f64m1(y+i, vl);
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        __riscv_vse64_v_f64m1(x+i, __riscv_vfdiv_vv_f64m1(xv, yv, vl), vl);
        i += (ae_int_t)vl;
    }
}

static void rmergemaxv_rvv10_impl(ae_int_t n,
     const double* __restrict y,
     double* __restrict x)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t yv = __riscv_vle64_v_f64m1(y+i, vl);
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        __riscv_vse64_v_f64m1(x+i, __riscv_vfmax_vv_f64m1(xv, yv, vl), vl);
        i += (ae_int_t)vl;
    }
}

static void rmergeminv_rvv10_impl(ae_int_t n,
     const double* __restrict y,
     double* __restrict x)
{
    ae_int_t i;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t yv = __riscv_vle64_v_f64m1(y+i, vl);
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        __riscv_vse64_v_f64m1(x+i, __riscv_vfmin_vv_f64m1(xv, yv, vl), vl);
        i += (ae_int_t)vl;
    }
}

static double rmaxv_rvv10_impl(ae_int_t n,
     const double* __restrict x)
{
    ae_int_t i;
    const size_t vlmax = __riscv_vsetvlmax_e64m1();
    vfloat64m1_t acc = __riscv_vfmv_v_f_f64m1(x[0], vlmax);
    vfloat64m1_t result;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        acc = __riscv_vfmax_vv_f64m1(acc, xv, vl);
        i += (ae_int_t)vl;
    }
    result = __riscv_vfmv_v_f_f64m1(x[0], vlmax);
    result = __riscv_vfredmax_vs_f64m1_f64m1(acc, result, vlmax);
    return __riscv_vfmv_f_s_f64m1_f64(result);
}

static double rmaxabsv_rvv10_impl(ae_int_t n,
     const double* __restrict x)
{
    ae_int_t i;
    const size_t vlmax = __riscv_vsetvlmax_e64m1();
    vfloat64m1_t acc = __riscv_vfmv_v_f_f64m1(0.0, vlmax);
    vfloat64m1_t result;

    for(i=0; i<n; )
    {
        const size_t vl = __riscv_vsetvl_e64m1((size_t)(n-i));
        const vfloat64m1_t xv = __riscv_vle64_v_f64m1(x+i, vl);
        vuint64m1_t bits = __riscv_vreinterpret_v_f64m1_u64m1(xv);
        bits = __riscv_vand_vx_u64m1(bits, 0x7fffffffffffffffULL, vl);
        acc = __riscv_vfmax_vv_f64m1(acc, __riscv_vreinterpret_v_u64m1_f64m1(bits), vl);
        i += (ae_int_t)vl;
    }
    result = __riscv_vfmv_v_f_f64m1(0.0, vlmax);
    result = __riscv_vfredmax_vs_f64m1_f64m1(acc, result, vlmax);
    return __riscv_vfmv_f_s_f64m1_f64(result);
}

void rcopyv_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopyv_rvv10_impl(n, x->ptr.p_double, y->ptr.p_double);
}

void rcopyvr_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopyv_rvv10_impl(n, x->ptr.p_double, a->ptr.pp_double[i]);
}

void rcopyrv_rvv10(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopyv_rvv10_impl(n, a->ptr.pp_double[i], x->ptr.p_double);
}

void rcopyrr_rvv10(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_matrix* b,
     ae_int_t k,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopyv_rvv10_impl(n, a->ptr.pp_double[i], b->ptr.pp_double[k]);
}

void rcopymulv_rvv10(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopymulv_rvv10_impl(n, v, x->ptr.p_double, y->ptr.p_double);
}

void rcopymulvr_rvv10(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t ridx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopymulv_rvv10_impl(n, v, x->ptr.p_double, y->ptr.pp_double[ridx]);
}

void rcopyrcpv_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopyrcpv_rvv10_impl(n, x->ptr.p_double, y->ptr.p_double);
}

void rcopyrcpvx_rvv10(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopyrcpv_rvv10_impl(n, x->ptr.p_double+offsx, y->ptr.p_double+offsy);
}

void rcopyvx_rvv10(ae_int_t n,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* y,
     ae_int_t offsy,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopyv_rvv10_impl(n, x->ptr.p_double+offsx, y->ptr.p_double+offsy);
}

void icopyvx_rvv10(ae_int_t n,
     const ae_vector* x,
     ae_int_t offsx,
     ae_vector* y,
     ae_int_t offsy,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rvv10_copy_bytes(n*(ae_int_t)sizeof(ae_int_t), x->ptr.p_int+offsx, y->ptr.p_int+offsy);
}

void icopyv_rvv10(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rvv10_copy_bytes(n*(ae_int_t)sizeof(ae_int_t), x->ptr.p_int, y->ptr.p_int);
}

void bcopyv_rvv10(ae_int_t n, const ae_vector* x, ae_vector* y, ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rvv10_copy_bytes(n, x->ptr.p_bool, y->ptr.p_bool);
}

void rsetv_rvv10(ae_int_t n,
     double v,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rsetv_rvv10_impl(n, v, x->ptr.p_double);
}

void rsetr_rvv10(ae_int_t n,
     double v,
     ae_matrix* a,
     ae_int_t i,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rsetv_rvv10_impl(n, v, a->ptr.pp_double[i]);
}

void rsetvx_rvv10(ae_int_t n,
     double v,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rsetv_rvv10_impl(n, v, x->ptr.p_double+offsx);
}

void isetv_rvv10(ae_int_t n, ae_int_t v, ae_vector* x, ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    isetv_rvv10_impl(n, v, x->ptr.p_int);
}

void bsetv_rvv10(ae_int_t n, ae_bool v, ae_vector* x, ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    bsetv_rvv10_impl(n, v, x->ptr.p_bool);
}

void rmulv_rvv10(ae_int_t n,
     double v,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmulv_rvv10_impl(n, v, x->ptr.p_double);
}

void rmulr_rvv10(ae_int_t n,
     double v,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmulv_rvv10_impl(n, v, x->ptr.pp_double[rowidx]);
}

void rsqrtv_rvv10(ae_int_t n,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rsqrtv_rvv10_impl(n, x->ptr.p_double);
}

void rsqrtr_rvv10(ae_int_t n,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rsqrtv_rvv10_impl(n, x->ptr.pp_double[rowidx]);
}

void rmulvx_rvv10(ae_int_t n,
     double v,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmulv_rvv10_impl(n, v, x->ptr.p_double+offsx);
}

void raddv_rvv10(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    raddv_rvv10_impl(n, alpha, y->ptr.p_double, x->ptr.p_double);
}

void raddvr_rvv10(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    raddv_rvv10_impl(n, alpha, y->ptr.p_double, x->ptr.pp_double[rowidx]);
}

void raddrv_rvv10(ae_int_t n,
     double alpha,
     const ae_matrix* y,
     ae_int_t ridx,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    raddv_rvv10_impl(n, alpha, y->ptr.pp_double[ridx], x->ptr.p_double);
}

void raddrr_rvv10(ae_int_t n,
     double alpha,
     const ae_matrix* y,
     ae_int_t ridxsrc,
     ae_matrix* x,
     ae_int_t ridxdst,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    raddv_rvv10_impl(n, alpha, y->ptr.pp_double[ridxsrc], x->ptr.pp_double[ridxdst]);
}

void raddvx_rvv10(ae_int_t n,
     double alpha,
     const ae_vector* y,
     ae_int_t offsy,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    raddv_rvv10_impl(n, alpha, y->ptr.p_double+offsy, x->ptr.p_double+offsx);
}

void rmuladdv_rvv10(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmuladdv_rvv10_impl(n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double);
}

void rmuladdvx_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmuladdv_rvv10_impl(n, y->ptr.p_double+offsy, z->ptr.p_double+offsz, x->ptr.p_double+offsx);
}

void rnegmuladdv_rvv10(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rnegmuladdv_rvv10_impl(n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double);
}

void rnegmuladdvx_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     const ae_vector* z,
     ae_int_t offsz,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rnegmuladdv_rvv10_impl(n, y->ptr.p_double+offsy, z->ptr.p_double+offsz, x->ptr.p_double+offsx);
}

void rcopymuladdv_rvv10(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     const ae_vector* x,
     ae_vector* r,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopymuladdv_rvv10_impl(n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double, r->ptr.p_double);
}

void rcopymuladdvx_rvv10(ae_int_t n,
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
    (void)_state;
    if( n==0 )
        return;
    rcopymuladdv_rvv10_impl(n, y->ptr.p_double+offsy, z->ptr.p_double+offsz, x->ptr.p_double+offsx, r->ptr.p_double+offsr);
}

void rcopynegmuladdv_rvv10(ae_int_t n,
     const ae_vector* y,
     const ae_vector* z,
     const ae_vector* x,
     ae_vector* r,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rcopynegmuladdv_rvv10_impl(n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double, r->ptr.p_double);
}

void rcopynegmuladdvx_rvv10(ae_int_t n,
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
    (void)_state;
    if( n==0 )
        return;
    rcopynegmuladdv_rvv10_impl(n, y->ptr.p_double+offsy, z->ptr.p_double+offsz, x->ptr.p_double+offsx, r->ptr.p_double+offsr);
}

void rmergemulv_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergemulv_rvv10_impl(n, y->ptr.p_double, x->ptr.p_double);
}

void rmergemulvx_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_int_t offsy,
     ae_vector* x,
     ae_int_t offsx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergemulv_rvv10_impl(n, y->ptr.p_double+offsy, x->ptr.p_double+offsx);
}

void rmergemulvr_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergemulv_rvv10_impl(n, y->ptr.p_double, x->ptr.pp_double[rowidx]);
}

void rmergemulrv_rvv10(ae_int_t n,
     const ae_matrix* y,
     ae_int_t rowidx,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergemulv_rvv10_impl(n, y->ptr.pp_double[rowidx], x->ptr.p_double);
}

void rmergedivv_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergedivv_rvv10_impl(n, y->ptr.p_double, x->ptr.p_double);
}

void rmergedivvr_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergedivv_rvv10_impl(n, y->ptr.p_double, x->ptr.pp_double[rowidx]);
}

void rmergedivrv_rvv10(ae_int_t n,
     const ae_matrix* y,
     ae_int_t rowidx,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergedivv_rvv10_impl(n, y->ptr.pp_double[rowidx], x->ptr.p_double);
}

void rmergemaxv_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergemaxv_rvv10_impl(n, y->ptr.p_double, x->ptr.p_double);
}

void rmergemaxvr_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergemaxv_rvv10_impl(n, y->ptr.p_double, x->ptr.pp_double[rowidx]);
}

void rmergemaxrv_rvv10(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_vector* y,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergemaxv_rvv10_impl(n, x->ptr.pp_double[rowidx], y->ptr.p_double);
}

void rmergeminv_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergeminv_rvv10_impl(n, y->ptr.p_double, x->ptr.p_double);
}

void rmergeminvr_rvv10(ae_int_t n,
     const ae_vector* y,
     ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergeminv_rvv10_impl(n, y->ptr.p_double, x->ptr.pp_double[rowidx]);
}

void rmergeminrv_rvv10(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_vector* y,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return;
    rmergeminv_rvv10_impl(n, x->ptr.pp_double[rowidx], y->ptr.p_double);
}

double rmaxv_rvv10(ae_int_t n,
     const ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return 0.0;
    return rmaxv_rvv10_impl(n, x->ptr.p_double);
}

double rmaxr_rvv10(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return 0.0;
    return rmaxv_rvv10_impl(n, x->ptr.pp_double[rowidx]);
}

double rmaxabsv_rvv10(ae_int_t n,
     const ae_vector* x,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return 0.0;
    return rmaxabsv_rvv10_impl(n, x->ptr.p_double);
}

double rmaxabsr_rvv10(ae_int_t n,
     const ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state)
{
    (void)_state;
    if( n==0 )
        return 0.0;
    return rmaxabsv_rvv10_impl(n, x->ptr.pp_double[rowidx]);
}

/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_RVV10_INTRINSICS */
#endif


}

