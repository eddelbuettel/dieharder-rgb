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
     diehard_3dsphere();
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

 fprintf(stdout, "\n");
 fprintf(stdout, "Usage:\n");
 fprintf(stdout, "  dieharder [-t testnumber] [-r rngnumber] [-f filename ]\n");
 fprintf(stdout, "           [-b number of bits] [-n length] [-s samples] [-i]\n");
 fprintf(stdout, "           [-q] [-h] [-v level]\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "  -r rngnumber selects the rng to be tested (list them with -t 0).\n");
 fprintf(stdout, "  -f filename will EVENTUALLY permit random strings to be tested to be\n");
 fprintf(stdout, "     read in from a file, but this is not yet implemented!\n");
 fprintf(stdout, "  -b controls the number of bits used in bit string tests\n");
 fprintf(stdout, "  -n controls the length of the test vector (int or double) for vector\n");
 fprintf(stdout, "     tests\n");
 fprintf(stdout, "  -s controls the number of samples (default 100)\n");
 fprintf(stdout, "  -i turns on a new seed for each sample -- better indicates the\n");
 fprintf(stdout, "     probability of a \"bad seed\" existing for an otherwise good\n");
 fprintf(stdout, "     generator.\n");
 fprintf(stdout, "  -v controls verbosity of output for debugging or amusement purposes.\n");
 fprintf(stdout, "  -q selects \"quiet\" operation: results only are printed on a single line\n");
 fprintf(stdout, "     (where applicable).\n");
 fprintf(stdout, "  -h prints usage statement (this message) and exits.\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "  NOTE WELL:  The \"bogomegarates\" returned by this tool are BOGUS\n");
 fprintf(stdout, "  and may not be even approximately correct in your context.  Also, the\n");
 fprintf(stdout, "  quality assessment(s) for the rngs may, in fact, be completely incorrect\n");
 fprintf(stdout, "  or misleading.  Use them at your Own Risk!  Be Warned!\n");
 fprintf(stdout,"\n");
 exit(0);

}
