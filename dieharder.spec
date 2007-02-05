# The following should match PROGRAM, VERSION and RELEASE in the
# Makefile accompanying this program (and the .tgz defined in Source
# below.


Name: dieharder
Version: 2.4.24
Summary: dieharder is a random number generator tester and timer.
Release: 3
Group: Development/Tools
License: Open Source (GPL v2b)
Source: dieharder.tgz
Buildroot: /var/tmp/%{name}-%{version}-%{release}-root

%description 

# This application consists of two parts, a UI and the library.
# To facilitate development of multiple UIs and interfaces, we
# split off the actual testing code into a library with an API.

# This is the UI package
%package -n dieharder-ui
Summary: dieharder is a random number generator tester and timer.
Group: Development/Tools
Requires: libdieharder = %{version}
%description -n dieharder-ui

dieharder is a fairly involved random number/uniform deviate generator
tester.  It can either test any of its many prebuilt and linked
generators (basically all of those in the Gnu Scientific Library plus
any others I've been able to find) or a potentially random dataset in a
file.  With file input, it can test either N-bit bitstrings (with N user
specifiable) or (double precision) floating point numbers in the range
[0.0,1.0) (uniform deviates).  It is thus suitable for use in testing
both software RNG's and hardware RNG's.

rand_test returns several things.  First of all, if the dataset or
generator is "random" within its ability to measure, it says so; if it
is definitely NON-random it also says so; in both cases it also tries to
estimate whether or not you should believe it and what to do to be more
certain.  Second, it generates a test result file with cumulated scores
on each test and explains WHY it passes or fails.  Finally, for its
embedded tests, rand_test ALSO tests the relative speed of the generator
and returns it as "megarands per second".

rand_test encapsulates basically all the random number tests I have been
able to find -- George Marsaglia's "diehard" battery of tests, STS
(v1.5) from NIST FIPS, Knuth's tests, and more.  Check in the man page
or /usr/share documentation for a complete list of the tests and
references where possible.

# This goes into the package libdieharder.
%package -n libdieharder
Summary: A library of random number generator tests and timing routines.
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

%prep
%setup -q -n %{name}

%build
make clean

# We MUST build the library FIRST
%install
make PREFIX=%{buildroot}/usr installlib
make PREFIX=%{buildroot}/usr installprog

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf %{builddir}

%files -n libdieharder

%defattr(-,root,root)

# The libdieharder library
/usr/lib/libdieharder.a
/usr/lib/libdieharder.so.%{version}

# The libdieharder include files are under here
%attr(644,root,root) /usr/include/dieharder

# The libdieharder man page
%attr(644,root,root) /usr/share/man/man3/libdieharder.3.gz

# The dieharder docs.
%doc Copyright README COPYING NOTES manual/dieharder.pdf

%files -n dieharder-ui
%defattr(-,root,root)

# The dieharder binary
%attr(755,root,root) /usr/bin/dieharder

# The dieharder man page
%attr(644,root,root) /usr/share/man/man1/dieharder.1.gz

%post -n libdieharder

cd /usr/lib
ln -sf libdieharder.so.%{version} libdieharder.so 
ldconfig -n .

%postun -n libdieharder

rm -f /usr/lib/libdieharder.so
ldconfig

%changelog
* Tue Nov  11 2004 Robert G. Brown <rgb@duke.edu>
- Releasing v 0.4.0 beta -- first public release.
