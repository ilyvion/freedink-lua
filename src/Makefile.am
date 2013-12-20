# Done in configure.ac so that Gnulib better detect environment
#AM_CFLAGS = -Wall -std=c99
# Cf. (autoconf.info)Installation Directory Variables:
# Don't use DATADIR, it conflicts with something in MinGW
AM_CPPFLAGS = -DDEFAULT_DATA_DIR=\"$(datadir)\"

# Gnulib:
# includes generated files from top_builddir (not only top_srcdir)
AM_CPPFLAGS += -I$(top_srcdir)/gnulib/lib -I$(top_builddir)/gnulib/lib
LDADD = $(top_builddir)/gnulib/lib/libgnu.a

# relocatable-prog
AM_CPPFLAGS += -DINSTALLDIR=\"$(bindir)\"
if RELOCATABLE_VIA_LD
AM_LDFLAGS = `$(RELOCATABLE_LDFLAGS) $(bindir)`
endif

# gettext:
AM_CPPFLAGS += -DLOCALEDIR=\"$(localedir)\"
LDADD += @LIBINTL@
AM_CPPFLAGS += -I$(top_srcdir)/intl -I$(top_builddir)/intl


LIBS += -lm

COMMON_OBJS=bgm.c bgm.h dinkini.c dinkini.h dinkvar.c		\
	dinkvar.h fastfile.c fastfile.h game_engine.c game_engine.h	\
	str_util.c str_util.h io_util.c io_util.h sfx.c sfx.h gfx.c	\
	gfx.h gfx_fade.c gfx_fade.h gfx_tiles.c gfx_tiles.h		\
	gfx_palette.c gfx_palette.h gfx_fonts.c gfx_fonts.h init.c	\
	init.h rect.c rect.h input.c input.h binreloc.c binreloc.h	\
	freedink_xpm.c freedink_xpm.h paths.c paths.h log.c log.h	\
	gfx_sprites.c gfx_sprites.h vgasys_fon.c vgasys_fon.h msgbox.c	\
	msgbox.h i18n.c i18n.h meminfo.c meminfo.h screen.c screen.h	\
	scripting.c scripting.h sp_custom.c	sp_custom.h

if HAVE_ZZIPLIB
COMMON_OBJS+=SDL_rwops_zzip.c SDL_rwops_zzip.h
endif
if HAVE_LIBZIP
COMMON_OBJS+=SDL_rwops_libzip.c SDL_rwops_libzip.h
endif

if HAVE_DINKC
COMMON_OBJS+=dinkc.c dinkc.h dinkc_bindings.c dinkc_bindings.h	\
	dinkc_console.c	dinkc_console.h 
endif
if HAVE_LUA
COMMON_OBJS+=dinklua.c dinklua.h dinklua_bindings.c dinklua_bindings.h
endif

bin_PROGRAMS = freedink freedinkedit
freedink_SOURCES = $(COMMON_OBJS) freedink.c freedink.h update_frame.c	\
update_frame.h
freedinkedit_SOURCES = $(COMMON_OBJS) freedinkedit.c
nodist_freedink_SOURCES = $(WOERES)
nodist_freedinkedit_SOURCES = $(WOERES)

EXTRA_DIST = media/freedink.ico media/system.sfd media/vgasys.fon	\
	media/README

resdir=$(top_srcdir)/share/freedink
install-exec-hook:
if HAVE_UPX
# UPX compression
# Not using --best because it takes much more time and the result is
# only a little bit smaller
# Needs to be done before bundling resources
	-$(UPX) "$(DESTDIR)$(bindir)/freedink$(EXEEXT)"
	-$(UPX) "$(DESTDIR)$(bindir)/freedinkedit$(EXEEXT)"
endif
if HAVE_ZIP
# Bundle resources
	zip -j res.zip $(resdir)/LiberationSans-Regular.ttf
	cat res.zip >> "$(DESTDIR)$(bindir)/freedink$(EXEEXT)"
	rm -f res.zip
	zip -A "$(DESTDIR)$(bindir)/freedink$(EXEEXT)"

	zip -j res.zip $(resdir)/LiberationSans-Regular.ttf	\
		$(resdir)/stop.wav $(resdir)/jump.wav
	cat res.zip >> "$(DESTDIR)$(bindir)/freedinkedit$(EXEEXT)"
	rm -f res.zip
	zip -A "$(DESTDIR)$(bindir)/freedinkedit$(EXEEXT)"
endif
# Add 'dink' and 'dinkedit' aliases, to please 3rd-party tools
	$(LN_S) -f "freedink$(EXEEXT)" "$(DESTDIR)$(bindir)/dink$(EXEEXT)"
	$(LN_S) -f "freedinkedit$(EXEEXT)" "$(DESTDIR)$(bindir)/dinkedit$(EXEEXT)"

uninstall-hook:
	rm -f "$(DESTDIR)$(bindir)/dink$(EXEEXT)"
	rm -f "$(DESTDIR)$(bindir)/dinkedit$(EXEEXT)"


# help2man-generated manpages
# Inspiration from (automake-1.10.info.gz)distcleancheck
dist_man_MANS = freedink.6 freedinkedit.6 dink.6 dinkedit.6
freedink.6: init.c $(top_srcdir)/configure.ac Makefile.am
	$(MAKE) $(AM_MAKEFLAGS) freedink$(EXEEXT)
	-$(HELP2MAN) -s6 -n "adventure and role-playing game (engine)"	\
	  --no-info --output=freedink.6 ./freedink$(EXEEXT)
freedinkedit.6: init.c $(top_srcdir)/configure.ac Makefile.am
	$(MAKE) $(AM_MAKEFLAGS) freedinkedit$(EXEEXT)
	-$(HELP2MAN) -s6 -n "adventure and role-playing game (editor)"	\
	  --no-info --output=freedinkedit.6 ./freedinkedit$(EXEEXT)
distclean-local:
	rm -f freedink.6 freedinkedit.6


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

# Other projects that use Autotools and support MinGW
# http://sourceforge.net/projects/chocolate-doom (Doom port)
# http://savannah.nongnu.org/projects/akfavatar (text display)


if PSP
TARGET=freedink
PSP_EBOOT_TITLE = "GNU FreeDink"

all-local:
#	Based on pspdev/psp/sdk/lib/build.mak
#	Fixup is mandatory. Without it the homebrew will freeze, and
#	after 10-20s the PSP will shutdown abruptly.
	psp-fixup-imports $(TARGET) -o $(TARGET).elf
	mksfoex -d MEMSIZE=1 $(PSP_EBOOT_TITLE) PARAM.SFO
	psp-prxgen $(TARGET).elf $(TARGET).prx  # also strips
	pack-pbp EBOOT.PBP PARAM.SFO $(top_srcdir)/psp/freedink.png  \
		NULL $(top_srcdir)/psp/icon1.png $(top_srcdir)/psp/splash.png  \
		NULL  $(TARGET).prx NULL

DEVICE=sdX1
install-psp:
	pmount /dev/$(DEVICE)
	mkdir -p /media/$(DEVICE)/PSP/GAME/$(TARGET)
	cp -a EBOOT.PBP $(top_srcdir)/share/freedink/LiberationSans-Regular.ttf \
		/media/$(DEVICE)/PSP/GAME/$(TARGET)
	pumount /dev/$(DEVICE)
endif
