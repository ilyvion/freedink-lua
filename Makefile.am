AUTOMAKE_OPTIONS = gnits
# Gnulib:
ACLOCAL_AMFLAGS = -I gnulib/m4

SUBDIRS = autopackage gnulib/lib gnulib/po nsis po src share

# Copy those directly and recursively:
# Build system
EXTRA_DIST = acsite.m4 bootstrap gnulib/m4/gnulib-cache.m4
# Documentation
EXTRA_DIST += doc
EXTRA_DIST += BUILD DEB TRANSLATIONS.txt TROUBLESHOOTING
# Packaging
# (don't include debian/ as this displeases debian devs)
EXTRA_DIST += freedink.spec gentoo psp
# Other programs of interest
EXTRA_DIST += test contrib
# Developer tools
EXTRA_DIST += valgrind

dist-hook:
	find $(distdir) -name '*~' | xargs -r rm -f
#	Remove .gitignore files; e.g. they prevent 'git-buildpackage'
#	from importing ignored files from release tarballs
	find $(distdir) -name '.gitignore' | xargs -r rm -f
#	At the end of each line (doubled $), add a \r, so we get \r\n
	cd $(distdir) && sed 's/$$/\r/' README > README.txt
	make -C $(distdir)/test/sdl clean
	make -C $(distdir)/autopackage -f Makefile.autopackage
