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


dpkg-shlibdeps warnings
=======================

You may get warnings like this:

 dpkg-shlibdeps: warning: dependency on libz.so.1 could be avoided if "debian/freedink/usr/games/freedink debian/freedink/usr/games/freedinkedit" were not uselessly linked against it (they use none of its symbols).

It's because '-lz', is added by 'zzip-config', is mistakently added at
link time.

Those *could* be fixed with the '--as-needed' GNU ld flag (or
'-Wl,--as-needed' from gcc).

However this is pretty much experimental and known to fail in some
situations or architectures. There is a good summary here:
http://www.gentoo.org/proj/en/qa/asneeded.xml

It would be better to fix 'zzip-config'
(http://bugs.debian.org/471065), if their developers agree.


Testing with piuparts
=====================

# Attemps with 'lenny' instead of 'sid'
sudo pbuilder --create --distribution lenny

sudo piuparts -d lenny -b /var/cache/pbuilder/base.tgz \
  -t /mnt/h/piuparts/ \
  -i /tmp/.ICE-unix -i /tmp/.X11-unix \
  freedink_1.08.20080804-1_i386.deb
