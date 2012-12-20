#!/bin/bash -ex
# MS Woe release, built with MXE

# Copyright (C) 2008, 2009, 2010, 2012  Sylvain Beucler

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

# cd /usr/src/mxe/
# make -j$(nproc) JOBS=$(nproc) gcc sdl sdl_gfx sdl_image sdl_mixer sdl_ttf libzip gettext nsis
## Disable MP3 support
# make sdl_mixer

function copy_extra_files {
  mkdir zip/

  # documentation
  for i in README.txt TRANSLATIONS.txt; do
      cp ../$i zip/freedink-$i
  done
  for i in AUTHORS COPYING NEWS THANKS TROUBLESHOOTING; do
      cp ../$i zip/freedink-$i.txt
  done
  cat <<EOF > zip/freedink-SOURCE.txt
The FreeDink source code is available at:
  http://ftp.gnu.org/gnu/freedink/

The source code is the "recipe" of FreeDink, that let you study it,
modify it, and redistribute your changes.  The GNU GPL license
explicitely allows you to do so (see freedink-COPYING.txt).

If you upload a FreeDink .exe on your website, you must also offer the
corresponding source code for download.
EOF

  # TODO: gettext translation files:
  #cp -a destdir/usr/local/lib/locale zip/po
  # Note: requires making freedink look in './po/'
  ## translation template:
  #cp ../po/dfarc.pot zip/po/
}

PACKAGE=freedink
VERSION=$1
if [ -z "$VERSION" ]; then
    VERSION=$(cd /mnt/snapshots/$PACKAGE && ls -d */ | sed 's,/$,,' | sort -n | tail -1)
fi

PUBDIR=${PUBDIR:=/mnt/snapshots/$PACKAGE/$VERSION}
PUBDIR_WOE=$PUBDIR/woe
mkdir -p $PUBDIR_WOE

rm -rf t/
mkdir t
pushd t
tar xzf $PUBDIR/$PACKAGE-$VERSION.tar.gz
cd $PACKAGE-$VERSION/

# Install extra-files first, to spot issues early (rather than after
# several minutes of compilation):
mkdir -p woe/
pushd woe
copy_extra_files
popd

HOST=i686-pc-mingw32
PATH=/usr/src/mxe/usr/bin:$PATH
PREFIX=/usr/src/mxe/usr/$HOST
BUILD=i686-pc-linux-gnu
# Already done in the MXE wrapper, but needed for other mingw environments:
#export PKG_CONFIG="i686-pc-mingw32-pkg-config --static"

# full-static
# - specify --build or autoconf will not understand we cross-compile
#   during some tests
pushd woe/
../configure --build=$BUILD --host=$HOST \
  --enable-static --enable-upx
make -j $(nproc)
make install-strip DESTDIR=$(pwd)/destdir
# move .exe but avoid symlinks
find destdir/usr/local/bin/ -type f -name "*.exe" | while read file; do
  cp -a $file $PUBDIR_WOE/$(basename ${file%.exe}-$VERSION.exe)
  mv $file zip/
done
rm -f $PUBDIR_WOE/$PACKAGE-$VERSION-bin.zip
(cd zip/ && zip -r $PUBDIR_WOE/$PACKAGE-$VERSION-bin.zip *)
popd

popd
#rm -rf t
