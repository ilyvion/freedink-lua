#CPPFLAGS=-I"C:/dx7sdk/include" -I"/mingw/include" 
CPPFLAGS=$(shell sdl-config --cflags)

#CFLAGS=-O3
CFLAGS=-g
CXXFLAGS=$(CFLAGS)

#LDFLAGS=-L"C:/dx7sdk/lib" -L"/mingw/lib"
#LOADLIBES=-mwindows
#LDLIBS=-ldxguid -ldinput -lddraw -lwinmm -lSDL -lSDL_mixer
#LDFLAGS=-L"C:/dx7sdk/lib"

LDLIBS=$(shell sdl-config --libs) -lSDL_mixer -lSDL_gfx -lSDL_ttf

#COMMON_OBJS=bgm.o dinkvar.o fastfile.o string_util.o sfx.o	\
#	gfx.o gfx_tiles.o gfx_utils.o gfx_fonts.o init.o freedink.res
COMMON_OBJS=bgm.o dinkvar.o fastfile.o str_util.o io_util.o sfx.o	\
	gfx.o gfx_tiles.o gfx_utils.o gfx_fonts.o init.o rect.o		\
	input.o
APPS=freedink freedinkedit
WOE_BINARIES=$(APPS:=.exe)

# TODO, add a resource file for both make and vc++.
# Dev-C++ can show how to use them (Dink_private.res).

all: $(APPS)

freedink: $(COMMON_OBJS) freedink.o update_frame.o
freedinkedit: $(COMMON_OBJS) freedinkedit.o

# .h deps
freedink.o:  		freedink.h dinkvar.h update_frame.h bgm.h sfx.h gfx.h gfx_tiles.h gfx_utils.h gfx_fonts.h resource.h
freedinkedit.o:		dinkvar.h bgm.h sfx.h gfx.h gfx_tiles.h gfx_fonts.h resource.h
dinkvar.o:		dinkvar.h bgm.h sfx.h fastfile.h freedink.h gfx.h gfx_tiles.h gfx_fonts.h bgm.h
update_frame.o:		update_frame.h dinkvar.h freedink.h gfx_tiles.h gfx.h gfx_fonts.h bgm.h
bgm.o:			bgm.h dinkvar.h
sfx.o:			sfx.h dinkvar.h
gfx.o:			gfx.h
gfx_tiles.o:		gfx_tiles.h gfx.h dinkvar.h
gfx_utils.o:		gfx_utils.h
init.o:			init.h gfx.h gfx_fonts.h dinkvar.h

# Add the icon resource
%.res: %.rc
	windres.exe -i $< --input-format=rc -o $@ -O coff 

clean:
	-rm -f $(COMMON_OBJS) freedink.o freedinkedit.o update_frame.o $(APPS) $(WOE_BINARIES)
	-rm -f *~

# Those targets are not files
.PHONY: all clean
