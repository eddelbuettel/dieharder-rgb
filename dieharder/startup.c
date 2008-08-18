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

#include "dieharder.h"

void startup()
{

 int i,imax,j,k;
 uint mask;
 struct stat sbuf;


 /*
  * The very first thing we do is see if any simple help options
  * were requested, as we exit immediately if they were and can
  * save all sorts of work.
  */
 if(list == YES) {
   list_tests();
   Exit(0);
 }

 /*
  * Set up the built-in gls generators.  We have to
  *
  *  a) keep the numbers the same from version to version.
  *
  *  b) permit calling rngs by name.
  *
  * Here we set up for this.  This means counting and adding and
  * optionally listing the available, built in gsl generators plus
  * any added generators, plus a parse routine for selecting a generator
  * from the command line.
  */
 types = dieharder_rng_types_setup ();
 i = 0;
 while(types[i] != NULL){
   i++;
 }
 num_gsl_rngs = i;

 /*
  * Now add the library gsl-wrapped generators
  */
 add_lib_rngs();
 while(types[i] != NULL){
   i++;
 }

 /*
  * If there are any, we can even add generators at the UI level
  */
 add_ui_rngs();
 while(types[i] != NULL){
   i++;
 }

 num_rngs = i;
 num_my_rngs = num_rngs - num_gsl_rngs;

 if(generator == -1){
   list_rngs();
   Exit(0);
 }

 if(generator > num_rngs-1){
   fprintf(stderr,"Error:  rng %d (> %d) does not exist!\n",generator,num_rngs-1);
   list_rngs();
   Exit(0);
 }

 /*
  * We need a sanity check for file input.  File input is permitted
  * iff we have a file name, if the output flag is not set, AND if
  * generator is either file_input or file_input_raw.  Otherwise
  * IF generator is a file input type (primary condition) we punt
  * with a list of types and a honk.
  */
 if(strncmp("file_input",types[generator]->name,10) == 0){
   if(fromfile != 1){
     fprintf(stderr,"Error: generator %s uses file input but no file has been loaded",types[generator]->name);
     list_rngs();
     Exit(0);
   }
   if(output){
     fprintf(stderr,"Error: generator %s uses file input but output flag set.",types[generator]->name);
     Usage();
     Exit(0);
   }
 }

 /*
  * If we get here, in principle the generator is valid and the right
  * inputs are defined to run it (in the case of file_input). We therefore
  * initialize the selected gsl rng using (if possible) random_seed()
  * seeds from /dev/random.  Note that we had to wait until now for the to
  * do this or we'd miss our own additions!
  */
 if(verbose == D_STARTUP || verbose == D_SEED || verbose == D_ALL){
   fprintf(stdout,"# startup(): Creating and seeding generator %s\n",types[generator]->name);
 }
 rng = gsl_rng_alloc(types[generator]);
 if(Seed == 0){
   seed = random_seed();
   if(verbose == D_STARTUP || verbose == D_SEED || verbose == D_ALL){
     fprintf(stdout,"# startup(): Generating random seed %u\n",seed);
   }
 } else {
   seed = Seed;
   if(verbose == D_STARTUP || verbose == D_SEED || verbose == D_ALL){
     fprintf(stdout,"# startup(): Setting random seed %u by hand.\n",seed);
   }
 }
 gsl_rng_set(rng,seed);

 /*
  * Simultaneously count the number of significant bits in the rng
  * AND create a mask.  Note that several routines in bits WILL NOT
  * WORK unless this is all set correctly, which actually complicates
  * things because I stupidly didn't make this data into components
  * of the rng object (difficult to do given that the latter is actually
  * already defined in the GSL, admittedly).
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

 if(output){
   output_rnds();
   /* We'll fix it so we don't have to exit here later, maybe. */
   Exit(0);
 }

 /*
  * TESTING PLAYGROUND.  bits.c code MUST be carefully validated as it is
  * quite complex and crucial to happy testing.  We cannot test this until
  * AFTER rng is set up.
  testbits();
  Exit(0);
  *
  */


 /*
  * This is the global vector of p-values generated by each test
  * run many times.  It has to be oversized because resizing it
  * to just right is more of a hassle then just spending the memory
  * in an era where systems with less than a GB of active memory
  * will be increasingly rare.  These p-values are ONLY used in
  * the end-stage e.g. KS tests that globally validate the distribution
  * of p-values returned by the test.  Set the kspi index to point to
  * the first element of the vector.
  *
  * I think the following is cruft
 ks_pvalue = (double *)malloc((size_t) KS_SAMPLES_PER_TEST_MAX*psamples*sizeof(double));
 ks_pvalue2 = (double *)malloc((size_t) KS_SAMPLES_PER_TEST_MAX*psamples*sizeof(double));
  *
  * and I'm HOPING that every test uses an absolute maximum of psamples
  * samples for its concluding KS test, although in the case of e.g. diehard
  * tests this is probably not always true.  I actually think that the
  * right way to do this at this point is to malloc the memory inside
  * the test itself and free it at the end.  Why not?
  */
 ks_pvalue = 0;
 ks_pvalue2 = 0;
 kspi = 0;

}
