Name:		freedink
Version:	1.08.20080821
Release:	1%{?dist}
Summary:	adventure and role-playing game (engine)

Group:		Amusements/Games
License:	GPLv3+
URL:		http://www.freedink.org/
Source0:	http://www.freedink.org/snapshots/freedink-1.08.20080821.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:	SDL-devel SDL_gfx-devel SDL_ttf-devel SDL_image-devel SDL_mixer-devel zziplib-devel zip
Requires:	dink-data, dfarc

%description
Dink Smallwood is an adventure/role-playing game, similar to Zelda,
made by RTsoft. Besides twisted humour, it includes the actual game
editor, allowing players to create hundreds of new adventures called
Dink Modules or D-Mods for short.

GNU FreeDink is a new and portable version of the game engine, which
runs the original game as well as its D-Mods, with close
compatibility, under multiple platforms.


%prep
%setup -q


%build
# Using '--disable-embedded-resources' because 'rpmbuild' will remove
#   them anyway (so it can make the -debuginfo package -- too bad :/
%configure --disable-embedded-resources 
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING NEWS README THANKS TROUBLESHOOTING
%{_bindir}/*
%{_datadir}/applications/*
%{_datadir}/%{name}/
%{_datadir}/pixmaps/*
%{_mandir}/man6/*


%changelog
* Fri Aug 22 2008 Sylvain Beucler <beuc@beuc.net> 1.08.20080821-1
- Initial package
