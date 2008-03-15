To build the Debian package
===========================

 tar xzf freedink-*.tar.gz
 cd freedink-*
 debuild -us -uc


dpkg-shlibdeps warnings
=======================

You may get warnings like this:

 dpkg-shlibdeps: warning: debian/freedink/usr/bin/freedink shouldn't be linked with libpthread.so.0 (it uses none of its symbols).
 dpkg-shlibdeps: warning: debian/freedink/usr/bin/freedink shouldn't be linked with libz.so.1 (it uses none of its symbols).
 dpkg-shlibdeps: warning: debian/freedink/usr/bin/freedinkedit shouldn't be linked with libpthread.so.0 (it uses none of its symbols).
 dpkg-shlibdeps: warning: debian/freedink/usr/bin/freedinkedit shouldn't be linked with libz.so.1 (it uses none of its symbols).

It's because -lpthread and -lz are mistakently added at link time.

 - '-lpthread' is added by 'sdl-config'

 - '-lz' is added by 'zzip-config'

Those *could* be fixed with the '--as-needed' GNU ld flag (or
'-Wl,--as-needed' from gcc).

However this is pretty much experimental and known to fail in some
situations or architectures. There is a good summary here:
http://www.gentoo.org/proj/en/qa/asneeded.xml

It would be better to fix 'sdl-config' and 'zzip-config' (#471065), if
their developers agree.
