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
VERSION=$1
if [ -z "$VERSION" ]; then
    VERSION=$(cd /mnt/snapshots/$PACKAGE && ls -d */ | sed 's,/$,,' | sort -n | tail -1)
fi

PUBDIR=/mnt/snapshots/$PACKAGE/$VERSION

rm -rf t/
mkdir t
pushd t
TARBALL=$PACKAGE-$VERSION.tar.gz
cp -a $PUBDIR/$TARBALL .
tar xzf $TARBALL
ln -s $TARBALL ${PACKAGE}_$VERSION.orig.tar.gz 
cd $PACKAGE-$VERSION/
cp -a ../../$PACKAGE/debian .
yes | DEBEMAIL="beuc@debian.org" DEBFULLNAME="Sylvain Beucler" dch -D stable \
  --newversion $VERSION-1 \
  --force-bad-version -- \
  "New upstream release"
pdebuild --debbuildopts '-sa' --buildresult /mnt/snapshots/debian \
  -- --basetgz /var/cache/pbuilder/base-lenny-bpo.tar.gz --bindmounts /usr/src/backports/lenny/debs
popd
make -C /mnt/snapshots/debian
rm -rf t

exit;

# construction:

# with cowbuilder / etch:
aptitude install cowbuilder fakeroot sudo
aptitude install debhelper # for dh_clean
mkdir /mnt/snapshots/debian/etch-backports/
(cd /mnt/snapshots/debian/etch-backports/ && apt-ftparchive packages . | gzip > Packages.gz)
cowbuilder --create --basepath /var/cache/pbuilder/base-etch.cow --distribution=etch \
  --othermirror "deb http://backports.org/debian etch-backports main|deb file:///mnt/snapshots/debian/etch-backports/ ./" \
  --bindmounts /mnt/snapshots/debian/etch-backports
# update:
cowbuilder --update --basepath /var/cache/pbuilder/base-etch.cow/ --bindmounts /mnt/snapshots/debian/etch-backports --debian-etch-workaround

# with pbuilder / lenny:
pbuilder --create --basetgz /var/cache/pbuilder/base-lenny-bpo.tar.gz --distribution lenny \
  --othermirror "deb file:///usr/src/backports/lenny/debs ./" --bindmounts /usr/src/backports/lenny/debs
# update:
pbuilder --update --basetgz /var/cache/pbuilder/base-lenny-bpo.tar.gz --bindmounts /usr/src/backports/lenny/debs
