/*
 * $Id: rgb_timing.c 142 2005-03-11 02:56:31Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 * See also accompanying file STS.COPYING
 *
 */

/*
 *========================================================================
 * This is not a test -- this just times the rng.
 *========================================================================
 */

#include "dieharder.h"

double rgb_timing()
{

 double total_time,avg_time;
 int i,j,nsamples = 100;
 unsigned int rand_uint[100000];

 if(!quiet){
   help_rgb_timing();
 }

 seed = random_seed();
 gsl_rng_set(rng,seed);

 total_time = 0.0;
 for(i=0;i<nsamples;i++){
   start_timing();
   for(j=0;j<100000;j++){
     rand_uint[j] = gsl_rng_get(rng);
   }
   stop_timing();
   total_time += delta_timing();
 }
 avg_time = total_time/(nsamples*100000);
 
 if(!quiet){
   printf("#==================================================================\n");
   printf("# rgb_timing() test using the %s generator \n",gsl_rng_name(rng));
   printf("# Average time per rand = %e nsec.\n",avg_time*1.0e+9);
   printf("# Rands per second = %e.\n",1.0/avg_time);
 }
 
}

void help_rgb_timing()
{

 if(!quiet){
   printf("#==================================================================\n");
   printf("#                        rgb_timing\n");
   printf("# This test times the selected random number generator, only.\n");
   printf("#==================================================================\n");
 }

}
