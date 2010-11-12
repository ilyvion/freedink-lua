#!/bin/bash -ex
# Source release

# Copyright (C) 2008, 2009, 2010  Sylvain Beucler

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

set -ex

PACKAGE=freedink

if [ -n "$1" -a "$1" != "release" ]; then
    echo "Usage: $0 [release x.y.z]"
    exit 1
fi

if [ "$1" == "release" ]; then
    VERSION=$2
    if [ -z "$VERSION" ]; then
	echo "Invalid version."
	exit 1
    fi
fi

cd /usr/src/
if [ ! -e $PACKAGE ]; then
    git clone git://git.savannah.gnu.org/$PACKAGE
fi
pushd freedink
git pull
git fetch --tags  # for updated tags
popd

rm -rf $PACKAGE-wd
git clone $PACKAGE/.git $PACKAGE-wd
pushd $PACKAGE-wd/
if [ "$1" == "release" ];
then
    git checkout v$VERSION
else
    git checkout master
fi
sh bootstrap clean

if [ "$1" != "release" ];
then
    VERSION=1.08.$(date +%Y%m%d)
    sed -i -e 's/^AC_INIT(\([^,]\+\),[^,]\+,\([^,]\+\))/AC_INIT(\1,['$VERSION'],\2)/' configure.ac
    sed -i -e '1s/.*/'$VERSION'/' NEWS
fi

sh bootstrap
mkdir build
pushd build/
../configure
# 'help2man' needs ./freedink needs ../gnulib/lib/*.[ah]
# and this isn't simple to implement this dependency,
# especially for 'make dist'.
# Let's do a 'make check' while we're at recompiling everything
make check

PUBDIR=/mnt/snapshots/$PACKAGE/$VERSION
mkdir -p $PUBDIR

# Enable .zip if you want woe users to use the archive (e.g. to
# get the .pot and help translate). Tarballs are badly handled by
# 7z (need double extraction .gz then .tar) and so are not
# recommended.
#make dist
make dist-gzip
make dist-zip
mv -f *.tar.gz *.zip $PUBDIR
popd

cp NEWS /mnt/snapshots/$PACKAGE-NEWS.txt
mv -f po/$PACKAGE.pot /mnt/snapshots/
#cp -r gentoo /mnt/snapshots/
popd

#rm -rf $PACKAGE-wd
