#!/bin/bash
# Source release

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

set -ex

if [ -n "$1" -a "$1" != "release" ];
then
    echo "Usage: $0 [release]"
    exit 1
fi

VERSION=1.08.$(date +%Y%m%d)
cd /usr/src/
if [ ! -e freedink ]; then
    git clone git://git.savannah.gnu.org/freedink
fi

pushd freedink/
git checkout .
git checkout master
git pull
if [ "$1" == "release" ];
then
    git checkout v$VERSION
fi
sh bootstrap clean

if [ "$1" != "release" ];
then
    sed -i -e 's/^AC_INIT(\([^,]\+\),[^,]\+,\([^,]\+\))/AC_INIT(\1,['$VERSION'],\2)/' configure.ac
    sed -i -e '1s/.*/'$VERSION'/' NEWS
fi

sh bootstrap
rm -rf build/
mkdir build
pushd build/
../configure
# 'help2man' needs ./freedink needs ../gnulib/lib/*.[ah]
# and this isn't simple to implement this dependency,
# especially for 'make dist'.
# Let's do a 'make check' while we're at recompiling everything
make check

if [ "$1" != "release" ];
then
    make dist-gzip
    mv *.tar.gz /mnt/snapshots/
else
    make dist
    mv *.tar.gz *.tar.bz2 *.zip /mnt/snapshots/
fi

popd

cp -r gentoo /mnt/snapshots/
popd
