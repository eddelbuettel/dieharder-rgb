/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
* See also accompanying file STS.COPYING
*
*/

/*
 *========================================================================
 * This just lists the available rng's.  This is the default action of
 * rand_rate when no test is specified.
 *========================================================================
 */

#include "rand_rate.h"

void list_rngs()
{
 int i;

 if(verbose){
   printf("list_rngs():\n");
 }
 
 printf("Listing available built-in gsl-linked generators:\n");
 printf("Test Number      Name\n");
 printf("================================\n");
 for(i=0;i<num_gsl_rngs;i++){
   printf(" %d\t\t%s\n", i, types[i]->name);
 }

 printf("Listing available non-gsl generators:\n");
 printf("Test Number      Name\n");
 printf("================================\n");
 while(types[i] != NULL){
   printf(" %d\t\t%s\n", i, types[i]->name);
   i++;
 }


}

