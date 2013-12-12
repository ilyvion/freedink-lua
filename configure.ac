#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Copyright (C) 2007, 2008, 2009  Sylvain Beucler

# This file is part of GNU FreeDink

# GNU FreeDink is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# GNU FreeDink is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see
# <http://www.gnu.org/licenses/>.
 
AC_PREREQ(2.61)
AC_INIT([GNU FreeDink],[1.08.20121232],[bug-freedink@gnu.org])
AC_CONFIG_AUX_DIR(autotools)
# For future version of aclocal:
AC_CONFIG_MACRO_DIR(autotools/macros)
AM_INIT_AUTOMAKE([gnits dist-bzip2 dist-zip])

AC_CONFIG_HEADERS(config.h)
# Define $host $host_os + distributes config.guess and config.sub
AC_CANONICAL_HOST
# Checks for programs.
AC_PROG_CC
AC_PROG_LN_S

# Force binary relocatability by default
enable_relocatable=1

# We use std=c99 which enables some macros, supports '//' comments,
# and does not warn about mixed declarations and code.  Let's do tests
# (namely Gnulib tests) with it too.
CFLAGS="$CFLAGS -Wall -std=c99"


# Gnulib - configuration in gnulib/m4/gnulib-cache.m4
gl_EARLY

AM_CONDITIONAL(PSP, test "$host" == "mipsallegrexel-psp-elf")

# Check if we can fork&exec (for fallback xmessage errors)
AC_CHECK_FUNCS(execlp)
# Check if we've got malloc stats
AC_CHECK_TYPES([struct mallinfo], [], [], [[#include <malloc.h>]])

# gettext
dnl Using [external] because bundling intl/ is deprecated as of 0.18:
AM_GNU_GETTEXT([external])
dnl Not a dependency: only a declaration used by autopoint:
AM_GNU_GETTEXT_VERSION([0.18.1])
dnl Not using ngettext in FreeDink or gnulib as of 2010-11:
dnl AM_GNU_GETTEXT_NEED(need-ngettext)


AC_ARG_VAR([WINDRES], [Resource compiler for woe executables])
AC_CHECK_TOOL(WINDRES, windres)
AM_CONDITIONAL(HAVE_WINDRES, test "$WINDRES" != "")

AC_ARG_ENABLE([embedded-resources],
  AS_HELP_STRING([--disable-embedded-resources],
    [don't embed base fonts in the executable -
     disable this for distros that strip executables and break resources (default is enabled)]),
  [], [enable_embedded_resources=yes])
if test x"$enable_embedded_resources" = x"yes"; then
AC_CHECK_PROG(ZIP, zip, zip)
fi
AM_CONDITIONAL(HAVE_ZIP, test "$ZIP" != "")

AC_ARG_VAR([HELP2MAN], [Manpage generator from '--help' output])
AC_PATH_PROG(HELP2MAN, help2man)
if test -z "$HELP2MAN"; then
  AC_MSG_WARN([You may need to install help2man (if you're a developer)])
  # Set HELP2MAN so 'make' errors will be understandable
  HELP2MAN="help2man"
  LACK_HELP2MAN=1
fi

AC_ARG_VAR([UPX], [Produce self-decompressing executables (if --enable-upx)])
AC_ARG_ENABLE([upx],
  AS_HELP_STRING([--enable-upx], [compress executables (default is disabled)]),
  [], [enable_upx=no])
if test x"$enable_upx" = x"yes"; then
  AC_PATH_PROG(UPX, upx)
fi
AM_CONDITIONAL(HAVE_UPX, test "$UPX" != "")

# SDL and core libs
PKG_CHECK_MODULES([SDL], [sdl >= 1.2  SDL_image  SDL_mixer  SDL_ttf >= 2.0.9  SDL_gfx],
  [CFLAGS="$CFLAGS $SDL_CFLAGS"; LIBS="$SDL_LIBS $LIBS"],
  AC_MSG_WARN($SDL_PKG_ERRORS); LACK_SDL=1)

AC_ARG_ENABLE([lua],
  AS_HELP_STRING([--enable-lua], [support for lua scripts (default is disabled)]),
  [], [enable_lua=no])
if test x"$enable_lua" = x"yes"; then
  PKG_CHECK_MODULES([LUA], [lua5.2],
    AC_DEFINE([HAVE_LUA], [], [lua library (-llua)]),
    AC_MSG_WARN($LUA_PKG_ERRORS); LACK_LUA=1)
  CFLAGS="$CFLAGS $LUA_CFLAGS"
  LIBS="$LUA_LIBS $LIBS"
fi
AM_CONDITIONAL(HAVE_LUA, test x"$enable_lua" = x"yes")

case "${host}" in
# (cf. (autoconf.info)Using System Type) (2002)
*-*-msdos* | *-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows* \
| mipsallegrexel-psp-elf)
    # Can't search fonts using fontconfig under ms woe
    # Nor under a PSP
    use_fontconfig="no"
    ;;
*)
    use_fontconfig="yes"
    ;;
esac
if test x"$use_fontconfig" = x"yes"; then
  PKG_CHECK_MODULES([FONTCONFIG], [fontconfig >= 2.3],
    AC_DEFINE([HAVE_FONTCONFIG], [], [fontconfig library (-lfontconfig)]),
    AC_MSG_WARN($FONTCONFIG_PKG_ERRORS); LACK_FONTCONFIG=1)
  CFLAGS="$CFLAGS $FONTCONFIG_CFLAGS"
  LIBS="$FONTCONFIG_LIBS $LIBS"
fi

if test x"$enable_embedded_resources" = x"yes"; then
  # libzip - cross-compiles well
  PKG_CHECK_MODULES([LIBZIP], [libzip >= 0.8],
    AC_DEFINE([HAVE_LIBZIP], [], [libzip library (-lzip)]),
    AC_MSG_WARN($LIBZIP_PKG_ERRORS [(non-fatal if there's zziplib)]); LACK_LIBZIP=1)
  CFLAGS="$CFLAGS $LIBZIP_CFLAGS"
  LIBS="$LIBZIP_LIBS $LIBS"
  # ZZipLib - doesn't cross-compile well and unstable under woe, but
  #   functional and initially more widespread under GNU/Linux
  #AC_CHECK_LIB(zzip,zzip_open_ext_io,,AC_MSG_WARN([You need to install zzip]);LACK_LIBZZIP=1)
  PKG_CHECK_MODULES([ZZIPLIB], [zziplib >= 0.12.83],
    AC_DEFINE([HAVE_ZZIPLIB], [], [zziplib library (-lzzip)]),
    AC_MSG_WARN($ZZIPLIB_PKG_ERRORS [(non-fatal if there's libzip)]); LACK_ZZIPLIB=1)
  CFLAGS="$CFLAGS $ZZIPLIB_CFLAGS"
  LIBS="$ZZIPLIB_LIBS $LIBS"
else
   LACK_LIBZIP=1
   LACK_ZZIPLIB=1
fi
AM_CONDITIONAL(HAVE_LIBZIP, test "$LACK_LIBZIP" != "1")
AM_CONDITIONAL(HAVE_ZZIPLIB, test "$LACK_ZZIPLIB" != "1")


# Dynamic and static builds
AC_ARG_ENABLE([static],
  AS_HELP_STRING([--enable-static],[embed SDL and dependencies in the executable (default is disabled)]),
  [], [enable_static=no])
if test x"$enable_static" = x"yes"; then
    case "${host}" in
    # (cf. (autoconf.info)Using System Type) (2002)
    *-*-mingw32* | *-*-cygwin* | *-*-windows*)
        # LIBS is handled by pkg-config and Libs.private/Requires.private
        # We still need to force choosing 'libxxx.a' over 'libxxx.dll.a':
        LDFLAGS="$LDFLAGS -static"
        ;;
    mipsallegrexel-psp-elf)
	# (Only static builds are possible for this architecture AFAIK)
        # Light version, if dependencies are compiled minimaly
        #LIBS="$LIBS -lfreetype -lz"
        # Full version, if planning to support more formats
	LIBS="-lvorbisidec -lfreetype -lpng -ljpeg -lz $LIBS"
        #LIBS="$LIBS -lfreetype -lpng -ltiff -ljpeg -lz -lvorbisfile -lvorbis -logg"
    esac
fi


# Lengthy Gnulib checks
gl_INIT


# BinReloc, if not cross-compiling (tests do not check for cross-compilation)
if test "$cross_compiling" = "no"; then
AP_BINRELOC
fi



# Somewhat optional. Defines matching contants in config.h
# (e.g. HAVE_SDL_FRAMERATE_H). Using a temporary CPPFLAGS to include
# Gnulib's src and build directories ($top_srcdir and $top_builddir
# not present here for some reason). Also sdl-config lacks a
# --cppflags switch so we use SDL_CFLAGS.
# http://lists.gnu.org/archive/html/bug-gnulib/2007-10/msg00157.html
# http://lists.gnu.org/archive/html/bug-gnulib/2007-12/msg00149.html
# https://bugs.freedesktop.org/show_bug.cgi?id=47996
old_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS -I${srcdir}/gnulib/lib -I./gnulib/lib"
AC_CHECK_HEADERS(SDL.h SDL_rotozoom.h SDL_framerate.h SDL_image.h,
  [], AC_MSG_WARN([Could not find necessary SDL libs headers]); LACK_HEADERS=1)
CPPFLAGS="$old_CPPFLAGS"


echo
echo "Errors and warnings recap:"
error=0
warning=0

if test "$LACK_LUA" = 1; then
  echo "* You need to install liblua, version 5.2."
  error=1
fi

if test "$LACK_SDL" = 1; then
  echo "* You need to install the SDL libraries:"
  echo "$SDL_PKG_ERRORS"
  error=1
fi
if test "$USE_NLS" = no; then
  echo "* You need to install the gettext library and enable NLS"
  error=1
fi

if test "$use_fontconfig" = "yes" -a "$LACK_FONTCONFIG" = 1; then
  echo "* You need to install the fontconfig library"
  error=1
fi

if test "$error" = 0 -a "$LACK_HEADERS" = 1; then
  echo "* Warning: You lack some library headers (.h) - see warnings above."
  #error=1
  warning=1
fi

if test x"$enable_embedded_resources" = x"yes"; then
  if test "$LACK_LIBZIP" = 1 -a "$LACK_ZZIPLIB" = 1; then
    echo "* You need to install libzip or ZZipLib (or --disable-embedded-resources)."
    error=1
  fi
  if test -z "$ZIP"; then
    echo "* The 'zip' utility cannot be found. Please install it (or --disable-embedded-resources)."
    error=1
  fi
fi

if test "$error" = 0 -a "$LACK_HELP2MAN" = 1; then
  echo -n "* Warning: The 'help2man' utility cannot be found. You won't be able to rebuild manpages"
  echo " (if you modify the source code)"
  warning=1
fi

if test "$enable_upx" = "yes" -a -z "$UPX"; then
  echo "* UPX is explicitely activated (--enable-upx) but cannot be found"
  error=1
fi

if test "$error" = 1; then
  echo
  echo "There are errors - stopping. Please check the reports above."
  echo "If dependencies (such as SDL_gfx) are missing,"
  echo "please check BUILD for distro package names."
  exit 1
elif test "$warning" = 1; then
  echo
  echo "There were warnings - let's continue for now,"
  echo "but please check make sure nothing is missing."
else
  echo "* No errors"
fi
echo


AC_CONFIG_FILES([Makefile autopackage/Makefile autopackage/default.apspec gnulib/lib/Makefile gnulib/po/Makefile.in nsis/Makefile po/Makefile.in share/Makefile share/freedink/Makefile src/Makefile src/woeres.rc])

AC_OUTPUT


echo
echo "Options summary:"
echo "* --enable-upx=$enable_upx"
echo "* --enable-static=$enable_static"
echo "* --enable-embedded-resources=$enable_embedded_resources"
echo "* --enable-lua=$enable_lua"
echo
echo "Now you can type 'make' to compile $PACKAGE_STRING"
