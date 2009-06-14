To build the Debian package
===========================

 tar xzf freedink-*.tar.gz
 ln -s freedink-x.y.z.tar.gz freedink_x.y.z.orig.tar.gz
 git clone git://git.sv.gnu.org/freedink freedinkgit
 cp -a freedinkgit/debian freedink-*/
 cd freedink-*/
 debuild -us -uc


TiMidity++
==========

'timidity' is explicitely specified as a dependency, because its
absence can cause troubles (see TROUBLESHOOTING in the source tarball
and http://bugs.debian.org/369670).  An alternative would be to 'touch
/etc/timidity/timidity.cfg' on install but this looks ugly.

We could specify it as a Recommend, but this isn't supported by
Synaptic, so let's keep it as a dependency until then.


Gnulib
======

While it may look like FreeDink is embedding a private copy of
FreeDink, this is absolutely not the case.

This point was raised on the mailing list with a favorable response
from the ftp-masters:
http://lists.gnu.org/archive/html/bug-gnulib/2008-11/msg00269.html
http://lists.gnu.org/archive/html/bug-gnulib/2008-12/msg00018.html

Learn more about Gnulib at
http://www.gnu.org/software/gnulib/manual/html_node/Introduction.html

Bottom-line: don't attempt to relink FreeDink with Debian's gnulib
package.


Testing with piuparts
=====================

# Attemps with 'lenny' instead of 'sid'
sudo pbuilder --create --distribution lenny

sudo piuparts -d lenny -b /var/cache/pbuilder/base.tgz \
  -t /mnt/h/piuparts/ \
  -i /tmp/.ICE-unix -i /tmp/.X11-unix \
  freedink_1.08.20080804-1_i386.deb


dpkg-shlibdeps warnings
=======================

Note: we're not using zziplib anymore, since it was used to access
resources embedded in the executable, and Debian strips executable
after the build process. Anyway here's a description of the problem
for reference:

You may get warnings like this:

 dpkg-shlibdeps: warning: dependency on libz.so.1 could be avoided if "debian/freedink/usr/games/freedink debian/freedink/usr/games/freedinkedit" were not uselessly linked against it (they use none of its symbols).

It's because '-lz', is added by 'zzip-config', is mistakently added at
link time.

Those *could* be fixed with the '--as-needed' GNU ld flag (or
'-Wl,--as-needed' from gcc).

However this is pretty much experimental and known to fail in some
situations or architectures. There is a good summary here:
http://www.gentoo.org/proj/en/qa/asneeded.xml

It would be better to fix 'zzip-config' (or the pkgconfig .pc files in
0.13), if their developers agree.  Cf. http://bugs.debian.org/471065
