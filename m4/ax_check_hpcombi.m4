# Check if HPCombi is enabled, and available
AC_DEFUN([AX_CHECK_HPCOMBI], [
AC_ARG_ENABLE([hpcombi],
    [AS_HELP_STRING([--enable-hpcombi], [enable HPCombi])],
    [],
    [enable_hpcombi=yes]
    )
AC_MSG_CHECKING([whether to enable HPCombi])
AC_MSG_RESULT([$enable_hpcombi])

# The next variable is used in Makefile.am
AM_CONDITIONAL([LIBSEMIGROUPS_HPCOMBI_ENABLED], [test "x$enable_hpcombi" = xyes])
# The next variable becomes the preprocessor macro LIBSEMIGROUPS_HPCOMBI_ENABLED 
AS_IF([test "x$enable_hpcombi" = xyes],
      [AC_DEFINE([HPCOMBI_ENABLED], [1], [define if building with HPCombi])])

# Although HPCombi no longer requires x86, we still require the following flags
# if they are available.
m4_define([ax_hpcombi_cxxflags_variable], [HPCOMBI_CXXFLAGS])
AS_IF([test "x$enable_hpcombi" = xyes], 
      [AX_CHECK_COMPILE_FLAG(-mavx, 
                             AX_APPEND_FLAG(-mavx, [ax_hpcombi_cxxflags_variable]),
                             [])])

AS_IF([test "x$enable_hpcombi" = xyes], 
      [AX_CHECK_COMPILE_FLAG(-flax-vector-conversions, 
                             AX_APPEND_FLAG(-flax-vector-conversions, 
                             [ax_hpcombi_cxxflags_variable]),
                             [])])

# TODO check if compiling in 32-bit, and in that case disable hpcombi

AS_IF([test "x$enable_hpcombi" = xyes],
      AC_SUBST(ax_hpcombi_cxxflags_variable))
])
