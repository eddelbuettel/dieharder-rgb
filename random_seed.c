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
 struct timeval tv;
 FILE *devrandom;

 if ((devrandom = fopen("/dev/random","r")) == NULL) {
   gettimeofday(&tv,0);
   seed = tv.tv_sec + tv.tv_usec;
   if(verbose == D_SEED) printf("Got seed %u from gettimeofday()\n",seed);
 } else {
   fread(&seed,sizeof(seed),1,devrandom);
   if(verbose == D_SEED) printf("Got seed %u from /dev/random\n",seed);
   fclose(devrandom);
 }

 return(seed);

}
