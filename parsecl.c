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

 samples = 100;		/* Should NOT be a "lot", 10-100 is plenty */
 iterations = -1;	/* This should be just enough to do empty accurately */
 size = 1024;		/* Small enough to easily fit into any cache */
 bits = 32;             /* Unsigned long in, probably */
 verbose = 0;		/* Default is not to be verbose. */
 quiet = 0;		/* Default is ALSO not to be quiet (output control). */
 testnum = 0;		/* Default is to list rng's */
 randnum = 12;          /* Default is mt19937, as the "best" overall */

 while ((c = getopt(argc,argv,"b:f:hi:n:qr:s:t:v:")) != EOF){
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
       iterations = strtol(optarg,(char **) NULL,10);
       break;
     case 'n':
       size = strtol(optarg,(char **) NULL,10);
       break;
     case 'q':
       quiet = 1;
       break;
     case 'r':
       randnum = strtol(optarg,(char **) NULL,10);
       break;
     case 's':
       samples = strtol(optarg,(char **) NULL,10);
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

 fprintf(stdout, "
Usage:
  rand_rate [-t testnumber] [-r rngnumber] [-f filename ]
           [-b number of bits] [-n length] [-s samples] [-i iterations]
           [-q] [-h] [-v level]

  -t testnumber selects the test to be performed.  Available tests:
     0 (default) list available rngs.
     1 write list of random integers and uniform deviates to stdout with
       selected rng.
     2 time selected generator, determining its bogomegarate.
     3 STS monobit test (number of 1's in bitstring rel to expected val).
     4 RGB binomial test (number of 1's in bitstrings rel to binomial dist).

  -r rngnumber selects the rng to be tested (list them with -t 0).

  -f filename will EVENTUALLY permit random strings to be tested to be
     read in from a file, but this is not yet implemented!

  -b controls the number of bits used in bit string tests
  -n controls the length of the test vector (int or double) for vector
     tests
  -s controls the number of samples (default 100)
  -i controls the number of TIMING iterations
     If omitted, iterations are automatically scaled in timing runs (best).

  -v controls verbosity of output for debugging or amusement purposes.
  -q selects \"quiet\" operation: results only are printed on a single line
     (where applicable).
  -h prints usage statement (this message) and exits.

  NOTE WELL:  The \"bogomegarates\" returned by this tool are BOGUS
  and may not be even approximately correct in your context.  Be Warned!
  ALSO NOTE:  The quality assessment(s) for the rngs may, in fact, be
  completely incorrect or misleading.  Use them at your Own Risk!

  NOTE FINALLY: If you use this tool, you owe me a beverage! (see GPL v2.0b)
");
 exit(0);

}
