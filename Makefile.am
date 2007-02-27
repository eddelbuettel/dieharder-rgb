## Process this file with automake to produce Makefile.in

SUBDIRS = dieharder libdieharder manual

# pkginclude_HEADERS = copyright.h diehard_parking_lot.h rgb_bitdist.h \
#  diehard_2dsphere.h diehard_rank_32x32.h rgb_lmn.h diehard_3dsphere.h \
#  diehard_rank_6x8.h rgb_persist.h diehard_birthdays.h diehard_runs.h \
#  rgb_timing.h diehard_bitstream.h diehard_squeeze.h std_test.h \
#  diehard_count_1s_byte.h diehard_sums.h sts_monobit.h \
#  diehard_count_1s_stream.h Dtest.h sts_runs.h diehard_craps.h \
#  libdieharder.h tests.h diehard_dna.h marsaglia_tsang_gcd.h verbose.h \
#  diehard_operm5.h marsaglia_tsang_gorilla.h Vtest.h diehard_opso.h \
#  operm5_rdata.h Xtest.h diehard_oqso.h parse.h

# bin_SCRIPTS = gsl-config
# pkgconfigdir = $(libdir)/pkgconfig
# pkgconfig_DATA= gsl.pc

EXTRA_DIST = autogen.sh dieharder-config.in configure.ac THANKS BUGS SUPPORT dieharder.spec.in dieharder.m4 test_dieharder.sh

# bin_PROGRAMS = dieharder/dieharder

# m4datadir = $(datadir)/aclocal
# m4data_DATA = dieharder.m4

# check_SCRIPTS = test_dieharder.sh
# TESTS = test_dieharder.sh

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
VERSION=@VERSION@
RELEASE=@RELEASE@

ABS = $(PROJECT).abs
PHP = $(PROJECT).php

# RPM/tarball target objects.  We need rules for all of these.
PROJECTDIR = $(PROJECT)-$(VERSION)
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

$(ABS): Makefile
	cat $(ABS) | \
	sed -e 's/^\(<center><H2>Version \)\(.*\)/\1$(VERSION)<\/H2><\/center>/' > /tmp/$(ABS).$$
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
	cp $(ABS) $(PROJECTDIR); \
	cp $(PHP) $(PROJECTDIR); \
	cp Makefile.am $(PROJECTDIR); \
	cp $(SPEC).in $(PROJECTDIR); \
	cp dieharder_version.h.in $(PROJECTDIR); \
	cp autogen.sh $(PROJECTDIR); \
	cp missing $(PROJECTDIR); \
	cp install.sh $(PROJECTDIR); \
	cp mkinstalldirs $(PROJECTDIR); \
	cp ltmain.sh $(PROJECTDIR); \
	cp configure.ac $(PROJECTDIR); \
	cp Copyright $(PROJECTDIR); \
	cp COPYING $(PROJECTDIR); \
	cp INSTALL $(PROJECTDIR); \
	cp AUTHORS $(PROJECTDIR); \
	cp NEWS $(PROJECTDIR); \
	cp ChangeLog $(PROJECTDIR); \
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
SSRPM = $(PROJECT)-src-$(VERSION)-$(RELEASE).src.rpm
SRPM = $(PROJECT)-$(VERSION)-$(RELEASE).src.rpm
LRPM = libdieharder-$(VERSION)-$(RELEASE).$(ARCH).rpm
PRPM = dieharder-$(VERSION)-$(RELEASE).$(ARCH).rpm
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

installweb : $(TGZ) $(RPM) $(SRPM) $(LRPM) $(PRPM) $(MRPM) $(ABS) $(PHP)
	(ssh $(WLOGIN) mkdir -p public_html/$(WDIR)/$(PROJECT);\
	rsync -avz $(TGZ) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(SRPM) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(LRPM) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(PRPM) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(MRPM) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	rsync -avz $(ABS) $(WLOGIN):public_html/$(WDIR)/$(PROJECT)/; \
	ssh $(WLOGIN) "cd public_html/$(WDIR)/$(PROJECT);ln -sf $(TGZ) $(PROJECT).tgz"; \
	ssh $(WLOGIN) "cd public_html/$(WDIR)/$(PROJECT);ln -sf $(SRPM) $(PROJECT).src.rpm"; \
	ssh $(WLOGIN) "cd public_html/$(WDIR)/$(PROJECT);rm Current*;touch Current_Is_$(VERSION)"; \
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
