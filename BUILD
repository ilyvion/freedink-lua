Instructions to build from scratch (from the repository sources).

If you're using a release (TODO: release a release ;)) you won't need
bootstrap instructions.


On a minimal Debian system
==========================

## Bootstrap
# Source code:
aptitude install git-core
git clone git://git.sv.gnu.org/freedink
cd freedink

# Gnulib
(cd /usr/src && git clone git://git.sv.gnu.org/gnulib)
# or:
#aptitude install gnulib

# autotools - libtool is requires by relocatable.m4
aptitude install autoconf automake libtool

aptitude install libsdl1.2-dev # for sdl.m4
sh bootstrap


## Dependencies
# Base: GCC, make & al.
aptitude install build-essential
# Required: SDL and zziplib
aptitude install libsdl1.2-dev libsdl-gfx1.2-dev libsdl-ttf2.0-dev \
  libsdl-image1.2-dev libsdl-mixer1.2-dev libzzip-dev zip
# Optional:
# - upx compresses binary
# - bzip is for .tar.bz2 release tarballs
aptitude install upx bzip2

## Compilation
./configure
make
make install

## Release tests
make dist
make distcheck

# :)


On a minimal Gentoo system
==========================

## Bootstrap
(cd /usr/src && git clone git://git.sv.gnu.org/gnulib)
# or:
# Gnulib is currently masked, you need to unmask it:
# http://gentoo-wiki.com/Masked#Masked_by_missing_keyword
# Not that there are issues with the Gentoo wrapper. Use
# /usr/share/gnulib/gnulib-tool instead of /usr/bin/gnulib-tool if
# possible
echo "dev-libs/gnulib **" >> /etc/portage/package.keywords
emerge gnulib

emerge dev-util/git
git clone git://git.sv.gnu.org/freedink
cd freedink

# I assume you already have autoconf & al. ;)

emerge libsdl # for sdl.m4
sh bootstrap


## Dependencies
# I also assume you already have GCC ;)
# Required: SDL and zziplib
emerge libsdl sdl-gfx sdl-ttf sdl-image sdl-mixer zziplib zip
# Optional:
# - upx compresses binary
# - bzip is for .tar.bz2 release tarballs (included in base Gentoo)
emerge upx

./configure
make
make install

## Release tests
make dist
make distcheck

# :)
