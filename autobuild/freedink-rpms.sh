#!/bin/bash -ex
# Fedora release

# Copyright (C) 2008  Sylvain Beucler

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
cp -a /mnt/snapshots/$TARBALL rpmbuild/SOURCES/
cp ~/$PACKAGE/$PACKAGE.spec rpmbuild/SPECS/
sed -i -e "s/^Version:.*/Version:	$VERSION/"  rpmbuild/SPECS/$PACKAGE.spec
rpmbuild -bs --nodeps rpmbuild/SPECS/$PACKAGE.spec
mock -r fedora-9-i386 --resultdir /mnt/snapshots/fedora --rebuild rpmbuild/SRPMS/$PACKAGE-$VERSION-1*.src.rpm
cp -f rpmbuild/SPECS/$PACKAGE.spec /mnt/snapshots/fedora
make -C /mnt/snapshots/fedora/
exit;


## Construction

yum install mock

# This one doesn't work in a VServer - use 'chroot' instead
mock -r fedora-9-i386 init

rpmdev-setuptree
# or
#echo '%_topdir      %(echo $HOME)/rpmbuild' >> ~/.rpmmacros
#mkdir -p rmpbuild/SOURCES rpmbuild/SRPMS
