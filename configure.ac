#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([GNU FreeDink],[1.07-20070916],[beuc@beuc.net])
AC_CONFIG_AUX_DIR(autotools)
# For future version of aclocal:
AC_CONFIG_MACRO_DIR(autotools/macros)
AM_INIT_AUTOMAKE([gnits dist-bzip2 dist-zip])

AC_CONFIG_HEADERS(config.h)
# Checks for programs.
AC_PROG_CC

# Gnulib
gl_EARLY
gl_SOURCE_BASE(gnulib/lib)
gl_M4_BASE(gnulib/m4)
gl_MODULES(getopt)
gl_INIT

if test x"$host" == x"$target"; then
AM_BINRELOC
fi
AC_CHECK_TOOL(WINDRES, windres)
AM_CONDITIONAL(HAVE_WINDRES, test "$WINDRES" != "")

dnl Check for SDL
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CHECK_LIB(SDL_gfx,zoomSurface,,AC_MSG_ERROR([You need to install SDL_gfx]))
AC_CHECK_LIB(SDL_ttf,TTF_Init,,AC_MSG_ERROR(You need to install SDL_ttf))
AC_CHECK_LIB(SDL_mixer,Mix_OpenAudio,,AC_MSG_ERROR([You need to install SDL_mixer]))

AC_CONFIG_FILES([Makefile gnulib/lib/Makefile \
  src/Makefile share/freedink/Makefile \
  autopackage/Makefile autopackage/default.apspec])

AC_OUTPUT
