# The following should match PROGRAM, VERSION and RELEASE in the
# Makefile accompanying this program (and the .tgz defined in Source
# below.

Summary: rand_rate is a random number generator tester and timer

Name: rand_rate
Version: 0.0.1
Release: 1
Group: Development/Tools
Copyright: Open Source (GPL v2b)
Source: %{name}.tgz
Buildroot: /var/tmp/%{name}-buildroot

%description 

rand_rate is a fairly involved random number/uniform deviate generator
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
%defattr(-,root,root)
# The xmlsysd binary
%attr(755,root,root) /usr/sbin/xmlsysd
# The xmlsysd man page
%attr(644,root,root) /usr/share/man/man8/xmlsysd.8.gz

# The xmlsysd docs
%doc README TODO COPYING CHANGES DESIGN

%post

# We now will REQUIRE xinetd.  If it isn't found we will bitch and
# install anyway, but not ready to run.
if [ -f /etc/xinetd.conf ]
then

  XINETD_CONF=/etc/xinetd.d/xmlsysd
  # This is an xinetd system.  Good.

  # If old xmlsysd config file exists, leave it but install
  # the new one as xmlsysd.rpmnew
  if [ -f /etc/xinetd.d/xmlsysd ]
  then
     echo "Warning:  /etc/xinetd.d/xmlsysd exists.  Installing /etc/xinetd.xmlsysd.rpmnew"
     XINETD_CONF=/etc/xinetd.d/xmlsysd.rpmnew
  fi
cat > $XINETD_CONF << EOF
# default: off
# description:  The xmlsysd daemon, to be managed by xinetd.
# Uncomment the "only_from" line below and edit it to permit access from
# only private internal network hosts and/or specific networks.  Or leave
# it as is and control via ipchains or iptables.  The default is to 
# permit connections from any host, which may not be right for your site.
service xmlsysd
{
	disable			= yes
	socket_type		= stream
	protocol		= tcp
	wait			= no
	user			= nobody
	group			= nobody
	server			= /usr/sbin/xmlsysd
	server_args		= -i 7887
	port			= 7887
	instances		= 10
#	only_from		= 10.0.0.0,172.16.0,0,192.168.0.0
}
EOF
  if [ -x /etc/rc.d/init.d/xinetd ]
  then
    /etc/rc.d/init.d/xinetd restart
  else
    killall -USR1 xinetd
  fi

else

  # If there is no xinetd, just tell them what to do.
  cat << EOF
Help!  

I cannot figure out how to install xmlsysd "ready to run" on your system
because you don't seem to have /etc/xinetd.conf.  If you want to run
xmlsysd as a forking daemon (which is fine but which has no intrinsic
access control) you might want to add a script starting it on whatever
port you desire to e.g. /etc/rc.d/init.d/xmlsysd.

I'd recommend using ipchains, iptables, or tcpd to control access if you
do this, although xmlsysd is possibly harmless even if granted
promiscuous access.  Or you can rpm --erase xmlsysd, install xinetd, and
reinstall xmlsysd and it should find and install itself as an xinetd
daemon the second time around.

At this point a reasonable amount of documentation should be available
in the man page (man xmlsyd) and in /usr/share/doc/xmlsysd*.  If you
have problems that the documentation doesn't resolve, I'm generally
available at:

   Robert G. Brown <rgb@phy.duke.edu>

Good Luck.

EOF

fi

exit

%postun

%changelog
* Wed Apr  29 2002 Robert G. Brown <rgb@duke.edu>
- Releasing v 0.1.0 beta -- this has now been "stable" for weeks.
* Wed Mar  13 2002 Robert G. Brown <rgb@duke.edu>
- set up and built for RH 7.2
