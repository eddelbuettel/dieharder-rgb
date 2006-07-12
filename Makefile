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
PROGRAM = dieharder
DIR = $(PROGRAM)

SVNTREE = $(HOME)/Src/svn-tree
SVNPATH = $(SVNTREE)/$(DIR)
SVNTIME = $(DIR:=.svn.time)

# Secondary/test binary programs next
# PROGRAM2 = project2

# This is revision information that MUST be set here.  It is minimally
# used to set variables in an accompanying spec file (see template in
# this directory) and/or in defines passed to the application so that
# it knows its own version information.
VERSION_MAJOR=1
VERSION_MINOR=0.21
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
    file_input.c \
    file_input_raw.c \
    list_rand.c \
    list_rngs.c \
    measure_rate.c \
    output_rnds.c \
    parse.c \
    parsecl.c \
    help.c \
    prob.c \
    timing.c \
    tensor.c \
    matrix.c \
    block.c \
    Btest.c \
    Ntest.c \
    Xtest.c \
    sample.c \
    rank.c \
    histogram.c \
    confidence.c \
    chisq.c \
    kstest.c \
    random_seed.c \
    startup.c \
    work.c \
    bits.c \
    diehard_runs.c \
    diehard_birthdays.c \
    diehard_2dsphere.c \
    diehard_3dsphere.c \
    diehard_rank_32x32.c \
    diehard_rank_6x8.c \
    diehard_parking_lot.c \
    diehard_craps.c \
    diehard_squeeze.c \
    diehard_bitstream.c \
    diehard_opso.c \
    diehard_oqso.c \
    diehard_dna.c \
    diehard_count_1s_stream.c \
    diehard_count_1s_byte.c \
    diehard_operm5.c \
    rgb_bitdist.c \
    rgb_persist.c \
    rgb_timing.c \
    sts_monobit.c \
    sts_runs.c \


INCLUDE = $(PROGRAM:=.h)
INCLUDES = $(INCLUDE)


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
CFLAGS = -O3 $(DEFINES)
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
all: $(PROGRAM)
$(RPM): tgz rpm
$(TGZ): tgz

$(PROGRAM): $(OBJECTS) $(INCLUDES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LIBS) $(OBJECTS)

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
	rpmbuild -ba  $(RPM_TOPDIR)/SPECS/$(SPEC)
	cp $(RPM_TOPDIR)/RPMS/i386/$(RPM) .
	cp $(RPM_TOPDIR)/RPMS/i386/$(RPM) $(WRPM)
	cp $(RPM_TOPDIR)/SRPMS/$(SRPM) .
	cp $(RPM_TOPDIR)/SRPMS/$(SRPM) $(WSRPM)

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
	$(LPR) $(SOURCES) $(INCLUDES) $(SOURCES2) $(INCLUDES2)

#========================================================================
#  A standard cleanup target
#========================================================================
clean:
	rm -f core $(PROGRAM) $(OBJECTS) $(PROGRAM).1.gz

#========================================================================
# Generic Rule to install.  Note that I presume that ALL applications
# have a man page for documentation!  They'd better!
#========================================================================
install : $(PROGRAM)
	(strip $(PROGRAM);\
	install -d $(PREFIX)/bin; \
	install -d $(PREFIX)/share/man/man1; \
	install -m 755 $(PROGRAM) $(PREFIX)/bin; \
	gzip -c $(PROGRAM).1 > $(PROGRAM).1.gz; \
	install -m 644 $(PROGRAM).1.gz $(PREFIX)/share/man/man1)

installweb : $(TGZ) $(RPM) $(SRPM)
	(mkdir $(DIR);\
	rsync -avz $(DIR) login.phy.duke.edu:public_html/General/; \
	rsync -avz $(TGZ) login.phy.duke.edu:public_html/General/$(DIR)/; \
	rsync -avz $(RPM) login.phy.duke.edu:public_html/General/$(DIR)/; \
	rsync -avz $(SRPM) login.phy.duke.edu:public_html/General/$(DIR)/; \
	rsync -avz $(ABS) login.phy.duke.edu:public_html/General/$(DIR)/; \
	rsync -avz $(PHP) login.phy.duke.edu:public_html/General/; \
	rmdir $(DIR))

#========================================================================
# We give all generic rules below.  Currently we only need a rule for 
# objects.
#========================================================================
%.o:%.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $<

