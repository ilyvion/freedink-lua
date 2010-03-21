Name:		freedink
Version:	1.08.20100321
Release:	1%{?dist}
Summary:	Adventure and role-playing game

Group:		Amusements/Games
BuildRequires:	SDL-devel SDL_gfx-devel SDL_ttf-devel SDL_image-devel SDL_mixer-devel
BuildRequires:  fontconfig-devel
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
Requires:	freedink-data
# TiMidity++ is useful to play midis when /dev/sequencer isn't
# functional (most of the case) and installing it prevents some
# SDL_mixer freezes (see TROUBLESHOOTING).  In Fedora this is done
# through SDL_mixer dependencies.
%if 0%{?suse_version}
Requires: timidity
%endif

%if 0%{?with_included_liberation_font}
# No dependency
%else
# Repect Fedora guidelines (see below)
%if 0%{?fedora}
Requires: liberation-sans-fonts
%else
Requires: liberation-fonts
%endif
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
# openSUSE does not allow empty packages, so create at least one file
%if 0%{?suse_version}
cat > README.META << EOF
This is just a meta package to require all needed packages.
EOF
%endif

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
%if 0%{?with_included_liberation_font}
# Include it nonetheless for the sake of avoiding
# liberation-fonts<->liberation-sans-fonts conflicts in the
# freedink.org repository
%else
# Remove it for compliance with Fedore guidelines
rm $RPM_BUILD_ROOT%{_datadir}/%{name}/LiberationSans-Regular.ttf
%endif

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
# openSUSE does not allow empty packages
%if 0%{?suse_version}
%doc README.META
%endif

%files engine -f %{name}.lang
%defattr(-,root,root,-)
%doc AUTHORS COPYING NEWS README THANKS TROUBLESHOOTING ChangeLog
%{_bindir}/*
%{_datadir}/applications/*
%{_datadir}/%{name}/
%{_datadir}/pixmaps/*
%{_mandir}/man6/*


%changelog
* Sun Mar 21 2010 Sylvain Beucler <beuc@beuc.net> - 1.08.20100321-1
- New upstream release

* Fri Sep 18 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090918-1
- New upstream release

* Wed Sep 16 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090916-1
- New upstream release
- Can optionaly bundle default font, to avoid liberation-fonts
  vs. liberation-sans-fonts issues when building snapshot RPMs

* Fri Jul 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.08.20090120-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild

* Tue Feb 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.08.20090120-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_11_Mass_Rebuild

* Wed Feb  4 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090120-2
- Apply Fedora font rename: liberation-fonts -> liberation-sans-fonts

* Tue Jan 20 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090120-1
- New upstream release (fix engine freeze in some DinkC scripts)

* Wed Jan  9 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090109-2
- Bump version to fix build tag issue

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
