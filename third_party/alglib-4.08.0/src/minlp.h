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
#ifndef _minlp_pkg_h
#define _minlp_pkg_h
#include "ap.h"
#include "alglibinternal.h"
#include "alglibmisc.h"
#include "linalg.h"
#include "solvers.h"
#include "optimization.h"
#include "interpolation.h"

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (DATATYPES)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_BBGD) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t leafid;
    ae_int_t n;
    ae_vector x0;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t branchbucket;
    double parentfdual;
    ae_vector parentlinearity;
    ae_int_t branchvar;
    double branchval;
    ae_int_t ncuttingplanes;
    ae_bool hasprimalsolution;
    ae_vector xprim;
    double fprim;
    double hprim;
    ae_bool hasdualsolution;
    ae_vector bestxdual;
    double bestfdual;
    double bestfdualearlyerror;
    double besthdual;
    ae_int_t besttt;
    ae_int_t bestits;
    ae_vector worstxdual;
    double worstfdual;
    double worsthdual;
    ae_bool bestdualisintfeas;
    double dualbound;
    ae_bool earlystopped;
    ae_bool donotfathom;
    ae_vector subproblemlinearity;
} bbgdsubproblem;
typedef struct
{
    ae_int_t subsolverstatus;
    bbgdsubproblem subproblem;
    minnlcstate nlpsubsolver;
    ipm2state qpsubsolver;
    minnlcreport nlprep;
    rcommstate rstate;
    ae_vector xsol;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector tmp3;
    ae_vector tmpi;
    ae_vector wrks;
    ae_vector wrkbndl;
    ae_vector wrkbndu;
    ae_vector wrkb;
    ae_vector psvpackxyperm;
    ae_vector psvunpackxyperm;
    ae_vector psvs;
    ae_vector psvxorigin;
    ae_vector psvbndl;
    ae_vector psvbndu;
    ae_vector psvb;
    ae_vector psvfixvals;
    ae_vector psvrawbndl;
    ae_vector psvrawbndu;
    ae_int_t npsv;
    sparsematrix psva;
    sparsematrix psvsparsec;
    ae_vector psvcl;
    ae_vector psvcu;
    ae_int_t psvlccnt;
    ae_vector psvqpordering;
} bbgdfrontsubsolver;
typedef struct
{
    ae_int_t entrystatus;
    ae_bool addstatussolutionsaggregated;
    ae_bool addstatusdecisionsmade;
    ae_int_t entrylock;
    ae_bool isrootentry;
    ae_int_t maxsubsolvers;
    ae_bool hastimeout;
    ae_int_t timeout;
    stimer timerlocal;
    bbgdsubproblem parentsubproblem;
    bbgdsubproblem rootproblem;
    bbgdsubproblem childsubproblem0;
    bbgdsubproblem childsubproblem1;
    rcommstate rstate;
    hqrndstate entryrng;
    ae_int_t entrynfev;
    ae_int_t entrynsubproblems;
    ae_bool fathomroot;
    ae_bool fathomchild0;
    ae_bool fathomchild1;
    ae_obj_array subsolvers;
    ae_obj_array spqueue;
    ae_obj_array solutions;
    bbgdsubproblem tmpsubproblem;
    ae_vector tmpreliablebranchidx;
    ae_vector tmpreliablebranchscore;
    ae_vector tmpunreliablebranchidx;
    ae_vector tmpunreliablebranchscore;
    ae_vector tmpchosenbranchidx;
    ae_vector tmpchosenbranchscore;
} bbgdfrontentry;
typedef struct
{
    ae_int_t frontmode;
    ae_int_t frontstatus;
    ae_bool popmostrecent;
    ae_int_t backtrackbudget;
    ae_int_t frontsize;
    ae_obj_array entries;
    ae_shared_pool entrypool;
    rcommstate rstate;
    ae_vector jobs;
} bbgdfront;
typedef struct
{
    ae_int_t n;
    nlpstoppingcriteria criteria;
    double diffstep;
    ae_int_t convexityflag;
    double nonconvexitygain;
    double pdgap;
    double ctol;
    double epsx;
    double epsf;
    ae_int_t subsolveralgo;
    ae_int_t subsolvermemlen;
    ae_int_t nonrootmaxitslin;
    ae_int_t nonrootmaxitsconst;
    double nonrootmaxitsaboveaverage;
    ae_int_t nonrootadditsforfeasibility;
    double pseudocostmu;
    double pseudocostminfrac;
    double pseudocostinfeaspenaly;
    ae_int_t nmultistarts;
    ae_int_t branchingtype;
    ae_int_t krel;
    ae_int_t kevalunreliable;
    ae_int_t kevalreliable;
    ae_int_t dodiving;
    ae_int_t timeout;
    ae_int_t bbgdgroupsize;
    ae_int_t maxsubsolvers;
    ae_bool issuesparserequests;
    ae_bool forceserial;
    ae_int_t softmaxnodes;
    ae_int_t hardmaxnodes;
    ae_int_t maxprimalcandidates;
    ae_int_t syncinterval;
    ae_vector s;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector isintegral;
    ae_vector isbinary;
    ae_vector islinear;
    ae_int_t objtype;
    sparsematrix obja;
    ae_vector objb;
    double objc0;
    ae_vector qpordering;
    sparsematrix rawa;
    ae_vector rawal;
    ae_vector rawau;
    ae_vector lcsrcidx;
    ae_int_t lccnt;
    ae_int_t nnlc;
    ae_vector nl;
    ae_vector nu;
    ae_bool hasx0;
    ae_vector x0;
    ae_bool userterminationneeded;
    ae_vector xc;
    ae_int_t repnfev;
    ae_int_t repnsubproblems;
    ae_int_t repntreenodes;
    ae_int_t repnnodesbeforefeasibility;
    ae_int_t repnprimalcandidates;
    ae_int_t repterminationtype;
    double repf;
    double reppdgap;
    stimer timerglobal;
    ae_int_t lastlaconicreportepoch;
    ae_bool dotrace;
    ae_bool dolaconictrace;
    ae_bool doanytrace;
    ae_int_t nextleafid;
    ae_bool hasprimalsolution;
    ae_vector xprim;
    double fprim;
    double hprim;
    double ffdual;
    ae_bool timedout;
    ae_bool unbounded;
    bbgdsubproblem rootsubproblem;
    ae_obj_array bbsubproblems;
    ae_int_t bbsubproblemsheapsize;
    ae_int_t bbsubproblemsrecentlyadded;
    bbgdfront front;
    ae_shared_pool sppool;
    ae_shared_pool subsolverspool;
    ae_vector pseudocostsup;
    ae_vector pseudocostsdown;
    ae_vector pseudocostscntup;
    ae_vector pseudocostscntdown;
    double globalpseudocostup;
    double globalpseudocostdown;
    ae_int_t globalpseudocostcntup;
    ae_int_t globalpseudocostcntdown;
    ae_int_t globalsynchronizednfev;
    ae_int_t globalsynchronizednsubproblems;
    hqrndstate unsafeglobalrng;
    ae_int_t requestsource;
    ae_int_t lastrequesttype;
    rcommstate rcommv2;
    ae_bool usehandlersandsync;
    bbgdsubproblem dummysubproblem;
    bbgdfrontsubsolver dummysubsolver;
    ipm2state dummyqpsubsolver;
    bbgdfrontentry dummyentry;
    ae_matrix densedummy2;
} bbgdstate;
#endif
#if defined(AE_COMPILE_MIRBFVNS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_bool isdense;
    ae_int_t n;
    ae_int_t nf;
    ae_vector vmodelbase;
    ae_vector vmodelscale;
    ae_vector multscale;
    ae_matrix clinear;
    ae_matrix mx0;
    ae_int_t nc;
    ae_matrix centers;
    ae_matrix crbf;
    ae_vector cridx;
    sparsematrix spcenters;
    ae_vector spcoeffs;
} mirbfmodel;
typedef struct
{
    double trustrad;
    ae_bool sufficientcloudsize;
    double basef;
    double baseh;
    double predf;
    double predh;
    double skrellen;
    double maxh;
    ae_vector successfhistory;
    ae_vector successhhistory;
    ae_int_t historymax;
} mirbfvnsnodesubsolver;
typedef struct
{
    nlpstoppingcriteria crit;
    ae_vector bndlx;
    ae_vector bndux;
    ae_vector x0x;
    ae_vector sx;
    ae_vector scalingfactors;
    minfsqpstate fsqpsolver;
    smoothnessmonitor smonitor;
    sparsematrix cx;
    ae_vector clx;
    ae_vector cux;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector tmpnl;
    ae_vector tmpnu;
    ae_vector tmpi;
} rbfmmtemporaries;
typedef struct
{
    stimer localtimer;
    hqrndstate localrng;
    ae_vector glbbndl;
    ae_vector glbbndu;
    ae_vector fullx0;
    ae_vector glbx0;
    ae_vector glbtmp0;
    ae_vector glbtmp1;
    ae_vector glbtmp2;
    ae_matrix glbxf;
    ae_matrix glbsx;
    ae_matrix ortdeltas;
    ae_vector glbmultscale;
    ae_vector glbvtrustregion;
    mirbfmodel glbmodel;
    rbfmmtemporaries buf;
    ae_vector glbsk;
    ae_matrix glbrandomprior;
    ae_vector glbprioratx0;
    ae_vector glbxtrial;
    ae_vector glbs;
    ae_vector mapfull2compact;
    sparsematrix glba;
    ae_vector glbal;
    ae_vector glbau;
    ae_vector glbmask;
    rbfmmtemporaries mmbuf;
    ae_vector lclidxfrac;
    ae_vector lcl2glb;
    ae_matrix lclxf;
    ae_matrix lclsx;
    ae_vector nodeslist;
    ae_matrix lclrandomprior;
    ae_vector lclmultscale;
    ae_vector lcls;
    ae_vector lclx0;
    mirbfmodel tmpmodel;
    ipm2state qpsubsolver;
    bbgdstate bbgdsubsolver;
    ae_vector wrkbndl;
    ae_vector wrkbndu;
    ae_vector tmpb;
    sparsematrix diaga;
    ae_vector linb;
} mirbfvnstemporaries;
typedef struct
{
    ae_int_t nnodes;
    ae_matrix nodesinfo;
    ae_int_t ptlistlength;
    ae_vector ptlistheads;
    ae_vector ptlistdata;
    ae_obj_array subsolvers;
    ae_int_t naddcols;
} mirbfvnsgrid;
typedef struct
{
    ae_int_t npoints;
    ae_int_t nvars;
    ae_int_t nnlc;
    ae_matrix pointinfo;
} mirbfvnsdataset;
typedef struct
{
    ae_int_t n;
    nlpstoppingcriteria criteria;
    ae_int_t algomode;
    ae_int_t budget;
    ae_int_t maxneighborhood;
    ae_int_t batchsize;
    ae_bool expandneighborhoodonstart;
    ae_bool retrylastcut;
    ae_int_t convexityflag;
    double ctol;
    double epsf;
    double quickepsf;
    double epsx;
    ae_int_t adaptiveinternalparallelism;
    ae_int_t timeout;
    ae_vector s;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector finitebndl;
    ae_vector finitebndu;
    ae_vector isintegral;
    ae_vector isbinary;
    sparsematrix rawa;
    ae_vector rawal;
    ae_vector rawau;
    ae_vector lcsrcidx;
    ae_int_t lccnt;
    ae_bool haslinearlyconstrainedints;
    ae_int_t nnlc;
    ae_vector nl;
    ae_vector nu;
    ae_bool nomask;
    ae_vector hasmask;
    sparsematrix varmask;
    ae_bool hasx0;
    ae_vector x0;
    ae_int_t requesttype;
    ae_vector reportx;
    double reportf;
    ae_int_t querysize;
    ae_int_t queryfuncs;
    ae_int_t queryvars;
    ae_int_t querydim;
    ae_int_t queryformulasize;
    ae_vector querydata;
    ae_vector replyfi;
    ae_vector replydj;
    sparsematrix replysj;
    ae_vector tmpx1;
    ae_vector tmpc1;
    ae_vector tmpf1;
    ae_vector tmpg1;
    ae_matrix tmpj1;
    sparsematrix tmps1;
    ae_bool userterminationneeded;
    ae_int_t repnfev;
    ae_int_t repsubsolverits;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    stimer timerglobal;
    stimer timerprepareneighbors;
    stimer timerproposetrial;
    ae_int_t explorativetrialcnt;
    ae_int_t explorativetrialtimems;
    ae_int_t localtrialsamplingcnt;
    ae_int_t localtrialsamplingtimems;
    ae_int_t localtrialrbfcnt;
    ae_int_t localtrialrbftimems;
    ae_int_t cutcnt;
    ae_int_t cuttimems;
    ae_int_t dbgpotentiallyparallelbatches;
    ae_int_t dbgsequentialbatches;
    ae_int_t dbgpotentiallyparallelcutrounds;
    ae_int_t dbgsequentialcutrounds;
    ae_bool prepareevaluationbatchparallelism;
    ae_bool expandcutgenerateneighborsparallelism;
    ae_bool doanytrace;
    ae_bool dotrace;
    ae_bool doextratrace;
    ae_bool dolaconictrace;
    ae_vector xc;
    double fc;
    double mxc;
    double hc;
    ae_int_t nodec;
    ae_vector nodecproducedbycut;
    mirbfvnsgrid grid;
    mirbfvnsdataset dataset;
    ae_int_t nfrac;
    ae_int_t nint;
    ae_vector xcneighbors;
    ae_vector xcreachedfrom;
    ae_matrix xcreachedbycut;
    ae_vector xcqueryflags;
    ae_int_t xcneighborscnt;
    ae_int_t xcpriorityneighborscnt;
    ae_int_t evalbatchsize;
    ae_matrix evalbatchpoints;
    ae_vector evalbatchnodeidx;
    ae_vector evalbatchneighboridx;
    ae_bool outofbudget;
    hqrndstate unsafeglobalrng;
    ae_vector maskint;
    ae_vector maskfrac;
    ae_vector idxint;
    ae_vector idxfrac;
    ae_vector xuneighbors;
    ae_matrix xucuts;
    ae_matrix xupoints;
    ae_vector xuflags;
    ae_vector xtrial;
    ae_vector trialfi;
    ae_vector tmpeb0;
    ae_vector tmpeb1;
    ae_vector tmpeb2;
    mirbfvnstemporaries dummytmp;
    ae_matrix densedummy2;
    ae_nxpool rpool;
    ae_shared_pool tmppool;
    rcommstate rstate;
} mirbfvnsstate;
#endif
#if defined(AE_COMPILE_MINLPSOLVERS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t algoidx;
    nlpstoppingcriteria criteria;
    double diffstep;
    ae_int_t convexityflag;
    double pdgap;
    double ctol;
    double subsolverepsx;
    double subsolverepsf;
    ae_int_t nmultistarts;
    ae_int_t timeout;
    ae_int_t bbgdgroupsize;
    ae_int_t bbsyncsubsolver;
    ae_int_t bbsyncsubsolvermemlen;
    ae_int_t mirbfvnsbudget;
    ae_int_t mirbfvnsmaxneighborhood;
    ae_int_t mirbfvnsbatchsize;
    ae_int_t mirbfvnsalgo;
    ae_int_t adaptiveinternalparallelism;
    ae_int_t bbsyncprofile;
    ae_vector s;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector isintegral;
    ae_vector islinear;
    ae_vector isbinary;
    ae_bool hasobjmask;
    ae_vector objmask;
    xlinearconstraints xlc;
    sparsematrix nlcmask;
    ae_int_t nnlc;
    ae_vector nl;
    ae_vector nu;
    ae_vector hasnlcmask;
    ae_bool hasx0;
    ae_vector x0;
    ae_int_t protocolversion;
    ae_bool issuesparserequests;
    ae_bool userterminationneeded;
    ae_vector xc;
    ae_int_t repnfev;
    ae_int_t repnsubproblems;
    ae_int_t repntreenodes;
    ae_int_t repnnodesbeforefeasibility;
    ae_int_t repterminationtype;
    double repf;
    double reppdgap;
    ae_int_t tracelevel;
    ae_smart_ptr _bbgdsubsolver;
    bbgdstate *bbgdsubsolver;
    ae_smart_ptr _mirbfvnssubsolver;
    mirbfvnsstate *mirbfvnssubsolver;
    ae_vector rdummy;
    ae_vector tmpb1;
    sparsematrix tmpsparse;
    rcommstate rcommv2;
} minlpsolverstate;
typedef struct
{
    double f;
    ae_int_t nfev;
    ae_int_t nsubproblems;
    ae_int_t ntreenodes;
    ae_int_t nnodesbeforefeasibility;
    ae_int_t terminationtype;
    double pdgap;
} minlpsolverreport;
#endif

}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS C++ INTERFACE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib
{

#if defined(AE_COMPILE_BBGD) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MIRBFVNS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINLPSOLVERS) || !defined(AE_PARTIAL_BUILD)
class _minlpsolverstate_owner;
class minlpsolverstate;
class _minlpsolverreport_owner;
class minlpsolverreport;


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinNLC subpackage to work  with  this
object
*************************************************************************/
class _minlpsolverstate_owner
{
public:
    _minlpsolverstate_owner();
    _minlpsolverstate_owner(alglib_impl::minlpsolverstate *attach_to);
    _minlpsolverstate_owner(const _minlpsolverstate_owner &rhs);
    _minlpsolverstate_owner& operator=(const _minlpsolverstate_owner &rhs);
    virtual ~_minlpsolverstate_owner();
    alglib_impl::minlpsolverstate* c_ptr();
    const alglib_impl::minlpsolverstate* c_ptr() const;
protected:
    alglib_impl::minlpsolverstate *p_struct;
    bool is_attached;
};
class minlpsolverstate : public _minlpsolverstate_owner
{
public:
    minlpsolverstate();
    minlpsolverstate(alglib_impl::minlpsolverstate *attach_to);
    minlpsolverstate(const minlpsolverstate &rhs);
    minlpsolverstate& operator=(const minlpsolverstate &rhs);
    virtual ~minlpsolverstate();


};


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
class _minlpsolverreport_owner
{
public:
    _minlpsolverreport_owner();
    _minlpsolverreport_owner(alglib_impl::minlpsolverreport *attach_to);
    _minlpsolverreport_owner(const _minlpsolverreport_owner &rhs);
    _minlpsolverreport_owner& operator=(const _minlpsolverreport_owner &rhs);
    virtual ~_minlpsolverreport_owner();
    alglib_impl::minlpsolverreport* c_ptr();
    const alglib_impl::minlpsolverreport* c_ptr() const;
protected:
    alglib_impl::minlpsolverreport *p_struct;
    bool is_attached;
};
class minlpsolverreport : public _minlpsolverreport_owner
{
public:
    minlpsolverreport();
    minlpsolverreport(alglib_impl::minlpsolverreport *attach_to);
    minlpsolverreport(const minlpsolverreport &rhs);
    minlpsolverreport& operator=(const minlpsolverreport &rhs);
    virtual ~minlpsolverreport();
    double &f;
    ae_int_t &nfev;
    ae_int_t &nsubproblems;
    ae_int_t &ntreenodes;
    ae_int_t &nnodesbeforefeasibility;
    ae_int_t &terminationtype;
    double &pdgap;


};
#endif

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
void minlpsolvercreate(const ae_int_t n, const real_1d_array &x, minlpsolverstate &state, const xparams _xparams = alglib::xdefault);
void minlpsolvercreate(const real_1d_array &x, minlpsolverstate &state, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetbc(minlpsolverstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetlc2dense(minlpsolverstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minlpsolversetlc2dense(minlpsolverstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetlc2(minlpsolverstate &state, const sparsematrix &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetlc2mixed(minlpsolverstate &state, const sparsematrix &sparsea, const ae_int_t ksparse, const real_2d_array &densea, const ae_int_t kdense, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


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
void minlpsolveraddlc2dense(minlpsolverstate &state, const real_1d_array &a, const double al, const double au, const xparams _xparams = alglib::xdefault);


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
void minlpsolveraddlc2(minlpsolverstate &state, const integer_1d_array &idxa, const real_1d_array &vala, const ae_int_t nnz, const double al, const double au, const xparams _xparams = alglib::xdefault);


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
void minlpsolveraddlc2sparsefromdense(minlpsolverstate &state, const real_1d_array &da, const double al, const double au, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetnlc2(minlpsolverstate &state, const real_1d_array &nl, const real_1d_array &nu, const ae_int_t nnlc, const xparams _xparams = alglib::xdefault);
void minlpsolversetnlc2(minlpsolverstate &state, const real_1d_array &nl, const real_1d_array &nu, const xparams _xparams = alglib::xdefault);


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
void minlpsolveraddnlc2(minlpsolverstate &state, const double nl, const double nu, const xparams _xparams = alglib::xdefault);


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
void minlpsolveraddnlc2masked(minlpsolverstate &state, const double nl, const double nu, const integer_1d_array &varidx, const ae_int_t nmsk, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetpdgap(minlpsolverstate &state, const double pdgap, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetbbsyncprofilesmalltree(minlpsolverstate &state, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetbbsyncprofilelargetree(minlpsolverstate &state, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetbbsyncsubsolveripm(minlpsolverstate &state, const ae_int_t memlen, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetbbsyncsubsolversqp(minlpsolverstate &state, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetctol(minlpsolverstate &state, const double ctol, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetsubsolverepsf(minlpsolverstate &state, const double epsf, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetsubsolverepsx(minlpsolverstate &state, const double epsx, const xparams _xparams = alglib::xdefault);


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
void minlpsolverfavorinternalparallelism(minlpsolverstate &state, const xparams _xparams = alglib::xdefault);


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
void minlpsolvercautiousinternalparallelism(minlpsolverstate &state, const xparams _xparams = alglib::xdefault);


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
void minlpsolvernoadaptiveinternalparallelism(minlpsolverstate &state, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetintkth(minlpsolverstate &state, const ae_int_t k, const xparams _xparams = alglib::xdefault);


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
void minlpsolvermarkaslinearvar(minlpsolverstate &state, const ae_int_t k, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetobjectivemaskdense(minlpsolverstate &state, const boolean_1d_array &objmask, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetscale(minlpsolverstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetalgobbsync(minlpsolverstate &state, const ae_int_t groupsize, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetalgomivns(minlpsolverstate &state, const ae_int_t budget, const ae_int_t maxneighborhood, const ae_int_t batchsize, const xparams _xparams = alglib::xdefault);


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
void minlpsolversetmultistarts(minlpsolverstate &state, const ae_int_t nmultistarts, const xparams _xparams = alglib::xdefault);


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
void minlpsolversettimeout(minlpsolverstate &state, const double timeout, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minlpsolveriteration(minlpsolverstate &state, const xparams _xparams = alglib::xdefault);


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



  -- ALGLIB --
     Copyright 01.01.2025 by Bochkanov Sergey


*************************************************************************/
void minlpsolveroptimize(minlpsolverstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minlpsolveroptimize(minlpsolverstate &state,
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minlpsolveroptimize(minlpsolverstate &state,
    void (*sjac)(const real_1d_array &x, real_1d_array &fi, sparsematrix &s, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


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
void minlpsolverrestartfrom(minlpsolverstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
MINLPSolver results:  the  solution  found,  completion  codes  and  additional
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
void minlpsolverresults(const minlpsolverstate &state, real_1d_array &x, minlpsolverreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
NLC results

Buffered implementation of MINLPSolverResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minlpsolverresultsbuf(const minlpsolverstate &state, real_1d_array &x, minlpsolverreport &rep, const xparams _xparams = alglib::xdefault);
#endif
}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (FUNCTIONS)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_BBGD) || !defined(AE_PARTIAL_BUILD)
void bbgdcreatebuf(ae_int_t n,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     /* Boolean */ const ae_vector* isintegral,
     /* Boolean */ const ae_vector* isbinary,
     /* Boolean */ const ae_vector* islinear,
     const sparsematrix* sparsea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t lccnt,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     ae_bool issuesparserequests,
     ae_int_t groupsize,
     ae_int_t nmultistarts,
     ae_int_t timeout,
     ae_int_t tracelevel,
     bbgdstate* state,
     ae_state *_state);
void bbgdforceserial(bbgdstate* state, ae_state *_state);
void bbgdsetpdgap(bbgdstate* state, double pdgap, ae_state *_state);
void bbgdsetctol(bbgdstate* state, double ctol, ae_state *_state);
void bbgdsetepsf(bbgdstate* state, double epsf, ae_state *_state);
void bbgdsetsmalltree(bbgdstate* state, ae_state *_state);
void bbgdsetlargetree(bbgdstate* state, ae_state *_state);
void bbgdsetipm(bbgdstate* state, ae_int_t memlen, ae_state *_state);
void bbgdsetsqp(bbgdstate* state, ae_state *_state);
void bbgdsetdiving(bbgdstate* state,
     ae_int_t divingmode,
     ae_state *_state);
void bbgdsetmaxprimalcandidates(bbgdstate* state,
     ae_int_t maxcand,
     ae_state *_state);
void bbgdsetsoftmaxnodes(bbgdstate* state,
     ae_int_t maxnodes,
     ae_state *_state);
void bbgdsethardmaxnodes(bbgdstate* state,
     ae_int_t maxnodes,
     ae_state *_state);
void bbgdsetepsx(bbgdstate* state, double epsx, ae_state *_state);
void bbgdsetquadraticobjective(bbgdstate* state,
     const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     double c0,
     ae_state *_state);
ae_bool bbgditeration(bbgdstate* state, ae_state *_state);
ae_bool bbgdisrequestfromfront(const bbgdstate* state, ae_state *_state);
void bbgdoffloadrcommrequest(bbgdstate* state,
     ae_int_t* requesttype,
     ae_int_t* querysize,
     ae_int_t* queryfuncs,
     ae_int_t* queryvars,
     ae_int_t* querydim,
     ae_int_t* queryformulasize,
     /* Real    */ ae_vector* querydata,
     ae_state *_state);
void bbgdloadrcommreply(bbgdstate* state,
     ae_int_t requesttype,
     ae_int_t querysize,
     ae_int_t queryfuncs,
     ae_int_t queryvars,
     ae_int_t querydim,
     ae_int_t queryformulasize,
     /* Real    */ const ae_vector* replyfi,
     /* Real    */ const ae_vector* replydj,
     const sparsematrix* replysj,
     ae_state *_state);
void _bbgdsubproblem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _bbgdsubproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _bbgdsubproblem_clear(void* _p);
void _bbgdsubproblem_destroy(void* _p);
void _bbgdfrontsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _bbgdfrontsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _bbgdfrontsubsolver_clear(void* _p);
void _bbgdfrontsubsolver_destroy(void* _p);
void _bbgdfrontentry_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _bbgdfrontentry_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _bbgdfrontentry_clear(void* _p);
void _bbgdfrontentry_destroy(void* _p);
void _bbgdfront_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _bbgdfront_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _bbgdfront_clear(void* _p);
void _bbgdfront_destroy(void* _p);
void _bbgdstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _bbgdstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _bbgdstate_clear(void* _p);
void _bbgdstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MIRBFVNS) || !defined(AE_PARTIAL_BUILD)
void mirbfvnscreatebuf(ae_int_t n,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     /* Boolean */ const ae_vector* isintegral,
     /* Boolean */ const ae_vector* isbinary,
     const sparsematrix* sparsea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t lccnt,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     ae_int_t algomode,
     ae_int_t budget,
     ae_int_t maxneighborhood,
     ae_int_t batchsize,
     ae_int_t timeout,
     ae_int_t tracelevel,
     mirbfvnsstate* state,
     ae_state *_state);
void mirbfvnssetctol(mirbfvnsstate* state, double ctol, ae_state *_state);
void mirbfvnssetepsf(mirbfvnsstate* state, double epsf, ae_state *_state);
void mirbfvnssetepsx(mirbfvnsstate* state, double epsx, ae_state *_state);
void mirbfvnssetvariablemask(mirbfvnsstate* state,
     /* Boolean */ const ae_vector* hasmask,
     const sparsematrix* mask,
     ae_state *_state);
void mirbfvnssetadaptiveinternalparallelism(mirbfvnsstate* state,
     ae_int_t smpmode,
     ae_state *_state);
ae_bool mirbfvnsiteration(mirbfvnsstate* state, ae_state *_state);
void _mirbfmodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mirbfmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mirbfmodel_clear(void* _p);
void _mirbfmodel_destroy(void* _p);
void _mirbfvnsnodesubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mirbfvnsnodesubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mirbfvnsnodesubsolver_clear(void* _p);
void _mirbfvnsnodesubsolver_destroy(void* _p);
void _rbfmmtemporaries_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfmmtemporaries_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfmmtemporaries_clear(void* _p);
void _rbfmmtemporaries_destroy(void* _p);
void _mirbfvnstemporaries_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mirbfvnstemporaries_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mirbfvnstemporaries_clear(void* _p);
void _mirbfvnstemporaries_destroy(void* _p);
void _mirbfvnsgrid_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mirbfvnsgrid_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mirbfvnsgrid_clear(void* _p);
void _mirbfvnsgrid_destroy(void* _p);
void _mirbfvnsdataset_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mirbfvnsdataset_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mirbfvnsdataset_clear(void* _p);
void _mirbfvnsdataset_destroy(void* _p);
void _mirbfvnsstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mirbfvnsstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mirbfvnsstate_clear(void* _p);
void _mirbfvnsstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINLPSOLVERS) || !defined(AE_PARTIAL_BUILD)
void minlpsolvercreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minlpsolverstate* state,
     ae_state *_state);
void minlpsolversetbc(minlpsolverstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minlpsolversetlc2dense(minlpsolverstate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minlpsolversetlc2(minlpsolverstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minlpsolversetlc2mixed(minlpsolverstate* state,
     const sparsematrix* sparsea,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t kdense,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_state *_state);
void minlpsolveraddlc2dense(minlpsolverstate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state);
void minlpsolveraddlc2(minlpsolverstate* state,
     /* Integer */ const ae_vector* idxa,
     /* Real    */ const ae_vector* vala,
     ae_int_t nnz,
     double al,
     double au,
     ae_state *_state);
void minlpsolveraddlc2sparsefromdense(minlpsolverstate* state,
     /* Real    */ const ae_vector* da,
     double al,
     double au,
     ae_state *_state);
void minlpsolversetnlc2(minlpsolverstate* state,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     ae_state *_state);
void minlpsolveraddnlc2(minlpsolverstate* state,
     double nl,
     double nu,
     ae_state *_state);
void minlpsolveraddnlc2masked(minlpsolverstate* state,
     double nl,
     double nu,
     /* Integer */ const ae_vector* varidx,
     ae_int_t nmsk,
     ae_state *_state);
void minlpsolversetpdgap(minlpsolverstate* state,
     double pdgap,
     ae_state *_state);
void minlpsolversetbbsyncprofilesmalltree(minlpsolverstate* state,
     ae_state *_state);
void minlpsolversetbbsyncprofilelargetree(minlpsolverstate* state,
     ae_state *_state);
void minlpsolversetbbsyncsubsolveripm(minlpsolverstate* state,
     ae_int_t memlen,
     ae_state *_state);
void minlpsolversetbbsyncsubsolversqp(minlpsolverstate* state,
     ae_state *_state);
void minlpsolversetctol(minlpsolverstate* state,
     double ctol,
     ae_state *_state);
void minlpsolversetsubsolverepsf(minlpsolverstate* state,
     double epsf,
     ae_state *_state);
void minlpsolversetsubsolverepsx(minlpsolverstate* state,
     double epsx,
     ae_state *_state);
void minlpsolverfavorinternalparallelism(minlpsolverstate* state,
     ae_state *_state);
void minlpsolvercautiousinternalparallelism(minlpsolverstate* state,
     ae_state *_state);
void minlpsolvernoadaptiveinternalparallelism(minlpsolverstate* state,
     ae_state *_state);
void minlpsolversetintkth(minlpsolverstate* state,
     ae_int_t k,
     ae_state *_state);
void minlpsolvermarkaslinearvar(minlpsolverstate* state,
     ae_int_t k,
     ae_state *_state);
void minlpsolversetobjectivemaskdense(minlpsolverstate* state,
     /* Boolean */ const ae_vector* _objmask,
     ae_state *_state);
void minlpsolversetscale(minlpsolverstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minlpsolversetalgobbsync(minlpsolverstate* state,
     ae_int_t groupsize,
     ae_state *_state);
void minlpsolversetalgomivns(minlpsolverstate* state,
     ae_int_t budget,
     ae_int_t maxneighborhood,
     ae_int_t batchsize,
     ae_state *_state);
void minlpsolversetmultistarts(minlpsolverstate* state,
     ae_int_t nmultistarts,
     ae_state *_state);
void minlpsolversettimeout(minlpsolverstate* state,
     double timeout,
     ae_state *_state);
ae_bool minlpsolveriteration(minlpsolverstate* state, ae_state *_state);
void minlpsolverrestartfrom(minlpsolverstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minlpsolverresults(const minlpsolverstate* state,
     /* Real    */ ae_vector* x,
     minlpsolverreport* rep,
     ae_state *_state);
void minlpsolverresultsbuf(const minlpsolverstate* state,
     /* Real    */ ae_vector* x,
     minlpsolverreport* rep,
     ae_state *_state);
void minlpsolversetprotocolv2(minlpsolverstate* state, ae_state *_state);
void minlpsolversetprotocolv2s(minlpsolverstate* state, ae_state *_state);
void _minlpsolverstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlpsolverstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlpsolverstate_clear(void* _p);
void _minlpsolverstate_destroy(void* _p);
void _minlpsolverreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlpsolverreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlpsolverreport_clear(void* _p);
void _minlpsolverreport_destroy(void* _p);
#endif

}
#endif

