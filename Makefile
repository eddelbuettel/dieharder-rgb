#========================================================================
# This is the toplevel Makefile for the dieharder project.  It has
# some very specialized targets:
#
# make          alone should build the entire application
# make install  should build and install the entire application
# make installweb installs to website for download
# make installrepo installs to yum repo
#
# make tgz      makes $(TGZ) of entire tree for standalone or rpm build
# make rpm      makes $(RPM) packages built by dieharder.spec
# make svn      does a svn commit and creates the timestamp $(SVNTIME)
# make sync     does a svn commit and syncs to list of svn-tree hosts
#
# make clean    cleans the source directories
#========================================================================
PROJECT = dieharder
LIBRARY = libdieharder
LIBTIME = libwulf.time
PROGRAM = dieharder
PROGTIME = dieharder.time
MANUAL = manual
MANTIME = manual.time

#========================================================================
# This is essential.  The rpmbuild overrides it, but we have to make
# build "work" when executed only in the source tree directory itself.
# This isn't easy, since the dependences are more than a bit scattered.
# We therefore point to the directory one level up, where we should
# find a ./lib, ./include, ./share and ./bin directory tree from which
# the various dieharder files will actually be assembled into an rpm
# with PREFIX=/usr (for example).
#========================================================================
BUILDROOT=$(PWD)/buildroot
PREFIX=$(BUILDROOT)/usr

# The destination on a remote webserver, used as:
#    $(HOME)/public_html/$(WDIR)
# on that server, and the name of an ssh-accessible login server
# that contains the public_html path.
WLOGIN = login.phy.duke.edu
WDIR = General

SVNTREE = $(HOME)/Src/svn-tree
SVNPATH = $(SVNTREE)/$(PROJECT)
SVNTIME = $(PROJECT:=.svn.time)

#========================================================================
# This is revision information that MUST be set here and ONLY here.  
# It will automagically set the related information in $(SPEC) and the
# subsidiary Makefiles in the source subdirectories.
#========================================================================
VERSION_MAJOR=2
VERSION_MINOR=6.24
RELEASE=1

ABS = $(PROJECT).abs
PHP = $(PROJECT).php

# RPM/tarball target objects.  We need rules for all of these.
PROJECTDIR = $(PROJECT)-$(VERSION_MAJOR).$(VERSION_MINOR)
TAR = $(PROJECTDIR).tar
TGZ = $(PROJECTDIR).tgz
SPEC = $(PROJECT).spec

#========================================================================
# List of variants one can make.  all is the default.  We always
# presume the simplest of dependencies and remake if includes change
# for example.
#========================================================================
all: $(LIBTIME) $(PROGTIME)

# This is not, actually, a particularly useful toplevel target.  To
# work correctly it also would require a full parsing of all
# lower level dependencies.  I'm leaving it in for the moment just
# to have a default target at the toplevel that CAN be used to test.
$(LIBTIME):
	(cd $(LIBRARY); \
	make)

$(PROGTIME):
	(cd $(PROGRAM); \
	make)

$(SPEC): Makefile $(SPEC)
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
	cat $(PROGRAM)/Makefile | \
	sed -e 's/^\(VERSION_MAJOR=\)\(.*\)/\1$(VERSION_MAJOR)/' \
	    -e 's/^\(VERSION_MINOR=\)\(.*\)/\1$(VERSION_MINOR)/' \
	    -e 's/^\(RELEASE=\)\(.*\)/\1$(RELEASE)/' > /tmp/Makefile.$$
	mv /tmp/Makefile.$$ $(PROGRAM)/Makefile
	cat $(MANUAL)/Makefile | \
	sed -e 's/^\(VERSION_MAJOR=\)\(.*\)/\1$(VERSION_MAJOR)/' \
	    -e 's/^\(VERSION_MINOR=\)\(.*\)/\1$(VERSION_MINOR)/' \
	    -e 's/^\(RELEASE=\)\(.*\)/\1$(RELEASE)/' > /tmp/Makefile.$$
	mv /tmp/Makefile.$$ $(MANUAL)/Makefile

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
tgz: Makefile COPYING Copyright NOTES README $(SPEC) $(ABS) $(PHP)
	( rm -rf $(TAR) $(TGZ) $(PROJECTDIR); \
	mkdir -p $(PROJECTDIR); \
	cd $(LIBRARY); \
	make clean; \
	cd ..; \
	cp -r $(LIBRARY) $(PROJECTDIR); \
	cd $(PROGRAM); \
	make clean; \
	cd ..; \
	cp -r $(PROGRAM) $(PROJECTDIR); \
	cd $(MANUAL); \
	make clean; \
	cd ..; \
	cp -r $(MANUAL) $(PROJECTDIR); \
	cp -r include $(PROJECTDIR); \
	cp $(SPEC) $(PROJECTDIR); \
	cp $(ABS) $(PROJECTDIR); \
	cp $(PHP) $(PROJECTDIR); \
	cp Makefile $(PROJECTDIR); \
	cp Copyright $(PROJECTDIR); \
	cp COPYING $(PROJECTDIR); \
	cp README $(PROJECTDIR); \
	cp NOTES $(PROJECTDIR); \
	tar -cvpf $(TAR) \
            --exclude=.svn \
	    --exclude=Cruft \
	    --exclude=Exclude \
            --exclude=*.tar \
            --exclude=*.tgz \
            --exclude=*.rpm \
            ./$(PROJECTDIR); \
	gzip $(TAR); \
	mv $(TAR).gz $(TGZ); \
	rm -rf $(PROJECTDIR))

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
ARCH=`uname -i`
# ARCH=i386
# These are the three rpms automagically built by the spec
SSRPM = $(PROJECT)-src-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).src.rpm
SRPM = $(PROJECT)-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).src.rpm
LRPM = libdieharder-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).$(ARCH).rpm
PRPM = dieharder-$(VERSION_MAJOR).$(VERSION_MINOR)-$(RELEASE).$(ARCH).rpm
$(TGZ): tgz
$(SRPM): rpm
$(LRPM): rpm
$(PRPM): rpm

#========================================================================
# One stop shop.  Basically we build this every time, we hope.
rpm:	Makefile $(TGZ)
	rm -rf /var/tmp/dieharder*
	cp $(TGZ) $(RPM_TOPDIR)/SOURCES
	cp $(SPEC) $(RPM_TOPDIR)/SPECS
	rpmbuild -ba --target=$(ARCH) $(RPM_TOPDIR)/SPECS/$(SPEC)
	cp $(RPM_TOPDIR)/SRPMS/$(SSRPM) $(SRPM)
	cp $(RPM_TOPDIR)/RPMS/$(ARCH)/$(LRPM) .
	cp $(RPM_TOPDIR)/RPMS/$(ARCH)/$(PRPM) .

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
	- (cd $(LIBRARY); \
	$(MAKE) clean; \
	cd ..; \
	cd $(PROGRAM); \
	$(MAKE) clean; \
	cd ..; \
	cd $(MANUAL); \
	$(MAKE) clean;)

#========================================================================
# We need two toplevel targets that have to be mirrored in $(SPEC).
#========================================================================
install:
	(make clean;\
	make installlib;\
	make installprog;\
	)

# Restore this here and in the specfile when we are ready to
# actually publish the manual, which is NOT YET.
#	make installman;\


installlib:
	(cd $(LIBRARY);\
	make BUILDROOT=$(BUILDROOT) PREFIX=$(PREFIX) install)

installprog:
	(cd $(PROGRAM);\
	make PREFIX=$(PREFIX) install)

installman:
	(cd $(MANUAL);\
	make PREFIX=$(PREFIX) install)

installweb : $(TGZ) $(RPM) $(SRPM) $(LRPM) $(PRPM) $(MRPM) $(ABS) $(PHP)
	(ssh $(WLOGIN) mkdir -p public_html/$(WDIR)/$(PROJECT);\
	rsync -avz $(TGZ) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(SRPM) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(LRPM) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(PRPM) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(MRPM) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(ABS) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(PHP) $(WLOGIN):public_html/$(WDIR)/)

REPOSERVER = uriel
REPOPATH = /var/www/html/fc/6/local/
installrepo : $(TGZ) $(RPM) $(SRPM) $(LRPM) $(PRPM) $(MRPM) $(ABS) $(PHP)
	(ssh $(REPOSERVER) mkdir -p $(REPOPATH);\
	rsync -avz $(TGZ) root@$(REPOSERVER):$(REPOPATH);\
	rsync -avz $(SRPM) root@$(REPOSERVER):$(REPOPATH)/SRPM;\
	ssh $(REPOSERVER) mkdir -p $(REPOPATH)/$(ARCH);\
	rsync -avz $(LRPM) root@$(REPOSERVER):$(REPOPATH)/$(ARCH);\
	rsync -avz $(PRPM) root@$(REPOSERVER):$(REPOPATH)/$(ARCH);\
	rsync -avz $(MRPM) root@$(REPOSERVER):$(REPOPATH)/$(ARCH);\
	ssh root@$(REPOSERVER) "cd $(REPOPATH)/$(ARCH);createrepo .")

#========================================================================
# We give all generic rules below.  Currently we only need a rule for 
# objects.
#========================================================================
%.o:%.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $<

