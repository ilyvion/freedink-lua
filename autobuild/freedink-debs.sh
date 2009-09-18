#!/bin/bash -ex
# Debian release

# Copyright (C) 2008, 2009  Sylvain Beucler

# This file is part of GNU FreeDink

# GNU FreeDink is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# GNU FreeDink is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see
# <http://www.gnu.org/licenses/>.

PACKAGE=freedink
TARBALL=$(cd /mnt/snapshots && ls $PACKAGE-*.tar.gz | grep ^$PACKAGE-[0-9] | sort -n | tail -1)
VERSION=${TARBALL#$PACKAGE-}
VERSION=${VERSION%.tar.gz}
cp -a /mnt/snapshots/$TARBALL .
rm -rf t/
mkdir t
pushd t
tar xzf ../$PACKAGE-$VERSION.tar.gz
ln -s ../$PACKAGE-$VERSION.tar.gz ${PACKAGE}_$VERSION.orig.tar.gz 
cd $PACKAGE-$VERSION/
cp -a ../../$PACKAGE/debian .
yes | DEBEMAIL="beuc@beuc.net" DEBFULLNAME="Sylvain Beucler" dch -D stable \
  --newversion $VERSION-1 \
  --force-bad-version -- \
  "New upstream release"
pdebuild --pbuilder cowbuilder --buildresult /mnt/snapshots/debian -- --basepath /var/cache/pbuilder/base-etch.cow --bindmounts /mnt/snapshots/debian/etch-backports --debian-etch-workaround --debbuildopts '-sa'
popd
make -C /mnt/snapshots/debian
rm -rf t

exit;

# construction:
aptitude install cowbuilder fakeroot sudo
aptitude install debhelper # for dh_clean
mkdir /mnt/snapshots/debian/etch-backports/
(cd /mnt/snapshots/debian/etch-backports/ && apt-ftparchive packages . | gzip > Packages.gz)
cowbuilder --create --basepath /var/cache/pbuilder/base-etch.cow --distribution=etch \
  --othermirror "deb http://backports.org/debian etch-backports main|deb file:///mnt/snapshots/debian/etch-backports/ ./" \
  --bindmounts /mnt/snapshots/debian/etch-backports

# update:
cowbuilder --update --basepath /var/cache/pbuilder/base-etch.cow/ --bindmounts /mnt/snapshots/debian/etch-backports --debian-etch-workaround
