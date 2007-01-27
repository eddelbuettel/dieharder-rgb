#========================================================================
# This is a Makefile for the dieharder tty application, part of the
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
SVNPATH = $(SVNTREE)/$(PROJECT)
SVNTIME = $(DIR:=.svn.time)

# This is automagically set in the toplevel build.  Do not edit by
# hand.
VERSION_MAJOR=2
VERSION_MINOR=4.24
RELEASE=1

#========================================================================
# Define all sources.  Note that we choose to depend on ALL the includes
# in the include directory, which cannot be empty or this will barf.
# We also have dependencies that extend to the libdieharder source
# directory, as we need to force a build if any of them change.
#========================================================================
SRCSOURCES = $(shell ls *.c  2>&1 | sed -e "/\/bin\/ls:/d")
SRCINCLUDES = $(shell ls *.h  2>&1 | sed -e "/\/bin\/ls:/d")

# These are important dependencies, although it isn't easy to
# see how to force a consistent build when working on the UI
# and library at the same time.  Maybe with particular make targets...
LIBINCLUDES = $(shell ls $(INCDIR)/*.h  2>&1 | sed -e "/\/bin\/ls:/d")
PROGLIB_SONAME = $(LIBDIR)/libdieharder.so.$(VERSION_MAJOR)

SOURCES = $(LIBSOURCES) $(SRCSOURCES)
INCLUDES = $(LIBINCLUDES) $(SRCINCLUDES)
OBJECTS = $(SRCSOURCES:.c=.o)

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
# List of variants one can make.  all is the default.  We always
# presume the simplest of dependencies and remake if includes change
# for example.
#========================================================================
all: $(PROGRAM)

$(PROGRAM): $(OBJECTS) $(PROGLIB_SONAME)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS) 

#========================================================================
# This is cautiously permitted.  REALLY, though, we need a conditional
# that punts with a warning if we try this during an rpm build and
# the library is not found -- the right solution then is to install
# the library first!
#========================================================================
$(PROGLIB_SONAME):
	- (cd ../libdieharder ; $(MAKE))

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
%.o:%.c
	$(CC) -c $(CFLAGS) $<

