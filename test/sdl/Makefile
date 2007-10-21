CFLAGS=-Wall -g $(shell sdl-config --cflags)
LDLIBS=$(shell sdl-config --libs)

.PHONY: all

PROGS = joytest mousetest msb ticks embedded_font
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
	zip -j font.zip ../../share/freedink/LiberationSans-Regular.ttf
	cat font.zip >> embedded_font$(EXEEXT)
	rm -f font.zip
	zip -A embedded_font$(EXEEXT)

clean:
	-rm -f $(PROGS) $(PROGS:%=%.exe) *.o *~
