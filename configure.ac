#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([GNU FreeDink],[1.07.20071209],[beuc@beuc.net])
AC_CONFIG_AUX_DIR(autotools)
# For future version of aclocal:
AC_CONFIG_MACRO_DIR(autotools/macros)
AM_INIT_AUTOMAKE([gnits dist-bzip2 dist-zip])

AC_CONFIG_HEADERS(config.h)
# Define $host $host_os $build
AC_CANONICAL_HOST
# Checks for programs.
AC_PROG_CC

# Force binary relocatability by default
enable_relocatable=1

# Gnulib - configuration in gnulib/m4/gnulib-cache.m4
# gnulib-tool --source-base=gnulib/lib --m4-base=gnulib/m4 --import
gl_EARLY
gl_INIT

if test x"$host" = x"$build"; then
AM_BINRELOC
fi
AC_CHECK_TOOL(WINDRES, windres)
AM_CONDITIONAL(HAVE_WINDRES, test "$WINDRES" != "")
AC_CHECK_PROG(ZIP, zip, zip)
AM_CONDITIONAL(HAVE_ZIP, test "$ZIP" != "")

AC_ARG_ENABLE([upx],
  AS_HELP_STRING([--enable-upx],[compress executables [[default=no]]]),
  [upx=1], [upx=0])
if test x"$upx" = x"1"; then
   AC_CHECK_PROG(UPX, upx, upx)
fi
AM_CONDITIONAL(HAVE_UPX, test "$UPX" != "")

dnl Check for SDL
SDL_VERSION=1.2.0
AC_ARG_VAR([SDL_CONFIG], [Path to the sdl-config utility, it needs to
  be redefined (and saved by autoconf) for cross-compilation.])
AM_PATH_SDL($SDL_VERSION,
  :,
  AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

# TODO: check with Cygwin + -mno-cygwin
# From SDL_mixer:
## We build SDL on cygwin without the UNIX emulation layer
##BASE_CFLAGS="-I/usr/include/mingw -mno-cygwin"

AC_CHECK_LIB(SDL_gfx,zoomSurface,,AC_MSG_ERROR([You need to install SDL_gfx]))
AC_CHECK_LIB(SDL_ttf,TTF_Init,,AC_MSG_ERROR(You need to install SDL_ttf))
AC_CHECK_LIB(SDL_image,IMG_ReadXPMFromArray,,AC_MSG_ERROR([You need to install SDL_image]))
AC_CHECK_LIB(SDL_mixer,Mix_OpenAudio,,AC_MSG_ERROR([You need to install SDL_mixer]))
AC_CHECK_LIB(zzip,zzip_open_ext_io,,AC_MSG_ERROR([You need to install zzip]))

# Somewhat optional. Defines matching contants in config.h
# (e.g. HAVE_SDL_FRAMERATE_H). Using a temporary CPPFLAGS to include
# Gnulib's src and build directories ($top_srcdir and $top_builddir
# not present here for some reason). Also sdl-config lacks a
# --cppflags switch so we use SDL_CFLAGS.
old_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS -I${srcdir}/gnulib/lib -I./gnulib/lib"
AC_CHECK_HEADERS(SDL.h SDL_rotozoom.h SDL_framerate.h SDL_image.h,
  [], AC_MSG_ERROR([Could not find necessary SDL libs headers]))
CPPFLAGS="$old_CPPFLAGS"


# Dynamic and static builds
AC_ARG_ENABLE([static],
  AS_HELP_STRING([--enable-static],[embed SDL and dependencies in the executable [[default=no]]]),
  [static=1], [static=0])


if test x"$static" = x"1"; then
    case "${host}" in
    # (cf. (autoconf.info)Using System Type)
    *-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows*)
        LIBS="$LIBS -lfreetype -lwinmm -ldxguid"
        LDFLAGS="$LDFLAGS -static"
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


AC_CONFIG_FILES([Makefile gnulib/lib/Makefile src/Makefile \
  src/woeres.rc share/Makefile share/freedink/Makefile \
  autopackage/Makefile autopackage/default.apspec])

AC_OUTPUT
