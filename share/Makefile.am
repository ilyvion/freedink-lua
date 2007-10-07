SUBDIRS = freedink

EXTRA_DIST = pixmaps/freedink.png applications/freedink.desktop	\
applications/freedinkedit.desktop README README.fonts


# freedesktop.org menu entry
install-data-local:
	$(mkdir_p) "$(DESTDIR)$(datadir)/pixmaps"
	$(mkdir_p) "$(DESTDIR)$(datadir)/applications"
	$(INSTALL_DATA) "$(srcdir)/pixmaps/freedink.png" "$(DESTDIR)$(datadir)/pixmaps/"
	$(INSTALL_DATA) "$(srcdir)/applications/freedink.desktop" "$(DESTDIR)$(datadir)/applications/"
	$(INSTALL_DATA) "$(srcdir)/applications/freedinkedit.desktop" "$(DESTDIR)$(datadir)/applications/"

uninstall-local:
	rm -f "$(DESTDIR)$(datadir)/pixmaps/freedink.png"
	rm -f "$(DESTDIR)$(datadir)/applications/freedink.desktop"
	rm -f "$(DESTDIR)$(datadir)/applications/freedinkedit.desktop"
