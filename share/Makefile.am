SUBDIRS = freedink

EXTRA_DIST = pixmaps/freedink.png applications/freedink.desktop	\
	applications/freedinkedit.desktop README README.fonts


# freedesktop.org menu entry
install-data-local:
	$(mkdir_p) "$(DESTDIR)$(datarootdir)/pixmaps"
	$(mkdir_p) "$(DESTDIR)$(datarootdir)/applications"
	$(INSTALL_DATA) "$(srcdir)/pixmaps/freedink.png" "$(DESTDIR)$(datarootdir)/pixmaps/"
	$(INSTALL_DATA) "$(srcdir)/applications/freedink.desktop" "$(DESTDIR)$(datarootdir)/applications/"
	$(INSTALL_DATA) "$(srcdir)/applications/freedinkedit.desktop" "$(DESTDIR)$(datarootdir)/applications/"

uninstall-local:
	rm -f "$(DESTDIR)$(datarootdir)/pixmaps/freedink.png"
	rm -f "$(DESTDIR)$(datarootdir)/applications/freedink.desktop"
	rm -f "$(DESTDIR)$(datarootdir)/applications/freedinkedit.desktop"
