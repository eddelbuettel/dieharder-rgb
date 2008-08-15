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
 * dieharder when no test is specified.
 *========================================================================
 */

#include "dieharder.h"

void list_rngs()
{
 int i,j;

 if(verbose){
   printf("list_rngs():\n");
 }
 /* Version string seems like a good idea */
 printf("\n\
dieharder version %s Copyright 2003 Robert G. Brown\n\
 \n",QUOTEME(VERSION));
 printf("            Listing available built-in gsl-linked generators:             |\n");
 printf("%3s %-20s|%3s %-20s|%3s %-20s|\n"," Id","Test Name",
         " Id","Test Name"," Id","Test Name");
 printf("==========================================================================|\n");
 for(i=0;i<num_gsl_rngs;i++){
   printf("%3d %-20s|", i, types[i]->name);
   if(((i+1)%3) == 0 && i>0) printf("\n");
 }
 if(j = (i%3) != 0) printf("\n");
 printf("                   Listing available non-gsl generators:                  |\n");
 printf("%3s %-20s|%3s %-20s|%3s %-20s|\n"," Id","Test Name",
         " Id","Test Name"," Id","Test Name");
 printf("==========================================================================|\n");
 while(types[i] != NULL){
   printf("%3d %-20s|", i, types[i]->name);
   if(((i-j)%3) == 0 && i>0) printf("\n");
   i++;
 }
 printf("\n\n");

}

