/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
* See also accompanying file STS.COPYING
*
*/

/*
 *========================================================================
 * This is the Diehard 3d spheres test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 * Basically, we choose 4000 points in a cube of side 1000.  Compute the
 * smallest nearest neighbor distance (radius R of the smallest sphere
 * that doesn't overlap any neighboring point). R^3 is exponentially
 * distributed with an empirical exponential distribution with mean 30.
 * Thus p = 1.0 - exp(-R^3/30.0) should be a uniform distribution.  Run
 * a KS test on a vector of independent samples of this entire test to
 * find out.
 *========================================================================
 */


#include "rand_rate.h"


int diehard_3dsphere()
{

 int i,j,k;

 if(!quiet){
   printf("#==================================================================\n");
   printf("#              Diehard \"3d Sphere\" test (modified).\n");
   printf("# Generate 4000 points in a 1000^3 integer cube.  Determine the\n");
   printf("# the shortest nearest neighbor distance R.  This should generate\n");
   printf("# p = 1.0 - exp(-R^3/30).  Repeat for lots of samples, apply a\n");
   printf("# KS test to see if p is uniform.\n");
   printf("#==================================================================\n");
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# size of vector tested = %u\n",size);
 }

}

