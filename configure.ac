#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#AC_PREREQ([2.69])
AC_INIT(SemiCrf, 0.0.1, jiro.iwai@theport.jp)
AC_CONFIG_SRCDIR([Main.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

#AC_PROG_RANLIB
#AC_LANG(C++)
# Checks for programs.
#AC_PROG_CC

cxxflags_save="$CXXFLAGS"
AC_PROG_CXX
CXXFLAGS="$cxxflags_save"
#test -n "$CXXFLAGS" || CXXFLAGS="-O2 -std=c++11 -pthread -I../src/spdlog/include -DNDEBUG"
test -n "$CXXFLAGS" || CXXFLAGS="-g -std=c++11 -pthread -I../src/spdlog/include"

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
