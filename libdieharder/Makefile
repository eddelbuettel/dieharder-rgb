#========================================================================
# This is a Makefile for the libdieharder library, part of the
# overall dieharder package.  It has a very abbreviated set of targets.
#
# make          alone should build the application.
#
# make clean    deletes the application and all object files
# make install  strips and installs application and a man page
# make printout prints out all source and include files
# 
# These two commands execute in the toplevel project directory only.
# make svn      does a svn commit and creates the timestamp $(SVN)
# make sync     does a svn commit and rsyncs to list of hosts
#========================================================================
PROJECT = dieharder
DIR = libdieharder

#========================================================================
# This is essential.  The rpmbuild overrides it, but we have to make
# build "work" when executed only in the source tree directory itself.
# This isn't easy, since the dependences are more than a bit scattered.
# We therefore point to the directory one level up, where we should
# find a ./lib, ./include, ./share and ./bin directory tree from which
# the various dieharder files will actually be assembled into an rpm
# with PREFIX=/usr (for example).
#========================================================================
BUILDROOT=../buildroot
PREFIX=$(BUILDROOT)/usr
LIBDIR=$(PREFIX)/lib
INCDIR=$(PREFIX)/include
INCTIME=include.time

SVNTREE = $(HOME)/Src/svn-tree
SVNPATH = $(SVNTREE)/$(PROJECT)
SVNTIME = $(DIR:=.svn.time)

# This is the library from which both wulfstat and wulflogger (and maybe
# other stuff in the future) is built.
PROGMAN = libdieharder.3
PROGLIB = libdieharder.a
PROGLIB_SONAME = libdieharder.so.$(VERSION_MAJOR)
PROGLIB_NAME = libdieharder.so.$(VERSION_MAJOR).$(VERSION_MINOR)

# This is automagically set in the toplevel build.  Do not edit by
# hand.
VERSION_MAJOR=2
VERSION_MINOR=4.24
RELEASE=3

#========================================================================
# Define all sources.  Note that we choose to depend on ALL the includes
# in the include directory, which cannot be empty or this will barf.
#========================================================================
LIBSOURCES = $(shell ls *.c  2>&1 | sed -e "/\/bin\/ls:/d")
LIBOBJECTS = $(LIBSOURCES:.c=.o)

DEFINES = -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) \
          -DRELEASE=$(RELEASE)

#========================================================================
# Define parameters and directives needed in compile/link steps.
#========================================================================
# C Compiler
CC = gcc

# Compile flags (use fairly standard -O3 as default)
CFLAGS = -O3 $(DEFINES) -I $(INCDIR)

# Linker flags
LDFLAGS = --shared -Wl,-soname,$(PROGLIB_SONAME)

# Libraries
LIBS = -lgsl -lgslcblas -lm

#========================================================================
# List of variants one can make.  all is the default.  We always
# presume the simplest of dependencies and remake if includes change
# for example.
#========================================================================
all: $(INCTIME) $(PROGLIB) $(PROGLIB_NAME)

# This has to ALWAYS run before a build.
$(INCTIME):
	(install -d $(INCDIR)/dieharder; \
	install -m 644 ../include/dieharder/*.h $(INCDIR)/dieharder;)
	
$(PROGLIB): $(LIBOBJECTS) $(LIBINCLUDES)
	ar r $(PROGLIB) $(LIBOBJECTS)
	ranlib $(PROGLIB)

$(PROGLIB_NAME): $(LIBOBJECTS) $(LIBINCLUDES)
	gcc $(LDFLAGS) -o $(PROGLIB_NAME) $(LIBOBJECTS)

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
#  A standard cleanup target
#========================================================================
clean:
	rm -f core $(PROGLIB) $(PROGLIB_NAME) $(LIBOBJECTS) $(PROGMAN).gz

#========================================================================
# This is critical.  For the toplevel rpm build to succeed,
#  make PREFIX=/usr install
# (run by rpmbuild from the specfile) has to work, in the right order.
# This target has to install precisely the files required by the
# specfile for the dieharder package, in precisely the right locations.
#========================================================================
install: $(PROGLIB) $(PROGLIB_NAME) $(PROGMAN)
	(install -d $(LIBDIR); \
	install -m 755 $(PROGLIB) $(LIBDIR); \
	install -m 755 $(PROGLIB_NAME) $(LIBDIR); \
	install -d $(INCDIR)/dieharder; \
	install -m 644 ../include/dieharder/*.h $(INCDIR)/dieharder; \
	install -d $(PREFIX)/share/man/man3; \
	gzip -c $(PROGMAN) > $(PROGMAN).gz; \
	install -m 644 $(DIR).3.gz $(PREFIX)/share/man/man3 )

#========================================================================
# We give all generic rules below.  Currently we only need a rule for 
# objects.
#========================================================================
%.o:%.c include/dieharder/libdieharder.h Makefile
	$(CC) -c $(CFLAGS) $<

# dependencies.  There has to be a better way?
rgb_timing.o: ../include/dieharder/rgb_timing.h
rgb_persist.o: ../include/dieharder/rgb_persist.h
rgb_bitdist.o: ../include/dieharder/rgb_bitdist.h
diehard_birthdays.o: ../include/dieharder/diehard_birthdays.h
diehard_operm5.o: ../include/dieharder/diehard_operm5.h
diehard_.o: ../include/dieharder/diehard_operm5.h
diehard_rank_32x32.o: ../include/dieharder/diehard_rank_32x32.h
diehard_rank_6x8.o: ../include/dieharder/diehard_rank_6x8.h
diehard_bitstream.o: ../include/dieharder/diehard_bitstream.h
diehard_opso.o: ../include/dieharder/diehard_opso.h
diehard_oqso.o: ../include/dieharder/diehard_oqso.h
diehard_dna.o: ../include/dieharder/diehard_dna.h
diehard_count_1s_stream.o: ../include/dieharder/diehard_count_1s_stream.h
diehard_count_1s_byte.o: ../include/dieharder/diehard_count_1s_byte.h
diehard_parking_lot.o: ../include/dieharder/diehard_parking_lot.h
diehard_2dsphere.o: ../include/dieharder/diehard_2dsphere.h
diehard_3dsphere.o: ../include/dieharder/diehard_3dsphere.h
diehard_squeeze.o: ../include/dieharder/diehard_squeeze.h
diehard_sums.o: ../include/dieharder/diehard_sums.h
diehard_runs.o: ../include/dieharder/diehard_runs.h
diehard_craps.o: ../include/dieharder/diehard_craps.h
marsaglia_tsang_gcd.o: ../include/dieharder/marsaglia_tsang_gcd.h
sts_monobit.o: ../include/dieharder/sts_monobit.h
sts_runs.o: ../include/dieharder/sts_runs.h
