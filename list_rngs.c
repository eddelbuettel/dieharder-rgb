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
 
 printf("              Listing available built-in gsl-linked generators:           |\n");
 printf("%3s %-20s|%3s %-20s|%3s %-20s|\n"," Id","Test Name",
         " Id","Test Name"," Id","Test Name");
 printf("==========================================================================|\n");
 for(i=0;i<num_gsl_rngs;i++){
   printf("%3d %-20s|", i, types[i]->name);
   if(((i+1)%3) == 0 && i>0) printf("\n");
 }
 printf("                   Listing available non-gsl generators:                  |\n");
 printf("%3s %-20s|%3s %-20s|%3s %-20s|\n"," Id","Test Name",
         " Id","Test Name"," Id","Test Name");
 printf("==========================================================================|\n");
 while(types[i] != NULL){
   printf("%3d %-20s|", i, types[i]->name);
   if(((i+1)%3) == 0 && i>0) printf("\n");
   i++;
 }
 printf("\n");

}
