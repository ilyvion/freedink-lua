#CPPFLAGS=-I"C:/dx7sdk/include" -I"/mingw/include" 
CPPFLAGS=-I"C:/dx7sdk/include" $(shell sdl-config --cflags)

#CFLAGS=-O3
#CFLAGS=-g
CXXFLAGS=$(CFLAGS)

#LDFLAGS=-L"C:/dx7sdk/lib" -L"/mingw/lib"
#LOADLIBES=-mwindows
#LDLIBS=-ldxguid -ldinput -lddraw -lwinmm -lSDL -lSDL_mixer
LDFLAGS=-L"C:/dx7sdk/lib"
LDLIBS=-ldxguid -ldinput -lddraw -lwinmm $(shell sdl-config --libs) -lSDL_mixer

# Do not add update_frame.cpp and dinkvar.cpp here.
# Those are included in dink.cpp and dinkedit.cpp
COMMON_OBJS=bgm.o ddutil.o dinkvar.o fastfile.o string_util.o sfx.o \
	freedink.res
APPS=freedink freedinkedit
BINARIES=$(APPS:=.exe)

# TODO, add a resource file for both make and vc++.
# Dev-C++ can show how to use them (Dink_private.res).

all: $(APPS)

freedink: $(COMMON_OBJS) freedink.o update_frame.o
freedinkedit: $(COMMON_OBJS) freedinkedit.o

# .h deps
freedink.o:  		freedink.h dinkvar.h update_frame.h bgm.h sfx.h
freedinkedit.o:		dinkvar.h bgm.h sfx.h
dinkvar.o:		dinkvar.h bgm.h sfx.h
update_frame.o:		update_frame.h dinkvar.h freedink.h
dsutil.o:		dsutil.h
bgm.o:			bgm.h
sfx.o:			sfx.h

# TODO: Dev-C++'s Dink_private.h add meta info about a binary. Reproduce that.

# Add the icon resource
%.res: %.rc
	windres.exe -i $< --input-format=rc -o $@ -O coff 

clean:
	-rm -f $(COMMON_OBJS) freedink.o freedinkedit.o $(BINARIES)
	-rm -f *~

# Those targets are not files
.PHONY: all clean
