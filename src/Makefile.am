AM_CFLAGS = -Wall -std=c99
# Cf. (autoconf.info)Installation Directory Variables:
# Don't use DATADIR, it conflicts with something in MinGW
AM_CPPFLAGS = -DDEFAULT_DATA_DIR=\"$(datadir)\"

# Gnulib:
# includes generated files from top_builddir (not only top_srcdir)
AM_CPPFLAGS += -I$(top_srcdir)/gnulib/lib -I$(top_builddir)/gnulib/lib
LDADD = $(top_builddir)/gnulib/lib/libgnu.a
# _Not_ LIBADD...

# relocatable-prog
AM_CPPFLAGS += -DINSTALLDIR=\"$(bindir)\"
if RELOCATABLE_VIA_LD
AM_LDFLAGS = `$(RELOCATABLE_LDFLAGS) $(bindir)`
endif


# Woe icon and app infos
# Also check http://fragglet.livejournal.com/4448.html
.rc.o:
	$(WINDRES) -i $< -o $@

# How do we handle resources embedded in the executable binary?
if HAVE_WINDRES
WOERES=woeres.rc
else
WOERES=
endif

COMMON_OBJS=bgm.c bgm.h dinkvar.c dinkvar.h fastfile.c fastfile.h	\
	str_util.c str_util.h io_util.c io_util.h sfx.c sfx.h gfx.c	\
	gfx.h gfx_tiles.c gfx_tiles.h gfx_utils.c gfx_utils.h		\
	gfx_fonts.c gfx_fonts.h init.c init.h rect.c rect.h input.c	\
	input.h binreloc.c binreloc.h freedink_xpm.c freedink_xpm.h	\
	SDL_rwops_zzip.c SDL_rwops_zzip.h $(WOERES)


bin_PROGRAMS = freedink freedinkedit
freedink_SOURCES = $(COMMON_OBJS) freedink.c freedink.h update_frame.c	\
	update_frame.h
freedinkedit_SOURCES = $(COMMON_OBJS) freedinkedit.c freedinkeditres


# TODO: move to post-install hook (after install-strip)

# UPX compression
# Needs to be done before bundling resources
if HAVE_UPX
all-local: freedink_upx_stamp freedinkedit_upx_stamp
freedink_upx_stamp: freedink
	-$(UPX) --best freedink
	touch freedink_upx_stamp
freedinkedit_upx_stamp: freedinkedit
	-$(UPX) --best freedinkedit
	touch freedinkedit_upx_stamp
else
freedink_upx_stamp:
freedinkedit_upx_stamp:
endif

# Bundle resources
if HAVE_ZIP
resdir=$(top_srcdir)/share/freedink
all-local: freedink_res_stamp freedinkedit_res_stamp
freedink_res_stamp: freedink freedink_upx_stamp
	zip -j res.zip $(resdir)/LiberationSans-Regular.ttf
	cat res.zip >> freedink$(EXEEXT)
	rm -f res.zip
	zip -A freedink$(EXEEXT)
	touch freedink_res_stamp

freedinkedit_res_stamp: freedinkedit freedinkedit_upx_stamp
	zip -j res.zip $(resdir)/LiberationSans-Regular.ttf	\
		$(resdir)/stop.wav $(resdir)/jump.wav
	cat res.zip >> freedinkedit$(EXEEXT)
	rm -f res.zip
	zip -A freedinkedit$(EXEEXT)
	touch freedinkedit_res_stamp
endif

clean-local:
	-rm -f *stamp

# Other projects that use Autotools while supporting MinGW
# http://sourceforge.net/projects/chocolate-doom (Doom port)
# http://savannah.nongnu.org/projects/akfavatar (text display)
