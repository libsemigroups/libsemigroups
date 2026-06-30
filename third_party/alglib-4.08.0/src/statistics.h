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
#ifndef _statistics_pkg_h
#define _statistics_pkg_h
#include "ap.h"
#include "alglibinternal.h"
#include "linalg.h"
#include "alglibmisc.h"
#include "specialfunctions.h"
#include "fasttransforms.h"

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (DATATYPES)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_BASESTAT) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_JARQUEBERA) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_MANNWHITNEYU) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_STUDENTTTESTS) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_CORRELATIONTESTS) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_STEST) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_MCMC) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t x0width;
    ae_int_t x0height;
    ae_matrix x0m;
    ae_int_t x0type;
    double x0stddev;
    ae_int_t algokind;
    ae_int_t ladderkind;
    ae_int_t proposalkind;
    ae_int_t helperscnt;
    double desigma;
    double degamma0;
    ae_vector initialladder;
    double laddernu0;
    double laddertau;
    ae_bool noladderadaptationafterburnin;
    ae_matrix gausslowerc;
    ae_matrix gaussl;
    ae_bool useparallelmoves;
    ae_int_t epochscnt;
    ae_int_t popwidth;
    ae_int_t popheight;
    ae_int_t burninlen;
    ae_int_t thinby;
    ae_bool reportalllevels;
    ae_int_t rngseed;
    ae_vector s;
    ae_bool initialstart;
    ae_bool xrep;
    ae_matrix population2d;
    ae_vector currentladder;
    hqrndstate globalrs;
    ae_bool reseedglobalrs;
    ae_vector propidx;
    ae_vector propz;
    ae_vector propt;
    ae_matrix propxf;
    ae_vector grpabidx;
    ae_int_t grpasize;
    ae_vector grpdsidx;
    ae_bool haslastpopulation;
    ae_matrix lastpopulation2d;
    ae_vector lastgrpabidx;
    ae_int_t lastpopulationwidth;
    ae_int_t lastpopulationheight;
    ae_bool userterminationneeded;
    ae_int_t protocolversion;
    ae_bool issuesparserequests;
    ae_int_t repnfev;
    ae_int_t repaccept1cnt;
    ae_int_t repaccepthcnt;
    ae_int_t repepochscnt;
    ae_int_t reppopwidth;
    ae_int_t reppopheight;
    ae_int_t repswapacceptcnt;
    ae_int_t repswapattemptcnt;
    ae_vector repavgswaprates;
    ae_matrix repsample;
    ae_int_t repsamplesize;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_bool dotimers;
    stimer timertotal;
    stimer timercallback;
    stimer timerreport;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector tmpi0;
    ae_vector gphelperidx;
    ae_vector gpmeanhelper;
    ae_vector gpproposal;
    ae_vector gpdelta;
    ae_vector gptmp0;
    ae_vector saacceptrates;
    ae_vector savecsi;
    ae_vector saproposedladder;
    rcommstate rcommv2;
} mcmcstate;
typedef struct
{
    ae_int_t nfev;
    double acceptrate;
    double swapacceptrate;
    ae_vector autocorrtimes;
} mcmcreport;
#endif
#if defined(AE_COMPILE_WSR) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_VARIANCETESTS) || !defined(AE_PARTIAL_BUILD)
#endif

}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS C++ INTERFACE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib
{

#if defined(AE_COMPILE_BASESTAT) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_JARQUEBERA) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MANNWHITNEYU) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_STUDENTTTESTS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_CORRELATIONTESTS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_STEST) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MCMC) || !defined(AE_PARTIAL_BUILD)
class _mcmcstate_owner;
class mcmcstate;
class _mcmcreport_owner;
class mcmcreport;


/*************************************************************************
This object stores MCMC sampler.

You should use functions provided by the MCMC subpackage to work with this
object.
*************************************************************************/
class _mcmcstate_owner
{
public:
    _mcmcstate_owner();
    _mcmcstate_owner(alglib_impl::mcmcstate *attach_to);
    _mcmcstate_owner(const _mcmcstate_owner &rhs);
    _mcmcstate_owner& operator=(const _mcmcstate_owner &rhs);
    virtual ~_mcmcstate_owner();
    alglib_impl::mcmcstate* c_ptr();
    const alglib_impl::mcmcstate* c_ptr() const;
protected:
    alglib_impl::mcmcstate *p_struct;
    bool is_attached;
};
class mcmcstate : public _mcmcstate_owner
{
public:
    mcmcstate();
    mcmcstate(alglib_impl::mcmcstate *attach_to);
    mcmcstate(const mcmcstate &rhs);
    mcmcstate& operator=(const mcmcstate &rhs);
    virtual ~mcmcstate();


};


/*************************************************************************
These fields store MCMC report:
* nfev                      number of function evaluations
* acceptrate                acceptance rate of a MCMC algo; when  parallel
                            tempering is used, this field stores acceptance
                            rate for the lowest level (T=1).
* swapacceptrate            acceptance rate for swaps between levels of the
                            temperature ladder. When no parallel tempering
                            is used, stores zero.
* autocorrtimes             array[N], per-variable autocorrelation times
*************************************************************************/
class _mcmcreport_owner
{
public:
    _mcmcreport_owner();
    _mcmcreport_owner(alglib_impl::mcmcreport *attach_to);
    _mcmcreport_owner(const _mcmcreport_owner &rhs);
    _mcmcreport_owner& operator=(const _mcmcreport_owner &rhs);
    virtual ~_mcmcreport_owner();
    alglib_impl::mcmcreport* c_ptr();
    const alglib_impl::mcmcreport* c_ptr() const;
protected:
    alglib_impl::mcmcreport *p_struct;
    bool is_attached;
};
class mcmcreport : public _mcmcreport_owner
{
public:
    mcmcreport();
    mcmcreport(alglib_impl::mcmcreport *attach_to);
    mcmcreport(const mcmcreport &rhs);
    mcmcreport& operator=(const mcmcreport &rhs);
    virtual ~mcmcreport();
    ae_int_t &nfev;
    double &acceptrate;
    double &swapacceptrate;
    real_1d_array autocorrtimes;


};
#endif

#if defined(AE_COMPILE_WSR) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_VARIANCETESTS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_BASESTAT) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Calculation of the distribution moments: mean, variance, skewness, kurtosis.

INPUT PARAMETERS:
    X       -   sample
    N       -   N>=0, sample size:
                * if given, only leading N elements of X are processed
                * if not given, automatically determined from size of X

OUTPUT PARAMETERS
    Mean    -   mean.
    Variance-   variance.
    Skewness-   skewness (if variance<>0; zero otherwise).
    Kurtosis-   kurtosis (if variance<>0; zero otherwise).

NOTE: variance is calculated by dividing sum of squares by N-1, not N.

  -- ALGLIB --
     Copyright 06.09.2006 by Bochkanov Sergey
*************************************************************************/
void samplemoments(const real_1d_array &x, const ae_int_t n, double &mean, double &variance, double &skewness, double &kurtosis, const xparams _xparams = alglib::xdefault);
void samplemoments(const real_1d_array &x, double &mean, double &variance, double &skewness, double &kurtosis, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Calculation of the mean.

INPUT PARAMETERS:
    X       -   sample
    N       -   N>=0, sample size:
                * if given, only leading N elements of X are processed
                * if not given, automatically determined from size of X

NOTE:

This function return result  which calculated by 'SampleMoments' function
and stored at 'Mean' variable.


  -- ALGLIB --
     Copyright 06.09.2006 by Bochkanov Sergey
*************************************************************************/
double samplemean(const real_1d_array &x, const ae_int_t n, const xparams _xparams = alglib::xdefault);
double samplemean(const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Calculation of the variance.

INPUT PARAMETERS:
    X       -   sample
    N       -   N>=0, sample size:
                * if given, only leading N elements of X are processed
                * if not given, automatically determined from size of X

NOTE:

This function return result  which calculated by 'SampleMoments' function
and stored at 'Variance' variable.


  -- ALGLIB --
     Copyright 06.09.2006 by Bochkanov Sergey
*************************************************************************/
double samplevariance(const real_1d_array &x, const ae_int_t n, const xparams _xparams = alglib::xdefault);
double samplevariance(const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Calculation of the skewness.

INPUT PARAMETERS:
    X       -   sample
    N       -   N>=0, sample size:
                * if given, only leading N elements of X are processed
                * if not given, automatically determined from size of X

NOTE:

This function return result  which calculated by 'SampleMoments' function
and stored at 'Skewness' variable.


  -- ALGLIB --
     Copyright 06.09.2006 by Bochkanov Sergey
*************************************************************************/
double sampleskewness(const real_1d_array &x, const ae_int_t n, const xparams _xparams = alglib::xdefault);
double sampleskewness(const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Calculation of the kurtosis.

INPUT PARAMETERS:
    X       -   sample
    N       -   N>=0, sample size:
                * if given, only leading N elements of X are processed
                * if not given, automatically determined from size of X

NOTE:

This function return result  which calculated by 'SampleMoments' function
and stored at 'Kurtosis' variable.


  -- ALGLIB --
     Copyright 06.09.2006 by Bochkanov Sergey
*************************************************************************/
double samplekurtosis(const real_1d_array &x, const ae_int_t n, const xparams _xparams = alglib::xdefault);
double samplekurtosis(const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
ADev

Input parameters:
    X   -   sample
    N   -   N>=0, sample size:
            * if given, only leading N elements of X are processed
            * if not given, automatically determined from size of X

Output parameters:
    ADev-   ADev

  -- ALGLIB --
     Copyright 06.09.2006 by Bochkanov Sergey
*************************************************************************/
void sampleadev(const real_1d_array &x, const ae_int_t n, double &adev, const xparams _xparams = alglib::xdefault);
void sampleadev(const real_1d_array &x, double &adev, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Median calculation.

Input parameters:
    X   -   sample (array indexes: [0..N-1])
    N   -   N>=0, sample size:
            * if given, only leading N elements of X are processed
            * if not given, automatically determined from size of X

Output parameters:
    Median

  -- ALGLIB --
     Copyright 06.09.2006 by Bochkanov Sergey
*************************************************************************/
void samplemedian(const real_1d_array &x, const ae_int_t n, double &median, const xparams _xparams = alglib::xdefault);
void samplemedian(const real_1d_array &x, double &median, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Percentile calculation.

Input parameters:
    X   -   sample (array indexes: [0..N-1])
    N   -   N>=0, sample size:
            * if given, only leading N elements of X are processed
            * if not given, automatically determined from size of X
    P   -   percentile (0<=P<=1)

Output parameters:
    V   -   percentile

  -- ALGLIB --
     Copyright 01.03.2008 by Bochkanov Sergey
*************************************************************************/
void samplepercentile(const real_1d_array &x, const ae_int_t n, const double p, double &v, const xparams _xparams = alglib::xdefault);
void samplepercentile(const real_1d_array &x, const double p, double &v, const xparams _xparams = alglib::xdefault);


/*************************************************************************
2-sample covariance

Input parameters:
    X       -   sample 1 (array indexes: [0..N-1])
    Y       -   sample 2 (array indexes: [0..N-1])
    N       -   N>=0, sample size:
                * if given, only N leading elements of X/Y are processed
                * if not given, automatically determined from input sizes

Result:
    covariance (zero for N=0 or N=1)

  -- ALGLIB --
     Copyright 28.10.2010 by Bochkanov Sergey
*************************************************************************/
double cov2(const real_1d_array &x, const real_1d_array &y, const ae_int_t n, const xparams _xparams = alglib::xdefault);
double cov2(const real_1d_array &x, const real_1d_array &y, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Pearson product-moment correlation coefficient

Input parameters:
    X       -   sample 1 (array indexes: [0..N-1])
    Y       -   sample 2 (array indexes: [0..N-1])
    N       -   N>=0, sample size:
                * if given, only N leading elements of X/Y are processed
                * if not given, automatically determined from input sizes

Result:
    Pearson product-moment correlation coefficient
    (zero for N=0 or N=1)

  -- ALGLIB --
     Copyright 28.10.2010 by Bochkanov Sergey
*************************************************************************/
double pearsoncorr2(const real_1d_array &x, const real_1d_array &y, const ae_int_t n, const xparams _xparams = alglib::xdefault);
double pearsoncorr2(const real_1d_array &x, const real_1d_array &y, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Spearman's rank correlation coefficient

Input parameters:
    X       -   sample 1 (array indexes: [0..N-1])
    Y       -   sample 2 (array indexes: [0..N-1])
    N       -   N>=0, sample size:
                * if given, only N leading elements of X/Y are processed
                * if not given, automatically determined from input sizes

Result:
    Spearman's rank correlation coefficient
    (zero for N=0 or N=1)

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
double spearmancorr2(const real_1d_array &x, const real_1d_array &y, const ae_int_t n, const xparams _xparams = alglib::xdefault);
double spearmancorr2(const real_1d_array &x, const real_1d_array &y, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Covariance matrix

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel, ARM) implementations of linear algebra and
  !   other primitives (C++ and C# versions)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    X   -   array[N,M], sample matrix:
            * J-th column corresponds to J-th variable
            * I-th row corresponds to I-th observation
    N   -   N>=0, number of observations:
            * if given, only leading N rows of X are used
            * if not given, automatically determined from input size
    M   -   M>0, number of variables:
            * if given, only leading M columns of X are used
            * if not given, automatically determined from input size

OUTPUT PARAMETERS:
    C   -   array[M,M], covariance matrix (zero if N=0 or N=1)

  -- ALGLIB --
     Copyright 28.10.2010 by Bochkanov Sergey
*************************************************************************/
void covm(const real_2d_array &x, const ae_int_t n, const ae_int_t m, real_2d_array &c, const xparams _xparams = alglib::xdefault);
void covm(const real_2d_array &x, real_2d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Pearson product-moment correlation matrix

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel, ARM) implementations of linear algebra and
  !   other primitives (C++ and C# versions)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    X   -   array[N,M], sample matrix:
            * J-th column corresponds to J-th variable
            * I-th row corresponds to I-th observation
    N   -   N>=0, number of observations:
            * if given, only leading N rows of X are used
            * if not given, automatically determined from input size
    M   -   M>0, number of variables:
            * if given, only leading M columns of X are used
            * if not given, automatically determined from input size

OUTPUT PARAMETERS:
    C   -   array[M,M], correlation matrix (zero if N=0 or N=1)

  -- ALGLIB --
     Copyright 28.10.2010 by Bochkanov Sergey
*************************************************************************/
void pearsoncorrm(const real_2d_array &x, const ae_int_t n, const ae_int_t m, real_2d_array &c, const xparams _xparams = alglib::xdefault);
void pearsoncorrm(const real_2d_array &x, real_2d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Spearman's rank correlation matrix

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel, ARM) implementations of linear algebra and
  !   other primitives (C++ and C# versions)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    X   -   array[N,M], sample matrix:
            * J-th column corresponds to J-th variable
            * I-th row corresponds to I-th observation
    N   -   N>=0, number of observations:
            * if given, only leading N rows of X are used
            * if not given, automatically determined from input size
    M   -   M>0, number of variables:
            * if given, only leading M columns of X are used
            * if not given, automatically determined from input size

OUTPUT PARAMETERS:
    C   -   array[M,M], correlation matrix (zero if N=0 or N=1)

  -- ALGLIB --
     Copyright 28.10.2010 by Bochkanov Sergey
*************************************************************************/
void spearmancorrm(const real_2d_array &x, const ae_int_t n, const ae_int_t m, real_2d_array &c, const xparams _xparams = alglib::xdefault);
void spearmancorrm(const real_2d_array &x, real_2d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Cross-covariance matrix

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel, ARM) implementations of linear algebra and
  !   other primitives (C++ and C# versions)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    X   -   array[N,M1], sample matrix:
            * J-th column corresponds to J-th variable
            * I-th row corresponds to I-th observation
    Y   -   array[N,M2], sample matrix:
            * J-th column corresponds to J-th variable
            * I-th row corresponds to I-th observation
    N   -   N>=0, number of observations:
            * if given, only leading N rows of X/Y are used
            * if not given, automatically determined from input sizes
    M1  -   M1>0, number of variables in X:
            * if given, only leading M1 columns of X are used
            * if not given, automatically determined from input size
    M2  -   M2>0, number of variables in Y:
            * if given, only leading M1 columns of X are used
            * if not given, automatically determined from input size

OUTPUT PARAMETERS:
    C   -   array[M1,M2], cross-covariance matrix (zero if N=0 or N=1)

  -- ALGLIB --
     Copyright 28.10.2010 by Bochkanov Sergey
*************************************************************************/
void covm2(const real_2d_array &x, const real_2d_array &y, const ae_int_t n, const ae_int_t m1, const ae_int_t m2, real_2d_array &c, const xparams _xparams = alglib::xdefault);
void covm2(const real_2d_array &x, const real_2d_array &y, real_2d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Pearson product-moment cross-correlation matrix

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel, ARM) implementations of linear algebra and
  !   other primitives (C++ and C# versions)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    X   -   array[N,M1], sample matrix:
            * J-th column corresponds to J-th variable
            * I-th row corresponds to I-th observation
    Y   -   array[N,M2], sample matrix:
            * J-th column corresponds to J-th variable
            * I-th row corresponds to I-th observation
    N   -   N>=0, number of observations:
            * if given, only leading N rows of X/Y are used
            * if not given, automatically determined from input sizes
    M1  -   M1>0, number of variables in X:
            * if given, only leading M1 columns of X are used
            * if not given, automatically determined from input size
    M2  -   M2>0, number of variables in Y:
            * if given, only leading M1 columns of X are used
            * if not given, automatically determined from input size

OUTPUT PARAMETERS:
    C   -   array[M1,M2], cross-correlation matrix (zero if N=0 or N=1)

  -- ALGLIB --
     Copyright 28.10.2010 by Bochkanov Sergey
*************************************************************************/
void pearsoncorrm2(const real_2d_array &x, const real_2d_array &y, const ae_int_t n, const ae_int_t m1, const ae_int_t m2, real_2d_array &c, const xparams _xparams = alglib::xdefault);
void pearsoncorrm2(const real_2d_array &x, const real_2d_array &y, real_2d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Spearman's rank cross-correlation matrix

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel, ARM) implementations of linear algebra and
  !   other primitives (C++ and C# versions)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    X   -   array[N,M1], sample matrix:
            * J-th column corresponds to J-th variable
            * I-th row corresponds to I-th observation
    Y   -   array[N,M2], sample matrix:
            * J-th column corresponds to J-th variable
            * I-th row corresponds to I-th observation
    N   -   N>=0, number of observations:
            * if given, only leading N rows of X/Y are used
            * if not given, automatically determined from input sizes
    M1  -   M1>0, number of variables in X:
            * if given, only leading M1 columns of X are used
            * if not given, automatically determined from input size
    M2  -   M2>0, number of variables in Y:
            * if given, only leading M1 columns of X are used
            * if not given, automatically determined from input size

OUTPUT PARAMETERS:
    C   -   array[M1,M2], cross-correlation matrix (zero if N=0 or N=1)

  -- ALGLIB --
     Copyright 28.10.2010 by Bochkanov Sergey
*************************************************************************/
void spearmancorrm2(const real_2d_array &x, const real_2d_array &y, const ae_int_t n, const ae_int_t m1, const ae_int_t m2, real_2d_array &c, const xparams _xparams = alglib::xdefault);
void spearmancorrm2(const real_2d_array &x, const real_2d_array &y, real_2d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function replaces data in XY by their ranks:
* XY is processed row-by-row
* rows are processed separately
* tied data are correctly handled (tied ranks are calculated)
* ranking starts from 0, ends at NFeatures-1
* sum of within-row values is equal to (NFeatures-1)*NFeatures/2

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    XY      -   array[NPoints,NFeatures], dataset
    NPoints -   number of points
    NFeatures-  number of features

OUTPUT PARAMETERS:
    XY      -   data are replaced by their within-row ranks;
                ranking starts from 0, ends at NFeatures-1

  -- ALGLIB --
     Copyright 18.04.2013 by Bochkanov Sergey
*************************************************************************/
void rankdata(real_2d_array &xy, const ae_int_t npoints, const ae_int_t nfeatures, const xparams _xparams = alglib::xdefault);
void rankdata(real_2d_array &xy, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function replaces data in XY by their CENTERED ranks:
* XY is processed row-by-row
* rows are processed separately
* tied data are correctly handled (tied ranks are calculated)
* centered ranks are just usual ranks, but centered in such way  that  sum
  of within-row values is equal to 0.0.
* centering is performed by subtracting mean from each row, i.e it changes
  mean value, but does NOT change higher moments

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    XY      -   array[NPoints,NFeatures], dataset
    NPoints -   number of points
    NFeatures-  number of features

OUTPUT PARAMETERS:
    XY      -   data are replaced by their within-row ranks;
                ranking starts from 0, ends at NFeatures-1

  -- ALGLIB --
     Copyright 18.04.2013 by Bochkanov Sergey
*************************************************************************/
void rankdatacentered(real_2d_array &xy, const ae_int_t npoints, const ae_int_t nfeatures, const xparams _xparams = alglib::xdefault);
void rankdatacentered(real_2d_array &xy, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete function, we recommend to use PearsonCorr2().

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
double pearsoncorrelation(const real_1d_array &x, const real_1d_array &y, const ae_int_t n, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete function, we recommend to use SpearmanCorr2().

    -- ALGLIB --
    Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
double spearmanrankcorrelation(const real_1d_array &x, const real_1d_array &y, const ae_int_t n, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_JARQUEBERA) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Jarque-Bera test

This test checks hypotheses about the fact that a  given  sample  X  is  a
sample of normal random variable.

Requirements:
    * the number of elements in the sample is not less than 5.

Input parameters:
    X   -   sample. Array whose index goes from 0 to N-1.
    N   -   size of the sample. N>=5

Output parameters:
    P           -   p-value for the test

Accuracy of the approximation used (5<=N<=1951):

p-value  	    relative error (5<=N<=1951)
[1, 0.1]            < 1%
[0.1, 0.01]         < 2%
[0.01, 0.001]       < 6%
[0.001, 0]          wasn't measured

For N>1951 accuracy wasn't measured but it shouldn't be sharply  different
from table values.

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
void jarqueberatest(const real_1d_array &x, const ae_int_t n, double &p, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MANNWHITNEYU) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Mann-Whitney U-test

This test checks hypotheses about whether X  and  Y  are  samples  of  two
continuous distributions of the same shape  and  same  median  or  whether
their medians are different.

The following tests are performed:
    * two-tailed test (null hypothesis - the medians are equal)
    * left-tailed test (null hypothesis - the median of the  first  sample
      is greater than or equal to the median of the second sample)
    * right-tailed test (null hypothesis - the median of the first  sample
      is less than or equal to the median of the second sample).

Requirements:
    * the samples are independent
    * X and Y are continuous distributions (or discrete distributions well-
      approximating continuous distributions)
    * distributions of X and Y have the  same  shape.  The  only  possible
      difference is their position (i.e. the value of the median)
    * the number of elements in each sample is not less than 5
    * the scale of measurement should be ordinal, interval or ratio  (i.e.
      the test could not be applied to nominal variables).

The test is non-parametric and doesn't require distributions to be normal.

Input parameters:
    X   -   sample 1. Array whose index goes from 0 to N-1.
    N   -   size of the sample. N>=5
    Y   -   sample 2. Array whose index goes from 0 to M-1.
    M   -   size of the sample. M>=5

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

To calculate p-values, special approximation is used. This method lets  us
calculate p-values with satisfactory  accuracy  in  interval  [0.0001, 1].
There is no approximation outside the [0.0001, 1] interval. Therefore,  if
the significance level outlies this interval, the test returns 0.0001.

Relative precision of approximation of p-value:

N          M          Max.err.   Rms.err.
5..10      N..10      1.4e-02    6.0e-04
5..10      N..100     2.2e-02    5.3e-06
10..15     N..15      1.0e-02    3.2e-04
10..15     N..100     1.0e-02    2.2e-05
15..100    N..100     6.1e-03    2.7e-06

For N,M>100 accuracy checks weren't put into  practice,  but  taking  into
account characteristics of asymptotic approximation used, precision should
not be sharply different from the values for interval [5, 100].

NOTE: P-value approximation was  optimized  for  0.0001<=p<=0.2500.  Thus,
      P's outside of this interval are enforced to these bounds. Say,  you
      may quite often get P equal to exactly 0.25 or 0.0001.

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
void mannwhitneyutest(const real_1d_array &x, const ae_int_t n, const real_1d_array &y, const ae_int_t m, double &bothtails, double &lefttail, double &righttail, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_STUDENTTTESTS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
One-sample t-test

This test checks three hypotheses about the mean of the given sample.  The
following tests are performed:
    * two-tailed test (null hypothesis - the mean is equal  to  the  given
      value)
    * left-tailed test (null hypothesis - the  mean  is  greater  than  or
      equal to the given value)
    * right-tailed test (null hypothesis - the mean is less than or  equal
      to the given value).

The test is based on the assumption that  a  given  sample  has  a  normal
distribution and  an  unknown  dispersion.  If  the  distribution  sharply
differs from normal, the test will work incorrectly.

INPUT PARAMETERS:
    X       -   sample. Array whose index goes from 0 to N-1.
    N       -   size of sample, N>=0
    Mean    -   assumed value of the mean.

OUTPUT PARAMETERS:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

NOTE: this function correctly handles degenerate cases:
      * when N=0, all p-values are set to 1.0
      * when variance of X[] is exactly zero, p-values are set
        to 1.0 or 0.0, depending on difference between sample mean and
        value of mean being tested.


  -- ALGLIB --
     Copyright 08.09.2006 by Bochkanov Sergey
*************************************************************************/
void studentttest1(const real_1d_array &x, const ae_int_t n, const double mean, double &bothtails, double &lefttail, double &righttail, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Two-sample pooled test

This test checks three hypotheses about the mean of the given samples. The
following tests are performed:
    * two-tailed test (null hypothesis - the means are equal)
    * left-tailed test (null hypothesis - the mean of the first sample  is
      greater than or equal to the mean of the second sample)
    * right-tailed test (null hypothesis - the mean of the first sample is
      less than or equal to the mean of the second sample).

Test is based on the following assumptions:
    * given samples have normal distributions
    * dispersions are equal
    * samples are independent.

Input parameters:
    X       -   sample 1. Array whose index goes from 0 to N-1.
    N       -   size of sample.
    Y       -   sample 2. Array whose index goes from 0 to M-1.
    M       -   size of sample.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

NOTE: this function correctly handles degenerate cases:
      * when N=0 or M=0, all p-values are set to 1.0
      * when both samples has exactly zero variance, p-values are set
        to 1.0 or 0.0, depending on difference between means.

  -- ALGLIB --
     Copyright 18.09.2006 by Bochkanov Sergey
*************************************************************************/
void studentttest2(const real_1d_array &x, const ae_int_t n, const real_1d_array &y, const ae_int_t m, double &bothtails, double &lefttail, double &righttail, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Two-sample unpooled test

This test checks three hypotheses about the mean of the given samples. The
following tests are performed:
    * two-tailed test (null hypothesis - the means are equal)
    * left-tailed test (null hypothesis - the mean of the first sample  is
      greater than or equal to the mean of the second sample)
    * right-tailed test (null hypothesis - the mean of the first sample is
      less than or equal to the mean of the second sample).

Test is based on the following assumptions:
    * given samples have normal distributions
    * samples are independent.
Equality of variances is NOT required.

Input parameters:
    X - sample 1. Array whose index goes from 0 to N-1.
    N - size of the sample.
    Y - sample 2. Array whose index goes from 0 to M-1.
    M - size of the sample.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

NOTE: this function correctly handles degenerate cases:
      * when N=0 or M=0, all p-values are set to 1.0
      * when both samples has zero variance, p-values are set
        to 1.0 or 0.0, depending on difference between means.
      * when only one sample has zero variance, test reduces to 1-sample
        version.

  -- ALGLIB --
     Copyright 18.09.2006 by Bochkanov Sergey
*************************************************************************/
void unequalvariancettest(const real_1d_array &x, const ae_int_t n, const real_1d_array &y, const ae_int_t m, double &bothtails, double &lefttail, double &righttail, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_CORRELATIONTESTS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Pearson's correlation coefficient significance test

This test checks hypotheses about whether X  and  Y  are  samples  of  two
continuous  distributions  having  zero  correlation  or   whether   their
correlation is non-zero.

The following tests are performed:
    * two-tailed test (null hypothesis - X and Y have zero correlation)
    * left-tailed test (null hypothesis - the correlation  coefficient  is
      greater than or equal to 0)
    * right-tailed test (null hypothesis - the correlation coefficient  is
      less than or equal to 0).

Requirements:
    * the number of elements in each sample is not less than 5
    * normality of distributions of X and Y.

Input parameters:
    R   -   Pearson's correlation coefficient for X and Y
    N   -   number of elements in samples, N>=5.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
void pearsoncorrelationsignificance(const double r, const ae_int_t n, double &bothtails, double &lefttail, double &righttail, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Spearman's rank correlation coefficient significance test

This test checks hypotheses about whether X  and  Y  are  samples  of  two
continuous  distributions  having  zero  correlation  or   whether   their
correlation is non-zero.

The following tests are performed:
    * two-tailed test (null hypothesis - X and Y have zero correlation)
    * left-tailed test (null hypothesis - the correlation  coefficient  is
      greater than or equal to 0)
    * right-tailed test (null hypothesis - the correlation coefficient  is
      less than or equal to 0).

Requirements:
    * the number of elements in each sample is not less than 5.

The test is non-parametric and doesn't require distributions X and Y to be
normal.

Input parameters:
    R   -   Spearman's rank correlation coefficient for X and Y
    N   -   number of elements in samples, N>=5.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
void spearmanrankcorrelationsignificance(const double r, const ae_int_t n, double &bothtails, double &lefttail, double &righttail, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_STEST) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Sign test

This test checks three hypotheses about the median of  the  given  sample.
The following tests are performed:
    * two-tailed test (null hypothesis - the median is equal to the  given
      value)
    * left-tailed test (null hypothesis - the median is  greater  than  or
      equal to the given value)
    * right-tailed test (null hypothesis - the  median  is  less  than  or
      equal to the given value)

Requirements:
    * the scale of measurement should be ordinal, interval or ratio  (i.e.
      the test could not be applied to nominal variables).

The test is non-parametric and doesn't require distribution X to be normal

Input parameters:
    X       -   sample. Array whose index goes from 0 to N-1.
    N       -   size of the sample.
    Median  -   assumed median value.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

While   calculating   p-values   high-precision   binomial    distribution
approximation is used, so significance levels have about 15 exact digits.

  -- ALGLIB --
     Copyright 08.09.2006 by Bochkanov Sergey
*************************************************************************/
void onesamplesigntest(const real_1d_array &x, const ae_int_t n, const double median, double &bothtails, double &lefttail, double &righttail, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MCMC) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This function initializes MCMC sampler using single initial point to  seed
the population.

The population is generated around the initial point with random  Gaussian
noise being added, having per-variable magnitude equal to XStdDev  or  (if
MCMCSetScale() was called) equal to XStdDev*S[I].


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point used to seed a MCMC algo, array[N]:
                * it is better to have X not too far away from the maximum
                  of log-likelihood
                * any point will do, if no maximum location is unknown
    XStdDev -   standard deviation of a population generated around X:
                * strictly greater than zero
                * nearly zero values are likely  to  cause  population  to
                  stagnate, whilst too large values are  likely  to  cause
                  population to spend excessive time converging

OUTPUT PARAMETERS:
    State   -   structure stores MCMC sampler state

  -- ALGLIB --
     Copyright 20.01.2025 by Bochkanov Sergey
*************************************************************************/
void mcmccreate1(const ae_int_t n, const real_1d_array &x, const double xstddev, mcmcstate &state, const xparams _xparams = alglib::xdefault);
void mcmccreate1(const real_1d_array &x, const double xstddev, mcmcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function initializes MCMC sampler using a population of user-specified
points.

A specific sampling algorithm that needs an  initial  population will  use
user-provided points. If an algorithm needs more initial points  than  was
specified, additional points will be randomly generated  using  population
as a distribution reference.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of P are used
                * if not given, automatically determined from size of P
    P       -   initial points, array[PopSize,N]
    PopSize -   population size, PopSize>0:
                * if given, only leading PopSize elements of P are used
                * if not given, automatically determined from size of P

OUTPUT PARAMETERS:
    State   -   structure stores MCMC sampler state

  -- ALGLIB --
     Copyright 20.05.2025 by Bochkanov Sergey
*************************************************************************/
void mcmccreatefrompopulation(const ae_int_t n, const real_2d_array &p, const ae_int_t popsize, mcmcstate &state, const xparams _xparams = alglib::xdefault);
void mcmccreatefrompopulation(const real_2d_array &p, mcmcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets per-variable scaling coefficients for MCMC sampler.

Present version of the MCMC sampler uses per-variable scales during initial
popilation generation: an initial point X0 is perturbed with random noise,
whose per-variable magnitude is XStdDev*S[I].

Future versions of the sampler may use scales for other purposes too,  but
are likely to do so in a backward-compatible manner.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 15.05.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetscale(mcmcstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function controls adaptation rate of the temperature ladder  used  by
adaptive parallel tempering algorithms.

The sampler changes the logarithmic difference between temperatures in the
ladder ln(T[i+1]-T[i]) as a product of different between swap accept rates
A[i]-A[i+1] and current adaptation rate, which is nu0/(1+iteridx/tau).

Here nu0 is an initial adaptation rate that similar to stochastic gradient
descent learning rate. Recommended values 0.01-0.1. And tau is a  learning
rate decay time, depending on the problem it can be 100 or 1000.

The MCMC sampler uses some default values for these parameters,  but  they
can change in future versions without notice.

This function has no effect when adaptive tempering is not active.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    Nu0     -   initial learning rate, >=0.
                Zero value effectively turns off adaptation.
    Tau     -   characteristic decay time, >=0.
                Zero value effectively turns off adaptation.

  -- ALGLIB --
     Copyright 15.05.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetladderadaptationrate(mcmcstate &state, const double nu0, const double tau, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function activates parallel  tempering  with  the  fixed  temperature
ladder.

Parallel tempering is  intended  for sampling of multimodal distributions,
with the T=1 corresponding to sampling of the original distribution  (what
you get as result), and  higher temperatures  corresponding  to   smoothed
versions of the distribution,  helping  the  sampler  to  reach  otherwise
unreachable remote peaks.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    T       -   array[NTemp], T[0]=1, T[I+1]>T[I], sampling  temperatures.
                If the  first  element  of  T  is  different  from  1,  or
                temperatures are not strictly increasing,  an exception is
                raised
    NTemp   -   >=1, temperature ladder height.

When running the algorithm with parallel  tempering  turned  on,  we  have
NTemp ladder levels, each having PopSize walkers. Thus, the  total  number
of walkers in the population is NTemp*PopSize, although only PopSize  ones
corresponding to the coldest chain are returned.

  -- ALGLIB --
     Copyright 15.05.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetfixedtemperatureladder(mcmcstate &state, const real_1d_array &t, const ae_int_t ntemp, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function activates parallel  tempering  with the adaptive temperature
ladder using uniform Swap Acceptance Rate (SAR) proposal.

Parallel tempering is  intended  for sampling of multimodal distributions,
with the T=1 corresponding to sampling of the original distribution  (what
you get as result), and  higher temperatures  corresponding  to   smoothed
versions of the distribution,  helping  the  sampler  to  reach  otherwise
unreachable remote peaks.

The function accepts the hottest temperature in the ladder TMax,  as  well
as ladder height NTemp>=1.

You can control adaptation rate wuth mcmcsetladderadaptationrate() function.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    TMax    -   initial value of the maximum temperature in the ladder,
                TMax>1 (strictly)
    NTemp   -   >=1, temperature ladder height:
                * NTemp=1 means that no temperature ladder is actually used
                * NTemp=2 means that we have a ladder with temperatures
                  [1,TMax] and no adaptation
                * NTemp>2 means that we have  a  ladder  with  T[0]=1  and
                  T[NTemp-1]=TMax, and adaptive temperatures between them.

When running the algorithm with parallel  tempering  turned  on,  we  have
NTemp ladder levels, each having PopSize walkers. Thus, the  total  number
of walkers in the population is NTemp*PopSize, although only PopSize  ones
corresponding to the coldest chain are returned.

  -- ALGLIB --
     Copyright 15.05.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetsartemperatureladder(mcmcstate &state, const double tmax, const ae_int_t ntemp, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Same as mcmcsetalgostretch().
*************************************************************************/
void mcmcsetalgogoodmanweare(mcmcstate &state, const ae_int_t popsize, const ae_int_t epochscnt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets MCMC algorithm to Goodman-Weare ( ensemble  MCMC)  with
the specified ensemble size and number of iterations being reported.

Uses  stretch  move,   as   defined  in  'Ensemble  samplers  with  affine
invariance', Goodman and Weare, 2010.

NOTE: the sampler always reports PopSize*EpochsCnt samples which corresponds
      to EpochsCnt iterations being  reported.

      By  default, it performs exactly the same number of iterations as it
      reports. However, it will perform more iterations than it reports if
      using a  burn-in  phase  (discards  initial  samples  that  are  too
      influenced by the initial state) and by specifying a thinning factor
      greater than 1 (helps to combat autocorrelations).

NOTE: when  using  parallel  tempering,  the  algorithm runs PopSize*NTemp
      walkers, but only lowest PopSize ones corresponding to the  original
      non-smoothed distribution are returned.

INPUT PARAMETERS:
    State   -   structure that stores MCMC sampler state
    PopSize -   ensemble size, PopSize>=N+1, recommended: >=2*N
    EpochsCnt-  iterations count to be reported, >=1

  -- ALGLIB --
     Copyright 20.01.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetalgostretch(mcmcstate &state, const ae_int_t popsize, const ae_int_t epochscnt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets MCMC algorithm to Goodman-Weare ( ensemble  MCMC)  with
the specified ensemble size and number of iterations being reported.

Uses walk move, as defined in 'Ensemble  samplers with affine invariance',
Goodman and Weare, 2010.

NOTE: the sampler always reports PopSize*EpochsCnt samples which corresponds
      to EpochsCnt iterations being  reported.

      By  default, it performs exactly the same number of iterations as it
      reports. However, it will perform more iterations than it reports if
      using a  burn-in  phase  (discards  initial  samples  that  are  too
      influenced by the initial state) and by specifying a thinning factor
      greater than 1 (helps to combat autocorrelations).

NOTE: when  using  parallel  tempering,  the  algorithm runs PopSize*NTemp
      walkers, but only lowest PopSize ones corresponding to the  original
      non-smoothed distribution are returned.

NOTE: for consistency with the rest of the library this move type requires
      PopSize>=N+1. However, it also has a special requirement  PopSize>=4
      that follows from the fact that  each  walker  needs  at  least  two
      helpers, and that we can use parallel moves.

      For N=1 or N=2 it is possible to  specify PopSize=N+1 that  is  less
      than 4. In order to simplify the algorithm, in this case we silently
      override selection with the stretch move.

INPUT PARAMETERS:
    State   -   structure that stores MCMC sampler state

    PopSize -   ensemble size, PopSize>=N+1, recommended: >=2*N

    EpochsCnt-  iterations count to be reported, >=1

    HelpersCnt- helpers count, >=2. Number of  helpers  used  to  generate
                proposal. Recommended values: some small number like  3-5.
                It is possible  to  specify  HelpersCnt=PopSize,  but  for
                large populations it will result  in  proposal  generation
                overhead growing as O(N*PopSize^2).
                Values larger than PopSize will be silently  truncated  to
                PopSize.

  -- ALGLIB --
     Copyright 20.11.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetalgowalk(mcmcstate &state, const ae_int_t popsize, const ae_int_t epochscnt, const ae_int_t helperscnt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets MCMC algorithm to Goodman-Weare ( ensemble  MCMC)  with
the specified ensemble size and number of iterations being reported.

Uses DE move, as defined in 'RUN DMC:  an  efficient,  parallel  code  for
analyzing radial  velocity  observations  using  n-body  integrations  and
differential evolution Markov chain Monte Carlo' by Benjamin Nelson,  Eric
B. Ford, and Matthew J. Payne.

NOTE: the sampler always reports PopSize*EpochsCnt samples which corresponds
      to EpochsCnt iterations being  reported.

      By  default, it performs exactly the same number of iterations as it
      reports. However, it will perform more iterations than it reports if
      using a  burn-in  phase  (discards  initial  samples  that  are  too
      influenced by the initial state) and by specifying a thinning factor
      greater than 1 (helps to combat autocorrelations).

NOTE: when  using  parallel  tempering,  the  algorithm runs PopSize*NTemp
      walkers, but only lowest PopSize ones corresponding to the  original
      non-smoothed distribution are returned.

NOTE: for consistency with the rest of the library this move type requires
      PopSize>=N+1. However, it also has a special requirement  PopSize>=4
      that follows from the fact that  each  walker  needs  at  least  two
      other walkers to produce a DE proposal, and that we can use parallel
      moves (that need larger ensembles).

      For N=1 or N=2 it is possible to  specify PopSize=N+1 that  is  less
      than 4. In order to simplify the algorithm, in this case we silently
      override selection with the stretch move.

NOTE: it is recommended to specify PopSize and EpochsCnt and  leave  other
      parameters to their default values.

INPUT PARAMETERS:
    State   -   structure that stores MCMC sampler state

    PopSize -   ensemble size, PopSize>=N+1, recommended: >=2*N

    EpochsCnt-  iterations count to be reported, >=1

    Sigma   -   non-negative, standard deviation of  a  Gaussian  used  to
                randomly modify the proposal vector.  Recommended  values:
                about 1E-5. Zero value (or omitted) means that  a  default
                one is used.

    Gamma0  -   the mean stretch factor for the proposal vector, >=0. Zero
                value  means  that  a  default  value  is  used  which  is
                2.38/sqrt(2N), as recommended by the original paper.


  -- ALGLIB --
     Copyright 20.11.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetalgode(mcmcstate &state, const ae_int_t popsize, const ae_int_t epochscnt, const double sigma, const double gamma0, const xparams _xparams = alglib::xdefault);
void mcmcsetalgode(mcmcstate &state, const ae_int_t popsize, const ae_int_t epochscnt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets MCMC algorithm to Goodman-Weare  (ensemble  MCMC)  with
the specified ensemble size and number of iterations being reported.

Uses DE move with snooker update, as defined  in  'Differential  Evolution
Markov Chain with snooker updater and fewer chains' by Cajo J.F. ter Braak
and Jasper A. Vrugt.

NOTE: the sampler always reports PopSize*EpochsCnt samples which corresponds
      to EpochsCnt iterations being  reported.

      By  default, it performs exactly the same number of iterations as it
      reports. However, it will perform more iterations than it reports if
      using a  burn-in  phase  (discards  initial  samples  that  are  too
      influenced by the initial state) and by specifying a thinning factor
      greater than 1 (helps to combat autocorrelations).

NOTE: when  using  parallel  tempering,  the  algorithm runs PopSize*NTemp
      walkers, but only lowest PopSize ones corresponding to the  original
      non-smoothed distribution are returned.

NOTE: for consistency with the rest of the library this move type requires
      PopSize>=N+1. However, it also has a special requirement  PopSize>=6
      that follows from the fact that  each update  needs  at  least three
      other walkers to produce a DE proposal, and that we can use parallel
      moves (that need larger ensembles).

      In order to simplify the algorithm, if N+1<=PopSize<6,  we  silently
      override selection with the stretch move.

NOTE: it is recommended to specify PopSize and EpochsCnt and  leave  other
      parameters to their default values.

INPUT PARAMETERS:
    State   -   structure that stores MCMC sampler state

    PopSize -   ensemble size, PopSize>=N+1, recommended: >=2*N

    EpochsCnt-  iterations count to be reported, >=1

    Gamma0  -   the mean stretch factor for the proposal vector, >=0. Zero
                value  means  that  a  default  value  is  used  which  is
                2.38/sqrt(2), as recommended by the original paper.


  -- ALGLIB --
     Copyright 20.11.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetalgodesnooker(mcmcstate &state, const ae_int_t popsize, const ae_int_t epochscnt, const double gamma0, const xparams _xparams = alglib::xdefault);
void mcmcsetalgodesnooker(mcmcstate &state, const ae_int_t popsize, const ae_int_t epochscnt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets MCMC algorithm to Goodman-Weare  (ensemble  MCMC)  with
the specified ensemble size and number of iterations being reported.

Uses Gaussian random walk, an ensemble of PopSize  completely  independent
walkers.

NOTE: the sampler always reports PopSize*EpochsCnt samples which corresponds
      to EpochsCnt iterations being  reported.

      By  default, it performs exactly the same number of iterations as it
      reports. However, it will perform more iterations than it reports if
      using a  burn-in  phase  (discards  initial  samples  that  are  too
      influenced by the initial state) and by specifying a thinning factor
      greater than 1 (helps to combat autocorrelations).

NOTE: when  using  parallel  tempering,  the  algorithm runs PopSize*NTemp
      walkers, but only lowest PopSize ones corresponding to the  original
      non-smoothed distribution are returned.

NOTE: this  move  is  special  because it can work with any ensemble size,
      including PopSize=1 (most other moves  need  at  least  4,  5  or  6
      walkers in the ensemble). Other moves will  throw  an  exception  if
      called with PopSize<N+1.

INPUT PARAMETERS:
    State   -   structure that stores MCMC sampler state

    PopSize -   ensemble size, PopSize>=1.

    EpochsCnt-  iterations count to be reported, >=1

    C       -   array[N,N], a positive definite covariance matrix.  Walker
                position  is  perturbed  with  Gaussian  perturbation with
                covariance C.

    IsUpper -   if IsUpper=True, only upper triangle of  C  is  used  (the
                lower one is ignored). Otherwise, only lower  triangle  is
                used.


  -- ALGLIB --
     Copyright 20.11.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetalgogaussian(mcmcstate &state, const ae_int_t popsize, const ae_int_t epochscnt, const real_2d_array &c, const bool isupper, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets number of additional initial iterations (in addition to
EpochsCnt) that will be performed  and  discarded  (not  stored  into  the
report sample), so called 'burn-in length'.

In total, BurnInLen+EpochsCnt iterations will be performed,  with  initial
BurnInLen ones being used solely to help MCMC spread walkers according  to
the density of the function being sampled.

INPUT PARAMETERS:
    State       -   structure that stores MCMC sampler state
    BurnInLen   -   burn-in length, >=0

  -- ALGLIB --
     Copyright 20.01.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetburninlength(mcmcstate &state, const ae_int_t burninlen, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets thinning factor: ThinBy*EpochsCnt  iterations  will  be
performed  (after  the  optional  burn-in  phase),  with  every  ThinBy-th
iteration being saved and the rest being discarded.

This option helps to avoid storing highly correlated samples.

INPUT PARAMETERS:
    State       -   structure that stores MCMC sampler state
    ThinBy      -   thinning factor, >=1

  -- ALGLIB --
     Copyright 20.01.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetthinningfactor(mcmcstate &state, const ae_int_t thinby, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets the seed  which  is used to initialize internal RNG. By
default, a deterministic seed is used - same for each run of the  sampler.
It means that the same sampling decisions are taken every time.

If you specify a non-deterministic seed value, then the sampler may return
slightly different results after each run.

INPUT PARAMETERS:
    S       -   sampler state
    Seed    -   seed:
                * positive values = use deterministic seed for each run of
                  algorithms which depend on random initialization
                * zero or negative values = use non-deterministic seed

  -- ALGLIB --
     Copyright 08.06.2017 by Bochkanov Sergey
*************************************************************************/
void mcmcsetseed(mcmcstate &s, const ae_int_t seed, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool mcmciteration(mcmcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
These functions accept following parameters:
    state   -   algorithm state
    func    -   callback which calculates function (or merit function)
                value func at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


  -- ALGLIB --
     Copyright 20.01.2025 by Bochkanov Sergey


*************************************************************************/
void mcmcrun(mcmcstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
Extract MCMC sampler results from the sampler. This function has significant
overhead coming from two sources:
* overhead of copying PopSize*EpochsCnt*N-sized array from internal memory
* overhead of computing per-variable integrated autocorrelating time

INPUT PARAMETERS:
    State           -   MCMC sampler, either after return  from  mcmcrun()
                        or  still  running (in  the  latter   case,   this
                        function can be safely called only from the  rep()
                        callback).

OUTPUT PARAMETERS:
    Sample          -   array[SampleSize,N+1], current sample:
                        * first N columns store variable values, the  last
                          one stores log-likelihood value as  computed  by
                          the callback
                        * first  PopSize  rows  store  population snapshot
                          after the iteration #0, subsequent PopSize  rows
                          correspond to iteration #1 and so on.
                        * each snapshot (a set  of  PopSize  rows)  stores
                          positions of PopSize walkers, each walker having
                          the same position in each of SampleSize snapshots.
                          Thus, walker #I at the iteration  #J  is  stored
                          at the row PopSize*J+I.

    SampleSize      -   current sample size:
                        * for a sampler that stopped it is equal to PopSize*EpochsCnt
                        * for a sampler that is  still  running,  we  have
                          0<=SampleSize<PopSize*EpochsCnt. Zero sample  is
                          reported upon the first call to rep().

    Rep             -   other information being reported, including:
                        * acceptance rate
                        * per-variable integrated autocorrelation time

IMPORTANT: unlike other [something]results() functions from ALGLIB library
           this function can be called on a sampler that is still running.

           Thus,  it  can  be  used  to peek into a sampler from the rep()
           callback, e.g. to check convergence. When called prior to  MCMC
           completion, it will return in  Sample/SampleSize/Rep  the  most
           recent snapshot of MCMC sampling.

           Note that due to significant overhead (iteration with number #K
           involves copying O(K) data and doing between O(K) and O(K*logK)
           job) calling  this function after each iteration will lead to a
           catastrophic slowdown of the sampler:  its  running  time  will
           become quadratic with respect  to  iterations  count!

           Consider doing it after  each  100-th  iteration  or  something
           like that.

IMPORTANT: THIS FUNCTION IS NOT THREAD-SAFE! Thus, the  only  place  where
           it can be called  is  rep()  callback,  and  it  must  complete
           prior to returning from the callback into the sampler.

NOTE: burn-in iterations are not reported. Similarly, for a primary  phase
      only each ThinFactor-th iteration is reported.

NOTE: when  using  parallel  tempering,  the  algorithm runs PopSize*NTemp
      walkers, but only lowest PopSize ones corresponding to the  original
      non-smoothed distribution are reported.

  -- ALGLIB --
     Copyright 18.01.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcresults(const mcmcstate &state, real_2d_array &sample, ae_int_t &samplesize, mcmcreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Buffered implementation of MCMCResults() which uses  pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 18.01.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcresultsbuf(const mcmcstate &state, real_2d_array &sample, ae_int_t &samplesize, mcmcreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  subroutine  submits  request  for  termination  of  a  running  MCMC
sampler. It should be called from user-supplied callback when user decides
that it is time to "smoothly" terminate optimization process. As a result,
sampler stops at the point which was "current accepted"  when  termination
request was submitted.

Alternatively,  this  function  can  be  called  from  some  other  thread
(different from one where the sampler is running).

INPUT PARAMETERS:
    State   -   sampler structure

NOTE: after  request  for  termination  sampler   may    perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on sampler  which is NOT running will have  no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 25.02.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcrequesttermination(mcmcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores sampler state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, the algorithm will call rep() callback function if it
was provided to mcmcrun().

NOTE: due to ALGLIB conventions regarding report  callbacks,  the  sampler
      passes two parameters to the rep() callback - an 1D  floating  point
      array, and a scalar floating-point value.

      In nonlinear optimizers these parameters are used  to report current
      point/objective, but it makes little sense in the context  of  MCMC.
      Because  of  that,  MCMC  sampler  sets  the first parameter to be a
      zero-initialized  single-element  array,  and  the  second  (scalar)
      parameter is set to zero.

  -- ALGLIB --
     Copyright 25.02.2025 by Bochkanov Sergey
*************************************************************************/
void mcmcsetxrep(mcmcstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells the sampler to restart the next sampling session using
the most recent population as an initial state.

The new sampling session is still considered an independent one.  It  uses
sampling distribution of the previous session to provide a smooth restart,
but its results do not  include  samples  collected  during  the  previous
session.

It  is  possible  to  specify  different  sampling  algorithm  or   change
parameters of the algorithm. If population size has changed, then we  have
two options:
* the new population size is less than the previous one; the population is
  truncated (only leading NewPopSize elements are retained)
* the  new population size is greater than the previous one; in this case,
  leading OldPopSize elements are retained from the  previous  population,
  and the rest (NewPopSize-OldPopSize) is randomly initialized  using  the
  previous population as a reference distribution.

INPUT PARAMETERS:
    State   -   structure which stores sampler state
    EpochsCnt-  >=0, whether to use original epochs count or set new:
                * >0 means that restarted sampling will continue for
                     EpochsCnt iterations (or EpochsCnt*ThinBy, of thinning
                     was configured)
                * =0 means that the previous settings will be used

RESULT:
    * False, if no  previous  population  was  stored  in the sampler (the
      restart was requested prior to running anything, so we have  nothing
      to restart from). In this case,  the  sampler  will  silently  reuse
      previous initial population generation strategy.
    * True, if a request was successfully accepted.

NOTE: this function also tells the sampler not to  re-seed  internal  RNG,
      so the new session  will  produce  the  same  sequence  of  sampling
      decisions.

      It also tells the sampler  to  ignore  burn-in  phase  (if  any  was
      configured), because the population is  assumed  to  be  already  in
      equilibrium.

  -- ALGLIB --
     Copyright 15.05.2025 by Bochkanov Sergey
*************************************************************************/
bool mcmcrestart(mcmcstate &state, const ae_int_t epochscnt, const xparams _xparams = alglib::xdefault);
bool mcmcrestart(mcmcstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_WSR) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Wilcoxon signed-rank test

This test checks three hypotheses about the median  of  the  given sample.
The following tests are performed:
    * two-tailed test (null hypothesis - the median is equal to the  given
      value)
    * left-tailed test (null hypothesis - the median is  greater  than  or
      equal to the given value)
    * right-tailed test (null hypothesis  -  the  median  is  less than or
      equal to the given value)

Requirements:
    * the scale of measurement should be ordinal, interval or  ratio (i.e.
      the test could not be applied to nominal variables).
    * the distribution should be continuous and symmetric relative to  its
      median.
    * number of distinct values in the X array should be greater than 4

The test is non-parametric and doesn't require distribution X to be normal

Input parameters:
    X       -   sample. Array whose index goes from 0 to N-1.
    N       -   size of the sample.
    Median  -   assumed median value.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

To calculate p-values, special approximation is used. This method lets  us
calculate p-values with two decimal places in interval [0.0001, 1].

"Two decimal places" does not sound very impressive, but in  practice  the
relative error of less than 1% is enough to make a decision.

There is no approximation outside the [0.0001, 1] interval. Therefore,  if
the significance level outlies this interval, the test returns 0.0001.

  -- ALGLIB --
     Copyright 08.09.2006 by Bochkanov Sergey
*************************************************************************/
void wilcoxonsignedranktest(const real_1d_array &x, const ae_int_t n, const double e, double &bothtails, double &lefttail, double &righttail, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_VARIANCETESTS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Two-sample F-test

This test checks three hypotheses about dispersions of the given  samples.
The following tests are performed:
    * two-tailed test (null hypothesis - the dispersions are equal)
    * left-tailed test (null hypothesis  -  the  dispersion  of  the first
      sample is greater than or equal to  the  dispersion  of  the  second
      sample).
    * right-tailed test (null hypothesis - the  dispersion  of  the  first
      sample is less than or equal to the dispersion of the second sample)

The test is based on the following assumptions:
    * the given samples have normal distributions
    * the samples are independent.

Input parameters:
    X   -   sample 1. Array whose index goes from 0 to N-1.
    N   -   sample size.
    Y   -   sample 2. Array whose index goes from 0 to M-1.
    M   -   sample size.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

  -- ALGLIB --
     Copyright 19.09.2006 by Bochkanov Sergey
*************************************************************************/
void ftest(const real_1d_array &x, const ae_int_t n, const real_1d_array &y, const ae_int_t m, double &bothtails, double &lefttail, double &righttail, const xparams _xparams = alglib::xdefault);


/*************************************************************************
One-sample chi-square test

This test checks three hypotheses about the dispersion of the given sample
The following tests are performed:
    * two-tailed test (null hypothesis - the dispersion equals  the  given
      number)
    * left-tailed test (null hypothesis - the dispersion is  greater  than
      or equal to the given number)
    * right-tailed test (null hypothesis  -  dispersion is  less  than  or
      equal to the given number).

Test is based on the following assumptions:
    * the given sample has a normal distribution.

Input parameters:
    X           -   sample 1. Array whose index goes from 0 to N-1.
    N           -   size of the sample.
    Variance    -   dispersion value to compare with.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

  -- ALGLIB --
     Copyright 19.09.2006 by Bochkanov Sergey
*************************************************************************/
void onesamplevariancetest(const real_1d_array &x, const ae_int_t n, const double variance, double &bothtails, double &lefttail, double &righttail, const xparams _xparams = alglib::xdefault);
#endif
}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (FUNCTIONS)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_BASESTAT) || !defined(AE_PARTIAL_BUILD)
void samplemoments(/* Real    */ const ae_vector* x,
     ae_int_t n,
     double* mean,
     double* variance,
     double* skewness,
     double* kurtosis,
     ae_state *_state);
double samplemean(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);
double samplevariance(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);
double sampleskewness(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);
double samplekurtosis(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void sampleadev(/* Real    */ const ae_vector* x,
     ae_int_t n,
     double* adev,
     ae_state *_state);
void samplemedian(/* Real    */ const ae_vector* _x,
     ae_int_t n,
     double* median,
     ae_state *_state);
void samplepercentile(/* Real    */ const ae_vector* _x,
     ae_int_t n,
     double p,
     double* v,
     ae_state *_state);
double cov2(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_state *_state);
double pearsoncorr2(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_state *_state);
double spearmancorr2(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_state *_state);
void covm(/* Real    */ const ae_matrix* _x,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_matrix* c,
     ae_state *_state);
void pearsoncorrm(/* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_matrix* c,
     ae_state *_state);
void spearmancorrm(/* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_matrix* c,
     ae_state *_state);
void covm2(/* Real    */ const ae_matrix* _x,
     /* Real    */ const ae_matrix* _y,
     ae_int_t n,
     ae_int_t m1,
     ae_int_t m2,
     /* Real    */ ae_matrix* c,
     ae_state *_state);
void pearsoncorrm2(/* Real    */ const ae_matrix* _x,
     /* Real    */ const ae_matrix* _y,
     ae_int_t n,
     ae_int_t m1,
     ae_int_t m2,
     /* Real    */ ae_matrix* c,
     ae_state *_state);
void spearmancorrm2(/* Real    */ const ae_matrix* x,
     /* Real    */ const ae_matrix* y,
     ae_int_t n,
     ae_int_t m1,
     ae_int_t m2,
     /* Real    */ ae_matrix* c,
     ae_state *_state);
void rankdata(/* Real    */ ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nfeatures,
     ae_state *_state);
ae_bool _trypexec_rankdata(/* Real    */ ae_matrix* xy,
    ae_int_t npoints,
    ae_int_t nfeatures, ae_state *_state);
void rankdatacentered(/* Real    */ ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nfeatures,
     ae_state *_state);
ae_bool _trypexec_rankdatacentered(/* Real    */ ae_matrix* xy,
    ae_int_t npoints,
    ae_int_t nfeatures, ae_state *_state);
double pearsoncorrelation(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_state *_state);
double spearmanrankcorrelation(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_JARQUEBERA) || !defined(AE_PARTIAL_BUILD)
void jarqueberatest(/* Real    */ const ae_vector* x,
     ae_int_t n,
     double* p,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_MANNWHITNEYU) || !defined(AE_PARTIAL_BUILD)
void mannwhitneyutest(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_STUDENTTTESTS) || !defined(AE_PARTIAL_BUILD)
void studentttest1(/* Real    */ const ae_vector* x,
     ae_int_t n,
     double mean,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);
void studentttest2(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);
void unequalvariancettest(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_CORRELATIONTESTS) || !defined(AE_PARTIAL_BUILD)
void pearsoncorrelationsignificance(double r,
     ae_int_t n,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);
void spearmanrankcorrelationsignificance(double r,
     ae_int_t n,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_STEST) || !defined(AE_PARTIAL_BUILD)
void onesamplesigntest(/* Real    */ const ae_vector* x,
     ae_int_t n,
     double median,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_MCMC) || !defined(AE_PARTIAL_BUILD)
void mcmccreate1(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double xstddev,
     mcmcstate* state,
     ae_state *_state);
void mcmccreatefrompopulation(ae_int_t n,
     /* Real    */ const ae_matrix* p,
     ae_int_t popsize,
     mcmcstate* state,
     ae_state *_state);
void mcmcsetscale(mcmcstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void mcmcsetladderadaptationrate(mcmcstate* state,
     double nu0,
     double tau,
     ae_state *_state);
void mcmcsetfixedtemperatureladder(mcmcstate* state,
     /* Real    */ const ae_vector* t,
     ae_int_t ntemp,
     ae_state *_state);
void mcmcsetsartemperatureladder(mcmcstate* state,
     double tmax,
     ae_int_t ntemp,
     ae_state *_state);
void mcmcsetalgogoodmanweare(mcmcstate* state,
     ae_int_t popsize,
     ae_int_t epochscnt,
     ae_state *_state);
void mcmcsetalgostretch(mcmcstate* state,
     ae_int_t popsize,
     ae_int_t epochscnt,
     ae_state *_state);
void mcmcsetalgowalk(mcmcstate* state,
     ae_int_t popsize,
     ae_int_t epochscnt,
     ae_int_t helperscnt,
     ae_state *_state);
void mcmcsetalgode(mcmcstate* state,
     ae_int_t popsize,
     ae_int_t epochscnt,
     double sigma,
     double gamma0,
     ae_state *_state);
void mcmcsetalgodesnooker(mcmcstate* state,
     ae_int_t popsize,
     ae_int_t epochscnt,
     double gamma0,
     ae_state *_state);
void mcmcsetalgogaussian(mcmcstate* state,
     ae_int_t popsize,
     ae_int_t epochscnt,
     /* Real    */ const ae_matrix* c,
     ae_bool isupper,
     ae_state *_state);
void mcmcsetburninlength(mcmcstate* state,
     ae_int_t burninlen,
     ae_state *_state);
void mcmcsetthinningfactor(mcmcstate* state,
     ae_int_t thinby,
     ae_state *_state);
void mcmcsetseed(mcmcstate* s, ae_int_t seed, ae_state *_state);
ae_bool mcmciteration(mcmcstate* state, ae_state *_state);
void mcmcresults(const mcmcstate* state,
     /* Real    */ ae_matrix* sample,
     ae_int_t* samplesize,
     mcmcreport* rep,
     ae_state *_state);
void mcmcresultsbuf(const mcmcstate* state,
     /* Real    */ ae_matrix* sample,
     ae_int_t* samplesize,
     mcmcreport* rep,
     ae_state *_state);
void mcmcrequesttermination(mcmcstate* state, ae_state *_state);
void mcmcsetxrep(mcmcstate* state, ae_bool needxrep, ae_state *_state);
ae_bool mcmcrestart(mcmcstate* state,
     ae_int_t epochscnt,
     ae_state *_state);
void mcmcsetprotocolv2(mcmcstate* state, ae_state *_state);
void mcmcsetprotocolv2s(mcmcstate* state, ae_state *_state);
void _mcmcstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mcmcstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mcmcstate_clear(void* _p);
void _mcmcstate_destroy(void* _p);
void _mcmcreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mcmcreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mcmcreport_clear(void* _p);
void _mcmcreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_WSR) || !defined(AE_PARTIAL_BUILD)
void wilcoxonsignedranktest(/* Real    */ const ae_vector* _x,
     ae_int_t n,
     double e,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_VARIANCETESTS) || !defined(AE_PARTIAL_BUILD)
void ftest(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);
void onesamplevariancetest(/* Real    */ const ae_vector* x,
     ae_int_t n,
     double variance,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);
#endif

}
#endif

