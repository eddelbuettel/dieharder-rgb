# The following should match PROGRAM, VERSION and RELEASE in the
# Makefile accompanying this program (and the .tgz defined in Source
# below.
%define name OPERM
%define version 1.1.2
%define release 1

%define builddir $RPM_BUILD_DIR/%{name}-%{version}.%{release}

Summary: A simple C source OPERM template

Name: %{name}
Version: %{version}
Release: %{release}
Group: Development
Copyright: Open Source (GPL v2b)
Source: %{name}.tgz
Buildroot: /var/tmp/%{name}-%{version}-%{release}-root

%description 
This is a very simple OPERM template.  It contains all that is needed
to start a simple C OPERM.  Note that it is NOT GNU autoconf based and
hence not terribly compatible with e.g. glade, but on the other hand one
can build tarballs and rpm's with ease. The package's primary author (as
of 12/31/00) can be contacted at rgb@phy.duke.edu.

%prep
%setup -q -n %{name}

%build
make clean
make

%install
make PREFIX=%{buildroot}/usr install

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf %{builddir}

%files 
%defattr(644,root,root)
# The OPERM binary
%attr(755,root,root) /usr/bin/OPERM
# The OPERM man page
%attr(644,root,root) /usr/share/man/man1/OPERM.1.gz

%post

cat << EOT

  Congratulations!  OPERM is now installed!

If it didn't work, I'm generally available at:

   Robert G. Brown <rgb@phy.duke.edu>

but please FIRST try to figure it out yourself!  I'm busy!

Good Luck.

EOT

exit

%postun

cat << EOT

  Congratulations!  OPERM is now (hopefully) uninstalled!

If it didn't work, I'm generally available at:

   Robert G. Brown <rgb@phy.duke.edu>

but please FIRST try to figure it out yourself!  I'm busy!

Good Luck.

EOT

%changelog
* Wed Dec  31 2000 Robert G. Brown <rgb@duke.edu>
- set up and built for RH 6.2
