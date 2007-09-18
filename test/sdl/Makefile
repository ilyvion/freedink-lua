CFLAGS=-Wall -g $(shell sdl-config --cflags)
LDLIBS=$(shell sdl-config --libs)

.PHONY: all

PROGS = joytest mousetest msb ticks
all: $(PROGS)

clean:
	-rm -f $(PROGS) $(PROGS:%=%.exe) *~
