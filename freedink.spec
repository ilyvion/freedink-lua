Name:		freedink
Version:	1.08.20080830
Release:	1%{?dist}
Summary:	Adventure and role-playing game

Group:		Amusements/Games
License:	GPLv3+
URL:		http://www.freedink.org/
#Source0:	ftp://ftp.gnu.org/gnu/freedink/freedink-1.08.20080828.tar.gz
Source0:	http://www.freedink.org/snapshots/freedink-1.08.20080830.tar.gz

BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Requires:	freedink-engine freedink-dfarc

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
BuildRequires:	SDL-devel SDL_gfx-devel SDL_ttf-devel SDL_image-devel SDL_mixer-devel fontconfig-devel
%if 0%{?suse_version}
BuildRequires:  update-desktop-files
%endif
Requires:	freedink-data
# TiMidity++ is useful to play midis when /dev/sequencer isn't
# functional (most of the case) and installing it prevents some
# SDL_mixer freezes (see TROUBLESHOOTING).  In Fedora this is done
# through SDL_mixer dependencies.
%if 0%{?suse_version}
Requires: timidity
%endif
%if 0%{?fedora_version} >= 10
# See %install for explanation
Requires: liberation-fonts
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
# them anyway (so it can make the -debuginfo package -- too bad :/
%configure --disable-embedded-resources 
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
%if 0%{?suse_version}
%suse_update_desktop_file -i %name
%suse_update_desktop_file -i %{name}edit
%endif
%if 0%{?fedora_version} >= 10
# Policy insists on not installing a different version of "Liberation
# Sans". Beware that the system version may be different than the
# official FreeDink font, because Liberation changes regularly.
rm $RPM_BUILD_ROOT%{_datadir}/%{name}/LiberationSans-Regular.ttf
%endif

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)

%files engine
%defattr(-,root,root,-)
%doc AUTHORS COPYING NEWS README THANKS TROUBLESHOOTING
%{_bindir}/*
%{_datadir}/applications/*
%{_datadir}/%{name}/
%{_datadir}/pixmaps/*
%{_mandir}/man6/*


%changelog
* Sat Aug 30 2008 Sylvain Beucler <beuc@beuc.net> 1.08.20080830-1
- UNRELEASED snapshot

* Thu Aug 28 2008 Sylvain Beucler <beuc@beuc.net> 1.08.20080828-1
- Initial package
