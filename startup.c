/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * This routine does all the required initialization and startup,
 * including memory allocation and prefilling of vectors.  It is
 * COMPLETELY outside the timing loops.
 *========================================================================
 */

#include "rand_rate.h"

void cpu_rate_startup()
{

 int i,imax,j,k;

 /*
  * What homebrew generators might we have today?
  * Always /dev/random...
  */
 sprintf(homebrews[0],"/dev/random");
 num_homebrews = 1;

 /*
  * Count and optionally list the available, built in gsl generators
  */
 types = gsl_rng_types_setup ();
 if(verbose == HELPGEN){
   printf("Listing available built-in gsl-linked generators:\n");
   printf("Test Number      Name\n");
   printf("================================\n");
 }
 i = 0;
 while(types[i] != NULL){
   if(verbose == HELPGEN){
     printf(" %d\t\t%s\n", i, types[i]->name);
   }
   i++;
 }
 num_gsl_rngs = i;
 if(verbose == HELPGEN){
   printf("================================\n");
   printf(" Listing additional (non-gsl-linked) tests with local sources:\n");
   printf("Test Number      Name\n");
   printf("================================\n");
   for(i = 0;i<num_homebrews;i++){
     printf(" %d\t\t%s\n", i+num_gsl_rngs, homebrews[i]);
   }
 }

 if(verbose == HELPGEN) exit(0);

 /*
  * This extracts the index of a homebrew test.  Eventually we'll
  * need to put initialization calls for its wrapper here as well.
  * In fact, we may want to just plain steal the entire wrapper
  * structure from gsl, and add tests to "it" instead of write
  * our own.  This would prevent us from having to mess with porting
  * any we discover that are "good" to the gsl...
  */
 if(testnum>=num_gsl_rngs) {
   hbtestnum = testnum - num_gsl_rngs;
   printf(" testnum = %d, hbtestnum = %d\n",testnum,hbtestnum);
 } else {
   /*
    * Initialize the selected gsl rng
    */
   random = gsl_rng_alloc (types[testnum]);
   random_max = gsl_rng_max(random);
   seed = random_seed();
   gsl_rng_set(random,seed);
 }

 if(verbose == TST_RNG){
   if(testnum >= num_gsl_rngs) {
     printf("hbtestnum = %d\n",hbtestnum);
     printf("generator type: %s\n", homebrews[hbtestnum]);
   } else {
     printf("generator type: %s\n", gsl_rng_name(random));
     printf("seed value: %u\n", seed);
     printf("max value = %u\n", random_max);
     for(i = 0;i<15;i++) printf("%d:  %u\n", i, gsl_rng_get(random));
   }
   exit(0);
 }

 /*
  * We now need to allocate space for at least one vector:  one
  * for 32-bit integer deviates and/or a vector of 64-bit uniform
  * deviates.  This will need to be done from information passed on
  * the command line as to the size (number of ints, bits, doubles).
  *
  * We'll start by using "size" for this, and do just ints (into
  * the vector rand_int[]).
  */
 if(size < 1000) {
   fprintf(stderr,"Warning:  size = %d too small, using size = 1000\n",size);
   size = 1000;
 }
 rand_int = (int *) malloc((size_t) (size*sizeof(unsigned int)));

}
