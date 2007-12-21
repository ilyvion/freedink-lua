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
	SDL_rwops_zzip.c SDL_rwops_zzip.h paths.c paths.h log.c log.h	\
	$(WOERES)


bin_PROGRAMS = freedink freedinkedit
freedink_SOURCES = $(COMMON_OBJS) freedink.c freedink.h update_frame.c	\
	update_frame.h
freedinkedit_SOURCES = $(COMMON_OBJS) freedinkedit.c


# TODO: move to post-install hook (after install-strip)


resdir=$(top_srcdir)/share/freedink
install-exec-hook:
if HAVE_UPX
# UPX compression
# Needs to be done before bundling resources
	-$(UPX) --best "$(DESTDIR)$(bindir)/freedink$(EXEEXT)"
	-$(UPX) --best "$(DESTDIR)$(bindir)/freedinkedit$(EXEEXT)"
endif
if HAVE_ZIP
# Bundle resources
	zip -j res.zip $(resdir)/LiberationSans-Regular.ttf	\
		$(resdir)/vgasys.fon
	cat res.zip >> "$(DESTDIR)$(bindir)/freedink$(EXEEXT)"
	rm -f res.zip
	zip -A "$(DESTDIR)$(bindir)/freedink$(EXEEXT)"

	zip -j res.zip $(resdir)/LiberationSans-Regular.ttf	\
		$(resdir)/vgasys.fon $(resdir)/stop.wav		\
		$(resdir)/jump.wav
	cat res.zip >> "$(DESTDIR)$(bindir)/freedinkedit$(EXEEXT)"
	rm -f res.zip
	zip -A "$(DESTDIR)$(bindir)/freedinkedit$(EXEEXT)"
endif

clean-local:
	-rm -f *stamp

# Other projects that use Autotools and support MinGW
# http://sourceforge.net/projects/chocolate-doom (Doom port)
# http://savannah.nongnu.org/projects/akfavatar (text display)
