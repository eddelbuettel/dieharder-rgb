Name: dieharder-src
Version: 2.6.24
Release: 1
Summary: Dieharder is a random number generator tester and timer
Group: Development/Tools
License: Open Source (GPL)
Source: dieharder-%{version}.tgz
URL: http://www.phy.duke.edu/~rgb/General/dieharder.php

# Mandatory path for Fedora Core builds
Buildroot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description 

########################################################################
# LIBRARY: This is the basic dieharder library
########################################################################
%package -n libdieharder
Summary: A library of random number generator tests and timing routines
Group: Development/Tools
%description -n libdieharder

libdieharder is the core library of dieharder and friends, designed to
be "the last suite of random number testers you'll ever wear".  It can
test any of its many prebuilt and library linked generators (basically
all of those in the Gnu Scientific Library plus a number of others from
various sources) or a potentially random dataset in either an
ascii-formatted or raw (presumed 32 bit uint) binary file.  It is fairly
straightforward to wrap new software generators for testing, or to add
hardware generators that have a software interface for testing, and the
file input method permits pretty much any software or hardware RNG to be
tested using libdieharder calls.

libdieharder has as a design goal the full encapsulation in an
extensible shell of basically all the random number tests I have been
able to find -- George Marsaglia's "diehard" battery of tests, STS
(v1.6) from NIST FIPS, Knuth's tests, and more.  Check in the man
page(s) or /usr/share/dieharder*/dieharder.pdf for documentation.

########################################################################
# This is dieharder itself, the tty UI
########################################################################
%package -n dieharder
Summary: dieharder is a random number generator tester and timer.
Group: Development/Tools
Requires: libdieharder = %{version}

%description -n dieharder

dieharder is a fairly involved random number/uniform deviate generator
tester.  It can either test any of its many prebuilt and linked
generators (basically all of those in the Gnu Scientific Library plus
others I've added) or a potentially random dataset in a file.  With file
input, it can manage either a variety of ascii-formatted input or a raw
binary bitstring.  It is thus suitable for use in testing both software
RNG's and hardware RNG's.

dieharder does all of its work with a standalone, extensible library,
libdieharder.  Its tests can therefore be integrated into other
programs, subject to the requirements of the GPL (NOT, note, the LGPL as
it uses the GSL which is full GSL, viral).

dieharder encapsulates (or will eventually encapsulate) basically all
the random number tests I have been able to find -- George Marsaglia's
"diehard" battery of tests, STS (v1.5) from NIST FIPS, Knuth's tests,
and more.  Check in the man page or /usr/share documentation for a
complete list of the tests and references where possible.  It is
intended to be the "swiss army knife of random number testers", or "the
last suite of random number testers you'll ever wear".

########################################################################
# The main section common to all builds.
########################################################################
%prep
%setup -q -n dieharder-%{version}

%build
make clean

# Note that multipackage sources with libraries are happier with
# their own local buildroot to facilitate development without a
# full install.
%install
make BUILDROOT=%{buildroot} PREFIX=%{buildroot}/usr install

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf %{builddir}

########################################################################
# Files in libdieharder
########################################################################
%files -n libdieharder

%defattr(-,root,root)

%dir /usr/lib

# The libdieharder library.  We ONLY install the primary
# versioned library -- ldconfig and ln are used to finish off
# later.
/usr/lib/libdieharder.so.%{version}

# The libdieharder include files are under here
%attr(644,root,root) /usr/include/dieharder

# The libdieharder man page
%attr(644,root,root) /usr/share/man/man3/libdieharder.3.gz

# The dieharder docs go in with the library, since the library
# is likely to be installed "everywhere".
%doc Copyright README COPYING NOTES

########################################################################
# Files installed with the dieharder tty UI
########################################################################
%files -n dieharder
%defattr(-,root,root)

# The dieharder binary
%attr(755,root,root) /usr/bin/dieharder

# The dieharder man page
%attr(644,root,root) /usr/share/man/man1/dieharder.1.gz

%doc Copyright README COPYING NOTES

########################################################################
# Command to execute post install or uninstall of libdieharder
########################################################################
%post -n libdieharder

cd /usr/lib
ldconfig -n .
ln -sf libdieharder.so.%{version} libdieharder.so

%postun -n libdieharder

rm -f /usr/lib/libdieharder.so
rm -f /usr/lib/libdieharder.so.%{version}
ldconfig

%changelog 
