# 
# $Id$
#
#========================================================================
# THIS IS A "GENERIC" MAKEFILE FOR C PROGRAMS
#
# make          alone should build the application.
#
# make tar      makes $(TAR) from >>all<< the sources
# make tgz      makes $(TGZ) from >>all<< the sources
# make rpm      makes $(RPM). This is experimental!  See below.
# make cvs      does a cvs commit and creates the timestamp $(CVS)
# make sync     does a cvs commit and runs synccvs to list of
#               CVSROOT hosts
# make clean    deletes the application and all object files
# make install  strips and installs application and a man page
# make printout prints out all source and include files
# 
#========================================================================
# 
# This should be the name of the binary produced by this package.
PROGRAM = rand_rate
# This is the name of the toplevel directory, the CVS repository
# and goes into e.g. the tarball names.  It will often (but not always)
# be the name of the main binary.
DIR = $(PROGRAM)

# Secondary/test binary programs next
# PROGRAM2 = project2

# This is revision information that MUST be set here.  It is minimally
# used to set variables in an accompanying spec file (see template in
# this directory) and/or in defines passed to the application so that
# it knows its own version information.
VERSION_MAJOR=0
VERSION_MINOR=3.1
RELEASE=1

#========================================================================
# Define all sources.  We ALWAYS have $(SOURCE) derived from $(PROGRAM)
# and $(INCLUDE) derived from $(PROGRAM) and will usually have parsecl.c
# and other modules.
#========================================================================
SOURCE = $(PROGRAM:=.c)
SOURCES = $(SOURCE)\
    add_my_types.c \
    dev_random.c \
    dev_urandom.c \
    empty_random.c \
    list_rand.c \
    list_rngs.c \
    measure_rate.c \
    parsecl.c \
    prob.c \
    rand_rate_seed.c \
    rand_rate_startup.c \
    rand_rate_work.c \
    rand_rate_utils.c \
    rgb_binomial.c \
    rgb_bitdist.c \
    rgb_persist.c \
    sts_monobit.c \
    sts_runs.c \
    timing.c \

# SOURCE2 = $(PROGRAM2:=.c)
# SOURCES2 = $(SOURCE2) project2_work.c

INCLUDE = $(PROGRAM:=.h)
INCLUDES = $(INCLUDE)

# INCLUDE2 = $(PROGRAM2:=.h)
# INCLUDES2 = $(INCLUDE2)

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
# Compile flags
CFLAGS = -O3 -ansi -g $(DEFINES)
# Or use the following for profiling as well as debugging.
# CFLAGS = -O3 -ansi -g -p $(DEFINES)
# Linker flags
LDFLAGS =
# Libraries (I always include math library)
LIBS = -lgsl -lgslcblas -lm

#========================================================================
# Define standard sources and targets.
#========================================================================
OBJECTS = $(SOURCES:.c=.o)
# OBJECTS2 = $(SOURCES2:.c=.o)
TAR = $(DIR).tar
TGZ = $(DIR).tgz
RPM = $(DIR)-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).i386.rpm
SRPM = $(DIR)-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).src.rpm
SPEC = $(DIR:=.spec)
CVS = $(DIR:=.cvs.time)

#========================================================================
# List of variants one can make.  all is the default.  We always
# presume the simplest of dependencies and remake if includes change
# for example.
#========================================================================
all: $(PROGRAM) # $(PROGRAM2)

$(PROGRAM): $(OBJECTS) $(INCLUDES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LIBS) $(OBJECTS)

# $(PROGRAM2): $(OBJECTS2) $(INCLUDES2)
# 	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LIBS) $(OBJECTS2)


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
	rm -f core $(PROGRAM) $(PROGRAM2) $(OBJECTS) $(OBJECTS2) \
           $(TGZ) $(TAR) $(PROGRAM).1.gz
	rm -f .$(DIR)
	# Update the specfile here, because we want the packed one to be 
	# in sync with the VERSION/RELEASE defined ABOVE (only)!
	# Might need another line or two here, e.g. %Source:
	cat $(SPEC) | \
        sed -e 's/^\(%define version\) \(.*\)/\1 $(VERSION_MAJOR).$(VERSION_MINOR)/' \
            -e 's/^\(%define release\) \(.*\)/\1 $(RELEASE)/' >> $(SPEC).$$
	mv $(SPEC).$$ $(SPEC)
	mkdir -p .$(DIR)
	cp -a * .$(DIR)
	mv .$(DIR) $(DIR)
	# Exclude any cruft/development directories and CVS stuff.  Add
	# lines as needed.
	tar -cvpf $(TAR) \
            --exclude=CVS --exclude=CRUFT \
            --exclude=*.tar \
            --exclude=*.tgz \
            --exclude=*rpm \
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
rpm:	Makefile $(SPEC) $(SOURCES) $(SOURCES2) $(TGZ)
	cp $(TGZ) $(RPM_TOPDIR)/SOURCES
	cp $(SPEC) $(RPM_TOPDIR)/SPECS
	rpm -ba  $(RPM_TOPDIR)/SPECS/$(SPEC)
	cp $(RPM_TOPDIR)/RPMS/i386/$(RPM) .
	cp $(RPM_TOPDIR)/SRPMS/$(SRPM) .

#========================================================================
# It will help greatly if $(CVS) exists, contains and RCS label and
# any comments, and is added to the CVS distribution, of course.
# The timestamp of for your own benefit -- an instant way to check
# which of several working directories is really currentest.
#========================================================================
cvs:
	cat $(CVS) | \
	sed -e '/^New Checkin/d' >> $(CVS).tmp
	mv $(CVS).tmp $(CVS)
	echo "New Checkin `date`" >> $(CVS)	# Will force a commit and increment revision
	cvs commit .		# doesn't sync (yet).

#========================================================================
# This is a poor man's solution to both backup and having several
# development "homes" each with their own CVSROOT (as I do -- one at
# literally my home, one at Duke, and one on my laptop that needs to
# be sync'd up before and after trips.  Just add one line per host
# to be kept identical.  Be warned that if a host on the list is down,
# this will hang boring while waiting for a timeout.  It presumes that
# you have rsync (and that you use ssh) and that you have installed
# the "synccvs" script:
#
#!/bin/sh
# CVS_PKG=$1;CVS_HOST=$2
# rsync -avz --delete --rsh=/usr/bin/ssh $CVSROOT/$CVS_PKG $CVS_HOST:\$CVSROOT
#
# on your path.  There's probably some way to protect the latter CVSROOT
# from early interpretation in a Makefile, but I cannot find it.
#========================================================================
sync:
	cat $(CVS) | \
	sed -e '/^New Checkin/d' >> $(CVS).tmp
	mv $(CVS).tmp $(CVS)
	echo "New Checkin `date`" >> $(CVS)	# Will force a commit and increment revision
	cvs commit .		# Do the commit
	synccvs $(DIR) ganesh.phy.duke.edu
	synccvs $(DIR) rgb.adsl.duke.edu


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
	rm -f core $(PROGRAM) $(OBJECTS) $(PROGRAM2) $(OBJECTS2) $(PROGRAM).1.gz

#========================================================================
# Generic Rule to install.  Note that I presume that ALL applications
# have a man page for documentation!  They'd better!  Note that I'm using
# the (Grrr) stupid FHS specification for putting man pages in
# /usr/share/man.  What a crock.
#========================================================================
install :
	(strip $(PROGRAM);\
	install -d $(PREFIX)/bin; \
	install -d $(PREFIX)/share/man/man1; \
	install -m 755 $(PROGRAM) $(PREFIX)/bin; \
	gzip -c $(PROGRAM).1 > $(PROGRAM).1.gz; \
	install -m 644 $(PROGRAM).1.gz $(PREFIX)/share/man/man1)

# Add these for second program...
#	install -m 755 $(PROGRAM2) $(PREFIX)/bin; \
#	gzip -c $(PROGRAM2).1 > $(PROGRAM2).1.gz; \
#	install -m 644 $(PROGRAM2).1.gz $(PREFIX2)/share/man/man1; \


#========================================================================
# We give all generic rules below.  Currently we only need a rule for 
# objects.
#========================================================================
%.o:%.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $<

