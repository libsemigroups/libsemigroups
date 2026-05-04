# handle catch2 checks

AC_DEFUN([AX_CHECK_CATCH], [
  AC_ARG_WITH(
    [external-catch],
    [AS_HELP_STRING([--with-external-catch], [use external catch (default: no)])],
    [],
    [with_external_catch=no]
  )
  AC_MSG_CHECKING([whether to use external catch])
  AC_MSG_RESULT([$with_external_catch])

  AS_IF([test "x$with_external_catch" = xyes], [
    m4_ifdef([PKG_CHECK_MODULES], [
      PKG_CHECK_MODULES(CATCH2, [catch2 >= 3.0], [
        PKG_CHECK_MODULES(CATCH2MAIN, [catch2-with-main >= 3.0], [
          AC_MSG_NOTICE([external catch will be used])
	  AC_DEFINE(
	    [CATCH_ALL_HEADER],
	    [<catch2/catch_all.hpp>],
	    [path to catch header]
	  )
        ], [
          AC_MSG_ERROR([external catch not found])
        ])
      ], [
        AC_MSG_ERROR([external catch not found])
      ])
    ], [
    AC_MSG_ERROR(m4_normalize([
        cannot use flag --with-external-catch, the libsemigroups configure file
        was created on a system without m4 macros for pkg-config available...
      ]))
    ])
  ], [
    AC_DEFINE(
      [CATCH_ALL_HEADER],
      ["Catch2-3.14.0/catch_amalgamated.hpp"],
      [path to catch header]
    )
  ])

  AM_CONDITIONAL([USE_BUNDLED_CATCH], [test "x$with_external_catch" != xyes])
])
