# No pedantic for now, will do that in a bit.
#AM_CFLAGS = -Wall -ansi -pedantic
# Cf. (autoconf.info)Installation Directory Variables:
# Don't use DATADIR, it conflicts with something in MinGW
AM_CPPFLAGS = -DDEFAULT_DATA_DIR=\"$(datadir)\"

# Gnulib:
# includes generated files from top_builddir (not only top_srcdir)
AM_CPPFLAGS += -I$(top_srcdir)/gnulib/lib -I$(top_builddir)/gnulib/lib
LDADD = $(top_builddir)/gnulib/lib/libgnu.a
# _Not_ LIBADD...

# Woe icon and app infos
# Also check http://fragglet.livejournal.com/4448.html
.rc.o:
	$(WINDRES) -i $< -o $@

if HAVE_WINDRES
WOERES=woeres.rc
else
WOERES=
endif

COMMON_OBJS=bgm.c bgm.h dinkvar.c dinkvar.h fastfile.c fastfile.h	\
	str_util.c str_util.h io_util.c io_util.h sfx.c sfx.h gfx.c	\
	gfx.h gfx_tiles.c gfx_tiles.h gfx_utils.c gfx_utils.h		\
	gfx_fonts.c gfx_fonts.h init.c init.h rect.c rect.h input.c	\
	input.h binreloc.c binreloc.h $(WOERES)

bin_PROGRAMS = freedink freedinkedit
freedink_SOURCES = $(COMMON_OBJS) freedink.c freedink.h update_frame.c	\
	update_frame.h
freedinkedit_SOURCES = $(COMMON_OBJS) freedinkedit.c

# Other projects that use Autotools while supporting MinGW
# http://sourceforge.net/projects/chocolate-doom (Doom port)
# http://savannah.nongnu.org/projects/akfavatar (text display)
