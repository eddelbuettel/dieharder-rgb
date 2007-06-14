/*
 *========================================================================
 * $Id: parsecl.c 14 2001-09-12 16:09:15Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include "operm.h"

/*
 * This routine sets certain globals to their default values and permits
 * those defaults to be overridden on the command line.
 */

void parsecl(int argc, char **argv)
{
 int opt_c,opt_errflg=0;
 extern char *optarg;
 extern int optind, opterr, optopt;

 /* 
  * Initialize global variables with defaults, then override if
  * desired on command line.  Read the getopt man page to see how
  * to set other variables.  Or insert your own parsing code in here --
  * Sometimes I use getopts, sometimes I parse myself, sometimes both.
  */
 verbose = 0;			/* Default is quiet */

 while ((opt_c = getopt(argc,argv,"hv")) != EOF){
   switch (opt_c){
     case 'h':
       Usage();
       exit(0);
       break;
     case 'v':
       verbose = 1;
       break;
     case '?':
       opt_errflg++;
   }
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
 fprintf(stdout, "  OPERM [-h] [-v]\n");
 fprintf(stdout, "\n");
 fprintf(stdout, "   -v selects \"verbose\" operation for debugging, very noisy.\n");
 fprintf(stdout, "   -h prints usage statement (this message) and exits.\n");
 fprintf(stdout, "\n");
 exit(0);

}
