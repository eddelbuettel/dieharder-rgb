/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

#include "rand_rate.h"

void parsecl(int argc, char **argv)
{
 int imax,i,c,errflg=0;
 extern char *optarg;
 extern int optind, opterr, optopt;

 psamples = 100;		/* Should NOT be a "lot", 10-100 is plenty */
 tsamples = 10000;		/* This SHOULD be a lot. */
 iterations = -1;	/* This should be just enough to do empty accurately */
 size = 1024;		/* Small enough to easily fit into any cache */
 bits = 128;            /* Maximum size bitstring is default */
 verbose = 0;		/* Default is not to be verbose. */
 quiet = 0;		/* Default is ALSO not to be quiet (output control). */
 testnum = 0;		/* Default is to list rng's */
 randnum = 12;          /* Default is mt19937, as the "best" overall */
 reseed_flag = 0;       /* Don't reseed for every sample (default) */

 while ((c = getopt(argc,argv,"b:f:hin:p:qr:s:t:v:")) != EOF){
   switch (c){
     case 'h':
       Usage();
       exit(0);
       break;
     case 'b':
       bits = strtol(optarg,(char **) NULL,10);
       break;
     case 'f':
       strncpy(filename,optarg,128);
       fprintf(stderr,"Error: -f %s not yet supported!  Exiting...\n",filename);
       exit(0);
       break;
     case 'i':
       reseed_flag = 1;
       break;
     case 'n':
       size = strtol(optarg,(char **) NULL,10);
       break;
     case 'p':
       psamples = strtol(optarg,(char **) NULL,10);
       break;
     case 'q':
       quiet = 1;
       break;
     case 'r':
       randnum = strtol(optarg,(char **) NULL,10);
       break;
     case 's':
       tsamples = strtol(optarg,(char **) NULL,10);
       break;
     case 't':
       testnum = strtol(optarg,(char **) NULL,10);
       break;
     case 'v':
       verbose = strtol(optarg,(char **) NULL,10);
       /* printf("Verbose is now %d\n",verbose); */
       break;
     case '?':
       errflg++;
   }
 }

 if(errflg){
   Usage();
   exit(0);
 }

 /*
  * If there are no more arguments, we are done.
  */
 if(argc-optind == 0){
   return;
 }

 /*
  * Anything else left is an erroneous call and should cause the Usage
  * message to be printed.  memtest -h or --help will therefore "work".
  */

 Usage();

}

void Usage()
{

 fprintf(stdout, "\n");
 fprintf(stdout, "Usage:\n");
 fprintf(stdout, "  rand_rate [-t testnumber] [-r rngnumber] [-f filename ]\n");
 fprintf(stdout, "           [-b number of bits] [-n length] [-s samples] [-i]\n");
 fprintf(stdout, "           [-q] [-h] [-v level]\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "  -t testnumber selects the test to be performed.  Available tests:\n");
 fprintf(stdout, "     %d (default) list available rngs.\n",LIST_RNGS);
 fprintf(stdout, "     %d write list of random integers and uniform deviates to stdout with\n",LIST_RAND);
 fprintf(stdout, "       selected rng.\n");
 fprintf(stdout, "     %d time selected generator, determining its bogomegarate.\n",BOGORATE);
 fprintf(stdout, "     %d Diehard runs (tests ascending/descending runs).\n",DIEHARD_RUNS);
 fprintf(stdout, "     %d Diehard birthday (tests repeated birthday intervals).\n",DIEHARD_BDAY);
 fprintf(stdout, "     %d Diehard 2d sphere (tests minimum distance between points in 2D).\n",DIEHARD_2DSPHERE);
 fprintf(stdout, "     %d Diehard 3d sphere (tests minimum distance between points in 3D).\n",DIEHARD_3DSPHERE);
 fprintf(stdout, "     %d RGB bitmask (tests for frozen/unchanging bits).\n",RGB_PERSIST);
 fprintf(stdout, "     %d RGB bitdist (tests frequency of all n-bit patterns:\n",RGB_BITDIST);
 fprintf(stdout, "       note that this is an extremely powerful master test and can \n");
 fprintf(stdout, "       replace a whole raft of weaker tests in STS and Diehard).\n");
 fprintf(stdout, "     %d STS monobit (tests number of 1s only).\n",STS_MONOBIT);
 fprintf(stdout, "     %d STS runs (de facto tests frequency of 2-bit patterns)\n",STS_RUNS);
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
