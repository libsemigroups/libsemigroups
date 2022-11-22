dnl handle HPCombi checks

AC_DEFUN([CHECK_INTRINSIC],
    [AC_MSG_CHECKING([for $1])
    saved_CXXFLAGS="$CXXFLAGS"
    CXXFLAGS="-mavx -flax-vector-conversions"
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
        [[#include <x86intrin.h>]],
        [$1[($2)];
        ]
        )],
      [],
      [enable_hpcombi=no]
      )
    AC_MSG_RESULT([$enable_hpcombi])
    CXXFLAGS="$saved_CXXFLAGS"
    ])

AC_DEFUN([AX_CHECK_HPCOMBI], [
  m4_define([ax_hpcombi_cxxflags_variable],[HPCOMBI_CXXFLAGS])
  AC_ARG_ENABLE([hpcombi],
      [AS_HELP_STRING([--enable-hpcombi], [enable HPCombi])],
      [],
      [enable_hpcombi=yes]
      )
  AC_MSG_CHECKING([whether to enable HPCombi])
  AC_MSG_RESULT([$enable_hpcombi])

  AS_IF([test "x$enable_hpcombi" = xyes],
        [AC_CHECK_FILE([$srcdir/extern/HPCombi/VERSION], 
                       [], 
                       [AC_MSG_WARN([HPCombi is not available])
                       [enable_hpcombi=no]])])

  AS_IF([test "x$enable_hpcombi" = xyes],
        [AC_MSG_CHECKING([the required version of HPCombi])
         MIN_HPCOMBI_VERSION="0.0.6"
         AC_MSG_RESULT([$MIN_HPCOMBI_VERSION])])

  AS_IF([test "x$enable_hpcombi" = xyes],
        [AC_MSG_CHECKING([the version of HPCombi that's present])
         FOUND_HPCOMBI="$(cat $srcdir/extern/HPCombi/VERSION)"
         AC_MSG_RESULT([$FOUND_HPCOMBI])])

  AS_IF([test "x$enable_hpcombi" = xyes],
        [AX_COMPARE_VERSION($FOUND_HPCOMBI, 
                            [ge], 
                            $MIN_HPCOMBI_VERSION,
                            [],
                            [AC_MSG_WARN([the incorrect version of HPCombi is present, HPCombi is disabled]) 
                            enable_hpcombi=no])])
  
  dnl # Check if the flags required for HPCombi are supported 
  AS_IF([test "x$enable_hpcombi" = xyes], 
        [AX_CHECK_COMPILE_FLAG(-mavx, 
                               AX_APPEND_FLAG(-mavx, [ax_hpcombi_cxxflags_variable]),
                               [AC_MSG_WARN([flag -mavx not supported, HPCombi is disabled])
                              enable_hpcombi=no])])

  AS_IF([test "x$enable_hpcombi" = xyes], 
        [AX_CHECK_COMPILE_FLAG(-flax-vector-conversions, 
                               AX_APPEND_FLAG(-flax-vector-conversions, [ax_hpcombi_cxxflags_variable]),
                               [AC_MSG_WARN([flag -flax-vector-conversions not supported, HPCombi is disabled])
                               enable_hpcombi=no])])

  dnl # Check if the x86intrin.h header is available
  AS_IF([test "x$enable_hpcombi" = xyes], 
        [AC_CHECK_HEADERS([x86intrin.h], [], [AC_MSG_WARN([header x86intrin.h is required for HPCombi, HPCombi is disabled])
                               enable_hpcombi=no])])
  
  AS_IF([test "x$enable_hpcombi" = xyes], 
        [enable_hpcombi_before_builtin_check=yes])

  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_blendv_epi8],[__m128i{},__m128i{},__m128i{}])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_cmpeq_epi8],[__m128i{},__m128i{}])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_cmpestri],[__m128i{},1,__m128i{},1,1])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_cmpestrm],[__m128i{},1,__m128i{},1,1])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_cmplt_epi8],[__m128i{},__m128i{}])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_extract_epi64],[__m128i{},1])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_max_epi8],[__m128i{},__m128i{}])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_max_epu8],[__m128i{},__m128i{}])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_min_epi8],[__m128i{},__m128i{}])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_min_epu8],[__m128i{},__m128i{}])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_movemask_epi8],[__m128i{}])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_popcnt_u32],[{}])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_set_epi64x],[1,1])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_shuffle_epi8],[__m128i{},__m128i{}])])
  AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_INTRINSIC([_mm_slli_epi32],[__m128i{},1])])

  AS_IF([test "x$enable_hpcombi_before_builtin_check" = xyes],
        [AS_IF([test "x$enable_hpcombi" = xno],
         [AC_MSG_WARN([compiler builtin not supported, HPCombi is disabled])])])

  AS_IF([test "x$enable_hpcombi" = xyes],
        [AC_DEFINE([HPCOMBI_ENABLED], [1], [define if building with HPCombi])])

  dnl # the following is used in Makefile.am
  AM_CONDITIONAL([LIBSEMIGROUPS_HPCOMBI_ENABLED], [test "x$enable_hpcombi" = xyes])

  dnl # check for HPCombi's preprocessor macro
  AS_IF([test "x$enable_hpcombi" = xyes], 
  [AC_MSG_CHECKING([for HPCOMBI_CONSTEXPR_FUN_ARGS])
   AC_COMPILE_IFELSE( 
                    [AC_LANG_PROGRAM( 
                        [[using T = int; constexpr int exec(T f()) { return f(); }
                        constexpr int foo() { return 1; }
                        static_assert(exec(foo) == 1, "Failed exec");]]
                      )],
                     [hpcombi_constexpr_fun_args=yes],
                     [hpcombi_constexpr_fun_args=no]
                   )
   AC_MSG_RESULT([$hpcombi_constexpr_fun_args])])

  AM_CONDITIONAL([HPCOMBI_CONSTEXPR_FUN_ARGS], 
                 [test "x$hpcombi_constexpr_fun_args" = xyes])
    
  AS_IF([test "x$enable_hpcombi" = xyes],
        AC_SUBST(ax_hpcombi_cxxflags_variable))
])
