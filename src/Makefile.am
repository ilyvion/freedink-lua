# No pedantic for now, will do that in a bit.
#AM_CFLAGS = -Wall -ansi -pedantic
# Cf. (autoconf.info)Installation Directory Variables:
AM_CPPFLAGS = -DDEFAULT_DATA_DIR=\"$(datadir)\"

COMMON_OBJS=bgm.c bgm.h dinkvar.c dinkvar.h fastfile.c fastfile.h	\
	str_util.c str_util.h io_util.c io_util.h sfx.c sfx.h gfx.c	\
	gfx.h gfx_tiles.c gfx_tiles.h gfx_utils.c gfx_utils.h		\
	gfx_fonts.c gfx_fonts.h init.c init.h rect.c rect.h input.c	\
	input.h resource.h binreloc.c binreloc.h

bin_PROGRAMS = freedink freedinkedit
freedink_SOURCES = $(COMMON_OBJS) freedink.c freedink.h update_frame.c	\
	update_frame.h
freedinkedit_SOURCES = $(COMMON_OBJS) freedinkedit.c
