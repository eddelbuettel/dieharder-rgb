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
static int firstcall=1;

void startup_ui()
{

 /*
  * This is temporary (to keep things from being overtly broken).
  * The short term goal is to insert code here that calls
  * select_generator() by name if/when a user selects a generator
  * by name on the CL, by number if they select a generator by
  * number on the CL or fall through to the default.  Note that
  * by using 0 for the seed, we force the selection of a random
  * seed.  AFTERWARDS there is fallthrough reimplementation of
  * the Seed strategy.
  */
 if(select_rng(generator,(char *)NULL,0)){
   fprintf(stderr,"Error: generator %d not found.\n",generator);
   list_rngs();
   Exit(0);
 }


 /*
  * This may or may not belong here.  We may move it somewhere else
  * if it needs to go.  But it should WORK here, at least for the moment.
  */
 if(output_file){
   output_rnds();
   Exit(0);
 }


}
