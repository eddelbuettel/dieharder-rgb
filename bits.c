/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * This should be a nice, big case switch where we add EACH test
 * we might want to do and either just configure and do it or
 * prompt for input (if absolutely necessary) and then do it.
 *========================================================================
 */

#include "rand_rate.h"

/*
 * dumpbits only can dump 8*sizeof(unsigned int) bits at a time.
 */
void dumpbits(unsigned int *data, unsigned int nbits)
{

 int i,j;
 unsigned int mask;

 if(nbits > 8*sizeof(unsigned int)) {
   nbits = 8*sizeof(unsigned int);
 }
 
 mask = (int)pow(2,nbits);
 for(i=0;i<nbits;i++){
   if(mask & *data){
     printf("1");
   } else {
     printf("0");
   }
   mask = mask >> 1;
 }
 printf("\n");
}

/*
 * This is still a good idea, but we have to modify it so that it ONLY
 * gets VALID bits by their absolute index.
 */
int get_bit(unsigned int n)
{

 int i;
 unsigned int index,offset,mask;
 static unsigned last_rand;

 /*
  * This routine is designed to get the nth VALID bit of the global 
  * unsigned int vector rand_int[].  The indexing is a bit tricky.
  * index tells us which vector element contains the bit being sought:
  */
 index = (int) (n/rmax_bits);
 if(index >= size){
   fprintf(stderr,"Error: bit offset %d exceeds length %d of bitstring in rand[]\n",n,size*sizeof(unsigned int));
   exit(0);
 }
 /*
  * Then we have to compute the offset of the bit desired, starting from
  * the first significant/valid bit in the unsigned int.
  */
 offset = (8*sizeof(unsigned int) - rmax_bits) + n%rmax_bits;
 mask = (int)pow(2,8*sizeof(unsigned int) - 1);
 mask = mask>>offset;
 if(mask & rand_int[index]){
   return(1);
 } else {
   return(0);
 }
 
}

/*
 * dumpbits only can dump 8*sizeof(unsigned int) bits at a time.
 */
void dumpbits_left(unsigned int *data, unsigned int nbits)
{

 int i,j;
 unsigned int mask;

 if(nbits > 8*sizeof(unsigned int)) {
   nbits = 8*sizeof(unsigned int);
 }
 
 mask = 1;
 for(i=0;i<nbits;i++){
   if(mask & *data){
     printf("1");
   } else {
     printf("0");
   }
   mask = mask << 1;
 }
 printf("\n");
}

/*
 * This is still a good idea, but we have to modify it so that it ONLY
 * gets VALID bits by their absolute index.
 */
int get_bit_left(unsigned int n)
{

 int i;
 unsigned int index,offset,mask;
 static unsigned last_rand;

 /*
  * This routine is designed to get the nth VALID bit of the global 
  * unsigned int vector rand_int[].  The indexing is a bit tricky.
  * index tells us which vector element contains the bit being sought:
  */
 index = (int) (n/rmax_bits);
 if(index >= size){
   fprintf(stderr,"Error: bit offset %d exceeds length %d of bitstring in rand[]\n",n,size*sizeof(unsigned int));
   exit(0);
 }
 /*
  * Then we have to compute the offset of the bit desired, starting from
  * the first significant/valid bit in the unsigned int.
  */
 offset = n%rmax_bits;
 mask = 1;
 mask = mask<<offset;
 if(mask & rand_int[index]){
   return(1);
 } else {
   return(0);
 }
 
}

