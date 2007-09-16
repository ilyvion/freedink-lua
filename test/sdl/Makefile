CFLAGS=$(shell sdl-config --cflags) -g
LDFLAGS=$(shell sdl-config --libs)

.PHONY: all

all: joytest mousetest msb

clean:
	-rm -f mousetest joytest msb *~
