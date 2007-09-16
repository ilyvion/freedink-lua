#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([GNU FreeDink],[1.07],[beuc@beuc.net])
AC_CONFIG_AUX_DIR(autotools)
# For future version of aclocal:
AC_CONFIG_MACRO_DIR(autotools/macros)
AM_INIT_AUTOMAKE([gnits dist-bzip2 dist-zip])

AC_CONFIG_HEADERS(config.h)
# Checks for programs.
AC_PROG_CC

if [ x"$host" == x"$target" ]; then
AM_BINRELOC
fi

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

AC_CONFIG_FILES([Makefile src/Makefile share/freedink/Makefile])
AC_OUTPUT
