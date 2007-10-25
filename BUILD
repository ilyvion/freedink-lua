On a minimal Debian system:

## Source code
aptitude install git-core
git clone git://git.sv.gnu.org/freedink
cd freedink

## Dependencies
aptitude install build-essential
# Gnulib
(cd /usr/src && git clone git://git.sv.gnu.org/gnulib)
# autotools - libtool is requires by relocatable.m4
aptitude install autoconf automake libtool
# Required: SDL and zziplib
aptitude install libsdl1.2-dev libsdl-gfx1.2-dev libsdl-ttf2.0-dev \
  libsdl-image1.2-dev libsdl-mixer1.2-dev libzzip-dev zip
# Optional:
# - upx compresses binary
# - bzip is for .tar.bz2 release tarballs
aptitude install upx bzip2

## Compilation
sh bootstrap
./configure
make
make install

## Release tests
make dist
make distcheck

# :)
