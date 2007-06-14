# 
# $Id: Makefile 27 2007-01-26 14:34:43Z rgb $
#
#========================================================================
# THIS IS A "GENERIC" MAKEFILE FOR C PROGRAMS
#
# make          alone should build the application.
#
# make tar      makes $(TAR) from >>all<< the sources
# make tgz      makes $(TGZ) from >>all<< the sources
# make rpm      makes $(RPM). This is experimental!  See below.
# make svn      does a svn commit and creates the timestamp $(SVNTIME)
# make sync     does a svn commit and syncs to list of svn-tree hosts
# make clean    deletes the application and all object files
# make install  strips and installs application and a man page
# make installweb installs a "standard distribution" set of files on
#               your personal website with rsync.
# make printout prints out all source and include files
#========================================================================
# This should be the name of the binary produced by this package.  The
# directory may or may not be the same:
PROGRAM = operm
# DIR = $(PROGRAM)
DIR = dieharder

# The destination on a remote webserver, used as:
#    $(HOME)/public_html/$(WDIR)
# on that server, and the name of an ssh-accessible login server
# that contains the public_html path.
WLOGIN = login.phy.duke.edu
WDIR = General

# These are needed for the subversion targets
SVNTREE = $(HOME)/Src/svn-tree
SVNPATH = $(SVNTREE)/$(DIR)
SVNTIME = $(DIR:=.svn.time)

# This is revision information that MUST be set here.  It is minimally
# used to set variables in an accompanying spec file (see template in
# this directory) and/or in defines passed to the application so that
# it knows its own version information.
VERSION_MAJOR=2
VERSION_MINOR=0.0
RELEASE=1

#========================================================================
# Define all sources.  We ALWAYS have $(SOURCE) derived from $(PROGRAM)
# and $(INCLUDE) derived from $(PROGRAM) and will usually have parsecl.c
# and other modules.
#========================================================================
SOURCE = $(PROGRAM:=.c)

SOURCES = $(SOURCE) \
  parsecl.c \
  operm_work.c \
  # Add more/new sources to list above this line...

INCLUDE = $(PROGRAM:=.h)

INCLUDES = $(INCLUDE) \
  # Add more/new includes to list above this line.  This makefile
  # doesn't support horribly complex includes and dependencies.


#========================================================================
# These defines are VERY USEFUL for versioning.  Don't mess with
# RELEASE -- it is used in rpm builds on remote sites to measure
# deviation from your major/minor release number, so for you it should
# always be 1 or unset.
#
# Note that by default the web install installs the current snapshot of
# the rpm's, with NO MAJOR/MINOR RELEASE NUMBERS.  You can easily alter 
# this and install the full major/minor labelled rpm (you do make it
# anyway) but you'll then have to work a bit harder to automagically
# present the rpm's in a sorted, timestamped table.
#========================================================================
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
# to e.g. i586 or i686) anyway?  Then there are alpha and noarch etc.
# Still...
ARCH=`uname -i`
# ARCH=i386

#
# For a root build/install, use
# RPM_TOPDIR=/usr/src/redhat
#
# Otherwise, add the following:
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
CFLAGS = -O3 -g $(DEFINES)
# Linker flags
LDFLAGS =
# Libraries (I always include math library)
LIBS = -lgsl -lgslcblas -lm

#========================================================================
# Define standard sources and targets.
#========================================================================
OBJECTS = $(SOURCES:.c=.o)
ABS = $(DIR).abs
PHP = $(DIR).php
TAR = $(DIR).tar
TGZ = $(DIR).tgz
RPM = $(DIR)-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).i386.rpm
SRPM = $(DIR)-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).src.rpm
SPEC = $(DIR:=.spec)

#========================================================================
# List of variants one can make.  all is the default.  We always
# presume the simplest of dependencies and remake if includes change
# for example.
#========================================================================
all: $(PROGRAM)

$(RPM): rpm $(TGZ)
$(TGZ): tgz

$(PROGRAM): $(OBJECTS) $(INCLUDES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LIBS) $(OBJECTS)

#========================================================================
# Build targets (from commands)
#========================================================================
tar:	$(SOURCES)
	rm -f core $(PROGRAM) $(OBJECTS) $(PROGRAM).1.gz
	tar -cvpf $(TAR) \
           --exclude=$(DIR)/$(TAR) \
           --exclude=$(DIR)/$(TGZ) \
           --exclude=$(DIR)/$(RPM) \
           --exclude=$(DIR)/CRUFT \
           -C .. $(DIR)

#========================================================================
# This is a complicated target, but VERY USEFUL.  It makes a .tgz tarball
# of the exact form preferred for an RPM.  Eventually I'll add a sed script
# that automatically fixes the accompanying rpm spec file to correspond
# right before building the tgz to really automate this, so that I can just
# move the spec to SPEC, move the source tgz to SOURCE and do an
# rpm -ba blah.spec.
#========================================================================
tgz:	$(SOURCES)
	# clean up stuff we don't want in the tarball.
	rm -f core $(PROGRAM) $(OBJECTS) $(WRPM) $(WSRPM) \
           $(TGZ) $(TAR) $(RPM) $(SRPM) $(PROGRAM).1.gz
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
            --exclude=.svn --exclude=CRUFT \
            --exclude=$(TAR) \
            --exclude=$(TGZ) \
            --exclude=$(RPM) \
            --exclude=$(SRPM) \
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
	rpmbuild -ba $(RPM_TOPDIR)/SPECS/$(SPEC)
	cp $(RPM_TOPDIR)/RPMS/i386/$(RPM) .
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
	rsync -avz --delete $(SVNPATH) login.phy.duke.edu:/home/einstein/prof/rgb/Src/svn-tree
	rsync -avz --delete $(SVNPATH) 209.42.212.5:$(SVNTREE)
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
	$(LPR) $(SOURCES) $(INCLUDES) $(ABS)

#========================================================================
#  A standard cleanup target
#========================================================================
clean:
	rm -f core $(PROGRAM) $(OBJECTS) $(PROGRAM).1.gz

#========================================================================
# Generic Rule to install.  Note that I presume that ALL applications
# have a man page for documentation!  They'd better!
#========================================================================
install :
	(strip $(PROGRAM);\
	install -d $(PREFIX)/bin; \
	install -d $(PREFIX)/share/man/man1; \
	install -m 755 $(PROGRAM) $(PREFIX)/bin; \
	gzip -c $(PROGRAM).1 > $(PROGRAM).1.gz; \
	install -m 644 $(PROGRAM).1.gz $(PREFIX)/share/man/man1)

installweb : $(TGZ) $(RPM) $(SRPM)
	(mkdir $(DIR);\
	rsync -avz $(DIR) $(WLOGIN):public_html/$(WDIR)/; \
	rsync -avz $(TGZ) $(WLOGIN):public_html/$(WDIR)/$(DIR)/; \
	rsync -avz $(RPM) $(WLOGIN):public_html/$(WDIR)/$(DIR)/; \
	rsync -avz $(SRPM) $(WLOGIN):public_html/$(WDIR)/$(DIR)/; \
	rsync -avz $(ABS) $(WLOGIN):public_html/$(WDIR)/$(DIR)/; \
	rsync -avz $(PHP) $(WLOGIN):public_html/$(WDIR)/; \
	rmdir $(DIR))

#========================================================================
# We give all generic rules below.  Currently we only need a rule for 
# objects.
#========================================================================
%.o:%.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $<

