#!/bin/bash -ex
# MS Woe release

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
cp -a /mnt/snapshots/$TARBALL .
rm -rf t/
mkdir t
pushd t
tar xzf ../freedink-$VERSION.tar.gz
cd freedink-$VERSION/

# full-static
mkdir cross
pushd cross/
SDL_CONFIG=/usr/local/i586-mingw32msvc/bin/sdl-config \
  ../configure --host=i586-mingw32msvc --build=i686-pc-linux-gnu \
  --enable-static --enable-upx
make
make install-strip DESTDIR=`pwd`/destdir
# move .exe but avoid symlinks
find destdir/usr/local/bin/ -type f -name "*.exe" | while read file; do
  mv $file /mnt/snapshots/woe/$(basename ${file%.exe}-$VERSION.exe)
done

# .dll
rm -rf destdir/
SDL_CONFIG=/usr/local/i586-mingw32msvc/bin/sdl-config \
  ../configure --host=i586-mingw32msvc --build=i686-pc-linux-gnu \
  --enable-upx
make
make install-strip DESTDIR=`pwd`/destdir
mkdir zip
find destdir/usr/local/bin/ -type f -name "*.exe" | while read file; do
  cp -a $file /mnt/snapshots/woe/$(basename ${file%.exe}-$VERSION-dll.exe)
  mv $file zip/$(basename ${file%.exe}-$VERSION-dll.exe)
done
for i in SDL.dll SDL_image.dll SDL_mixer.dll SDL_ttf.dll \
    libSDL_gfx-0.dll libfreetype-6.dll libjpeg-62.dll libogg-0.dll \
    libpng12-0.dll libtiff-3.dll libvorbis-0.dll libvorbisfile-3.dll \
    libz-1.dll libzip-1.dll; do
   cp -a /usr/local/i586-mingw32msvc/bin/$i zip/
done
rm -f /mnt/snapshots/woe/freedink-dlls.zip
(cd zip/ && zip /mnt/snapshots/woe/freedink-dlls.zip *)
popd

popd
#rm -rf t
