/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include "dieharder.h"

void help()
{

 /*
  * help for EACH test must be defined in the test.c package, even if
  * it is null or says "sorry, no help available".  help should be
  * a brief test description all pretty with each line beginning with
  * "#" so it is easy to filter out of standard test results output.
  */
 if(all==YES){
   help_diehard_runs();
   help_diehard_birthdays();
   help_diehard_2dsphere();
   help_diehard_3dsphere();
   help_diehard_rank_32x32();
   help_diehard_rank_6x8();
   help_rgb_persist();
   help_rgb_bitdist();
   help_sts_monobit();
   help_sts_runs();
   exit(0);
 }

 /*
  * Switches to do each test by number
  */
 switch(diehard){
   default:
     break;
   case DIEHARD_RUNS:
     help_diehard_runs();
     break;
   case DIEHARD_BDAY:
     help_diehard_birthdays();
     break;
   case DIEHARD_2DSPHERE:
     help_diehard_2dsphere();
     break;
   case DIEHARD_3DSPHERE:
     help_diehard_3dsphere();
     break;
   case DIEHARD_RANK_32x32:
     help_diehard_rank_32x32();
     break;
   case DIEHARD_RANK_6x8:
     help_diehard_rank_6x8();
     break;
   case DIEHARD_PARKING_LOT:
     help_diehard_parking_lot();
     break;
 }
 switch(rgb){
   default:
     break;
   case RGB_PERSIST:
     help_rgb_persist();
     break;
   case RGB_BITDIST:
     help_rgb_bitdist();
     break;
 }
 switch(sts){
   default:
     break;
   case STS_MONOBIT:
     help_sts_monobit();
     break;
   case STS_RUNS:
     help_sts_runs();
     break;
 }
 /*
  * Add user-defined tests here following the pattern above.  Be sure
  * to add the new tests to the enum in dieharder.h.  Also add its
  * prototypes there, following the patterns of the tests already
  * implemented.
  */
 switch(user){
   default:
     break;
 }

 if(diehard == NO && rgb == NO && sts == NO && user == NO && all == NO){
   Usage();
 }
 exit(0);

}


void Usage()
{

 fprintf(stdout, "\n\
Usage:\n\
  dieharder [-a] [-b bits] [-d diehard test number] [-f filename]\n\
           [-g generator number] [-h] [-i iterations] [-l] \n\
           [-n ntuple] [-p number of p samples] [-q] [-o filename] \n\
           [-O] [-r rgb test number] [-s sts test number] \n\
	   [-t number of test samples] [-u user test number] \n\
	   [-v verbose flag] [-x xvalue] [-y yvalue] [-z zvalue] \n\
\n\
  -a Runs all the tests with standard/default options to create a report\n\
  -b bits - sets the number of bits to be used in tests that act on a bit\n\
     string of variable length, e.g. the rgb bitdist test.\n\
  -d test number -  selects specific diehard test.\n\
  -f filename will EVENTUALLY permit random strings to be tested to be\n\
     read in from a file, but this is not yet implemented!\n\
  -g generator number - selects a specific generator for testing.  Using\n\
     -1 causes all known generators to be printed out to the display.\n\
  -h prints context-sensitive help -- usually Usage (this message) or a\n\
     test synopsis if entered as e.g. dieharder -d 3 -h.\n\
  -i iterations - sets iteration count for timing runs (should not be\n\
     needed).\n\
  -l list all known tests.\n\
  -n ntuple - set ntuple length for tests on short bit strings that permit\n\
     the length to be varied (e.g. rgb bitdist).\n\
  -o filename - output -t count random numbers to file.\n\
  -O force the usage of overlapping samples in certain diehard tests.\n\
  -p count - sets the number of p-value samples per test (default 100).\n\
  -q selects \"quiet\" operation: results only are printed on a single line\n\
     (where applicable).  This is a way of getting a very compact report.\n\
  -r test number - selects specific rgb test.\n\
  -S seed.  Any (uint) seed but (default) 0 surpresses per-test,\n\
     fully automated reseeding from /dev/random if available or the \n\
     clock if not (where the latter is NOT really desireable).\n\
  -s test number - selects specific sts test.\n\
  -t count - sets the number of random entities used in each test, where\n\
     possible.  Be warned -- some tests will take a long time with the\n\
     default value of 10000.  Read the test synopses for suggested settings\n\
     for -t or use -a first.\n\
  -u test number - selects specific user-developed test, if you've added\n\
     one or more to this tool.  dieharder provides this interface to make\n\
     it easy to add your own tests.\n\
  -v verbose flag -- controls the verbosity of the output for debugging\n\
     only.  Probably of little use to non-developers, and developers can\n\
     read the enum(s) in dieharder.h and the test sources to see which\n\
     flag values turn on output on which routines.  1 is \"all\" and will\n\
     result in a fairly detailed trace of program activity.\n\
  -x,-y,-z number - Some tests have parameters that can safely be varied\n\
     from their default value.  For example, in the diehard birthdays test,\n\
     one can vary the number of \"dates\" drawn from the \"year\" of some\n\
     length, which can also be varied.  -x 2048 -y 30 alters these two\n\
     values but should still run fine.\n\
\n\
  NOTE WELL:  The \"bogomegarates\" returned by this tool are BOGUS\n\
  and may not be even approximately correct in your context.  Also, the\n\
  quality assessment(s) for the rngs may, in fact, be completely incorrect\n\
  or misleading.  Use them at your Own Risk!  Be Warned!\n\
\n");

 exit(0);

}

/*
 * This is a standard way of putting out a test description
 */
void help_test(Dtest *dtest)
{

 printf("%s",dtest->description);
 
}
