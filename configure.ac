#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Copyright (C) 2007, 2008  Sylvain Beucler

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
AC_INIT([GNU FreeDink],[1.08.20090109],[bug-freedink@gnu.org])
AC_CONFIG_AUX_DIR(autotools)
# For future version of aclocal:
AC_CONFIG_MACRO_DIR(autotools/macros)
AM_INIT_AUTOMAKE([gnits dist-bzip2 dist-zip])

AC_CONFIG_HEADERS(config.h)
# Define $host $host_os $build
AC_CANONICAL_HOST
# Checks for programs.
AC_PROG_CC
AC_PROG_LN_S

# Force binary relocatability by default
enable_relocatable=1

# We use std=c99 which enables some macros - do tests with it too.
CFLAGS="$CFLAGS -Wall -std=c99"


# Gnulib - configuration in gnulib/m4/gnulib-cache.m4
gl_EARLY


# gettext
AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION(0.17)
AM_GNU_GETTEXT_NEED(need-ngettext)


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

dnl TODO: check for pkg-config's presence manually?


# Explicitely call PKG_PROG_PKG_CONFIG because PKG_CHECK_MODULES is
# called conditionaly
PKG_PROG_PKG_CONFIG()

case "${host}" in
# (cf. (autoconf.info)Using System Type)
# Modified to accept "biplet" instead of "triplet", because at
# least Debian's MinGW package uses i586-mingw32msvc (instead of
# i586-pc-mingw32msvc)
*-*-go32* | *-mingw32* | *-*-cygwin* | *-*-windows*)
    # Can't search fonts using fontconfig under ms woe
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
  LIBS="$LIBS $FONTCONFIG_LIBS"
fi

if test x"$enable_embedded_resources" = x"yes"; then
  # libzip - cross-compiles well but not widespread (distro packages) yet
  PKG_CHECK_MODULES([LIBZIP], [libzip >= 0.8],
    AC_DEFINE([HAVE_LIBZIP], [], [libzip library (-lzip)]),
    AC_MSG_WARN($LIBZIP_PKG_ERRORS [(non-fatal if there's zziplib)]); LACK_LIBZIP=1)
  CFLAGS="$CFLAGS $LIBZIP_CFLAGS"
  LIBS="$LIBS $LIBZIP_LIBS"
  # ZZipLib - doesn't cross-compile well and unstable under woe, but
  #   functional and more widespread under GNU/Linux
  #AC_CHECK_LIB(zzip,zzip_open_ext_io,,AC_MSG_WARN([You need to install zzip]);LACK_LIBZZIP=1)
  PKG_CHECK_MODULES([ZZIPLIB], [zziplib >= 0.12.83],
    AC_DEFINE([HAVE_ZZIPLIB], [], [zziplib library (-lzzip)]),
    AC_MSG_WARN($ZZIPLIB_PKG_ERRORS [(non-fatal if there's libzip)]); LACK_ZZIPLIB=1)
  CFLAGS="$CFLAGS $ZZIPLIB_CFLAGS"
  LIBS="$LIBS $ZZIPLIB_LIBS"
else
   LACK_LIBZIP=1
   LACK_ZZIPLIB=1
fi
AM_CONDITIONAL(HAVE_LIBZIP, test "$LACK_LIBZIP" != "1")
AM_CONDITIONAL(HAVE_ZZIPLIB, test "$LACK_ZZIPLIB" != "1")

PKG_CHECK_MODULES([LIBFFI], [libffi >= 3],
  AC_DEFINE([HAVE_LIBFFI], [], [libffi library (-lffi)]),
  AC_MSG_WARN($LIBFFI_PKG_ERRORS); LACK_LIBFFI=1)
CFLAGS="$CFLAGS $LIBFFI_CFLAGS"
LIBS="$LIBS $LIBFFI_LIBS"
# Support libffi4 which is bundled with GCC and doesn't have a libffi.pc:
if test $LACK_LIBFFI = 1; then
   LACK_LIBFFI=0
   AC_CHECK_LIB(ffi,ffi_prep_cif,,AC_MSG_WARN([You need to install libffi]);LACK_LIBFFI=1)
fi


dnl Check for SDL
SDL_VERSION=1.2.0
AC_ARG_VAR([SDL_CONFIG], [Path to the sdl-config utility, it needs to
  be redefined (and saved by autoconf) for cross-compilation.])
AM_PATH_SDL($SDL_VERSION,
  :,
  AC_MSG_WARN([*** SDL version $SDL_VERSION not found!]); LACK_LIBSDL=1
)
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"


# Lengthy Gnulib checks
gl_INIT


# BinReloc, if not cross-compiling (tests do not check for cross-compilation)
if test "$cross_compiling" = "no"; then
AP_BINRELOC
fi


# Dynamic and static builds
AC_ARG_ENABLE([static],
  AS_HELP_STRING([--enable-static],[embed SDL and dependencies in the executable (default is disabled)]),
  [], [enable_static=no])

if test x"$enable_static" = x"yes"; then
    case "${host}" in
    # (cf. (autoconf.info)Using System Type)
    # Modified to accept "biplet" instead of "triplet", because at
    # least Debian's MinGW package uses i586-mingw32msvc (instead of
    # i586-pc-mingw32msvc)
    *-*-go32* | *-mingw32* | *-*-cygwin* | *-*-windows*)
        # Light version, if dependencies are compiled minimaly
        LIBS="$LIBS -lfreetype -lz -lwinmm -ldxguid"
        # Full version, if planning to support more formats
        LIBS="$LIBS -lfreetype -lpng -ltiff -ljpeg -lz -lvorbisfile -lvorbis -logg -lwinmm -ldxguid"
        LDFLAGS="$LDFLAGS -static"
        ;;
    esac
fi
# TODO: check with Cygwin + -mno-cygwin
# From SDL_mixer:
## We build SDL on cygwin without the UNIX emulation layer
##BASE_CFLAGS="-I/usr/include/mingw -mno-cygwin"


AC_CHECK_LIB(SDL_gfx,zoomSurface,,AC_MSG_WARN([You need to install SDL_gfx]); LACK_LIBSDL_GFX=1)
AC_CHECK_LIB(SDL_ttf,TTF_Init,,AC_MSG_WARN([You need to install SDL_ttf]); LACK_LIBSDL_TTF=1)
AC_CHECK_LIB(SDL_image,IMG_ReadXPMFromArray,,AC_MSG_WARN([You need to install SDL_image]); LACK_LIBSDL_IMAGE=1)
AC_CHECK_LIB(SDL_mixer,Mix_OpenAudio,,AC_MSG_WARN([You need to install SDL_mixer]); LACK_LIBSDL_MIXER=1)

# Check that SDL_ttf is 2.0.9 minimum (TTF_BOLD_STYLE fix)
AC_MSG_CHECKING([SDL_ttf version >= 2.0.9])
AC_RUN_IFELSE(
  [AC_LANG_PROGRAM(
    [
#include "SDL.h"
#include "SDL_ttf.h"
    ],
    [[
SDL_version *link_version;
link_version = TTF_Linked_Version();
if (link_version->major < 2)
{
   return 1;
}
else if (link_version->major == 2)
{
  /* if (link_version->minor >= 0) */
  if (link_version->patch < 9)
    return 1;
  return 0;
}
else
{
  /* major > 2 */
  return 0;
}
    ]])],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_RESULT([no]); AC_MSG_WARN([Your version of SDL_ttf is too old (need >= 2.0.9)]); LACK_LIBSDL_TTF=1],
    [AC_MSG_RESULT([yes (cross-compiling; assumed OK...)])]
)


# Somewhat optional. Defines matching contants in config.h
# (e.g. HAVE_SDL_FRAMERATE_H). Using a temporary CPPFLAGS to include
# Gnulib's src and build directories ($top_srcdir and $top_builddir
# not present here for some reason). Also sdl-config lacks a
# --cppflags switch so we use SDL_CFLAGS.
# http://lists.gnu.org/archive/html/bug-gnulib/2007-10/msg00157.html
# http://lists.gnu.org/archive/html/bug-gnulib/2007-12/msg00149.html
old_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS -I${srcdir}/gnulib/lib -I./gnulib/lib"
AC_CHECK_HEADERS(SDL.h SDL_rotozoom.h SDL_framerate.h SDL_image.h,
  [], AC_MSG_WARN([Could not find necessary SDL libs headers]); LACK_HEADERS=1)
CPPFLAGS="$old_CPPFLAGS"


if test x"$static" = x"1"; then
    case "${host}" in
    *-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows*)
        # See above
        ;;
    *)
        # sdl-config --static-libs is far from complete :/ Plus I
        # don't know how to make autoconf understand which libs must
        # be static and which ones must be dynamic, so let's overwrite
        # LIBS for now..

        # There can be different configurations, for example with or
        # without including X11. I suggest using a generic
        # configuration here, that could be used for the
        # autopackage. Users will have to modify this directly if they
        # need something different. This is meant to work with Debian
        # Etch SDL packages.
        LIBS="-Wl,-Bstatic \
          -lSDL_gfx \
          -lSDL_image -ljpeg -lpng -ltiff -lz \
          -lSDL_ttf -lfreetype -lz \
          -lSDL_mixer -lvorbisfile -lvorbis -logg -lsmpeg \
          -lSDL \
          -lvga -ldirectfb -ldirect -lfusion \
          -lcaca -lcucul -laa -lslang -lcurses \
          -laudio -lesd -lartsc -lasound -lgpm \
          -lzzip \
          -lm \
          -Wl,-Bdynamic -lX11 -ldl -lpthread -lstdc++"
        ;;
    esac
fi



echo
echo "Errors and warnings recap:"
error=0
warning=0

if test "$LACK_LIBSDL" = 1; then
  echo "* You need to install the libSDL";
  error=1
fi
if test "$LACK_LIBSDL_GFX" = 1; then
  echo "* You need to install the SDL_gfx library";
  error=1
fi
if test "$LACK_LIBSDL_TTF" = 1; then
  echo "* You need to install the SDL_ttf library, version >= 2.0.9";
  error=1
fi
if test "$LACK_LIBSDL_IMAGE" = 1; then
  echo "* You need to install the SDL_image library";
  error=1
fi
if test "$LACK_LIBSDL_MIXER" = 1; then
  echo "* You need to install the SDL_mixer library";
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

if test "$LACK_LIBFFI" = 1; then
  echo "* You need to install the libffi library."
  error=1
fi

if test "$error" = 0 -a "$LACK_HELP2MAN" = 1; then
  echo "* Warning: The 'help2man' utility cannot be found. You won't be able to rebuild manpages."
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


AC_CONFIG_FILES([Makefile autopackage/Makefile autopackage/default.apspec intl/Makefile gnulib/lib/Makefile gnulib/po/Makefile.in po/Makefile.in share/Makefile share/freedink/Makefile src/Makefile src/woeres.rc])

AC_OUTPUT


echo
echo "Options summary:"
echo "* --enable-upx=$enable_upx"
echo "* --enable-static=$enable_static"
echo "* --enable-embedded-resources=$enable_embedded_resources"
echo
echo "Now you can type 'make' to compile $PACKAGE_STRING"
