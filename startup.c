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

void rand_rate_startup()
{

 int i,imax,j,k;

 /*
  * Count and optionally list the available, built in gsl generators
  */
 types = gsl_rng_types_setup ();
 i = 0;
 while(types[i] != NULL){
   i++;
 }
 num_gsl_rngs = i;

 /*
  * Now add my own types and count THEM.
  */
 add_my_types();
 while(types[i] != NULL){
   i++;
 }

 num_rngs = i;
 num_my_rngs = num_rngs - num_gsl_rngs;

 if(randnum > num_rngs-1){
   fprintf(stderr,"Error:  rng %d (> %d) does not exist!\n",randnum,num_rngs-1);
   list_rngs();
   exit(0);
 }

 /*
  * Initialize the selected gsl rng.  random_seed() seeds from
  * /dev/random.  Note that any locally defined rng's were "added"
  * to the gsl set above and can now be called with the gsl
  * wrapper!
  */
 random = gsl_rng_alloc (types[randnum]);
 random_max = gsl_rng_max(random);
 seed = random_seed();
 gsl_rng_set(random,seed);

 /*
  * We now need to allocate space for at least one vector:  one
  * for unsigned int integer deviates and/or a vector of (double) uniform
  * deviates.  This will need to be done from information passed on
  * the command line as to the size or number of bits.  size indicates
  * the length of the vectors directly; bits indicates the number of
  * bits to be tested (in bit tests), which then determines the size.
  *
  * HOWEVER, we have to correct (either way) for the number of bits that
  * are SIGNIFICANT or VALID in the given rng, derived from random_max.
  * This isn't particularly simple to do, since random_max is not always
  * power of 2 (in particular see ran3, for example).  We have to count
  * the number of signficant bits returned by the rng and adjust our
  * bits and size allocations accordingly.
  */
 if(size < 1) {
   fprintf(stderr,"Warning:  size = %d too small, using size = 1\n",size);
   size = 1;
 }

 /*
  * Simultaneously count the number of significant bits in the rng
  * AND create a mask (which we need in e.g. rgb_persist and possibly
  * elsewhere).
  */
 rmax = random_max;
 rmax_bits = 0;
 rmax_mask = 0;
 while(rmax){
   rmax >>= 1;
   rmax_mask = rmax_mask << 1;
   rmax_mask++;
   rmax_bits++;
 }

 /*
  * Now, if bits were specified, we need to allocate a size large enough
  * to accumulate at least enough bits, given the number of VALID bits
  * per unsigned int.  If size was specified, we compute the exact number
  * of valid bits it will contain (which might NOT be
  * size*sizeof(unsigned int)).
  */
 if(bits){
   size = bits/rmax_bits + 1;
 } else {
   bits = size*rmax_bits;
 }
		
 rand_int = (int *) malloc((size_t) (size*sizeof(unsigned int)));

}
