AC_INIT(tribe,0.3.3,jcrescin@cit.uws.edu.au)

AC_CONFIG_HEADERS(lib/config.h)

# set language
AC_LANG(C)

# set vars to itself to prevent the macros from setting default value
CFLAGS="$CFLAGS"

# set flags
TBE_C_WFLG="-Wall -Wshadow -Wcast-align -Wstrict-prototypes"
TBE_C_OFLG="-ansi -pedantic"
TBE_CFLAGS="$TBE_C_OFLG $TBE_C_WFLG -I\$(DISTDIR)/include"
TBE_LDFLAGS="-L\$(DISTDIR)/lib"

# some tests
AC_PROG_CC
AC_PROG_LN_S

# check compiler options
cat >> $$.c << \EOF
int main(void) {return 0;}
EOF

# check if -fPIC works
AC_MSG_CHECKING([whether $CC accepts -fPIC])
if "${CC}" -fPIC -o $$ $$.c > /dev/null 2>&1; then
  TBE_FPIC="PIC"
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])

  # no? try -fpic
  AC_MSG_CHECKING([whether $CC accepts -fpic])
  if "${CC}" -fpic -o $$ $$.c > /dev/null 2>&1; then
    TBE_FPIC="pic"
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    TBE_FPIC="no"
  fi
fi

# check if -shared works
AC_MSG_CHECKING([whether $CC compiler accepts -shared])
if "${CC}" -shared -o $$ $$.c > /dev/null 2>&1; then
  TBE_SHARED="yes"
  AC_MSG_RESULT([yes])
else
  TBE_SHARED="no"
  AC_MSG_RESULT([no])
fi

# make sure they both are supported
if test "x$TBE_FPIC" != "xno" && test "x$TBE_SHARED" != "xno"; then
  AC_SUBST(BUILDSHARED,[$TBE_FPIC])
fi

# cleanup
rm -f $$ $$.c

# --enable-static
AC_ARG_ENABLE(static,AC_HELP_STRING([--enable-static],[generate a statically-linked executable (default is disabled)]))

if test "x$enable_static" != "x" && test "x$enable_static" != "xno"; then
  AC_SUBST(STATIC,[yes])
else
  if test "x$TBE_FPIC" = "xno" || test "x$TBE_SHARED" = "xno"; then
    AC_MSG_WARN([cannot build shared libraries, will link statically])
    AC_SUBST(STATIC,[yes])
  fi
fi

# --enable-debug
AC_ARG_ENABLE(debug,AC_HELP_STRING([--enable-debug],[enable debugging output & compile with -g (default is disabled)]))

if test "x$enable_debug" != "x" && test "x$enable_debug" != "xno"; then
  AC_DEFINE(TBE_DEBUG,[yes])
  AC_SUBST(DEBUG,[yes])
  TBE_CFLAGS="-g $TBE_CFLAGS"
else
  TBE_CFLAGS="-O3 $TBE_CFLAGS"
fi

# --enable-testprog
AC_ARG_ENABLE(testprog,AC_HELP_STRING([--enable-testprog],[compile test program (default is disabled)]))

if test "x$enable_testprog" != "x" && test "x$enable_testprog" != "xno"; then
  AC_SUBST(TESTPROG,[yes])
fi

# --enable-strip
AC_ARG_ENABLE(strip,AC_HELP_STRING([--enable-strip],[run strip program (default is disabled)]))

if test "x$enable_strip" != "x" && test "x$enable_strip" != "xno"; then
  AC_CHECK_PROG(STRIP,strip,[yes])
fi

CFLAGS="$TBE_CFLAGS $CFLAGS"
LDFLAGS="$TBE_LDFLAGS $LDFLAGS"

AC_DEFINE_UNQUOTED(TBE_VERSION,"$PACKAGE_VERSION")
AC_SUBST(VERSION,"$PACKAGE_VERSION")
AC_SUBST(DISTDIR,"\$(TOPSRCDIR)/dist")
AC_SUBST(LIBDIR,"\$(TOPSRCDIR)/lib")

AC_CONFIG_FILES(  \
  Makefile        \
  lib/Makefile    \
  test/Makefile   \
)

AC_OUTPUT
