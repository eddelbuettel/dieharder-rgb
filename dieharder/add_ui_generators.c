/*
 *========================================================================
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * generators.c contains all the code that sets up random number
 * generators from the GSL, from libdieharder, from R, and any "user
 * contributed" generators so that they can easily be used.  Each routine
 * is built to be output-free (and shareable with rdh or other UIs)
 *========================================================================
 */

#include "dieharder.h"

/*
 * Set up the built-in GSL-wrapped generators.  We have to
 *
 *  a) keep the numbers the same from version to version.
 *
 *  b) (eventually) permit calling rngs by name.
 *
 * The code below loads rngs into predefined RANGES in a
 * large types[] vector to where they can easily be selected
 * by number or name.  Note that this load preserves the order
 * of GSL-defined generators with the GSL numbers, which CAN
 * CHANGE.  We can only lock down the numbers of "our own"
 * added generators.
 *
 */
void add_ui_rngs()
{

 int i;

 /*
  * If you are hacking dieharder to add your own rng, add it below using
  * the "empty_random" example as a template (right AFTER the example) and
  * it should "just appear" in the dieharder list of available generators
  * and be immediately useful.  Note that the range 600-699 is currently
  * reserved "only" for UI rngs.
  */
 i = 600;
 dh_num_user_rngs = 0;
 /* Template from here: */
 types[i] = gsl_rng_empty_random;
 i++;
 dh_num_user_rngs++;
 dh_num_rngs++;
 /* to here.*/
 MYDEBUG(D_STARTUP){
   printf("# startup:  Found %u user interface generators.\n",dh_num_user_rngs);
   printf("# startup:  A total of %u generators available.\n",dh_num_rngs);
 }

 /*
  * If you are writing an entirely new UI, e.g. to add dieharder
  * functionality to things such as octave or a new GUI, yes, there is
  * a reserved range for you as well -- 700-999 is for custom UI rngs that
  * don't fit into any of the categories above.  Just block copy the
  * enter user example and set i = 700 to start and increment the
  * global dh_num_reserved_rngs counter.
  */
  
}

