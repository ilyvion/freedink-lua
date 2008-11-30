# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit games

DESCRIPTION="adventure and role-playing game (engine)"
MY_PN="freedink"
MY_P="${MY_PN}-${PV}"
HOMEPAGE="http://www.freedink.org/"
SRC_URI="http://ftp.gnu.org/gnu/freedink/${MY_P}.tar.bz2
		 mirror://gnu/freedink/${P}.tar.bz2"
#SRC_URI="http://www.freedink.org/snapshots/${MY_P}.tar.gz"
LICENSE="GPL-3"
SLOT="0"
IUSE=""
KEYWORDS="~x86"
DEPEND="
	>=dev-util/pkgconfig-0.9
	>=media-libs/libsdl-1.2
	>=media-libs/sdl-mixer-1.2
	>=media-libs/sdl-ttf-2.0.9
	>=media-libs/sdl-image-1.2
	>=media-libs/sdl-gfx-2.0
	>=media-libs/fontconfig-2.4
	>=dev-libs/zziplib-0.12
	>=dev-libs/libffi-3
	games-rpg/freedink-data
"

S="${WORKDIR}/${MY_P}"

src_compile() {
	egamesconf --disable-embedded-resources || die
	emake || die "emake failed"
}

src_install() {
	make DESTDIR="${D}" install || die "make install failed"
	dodoc AUTHORS NEWS README THANKS TROUBLESHOOTING
	prepgamesdirs
}

