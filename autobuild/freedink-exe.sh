#!/bin/bash -ex
# MS Woe release

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
  http://www.freedink.org/snapshots/

The source code is the "recipe" of FreeDink, that let you study it,
modify it, and redistribute your changes.  The GNU GPL license
explicitely allows you to do so (see freedink-COPYING.txt).

If you upload a FreeDink .exe on your website, you must also offer the
corresponding source code for download.
EOF

  cat <<EOF > zip/freedink-DLL.txt
The .dll files are compiled versions of several free software
projects.

See
  http://www.freedink.org/releases/woe/depsources/
  http://www.freedink.org/snapshots/woe/depsources/
for their source code, and
  doc/cross.txt
in the FreeDink source code to see how they were compiled.

See also licenses/ for your rights on these projects.
EOF
  mkdir -m 755 zip/licenses
  cp -a ../COPYING zip/licenses/${PACKAGE}-${VERSION}_COPYING
  cp -a /usr/src/SDL-1.2.14/COPYING zip/licenses/SDL-1.2.14_COPYING
  cp -a /usr/src/libogg-1.2.1/COPYING zip/licenses/libogg-1.2.1_COPYING
  cp -a /usr/src/libvorbis-1.3.1/COPYING zip/licenses/libvorbis-1.3.1_COPYING
  cp -a /usr/src/SDL_mixer-1.2.11/COPYING zip/licenses/SDL_mixer-1.2.11_COPYING
  cp -a /usr/src/SDL_mixer-1.2.11/libmikmod-3.1.12.patched/COPYING.LESSER zip/licenses/libmikmod-3.1.12.patched_LESSER
  cp -a /usr/src/freetype-2.4.3/docs/GPL.TXT zip/licenses/freetype-2.4.3_GPL.TXT
  cp -a /usr/src/SDL_ttf-2.0.9/COPYING zip/licenses/SDL_ttf-2.0.9_COPYING
  cp -a /usr/src/SDL_gfx-2.0.22/LICENSE zip/licenses/SDL_gfx-2.0.22_LICENSE
  cp -a /usr/src/jpeg-8b/README zip/licenses/jpeg-8b_README
  cp -a /usr/src/zlib-1.2.5/README zip/licenses/zlib-1.2.5_README
  cp -a /usr/src/libpng-1.4.4/LICENSE zip/licenses/libpng-1.4.4_LICENSE
  cp -a /usr/src/tiff-3.9.4/COPYRIGHT zip/licenses/tiff-3.9.4_COPYRIGHT
  cp -a /usr/src/SDL_image-1.2.10/COPYING zip/licenses/SDL_image-1.2.10_COPYING
  cp -a /usr/src/libzip-0.9.3/lib/zip.h zip/licenses/libzip-0.9.3_zip.h
  cp -a /usr/src/libiconv-1.13.1/COPYING.LIB zip/licenses/libiconv-1.13.1_COPYING.LIB
  cp -a /usr/src/gettext-0.18.1.1/COPYING zip/licenses/gettext-0.18.1.1_COPYING
  
  # Include documentation with MS-DOS newlines (if not already)
  for i in freedink-DLL.txt licenses/*; do
      sed -i -e 's/\(^\|[^\r]\)$/\1\r/' zip/freedink-*.txt zip/licenses/*
  done

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

PUBDIR=/mnt/snapshots/$PACKAGE/$VERSION
PUBDIR_WOE=$PUBDIR/woe
mkdir -p $PUBDIR_WOE

rm -rf t/
mkdir t
pushd t
tar xzf $PUBDIR/$PACKAGE-$VERSION.tar.gz
cd $PACKAGE-$VERSION/

# Install extra-files first, to spot issues early (rather than after
# several minutes of compilation):
mkdir -p cross/ cross-dll/
pushd cross
copy_extra_files
popd
pushd cross-dll
copy_extra_files
# DLLs
for i in SDL.dll SDL_image.dll SDL_mixer.dll libmikmod-2.dll SDL_ttf.dll \
    libSDL_gfx-13.dll libfreetype-6.dll libjpeg-8.dll libogg-0.dll \
    libpng14-14.dll libtiff-3.dll libvorbis-0.dll libvorbisfile-3.dll \
    libz-1.dll libzip-1.dll; do
    cp -a /usr/local/i586-mingw32msvc/bin/$i zip/
done
upx zip/*.dll
popd

# Keep track of dependencies for _each_ build, since dependencies
# versions may differ
mkdir -p $PUBDIR_WOE/depsources/
for i in directx-devel.tar.gz SDL-1.2.14.tar.gz pkg-config-0.25.tar.gz \
    zlib-1.2.5.tar.gz jpegsrc.v8b.tar.gz libpng-1.4.4.tar.bz2 tiff-3.9.4.tar.gz SDL_image-1.2.10.tar.gz \
    libogg-1.2.1.tar.gz libvorbis-1.3.1.tar.gz SDL_mixer-1.2.11.tar.gz \
    freetype-2.4.3.tar.bz2 SDL_ttf-2.0.9.tar.gz \
    SDL_gfx-2.0.22.tar.gz libzip-0.9.3.tar.gz \
    libiconv-1.13.1 gettext-0.18.1.1.tar.gz; do
    ln -f /mnt/snapshots/woe/depsources/$i $PUBDIR_WOE/depsources/
done

HOST=i586-mingw32msvc
PREFIX=/usr/local/$HOST
BUILD=i686-pc-linux-gnu

# full-static
pushd cross/
SDL_CONFIG=$PREFIX/bin/sdl-config \
  CPPFLAGS="-I $PREFIX/include" \
  LDFLAGS="-L$PREFIX/lib" \
  ../configure --build=$BUILD --host=$HOST \
  --enable-static --enable-upx
make
make install-strip DESTDIR=`pwd`/destdir
# move .exe but avoid symlinks
find destdir/usr/local/bin/ -type f -name "*.exe" | while read file; do
  cp -a $file $PUBDIR_WOE/$(basename ${file%.exe}-$VERSION.exe)
  mv $file zip/
done
rm -f $PUBDIR_WOE/$PACKAGE-$VERSION-bin.zip
(cd zip/ && zip -r $PUBDIR_WOE/$PACKAGE-$VERSION-bin.zip *)
popd

# .dll
pushd cross-dll/
SDL_CONFIG=$PREFIX/bin/sdl-config \
  CPPFLAGS="-I $PREFIX/include" \
  LDFLAGS="-L$PREFIX/lib" \
  ../configure --build=$BUILD --host=$HOST \
  --enable-upx
make
make install-strip DESTDIR=`pwd`/destdir
find destdir/usr/local/bin/ -type f -name "*.exe" | while read file; do
    cp -a $file $PUBDIR_WOE/$(basename ${file%.exe}-$VERSION-dll.exe)
    mv $file zip/
done
rm -f $PUBDIR_WOE/$PACKAGE-$VERSION-bin-dll.zip
(cd zip/ && zip -r $PUBDIR_WOE/$PACKAGE-$VERSION-bin-dll.zip *)
popd

popd
#rm -rf t
