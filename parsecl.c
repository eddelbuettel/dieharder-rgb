/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

#include "cpu_rate.h"

void parsecl(int argc, char **argv)
{
 int imax,i,c,errflg=0;
 extern char *optarg;
 extern int optind, opterr, optopt;

 samples = 100;		/* Should NOT be a "lot", 10-100 is plenty */
 iterations = -1;	/* This should be just enough to do empty accurately */
 size = 1000;		/* Small enough to easily fit into any cache */
 stride = 1;		/* Vector stride used */
 random_flag = 0;	/* Default is to NOT randomize with usleep */
 verbose = 1;		/* Default is to be a bit noisy (debugging control). */
 quiet = 0;		/* Default is NOT to be quiet (output control). */
 xtest = PI;		/* Default is PI to many digits */
 testnum = -1;		/* Default is no test at all */

 while ((c = getopt(argc,argv,"cd:fhi:n:qrs:t:v:x:")) != EOF){
   switch (c){
     case 'd':
       stride = strtol(optarg,(char **) NULL,10);
       break;
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
       random_flag = 1;
       break;
     case 's':
       size = strtol(optarg,(char **) NULL,10);
       break;
     case 't':
       testnum = strtol(optarg,(char **) NULL,10);
       break;
     case 'v':
       verbose = strtol(optarg,(char **) NULL,10);
       printf("Verbose is now %d\n",verbose);
       break;
     case 'x':
       xtest = strtod(optarg,(char **) NULL);
       break;
     case '?':
       errflg++;
   }
 }

 if(testnum<0){
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
 fprintf(stdout, "  cpu_rate -t testnumber [-q] [-h] [-i iterations]\n");
 fprintf(stdout, "           [-n num_samples] [-s size] [-d stride] [-x xtest]\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "  -t testnumber selects benchmark to be run:\n");
 fprintf(stdout, "     0 is idiot = dummy (almost) empty loop\n");
 fprintf(stdout, "     1 is bogomflops d[i] = (d[i] + ad)*(bd - d[i])/d[i]\n");
 fprintf(stdout, "     2 is copy d[i] = a[i]\n");
 fprintf(stdout, "     3 is scale d[i] = xtest*d[i]\n");
 fprintf(stdout, "     4 is add d[i] = a[i] + b[i]\n");
 fprintf(stdout, "     5 is triad d[i] = a[i] + xtest*b[i]\n");
 fprintf(stdout, "     6 is trid (average +-*) d[i] = (ad + bd - cd)*d[i]\n");
 fprintf(stdout, "     7 is memtest sequential ai[ai[i]] swap\n");
 fprintf(stdout, "     8 is memtest random ai[ai[i]] swap\n");
 fprintf(stdout, "     9 is savage: xtest = tan(atan(exp(log(sqrt(xtest*xtest)))))\n");
 fprintf(stdout, "    10 is /dev/random\n");
 fprintf(stdout, "  Any other values test the empty (timing) loop\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "  -i controls the number of iterations inside the sampling loop.\n");
 fprintf(stdout, "     If omitted, iterations are automatically scaled (best).\n");
 fprintf(stdout, "  -n controls the number of samples (default 100)\n");
 fprintf(stdout, "  -x xtest allows one to set the x used in the vector (and hence to divide).\n");
 fprintf(stdout, "     Powers of 2 are anomalously optimized on some architectures.\n");
 fprintf(stdout, "  -s controls the size of the double vector.  This allows one to \n");
 fprintf(stdout, "     observe the effects of L1 and L2 cache size on the result.\n");
 fprintf(stdout, "  -d controls the delta/stride used to step through vector sizes. \n");
 fprintf(stdout, "  -q selects \"quiet\" operation: results only are printed on a single line\n");
 fprintf(stdout, "  -v verbosity controls selectable debugging output.\n");
 fprintf(stdout, "  -h prints usage statement (this message) and exits.\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "  NOTE WELL:  The \"Mega-rates\" returned by this tool are BOGUS\n");
 fprintf(stdout, "  and may not be even approximately correct.  Be Warned!\n");
 fprintf(stdout, "\n");
 exit(0);

}
