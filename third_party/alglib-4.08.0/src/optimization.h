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
#ifndef _optimization_pkg_h
#define _optimization_pkg_h
#include "ap.h"
#include "alglibinternal.h"
#include "alglibmisc.h"
#include "linalg.h"
#include "solvers.h"

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (DATATYPES)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_OPTGUARDAPI) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_bool nonc0suspected;
    ae_bool nonc0test0positive;
    ae_int_t nonc0fidx;
    double nonc0lipschitzc;
    ae_bool nonc1suspected;
    ae_bool nonc1test0positive;
    ae_bool nonc1test1positive;
    ae_int_t nonc1fidx;
    double nonc1lipschitzc;
    ae_bool badgradsuspected;
    ae_int_t badgradfidx;
    ae_int_t badgradvidx;
    ae_vector badgradxbase;
    ae_matrix badgraduser;
    ae_matrix badgradnum;
} optguardreport;
typedef struct
{
    ae_bool positive;
    ae_int_t fidx;
    ae_vector x0;
    ae_vector d;
    ae_int_t n;
    ae_vector stp;
    ae_vector f;
    ae_int_t cnt;
    ae_int_t stpidxa;
    ae_int_t stpidxb;
    ae_int_t inneriter;
    ae_int_t outeriter;
} optguardnonc0report;
typedef struct
{
    ae_bool positive;
    ae_int_t fidx;
    ae_vector x0;
    ae_vector d;
    ae_int_t n;
    ae_vector stp;
    ae_vector f;
    ae_int_t cnt;
    ae_int_t stpidxa;
    ae_int_t stpidxb;
    ae_int_t inneriter;
    ae_int_t outeriter;
} optguardnonc1test0report;
typedef struct
{
    ae_bool positive;
    ae_int_t fidx;
    ae_int_t vidx;
    ae_vector x0;
    ae_vector d;
    ae_int_t n;
    ae_vector stp;
    ae_vector g;
    ae_int_t cnt;
    ae_int_t stpidxa;
    ae_int_t stpidxb;
    ae_int_t inneriter;
    ae_int_t outeriter;
} optguardnonc1test1report;
#endif
#if defined(AE_COMPILE_OPTS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t ndense;
    ae_int_t nsparse;
    ae_matrix densec;
    sparsematrix sparsec;
    ae_vector dcl;
    ae_vector dcu;
    ae_vector scl;
    ae_vector scu;
    ae_int_t nec;
    ae_int_t nic;
    ae_matrix cleic;
    ae_vector lcsrcidx;
    ae_vector lcsrcmult;
    sparsematrix effsparsea;
    ae_vector effal;
    ae_vector effau;
    sparsematrix tmps;
} xlinearconstraints;
typedef struct
{
    ae_int_t n;
    ae_bool hasknowntarget;
    double targetf;
    ae_vector s;
    ae_vector c;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t m;
    sparsematrix a;
    ae_vector al;
    ae_vector au;
} lptestproblem;
#endif
#if defined(AE_COMPILE_OPTSERV) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_vector norms;
    ae_vector alpha;
    ae_vector rho;
    ae_matrix yk;
    ae_vector idx;
    ae_vector bufa;
    ae_vector bufb;
} precbuflbfgs;
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_vector d;
    ae_matrix v;
    ae_vector bufc;
    ae_matrix bufz;
    ae_matrix bufw;
    ae_vector tmp;
} precbuflowrank;
typedef struct
{
    ae_int_t htype;
    ae_vector varscale;
    ae_vector invscale;
    ae_int_t sr1ver;
    ae_int_t n;
    ae_int_t resetfreq;
    double stpshort;
    double gammasml;
    double reg;
    double smallreg;
    double microreg;
    double wolfeeps;
    double maxhess;
    ae_int_t m;
    double mincostheta;
    double mincrv;
    ae_matrix hcurrent;
    ae_int_t hage;
    double sumy2;
    double sums2;
    double sumsy;
    ae_int_t memlen;
    double sigma;
    ae_matrix s;
    ae_matrix y;
    ae_matrix lowranksst;
    ae_matrix lowranksyt;
    ae_bool lowrankmodelvalid;
    ae_int_t lowrankk;
    ae_matrix lowrankcp;
    ae_matrix lowrankcm;
    ae_bool lowrankeffdvalid;
    ae_vector lowrankeffd;
    ae_bool sr1modelvalid;
    ae_int_t sr1k;
    ae_matrix sr1c;
    ae_vector sr1d;
    ae_vector sr1z;
    ae_matrix sr1stabc;
    ae_vector sr1stabz;
    ae_vector sr1stabd;
    ae_bool sr1effdvalid;
    ae_vector sr1effd;
    double sr1nrm2;
    double sr1nrm2stab;
    ae_int_t updatestatus;
    ae_matrix hincoming;
    ae_vector sk;
    ae_vector yk;
    ae_vector rk;
    ae_vector hsk;
    ae_vector buf;
    ae_vector buf2;
    ae_matrix corr2;
    ae_matrix tmps;
    ae_matrix tmpy;
    ae_matrix jk;
    ae_matrix blk;
    ae_matrix blk2;
    ae_matrix blk3;
    ae_matrix invsqrtdlk;
    ae_vector bufvmv;
    ae_vector bufupdhx;
    ae_matrix tmpunstablec;
    ae_vector tmpunstables;
    ae_matrix tmpu;
    ae_matrix tmpq;
    ae_matrix tmpw;
    ae_matrix tmpsl;
    ae_matrix tmpl;
    ae_vector tmpe;
    ae_vector tmptau;
} xbfgshessian;
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_bool isdense;
    ae_vector x;
    ae_vector fi;
    ae_matrix jac;
    sparsematrix sj;
} varsfuncjac;
typedef struct
{
    double epsf;
    double eps;
    ae_int_t maxits;
} nlpstoppingcriteria;
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_bool checksmoothness;
    ae_vector s;
    ae_vector dcur;
    ae_int_t enqueuedcnt;
    ae_vector enqueuedstp;
    ae_vector enqueuedx;
    ae_vector enqueuedfunc;
    ae_matrix enqueuedjac;
    ae_vector sortedstp;
    ae_vector sortedidx;
    ae_int_t sortedcnt;
    ae_int_t lagprobinneriter;
    ae_int_t lagprobouteriter;
    double lagprobstepmax;
    ae_int_t lagprobnstepsstored;
    ae_vector lagprobxs;
    ae_vector lagprobd;
    double lagprobstp;
    ae_vector lagprobx;
    ae_vector lagprobfi;
    double lagprobrawlag;
    double lagprobcvpred;
    double lagprobcvactual;
    ae_matrix lagprobj;
    ae_matrix lagprobvalues;
    ae_matrix lagprobjacobians;
    ae_vector lagprobsteps;
    ae_vector lagproblagrangians;
    ae_vector lagprobpredictedcv;
    ae_vector lagprobactualcv;
    rcommstate lagrangianprobingrcomm;
    ae_bool linesearchspoiled;
    ae_bool linesearchstarted;
    ae_int_t linesearchinneridx;
    ae_int_t linesearchouteridx;
    double nonc0currentrating;
    double nonc1currentrating;
    ae_bool badgradhasxj;
    optguardreport rep;
    double nonc0strrating;
    double nonc0lngrating;
    optguardnonc0report nonc0strrep;
    optguardnonc0report nonc0lngrep;
    double nonc1test0strrating;
    double nonc1test0lngrating;
    optguardnonc1test0report nonc1test0strrep;
    optguardnonc1test0report nonc1test0lngrep;
    double nonc1test1strrating;
    double nonc1test1lngrating;
    optguardnonc1test1report nonc1test1strrep;
    optguardnonc1test1report nonc1test1lngrep;
    ae_bool needfij;
    ae_vector x;
    ae_vector fi;
    ae_matrix j;
    rcommstate rstateg0;
    ae_vector xbase;
    ae_vector fbase;
    ae_vector fm;
    ae_vector fc;
    ae_vector fp;
    ae_vector jm;
    ae_vector jc;
    ae_vector jp;
    ae_matrix jbaseusr;
    ae_matrix jbasenum;
    ae_vector stp;
    ae_vector bufr;
    ae_vector f;
    ae_vector g;
    ae_vector deltax;
    ae_vector tmpidx;
    ae_vector bufi;
    ae_vector xu;
    ae_vector du;
    ae_vector f0;
    ae_matrix j0;
} smoothnessmonitor;
typedef struct
{
    ae_int_t problemtype;
    ae_int_t problemsubtype;
    ae_int_t n;
    ae_int_t m;
    ae_matrix x0;
    ae_int_t k0;
    ae_matrix xsol;
    ae_matrix fsol;
    ae_int_t ksol;
    ae_vector lagmultbc;
    ae_vector lagmultlc;
    ae_vector lagmultnlc;
    ae_bool isrotated;
    ae_matrix rotq;
    ae_vector tgtc;
    ae_matrix tgtb;
    ae_matrix tgta;
    ae_matrix tgtd;
    ae_vector bndl;
    ae_vector bndu;
    ae_matrix densea;
    ae_vector al;
    ae_vector au;
    ae_int_t nlinear;
    ae_vector nlc;
    ae_matrix nlb;
    ae_matrix nla;
    ae_matrix nld;
    ae_vector hl;
    ae_vector hu;
    ae_int_t nnonlinear;
} multiobjectivetestfunction;
typedef struct
{
    double f0;
    double g0;
    double alpha1;
    double alphamax;
    double c1;
    double c2;
    ae_bool strongwolfecond;
    ae_int_t maxits;
    ae_bool dotrace;
    ae_int_t tracelevel;
    double bestalphasofar;
    double bestfsofar;
    double alphaprev;
    double fprev;
    double gprev;
    double alphacur;
    double fcur;
    double gcur;
    double alphalo;
    double alphahi;
    double flo;
    double fhi;
    double glo;
    double ghi;
    ae_int_t nfev;
    double alphasol;
    ae_int_t terminationtype;
    double alphatrial;
    double ftrial;
    double gtrial;
    rcommstate rstate;
} linesearchstate;
typedef struct
{
    double maxh;
    ae_int_t filtersize;
    ae_int_t maxdominating;
    double gammaf;
    double gammah;
    ae_bool violationistoohigh;
    ae_vector filterf;
    ae_vector filterh;
} nlpfilter;
#endif
#if defined(AE_COMPILE_CQMODELS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    double alpha;
    double tau;
    double theta;
    ae_matrix a;
    ae_matrix q;
    ae_vector b;
    ae_vector r;
    ae_vector xc;
    ae_vector d;
    ae_vector activeset;
    ae_matrix tq2dense;
    ae_matrix tk2;
    ae_vector tq2diag;
    ae_vector tq1;
    ae_vector tk1;
    double tq0;
    double tk0;
    ae_vector txc;
    ae_vector tb;
    ae_int_t nfree;
    ae_int_t ecakind;
    ae_matrix ecadense;
    ae_matrix eq;
    ae_matrix eccm;
    ae_vector ecadiag;
    ae_vector eb;
    double ec;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmpg;
    ae_matrix tmp2;
    ae_bool ismaintermchanged;
    ae_bool issecondarytermchanged;
    ae_bool islineartermchanged;
    ae_bool isactivesetchanged;
} convexquadraticmodel;
#endif
#if defined(AE_COMPILE_SNNLS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t ns;
    ae_int_t nd;
    ae_int_t nr;
    ae_matrix densea;
    ae_vector b;
    ae_vector nnc;
    double debugflops;
    ae_int_t debugmaxinnerits;
    ae_vector xn;
    ae_vector xp;
    ae_matrix tmpca;
    ae_matrix tmplq;
    ae_matrix trda;
    ae_vector trdd;
    ae_vector crb;
    ae_vector g;
    ae_vector d;
    ae_vector dx;
    ae_vector diagaa;
    ae_vector cb;
    ae_vector cx;
    ae_vector cborg;
    ae_vector tmpcholesky;
    ae_vector r;
    ae_vector regdiag;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector rdtmprowmap;
} snnlssolver;
#endif
#if defined(AE_COMPILE_SACTIVESETS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t algostate;
    ae_vector xc;
    ae_bool hasxc;
    ae_vector s;
    ae_vector h;
    ae_vector cstatus;
    ae_bool basisisready;
    ae_matrix sdensebatch;
    ae_matrix pdensebatch;
    ae_matrix idensebatch;
    ae_int_t densebatchsize;
    ae_vector sparsebatch;
    ae_int_t sparsebatchsize;
    ae_int_t basisage;
    ae_bool feasinitpt;
    ae_bool constraintschanged;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector bndl;
    ae_vector bndu;
    ae_matrix cleic;
    ae_int_t nec;
    ae_int_t nic;
    ae_vector mtnew;
    ae_vector mtx;
    ae_vector mtas;
    ae_vector cdtmp;
    ae_vector corrtmp;
    ae_vector unitdiagonal;
    snnlssolver solver;
    ae_vector scntmp;
    ae_vector tmp0;
    ae_vector tmpfeas;
    ae_matrix tmpm0;
    ae_vector rctmps;
    ae_vector rctmpg;
    ae_vector rctmprightpart;
    ae_matrix rctmpdense0;
    ae_matrix rctmpdense1;
    ae_vector rctmpisequality;
    ae_vector rctmpconstraintidx;
    ae_vector rctmplambdas;
    ae_matrix tmpbasis;
    ae_vector tmpnormestimates;
    ae_vector tmpreciph;
    ae_vector tmpprodp;
    ae_vector tmpprods;
    ae_vector tmpcp;
    ae_vector tmpcs;
    ae_vector tmpci;
} sactiveset;
#endif
#if defined(AE_COMPILE_MINBLEIC) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t nmain;
    ae_int_t nslack;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    ae_bool drep;
    double stpmax;
    double diffstep;
    sactiveset sas;
    ae_vector s;
    ae_int_t prectype;
    ae_vector diagh;
    ae_int_t protocolversion;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool xupdated;
    ae_bool lsstart;
    ae_bool steepestdescentstep;
    ae_bool boundedstep;
    ae_bool userterminationneeded;
    rcommstate rstate;
    ae_vector ugc;
    ae_vector cgc;
    ae_vector xn;
    ae_vector ugn;
    ae_vector cgn;
    ae_vector xp;
    double fc;
    double fn;
    double fp;
    ae_vector d;
    ae_matrix cleic;
    ae_int_t nec;
    ae_int_t nic;
    double lastgoodstep;
    double lastscaledgoodstep;
    double maxscaledgrad;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repvaridx;
    ae_int_t repterminationtype;
    double repdebugeqerr;
    double repdebugfs;
    double repdebugff;
    double repdebugdx;
    ae_int_t repdebugfeasqpits;
    ae_int_t repdebugfeasgpaits;
    ae_vector xstart;
    snnlssolver solver;
    double fbase;
    double fm2;
    double fm1;
    double fp1;
    double fp2;
    double xm1;
    double xp1;
    double gm1;
    double gp1;
    ae_int_t cidx;
    double cval;
    ae_vector tmpprec;
    ae_vector tmp0;
    ae_int_t nfev;
    ae_int_t mcstage;
    double stp;
    double curstpmax;
    double activationstep;
    ae_vector work;
    linminstate lstate;
    double trimthreshold;
    ae_int_t nonmonotoniccnt;
    ae_matrix bufyk;
    ae_matrix bufsk;
    ae_vector bufrho;
    ae_vector buftheta;
    ae_int_t bufsize;
    double teststep;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
    ae_vector invs;
} minbleicstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t varidx;
    ae_int_t terminationtype;
    double debugeqerr;
    double debugfs;
    double debugff;
    double debugdx;
    ae_int_t debugfeasqpits;
    ae_int_t debugfeasgpaits;
    ae_int_t inneriterationscount;
    ae_int_t outeriterationscount;
} minbleicreport;
#endif
#if defined(AE_COMPILE_LPQPSERV) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_GIPM) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t np;
    ae_int_t nf;
    ae_int_t mflex;
    ae_int_t mhard;
    ae_vector xp;
    ae_vector zlp;
    ae_vector zup;
    ae_vector yp;
    ae_vector yl;
    ae_vector yu;
    ae_vector yv;
    ae_vector xs;
    ae_vector gl;
    ae_vector gu;
    ae_vector zls;
    ae_vector zus;
} gipmvars;
typedef struct
{
    ae_vector rp;
    ae_vector rs;
    ae_vector rgl;
    ae_vector rgu;
    ae_vector ry;
    ae_vector rcl;
    ae_vector rcu;
    ae_vector rlp;
    ae_vector rup;
    ae_vector rls;
    ae_vector rus;
    ae_vector rv;
} gipmrhs;
typedef struct
{
    ae_vector tls;
    ae_vector tus;
    ae_vector tgl;
    ae_vector tgu;
    ae_vector ts;
    ae_vector ta;
    ae_vector t0;
    ae_vector t1;
    ae_vector t2;
    ae_vector t3;
    ae_vector t4;
    ae_vector t5;
    ae_vector wx;
    ae_vector wy;
    ae_vector wyv;
    ae_vector d0;
    ae_vector d1;
    ae_vector d1v;
} gipmcondensedsystem;
typedef struct
{
    ae_int_t nraw;
    ae_int_t mflex;
    ae_int_t mhard;
    ae_vector x0;
    ae_vector bndlx;
    ae_vector bndux;
    ae_vector hasbndlx;
    ae_vector hasbndux;
    ae_vector isequality;
    ae_vector fscales;
    ae_bool isfirstorder;
    ae_vector qnmask;
    ae_bool isqnmasknonzero;
    double trustradxp;
    ae_bool mudependent;
    double eps;
    ae_int_t maxits;
    ae_int_t requesttype;
    ae_vector reportx;
    double reportf;
    ae_vector querydata;
    double querymu;
    ae_vector replyfi;
    sparsematrix replysj;
    ae_vector replysol;
    ae_vector replyprod;
    ae_bool hasjac;
    ae_bool factsuccess;
    ae_vector elp;
    ae_vector eup;
    ae_vector els;
    ae_vector eus;
    ae_vector finitebndlx;
    ae_vector finitebndux;
    ae_vector safeguardbndlp;
    ae_vector safeguardbndup;
    double nrmx0;
    double absf0;
    double mu;
    double lambdav;
    double lambdadecay;
    ae_int_t subsequentfactfailures;
    gipmvars current;
    gipmvars best;
    varsfuncjac currentfj;
    varsfuncjac bestfj;
    double besterr;
    ae_int_t rhsstagnationcounter;
    ae_bool filterready;
    nlpfilter filter;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_bool dolaconictrace;
    ae_int_t nonmonotonicmemlen;
    ae_int_t nonmonotonicpos;
    ae_vector nonmonotonicmerit;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    stimer timertotal;
    ae_nxpool mnx1pool;
    gipmrhs rhs;
    gipmcondensedsystem condensed;
    gipmvars delta;
    gipmvars corr;
    ae_vector tmphdxp;
    ae_vector tmpjdxp;
    ae_vector tmpjtdy;
    gipmvars trial;
    varsfuncjac trialfj;
    rcommstate rstate;
} gipmstate;
#endif
#if defined(AE_COMPILE_GQPIPM) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    sparsematrix lowerh;
    ae_vector ordervaridx;
    ae_vector injectionoffsets;
} gqpsparseconichessian;
typedef struct
{
    ae_bool hasdenseterm;
    ae_int_t nvars;
    ae_vector varidx;
    ae_matrix q;
} gqpoptionaldense;
typedef struct
{
    ae_int_t algomode;
    ae_int_t hessmemlen;
    ae_int_t hessmaxoffdiag;
    ae_int_t n;
    ae_int_t mlc;
    ae_int_t mqc;
    ae_int_t msocc;
    ae_vector x0;
    ae_vector wrkbndl;
    ae_vector wrkbndu;
    ae_vector finiterawbndl;
    ae_vector finiterawbndu;
    ae_vector isfixed;
    ae_vector fixedidx;
    ae_int_t fixedvarscnt;
    ae_vector c;
    ae_vector iotac;
    ae_matrix denseq;
    sparsematrix sparseq1;
    gqpoptionaldense optionaldenseq1;
    ae_int_t nq;
    ae_int_t cvxobjective;
    sparsematrix sparselc;
    ae_vector lcbndl;
    ae_vector lcbndu;
    xquadraticconstraints xqc;
    xconicconstraints xcc;
    ae_obj_array optionaldensexqc;
    ae_obj_array xchsparse;
    ae_int_t pctype;
    ae_int_t cvxqc;
    ae_int_t cvxcc;
    double eps;
    ae_int_t maxits;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_bool dotrace;
    ae_bool dolaconictrace;
    stimer timertotal;
    gipmstate solver;
    sparsematrix curjac;
    ae_matrix currenth;
    ae_vector tmpd1;
    ae_vector tmpd2;
    ae_matrix currentch;
    ae_matrix tmpwj;
    ae_vector currentx;
    ae_vector currentlag;
    sparsematrix sparseh;
    sparsematrix sparsehdeps;
    sparsematrix sparsesys;
    ae_vector priorities;
    spcholanalysis analysis;
    xbfgshessian hess;
    ae_vector basehsr1diag;
    ae_vector basehsr1corrd;
    ae_matrix basehsr1corrc;
    ae_int_t basehrank;
    ae_nxpool nrealpool;
    ae_vector totalnl;
    ae_vector totalnu;
    sparsematrix wrklc;
    ae_vector fscales;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector tmp3;
    ae_vector tmpx;
    ae_vector tmprhsy;
    ae_vector tmpi;
    ae_matrix dummy2;
    ae_vector tmphr;
    ae_vector tmphc;
    ae_vector tmphk;
    ae_vector tmphr2;
    ae_vector tmphc2;
    ae_vector tmphk2;
    ae_vector tmphv;
    ae_vector tmphv2;
    ae_vector tmpb;
    ae_vector doqnupdates;
} gqpipmstate;
#endif
#if defined(AE_COMPILE_MINLBFGS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    ae_vector s;
    double diffstep;
    ae_int_t nfev;
    ae_int_t mcstage;
    ae_int_t k;
    ae_int_t q;
    ae_int_t p;
    ae_vector rho;
    ae_matrix yk;
    ae_matrix sk;
    ae_vector xp;
    ae_vector theta;
    ae_vector d;
    double stp;
    double longeststp;
    ae_vector work;
    double fold;
    double trimthreshold;
    ae_vector xbase;
    ae_int_t prectype;
    double gammak;
    ae_matrix denseh;
    ae_vector diagh;
    ae_vector precc;
    ae_vector precd;
    ae_matrix precw;
    ae_int_t preck;
    precbuflbfgs precbuf;
    precbuflowrank lowrankbuf;
    double fbase;
    double fm2;
    double fm1;
    double fp1;
    double fp2;
    double stplimit;
    ae_vector autobuf;
    ae_vector invs;
    ae_int_t protocolversion;
    ae_bool userterminationneeded;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool xupdated;
    double teststep;
    rcommstate rcommv2;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    linminstate lstate;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
} minlbfgsstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
} minlbfgsreport;
#endif
#if defined(AE_COMPILE_VIPMSOLVER) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_vector x;
    ae_vector g;
    ae_vector w;
    ae_vector t;
    ae_vector p;
    ae_vector y;
    ae_vector z;
    ae_vector v;
    ae_vector s;
    ae_vector q;
} vipmvars;
typedef struct
{
    sparsematrix rawsystem;
    ae_vector effectivediag;
    ae_vector isdiagonal;
    ae_vector rowdegrees;
    ae_vector coldegrees;
    ae_int_t ntotal;
    spcholanalysis analysis;
    ae_vector priorities;
    ae_vector diagterm;
    ae_vector dampterm;
    ae_vector tmpb;
    ae_vector tmprhs;
    ae_vector tmpcorr;
} vipmreducedsparsesystem;
typedef struct
{
    ae_vector sigma;
    ae_vector beta;
    ae_vector rho;
    ae_vector nu;
    ae_vector tau;
    ae_vector alpha;
    ae_vector gammaz;
    ae_vector gammas;
    ae_vector gammaw;
    ae_vector gammaq;
} vipmrighthandside;
typedef struct
{
    ae_bool slacksforequalityconstraints;
    ae_bool normalize;
    ae_int_t n;
    ae_int_t nmain;
    double epsp;
    double epsd;
    double epsgap;
    ae_bool islinear;
    ae_vector scl;
    ae_vector invscl;
    ae_vector xorigin;
    double targetscale;
    ae_vector c;
    ae_matrix denseh;
    sparsematrix sparseh;
    ae_vector diagr;
    ae_int_t hkind;
    ae_bool isdiagonalh;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector rawbndl;
    ae_vector rawbndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_matrix denseafull;
    ae_matrix denseamain;
    sparsematrix sparseafull;
    sparsematrix sparseamain;
    sparsematrix combinedaslack;
    ae_vector ascales;
    ae_vector aflips;
    ae_vector b;
    ae_vector r;
    ae_vector hasr;
    ae_int_t mdense;
    ae_int_t msparse;
    vipmvars current;
    vipmvars best;
    vipmvars trial;
    vipmvars deltaaff;
    vipmvars deltacorr;
    ae_vector isfrozen;
    ae_vector hasgz;
    ae_vector hasts;
    ae_vector haswv;
    ae_vector haspq;
    ae_int_t cntgz;
    ae_int_t cntts;
    ae_int_t cntwv;
    ae_int_t cntpq;
    ae_int_t repiterationscount;
    ae_int_t repncholesky;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_int_t factorizationtype;
    ae_bool factorizationpoweredup;
    ae_bool factorizationpresent;
    ae_vector diagdz;
    ae_vector diagdzi;
    ae_vector diagdziri;
    ae_vector diagds;
    ae_vector diagdsi;
    ae_vector diagdsiri;
    ae_vector diagdw;
    ae_vector diagdwi;
    ae_vector diagdwir;
    ae_vector diagdq;
    ae_vector diagdqi;
    ae_vector diagdqiri;
    ae_vector diagddr;
    ae_vector diagde;
    ae_vector diagder;
    ae_matrix factdensehaug;
    ae_vector factregdhrh;
    ae_vector factinvregdzrz;
    ae_vector factregewave;
    ae_vector facttmpdiag;
    ae_vector facttmpdamp;
    vipmreducedsparsesystem reducedsparsesystem;
    vipmrighthandside rhs;
    ae_vector rhsalphacap;
    ae_vector rhsbetacap;
    ae_vector rhsnucap;
    ae_vector rhstaucap;
    ae_vector deltaxy;
    ae_vector tmphx;
    ae_vector tmpax;
    ae_vector tmpaty;
    vipmvars zerovars;
    ae_vector dummyr;
    ae_vector tmpy;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_matrix tmpr2;
    ae_vector tmplaggrad;
    ae_vector tmpi;
    sparsematrix tmpsparse0;
} vipmstate;
#endif
#if defined(AE_COMPILE_IPM2SOLVER) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t ntotal;
    ae_int_t m;
    ae_vector x;
    ae_vector g;
    ae_vector t;
    ae_vector y;
    ae_vector z;
    ae_vector s;
} ipm2vars;
typedef struct
{
    sparsematrix compactkkt;
    ae_vector compactpriorities;
    spcholanalysis analysis;
    ae_vector extendedrawdiagonal;
    ae_vector extendedeffdiagonal;
    ae_vector compacteffdiagonal;
    ae_vector extendedrhs;
    ae_vector compactrhs;
    ae_vector u0;
    ae_vector u1;
    ae_vector v0;
    ae_vector v1;
    ae_vector vm0;
    ae_vector vm1;
    ae_vector um0;
    ae_vector d0;
    ae_vector d1;
    ae_vector d2;
    ae_vector d3;
    ae_vector d4;
    ae_vector d5;
    ae_vector d6;
    ae_vector d7;
    ae_vector rowdegrees;
    ae_vector coldegrees;
    ae_vector tmpregrhs;
} ipm2reducedsystem;
typedef struct
{
    ae_vector gammagz;
    ae_vector gammats;
    ae_vector ed;
    ae_vector ea;
    ae_vector el;
    ae_vector eu;
} ipm2righthandside;
typedef struct
{
    ae_int_t nuser;
    ae_int_t naug;
    ae_int_t ntotal;
    ae_bool hasuserpermutation;
    ae_vector userpermutation;
    double epsp;
    double epsd;
    double epsgap;
    ae_int_t maxipmits;
    ae_bool islinear;
    ae_vector sclx;
    ae_vector invsclx;
    ae_vector xoriginx;
    double targetscale;
    ae_vector ce;
    sparsematrix sparsehe;
    ae_vector diagr;
    ae_bool isdiagonalh;
    double greg;
    ae_vector gregoriginxuser;
    ae_vector gregoriginyuser;
    ae_vector gregdiag;
    ae_vector gregrhsx;
    ae_vector gregrhsy;
    ae_vector gregoriginx;
    ae_vector gregoriginy;
    ae_vector rawbndl;
    ae_vector rawbndu;
    ae_vector bndle;
    ae_vector bndue;
    ae_vector bndlef;
    ae_vector bnduef;
    ae_vector hasbndle;
    ae_vector hasbndue;
    sparsematrix rawae;
    ae_vector ascales;
    ae_int_t mraw;
    ipm2vars current;
    ipm2vars best;
    ipm2vars delta;
    ipm2vars corr;
    ae_vector hasgz;
    ae_vector hasts;
    ae_vector maskgz;
    ae_vector maskts;
    ae_int_t cntgz;
    ae_int_t cntts;
    ae_vector currenthx;
    ae_vector currentax;
    ae_vector currentaty;
    ipm2righthandside rhstarget;
    ae_int_t repiterationscount;
    ae_int_t repncholesky;
    ae_bool dotrace;
    ae_bool dotimers;
    ae_bool dodetailedtrace;
    stimer timertotal;
    stimer timeranalyze;
    stimer timerfactorize;
    stimer timerspsymmsolve;
    stimer timerothersolve;
    ipm2reducedsystem reducedsystem;
    ipm2righthandside rhsprogress;
    ae_vector tmphx;
    ae_vector tmpax;
    ae_vector tmpaty;
    ae_vector dummyr;
    ae_vector tmp0;
    ae_matrix tmpr2;
    sparsematrix tmpa;
    ae_vector tmpal;
    ae_vector tmpau;
    sparsematrix tmpsparse0;
    sparsematrix tmplowerh;
    ae_matrix tmpccorr;
    ae_vector tmpdcorr;
} ipm2state;
#endif
#if defined(AE_COMPILE_NLCFSQP) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    vipmstate ipmsolver;
    ipm2state ipm2;
    ae_vector ordering;
    ae_vector elasticordering;
    sparsematrix pattern;
    sparsematrix elasticpattern;
    sparsematrix upcomingpattern;
    ae_vector curb;
    ae_vector curbndl;
    ae_vector curbndu;
    ae_vector cural;
    ae_vector curau;
    sparsematrix sparserawlc;
    sparsematrix sparseefflc;
    ae_vector lastlaglc;
    double penalty0;
    double penalty1;
    ae_matrix denseh;
    ae_vector dummy1;
    ae_matrix densedummy;
    sparsematrix sparsetmp;
    sparsematrix sparsedummy;
    ae_vector tmps;
    ae_vector tmporigin;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector retainnegativebclm;
    ae_vector retainpositivebclm;
    ae_vector rescalelag;
    ae_vector isinactive;
    ae_matrix tmpcorrc;
    ae_vector tmpdiag;
    ae_vector isbounded;
    ae_vector hessdiag;
    ae_matrix hesscorrc;
    ae_vector hesscorrd;
    ae_int_t hessrank;
    sparsematrix hesssparsediag;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector hasal;
    ae_vector hasau;
} minfsqpsubsolver;
typedef struct
{
    ae_int_t n;
    ae_int_t lccnt;
    ae_int_t nnlc;
    ae_bool usedensebfgs;
    double inittrustrad;
    ae_vector s;
    sparsematrix scla;
    ae_vector hasal;
    ae_vector hasau;
    ae_vector sclal;
    ae_vector sclau;
    ae_vector lcscales;
    ae_vector lcsrcidx;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_vector hasnl;
    ae_vector hasnu;
    ae_vector rawnl;
    ae_vector rawnu;
    ae_vector scalednl;
    ae_vector scalednu;
    ae_vector fscaleszzz;
    ae_vector invfscales;
    double sqpbigscale;
    double sqpsmallscale;
    nlpstoppingcriteria criteria;
    ae_int_t bfgsresetfreq;
    ae_int_t additsforctol;
    double ctol;
    ae_vector x;
    ae_vector fi;
    sparsematrix sj;
    double f;
    ae_bool needfisj;
    ae_bool xupdated;
    ae_bool werepointsbelowctol;
    double trustrad;
    double trustradgrowth;
    ae_int_t trustradstagnationcnt;
    ae_int_t fstagnationcnt;
    varsfuncjac stepk;
    ae_vector x0;
    ae_vector xprev;
    minfsqpsubsolver subsolver;
    xbfgshessian hess;
    ae_vector lagbcmult;
    ae_vector lagxcmult;
    varsfuncjac cand;
    varsfuncjac probe;
    ae_vector dtrial;
    ae_vector d0;
    ae_vector dmu;
    nlpfilter filter;
    ae_vector replagbc;
    ae_vector replaglc;
    ae_vector replagnlc;
    ae_vector varscales;
    ae_vector tmppend;
    ae_vector tmphd;
    ae_vector dummylagbcmult;
    ae_vector dummylagxcmult;
    ae_vector tmplaggrad;
    ae_vector tmpcandlaggrad;
    ae_vector tmphdiag;
    ae_vector sclagtmp0;
    ae_vector sclagtmp1;
    ae_vector mftmp0;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    double repsclerr;
    stimer timertotal;
    stimer timerqpgen;
    stimer timeripminit;
    stimer timeripmsolve;
    stimer timercallback;
    rcommstate rstate;
} minfsqpstate;
#endif
#if defined(AE_COMPILE_DIFFEVO) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t cnt;
    ae_matrix x;
    ae_matrix rawreplies;
    ae_matrix fitness;
    ae_matrix deparams;
} gdemopopulation;
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_vector s;
    ae_bool hasx0;
    ae_vector x0;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector finitebndl;
    ae_vector finitebndu;
    ae_matrix densea;
    ae_vector al;
    ae_vector au;
    ae_vector hasal;
    ae_vector hasau;
    ae_vector rawnl;
    ae_vector rawnu;
    ae_vector hasnl;
    ae_vector hasnu;
    ae_vector ascales;
    ae_int_t popsize;
    ae_int_t epochscnt;
    ae_int_t constrmode;
    double rho1;
    double rho2;
    double eps;
    double nsample;
    ae_int_t profile;
    ae_bool fixedparams;
    ae_int_t fixedstrategy;
    double fixedcrossoverprob;
    double fixeddifferentialweight;
    ae_bool stoponsmallf;
    double smallf;
    double condfxepsf;
    double condfxepsx;
    double mucr;
    double muf;
    ae_int_t requesttype;
    ae_vector reportx;
    double reportf;
    ae_int_t querysize;
    ae_vector querydata;
    ae_vector replyfi;
    rcommstate rstate;
    hqrndstate rs;
    gdemopopulation population;
    gdemopopulation offspring;
    ae_matrix xbest;
    ae_vector fitbest;
    ae_vector rawfbest;
    ae_int_t repterminationtype;
    ae_int_t repiterationscount;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_vector tmpzero;
    sparsematrix dummysparsea;
    ae_vector goodbndl;
    ae_vector goodbndu;
    ae_vector empiricalbndl;
    ae_vector empiricalbndu;
} gdemostate;
#endif
#if defined(AE_COMPILE_SSGD) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_vector x0;
    ae_vector s;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector finitebndl;
    ae_vector finitebndu;
    ae_matrix densea;
    ae_vector al;
    ae_vector au;
    ae_vector hasal;
    ae_vector hasau;
    ae_vector rawnl;
    ae_vector rawnu;
    ae_vector hasnl;
    ae_vector hasnu;
    ae_vector ascales;
    ae_int_t maxits;
    double rate0;
    double rate1;
    double momentum;
    double blur0;
    double blur1;
    ae_int_t outerits;
    double rho;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_bool needfi;
    ae_bool xupdated;
    ae_vector avgsqj;
    ae_vector fscales;
    hqrndstate rs;
    ae_vector xbest;
    ae_vector fibest;
    ae_vector xcur;
    ae_vector gcur;
    ae_vector dcur;
    double currate;
    double curblur;
    double zcursecantstep;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    rcommstate rstate;
    ae_vector sqj;
    ae_vector xleft;
    ae_vector xright;
    ae_vector fileft;
    ae_vector firight;
    ae_vector xoffs0;
    ae_vector xoffs1;
    ae_vector tmpzero;
    sparsematrix dummysparsea;
} ssgdstate;
#endif
#if defined(AE_COMPILE_ECQPSOLVER) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    sparsematrix haug;
    ae_vector priorities;
    spcholanalysis analysis;
    ae_vector effdiagonal;
    ae_vector dampdiagonal;
    ae_vector dx;
    ae_vector bx;
    ae_vector dh;
    fblsgmresstate gmressolver;
    ae_int_t repiterationscount;
    ae_int_t repncholesky;
    ae_bool dotrace;
    ae_bool dotimers;
    stimer timertotal;
    stimer timeranalyze;
    stimer timerfactorize;
    stimer timerspsymmsolve;
    stimer timerothersolve;
} ecqpstate;
#endif
#if defined(AE_COMPILE_GIPM3) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t meq;
    ae_int_t mi;
    ae_int_t mh;
    ae_int_t mxx;
    ae_vector x;
    ae_vector s;
    ae_vector ze;
    ae_vector zi;
    ae_vector zh;
    ae_vector zs;
    ae_bool hasfunctions;
    double f;
    ae_vector g;
    ae_vector ce;
    ae_vector ci;
    ae_vector ch;
    ae_bool haslaggrad;
    ae_vector laggrad;
    ae_vector fi;
} gipm3vars;
typedef struct
{
    ae_vector zeprim;
    ae_vector ziprim;
    ae_vector zhprim;
    ae_vector zsprim;
} gipm3primalmult;
typedef struct
{
    ae_vector rpx;
    ae_vector rps;
    ae_vector rze;
    ae_vector rzi;
    ae_vector rzh;
    ae_vector rzs;
} gipm3rhs;
typedef struct
{
    ae_vector wx;
    ae_vector wy;
    ae_vector d0;
    ae_vector d1;
    ae_vector cached0;
} gipm3condensedsystem;
typedef struct
{
    double vscl;
    double rege;
    double regi;
    double regh;
    double regs;
    ae_vector wx;
    ae_vector wy;
    ae_vector d0;
    ae_vector d1;
    ae_vector regem;
    ae_vector regim;
    ae_vector reghm;
    ae_vector regsm;
    ae_vector regi3;
    ae_vector regs3;
    ae_vector cached1;
} gipm3condensedleastsquaressystem;
typedef struct
{
    ae_int_t n;
    ae_int_t meq;
    ae_int_t mi;
    ae_int_t mh;
    ae_int_t mxx;
    ae_vector x0;
    ae_vector rhseq;
    ae_vector rhsi;
    ae_vector rhsh;
    ae_vector pg;
    ae_vector fractohard;
    double primalbound;
    double dualbound;
    ae_vector callerfscales;
    double esdualbound;
    double esprimbound;
    ae_int_t esminits;
    double estol;
    ae_bool eshasintmask;
    ae_vector esintmask;
    ae_bool sendqnupdates;
    ae_bool sendqnupdatesonfailedstep;
    ae_bool refactorizeafterqnupdate;
    ae_bool longsteps;
    double maxnewtontrustratio;
    double muepsfactor;
    double dualerrwgt;
    double mudecaylinear;
    double mudecaypower;
    double smalltrustrad;
    double eps;
    ae_int_t maxits;
    ae_int_t maxcheckpointstagnationits;
    ae_int_t requesttype;
    ae_vector reportx;
    double reportf;
    ae_vector querydata;
    ae_vector replyfi;
    ae_vector replyg;
    ae_vector replysol;
    ae_vector replyprod;
    ae_vector replyv;
    ae_bool replywasrescale;
    double replyalpha;
    ae_bool hasjac;
    ae_bool factsuccess;
    ae_bool userterminationneeded;
    sparsematrix probingsj;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_bool doprobeonfailure;
    ae_bool doanalyzestep;
    ae_bool doanytrace;
    ae_bool dotracelaconic;
    ae_smart_ptr _smonitor;
    smoothnessmonitor *smonitor;
    double besterr;
    double besterrchange;
    gipm3vars best;
    gipm3vars scalingt;
    gipm3vars current;
    double currentmerit;
    gipm3primalmult currentprimmult;
    double currentmu;
    double mumin;
    double trustrad;
    double trustradscale;
    ae_int_t fstagnationcnt;
    double curbigval;
    double curbiggrowth;
    double lambdav;
    ae_vector lambdadiag;
    double lambdadecay;
    ae_bool lambdafound;
    ae_bool infinitiesdetected;
    ae_bool failedtorecoverfrominfinities;
    ae_int_t subsequentfactfailures;
    ae_bool firstouteriteration;
    ae_bool unboundednesssuspected;
    ae_int_t inneriterationidx;
    ae_bool hasqnlag0;
    ae_vector qnlag0;
    ae_int_t nonmonotoniccurstored;
    ae_int_t nonmonotonicmaxstored;
    ae_int_t nonmonotonicpos;
    ae_vector nonmonotonicmerit;
    double nrmx0;
    double absf0;
    ae_int_t newtonresult;
    double newtonproposedlambda;
    rcommstate rnewtonstate;
    ae_int_t doglegresult;
    double doglegproposedlambda;
    rcommstate rlongdoglegstate;
    ae_bool trstepshortened;
    gipm3vars deltanewt;
    gipm3vars scaledg;
    gipm3vars deltacauchy;
    gipm3vars deltacand;
    ae_vector hdeltanewtx;
    ae_vector jdeltanewtx;
    ae_vector jtdeltanewty;
    ae_vector hscaledgx;
    ae_vector jscaledgx;
    ae_vector jtscaledgy;
    ae_vector hdeltacauchyx;
    ae_vector jdeltacauchyx;
    ae_vector jtdeltacauchyy;
    ae_vector jdeltacandx;
    gipm3vars corr;
    gipm3vars cand;
    double candmerit;
    gipm3vars candsd;
    gipm3vars probe;
    gipm3primalmult candprimmult;
    gipm3condensedsystem condensed;
    gipm3condensedleastsquaressystem condensedlls;
    gipm3rhs rhs;
    gipm3rhs candrhs;
    gipm3rhs candsdrhs;
    gipm3rhs tmprhs;
    ae_vector tmphdxp;
    ae_vector tmpjdxp;
    ae_vector tmpjtdy;
    ae_vector tmpztotal;
    ae_nxpool mnx1pool;
    ae_vector qnlag1;
    ae_int_t repnfev;
    ae_int_t repnfact;
    ae_int_t repnllsfact;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    double repearlyerrorestimate;
    double repsclerr;
    rcommstate rstate;
} gipm3state;
#endif
#if defined(AE_COMPILE_NLCGIPM3) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_bool isdense;
    ae_bool isconvex;
    ae_int_t nraw;
    ae_int_t mlc;
    ae_int_t mnlc;
    ae_int_t mxx;
    double fixtol;
    ae_vector raws;
    double primalbound;
    double dualbound;
    ae_vector islinear;
    ae_vector idxnonlinear;
    ae_int_t qnmemlen;
    double esdualbound;
    double esprimbound;
    ae_int_t esminits;
    double estol;
    ae_bool eshasintmask;
    ae_vector esintmask;
    ae_bool dononlinearityreports;
    ae_int_t nwrk;
    ae_int_t nnonlinear;
    ae_vector scaledx0;
    ae_bool hasbc;
    ae_bool hasfixedbc;
    ae_bool hasnonfixedbc;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_vector finitescaledbndl;
    ae_vector finitescaledbndu;
    ae_vector isfixed;
    ae_int_t cntfixed;
    ae_vector idxfixed;
    ae_int_t mhardbc;
    ae_vector hardbca;
    ae_vector hardbcb;
    ae_vector x;
    ae_vector rawreplyfi;
    sparsematrix rawreplysj;
    ae_vector rawreplyfs;
    double reportf;
    ae_bool needfisj;
    ae_bool xupdated;
    ae_bool userterminationneeded;
    ae_int_t offsbce;
    ae_int_t offshardbc;
    ae_int_t hardbcfirstch;
    ae_vector constrbucket;
    ae_vector srceqslkidx;
    ae_vector srceqslkscl;
    ae_vector srceihidx;
    ae_vector srceihscl;
    ae_vector srceihsft;
    xbfgshessian hess;
    ae_vector hessscales;
    ae_vector probingx;
    ae_vector probingfi;
    ae_vector probingfs;
    sparsematrix probingsj;
    ae_vector lastx;
    ae_vector xxlastfi;
    ae_vector xxlastfs;
    sparsematrix xxlastsj;
    double lastrawf0;
    ae_vector basex;
    ae_vector xxbasefi;
    ae_vector xxbasefs;
    sparsematrix xxbasesj;
    double baserawf0;
    ae_vector bestx;
    ae_vector xxbestfi;
    ae_vector xxbestfs;
    sparsematrix ccbestsj;
    double bestrawf0;
    ae_int_t hessmemlen;
    ae_int_t hessmaxrank;
    ae_vector basehsr1diag;
    ae_vector basehsr1corrd;
    ae_matrix basehsr1corrc;
    ae_int_t basehrank;
    double baselinecurvaturev;
    ae_vector baselinecurvaturediag;
    ae_bool hasinitialpointinrawreply;
    ae_bool hasprobing;
    ae_bool haslast;
    ae_bool hasbase;
    ae_bool hasbaseh;
    ae_bool hasbasekkt;
    ae_bool hasbest;
    ae_vector priorities;
    spcholanalysis nonbcanalysis;
    spcholanalysis nonbcanalysislls;
    sparsematrix nonbcsparsesys;
    sparsematrix nonbcsparsellssys;
    sparsematrix xxlastanalyzedjac;
    sparsematrix xxlastanalyzedllsjac;
    ae_bool hassparsityanalyzed;
    ae_bool hasllssparsityanalyzed;
    ae_bool factorized;
    ae_bool factorizedlls;
    ae_vector cholinvdh;
    ae_vector cholinvdhhardbca;
    ae_vector llscholinvdh;
    ae_vector llscholinvdhhardbca;
    ae_vector wrkfscales;
    ae_vector invwrkfscales;
    ae_vector initwrkfscales;
    double fbigscale;
    double fsmallscale;
    ae_vector rawfrescale;
    ae_bool wasrawfrescale;
    gipm3state solver;
    sparsematrix sparselc;
    ae_vector lcbndl;
    ae_vector lcbndu;
    ae_vector lcsrcidx;
    ae_vector lcscales;
    ae_vector rawnl;
    ae_vector rawnu;
    ae_vector nlcishard;
    ae_int_t meqnlc;
    ae_int_t meqlc;
    ae_int_t msofteqbc;
    ae_int_t meqxx;
    ae_int_t minlc;
    ae_int_t milc;
    ae_int_t mixx;
    ae_int_t mhnlc;
    ae_int_t mhlc;
    ae_int_t mhxx;
    ae_int_t mtotale;
    ae_int_t mtotali;
    ae_int_t mtotalh;
    ae_vector ceq;
    ae_vector ci;
    ae_vector ch;
    ae_vector chislinear;
    double eps;
    ae_int_t maxits;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    double repsclerr;
    double repearlyerrorestimate;
    ae_vector xs;
    ae_vector replagbc;
    ae_vector replaglc;
    ae_vector replagnlc;
    ae_vector repnonlinearityreport;
    ae_bool dotrace;
    ae_bool dolaconictrace;
    ae_bool doprobeonfailure;
    ae_bool doanalyzestep;
    ae_bool dotracedetailedmodel;
    stimer timertotal;
    stimer timerfactorize;
    stimer timersolve;
    stimer timercallback;
    stimer timerreport;
    stimer timeranalyze;
    stimer timermv;
    stimer timerrescale;
    ae_vector tmp0;
    ae_vector tmplag;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector tmp3;
    ae_vector tmpb;
    ae_vector tmpgfs0;
    ae_vector tmpgfs1;
    ae_vector tmpmrnrm;
    ae_vector tmpmrmultiply;
    ae_vector tmpmrmultiplyvars;
    ae_vector tmpmrmultiplycols;
    ae_vector tmpmrset;
    sparsematrix tmpsparse;
    rcommstate rstate;
} nlpgipm3state;
#endif
#if defined(AE_COMPILE_NLCSQP) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    vipmstate ipmsolver;
    ipm2state ipm2;
    ae_vector curb;
    ae_vector curbndl;
    ae_vector curbndu;
    ae_vector cural;
    ae_vector curau;
    sparsematrix sparserawlc;
    sparsematrix sparseefflc;
    ae_vector d0;
    ae_matrix denseh;
    ae_vector dummy1;
    ae_matrix densedummy;
    sparsematrix sparsedummy;
    ae_vector tmps;
    ae_vector tmporigin;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector retainnegativebclm;
    ae_vector retainpositivebclm;
    ae_vector rescalelag;
    ae_matrix tmpcorrc;
    ae_vector tmpdiag;
    ae_vector hessdiag;
    ae_matrix hesscorrc;
    ae_vector hesscorrd;
    ae_int_t hessrank;
    sparsematrix hesssparsediag;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector hasal;
    ae_vector hasau;
} minsqpsubsolver;
typedef struct
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_bool usedensebfgs;
    ae_vector s;
    ae_matrix scaledcleic;
    ae_vector lcsrcidx;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    nlpstoppingcriteria criteria;
    ae_int_t bfgsresetfreq;
    ae_vector x;
    ae_vector fi;
    ae_matrix j;
    double f;
    ae_bool needfij;
    ae_bool xupdated;
    double meritmu;
    double trustrad;
    double trustradgrowth;
    ae_int_t trustradstagnationcnt;
    ae_int_t fstagnationcnt;
    varsfuncjac stepk;
    ae_vector x0;
    ae_vector xprev;
    ae_vector fscales;
    ae_vector tracegamma;
    minsqpsubsolver subsolver;
    xbfgshessian hess;
    ae_obj_array nonmonotonicmem;
    ae_int_t nonmonotoniccnt;
    ae_vector lagbcmult;
    ae_vector lagxcmult;
    varsfuncjac cand;
    varsfuncjac corr;
    varsfuncjac probe;
    varsfuncjac currentlinearization;
    ae_vector dtrial;
    ae_vector d0;
    ae_vector d1;
    ae_vector dmu;
    ae_vector tmppend;
    ae_vector tmphd;
    ae_vector dummylagbcmult;
    ae_vector dummylagxcmult;
    ae_vector tmplaggrad;
    ae_vector tmpcandlaggrad;
    ae_vector tmphdiag;
    ae_vector sclagtmp0;
    ae_vector sclagtmp1;
    ae_vector mftmp0;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    stimer timertotal;
    stimer timerqp;
    stimer timercallback;
    rcommstate rstate;
} minsqpstate;
#endif
#if defined(AE_COMPILE_NLCAUL) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t refreshfreq;
    ae_int_t modelage;
    spcholanalysis analysis;
    sparsematrix augsys;
    ae_int_t naug;
    ae_matrix s;
    ae_matrix y;
    ae_int_t updcnt;
    ae_matrix tmpcorrc;
    ae_vector tmpcorrd;
    ae_vector tmpdiag;
    ae_vector priorities;
    ae_vector dx;
    ae_matrix dummys;
    ae_matrix dummyy;
    ae_vector alphak;
    ae_vector rhok;
} minaulpreconditioner;
typedef struct
{
    ae_int_t n;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_vector s;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_vector finitebndl;
    ae_vector finitebndu;
    sparsematrix sparsea;
    ae_vector al;
    ae_vector au;
    ae_vector hasal;
    ae_vector hasau;
    ae_vector rawnl;
    ae_vector rawnu;
    ae_vector hasnl;
    ae_vector hasnu;
    ae_vector lcsrcidx;
    nlpstoppingcriteria criteria;
    ae_int_t maxouterits;
    ae_int_t restartfreq;
    ae_vector x;
    ae_vector fi;
    sparsematrix sj;
    double f;
    ae_bool needsj;
    ae_bool precrefreshupcoming;
    ae_bool xupdated;
    double rho;
    varsfuncjac xvirt;
    varsfuncjac xvirtprev;
    varsfuncjac xvirtbest;
    varsfuncjac xtrue;
    varsfuncjac xtrueprev;
    varsfuncjac xtruebest;
    double besterr;
    ae_int_t bestiteridx;
    ae_int_t besterrnegligibleupdates;
    ae_vector lagmultbc2;
    ae_vector lagmultxc2;
    ae_vector x0;
    ae_vector d;
    ae_vector du;
    ae_vector fscales;
    ae_vector tracegamma;
    minaulpreconditioner preconditioner;
    xbfgshessian hessaug;
    xbfgshessian hesstgt;
    ae_vector laggrad0;
    ae_vector laggradcur;
    ae_vector modtgtgrad0;
    ae_vector modtgtgrad1;
    ae_int_t inneriterationscount;
    ae_int_t outeriterationscount;
    ae_int_t fstagnationcnt;
    double longeststp;
    ae_int_t mcstage;
    linminstate lstate;
    double stp;
    double stplimit;
    ae_vector ascales;
    ae_vector tmpzero;
    ae_vector tmpy;
    ae_matrix dummy2;
    ae_vector tmpretrdelta;
    ae_vector tmpg;
    varsfuncjac xvirtprobe;
    varsfuncjac xtrueprobe;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_int_t repnfev;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_int_t nfev;
    ae_vector work;
    rcommstate rstate;
} minaulstate;
#endif
#if defined(AE_COMPILE_DFGENMOD) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_vector deltas;
    ae_matrix jac;
    ae_vector g;
    ae_matrix q;
} df2psmodel;
typedef struct
{
    ae_matrix w;
    ae_matrix invw;
    ae_matrix jac;
    ae_vector g;
    ae_matrix q;
    ae_vector tmp0;
    ae_matrix tmprhs;
} dfolsamodel;
typedef struct
{
    ae_int_t ncenters;
    double modelbase;
    double modelscale;
    ae_vector multscale;
    ae_matrix centers;
    ae_vector x0;
    ae_matrix crbf;
    ae_matrix clinear;
    minfsqpstate fsqpsolver;
    ae_matrix currentcloud;
    ae_int_t currentcloudsize;
    ae_vector distfromcenter;
    ae_vector distfromchosen;
    ae_vector available;
    ae_matrix curwrkset;
    ae_int_t curwrksetsize;
    ae_matrix residualdisplacements;
    ae_matrix q;
    ae_vector tmpcand;
    ae_vector tmpdisp;
    ae_vector tmpwinner;
    ae_vector tmpbndl;
    ae_vector tmpbndu;
    ae_vector tmpx0;
    ae_vector tmps;
    double winnerrating;
    sparsematrix cx;
    ae_vector clx;
    ae_vector cux;
    nlpstoppingcriteria crit;
    smoothnessmonitor smonitor;
} dforbfmodel;
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_bool isleastsquares;
    ae_int_t modeltype;
    double rad0;
    ae_int_t nnoisyrestarts;
    ae_vector x0;
    ae_vector s;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_vector finitebndl;
    ae_vector finitebndu;
    sparsematrix c;
    ae_vector cl;
    ae_vector cu;
    ae_vector nl;
    ae_vector nu;
    double epsx;
    double epsf;
    ae_int_t maxits;
    ae_int_t maxfev;
    double toosmalltrustrad;
    ae_int_t requesttype;
    ae_vector reportx;
    double reportf;
    ae_int_t querysize;
    ae_vector querydata;
    ae_vector replyfi;
    rcommstate rstate;
    rcommstate rstateimprove;
    rcommstate rstateupdate;
    ae_bool dotrace;
    ae_vector trustregion;
    ae_vector invtrustregion;
    double trustradfactor;
    double trustradbnd;
    ae_vector xp;
    ae_vector fpvec;
    double fp;
    double hp;
    ae_vector xk;
    ae_vector sk;
    ae_vector xn;
    ae_vector fkvec;
    ae_vector fnvec;
    double fk;
    double hk;
    ae_matrix wrkset;
    ae_int_t wrksetsize;
    hqrndstate rs;
    df2psmodel tpsmodel;
    dfolsamodel lsamodel;
    dforbfmodel rbfmodel;
    ae_bool infinitiesencountered;
    ae_bool recoveredfrominfinities;
    nlpfilter filter;
    ae_bool savepointstocloud;
    ae_matrix clouddata;
    ae_int_t cloudsize;
    ae_matrix cloudpending;
    ae_int_t cloudpendingcnt;
    ae_int_t successhistorylen;
    ae_int_t successhistorymax;
    ae_int_t successhistorynext;
    ae_vector successhistory;
    ae_int_t restartstoperform;
    double lasttrustrad;
    ae_matrix lastjac;
    ae_vector trustradhistory;
    ae_vector jacdiffhistory;
    ae_vector iteridxhistory;
    ae_int_t historylen;
    ae_int_t historynext;
    ae_vector tmpi;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector tmpdeltas;
    vipmstate ipmsolver;
    ae_matrix densedummy;
    sparsematrix sparsedummy;
    matinvreport invrep;
    ae_int_t repterminationtype;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repsubsolverits;
} dfgmstate;
#endif
#if defined(AE_COMPILE_DYNCRS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t m;
    ae_int_t n;
    ae_int_t maxallocated;
    ae_vector rowbegin;
    ae_vector rowend;
    ae_vector idx;
    ae_vector vals;
} dynamiccrs;
#endif
#if defined(AE_COMPILE_NLPPRESOLVE) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t mlc;
    ae_int_t mnlc;
    ae_int_t problemstatus;
    ae_vector x0;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector integralmask;
    ae_vector linearmask;
    ae_vector hardmask;
    dynamiccrs a;
    dynamiccrs at;
    ae_vector al;
    ae_vector au;
    ae_vector nl;
    ae_vector nu;
    ae_vector lcsrcidx;
    ae_vector lcscales;
    ae_vector nlcscales;
    ae_vector isfixed;
    ae_vector isdroppedlc;
    ae_vector fixedvals;
    niset setn;
    niset setn2;
    niset setm;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_int_t dbgemptycol;
    ae_int_t dbgemptylc;
    ae_int_t dbgnonbindingrows;
    ae_int_t dbgfixed;
    ae_int_t dbgsingletonrow;
    ae_int_t dbgdoubletonrow;
    ae_int_t dbgforcing;
    ae_int_t dbgirrelevant;
    ae_int_t dbgnobounds;
} nlppresolveworkspace;
typedef struct
{
    double eps;
    ae_int_t oldn;
    ae_int_t oldmlc;
    ae_int_t oldmnlc;
    ae_vector oldx0;
    ae_vector oldbndl;
    ae_vector oldbndu;
    ae_vector olds;
    ae_vector oldfinitebndl;
    ae_vector oldfinitebndu;
    ae_vector oldintegralmask;
    ae_vector oldlinearmask;
    ae_vector oldhardmask;
    sparsematrix olda;
    ae_vector oldal;
    ae_vector oldau;
    ae_vector oldlcsrcidx;
    ae_vector oldnl;
    ae_vector oldnu;
    ae_vector isoldfixed;
    ae_vector fixedvals;
    ae_vector packxperm;
    ae_vector unpackxperm;
    ae_int_t kxx;
    ae_bool justscale;
    ae_vector packeds;
    ae_int_t problemstatus;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_int_t newn;
    ae_int_t newmlc;
    ae_int_t newmnlc;
    ae_vector newx0;
    ae_vector newbndl;
    ae_vector newbndu;
    ae_vector news;
    ae_vector newintegralmask;
    ae_vector newlinearmask;
    ae_vector newhardmask;
    sparsematrix newa;
    ae_vector newal;
    ae_vector newau;
    ae_vector newlcsrcidx;
    ae_vector newnl;
    ae_vector newnu;
    ae_vector newlcscales;
    ae_vector newnlcscales;
    nlppresolveworkspace wrk;
    sparsematrix tmpa;
    sparsematrix tmpat;
} nlppresolveinfo;
#endif
#if defined(AE_COMPILE_MINNLC) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t solvertype;
    ae_int_t n;
    nlpstoppingcriteria criteria;
    ae_int_t aulitscnt;
    ae_bool xrep;
    double stpmax;
    double diffstep;
    double teststep;
    ae_int_t formulatype;
    double orbitrad0;
    ae_int_t orbitmaxnfev;
    ae_int_t fsqpadditsforctol;
    double fsqpctol;
    ae_int_t qnmemlen;
    ae_vector s;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector islinear;
    xlinearconstraints xlc;
    ae_int_t nnlc;
    ae_vector nl;
    ae_vector nu;
    ae_vector nlcishard;
    double primalbound;
    double esdualbound;
    double esprimbound;
    ae_int_t esminits;
    double estol;
    ae_bool eshasintmask;
    ae_vector esintmask;
    ae_bool dononlinearityreports;
    ae_int_t protocolversion;
    ae_bool issuesparserequests;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
    rcommstate rcommv2;
    rcommstate rstateaul;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_matrix scaledcleic;
    ae_vector xc;
    ae_vector xstart;
    ae_vector xbase;
    ae_vector fbase;
    ae_vector dfbase;
    ae_vector fm2;
    ae_vector fm1;
    ae_vector fp1;
    ae_vector fp2;
    ae_vector batchquerydata;
    ae_vector tmpspaf;
    ae_vector tmpsptolj;
    ae_int_t presolvermode;
    nlppresolveinfo presolver;
    ae_smart_ptr _aulsolverstate;
    minaulstate *aulsolverstate;
    ae_bool userterminationneeded;
    ae_smart_ptr _sqpsolverstate;
    minsqpstate *sqpsolverstate;
    ae_smart_ptr _fsqpsolverstate;
    minfsqpstate *fsqpsolverstate;
    ae_smart_ptr _orbitsolver;
    dfgmstate *orbitsolver;
    ae_int_t nreduced;
    ae_vector idxraw2red;
    ae_vector redx0;
    ae_vector reds;
    ae_vector redbl;
    ae_vector redbu;
    ae_smart_ptr _gipm3solver;
    nlpgipm3state *gipm3solver;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
    double repf;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    double repsclfeaserr;
    ae_int_t repdbgphase0its;
    double repearlyerrorestimate;
    ae_vector replagbc;
    ae_vector replagbcnz;
    ae_vector replaglc;
    ae_vector replagnlc;
    ae_vector repnonlinearityreport;
    ae_vector nlcidx;
    ae_vector nlcmul;
    ae_vector nlcadd;
    ae_int_t nlcnlec;
    ae_int_t nlcnlic;
    ae_vector tmpi;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
} minnlcstate;
typedef struct
{
    double f;
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
    double bcerr;
    ae_int_t bcidx;
    double lcerr;
    ae_int_t lcidx;
    double nlcerr;
    ae_int_t nlcidx;
    double sclfeaserr;
    ae_vector lagbc;
    ae_vector lagbcnz;
    ae_vector laglc;
    ae_vector lagnlc;
    ae_int_t dbgphase0its;
} minnlcreport;
#endif
#if defined(AE_COMPILE_QQPSOLVER) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxouterits;
    ae_bool cgphase;
    ae_bool cnphase;
    ae_int_t cgminits;
    ae_int_t cgmaxits;
    ae_int_t cnmaxupdates;
    ae_int_t sparsesolver;
} qqpsettings;
typedef struct
{
    ae_int_t n;
    ae_int_t akind;
    ae_matrix densea;
    sparsematrix sparsea;
    ae_bool sparseupper;
    double absamax;
    double absasum;
    double absasum2;
    ae_vector b;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector xs;
    ae_vector xf;
    ae_vector gc;
    ae_vector xp;
    ae_vector dc;
    ae_vector dp;
    ae_vector cgc;
    ae_vector cgp;
    sactiveset sas;
    ae_vector activated;
    ae_int_t nfree;
    ae_int_t cnmodelage;
    ae_matrix densez;
    sparsematrix sparsecca;
    ae_vector yidx;
    ae_vector regdiag;
    ae_vector regx0;
    ae_vector tmpcn;
    ae_vector tmpcni;
    ae_vector tmpcnb;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector stpbuf;
    sparsebuffers sbuf;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repncholesky;
    ae_int_t repncupdates;
} qqpbuffers;
#endif
#if defined(AE_COMPILE_QPDENSEAULSOLVER) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    double epsx;
    ae_int_t outerits;
    double rho;
} qpdenseaulsettings;
typedef struct
{
    ae_vector nulc;
    ae_matrix sclsfta;
    ae_vector sclsftb;
    ae_vector sclsfthasbndl;
    ae_vector sclsfthasbndu;
    ae_vector sclsftbndl;
    ae_vector sclsftbndu;
    ae_vector sclsftxc;
    ae_matrix sclsftcleic;
    ae_vector cidx;
    ae_vector cscales;
    ae_matrix exa;
    ae_vector exb;
    ae_vector exxc;
    ae_vector exbndl;
    ae_vector exbndu;
    ae_vector exscale;
    ae_vector exxorigin;
    qqpsettings qqpsettingsuser;
    qqpbuffers qqpbuf;
    ae_vector nulcest;
    ae_vector tmpg;
    ae_vector tmp0;
    ae_matrix tmp2;
    ae_vector modelg;
    ae_vector d;
    ae_vector deltax;
    convexquadraticmodel dummycqm;
    sparsematrix dummysparse;
    ae_matrix qrkkt;
    ae_vector qrrightpart;
    ae_vector qrtau;
    ae_vector qrsv0;
    ae_vector qrsvx1;
    ae_vector nicerr;
    ae_vector nicnact;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repncholesky;
    ae_int_t repnwrkchanges;
    ae_int_t repnwrk0;
    ae_int_t repnwrk1;
    ae_int_t repnwrkf;
    ae_int_t repnmv;
} qpdenseaulbuffers;
#endif
#if defined(AE_COMPILE_MONBI) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    ae_vector xstart;
    ae_int_t frontsize;
    ae_bool polishsolutions;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t ksparse;
    ae_int_t kdense;
    ae_matrix densec;
    sparsematrix sparsec;
    ae_vector cl;
    ae_vector cu;
    ae_int_t nnlc;
    ae_vector nl;
    ae_vector nu;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool xupdated;
    rcommstate rstate;
    ae_bool userrequestedtermination;
    ae_matrix repparetofront;
    ae_int_t repfrontsize;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    minnlcstate nlcsolver;
    minnlcreport nlcrep;
    ae_vector tmpzero;
    ae_vector tmpone;
    ae_vector tmp0;
    ae_matrix olddensec;
    ae_vector olddensect;
    ae_int_t olddensek;
    ae_vector nlcidx;
    ae_vector nlcmul;
    ae_vector nlcadd;
    ae_int_t nlcnlec;
    ae_int_t nlcnlic;
    ae_vector fideal;
    ae_matrix payoff;
    ae_vector beta;
    ae_vector delta;
    ae_vector subproblemstart;
    hqrndstate rs;
    ae_vector bndlx;
    ae_vector bndux;
    ae_matrix olddensecx;
    ae_vector x1;
    ae_vector x2;
    ae_vector fix1;
} nbistate;
#endif
#if defined(AE_COMPILE_MINMO) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double diffstep;
    ae_int_t solvertype;
    double epsx;
    ae_int_t maxits;
    ae_vector s;
    ae_bool xrep;
    ae_vector xstart;
    ae_int_t frontsize;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_int_t msparse;
    ae_int_t mdense;
    ae_matrix densec;
    sparsematrix sparsec;
    ae_vector cl;
    ae_vector cu;
    ae_int_t nnlc;
    ae_vector nl;
    ae_vector nu;
    ae_int_t protocolversion;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
    rcommstate rstate;
    nbistate nbi;
    ae_int_t repfrontsize;
    ae_matrix repparetofront;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_vector dummyr1;
    ae_matrix dummyr2;
    sparsematrix dummysparse;
    ae_vector xbase;
    ae_vector fm2;
    ae_vector fm1;
    ae_vector fp1;
    ae_vector fp2;
} minmostate;
typedef struct
{
    ae_int_t inneriterationscount;
    ae_int_t outeriterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
    double bcerr;
    ae_int_t bcidx;
    double lcerr;
    ae_int_t lcidx;
    double nlcerr;
    ae_int_t nlcidx;
} minmoreport;
#endif
#if defined(AE_COMPILE_MINBC) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t nmain;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    double diffstep;
    ae_vector s;
    ae_int_t prectype;
    ae_vector diagh;
    ae_int_t protocolversion;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool xupdated;
    ae_bool userterminationneeded;
    rcommstate rstate;
    ae_vector xc;
    ae_vector ugc;
    ae_vector cgc;
    ae_vector xn;
    ae_vector ugn;
    ae_vector cgn;
    ae_vector xp;
    double fc;
    double fn;
    double fp;
    ae_vector d;
    double lastscaledgoodstep;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repvaridx;
    ae_int_t repterminationtype;
    ae_vector xstart;
    double fbase;
    double fm2;
    double fm1;
    double fp1;
    double fp2;
    double xm1;
    double xp1;
    double gm1;
    double gp1;
    ae_vector tmpprec;
    ae_vector tmp0;
    ae_int_t nfev;
    ae_int_t mcstage;
    double stp;
    double curstpmax;
    ae_vector work;
    linminstate lstate;
    double trimthreshold;
    ae_int_t nonmonotoniccnt;
    ae_matrix bufyk;
    ae_matrix bufsk;
    ae_vector bufrho;
    ae_vector buftheta;
    ae_int_t bufsize;
    double teststep;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
    ae_vector invs;
} minbcstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t varidx;
    ae_int_t terminationtype;
} minbcreport;
#endif
#if defined(AE_COMPILE_MINNS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    double fc;
    double fn;
    ae_vector xc;
    ae_vector xn;
    ae_vector x0;
    ae_vector gc;
    ae_vector d;
    ae_matrix uh;
    ae_matrix ch;
    ae_matrix rk;
    ae_vector invutc;
    ae_vector tmp0;
    ae_vector tmpidx;
    ae_vector tmpd;
    ae_vector tmpc;
    ae_vector tmplambdas;
    ae_matrix tmpc2;
    ae_vector tmpb;
    snnlssolver nnls;
} minnsqp;
typedef struct
{
    ae_int_t solvertype;
    ae_int_t n;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double diffstep;
    ae_vector s;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_int_t nec;
    ae_int_t nic;
    ae_matrix cleic;
    ae_int_t ng;
    ae_int_t nh;
    ae_int_t protocolversion;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
    rcommstate rstate;
    rcommstate rstateags;
    hqrndstate agsrs;
    double agsradius;
    ae_int_t agssamplesize;
    double agsraddecay;
    double agsalphadecay;
    double agsdecrease;
    double agsinitstp;
    double agsstattold;
    double agsshortstpabs;
    double agsshortstprel;
    double agsshortf;
    ae_int_t agsshortlimit;
    double agsrhononlinear;
    ae_int_t agsminupdate;
    ae_int_t agsmaxraddecays;
    ae_int_t agsmaxbacktrack;
    ae_int_t agsmaxbacktracknonfull;
    double agspenaltylevel;
    double agspenaltyincrease;
    ae_vector xstart;
    ae_vector xc;
    ae_vector xn;
    ae_vector rawg;
    ae_vector meritg;
    double rawf;
    double meritf;
    ae_vector d;
    ae_vector colmax;
    ae_vector diagh;
    ae_vector signmin;
    ae_vector signmax;
    ae_bool userterminationneeded;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_matrix scaledcleic;
    double rholinear;
    ae_matrix samplex;
    ae_matrix samplegm;
    ae_matrix samplegmbc;
    ae_vector samplef;
    minnsqp nsqp;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_matrix tmp2;
    ae_vector tmp3;
    ae_vector xbase;
    ae_vector fbase;
    ae_vector fp;
    ae_vector fm;
    ae_vector xscaled;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repvaridx;
    ae_int_t repfuncidx;
    ae_int_t repterminationtype;
    double replcerr;
    double repnlcerr;
    ae_int_t dbgncholesky;
} minnsstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    double cerr;
    double lcerr;
    double nlcerr;
    ae_int_t terminationtype;
    ae_int_t varidx;
    ae_int_t funcidx;
} minnsreport;
#endif
#if defined(AE_COMPILE_MINDF) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t solvertype;
    ae_int_t n;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector finitebndl;
    ae_vector finitebndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_matrix densea;
    ae_vector al;
    ae_vector au;
    ae_int_t m;
    ae_vector nl;
    ae_vector nu;
    ae_int_t nnlc;
    ae_vector s;
    ae_bool hasx0;
    ae_vector x0;
    double condfxepsf;
    double condfxepsx;
    ae_bool stoponsmallf;
    ae_bool dotimers;
    double smallf;
    ae_int_t rngseed;
    ae_bool xrep;
    ae_vector xf;
    ae_int_t protocolversion;
    ae_bool userterminationneeded;
    ae_int_t gdemoprofile;
    ae_int_t gdemoepochscnt;
    ae_int_t gdemopopsize;
    double gdemorho1;
    double gdemorho2;
    gdemostate gdemosolver;
    ae_bool gdemofixedparams;
    ae_int_t gdemostrategy;
    double gdemocrossoverprob;
    double gdemodifferentialweight;
    ae_int_t ssgdmaxits;
    double ssgdrate0;
    double ssgdrate1;
    double ssgdblur0;
    double ssgdblur1;
    ae_int_t ssgdouterits;
    double ssgdmomentum;
    double ssgdrho;
    ssgdstate ssgdsolver;
    rcommstate rcommv2;
    double repf;
    ae_int_t repterminationtype;
    ae_int_t repiterationscount;
    ae_int_t repnfunc;
    ae_int_t repnrequests;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    double reptimetotal;
    double reptimecallback;
    stimer timertotal;
    stimer timercallback;
    ae_vector unscaled;
    ae_vector combined;
    ae_int_t batchsize;
    ae_vector tmp0;
} mindfstate;
typedef struct
{
    double f;
    ae_int_t iterationscount;
    ae_int_t nfev;
    double bcerr;
    double lcerr;
    double nlcerr;
    ae_int_t terminationtype;
    double timetotal;
    double timesolver;
    double timecallback;
} mindfreport;
#endif
#if defined(AE_COMPILE_NLS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t protocolversion;
    ae_int_t n;
    ae_int_t m;
    ae_int_t problemtype;
    ae_int_t algorithm;
    double diffstep;
    nlpstoppingcriteria criteria;
    ae_bool xrep;
    ae_vector s;
    ae_vector x0;
    double rad0;
    ae_int_t maxfev;
    ae_int_t nnoisyrestarts;
    ae_int_t nnlc;
    ae_int_t cntlc;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector idxraw2red;
    ae_vector redbl;
    ae_vector redbu;
    ae_vector reds;
    ae_vector redx0;
    ae_bool userterminationneeded;
    rcommstate rcommv2;
    dfgmstate dfgmsolver;
    ae_bool dotrace;
    ae_vector xc;
    ae_int_t repnfunc;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_int_t repnrequests;
    ae_vector unscaled;
    ae_vector combined;
    ae_int_t batchsize;
} nlsstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t terminationtype;
    ae_int_t nfunc;
} nlsreport;
#endif
#if defined(AE_COMPILE_LPQPPRESOLVE) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t nvars;
    ae_vector varidx;
    ae_vector b;
    dynamiccrs fullq;
    double cl;
    double cu;
    ae_bool applyorigin;
} dynamiccrsqconstraint;
typedef struct
{
    ae_obj_array constraints;
} dynamiccrsqconstraints;
typedef struct
{
    niset setn;
    niset setn2;
    niset setn3;
    niset setm;
    ae_vector sparseidx;
    ae_vector sparsevals;
    ae_vector fixedlocal;
    ae_vector tmp0;
    dynamiccrsqconstraint qc;
} presolvebuffers;
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t mqc;
    ae_int_t mcc;
    ae_int_t ntrf;
    ae_vector trftype;
    ae_vector idata;
    ae_vector rdata;
    ae_vector idataridx;
    ae_vector rdataridx;
    ae_int_t sourceidx;
    ae_int_t isrc;
    ae_int_t rsrc;
    ae_vector sparseidx0;
    ae_vector sparseidx1;
    ae_vector sparseidx2;
    ae_vector sparseidx3;
    ae_vector sparseval0;
    ae_vector sparseval1;
    ae_vector sparseval2;
    ae_vector sparseval3;
} presolverstack;
typedef struct
{
    ae_vector isdroppedcol;
    ae_vector isdroppedlc;
    ae_vector isdroppedqc;
    ae_vector isdroppedcc;
    ae_vector cntdownwardaxial;
    ae_vector cntupwardaxial;
    ae_vector cntaxial;
    ae_vector cntradial;
    ae_vector nlcpervar;
    kniset varbyqc;
    kniset varbycc;
} presolvervcstats;
typedef struct
{
    double eps;
    ae_int_t newn;
    ae_int_t oldn;
    ae_int_t oldmqc;
    ae_int_t oldmcc;
    ae_int_t newm;
    ae_int_t oldm;
    ae_int_t newmqc;
    ae_int_t newmcc;
    ae_vector rawc;
    ae_vector rawbndl;
    ae_vector rawbndu;
    ae_bool hash;
    sparsematrix rawh;
    sparsematrix rawa;
    xquadraticconstraints rawxqc;
    xconicconstraints rawxcc;
    ae_int_t problemstatus;
    ae_vector lagrangefromresidual;
    ae_vector c;
    ae_vector bndl;
    ae_vector bndu;
    sparsematrix sparseh;
    sparsematrix sparsea;
    ae_vector al;
    ae_vector au;
    xquadraticconstraints xqc;
    xconicconstraints xcc;
    ae_vector packxperm;
    ae_vector packyperm;
    ae_vector packqcperm;
    ae_vector packccperm;
    ae_vector packstatperm;
    ae_vector unpackxperm;
    ae_vector unpackyperm;
    ae_vector unpackqcperm;
    ae_vector unpackccperm;
    ae_vector unpackstatperm;
    presolverstack trfstack;
    ae_vector s1;
    ae_vector bc1;
    ae_vector x1;
    ae_vector y1;
    ae_vector yqc;
    ae_vector d;
    ae_vector tmpi;
    presolvebuffers buf;
} presolveinfo;
#endif
#if defined(AE_COMPILE_MINQP) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    qqpsettings qqpsettingsuser;
    qpdenseaulsettings qpdenseaulsettingsuser;
    double veps;
    ae_int_t vmemlen;
    ae_int_t vmaxoffdiag;
    ae_int_t gmaxits;
    ae_bool dbgskipconstraintnormalization;
    ae_bool dbgdopresolve;
    ae_int_t algokind;
    ae_int_t akind;
    convexquadraticmodel a;
    ae_bool cqmready;
    sparsematrix sparsea;
    ae_bool sparseaupper;
    double absamax;
    double absasum;
    double absasum2;
    ae_vector b;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t stype;
    ae_vector s;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector xorigin;
    ae_vector startx;
    ae_bool havex;
    xlinearconstraints xlc;
    xquadraticconstraints xqc;
    xconicconstraints xcc;
    ae_vector xs;
    double repf;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repncholesky;
    ae_int_t repnmv;
    ae_int_t repterminationtype;
    ae_vector replagbc;
    ae_vector replaglc;
    ae_vector replagqc;
    ae_int_t repn;
    ae_int_t repm;
    ae_int_t repmqc;
    ae_int_t repmcc;
    ae_vector effectives;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmpi;
    ae_vector elaglc;
    ae_vector dummyr;
    ae_matrix dummyr2;
    sparsematrix dummysparse;
    ae_matrix tmpr2;
    presolveinfo presolver;
    ae_nxpool n1realpool;
    ae_nxpool n1intpool;
    qqpbuffers qqpbuf;
    qpdenseaulbuffers qpdenseaulbuf;
    vipmstate vsolver;
    ipm2state ipm2;
    ecqpstate ecqp;
    gqpipmstate genipm;
    sparsematrix modsparsea;
    xconicconstraints modxcc;
} minqpstate;
typedef struct
{
    ae_int_t inneriterationscount;
    ae_int_t outeriterationscount;
    ae_int_t nmv;
    ae_int_t ncholesky;
    ae_int_t terminationtype;
    double f;
    ae_vector lagbc;
    ae_vector laglc;
    ae_vector lagqc;
} minqpreport;
#endif
#if defined(AE_COMPILE_MINLM) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double stpmax;
    ae_int_t modelage;
    ae_int_t maxmodelage;
    double epsx;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_bool needf;
    ae_bool needfi;
    double fbase;
    ae_vector modeldiag;
    ae_vector xbase;
    ae_vector fibase;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector s;
    rcommstate rstate;
    ae_vector xdir;
    ae_vector choleskybuf;
    ae_vector tmp0;
    ae_vector tmpct;
    double actualdecrease;
    double predicteddecrease;
    minqpstate qpstate;
    minqpreport qprep;
    sparsematrix tmpsp;
} minlmstepfinder;
typedef struct
{
    ae_int_t protocolversion;
    ae_int_t n;
    ae_int_t m;
    double diffstep;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    ae_int_t maxmodelage;
    ae_bool makeadditers;
    ae_int_t nonmonotoniccnt;
    ae_int_t formulatype;
    ae_bool userterminationneeded;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_matrix h;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool needfgh;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
    ae_int_t algomode;
    ae_vector xbase;
    double fbase;
    ae_vector fibase;
    ae_vector gbase;
    ae_matrix quadraticmodel;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector s;
    ae_matrix cleic;
    ae_int_t nec;
    ae_int_t nic;
    double lambdav;
    double nu;
    ae_int_t modelage;
    ae_int_t nonmonotonicpos;
    ae_vector nonmonotonicbuf;
    ae_vector xnew;
    ae_vector xdir;
    ae_vector deltax;
    ae_vector deltaf;
    ae_bool deltaxready;
    ae_bool deltafready;
    smoothnessmonitor smonitor;
    double teststep;
    ae_vector lastscaleused;
    ae_int_t fstagnationcnt;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_int_t repnfunc;
    ae_int_t repnjac;
    ae_int_t repngrad;
    ae_int_t repnhess;
    ae_int_t repncholesky;
    ae_bool dotimers;
    ae_int_t tstart;
    ae_int_t tqp;
    rcommstate rcommv2;
    ae_vector choleskybuf;
    ae_vector tmp0;
    double actualdecrease;
    double predicteddecrease;
    double xm1;
    double xp1;
    ae_vector fm1;
    ae_vector fp1;
    ae_vector fc1;
    ae_vector gm1;
    ae_vector gp1;
    ae_vector gc1;
    minlbfgsstate internalstate;
    minlbfgsreport internalrep;
    minqpstate qpstate;
    minqpreport qprep;
    minlmstepfinder finderstate;
} minlmstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t terminationtype;
    double f;
    ae_int_t nfunc;
    ae_int_t njac;
    ae_int_t ngrad;
    ae_int_t nhess;
    ae_int_t ncholesky;
} minlmreport;
#endif
#if defined(AE_COMPILE_REVISEDDUALSIMPLEX) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    double pivottol;
    double perturbmag;
    ae_int_t maxtrfage;
    ae_int_t trftype;
    ae_int_t ratiotest;
    ae_int_t pricing;
    ae_int_t shifting;
    double xtolabs;
    double xtolrelabs;
    double dtolabs;
} dualsimplexsettings;
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_vector idx;
    ae_vector vals;
    ae_vector dense;
} dssvector;
typedef struct
{
    ae_int_t ns;
    ae_int_t m;
    ae_vector idx;
    ae_vector nidx;
    ae_vector isbasic;
    ae_int_t trftype;
    ae_bool isvalidtrf;
    ae_int_t trfage;
    ae_matrix denselu;
    sparsematrix sparsel;
    sparsematrix sparseu;
    sparsematrix sparseut;
    ae_vector rowpermbwd;
    ae_vector colpermbwd;
    ae_vector densepfieta;
    ae_vector densemu;
    ae_vector rk;
    ae_vector dk;
    ae_vector dseweights;
    ae_bool dsevalid;
    double eminu;
    ae_int_t statfact;
    ae_int_t statupdt;
    double statoffdiag;
    ae_vector wtmp0;
    ae_vector wtmp1;
    ae_vector wtmp2;
    ae_vector nrs;
    ae_vector tcinvidx;
    ae_matrix denselu2;
    ae_vector densep2;
    ae_vector densep2c;
    sparsematrix sparselu1;
    sparsematrix sparselu2;
    sluv2buffer lubuf2;
    ae_vector tmpi;
    ae_vector utmp0;
    ae_vector utmpi;
    sparsematrix sparseludbg;
} dualsimplexbasis;
typedef struct
{
    ae_int_t ns;
    ae_int_t m;
    ae_vector rawc;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector bndt;
    ae_vector xa;
    ae_vector d;
    ae_int_t state;
    ae_vector xb;
    ae_vector bndlb;
    ae_vector bndub;
    ae_vector bndtb;
    ae_vector bndtollb;
    ae_vector bndtolub;
    ae_vector effc;
} dualsimplexsubproblem;
typedef struct
{
    ae_vector rowscales;
    ae_vector rawbndl;
    ae_vector rawbndu;
    ae_int_t ns;
    ae_int_t m;
    sparsematrix a;
    sparsematrix at;
    dualsimplexbasis basis;
    dualsimplexsubproblem primary;
    dualsimplexsubproblem phase1;
    dualsimplexsubproblem phase3;
    ae_vector repx;
    ae_vector replagbc;
    ae_vector replaglc;
    ae_vector repstats;
    ae_int_t repterminationtype;
    ae_int_t repiterationscount;
    ae_int_t repiterationscount1;
    ae_int_t repiterationscount2;
    ae_int_t repiterationscount3;
    ae_int_t repphase1time;
    ae_int_t repphase2time;
    ae_int_t repphase3time;
    ae_int_t repdualpricingtime;
    ae_int_t repdualbtrantime;
    ae_int_t repdualpivotrowtime;
    ae_int_t repdualratiotesttime;
    ae_int_t repdualftrantime;
    ae_int_t repdualupdatesteptime;
    double repfillpivotrow;
    ae_int_t repfillpivotrowcnt;
    double repfillrhor;
    ae_int_t repfillrhorcnt;
    double repfilldensemu;
    ae_int_t repfilldensemucnt;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_bool dotimers;
    ae_vector btrantmp0;
    ae_vector btrantmp1;
    ae_vector btrantmp2;
    ae_vector ftrantmp0;
    ae_vector ftrantmp1;
    ae_vector possibleflips;
    ae_int_t possibleflipscnt;
    ae_vector dfctmp0;
    ae_vector dfctmp1;
    ae_vector dfctmp2;
    ae_vector ustmpi;
    apbuffers xydsbuf;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    dssvector alphar;
    dssvector rhor;
    ae_vector tau;
    ae_vector alphaq;
    ae_vector alphaqim;
    ae_vector eligiblealphar;
    ae_vector harrisset;
} dualsimplexstate;
#endif
#if defined(AE_COMPILE_LPSOLVERS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t algokind;
    double ipmlambda;
    ae_vector s;
    ae_vector c;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t m;
    sparsematrix a;
    ae_vector al;
    ae_vector au;
    xquadraticconstraints dummyxqc;
    xconicconstraints dummyxcc;
    ae_vector xs;
    ae_vector lagbc;
    ae_vector laglc;
    ae_vector cs;
    double repf;
    double repprimalerror;
    double repdualerror;
    double repslackerror;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_int_t repn;
    ae_int_t repm;
    double dsseps;
    double ipmeps;
    dualsimplexstate dss;
    vipmstate ipm;
    ipm2state ipm2;
    ae_vector adddtmpi;
    ae_vector adddtmpr;
    ae_vector tmpax;
    ae_vector tmpg;
    presolveinfo presolver;
    ae_vector zeroorigin;
    ae_vector units;
    sparsematrix ipmquadratic;
    ae_vector dummylagqc;
} minlpstate;
typedef struct
{
    double f;
    ae_vector lagbc;
    ae_vector laglc;
    ae_vector y;
    ae_vector stats;
    double primalerror;
    double dualerror;
    double slackerror;
    ae_int_t iterationscount;
    ae_int_t terminationtype;
} minlpreport;
#endif
#if defined(AE_COMPILE_MINCG) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    double stpmax;
    double suggestedstep;
    ae_bool xrep;
    ae_bool drep;
    ae_int_t cgtype;
    ae_int_t prectype;
    ae_vector diagh;
    ae_vector diaghl2;
    ae_matrix vcorr;
    ae_int_t vcnt;
    ae_vector s;
    double diffstep;
    ae_int_t nfev;
    ae_int_t mcstage;
    ae_int_t k;
    ae_vector xk;
    ae_vector dk;
    ae_vector xn;
    ae_vector dn;
    ae_vector d;
    double fold;
    double stp;
    double curstpmax;
    ae_vector yk;
    double lastgoodstep;
    double lastscaledstep;
    ae_int_t mcinfo;
    ae_bool innerresetneeded;
    ae_bool terminationneeded;
    double trimthreshold;
    ae_vector xbase;
    ae_int_t rstimer;
    ae_int_t protocolversion;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool xupdated;
    ae_bool algpowerup;
    ae_bool lsstart;
    ae_bool lsend;
    ae_bool userterminationneeded;
    rcommstate rstate;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    ae_int_t debugrestartscount;
    linminstate lstate;
    double fbase;
    double fm2;
    double fm1;
    double fp1;
    double fp2;
    double betahs;
    double betady;
    ae_vector work0;
    ae_vector work1;
    ae_vector invs;
    double teststep;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
} mincgstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
} mincgreport;
#endif
#if defined(AE_COMPILE_MINCOMP) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    ae_int_t cgtype;
    ae_int_t k;
    ae_int_t nfev;
    ae_int_t mcstage;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t curalgo;
    ae_int_t acount;
    double mu;
    double finit;
    double dginit;
    ae_vector ak;
    ae_vector xk;
    ae_vector dk;
    ae_vector an;
    ae_vector xn;
    ae_vector dn;
    ae_vector d;
    double fold;
    double stp;
    ae_vector work;
    ae_vector yk;
    ae_vector gc;
    double laststep;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needfg;
    ae_bool xupdated;
    rcommstate rstate;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    ae_int_t debugrestartscount;
    linminstate lstate;
    double betahs;
    double betady;
} minasastate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
    ae_int_t activeconstraints;
} minasareport;
#endif

}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS C++ INTERFACE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib
{

#if defined(AE_COMPILE_OPTGUARDAPI) || !defined(AE_PARTIAL_BUILD)
class _optguardreport_owner;
class optguardreport;
class _optguardnonc0report_owner;
class optguardnonc0report;
class _optguardnonc1test0report_owner;
class optguardnonc1test0report;
class _optguardnonc1test1report_owner;
class optguardnonc1test1report;


/*************************************************************************
This structure is used to store  OptGuard  report,  i.e.  report  on   the
properties of the nonlinear function being optimized with ALGLIB.

After you tell your optimizer to activate OptGuard  this technology starts
to silently monitor function values and gradients/Jacobians  being  passed
all around during your optimization session. Depending on specific set  of
checks enabled OptGuard may perform additional function evaluations  (say,
about 3*N evaluations if you want to check analytic gradient for errors).

Upon discovering that something strange happens  (function  values  and/or
gradient components change too sharply and/or unexpectedly) OptGuard  sets
one of the "suspicion  flags" (without interrupting optimization session).
After optimization is done, you can examine OptGuard report.

Following report fields can be set:
* nonc0suspected
* nonc1suspected
* badgradsuspected


=== WHAT CAN BE DETECTED WITH OptGuard INTEGRITY CHECKER =================

Following  types  of  errors  in your target function (constraints) can be
caught:
a) discontinuous functions ("non-C0" part of the report)
b) functions with discontinuous derivative ("non-C1" part of the report)
c) errors in the analytic gradient provided by user

These types of errors result in optimizer  stopping  well  before reaching
solution (most often - right after encountering discontinuity).

Type A errors are usually  coding  errors  during  implementation  of  the
target function. Most "normal" problems involve continuous functions,  and
anyway you can't reliably optimize discontinuous function.

Type B errors are either coding errors or (in case code itself is correct)
evidence of the fact  that  your  problem  is  an  "incorrect"  one.  Most
optimizers (except for ones provided by MINNS subpackage) do  not  support
nonsmooth problems.

Type C errors are coding errors which often prevent optimizer from  making
even one step  or result in optimizing stopping  too  early,  as  soon  as
actual descent direction becomes too different from one suggested by user-
supplied gradient.


=== WHAT IS REPORTED =====================================================

Following set of report fields deals with discontinuous  target functions,
ones not belonging to C0 continuity class:

* nonc0suspected - is a flag which is set upon discovering some indication
  of the discontinuity. If this flag is false, the rest of "non-C0" fields
  should be ignored
* nonc0fidx - is an index of the function (0 for  target  function,  1  or
  higher for nonlinear constraints) which is suspected of being "non-C0"
* nonc0lipshitzc - a Lipchitz constant for a function which was  suspected
  of being non-continuous.
* nonc0test0positive -  set  to  indicate  specific  test  which  detected
  continuity violation (test #0)

Following set of report fields deals with discontinuous gradient/Jacobian,
i.e. with functions violating C1 continuity:

* nonc1suspected - is a flag which is set upon discovering some indication
  of the discontinuity. If this flag is false, the rest of "non-C1" fields
  should be ignored
* nonc1fidx - is an index of the function (0 for  target  function,  1  or
  higher for nonlinear constraints) which is suspected of being "non-C1"
* nonc1lipshitzc - a Lipchitz constant for a function gradient  which  was
  suspected of being non-smooth.
* nonc1test0positive -  set  to  indicate  specific  test  which  detected
  continuity violation (test #0)
* nonc1test1positive -  set  to  indicate  specific  test  which  detected
  continuity violation (test #1)

Following set of report fields deals with errors in the gradient:
* badgradsuspected - is a flad which is set upon discovering an  error  in
  the analytic gradient supplied by user
* badgradfidx - index  of   the  function  with bad gradient (0 for target
  function, 1 or higher for nonlinear constraints)
* badgradvidx - index of the variable
* badgradxbase - location where Jacobian is tested
* following  matrices  store  user-supplied  Jacobian  and  its  numerical
  differentiation version (which is assumed to be  free  from  the  coding
  errors), both of them computed near the initial point:
  * badgraduser, an array[K,N], analytic Jacobian supplied by user
  * badgradnum,  an array[K,N], numeric  Jacobian computed by ALGLIB
  Here K is a total number of  nonlinear  functions  (target  +  nonlinear
  constraints), N is a variable number.
  The  element  of  badgraduser[] with index [badgradfidx,badgradvidx]  is
  assumed to be wrong.

More detailed error log can  be  obtained  from  optimizer  by  explicitly
requesting reports for tests C0.0, C1.0, C1.1.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
class _optguardreport_owner
{
public:
    _optguardreport_owner();
    _optguardreport_owner(alglib_impl::optguardreport *attach_to);
    _optguardreport_owner(const _optguardreport_owner &rhs);
    _optguardreport_owner& operator=(const _optguardreport_owner &rhs);
    virtual ~_optguardreport_owner();
    alglib_impl::optguardreport* c_ptr();
    const alglib_impl::optguardreport* c_ptr() const;
protected:
    alglib_impl::optguardreport *p_struct;
    bool is_attached;
};
class optguardreport : public _optguardreport_owner
{
public:
    optguardreport();
    optguardreport(alglib_impl::optguardreport *attach_to);
    optguardreport(const optguardreport &rhs);
    optguardreport& operator=(const optguardreport &rhs);
    virtual ~optguardreport();
    ae_bool &nonc0suspected;
    ae_bool &nonc0test0positive;
    ae_int_t &nonc0fidx;
    double &nonc0lipschitzc;
    ae_bool &nonc1suspected;
    ae_bool &nonc1test0positive;
    ae_bool &nonc1test1positive;
    ae_int_t &nonc1fidx;
    double &nonc1lipschitzc;
    ae_bool &badgradsuspected;
    ae_int_t &badgradfidx;
    ae_int_t &badgradvidx;
    real_1d_array badgradxbase;
    real_2d_array badgraduser;
    real_2d_array badgradnum;


};


/*************************************************************************
This  structure  is  used  for  detailed   reporting  about  suspected  C0
continuity violation.

=== WHAT IS TESTED =======================================================

C0 test  studies  function  values (not gradient!)  obtained  during  line
searches and monitors estimate of the Lipschitz  constant.  Sudden  spikes
usually indicate that discontinuity was detected.


=== WHAT IS REPORTED =====================================================

Actually, report retrieval function returns TWO report structures:

* one for most suspicious point found so far (one with highest  change  in
  the function value), so called "strongest" report
* another one for most detailed line search (more function  evaluations  =
  easier to understand what's going on) which triggered  test #0 criteria,
  so called "longest" report

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
* inneriter, outeriter - inner and outer iteration indexes (can be -1 if no
  iteration information was specified)

You can plot function values stored in stp[]  and  f[]  arrays  and  study
behavior of your function by your own eyes, just  to  be  sure  that  test
correctly reported C1 violation.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
class _optguardnonc0report_owner
{
public:
    _optguardnonc0report_owner();
    _optguardnonc0report_owner(alglib_impl::optguardnonc0report *attach_to);
    _optguardnonc0report_owner(const _optguardnonc0report_owner &rhs);
    _optguardnonc0report_owner& operator=(const _optguardnonc0report_owner &rhs);
    virtual ~_optguardnonc0report_owner();
    alglib_impl::optguardnonc0report* c_ptr();
    const alglib_impl::optguardnonc0report* c_ptr() const;
protected:
    alglib_impl::optguardnonc0report *p_struct;
    bool is_attached;
};
class optguardnonc0report : public _optguardnonc0report_owner
{
public:
    optguardnonc0report();
    optguardnonc0report(alglib_impl::optguardnonc0report *attach_to);
    optguardnonc0report(const optguardnonc0report &rhs);
    optguardnonc0report& operator=(const optguardnonc0report &rhs);
    virtual ~optguardnonc0report();
    ae_bool &positive;
    ae_int_t &fidx;
    real_1d_array x0;
    real_1d_array d;
    ae_int_t &n;
    real_1d_array stp;
    real_1d_array f;
    ae_int_t &cnt;
    ae_int_t &stpidxa;
    ae_int_t &stpidxb;
    ae_int_t &inneriter;
    ae_int_t &outeriter;


};


/*************************************************************************
This  structure  is  used  for  detailed   reporting  about  suspected  C1
continuity violation as flagged by C1 test #0 (OptGuard  has several tests
for C1 continuity, this report is used by #0).

=== WHAT IS TESTED =======================================================

C1 test #0 studies function values (not gradient!)  obtained  during  line
searches and monitors behavior of directional  derivative  estimate.  This
test is less powerful than test #1, but it does  not  depend  on  gradient
values  and  thus  it  is  more  robust  against  artifacts  introduced by
numerical differentiation.


=== WHAT IS REPORTED =====================================================

Actually, report retrieval function returns TWO report structures:

* one for most suspicious point found so far (one with highest  change  in
  the directional derivative), so called "strongest" report
* another one for most detailed line search (more function  evaluations  =
  easier to understand what's going on) which triggered  test #0 criteria,
  so called "longest" report

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
* inneriter, outeriter - inner and outer iteration indexes (can be -1 if no
  iteration information was specified)

You can plot function values stored in stp[]  and  f[]  arrays  and  study
behavior of your function by your own eyes, just  to  be  sure  that  test
correctly reported C1 violation.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
class _optguardnonc1test0report_owner
{
public:
    _optguardnonc1test0report_owner();
    _optguardnonc1test0report_owner(alglib_impl::optguardnonc1test0report *attach_to);
    _optguardnonc1test0report_owner(const _optguardnonc1test0report_owner &rhs);
    _optguardnonc1test0report_owner& operator=(const _optguardnonc1test0report_owner &rhs);
    virtual ~_optguardnonc1test0report_owner();
    alglib_impl::optguardnonc1test0report* c_ptr();
    const alglib_impl::optguardnonc1test0report* c_ptr() const;
protected:
    alglib_impl::optguardnonc1test0report *p_struct;
    bool is_attached;
};
class optguardnonc1test0report : public _optguardnonc1test0report_owner
{
public:
    optguardnonc1test0report();
    optguardnonc1test0report(alglib_impl::optguardnonc1test0report *attach_to);
    optguardnonc1test0report(const optguardnonc1test0report &rhs);
    optguardnonc1test0report& operator=(const optguardnonc1test0report &rhs);
    virtual ~optguardnonc1test0report();
    ae_bool &positive;
    ae_int_t &fidx;
    real_1d_array x0;
    real_1d_array d;
    ae_int_t &n;
    real_1d_array stp;
    real_1d_array f;
    ae_int_t &cnt;
    ae_int_t &stpidxa;
    ae_int_t &stpidxb;
    ae_int_t &inneriter;
    ae_int_t &outeriter;


};


/*************************************************************************
This  structure  is  used  for  detailed   reporting  about  suspected  C1
continuity violation as flagged by C1 test #1 (OptGuard  has several tests
for C1 continuity, this report is used by #1).

=== WHAT IS TESTED =======================================================

C1 test #1 studies individual  components  of  the  gradient  as  recorded
during line searches. Upon discovering discontinuity in the gradient  this
test records specific component which was suspected (or  one  with  highest
indication of discontinuity if multiple components are suspected).

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.


=== WHAT IS REPORTED =====================================================

Actually, report retrieval function returns TWO report structures:

* one for most suspicious point found so far (one with highest  change  in
  the directional derivative), so called "strongest" report
* another one for most detailed line search (more function  evaluations  =
  easier to understand what's going on) which triggered  test #1 criteria,
  so called "longest" report

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
* inneriter, outeriter - inner and outer iteration indexes (can be -1 if  no
  iteration information was specified)

You can plot function values stored in stp[]  and  g[]  arrays  and  study
behavior of your function by your own eyes, just  to  be  sure  that  test
correctly reported C1 violation.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
class _optguardnonc1test1report_owner
{
public:
    _optguardnonc1test1report_owner();
    _optguardnonc1test1report_owner(alglib_impl::optguardnonc1test1report *attach_to);
    _optguardnonc1test1report_owner(const _optguardnonc1test1report_owner &rhs);
    _optguardnonc1test1report_owner& operator=(const _optguardnonc1test1report_owner &rhs);
    virtual ~_optguardnonc1test1report_owner();
    alglib_impl::optguardnonc1test1report* c_ptr();
    const alglib_impl::optguardnonc1test1report* c_ptr() const;
protected:
    alglib_impl::optguardnonc1test1report *p_struct;
    bool is_attached;
};
class optguardnonc1test1report : public _optguardnonc1test1report_owner
{
public:
    optguardnonc1test1report();
    optguardnonc1test1report(alglib_impl::optguardnonc1test1report *attach_to);
    optguardnonc1test1report(const optguardnonc1test1report &rhs);
    optguardnonc1test1report& operator=(const optguardnonc1test1report &rhs);
    virtual ~optguardnonc1test1report();
    ae_bool &positive;
    ae_int_t &fidx;
    ae_int_t &vidx;
    real_1d_array x0;
    real_1d_array d;
    ae_int_t &n;
    real_1d_array stp;
    real_1d_array g;
    ae_int_t &cnt;
    ae_int_t &stpidxa;
    ae_int_t &stpidxb;
    ae_int_t &inneriter;
    ae_int_t &outeriter;


};
#endif

#if defined(AE_COMPILE_OPTS) || !defined(AE_PARTIAL_BUILD)
class _lptestproblem_owner;
class lptestproblem;


/*************************************************************************
This is a test problem class  intended  for  internal  performance  tests.
Never use it directly in your projects.
*************************************************************************/
class _lptestproblem_owner
{
public:
    _lptestproblem_owner();
    _lptestproblem_owner(alglib_impl::lptestproblem *attach_to);
    _lptestproblem_owner(const _lptestproblem_owner &rhs);
    _lptestproblem_owner& operator=(const _lptestproblem_owner &rhs);
    virtual ~_lptestproblem_owner();
    alglib_impl::lptestproblem* c_ptr();
    const alglib_impl::lptestproblem* c_ptr() const;
protected:
    alglib_impl::lptestproblem *p_struct;
    bool is_attached;
};
class lptestproblem : public _lptestproblem_owner
{
public:
    lptestproblem();
    lptestproblem(alglib_impl::lptestproblem *attach_to);
    lptestproblem(const lptestproblem &rhs);
    lptestproblem& operator=(const lptestproblem &rhs);
    virtual ~lptestproblem();


};
#endif

#if defined(AE_COMPILE_OPTSERV) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_CQMODELS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_SNNLS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_SACTIVESETS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINBLEIC) || !defined(AE_PARTIAL_BUILD)
class _minbleicstate_owner;
class minbleicstate;
class _minbleicreport_owner;
class minbleicreport;


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinBLEIC subpackage to work with this
object
*************************************************************************/
class _minbleicstate_owner
{
public:
    _minbleicstate_owner();
    _minbleicstate_owner(alglib_impl::minbleicstate *attach_to);
    _minbleicstate_owner(const _minbleicstate_owner &rhs);
    _minbleicstate_owner& operator=(const _minbleicstate_owner &rhs);
    virtual ~_minbleicstate_owner();
    alglib_impl::minbleicstate* c_ptr();
    const alglib_impl::minbleicstate* c_ptr() const;
protected:
    alglib_impl::minbleicstate *p_struct;
    bool is_attached;
};
class minbleicstate : public _minbleicstate_owner
{
public:
    minbleicstate();
    minbleicstate(alglib_impl::minbleicstate *attach_to);
    minbleicstate(const minbleicstate &rhs);
    minbleicstate& operator=(const minbleicstate &rhs);
    virtual ~minbleicstate();
    ae_bool &needf;
    ae_bool &needfg;
    ae_bool &xupdated;
    double &f;
    real_1d_array g;
    real_1d_array x;


};


/*************************************************************************
This structure stores optimization report:
* IterationsCount           number of iterations
* NFEV                      number of gradient evaluations
* TerminationType           termination type (see below)

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
  -3    inconsistent constraints. Feasible point is
        either nonexistent or too hard to find. Try to
        restart optimizer with better initial approximation
   1    relative function improvement is no more than EpsF.
   2    relative step is no more than EpsX.
   4    gradient norm is no more than EpsG
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    terminated by user who called minbleicrequesttermination(). X contains
        point which was "current accepted" when  termination  request  was
        submitted.

ADDITIONAL FIELDS

There are additional fields which can be used for debugging:
* DebugEqErr                error in the equality constraints (2-norm)
* DebugFS                   f, calculated at projection of initial point
                            to the feasible set
* DebugFF                   f, calculated at the final point
* DebugDX                   |X_start-X_final|
*************************************************************************/
class _minbleicreport_owner
{
public:
    _minbleicreport_owner();
    _minbleicreport_owner(alglib_impl::minbleicreport *attach_to);
    _minbleicreport_owner(const _minbleicreport_owner &rhs);
    _minbleicreport_owner& operator=(const _minbleicreport_owner &rhs);
    virtual ~_minbleicreport_owner();
    alglib_impl::minbleicreport* c_ptr();
    const alglib_impl::minbleicreport* c_ptr() const;
protected:
    alglib_impl::minbleicreport *p_struct;
    bool is_attached;
};
class minbleicreport : public _minbleicreport_owner
{
public:
    minbleicreport();
    minbleicreport(alglib_impl::minbleicreport *attach_to);
    minbleicreport(const minbleicreport &rhs);
    minbleicreport& operator=(const minbleicreport &rhs);
    virtual ~minbleicreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &varidx;
    ae_int_t &terminationtype;
    double &debugeqerr;
    double &debugfs;
    double &debugff;
    double &debugdx;
    ae_int_t &debugfeasqpits;
    ae_int_t &debugfeasgpaits;
    ae_int_t &inneriterationscount;
    ae_int_t &outeriterationscount;


};
#endif

#if defined(AE_COMPILE_LPQPSERV) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_GIPM) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_GQPIPM) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINLBFGS) || !defined(AE_PARTIAL_BUILD)
class _minlbfgsstate_owner;
class minlbfgsstate;
class _minlbfgsreport_owner;
class minlbfgsreport;


/*************************************************************************

*************************************************************************/
class _minlbfgsstate_owner
{
public:
    _minlbfgsstate_owner();
    _minlbfgsstate_owner(alglib_impl::minlbfgsstate *attach_to);
    _minlbfgsstate_owner(const _minlbfgsstate_owner &rhs);
    _minlbfgsstate_owner& operator=(const _minlbfgsstate_owner &rhs);
    virtual ~_minlbfgsstate_owner();
    alglib_impl::minlbfgsstate* c_ptr();
    const alglib_impl::minlbfgsstate* c_ptr() const;
protected:
    alglib_impl::minlbfgsstate *p_struct;
    bool is_attached;
};
class minlbfgsstate : public _minlbfgsstate_owner
{
public:
    minlbfgsstate();
    minlbfgsstate(alglib_impl::minlbfgsstate *attach_to);
    minlbfgsstate(const minlbfgsstate &rhs);
    minlbfgsstate& operator=(const minlbfgsstate &rhs);
    virtual ~minlbfgsstate();


};


/*************************************************************************
This structure stores optimization report:
* IterationsCount           total number of inner iterations
* NFEV                      number of gradient evaluations
* TerminationType           termination type (see below)

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
   1    relative function improvement is no more than EpsF.
   2    relative step is no more than EpsX.
   4    gradient norm is no more than EpsG
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    terminated    by  user  who  called  minlbfgsrequesttermination().
        X contains point which was   "current accepted"  when  termination
        request was submitted.

Other fields of this structure are not documented and should not be used!
*************************************************************************/
class _minlbfgsreport_owner
{
public:
    _minlbfgsreport_owner();
    _minlbfgsreport_owner(alglib_impl::minlbfgsreport *attach_to);
    _minlbfgsreport_owner(const _minlbfgsreport_owner &rhs);
    _minlbfgsreport_owner& operator=(const _minlbfgsreport_owner &rhs);
    virtual ~_minlbfgsreport_owner();
    alglib_impl::minlbfgsreport* c_ptr();
    const alglib_impl::minlbfgsreport* c_ptr() const;
protected:
    alglib_impl::minlbfgsreport *p_struct;
    bool is_attached;
};
class minlbfgsreport : public _minlbfgsreport_owner
{
public:
    minlbfgsreport();
    minlbfgsreport(alglib_impl::minlbfgsreport *attach_to);
    minlbfgsreport(const minlbfgsreport &rhs);
    minlbfgsreport& operator=(const minlbfgsreport &rhs);
    virtual ~minlbfgsreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &terminationtype;


};
#endif

#if defined(AE_COMPILE_VIPMSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_IPM2SOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLCFSQP) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_DIFFEVO) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_SSGD) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_ECQPSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_GIPM3) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLCGIPM3) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLCSQP) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLCAUL) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_DFGENMOD) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_DYNCRS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLPPRESOLVE) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINNLC) || !defined(AE_PARTIAL_BUILD)
class _minnlcstate_owner;
class minnlcstate;
class _minnlcreport_owner;
class minnlcreport;


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinNLC subpackage to work  with  this
object
*************************************************************************/
class _minnlcstate_owner
{
public:
    _minnlcstate_owner();
    _minnlcstate_owner(alglib_impl::minnlcstate *attach_to);
    _minnlcstate_owner(const _minnlcstate_owner &rhs);
    _minnlcstate_owner& operator=(const _minnlcstate_owner &rhs);
    virtual ~_minnlcstate_owner();
    alglib_impl::minnlcstate* c_ptr();
    const alglib_impl::minnlcstate* c_ptr() const;
protected:
    alglib_impl::minnlcstate *p_struct;
    bool is_attached;
};
class minnlcstate : public _minnlcstate_owner
{
public:
    minnlcstate();
    minnlcstate(alglib_impl::minnlcstate *attach_to);
    minnlcstate(const minnlcstate &rhs);
    minnlcstate& operator=(const minnlcstate &rhs);
    virtual ~minnlcstate();


};


/*************************************************************************
These fields store optimization report:
* f                         objective value at the solution
* iterationscount           total number of inner iterations
* nfev                      number of gradient evaluations
* terminationtype           termination type (see below)

Scaled constraint violations are reported:
* bcerr                     maximum violation of the box constraints
* bcidx                     index of the most violated box  constraint (or
                            -1, if all box constraints  are  satisfied  or
                            there is no box constraint)
* lcerr                     maximum violation of the  linear  constraints,
                            computed as maximum  scaled  distance  between
                            final point and constraint boundary.
* lcidx                     index of the most violated  linear  constraint
                            (or -1, if all constraints  are  satisfied  or
                            there is no general linear constraints)
* nlcerr                    maximum violation of the nonlinear constraints
* nlcidx                    index of the most violated nonlinear constraint
                            (or -1, if all constraints  are  satisfied  or
                            there is no nonlinear constraints)
* sclfeaserr                maximum  violation  over   all    constraints,
                            computed with scaling/renormalization applied.

Violations of box constraints are scaled on per-component basis  according
to  the  scale  vector s[] as specified by minnlcsetscale(). Violations of
the general linear  constraints  are  also  computed  using  user-supplied
variable scaling. Violations of nonlinear constraints are computed "as is"

LAGRANGE COEFFICIENTS

NLPIPM and modern SQP (one activated by setalgosqp()/setalgosqpbfgs(), but
not with legacy functions) set the following fields  (other  solvers  fill
them by zeros):

* lagbc[]                   array[N],   Lagrange   multipliers   for   box
                            constraints. IMPORTANT: COEFFICIENTS FOR FIXED
                            VARIABLES ARE SET TO ZERO. See  below  for  an
                            explanation.
                            This  parameter   stores    the  same  results
                            independently of whether analytic gradient  is
                            provided or numerical differentiation is used.

* lagbcnz[]                 array[N],   Lagrange   multipliers   for   box
                            constraints, behaves differently depending  on
                            whether  analytic  gradient  is   provided  or
                            numerical differentiation is used:
                            * for analytic Jacobian,   lagbcnz[]  contains
                              correct   coefficients   for  all  kinds  of
                              variables - fixed or not.
                            * for numerical Jacobian, it is  the  same  as
                              lagbc[], i.e. components corresponding  to
                              fixed vars are zero.
                            See below for an explanation.

* laglc[]                   array[Mlin], coeffs for linear constraints

* lagnlc[]                  array[Mnlc], coeffs for nonlinear constraints


Positive Lagrange coefficient means that constraint is at its upper bound.
Negative coefficient means that constraint is at its lower  bound.  It  is
expected that at the solution the dual feasibility condition holds:

    grad + SUM(Ei*LagBC[i],i=0..n-1) +
        SUM(Ai*LagLC[i],i=0..mlin-1) +
        SUM(Ni*LagNLC[i],i=0..mnlc-1) ~ 0

    (except for fixed variables which are handled specially)

where
* grad is a gradient at the solution
* Ei is a vector with 1.0 at position I and 0 in other positions
* Ai is an I-th row of linear constraint matrix
* Ni is an gradient of I-th nonlinear constraint

Fixed variables have two sets of Lagrange multipliers  for  the  following
reasons:
* analytic gradient and numerical gradient behave  differently  for  fixed
  vars. Numerical differentiation does not violate box  constraints,  thus
  gradient components corresponding to fixed vars are zero because we have
  no way of  differentiating  for  these  vars   without   violating   box
  constraints.
  Contrary to that, analytic gradient usually returns correct values  even
  for fixed vars.
* ideally,  we  would  like  numerical  gradient  to  be an almost perfect
  replacement for an analytic one.  Thus,  we  need  Lagrange  multipliers
  which do not change when we change the gradient type.
* on the other hand, we do not want to  lose  the  possibility  of  having
  a full set of Lagrange multipliers for problems with analytic gradient.
  Thus, there is a special field lagbcnz[] whose contents depends  on  the
  information available to us.

TERMINATION CODES

TerminationType field contains completion code, which can be either FAILURE
code, SUCCESS code, or SUCCESS code + ADDITIONAL code.  The  latter option
is used for more detailed reporting.

=== FAILURE CODE ===
  -9    initial point violates hard constraints, immediately stopping
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient, recovery was impossible.  Abnormal  termination
        signaled.
  -6    one of hard constraints has both bounds present;  this  option  is
        not supported.
  -4    the problem is likely  to  be  unbounded;  presently  only  NLPIPM
        solver reports this code. For NLP problems (not LP, nor QP) it  is
        generally impossible to provide an unboundedness  certificate,  so
        only heuristics are possible, based on growth of |x| and  decrease
        of f compared to |f(x0)|. It is the only failure code that returns
        something meaningful in X.
  -3    box  constraints  are  infeasible.  Note: infeasibility of non-box
        constraints does NOT trigger emergency  completion;  you  have  to
        examine  bcerr/lcerr/nlcerr   to  detect   possibly   inconsistent
        constraints.

When a failure code, different from -4, is set, the solution is filled  by
NANs. When failure code is  equal  to  -4,  a  point  where  unboundedness
suspicions were triggered, is returned.

=== SUCCESS CODE ===
   1    small objective decrease indicates convergence
   2    Eps-based condition (depending on the solver):
        * relative step is no more than Eps (for SQP and similar)
        * primal/dual/complementarity errors are less than Eps (for NLPIPM)
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    user requested algorithm termination via minnlcrequesttermination(),
        last accepted point is returned
  21    special code reserved for MINLP subproblems
  22    special code reserved for MINLP subproblems

=== ADDITIONAL CODES ===
* +800      if   during   algorithm   execution   the   solver encountered
            NAN/INF values in the target or  constraints  but  managed  to
            recover by reducing trust region radius,  the  solver  returns
            one of SUCCESS codes but adds +800 to the code.

Other fields of this structure are not documented and should not be used!
*************************************************************************/
class _minnlcreport_owner
{
public:
    _minnlcreport_owner();
    _minnlcreport_owner(alglib_impl::minnlcreport *attach_to);
    _minnlcreport_owner(const _minnlcreport_owner &rhs);
    _minnlcreport_owner& operator=(const _minnlcreport_owner &rhs);
    virtual ~_minnlcreport_owner();
    alglib_impl::minnlcreport* c_ptr();
    const alglib_impl::minnlcreport* c_ptr() const;
protected:
    alglib_impl::minnlcreport *p_struct;
    bool is_attached;
};
class minnlcreport : public _minnlcreport_owner
{
public:
    minnlcreport();
    minnlcreport(alglib_impl::minnlcreport *attach_to);
    minnlcreport(const minnlcreport &rhs);
    minnlcreport& operator=(const minnlcreport &rhs);
    virtual ~minnlcreport();
    double &f;
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &terminationtype;
    double &bcerr;
    ae_int_t &bcidx;
    double &lcerr;
    ae_int_t &lcidx;
    double &nlcerr;
    ae_int_t &nlcidx;
    double &sclfeaserr;
    real_1d_array lagbc;
    real_1d_array lagbcnz;
    real_1d_array laglc;
    real_1d_array lagnlc;
    ae_int_t &dbgphase0its;


};
#endif

#if defined(AE_COMPILE_QQPSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_QPDENSEAULSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MONBI) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINMO) || !defined(AE_PARTIAL_BUILD)
class _minmostate_owner;
class minmostate;
class _minmoreport_owner;
class minmoreport;


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinMO subpackage to work  with  this
object
*************************************************************************/
class _minmostate_owner
{
public:
    _minmostate_owner();
    _minmostate_owner(alglib_impl::minmostate *attach_to);
    _minmostate_owner(const _minmostate_owner &rhs);
    _minmostate_owner& operator=(const _minmostate_owner &rhs);
    virtual ~_minmostate_owner();
    alglib_impl::minmostate* c_ptr();
    const alglib_impl::minmostate* c_ptr() const;
protected:
    alglib_impl::minmostate *p_struct;
    bool is_attached;
};
class minmostate : public _minmostate_owner
{
public:
    minmostate();
    minmostate(alglib_impl::minmostate *attach_to);
    minmostate(const minmostate &rhs);
    minmostate& operator=(const minmostate &rhs);
    virtual ~minmostate();
    ae_bool &needfi;
    ae_bool &needfij;
    ae_bool &xupdated;
    double &f;
    real_1d_array fi;
    real_2d_array j;
    real_1d_array x;


};


/*************************************************************************
These fields store optimization report:
* inneriterationscount      total number of inner iterations
* outeriterationscount      number of internal optimization sessions performed
* nfev                      number of gradient evaluations
* terminationtype           termination type (see below)

Scaled constraint violations (maximum over all Pareto points) are reported:
* bcerr                     maximum violation of the box constraints
* bcidx                     index of the most violated box  constraint (or
                            -1, if all box constraints  are  satisfied  or
                            there are no box constraint)
* lcerr                     maximum violation of the  linear  constraints,
                            computed as maximum  scaled  distance  between
                            final point and constraint boundary.
* lcidx                     index of the most violated  linear  constraint
                            (or -1, if all constraints  are  satisfied  or
                            there are no general linear constraints)
* nlcerr                    maximum violation of the nonlinear constraints
* nlcidx                    index of the most violated nonlinear constraint
                            (or -1, if all constraints  are  satisfied  or
                            there are no nonlinear constraints)

Violations  of  the  box  constraints  are  scaled  on per-component basis
according to  the  scale  vector s[]  as specified by the minmosetscale().
Violations of the general linear  constraints  are  also   computed  using
user-supplied variable scaling. Violations of  the  nonlinear  constraints
are computed "as is"

TERMINATION CODES

TerminationType field contains completion code, which can be either:

=== FAILURE CODE ===
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signaled.
  -3    box  constraints  are  infeasible.  Note: infeasibility of non-box
        constraints does NOT trigger emergency  completion;  you  have  to
        examine  bcerr/lcerr/nlcerr   to  detect   possibly   inconsistent
        constraints.

=== SUCCESS CODE ===
   2    relative step is no more than EpsX.
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.

NOTE: The solver internally performs many optimization sessions:  one  for
      each Pareto point, and some  amount  of  preparatory  optimizations.
      Different optimization  sessions  may  return  different  completion
      codes. If at least one of internal optimizations failed, its failure
      code is returned. If none of them failed, the most frequent code  is
      returned.

Other fields of this structure are not documented and should not be used!
*************************************************************************/
class _minmoreport_owner
{
public:
    _minmoreport_owner();
    _minmoreport_owner(alglib_impl::minmoreport *attach_to);
    _minmoreport_owner(const _minmoreport_owner &rhs);
    _minmoreport_owner& operator=(const _minmoreport_owner &rhs);
    virtual ~_minmoreport_owner();
    alglib_impl::minmoreport* c_ptr();
    const alglib_impl::minmoreport* c_ptr() const;
protected:
    alglib_impl::minmoreport *p_struct;
    bool is_attached;
};
class minmoreport : public _minmoreport_owner
{
public:
    minmoreport();
    minmoreport(alglib_impl::minmoreport *attach_to);
    minmoreport(const minmoreport &rhs);
    minmoreport& operator=(const minmoreport &rhs);
    virtual ~minmoreport();
    ae_int_t &inneriterationscount;
    ae_int_t &outeriterationscount;
    ae_int_t &nfev;
    ae_int_t &terminationtype;
    double &bcerr;
    ae_int_t &bcidx;
    double &lcerr;
    ae_int_t &lcidx;
    double &nlcerr;
    ae_int_t &nlcidx;


};
#endif

#if defined(AE_COMPILE_MINBC) || !defined(AE_PARTIAL_BUILD)
class _minbcstate_owner;
class minbcstate;
class _minbcreport_owner;
class minbcreport;


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinBC subpackage to work with this
object
*************************************************************************/
class _minbcstate_owner
{
public:
    _minbcstate_owner();
    _minbcstate_owner(alglib_impl::minbcstate *attach_to);
    _minbcstate_owner(const _minbcstate_owner &rhs);
    _minbcstate_owner& operator=(const _minbcstate_owner &rhs);
    virtual ~_minbcstate_owner();
    alglib_impl::minbcstate* c_ptr();
    const alglib_impl::minbcstate* c_ptr() const;
protected:
    alglib_impl::minbcstate *p_struct;
    bool is_attached;
};
class minbcstate : public _minbcstate_owner
{
public:
    minbcstate();
    minbcstate(alglib_impl::minbcstate *attach_to);
    minbcstate(const minbcstate &rhs);
    minbcstate& operator=(const minbcstate &rhs);
    virtual ~minbcstate();
    ae_bool &needf;
    ae_bool &needfg;
    ae_bool &xupdated;
    double &f;
    real_1d_array g;
    real_1d_array x;


};


/*************************************************************************
This structure stores optimization report:
* iterationscount           number of iterations
* nfev                      number of gradient evaluations
* terminationtype           termination type (see below)

TERMINATION CODES

terminationtype field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
  -3    inconsistent constraints.
   1    relative function improvement is no more than EpsF.
   2    relative step is no more than EpsX.
   4    gradient norm is no more than EpsG
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    terminated by user who called minbcrequesttermination(). X contains
        point which was "current accepted" when  termination  request  was
        submitted.
*************************************************************************/
class _minbcreport_owner
{
public:
    _minbcreport_owner();
    _minbcreport_owner(alglib_impl::minbcreport *attach_to);
    _minbcreport_owner(const _minbcreport_owner &rhs);
    _minbcreport_owner& operator=(const _minbcreport_owner &rhs);
    virtual ~_minbcreport_owner();
    alglib_impl::minbcreport* c_ptr();
    const alglib_impl::minbcreport* c_ptr() const;
protected:
    alglib_impl::minbcreport *p_struct;
    bool is_attached;
};
class minbcreport : public _minbcreport_owner
{
public:
    minbcreport();
    minbcreport(alglib_impl::minbcreport *attach_to);
    minbcreport(const minbcreport &rhs);
    minbcreport& operator=(const minbcreport &rhs);
    virtual ~minbcreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &varidx;
    ae_int_t &terminationtype;


};
#endif

#if defined(AE_COMPILE_MINNS) || !defined(AE_PARTIAL_BUILD)
class _minnsstate_owner;
class minnsstate;
class _minnsreport_owner;
class minnsreport;


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinNS subpackage to work  with  this
object
*************************************************************************/
class _minnsstate_owner
{
public:
    _minnsstate_owner();
    _minnsstate_owner(alglib_impl::minnsstate *attach_to);
    _minnsstate_owner(const _minnsstate_owner &rhs);
    _minnsstate_owner& operator=(const _minnsstate_owner &rhs);
    virtual ~_minnsstate_owner();
    alglib_impl::minnsstate* c_ptr();
    const alglib_impl::minnsstate* c_ptr() const;
protected:
    alglib_impl::minnsstate *p_struct;
    bool is_attached;
};
class minnsstate : public _minnsstate_owner
{
public:
    minnsstate();
    minnsstate(alglib_impl::minnsstate *attach_to);
    minnsstate(const minnsstate &rhs);
    minnsstate& operator=(const minnsstate &rhs);
    virtual ~minnsstate();
    ae_bool &needfi;
    ae_bool &needfij;
    ae_bool &xupdated;
    double &f;
    real_1d_array fi;
    real_2d_array j;
    real_1d_array x;


};


/*************************************************************************
This structure stores optimization report:
* IterationsCount           total number of inner iterations
* NFEV                      number of gradient evaluations
* TerminationType           termination type (see below)
* CErr                      maximum violation of all types of constraints
* LCErr                     maximum violation of linear constraints
* NLCErr                    maximum violation of nonlinear constraints

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
  -3    box constraints are inconsistent
  -1    inconsistent parameters were passed:
        * penalty parameter for minnssetalgoags() is zero,
          but we have nonlinear constraints set by minnssetnlc()
   2    sampling radius decreased below epsx
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    User requested termination via MinNSRequestTermination()

Other fields of this structure are not documented and should not be used!
*************************************************************************/
class _minnsreport_owner
{
public:
    _minnsreport_owner();
    _minnsreport_owner(alglib_impl::minnsreport *attach_to);
    _minnsreport_owner(const _minnsreport_owner &rhs);
    _minnsreport_owner& operator=(const _minnsreport_owner &rhs);
    virtual ~_minnsreport_owner();
    alglib_impl::minnsreport* c_ptr();
    const alglib_impl::minnsreport* c_ptr() const;
protected:
    alglib_impl::minnsreport *p_struct;
    bool is_attached;
};
class minnsreport : public _minnsreport_owner
{
public:
    minnsreport();
    minnsreport(alglib_impl::minnsreport *attach_to);
    minnsreport(const minnsreport &rhs);
    minnsreport& operator=(const minnsreport &rhs);
    virtual ~minnsreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    double &cerr;
    double &lcerr;
    double &nlcerr;
    ae_int_t &terminationtype;
    ae_int_t &varidx;
    ae_int_t &funcidx;


};
#endif

#if defined(AE_COMPILE_MINDF) || !defined(AE_PARTIAL_BUILD)
class _mindfstate_owner;
class mindfstate;
class _mindfreport_owner;
class mindfreport;


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinDF subpackage to work  with  this
object
*************************************************************************/
class _mindfstate_owner
{
public:
    _mindfstate_owner();
    _mindfstate_owner(alglib_impl::mindfstate *attach_to);
    _mindfstate_owner(const _mindfstate_owner &rhs);
    _mindfstate_owner& operator=(const _mindfstate_owner &rhs);
    virtual ~_mindfstate_owner();
    alglib_impl::mindfstate* c_ptr();
    const alglib_impl::mindfstate* c_ptr() const;
protected:
    alglib_impl::mindfstate *p_struct;
    bool is_attached;
};
class mindfstate : public _mindfstate_owner
{
public:
    mindfstate();
    mindfstate(alglib_impl::mindfstate *attach_to);
    mindfstate(const mindfstate &rhs);
    mindfstate& operator=(const mindfstate &rhs);
    virtual ~mindfstate();


};


/*************************************************************************
This structure stores optimization report:
* f                         objective value at the solution
* iterationscount           total number of inner iterations
* nfev                      number of gradient evaluations
* terminationtype           termination type (see below)
* bcerr                     maximum violation of box constraints
* lcerr                     maximum violation of linear constraints
* nlcerr                    maximum violation of nonlinear constraints

If timers were activated, the structure also stores running times:
* timesolver                time (in seconds, stored as a floating-point
                            value) spent in the solver itself. Time spent
                            in the user callback is not included.
                            See 'TIMERS' below for more information.
* timecallback              time (in seconds, stored as a floating-point
                            value) spent in the user callback.
                            See 'TIMERS' below for more information.
* timetotal                 total time spent during the optimization,
                            including both the solver and callbacks.
                            See 'TIMERS' below for more information.
In  order  to  activate timers, the caller has to  call  mindfusetimers()
function.

Other fields of this structure are not documented and should not be used!

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
  -3    box constraints are inconsistent
  -1    inconsistent parameters were passed:
        * penalty parameter is zero, but we have nonlinear constraints
          set by mindfsetnlc2()
   1    function value has converged within epsf
   2    sampling radius decreased below epsx
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    User requested termination via mindfrequesttermination()

TIMERS

Starting from ALGLIB 4.04, many optimizers report time  spent  in  in  the
solver itself and in user  callbacks. The  time  is  reported  in seconds,
using floating-point (i.e. fractional-length intervals can  be  reported).

In  order  to  activate timers, the caller has to  call  mindfusetimers()
function.

The accuracy of   the  reported  value depends on the specific programming
language and OS being used:
* C++, no AE_OS is #defined - the accuracy is that of time() function, i.e.
  one second.
* C++, AE_OS=AE_WINDOWS is #defined - the accuracy is that of GetTickCount(),
  i.e. about 10-20ms
* C++, AE_OS=AE_POSIX is #defined - the accuracy is that of gettimeofday()
* C#, managed core, any OS - the accuracy is that of Environment.TickCount
* C#, HPC core, any OS - the accuracy is that of a corresponding C++ version
* any other language - the accuracy is that of a corresponding C++ version

Whilst modern operating systems provide more accurate timers, these timers
often have significant overhead or backward  compatibility  issues.  Thus,
ALGLIB stick to the most basic and efficient functions, even at  the  cost
of some accuracy being lost.
*************************************************************************/
class _mindfreport_owner
{
public:
    _mindfreport_owner();
    _mindfreport_owner(alglib_impl::mindfreport *attach_to);
    _mindfreport_owner(const _mindfreport_owner &rhs);
    _mindfreport_owner& operator=(const _mindfreport_owner &rhs);
    virtual ~_mindfreport_owner();
    alglib_impl::mindfreport* c_ptr();
    const alglib_impl::mindfreport* c_ptr() const;
protected:
    alglib_impl::mindfreport *p_struct;
    bool is_attached;
};
class mindfreport : public _mindfreport_owner
{
public:
    mindfreport();
    mindfreport(alglib_impl::mindfreport *attach_to);
    mindfreport(const mindfreport &rhs);
    mindfreport& operator=(const mindfreport &rhs);
    virtual ~mindfreport();
    double &f;
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    double &bcerr;
    double &lcerr;
    double &nlcerr;
    ae_int_t &terminationtype;
    double &timetotal;
    double &timesolver;
    double &timecallback;


};
#endif

#if defined(AE_COMPILE_NLS) || !defined(AE_PARTIAL_BUILD)
class _nlsstate_owner;
class nlsstate;
class _nlsreport_owner;
class nlsreport;


/*************************************************************************
Nonlinear least squares solver
*************************************************************************/
class _nlsstate_owner
{
public:
    _nlsstate_owner();
    _nlsstate_owner(alglib_impl::nlsstate *attach_to);
    _nlsstate_owner(const _nlsstate_owner &rhs);
    _nlsstate_owner& operator=(const _nlsstate_owner &rhs);
    virtual ~_nlsstate_owner();
    alglib_impl::nlsstate* c_ptr();
    const alglib_impl::nlsstate* c_ptr() const;
protected:
    alglib_impl::nlsstate *p_struct;
    bool is_attached;
};
class nlsstate : public _nlsstate_owner
{
public:
    nlsstate();
    nlsstate(alglib_impl::nlsstate *attach_to);
    nlsstate(const nlsstate &rhs);
    nlsstate& operator=(const nlsstate &rhs);
    virtual ~nlsstate();


};


/*************************************************************************
Optimization report, filled by NLSResults() function

FIELDS:
* TerminationType, completion code, which is a sum of a BASIC code and  an
ADDITIONAL code.

  The following basic codes denote failure:
    * -8    optimizer detected NAN/INF  either  in  the  function  itself,
            or its Jacobian; recovery was impossible, abnormal termination
            reported.
    * -3    box constraints are inconsistent

  The following basic codes denote success:
    *  2    relative step is no more than EpsX.
    *  5    MaxIts steps was taken
    *  7    stopping conditions are too stringent,
            further improvement is impossible
    *  8    terminated   by  user  who  called  NLSRequestTermination().
            X contains point which was "current accepted" when termination
            request was submitted.

  Additional codes can be set on success, but not on failure:
    * +800  if   during   algorithm   execution   the   solver encountered
            NAN/INF values in the target or  constraints  but  managed  to
            recover by reducing trust region radius,  the  solver  returns
            one of SUCCESS codes but adds +800 to the code.

* IterationsCount, contains iterations count
* NFunc, number of function calculations
*************************************************************************/
class _nlsreport_owner
{
public:
    _nlsreport_owner();
    _nlsreport_owner(alglib_impl::nlsreport *attach_to);
    _nlsreport_owner(const _nlsreport_owner &rhs);
    _nlsreport_owner& operator=(const _nlsreport_owner &rhs);
    virtual ~_nlsreport_owner();
    alglib_impl::nlsreport* c_ptr();
    const alglib_impl::nlsreport* c_ptr() const;
protected:
    alglib_impl::nlsreport *p_struct;
    bool is_attached;
};
class nlsreport : public _nlsreport_owner
{
public:
    nlsreport();
    nlsreport(alglib_impl::nlsreport *attach_to);
    nlsreport(const nlsreport &rhs);
    nlsreport& operator=(const nlsreport &rhs);
    virtual ~nlsreport();
    ae_int_t &iterationscount;
    ae_int_t &terminationtype;
    ae_int_t &nfunc;


};
#endif

#if defined(AE_COMPILE_LPQPPRESOLVE) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINQP) || !defined(AE_PARTIAL_BUILD)
class _minqpstate_owner;
class minqpstate;
class _minqpreport_owner;
class minqpreport;


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinQP subpackage to work with this
object
*************************************************************************/
class _minqpstate_owner
{
public:
    _minqpstate_owner();
    _minqpstate_owner(alglib_impl::minqpstate *attach_to);
    _minqpstate_owner(const _minqpstate_owner &rhs);
    _minqpstate_owner& operator=(const _minqpstate_owner &rhs);
    virtual ~_minqpstate_owner();
    alglib_impl::minqpstate* c_ptr();
    const alglib_impl::minqpstate* c_ptr() const;
protected:
    alglib_impl::minqpstate *p_struct;
    bool is_attached;
};
class minqpstate : public _minqpstate_owner
{
public:
    minqpstate();
    minqpstate(alglib_impl::minqpstate *attach_to);
    minqpstate(const minqpstate &rhs);
    minqpstate& operator=(const minqpstate &rhs);
    virtual ~minqpstate();


};


/*************************************************************************
This structure stores optimization report:
* InnerIterationsCount      number of inner iterations
* OuterIterationsCount      number of outer iterations
* NCholesky                 number of Cholesky decomposition
* NMV                       number of matrix-vector products
                            (only products calculated as part of iterative
                            process are counted)
* TerminationType           completion code (see below)
* F                         for positive terminationtype stores quadratic
                            model value at the solution
* LagBC                     Lagrange multipliers for box constraints,
                            array[N]
* LagLC                     Lagrange multipliers for linear constraints,
                            array[MSparse+MDense]
* LagQC                     Lagrange multipliers for quadratic constraints

=== COMPLETION CODES =====================================================

Completion codes:
* -9    failure of the automatic scale evaluation:  one  of  the  diagonal
        elements of the quadratic term is non-positive.  Specify  variable
        scales manually!
* -5    inappropriate solver was used:
        * QuickQP solver for a problem with general linear constraints (dense/sparse)
        * QuickQP/DENSE-AUL/DENSE-IPM/SPARSE-IPM for a problem with
          quadratic/conic constraints
        * ECQP for a problem with inequality or nonlinear equality constraints
* -4    the problem is highly likely to be unbounded; either one of the solvers
        found an unconstrained direction of negative curvature, or objective
        simply decreased for too much (more than 1E50).
* -3    inconsistent constraints (or, maybe, feasible point is
        too hard to find). If you are sure that constraints are feasible,
        try to restart optimizer with better initial approximation.
* -2    IPM solver has difficulty finding primal/dual feasible point.
        It is likely that the problem is either infeasible or unbounded,
        but it is difficult to determine exact reason for termination.
        X contains best point found so far.
*  1..4 successful completion
*  5    MaxIts steps was taken
*  7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.

=== LAGRANGE MULTIPLIERS =================================================

Some  optimizers  report  values of  Lagrange  multipliers  on  successful
completion (positive completion code):
* dense and sparse IPM/GENIPM return very precise Lagrange  multipliers as
  determined during solution process.
* DENSE-AUL-QP returns approximate Lagrange multipliers  (which  are  very
  close to "true"  Lagrange  multipliers  except  for  overconstrained  or
  degenerate problems)

Three arrays of multipliers are returned:
* LagBC is array[N] which is loaded with multipliers from box constraints;
  LagBC[i]>0 means that I-th constraint is at the  upper bound, LagBC[I]<0
  means that I-th constraint is at the lower bound, LagBC[I]=0 means  that
  I-th box constraint is inactive.
* LagLC is array[MSparse+MDense] which is  loaded  with  multipliers  from
  general  linear  constraints  (former  MSparse  elements  corresponds to
  sparse part of the constraint matrix, latter MDense are  for  the  dense
  constraints, as was specified by user).
  LagLC[i]>0 means that I-th constraint at  the  upper  bound,  LagLC[i]<0
  means that I-th constraint is at the lower bound, LagLC[i]=0 means  that
  I-th linear constraint is inactive.
* LagQC is array[MQC]  which stores multipliers for quadratic constraints.
  LagQC[i]>0 means that I-th constraint at  the  upper  bound,  LagQC[i]<0
  means that I-th constraint is at the lower bound, LagQC[i]=0 means  that
  I-th linear constraint is inactive.

On failure (or when optimizer does not support Lagrange multipliers) these
arrays are zero-filled.

It is expected that at solution the dual feasibility condition holds:

    C+H*(Xs-X0) + SUM(Ei*LagBC[i],i=0..n-1) + SUM(Ai*LagLC[i],i=0..m-1) + ... ~ 0

where
* C is a linear term
* H is a quadratic term
* Xs is a solution, and X0 is an origin term (zero by default)
* Ei is a vector with 1.0 at position I and 0 in other positions
* Ai is an I-th row of linear constraint matrix

NOTE: methods  from  IPM  family  may  also  return  meaningful   Lagrange
      multipliers  on  completion   with   code   -2   (infeasibility   or
      unboundedness  detected).
*************************************************************************/
class _minqpreport_owner
{
public:
    _minqpreport_owner();
    _minqpreport_owner(alglib_impl::minqpreport *attach_to);
    _minqpreport_owner(const _minqpreport_owner &rhs);
    _minqpreport_owner& operator=(const _minqpreport_owner &rhs);
    virtual ~_minqpreport_owner();
    alglib_impl::minqpreport* c_ptr();
    const alglib_impl::minqpreport* c_ptr() const;
protected:
    alglib_impl::minqpreport *p_struct;
    bool is_attached;
};
class minqpreport : public _minqpreport_owner
{
public:
    minqpreport();
    minqpreport(alglib_impl::minqpreport *attach_to);
    minqpreport(const minqpreport &rhs);
    minqpreport& operator=(const minqpreport &rhs);
    virtual ~minqpreport();
    ae_int_t &inneriterationscount;
    ae_int_t &outeriterationscount;
    ae_int_t &nmv;
    ae_int_t &ncholesky;
    ae_int_t &terminationtype;
    double &f;
    real_1d_array lagbc;
    real_1d_array laglc;
    real_1d_array lagqc;


};
#endif

#if defined(AE_COMPILE_MINLM) || !defined(AE_PARTIAL_BUILD)
class _minlmstate_owner;
class minlmstate;
class _minlmreport_owner;
class minlmreport;


/*************************************************************************
Levenberg-Marquardt optimizer.

This structure should be created using one of the MinLMCreate???()
functions. You should not access its fields directly; use ALGLIB functions
to work with it.
*************************************************************************/
class _minlmstate_owner
{
public:
    _minlmstate_owner();
    _minlmstate_owner(alglib_impl::minlmstate *attach_to);
    _minlmstate_owner(const _minlmstate_owner &rhs);
    _minlmstate_owner& operator=(const _minlmstate_owner &rhs);
    virtual ~_minlmstate_owner();
    alglib_impl::minlmstate* c_ptr();
    const alglib_impl::minlmstate* c_ptr() const;
protected:
    alglib_impl::minlmstate *p_struct;
    bool is_attached;
};
class minlmstate : public _minlmstate_owner
{
public:
    minlmstate();
    minlmstate(alglib_impl::minlmstate *attach_to);
    minlmstate(const minlmstate &rhs);
    minlmstate& operator=(const minlmstate &rhs);
    virtual ~minlmstate();


};


/*************************************************************************
Optimization report, filled by MinLMResults() function

FIELDS:
* TerminationType, completetion code:
    * -8    optimizer detected NAN/INF values either in the function itself,
            or in its Jacobian
    * -5    inappropriate solver was used:
            * solver created with minlmcreatefgh() used  on  problem  with
              general linear constraints (set with minlmsetlc() call).
    * -3    constraints are inconsistent
    *  2    relative step is no more than EpsX.
    *  5    MaxIts steps was taken
    *  7    stopping conditions are too stringent,
            further improvement is impossible
    *  8    terminated   by  user  who  called  MinLMRequestTermination().
            X contains point which was "current accepted" when termination
            request was submitted.
* F, objective value, SUM(f[i]^2)
* IterationsCount, contains iterations count
* NFunc, number of function calculations
* NJac, number of Jacobi matrix calculations
* NGrad, number of gradient calculations
* NHess, number of Hessian calculations
* NCholesky, number of Cholesky decomposition calculations
*************************************************************************/
class _minlmreport_owner
{
public:
    _minlmreport_owner();
    _minlmreport_owner(alglib_impl::minlmreport *attach_to);
    _minlmreport_owner(const _minlmreport_owner &rhs);
    _minlmreport_owner& operator=(const _minlmreport_owner &rhs);
    virtual ~_minlmreport_owner();
    alglib_impl::minlmreport* c_ptr();
    const alglib_impl::minlmreport* c_ptr() const;
protected:
    alglib_impl::minlmreport *p_struct;
    bool is_attached;
};
class minlmreport : public _minlmreport_owner
{
public:
    minlmreport();
    minlmreport(alglib_impl::minlmreport *attach_to);
    minlmreport(const minlmreport &rhs);
    minlmreport& operator=(const minlmreport &rhs);
    virtual ~minlmreport();
    ae_int_t &iterationscount;
    ae_int_t &terminationtype;
    double &f;
    ae_int_t &nfunc;
    ae_int_t &njac;
    ae_int_t &ngrad;
    ae_int_t &nhess;
    ae_int_t &ncholesky;


};
#endif

#if defined(AE_COMPILE_REVISEDDUALSIMPLEX) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_LPSOLVERS) || !defined(AE_PARTIAL_BUILD)
class _minlpstate_owner;
class minlpstate;
class _minlpreport_owner;
class minlpreport;


/*************************************************************************
This object stores linear solver state.
You should use functions provided by MinLP subpackage to work with this
object
*************************************************************************/
class _minlpstate_owner
{
public:
    _minlpstate_owner();
    _minlpstate_owner(alglib_impl::minlpstate *attach_to);
    _minlpstate_owner(const _minlpstate_owner &rhs);
    _minlpstate_owner& operator=(const _minlpstate_owner &rhs);
    virtual ~_minlpstate_owner();
    alglib_impl::minlpstate* c_ptr();
    const alglib_impl::minlpstate* c_ptr() const;
protected:
    alglib_impl::minlpstate *p_struct;
    bool is_attached;
};
class minlpstate : public _minlpstate_owner
{
public:
    minlpstate();
    minlpstate(alglib_impl::minlpstate *attach_to);
    minlpstate(const minlpstate &rhs);
    minlpstate& operator=(const minlpstate &rhs);
    virtual ~minlpstate();


};


/*************************************************************************
This structure stores optimization report:
* f                         target function value
* lagbc                     Lagrange coefficients for box constraints
* laglc                     Lagrange coefficients for linear constraints
* y                         dual variables
* stats                     array[N+M], statuses of box (N) and linear (M)
                            constraints. This array is filled only by  DSS
                            algorithm because IPM always stops at INTERIOR
                            point:
                            * stats[i]>0  =>  constraint at upper bound
                                              (also used for free non-basic
                                              variables set to zero)
                            * stats[i]<0  =>  constraint at lower bound
                            * stats[i]=0  =>  constraint is inactive, basic
                                              variable
* primalerror               primal feasibility error
* dualerror                 dual feasibility error
* slackerror                complementary slackness error
* iterationscount           iteration count
* terminationtype           completion code (see below)

COMPLETION CODES

Completion codes:
* -4    LP problem is primal unbounded (dual infeasible)
* -3    LP problem is primal infeasible (dual unbounded)
*  1..4 successful completion
*  5    MaxIts steps was taken
*  7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.

LAGRANGE COEFFICIENTS

Positive Lagrange coefficient means that constraint is at its upper bound.
Negative coefficient means that constraint is at its lower  bound.  It  is
expected that at the solution the dual feasibility condition holds:

    C + SUM(Ei*LagBC[i],i=0..n-1) + SUM(Ai*LagLC[i],i=0..m-1) ~ 0

where
* C is a cost vector (linear term)
* Ei is a vector with 1.0 at position I and 0 in other positions
* Ai is an I-th row of linear constraint matrix
*************************************************************************/
class _minlpreport_owner
{
public:
    _minlpreport_owner();
    _minlpreport_owner(alglib_impl::minlpreport *attach_to);
    _minlpreport_owner(const _minlpreport_owner &rhs);
    _minlpreport_owner& operator=(const _minlpreport_owner &rhs);
    virtual ~_minlpreport_owner();
    alglib_impl::minlpreport* c_ptr();
    const alglib_impl::minlpreport* c_ptr() const;
protected:
    alglib_impl::minlpreport *p_struct;
    bool is_attached;
};
class minlpreport : public _minlpreport_owner
{
public:
    minlpreport();
    minlpreport(alglib_impl::minlpreport *attach_to);
    minlpreport(const minlpreport &rhs);
    minlpreport& operator=(const minlpreport &rhs);
    virtual ~minlpreport();
    double &f;
    real_1d_array lagbc;
    real_1d_array laglc;
    real_1d_array y;
    integer_1d_array stats;
    double &primalerror;
    double &dualerror;
    double &slackerror;
    ae_int_t &iterationscount;
    ae_int_t &terminationtype;


};
#endif

#if defined(AE_COMPILE_MINCG) || !defined(AE_PARTIAL_BUILD)
class _mincgstate_owner;
class mincgstate;
class _mincgreport_owner;
class mincgreport;


/*************************************************************************
This object stores state of the nonlinear CG optimizer.

You should use ALGLIB functions to work with this object.
*************************************************************************/
class _mincgstate_owner
{
public:
    _mincgstate_owner();
    _mincgstate_owner(alglib_impl::mincgstate *attach_to);
    _mincgstate_owner(const _mincgstate_owner &rhs);
    _mincgstate_owner& operator=(const _mincgstate_owner &rhs);
    virtual ~_mincgstate_owner();
    alglib_impl::mincgstate* c_ptr();
    const alglib_impl::mincgstate* c_ptr() const;
protected:
    alglib_impl::mincgstate *p_struct;
    bool is_attached;
};
class mincgstate : public _mincgstate_owner
{
public:
    mincgstate();
    mincgstate(alglib_impl::mincgstate *attach_to);
    mincgstate(const mincgstate &rhs);
    mincgstate& operator=(const mincgstate &rhs);
    virtual ~mincgstate();
    ae_bool &needf;
    ae_bool &needfg;
    ae_bool &xupdated;
    double &f;
    real_1d_array g;
    real_1d_array x;


};


/*************************************************************************
This structure stores optimization report:
* IterationsCount           total number of inner iterations
* NFEV                      number of gradient evaluations
* TerminationType           termination type (see below)

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
   1    relative function improvement is no more than EpsF.
   2    relative step is no more than EpsX.
   4    gradient norm is no more than EpsG
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    terminated by user who called mincgrequesttermination(). X contains
        point which was "current accepted" when  termination  request  was
        submitted.

Other fields of this structure are not documented and should not be used!
*************************************************************************/
class _mincgreport_owner
{
public:
    _mincgreport_owner();
    _mincgreport_owner(alglib_impl::mincgreport *attach_to);
    _mincgreport_owner(const _mincgreport_owner &rhs);
    _mincgreport_owner& operator=(const _mincgreport_owner &rhs);
    virtual ~_mincgreport_owner();
    alglib_impl::mincgreport* c_ptr();
    const alglib_impl::mincgreport* c_ptr() const;
protected:
    alglib_impl::mincgreport *p_struct;
    bool is_attached;
};
class mincgreport : public _mincgreport_owner
{
public:
    mincgreport();
    mincgreport(alglib_impl::mincgreport *attach_to);
    mincgreport(const mincgreport &rhs);
    mincgreport& operator=(const mincgreport &rhs);
    virtual ~mincgreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &terminationtype;


};
#endif

#if defined(AE_COMPILE_MINCOMP) || !defined(AE_PARTIAL_BUILD)
class _minasastate_owner;
class minasastate;
class _minasareport_owner;
class minasareport;


/*************************************************************************

*************************************************************************/
class _minasastate_owner
{
public:
    _minasastate_owner();
    _minasastate_owner(alglib_impl::minasastate *attach_to);
    _minasastate_owner(const _minasastate_owner &rhs);
    _minasastate_owner& operator=(const _minasastate_owner &rhs);
    virtual ~_minasastate_owner();
    alglib_impl::minasastate* c_ptr();
    const alglib_impl::minasastate* c_ptr() const;
protected:
    alglib_impl::minasastate *p_struct;
    bool is_attached;
};
class minasastate : public _minasastate_owner
{
public:
    minasastate();
    minasastate(alglib_impl::minasastate *attach_to);
    minasastate(const minasastate &rhs);
    minasastate& operator=(const minasastate &rhs);
    virtual ~minasastate();
    ae_bool &needfg;
    ae_bool &xupdated;
    double &f;
    real_1d_array g;
    real_1d_array x;


};


/*************************************************************************

*************************************************************************/
class _minasareport_owner
{
public:
    _minasareport_owner();
    _minasareport_owner(alglib_impl::minasareport *attach_to);
    _minasareport_owner(const _minasareport_owner &rhs);
    _minasareport_owner& operator=(const _minasareport_owner &rhs);
    virtual ~_minasareport_owner();
    alglib_impl::minasareport* c_ptr();
    const alglib_impl::minasareport* c_ptr() const;
protected:
    alglib_impl::minasareport *p_struct;
    bool is_attached;
};
class minasareport : public _minasareport_owner
{
public:
    minasareport();
    minasareport(alglib_impl::minasareport *attach_to);
    minasareport(const minasareport &rhs);
    minasareport& operator=(const minasareport &rhs);
    virtual ~minasareport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &terminationtype;
    ae_int_t &activeconstraints;


};
#endif

#if defined(AE_COMPILE_OPTGUARDAPI) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_OPTS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This function serializes data structure to string.

Important properties of s_out:
* it contains alphanumeric characters, dots, underscores, minus signs
* these symbols are grouped into words, which are separated by spaces
  and Windows-style (CR+LF) newlines
* although  serializer  uses  spaces and CR+LF as separators, you can 
  replace any separator character by arbitrary combination of spaces,
  tabs, Windows or Unix newlines. It allows flexible reformatting  of
  the  string in case you want to include it into a text or XML file. 
  But you should not insert separators into the middle of the "words"
  nor should you change the case of letters.
* s_out can be freely moved between 32-bit and 64-bit systems, little
  and big endian machines, and so on. You can serialize structure  on
  32-bit machine and unserialize it on 64-bit one (or vice versa), or
  serialize  it  on  SPARC  and  unserialize  on  x86.  You  can also 
  serialize it in C++ version of ALGLIB and unserialize it in C# one, 
  and vice versa.
*************************************************************************/
void lptestproblemserialize(const lptestproblem &obj, std::string &s_out);


/*************************************************************************
This function serializes data structure to C++ stream.

Data stream generated by this function is same as  string  representation
generated  by  string  version  of  serializer - alphanumeric characters,
dots, underscores, minus signs, which are grouped into words separated by
spaces and CR+LF.

We recommend you to read comments on string version of serializer to find
out more about serialization of AlGLIB objects.
*************************************************************************/
void lptestproblemserialize(const lptestproblem &obj, std::ostream &s_out);


/*************************************************************************
This function unserializes data structure from string.
*************************************************************************/
void lptestproblemunserialize(const std::string &s_in, lptestproblem &obj);


/*************************************************************************
This function unserializes data structure from stream.
*************************************************************************/
void lptestproblemunserialize(const std::istream &s_in, lptestproblem &obj);


/*************************************************************************
Initialize test LP problem.

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemcreate(const ae_int_t n, const bool hasknowntarget, const double targetf, lptestproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
bool lptestproblemhasknowntarget(lptestproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
double lptestproblemgettargetf(lptestproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
ae_int_t lptestproblemgetn(lptestproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
ae_int_t lptestproblemgetm(lptestproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Set scale for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetscale(lptestproblem &p, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Set cost for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetcost(lptestproblem &p, const real_1d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Set box constraints for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetbc(lptestproblem &p, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Set box constraints for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetlc2(lptestproblem &p, const sparsematrix &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t m, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Returns objective type: True for zero/linear/constant.

Present version does not return False.

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
bool qpxproblemisquadraticobjective(qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get variables count

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t qpxproblemgetn(qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get linear constraints count

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t qpxproblemgetmlc(qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get quadratic constraints count

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t qpxproblemgetmqc(qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get conic constraints count

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t qpxproblemgetmcc(qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get total constraints count

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t qpxproblemgettotalconstraints(qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get initial point

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
void qpxproblemgetinitialpoint(qpxproblem &p, real_1d_array &x0, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get initial point presence

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
bool qpxproblemhasinitialpoint(qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get scale

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
void qpxproblemgetscale(qpxproblem &p, real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get scale presence

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
bool qpxproblemhasscale(qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get origin

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
void qpxproblemgetorigin(qpxproblem &p, real_1d_array &xorigin, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get origin presence

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
bool qpxproblemhasorigin(qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get linear term

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
void qpxproblemgetlinearterm(qpxproblem &p, real_1d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get quadratic term, returns zero matrix if no quadratic term is present

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
void qpxproblemgetquadraticterm(qpxproblem &p, sparsematrix &q, bool &isupper, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Returns False if no quadratic term was specified, or quadratic term is
numerically zero.

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
bool qpxproblemhasquadraticterm(qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get box constraints

  -- ALGLIB --
     Copyright 20.08.2024 by Bochkanov Sergey
*************************************************************************/
void qpxproblemgetbc(qpxproblem &p, real_1d_array &bndl, real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get linear constraints

  -- ALGLIB --
     Copyright 20.08.2024 by Bochkanov Sergey
*************************************************************************/
void qpxproblemgetlc2(qpxproblem &p, sparsematrix &a, real_1d_array &al, real_1d_array &au, ae_int_t &m, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Get IDX-th two-sided quadratic constraint, same format as minqpaddqc2(),
except for the fact that it always returns isUpper=False, even if the
original matrix was an upper triangular one.

NOTE: this function is not optimized for big matrices. Whilst still having
      O(max(N,Nonzeros)) running time, it may  be  somewhat  slow  due  to
      dynamic structures being used internally.


  -- ALGLIB --
     Copyright 19.08.2024 by Bochkanov Sergey
*************************************************************************/
void qpxproblemgetqc2i(qpxproblem &p, const ae_int_t idx, sparsematrix &q, bool &isupper, real_1d_array &b, double &cl, double &cu, bool &applyorigin, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_OPTSERV) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_CQMODELS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_SNNLS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_SACTIVESETS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINBLEIC) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                     BOUND CONSTRAINED OPTIMIZATION
       WITH ADDITIONAL LINEAR EQUALITY AND INEQUALITY CONSTRAINTS

DESCRIPTION:
The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints

REQUIREMENTS:
* user must provide function value and gradient
* starting point X0 must be feasible or
  not too far away from the feasible set
* grad(f) must be Lipschitz continuous on a level set:
  L = { x : f(x)<=f(x0) }
* function must be defined everywhere on the feasible set F

USAGE:

Constrained optimization if far more complex than the unconstrained one.
Here we give very brief outline of the BLEIC optimizer. We strongly recommend
you to read examples in the ALGLIB Reference Manual and to read ALGLIB User Guide
on optimization, which is available at http://www.alglib.net/optimization/

1. User initializes algorithm state with MinBLEICCreate() call

2. USer adds boundary and/or linear constraints by calling
   MinBLEICSetBC() and MinBLEICSetLC() functions.

3. User sets stopping conditions with MinBLEICSetCond().

4. User calls MinBLEICOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.

5. User calls MinBLEICResults() to get solution

6. Optionally user may call MinBLEICRestartFrom() to solve another problem
   with same N but another starting point.
   MinBLEICRestartFrom() allows to reuse already initialized structure.

NOTE: if you have box-only constraints (no  general  linear  constraints),
      then MinBC optimizer can be better option. It uses  special,  faster
      constraint activation method, which performs better on problems with
      multiple constraints active at the solution.

      On small-scale problems performance of MinBC is similar to  that  of
      MinBLEIC, but on large-scale ones (hundreds and thousands of  active
      constraints) it can be several times faster than MinBLEIC.

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
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleiccreate(const ae_int_t n, const real_1d_array &x, minbleicstate &state, const xparams _xparams = alglib::xdefault);
void minbleiccreate(const real_1d_array &x, minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
The subroutine is finite difference variant of MinBLEICCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinBLEICCreate() in  order  to  get
more information about creation of BLEIC optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinBLEICSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust and precise. CG needs exact gradient values. Imprecise
   gradient may slow  down  convergence, especially  on  highly  nonlinear
   problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minbleiccreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, minbleicstate &state, const xparams _xparams = alglib::xdefault);
void minbleiccreatef(const real_1d_array &x, const double diffstep, minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints for BLEIC optimizer.

Boundary constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

NOTE: if you have box-only constraints (no  general  linear  constraints),
      then MinBC optimizer can be better option. It uses  special,  faster
      constraint activation method, which performs better on problems with
      multiple constraints active at the solution.

      On small-scale problems performance of MinBC is similar to  that  of
      MinBLEIC, but on large-scale ones (hundreds and thousands of  active
      constraints) it can be several times faster than MinBLEIC.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by  bound  constraints,
  even  when  numerical  differentiation is used (algorithm adjusts  nodes
  according to boundary constraints)

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbc(minbleicstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets linear constraints for BLEIC optimizer.

Linear constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately:
* there always exists some minor violation (about Epsilon in magnitude)
  due to rounding errors
* numerical differentiation, if used, may  lead  to  function  evaluations
  outside  of the feasible  area,   because   algorithm  does  NOT  change
  numerical differentiation formula according to linear constraints.
If you want constraints to be  satisfied  exactly, try to reformulate your
problem  in  such  manner  that  all constraints will become boundary ones
(this kind of constraints is always satisfied exactly, both in  the  final
solution and in all intermediate points).

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetlc(minbleicstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minbleicsetlc(minbleicstate &state, const real_2d_array &c, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for the optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinBLEICSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinBLEICSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection.

NOTE: when SetCond() called with non-zero MaxIts, BLEIC solver may perform
      slightly more than MaxIts iterations. I.e., MaxIts  sets  non-strict
      limit on iterations count.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetcond(minbleicstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for BLEIC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  BLEIC  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinBLEICSetPrec...()
functions.

There is a special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minbleicsetscale(minbleicstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecdefault(minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE 1: D[i] should be positive. Exception will be thrown otherwise.

NOTE 2: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecdiag(minbleicstate &state, const real_1d_array &d, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinBLEICSetScale()
call  (before  or after MinBLEICSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecscale(minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinBLEICOptimize().

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetxrep(minbleicstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length

IMPORTANT: this feature is hard to combine with preconditioning. You can't
set upper limit on step length, when you solve optimization  problem  with
linear (non-boundary) constraints AND preconditioner turned on.

When  non-boundary  constraints  are  present,  you  have to either a) use
preconditioner, or b) use upper limit on step length.  YOU CAN'T USE BOTH!
In this case algorithm will terminate with appropriate error code.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  lead   to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetstpmax(minbleicstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minbleiciteration(minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    func    -   callback which calculates function (or merit function)
                value func at given point x
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL

NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied gradient,  and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either  MinBLEICCreate() for analytical gradient or  MinBLEICCreateF()
   for numerical differentiation) you should choose appropriate variant of
   MinBLEICOptimize() - one  which  accepts  function  AND gradient or one
   which accepts function ONLY.

   Be careful to choose variant of MinBLEICOptimize() which corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinBLEICOptimize()  and specific
   function used to create optimizer.


                     |         USER PASSED TO MinBLEICOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinBLEICCreateF() |     work                FAIL
   MinBLEICCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function  and  MinBLEICOptimize()  version.   Attemps   to   use   such
   combination (for  example,  to  create optimizer with MinBLEICCreateF()
   and  to  pass  gradient information to MinBLEICOptimize()) will lead to
   exception being thrown. Either  you  did  not pass gradient when it WAS
   needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey


*************************************************************************/
void minbleicoptimize(minbleicstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minbleicoptimize(minbleicstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function  at  the  initial  point
(note: future versions may also perform check  at  the  final  point)  and
compares numerical gradient with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which  stores  both  gradients  and  specific  components  highlighted  as
suspicious by the OptGuard.

The primary OptGuard report can be retrieved with minbleicoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with minbleicsetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minbleicoptguardgradient(minbleicstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0)
b) nonsmooth     target function (non-C1)

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.

      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.

      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.

      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.

=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbleicoptguardsmoothness(minbleicstate &state, const ae_int_t level, const xparams _xparams = alglib::xdefault);
void minbleicoptguardsmoothness(minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* minbleicoptguardgradient() for gradient verification
* minbleicoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradvidx for specific variable (gradient element) suspected
  * rep.badgradxbase, a point where gradient is tested
  * rep.badgraduser, user-provided gradient  (stored  as  2D  matrix  with
    single row in order to make  report  structure  compatible  with  more
    complex optimizers like MinNLC or MinLM)
  * rep.badgradnum,   reference    gradient    obtained    via   numerical
    differentiation (stored as  2D matrix with single row in order to make
    report structure compatible with more complex optimizers  like  MinNLC
    or MinLM)
* rep.nonc0suspected
* rep.nonc1suspected

=== ADDITIONAL REPORTS/LOGS ==============================================

Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* minbleicoptguardnonc1test0results()
* minbleicoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.

      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.

      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbleicoptguardresults(minbleicstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbleicoptguardnonc1test0results(const minbleicstate &state, optguardnonc1test0report &strrep, optguardnonc1test0report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbleicoptguardnonc1test1results(minbleicstate &state, optguardnonc1test1report &strrep, optguardnonc1test1report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
BLEIC results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report. You should check Rep.TerminationType
                in  order  to  distinguish  successful  termination  from
                unsuccessful one:
                * -8    internal integrity control  detected  infinite or
                        NAN   values   in   function/gradient.   Abnormal
                        termination signalled.
                * -3   inconsistent constraints. Feasible point is
                       either nonexistent or too hard to find. Try to
                       restart optimizer with better initial approximation
                *  1   relative function improvement is no more than EpsF.
                *  2   scaled step is no more than EpsX.
                *  4   scaled gradient norm is no more than EpsG.
                *  5   MaxIts steps was taken
                *  8   terminated by user who called minbleicrequesttermination().
                       X contains point which was "current accepted"  when
                       termination request was submitted.
                More information about fields of this  structure  can  be
                found in the comments on MinBLEICReport datatype.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicresults(const minbleicstate &state, real_1d_array &x, minbleicreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
BLEIC results

Buffered implementation of MinBLEICResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicresultsbuf(const minbleicstate &state, real_1d_array &x, minbleicreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicrestartfrom(minbleicstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void minbleicrequesttermination(minbleicstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_LPQPSERV) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_GIPM) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_GQPIPM) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINLBFGS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
        LIMITED MEMORY BFGS METHOD FOR LARGE SCALE OPTIMIZATION

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by  using  a  quasi-
Newton method (LBFGS scheme) which is optimized to use  a  minimum  amount
of memory.
The subroutine generates the approximation of an inverse Hessian matrix by
using information about the last M steps of the algorithm  (instead of N).
It lessens a required amount of memory from a value  of  order  N^2  to  a
value of order 2*N*M.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinLBFGSCreate() call
2. User tunes solver parameters with MinLBFGSSetCond() MinLBFGSSetStpMax()
   and other functions
3. User calls MinLBFGSOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinLBFGSResults() to get solution
5. Optionally user may call MinLBFGSRestartFrom() to solve another problem
   with same N/M but another starting point and/or another function.
   MinLBFGSRestartFrom() allows to reuse already initialized structure.

INPUT PARAMETERS:
    N       -   problem dimension. N>0
    M       -   number of corrections in the BFGS scheme of Hessian
                approximation update. Recommended value:  3<=M<=7. The smaller
                value causes worse convergence, the bigger will  not  cause  a
                considerably better convergence, but will cause a fall in  the
                performance. M<=N.
    X       -   initial solution approximation, array[0..N-1].


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

IMPORTANT: the   LBFGS  optimizer  supports  parallel  parallel  numerical
           differentiation  ('callback    parallelism').   This   feature,
           which  is  present  in  commercial   ALGLIB   editions  greatly
           accelerates  optimization  with  numerical  differentiation  of
           an expensive target functions.

           Callback parallelism is usually  beneficial  when  computing  a
           numerical gradient requires  more  than  several  milliseconds.

           See ALGLIB Reference Manual, 'Working with commercial version'
           section,  and  comments  on  minlbfgsoptimize() function for
           more information.

NOTES:
1. you may tune stopping conditions with MinLBFGSSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLBFGSSetStpMax() function to bound algorithm's  steps.  However,
   L-BFGS rarely needs such a tuning.


  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreate(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlbfgsstate &state, const xparams _xparams = alglib::xdefault);
void minlbfgscreate(const ae_int_t m, const real_1d_array &x, minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
The subroutine is finite difference variant of MinLBFGSCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinLBFGSCreate() in  order  to  get
more information about creation of LBFGS optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of corrections in the BFGS scheme of Hessian
                approximation update. Recommended value:  3<=M<=7. The smaller
                value causes worse convergence, the bigger will  not  cause  a
                considerably better convergence, but will cause a fall in  the
                performance. M<=N.
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state


IMPORTANT: the   LBFGS  optimizer  supports  parallel  parallel  numerical
           differentiation  ('callback    parallelism').   This   feature,
           which  is  present  in  commercial   ALGLIB   editions  greatly
           accelerates  optimization  with  numerical  differentiation  of
           an expensive target functions.

           Callback parallelism is usually  beneficial  when  computing  a
           numerical gradient requires  more  than  several  milliseconds.

           See ALGLIB Reference Manual, 'Working with commercial version'
           section,  and  comments  on  minlbfgsoptimize() function for
           more information.

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinLBFGSSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is   less  robust  and  precise.  LBFGS  needs  exact  gradient values.
   Imprecise gradient may slow  down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreatef(const ae_int_t n, const ae_int_t m, const real_1d_array &x, const double diffstep, minlbfgsstate &state, const xparams _xparams = alglib::xdefault);
void minlbfgscreatef(const ae_int_t m, const real_1d_array &x, const double diffstep, minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for L-BFGS optimization algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinLBFGSSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinLBFGSSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetcond(minlbfgsstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinLBFGSOptimize().


  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetxrep(minlbfgsstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0 (default),  if
                you don't want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetstpmax(minlbfgsstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for LBFGS optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  LBFGS  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinLBFGSSetPrec...()
functions.

There  is  special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetscale(minlbfgsstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification  of  the  preconditioner:  default  preconditioner    (simple
scaling, same for all elements of X) is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecdefault(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: Cholesky factorization of  approximate
Hessian is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    P       -   triangular preconditioner, Cholesky factorization of
                the approximate Hessian. array[0..N-1,0..N-1],
                (if larger, only leading N elements are used).
    IsUpper -   whether upper or lower triangle of P is given
                (other triangle is not referenced)

After call to this function preconditioner is changed to P  (P  is  copied
into the internal buffer).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2:  P  should  be nonsingular. Exception will be thrown otherwise.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetpreccholesky(minlbfgsstate &state, const real_2d_array &p, const bool isupper, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2: D[i] should be positive. Exception will be thrown otherwise.

NOTE 3: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecdiag(minlbfgsstate &state, const real_1d_array &d, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinLBFGSSetScale()
call  (before  or after MinLBFGSSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecscale(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minlbfgsiteration(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    func    -   callback which calculates function (or merit function)
                value func at given point x
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


CALLBACK PARALLELISM:

The LBFGS optimizer supports parallel numerical differentiation ('callback
parallelism').  This  feature,  which  is  present  in  commercial  ALGLIB
editions,  greatly  accelerates  numerical  differentiation  of  expensive
targets.

Callback parallelism is usually beneficial computing a numerical  gradient
requires more than several milliseconds. In this case the job of computing
individual gradient components can be split between multiple threads. Even
inexpensive  targets  can  benefit  from  parallelism, if  you  have  many
variables.

ALGLIB Reference Manual, 'Working with commercial  version' section, tells
how to activate callback parallelism for your programming language.

CALLBACKS ACCEPTED

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied gradient,  and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either MinLBFGSCreate() for analytical gradient  or  MinLBFGSCreateF()
   for numerical differentiation) you should choose appropriate variant of
   MinLBFGSOptimize() - one  which  accepts  function  AND gradient or one
   which accepts function ONLY.

   Be careful to choose variant of MinLBFGSOptimize() which corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinLBFGSOptimize()  and specific
   function used to create optimizer.


                     |         USER PASSED TO MinLBFGSOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinLBFGSCreateF() |     work                FAIL
   MinLBFGSCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function  and  MinLBFGSOptimize()  version.   Attemps   to   use   such
   combination (for example, to create optimizer with MinLBFGSCreateF() and
   to pass gradient information to MinCGOptimize()) will lead to exception
   being thrown. Either  you  did  not pass gradient when it WAS needed or
   you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey


*************************************************************************/
void minlbfgsoptimize(minlbfgsstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minlbfgsoptimize(minlbfgsstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function  at  the  initial  point
(note: future versions may also perform check  at  the  final  point)  and
compares numerical gradient with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which  stores  both  gradients  and  specific  components  highlighted  as
suspicious by the OptGuard.

The primary OptGuard report can be retrieved with minlbfgsoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with minlbfgssetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minlbfgsoptguardgradient(minlbfgsstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0)
b) nonsmooth     target function (non-C1)

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.

      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.

      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.

      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.

=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlbfgsoptguardsmoothness(minlbfgsstate &state, const ae_int_t level, const xparams _xparams = alglib::xdefault);
void minlbfgsoptguardsmoothness(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* minlbfgsoptguardgradient() for gradient verification
* minlbfgsoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradvidx for specific variable (gradient element) suspected
  * rep.badgradxbase, a point where gradient is tested
  * rep.badgraduser, user-provided gradient  (stored  as  2D  matrix  with
    single row in order to make  report  structure  compatible  with  more
    complex optimizers like MinNLC or MinLM)
  * rep.badgradnum,   reference    gradient    obtained    via   numerical
    differentiation (stored as  2D matrix with single row in order to make
    report structure compatible with more complex optimizers  like  MinNLC
    or MinLM)
* rep.nonc0suspected
* rep.nonc1suspected

=== ADDITIONAL REPORTS/LOGS ==============================================

Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* minlbfgsoptguardnonc1test0results()
* minlbfgsoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.

      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.

      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlbfgsoptguardresults(minlbfgsstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlbfgsoptguardnonc1test0results(const minlbfgsstate &state, optguardnonc1test0report &strrep, optguardnonc1test0report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlbfgsoptguardnonc1test1results(minlbfgsstate &state, optguardnonc1test1report &strrep, optguardnonc1test1report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
L-BFGS algorithm results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -8    internal integrity control  detected  infinite
                            or NAN values in  function/gradient.  Abnormal
                            termination signalled.
                    * -2    rounding errors prevent further improvement.
                            X contains best point found.
                    * -1    incorrect parameters were specified
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible
                    *  8    terminated by user who called minlbfgsrequesttermination().
                            X contains point which was "current accepted" when
                            termination request was submitted.
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsresults(const minlbfgsstate &state, real_1d_array &x, minlbfgsreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
L-BFGS algorithm results

Buffered implementation of MinLBFGSResults which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsresultsbuf(const minlbfgsstate &state, real_1d_array &x, minlbfgsreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  subroutine restarts LBFGS algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsrestartfrom(minlbfgsstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void minlbfgsrequesttermination(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_VIPMSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_IPM2SOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLCFSQP) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_DIFFEVO) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_SSGD) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_ECQPSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_GIPM3) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLCGIPM3) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLCSQP) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLCAUL) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_DFGENMOD) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_DYNCRS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLPPRESOLVE) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINNLC) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                  NONLINEARLY  CONSTRAINED  OPTIMIZATION

DESCRIPTION:
The  subroutine  minimizes a function  F(x)  of N arguments subject to the
any combination of the:
* box constraints
* linear equality/inequality/range constraints CL<=Ax<=CU
* nonlinear equality/inequality/range constraints HL<=Hi(x)<=HU

REQUIREMENTS:
* F(), H() are continuously differentiable on the  feasible  set  and  its
  neighborhood
* starting point X0, which can be infeasible

USAGE:

Here we give the very brief outline  of  the MinNLC optimizer. We strongly
recommend you to study examples in the ALGLIB Reference Manual and to read
ALGLIB User Guide: https://www.alglib.net/nonlinear-programming/

1. The user initializes the solver with minnlccreate() or minnlccreatef(),
   depending on the specific solver chosen:
   * minnlccreate() for solvers capable of handling the  problem  'as is',
     without relying on numerical differentiation (SQP/NLPIPM for problems
     with analytic derivatives, ORBIT for derivative-free problems)
   * minnlccreatef(), when a numerical differentiation is used to  provide
     derivatives to a smooth derivative-based solver (SQP or NLPIPM).

   In the current release the following solvers can be used:

   * sparse large-scale filter-based SQP solver, recommended  for problems
     of any size (from several  variables  to  hundreds  of  thousands  of
     variables). Good at warm-starts. Activated with the minnlcsetalgosqp()
     function.

   * sparse large-scale nonlinear nonconvex interior point method (NLPIPM),
     recommended  for  problems  of  any  size  (from several variables to
     hundreds of thousands of variables).  Has  lower  iteration  overhead
     than SQP, but is worse at using good initial  points  (it  will  need
     at  least  tens  of  iterations even when started from the solution).
     Activated with minnlcsetalgonlpipm() function.

   * dense SQP-BFGS solver, recommended  for small-scale problems  (up  to
     several hundreds of variables). Requires  less  function  evaluations
     than SQP, but has more expensive iteration.
     Activated with minnlcsetalgosqpbfgs() function.

   * ORBIT, a model-based derivative  free  solver  that  uses  local  RBF
     models to optimize expensive objectives.  This  solver  is  activated
     with minnlcsetalgoorbit() function.

   * several other solvers, including legacy ones

2. [optional] user activates OptGuard  integrity checker  which  tries  to
   detect possible errors in the user-supplied callbacks:
   * discontinuity/nonsmoothness of the target/nonlinear constraints
   * errors in the analytic gradient provided by user
   This feature is essential for early prototyping stages because it helps
   to catch common coding and problem statement errors.
   OptGuard can be activated with following functions (one per each  check
   performed):
   * minnlcoptguardsmoothness()
   * minnlcoptguardgradient()

3. User adds boundary and/or linear and/or nonlinear constraints by  means
   of calling one of the following functions:
   a) minnlcsetbc() for boundary constraints
   b) minnlcsetlc2() for sparse two-sided linear constraints,
      minnlcsetlc2dense() for dense two-sided linear constraints,
      minnlcsetlc2mixed() for mixed sparse/dense two-sided linear constraints
    * minqpaddlc2dense()  to add one dense row to the dense constraint submatrix
    * minqpaddlc2()       to add one sparse row to the sparse constraint submatrix
    * minqpaddlc2sparsefromdense() to add one sparse row (passed as a dense array) to the sparse constraint submatrix
   c) minnlcsetnlc2() for nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.

4. User sets scale of the variables with minnlcsetscale() function. It  is
   VERY important to set  scale  of  the  variables,  because  nonlinearly
   constrained problems are hard to solve when variables are badly scaled.
   Knowing  variable   scales   helps   to  check  stopping  criteria  and
   precondition the solver.

5. User sets stopping conditions with minnlcsetcond3() or minnlcsetcond().
   If NLC solver uses inner/outer  iteration  layout,  this  function sets
   stopping conditions for INNER iterations.

6. Finally, user calls minnlcoptimize()  function  which  takes  algorithm
   state and pointer (delegate, etc.) to callback function which calculates
   F/G/H.

7. User calls  minnlcresults()  to  get  solution;  additionally  you  can
   retrieve OptGuard report with minnlcoptguardresults(), and get detailed
   report about purported errors in the target function with:
   * minnlcoptguardnonc1test0results()
   * minnlcoptguardnonc1test1results()

8. Optionally user may call minnlcrestartfrom() to solve  another  problem
   with same N but another starting point. minnlcrestartfrom()  allows  to
   reuse already initialized structure.


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
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlccreate(const ae_int_t n, const real_1d_array &x, minnlcstate &state, const xparams _xparams = alglib::xdefault);
void minnlccreate(const real_1d_array &x, minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Buffered version of minnlccreate() which reuses already  allocated  memory
as much as possible.

  -- ALGLIB --
     Copyright 06.10.2024 by Bochkanov Sergey
*************************************************************************/
void minnlccreatebuf(const ae_int_t n, const real_1d_array &x, minnlcstate &state, const xparams _xparams = alglib::xdefault);
void minnlccreatebuf(const real_1d_array &x, minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine is a finite  difference variant of MinNLCCreate(). It uses
finite differences in order to differentiate target function.

This function is relevant when you want to solve a derivative-free problem
using a solver that relies on gradient info being available (e.g.  SQP  or
NLPIPM). Do not use it for derivative-free solvers  like  ORBIT,  the algo
will generate an exception during optimization.

Description below contains information which is specific to this  function
only. We recommend to read comments on MinNLCCreate() in order to get more
information about creation of the NLC optimizer.

CALLBACK PARALLELISM

The MINNLC optimizer supports parallel parallel  numerical differentiation
('callback parallelism'). This feature, which  is  present  in  commercial
ALGLIB  editions,  greatly   accelerates   optimization   with   numerical
differentiation of an expensive target functions.

Callback parallelism is usually  beneficial  when  computing  a  numerical
gradient requires more than several  milliseconds.  In this case  the  job
of computing individual gradient components can be split between  multiple
threads. Even inexpensive targets can benefit  from  parallelism,  if  you
have many variables.

ALGLIB Reference Manual, 'Working with commercial  version' section, tells
how to activate callback parallelism for your programming language.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.
    DiffStep-   differentiation step, >0.
                By default, a 5-point formula is used  (actually,  only  4
                function values per variable are used because  the central
                one has zero coefficient due to symmetry; that's why  this
                formula is often called a 4-point one). It can be  changed
                with minnlcsetnumdiff() function.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

NOTES:

1. the differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinNLCSetScale() call.

2. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large TRUNCATION  errors, while too small
   step will result in too large NUMERICAL  errors.  1.0E-4  can  be  good
   value to start from.

3. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs ~N function evaluations. This function will work  for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.

   We should also say that code which relies on numerical  differentiation
   is  less   robust   and  precise.  Imprecise  gradient  may  slow  down
   convergence, especially  on  highly  nonlinear  problems  or  near  the
   solution.

   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlccreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, minnlcstate &state, const xparams _xparams = alglib::xdefault);
void minnlccreatef(const real_1d_array &x, const double diffstep, minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Buffered version of minnlccreatef() which reuses already allocated memory
as much as possible.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlccreatefbuf(const ae_int_t n, const real_1d_array &x, const double diffstep, minnlcstate &state, const xparams _xparams = alglib::xdefault);
void minnlccreatefbuf(const real_1d_array &x, const double diffstep, minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints for NLC optimizer.

Boundary constraints are inactive by  default  (after  initial  creation).
They are preserved after algorithm restart with  MinNLCRestartFrom().

You may combine boundary constraints with  general  linear ones - and with
nonlinear ones! Boundary constraints are  handled  more  efficiently  than
other types.  Thus,  if  your  problem  has  mixed  constraints,  you  may
explicitly specify some of them as boundary and save some time/space.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1:  it is possible to specify  BndL[i]=BndU[i].  In  this  case  I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2:  when you solve your problem  with  augmented  Lagrangian  solver,
         boundary constraints are  satisfied  only  approximately!  It  is
         possible   that  algorithm  will  evaluate  function  outside  of
         feasible area!

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetbc(minnlcstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets linear constraints for MinNLC optimizer.

Linear constraints are inactive by default (after initial creation).  They
are preserved after algorithm restart with MinNLCRestartFrom().

You may combine linear constraints with boundary ones - and with nonlinear
ones! If your problem has mixed constraints, you  may  explicitly  specify
some of them as linear. It  may  help  optimizer   to   handle  them  more
efficiently.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinNLCCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: when you solve your problem  with  augmented  Lagrangian   solver,
        linear constraints are  satisfied  only   approximately!   It   is
        possible   that  algorithm  will  evaluate  function  outside   of
        feasible area!

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetlc(minnlcstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minnlcsetlc(minnlcstate &state, const real_2d_array &c, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU with dense
constraint matrix A.

INPUT PARAMETERS:
    State   -   structure previously allocated with minnlccreate() call.
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
void minnlcsetlc2dense(minnlcstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minnlcsetlc2dense(minnlcstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
a sparse constraining matrix A. Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

INPUT PARAMETERS:
    State   -   structure previously allocated with minnlccreate() call.
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
void minnlcsetlc2(minnlcstate &state, const sparsematrix &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);


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
    State   -   structure previously allocated with minnlccreate() call.
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
void minnlcsetlc2mixed(minnlcstate &state, const sparsematrix &sparsea, const ae_int_t ksparse, const real_2d_array &densea, const ae_int_t kdense, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends a two-sided linear constraint AL <= A*x <= AU to the
matrix of dense constraints.

INPUT PARAMETERS:
    State   -   structure previously allocated with minnlccreate() call.
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
void minnlcaddlc2dense(minnlcstate &state, const real_1d_array &a, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present sparse constraints.

Constraint is passed in compressed format: as list of non-zero entries  of
coefficient vector A. Such approach is more efficient than  dense  storage
for highly sparse constraint vectors.

INPUT PARAMETERS:
    State   -   structure previously allocated with minnlccreate() call.
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
void minnlcaddlc2(minnlcstate &state, const integer_1d_array &idxa, const real_1d_array &vala, const ae_int_t nnz, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present sparse constraints.

Constraint vector A is  passed  as  a  dense  array  which  is  internally
sparsified by this function.

INPUT PARAMETERS:
    State   -   structure previously allocated with minnlccreate() call.
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
void minnlcaddlc2sparsefromdense(minnlcstate &state, const real_1d_array &da, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets nonlinear constraints for MinNLC optimizer.

It sets constraints of the form

    Ci(x)=0 for i=0..NLEC-1
    Ci(x)<=0 for i=NLEC..NLEC+NLIC-1

See MinNLCSetNLC2() for a modern function which allows greater flexibility
in the constraint specification.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetnlc(minnlcstate &state, const ae_int_t nlec, const ae_int_t nlic, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided nonlinear constraints for MinNLC optimizer.

In fact, this function sets  only  constraints  COUNT  and  their  BOUNDS.
Constraints  themselves  (constraint  functions)   are   passed   to   the
MinNLCOptimize() method as callbacks.

MinNLCOptimize() method accepts a user-defined vector function F[] and its
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
    State   -   structure previously allocated with MinNLCCreate call.
    NL      -   array[NNLC], lower bounds, can contain -INF
    NU      -   array[NNLC], lower bounds, can contain +INF
    NNLC    -   constraints count, NNLC>=0

NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
        possible that the algorithm will evaluate the function  outside of
        the feasible area!

NOTE 2: algorithm scales variables  according  to the scale  specified by
        MinNLCSetScale()  function,  so it can handle problems with badly
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
        a scale set by MinNLCSetScale() function).

  -- ALGLIB --
     Copyright 23.09.2023 by Bochkanov Sergey
*************************************************************************/
void minnlcsetnlc2(minnlcstate &state, const real_1d_array &nl, const real_1d_array &nu, const ae_int_t nnlc, const xparams _xparams = alglib::xdefault);
void minnlcsetnlc2(minnlcstate &state, const real_1d_array &nl, const real_1d_array &nu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function marks specific nonlinear constraint  given by its number  in
[0,NNLC) range as "hard".

A hard nonlinear constraint is a single-sided  nonlinear  constraint  that
must be strictly satisfied during  algorithm  iterations.  The  solver  is
allowed to evaluate objective/constraints at points where hard constraints
are violated or exactly at the bound (e.g. during trial steps or numerical
differentiation), but it will never move to such a point. In other  words,
the solver will always remain in the strict interior with respect to  such
constraints.

Presently, only nonlinear IPM method respects this mark.  SQP  and  others
ignore it.

NOTE: the initial point must be strictly feasible (interior) with  respect
      to all hard constraints. Otherwise an error code -9 will be returned
      after starting optimization.

      It is expected that the initial point is at some distance  from  the
      boundary. Passing a point that is 1E-15 away from violating  a  hard
      constraint is possible, but  the  optimizer  is  likely  to  have  a
      difficult time moving deeper into the feasible set.

NOTE: only single-sided (inequality) constraints can be marked as hard. If
      you mark a constraint having two bounds (equality or range) as hard,
      the solver will fail with the error code -6.

NOTE: the function also allows to remove the mark.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinNLCCreate call.
    CIdx    -   index of a constraint, must be non-negative and less than
                the number of constraints already added (numeration starts
                from zero).
    IsHard  -   if True (default), marks constraint as hard.
                if False, removes the mark.

  -- ALGLIB --
     Copyright 02.02.2026 by Bochkanov Sergey
*************************************************************************/
void minnlcmarkashardnlc(minnlcstate &state, const ae_int_t cidx, const bool ishard, const xparams _xparams = alglib::xdefault);


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
void minnlcmarkaslinearvar(minnlcstate &state, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets specific finite  difference  formula  to  be  used  for
numerical differentiation.

It works only for optimizers created  with  minnlccreatef()  function;  in
other cases it has no effect.

INPUT PARAMETERS:
    State       -   structure previously allocated with MinNLCCreateF call.
    FormulaType -   formula type:
                    * 5 for a 5-point formula (actually, only 4 values per
                      variable  are  used, ones at x+h, x+h/2,  x-h/2  and
                      x-h; the central  one has  zero  multiplier  due  to
                      symmetry). The most precise and the  most  expensive
                      option, chosen by default
                    * 3 for a 3-point formula, which is also  known  as  a
                      symmetric difference quotient (the formula  actually
                      uses only two function values per variable:  at  x+h
                      and x-h). A   good  compromise  for  medium-accuracy
                      setups
                    * 2 for a forward (or backward, depending  on variable
                      bounds)  finite   difference  (f(x+h)-f(x))/h.  This
                      formula has the lowest accuracy. However, it  is  4x
                      faster than the 5-point formula and 2x  faster  than
                      the 3-point one because, in addition to the  central
                      value f(x), it needs only  one  additional  function
                      evaluation per variable.


  -- ALGLIB --
     Copyright 03.12.2024 by Bochkanov Sergey
*************************************************************************/
void minnlcsetnumdiff(minnlcstate &state, const ae_int_t formulatype, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for the optimizer.

This  function allows to set  iterations  limit  and  step-based  stopping
conditions. If you want the solver to stop upon having a small  change  in
the target, use minnlcsetcond3() function.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Eps     -   >=0, specific meaning depends on the algorithm:
                * for NLPIPM - stop when primal/dual/compl errors are less
                  than Eps
                * for SQP-based solvers - stop when the scaled trust region
                  radius is less than Eps
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsX=0 and MaxIts=0 (simultaneously) will lead to automatic
selection of the stopping condition.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetcond(minnlcstate &state, const double eps, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for the optimizer.

This function allows to set three types of stopping conditions:
* iterations limit
* stopping upon performing a short step (depending on the specific  solver
  being used  it may stop as soon as the first short step was made, or
  only after performing several sequential short steps)
* stopping upon having a small change in  the  target  (depending  on  the
  specific solver being used it may stop as soon as the  first  step  with
  small change in the target was made, or only  after  performing  several
  sequential steps)

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsF    -   >=0
                The optimizer will stop as soon as the following condition
                is met:

                    |f_scl(k+1)-f_scl(k)| <= max(|f_scl(k+1)|,|f_scl(k)|,1)

                where f_scl is an internally used by the optimizer rescaled
                target (ALGLIB optimizers usually apply rescaling in order
                to normalize target and constraints).
    Eps     -   >=0, specific meaning depends on the algorithm:
                * for NLPIPM - stop when primal/dual/compl errors are  less
                  than Eps
                * for SQP-based solvers - stop when the scaled trust region
                  radius is less than Eps
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsF, EpsX=0 and MaxIts=0 (simultaneously) will lead to the
automatic selection of the stopping condition.

  -- ALGLIB --
     Copyright 21.09.2023 by Bochkanov Sergey
*************************************************************************/
void minnlcsetcond3(minnlcstate &state, const double epsf, const double eps, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for NLC optimizer.

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
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetscale(minnlcstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length (after scaling of step vector  with
respect to variable scales specified by minnlcsetscale() call).

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0 (default),  if
                you don't want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

NOTE: different solvers employed by MinNLC  optimizer  may  use  different
      norms for the step.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcsetstpmax(minnlcstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells MinNLC unit to use the large-scale augmented Lagrangian
algorithm for nonlinearly constrained optimization.

This  algorithm  is  a  significant  refactoring  of  one  described in "A
Modified Barrier-Augmented  Lagrangian Method for Constrained Minimization
(1999)" by D.GOLDFARB,  R.POLYAK,  K. SCHEINBERG,  I.YUZEFOVICH  with  the
following additions:
* improved sparsity support
* improved handling of large-scale problems with the low rank  LBFGS-based
  sparse preconditioner
* automatic selection of the penalty parameter Rho

AUL solver can be significantly faster than SQP on easy  problems  due  to
cheaper iterations, although it needs more function evaluations. On large-
scale sparse problems one iteration of the AUL solver usually  costs  tens
times less than one iteration of the SQP solver.

However, the SQP solver is more robust than the AUL. In particular, it  is
much better at constraint enforcement and will  never escape feasible area
after constraints were successfully enforced.  It  also  needs  much  less
target function evaluations.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    MaxOuterIts-upper limit on outer iterations count:
                * MaxOuterIts=0 means that the solver  will  automatically
                  choose an upper limit. Recommended value.
                * MaxOuterIts>1 means that the AUL solver will performs at
                  most specified number of outer iterations

  -- ALGLIB --
     Copyright 22.09.2023 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgoaul2(minnlcstate &state, const ae_int_t maxouterits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function selects a legacy solver: an L1 merit function based SQP with
the sparse l-BFGS update.

It is recommended to use either SQP or SQP-BFGS solvers  instead  of  this
one.  These  solvers  use  filters  to  provide  much  faster  and  robust
convergence.
>

  -- ALGLIB --
     Copyright 02.12.2019 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgosl1qp(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function selects a legacy solver: an L1 merit function based SQP with
the dense BFGS update.

It is recommended to use either SQP or SQP-BFGS solvers  instead  of  this
one.  These  solvers  use  filters  to  provide  much  faster  and  robust
convergence.

  -- ALGLIB --
     Copyright 02.12.2019 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgosl1qpbfgs(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function selects large-scale  sparse  filter-based  SQP  solver,  the
most robust solver in ALGLIB, a recommended option.

This algorithm is scalable to problems with millions of variables and  can
efficiently handle sparsity of constraints.

The convergence is proved for the following case:
* function and constraints are continuously differentiable (C1 class)

This algorithm has the following nice properties:
* no parameters to tune
* no convexity requirements for target function or constraints
* the initial point can be infeasible
* the algorithm respects box constraints in all  intermediate  points  (it
  does not even evaluate the target outside of the box constrained area)
* once linear constraints are enforced, the algorithm will not violate them
* no such guarantees can be provided for nonlinear constraints,  but  once
  nonlinear  constraints  are  enforced,  the algorithm will try to respect
  them as much as possible
* numerical differentiation does  not  violate  box  constraints  (although
  general linear and nonlinear ones can be violated during differentiation)

IMPORTANT: the commercial edition of ALGLIB can parallelize  factorization
           phase of this function (this phase takes significant amounts of
           time when solving large-scale problems).  It  can  also  use  a
           performance backend (e.g. Intel PARDISO  or  another  platform-
           specific library) to accelerate sparse factorization.

           Specific speed-up due to parallelism  and  performance  backend
           usage heavily depends on the sparsity pattern  of  constraints.
           For some  problem  types  performance  backends  provide  great
           speed-up. For other ones,  ALGLIB's  own  sparse  factorization
           code is the preferred option.

           See the ALGLIB Reference Manual for more information on how  to
           activate parallelism and backend support.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

===== TRACING SQP SOLVER =================================================

SQP solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'SQP'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
                  It also prints OptGuard  integrity  checker  report when
                  nonsmoothness of target/constraints is suspected.
* 'SQP.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'SQP'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format
* 'SQP.PROBING' - to let algorithm insert additional function  evaluations
                  before line search  in  order  to  build  human-readable
                  chart of the raw  Lagrangian  (~40  additional  function
                  evaluations is performed for  each  line  search).  This
                  symbol  also  implicitly  defines  'SQP'  and  activates
                  OptGuard integrity checker which detects continuity  and
                  smoothness violations. An OptGuard log is printed at the
                  end of the file.

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related   overhead.  Specifying  'SQP.PROBING'  adds   even  larger
overhead due to additional function evaluations being performed.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("SQP,SQP.PROBING,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 02.12.2023 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgosqp(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function selects ORBIT solver, a model-based  derivative-free  solver
for minimization of expensive derivative-free functions.

The ORBIT algorithm by Wild and Shoemaker (2013) is an algorithm that uses
objective function values to build a smooth  RBF  model  (f=r^3)  that  is
minimized over a trust region in order to  identify  step  direction.  The
algorithm saves and reuses function values at all previously known points.

ALGLIB added to the original algorithm the following modifications:
* box, linear and nonlinear constraints
* improved tolerance to noise in the objective/constraints

Its  intended  area  of  application  is  a  low-accuracy  minimization of
expensive objectives with no  gradient  available.  It  is  expected  that
additional overhead of building and minimizing an RBF model is  negligible
when compared with the objective evaluation cost. Iteration overhead grows
as O(N^3), so this solver is recommended for problems with N below 100.

This algorithm has the following nice properties:
* no parameters to tune
* no convexity requirements for target function or constraints
* the initial point can be infeasible
* the algorithm respects box constraints in all  intermediate  points  (it
  does not even evaluate the target outside of the box constrained area)
* once linear and nonlinear constraints are enforced, the  algorithm  will
  try to respect them as much as possible.

When compared with SQP solver, ORBIT:
* is much faster than the finite-difference  based  serial  SQP  at  early
  stages of optimization, being able to achieve 0.1-0.01 relative accuracy
  about 4x-10x faster than SQP solver
* has slower asymptotic convergence on ill-conditioned problems, sometimes
  being unable to reduce error in objective or constraints below  1E-5  in
  a reasonable amount of time
* has  no  obvious  benefits  over  SQP  with  analytic gradient or highly
  parallelized (more than 10 cores) finite-difference SQP

NOTE: whilst technically this algorithm supports callback parallelism,  in
      practice it can't efficiently utilize parallel resources because  it
      issues requests for objective/constraints in  an  inherently  serial
      manner.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

    Rad0    -   initial  sampling   radius  (multiplied  by   per-variable
                scales),  Rad0>=0,  zero  value  means  automatic   radius
                selection.
                An ideal  value  is  large  enough  to  allow  significant
                progress by making a Rad0-sized step, but  not  too  large
                (so  that  initial  linear  model  well  approximates  the
                objective).
                Recommended values: 0.1 or 1  (assuming  properly   chosen
                variable scales.
                The solver can tolerate inappropriately  chosen  Rad0,  at
                the expense of additional function evaluations  needed  to
                adjust it.

    MaxNFEV -   MaxNFEV>=0,  with   zero  value  meaning  no  limit.  This
                parameter allows to control computational budget (measured
                in  function  evaluations).

                It provides somewhat finer control than  MaxIts  parameter
                of minnlcsetcond(), which controls the maximum  amount  of
                iterations performed by the algorithm, with  one iteration
                usually needing more than one function evaluation.

                The  solver  does  not  stop  immediately  after  reaching
                MaxNFEV evaluations, but  will  stop  shortly  after  that
                (usually within N+1 evaluations, often within 1-2).

  -- ALGLIB --
     Copyright 02.10.2024 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgoorbit(minnlcstate &state, const double rad0, const ae_int_t maxnfev, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function selects a SQP solver specialized on low-dimensional problems
- dense filter-based SQP-BFGS solver.

This algorithm uses a dense quadratic model of the  target  and  solves  a
dense QP subproblem at each step. Thus, it has difficulties scaling beyond
several hundreds of variables.  However,  it  usually  needs  the smallest
number of the target evaluations - sometimes  up  to  30%  less  than  the
sparse large-scale filter-based SQP.

The convergence is proved for the following case:
* function and constraints are continuously differentiable (C1 class)

This algorithm has the following nice properties:
* no parameters to tune
* no convexity requirements for target function or constraints
* the initial point can be infeasible
* the algorithm respects box constraints in all  intermediate  points  (it
  does not even evaluate the target outside of the box constrained area)
* once linear constraints are enforced, the algorithm will not violate them
* no such guarantees can be provided for nonlinear constraints,  but  once
  nonlinear  constraints  are  enforced,  the algorithm will try to respect
  them as much as possible
* numerical differentiation does  not  violate  box  constraints  (although
  general linear and nonlinear ones can be violated during differentiation)


IMPORTANT: the commercial edition of ALGLIB can parallelize  factorization
           phase of this function (this phase takes most time when solving
           large-scale problems). It can also use  a  performance  backend
           (e.g.  Intel  MKL  or  another  platform-specific  library)  to
           accelerate dense factorization.

           Dense Cholesky factorization is a well  studied  and  optimized
           algorithm. ALGLIB includes  a  well  optimized  implementation;
           however, using a hardware vendor-provided  performance  library
           usually results in a better performance.

           See the ALGLIB Reference Manual for more information on how  to
           activate parallelism and backend support.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

===== TRACING SQP SOLVER =================================================

SQP solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'SQP'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
                  It also prints OptGuard  integrity  checker  report when
                  nonsmoothness of target/constraints is suspected.
* 'SQP.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'SQP'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format
* 'SQP.PROBING' - to let algorithm insert additional function  evaluations
                  before line search  in  order  to  build  human-readable
                  chart of the raw  Lagrangian  (~40  additional  function
                  evaluations is performed for  each  line  search).  This
                  symbol  also  implicitly  defines  'SQP'  and  activates
                  OptGuard integrity checker which detects continuity  and
                  smoothness violations. An OptGuard log is printed at the
                  end of the file.

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related   overhead.  Specifying  'SQP.PROBING'  adds   even  larger
overhead due to additional function evaluations being performed.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("SQP,SQP.PROBING,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 02.12.2023 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgosqpbfgs(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
In pre-4.08 ALGLIB this function selected the deprecated  version  of  the
large-scale  sparse interior point solver (GIPM2).

Starting from 4.08, it silently redirects users to the new version of  the
solver, NLPIPM.

  -- ALGLIB --
     Copyright 02.02.2025 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgogipm2(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function selects NLPIPM - a large-scale sparse interior point solver.

This  algorithm  is scalable to millions of variables and  can efficiently
handle sparsity of constraints.

When compared to SQP, the following differences can be noted:
* NLPIPM has lower iteration overhead than SQP
* NLPIPM  violates all non-box constraints  (linear,  nonlinear,  equality
  and inequality ones) that were not explicitly marked as 'hard'  until it
  finally converges. Contrary to that,  SQP  respects  linear  constraints
  after initial enforcement, and tends to closely follow nonlinear ones.
* as all interior point  methods,  NLPIPM  is  bad  at hot starts. Even if
  started from solution, it will perform 20-60 iterations (first, it  will
  move  away from the solution to find a centered initial point,  then  it
  will converge back along the central path).

The shorty summary is that SQP wins when starting close to  the  solution,
and NLPIPM  tends to win on large-scale  sparse problems where  constraint
factorization adds significant linear algebra overhead.

The convergence is proved for the following case:
* function and constraints are continuously differentiable (C1 class)

This algorithm has the following nice properties:
* no parameters to tune, except for quasi-Newton  memory length  (defaults
  are usually ok, unless you solve a pretty hard problem)
* no convexity requirements for target function or constraints
* the initial point can be infeasible
* the algorithm respects box constraints in all  intermediate  points  (it
  does not even evaluate the target outside of the box constrained area)
* nonlinear inequality constraints that are marked as hard will be exactly
  enforced in all intermediate points (the solver may occasionally evaluate
  a point where the constraint is violated, but it will not step into such
  a point).
* numerical differentiation does  not  violate  box  constraints  (although
  general linear and nonlinear ones can be violated during differentiation)

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    MemLen  -   >=0, memory length for quasi-Newton update (similar to
                LBFGS memory parameter):
                * 0 means default value which may change in future versions;
                  presently it is 8.
                * 8 is a good default value for moderately nonlinear tasks
                * 32 is a good value for problems with more nonlinear
                  objective/constraints
                * values larger than the variables count N  are  possible;
                  these will be silently truncated to N.

===== TRACING NLPIPM SOLVER ==============================================

NLPIPM   solver  supports  advanced  tracing  capabilities.  You can trace
algorithm output by specifying following trace symbols  (case-insensitive)
by means of trace_file() call:
* 'NLPIPM'      - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related   overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("NLPIPM,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 02.11.2025 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgonlpipm(minnlcstate &state, const ae_int_t memlen, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinNLCOptimize().

NOTE: algorithm passes two parameters to rep() callback  -  current  point
      and penalized function value at current point. Important -  function
      value which is returned is NOT function being minimized. It  is  sum
      of the value of the function being minimized - and penalty term.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcsetxrep(minnlcstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minnlciteration(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    fvec    -   callback which calculates function vector fi[]
                at given point x
    jac     -   callback which calculates function vector fi[]
                and Jacobian jac at given point x
    sjac    -   callback which calculates function vector fi[]
                and sparse Jacobian sjac at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


CALLBACK PARALLELISM

The MINNLC optimizer supports parallel parallel  numerical differentiation
('callback parallelism'). This feature, which  is  present  in  commercial
ALGLIB  editions,  greatly   accelerates   optimization   with   numerical
differentiation of an expensive target functions.

Callback parallelism is usually  beneficial  when  computing  a  numerical
gradient requires more than several  milliseconds.  In this case  the  job
of computing individual gradient components can be split between  multiple
threads. Even inexpensive targets can benefit  from  parallelism,  if  you
have many variables.

ALGLIB Reference Manual, 'Working with commercial  version' section, tells
how to activate callback parallelism for your programming language.

NOTES:

1. This function has two different implementations:
   * one which accepts objective/constraints values and their gradients
   * and one which accepts only objective/constraints values

   The former function should be called  when  using  solvers  relying  on
   analytic gradients (SQP, NLPIPM, etc). The latter one  is  intended for
   derivative-free NLP solvers like ORBIT and, additionally, for numerical
   differentiation based versions of SQP/NLPIPM/etc.

   Be careful to choose variant of MinNLCOptimize()  which  corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinNLCOptimize()   and  specific
   function used to create optimizer.


                                            |  PASSED TO MinNLCOptimize()
                                            |              |
       PROBLEM SETUP                        |  values only |  values
                                            |              | and gradients
   -----------------------------------------------------------------------
   MinNLCCreate()+SQP/NLPIPM                |      FAILS          +
   MinNLCCreate()+ORBIT                     |        +          FAILS
   MinNLCCreateF()+SQP/NLPIPM               |        +          FAILS
   MinNLCCreateF()+ORBIT                    |      FAILS        FAILS

   Here "FAILS" denotes inappropriate combinations  of  optimizer creation
   function  and  MinNLCOptimize()  version.   Any attempts   to  use such
   combination will lead to exception. Either  you  did  not pass gradient
   when it WAS needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 06.05.2025 by Bochkanov Sergey


*************************************************************************/
void minnlcoptimize(minnlcstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minnlcoptimize(minnlcstate &state,
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minnlcoptimize(minnlcstate &state,
    void (*sjac)(const real_1d_array &x, real_1d_array &fi, sparsematrix &s, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient/Jacobian.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target  function  (constraints)  at  the
initial point (note: future versions may also perform check  at  the final
point) and compares numerical gradient/Jacobian with analytic one provided
by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which stores both gradients/Jacobians, and specific components highlighted
as suspicious by the OptGuard.

The primary OptGuard report can be retrieved with minnlcoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with minnlcsetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcoptguardgradient(minnlcstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0) and/or constraints
b) nonsmooth     target function (non-C1) and/or constraints

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.

      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.

      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.

      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.
                      This kind of monitoring does not work well with  SQP
                      because SQP solver needs just 1-2 function evaluations
                      per step, which is not enough for OptGuard  to  make
                      any conclusions.

=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minnlcoptguardsmoothness(minnlcstate &state, const ae_int_t level, const xparams _xparams = alglib::xdefault);
void minnlcoptguardsmoothness(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* minnlcoptguardgradient() for gradient verification
* minnlcoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradfidx for specific function (Jacobian row) suspected
  * rep.badgradvidx for specific variable (Jacobian column) suspected
  * rep.badgradxbase, a point where gradient/Jacobian is tested
  * rep.badgraduser, user-provided gradient/Jacobian
  * rep.badgradnum, reference gradient/Jacobian obtained via numerical
    differentiation
* rep.nonc0suspected, and additionally:
  * rep.nonc0fidx - an index of specific function violating C0 continuity
* rep.nonc1suspected, and additionally
  * rep.nonc1fidx - an index of specific function violating C1 continuity
Here function index 0 means  target function, index 1  or  higher  denotes
nonlinear constraints.

=== ADDITIONAL REPORTS/LOGS ==============================================

Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* minnlcoptguardnonc1test0results()
* minnlcoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.

      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.

      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minnlcoptguardresults(minnlcstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minnlcoptguardnonc1test0results(const minnlcstate &state, optguardnonc1test0report &strrep, optguardnonc1test0report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minnlcoptguardnonc1test1results(minnlcstate &state, optguardnonc1test1report &strrep, optguardnonc1test1report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
MinNLC results:  the  solution  found,  completion  codes  and  additional
information.

If you activated OptGuard integrity checking functionality and want to get
OptGuard report, it can be retrieved with:
* minnlcoptguardresults() - for a primary report about (a) suspected C0/C1
  continuity violations and (b) errors in the analytic gradient.
* minnlcoptguardnonc1test0results() - for C1 continuity violation test #0,
  detailed line search log
* minnlcoptguardnonc1test1results() - for C1 continuity violation test #1,
  detailed line search log

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report, contains information about completion
                code, constraint violation at the solution and so on.

                rep.f contains objective value at the solution.

                You   should   check   rep.terminationtype  in  order   to
                distinguish successful termination from unsuccessful one:

                === FAILURE CODES ===
                * -9    initial point violates hard constraints,  stopping
                        immediately
                * -8    internal  integrity control  detected  infinite or
                        NAN  values  in  function/gradient,  recovery  was
                        impossible. Abnormal termination signalled.
                * -6    one of hard constraints has both  bounds  present;
                        this option is not supported.
                * -4    the problem is likely  to  be unbounded; presently
                        only NLPIPM solver reports  this  code.   For  NLP
                        problems (not LP/QP) it is generally impossible to
                        provide  an  unboundedness  certificate,  so  only
                        heuristics  are  possible,  based on growth of |x|
                        and decrease of f compared to |f(x0)|. It  is  the
                        only failure code than returns something meaningful
                        in X.
                * -3    box  constraints are infeasible.
                        Note: infeasibility of  non-box  constraints  does
                              NOT trigger emergency completion;  you  have
                              to examine rep.bcerr/rep.lcerr/rep.nlcerr to
                              detect possibly inconsistent constraints.

                When a failure code, different from -4, is  returned,  the
                X is filled by NANs. When failure code is  equal  to -4, a
                point where unboundedness suspicions  were  triggered,  is
                returned.

                === SUCCESS CODES ===
                *  1   small objective decrease indicates convergence
                *  2   scaled step is no more than EpsX.
                *  5   MaxIts steps were taken.
                *  7   stopping conditions are  too  stringent,  returning
                       the best point so far
                *  8   user   requested    algorithm    termination    via
                       minnlcrequesttermination(), last accepted point  is
                       returned.

                === ADDITIONAL CODES ===
                * +800      if   during  algorithm  execution  the  solver
                            encountered NAN/INF values in  the  target  or
                            constraints but managed to recover by reducing
                            trust region radius, the  solver  returns  one
                            of SUCCESS codes but adds +800 to the code.

                Some  solvers  (as  of ALGLIB 4.08, SQP and NLPIPM) return
                Lagrange multipliers in rep.lagbc/lagbcnz,  laglc,  lagnlc
                fields.

                More information about fields of this  structure  can  be
                found in the comments on the minnlcreport datatype.

  -- ALGLIB --
     Copyright 18.01.2024 by Bochkanov Sergey
*************************************************************************/
void minnlcresults(const minnlcstate &state, real_1d_array &x, minnlcreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
NLC results

Buffered implementation of MinNLCResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcresultsbuf(const minnlcstate &state, real_1d_array &x, minnlcreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcrequesttermination(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinNLCCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcrestartfrom(minnlcstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_QQPSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_QPDENSEAULSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MONBI) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINMO) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                    MULTI-OBJECTIVE  OPTIMIZATION

DESCRIPTION:

The  solver  minimizes an M-dimensional vector function F(x) of N arguments
subject to any combination of:
* box constraints
* two-sided linear equality/inequality constraints AL<=A*x<=AU, where some
  of AL/AU can be infinite (i.e. missing)
* two-sided nonlinear equality/inequality constraints NL<=C(x)<=NU,  where
  some of NL/NU can be infinite (i.e. missing)

REQUIREMENTS:
* F(), C() are continuously differentiable on the feasible set and on  its
  neighborhood

USAGE:

1. User initializes algorithm state using either:
   * minmocreate()  to perform optimization with user-supplied Jacobian
   * minmocreatef() to perform optimization with numerical differentiation

2. User chooses which multi-objective solver to use. At the present moment
   only NBI (Normal Boundary Intersection) solver is implemented, which is
   activated by calling minmosetalgonbi().

3. User adds boundary and/or linear and/or nonlinear constraints by  means
   of calling one of the following functions:
   a) minmosetbc() for boundary constraints
   b) minmosetlc2()      for two-sided sparse linear constraints;
      minmosetlc2dense() for two-sided dense  linear constraints;
      minmosetlc2mixed() for two-sided mixed sparse/dense constraints
   c) minmosetnlc2()     for two-sided nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.

4. User sets scale of the variables with minmosetscale() function.  It  is
   VERY important to set  scale  of  the  variables,  because  nonlinearly
   constrained problems are hard to solve when variables are badly scaled.

5. User sets  stopping  conditions  with  minmosetcond().

6. Finally, user calls minmooptimize()   function  which  takes  algorithm
   state  and  pointers  (delegate, etc.) to the callback functions  which
   calculate F/C

7. User calls  minmoresults()  to  get the solution

8. Optionally user may call minmorestartfrom() to solve  another   problem
   with same M,N but another starting point. minmorestartfrom() allows  to
   reuse an already initialized optimizer structure.


INPUT PARAMETERS:
    N       -   variables count, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from the size of X
    M       -   objectives count, M>0.
                M=1 is possible, although makes little sense - it is better
                to use MinNLC directly.
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to reinforce feasibility during  initial  stages  of  the
                  optimization

OUTPUT PARAMETERS:
    State   -   structure that stores algorithm state

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmocreate(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minmostate &state, const xparams _xparams = alglib::xdefault);
void minmocreate(const ae_int_t m, const real_1d_array &x, minmostate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine is a finite  difference variant of minmocreate().  It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this  function
only. We recommend to read comments on minmocreate() too.

INPUT PARAMETERS:
    N       -   variables count, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from the size of X
    M       -   objectives count, M>0.
                M=1 is possible, although makes little sense - it is better
                to use MinNLC directly.
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to reinforce feasibility during  initial  stages  of  the
                  optimization
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure that stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is a scaling vector which can be set by minmosetscale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step means too large TRUNCATION errors,  whilst  too  small  step
   means too large NUMERICAL errors.
   1.0E-4 can be good value to start from for a unit-scaled problem.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less   robust   and  precise.  Imprecise  gradient  may  slow  down
   convergence, especially on highly nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmocreatef(const ae_int_t n, const ae_int_t m, const real_1d_array &x, const double diffstep, minmostate &state, const xparams _xparams = alglib::xdefault);
void minmocreatef(const ae_int_t m, const real_1d_array &x, const double diffstep, minmostate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Use the NBI (Normal Boundary Intersection)  algorithm  for  multiobjective
optimization.

NBI is a simple yet powerful multiobjective  optimization  algorithm  that
has the following attractive properties:
* it generates nearly uniformly distributed Pareto points
* it is applicable to problems with more than 2 objectives
* it naturally supports a mix of box, linear and nonlinear constraints
* it is less sensitive to the bad scaling of the targets

The only drawback of the algorithm is that for more than 2  objectives  it
can miss some small parts of the Pareto front that are  located  near  its
boundaries.

INPUT PARAMETERS:
    State       -   structure which stores algorithm state
    FrontSize   -   desired Pareto front size, FrontSize>=M,
                    where M is an objectives count
    PolishSolutions-whether additional solution improving phase is needed
                    or not:
                    * if False, the original NBI as formulated  by Das and
                      Dennis is used. It quickly produces  good solutions,
                      but these solutions can be suboptimal (usually within
                      0.1% of the optimal values).
                      The reason is that the original NBI formulation does
                      not account for  degeneracies that allow significant
                      progress for one objective with no deterioration for
                      other objectives.
                    * if True,  the  original  NBI  is  followed  by   the
                      additional solution  polishing  phase.  This  solver
                      mode is several times slower than the original  NBI,
                      but produces better solutions.

  -- ALGLIB --
     Copyright 20.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetalgonbi(minmostate &state, const ae_int_t frontsize, const bool polishsolutions, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints for the MO optimizer.

Boundary constraints are inactive by  default  (after  initial  creation).
They are preserved after algorithm restart with MinMORestartFrom().

You may combine boundary constraints with  general  linear ones - and with
nonlinear ones! Boundary constraints are  handled  more  efficiently  than
other types.  Thus,  if  your  problem  has  mixed  constraints,  you  may
explicitly specify some of them as boundary and save some time/space.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                a very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                a very large number or +INF.

NOTE 1:  it is possible to specify  BndL[i]=BndU[i].  In  this  case  I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetbc(minmostate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU with dense
constraint matrix A.

NOTE: knowing  that  constraint  matrix  is dense may help some MO solvers
      to utilize efficient dense Level 3  BLAS  for  dense  parts  of  the
      problem. If your problem has both dense and sparse constraints,  you
      can use minmosetlc2mixed() function.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
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
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetlc2dense(minmostate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minmosetlc2dense(minmostate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
sparse constraining matrix A. Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
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
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minmosetlc2(minmostate &state, const sparsematrix &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
mixed constraining matrix A including sparse part (first SparseK rows) and
dense part (last DenseK rows). Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

This function may be useful if constraint matrix includes large number  of
both types of rows - dense and sparse. If you have just a few sparse rows,
you  may  represent  them  in  dense  format  without losing  performance.
Similarly, if you have just a few dense rows,  you  can  store them in the
sparse format with almost same performance.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
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
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minmosetlc2mixed(minmostate &state, const sparsematrix &sparsea, const ae_int_t ksparse, const real_2d_array &densea, const ae_int_t kdense, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear  constraint  AL<=A*x<=AU  to  dense
constraints list.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
    A       -   linear constraint coefficient, array[N], right side is NOT
                included.
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint Ai*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint Ai*x<=AU
                * AU=+INF  => one-sided constraint AL<=Ai*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minmoaddlc2dense(minmostate &state, const real_1d_array &a, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of sparse constraints.

Constraint is passed in the compressed  format:  as  a  list  of  non-zero
entries of the coefficient vector A. Such approach is more efficient  than
the dense storage for highly sparse constraint vectors.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
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
void minmoaddlc2(minmostate &state, const integer_1d_array &idxa, const real_1d_array &vala, const ae_int_t nnz, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present sparse constraints.

Constraint vector A is  passed  as  a  dense  array  which  is  internally
sparsified by this function.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
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
void minmoaddlc2sparsefromdense(minmostate &state, const real_1d_array &da, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided nonlinear constraints for MinMO optimizer.

In fact, this function sets  only  constraints  COUNT  and  their  BOUNDS.
Constraints  themselves  (constraint  functions)   are   passed   to   the
MinMOOptimize() method as callbacks.

MinMOOptimize() method accepts a user-defined vector function F[]  and its
Jacobian J[], where:
* first M components of F[]  and  first  M rows  of  J[]   correspond   to
  multiple objectives
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
    State   -   structure previously allocated with MinMOCreate call.
    NL      -   array[NNLC], lower bounds, can contain -INF
    NU      -   array[NNLC], lower bounds, can contain +INF
    NNLC    -   constraints count, NNLC>=0

NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
        possible that the algorithm will evaluate the function  outside of
        the feasible area!

NOTE 2: algorithm scales variables  according  to the scale  specified by
        MinMOSetScale()  function,  so  it can handle problems with badly
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
        a scale set by MinMOSetScale() function).

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetnlc2(minmostate &state, const real_1d_array &nl, const real_1d_array &nu, const ae_int_t nnlc, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for inner iterations of the optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinMOSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsX=0 and MaxIts=0 (simultaneously) will lead to an automatic
selection of the stopping condition.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetcond(minmostate &state, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for the MO optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetscale(minmostate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting of the Pareto front points.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback  function  (if  it
was provided to MinMOOptimize) every time we find a Pareto front point.

NOTE: according to the communication protocol used by ALGLIB,  the  solver
      passes two parameters to the rep() callback - a current point and  a
      target value at the current point.
      However, because  we solve a  multi-objective  problem,  the  target
      parameter is not used and set to zero.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetxrep(minmostate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minmoiteration(minmostate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    fvec    -   callback which calculates function vector fi[]
                at given point x
    jac     -   callback which calculates function vector fi[]
                and Jacobian jac at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied Jacobian, and one which uses  only  function
   vector and numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   you should choose appropriate variant of MinMOOptimize() -  one   which
   needs function vector AND Jacobian or one which needs ONLY function.

   Be careful to choose variant of MinMOOptimize()  which  corresponds  to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinMOOptimize()   and   specific
   function used to create optimizer.


                     |         USER PASSED TO MinMOOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinMOCreateF()    |     works               FAILS
   MinMOCreate()     |     FAILS               works

   Here "FAILS" denotes inappropriate combinations  of  optimizer creation
   function  and  MinMOOptimize()  version.   Attemps   to    use     such
   combination will lead to exception. Either  you  did  not pass gradient
   when it WAS needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey


*************************************************************************/
void minmooptimize(minmostate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minmooptimize(minmostate &state,
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
MinMO results:  the  solution  found,  completion  codes  and   additional
information.

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    ParetoFront-array[FrontSize,N+M], approximate Pareto front.
                Its columns have the following structure:
                * first N columns are variable values
                * next  M columns are objectives at these points
                Its rows have the following structure:
                * first M rows contain solutions to single-objective tasks
                  with I-th row storing result for  I-th  objective  being
                  minimized ignoring other ones.
                  Thus, ParetoFront[I,N+I] for  0<=I<M  stores  so  called
                  'ideal objective vector'.
                * subsequent FrontSize-M rows  store  variables/objectives
                  at  various  randomly  and  nearly   uniformly   sampled
                  locations of the Pareto front.

    FrontSize-  front size, >=0.
                * no larger than the number passed to setalgo()
                * for  a  single-objective  task,  FrontSize=1  is  ALWAYS
                  returned, no matter what was specified during setalgo()
                  call.
                * if  the   solver   was   prematurely   terminated   with
                  minnorequesttermination(), an  incomplete  Pareto  front
                  will be returned (it may even have less than M rows)
                * if a  failure (negative completion code) was   signaled,
                  FrontSize=0 will be returned

    Rep     -   optimization report, contains information about completion
                code, constraint violation at the solution and so on.

                You   should   check   rep.terminationtype  in  order   to
                distinguish successful termination from unsuccessful one:

                === FAILURE CODES ===
                * -8    internal  integrity control  detected  infinite or
                        NAN   values    in   function/gradient.   Abnormal
                        termination signalled.
                * -3    constraint bounds are  infeasible,  i.e.  we  have
                        box/linear/nonlinear constraint  with  two  bounds
                        present, and a lower one being  greater  than  the
                        upper one.
                        Note: less obvious infeasibilities of  constraints
                              do NOT  trigger  emergency  completion;  you
                              have to examine rep.bcerr/rep.lcerr/rep.nlcerr
                              to detect possibly inconsistent constraints.

                === SUCCESS CODES ===
                *  2   scaled step is no more than EpsX.
                *  5   MaxIts steps were taken.
                *  8   user   requested    algorithm    termination    via
                       minmorequesttermination(), last accepted point   is
                       returned.

                More information about fields of this  structure  can  be
                found in the comments on minmoreport datatype.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmoresults(const minmostate &state, real_2d_array &paretofront, ae_int_t &frontsize, minmoreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine  submits  request  for  the  termination  of  the  running
optimizer.

It should be called from the user-supplied callback when user decides that
it is time to "smoothly" terminate optimization process, or from some other
thread. As a result, optimizer stops  at  the  state  which  was  "current
accepted" when termination request was submitted and returns error code  8
(successful termination).

Usually it results in an incomplete Pareto front being returned.

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmorequesttermination(minmostate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine restarts algorithm from the new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinMOCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmorestartfrom(minmostate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINBC) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                     BOX CONSTRAINED OPTIMIZATION
          WITH FAST ACTIVATION OF MULTIPLE BOX CONSTRAINTS

DESCRIPTION:
The  subroutine  minimizes  function   F(x) of N arguments subject  to box
constraints (with some of box constraints actually being equality ones).

This optimizer uses algorithm similar to that of MinBLEIC (optimizer  with
general linear constraints), but presence of box-only  constraints  allows
us to use faster constraint activation strategies. On large-scale problems,
with multiple constraints active at the solution, this  optimizer  can  be
several times faster than BLEIC.

REQUIREMENTS:
* user must provide function value and gradient
* starting point X0 must be feasible or
  not too far away from the feasible set
* grad(f) must be Lipschitz continuous on a level set:
  L = { x : f(x)<=f(x0) }
* function must be defined everywhere on the feasible set F

USAGE:

Constrained optimization if far more complex than the unconstrained one.
Here we give very brief outline of the BC optimizer. We strongly recommend
you to read examples in the ALGLIB Reference Manual and to read ALGLIB User Guide
on optimization, which is available at http://www.alglib.net/optimization/

1. User initializes algorithm state with MinBCCreate() call

2. USer adds box constraints by calling MinBCSetBC() function.

3. User sets stopping conditions with MinBCSetCond().

4. User calls MinBCOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.

5. User calls MinBCResults() to get solution

6. Optionally user may call MinBCRestartFrom() to solve another problem
   with same N but another starting point.
   MinBCRestartFrom() allows to reuse already initialized structure.


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
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbccreate(const ae_int_t n, const real_1d_array &x, minbcstate &state, const xparams _xparams = alglib::xdefault);
void minbccreate(const real_1d_array &x, minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
The subroutine is finite difference variant of MinBCCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinBCCreate() in  order  to  get
more information about creation of BC optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinBCSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust and precise. CG needs exact gradient values. Imprecise
   gradient may slow  down  convergence, especially  on  highly  nonlinear
   problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minbccreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, minbcstate &state, const xparams _xparams = alglib::xdefault);
void minbccreatef(const real_1d_array &x, const double diffstep, minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints for BC optimizer.

Boundary constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBCRestartFrom().

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by  bound  constraints,
  even  when  numerical  differentiation is used (algorithm adjusts  nodes
  according to boundary constraints)

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetbc(minbcstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for the optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinBCSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinBCSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection.

NOTE: when SetCond() called with non-zero MaxIts, BC solver may perform
      slightly more than MaxIts iterations. I.e., MaxIts  sets  non-strict
      limit on iterations count.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetcond(minbcstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for BC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  BC  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinBCSetPrec...()
functions.

There is a special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minbcsetscale(minbcstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetprecdefault(minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE 1: D[i] should be positive. Exception will be thrown otherwise.

NOTE 2: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetprecdiag(minbcstate &state, const real_1d_array &d, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinBCSetScale()
call  (before  or after MinBCSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetprecscale(minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinBCOptimize().

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetxrep(minbcstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  lead   to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetstpmax(minbcstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minbciteration(minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    func    -   callback which calculates function (or merit function)
                value func at given point x
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL

NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied gradient,  and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either  MinBCCreate() for analytical gradient or  MinBCCreateF()
   for numerical differentiation) you should choose appropriate variant of
   MinBCOptimize() - one  which  accepts  function  AND gradient or one
   which accepts function ONLY.

   Be careful to choose variant of MinBCOptimize() which corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinBCOptimize()  and specific
   function used to create optimizer.


                     |         USER PASSED TO MinBCOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinBCCreateF()    |     works               FAILS
   MinBCCreate()     |     FAILS               works

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function  and  MinBCOptimize()  version.   Attemps   to   use   such
   combination (for  example,  to  create optimizer with MinBCCreateF()
   and  to  pass  gradient  information  to  MinCGOptimize()) will lead to
   exception being thrown. Either  you  did  not pass gradient when it WAS
   needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey


*************************************************************************/
void minbcoptimize(minbcstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minbcoptimize(minbcstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function  at  the  initial  point
(note: future versions may also perform check  at  the  final  point)  and
compares numerical gradient with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which  stores  both  gradients  and  specific  components  highlighted  as
suspicious by the OptGuard.

The primary OptGuard report can be retrieved with minbcoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with minbcsetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardgradient(minbcstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0)
b) nonsmooth     target function (non-C1)

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.

      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.

      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.

      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.

=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardsmoothness(minbcstate &state, const ae_int_t level, const xparams _xparams = alglib::xdefault);
void minbcoptguardsmoothness(minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* minbcoptguardgradient() for gradient verification
* minbcoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradvidx for specific variable (gradient element) suspected
  * rep.badgradxbase, a point where gradient is tested
  * rep.badgraduser, user-provided gradient  (stored  as  2D  matrix  with
    single row in order to make  report  structure  compatible  with  more
    complex optimizers like MinNLC or MinLM)
  * rep.badgradnum,   reference    gradient    obtained    via   numerical
    differentiation (stored as  2D matrix with single row in order to make
    report structure compatible with more complex optimizers  like  MinNLC
    or MinLM)
* rep.nonc0suspected
* rep.nonc1suspected

=== ADDITIONAL REPORTS/LOGS ==============================================

Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* minbcoptguardnonc1test0results()
* minbcoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.

      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.

      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardresults(minbcstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardnonc1test0results(const minbcstate &state, optguardnonc1test0report &strrep, optguardnonc1test0report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardnonc1test1results(minbcstate &state, optguardnonc1test1report &strrep, optguardnonc1test1report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
BC results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report. You should check Rep.TerminationType
                in  order  to  distinguish  successful  termination  from
                unsuccessful one:
                * -8    internal integrity control  detected  infinite or
                        NAN   values   in   function/gradient.   Abnormal
                        termination signalled.
                * -3   inconsistent constraints.
                *  1   relative function improvement is no more than EpsF.
                *  2   scaled step is no more than EpsX.
                *  4   scaled gradient norm is no more than EpsG.
                *  5   MaxIts steps was taken
                *  8   terminated by user who called minbcrequesttermination().
                       X contains point which was "current accepted"  when
                       termination request was submitted.
                More information about fields of this  structure  can  be
                found in the comments on MinBCReport datatype.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcresults(const minbcstate &state, real_1d_array &x, minbcreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
BC results

Buffered implementation of MinBCResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcresultsbuf(const minbcstate &state, real_1d_array &x, minbcreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBCCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcrestartfrom(minbcstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void minbcrequesttermination(minbcstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINNS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                  NONSMOOTH NONCONVEX OPTIMIZATION
            SUBJECT TO BOX/LINEAR/NONLINEAR-NONSMOOTH CONSTRAINTS

DESCRIPTION:

The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints
* nonlinear equality constraints Gi(x)=0
* nonlinear inequality constraints Hi(x)<=0

IMPORTANT: see MinNSSetAlgoAGS for important  information  on  performance
           restrictions of AGS solver.

REQUIREMENTS:
* starting point X0 must be feasible or not too far away from the feasible
  set
* F(), G(), H() are continuous, locally Lipschitz  and  continuously  (but
  not necessarily twice) differentiable in an open dense  subset  of  R^N.
  Functions F(), G() and H() may be nonsmooth and non-convex.
  Informally speaking, it means  that  functions  are  composed  of  large
  differentiable "patches" with nonsmoothness having  place  only  at  the
  boundaries between these "patches".
  Most real-life nonsmooth  functions  satisfy  these  requirements.  Say,
  anything which involves finite number of abs(), min() and max() is  very
  likely to pass the test.
  Say, it is possible to optimize anything of the following:
  * f=abs(x0)+2*abs(x1)
  * f=max(x0,x1)
  * f=sin(max(x0,x1)+abs(x2))
* for nonlinearly constrained problems: F()  must  be  bounded from  below
  without nonlinear constraints (this requirement is due to the fact that,
  contrary to box and linear constraints, nonlinear ones  require  special
  handling).
* user must provide function value and gradient for F(), H(), G()  at  all
  points where function/gradient can be calculated. If optimizer  requires
  value exactly at the boundary between "patches" (say, at x=0 for f=abs(x)),
  where gradient is not defined, user may resolve tie arbitrarily (in  our
  case - return +1 or -1 at its discretion).
* NS solver supports numerical differentiation, i.e. it may  differentiate
  your function for you,  but  it  results  in  2N  increase  of  function
  evaluations. Not recommended unless you solve really small problems. See
  minnscreatef() for more information on this functionality.

USAGE:

1. User initializes algorithm state with MinNSCreate() call  and   chooses
   what NLC solver to use. There is some solver which is used by  default,
   with default settings, but you should NOT rely on  default  choice.  It
   may change in future releases of ALGLIB without notice, and no one  can
   guarantee that new solver will be  able  to  solve  your  problem  with
   default settings.

   From the other side, if you choose solver explicitly, you can be pretty
   sure that it will work with new ALGLIB releases.

   In the current release following solvers can be used:
   * AGS solver (activated with MinNSSetAlgoAGS() function)

2. User adds boundary and/or linear and/or nonlinear constraints by  means
   of calling one of the following functions:
   a) MinNSSetBC() for boundary constraints
   b) MinNSSetLC() for linear constraints
   c) MinNSSetNLC() for nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.

3. User sets scale of the variables with MinNSSetScale() function. It   is
   VERY important to set  scale  of  the  variables,  because  nonlinearly
   constrained problems are hard to solve when variables are badly scaled.

4. User sets stopping conditions with MinNSSetCond().

5. Finally, user calls MinNSOptimize()  function  which  takes   algorithm
   state and pointer (delegate, etc) to callback function which calculates
   F/G/H.

7. User calls MinNSResults() to get solution

8. Optionally user may call MinNSRestartFrom() to solve   another  problem
   with same N but another starting point. MinNSRestartFrom()  allows   to
   reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

NOTE: minnscreatef() function may be used if  you  do  not  have  analytic
      gradient.   This   function  creates  solver  which  uses  numerical
      differentiation with user-specified step.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnscreate(const ae_int_t n, const real_1d_array &x, minnsstate &state, const xparams _xparams = alglib::xdefault);
void minnscreate(const real_1d_array &x, minnsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Version of minnscreatef() which uses numerical differentiation. I.e.,  you
do not have to calculate derivatives yourself. However, this version needs
2N times more function evaluations.

2-point differentiation formula is  used,  because  more  precise  4-point
formula is unstable when used on non-smooth functions.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.
    DiffStep-   differentiation  step,  DiffStep>0.   Algorithm   performs
                numerical differentiation  with  step  for  I-th  variable
                being equal to DiffStep*S[I] (here S[] is a  scale vector,
                set by minnssetscale() function).
                Do not use  too  small  steps,  because  it  may  lead  to
                catastrophic cancellation during intermediate calculations.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnscreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, minnsstate &state, const xparams _xparams = alglib::xdefault);
void minnscreatef(const real_1d_array &x, const double diffstep, minnsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints.

Boundary constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with minnsrestartfrom().

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: AGS solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by  bound  constraints,
  even  when  numerical  differentiation is used (algorithm adjusts  nodes
  according to boundary constraints)

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetbc(minnsstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets linear constraints.

Linear constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with minnsrestartfrom().

INPUT PARAMETERS:
    State   -   structure previously allocated with minnscreate() call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE: linear (non-bound) constraints are satisfied only approximately:

* there always exists some minor violation (about current sampling  radius
  in magnitude during optimization, about EpsX in the solution) due to use
  of penalty method to handle constraints.
* numerical differentiation, if used, may  lead  to  function  evaluations
  outside  of the feasible  area,   because   algorithm  does  NOT  change
  numerical differentiation formula according to linear constraints.

If you want constraints to be  satisfied  exactly, try to reformulate your
problem  in  such  manner  that  all constraints will become boundary ones
(this kind of constraints is always satisfied exactly, both in  the  final
solution and in all intermediate points).

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetlc(minnsstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minnssetlc(minnsstate &state, const real_2d_array &c, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets nonlinear constraints.

In fact, this function sets NUMBER of nonlinear  constraints.  Constraints
itself (constraint functions) are passed to minnsoptimize() method.   This
method requires user-defined vector function F[]  and  its  Jacobian  J[],
where:
* first component of F[] and first row  of  Jacobian  J[]  correspond   to
  function being minimized
* next NLEC components of F[] (and rows  of  J)  correspond  to  nonlinear
  equality constraints G_i(x)=0
* next NLIC components of F[] (and rows  of  J)  correspond  to  nonlinear
  inequality constraints H_i(x)<=0

NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
      your problem has mixed constraints, you  may explicitly specify some
      of them as linear ones. It may help optimizer to  handle  them  more
      efficiently.

INPUT PARAMETERS:
    State   -   structure previously allocated with minnscreate() call.
    NLEC    -   number of Non-Linear Equality Constraints (NLEC), >=0
    NLIC    -   number of Non-Linear Inquality Constraints (NLIC), >=0

NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
        possible   that  algorithm  will  evaluate  function  outside   of
        the feasible area!

NOTE 2: algorithm scales variables  according  to   scale   specified   by
        minnssetscale()  function,  so  it can handle problems with  badly
        scaled variables (as long as we KNOW their scales).

        However,  there  is  no  way  to  automatically  scale   nonlinear
        constraints Gi(x) and Hi(x). Inappropriate scaling  of  Gi/Hi  may
        ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
        is NOT same as solving it with constraint "0.001*G0(x)=0".

        It  means  that  YOU  are  the  one who is responsible for correct
        scaling of nonlinear constraints Gi(x) and Hi(x). We recommend you
        to scale nonlinear constraints in such way that I-th component  of
        dG/dX (or dH/dx) has approximately unit  magnitude  (for  problems
        with unit scale)  or  has  magnitude approximately equal to 1/S[i]
        (where S is a scale set by minnssetscale() function).

NOTE 3: nonlinear constraints are always hard to handle,  no  matter  what
        algorithm you try to use. Even basic box/linear constraints modify
        function  curvature   by  adding   valleys  and  ridges.  However,
        nonlinear constraints add valleys which are very  hard  to  follow
        due to their "curved" nature.

        It means that optimization with single nonlinear constraint may be
        significantly slower than optimization with multiple linear  ones.
        It is normal situation, and we recommend you to  carefully  choose
        Rho parameter of minnssetalgoags(), because too  large  value  may
        slow down convergence.


  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetnlc(minnsstate &state, const ae_int_t nlec, const ae_int_t nlic, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for iterations of optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The AGS solver finishes its work if  on  k+1-th  iteration
                sampling radius decreases below EpsX.
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsX=0  and  MaxIts=0  (simultaneously)  will  lead  to  automatic
stopping criterion selection. We do not recommend you to rely  on  default
choice in production code.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetcond(minnsstate &state, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for NLC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetscale(minnsstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells MinNS unit to use  AGS  (adaptive  gradient  sampling)
algorithm for nonsmooth constrained  optimization.  This  algorithm  is  a
slight modification of one described in  "An  Adaptive  Gradient  Sampling
Algorithm for Nonsmooth Optimization" by Frank E. Curtisy and Xiaocun Quez.

This optimizer has following benefits and drawbacks:
+ robustness; it can be used with nonsmooth and nonconvex functions.
+ relatively easy tuning; most of the metaparameters are easy to select.
- it has convergence of steepest descent, slower than CG/LBFGS.
- each iteration involves evaluation of ~2N gradient values  and  solution
  of 2Nx2N quadratic programming problem, which  limits  applicability  of
  algorithm by small-scale problems (up to 50-100).

IMPORTANT: this  algorithm  has  convergence  guarantees,   i.e.  it  will
           steadily move towards some stationary point of the function.

           However, "stationary point" does not  always  mean  "solution".
           Nonsmooth problems often have "flat spots",  i.e.  areas  where
           function do not change at all. Such "flat spots" are stationary
           points by definition, and algorithm may be caught here.

           Nonsmooth CONVEX tasks are not prone to  this  problem. Say, if
           your function has form f()=MAX(f0,f1,...), and f_i are  convex,
           then f() is convex too and you have guaranteed  convergence  to
           solution.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Radius  -   initial sampling radius, >=0.

                Internally multiplied  by  vector of  per-variable  scales
                specified by minnssetscale()).

                You should select relatively large sampling radius, roughly
                proportional to scaled length of the first  steps  of  the
                algorithm. Something close to 0.1 in magnitude  should  be
                good for most problems.

                AGS solver can automatically decrease radius, so too large
                radius is  not a problem (assuming that you  won't  choose
                so large radius that algorithm  will  sample  function  in
                too far away points, where gradient value is irrelevant).

                Too small radius won't cause algorithm to fail, but it may
                slow down algorithm (it may  have  to  perform  too  short
                steps).
    Penalty -   penalty coefficient for nonlinear constraints:
                * for problem with nonlinear constraints  should  be  some
                  problem-specific  positive   value,  large  enough  that
                  penalty term changes shape of the function.
                  Starting  from  some  problem-specific   value   penalty
                  coefficient becomes  large  enough  to  exactly  enforce
                  nonlinear constraints;  larger  values  do  not  improve
                  precision.
                  Increasing it too much may slow down convergence, so you
                  should choose it carefully.
                * can be zero for problems WITHOUT  nonlinear  constraints
                  (i.e. for unconstrained ones or ones with  just  box  or
                  linear constraints)
                * if you specify zero value for problem with at least  one
                  nonlinear  constraint,  algorithm  will  terminate  with
                  error code -1.

ALGORITHM OUTLINE

The very basic outline of unconstrained AGS algorithm is given below:

0. If sampling radius is below EpsX  or  we  performed  more  then  MaxIts
   iterations - STOP.
1. sample O(N) gradient values at random locations  around  current point;
   informally speaking, this sample is an implicit piecewise  linear model
   of the function, although algorithm formulation does  not  mention that
   explicitly
2. solve quadratic programming problem in order to find descent direction
3. if QP solver tells us that we  are  near  solution,  decrease  sampling
   radius and move to (0)
4. perform backtracking line search
5. after moving to new point, goto (0)

Constraint handling details:
* box constraints are handled exactly by algorithm
* linear/nonlinear constraints are handled by adding L1  penalty.  Because
  our solver can handle nonsmoothness, we can  use  L1  penalty  function,
  which is an exact one  (i.e.  exact  solution  is  returned  under  such
  penalty).
* penalty coefficient for  linear  constraints  is  chosen  automatically;
  however, penalty coefficient for nonlinear constraints must be specified
  by user.

===== TRACING AGS SOLVER =================================================

AGS solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'AGS'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'AGS.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'AGS'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format
* 'AGS.DETAILED.SAMPLE'-
                  for output of points being visited ,  search  directions
                  and gradient sample. May take a LOT of  space ,  do  not
                  use it on problems with more that several tens of vars.
                  This  symbol   also    implicitly   defines   'AGS'  and
                  'AGS.DETAILED'.

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("AGS,PREC.F6", "path/to/trace.log")
>


  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetalgoags(minnsstate &state, const double radius, const double penalty, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to minnsoptimize().

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnssetxrep(minnsstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnsrequesttermination(minnsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minnsiteration(minnsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    fvec    -   callback which calculates function vector fi[]
                at given point x
    jac     -   callback which calculates function vector fi[]
                and Jacobian jac at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied Jacobian, and one which uses  only  function
   vector and numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   you should choose appropriate variant of  minnsoptimize() -  one  which
   accepts function AND Jacobian or one which accepts ONLY function.

   Be careful to choose variant of minnsoptimize()  which  corresponds  to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to minnsoptimize()    and  specific
   function used to create optimizer.


                     |         USER PASSED TO minnsoptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   minnscreatef()    |     works               FAILS
   minnscreate()     |     FAILS               works

   Here "FAILS" denotes inappropriate combinations  of  optimizer creation
   function  and  minnsoptimize()  version.   Attemps   to    use     such
   combination will lead to exception. Either  you  did  not pass gradient
   when it WAS needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey


*************************************************************************/
void minnsoptimize(minnsstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minnsoptimize(minnsstate &state,
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
MinNS results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report. You should check Rep.TerminationType
                in  order  to  distinguish  successful  termination  from
                unsuccessful one:
                * -8   internal integrity control  detected  infinite  or
                       NAN   values   in   function/gradient.    Abnormal
                       termination signalled.
                * -3   box constraints are inconsistent
                * -1   inconsistent parameters were passed:
                       * penalty parameter for minnssetalgoags() is zero,
                         but we have nonlinear constraints set by minnssetnlc()
                *  2   sampling radius decreased below epsx
                *  7    stopping conditions are too stringent,
                        further improvement is impossible,
                        X contains best point found so far.
                *  8    User requested termination via minnsrequesttermination()

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnsresults(const minnsstate &state, real_1d_array &x, minnsreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************

Buffered implementation of minnsresults() which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnsresultsbuf(const minnsstate &state, real_1d_array &x, minnsreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with minnscreate() call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnsrestartfrom(minnsstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINDF) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                          GLOBAL OPTIMIZATION
               SUBJECT TO BOX/LINEAR/NONLINEAR CONSTRAINTS

The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints
* nonlinear generalized inequality constraints Li<=Ci(x)<=Ui, with one  of
  Li/Ui possibly being infinite

REQUIREMENTS:
* F() and C()  do  NOT  have  to be differentiable, locally Lipschitz   or
  continuous. Most solvers in this subpackage can deal with  nonsmoothness
  or minor discontinuities, although obviously smoother problems  are  the
  most easy ones.
* generally, F() and C() must be computable at any point which is feasible
  subject to box constraints

USAGE:

1. User  initializes  algorithm  state   with   mindfcreate()   call   and
   chooses specific solver to be used. There is some solver which is  used
   by default, with default settings, but  you  should  NOT  rely  on  the
   default choice. It may change in the future releases of ALGLIB  without
   notice, and no one can guarantee that the new solver will be  able   to
   solve your problem with default settings.

2. User adds boundary and/or linear and/or nonlinear constraints by  means
   of calling one of the following functions:
   a) mindfsetbc() for boundary constraints
   b) mindfsetlc2dense() for linear constraints
   c) mindfsetnlc2() for nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.

3. User sets scale of the variables with mindfsetscale() function.  It  is
   VERY important to set  variable  scales  because  many  derivative-free
   algorithms refuse to work when variables are badly scaled.
   Scaling  helps  to seed initial  population,  control  convergence  and
   enforce penalties for constraint violation.

4. Finally, user calls mindfoptimize()  function which takes algorithm
   state and pointer (delegate, etc) to callback function which calculates
   F and G.

5. User calls mindfresults() to get a solution


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from  size  of  X
    X       -   starting point, array[N]. Some solvers can utilize  a good
                initial point to seed computations.

                As of ALGLIB 4.04, the initial point is:
                * used by GDEMO

                If the chosen solver does not need initial point, one  can
                supply zeros.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state


IMPORTANT: the  MINDF  optimizer   supports   parallel   model  evaluation
           ('callback  parallelism'). This feature, which  is  present  in
           commercial ALGLIB editions, greatly accelerates algorithms like
           differential evolution which usually issue  batch  requests  to
           user callbacks which can be efficiently parallelized.

           Callback parallelism  is  usually  beneficial  when  the  batch
           evalution requires more than several milliseconds.

           See ALGLIB Reference Manual, 'Working with commercial  version'
           section,  and  comments  on  mindfoptimize()  function for more
           information.

  -- ALGLIB --
     Copyright 24.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfcreate(const ae_int_t n, const real_1d_array &x, mindfstate &state, const xparams _xparams = alglib::xdefault);
void mindfcreate(const real_1d_array &x, mindfstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints.

Box constraints are inactive by default (after initial creation).

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

  -- ALGLIB --
     Copyright 24.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetbc(mindfstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU with dense
constraint matrix A.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinDFcreate() call.
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
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetlc2dense(mindfstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void mindfsetlc2dense(mindfstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided nonlinear constraints.

In fact, this function sets only  NUMBER  of  the  nonlinear  constraints.
Constraints  themselves  (constraint  functions)   are   passed   to   the
MinDFOptimize() method.

This method accepts user-defined vector function F[] where:
* first component of F[] corresponds to the objective
* subsequent NNLC components of F[] correspond to the two-sided  nonlinear
  constraints NL<=C(x)<=NU, where
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
    State   -   structure previously allocated with MinDFCreate call.
    NL      -   array[NNLC], lower bounds, can contain -INF
    NU      -   array[NNLC], lower bounds, can contain +INF
    NNLC    -   constraints count, NNLC>=0

NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
        possible   that  algorithm  will  evaluate  function  outside   of
        feasible area!

NOTE 2: the algorithm scales variables according to the scale specified by
        MinDFSetScale() function, so it can handle problems with badly
        scaled variables (as long as we KNOW their scales).

        However,  there  is  no  way  to  automatically  scale   nonlinear
        constraints. Inappropriate scaling  of nonlinear  constraints  may
        ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
        is NOT the same as solving it with constraint "0.001*G0(x)=0".

        It means that YOU are  the  one who is responsible for the correct
        scaling of the nonlinear constraints. We recommend  you  to  scale
        nonlinear constraints in such  a  way  that  the  derivatives  (if
        constraints are differentiable) have approximately unit  magnitude
        (for problems  with  unit  variable  scales)  or  have  magnitudes
        approximately equal to 1/S[i] (where S is a variable scale set  by
        MinDFSetScale() function).

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetnlc2(mindfstate &state, const real_1d_array &nl, const real_1d_array &nu, const ae_int_t nnlc, const xparams _xparams = alglib::xdefault);
void mindfsetnlc2(mindfstate &state, const real_1d_array &nl, const real_1d_array &nu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for the optimizer.

This function sets a  combined   stopping  condition:  stopping  when  two
criteria are met simultaneously:
* function   values   has  converged  to  a  neighborhood  whose  size  is
  proportional to epsF
* variable   values   has  converged  to  a  neighborhood  whose  size  is
  proportional to epsX
It is possible to use only one condition by setting another EPS to zero.

Most derivarive-free solvers are heuristics, so the code used to implement
this stopping condition is an heuristic too. Usually 'proportional to EPS'
means that we are somewhere between Eps/10...Eps*10 away from the solution.
However, there are no warranties that the solver  has  actually  converged
to something, although in practice it works well.

The  specific  meaning  of  'converging'  is  algorithm-dependent.  It  is
possible that some future ALGLIB optimizers will  ignore  this  condition,
see comments on specific solvers for more info. This  condition  does  not
work for multi-objective problems.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsF    -   >=0:
                * zero value means no condition for F
                * EpsF>0 means stopping when the solver converged with
                  an error estimate less than EpsF*max(|F|,1)
    EpsX    -   >=0:
                * zero value means no condition for X
                * EpsX>0 means stopping when the solver converged with
                  error in I-th variable less than EpsX*S[i], where S[i]
                  is a variable scale

  -- ALGLIB --
     Copyright 23.04.2024 by Bochkanov Sergey
*************************************************************************/
void mindfsetcondfx(mindfstate &state, const double epsf, const double epsx, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function activates/deactivates internal timers  used  to  track  time
spent in various parts of the solver (mostly, callbacks vs solver itself).

When activated with this function, the following timings are stored in the
mindfreport structure fields:
* total time spend in the optimization
* time spent in the callback
* time spent in the solver itself

See comments on mindfreport structure for more  information  about  timers
and their accuracy.

Timers are an essential part of reports that helps to find out  where  the
most time is spent and how to  optimize  the  code.  E.g.,  noticing  that
significant amount of time is spent  in  numerical  differentiation  makes
obvious that ALGLIB-provided parallel numerical differentiation is needed.

However, time measurements add noticeable  overhead,  about  50-100ns  per
function call. In some applications it results in a significant  slowdown,
that's why this option is inactive  by  default  and  should  be  manually
activated.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    UseTimers-  true or false

NOTE: when tracing is turned on with alglib::trace_file(), some derivative
      free  solvers   may  also  perform  internal,  more  detailed   time
      measurements, which are printed to the log file.

  -- ALGLIB --
     Copyright 23.04.2024 by Bochkanov Sergey
*************************************************************************/
void mindfusetimers(mindfstate &state, const bool usetimers, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping condition for the optimizer: function   values
has converged to a neighborhood whose size is proportional to epsF.

Most derivarive-free solvers are heuristics, so the code used to implement
this stopping condition is an heuristic too. Usually 'proportional to EPS'
means that we are somewhere between Eps/10...Eps*10 away from the solution.
However, there are no warranties that the solver  has  actually  converged
to something, although in practice it works well.

The  specific  meaning  of  'converging'  is  algorithm-dependent.  It  is
possible that some future ALGLIB optimizers will  ignore  this  condition,
see comments on specific solvers for more info. This  condition  does  not
work for multi-objective problems.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsF    -   >=0:
                * zero value means no condition for F
                * EpsF>0 means stopping when the solver converged with
                  an error estimate less than EpsF*max(|F|,1)

  -- ALGLIB --
     Copyright 23.04.2024 by Bochkanov Sergey
*************************************************************************/
void mindfsetcondf(mindfstate &state, const double epsf, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets variable scales.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison  with  tolerances)  and  to
guide algorithm steps.

The scale of a variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetscale(mindfstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinDFOptimize().

NOTE: algorithm passes two parameters to rep() callback  - the best  point
      so far and a function value at the point. For unconstrained problems
      the function value is  non-increasing (the most recent best point is
      always at least not worse than the previous best one).  However,  it
      can increase between iterations when  solving  constrained  problems
      (a  better  point  may  have  higher  objective  value  but  smaller
      constraint violation).

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void mindfsetxrep(mindfstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of a running optimizer. It
should be called from a user-supplied callback when user decides  that  it
is time to "smoothly" terminate optimization process.  As  a  result,  the
optimizer  stops  at  the  point  which  was  "current  accepted" when the
termination request was submitted and returns  error  code  8  (successful
termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on an optimizer which is NOT running will have
      no effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfrequesttermination(mindfstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets seed used by internal RNG.

By default, a random seed is used, i.e. every time you run the solver,  we
seed its generator with a new value obtained  from  the  system-wide  RNG.
Thus, the solver returns non-deterministic results. You  can  change  such
a behavior by specifying a fixed positive seed value.

INPUT PARAMETERS:
    S           -   optimizer structure
    SeedVal     -   seed value:
                    * positive values are used for  seeding  RNG  with  a
                      fixed  seed,  i.e.  subsequent  runs  on  the  same
                      objective will return the same results
                    * non-positive seed means that a random  seed is used
                      for every run, i.e. subsequent  runs  on  the  same
                      objective will return slightly different results

  -- ALGLIB --
     Copyright 26.04.2024 by Bochkanov Sergey
*************************************************************************/
void mindfsetseed(mindfstate &s, const ae_int_t seedval, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine sets optimization algorithm to the differential  evolution
solver GDEMO (Generalized Differential Evolution Multiobjective)  with  an
automatic parameters selection.

NOTE: a version with manually tuned parameters can be activated by calling
      the mindfsetalgogdemofixed() function.

The primary stopping condition for the solver is to stop after the specified
number of iterations. You can also specify  additional  criteria  to  stop
early:
* stop when subpopulation target values (2N+1 best individuals) are within
  EPS from the best one so far (function values seem to converge)
* stop when both subpopulation target values  AND  subpopulation  variable
  values are within EPS from the best one so far

The first condition is specified with mindfsetcondf(), the second  one  is
activated with mindfsetcondfx().

Both conditions are heuristics which  may fail. Being 'within EPS from the
best value so far'  in  practice  means  that  we  are  somewhere   within
[0.1EPS,10EPS] from the true  solution;  however,  on  difficult  problems
this condition may fire too early.

Imposing an additional requirement that variable values have clustered  too
may prevent us from premature  stopping. However, on multi-extremal and/or
noisy problems too many individuals may be trapped away from the  optimum,
preventing this condition from activation.

ALGORITHM PROPERTIES:

* the solver uses a variant of  the  adaptive  parameter  tuning  strategy
  called 'Success-History  Based  Parameter  Adaptation  for  Differential
  Evolution Ensemble' (SHADE) by Ryoji Tanabe and Alex  Fukunaga.  You  do
  not have to specify crossover probability and differential weight,   the
  solver will automatically choose the most appropriate strategy.

* the solver can handle box, linear,  nonlinear  constraints.  Linear  and
  nonlinear constraints are  handled  by  means  of an L1/L2  penalty. The
  solver does not violate box constraints at any point,  but  may  violate
  linear and nonlinear ones. Penalty coefficient can be changed with   the
  mindfsetgdemopenalty() function.

* the solver heavily depends on variable scales being available (specified
  by means of mindfsetscale() call) and on box constraints with both lower
  and upper bounds being available which are used to determine the  search
  region. It will work without box constraints  and  without  scales,  but
  results are likely to be suboptimal.

* the solver is SIMD-optimized  and  parallelized  (in  commercial  ALGLIB
  editions), with nearly linear scalability of parallel processing.

* this solver is intended for finding solutions  with up to several digits
  of precision at best. Its primary purpose  is  to  find  at  least  some
  solution to an otherwise intractable problem.

IMPORTANT: derivative-free optimization is inherently less robust than the
           smooth nonlinear programming, especially when nonsmoothness and
           discontinuities are present.

           Derivative-free  algorithms  have  less  convergence guarantees
           than their smooth  counterparts.  It  is  considered  a  normal
           (although, obviously, undesirable) situation when a derivative-
           -free algorithm fails to converge with desired precision. Having
           2 digits of accurasy is already a  good  result,  on  difficult
           problems (high numerical noise, discontinuities) you  may  have
           even less than that.

INPUT PARAMETERS:
    State       -   solver
    EpochsCnt   -   iterations count, >0.  Usually   the  algorithm  needs
                    hundreds of iterations to converge.
    PopSize     -   population  size,  >=0.  Zero  value  means  that  the
                    default value (which is 10*N in the  current  version)
                    will be chosen. Good values are  in  5*N...20*N,  with
                    the smaller values being recommended for easy problems
                    and the larger  values  for  difficult  multi-extremal
                    and/or noisy tasks.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetalgogdemo(mindfstate &state, const ae_int_t epochscnt, const ae_int_t popsize, const xparams _xparams = alglib::xdefault);
void mindfsetalgogdemo(mindfstate &state, const ae_int_t epochscnt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine tells GDEMO differential  evolution  optimizer  to  use  a
ROBUST profile (the default option).

The ROBUST profile is intended  to  facilitate  explorative  behavior  and
robust convergence even on difficult multi-extremal problems. It comes  at
the expense of increased running time  even  on  easy  problems. The QUICK
profile can be chosen if your problem is relatively easy to handle and you
prefer speed over robustness. In most cases, the QUICK profile  is  ~2x-3x
faster than the robust one.

This function has effect only on adaptive GDEMO with automatic  parameters
selection.  It  has  no  effect  on  fixed-parameters  GDEMO  or any other
solvers.

IMPORTANT: this function does NOT change the  optimization  algorithm.  If
           want to activate differential evolution solver, you still  have
           to call a proper mindfsetalgo???() function.

INPUT PARAMETERS:
    State       -   solver

  -- ALGLIB --
     Copyright 25.04.2024 by Bochkanov Sergey
*************************************************************************/
void mindfsetgdemoprofilerobust(mindfstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine tells GDEMO differential  evolution  optimizer  to  use  a
QUICK profile.

The QUICK profile is intended  to  facilitate accelerated  convergence  on
medium-complexity problems at the cost  of  (sometimes)  having  premature
convergence  on  difficult  and/or  multi-extremal  problems.  The  ROBUST
profile can be selected if you favor convergence  warranties  over  speed.
In most cases, the ROBUST profile is ~2x-3x slower than the QUICK one.

This function has effect only on adaptive GDEMO with automatic  parameters
selection.  It  has  no  effect  on  fixed-parameters  GDEMO  or any other
solvers.

IMPORTANT: this function does NOT change the  optimization  algorithm.  If
           you want to activate differential evolution solver,  you  still
           have to call a proper mindfsetalgo???() function.

INPUT PARAMETERS:
    State       -   solver

  -- ALGLIB --
     Copyright 25.04.2024 by Bochkanov Sergey
*************************************************************************/
void mindfsetgdemoprofilequick(mindfstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine tells GDEMO differential  evolution  optimizer  to  handle
linear/nonlinear constraints by an L1/L2 penalty function.

IMPORTANT: this function does NOT change the  optimization  algorithm.  If
           want to activate differential evolution solver, you still  have
           to call a proper mindfsetalgo???() function.

INPUT PARAMETERS:
    State       -   solver
    Rho1, Rho2  -   penalty parameters for constraint violations:
                    * Rho1 is a multiplier for L1 penalty
                    * Rho2 is a multiplier for L2 penalty
                    * Rho1,Rho2>=0
                    * having both of them at zero means that some  default
                      value will be chosen.
                    Ignored for problems with box-only constraints.

                    L1 penalty is usually better at enforcing constraints,
                    but leads to slower convergence than L2 penalty. It is
                    possible to combine both kinds of penalties together.

                    There is a compromise between constraint  satisfaction
                    and  optimality:  high  values  of   Rho   mean   that
                    constraints are satisfied with high accuracy  but that
                    the target may  be  underoptimized  due  to  numerical
                    difficulties.  Small  values  of  Rho  mean  that  the
                    solution may  grossly  violate  constraints.  Choosing
                    good Rho is usually a matter of trial and error.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetgdemopenalty(mindfstate &state, const double rho1, const double rho2, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine sets optimization algorithm to the differential  evolution
solver GDEMO (Generalized Differential Evolution Multiobjective)  with the
manual parameters selection.

Unlike DE with an automatic parameters selection  this  function  requires
user to manually specify  algorithm  parameters. In the general  case  the
full-auto GDEMO is better. However, it has to spend some time finding  out
properties of a problem being solved; furthermore, it is  not  allowed  to
try potentially dangerous values of  parameters  that  lead  to  premature
stopping. Manually tuning the solver to the specific problem at  hand  can
get 2x-3x better running time.

Aside from that, the algorithm is fully equivalent to automatic GDEMO, and
we   recommend  you  reading  comments  on  mindfsetalgogdemo()  for  more
information about algorithm properties and stopping criteria.

INPUT PARAMETERS:
    State       -   solver
    EpochsCnt   -   iterations count, >0.  Usually   the  algorithm  needs
                    hundreds of iterations to converge.
    Strategy    -   specific DE strategy to use:
                    * 0 for DE/rand/1
                    * 1 for DE/best/2
                    * 2 for DE/current-to-best/1
    CrossoverProb-  crossover probability, 0<CrossoverProb<1
    DifferentialWeight- weight, 0<DifferentialWeight<2
    PopSize     -   population  size,  >=0.  Zero  value  means  that  the
                    default value (which is 10*N in the  current  version)
                    will be chosen. Good values are  in  5*N...20*N,  with
                    the smaller values being recommended for easy problems
                    and the larger  values  for  difficult  multi-extremal
                    and/or noisy tasks.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetalgogdemofixed(mindfstate &state, const ae_int_t epochscnt, const ae_int_t strategy, const double crossoverprob, const double differentialweight, const ae_int_t popsize, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool mindfiteration(mindfstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    fvec    -   callback which calculates function vector fi[]
                at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


CALLBACK PARALLELISM

The  MINDF  optimizer  supports   parallel   model  evaluation  ('callback
parallelism').  This  feature,  which  is  present  in  commercial  ALGLIB
editions, greatly accelerates  optimization  when  using  a  solver  which
issues batch requests, i.e. multiple requests  for  target  values,  which
can be computed independently by different threads.

Callback parallelism is  usually   beneficial  when   processing  a  batch
request requires more than several  milliseconds.  It  also  requires  the
solver which issues requests in convenient batches, e.g. the  differential
evolution solver.

See ALGLIB Reference Manual, 'Working with commercial version' section for
more information.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey


*************************************************************************/
void mindfoptimize(mindfstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
MinDF results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:

                rep.f stores objective value at the solution

                You  should     check  rep.terminationtype  in   order  to
                distinguish successful termination from unsuccessful one:
                * -8   internal integrity control  detected  infinite  or
                       NAN   values   in   function/gradient.    Abnormal
                       termination signalled.
                * -3   box constraints are inconsistent
                * -1   inconsistent parameters were passed:
                       * penalty parameter is zero, but we have nonlinear
                         constraints set by MinDFsetnlc2()
                *  1   successful termination according to a solver-specific
                       set of conditions
                *  8   User requested termination via minnsrequesttermination()

                If you activated timers  with  mindfusetimers(),  you  can
                also find out how much time was spent in various code parts:
                * rep.timetotal    - for a total time in seconds
                * rep.timesolver   - for a time spent in the solver itself
                * rep.timecallback - for a time spent in user callbacks
                See comments on mindfreport structure for more information
                about timers and their accuracy.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfresults(const mindfstate &state, real_1d_array &x, mindfreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************

Buffered implementation of MinDFresults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfresultsbuf(const mindfstate &state, real_1d_array &x, mindfreport &rep, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_NLS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                DERIVATIVE-FREE NONLINEAR LEAST SQUARES

DESCRIPTION:

This function creates a NLS  solver  configured  to  solve  a  constrained
nonlinear least squares problem

    min F(x) = f[0]^2 + f[1]^2 + ... + f[m-1]^2

where f[i] are available, but not their derivatives.

The  functions  f[i]  are  assumed  to  be   smooth,  but  may  have  some
amount of numerical noise (either  random  noise  or  deterministic  noise
arising from numerical simulations or other complex numerical processes).

INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i], M>=1
    X       -   initial point, array[N]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlscreatedfo(const ae_int_t n, const ae_int_t m, const real_1d_array &x, nlsstate &state, const xparams _xparams = alglib::xdefault);
void nlscreatedfo(const ae_int_t m, const real_1d_array &x, nlsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets the derivative-free NLS optimization algorithm  to  the
2PS (2-Point Stencil) algorithm.

This solver is recommended for the following cases:
* an expensive target function is minimized  by the commercial ALGLIB with
  callback  parallelism  activated  (see  ALGLIB Reference Manual for more
  information about parallel callbacks)
* an inexpensive target function is minimized by any ALGLIB edition  (free
  or commercial)

This function works only with solvers created with nlscreatedfo(), i.e. in
the derivative-free mode.

See the end of this comment for more information about the algorithm.

INPUT PARAMETERS:
    State           -   solver; must be created with nlscreatedfo() call -
                        passing  an  object   initialized   with   another
                        constructor function will result in an exception.
    NNoisyRestarts  -   number of restarts performed to combat a noise  in
                        the target. (see below, section 'RESTARTS', for  a
                        detailed discussion):
                        * 0     means that no restarts is performed, the
                                solver stops as soon as stopping  criteria
                                are met. Recommended for noise-free tasks.
                        * >0    means  that when the stopping criteria are
                                met, the solver will  perform  a  restart:
                                increase the trust   radius  and  resample
                                points. It often helps to  solve  problems
                                with random or deterministic noise.

ALGORITHM DESCRIPTION AND DISCUSSION

The 2PS algorithm is a derivative-free model-based nonlinear least squares
solver which builds local models by evaluating the target at  N additional
points around the current one, with geometry similar to the 2-point finite
difference stencil.

Similarly to the Levenberg-Marquardt algorithm, the solver shows quadratic
convergence despite the fact that it builds linear models.

When compared with the DFO-LSA solver, the 2PS algorithm has the following
distinctive properties:
* the 2PS algorithm performs more target function evaluations per iteration
  (at least N+1 instead of 1-2 usually performed by the DFO-LSA)
* 2PS requires several times less iterations than the DFO-LSA because each
  iteration extracts and utilizes more information about the  target. This
  difference tends to exaggerate when N increases
* contrary to that, DFO-LSA is much better at reuse of previously computed
  points. Thus, DFO-LSA needs several times less target  evaluations  than
  2PS, usually about 3-4 times less (this ratio seems to be more  or  less
  constant independently of N).

The summary is that:
* for  expensive  targets  2PS  provides better parallelism potential than
  DFO-LSA because the former issues many  simultaneous  target  evaluation
  requests which can be easily parallelized. It  is  possible  for  2PS to
  outperform DFO-LSA by parallelism  alone,  despite  the  fact  that  the
  latter needs 3-4 times less target function evaluations.
* for inexpensive targets 2PS may win  because  it  needs many  times less
  iterations, and thus the overhead associated with the working set updates
  is also many times less.

RESTARTS

Restarts is a strategy used to deal with random and deterministic noise in
the target/constraints.

Noise in the objective function can be random, arising from measurement or
simulation   uncertainty,  or  deterministic,   resulting   from   complex
underlying phenomena like numerical errors or branches in the target.  Its
influence is especially high at last stages of the optimization, when  all
computations are performed with small values of a trust radius.

Restarts allow the optimization algorithm to be robust against both  types
of noise by temporarily increasing a trust radius in order  to  capture  a
global structure of the target and avoid being trapped  by  noise-produced
local features.

A restart is usually performed when the stopping criteria  are  triggered.
Instead of stopping, the solver increases  trust  radius  to  its  initial
value and tries to rebuild a model.

If you decide to optimize with restarts,  it  is  recommended  to  perform
a small amount of restarts, up to 5. Generally, restarts do not allow  one
to completely solve the problem of noise, but still   it  is  possible  to
achieve some additional progress.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetalgo2ps(nlsstate &state, const ae_int_t nnoisyrestarts, const xparams _xparams = alglib::xdefault);
void nlssetalgo2ps(nlsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets the derivative-free NLS optimization algorithm  to  the
DFO-LSA algorithm, an ALGLIB implementation (with several modifications)of
the original DFO-LS algorithm  by  Cartis, C., Fiala, J., Marteau, B.  and
Roberts, L. ('Improving the  Flexibility  and  Robustness  of  Model-Based
Derivative-Free Optimization Solvers', 2019). The A in DFO-LSA stands  for
ALGLIB, in order to distinguish our slightly modified implementation  from
the original algorithm.

This solver is recommended for the following  case:  an  expensive  target
function is minimized without parallelism being used (either  free  ALGLIB
is used or commercial one is used but the target callback is non-reentrant
i.e. it can not be simultaneously called from multiple threads)

This function works only with solvers created with nlscreatedfo(), i.e. in
the derivative-free mode.

See the end of this comment for more information about the algorithm.

INPUT PARAMETERS:
    State           -   solver; must be created with nlscreatedfo() call -
                        passing  an  object   initialized   with   another
                        constructor function will result in an exception.
    NNoisyRestarts  -   number of restarts performed to combat a noise  in
                        the target. (see below, section 'RESTARTS', for  a
                        detailed discussion):
                        * 0     means that no restarts is performed, the
                                solver stops as soon as stopping  criteria
                                are met. Recommended for noise-free tasks.
                        * >0    means  that when the stopping criteria are
                                met, the solver will  perform  a  restart:
                                increase the trust   radius  and  resample
                                points. It often helps to  solve  problems
                                with random or deterministic noise.

ALGORITHM DESCRIPTION AND DISCUSSION

The DFO-LSA algorithm is a derivative-free model-based  NLS  solver  which
builds local models by remembering N+1 previously computed  target  values
and updating them as optimization progresses.

Similarly to the Levenberg-Marquardt algorithm, the solver shows quadratic
convergence  despite  the  fact  that  it  builds   linear   models.   Our
implementation generally follows the same lines as the  original  DFO-LSA,
with several modifications to trust radius  update  strategies,  stability
fixes (unlike original DFO-LS, our implementation can handle  and  recover
from the target breaking down due to infeasible arguments) and other minor
implementation details.

When compared with the 2PS solver, the DFO-LSA algorithm has the following
distinctive properties:
* the 2PS algorithm performs more target function evaluations per iteration
  (at least N+1 instead of 1-2 usually performed by DFO-LSA)
* 2PS requires several times less iterations  than  DFO-LSA  because  each
  iterations extracts and utilizes more information about the target. This
  difference tends to exaggerate when N increases
* contrary to that, DFO-LSA is much better at reuse of previously computed
  points. Thus, DFO-LSA needs several times less target  evaluations  than
  2PS, usually about 3-4 times less (this ratio seems to be more  or  less
  constant independently of N).

The summary is that:
* for  expensive  targets DFO-LSA is much more efficient than 2PS  because
  it reuses previously computed target values as much as possible.
* however, DFO-LSA has little parallelism potential because  (unlike  2PS)
  it  does  not  evaluate the target  in several points simultaneously and
  independently
* additionally, because DFO-LSA performs many times more  iterations  than
  2PS, iteration overhead (working set updates and matrix  inversions)  is
  an issue here. For inexpensive targets it is possible for DFO-LSA to  be
  outperformed by 2PS merely because of the linear algebra cost.

RESTARTS

Restarts is a strategy used to deal with random and deterministic noise in
the target/constraints.

Noise in the objective function can be random, arising from measurement or
simulation   uncertainty,  or  deterministic,   resulting   from   complex
underlying phenomena like numerical errors or branches in the target.  Its
influence is especially high at last stages of the optimization, when  all
computations are performed with small values of a trust radius.

Restarts allow the optimization algorithm to be robust against both  types
of noise by temporarily increasing a trust radius in order  to  capture  a
global structure of the target and avoid being trapped  by  noise-produced
local features.

A restart is usually performed when the stopping criteria  are  triggered.
Instead of stopping, the solver increases  trust  radius  to  its  initial
value and tries to rebuild a model.

If you decide to optimize with restarts,  it  is  recommended  to  perform
a small amount of restarts, up to 5. Generally, restarts do not allow  one
to completely solve the problem of noise, but still   it  is  possible  to
achieve some additional progress.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetalgodfolsa(nlsstate &state, const ae_int_t nnoisyrestarts, const xparams _xparams = alglib::xdefault);
void nlssetalgodfolsa(nlsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   stop when the scaled trust region radius is  smaller  than
                EpsX.
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations   is    unlimited.

Passing  EpsX=0  and  MaxIts=0  (simultaneously)  will  lead  to automatic
stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetcond(nlsstate &state, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to NLSOptimize().

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetxrep(nlsstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets variable scales

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Generally, scale is NOT considered to be a  form  of  preconditioner.  But
derivative-free optimizers often use scaling matrix both  in  the stopping
condition tests and as a preconditioner.

Proper scaling is very important for the algorithm performance. It is less
important for the quality of results, but still has some influence (it  is
easier  to  converge  when  variables  are  properly  scaled, so premature
stopping is possible when very badly scalled variables are  combined  with
relaxed stopping conditions).

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetscale(nlsstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints

Box constraints are inactive by default (after initial creation). They are
preserved until explicitly turned off with another SetBC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (the latter is recommended).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (the latter is recommended).

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: unless  explicitly  mentioned  in  the   specific   NLS  algorithm
        description, the following holds:
        * box constraints are always satisfied exactly
        * the target is NOT evaluated outside of the box-constrained area

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetbc(nlsstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool nlsiteration(nlsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    fvec    -   callback which calculates function vector fi[]
                at given point x
    jac     -   callback which calculates function vector fi[]
                and Jacobian jac at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


CALLBACK PARALLELISM

The  NLS  optimizer  supports   parallel   model   evaluation   ('callback
parallelism').  This  feature,  which  is  present  in  commercial  ALGLIB
editions, greatly accelerates  optimization  when  using  a  solver  which
issues batch requests, i.e. multiple requests  for  target  values,  which
can be computed independently by different threads.

Callback parallelism is  usually   beneficial  when   processing  a  batch
request requires more than several  milliseconds.  It  also  requires  the
solver which issues requests in convenient batches, e.g. 2PS solver.

See ALGLIB Reference Manual, 'Working with commercial version' section for
more information.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey


*************************************************************************/
void nlsoptimize(nlsstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void nlsoptimize(nlsstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
Nonlinear least squares solver results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization  report;  includes  termination   codes   and
                additional information. Termination codes are returned  in
                rep.terminationtype field, its possible values are  listed
                below, see comments for this structure for more info.

                The termination code is a sum of a basic code (success  or
                failure)  and  one/several  additional  codes.  Additional
                codes are returned only for successful termination.

                The following basic codes can be returned:
                * -8    optimizer detected NAN/INF values in the target or
                        nonlinear constraints and failed to recover
                * -3    box constraints are inconsistent
                *  2    relative step is no more than EpsX.
                *  5    MaxIts steps was taken
                *  7    stopping conditions are too stringent,
                        further improvement is impossible
                *  8    terminated by user who called nlsrequesttermination().
                        X contains point which was "current accepted" when
                        termination request was submitted.

                The following additional codes can be returned  (added  to
                a basic code):
                * +800      if   during  algorithm  execution  the  solver
                            encountered NAN/INF values in  the  target  or
                            constraints but managed to recover by reducing
                            trust region radius, the  solver  returns  one
                            of SUCCESS codes but adds +800 to the code.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlsresults(const nlsstate &state, real_1d_array &x, nlsreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Buffered implementation of NLSResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void nlsresultsbuf(const nlsstate &state, real_1d_array &x, nlsreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  subroutine  restarts solver from the new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   optimizer
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void nlsrestartfrom(nlsstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void nlsrequesttermination(nlsstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_LPQPPRESOLVE) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINQP) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                    CONSTRAINED QUADRATIC PROGRAMMING

The subroutine creates QP optimizer. After initial creation,  it  contains
default optimization problem with zero quadratic and linear terms  and  no
constraints.

In order to actually solve something you should:

specify objective:
* set linear term with minqpsetlinearterm()
* set quadratic term with minqpsetquadraticterm() or
  minqpsetquadratictermsparse()

specify constraints:
* set variable bounds with minqpsetbc() or minqpsetbcall()
* specify linear constraint matrix with one of the following functions:
  * modern API:
    * minqpsetlc2()       for sparse two-sided constraints AL <= A*x <= AU
    * minqpsetlc2dense()  for dense  two-sided constraints AL <= A*x <= AU
    * minqpsetlc2mixed()  for mixed  two-sided constraints AL <= A*x <= AU
    * minqpaddlc2dense()  to add one dense row to the dense constraint submatrix
    * minqpaddlc2()       to add one sparse row to the sparse constraint submatrix
    * minqpaddlc2sparsefromdense() to add one sparse row (passed as a dense array) to the sparse constraint submatrix
  * legacy API:
    * minqpsetlc()        for dense one-sided equality/inequality constraints
    * minqpsetlcsparse()  for sparse one-sided equality/inequality constraints
    * minqpsetlcmixed()   for mixed dense/sparse one-sided equality/inequality constraints
* add two-sided quadratic constraint(s) of the form CL <= b'x+0.5*x'Qx <= CU
  with one of the following functions:
  * minqpaddqc2()         for a quadratic constraint given by a sparse
                          matrix structure; has O(max(N,NNZ)) memory and
                          running time requirements.
  * minqpaddqc2dense()    for a quadratic constraint given by a dense
                          matrix; has O(N^2) memory and running time requirements.
  * minqpaddqc2list()     for a sparse quadratic constraint given by
                          a list of non-zero  entries;  has  O(NNZ) memory
                          and O(NNZ*logNNZ)  running   time  requirements,
                          ideal for constraints  with  much  less  than  N
                          nonzero elements.
* add second order cone constraints with:
  * minqpaddsoccprimitive()     for a primitive second order cone constraint
  * minqpaddsoccorthogonal()    for an axis-orthogonal second order cone constraint
* add power cone constraints with:
  * minqpaddpowccprimitive()    for a primitive power cone constraint
  * minqpaddpowccorthogonal()   for an axis-orthogonal power cone constraint

configure and run QP solver:
* choose appropriate QP solver and set it  and  its stopping  criteria  by
  means of minqpsetalgo??????() function
* call minqpoptimize() to run the solver and  minqpresults()  to  get  the
  solution vector and additional information.

Following solvers are recommended for  convex  and  semidefinite  problems
with box and linear constraints:
* QuickQP for small  dense  problems  with  box-only  constraints  (or  no
  constraints at all)
* DENSE-IPM-QP for  convex  or  semidefinite  problems  with   medium  (up
  to several thousands) variable count, dense/sparse  quadratic  term  and
  any number  (up  to  many  thousands)  of  dense/sparse  general  linear
  constraints
* SPARSE-IPM-QP for convex  or  semidefinite  problems  with   large (many
  thousands) variable count, sparse quadratic term AND linear constraints.
* SPARSE-ECQP for convex having only  linear  equality  constraints.  This
  specialized solver can be orders of magnitude faster than IPM.

If your problem happens to be nonconvex or has nonlinear constraints, then
you can use:
* DENSE-GENIPM or SPARSE-GENIPM  solver  which  supports  convex/nonconvex
  QP   problems  with  box,  linear,  quadratic  equality/inequality   and
  conic constraints.
* HYBRID-GENIPM solver which is optimized  for  well-conditioned  problems
  with large dense quadratic term/constraints or for problems  with  large
  sparse quadratic terms having prohibitively dense Cholesky factors.  The
  solver uses limited-memory SR1 quadratic model to accelerate Newton step.
* QuickQP for small dense nonconvex problems with box-only constraints

INPUT PARAMETERS:
    N       -   problem size

OUTPUT PARAMETERS:
    State   -   optimizer with zero quadratic/linear terms
                and no constraints

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpcreate(const ae_int_t n, minqpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets linear term for QP solver.

By default, linear term is zero.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    B       -   linear term, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetlinearterm(minqpstate &state, const real_1d_array &b, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  dense  quadratic  term  for  QP solver. By  default,
quadratic term is zero.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn't used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn't used

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadraticterm(minqpstate &state, const real_2d_array &a, const bool isupper, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  sparse  quadratic  term  for  QP solver. By default,
quadratic  term  is  zero.  This  function  overrides  previous  calls  to
minqpsetquadraticterm() or minqpsetquadratictermsparse().

NOTE: dense solvers like DENSE-AUL-QP or DENSE-IPM-QP  will  convert  this
      matrix to dense storage anyway.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   (optional) storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn't used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn't used
                * if not given, both lower and upper  triangles  must  be
                  filled.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadratictermsparse(minqpstate &state, const sparsematrix &a, const bool isupper, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets starting point for QP solver. It is useful to have good
initial approximation to the solution, because it will increase  speed  of
convergence and identification of active constraints.

NOTE: interior point solvers ignore initial point provided by user.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    X       -   starting point, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetstartingpoint(minqpstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function sets origin for QP solver. By default, following QP program
is solved:

    min(0.5*x'*A*x+b'*x)

This function allows to solve a different problem:

    min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))

Specification of non-zero origin  affects  function  being  minimized  and
quadratic/conic constraints, but not box and linear constraints which  are
still calculated without origin.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    XOrigin -   origin, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetorigin(minqpstate &state, const real_1d_array &xorigin, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison  with  tolerances)  and  as
preconditioner.

Scale of the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the
   function

If you do not know how to choose scales of your variables, you can:
* read www.alglib.net/optimization/scaling.php article
* use minqpsetscaleautodiag(), which calculates scale  using  diagonal  of
  the  quadratic  term:  S  is  set to 1/sqrt(diag(A)), which works well
  sometimes.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetscale(minqpstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets automatic evaluation of variable scaling.

IMPORTANT: this function works only for  matrices  with positive  diagonal
           elements! Zero or negative elements will  result  in  -9  error
           code  being  returned.  Specify  scale  vector  manually   with
           minqpsetscale() in such cases.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison  with  tolerances)  and  as
preconditioner.

The  best  way  to  set  scaling  is  to manually specify variable scales.
However, sometimes you just need quick-and-dirty solution  -  either  when
you perform fast prototyping, or when you know your problem well  and  you
are 100% sure that this quick solution is robust enough in your case.

One such solution is to evaluate scale of I-th variable as 1/Sqrt(A[i,i]),
where A[i,i] is an I-th diagonal element of the quadratic term.

Such approach works well sometimes, but you have to be careful here.

INPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void minqpsetscaleautodiag(minqpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells QP solver to use DENSE-AUL algorithm and sets stopping
criteria for the algorithm.

This  algorithm  is  intended  for  non-convex problems with moderate  (up
to several thousands) variable count and arbitrary number  of  constraints
which are either (a) effectively convexified under constraints or (b) have
unique solution even with nonconvex target.

IMPORTANT: when DENSE-IPM solver is applicable, its performance is usually
           much better than that of DENSE-AUL.
           We recommend  you to use DENSE-AUL only when other solvers  can
           not be used.

ALGORITHM FEATURES:

* supports  box  and  dense/sparse  general   linear   equality/inequality
  constraints
* convergence is theoretically proved for positive-definite  (convex)   QP
  problems. Semidefinite and non-convex problems can be solved as long  as
  they  are   bounded  from  below  under  constraints,  although  without
  theoretical guarantees.

ALGORITHM OUTLINE:

* this  algorithm   is   an   augmented   Lagrangian   method  with  dense
  preconditioner (hence  its  name).
* it performs several outer iterations in order to refine  values  of  the
  Lagrange multipliers. Single outer  iteration  is  a  solution  of  some
  unconstrained optimization problem: first  it  performs  dense  Cholesky
  factorization of the Hessian in order to build preconditioner  (adaptive
  regularization is applied to enforce positive  definiteness),  and  then
  it uses L-BFGS optimizer to solve optimization problem.
* typically you need about 5-10 outer iterations to converge to solution

ALGORITHM LIMITATIONS:

* because dense Cholesky driver is used, this algorithm has O(N^2)  memory
  requirements and O(OuterIterations*N^3) minimum running time.  From  the
  practical  point  of  view,  it  limits  its  applicability  by  several
  thousands of variables.
  From  the  other  side,  variables  count  is  the most limiting factor,
  and dependence on constraint count is  much  more  lower. Assuming  that
  constraint matrix is sparse, it may handle tens of thousands  of general
  linear constraints.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0, stopping criteria for inner optimizer.
                Inner  iterations  are  stopped  when  step  length  (with
                variable scaling being applied) is less than EpsX.
                See  minqpsetscale()  for  more  information  on  variable
                scaling.
    Rho     -   penalty coefficient, Rho>0:
                * large enough  that  algorithm  converges  with   desired
                  precision.
                * not TOO large to prevent ill-conditioning
                * recommended values are 100, 1000 or 10000
    ItsCnt  -   number of outer iterations:
                * recommended values: 10-15 (although  in  most  cases  it
                  converges within 5 iterations, you may need a  few  more
                  to be sure).
                * ItsCnt=0 means that small number of outer iterations  is
                  automatically chosen (10 iterations in current version).
                * ItsCnt=1 means that AUL algorithm performs just as usual
                  penalty method.
                * ItsCnt>1 means that  AUL  algorithm  performs  specified
                  number of outer iterations

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing  EpsX=0  will  lead  to  automatic  step  length  selection
      (specific step length chosen may change in the future  versions  of
      ALGLIB, so it is better to specify step length explicitly).

  -- ALGLIB --
     Copyright 20.08.2016 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgodenseaul(minqpstate &state, const double epsx, const double rho, const ae_int_t itscnt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells QP solver to  use  DENSE-IPM  QP  algorithm  and  sets
stopping criteria for the algorithm.

This  algorithm  is  intended for convex and semidefinite QP (but not QCQP
or conic) problems with moderate (up to several thousands) variable  count
and arbitrary number of linear constraints. Quadratic and conic constraints
are supported by another solver (DENSE-GENIPM).

IMPORTANT: the commercial edition of ALGLIB can parallelize  factorization
           phase of this function (this phase takes most time when solving
           large-scale problems). It can also use  a  performance  backend
           (e.g.  Intel  MKL  or  another  platform-specific  library)  to
           accelerate dense factorization.

           Dense Cholesky factorization is a well  studied  and  optimized
           algorithm. ALGLIB  includes  a  good  implementation;  however,
           using a hardware vendor-provided  performance  library  usually
           results in a better performance.

           See the ALGLIB Reference Manual for more information on how  to
           activate parallelism and backend support.

IMPORTANT: this  algorithm  is  likely  to  fail  on  nonconvex  problems,
           furthermore, sometimes it fails without a notice. If you try to
           run DENSE-IPM on a problem  with  indefinite  matrix (a  matrix
           having  at least one negative eigenvalue) then depending on the
           circumstances it may either (a) stall at some  arbitrary point,
           or (b) throw an exception due to the failure  of  the  Cholesky
           decomposition.

           Use GENIPM algorithm if  your  problem  is  nonconvex  or has a
           potential of becoming  nonconvex.  The GENIPM solver  can  also
           handle problems with quadratic and conic constraints.

ALGORITHM FEATURES:

* supports  box  and  dense/sparse  general   linear   equality/inequality
  constraints

ALGORITHM OUTLINE:

* this  algorithm  is  our implementation  of  interior  point  method  as
  formulated by  R.J.Vanderbei, with minor modifications to the  algorithm
  (damped Newton directions are extensively used)
* like all interior point methods, this algorithm  tends  to  converge  in
  roughly same number of iterations (between 15 and 50) independently from
  the problem dimensionality

ALGORITHM LIMITATIONS:

* because a dense Cholesky driver is used, for  N-dimensional problem with
  M dense constaints this algorithm has O(N^2+N*M) memory requirements and
  O(N^3+M*N^2) running time.
  Having sparse constraints with Z nonzeros per row  relaxes  storage  and
  running time down to O(N^2+M*Z) and O(N^3+M*Z^2)
  From the practical  point  of  view,  it  limits  its  applicability  by
  several thousands of variables.
  From  the  other  side,  variables  count  is  the most limiting factor,
  and dependence on constraint count is  much  more  lower. Assuming  that
  the constraint matrix is sparse, it may  handle  tens  of  thousands  of
  general linear constraints.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Eps     -   >=0, stopping criteria. The algorithm stops  when   primal
                and dual infeasiblities as well as complementarity gap are
                less than Eps.

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing EpsX=0 will lead to automatic selection of small epsilon.

===== TRACING IPM SOLVER =================================================

IPM solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'IPM'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'IPM.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'IPM'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("IPM,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgodenseipm(minqpstate &state, const double eps, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells QP solver to  use  SPARSE-IPM  QP algorithm  and  sets
stopping criteria for the algorithm.

This  algorithm  is  intended for convex and semidefinite QP (but not QCQP
or conic) problems with large  variable  and  constraint  count and sparse
quadratic term and sparse linear constraints. It was successfully used for
problems with millions of variables and constraints. Quadratic  and  conic
constraints are supported by another solver (SPARSE-GENIPM).

It is possible to have some limited set of dense linear constraints - they
will be handled separately  by  the  dense  BLAS  -  but  the  more  dense
constraints you have, the more time solver needs.

IMPORTANT: the commercial edition of ALGLIB can parallelize  factorization
           phase of this function (this phase takes most time when solving
           large-scale problems). It can also use  a  performance  backend
           (e.g. Intel PARDISO or another  platform-specific  library)  to
           accelerate sparse factorization.

           Specific speed-up due to parallelism  and  performance  backend
           usage heavily depends on the sparsity pattern of quadratic term
           and constraints. For some problem  types  performance  backends
           provide great speed-up. For other  ones,  ALGLIB's  own  sparse
           factorization code is the preferred option.

           See the ALGLIB Reference Manual for more information on how  to
           activate parallelism and backend support.

IMPORTANT: internally this solver performs large  and  sparse  (N+M)x(N+M)
           triangular factorization. So it expects both quadratic term and
           constraints to be highly sparse. However, its  running  time is
           influenced by BOTH fill factor and sparsity pattern.

           Generally we expect that no more than few nonzero  elements per
           row are present. However different sparsity patterns may result
           in completely different running  times  even  given  same  fill
           factor.

           In many cases this algorithm outperforms DENSE-IPM by order  of
           magnitude. However, in some cases you may  get  better  results
           with DENSE-IPM even when solving sparse task.

IMPORTANT: this algorithm won't work for nonconvex problems. If you try to
           run SPARSE-IPM  on a problem with indefinite quadratic term  (a
           matrix having at least one negative eigenvalue) then  depending
           on the circumstances it may either (a) stall  at some arbitrary
           point, or (b) throw an exception due  to  the  failure  of  the
           Cholesky decomposition.

           Use GENIPM algorithm if  your  problem  is  nonconvex  or has a
           potential of becoming  nonconvex.  The  GENIPM  solver can also
           handle problems with quadratic and conic constraints.

ALGORITHM FEATURES:

* supports  box  and  dense/sparse  general   linear   equality/inequality
  constraints
* specializes on large-scale sparse problems

ALGORITHM OUTLINE:

* this  algorithm  is  our implementation  of  interior  point  method  as
  formulated by  R.J.Vanderbei, with minor modifications to the  algorithm
  (damped Newton directions are extensively used)
* like all interior point methods, this algorithm  tends  to  converge  in
  roughly same number of iterations (between 15 and 50) independently from
  the problem dimensionality

ALGORITHM LIMITATIONS:

* this algorithm may handle moderate number  of dense constraints, usually
  no more than a thousand of dense ones without losing its efficiency.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Eps     -   >=0, stopping criteria. The algorithm stops  when   primal
                and dual infeasiblities as well as complementarity gap are
                less than Eps.

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing EpsX=0 will lead to automatic selection of small epsilon.

===== TRACING IPM SOLVER =================================================

IPM solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'IPM'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'IPM.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'IPM'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("IPM,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgosparseipm(minqpstate &state, const double eps, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells QP solver to  use  DENSE-GENIPM  QP algorithm and sets
stopping criteria for the algorithm.

This  algorithm  is  intended  for convex/nonconvex box/linearly/conically
constrained QP problems with moderate (up to several thousands)  variables
count and arbitrary number  of  constraints.  Use  SPARSE-GENIPM  if  your
problem is sparse.

The algorithm is a generalization of DENSE-IPM solver, capable of handling
more general constraints as well as nonconvexity of  the  target.  In  the
latter case, a local solution is found.

IMPORTANT: the commercial edition of ALGLIB can parallelize  factorization
           phase of this function (this phase takes most time when solving
           large-scale problems). It can also use  a  performance  backend
           (e.g.  Intel  MKL  or  another  platform-specific  library)  to
           accelerate dense factorization.

           Dense Cholesky factorization is a well  studied  and  optimized
           algorithm.  ALGLIB  includes  a  good  implementation; however,
           using a hardware vendor-provided  performance  library  usually
           results in a better performance.

           See the ALGLIB Reference Manual for more information on how  to
           activate parallelism and backend support.

ALGORITHM FEATURES:

* supports box, linear equality/inequality and conic constraints
* for convex problems returns the global (and the only) solution
* can handle non-convex  problem  (only  a  locally  optimal  solution  is
  returned in this case)

ALGORITHM LIMITATIONS:

* because a dense Cholesky driver is used, for  N-dimensional problem with
  M dense constaints this algorithm has O(N^2+N*M) memory requirements and
  O(N^3+M*N^2) running time.
  Having sparse constraints with Z nonzeros per row  relaxes  storage  and
  running time down to O(N^2+M*Z) and O(N^3+M*Z^2)
  From the practical  point  of  view,  it  limits  its  applicability  by
  several thousands of variables.
  From  the  other  side,  variables  count  is  the most limiting factor,
  and dependence on constraint count is  much  more  lower. Assuming  that
  the constraint matrix is sparse, it may  handle  tens  of  thousands  of
  general linear constraints.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Eps     -   >=0, stopping criteria. The algorithm stops  when   primal
                and dual infeasiblities as well as complementarity gap are
                less than Eps.

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing EpsX=0 will lead to automatic selection of small epsilon.

===== TRACING GENIPM SOLVER ==============================================

GENIPM solver supports advanced tracing capabilities. You can log algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'GENIPM'      - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'GENIPM.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'GENIPM'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("GENIPM,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 01.05.2024 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgodensegenipm(minqpstate &state, const double eps, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells QP solver to  use SPARSE-GENIPM  QP algorithm and sets
stopping criteria for the algorithm.

This   algorithm  is intended for convex/nonconvex box/linearly/conically/
quadratically constrained QP  problems  with  sparse  quadratic  term  and
constraints. It can handle millions of variables and constraints, assuming
that the problem is sufficiently sparse. If your problem is small (several
thousands vars at most) and dense, consider using DENSE-GENIPM  as  a more
efficient alternative.

The  algorithm  is  a  generalization of the SPARSE-IPM solver, capable of
handling more general constraints as well as nonconvexity of  the  target.
In the latter case, a local solution is found.

IMPORTANT: the commercial edition of ALGLIB can parallelize  factorization
           phase of this function (this phase takes most time when solving
           large-scale problems). Specific  speed-up  due  to  parallelism
           heavily depends on the sparsity pattern of quadratic  term  and
           constraints.

           See the ALGLIB Reference Manual for more information on how  to
           activate parallelism support.

           This function does not use performance backends  to  accelerate
           sparse factorization because external  libraries  typically  do
           not  provide   fine  control over regularization, pivoting  and
           sparse orderings.

IMPORTANT: internally this solver performs large  and  sparse  (N+M)x(N+M)
           triangular factorization. So it expects both quadratic term and
           constraints to be highly sparse. However, its  running  time is
           influenced by BOTH fill factor and sparsity pattern.

           Generally we expect that no more than few nonzero  elements per
           row are present. However different sparsity patterns may result
           in completely different running  times  even  given  same  fill
           factor.

           In many cases this algorithm outperforms DENSE-IPM by order  of
           magnitude. However, in some cases you may  get  better  results
           with DENSE-IPM even when solving sparse task.

ALGORITHM FEATURES:


* supports box, linear equality/inequality constraints
* for convex problems returns the global (and the only) solution
* can handle non-convex  problem  (only  a  locally  optimal  solution  is
  returned in this case)
* specializes on large-scale sparse problems

ALGORITHM LIMITATIONS:

* this algorithm may handle moderate number  of dense constraints, usually
  no more than a thousand of dense ones without losing its efficiency.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Eps     -   >=0, stopping criteria. The algorithm stops  when   primal
                and dual infeasiblities as well as complementarity gap are
                less than Eps.

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing EpsX=0 will lead to automatic selection of small epsilon.

===== TRACING GENIPM SOLVER ==============================================

GENIPM solver supports advanced tracing capabilities. You can log algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'GENIPM'      - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'GENIPM.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'IPM'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("GENIPM,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 01.05.2024 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgosparsegenipm(minqpstate &state, const double eps, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells QP solver to  use HYBRID-GENIPM  QP algorithm and sets
stopping criteria for the algorithm.

This  algorithm is intended for finding medium-accuracy solutions (say, no
more than 5 accurate digits) to large-scale convex/nonconvex box/linearly/
conically/quadratically constrained  QP  problems  with  objective  and/or
quadratic constraints being  well-conditioned  but  having   prohibitively
dense Cholesky factors. This includes both  dense  problems  and  problems
with sparse terms having too dense factorizations.

Being well conditioned allows us to use low rank LBFGS/LSR1 approximations
to the objective (quadratic constraints) and use sparse linear algebra for
the rest of the problem,  thus  achieving  significant  speed-up  on  many
types of problems.

IMPORTANT: the commercial edition of ALGLIB can parallelize  factorization
           phase of this function (this phase takes most time when solving
           large-scale problems). Specific  speed-up  due  to  parallelism
           heavily depends on the sparsity pattern of quadratic  term  and
           constraints.

           See the ALGLIB Reference Manual for more information on how  to
           activate parallelism support.

           This function does not use performance backends  to  accelerate
           sparse factorization because external  libraries  typically  do
           not  provide   fine  control over regularization, pivoting  and
           sparse orderings.

IMPORTANT: performance of this function depends on both  sparsity  pattern
           of the problem and on its conditioning properties.  The running
           time for a dense QP/QCQP problem grows with variables count  as
           O(N^2) while DENSE-GENIPM running time grows as O(N^3).

           On the other hand, DENSE/SPARSE-GENIPM running time shows  only
           moderate dependence on the condition number, while HYBRID-GENIPM
           running time often explodes with  condition  number larger than
           10000.

           In practice, on a dense QCQP problem HYBRID algorithm starts to
           singificantly outperform DENSE-GENIPM for N>=1000.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

    Eps     -   >=0, stopping criteria. The algorithm stops  when   primal
                and dual infeasiblities as well as complementarity gap are
                less than Eps.

                This   solver  is  optimized for  finding  medium-accuracy
                solutions, with eps being between 1E-3 and 1E-5.

    MemLen  -   >=0, memory length for the limited memory SR1 model.  Zero
                value means automatic selection of some  small  predefined
                value  which  may  change  in  future  versions of ALGLIB.
                Optimal value is problem-dependent, with  8...32  being  a
                good initial point for the experimentation.  Values  above
                N are silently truncated down to N.

    MaxOffdiag- >=0, quadratic terms with more than MaxOffdiag offdiagonal
                elements are handled via quasi-Newton approximation. Terms
                with no more than MaxOffdiag elements are handled exactly,
                including strictly diagonal terms that are always  handled
                exactly (because they have the minimum number off-diagonal
                elements possible - zero). Symmetric terms appearing above
                and below diagonal are counted as one  term,  not  two.  A
                single quasi-Newton model is built, which approximates all
                eligible quadratic terms together.

                The following values are possible:
                * 0 - approximate every quadratic term having at least one
                  off-diagonal element with a quasi-Newton model.
                * 0<MaxOffdiag<N*(N-1)/2 - eligible  quadratic terms (ones
                  that are too dense) are approximated, sufficiently sparse
                  ones are handled precisely
                * MaxOffdiag>=N*(N-1)/2 - no quadratic  term  is  eligible
                  for quasi-Newton approximation, algorithm performance is
                  roughly similar to  that  of  SPARSE-GENIPM,  with  some
                  additional small overhead for quasi-Newton code checks.

  -- ALGLIB --
     Copyright 01.05.2024 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgohybridgenipm(minqpstate &state, const double eps, const ae_int_t memlen, const ae_int_t maxoffdiag, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells QP solver to use an ECQP algorithm.

This  algorithm  is  intended  for sparse convex problems with only linear
equality constraints. It can handle millions of variables and constraints,
assuming that the problem is sufficiently  sparse.  However,  it  can  NOT
deal with nonlinear equality constraints or inequality constraints of  any
type (including box ones), nor it can deal with nonconvex problems.

When applicable, it outperforms SPARSE-IPM by  tens  of  times.  It  is  a
regularized direct linear algebra solver that performs several  rounds  of
iterative  refinement  in  order to improve a solution. Thus, due  to  its
direct nature, it does not need stopping criteria and performs much faster
than interior point methods.

IMPORTANT: the commercial edition of ALGLIB can parallelize  factorization
           phase of this function (this phase takes most time when solving
           large-scale problems). It can also use  a  performance  backend
           (e.g. Intel PARDISO or another  platform-specific  library)  to
           accelerate sparse factorization.

           Specific speed-up due to parallelism  and  performance  backend
           usage heavily depends on the sparsity pattern of quadratic term
           and constraints. For some problem  types  performance  backends
           provide great speed-up. For other  ones,  ALGLIB's  own  sparse
           factorization code is the preferred option.

           See the ALGLIB Reference Manual for more information on how  to
           activate parallelism and backend support.

IMPORTANT: internally this solver performs large  and  sparse  (N+M)x(N+M)
           triangular factorization. So it expects both quadratic term and
           constraints to be highly sparse. However, its  running  time is
           influenced by BOTH fill factor and sparsity pattern.

           Generally we expect that no more than few nonzero  elements per
           row are present. However different sparsity patterns may result
           in completely different running  times  even  given  same  fill
           factor.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Eps     -   >=0, stopping criteria. The algorithm stops  when   primal
                and dual infeasiblities are less than Eps.

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing EpsX=0 will lead to automatic selection of small epsilon.

  -- ALGLIB --
     Copyright 01.07.2024 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgosparseecqp(minqpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells solver to use QuickQP  algorithm:  special  extra-fast
algorithm for problems with box-only constrants. It may  solve  non-convex
problems as long as they are bounded from below under constraints.

ALGORITHM FEATURES:
* several times faster than DENSE-IPM when running on box-only problem
* utilizes accelerated methods for activation of constraints.
* supports dense and sparse QP problems
* supports ONLY box constraints; general linear constraints are NOT
  supported by this solver
* can solve all types of problems  (convex,  semidefinite,  nonconvex)  as
  long as they are bounded from below under constraints.
  Say, it is possible to solve "min{-x^2} subject to -1<=x<=+1".
  In convex/semidefinite case global minimum  is  returned,  in  nonconvex
  case - algorithm returns one of the local minimums.

ALGORITHM OUTLINE:

* algorithm  performs  two kinds of iterations: constrained CG  iterations
  and constrained Newton iterations
* initially it performs small number of constrained CG  iterations,  which
  can efficiently activate/deactivate multiple constraints
* after CG phase algorithm tries to calculate Cholesky  decomposition  and
  to perform several constrained Newton steps. If  Cholesky  decomposition
  failed (matrix is indefinite even under constraints),  we  perform  more
  CG iterations until we converge to such set of constraints  that  system
  matrix becomes  positive  definite.  Constrained  Newton  steps  greatly
  increase convergence speed and precision.
* algorithm interleaves CG and Newton iterations which  allows  to  handle
  indefinite matrices (CG phase) and quickly converge after final  set  of
  constraints is found (Newton phase). Combination of CG and Newton phases
  is called "outer iteration".
* it is possible to turn off Newton  phase  (beneficial  for  semidefinite
  problems - Cholesky decomposition will fail too often)

ALGORITHM LIMITATIONS:

* algorithm does not support general  linear  constraints;  only  box ones
  are supported
* Cholesky decomposition for sparse problems  is  performed  with  Skyline
  Cholesky solver, which is intended for low-profile matrices. No profile-
  reducing reordering of variables is performed in this version of ALGLIB.
* problems with near-zero negative eigenvalues (or exacty zero  ones)  may
  experience about 2-3x performance penalty. The reason is  that  Cholesky
  decomposition can not be performed until we identify directions of  zero
  and negative curvature and activate corresponding boundary constraints -
  but we need a lot of trial and errors because these directions  are hard
  to notice in the matrix spectrum.
  In this case you may turn off Newton phase of algorithm.
  Large negative eigenvalues  are  not  an  issue,  so  highly  non-convex
  problems can be solved very efficiently.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled constrained gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinQPSetScale()
    EpsF    -   >=0
                The  subroutine  finishes its work if exploratory steepest
                descent  step  on  k+1-th iteration  satisfies   following
                condition:  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
    EpsX    -   >=0
                The  subroutine  finishes its work if exploratory steepest
                descent  step  on  k+1-th iteration  satisfies   following
                condition:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinQPSetScale()
    MaxOuterIts-maximum number of OUTER iterations.  One  outer  iteration
                includes some amount of CG iterations (from 5 to  ~N)  and
                one or several (usually small amount) Newton steps.  Thus,
                one outer iteration has high cost, but can greatly  reduce
                funcation value.
                Use 0 if you do not want to limit number of outer iterations.
    UseNewton-  use Newton phase or not:
                * Newton phase improves performance of  positive  definite
                  dense problems (about 2 times improvement can be observed)
                * can result in some performance penalty  on  semidefinite
                  or slightly negative definite  problems  -  each  Newton
                  phase will bring no improvement (Cholesky failure),  but
                  still will require computational time.
                * if you doubt, you can turn off this  phase  -  optimizer
                  will retain its most of its high speed.

IT IS VERY IMPORTANT TO CALL MinQPSetScale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS STOPPING CRITERIA ARE SCALE-DEPENDENT!

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection (presently it is  small    step
length, but it may change in the future versions of ALGLIB).

  -- ALGLIB --
     Copyright 22.05.2014 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgoquickqp(minqpstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxouterits, const bool usenewton, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for QP solver

Box constraints are inactive by default (after  initial  creation).  After
being  set,  they are  preserved until explicitly overwritten with another
minqpsetbc()  or  minqpsetbcall()  call,  or  partially  overwritten  with
minqpsetbci() call.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (latter is recommended because
                it will allow solver to use better algorithm).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (latter is recommended because
                it will allow solver to use better algorithm).

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: if constraints for all variables are same you may use minqpsetbcall()
      which allows to specify constraints without using arrays.

NOTE: BndL>BndU will result in QP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbc(minqpstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for QP solver (all variables  at  once,
same constraints for all variables)

Box constraints are inactive by default (after  initial  creation).  After
being  set,  they are  preserved until explicitly overwritten with another
minqpsetbc()  or  minqpsetbcall()  call,  or  partially  overwritten  with
minqpsetbci() call.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd                BndL=BndU
    lower bound         BndL<=x[i]              BndU=+INF
    upper bound         x[i]<=BndU              BndL=-INF
    range               BndL<=x[i]<=BndU        ...
    free variable       -                       BndL=-INF, BndU+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bound, same for all variables
    BndU    -   upper bound, same for all variables

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: BndL>BndU will result in QP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbcall(minqpstate &state, const double bndl, const double bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for I-th variable (other variables are
not modified).

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd                BndL=BndU
    lower bound         BndL<=x[i]              BndU=+INF
    upper bound         x[i]<=BndU              BndL=-INF
    range               BndL<=x[i]<=BndU        ...
    free variable       -                       BndL=-INF, BndU+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bound
    BndU    -   upper bound

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: BndL>BndU will result in QP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbci(minqpstate &state, const ae_int_t i, const double bndl, const double bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets dense linear constraints for QP optimizer.

This  function  overrides  results  of  previous  calls  to  minqpsetlc(),
minqpsetlcsparse() and minqpsetlcmixed().  After  call  to  this  function
all non-box constraints are dropped, and you have only  those  constraints
which were specified in the present call.

If you want  to  specify  mixed  (with  dense  and  sparse  terms)  linear
constraints, you should call minqpsetlcmixed().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately  -
        there always exists some violation due  to  numerical  errors  and
        algorithmic limitations.

  -- ALGLIB --
     Copyright 19.06.2012 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc(minqpstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minqpsetlc(minqpstate &state, const real_2d_array &c, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets sparse linear constraints for QP optimizer.

This  function  overrides  results  of  previous  calls  to  minqpsetlc(),
minqpsetlcsparse() and minqpsetlcmixed().  After  call  to  this  function
all non-box constraints are dropped, and you have only  those  constraints
which were specified in the present call.

If you want  to  specify  mixed  (with  dense  and  sparse  terms)  linear
constraints, you should call minqpsetlcmixed().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    C       -   linear  constraints,  sparse  matrix  with  dimensions  at
                least [K,N+1]. If matrix has  larger  size,  only  leading
                Kx(N+1) rectangle is used.
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0

NOTE 1: linear (non-bound) constraints are satisfied only approximately  -
        there always exists some violation due  to  numerical  errors  and
        algorithmic limitations.

  -- ALGLIB --
     Copyright 22.08.2016 by Bochkanov Sergey
*************************************************************************/
void minqpsetlcsparse(minqpstate &state, const sparsematrix &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets mixed linear constraints, which include a set of  dense
rows, and a set of sparse rows.

This  function  overrides  results  of  previous  calls  to  minqpsetlc(),
minqpsetlcsparse() and minqpsetlcmixed().

This function may be useful if constraint matrix includes large number  of
both types of rows - dense and sparse. If you have just a few sparse rows,
you  may  represent  them  in  dense  format  without losing  performance.
Similarly, if you have just a few dense rows, you may store them in sparse
format with almost same performance.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    SparseC -   linear constraints, sparse  matrix with dimensions EXACTLY
                EQUAL TO [SparseK,N+1].  Each  row  of  C  represents  one
                constraint, either equality or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    SparseCT-   type of sparse constraints, array[K]:
                * if SparseCT[i]>0, then I-th constraint is SparseC[i,*]*x >= SparseC[i,n+1]
                * if SparseCT[i]=0, then I-th constraint is SparseC[i,*]*x  = SparseC[i,n+1]
                * if SparseCT[i]<0, then I-th constraint is SparseC[i,*]*x <= SparseC[i,n+1]
    SparseK -   number of sparse equality/inequality constraints, K>=0
    DenseC  -   dense linear constraints, array[K,N+1].
                Each row of DenseC represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of DenseC (including right part) must be finite.
    DenseCT -   type of constraints, array[K]:
                * if DenseCT[i]>0, then I-th constraint is DenseC[i,*]*x >= DenseC[i,n+1]
                * if DenseCT[i]=0, then I-th constraint is DenseC[i,*]*x  = DenseC[i,n+1]
                * if DenseCT[i]<0, then I-th constraint is DenseC[i,*]*x <= DenseC[i,n+1]
    DenseK  -   number of equality/inequality constraints, DenseK>=0

NOTE 1: linear (non-box) constraints  are  satisfied only approximately  -
        there always exists some violation due  to  numerical  errors  and
        algorithmic limitations.

NOTE 2: due to backward compatibility reasons SparseC can be  larger  than
        [SparseK,N+1]. In this case only leading  [SparseK,N+1]  submatrix
        will be  used.  However,  the  rest  of  ALGLIB  has  more  strict
        requirements on the input size, so we recommend you to pass sparse
        term whose size exactly matches algorithm expectations.

  -- ALGLIB --
     Copyright 22.08.2016 by Bochkanov Sergey
*************************************************************************/
void minqpsetlcmixed(minqpstate &state, const sparsematrix &sparsec, const integer_1d_array &sparsect, const ae_int_t sparsek, const real_2d_array &densec, const integer_1d_array &densect, const ae_int_t densek, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides legacy API for specification of mixed  dense/sparse
linear constraints.

New conventions used by ALGLIB since release  3.16.0  state  that  set  of
sparse constraints comes first,  followed  by  set  of  dense  ones.  This
convention is essential when you talk about things like order of  Lagrange
multipliers.

However, legacy API accepted mixed  constraints  in  reverse  order.  This
function is here to simplify situation with code relying on legacy API. It
simply accepts constraints in one order (old) and passes them to new  API,
now in correct order.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlcmixedlegacy(minqpstate &state, const real_2d_array &densec, const integer_1d_array &densect, const ae_int_t densek, const sparsematrix &sparsec, const integer_1d_array &sparsect, const ae_int_t sparsek, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU with dense
constraint matrix A.

NOTE: knowing  that  constraint  matrix  is  dense  helps  some QP solvers
      (especially modern IPM method) to utilize efficient  dense  Level  3
      BLAS for dense parts of the problem. If your problem has both  dense
      and sparse constraints, you  can  use  minqpsetlc2mixed()  function,
      which will result in dense algebra being applied to dense terms, and
      sparse sparse linear algebra applied to sparse terms.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
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
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc2dense(minqpstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minqpsetlc2dense(minqpstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
sparse constraining matrix A. Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
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
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc2(minqpstate &state, const sparsematrix &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
mixed constraining matrix A including sparse part (first SparseK rows) and
dense part (last DenseK rows). Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

This function may be useful if constraint matrix includes large number  of
both types of rows - dense and sparse. If you have just a few sparse rows,
you  may  represent  them  in  dense  format  without losing  performance.
Similarly, if you have just a few dense rows, you may store them in sparse
format with almost same performance.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
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
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc2mixed(minqpstate &state, const sparsematrix &sparsea, const ae_int_t ksparse, const real_2d_array &densea, const ae_int_t kdense, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
matrix of currently present dense constraints.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    A       -   linear constraint coefficient, array[N], right side is NOT
                included.
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint Ai*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint Ai*x<=AU
                * AU=+INF  => one-sided constraint AL<=Ai*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minqpaddlc2dense(minqpstate &state, const real_1d_array &a, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present sparse constraints.

Constraint is passed in compressed format: as list of non-zero entries  of
coefficient vector A. Such approach is more efficient than  dense  storage
for highly sparse constraint vectors.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
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
void minqpaddlc2(minqpstate &state, const integer_1d_array &idxa, const real_1d_array &vala, const ae_int_t nnz, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present sparse constraints.

Constraint vector A is  passed  as  a  dense  array  which  is  internally
sparsified by this function.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
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
void minqpaddlc2sparsefromdense(minqpstate &state, const real_1d_array &da, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function clears the list of quadratic constraints. Other  constraints
are not modified.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.

  -- ALGLIB --
     Copyright 19.06.2024 by Bochkanov Sergey
*************************************************************************/
void minqpclearqc(minqpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends a two-sided quadratic constraint of the form

    CL <= b'x + 0.5*x'*Q*x <= CU

or (depending on the ApplyOrigin parameter)

    CL <= b'(x-origin) + 0.5*(x-origin)'*Q*(x-origin) <= CU

to the set of currently present constraints. The linear term is  given  by
a dense array, the quadratic term is given by a sparse array.

Here CL can be finite or -INF (absense of constraint), CU can be finite or
+INF (absense of constraint), CL<=CU, with  CL=CU  denoting   an  equality
constraint. Q is an arbitrary (including indefinite) symmetric matrix.

The  function  has  O(max(N,NNZ)) memory and time requirements  because  a
dense array is used to store linear term and because  most  sparse  matrix
storage formats supported by ALGLIB need at least O(N) memory even for  an
empty quadratic constraint matrix.

Use minqpaddqc2list() if you have to add many constraints with  much  less
than N nonzero elements.

IMPORTANT: ALGLIB  supports  arbitrary  quadratic  constraints,  including
           nonconvex ones. However, only convex constraints (combined with
           the convex objective) result in guaranteed convergence  to  the
           global minimizer. In all other cases, only local convergence to
           a local minimum is guaranteed.

           A convex constraint is a  constraint  of  the  following  form:
           b'*(x-origin) + 0.5(x-origin)'*Q*(x-origin) <=CU, with Q  being
           a semidefinite matrix. All other modifications  are  nonconvex:
           * -x0^2<=1 is nonconvex
           *  x0^2>=1 is nonconvex (despite Q=1 being positive definite)
           *  x0^2 =1 is nonconvex

           The latter case is notable because it effectively converts a QP
           problem into a mixed integer QP program. Smooth interior  point
           solver can not efficiently handle such programs, converging  to
           a randomly chosen x0 (either +1 or -1) and  keeping  its  value
           fixed during the optimization.

           It is also  notable  that  larger  equality  constraints  (e.g.
           x0^2+x1^2=1) are much less difficult  to  handle  because  they
           form large connected regions within the parameters space.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    Q       -   symmetric matrix Q in a sparse matrix storage format:
                * if IsUpper=True, then the upper triangle  is  given, and
                  the lower triangle is ignored
                * if IsUpper=False, then the lower triangle is  given, and
                  the upper triangle is ignored
                * any sparse matrix storage format present in ALGLIB is
                  supported
                * the matrix must be exactly NxN
    IsUpper -   whether upper or lower triangle of Q is used
    B       -   array[N], linear term
    CL, CU  -   lower and upper bounds:
                * CL can be finite or -INF (absence of a bound)
                * CU can be finite or +INF (absence of a bound)
                * CL<=CU, with CL=CU meaning an equality constraint
                * CL=-INF, CU=+INF => constraint is ignored
    ApplyOrigin-whether origin (as specified by minqpsetorigin) is applied
                to the constraint or not. If no origin was specified, this
                parameter has no effect.

RESULT:
    constraint index, starting from 0

  -- ALGLIB --
     Copyright 19.07.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t minqpaddqc2(minqpstate &state, const sparsematrix &q, const bool isupper, const real_1d_array &b, const double cl, const double cu, const bool applyorigin, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends a two-sided quadratic constraint of the form

    CL <= b'x + 0.5*x'*Q*x <= CU

or (depending on the ApplyOrigin parameter)

    CL <= b'(x-origin) + 0.5*(x-origin)'*Q*(x-origin) <= CU

to the set of currently present constraints.  Both  linear  and  quadratic
terms are given as lists of non-zero entries.

Here CL can be finite or -INF (absense of constraint), CU can be finite or
+INF (absense of constraint), CL<=CU, with  CL=CU  denoting   an  equality
constraint. Q is an arbitrary (including indefinite) symmetric matrix.

The function needs O(NNZ) memory for temporaries and  O(NNZ*logNNZ)  time,
where NNZ is a  total  number  of  non-zeros  in  both  lists.  For  small
constraints it can be orders of magnitude faster than  minqpaddqc2()  with
its  O(max(N,NNZ)) temporary memory or minqpaddqc2dense() with its  O(N^2)
temporaries. Thus, it is recommended if you have many small constraints.

NOTE: in  the  end,  all  quadratic  constraints  are  stored  in the same
      memory-efficient compressed format. However, you have to allocate an
      NxN  temporary  dense  matrix  when  you  pass  a  constraint  using
      minqpaddqc2dense(). Similarly, data structures used as a part of the
      API  provided  by   minqpaddqc2()   have   O(N)   temporary   memory
      requirements.

IMPORTANT: ALGLIB  supports  arbitrary  quadratic  constraints,  including
           nonconvex ones. However, only convex constraints (combined with
           the convex objective) result in guaranteed convergence  to  the
           global minimizer. In all other cases, only local convergence to
           a local minimum is guaranteed.

           A convex constraint is a  constraint  of  the  following  form:
           b'*(x-origin) + 0.5(x-origin)'*Q*(x-origin) <=CU, with Q  being
           a semidefinite matrix. All other modifications  are  nonconvex:
           * -x0^2<=1 is nonconvex
           *  x0^2>=1 is nonconvex (despite Q=1 being positive definite)
           *  x0^2 =1 is nonconvex

           The latter case is notable because it effectively converts a QP
           problem into a mixed integer QP program. Smooth interior  point
           solver can not efficiently handle such programs, converging  to
           a randomly chosen x0 (either +1 or -1) and  keeping  its  value
           fixed during the optimization.

           It is also  notable  that  larger  equality  constraints  (e.g.
           x0^2+x1^2=1) are much less difficult  to  handle  because  they
           form large connected regions within the parameters space.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    QRIdx   -   array[QNNZ], stores row indexes of QNNZ  nonzero  elements
                of a symmetric matrix Q
    QCIdx   -   array[QNNZ], stores col indexes of QNNZ  nonzero  elements
                of a symmetric matrix Q
    QVals   -   array[QNNZ], stores values of QNNZ  nonzero  elements of a
                symmetric matrix Q
    QNNZ    -   number of non-zero elements in Q, QNNZ>=0
    IsUpper -   whether upper or lower triangle of Q is used:
                * if IsUpper=True, then only elements with QRIdx[I]<=QCIdx[I]
                  are used and the rest is ignored
                * if IsUpper=False, then only elements with QRIdx[I]>=QCIdx[I]
                  are used and the rest is ignored
    BIdx    -   array[BNNZ], indexes of BNNZ nonzero elements of a linear term
    BVals   -   array[BNNZ], values of BNNZ nonzero elements of a linear term
    BNNZ    -   number of nonzero elements in B, BNNZ>=0
    CL, CU  -   lower and upper bounds:
                * CL can be finite or -INF (absence of a bound)
                * CU can be finite or +INF (absence of a bound)
                * CL<=CU, with CL=CU meaning an equality constraint
                * CL=-INF, CU=+INF => constraint is ignored
    ApplyOrigin-whether origin (as specified by minqpsetorigin) is applied
                to the constraint or not. If no origin was specified, this
                parameter has no effect.

RESULT:
    constraint index, starting from 0

  -- ALGLIB --
     Copyright 19.07.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t minqpaddqc2list(minqpstate &state, const integer_1d_array &qridx, const integer_1d_array &qcidx, const real_1d_array &qvals, const ae_int_t qnnz, const bool isupper, const integer_1d_array &bidx, const real_1d_array &bvals, const ae_int_t bnnz, const double cl, const double cu, const bool applyorigin, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends a two-sided quadratic constraint of the form

    CL <= b'x + 0.5*x'*Q*x <= CU

or (depending on the ApplyOrigin parameter)

    CL <= b'(x-origin) + 0.5*(x-origin)'*Q*(x-origin) <= CU

to the set of currently present constraints. The linear and quadratic terms
are given by dense arrays.

Here CL can be finite or -INF (absense of constraint), CU can be finite or
+INF (absense of constraint), CL<=CU, with  CL=CU  denoting   an  equality
constraint. Q is an arbitrary (including indefinite) symmetric matrix.

This function trades convenience of using dense arrays for the efficiency.
Because dense NxN storage is used, merely calling this function has O(N^2)
complexity, no matter how sparse the Q is.

Use minqpaddqc2() or minqpaddqc2list() if you have sparse  Q  and/or  many
constraints to handle.

IMPORTANT: ALGLIB  supports  arbitrary  quadratic  constraints,  including
           nonconvex ones. However, only convex constraints (combined with
           the convex objective) result in guaranteed convergence  to  the
           global minimizer. In all other cases, only local convergence to
           a local minimum is guaranteed.

           A convex constraint is a  constraint  of  the  following  form:
           b'*(x-origin) + 0.5(x-origin)'*Q*(x-origin) <=CU, with Q  being
           a semidefinite matrix. All other modifications  are  nonconvex:
           * -x0^2<=1 is nonconvex
           *  x0^2>=1 is nonconvex (despite Q=1 being positive definite)
           *  x0^2 =1 is nonconvex

           The latter case is notable because it effectively converts a QP
           problem into a mixed integer QP program. Smooth interior  point
           solver can not efficiently handle such programs, converging  to
           a randomly chosen x0 (either +1 or -1) and  keeping  its  value
           fixed during the optimization.

           It is also  notable  that  larger  equality  constraints  (e.g.
           x0^2+x1^2=1) are much less difficult  to  handle  because  they
           form large connected regions within the parameters space.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    Q       -   array[N,N], symmetric matrix Q:
                * if IsUpper=True, then the upper triangle  is  given, and
                  the lower triangle is ignored
                * if IsUpper=False, then the lower triangle is  given, and
                  the upper triangle is ignored
                * if more than N rows/cols are present, only leading N
                  elements are used
    IsUpper -   whether upper or lower triangle of Q is used
    B       -   array[N], linear term
    CL, CU  -   lower and upper bounds:
                * CL can be finite or -INF (absence of a bound)
                * CU can be finite or +INF (absence of a bound)
                * CL<=CU, with CL=CU meaning an equality constraint
                * CL=-INF, CU=+INF => constraint is ignored
    ApplyOrigin-whether origin (as specified by minqpsetorigin) is applied
                to the constraint or not. If no origin was specified, this
                parameter has no effect.

RESULT:
    constraint index, starting from 0

  -- ALGLIB --
     Copyright 19.06.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t minqpaddqc2dense(minqpstate &state, const real_2d_array &q, const bool isupper, const real_1d_array &b, const double cl, const double cu, const bool applyorigin, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function clears the list of conic constraints. Other  constraints are
not modified.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.

  -- ALGLIB --
     Copyright 19.06.2024 by Bochkanov Sergey
*************************************************************************/
void minqpclearcc(minqpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends a primitive second-order  conic  constraint  of  the
form

        (                                                  )
    sqrt(x[range0]^2 + x[range0+1]^2 + ... + x[range1-1]^2 ) <= x[axisidx]
        (                                                  )

with 'primitive' meaning that there are no per-variable  scales  and  that
variables under the square root have sequential indexes. More general form
of conic constraints can be specified with minqpaddsoccorthogonal().

Alternatively, if ApplyOrigin parameter  is  True,  x[i]  is  replaced  by
x[i]-origin[i] (applies to all variables).

Unlike  many  other  conic  solvers,  ALGLIB  allows  conic constraints to
overlap, i.e. it allows  a  variable  to  be  a  part  of  multiple  conic
constraints.

NOTE: second-order conic constraints are always  convex,  so  having  them
      preserves convexity of the QP problem.

NOTE: A starting point that is strictly feasible with respect to both  box
      and conic constraints greatly helps the solver to power up; however,
      it will work even without such a point,  albeit  at  somewhat  lower
      performance.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    Range0,
    Range1  -   0<=range0<=range1<=N, variable range for the LHS;
                * squared variables x[range0]...x[range1-1] are summed up
                  under the square root.
                * range0=range1 means that the constraint is interpreted
                  as x[AxisIdx]>=0.
    AxisIdx -   RHS variable index:
                * 0<=AxisIdx<N
                * either AxisIdx<range0 or AxisAdx>=Range1.

RESULT:
    constraint index in a conic constraints list, starting from 0

  -- ALGLIB --
     Copyright 09.09.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t minqpaddsoccprimitive(minqpstate &state, const ae_int_t range0, const ae_int_t range1, const ae_int_t axisidx, const bool applyorigin, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends an axis-orthogonal second-order  conic constraint of
the form

         ( k-2 (                     )^2          )
    sqrt ( SUM ( a[i]*x[idx[i]]+c[i] )  + theta^2 ) <= a[k-1]*x[idx[k-1]]+c[k-1]
         ( i=0 (                     )            )

Alternatively, if ApplyOrigin parameter  is  True,  x[i]  is  replaced  by
x[i]-origin[i] (applies to all variables).

Unlike many other conic solvers, ALGLIB provides a flexible conic API that
allows a[] to have zero elements at arbitrary positions (e.g.,  |x|<=const
can be handled just as easy as |x|<=y). Furthermore, ALGLIB  allows  conic
constraints to overlap, i.e. it allows a variable to be a part of multiple
conic constraints, or to appear multiple times in the same constraint.

NOTE: second-order conic constraints are always  convex,  so  having  them
      preserves convexity of the QP problem.

NOTE: A starting point that is strictly feasible with respect to both  box
      and conic constraints greatly helps the solver to power up; however,
      it will work even without such a point,  albeit  at  somewhat  lower
      performance.

INPUT PARAMETERS:
    State   -   structure previously allocated  with  minqpcreate()  call.
    Idx     -   array[K] (or larger, only leading  K  elements  are  used)
                storing variable indexes. Indexes can be  unsorted  and/or
                non-distinct.
    A       -   array[K]  (or larger, only leading  K  elements are used),
                variable multipliers. Can contain zero values.
    C       -   array[K] (or larger, only leading  K  elements  are used),
                variable shifts.
    K       -   cone dimensionality, K>=1. It is possible to have K>N.
    Theta   -   additional constant term, can be zero

RESULT:
    constraint index in a conic constraints list, starting from 0

  -- ALGLIB --
     Copyright 09.09.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t minqpaddsoccorthogonal(minqpstate &state, const integer_1d_array &idx, const real_1d_array &a, const real_1d_array &c, const ae_int_t k, const double theta, const bool applyorigin, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends a primitive power cone constraint of the form

        (                                                  )
    sqrt(x[range0]^2 + x[range0+1]^2 + ... + x[range1-1]^2 ) <= x[axisidx]^alpha
        (                                                  )

or, written in another form,

    (    (                                                  ))^(1/alpha)
    (sqrt(x[range0]^2 + x[range0+1]^2 + ... + x[range1-1]^2 ))           <= x[axisidx]
    (    (                                                  ))

where

    0<alpha<=1

with 'primitive' meaning that there are no per-variable  scales  and  that
variables under the square root have sequential indexes. More general form
of power cone constraints can be specified with minqpaddpowccorthogonal().

Alternatively, if ApplyOrigin parameter  is  True,  x[i]  is  replaced  by
x[i]-origin[i] (applies to all variables).

Unlike many other conic solvers, ALGLIB provides a flexible conic API that
allows alpha to be any positive value less than or equal to 1 (e.g.  it is
possible to formulate |x|<z^0.33 without using  slack vars that are  fixed
at 1.0). Furthermore, ALGLIB allows conic constraints to overlap, i.e.  it
allows a variable to be a part of multiple conic constraints.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    Range0,
    Range1  -   0<=range0<=range1<=N, variable range for the LHS;
                * squared variables x[range0]...x[range1-1] are summed up
                  under the square root.
                * range0=range1 means that the constraint is interpreted
                  as x[AxisIdx]>=0.
    AxisIdx -   RHS variable index:
                * 0<=AxisIdx<N
                * either AxisIdx<range0 or AxisAdx>=Range1.
    Alpha   -   power parameter, 0<alpha<=1

RESULT:
    constraint index in a conic constraints list, starting from 0

NOTE: power cone constraints are always  convex,  so having them preserves
      convexity of the QP problem.

NOTE: A starting point that is strictly feasible with respect to both  box
      and conic constraints greatly helps the solver to power up; however,
      it will work even without such a point,  albeit  at  somewhat  lower
      performance.

NOTE: a power cone with alpha<1 is sensitive to numerical errors near  the
      origin.  Suppose,  for  example,  that  we  have a constraint of the
      form |y|<=x^alpha with alpha=0.25 and that x is zero at the solution
      Furthermore, suppose that we perturbed x by as little  as  eps=1E-8.
      Because of alpha=0.25 the constraint is  perturbed  by  as  much  as
      eps^(1/4)=0.01!

      Such great sensitivity is  explained  by  the  non-differentiability
      of x^alpha for alpha<1 near x=0.  It  does  not  prevent  the  conic
      solver  from  converging  to   the precise solution, it merely makes
      constraints  extremely  sensitive  to  small   errors,   e.g.   ones
      introduced during the presolve/postsolve, like discussed below.

      The conic solver sometimes has to insert slack variables, most often
      because of constraints referring the same right-hand  side  variable
      twice.   For   example,  for  sqrt(x0^2+x1^2)<=(y0-1)^0.5*(y0+1)^0.5
      it will automatically add t=y0+1 (a linear equality constraint)  and
      will rewrite the  constraint  as  sqrt(x0^2+x1^2)<=(y0-1)^0.5*t^0.5.
      The solver will have no  difficulty  solving  the  problem,  even if
      the optimal t is close to zero. However, the equality y0=t-1 will be
      satisfied only approximately, and even a tiny error will be  greatly
      magnified when evaluating the constraint violation.

      Thus,   one  has  to  be  very  careful  when  evaluating constraint
      violation errors  for  power  cones.  Having  high  error  does  not
      necessarily mean that the solver has failed.

  -- ALGLIB --
     Copyright 19.11.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t minqpaddpowccprimitive(minqpstate &state, const ae_int_t range0, const ae_int_t range1, const ae_int_t axisidx, const double alpha, const bool applyorigin, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends an axis-orthogonal power cone constraint of the form

         (          k-kp-1      )     k-1
    sqrt ( theta^2 + SUM y[i]^2 ) <=  MUL y[i]^alpha[i]
         (           i=0        )    i=k-kp

where

    y[i] = a[i]*x[idx[i]]+c[i], y[i]>=0

    0<alpha[i]<1, with SUM(alpha[i])<=1

    1<=kp<=k, with kp=k meaning that we have MUL(y[i]^alpha[i])>=|theta|

Alternatively, if ApplyOrigin parameter  is  True,  x[i]  is  replaced  by
x[i]-origin[i] (applies to all variables).

Unlike many other conic solvers, ALGLIB provides a flexible conic API that
allows alpha[] to sum up to any positive value less than  or  equal  to  1
(e.g. it is possible to formulate |x|<z^0.33 without  using  slack  vars).
Furthermore, ALGLIB allows conic constraints to overlap, i.e. it allows  a
variable to be a part of multiple conic constraints, or to appear multiple
times in the same constraint.

INPUT PARAMETERS:
    State   -   structure previously allocated  with  minqpcreate()  call.
    Idx     -   array[K] (or larger, only leading  K  elements  are  used)
                storing variable indexes. Indexes can be  unsorted  and/or
                non-distinct.
    A       -   array[K]  (or larger, only leading  K  elements are used),
                variable multipliers. Can contain zero values.
    C       -   array[K] (or larger, only leading  K  elements  are used),
                variable shifts.
    K       -   cone dimensionality, K>=1. It is possible to have K>N.
    Theta   -   additional constant term, can be zero
    AlphaV  -   array[KPow], power coefficients:
                * 0<AlphaV[i]<=1
                * 0<SUM(AlphaV[])<=1
    KPow    -   1<=KPow<=K, with KPow=K being correctly handled.

RESULT:
    constraint index in a conic constraints list, starting from 0

NOTE: power cone constraints are always  convex,  so having them preserves
      convexity of the QP problem.

NOTE: A starting point that is strictly feasible with respect to both  box
      and conic constraints greatly helps the solver to power up; however,
      it will work even without such a point,  albeit  at  somewhat  lower
      performance.

NOTE: a power cone with alpha<1 is sensitive to numerical errors near  the
      origin.  Suppose,  for  example,  that  we  have a constraint of the
      form |y|<=x^alpha with alpha=0.25 and that x is zero at the solution
      Furthermore, suppose that we perturbed x by as little  as  eps=1E-8.
      Because of alpha=0.25 the constraint is  perturbed  by  as  much  as
      eps^(1/4)=0.01!

      Such great sensitivity is  explained  by  the  non-differentiability
      of x^alpha for alpha<1 near x=0.  It  does  not  prevent  the  conic
      solver  from  converging  to   the precise solution, it merely makes
      constraints  extremely  sensitive  to  small   errors,   e.g.   ones
      introduced during the presolve/postsolve, like discussed below.

      The conic solver sometimes has to insert slack variables, most often
      because of constraints referring the same right-hand  side  variable
      twice.   For   example,  for  sqrt(x0^2+x1^2)<=(y0-1)^0.5*(y0+1)^0.5
      it will automatically add t=y0+1 (a linear equality constraint)  and
      will rewrite the  constraint  as  sqrt(x0^2+x1^2)<=(y0-1)^0.5*t^0.5.
      The solver will have no  difficulty  solving  the  problem,  even if
      the optimal t is close to zero. However, the equality y0=t-1 will be
      satisfied only approximately, and even a tiny error will be  greatly
      magnified when evaluating the constraint violation.

      Thus,   one  has  to  be  very  careful  when  evaluating constraint
      violation errors  for  power  cones.  Having  high  error  does  not
      necessarily mean that the solver has failed.

  -- ALGLIB --
     Copyright 09.09.2024 by Bochkanov Sergey
*************************************************************************/
ae_int_t minqpaddpowccorthogonal(minqpstate &state, const integer_1d_array &idx, const real_1d_array &a, const real_1d_array &c, const ae_int_t k, const double theta, const real_1d_array &alphav, const ae_int_t kpow, const bool applyorigin, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function solves quadratic programming problem.

Prior to calling this function you should choose solver by means of one of
the following functions:

* minqpsetalgoquickqp()     - for QuickQP solver
* minqpsetalgodenseaul()    - for Dense-AUL-QP solver
* minqpsetalgodenseipm()    - for convex Dense-IPM-QP solver
* minqpsetalgosparseipm()   - for convex Sparse-IPM-QP solver
* minqpsetalgodensegenipm() - for convex/nonconvex Dense-IPM-QP solver with conic constraints
* minqpsetalgosparsegenipm()- for convex/nonconvex Sparse-IPM-QP solver with conic constraints

These functions also allow you to control stopping criteria of the solver.
If you did not set solver,  MinQP  subpackage  will  automatically  select
solver for your problem and will run it with default stopping criteria.

However, it is better to set explicitly solver and its stopping criteria.

INPUT PARAMETERS:
    State   -   algorithm state

You should use MinQPResults() function to access results after calls
to this function.

  -- ALGLIB --
     Copyright 2011-2024 by Bochkanov Sergey.
     Special thanks to Elvira Illarionova  for  important  suggestions  on
     the linearly constrained QP algorithm.
*************************************************************************/
void minqpoptimize(minqpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
QP solver results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution (on failure - the best point found
                so far).
    Rep     -   optimization report, contains:
                * completion code in Rep.TerminationType (positive  values
                  denote some kind of success, negative - failures)
                * Lagrange multipliers - for QP solvers which support them
                * other statistics
                See comments on minqpreport structure for more information

Following completion codes are returned in Rep.TerminationType:
* -9    failure of the automatic scale evaluation:  one  of  the  diagonal
        elements of the quadratic term is non-positive.  Specify  variable
        scales manually!
* -5    inappropriate solver was used:
        * QuickQP solver for problem with general linear constraints
        * QuickQP/DENSE-AUL/DENSE-IPM/SPARSE-IPM for a problem with
          quadratic/conic constraints
* -4    the function is unbounded from below even under constraints,
        no meaningful minimum can be found.
* -3    inconsistent constraints (or, maybe, feasible point is too hard to
        find).
* -2    IPM solver has difficulty finding primal/dual feasible point.
        It is likely that the problem is either infeasible or unbounded,
        but it is difficult to determine exact reason for termination.
        X contains best point found so far.
*  >0   success
*  7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpresults(const minqpstate &state, real_1d_array &x, minqpreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
QP results

Buffered implementation of MinQPResults() which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpresultsbuf(const minqpstate &state, real_1d_array &x, minqpreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Export current QP problem stored in the solver into  QPXProblem  instance.
This  instance  can  be  serialized  into   ALGLIB-specific   format   and
unserialized from several widely acknowledged formats.

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    P       -   QPXProblem instance storing current objective and
                constraints.

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
void minqpexport(minqpstate &state, qpxproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Imports QP problem, as defined by QPXProblem instance, creating a QP solver
with objective/constraints/scales/origin set to that stored in the instance.

INPUT PARAMETERS:
    P       -   QPXProblem instance storing current objective and
                constraints.

OUTPUT PARAMETERS:
    State   -   newly created solver

  -- ALGLIB --
     Copyright 25.08.2024 by Bochkanov Sergey
*************************************************************************/
void minqpimport(qpxproblem &p, minqpstate &s, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINLM) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] and Jacobian of f[].


REQUIREMENTS:
This algorithm will request following information during its operation:

* function vector f[] at given point X
* function vector f[] and Jacobian of f[] (simultaneously) at given point

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec()  and jac() callbacks.
First  one  is used to calculate f[] at given point, second one calculates
f[] and Jacobian df[i]/dx[j].

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works  with  general  form function and does not provide Jacobian), but it
will  lead  to  exception  being  thrown  after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateVJ() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatevj(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);
void minlmcreatevj(const ae_int_t m, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] only. Finite differences  are  used  to
calculate Jacobian.


REQUIREMENTS:
This algorithm will request following information during its operation:
* function vector f[] at given point X

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec() callback.

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works with general form function and does not accept function vector), but
it will  lead  to  exception being thrown after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateV() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]
    DiffStep-   differentiation step, >0. By  default,  symmetric  3-point
                formula which provides good accuracy is used.  It  can  be
                changed to a faster but  less  precise  2-point  one  with
                minlmsetnumdiff() function.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

See also MinLMIteration, MinLMResults.

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatev(const ae_int_t n, const ae_int_t m, const real_1d_array &x, const double diffstep, minlmstate &state, const xparams _xparams = alglib::xdefault);
void minlmcreatev(const ae_int_t m, const real_1d_array &x, const double diffstep, minlmstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for Levenberg-Marquardt optimization
algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinLMSetScale()
                Recommended values: 1E-9 ... 1E-12.
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations   is    unlimited.   Only   Levenberg-Marquardt
                iterations  are  counted  (L-BFGS/CG  iterations  are  NOT
                counted because their cost is very low compared to that of
                LM).

Passing  EpsX=0  and  MaxIts=0  (simultaneously)  will  lead  to automatic
stopping criterion selection (small EpsX).

NOTE: it is not recommended to set large EpsX (say, 0.001). Because LM  is
      a second-order method, it performs very precise steps anyway.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetcond(minlmstate &state, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinLMOptimize(). Both Levenberg-Marquardt and internal  L-BFGS
iterations are reported.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetxrep(minlmstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

NOTE: non-zero StpMax leads to moderate  performance  degradation  because
intermediate  step  of  preconditioned L-BFGS optimization is incompatible
with limits on step size.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetstpmax(minlmstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for LM optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Generally, scale is NOT considered to be a form of preconditioner.  But LM
optimizer is unique in that it uses scaling matrix both  in  the  stopping
condition tests and as Marquardt damping factor.

Proper scaling is very important for the algorithm performance. It is less
important for the quality of results, but still has some influence (it  is
easier  to  converge  when  variables  are  properly  scaled, so premature
stopping is possible when very badly scalled variables are  combined  with
relaxed stopping conditions).

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetscale(minlmstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints for LM optimizer

Boundary constraints are inactive by default (after initial creation).
They are preserved until explicitly turned off with another SetBC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (latter is recommended because
                it will allow solver to use better algorithm).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (latter is recommended because
                it will allow solver to use better algorithm).

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by bound constraints
  or at its boundary

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetbc(minlmstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets general linear constraints for LM optimizer

Linear constraints are inactive by default (after initial creation).  They
are preserved until explicitly turned off with another minlmsetlc() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

IMPORTANT: if you have linear constraints, it is strongly  recommended  to
           set scale of variables with minlmsetscale(). QP solver which is
           used to calculate linearly constrained steps heavily relies  on
           good scaling of input problems.

IMPORTANT: solvers created with minlmcreatefgh()  do  not  support  linear
           constraints.

NOTE: linear  (non-bound)  constraints are satisfied only approximately  -
      there  always  exists some violation due  to  numerical  errors  and
      algorithmic limitations.

NOTE: general linear constraints  add  significant  overhead  to  solution
      process. Although solver performs roughly same amount of  iterations
      (when compared  with  similar  box-only  constrained  problem), each
      iteration   now    involves  solution  of  linearly  constrained  QP
      subproblem, which requires ~3-5 times more Cholesky  decompositions.
      Thus, if you can reformulate your problem in such way  this  it  has
      only box constraints, it may be beneficial to do so.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetlc(minlmstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minlmsetlc(minlmstate &state, const real_2d_array &c, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function is used to change acceleration settings

You can choose between three acceleration strategies:
* AccType=0, no acceleration.
* AccType=1, secant updates are used to update quadratic model after  each
  iteration. After fixed number of iterations (or after  model  breakdown)
  we  recalculate  quadratic  model  using  analytic  Jacobian  or  finite
  differences. Number of secant-based iterations depends  on  optimization
  settings: about 3 iterations - when we have analytic Jacobian, up to 2*N
  iterations - when we use finite differences to calculate Jacobian.

AccType=1 is recommended when Jacobian  calculation  cost is prohibitively
high (several Mx1 function vector calculations  followed  by  several  NxN
Cholesky factorizations are faster than calculation of one M*N  Jacobian).
It should also be used when we have no Jacobian, because finite difference
approximation takes too much time to compute.

Table below list  optimization  protocols  (XYZ  protocol  corresponds  to
MinLMCreateXYZ) and acceleration types they support (and use by  default).

ACCELERATION TYPES SUPPORTED BY OPTIMIZATION PROTOCOLS:

protocol    0   1   comment
V           +   +
VJ          +   +
FGH         +

DEFAULT VALUES:

protocol    0   1   comment
V               x   without acceleration it is so slooooooooow
VJ          x
FGH         x

NOTE: this  function should be called before optimization. Attempt to call
it during algorithm iterations may result in unexpected behavior.

NOTE: attempt to call this function with unsupported protocol/acceleration
combination will result in exception being thrown.

  -- ALGLIB --
     Copyright 14.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetacctype(minlmstate &state, const ae_int_t acctype, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function is used  to  activate/deactivate  nonmonotonic  steps.  Such
steps may improve  convergence  on  noisy  problems  or  ones  with  minor
smoothness defects.

In its standard mode, LM solver compares value at the trial   point   f[1]
with the value at the current point f[0]. Only steps that decrease f() are
accepted.

When the nonmonotonic mode is activated, f[1]  is  compared  with  maximum
over several previous  locations:  max(f[0],f[-1],...,f[-CNT]).  We  still
accept only steps that decrease  f(),  however  our  reference  value  has
changed. The net results is that f[1]>f[0] are now allowed.

Nonmonotonic steps can help to handle minor defects in the objective (e.g.
small  noise,  discontinuous  jumps  or  nonsmoothness).  However,  it  is
important  that  the  overall  shape  of  the  problem  is  still  smooth.
It  may  also  help  to  minimize  perfectly  smooth  targets with complex
geometries by allowing to jump through curved valleys.

However, sometimes nonmonotonic steps degrade convergence by  allowing  an
optimizer to wander too far away from the solution, so this feature should
be used only after careful testing.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    Cnt     -   nonmonotonic memory length, Cnt>=0:
                * 0 for traditional monotonic steps
                * 2..3 is recommended for the nonmonotonic optimization

  -- ALGLIB --
     Copyright 07.04.2024 by Bochkanov Sergey
*************************************************************************/
void minlmsetnonmonotonicsteps(minlmstate &state, const ae_int_t cnt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets specific finite  difference  formula  to  be  used  for
numerical differentiation.

It works only for optimizers created  with  minlmcreatev()  function;   in
other cases it has no effect.

INPUT PARAMETERS:
    State       -   structure previously allocated with MinLMCreateV() call.
    FormulaType -   formula type:
                    * 3 for a 3-point formula, which is also  known  as  a
                      symmetric difference quotient (the formula  actually
                      uses only two function values per variable:  at  x+h
                      and x-h). A good choice for medium-accuracy  setups,
                      a default option.
                    * 2 for a forward (or backward, depending  on variable
                      bounds)  finite   difference  (f(x+h)-f(x))/h.  This
                      formula has the lowest accuracy. However, it  is  4x
                      faster than the 5-point formula and 2x  faster  than
                      the 3-point one because, in addition to the  central
                      value f(x), it needs only  one  additional  function
                      evaluation per variable.


  -- ALGLIB --
     Copyright 03.12.2024 by Bochkanov Sergey
*************************************************************************/
void minlmsetnumdiff(minlmstate &state, const ae_int_t formulatype, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minlmiteration(minlmstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    fvec    -   callback which calculates function vector fi[]
                at given point x
    jac     -   callback which calculates function vector fi[]
                and Jacobian jac at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


CALLBACK PARALLELISM

The MINLM optimizer supports parallel parallel  numerical  differentiation
('callback parallelism'). This feature, which  is  present  in  commercial
ALGLIB  editions,  greatly   accelerates   optimization   with   numerical
differentiation of an expensive target functions.

Callback parallelism is usually  beneficial  when  computing  a  numerical
gradient requires more than several  milliseconds.  In this case  the  job
of computing individual gradient components can be split between  multiple
threads. Even inexpensive targets can benefit  from  parallelism,  if  you
have many variables.

If you solve a curve fitting problem, i.e. the function vector is actually
the same function computed at different points of a data points space,  it
may  be  better  to  use  an LSFIT curve fitting solver, which offers more
fine-grained parallelism due to knowledge of  the  problem  structure.  In
particular, it can accelerate both numerical differentiation  and problems
with user-supplied gradients.

ALGLIB Reference Manual, 'Working with commercial  version' section, tells
how to activate callback parallelism for your programming language.

  -- ALGLIB --
     Copyright 03.12.2023 by Bochkanov Sergey


*************************************************************************/
void minlmoptimize(minlmstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minlmoptimize(minlmstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic Jacobian.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function vector  at  the  initial
point (note: future versions may also perform check  at  the final  point)
and compares numerical Jacobian with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which stores  both  Jacobians,  and  specific  components  highlighted  as
suspicious by the OptGuard.

The OptGuard report can be retrieved with minlmoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with minlmsetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minlmoptguardgradient(minlmstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

OptGuard checks analytic Jacobian  against  reference  value  obtained  by
numerical differentiation with user-specified step.

NOTE: other optimizers perform additional OptGuard checks for things  like
      C0/C1-continuity violations. However, LM optimizer  can  check  only
      for incorrect Jacobian.

      The reason is that unlike line search methods LM optimizer does  not
      perform extensive evaluations along the line. Thus, we simply do not
      have enough data to catch C0/C1-violations.

This check is activated with  minlmoptguardgradient() function.

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradfidx for specific function (Jacobian row) suspected
  * rep.badgradvidx for specific variable (Jacobian column) suspected
  * rep.badgradxbase, a point where gradient/Jacobian is tested
  * rep.badgraduser, user-provided gradient/Jacobian
  * rep.badgradnum, reference gradient/Jacobian obtained via numerical
    differentiation

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   OptGuard report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlmoptguardresults(minlmstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Levenberg-Marquardt algorithm results

NOTE: if you activated OptGuard integrity checking functionality and  want
      to get OptGuard report,  it  can  be  retrieved  with  the  help  of
      minlmoptguardresults() function.

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization  report;  includes  termination   codes   and
                additional information. Termination codes are listed below,
                see comments for this structure for more info.

                Termination code is stored in rep.terminationtype field:
                * -8    optimizer detected NAN/INF values either in the
                        function itself, or in its Jacobian
                * -3    constraints are inconsistent
                *  2    relative step is no more than EpsX.
                *  5    MaxIts steps was taken
                *  7    stopping conditions are too stringent,
                        further improvement is impossible
                *  8    terminated by user who called minlmrequesttermination().
                        X contains point which was "current accepted" when
                        termination request was submitted.

                rep.f contains SUM(f[i]^2) at X

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresults(const minlmstate &state, real_1d_array &x, minlmreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Levenberg-Marquardt algorithm results

Buffered implementation of MinLMResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresultsbuf(const minlmstate &state, real_1d_array &x, minlmreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  subroutine  restarts  LM  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used for reverse communication previously
                allocated with MinLMCreateXXX call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minlmrestartfrom(minlmstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void minlmrequesttermination(minlmstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_REVISEDDUALSIMPLEX) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_LPSOLVERS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                            LINEAR PROGRAMMING

The subroutine creates LP  solver.  After  initial  creation  it  contains
default optimization problem with zero cost vector and all variables being
fixed to zero values and no constraints.

In order to actually solve something you should:
* set cost vector with minlpsetcost()
* set variable bounds with minlpsetbc() or minlpsetbcall()
* specify constraint matrix with one of the following functions:
  [*] minlpsetlc()        for dense one-sided constraints
  [*] minlpsetlc2dense()  for dense two-sided constraints
  [*] minlpsetlc2()       for sparse two-sided constraints
  [*] minlpaddlc2dense()  to add one dense row to constraint matrix
  [*] minlpaddlc2()       to add one row to constraint matrix (compressed format)
* call minlpoptimize() to run the solver and  minlpresults()  to  get  the
  solution vector and additional information.

By  default,  LP  solver uses best algorithm available. As of ALGLIB 3.17,
sparse interior point (barrier) solver is used. Future releases of  ALGLIB
may introduce other solvers.

User may choose specific LP algorithm by calling:
* minlpsetalgodss() for revised dual simplex method with DSE  pricing  and
  bounds flipping ratio test (aka long dual step).  Large-scale  sparse LU
  solverwith  Forest-Tomlin update is used internally  as  linear  algebra
  driver.
* minlpsetalgoipm() for sparse interior point method

INPUT PARAMETERS:
    N       -   problem size

OUTPUT PARAMETERS:
    State   -   optimizer in the default state

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpcreate(const ae_int_t n, minlpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets LP algorithm to revised dual simplex method.

ALGLIB implementation of dual simplex method supports advanced performance
and stability improvements like DSE pricing , bounds flipping  ratio  test
(aka long dual step), Forest-Tomlin update, shifting.

INPUT PARAMETERS:
    State   -   optimizer
    Eps     -   stopping condition, Eps>=0:
                * should be small number about 1E-6 or 1E-7.
                * zero value means that solver automatically selects good
                  value (can be different in different ALGLIB versions)
                * default value is zero
                Algorithm stops when relative error is less than Eps.

===== TRACING DSS SOLVER =================================================

DSS solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'DSS'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'DSS.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'DSS'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("DSS,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 08.11.2020 by Bochkanov Sergey
*************************************************************************/
void minlpsetalgodss(minlpstate &state, const double eps, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets LP algorithm to sparse interior point method.

ALGORITHM INFORMATION:

* this  algorithm  is  our implementation  of  interior  point  method  as
  formulated by  R.J.Vanderbei, with minor modifications to the  algorithm
  (damped Newton directions are extensively used)
* like all interior point methods, this algorithm  tends  to  converge  in
  roughly same number of iterations (between 15 and 50) independently from
  the problem dimensionality

INPUT PARAMETERS:
    State   -   optimizer
    Eps     -   stopping condition, Eps>=0:
                * should be small number about 1E-6 or 1E-8.
                * zero value means that solver automatically selects good
                  value (can be different in different ALGLIB versions)
                * default value is zero
                Algorithm  stops  when  primal  error  AND  dual error AND
                duality gap are less than Eps.

===== TRACING IPM SOLVER =================================================

IPM solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'IPM'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'IPM.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'IPM'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("IPM,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 08.11.2020 by Bochkanov Sergey
*************************************************************************/
void minlpsetalgoipm(minlpstate &state, const double eps, const xparams _xparams = alglib::xdefault);
void minlpsetalgoipm(minlpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets cost term for LP solver.

By default, cost term is zero.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    C       -   cost term, array[N].

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetcost(minlpstate &state, const real_1d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients.

ALGLIB optimizers use scaling matrices to test stopping  conditions and as
preconditioner.

Scale of the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the
   function

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetscale(minlpstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for LP solver (all variables  at  once,
different constraints for different variables).

The default state of constraints is to have all variables fixed  at  zero.
You have to overwrite it by your own constraint vector. Constraint  status
is preserved until constraints are  explicitly  overwritten  with  another
minlpsetbc()  call,   overwritten   with  minlpsetbcall(),  or   partially
overwritten with minlmsetbci() call.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
    BndU    -   upper bounds, array[N].

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: if constraints for all variables are same you may use minlpsetbcall()
      which allows to specify constraints without using arrays.

NOTE: BndL>BndU will result in LP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetbc(minlpstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for LP solver (all variables  at  once,
same constraints for all variables)

The default state of constraints is to have all variables fixed  at  zero.
You have to overwrite it by your own constraint vector. Constraint  status
is preserved until constraints are  explicitly  overwritten  with  another
minlpsetbc() call or partially overwritten with minlpsetbcall().

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bound, same for all variables
    BndU    -   upper bound, same for all variables

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: minlpsetbc() can  be  used  to  specify  different  constraints  for
      different variables.

NOTE: BndL>BndU will result in LP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetbcall(minlpstate &state, const double bndl, const double bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for I-th variable (other variables are
not modified).

The default state of constraints is to have all variables fixed  at  zero.
You have to overwrite it by your own constraint vector.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    I       -   variable index, in [0,N)
    BndL    -   lower bound for I-th variable
    BndU    -   upper bound for I-th variable

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: minlpsetbc() can  be  used  to  specify  different  constraints  for
      different variables.

NOTE: BndL>BndU will result in LP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetbci(minlpstate &state, const ae_int_t i, const double bndl, const double bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets one-sided linear constraints A*x ~ AU, where "~" can be
a mix of "<=", "=" and ">=".

IMPORTANT: this function is provided here for compatibility with the  rest
           of ALGLIB optimizers which accept constraints  in  format  like
           this one. Many real-life problems feature two-sided constraints
           like a0 <= a*x <= a1. It is really inefficient to add them as a
           pair of one-sided constraints.

           Use minlpsetlc2dense(), minlpsetlc2(), minlpaddlc2()  (or   its
           sparse version) wherever possible.

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
    A       -   linear constraints, array[K,N+1]. Each row of A represents
                one constraint, with first N elements being linear coefficients,
                and last element being right side.
    CT      -   constraint types, array[K]:
                * if CT[i]>0, then I-th constraint is A[i,*]*x >= A[i,n]
                * if CT[i]=0, then I-th constraint is A[i,*]*x  = A[i,n]
                * if CT[i]<0, then I-th constraint is A[i,*]*x <= A[i,n]
    K       -   number of equality/inequality constraints,  K>=0;  if  not
                given, inferred from sizes of A and CT.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetlc(minlpstate &state, const real_2d_array &a, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minlpsetlc(minlpstate &state, const real_2d_array &a, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU.

This version accepts dense matrix as  input;  internally  LP  solver  uses
sparse storage  anyway  (most  LP  problems  are  sparse),  but  for  your
convenience it may accept dense inputs. This  function  overwrites  linear
constraints set by previous calls (if such calls were made).

We recommend you to use sparse version of this function unless  you  solve
small-scale LP problem (less than few hundreds of variables).

NOTE: there also exist several versions of this function:
      * one-sided dense version which  accepts  constraints  in  the  same
        format as one used by QP and  NLP solvers
      * two-sided sparse version which accepts sparse matrix
      * two-sided dense  version which allows you to add constraints row by row
      * two-sided sparse version which allows you to add constraints row by row

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
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
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetlc2dense(minlpstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minlpsetlc2dense(minlpstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
sparse constraining matrix A. Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
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
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetlc2(minlpstate &state, const sparsematrix &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present constraints.

This version accepts dense constraint vector as input, but  sparsifies  it
for internal storage and processing. Thus, time to add one  constraint  in
is O(N) - we have to scan entire array of length N. Sparse version of this
function is order of magnitude faster for  constraints  with  just  a  few
nonzeros per row.

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
    A       -   linear constraint coefficient, array[N], right side is NOT
                included.
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint Ai*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint Ai*x<=AU
                * AU=+INF  => one-sided constraint AL<=Ai*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpaddlc2dense(minlpstate &state, const real_1d_array &a, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present constraints.

Constraint is passed in compressed format: as list of non-zero entries  of
coefficient vector A. Such approach is more efficient than  dense  storage
for highly sparse constraint vectors.

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
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
void minlpaddlc2(minlpstate &state, const integer_1d_array &idxa, const real_1d_array &vala, const ae_int_t nnz, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function solves LP problem.

INPUT PARAMETERS:
    State   -   algorithm state

You should use minlpresults() function to access results  after  calls  to
this function.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey.
*************************************************************************/
void minlpoptimize(minlpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
LP solver results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution (on failure: last trial point)
    Rep     -   optimization report. You should check Rep.TerminationType,
                which contains completion code, and you may check  another
                fields which contain another information  about  algorithm
                functioning.

                Failure codes returned by algorithm are:
                * -4    LP problem is primal unbounded (dual infeasible)
                * -3    LP problem is primal infeasible (dual unbounded)
                * -2    IPM solver detected that problem is either
                        infeasible or unbounded

                Success codes:
                *  1..4 successful completion
                *  5    MaxIts steps was taken

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlpresults(const minlpstate &state, real_1d_array &x, minlpreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
LP results

Buffered implementation of MinLPResults() which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlpresultsbuf(const minlpstate &state, real_1d_array &x, minlpreport &rep, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINCG) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
        NONLINEAR CONJUGATE GRADIENT METHOD

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by using one of  the
nonlinear conjugate gradient methods.

These CG methods are globally convergent (even on non-convex functions) as
long as grad(f) is Lipschitz continuous in  a  some  neighborhood  of  the
L = { x : f(x)<=f(x0) }.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinCGCreate() call
2. User tunes solver parameters with MinCGSetCond(), MinCGSetStpMax() and
   other functions
3. User calls MinCGOptimize() function which takes algorithm  state   and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinCGResults() to get solution
5. Optionally, user may call MinCGRestartFrom() to solve another  problem
   with same N but another starting point and/or another function.
   MinCGRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void mincgcreate(const ae_int_t n, const real_1d_array &x, mincgstate &state, const xparams _xparams = alglib::xdefault);
void mincgcreate(const real_1d_array &x, mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
The subroutine is finite difference variant of MinCGCreate(). It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this function
only. We recommend to read comments on MinCGCreate() in order to get more
information about creation of CG optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinCGSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust  and  precise.  L-BFGS  needs  exact  gradient values.
   Imprecise  gradient may slow down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void mincgcreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, mincgstate &state, const xparams _xparams = alglib::xdefault);
void mincgcreatef(const real_1d_array &x, const double diffstep, mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for CG optimization algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinCGSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinCGSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetcond(mincgstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for CG optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of CG optimizer  -  step
along I-th axis is equal to DiffStep*S[I].

In   most   optimizers  (and  in  the  CG  too)  scaling is NOT a form  of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner by separate call to one of the MinCGSetPrec...() functions.

There  is  special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void mincgsetscale(mincgstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetxrep(mincgstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets CG algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    CGType  -   algorithm type:
                * -1    automatic selection of the best algorithm
                * 0     DY (Dai and Yuan) algorithm
                * 1     Hybrid DY-HS algorithm

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetcgtype(mincgstate &state, const ae_int_t cgtype, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetstpmax(mincgstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function allows to suggest initial step length to the CG algorithm.

Suggested  step  length  is used as starting point for the line search. It
can be useful when you have  badly  scaled  problem,  i.e.  when  ||grad||
(which is used as initial estimate for the first step) is many  orders  of
magnitude different from the desired step.

Line search  may  fail  on  such problems without good estimate of initial
step length. Imagine, for example, problem with ||grad||=10^50 and desired
step equal to 0.1 Line  search function will use 10^50  as  initial  step,
then  it  will  decrease step length by 2 (up to 20 attempts) and will get
10^44, which is still too large.

This function allows us to tell than line search should  be  started  from
some moderate step length, like 1.0, so algorithm will be able  to  detect
desired step length in a several searches.

Default behavior (when no step is suggested) is to use preconditioner,  if
it is available, to generate initial estimate of step length.

This function influences only first iteration of algorithm. It  should  be
called between MinCGCreate/MinCGRestartFrom() call and MinCGOptimize call.
Suggested step is ignored if you have preconditioner.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state.
    Stp     -   initial estimate of the step length.
                Can be zero (no estimate).

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsuggeststep(mincgstate &state, const double stp, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdefault(mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2: D[i] should be positive. Exception will be thrown otherwise.

NOTE 3: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdiag(mincgstate &state, const real_1d_array &d, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables with MinCGSetScale() call
(before or after MinCGSetPrecScale() call). Without knowledge of the scale
of your variables scale-based preconditioner will be just unit matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecscale(mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool mincgiteration(mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    func    -   callback which calculates function (or merit function)
                value func at given point x
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL

NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied  gradient, and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either MinCGCreate()  for analytical gradient  or  MinCGCreateF()  for
   numerical differentiation) you should  choose  appropriate  variant  of
   MinCGOptimize() - one which accepts function AND gradient or one  which
   accepts function ONLY.

   Be careful to choose variant of MinCGOptimize()  which  corresponds  to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed  to  MinCGOptimize()  and  specific
   function used to create optimizer.


                  |         USER PASSED TO MinCGOptimize()
   CREATED WITH   |  function only   |  function and gradient
   ------------------------------------------------------------
   MinCGCreateF() |     work                FAIL
   MinCGCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function and MinCGOptimize() version. Attemps to use  such  combination
   (for  example,  to create optimizer with  MinCGCreateF()  and  to  pass
   gradient information to MinCGOptimize()) will lead to  exception  being
   thrown. Either  you  did  not  pass  gradient when it WAS needed or you
   passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey


*************************************************************************/
void mincgoptimize(mincgstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void mincgoptimize(mincgstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function  at  the  initial  point
(note: future versions may also perform check  at  the  final  point)  and
compares numerical gradient with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which  stores  both  gradients  and  specific  components  highlighted  as
suspicious by the OptGuard.

The primary OptGuard report can be retrieved with mincgoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with mincgsetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void mincgoptguardgradient(mincgstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0)
b) nonsmooth     target function (non-C1)

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.

      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.

      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.

      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.

=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void mincgoptguardsmoothness(mincgstate &state, const ae_int_t level, const xparams _xparams = alglib::xdefault);
void mincgoptguardsmoothness(mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* mincgoptguardgradient() for gradient verification
* mincgoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradvidx for specific variable (gradient element) suspected
  * rep.badgradxbase, a point where gradient is tested
  * rep.badgraduser, user-provided gradient  (stored  as  2D  matrix  with
    single row in order to make  report  structure  compatible  with  more
    complex optimizers like MinNLC or MinLM)
  * rep.badgradnum,   reference    gradient    obtained    via   numerical
    differentiation (stored as  2D matrix with single row in order to make
    report structure compatible with more complex optimizers  like  MinNLC
    or MinLM)
* rep.nonc0suspected
* rep.nonc1suspected

=== ADDITIONAL REPORTS/LOGS ==============================================

Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* mincgoptguardnonc1test0results()
* mincgoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.

      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.

      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void mincgoptguardresults(mincgstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void mincgoptguardnonc1test0results(const mincgstate &state, optguardnonc1test0report &strrep, optguardnonc1test0report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void mincgoptguardnonc1test1results(mincgstate &state, optguardnonc1test1report &strrep, optguardnonc1test1report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Conjugate gradient results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -8    internal integrity control  detected  infinite
                            or NAN values in  function/gradient.  Abnormal
                            termination signalled.
                    * -7    gradient verification failed.
                            See MinCGSetGradientCheck() for more information.
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible,
                            we return best X found so far
                    *  8    terminated by user
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
void mincgresults(const mincgstate &state, real_1d_array &x, mincgreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Conjugate gradient results

Buffered implementation of MinCGResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
void mincgresultsbuf(const mincgstate &state, real_1d_array &x, mincgreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  subroutine  restarts  CG  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void mincgrestartfrom(mincgstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void mincgrequesttermination(mincgstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINCOMP) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Obsolete function, use MinLBFGSSetPrecDefault() instead.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetdefaultpreconditioner(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete function, use MinLBFGSSetCholeskyPreconditioner() instead.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetcholeskypreconditioner(minlbfgsstate &state, const real_2d_array &p, const bool isupper, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is obsolete function which was used by previous version of the  BLEIC
optimizer. It does nothing in the current version of BLEIC.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbarrierwidth(minbleicstate &state, const double mu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is obsolete function which was used by previous version of the  BLEIC
optimizer. It does nothing in the current version of BLEIC.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbarrierdecay(minbleicstate &state, const double mudecay, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void minasacreate(const ae_int_t n, const real_1d_array &x, const real_1d_array &bndl, const real_1d_array &bndu, minasastate &state, const xparams _xparams = alglib::xdefault);
void minasacreate(const real_1d_array &x, const real_1d_array &bndl, const real_1d_array &bndu, minasastate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetcond(minasastate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetxrep(minasastate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetalgorithm(minasastate &state, const ae_int_t algotype, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetstpmax(minasastate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minasaiteration(minasastate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to start iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey


*************************************************************************/
void minasaoptimize(minasastate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void minasaresults(const minasastate &state, real_1d_array &x, minasareport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void minasaresultsbuf(const minasastate &state, real_1d_array &x, minasareport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minasarestartfrom(minasastate &state, const real_1d_array &x, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);
#endif
}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (FUNCTIONS)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_OPTGUARDAPI) || !defined(AE_PARTIAL_BUILD)
void optguardinitinternal(optguardreport* rep,
     ae_int_t n,
     ae_int_t k,
     ae_state *_state);
void optguardexportreport(const optguardreport* srcrep,
     ae_int_t n,
     ae_int_t k,
     ae_bool badgradhasxj,
     optguardreport* dstrep,
     ae_state *_state);
void smoothnessmonitorexportc1test0report(const optguardnonc1test0report* srcrep,
     /* Real    */ const ae_vector* s,
     optguardnonc1test0report* dstrep,
     ae_state *_state);
void smoothnessmonitorexportc1test1report(const optguardnonc1test1report* srcrep,
     /* Real    */ const ae_vector* s,
     optguardnonc1test1report* dstrep,
     ae_state *_state);
ae_bool optguardallclear(const optguardreport* rep, ae_state *_state);
void _optguardreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardreport_clear(void* _p);
void _optguardreport_destroy(void* _p);
void _optguardnonc0report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardnonc0report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardnonc0report_clear(void* _p);
void _optguardnonc0report_destroy(void* _p);
void _optguardnonc1test0report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test0report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test0report_clear(void* _p);
void _optguardnonc1test0report_destroy(void* _p);
void _optguardnonc1test1report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test1report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test1report_clear(void* _p);
void _optguardnonc1test1report_destroy(void* _p);
#endif
#if defined(AE_COMPILE_OPTS) || !defined(AE_PARTIAL_BUILD)
void lptestproblemcreate(ae_int_t n,
     ae_bool hasknowntarget,
     double targetf,
     lptestproblem* p,
     ae_state *_state);
ae_bool lptestproblemhasknowntarget(lptestproblem* p, ae_state *_state);
double lptestproblemgettargetf(lptestproblem* p, ae_state *_state);
ae_int_t lptestproblemgetn(lptestproblem* p, ae_state *_state);
ae_int_t lptestproblemgetm(lptestproblem* p, ae_state *_state);
void lptestproblemsetscale(lptestproblem* p,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void lptestproblemsetcost(lptestproblem* p,
     /* Real    */ const ae_vector* c,
     ae_state *_state);
void lptestproblemsetbc(lptestproblem* p,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void lptestproblemsetlc2(lptestproblem* p,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t m,
     ae_state *_state);
void lptestproblemalloc(ae_serializer* s,
     const lptestproblem* p,
     ae_state *_state);
void lptestproblemserialize(ae_serializer* s,
     const lptestproblem* p,
     ae_state *_state);
void lptestproblemunserialize(ae_serializer* s,
     lptestproblem* p,
     ae_state *_state);
ae_bool qpxproblemisquadraticobjective(qpxproblem* p, ae_state *_state);
ae_int_t qpxproblemgetn(qpxproblem* p, ae_state *_state);
ae_int_t qpxproblemgetmlc(qpxproblem* p, ae_state *_state);
ae_int_t qpxproblemgetmqc(qpxproblem* p, ae_state *_state);
ae_int_t qpxproblemgetmcc(qpxproblem* p, ae_state *_state);
ae_int_t qpxproblemgettotalconstraints(qpxproblem* p, ae_state *_state);
void qpxproblemgetinitialpoint(qpxproblem* p,
     /* Real    */ ae_vector* x0,
     ae_state *_state);
ae_bool qpxproblemhasinitialpoint(qpxproblem* p, ae_state *_state);
void qpxproblemgetscale(qpxproblem* p,
     /* Real    */ ae_vector* s,
     ae_state *_state);
ae_bool qpxproblemhasscale(qpxproblem* p, ae_state *_state);
void qpxproblemgetorigin(qpxproblem* p,
     /* Real    */ ae_vector* xorigin,
     ae_state *_state);
ae_bool qpxproblemhasorigin(qpxproblem* p, ae_state *_state);
void qpxproblemgetlinearterm(qpxproblem* p,
     /* Real    */ ae_vector* c,
     ae_state *_state);
void qpxproblemgetquadraticterm(qpxproblem* p,
     sparsematrix* q,
     ae_bool* isupper,
     ae_state *_state);
ae_bool qpxproblemhasquadraticterm(qpxproblem* p, ae_state *_state);
void qpxproblemgetbc(qpxproblem* p,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     ae_state *_state);
void qpxproblemgetlc2(qpxproblem* p,
     sparsematrix* a,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t* m,
     ae_state *_state);
void qpxproblemsetxqc(qpxproblem* p,
     xquadraticconstraints* src,
     ae_state *_state);
void qpxproblemgetqc2i(qpxproblem* p,
     ae_int_t idx,
     sparsematrix* q,
     ae_bool* isupper,
     /* Real    */ ae_vector* b,
     double* cl,
     double* cu,
     ae_bool* applyorigin,
     ae_state *_state);
void xqccopy(xquadraticconstraints* src,
     xquadraticconstraints* dst,
     ae_state *_state);
ae_int_t xqcgetcount(const xquadraticconstraints* xqc, ae_state *_state);
void _xlinearconstraints_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _xlinearconstraints_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _xlinearconstraints_clear(void* _p);
void _xlinearconstraints_destroy(void* _p);
void _lptestproblem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _lptestproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _lptestproblem_clear(void* _p);
void _lptestproblem_destroy(void* _p);
#endif
#if defined(AE_COMPILE_OPTSERV) || !defined(AE_PARTIAL_BUILD)
void checkbcviolation(/* Boolean */ const ae_vector* hasbndl,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* hasbndu,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* s,
     ae_bool nonunits,
     double* bcerr,
     ae_int_t* bcidx,
     ae_state *_state);
void checklcviolation(/* Real    */ const ae_matrix* cleic,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t nec,
     ae_int_t nic,
     /* Real    */ const ae_vector* x,
     ae_int_t n,
     double* lcerr,
     ae_int_t* lcidx,
     ae_state *_state);
void checklc2violation(const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t cntlc,
     /* Real    */ const ae_vector* x,
     double* lcerr,
     ae_int_t* lcidx,
     ae_state *_state);
void unscaleandchecklc2violation(/* Real    */ const ae_vector* s,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t cntlc,
     /* Real    */ const ae_vector* x,
     double* sumerr,
     double* maxerr,
     ae_int_t* maxidx,
     ae_state *_state);
void checknlcviolation(/* Real    */ const ae_vector* fi,
     ae_int_t ng,
     ae_int_t nh,
     double* nlcerr,
     ae_int_t* nlcidx,
     ae_state *_state);
void unscaleandchecknlcviolation(/* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* fscales,
     ae_int_t ng,
     ae_int_t nh,
     double* nlcerr,
     ae_int_t* nlcidx,
     ae_state *_state);
void unscaleandchecknlc2violation(/* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* fscales,
     /* Real    */ const ae_vector* rawnl,
     /* Real    */ const ae_vector* rawnu,
     ae_int_t cntnlc,
     double* nlcerr,
     ae_int_t* nlcidx,
     ae_state *_state);
void checknlc2violation(/* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* rawnl,
     /* Real    */ const ae_vector* rawnu,
     ae_int_t cntnlc,
     double* nlcerr,
     ae_int_t* nlcidx,
     ae_state *_state);
void trimprepare(double f, double* threshold, ae_state *_state);
void trimfunction(double* f,
     /* Real    */ ae_vector* g,
     ae_int_t n,
     double threshold,
     ae_state *_state);
ae_bool enforceboundaryconstraints(/* Real    */ ae_vector* x,
     /* Real    */ const ae_vector* bl,
     /* Boolean */ const ae_vector* havebl,
     /* Real    */ const ae_vector* bu,
     /* Boolean */ const ae_vector* havebu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state);
void projectgradientintobc(/* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* g,
     /* Real    */ const ae_vector* bl,
     /* Boolean */ const ae_vector* havebl,
     /* Real    */ const ae_vector* bu,
     /* Boolean */ const ae_vector* havebu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state);
void calculatestepbound(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* d,
     double alpha,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t* variabletofreeze,
     double* valuetofreeze,
     double* maxsteplen,
     ae_state *_state);
ae_int_t postprocessboundedstep(/* Real    */ ae_vector* x,
     /* Real    */ const ae_vector* xprev,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t variabletofreeze,
     double valuetofreeze,
     double steptaken,
     double maxsteplen,
     ae_state *_state);
void filterdirection(/* Real    */ ae_vector* d,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     /* Real    */ const ae_vector* s,
     ae_int_t nmain,
     ae_int_t nslack,
     double droptol,
     ae_state *_state);
ae_int_t numberofchangedconstraints(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* xprev,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state);
ae_bool findfeasiblepoint(/* Real    */ ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     /* Real    */ const ae_matrix* _ce,
     ae_int_t k,
     double epsi,
     ae_int_t* qpits,
     ae_int_t* gpaits,
     ae_state *_state);
ae_bool derivativecheck(double f0,
     double df0,
     double f1,
     double df1,
     double f,
     double df,
     double width,
     ae_state *_state);
void estimateparabolicmodel(double absasum,
     double absasum2,
     double mx,
     double mb,
     double md,
     double d1,
     double d2,
     ae_int_t* d1est,
     ae_int_t* d2est,
     ae_state *_state);
void inexactlbfgspreconditioner(/* Real    */ ae_vector* s,
     ae_int_t n,
     /* Real    */ const ae_vector* d,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* w,
     ae_int_t k,
     precbuflbfgs* buf,
     ae_state *_state);
void preparelowrankpreconditioner(/* Real    */ const ae_vector* d,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* w,
     ae_int_t n,
     ae_int_t k,
     precbuflowrank* buf,
     ae_state *_state);
void applylowrankpreconditioner(/* Real    */ ae_vector* s,
     precbuflowrank* buf,
     ae_state *_state);
void smoothnessmonitorinit(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     ae_int_t k,
     ae_bool checksmoothness,
     ae_state *_state);
void smoothnessmonitorstartlinesearch(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     ae_int_t inneriter,
     ae_int_t outeriter,
     ae_state *_state);
void smoothnessmonitorstartlinesearch1u(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* invs,
     /* Real    */ const ae_vector* x,
     double f0,
     /* Real    */ const ae_vector* j0,
     ae_int_t inneriter,
     ae_int_t outeriter,
     ae_state *_state);
void smoothnessmonitorenqueuepoint(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* d,
     double stp,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     ae_state *_state);
void smoothnessmonitorenqueuepoint1u(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* invs,
     /* Real    */ const ae_vector* d,
     double stp,
     /* Real    */ const ae_vector* x,
     double f0,
     /* Real    */ const ae_vector* j0,
     ae_state *_state);
void smoothnessmonitorfinalizelinesearch(smoothnessmonitor* monitor,
     ae_state *_state);
void smoothnessmonitorstartlagrangianprobing(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* d,
     double stpmax,
     ae_int_t inneriter,
     ae_int_t outeriter,
     ae_state *_state);
ae_bool smoothnessmonitorprobelagrangian(smoothnessmonitor* monitor,
     ae_state *_state);
void smoothnessmonitortracelagrangianprobingresults(smoothnessmonitor* monitor,
     ae_state *_state);
void smoothnessmonitortracelagrangianprobingresultsx(smoothnessmonitor* monitor,
     ae_bool needcv,
     ae_state *_state);
void smoothnessmonitortracestatus(const smoothnessmonitor* monitor,
     ae_bool callersuggeststrace,
     ae_state *_state);
void smoothnessmonitorexportreport(smoothnessmonitor* monitor,
     optguardreport* rep,
     ae_state *_state);
ae_bool smoothnessmonitorcheckgradientatx0(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* unscaledx0,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_bool hasboxconstraints,
     double teststep,
     ae_state *_state);
void hessianinitbfgs(xbfgshessian* hess,
     ae_int_t n,
     ae_int_t resetfreq,
     double stpshort,
     ae_state *_state);
void hessianinitlowrank(xbfgshessian* hess,
     ae_int_t n,
     ae_int_t m,
     double stpshort,
     double maxhess,
     ae_state *_state);
void hessianinitlowranksr1(xbfgshessian* hess,
     ae_int_t n,
     ae_int_t m,
     double stpshort,
     double maxhess,
     ae_state *_state);
void hessianinitlowranksr1v2(xbfgshessian* hess,
     ae_int_t n,
     ae_int_t m,
     double stpshort,
     double maxhess,
     ae_state *_state);
void hessiansetmaxhess(xbfgshessian* hess,
     double maxhess,
     ae_state *_state);
void hessiansetscales(xbfgshessian* hess,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void hessiansetscalesinertial(xbfgshessian* hess,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void hessianupdate(xbfgshessian* hess,
     /* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* g0,
     /* Real    */ const ae_vector* x1,
     /* Real    */ const ae_vector* g1,
     ae_bool dotrace,
     ae_state *_state);
void hessianupdatev2(xbfgshessian* hess,
     /* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* g0,
     /* Real    */ const ae_vector* x1,
     /* Real    */ const ae_vector* g1,
     ae_int_t strategy,
     ae_bool tryreplacelast,
     ae_bool dotrace,
     ae_int_t tracelevel,
     ae_state *_state);
void hessianupdatesr1(xbfgshessian* hess,
     /* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* g0,
     /* Real    */ const ae_vector* x1,
     /* Real    */ const ae_vector* g1,
     ae_bool dotrace,
     ae_int_t tracelevel,
     ae_state *_state);
void hessianpoplatestifpossible(xbfgshessian* hess, ae_state *_state);
void hessianmultiplyby(xbfgshessian* hess, double s, ae_state *_state);
void hessiangetdiagonal(xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     ae_state *_state);
void hessiangetmatrix(xbfgshessian* hess,
     ae_bool isupper,
     /* Real    */ ae_matrix* h,
     ae_state *_state);
double hessiangetnrm2(xbfgshessian* hess, ae_state *_state);
double hessiangetnrm2stab(xbfgshessian* hess, ae_state *_state);
void hessiangetlowrank(xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_matrix* corrc,
     /* Real    */ ae_vector* corrs,
     ae_int_t* corrk,
     ae_state *_state);
void hessiangetlowrankmemory(xbfgshessian* hess,
     double* sigma,
     /* Real    */ ae_matrix* s,
     /* Real    */ ae_matrix* y,
     ae_int_t* updcnt,
     ae_state *_state);
void hessiangetlowrankstabilized(xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_matrix* corrc,
     /* Real    */ ae_vector* corrs,
     ae_int_t* corrk,
     ae_state *_state);
void hessiangetlowrankstabilizedlbfgs(xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_matrix* corrc,
     /* Real    */ ae_vector* corrs,
     ae_int_t* corrk,
     ae_state *_state);
void hessiangetlowrankstabilizedsr1(xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_matrix* corrc,
     /* Real    */ ae_vector* corrs,
     ae_int_t* corrk,
     ae_state *_state);
ae_int_t hessiangetmaxrank(const xbfgshessian* hess, ae_state *_state);
void hessianmv(xbfgshessian* hess,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* hx,
     ae_state *_state);
double hessianvmv(xbfgshessian* hess,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void hessianxmv(xbfgshessian* hess,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* hx,
     double* xhx,
     ae_state *_state);
void motfcreaterandomunknown(ae_int_t n,
     ae_int_t m,
     ae_int_t nequality,
     ae_int_t ninequality,
     ae_int_t taskkind,
     double nlquadratic,
     double nlquartic,
     hqrndstate* rs,
     multiobjectivetestfunction* problem,
     ae_state *_state);
void motfcreate1knownanswer(ae_int_t n,
     double nlquadratic,
     double nlquartic,
     hqrndstate* rs,
     multiobjectivetestfunction* problem,
     ae_state *_state);
ae_int_t motfgetmetaheuristicu1size(ae_state *_state);
ae_int_t motfgetmetaheuristicu2size(ae_state *_state);
ae_int_t motfgetmetaheuristicu3size(ae_state *_state);
ae_int_t motfgetnls1size(ae_state *_state);
ae_int_t motfgetnls2size(ae_state *_state);
void motfcreatemetaheuristicu1(ae_int_t problemidx,
     ae_bool isrotated,
     hqrndstate* rs,
     multiobjectivetestfunction* problem,
     ae_state *_state);
void motfcreatemetaheuristicu2(ae_int_t problemidx,
     ae_bool isrotated,
     hqrndstate* rs,
     multiobjectivetestfunction* problem,
     ae_state *_state);
void motfcreatemetaheuristicu3(ae_int_t problemidx,
     hqrndstate* rs,
     multiobjectivetestfunction* problem,
     ae_state *_state);
void motfcreatenls1(ae_int_t problemidx,
     hqrndstate* rs,
     multiobjectivetestfunction* problem,
     ae_state *_state);
void motfcreatenls2(ae_int_t problemidx,
     hqrndstate* rs,
     multiobjectivetestfunction* problem,
     ae_state *_state);
void motfeval(const multiobjectivetestfunction* problem,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* fi,
     ae_bool needfi,
     /* Real    */ ae_matrix* jac,
     ae_bool needjac,
     ae_state *_state);
void converttwosidedlctoonesidedold(const sparsematrix* sparsec,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densec,
     ae_int_t kdense,
     ae_int_t n,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     /* Real    */ ae_matrix* olddensec,
     /* Integer */ ae_vector* olddensect,
     ae_int_t* olddensek,
     ae_state *_state);
void converttwosidednlctoonesidedold(/* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     /* Integer */ ae_vector* nlcidx,
     /* Real    */ ae_vector* nlcmul,
     /* Real    */ ae_vector* nlcadd,
     ae_int_t* cntnlec,
     ae_int_t* cntnlic,
     ae_state *_state);
void trustradincreasemomentum(double* growthfactor,
     double growthincrease,
     double maxgrowthfactor,
     ae_state *_state);
void trustradresetmomentum(double* growthfactor,
     double mingrowthfactor,
     ae_state *_state);
void vfjallocdense(ae_int_t n,
     ae_int_t m,
     varsfuncjac* s,
     ae_state *_state);
void vfjallocsparse(ae_int_t n,
     ae_int_t m,
     varsfuncjac* s,
     ae_state *_state);
void vfjinitfromdense(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* fi,
     ae_int_t m,
     /* Real    */ const ae_matrix* jac,
     varsfuncjac* s,
     ae_state *_state);
void vfjinitfromsparse(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* fi,
     ae_int_t m,
     const sparsematrix* jac,
     varsfuncjac* s,
     ae_state *_state);
void vfjcopy(const varsfuncjac* src, varsfuncjac* dst, ae_state *_state);
void critinitdefault(nlpstoppingcriteria* crit, ae_state *_state);
void critcopy(const nlpstoppingcriteria* src,
     nlpstoppingcriteria* dst,
     ae_state *_state);
void critsetcondv1(nlpstoppingcriteria* crit,
     double epsf,
     double eps,
     ae_int_t maxits,
     ae_state *_state);
double critgetepsf(const nlpstoppingcriteria* crit, ae_state *_state);
double critgeteps(const nlpstoppingcriteria* crit, ae_state *_state);
double critgetepswithdefault(const nlpstoppingcriteria* crit,
     double defval,
     ae_state *_state);
ae_int_t critgetmaxits(const nlpstoppingcriteria* crit, ae_state *_state);
void linesearchinitbisect(double f0,
     double g0,
     double alpha1,
     double alphamax,
     double c1,
     double c2,
     ae_bool strongwolfecond,
     ae_int_t maxits,
     ae_bool dotrace,
     ae_int_t tracelevel,
     linesearchstate* state,
     ae_state *_state);
ae_bool linesearchiteration(linesearchstate* state, ae_state *_state);
void nlpfinit(double maxh, nlpfilter* s, ae_state *_state);
void nlpfinitfrom(const nlpfilter* src, nlpfilter* dst, ae_state *_state);
void nlpfinitx(double maxh, ae_int_t md, nlpfilter* s, ae_state *_state);
ae_bool nlpfisacceptable(nlpfilter* s,
     double f0,
     double h0,
     double f1,
     double h1,
     ae_state *_state);
ae_bool nlpfisacceptable1(nlpfilter* s,
     double f,
     double h,
     ae_state *_state);
void nlpfappend(nlpfilter* s, double f, double h, ae_state *_state);
void nlpfclear(nlpfilter* s, ae_state *_state);
void nlpfsetmaxh(nlpfilter* s, double maxh, ae_state *_state);
void xlcinit(ae_int_t n, xlinearconstraints* state, ae_state *_state);
void xlcsetlcmixed(xlinearconstraints* state,
     const sparsematrix* sparsec,
     /* Integer */ const ae_vector* sparsect,
     ae_int_t sparsek,
     /* Real    */ const ae_matrix* densec,
     /* Integer */ const ae_vector* densect,
     ae_int_t densek,
     ae_state *_state);
void xlcsetlc2mixed(xlinearconstraints* state,
     const sparsematrix* sparsea,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t kdense,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_state *_state);
void xlcaddlc2dense(xlinearconstraints* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state);
void xlcaddlc2(xlinearconstraints* state,
     /* Integer */ const ae_vector* idxa,
     /* Real    */ const ae_vector* vala,
     ae_int_t nnz,
     double al,
     double au,
     ae_state *_state);
void xlcaddlc2sparsefromdense(xlinearconstraints* state,
     /* Real    */ const ae_vector* da,
     double al,
     double au,
     ae_state *_state);
void xlcconverttoold(xlinearconstraints* state, ae_state *_state);
void xlcconverttosparse(xlinearconstraints* state, ae_state *_state);
void xqcaddqc2list(xquadraticconstraints* xqc,
     /* Integer */ const ae_vector* qridx,
     /* Integer */ const ae_vector* qcidx,
     /* Real    */ const ae_vector* qvals,
     ae_int_t qnnz,
     ae_bool isupper,
     /* Integer */ const ae_vector* bidx,
     /* Real    */ const ae_vector* bvals,
     ae_int_t bnnz,
     double cl,
     double cu,
     ae_bool applyorigin,
     ae_state *_state);
void xqcaddqc2dense(xquadraticconstraints* xqc,
     /* Real    */ const ae_matrix* q,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     double cl,
     double cu,
     ae_bool applyorigin,
     ae_state *_state);
ae_int_t xccdenormalizedprimitivepowerconetype(ae_state *_state);
ae_int_t xccgenericorthogonalpowerconetype(ae_state *_state);
ae_int_t xccdenormalizedprimitiveconetype(ae_state *_state);
ae_int_t xccgenericorthogonalconetype(ae_state *_state);
ae_int_t xccprimitiveconetype(ae_state *_state);
ae_int_t xccprimitivepowerconetype(ae_state *_state);
void xccclear(xconicconstraints* state, ae_state *_state);
void xcccopy(xconicconstraints* src,
     xconicconstraints* dst,
     ae_state *_state);
void xcccopywithskipandpack(xconicconstraints* src,
     /* Boolean */ const ae_vector* skipflags,
     /* Integer */ const ae_vector* packxperm,
     xconicconstraints* dst,
     ae_state *_state);
ae_int_t xccgetcount(const xconicconstraints* xcc, ae_state *_state);
void xccaddsoccorthogonalnoncanonic(xconicconstraints* xcc,
     /* Integer */ const ae_vector* varidx,
     /* Real    */ const ae_vector* diag,
     /* Real    */ const ae_vector* shft,
     ae_int_t nvars,
     double theta,
     ae_bool applyorigin,
     ae_state *_state);
void xccaddsoccprimitivecanonic(xconicconstraints* xcc,
     /* Integer */ const ae_vector* varidx,
     /* Real    */ const ae_vector* diag,
     /* Real    */ const ae_vector* shft,
     ae_int_t nvars,
     ae_bool applyorigin,
     ae_state *_state);
void xccaddpowccprimitivecanonic(xconicconstraints* xcc,
     /* Integer */ const ae_vector* varidx,
     /* Real    */ const ae_vector* diag,
     /* Real    */ const ae_vector* shft,
     ae_int_t nvars,
     /* Real    */ const ae_vector* alphapow,
     ae_int_t kpow,
     ae_bool applyorigin,
     ae_state *_state);
void xccaddpowccorthogonalnoncanonic(xconicconstraints* xcc,
     /* Integer */ const ae_vector* varidx,
     /* Real    */ const ae_vector* diag,
     /* Real    */ const ae_vector* shft,
     ae_int_t nvars,
     double theta,
     /* Real    */ const ae_vector* alphapow,
     ae_int_t kpow,
     ae_bool applyorigin,
     ae_state *_state);
void xccfactoroutnonaxial(xconicconstraint* c,
     ae_int_t k0,
     double alpha0,
     double alpha1,
     ae_int_t k1,
     ae_bool* hadk0,
     ae_bool* hadk1,
     ae_state *_state);
void unscalexbatchfinitebnd(/* Real    */ const ae_vector* xs,
     ae_int_t batchsize,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     /* Real    */ const ae_vector* sclfinitebndl,
     /* Real    */ const ae_vector* sclfinitebndu,
     /* Real    */ const ae_vector* rawfinitebndl,
     /* Real    */ const ae_vector* rawfinitebndu,
     /* Real    */ ae_vector* xu,
     ae_state *_state);
void _precbuflbfgs_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _precbuflbfgs_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _precbuflbfgs_clear(void* _p);
void _precbuflbfgs_destroy(void* _p);
void _precbuflowrank_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _precbuflowrank_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _precbuflowrank_clear(void* _p);
void _precbuflowrank_destroy(void* _p);
void _xbfgshessian_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _xbfgshessian_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _xbfgshessian_clear(void* _p);
void _xbfgshessian_destroy(void* _p);
void _varsfuncjac_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _varsfuncjac_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _varsfuncjac_clear(void* _p);
void _varsfuncjac_destroy(void* _p);
void _nlpstoppingcriteria_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nlpstoppingcriteria_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nlpstoppingcriteria_clear(void* _p);
void _nlpstoppingcriteria_destroy(void* _p);
void _smoothnessmonitor_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _smoothnessmonitor_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _smoothnessmonitor_clear(void* _p);
void _smoothnessmonitor_destroy(void* _p);
void _multiobjectivetestfunction_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _multiobjectivetestfunction_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _multiobjectivetestfunction_clear(void* _p);
void _multiobjectivetestfunction_destroy(void* _p);
void _linesearchstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _linesearchstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _linesearchstate_clear(void* _p);
void _linesearchstate_destroy(void* _p);
void _nlpfilter_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nlpfilter_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nlpfilter_clear(void* _p);
void _nlpfilter_destroy(void* _p);
#endif
#if defined(AE_COMPILE_CQMODELS) || !defined(AE_PARTIAL_BUILD)
void cqminit(ae_int_t n, convexquadraticmodel* s, ae_state *_state);
void cqmseta(convexquadraticmodel* s,
     /* Real    */ const ae_matrix* a,
     ae_bool isupper,
     double alpha,
     ae_state *_state);
void cqmgeta(const convexquadraticmodel* s,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
void cqmrewritedensediagonal(convexquadraticmodel* s,
     /* Real    */ const ae_vector* z,
     ae_state *_state);
void cqmsetd(convexquadraticmodel* s,
     /* Real    */ const ae_vector* d,
     double tau,
     ae_state *_state);
void cqmdropa(convexquadraticmodel* s, ae_state *_state);
void cqmsetb(convexquadraticmodel* s,
     /* Real    */ const ae_vector* b,
     ae_state *_state);
void cqmsetq(convexquadraticmodel* s,
     /* Real    */ const ae_matrix* q,
     /* Real    */ const ae_vector* r,
     ae_int_t k,
     double theta,
     ae_state *_state);
void cqmsetactiveset(convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     /* Boolean */ const ae_vector* activeset,
     ae_state *_state);
double cqmeval(const convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void cqmevalx(const convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     double* r,
     double* noise,
     ae_state *_state);
void cqmgradunconstrained(const convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* g,
     ae_state *_state);
double cqmxtadx2(const convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);
void cqmadx(const convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
ae_bool cqmconstrainedoptimum(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void cqmscalevector(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void cqmgetdiaga(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state);
double cqmdebugconstrainedevalt(convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
double cqmdebugconstrainedevale(convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void _convexquadraticmodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _convexquadraticmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _convexquadraticmodel_clear(void* _p);
void _convexquadraticmodel_destroy(void* _p);
#endif
#if defined(AE_COMPILE_SNNLS) || !defined(AE_PARTIAL_BUILD)
void snnlsinit(ae_int_t nsmax,
     ae_int_t ndmax,
     ae_int_t nrmax,
     snnlssolver* s,
     ae_state *_state);
void snnlssetproblem(snnlssolver* s,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* b,
     ae_int_t ns,
     ae_int_t nd,
     ae_int_t nr,
     ae_state *_state);
void snnlsdropnnc(snnlssolver* s, ae_int_t idx, ae_state *_state);
void snnlssolve(snnlssolver* s,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void _snnlssolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _snnlssolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _snnlssolver_clear(void* _p);
void _snnlssolver_destroy(void* _p);
#endif
#if defined(AE_COMPILE_SACTIVESETS) || !defined(AE_PARTIAL_BUILD)
void sasinit(ae_int_t n, sactiveset* s, ae_state *_state);
void sassetscale(sactiveset* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void sassetprecdiag(sactiveset* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void sassetbc(sactiveset* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void sassetlc(sactiveset* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void sassetlcx(sactiveset* state,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t nec,
     ae_int_t nic,
     ae_state *_state);
ae_bool sasstartoptimization(sactiveset* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void sasexploredirection(const sactiveset* state,
     /* Real    */ const ae_vector* d,
     double* stpmax,
     ae_int_t* cidx,
     double* vval,
     ae_state *_state);
ae_int_t sasmoveto(sactiveset* state,
     /* Real    */ const ae_vector* xn,
     ae_bool needact,
     ae_int_t cidx,
     double cval,
     ae_state *_state);
void sasimmediateactivation(sactiveset* state,
     ae_int_t cidx,
     double cval,
     ae_state *_state);
void sasconstraineddescent(sactiveset* state,
     /* Real    */ const ae_vector* g,
     /* Real    */ ae_vector* d,
     ae_state *_state);
void sasconstraineddescentprec(sactiveset* state,
     /* Real    */ const ae_vector* g,
     /* Real    */ ae_vector* d,
     ae_state *_state);
void sasconstraineddirection(sactiveset* state,
     /* Real    */ ae_vector* d,
     ae_state *_state);
void sasconstraineddirectionprec(sactiveset* state,
     /* Real    */ ae_vector* d,
     ae_state *_state);
void sascorrection(sactiveset* state,
     /* Real    */ ae_vector* x,
     double* penalty,
     ae_state *_state);
double sasactivelcpenalty1(sactiveset* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
double sasscaledconstrainednorm(sactiveset* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void sasstopoptimization(sactiveset* state, ae_state *_state);
void sasreactivateconstraints(sactiveset* state,
     /* Real    */ const ae_vector* gc,
     ae_state *_state);
void sasreactivateconstraintsprec(sactiveset* state,
     /* Real    */ const ae_vector* gc,
     ae_state *_state);
void sasrebuildbasis(sactiveset* state, ae_state *_state);
void sasappendtobasis(sactiveset* state,
     /* Boolean */ const ae_vector* newentries,
     ae_state *_state);
void _sactiveset_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sactiveset_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sactiveset_clear(void* _p);
void _sactiveset_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINBLEIC) || !defined(AE_PARTIAL_BUILD)
void minbleiccreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minbleicstate* state,
     ae_state *_state);
void minbleiccreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minbleicstate* state,
     ae_state *_state);
void minbleicsetbc(minbleicstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minbleicsetlc(minbleicstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minbleicsetcond(minbleicstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minbleicsetscale(minbleicstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minbleicsetprecdefault(minbleicstate* state, ae_state *_state);
void minbleicsetprecdiag(minbleicstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void minbleicsetprecscale(minbleicstate* state, ae_state *_state);
void minbleicsetxrep(minbleicstate* state,
     ae_bool needxrep,
     ae_state *_state);
void minbleicsetdrep(minbleicstate* state,
     ae_bool needdrep,
     ae_state *_state);
void minbleicsetstpmax(minbleicstate* state,
     double stpmax,
     ae_state *_state);
ae_bool minbleiciteration(minbleicstate* state, ae_state *_state);
void minbleicoptguardgradient(minbleicstate* state,
     double teststep,
     ae_state *_state);
void minbleicoptguardsmoothness(minbleicstate* state,
     ae_int_t level,
     ae_state *_state);
void minbleicoptguardresults(minbleicstate* state,
     optguardreport* rep,
     ae_state *_state);
void minbleicoptguardnonc1test0results(const minbleicstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);
void minbleicoptguardnonc1test1results(minbleicstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);
void minbleicresults(const minbleicstate* state,
     /* Real    */ ae_vector* x,
     minbleicreport* rep,
     ae_state *_state);
void minbleicresultsbuf(const minbleicstate* state,
     /* Real    */ ae_vector* x,
     minbleicreport* rep,
     ae_state *_state);
void minbleicrestartfrom(minbleicstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minbleicrequesttermination(minbleicstate* state, ae_state *_state);
void minbleicemergencytermination(minbleicstate* state, ae_state *_state);
void minbleicsetprotocolv1(minbleicstate* state, ae_state *_state);
void _minbleicstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minbleicstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minbleicstate_clear(void* _p);
void _minbleicstate_destroy(void* _p);
void _minbleicreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minbleicreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minbleicreport_clear(void* _p);
void _minbleicreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_LPQPSERV) || !defined(AE_PARTIAL_BUILD)
void scaleshiftmixedlcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_state *_state);
void scaleshiftbcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     ae_int_t n,
     ae_state *_state);
void scaleshiftdensebrlcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     /* Real    */ ae_matrix* densea,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_int_t m,
     ae_state *_state);
void scaleshiftmixedbrlcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_state *_state);
void scaleshiftsparselcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     sparsematrix* sparsea,
     ae_int_t m,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_state *_state);
void scaledenseqpinplace(/* Real    */ ae_matrix* densea,
     ae_bool isupper,
     ae_int_t nmain,
     /* Real    */ ae_vector* denseb,
     ae_int_t ntotal,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void scalesparseqpinplace(/* Real    */ const ae_vector* s,
     ae_int_t n,
     sparsematrix* sparsea,
     /* Real    */ ae_matrix* densecorrc,
     /* Real    */ ae_vector* densecorrd,
     ae_int_t corrrank,
     /* Real    */ ae_vector* denseb,
     ae_state *_state);
void normalizedensebrlcinplace(/* Real    */ ae_matrix* densea,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* rownorms,
     ae_bool neednorms,
     ae_state *_state);
void normalizemixedbrlcinplace(sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_int_t n,
     ae_bool limitedamplification,
     /* Real    */ ae_vector* rownorms,
     ae_bool neednorms,
     ae_state *_state);
void normalizedenselcinplace(/* Real    */ ae_matrix* densea,
     ae_int_t m,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t n,
     ae_bool limitedamplification,
     /* Real    */ ae_vector* rownorms,
     ae_bool neednorms,
     ae_state *_state);
void normalizesparselcinplace(sparsematrix* sparsea,
     ae_int_t m,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t n,
     ae_bool limitedamplification,
     /* Real    */ ae_vector* rownorms,
     ae_bool neednorms,
     ae_state *_state);
void normalizesparselcinplaceuniform(sparsematrix* sparsea,
     ae_int_t m,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t n,
     /* Real    */ ae_vector* ascales,
     ae_bool needscales,
     ae_state *_state);
double normalizedenseqpinplace(/* Real    */ ae_matrix* densea,
     ae_bool isupper,
     ae_int_t nmain,
     /* Real    */ ae_vector* denseb,
     ae_int_t ntotal,
     ae_state *_state);
double normalizesparseqpinplace(sparsematrix* sparsea,
     ae_bool isupper,
     /* Real    */ ae_matrix* densecorrc,
     /* Real    */ ae_vector* densecorrd,
     ae_int_t corrrank,
     /* Real    */ ae_vector* denseb,
     ae_int_t n,
     ae_state *_state);
void unscaleunshiftpointbc(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     /* Real    */ const ae_vector* rawbndl,
     /* Real    */ const ae_vector* rawbndu,
     /* Real    */ const ae_vector* sclsftbndl,
     /* Real    */ const ae_vector* sclsftbndu,
     /* Boolean */ const ae_vector* hasbndl,
     /* Boolean */ const ae_vector* hasbndu,
     /* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void quadraticlinearconverttodenseltr(/* Real    */ const ae_vector* rawc,
     ae_int_t n,
     /* Real    */ const ae_matrix* denseh,
     const sparsematrix* sparseh,
     ae_int_t hkind,
     ae_bool isupper,
     /* Real    */ ae_vector* c,
     /* Real    */ ae_matrix* h,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_GIPM) || !defined(AE_PARTIAL_BUILD)
double gipmgetinitprimdual(ae_state *_state);
void gipminitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* x0,
     ae_int_t nraw,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t mflex,
     ae_int_t mhard,
     /* Real    */ const ae_vector* fscales,
     /* Boolean */ const ae_vector* doqnreport,
     double eps,
     ae_int_t maxits,
     ae_bool isfirstorder,
     gipmstate* state,
     ae_state *_state);
ae_bool gipmiteration(gipmstate* state,
     ae_bool userterminationneeded,
     ae_state *_state);
void gipmresults(gipmstate* state,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* lagxc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _gipmvars_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gipmvars_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gipmvars_clear(void* _p);
void _gipmvars_destroy(void* _p);
void _gipmrhs_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gipmrhs_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gipmrhs_clear(void* _p);
void _gipmrhs_destroy(void* _p);
void _gipmcondensedsystem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gipmcondensedsystem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gipmcondensedsystem_clear(void* _p);
void _gipmcondensedsystem_destroy(void* _p);
void _gipmstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gipmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gipmstate_clear(void* _p);
void _gipmstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_GQPIPM) || !defined(AE_PARTIAL_BUILD)
void gqpipmcanonicalizeconicconstraints(/* Real    */ ae_vector* s,
     /* Real    */ ae_vector* xorigin,
     /* Real    */ ae_vector* b,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     /* Real    */ ae_vector* xs,
     sparsematrix* sparseh,
     ae_bool hupper,
     ae_bool hash,
     ae_int_t* n,
     sparsematrix* a,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t* m,
     xquadraticconstraints* xqc,
     xconicconstraints* xcc,
     /* Real    */ ae_vector* lagbcmin,
     /* Real    */ ae_vector* lagbcmax,
     ae_state *_state);
void gqpipminitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     ae_bool isdense,
     ae_bool isfirstorder,
     double eps,
     ae_int_t maxits,
     gqpipmstate* state,
     ae_state *_state);
void gqpipmsetquadraticlinear(gqpipmstate* state,
     /* Real    */ const ae_matrix* denseh,
     const sparsematrix* sparseh,
     ae_int_t hkind,
     /* Real    */ const ae_vector* c,
     ae_state *_state);
void gqpipmsetconstraints(gqpipmstate* state,
     const sparsematrix* sparsea,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     ae_int_t m,
     xquadraticconstraints* xqc,
     xconicconstraints* xcc,
     ae_state *_state);
void gqpipmsetquasinewtonparams(gqpipmstate* state,
     ae_int_t memlen,
     ae_int_t maxoffdiag,
     ae_state *_state);
void gqpipmoptimize(gqpipmstate* state,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     /* Real    */ ae_vector* lagqc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _gqpsparseconichessian_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gqpsparseconichessian_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gqpsparseconichessian_clear(void* _p);
void _gqpsparseconichessian_destroy(void* _p);
void _gqpoptionaldense_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gqpoptionaldense_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gqpoptionaldense_clear(void* _p);
void _gqpoptionaldense_destroy(void* _p);
void _gqpipmstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gqpipmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gqpipmstate_clear(void* _p);
void _gqpipmstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINLBFGS) || !defined(AE_PARTIAL_BUILD)
void minlbfgscreate(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minlbfgsstate* state,
     ae_state *_state);
void minlbfgscreatef(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minlbfgsstate* state,
     ae_state *_state);
void minlbfgssetcond(minlbfgsstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minlbfgssetxrep(minlbfgsstate* state,
     ae_bool needxrep,
     ae_state *_state);
void minlbfgssetstpmax(minlbfgsstate* state,
     double stpmax,
     ae_state *_state);
void minlbfgssetscale(minlbfgsstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minlbfgscreatex(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     ae_int_t flags,
     double diffstep,
     minlbfgsstate* state,
     ae_state *_state);
void minlbfgssetprecdefault(minlbfgsstate* state, ae_state *_state);
void minlbfgssetpreccholesky(minlbfgsstate* state,
     /* Real    */ const ae_matrix* p,
     ae_bool isupper,
     ae_state *_state);
void minlbfgssetprecdiag(minlbfgsstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void minlbfgssetprecscale(minlbfgsstate* state, ae_state *_state);
void minlbfgssetprecrankklbfgsfast(minlbfgsstate* state,
     /* Real    */ const ae_vector* d,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* w,
     ae_int_t cnt,
     ae_state *_state);
void minlbfgssetpreclowrankexact(minlbfgsstate* state,
     /* Real    */ const ae_vector* d,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* w,
     ae_int_t cnt,
     ae_state *_state);
ae_bool minlbfgsiteration(minlbfgsstate* state, ae_state *_state);
void minlbfgsoptguardgradient(minlbfgsstate* state,
     double teststep,
     ae_state *_state);
void minlbfgsoptguardsmoothness(minlbfgsstate* state,
     ae_int_t level,
     ae_state *_state);
void minlbfgsoptguardresults(minlbfgsstate* state,
     optguardreport* rep,
     ae_state *_state);
void minlbfgsoptguardnonc1test0results(const minlbfgsstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);
void minlbfgsoptguardnonc1test1results(minlbfgsstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);
void minlbfgsresults(const minlbfgsstate* state,
     /* Real    */ ae_vector* x,
     minlbfgsreport* rep,
     ae_state *_state);
void minlbfgsresultsbuf(const minlbfgsstate* state,
     /* Real    */ ae_vector* x,
     minlbfgsreport* rep,
     ae_state *_state);
void minlbfgsrestartfrom(minlbfgsstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minlbfgsrequesttermination(minlbfgsstate* state, ae_state *_state);
void minlbfgssetprotocolv1(minlbfgsstate* state, ae_state *_state);
void minlbfgssetprotocolv2(minlbfgsstate* state, ae_state *_state);
void _minlbfgsstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlbfgsstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlbfgsstate_clear(void* _p);
void _minlbfgsstate_destroy(void* _p);
void _minlbfgsreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlbfgsreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlbfgsreport_clear(void* _p);
void _minlbfgsreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_VIPMSOLVER) || !defined(AE_PARTIAL_BUILD)
void vipminitdense(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     ae_bool normalize,
     ae_state *_state);
void vipminitdensewithslacks(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nmain,
     ae_int_t n,
     ae_bool normalize,
     ae_state *_state);
void vipminitsparse(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     ae_bool normalize,
     ae_state *_state);
void vipmsetquadraticlinear(vipmstate* state,
     /* Real    */ const ae_matrix* denseh,
     const sparsematrix* sparseh,
     ae_int_t hkind,
     ae_bool isupper,
     /* Real    */ const ae_vector* c,
     ae_state *_state);
void vipmsetconstraints(vipmstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     ae_state *_state);
void vipmsetcond(vipmstate* state,
     double epsp,
     double epsd,
     double epsgap,
     ae_state *_state);
void vipmoptimize(vipmstate* state,
     ae_bool dropbigbounds,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _vipmvars_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmvars_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmvars_clear(void* _p);
void _vipmvars_destroy(void* _p);
void _vipmreducedsparsesystem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmreducedsparsesystem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmreducedsparsesystem_clear(void* _p);
void _vipmreducedsparsesystem_destroy(void* _p);
void _vipmrighthandside_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmrighthandside_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmrighthandside_clear(void* _p);
void _vipmrighthandside_destroy(void* _p);
void _vipmstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmstate_clear(void* _p);
void _vipmstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_IPM2SOLVER) || !defined(AE_PARTIAL_BUILD)
void ipm2init(ipm2state* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nuser,
     /* Real    */ const ae_matrix* denseh,
     const sparsematrix* sparseh,
     ae_int_t hkind,
     ae_bool isupper,
     /* Real    */ const ae_matrix* ccorr,
     /* Real    */ const ae_vector* dcorr,
     ae_int_t kcorr,
     /* Real    */ const ae_vector* c,
     double targetscale,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     ae_bool uniformlcscale,
     ae_bool skipnormalization,
     ae_state *_state);
void ipm2setreg(ipm2state* state,
     double regxy,
     /* Real    */ const ae_vector* originx,
     /* Real    */ const ae_vector* originy,
     ae_state *_state);
void ipm2setcond(ipm2state* state,
     double epsp,
     double epsd,
     double epsgap,
     ae_state *_state);
void ipm2setmaxits(ipm2state* state, ae_int_t maxits, ae_state *_state);
void ipm2proposeordering(ipm2state* state,
     ae_int_t n,
     ae_bool isdiagonalq,
     /* Boolean */ const ae_vector* hasbndl,
     /* Boolean */ const ae_vector* hasbndu,
     const sparsematrix* c,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     ae_int_t m,
     /* Integer */ ae_vector* p,
     ae_state *_state);
void ipm2setordering(ipm2state* state,
     /* Integer */ const ae_vector* p,
     ae_state *_state);
void ipm2optimize(ipm2state* state,
     ae_bool dropbigbounds,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _ipm2vars_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ipm2vars_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ipm2vars_clear(void* _p);
void _ipm2vars_destroy(void* _p);
void _ipm2reducedsystem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ipm2reducedsystem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ipm2reducedsystem_clear(void* _p);
void _ipm2reducedsystem_destroy(void* _p);
void _ipm2righthandside_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ipm2righthandside_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ipm2righthandside_clear(void* _p);
void _ipm2righthandside_destroy(void* _p);
void _ipm2state_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ipm2state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ipm2state_clear(void* _p);
void _ipm2state_destroy(void* _p);
#endif
#if defined(AE_COMPILE_NLCFSQP) || !defined(AE_PARTIAL_BUILD)
void minfsqpinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     const sparsematrix* sparsea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t lccnt,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     const nlpstoppingcriteria* criteria,
     ae_bool usedensebfgs,
     minfsqpstate* state,
     ae_state *_state);
void minfsqpsetadditsforctol(minfsqpstate* state,
     ae_int_t addits,
     double ctol,
     ae_state *_state);
void minfsqpsetinittrustrad(minfsqpstate* state,
     double rad0,
     ae_state *_state);
ae_bool minfsqpiteration(minfsqpstate* state,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     ae_state *_state);
void _minfsqpsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minfsqpsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minfsqpsubsolver_clear(void* _p);
void _minfsqpsubsolver_destroy(void* _p);
void _minfsqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minfsqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minfsqpstate_clear(void* _p);
void _minfsqpstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_DIFFEVO) || !defined(AE_PARTIAL_BUILD)
void gdemoinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t cntlc,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t cntnlc,
     ae_int_t popsize,
     ae_int_t epochscnt,
     ae_int_t seed,
     gdemostate* state,
     ae_state *_state);
void gdemosetprofile(gdemostate* state,
     ae_int_t profile,
     ae_state *_state);
void gdemosetfixedparams(gdemostate* state,
     ae_int_t strategy,
     double crossoverprob,
     double differentialweight,
     ae_state *_state);
void gdemosetsmallf(gdemostate* state, double f, ae_state *_state);
void gdemosetcondfx(gdemostate* state,
     double epsf,
     double epsx,
     ae_state *_state);
void gdemosetmodepenalty(gdemostate* state,
     double rhol1,
     double rhol2,
     ae_state *_state);
void gdemosetx0(gdemostate* state,
     /* Real    */ const ae_vector* x0,
     ae_state *_state);
ae_bool gdemoiteration(gdemostate* state,
     ae_bool userterminationneeded,
     ae_state *_state);
void _gdemopopulation_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gdemopopulation_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gdemopopulation_clear(void* _p);
void _gdemopopulation_destroy(void* _p);
void _gdemostate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gdemostate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gdemostate_clear(void* _p);
void _gdemostate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_SSGD) || !defined(AE_PARTIAL_BUILD)
void ssgdinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t cntlc,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t cntnlc,
     double rad0,
     double rad1,
     ae_int_t outerits,
     double rate0,
     double rate1,
     double momentum,
     ae_int_t maxits,
     double rho,
     ssgdstate* state,
     ae_state *_state);
ae_bool ssgditeration(ssgdstate* state,
     ae_bool userterminationneeded,
     ae_state *_state);
void _ssgdstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ssgdstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ssgdstate_clear(void* _p);
void _ssgdstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_ECQPSOLVER) || !defined(AE_PARTIAL_BUILD)
void ecqpsolve(ecqpstate* state,
     ae_int_t nuser,
     const sparsematrix* lowerh,
     /* Real    */ const ae_vector* c,
     const sparsematrix* sparsea,
     /* Real    */ const ae_vector* ae,
     ae_int_t m,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _ecqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ecqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ecqpstate_clear(void* _p);
void _ecqpstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_GIPM3) || !defined(AE_PARTIAL_BUILD)
double gipm3getinitprimdual(ae_state *_state);
void gipm3initbuf(/* Real    */ const ae_vector* x0,
     ae_int_t n,
     double eps,
     ae_int_t maxits,
     ae_int_t tracelevel,
     ae_bool probeonfailure,
     ae_bool analyzestep,
     gipm3state* state,
     ae_state *_state);
void gipm3setprofilequasinewton(gipm3state* state, ae_state *_state);
void gipm3setconstr(gipm3state* state,
     /* Real    */ const ae_vector* ceq,
     ae_int_t meq,
     /* Real    */ const ae_vector* ci,
     ae_int_t mi,
     /* Real    */ const ae_vector* ch,
     /* Boolean */ const ae_vector* chislinear,
     ae_int_t mh,
     ae_state *_state);
void gipm3setprimalbound(gipm3state* state, double bnd, ae_state *_state);
void gipm3setdualbound(gipm3state* state, double bnd, ae_state *_state);
void gipm3setearlystopping(gipm3state* state,
     /* Boolean */ const ae_vector* intmask,
     double dualbound,
     double primbound,
     ae_int_t minearlyits,
     double etol,
     ae_state *_state);
ae_bool gipm3iteration(gipm3state* state, ae_state *_state);
void gipm3results(gipm3state* state,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lag,
     ae_int_t* terminationtype,
     ae_state *_state);
void _gipm3vars_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gipm3vars_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gipm3vars_clear(void* _p);
void _gipm3vars_destroy(void* _p);
void _gipm3primalmult_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gipm3primalmult_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gipm3primalmult_clear(void* _p);
void _gipm3primalmult_destroy(void* _p);
void _gipm3rhs_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gipm3rhs_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gipm3rhs_clear(void* _p);
void _gipm3rhs_destroy(void* _p);
void _gipm3condensedsystem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gipm3condensedsystem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gipm3condensedsystem_clear(void* _p);
void _gipm3condensedsystem_destroy(void* _p);
void _gipm3condensedleastsquaressystem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gipm3condensedleastsquaressystem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gipm3condensedleastsquaressystem_clear(void* _p);
void _gipm3condensedleastsquaressystem_destroy(void* _p);
void _gipm3state_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gipm3state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gipm3state_clear(void* _p);
void _gipm3state_destroy(void* _p);
#endif
#if defined(AE_COMPILE_NLCGIPM3) || !defined(AE_PARTIAL_BUILD)
void nlpgipm3initbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     const nlpstoppingcriteria* criteria,
     ae_bool isdense,
     nlpgipm3state* state,
     ae_state *_state);
void nlpgipm3setlinearconstraints(nlpgipm3state* state,
     const sparsematrix* sparsea,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t m,
     ae_bool normalizea,
     ae_state *_state);
void nlpgipm3setnonlinearconstraints(nlpgipm3state* state,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     /* Boolean */ const ae_vector* ishard,
     ae_int_t nnlc,
     ae_state *_state);
void nlpgipm3setprimalbound(nlpgipm3state* state,
     double bnd,
     ae_state *_state);
void nlpgipm3setnonlinearityreports(nlpgipm3state* state,
     ae_bool doreports,
     ae_state *_state);
void nlpgipm3setdualbound(nlpgipm3state* state,
     double bnd,
     ae_state *_state);
void nlpgipm3setearlystopping(nlpgipm3state* state,
     /* Boolean */ const ae_vector* intmask,
     double dualbound,
     double primbound,
     ae_int_t minearlyits,
     double etol,
     ae_state *_state);
void nlpgipm3setlinearmask(nlpgipm3state* state,
     /* Boolean */ const ae_vector* linmask,
     ae_state *_state);
void nlpgipm3setmemlen(nlpgipm3state* state,
     ae_int_t memlen,
     ae_state *_state);
ae_bool nlpgipm3iteration(nlpgipm3state* state, ae_state *_state);
void _nlpgipm3state_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nlpgipm3state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nlpgipm3state_clear(void* _p);
void _nlpgipm3state_destroy(void* _p);
#endif
#if defined(AE_COMPILE_NLCSQP) || !defined(AE_PARTIAL_BUILD)
void minsqpinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     /* Real    */ const ae_matrix* cleic,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     const nlpstoppingcriteria* criteria,
     ae_bool usedensebfgs,
     minsqpstate* state,
     ae_state *_state);
ae_bool minsqpiteration(minsqpstate* state,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     ae_state *_state);
void _minsqpsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minsqpsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minsqpsubsolver_clear(void* _p);
void _minsqpsubsolver_destroy(void* _p);
void _minsqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minsqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minsqpstate_clear(void* _p);
void _minsqpstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_NLCAUL) || !defined(AE_PARTIAL_BUILD)
void minaulinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t cntlc,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t cntnlc,
     const nlpstoppingcriteria* criteria,
     ae_int_t maxouterits,
     minaulstate* state,
     ae_state *_state);
ae_bool minauliteration(minaulstate* state,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     ae_state *_state);
void inequalityshiftedbarrierfunction(double alpha,
     double* f,
     double* df,
     double* d2f,
     ae_state *_state);
void _minaulpreconditioner_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minaulpreconditioner_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minaulpreconditioner_clear(void* _p);
void _minaulpreconditioner_destroy(void* _p);
void _minaulstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minaulstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minaulstate_clear(void* _p);
void _minaulstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_DFGENMOD) || !defined(AE_PARTIAL_BUILD)
void dfgminitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     ae_int_t m,
     ae_bool isls,
     ae_int_t modeltype,
     const nlpstoppingcriteria* criteria,
     ae_int_t nnoisyrestarts,
     double rad0,
     ae_int_t maxfev,
     dfgmstate* state,
     ae_state *_state);
void dfgmsetlc2(dfgmstate* state,
     const sparsematrix* c,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     ae_int_t m,
     ae_state *_state);
void dfgmsetnlc2(dfgmstate* state,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t m,
     ae_state *_state);
ae_bool dfgmiteration(dfgmstate* state,
     ae_bool userterminationneeded,
     ae_state *_state);
void linregline(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     double* a,
     double* b,
     double* corrxy,
     ae_state *_state);
void _df2psmodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _df2psmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _df2psmodel_clear(void* _p);
void _df2psmodel_destroy(void* _p);
void _dfolsamodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dfolsamodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dfolsamodel_clear(void* _p);
void _dfolsamodel_destroy(void* _p);
void _dforbfmodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dforbfmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dforbfmodel_clear(void* _p);
void _dforbfmodel_destroy(void* _p);
void _dfgmstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dfgmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dfgmstate_clear(void* _p);
void _dfgmstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_DYNCRS) || !defined(AE_PARTIAL_BUILD)
void dyncrsinitempty(dynamiccrs* r,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state);
void dyncrscopy(const dynamiccrs* src, dynamiccrs* dst, ae_state *_state);
void dyncrscopydeleterowcol(const dynamiccrs* src,
     ae_int_t deletepos,
     dynamiccrs* dst,
     ae_state *_state);
void dyncrscopydeleterowscols(const dynamiccrs* src,
     const niset* deleteset,
     dynamiccrs* dst,
     /* Integer */ ae_vector* buf,
     ae_state *_state);
void dyncrscopyinsertrowcol(const dynamiccrs* src,
     ae_int_t insertpos,
     dynamiccrs* dst,
     ae_state *_state);
void dyncrsinitfromsparsecrs(const sparsematrix* s,
     dynamiccrs* r,
     ae_state *_state);
void dyncrsdropzeros(dynamiccrs* a, ae_state *_state);
void dyncrsdropoffdiagonalzeros(dynamiccrs* a, ae_state *_state);
void dyncrsclearrow(dynamiccrs* a, ae_int_t rowidx, ae_state *_state);
void dyncrsremovefromrow(dynamiccrs* a,
     ae_int_t rowidx,
     ae_int_t j,
     ae_state *_state);
void dyncrsfactoroutfromrow(dynamiccrs* a,
     ae_int_t rowidx,
     ae_int_t j,
     double c0,
     double c1,
     ae_int_t k,
     ae_bool checkcancellation,
     double* newval,
     double* shift,
     ae_state *_state);
void dyncrsaddrowto(dynamiccrs* a,
     ae_int_t row0,
     double c,
     ae_int_t row1,
     ae_bool retaindiagonal,
     ae_bool checkcancellation,
     ae_state *_state);
void dyncrsinsertwithrewrite(dynamiccrs* a,
     ae_int_t tgtrow,
     /* Integer */ const ae_vector* idx,
     /* Real    */ const ae_vector* vals,
     ae_int_t cnt,
     ae_state *_state);
void dyncrsremovesetfromrow(dynamiccrs* a,
     ae_int_t rowidx,
     const niset* s,
     ae_state *_state);
double dyncrsdotremovesetfromrow(dynamiccrs* a,
     ae_int_t rowidx,
     /* Real    */ const ae_vector* c,
     const niset* s,
     ae_state *_state);
double dyncrsgetexistingelement(dynamiccrs* a,
     ae_int_t i,
     ae_int_t j,
     ae_state *_state);
double dyncrsgetelementifexists(dynamiccrs* a,
     ae_int_t i,
     ae_int_t j,
     ae_state *_state);
void dyncrssetexistingelement(dynamiccrs* a,
     ae_int_t i,
     ae_int_t j,
     double v,
     ae_state *_state);
ae_int_t dyncrscountnonzeros(const dynamiccrs* a, ae_state *_state);
void _dynamiccrs_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dynamiccrs_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dynamiccrs_clear(void* _p);
void _dynamiccrs_destroy(void* _p);
#endif
#if defined(AE_COMPILE_NLPPRESOLVE) || !defined(AE_PARTIAL_BUILD)
void nlppresolvenonescaleuser(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* isintegral,
     /* Boolean */ const ae_vector* islinear,
     ae_int_t n,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t mlc,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     /* Boolean */ const ae_vector* ishard,
     ae_int_t mnlc,
     double eps,
     ae_bool dotrace,
     nlppresolveinfo* info,
     ae_state *_state);
void nlppresolvepowerfulnolagrangemults(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* isintegral,
     /* Boolean */ const ae_vector* islinear,
     ae_int_t n,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t mlc,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     /* Boolean */ const ae_vector* ishard,
     ae_int_t mnlc,
     double eps,
     ae_bool dotrace,
     nlppresolveinfo* info,
     ae_state *_state);
void nlppresolverunpackpoint(nlppresolveinfo* info,
     /* Real    */ const ae_vector* newx,
     /* Real    */ ae_vector* oldx,
     ae_state *_state);
void nlppresolverpackreplytosparse(nlppresolveinfo* info,
     /* Real    */ const ae_vector* replyfi,
     /* Real    */ const ae_vector* replydj,
     const sparsematrix* replysj,
     ae_bool isdensereply,
     /* Real    */ ae_vector* psfi,
     sparsematrix* pssj,
     /* Real    */ ae_vector* psfs,
     ae_state *_state);
void nlppresolverpackreplytosparsev1(nlppresolveinfo* info,
     /* Real    */ const ae_vector* replyfi,
     /* Real    */ const ae_matrix* replydj,
     /* Real    */ ae_vector* psfi,
     sparsematrix* pssj,
     /* Real    */ ae_vector* psfs,
     ae_state *_state);
void nlppresolverunpacksolution(nlppresolveinfo* info,
     /* Real    */ const ae_vector* psxs,
     /* Real    */ const ae_vector* psnlrep,
     /* Real    */ const ae_vector* pslagbc,
     /* Real    */ const ae_vector* pslaglc,
     /* Real    */ const ae_vector* pslagnlc,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* nlrep,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     /* Real    */ ae_vector* lagnlc,
     ae_state *_state);
void nlppresolverunpackerrors(nlppresolveinfo* info,
     double psrepbcerr,
     ae_int_t psrepbcidx,
     double psreplcerr,
     ae_int_t psreplcidx,
     double psrepnlcerr,
     ae_int_t psrepnlcidx,
     double psrepsclerr,
     double* repbcerr,
     ae_int_t* repbcidx,
     double* replcerr,
     ae_int_t* replcidx,
     double* repnlcerr,
     ae_int_t* repnlcidx,
     double* repsclfeaserr,
     ae_state *_state);
void _nlppresolveworkspace_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nlppresolveworkspace_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nlppresolveworkspace_clear(void* _p);
void _nlppresolveworkspace_destroy(void* _p);
void _nlppresolveinfo_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nlppresolveinfo_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nlppresolveinfo_clear(void* _p);
void _nlppresolveinfo_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINNLC) || !defined(AE_PARTIAL_BUILD)
void minnlccreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minnlcstate* state,
     ae_state *_state);
void minnlccreatebuf(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minnlcstate* state,
     ae_state *_state);
void minnlccreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minnlcstate* state,
     ae_state *_state);
void minnlccreatefbuf(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minnlcstate* state,
     ae_state *_state);
void minnlcsetbc(minnlcstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minnlcsetlc(minnlcstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minnlcsetlc2dense(minnlcstate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minnlcsetlc2(minnlcstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minnlcsetlc2mixed(minnlcstate* state,
     const sparsematrix* sparsea,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t kdense,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_state *_state);
void minnlcaddlc2dense(minnlcstate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state);
void minnlcaddlc2(minnlcstate* state,
     /* Integer */ const ae_vector* idxa,
     /* Real    */ const ae_vector* vala,
     ae_int_t nnz,
     double al,
     double au,
     ae_state *_state);
void minnlcaddlc2sparsefromdense(minnlcstate* state,
     /* Real    */ const ae_vector* da,
     double al,
     double au,
     ae_state *_state);
void minnlcsetnlc(minnlcstate* state,
     ae_int_t nlec,
     ae_int_t nlic,
     ae_state *_state);
void minnlcsetnlc2(minnlcstate* state,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     ae_state *_state);
void minnlcmarkashardnlc(minnlcstate* state,
     ae_int_t cidx,
     ae_bool ishard,
     ae_state *_state);
void minnlcmarkaslinearvar(minnlcstate* state,
     ae_int_t k,
     ae_state *_state);
void minnlcsetprimalbound(minnlcstate* state,
     double bnd,
     ae_state *_state);
void minnlcsetearlystopping(minnlcstate* state,
     /* Boolean */ const ae_vector* intmask,
     double dualbound,
     double primbound,
     ae_int_t minearlyits,
     double etol,
     ae_state *_state);
void minnlcsetpowerfulpresolvernomults(minnlcstate* state,
     ae_state *_state);
void minnlcsetnonlinearityreports(minnlcstate* state,
     ae_bool doreports,
     ae_state *_state);
void minnlcsetnumdiff(minnlcstate* state,
     ae_int_t formulatype,
     ae_state *_state);
void minnlcsetcond(minnlcstate* state,
     double eps,
     ae_int_t maxits,
     ae_state *_state);
void minnlcsetcond3(minnlcstate* state,
     double epsf,
     double eps,
     ae_int_t maxits,
     ae_state *_state);
void minnlcsetscale(minnlcstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minnlcsetstpmax(minnlcstate* state, double stpmax, ae_state *_state);
void minnlcsetalgoaul2(minnlcstate* state,
     ae_int_t maxouterits,
     ae_state *_state);
void minnlcsetalgosl1qp(minnlcstate* state, ae_state *_state);
void minnlcsetalgosl1qpbfgs(minnlcstate* state, ae_state *_state);
void minnlcsetalgosqp(minnlcstate* state, ae_state *_state);
void minnlcsetalgoorbit(minnlcstate* state,
     double rad0,
     ae_int_t maxnfev,
     ae_state *_state);
void minnlcsetalgosqpbfgs(minnlcstate* state, ae_state *_state);
void minnlcsetalgogipm2(minnlcstate* state, ae_state *_state);
void minnlcsetalgonlpipm(minnlcstate* state,
     ae_int_t memlen,
     ae_state *_state);
void minnlcsetxrep(minnlcstate* state, ae_bool needxrep, ae_state *_state);
ae_bool minnlciteration(minnlcstate* state, ae_state *_state);
void minnlcoptguardgradient(minnlcstate* state,
     double teststep,
     ae_state *_state);
void minnlcoptguardsmoothness(minnlcstate* state,
     ae_int_t level,
     ae_state *_state);
void minnlcoptguardresults(minnlcstate* state,
     optguardreport* rep,
     ae_state *_state);
void minnlcoptguardnonc1test0results(const minnlcstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);
void minnlcoptguardnonc1test1results(minnlcstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);
void minnlcresults(const minnlcstate* state,
     /* Real    */ ae_vector* x,
     minnlcreport* rep,
     ae_state *_state);
void minnlcresultsbuf(const minnlcstate* state,
     /* Real    */ ae_vector* x,
     minnlcreport* rep,
     ae_state *_state);
void minnlcrequesttermination(minnlcstate* state, ae_state *_state);
void minnlcrestartfrom(minnlcstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minnlcsetfsqpadditsforctol(minnlcstate* state,
     ae_int_t addits,
     double ctol,
     ae_state *_state);
void minnlcsetprotocolv1(minnlcstate* state, ae_state *_state);
void minnlcsetprotocolv2(minnlcstate* state, ae_state *_state);
void minnlcsetprotocolv2s(minnlcstate* state, ae_state *_state);
void _minnlcstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnlcstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnlcstate_clear(void* _p);
void _minnlcstate_destroy(void* _p);
void _minnlcreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnlcreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnlcreport_clear(void* _p);
void _minnlcreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_QQPSOLVER) || !defined(AE_PARTIAL_BUILD)
void qqploaddefaults(ae_int_t n, qqpsettings* s, ae_state *_state);
void qqpcopysettings(const qqpsettings* src,
     qqpsettings* dst,
     ae_state *_state);
void qqppreallocategrowdense(qqpbuffers* sstate,
     ae_int_t nexpected,
     ae_int_t ngrowto,
     ae_state *_state);
void qqpoptimize(const convexquadraticmodel* cqmac,
     const sparsematrix* sparseac,
     /* Real    */ const ae_matrix* denseac,
     ae_int_t akind,
     ae_bool isupper,
     /* Real    */ const ae_vector* bc,
     /* Real    */ const ae_vector* bndlc,
     /* Real    */ const ae_vector* bnduc,
     /* Real    */ const ae_vector* sc,
     /* Real    */ const ae_vector* xoriginc,
     ae_int_t nc,
     const qqpsettings* settings,
     qqpbuffers* sstate,
     /* Real    */ ae_vector* xs,
     ae_int_t* terminationtype,
     ae_state *_state);
void _qqpsettings_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qqpsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qqpsettings_clear(void* _p);
void _qqpsettings_destroy(void* _p);
void _qqpbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qqpbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qqpbuffers_clear(void* _p);
void _qqpbuffers_destroy(void* _p);
#endif
#if defined(AE_COMPILE_QPDENSEAULSOLVER) || !defined(AE_PARTIAL_BUILD)
void qpdenseaulloaddefaults(ae_int_t nmain,
     qpdenseaulsettings* s,
     ae_state *_state);
void qpdenseauloptimize(const convexquadraticmodel* a,
     const sparsematrix* sparsea,
     ae_int_t akind,
     ae_bool sparseaupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nn,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t dnec,
     ae_int_t dnic,
     const sparsematrix* scleic,
     ae_int_t snec,
     ae_int_t snic,
     ae_bool renormlc,
     const qpdenseaulsettings* settings,
     qpdenseaulbuffers* state,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _qpdenseaulsettings_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulsettings_clear(void* _p);
void _qpdenseaulsettings_destroy(void* _p);
void _qpdenseaulbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulbuffers_clear(void* _p);
void _qpdenseaulbuffers_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MONBI) || !defined(AE_PARTIAL_BUILD)
void nbiscaleandinitbuf(/* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     ae_int_t m,
     ae_int_t frontsize,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* sparsea,
     /* Real    */ const ae_matrix* densea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t ksparse,
     ae_int_t kdense,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     double epsx,
     ae_int_t maxits,
     ae_bool polishsolutions,
     nbistate* state,
     ae_state *_state);
ae_bool nbiiteration(nbistate* state, ae_state *_state);
void _nbistate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nbistate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nbistate_clear(void* _p);
void _nbistate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINMO) || !defined(AE_PARTIAL_BUILD)
void minmocreate(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minmostate* state,
     ae_state *_state);
void minmocreatef(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minmostate* state,
     ae_state *_state);
void minmosetalgonbi(minmostate* state,
     ae_int_t frontsize,
     ae_bool polishsolutions,
     ae_state *_state);
void minmosetbc(minmostate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minmosetlc2dense(minmostate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minmosetlc2(minmostate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minmosetlc2mixed(minmostate* state,
     const sparsematrix* sparsea,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t kdense,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_state *_state);
void minmoaddlc2dense(minmostate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state);
void minmoaddlc2(minmostate* state,
     /* Integer */ const ae_vector* idxa,
     /* Real    */ const ae_vector* vala,
     ae_int_t nnz,
     double al,
     double au,
     ae_state *_state);
void minmoaddlc2sparsefromdense(minmostate* state,
     /* Real    */ const ae_vector* da,
     double al,
     double au,
     ae_state *_state);
void minmosetnlc2(minmostate* state,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     ae_state *_state);
void minmosetcond(minmostate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minmosetscale(minmostate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minmosetxrep(minmostate* state, ae_bool needxrep, ae_state *_state);
ae_bool minmoiteration(minmostate* state, ae_state *_state);
void minmoresults(const minmostate* state,
     /* Real    */ ae_matrix* paretofront,
     ae_int_t* frontsize,
     minmoreport* rep,
     ae_state *_state);
void minmorequesttermination(minmostate* state, ae_state *_state);
void minmorestartfrom(minmostate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minmosetprotocolv1(minmostate* state, ae_state *_state);
void _minmostate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minmostate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minmostate_clear(void* _p);
void _minmostate_destroy(void* _p);
void _minmoreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minmoreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minmoreport_clear(void* _p);
void _minmoreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINBC) || !defined(AE_PARTIAL_BUILD)
void minbccreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minbcstate* state,
     ae_state *_state);
void minbccreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minbcstate* state,
     ae_state *_state);
void minbcsetbc(minbcstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minbcsetcond(minbcstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minbcsetscale(minbcstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minbcsetprecdefault(minbcstate* state, ae_state *_state);
void minbcsetprecdiag(minbcstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void minbcsetprecscale(minbcstate* state, ae_state *_state);
void minbcsetxrep(minbcstate* state, ae_bool needxrep, ae_state *_state);
void minbcsetstpmax(minbcstate* state, double stpmax, ae_state *_state);
ae_bool minbciteration(minbcstate* state, ae_state *_state);
void minbcoptguardgradient(minbcstate* state,
     double teststep,
     ae_state *_state);
void minbcoptguardsmoothness(minbcstate* state,
     ae_int_t level,
     ae_state *_state);
void minbcoptguardresults(minbcstate* state,
     optguardreport* rep,
     ae_state *_state);
void minbcoptguardnonc1test0results(const minbcstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);
void minbcoptguardnonc1test1results(minbcstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);
void minbcresults(const minbcstate* state,
     /* Real    */ ae_vector* x,
     minbcreport* rep,
     ae_state *_state);
void minbcresultsbuf(const minbcstate* state,
     /* Real    */ ae_vector* x,
     minbcreport* rep,
     ae_state *_state);
void minbcrestartfrom(minbcstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minbcrequesttermination(minbcstate* state, ae_state *_state);
void minbcsetprotocolv1(minbcstate* state, ae_state *_state);
void _minbcstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minbcstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minbcstate_clear(void* _p);
void _minbcstate_destroy(void* _p);
void _minbcreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minbcreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minbcreport_clear(void* _p);
void _minbcreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINNS) || !defined(AE_PARTIAL_BUILD)
void minnscreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minnsstate* state,
     ae_state *_state);
void minnscreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minnsstate* state,
     ae_state *_state);
void minnssetbc(minnsstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minnssetlc(minnsstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minnssetnlc(minnsstate* state,
     ae_int_t nlec,
     ae_int_t nlic,
     ae_state *_state);
void minnssetcond(minnsstate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minnssetscale(minnsstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minnssetalgoags(minnsstate* state,
     double radius,
     double penalty,
     ae_state *_state);
void minnssetxrep(minnsstate* state, ae_bool needxrep, ae_state *_state);
void minnsrequesttermination(minnsstate* state, ae_state *_state);
ae_bool minnsiteration(minnsstate* state, ae_state *_state);
void minnsresults(const minnsstate* state,
     /* Real    */ ae_vector* x,
     minnsreport* rep,
     ae_state *_state);
void minnsresultsbuf(const minnsstate* state,
     /* Real    */ ae_vector* x,
     minnsreport* rep,
     ae_state *_state);
void minnsrestartfrom(minnsstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minnssetprotocolv1(minnsstate* state, ae_state *_state);
void _minnsqp_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnsqp_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnsqp_clear(void* _p);
void _minnsqp_destroy(void* _p);
void _minnsstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnsstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnsstate_clear(void* _p);
void _minnsstate_destroy(void* _p);
void _minnsreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnsreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnsreport_clear(void* _p);
void _minnsreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINDF) || !defined(AE_PARTIAL_BUILD)
void mindfcreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     mindfstate* state,
     ae_state *_state);
void mindfsetbc(mindfstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void mindfsetlc2dense(mindfstate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void mindfsetnlc2(mindfstate* state,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     ae_state *_state);
void mindfsetcondfx(mindfstate* state,
     double epsf,
     double epsx,
     ae_state *_state);
void mindfusetimers(mindfstate* state,
     ae_bool usetimers,
     ae_state *_state);
void mindfsetcondf(mindfstate* state, double epsf, ae_state *_state);
void mindfsetscale(mindfstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void mindfsetxrep(mindfstate* state, ae_bool needxrep, ae_state *_state);
void mindfrequesttermination(mindfstate* state, ae_state *_state);
void mindfsetseed(mindfstate* s, ae_int_t seedval, ae_state *_state);
void mindfsetalgogdemo(mindfstate* state,
     ae_int_t epochscnt,
     ae_int_t popsize,
     ae_state *_state);
void mindfsetgdemoprofilerobust(mindfstate* state, ae_state *_state);
void mindfsetgdemoprofilequick(mindfstate* state, ae_state *_state);
void mindfsetgdemopenalty(mindfstate* state,
     double rho1,
     double rho2,
     ae_state *_state);
void mindfsetsmallf(mindfstate* state, double f, ae_state *_state);
void mindfsetalgogdemofixed(mindfstate* state,
     ae_int_t epochscnt,
     ae_int_t strategy,
     double crossoverprob,
     double differentialweight,
     ae_int_t popsize,
     ae_state *_state);
ae_bool mindfiteration(mindfstate* state, ae_state *_state);
void mindfresults(const mindfstate* state,
     /* Real    */ ae_vector* x,
     mindfreport* rep,
     ae_state *_state);
void mindfresultsbuf(const mindfstate* state,
     /* Real    */ ae_vector* x,
     mindfreport* rep,
     ae_state *_state);
void mindfsetprotocolv2(mindfstate* state, ae_state *_state);
void _mindfstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mindfstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mindfstate_clear(void* _p);
void _mindfstate_destroy(void* _p);
void _mindfreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mindfreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mindfreport_clear(void* _p);
void _mindfreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_NLS) || !defined(AE_PARTIAL_BUILD)
void nlscreatedfo(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     nlsstate* state,
     ae_state *_state);
void nlssetalgo2ps(nlsstate* state,
     ae_int_t nnoisyrestarts,
     ae_state *_state);
void nlssetalgodfolsa(nlsstate* state,
     ae_int_t nnoisyrestarts,
     ae_state *_state);
void nlssetcond(nlsstate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void nlssetxrep(nlsstate* state, ae_bool needxrep, ae_state *_state);
void nlssetscale(nlsstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void nlssetbc(nlsstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
ae_bool nlsiteration(nlsstate* state, ae_state *_state);
void nlsresults(const nlsstate* state,
     /* Real    */ ae_vector* x,
     nlsreport* rep,
     ae_state *_state);
void nlsresultsbuf(const nlsstate* state,
     /* Real    */ ae_vector* x,
     nlsreport* rep,
     ae_state *_state);
void nlsrestartfrom(nlsstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void nlsrequesttermination(nlsstate* state, ae_state *_state);
void nlssetprotocolv2(nlsstate* state, ae_state *_state);
void _nlsstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nlsstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nlsstate_clear(void* _p);
void _nlsstate_destroy(void* _p);
void _nlsreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nlsreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nlsreport_clear(void* _p);
void _nlsreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_LPQPPRESOLVE) || !defined(AE_PARTIAL_BUILD)
void presolvenonescaleuser(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* sparseh,
     ae_bool isupper,
     ae_bool hash,
     ae_int_t n,
     const sparsematrix* sparsea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     xquadraticconstraints* xqc,
     xconicconstraints* xcc,
     ae_bool dotrace,
     presolveinfo* info,
     ae_state *_state);
void presolvelp(/* Real    */ const ae_vector* raws,
     /* Real    */ const ae_vector* rawc,
     /* Real    */ const ae_vector* rawbndl,
     /* Real    */ const ae_vector* rawbndu,
     ae_int_t n,
     const sparsematrix* rawsparsea,
     /* Real    */ const ae_vector* rawal,
     /* Real    */ const ae_vector* rawau,
     ae_int_t m,
     xquadraticconstraints* xqc,
     xconicconstraints* xcc,
     ae_bool dotrace,
     presolveinfo* presolved,
     ae_state *_state);
void presolveqp(/* Real    */ const ae_vector* raws,
     /* Real    */ const ae_vector* xorigin,
     /* Real    */ const ae_vector* rawc,
     /* Real    */ const ae_vector* rawbndl,
     /* Real    */ const ae_vector* rawbndu,
     const sparsematrix* rawsparseh,
     ae_bool isupper,
     ae_bool hash,
     ae_int_t n,
     const sparsematrix* rawsparsea,
     /* Real    */ const ae_vector* rawal,
     /* Real    */ const ae_vector* rawau,
     ae_int_t m,
     xquadraticconstraints* rawxqc,
     xconicconstraints* rawxcc,
     ae_bool dotrace,
     presolveinfo* presolved,
     ae_state *_state);
void presolvefwd(presolveinfo* s,
     /* Real    */ ae_vector* xx,
     ae_state *_state);
void presolvebwd(presolveinfo* info,
     /* Real    */ ae_vector* x,
     /* Integer */ ae_vector* stats,
     ae_bool needstats,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     /* Real    */ ae_vector* lagqc,
     ae_state *_state);
void _dynamiccrsqconstraint_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dynamiccrsqconstraint_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dynamiccrsqconstraint_clear(void* _p);
void _dynamiccrsqconstraint_destroy(void* _p);
void _dynamiccrsqconstraints_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dynamiccrsqconstraints_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dynamiccrsqconstraints_clear(void* _p);
void _dynamiccrsqconstraints_destroy(void* _p);
void _presolvebuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _presolvebuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _presolvebuffers_clear(void* _p);
void _presolvebuffers_destroy(void* _p);
void _presolverstack_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _presolverstack_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _presolverstack_clear(void* _p);
void _presolverstack_destroy(void* _p);
void _presolvervcstats_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _presolvervcstats_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _presolvervcstats_clear(void* _p);
void _presolvervcstats_destroy(void* _p);
void _presolveinfo_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _presolveinfo_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _presolveinfo_clear(void* _p);
void _presolveinfo_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINQP) || !defined(AE_PARTIAL_BUILD)
void minqpcreate(ae_int_t n, minqpstate* state, ae_state *_state);
void minqpsetlinearterm(minqpstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state);
void minqpsetquadraticterm(minqpstate* state,
     /* Real    */ const ae_matrix* a,
     ae_bool isupper,
     ae_state *_state);
void minqpsetquadratictermsparse(minqpstate* state,
     const sparsematrix* a,
     ae_bool isupper,
     ae_state *_state);
void minqpsetstartingpoint(minqpstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minqpsetorigin(minqpstate* state,
     /* Real    */ const ae_vector* xorigin,
     ae_state *_state);
void minqpsetscale(minqpstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minqpsetscaleautodiag(minqpstate* state, ae_state *_state);
void minqpsetalgodenseaul(minqpstate* state,
     double epsx,
     double rho,
     ae_int_t itscnt,
     ae_state *_state);
void minqpsetalgodenseipm(minqpstate* state, double eps, ae_state *_state);
void minqpsetalgosparseipm(minqpstate* state,
     double eps,
     ae_state *_state);
void minqpsetalgodensegenipm(minqpstate* state,
     double eps,
     ae_state *_state);
void minqpsetalgosparsegenipm(minqpstate* state,
     double eps,
     ae_state *_state);
void minqpsetalgohybridgenipm(minqpstate* state,
     double eps,
     ae_int_t memlen,
     ae_int_t maxoffdiag,
     ae_state *_state);
void minqpsetalgosparseecqp(minqpstate* state, ae_state *_state);
void minqpsetalgoquickqp(minqpstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxouterits,
     ae_bool usenewton,
     ae_state *_state);
void minqpsetbc(minqpstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minqpsetbcall(minqpstate* state,
     double bndl,
     double bndu,
     ae_state *_state);
void minqpsetbci(minqpstate* state,
     ae_int_t i,
     double bndl,
     double bndu,
     ae_state *_state);
void minqpsetlc(minqpstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minqpsetlcsparse(minqpstate* state,
     const sparsematrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minqpsetlcmixed(minqpstate* state,
     const sparsematrix* sparsec,
     /* Integer */ const ae_vector* sparsect,
     ae_int_t sparsek,
     /* Real    */ const ae_matrix* densec,
     /* Integer */ const ae_vector* densect,
     ae_int_t densek,
     ae_state *_state);
void minqpsetlcmixedlegacy(minqpstate* state,
     /* Real    */ const ae_matrix* densec,
     /* Integer */ const ae_vector* densect,
     ae_int_t densek,
     const sparsematrix* sparsec,
     /* Integer */ const ae_vector* sparsect,
     ae_int_t sparsek,
     ae_state *_state);
void minqpsetlc2dense(minqpstate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minqpsetlc2(minqpstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minqpsetlc2mixed(minqpstate* state,
     const sparsematrix* sparsea,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t kdense,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_state *_state);
void minqpaddlc2dense(minqpstate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state);
void minqpaddlc2(minqpstate* state,
     /* Integer */ const ae_vector* idxa,
     /* Real    */ const ae_vector* vala,
     ae_int_t nnz,
     double al,
     double au,
     ae_state *_state);
void minqpaddlc2sparsefromdense(minqpstate* state,
     /* Real    */ const ae_vector* da,
     double al,
     double au,
     ae_state *_state);
void minqpclearqc(minqpstate* state, ae_state *_state);
ae_int_t minqpaddqc2(minqpstate* state,
     const sparsematrix* q,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     double cl,
     double cu,
     ae_bool applyorigin,
     ae_state *_state);
ae_int_t minqpaddqc2list(minqpstate* state,
     /* Integer */ const ae_vector* qridx,
     /* Integer */ const ae_vector* qcidx,
     /* Real    */ const ae_vector* qvals,
     ae_int_t qnnz,
     ae_bool isupper,
     /* Integer */ const ae_vector* bidx,
     /* Real    */ const ae_vector* bvals,
     ae_int_t bnnz,
     double cl,
     double cu,
     ae_bool applyorigin,
     ae_state *_state);
ae_int_t minqpaddqc2dense(minqpstate* state,
     /* Real    */ const ae_matrix* q,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     double cl,
     double cu,
     ae_bool applyorigin,
     ae_state *_state);
void minqpclearcc(minqpstate* state, ae_state *_state);
ae_int_t minqpaddsoccprimitive(minqpstate* state,
     ae_int_t range0,
     ae_int_t range1,
     ae_int_t axisidx,
     ae_bool applyorigin,
     ae_state *_state);
ae_int_t minqpaddsoccorthogonal(minqpstate* state,
     /* Integer */ const ae_vector* idx,
     /* Real    */ const ae_vector* a,
     /* Real    */ const ae_vector* c,
     ae_int_t k,
     double theta,
     ae_bool applyorigin,
     ae_state *_state);
ae_int_t minqpaddpowccprimitive(minqpstate* state,
     ae_int_t range0,
     ae_int_t range1,
     ae_int_t axisidx,
     double alpha,
     ae_bool applyorigin,
     ae_state *_state);
ae_int_t minqpaddpowccorthogonal(minqpstate* state,
     /* Integer */ const ae_vector* idx,
     /* Real    */ const ae_vector* a,
     /* Real    */ const ae_vector* c,
     ae_int_t k,
     double theta,
     /* Real    */ const ae_vector* alphav,
     ae_int_t kpow,
     ae_bool applyorigin,
     ae_state *_state);
void minqpoptimize(minqpstate* state, ae_state *_state);
void minqpresults(const minqpstate* state,
     /* Real    */ ae_vector* x,
     minqpreport* rep,
     ae_state *_state);
void minqpresultsbuf(const minqpstate* state,
     /* Real    */ ae_vector* x,
     minqpreport* rep,
     ae_state *_state);
void minqpexport(minqpstate* state, qpxproblem* p, ae_state *_state);
void minqpimport(qpxproblem* p, minqpstate* s, ae_state *_state);
void minqpsetlineartermfast(minqpstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state);
void minqpsetquadratictermfast(minqpstate* state,
     /* Real    */ const ae_matrix* a,
     ae_bool isupper,
     double s,
     ae_state *_state);
void minqprewritediagonal(minqpstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minqpsetstartingpointfast(minqpstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minqpsetoriginfast(minqpstate* state,
     /* Real    */ const ae_vector* xorigin,
     ae_state *_state);
void _minqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minqpstate_clear(void* _p);
void _minqpstate_destroy(void* _p);
void _minqpreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minqpreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minqpreport_clear(void* _p);
void _minqpreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINLM) || !defined(AE_PARTIAL_BUILD)
void minlmcreatevj(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minlmstate* state,
     ae_state *_state);
void minlmcreatev(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minlmstate* state,
     ae_state *_state);
void minlmsetcond(minlmstate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minlmsetxrep(minlmstate* state, ae_bool needxrep, ae_state *_state);
void minlmsetstpmax(minlmstate* state, double stpmax, ae_state *_state);
void minlmsetscale(minlmstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minlmsetbc(minlmstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minlmsetlc(minlmstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minlmsetacctype(minlmstate* state,
     ae_int_t acctype,
     ae_state *_state);
void minlmsetnonmonotonicsteps(minlmstate* state,
     ae_int_t cnt,
     ae_state *_state);
void minlmsetnumdiff(minlmstate* state,
     ae_int_t formulatype,
     ae_state *_state);
ae_bool minlmiteration(minlmstate* state, ae_state *_state);
void minlmoptguardgradient(minlmstate* state,
     double teststep,
     ae_state *_state);
void minlmoptguardresults(minlmstate* state,
     optguardreport* rep,
     ae_state *_state);
void minlmresults(const minlmstate* state,
     /* Real    */ ae_vector* x,
     minlmreport* rep,
     ae_state *_state);
void minlmresultsbuf(const minlmstate* state,
     /* Real    */ ae_vector* x,
     minlmreport* rep,
     ae_state *_state);
void minlmrestartfrom(minlmstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minlmrequesttermination(minlmstate* state, ae_state *_state);
void minlmsetprotocolv1(minlmstate* state, ae_state *_state);
void minlmsetprotocolv2(minlmstate* state, ae_state *_state);
void unpackdj(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_vector* replydj,
     /* Real    */ ae_matrix* jac,
     ae_state *_state);
void _minlmstepfinder_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlmstepfinder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlmstepfinder_clear(void* _p);
void _minlmstepfinder_destroy(void* _p);
void _minlmstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlmstate_clear(void* _p);
void _minlmstate_destroy(void* _p);
void _minlmreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlmreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlmreport_clear(void* _p);
void _minlmreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_REVISEDDUALSIMPLEX) || !defined(AE_PARTIAL_BUILD)
void dsssettingsinit(dualsimplexsettings* settings, ae_state *_state);
void dssinit(ae_int_t n, dualsimplexstate* s, ae_state *_state);
void dsssetproblem(dualsimplexstate* state,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_matrix* densea,
     const sparsematrix* sparsea,
     ae_int_t akind,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     const dualsimplexbasis* proposedbasis,
     ae_int_t basisinittype,
     const dualsimplexsettings* settings,
     ae_state *_state);
void dssexportbasis(const dualsimplexstate* state,
     dualsimplexbasis* basis,
     ae_state *_state);
void dssoptimize(dualsimplexstate* state,
     const dualsimplexsettings* settings,
     ae_state *_state);
void _dualsimplexsettings_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsettings_clear(void* _p);
void _dualsimplexsettings_destroy(void* _p);
void _dssvector_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dssvector_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dssvector_clear(void* _p);
void _dssvector_destroy(void* _p);
void _dualsimplexbasis_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexbasis_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexbasis_clear(void* _p);
void _dualsimplexbasis_destroy(void* _p);
void _dualsimplexsubproblem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsubproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsubproblem_clear(void* _p);
void _dualsimplexsubproblem_destroy(void* _p);
void _dualsimplexstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexstate_clear(void* _p);
void _dualsimplexstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_LPSOLVERS) || !defined(AE_PARTIAL_BUILD)
void minlpcreate(ae_int_t n, minlpstate* state, ae_state *_state);
void minlpsetalgodss(minlpstate* state, double eps, ae_state *_state);
void minlpsetalgoipm(minlpstate* state, double eps, ae_state *_state);
void minlpsetcost(minlpstate* state,
     /* Real    */ const ae_vector* c,
     ae_state *_state);
void minlpsetscale(minlpstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minlpsetbc(minlpstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minlpsetbcall(minlpstate* state,
     double bndl,
     double bndu,
     ae_state *_state);
void minlpsetbci(minlpstate* state,
     ae_int_t i,
     double bndl,
     double bndu,
     ae_state *_state);
void minlpsetlc(minlpstate* state,
     /* Real    */ const ae_matrix* a,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minlpsetlc2dense(minlpstate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minlpsetlc2(minlpstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minlpaddlc2dense(minlpstate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state);
void minlpaddlc2(minlpstate* state,
     /* Integer */ const ae_vector* idxa,
     /* Real    */ const ae_vector* vala,
     ae_int_t nnz,
     double al,
     double au,
     ae_state *_state);
void minlpoptimize(minlpstate* state, ae_state *_state);
void minlpresults(const minlpstate* state,
     /* Real    */ ae_vector* x,
     minlpreport* rep,
     ae_state *_state);
void minlpresultsbuf(const minlpstate* state,
     /* Real    */ ae_vector* x,
     minlpreport* rep,
     ae_state *_state);
void _minlpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlpstate_clear(void* _p);
void _minlpstate_destroy(void* _p);
void _minlpreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlpreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlpreport_clear(void* _p);
void _minlpreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINCG) || !defined(AE_PARTIAL_BUILD)
void mincgcreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     mincgstate* state,
     ae_state *_state);
void mincgcreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     mincgstate* state,
     ae_state *_state);
void mincgsetcond(mincgstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void mincgsetscale(mincgstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void mincgsetxrep(mincgstate* state, ae_bool needxrep, ae_state *_state);
void mincgsetdrep(mincgstate* state, ae_bool needdrep, ae_state *_state);
void mincgsetcgtype(mincgstate* state, ae_int_t cgtype, ae_state *_state);
void mincgsetstpmax(mincgstate* state, double stpmax, ae_state *_state);
void mincgsuggeststep(mincgstate* state, double stp, ae_state *_state);
double mincglastgoodstep(mincgstate* state, ae_state *_state);
void mincgsetprecdefault(mincgstate* state, ae_state *_state);
void mincgsetprecdiag(mincgstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void mincgsetprecscale(mincgstate* state, ae_state *_state);
ae_bool mincgiteration(mincgstate* state, ae_state *_state);
void mincgoptguardgradient(mincgstate* state,
     double teststep,
     ae_state *_state);
void mincgoptguardsmoothness(mincgstate* state,
     ae_int_t level,
     ae_state *_state);
void mincgoptguardresults(mincgstate* state,
     optguardreport* rep,
     ae_state *_state);
void mincgoptguardnonc1test0results(const mincgstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);
void mincgoptguardnonc1test1results(mincgstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);
void mincgresults(const mincgstate* state,
     /* Real    */ ae_vector* x,
     mincgreport* rep,
     ae_state *_state);
void mincgresultsbuf(const mincgstate* state,
     /* Real    */ ae_vector* x,
     mincgreport* rep,
     ae_state *_state);
void mincgrestartfrom(mincgstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void mincgrequesttermination(mincgstate* state, ae_state *_state);
void mincgsetprecdiagfast(mincgstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void mincgsetpreclowrankfast(mincgstate* state,
     /* Real    */ const ae_vector* d1,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* v,
     ae_int_t vcnt,
     ae_state *_state);
void mincgsetprecvarpart(mincgstate* state,
     /* Real    */ const ae_vector* d2,
     ae_state *_state);
void mincgsetprotocolv1(mincgstate* state, ae_state *_state);
void _mincgstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mincgstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mincgstate_clear(void* _p);
void _mincgstate_destroy(void* _p);
void _mincgreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mincgreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mincgreport_clear(void* _p);
void _mincgreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINCOMP) || !defined(AE_PARTIAL_BUILD)
void minlbfgssetdefaultpreconditioner(minlbfgsstate* state,
     ae_state *_state);
void minlbfgssetcholeskypreconditioner(minlbfgsstate* state,
     /* Real    */ const ae_matrix* p,
     ae_bool isupper,
     ae_state *_state);
void minbleicsetbarrierwidth(minbleicstate* state,
     double mu,
     ae_state *_state);
void minbleicsetbarrierdecay(minbleicstate* state,
     double mudecay,
     ae_state *_state);
void minasacreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     minasastate* state,
     ae_state *_state);
void minasasetcond(minasastate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minasasetxrep(minasastate* state, ae_bool needxrep, ae_state *_state);
void minasasetalgorithm(minasastate* state,
     ae_int_t algotype,
     ae_state *_state);
void minasasetstpmax(minasastate* state, double stpmax, ae_state *_state);
ae_bool minasaiteration(minasastate* state, ae_state *_state);
void minasaresults(const minasastate* state,
     /* Real    */ ae_vector* x,
     minasareport* rep,
     ae_state *_state);
void minasaresultsbuf(const minasastate* state,
     /* Real    */ ae_vector* x,
     minasareport* rep,
     ae_state *_state);
void minasarestartfrom(minasastate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minasasetprotocolv1(minasastate* state, ae_state *_state);
void _minasastate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minasastate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minasastate_clear(void* _p);
void _minasastate_destroy(void* _p);
void _minasareport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minasareport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minasareport_clear(void* _p);
void _minasareport_destroy(void* _p);
#endif

}
#endif

