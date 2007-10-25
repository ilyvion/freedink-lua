On a minimal Debian system:

aptitude install build-essential git-core
(cd /usr/src && git clone git://git.sv.gnu.org/freedink)
git clone git://git.sv.gnu.org/gnulib
cd freedink
aptitude install autoconf automake libtool
aptitude install libsdl1.2-dev libsdl-gfx1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libzzip-dev zip upx
sh bootstrap
./configure
make
make install
# :)
