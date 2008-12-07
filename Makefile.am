AUTOMAKE_OPTIONS = gnits
# Gnulib:
ACLOCAL_AMFLAGS = -I gnulib/m4

SUBDIRS = autopackage intl gnulib/lib gnulib/po po src share

# Copy those directly and recursively:
# (don't include debian/ as this displeases debian devs)
EXTRA_DIST = bootstrap doc test contrib gnulib/m4/gnulib-cache.m4	\
	BUILD DEB TROUBLESHOOTING freedink.spec

dist-hook:
	find $(distdir) -name '*~' | xargs -r rm -f
#	Remove .gitignore files; e.g. they prevent 'git-buildpackage'
#	from importing ignored files from release tarballs
	find $(distdir) -name '.gitignore' | xargs -r rm -f
#	At the end of each line (doubled $), add a \r, so we get \r\n
	cd $(distdir) && sed 's/$$/\r/' README > README.txt
	make -C $(distdir)/test/sdl clean
	make -C $(distdir)/autopackage -f Makefile.autopackage
