#!/bin/bash -ex
# Fedora release

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
cp -a /mnt/snapshots/$TARBALL rpmbuild/SOURCES/
cp ~/$PACKAGE/$PACKAGE.spec rpmbuild/SPECS/
sed -i -e "s/^Version:.*/Version:	$VERSION/"  rpmbuild/SPECS/$PACKAGE.spec
rpmbuild -bs --nodeps rpmbuild/SPECS/$PACKAGE.spec
mock -r fedora-10-i386 --resultdir /mnt/snapshots/fedora --rebuild -D "with_included_liberation_font 1" rpmbuild/SRPMS/$PACKAGE-$VERSION-1*.src.rpm
cp -f rpmbuild/SPECS/$PACKAGE.spec /mnt/snapshots/fedora
make -C /mnt/snapshots/fedora/
exit


## Construction

# - Doesn't seem to be a way to install using VServer
# - Get a minimal install, using "Install CDs", e.g.:
#   http://mirror.ovh.net/download.fedora.redhat.com/linux/releases/13/Fedora/x86_64/iso/
#   Note: probably need CD 1 AND 2...
# - Install using real or virtualized hardware
# - Copy the result here and chroot in it
mount /proc
mount none -t devtmpfs /dev
yum update
yum groupinstall 'Development Tools'

yum install mock

# This one doesn't work in a VServer - use 'chroot' instead
mock -r fedora-12-i386 init

rpmdev-setuptree
# or
#echo '%_topdir      %(echo $HOME)/rpmbuild' >> ~/.rpmmacros
#mkdir -p rmpbuild/SOURCES rpmbuild/SRPMS
