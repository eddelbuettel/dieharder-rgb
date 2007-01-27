#========================================================================
# This is the toplevel Makefile for the dieharder project.  It has
# some very specialized targets:
#
# make          alone should build the entire application
# make install  should build and install the entire application
#
# make tgz      makes $(TGZ) of entire tree for standalone or rpm build
# make rpm      makes $(RPM) packages built by dieharder.spec
# make svn      does a svn commit and creates the timestamp $(SVNTIME)
# make sync     does a svn commit and syncs to list of svn-tree hosts
#
# make clean    cleans the source directories
#========================================================================
PROJECT = dieharder
PROGRAM = dieharder
LIBRARY = libdieharder
PROGSRC = dieharder_src

#========================================================================
# This is essential.  The rpmbuild overrides it, but we have to make
# build "work" when executed only in the source tree directory itself.
# This isn't easy, since the dependences are more than a bit scattered.
# We therefore point to the directory one level up, where we should
# find a ./lib, ./include, ./share and ./bin directory tree from which
# the various dieharder files will actually be assembled into an rpm
# with PREFIX=/usr (for example).
#========================================================================
PREFIX=..

SVNTREE = $(HOME)/Src/svn-tree
SVNPATH = $(SVNTREE)/$(PROJECT)
SVNTIME = $(PROJECT:=.svn.time)

#========================================================================
# This is revision information that MUST be set here and ONLY here.  
# It will automagically set the related information in $(SPEC) and the
# subsidiary Makefiles in the source subdirectories.
#========================================================================
VERSION_MAJOR=2
VERSION_MINOR=4.24
RELEASE=3

ABS = $(PROJECT).abs
PHP = $(PROJECT).php

# RPM/tarball target objects.  We need rules for all of these.
TGZ = $(PROJECT).tar
TGZ = $(PROJECT).tgz
SPEC = $(PROJECT).spec
# These are the three rpms automagically built by the spec
SRPM = $(PROJECT)-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).src.rpm
PRPM = $(PROGRAM)-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).i386.rpm
LRPM = $(LIBRARY)-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).i386.rpm

#========================================================================
# List of variants one can make.  all is the default.  We always
# presume the simplest of dependencies and remake if includes change
# for example.
#========================================================================
all: $(PROGRAM)

# This is not, actually, a particularly useful toplevel target.  To
# work correctly it alsoo would require a full parsing of all
# lower level dependencies.  I'm leaving it in for the moment just
# to have a default target at the toplevel that CAN be used to test.
$(PROGRAM):
	(cd $(PROGSRC); \
	make; \
	cp $(PROGRAM) ..)

$(RPM): tgz rpm
$(TGZ): tgz

$(SPEC): Makefile
	# Version information is set ONLY in the toplevel Makefile.
	cat $(SPEC) | \
	sed -e 's/^\(Version:\) \(.*\)/\1 $(VERSION_MAJOR).$(VERSION_MINOR)/' \
	    -e 's/^\(Release:\) \(.*\)/\1 $(RELEASE)/' > /tmp/$(SPEC).$$
	mv /tmp/$(SPEC).$$ $(SPEC)
	# While we're at it, update program and library Makefiles
	cat $(LIBRARY)/Makefile | \
	sed -e 's/^\(VERSION_MAJOR=\)\(.*\)/\1$(VERSION_MAJOR)/' \
	    -e 's/^\(VERSION_MINOR=\)\(.*\)/\1$(VERSION_MINOR)/' \
	    -e 's/^\(RELEASE=\)\(.*\)/\1$(RELEASE)/' > /tmp/Makefile.$$
	mv /tmp/Makefile.$$ $(LIBRARY)/Makefile
	cat $(PROGSRC)/Makefile | \
	sed -e 's/^\(VERSION_MAJOR=\)\(.*\)/\1$(VERSION_MAJOR)/' \
	    -e 's/^\(VERSION_MINOR=\)\(.*\)/\1$(VERSION_MINOR)/' \
	    -e 's/^\(RELEASE=\)\(.*\)/\1$(RELEASE)/' > /tmp/Makefile.$$
	mv /tmp/Makefile.$$ $(PROGSRC)/Makefile

$(ABS): Makefile
	cat $(ABS) | \
	sed -e 's/^\(<center><H2>Version \)\(.*\)/\1$(VERSION_MAJOR).$(VERSION_MINOR)<\/H2><\/center>/' > /tmp/$(ABS).$$
	mv /tmp/$(ABS).$$ $(ABS)

#========================================================================
# This is a required target for both its own sake and to support the
# rpm build.  It has to run unconditionally when called.  Note that we
# make PRECISELY what we need in terms of the source directories,
# excluding all restricted material and irrelevant data.
#========================================================================
tgz: $(SPEC) $(ABS)
	(rm -rf $(TGZ) $(PROJECT); \
	mkdir -p $(PROJECT); \
	cd $(PROGSRC); \
	make clean; \
	cd ../$(LIBRARY); \
	make clean; \
	cd ..; \
	cp -r $(PROGSRC) $(PROJECT); \
	cp -r $(LIBRARY) $(PROJECT); \
	cp -r include $(PROJECT); \
	rm -f lib/*; \
	cp -r lib $(PROJECT); \
	cp -r manual $(PROJECT); \
	cp $(SPEC) $(PROJECT); \
	cp $(ABS) $(PROJECT); \
	cp $(PHP) $(PROJECT); \
	cp Makefile $(PROJECT); \
	cp Copyright $(PROJECT); \
	cp COPYING $(PROJECT); \
	cp README $(PROJECT); \
	cp NOTES $(PROJECT); \
	tar -cvpf $(PROJECT).tar \
            --exclude=.svn \
	    --exclude=Cruft \
	    --exclude=Data \
	    --exclude=Exclude \
            --exclude=Results \
            --exclude=*.tar \
            --exclude=*.tgz \
            --exclude=*.rpm \
            --exclude=doc \
            ./$(PROJECT); \
	gzip $(PROJECT).tar; \
	mv $(PROJECT).tar.gz $(TGZ); \
	rm -rf $(PROJECT))

#========================================================================
# rpm target special stuff
#
# To work in userspace, add the following:
# %_topdir	/home/rgb/Src/redhat
# to your personal $(HOME)/.rpmmacros after building
# yourself a private copy of the /usr/src/redhat directory structure.
#
# RPM_TOPDIR=/usr/src/redhat
RPM_TOPDIR=$(HOME)/Src/rpm_tree

# This is needed to get the right library and binary rpm.
BINARCH=`uname -i`
# ARCH=i386
#========================================================================
# One stop shop.  Basically we build this every time, we hope.
rpm:	Makefile $(TGZ)
	cp $(TGZ) $(RPM_TOPDIR)/SOURCES
	cp $(SPEC) $(RPM_TOPDIR)/SPECS
	rpmbuild -ba  $(RPM_TOPDIR)/SPECS/$(SPEC)
	cp $(RPM_TOPDIR)/SRPMS/$(SRPM) .
	cp $(RPM_TOPDIR)/RPMS/i386/$(BRPM) .
	cp $(RPM_TOPDIR)/RPMS/i386/$(LRPM) .

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
#  A standard cleanup target
#========================================================================
clean : 
	- (cd dieharder_src; \
	$(MAKE) clean; \
	cd ../libdieharder; \
	$(MAKE) clean;)

#========================================================================
# We need two toplevel targets that have to be mirrored in $(SPEC).
#========================================================================
installlib:
	(cd $(LIBRARY);\
	make PREFIX=$(PREFIX) install)

installprog:
	(cd $(PROGSRC);\
	make PREFIX=$(PREFIX) install)

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

