# Instrumenting Todd-Coxeter

This page contains some details about the information printed during a
congruence enumeration performed by a \ref_todd_coxeter instance. This
information is output when \ref libsemigroups::reporting_enabled returns \c
true. You can enable reporting using a \ref libsemigroups::ReportGuard. If
reporting is enabled, any call to
\ref libsemigroups::ToddCoxeter::run "ToddCoxeter::run",
\ref libsemigroups::ToddCoxeter::run_until "ToddCoxeter::run_until",
or \ref libsemigroups::ToddCoxeter::run_for "ToddCoxeter::run_for"
will print output that describes what is happening during the enumeration.
This generated output is intended to be helpful when running a long (at least
multiple seconds) congruence enumeration. If reporting is enabled for short
running enumerations, then a fairly large amount of information will be
generated in a very short space of time, which may not be desirable.

A single call to one of:
\ref libsemigroups::ToddCoxeter::run "ToddCoxeter::run",
\ref libsemigroups::ToddCoxeter::run_until "ToddCoxeter::run_until",
or \ref libsemigroups::ToddCoxeter::run_for "ToddCoxeter::run_for"
is referred to as a **run**, each run is comprised of **phases** (namely,
HLT, Felsch, or lookaheads), and with each phase there will be a number of
**reports**. Reports are generated at the start and end of every phase, and
every second during a phase.

## Annotated images of the generated output

\image html todd-coxeter-report-1.png

\image html todd-coxeter-report-2.png

\image html todd-coxeter-report-3.png
