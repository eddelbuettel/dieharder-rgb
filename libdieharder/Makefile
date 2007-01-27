# 
# $Id: Makefile 243 2006-10-05 13:05:47Z rgb $
#
#========================================================================
# THIS IS A "GENERIC" MAKEFILE FOR C PROGRAMS
#
# make          alone should build the application.
#
# make tar      makes $(TAR) from >>all<< the sources
# make tgz      makes $(TGZ) from >>all<< the sources
# make rpm      makes $(RPM). This is experimental!  See below.
# make svn      does a svn commit and creates the timestamp $(SVN)
# make sync     does a svn commit and runs syncsvn to list of
#               SVNROOT hosts
# make clean    deletes the application and all object files
# make install  strips and installs application and a man page
# make printout prints out all source and include files
# 
#========================================================================
# 
# This should be the name of the binary produced by this package.
DIR = libdieharder

SVNTREE = $(HOME)/Src/svn-tree
SVNPATH = $(SVNTREE)/$(DIR)
SVNTIME = $(DIR:=.svn.time)

# This is the rsync target (web) yum repository in two pieces (both
# needed).
YUMPATH = $(HOME)/Src/yumrepo
SSHHOST = login.phy.duke.edu
WDIR = public_html/Beowulf
YDIR = wulfware

# This is the library from which both wulfstat and wulflogger (and maybe
# other stuff in the future) is built.
PROGLIB = libdieharder.a
PROGLIB_SONAME = libdieharder.so.$(VERSION_MAJOR)
PROGLIB_NAME = libdieharder.so.$(VERSION_MAJOR).$(VERSION_MINOR)
PROGLIB_DOC = doc/dieharder.pdf

# This is revision information that MUST be set here.  It is minimally
# used to set variables in an accompanying spec file (see template in
# this directory) and/or in defines passed to the application so that
# it knows its own version information.
VERSION_MAJOR=1
VERSION_MINOR=4.24
RELEASE=1

#========================================================================
# Define all sources.  We ALWAYS have $(SOURCE) derived from $(PROGRAM)
# and $(INCLUDE) derived from $(PROGRAM) and will usually have parsecl.c
# and other modules.
#========================================================================
LIBSOURCES = $(shell ls *.c  2>&1 | sed -e "/\/bin\/ls:/d")
LIBINCLUDES = $(LIBSOURCES.c=.h)
LIBOBJECTS = $(LIBSOURCES:.c=.o)
DEFINES = -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) \
          -DRELEASE=$(RELEASE)

#========================================================================
# Tree point to install (Choose one or set your own)
#========================================================================
# RPM's into /usr, please!  (Red Hat standard rule)
PREFIX=/usr
# Alternatives for public tarball build or private copy
# PREFIX=/usr/local
# PREFIX=$(HOME)

#========================================================================
# AMAZING!  INCREDIBLE!  BUILT IN "make rpm" SUPPORT!
# So much to break, but so much mindless work saved.  Oh, well.  Be
# patient and make it work; it is worth it.  For this to work one
# OBVIOUSLY needs write permission to RPM_TOPDIR and it needs to either
# be the default /usr/src/redhat or whatever is defined by the following
# macro in your ~/.rpmmacros (without the "#", obviously).  All the
# standard subdirectories [BUILD,SOURCES,SRPMS,RPMS,SPEC...etc] need to
# exist in RPM_TOPDIR as well.
#
# This defines your local architecture, also the place the RPM is left
# after a build in RPM_TOPDIR/RPMS.  Probably should set this from
# whereever rpm sets it.  Who knows why/when it builds for i386 (as opposed
# to e.g. i586 or i686) anyway?  Then there are alpha and noarch etc....
ARCH=i386
#
# For a root build/install, use
# RPM_TOPDIR=/usr/src/redhat
#
# Add the following:
# %_topdir	/home/rgb/Src/redhat
# to your personal $(HOME)/.rpmmacros after building
# yourself a private copy of the /usr/src/redhat directory structure.
# Change the "rgb" to your OWN home directory and "Src" to your source
# directory, of course.
RPM_TOPDIR=$(HOME)/Src/rpm_tree
#========================================================================

#========================================================================
# Define parameters and directives needed in compile/link steps.  We
# presume Gnu, optimized, ANSI C and the math link library.  -g -p is
# always set as we strip before installing below.  We always
# pass a definition of VERSION and RELEASE in the event that it is 
# needed in the program.  Adjust as needed.
#========================================================================
# C Compiler
CC = gcc
# Compile flags (use fairly standard -O3 as default)
CFLAGS = -O3 $(DEFINES) -I ./include
# Or use the following for profiling as well as debugging.
# CFLAGS = -O3 -ansi -g -p $(DEFINES)
# Linker flags
LDFLAGS = --shared -Wl,-soname,$(PROGLIB_SONAME)
# Libraries (I always include math library)
LIBS = -lgsl -lgslcblas -lm

#========================================================================
# Define standard sources and targets.
#========================================================================
TAR = $(DIR).tar
ABS = $(DIR).abs
PHP = $(DIR).php
TGZ = $(DIR).tgz
RPM = $(DIR)-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).i386.rpm
SRPM = $(DIR)-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).src.rpm
WRPM = $(DIR).i386.rpm
WSRPM = $(DIR).src.rpm
SPEC = $(DIR:=.spec)
SVN = $(DIR:=.svn.time)

#========================================================================
# List of variants one can make.  all is the default.  We always
# presume the simplest of dependencies and remake if includes change
# for example.
#========================================================================
all: $(PROGLIB) $(PROGLIB_NAME) $(PROGLIB_DOC)

$(PROGLIB): $(LIBOBJECTS) $(LIBINCLUDES)
	ar r $(PROGLIB) $(LIBOBJECTS)
	ranlib $(PROGLIB)

$(PROGLIB_NAME): $(LIBOBJECTS) $(LIBINCLUDES)
	gcc $(LDFLAGS) -o $(PROGLIB_NAME) $(LIBOBJECTS)

$(PROGLIB_DOC): doc/dieharder.tex doc/Makefile
	( cd doc; \
	make pdf )
	

$(RPM): rpm
$(WRPM): rpm
$(TGZ): tgz

#========================================================================
# Build targets (from commands)
#========================================================================
tar:	$(SOURCES) $(SCSOURCES)
	rm -f core $(PROGRAM) $(PROGRAM2) $(OBJECTS) $(OBJECTS2) \
           $(PROGRAM).1.gz
	tar -cvpf $(TAR) \
           --exclude=$(DIR)/$(TAR) \
           --exclude=$(DIR)/$(TGZ) \
           --exclude=$(DIR)/$(RPM) \
           -C .. $(DIR)

#========================================================================
# This is a complicated target, but VERY USEFUL.  It makes a .tgz tarball
# of the exact form preferred for an RPM.  Eventually I'll add a sed script
# that automatically fixes the accompanying rpm spec file to correspond
# right before building the tgz to really automate this, so that I can just
# move the spec to SPEC, move the source tgz to SOURCE and do an
# rpm -ba blah.spec.
#========================================================================
tgz:	$(SOURCES) $(SCSOURCES)
	rm -f core $(PROGLIB) $(LIBOBJECTS) \
           $(TGZ) $(TAR) $(PROGRAM).1.gz
	rm -f .$(DIR)
	# Update the specfile here, because we want the packed one to be 
	# in sync with the VERSION/RELEASE defined ABOVE (only)!
	# Might need another line or two here, e.g. %Source:
	cat $(SPEC) | \
	sed -e 's/^\(Version:\) \(.*\)/\1 $(VERSION_MAJOR).$(VERSION_MINOR)/' \
	    -e 's/^\(Release:\) \(.*\)/\1 $(RELEASE)/' >> $(SPEC).$$
	mv $(SPEC).$$ $(SPEC)
	cat $(ABS) | \
	sed -e 's/^<H2>\(.*\)<\/H2>/<H2>dieharder Version $(VERSION_MAJOR).$(VERSION_MINOR)<\/H2>/' >> $(ABS).$$
	mv $(ABS).$$ $(ABS)
	mkdir -p .$(DIR)
	cp -a * .$(DIR)
	mv .$(DIR) $(DIR)
	# Exclude any cruft/development directories and SVN stuff.  Add
	# lines as needed.
	tar -cvpf $(TAR) \
            --exclude=SVN --exclude=CRUFT \
            --exclude=*.tar \
            --exclude=*.tgz \
            --exclude=*rpm \
            --exclude=*dat.long \
            --exclude=*.raw \
            --exclude=*.bin \
            --exclude=Restricted \
            ./$(DIR)
	gzip $(TAR)
	mv $(TAR).gz $(TGZ)
	rm -rf $(DIR)

#========================================================================
# This is an EVEN MORE USEFUL target, but take a moment to understand it.  
# We take the .tgz script above, fix the revision information in the 
# (presumed existing) $(PROGRAM).spec file, copy them both to (note 
# CAREFULLY!) a presumed environmentally defined $(RPM_TOPDIR)/[SOURCE,SPEC], 
# do a build, and (Inshallah!) retrieve the results from 
# $(RPM_TOPDIR)/RPMS/i386.  Works for me...  Note well you should have
# set RPM_TOPDIR and ARCH according to the instructions above.
# 
#========================================================================
rpm:	Makefile $(SPEC) $(SOURCES) $(TGZ)
	make tgz
	cp $(TGZ) $(RPM_TOPDIR)/SOURCES
	cp $(SPEC) $(RPM_TOPDIR)/SPECS
	rpmbuild -ba $(RPM_TOPDIR)/SPECS/$(SPEC)
	cp $(RPM_TOPDIR)/RPMS/$(ARCH)/$(RPM) .
	cp $(RPM_TOPDIR)/SRPMS/$(SRPM) .

svn:
	echo "New Checkin `date`" >> $(SVNTIME)	# Will force a commit and increment revision
	svn commit .
	cat $(SVNTIME) | \
	sed -e '/^New Checkin/d' >> $(SVNTIME).tmp
	mv $(SVNTIME).tmp $(SVNTIME)

sync:
	echo "New Checkin `date`" >> $(SVNTIME)	# Will force a commit and increment revision
	svn commit .		# Do the commit
	rsync -avz --delete $(SVNPATH) 209.42.212.5:$(SVNTREE)
	rsync -avz --delete $(SVNPATH) login.phy.duke.edu:/home/einstein/prof/rgb/Src/svn-tree
	cat $(SVNTIME) | \
	sed -e '/^New Checkin/d' >> $(SVNTIME).tmp
	mv $(SVNTIME).tmp $(SVNTIME)

#========================================================================
# printout makes an enscript -2r printout of SOURCES and
# and INCLUDES.  Use lpr if you don't have enscript
#========================================================================
LPR = enscript -2r
# LPR = lpr
printout:
	$(LPR) $(SOURCES) $(INCLUDES) $(SOURCES2) $(INCLUDES2)

#========================================================================
#  A standard cleanup target
#========================================================================
clean:
	rm -f core $(PROGLIB) $(LIBOBJECTS) $(PROGLIB).1.gz

#========================================================================
# install
#	ln -sf $(PREFIX)/lib/$(PROGLIB_NAME) $(PREFIX)/lib/$(PROGLIB_SONAME); \
#========================================================================
install :
	(install -d $(PREFIX)/share/man/man3; \
	install -d $(PREFIX)/lib; \
	install -m 755 $(PROGLIB) $(PREFIX)/lib; \
	install -m 755 $(PROGLIB_NAME) $(PREFIX)/lib; \
	install -d $(PREFIX)/include; \
	install -d $(PREFIX)/include/dieharder; \
	install -m 644 include/dieharder/*.h $(PREFIX)/include/dieharder; \
	gzip -c $(DIR).3 > $(DIR).3.gz; \
	install -m 644 $(DIR).3.gz $(PREFIX)/share/man/man3 )

#========================================================================
# We give all generic rules below.  Currently we only need a rule for 
# objects.
#========================================================================
%.o:%.c include/dieharder/libdieharder.h Makefile
	$(CC) -c $(CFLAGS) $<

# dependencies:
rgb_timing.o: include/dieharder/rgb_timing.h
rgb_persist.o: include/dieharder/rgb_persist.h
rgb_bitdist.o: include/dieharder/rgb_bitdist.h
diehard_birthdays.o: include/dieharder/diehard_birthdays.h
diehard_operm5.o: include/dieharder/diehard_operm5.h
diehard_.o: include/dieharder/diehard_operm5.h
diehard_rank_32x32.o: include/dieharder/diehard_rank_32x32.h
diehard_rank_6x8.o: include/dieharder/diehard_rank_6x8.h
diehard_bitstream.o: include/dieharder/diehard_bitstream.h
diehard_opso.o: include/dieharder/diehard_opso.h
diehard_oqso.o: include/dieharder/diehard_oqso.h
diehard_dna.o: include/dieharder/diehard_dna.h
diehard_count_1s_stream.o: include/dieharder/diehard_count_1s_stream.h
diehard_count_1s_byte.o: include/dieharder/diehard_count_1s_byte.h
diehard_parking_lot.o: include/dieharder/diehard_parking_lot.h
diehard_2dsphere.o: include/dieharder/diehard_2dsphere.h
diehard_3dsphere.o: include/dieharder/diehard_3dsphere.h
diehard_squeeze.o: include/dieharder/diehard_squeeze.h
diehard_sums.o: include/dieharder/diehard_sums.h
diehard_runs.o: include/dieharder/diehard_runs.h
diehard_craps.o: include/dieharder/diehard_craps.h
marsaglia_tsang_gcd.o: include/dieharder/marsaglia_tsang_gcd.h
sts_monobit.o: include/dieharder/sts_monobit.h
sts_runs.o: include/dieharder/sts_runs.h
