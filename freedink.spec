Name:		freedink
Version:	1.08.20090109
Release:	1%{?dist}
Summary:	Adventure and role-playing game

Group:		Amusements/Games
BuildRequires:	SDL-devel SDL_gfx-devel SDL_ttf-devel SDL_image-devel SDL_mixer-devel
BuildRequires:  fontconfig-devel libffi-devel
BuildRequires:  desktop-file-utils
%if 0%{?suse_version}
BuildRequires:  update-desktop-files
%endif
License:	GPLv3+
URL:		http://www.freedink.org/
Source0:	ftp://ftp.gnu.org/gnu/freedink/freedink-%{version}.tar.gz
#Source0:	http://www.freedink.org/snapshots/freedink-%{version}.tar.gz

BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Requires:	freedink-engine = %{version}-%{release}  freedink-dfarc

%description
Dink Smallwood is an adventure/role-playing game, similar to Zelda,
made by RTsoft. Besides twisted humour, it includes the actual game
editor, allowing players to create hundreds of new adventures called
Dink Modules or D-Mods for short.

GNU FreeDink is a new and portable version of the game engine, which
runs the original game as well as its D-Mods, with close
compatibility, under multiple platforms.

This package is a metapackage to install the game, its data and a
front-end to manage game options and D-Mods.


%package engine
Summary:	Adventure and role-playing game (engine)
Group:		Amusements/Games
Requires:	freedink-data liberation-fonts
# TiMidity++ is useful to play midis when /dev/sequencer isn't
# functional (most of the case) and installing it prevents some
# SDL_mixer freezes (see TROUBLESHOOTING).  In Fedora this is done
# through SDL_mixer dependencies.
%if 0%{?suse_version}
Requires: timidity
%endif

%description engine
Dink Smallwood is an adventure/role-playing game, similar to Zelda,
made by RTsoft. Besides twisted humour, it includes the actual game
editor, allowing players to create hundreds of new adventures called
Dink Modules or D-Mods for short.

GNU FreeDink is a new and portable version of the game engine, which
runs the original game as well as its D-Mods, with close
compatibility, under multiple platforms.

This package contains the game engine alone.


%prep
%setup -q

%build
# Using '--disable-embedded-resources' because 'rpmbuild' will remove
# them anyway (so it can make the -debuginfo package -- too bad :/)
%configure --disable-embedded-resources 
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT INSTALL="install -p"
%find_lang %{name}
%find_lang %{name}-gnulib
# %files only support one '-f' argument (see below)
cat %{name}-gnulib.lang >> %{name}.lang
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}edit.desktop
%if 0%{?suse_version}
%suse_update_desktop_file -i %name
%suse_update_desktop_file -i %{name}edit
%endif
# http://fedoraproject.org/wiki/Packaging/Guidelines#Avoid_bundling_of_fonts_in_other_packages
# Policy insists on not installing a different version of "Liberation
# Sans". Beware that the system version may be different than the
# official FreeDink font, because Liberation changes regularly.
rm $RPM_BUILD_ROOT%{_datadir}/%{name}/LiberationSans-Regular.ttf

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)

%files engine -f %{name}.lang
%defattr(-,root,root,-)
%doc AUTHORS COPYING NEWS README THANKS TROUBLESHOOTING ChangeLog
%{_bindir}/*
%{_datadir}/applications/*
%{_datadir}/%{name}/
%{_datadir}/pixmaps/*
%{_mandir}/man6/*


%changelog
* Wed Jan  9 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090109-1
- New upstream release
- Declare .mo translation catalogs

* Sun Oct  5 2008 Sylvain Beucler <beuc@beuc.net> - 1.08.20080920-4
- Use liberation-fonts in all distro versions

* Wed Sep 24 2008 Sylvain Beucler <beuc@beuc.net> - 1.08.20080920-3
- Don't use 'update-desktop-database' for simple desktop files
- Fix unescaped macros in comments
- Use spaces around '=' in version-specific dependency

* Wed Sep 24 2008 Sylvain Beucler <beuc@beuc.net> - 1.08.20080920-2
- Fix variable s/fedora_version/fedora/
- Meta-package depends on same version of freedink-engine
- Use "install -p" to preserve timestamps
- Validate installed .desktop files

* Sat Sep 20 2008 Sylvain Beucler <beuc@beuc.net> - 1.08.20080920-1
- New upstream release

* Thu Aug 28 2008 Sylvain Beucler <beuc@beuc.net> - 1.08.20080828-1
- Initial package
