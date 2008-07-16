CFLAGS=-Wall -g $(shell sdl-config --cflags)
LDLIBS=$(shell sdl-config --libs)

.PHONY: all

PROGS = joytest mousetest msb ticks embedded_font embedded_font2	\
	keycodes videoinfo fade
all: $(PROGS)

embedded_font: embedded_font.o SDL_rwops_zzip.o
# statically compile otherwise it's too small to test upx
	$(CC) embedded_font.o SDL_rwops_zzip.o $(LDLIBS) -lSDL_ttf -Wl,-Bstatic -lzzip -Wl,-Bdynamic -o $@
#	UPX needs to be called first, otherwise it'll make the zip unreadable
	upx --ultra-brute --best embedded_font$(EXEEXT)

# - Append a zip resource archive to the executable -
# http://zziplib.sourceforge.net/sfx-make.html trick:
#	zip -0 -j tmp.zip embedded_font$(EXEEXT)
#	zip -9 -j tmp.zip ../../share/freedink/LiberationSans-Regular.ttf
##	zzipsetstub example.zip example.exe
#	dd conv=notrunc if=embedded_font$(EXEEXT) of=tmp.zip
#	mv tmp.zip embedded_font$(EXEEXT)
#	chmod +x embedded_font$(EXEEXT)

# Simply using zip -A:
# - can't use -A and add files at the same time,
# - can't >> from 'zip',
# => temporary file
	zip -j res.zip ../../share/freedink/LiberationSans-Regular.ttf
	cat res.zip >> embedded_font$(EXEEXT)
	rm -f res.zip
	zip -A embedded_font$(EXEEXT)

embedded_font2: embedded_font2.o SDL_rwops_libzip.o
	$(CC) embedded_font2.o SDL_rwops_libzip.o $(LDLIBS) -lSDL_ttf -Wl,-Bstatic -lzip -Wl,-Bdynamic -o $@
	zip -j res.zip ../../share/freedink/LiberationSans-Regular.ttf
	cat res.zip >> embedded_font2$(EXEEXT)
	rm -f res.zip
	zip -A embedded_font2$(EXEEXT)

# woe: i586-mingw32msvc-gcc -Dmain=SDL_main -static -I /usr/local/cross-tools/i386-mingw32msvc/include -I/usr/local/cross-tools/i386-mingw32msvc/include/SDL -L/usr/local/cross-tools/i386-mingw32msvc/lib SDL_rwops_libzip.c embedded_font2.c -lmingw32 -lSDL_ttf -lfreetype  -lSDLmain -lSDL -mwindows -lzip -lz -lwinmm -ldxguid

clean:
	-rm -f $(PROGS) $(PROGS:%=%.exe) *.o *~
