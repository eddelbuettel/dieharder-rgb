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

unsigned long int random_seed()
{

 unsigned int seed;
 FILE *devrandom;

 if ((devrandom = fopen("/dev/random","r")) == NULL) {
   fprintf(stderr,"Cannot open /dev/random, setting seed to 0\n");
   seed = 0;
 } else {
   fread(&seed,sizeof(seed),1,devrandom);
   if(verbose == D_SEED) printf("Got seed %u from /dev/random\n",seed);
   fclose(devrandom);
 }

 return(seed);

}
