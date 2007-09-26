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
# TODO: Claudio says those next 3 lines should be handled by
# gnulib/m4/gnulib-cache.m4. Test and remove if there's no problem.
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
AC_ARG_VAR([SDL_CONFIG], [Path to the sdl-config utility, it needs to
  be redefined (and saved by autoconf) for cross-compilation.])
AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
# Technically this should set CFLAGS, but sdl-config lacks a
# --cppflags switch, and setting CPPFLAGS is needed for
# AC_CHECK_HEADERS, so we define CPPFLAGS.
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CHECK_LIB(SDL_gfx,zoomSurface,,AC_MSG_ERROR([You need to install SDL_gfx]))
AC_CHECK_LIB(SDL_ttf,TTF_Init,,AC_MSG_ERROR(You need to install SDL_ttf))
AC_CHECK_LIB(SDL_mixer,Mix_OpenAudio,,AC_MSG_ERROR([You need to install SDL_mixer]))

# Somewhat optional. Defines matching contants in config.h
# (e.g. HAVE_SDL_FRAMERATE_H).
AC_CHECK_HEADERS(SDL.h SDL_rotozoom.h SDL_framerate.h SDL_ttf.h SDL_mixer.h,
  [], AC_MSG_ERROR([Could not find necessary SDL libs headers]))


AC_CONFIG_FILES([Makefile gnulib/lib/Makefile \
  src/Makefile src/woeres.rc share/freedink/Makefile \
  autopackage/Makefile autopackage/default.apspec])

AC_OUTPUT
