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
 size = 1000;		/* Small enough to easily fit into any cache */
 verbose = 0;		/* Default is not to be verbose. */
 quiet = 0;		/* Default is ALSO not to be quiet (output control). */
 testnum = 0;		/* Default is benchmark */
 randnum = 12;          /* Default is mt19937, as the "best" overall */

 while ((c = getopt(argc,argv,"hi:n:qr:s:t:v:")) != EOF){
   switch (c){
     case 'h':
       Usage();
       exit(0);
       break;
     case 'i':
       iterations = strtol(optarg,(char **) NULL,10);
       break;
     case 'n':
       samples = strtol(optarg,(char **) NULL,10);
       break;
     case 'q':
       quiet = 1;
       break;
     case 'r':
       randnum = strtol(optarg,(char **) NULL,10);
       break;
     case 's':
       size = strtol(optarg,(char **) NULL,10);
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
 fprintf(stdout, "Usage: \n");
 fprintf(stdout, "  rand_rate -r rngnumber [-t testnumber] \n");
 fprintf(stdout, "           [-s size] [-n num_samples] [-i iterations]\n");
 fprintf(stdout, "           [-q] [-h] [-v level]\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "  -r rngnumber selects the rng to be tested.\n");
 fprintf(stdout, "  -t testnumber selects the test to be performed.\n");
 fprintf(stdout, "     The default test is 0, the bogomegarate timing.\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "  -s controls the length of the test vector (int or double).\n");
 fprintf(stdout, "  -n controls the number of samples (default 100)\n");
 fprintf(stdout, "  -i controls the number of iterations inside the sampling loop.\n");
 fprintf(stdout, "     If omitted, iterations are automatically scaled (best).\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "  -v 1 causes available rng's to be listed.\n");
 fprintf(stdout, "     2 causes available tests to be listed.\n");
 fprintf(stdout, "     3 causes specific rng to be briefly demo'd.\n");
 fprintf(stdout, "  -q selects \"quiet\" operation: results only are printed on a single line\n");
 fprintf(stdout, "  -h prints usage statement (this message) and exits.\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "  NOTE WELL:  The \"Mega-rates\" returned by this tool are BOGUS\n");
 fprintf(stdout, "  and may not be even approximately correct.  Be Warned!\n");
 fprintf(stdout, "  ALSO NOTE:  The quality assessment(s) for the rngs may, in fact, be\n");
 fprintf(stdout, "  incorrect or misleading.  Use them at your Own Risk!\n");
 fprintf(stdout, "\n");
 exit(0);

}
