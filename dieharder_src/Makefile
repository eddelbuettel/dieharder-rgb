#========================================================================
# This is a Makefile for the dieharder tty application, part of the
# overall dieharder package.  It has a very abbreviated set of targets.
#
# make          alone should build the application.
#
# make clean    deletes the application and all object files
# make install  strips and installs application and a man page
# make printout prints out all source and include files
#========================================================================
PROGRAM = dieharder
DIR = $(PROGRAM)

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
LIBDIR=$(PREFIX)/lib
INCDIR=$(PREFIX)/include

SVNTREE = $(HOME)/Src/svn-tree
SVNPATH = $(SVNTREE)/$(DIR)
SVNTIME = $(DIR:=.svn.time)

# Secondary/test binary programs next
# PROGRAM2 = project2

# This is revision information that MUST be set here.  It is minimally
# used to set variables in an accompanying spec file (see template in
# this directory) and/or in defines passed to the application so that
# it knows its own version information.
VERSION_MAJOR=2
VERSION_MINOR=4.24
RELEASE=1

#========================================================================
# Define all sources.  We ALWAYS have $(SOURCE) derived from $(PROGRAM)
# and $(INCLUDE) derived from $(PROGRAM) and will usually have parsecl.c
# and other modules.
#========================================================================
SOURCE = $(PROGRAM:=.c)

#========================================================================
# Test encapsulations for display.  Separating the tests themselves into
# library calls requires that each test be encapsulated in a call that
# runs the test from the library and then recovers and displays the
# results.  This encapsulation will likely continue (with suitable
# callbacks) even in GUI versions of dieharder.
#========================================================================
TESTS = \
    run_rgb_timing.c \
    run_rgb_persist.c \
    run_rgb_bitdist.c \
    run_diehard_birthdays.c \
    run_diehard_operm5.c \
    run_diehard_rank_32x32.c \
    run_diehard_rank_6x8.c \
    run_diehard_bitstream.c \
    run_diehard_opso.c \
    run_diehard_oqso.c \
    run_diehard_dna.c \
    run_diehard_count_1s_stream.c \
    run_diehard_count_1s_byte.c \
    run_diehard_parking_lot.c \
    run_diehard_2dsphere.c \
    run_diehard_3dsphere.c \
    run_diehard_squeeze.c \
    run_diehard_sums.c \
    run_diehard_runs.c \
    run_diehard_craps.c \
    run_marsaglia_tsang_gcd.c \
    run_sts_monobit.c \
    run_sts_runs.c \
    run_user_template.c \

SOURCES = \
    $(SOURCE) \
    $(TESTS) \
    add_my_types.c \
    empty_random.c \
    list_rand.c \
    list_rngs.c \
    output_rnds.c \
    parsecl.c \
    help.c \
    histogram.c \
    startup.c \
    test.c \
    work.c \
    user_template.c \
    testbits.c \

INCLUDE = $(PROGRAM:=.h)
INCLUDES = $(INCLUDE) $(TINCLUDES)

DEFINES = -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) \
          -DRELEASE=$(RELEASE)

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
CFLAGS = -O3 $(DEFINES) -I $(INCDIR)

# Linker flags
LDFLAGS = -L $(LIBDIR)

# Libraries
LIBS = -ldieharder -lgsl -lgslcblas -lm

#========================================================================
# Define standard sources and targets.
#========================================================================
OBJECTS = $(SOURCES:.c=.o)

#========================================================================
# List of variants one can make.  all is the default.  We always
# presume the simplest of dependencies and remake if includes change
# for example.
#========================================================================
all: $(PROGRAM)

$(PROGRAM): $(OBJECTS) $(INCLUDES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS) 

#========================================================================
# Build targets (from commands)
#========================================================================
# library:
# 	- (cd libdieharder ; $(MAKE))

# The only safe place to do commits is in the toplevel directory
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
	$(LPR) $(SOURCES) $(INCLUDES)

#========================================================================
#  A standard cleanup target
#========================================================================
clean : 
	- rm -f core $(PROGRAM) *.o $(PROGRAM).1.gz

#========================================================================
# This is critical.  For the toplevel rpm build to succeed,
#  make PREFIX=/usr install
# (run by rpmbuild from the specfile) has to work, in the right order.
# This target has to install precisely the files required by the
# specfile for the dieharder package, in precisely the right locations.
#========================================================================
install : $(PROGRAM)
	(strip $(PROGRAM);\
	install -d $(PREFIX)/bin; \
	install -m 755 $(PROGRAM) $(PREFIX)/bin; \
	install -d $(PREFIX)/share/man/man1; \
	gzip -c $(PROGRAM).1 > $(PROGRAM).1.gz; \
	install -m 644 $(PROGRAM).1.gz $(PREFIX)/share/man/man1)

#========================================================================
# We give all generic rules below.  Currently we only need a rule for 
# objects.
#========================================================================
%.o:%.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $<

