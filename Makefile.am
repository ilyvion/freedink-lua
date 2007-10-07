AUTOMAKE_OPTIONS = gnits
# Gnulib:
ACLOCAL_AMFLAGS = -I gnulib/m4

SUBDIRS = autopackage gnulib/lib src share

# Copy those directly and recursively:
EXTRA_DIST = bootstrap debian doc test utils

dist-hook:
	find $(distdir) -name '*~' | xargs -r rm -f
	cd $(distdir) && sed 's/$$'"/`echo \\\r`/" README > README.txt
	make -C $(distdir)/test/sdl clean
	make -C $(distdir)/autopackage -f Makefile.autopackage
