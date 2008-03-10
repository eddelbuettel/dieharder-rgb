#========================================================================
# This is a Makefile for the libdieharder library, part of the
# overall dieharder package.  It has a very abbreviated set of targets.
#
# make          alone should build the application.
#
# make clean    deletes the application and all object files
# make install  installs application and a man page
# make printout prints out all source and include files
# 
# These two commands execute in the toplevel project directory only.
# make svn      does a svn commit and creates the timestamp $(SVN)
# make sync     does a svn commit and rsyncs to list of hosts
#========================================================================
# PROJECT = dieharder
# DIR = libdieharder

SVNTREE = $(HOME)/Src/svn-tree
SVNPATH = $(SVNTREE)/dieharder
SVNTIME = dieharder.svn.time

# This is automagically set in the toplevel build.  Do not edit by
# hand.
VERSION=@VERSION@
VERSION_MAJOR = $(shell expr $(VERSION) : '\(.*\)\..*\..*' \| $(VERSION))
RELEASE=0

# PROGLIB_A = libdieharder.a
# PROGLIB_SO = libdieharder.so
# PROGLIB_SONAME = libdieharder.so.$(VERSION_MAJOR)
# PROGLIB_SO_VERSION = libdieharder.so.$(VERSION)
man3_MANS = libdieharder.3
lib_LTLIBRARIES = libdieharder.la
libdieharder_la_LIBADD = -L${libdir} -lgsl -lgslcblas -lm
libdieharder_la_LDFLAGS = -version-number @DIEHARDER_LT_VERSION@
# The shell script is the easy way to do this, by far.  But it may not
# be sufficiently portable.
# libdieharder_la_SOURCES = $(shell ls *.c  2>&1 | sed -e "/\/bin\/ls:/d")
libdieharder_la_SOURCES = \
	add_lib_rngs.c \
	bits.c \
	chisq.c \
	countx.c \
	diehard_2dsphere.c \
	diehard_3dsphere.c \
	diehard_birthdays.c \
	diehard_bitstream.c \
	diehard_count_1s_byte.c \
	diehard_count_1s_stream.c \
	diehard_craps.c \
	diehard_dna.c \
	diehard_operm5.c \
	diehard_opso.c \
	diehard_oqso.c \
	diehard_parking_lot.c \
	diehard_rank_32x32.c \
	diehard_rank_6x8.c \
	diehard_runs.c \
	diehard_squeeze.c \
	diehard_sums.c \
	kstest.c \
	marsaglia_tsang_gcd.c \
	marsaglia_tsang_gorilla.c \
	parse.c \
	prob.c \
	random_seed.c \
	rank.c \
	rgb_bitdist.c \
	rgb_lmn.c \
	rgb_minimum_distance.c \
	rgb_operm.c \
	rgb_persist.c \
	rgb_timing.c \
	rngav.c \
	rng_ca.c \
	rng_dev_random.c \
	rng_dev_urandom.c \
	rng_file_input.c \
	rng_file_input_raw.c \
	rng_kiss.c \
	rngs_gnu_r.c \
	rng_uvag.c \
	sample.c \
	std_test.c \
	sts_monobit.c \
	sts_runs.c \
	timing.c \
	Vtest.c \
	Xtest.c

DEFINES = -DVERSION=$(VERSION)

#========================================================================
# Define parameters and directives needed in compile/link steps.
#========================================================================
# C Compiler
# CC = gcc

# Compile flags (use fairly standard -O3 as default)
AM_CPPFLAGS = $(DEFINES) -I ${top_srcdir}/include -I ${includedir}
AM_CFLAGS = -O3

# Linker flags
# LDFLAGS += -g -fpic -shared -Wl,-soname,$(PROGLIB_SONAME)

# Libraries
# LIBS = -lgsl -lgslcblas -lm

#========================================================================
# List of variants one can make.  all is the default.  We always
# presume the simplest of dependencies and remake if includes change
# for example.
#========================================================================
# all: $(PROGLIB_SO_VERSION)

# $(PROGLIB_SO_VERSION): $(LIBOBJECTS) $(LIBINCLUDES)
# 	$(CC) $(LDFLAGS) -o $(PROGLIB_SO_VERSION) $(LIBOBJECTS)
# 	ldconfig -n .
# 	ln -sf $(PROGLIB_SO_VERSION) $(PROGLIB_SO)

#========================================================================
# The only safe place to do commits is in the toplevel directory
#========================================================================
svn:
	(cd ..;make svn)

sync:
	(cd ..;make sync)

#========================================================================
# printout makes an enscript -2r printout of SOURCES and
# and INCLUDES.  Use lpr if you don't have enscript
#========================================================================
LPR = enscript -2r
# LPR = lpr
printout:
	$(LPR) $(LIBSOURCES) $(LIBINCLUDES)

#========================================================================
# This is critical.  For the toplevel rpm build to succeed,
#  make prefix=/usr install
# (run by rpmbuild from the specfile) has to work, in the right order.
# This target has to install precisely the files required by the
# specfile for the dieharder package, in precisely the right locations.
#========================================================================
# install: $(PROGLIB_SO_VERSION) $(PROGLIB_A) $(PROGMAN)
#	(install -d $(DESTDIR)$(libdir); \
#	install -m 755 $(PROGLIB_SO_VERSION) $(DESTDIR)$(libdir)/$(PROGLIB_SO_VERSION); \
#	ln -sf $(libdir)/$(PROGLIB_SO_VERSION) $(DESTDIR)$(libdir)/$(PROGLIB_SO); \
#	install -d $(DESTDIR)$(includedir)/dieharder; \
#	install -m 644 ../include/dieharder/*.h $(DESTDIR)$(includedir)/dieharder; \
#	install -d $(DESTDIR)$(mandir)/man3; \
#	install -m 644 $(DIR).3 $(DESTDIR)$(mandir)/man3)

#========================================================================
# We give all generic rules below.  Currently we only need a rule for 
# objects.
#========================================================================
# .o:.c $(LIBINCLUDES) Makefile
#	$(CC) -fpic -c $(CFLAGS) $<
