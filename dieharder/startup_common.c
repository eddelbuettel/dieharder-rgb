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

void startup_common()
{

 /*
  * This fills the global *types (which points to the global dh_types[]
  * defined in libdieharder) with all the rngs known directly to dieharder
  * including known hardware generators and file input "generators".  This
  * routine also sets the counts of each "kind" of generator into
  * global/shared variables.  It must be run BEFORE adding any UI
  * generators, and BEFORE selecting a generator or input stream to test.
  */
 types = dieharder_rng_types_setup();

}
