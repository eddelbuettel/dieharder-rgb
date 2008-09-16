/*
 *========================================================================
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 * ========================================================================
 * select_rng()
 *
 * This code can be executed by either the CLI
 * dieharder or rdieharder to select a generator based on EITHER generator
 * name (via a brute force search) OR generator number.  Number is more
 * convenient for automation and obviously faster, name is perhaps easier
 * to remember.  Note that it produces NO OUTPUT under normal operation
 * and returns -1 if it cannot find the generator however it was entered;
 * it is the responsibility of the UI to handle the error and warn the
 * user.
 *
 * The way it works is it checks name FIRST and if not null, it uses the
 * name it finds.  Otherwise it tries to use the number, where gennum=0
 * actually corresponds to a generator.  It does very minimal bounds
 * checking and will just return -1 if it falls through without finding
 * a generator corresponding to its arguments.
 * ========================================================================
 */

#include "dieharder.h"

/*
 */
int select_rng(int gennum,char *genname,uint initial_seed)
{

 /*
  * REALLY out of bounds we can just test for and return an error.
  */
 if(generator < 0 || generator >= MAXRNGS){
   return(-1);
 }

 /*
  * See if a generator name has been set (genname not null).  If
  * so, loop through all the generators in types looking for a
  * match and return a hit if there is one.  Note that this
  * routine just sets gennum and passes a (presumed valid)
  * gennum on for further processing, hence it has to be first.
  */
 if(genname){
   /* Not yet implemented (so any call an error)! */
   /* return -1 if non-null and no match found */
   return(-1);
 }

 /*
  * If we get here, then we are entering a generator type by number.
  * We check to be sure there is a generator with the given
  * number that CAN be used and return an error if there isn't.
  */
 if(types[gennum] == 0){
   return(-1);
 }

 /*
  * We need a sanity check for file input.  File input is permitted
  * iff we have a file name, if the output flag is not set, AND if
  * generator is either file_input or file_input_raw.  Otherwise
  * IF generator is a file input type (primary condition) we punt.
  *
  * For the moment we actually return an error message, but we may
  * want to pass the message back in a shared error buffer that
  * rdh can pick up or ignore, ditto dh, on a -1 return.
  */
 if(strncmp("file_input",types[gennum]->name,10) == 0){
   if(fromfile != 1){
     fprintf(stderr,"Error: generator %s uses file input but no filename has been specified",types[gennum]->name);
     return(-1);
   }
 }

 /*
  * If we get here, in principle the generator is valid and the right
  * inputs are defined to run it (in the case of file_input). We therefore
  * allocate the selected rng.  Note well that we seed it ONE TIME on the
  * basis of initial_seed.  If initial_seed is 0, we use random_seed().
  * otherwise we use whatever seed is passed.
  *
  * If the UI wants to enforce a particular seed, it therefore has two
  * ways to do so.  It can pass it this seed as initial_seed above and
  * ensure that the global Seed is set to zero, in which case no matter
  * how many tests are run for this rng it will not be reseeded unless
  * the UI code does it.
  *
  * Alternatively, it can just set Seed, which will cause the rng to be
  * reseeded/rewound at the beginning of any given test (sequence) with
  * the specified Seed.  Thus there are three distinct modes of operation:
  *
  *  randomly seeded one time (desireable for most real test runs so they
  *    are iid).
  *  specified one time (to reproduce a randomly seeded run, e.g.)
  *  specified at the beginning of each test (for e.g. validation).
  *
  * Of course the UI can reseed any time it wishes to anything it likes
  * as long as it leaves Seed=0 while it does so.  Seed=0 basically
  * means "don't reseed, the UI will do it" at any time after this
  * routine is called or using the one-time initial_seed argument above.
  */
 MYDEBUG(D_SEED){
   fprintf(stdout,"# startup(): Creating and seeding generator %s\n",types[gennum]->name);
 }
 rng = gsl_rng_alloc(types[gennum]);
 if(initial_seed == 0){
   /*
    * seed is still global, so it is accessible to e.g. output routines.
    * It is a good idea to make it part of the header report on a test
    * run so it can be reproduced if desired.
    */
   initial_seed = random_seed();
   seed = initial_seed;
   MYDEBUG(D_SEED){
     fprintf(stdout,"# startup(): Generating random seed %u\n",initial_seed);
   }
 }

 /*
  * Note well that this is either random or was specified as a local
  * argument above, no other choice.  It SETS the global seed, although of
  * course one could call it with seed as the argument.  It ignores Seed
  * altogether -- it is the UI's responsibility to figure out what and
  * how to handle setting it and thereby forcing the tests to reseed with
  * it at the beginning of the test.
  */
 gsl_rng_set(rng,initial_seed);

 /* It is indeed the same on looped calls, so this is really cruft.
 printf("Just for grins, the first rand returned by the generator is %lu\n",
  gsl_rng_get(rng));
 */

 /*
  * Before we quit, we must count the number of significant bits in the
  * rng AND create a mask.  Note that several routines in bits WILL NOT
  * WORK unless this is all set correctly, which actually complicates
  * things because I stupidly didn't make this data into components of the
  * rng object (difficult to do given that the latter is actually already
  * defined in the GSL, admittedly).
  */
 random_max = gsl_rng_max(rng);
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
  * If we get here, we are all happy, and return false (no error).
  */
 return(0);

}
