/*
 *========================================================================
 *  rng_XOR.c
 *
 * This generator takes a list of generators on the dieharder
 * command line and XOR's their output together.
 *========================================================================
 */

#include <dieharder/libdieharder.h>

/*
 * This is a special XOR generator that takes a list of GSL
 * wrapped rngs and XOR's their uint output together to produce
 * each new random number.
 */
static unsigned long int XOR_get (void *vstate);
static double XOR_get_double (void *vstate);
static void XOR_set (void *vstate, unsigned long int s);

/*
 * internal gsl random number generator vector
 */
gsl_rng *grngs[GVECMAX];

typedef struct
  {
  }
XOR_state_t;

static inline unsigned long int
XOR_get (void *vstate)
{

}

static double
XOR_get_double (void *vstate)
{
  return XOR_get (vstate) / (double) UINT_MAX;
}

static void
XOR_set (void *vstate, unsigned long int s) {

 XOR_state_t *state = (XOR_state_t *) vstate;
 int i;

 /*
  * Basically, we need to go through the list and seed each generator.
  * If the list of seeds is empty (a single seed is all that was
  * read in) we use that seed to seed all the generators on the list.
  * Otherwise we use the vector of seeds.  This isn't necessarily the
  * "best" way to do things -- we'll have to see once we get the whole
  * thing working.
  */
 for(i=0;i<gvcount;i++){

   grngs[i] = gsl_rng_alloc(dh_rng_types[gnumbs[i]]);

   /*
    * We really don't need to worry about the value of strategy here, just
    * Seed.  If it is is 0 we reseed randomly, otherwise we PROceed.  We
    * actually seed from the variable seed, not Seed (which then remembers
    * the value as long as it remains valid).
    */
   if(Seed == 0){
     seed = random_seed();
     MYDEBUG(D_SEED){
       fprintf(stdout,"# choose_rng(): Generating random seed %u\n",seed);
     }
   } else {
     seed = gseeds[i];
     MYDEBUG(D_SEED){
       fprintf(stdout,"# choose_rng(): Setting fixed seed %u\n",seed);
     }
   }

   /*
    * Set the seed.  We do this here just so it is set for the timing
    * test.  It may or may not ever be reset.
    */
   gsl_rng_set(grngs[i],seed);

 }

}

static const gsl_rng_type XOR_type =
{"XOR",                         /* name */
 UINT_MAX,			/* RAND_MAX */
 0,				/* RAND_MIN */
 sizeof (XOR_state_t),
 &XOR_set,
 &XOR_get,
 &XOR_get_double};

const gsl_rng_type *gsl_rng_XOR = &XOR_type;
