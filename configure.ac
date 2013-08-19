
AC_PREREQ(2.61)
AC_INIT(yyast, 0.2, take.vos@vosgames.nl)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([buildutils])
AM_INIT_AUTOMAKE()

dnl    Here are a set of rules to help you update your library version information:
dnl
dnl    * Start with version information of `0:0:0' for each libtool library.
dnl    * Update the version information only immediately before a public release of your software.
dnl      More frequent updates are unnecessary, and only guarantee that the current interface
dnl      number gets larger faster.
dnl    * If the library source code has changed at all since the last update, then increment revision
dnl      (`c:r:a' becomes `c:r+1:a').
dnl    * If any interfaces have been added, removed, or changed since the last update, increment current,
dnl      and set revision to 0.
dnl    * If any interfaces have been added since the last public release, then increment age.
dnl    * If any interfaces have been removed since the last public release, then set age to 0.
SHARED_VERSION_INFO=0:1:0
AC_SUBST(SHARED_VERSION_INFO)

DX_INIT_DOXYGEN(yyast, doxygen.cfg)
DX_DOXYGEN_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_DOT_FEATURE(OFF)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)

AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_C_BIGENDIAN

AC_CONFIG_HEADERS([yyast/config.h])
AC_CONFIG_FILES([Makefile yyast/Makefile yyast/yyast.pc])
AC_OUTPUT

