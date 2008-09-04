/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include "dieharder.h"


/*
 * main() is just a shell for routines that parse the command line,
 * do all the requested work, and then exit cleanly.
 */
int main(int argc, char *argv[]) 
{

 int i;

 /*
  * Parse command line and set global variables
  */
 parsecl(argc,argv);

 /*
  * Note that most of my cpu_rates (except the terminally simple/stupid) 
  * have three phases after parsecl():
  *
  * Startup: Allocate memory, initialize all derivative variables from
  * command line values.  
  */
 for(i=0;i<=2;i++){
 /* printf("Running startup for i = %d\n",i); */
 /* printf("Seed = %u, seed = %u\n",Seed,seed); */
 startup();

 /*
  * Work: Do all the work.  In a complicated cpu_rate, project_work would
  * itself be a shell for a lot of other modular routines.
  */
 /* printf("Running work for i = %d\n",i); */
 work();

 /*
  * If we're going to run inside a loop, we MUST free the generator allocated
  * in startup().
  */
 /* printf("Freeing generator %s and moving on.\n",gsl_rng_name(rng)); */
 gsl_rng_free(rng);
 /* printf("Clearing/resetting bit buffers.\n"); */
 reset_bit_buffers();

 }

 /* 
  * Quit:  Done. Clean up (if necessary) and exit.
 dieharder_quit();
  */

}
