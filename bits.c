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

void cycle(unsigned int *data, unsigned int nbits)
{
 unsigned int i;
 unsigned int result,rbit,lmask,rmask;
 /*
  * We need two masks.  One is a mask of all the significant bits
  * in the rng, which may be as few as 24 and can easily be only
  * 31.
  *
  * The other is lmask, with a single bit in the position of the
  * leftmost significant bit.  We can make them both at once.
  */
 rmask = 1;
 lmask = 1;
 for(i=0;i<nbits-1;i++) {
  rmask = rmask << 1;
  rmask++;
  lmask = lmask << 1;
 }
 if(verbose){
   printf("%u bit rmask = ",nbits);
   dumpbits(&rmask,32);
   printf("%u bit lmask = ",nbits);
   dumpbits(&lmask,32);
 }
 /*
  * Next, we create mask the int being bit cycled into an internal
  * register.
  */
 result = *data & rmask;
 if(verbose){
   printf("Original int: ");
   dumpbits(data,32);
   printf("  Masked int: ");
   dumpbits(&result,32);
 }
 rbit = 1 & result;
 result = result >> 1;
 result += rbit*lmask;
 *data = result;
 if(verbose){
   printf(" Rotated int: ");
   dumpbits(data,32);
 }

}

/*
 * This should be the only tool we use to access bit substrings
 * from now on.  Note that we write our own bitstring access tools
 * instead of using ldap's LBER (Basic Encoding Rules) library call
 * to both retain control and because it would introduce a slightly
 * exotic dependency in the resulting application.
 *
 * bstring is a pointer to the char string to be parsed.  It is a char
 * pointer to make it easy to pass arbitrary strings which will generally
 * be e.g. unsigned ints in rand_rate but might be other data types
 * in other applications (might as well make this semi-portable while I'm
 * writing it at all).  bslen is the length of the bitstring (in characters)
 * included to enable cyclic wraparound of the returned value.  blen is
 * the length of the bitstring to be returned (as an unsigned int) and has
 * to be less than the length, in bits, of bitstring.  Finally, boffset
 * is the bit index of the point in the bitstring from which the result
 * will be returned.
 *
 * The only other thing that has to be defined is the direction from which
 * the bit offset and bit length are counted.  We will make the
 * LEAST significant bit offset 0, and take the string from the direction
 * of increasing signicance.  Examples:
 *
 *   bitstring:  10010110, length 1 (byte, char).
 * for offset 0, length 4 this should return: 0110
 *     offset 1, length 4: 1011
 *     offset 5, length 4: 0100 (note periodic wraparound)
 *     offset 6, length 4: 1010 (ditto)
 *
 * where of course the strings are actually returned as e.g.
 *     00000000000000000000000000000110  (unsigned int).
 */
unsigned int get_bit_ntuple(char *bitstring,
         unsigned int bslen,unsigned int blen,unsigned int boffset)
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
 /*
  * This is broken, I think.
 if(index >= size){
   fprintf(stderr,"Error: bit offset %d exceeds length %d of bitstring in rand[]\n",n,size*sizeof(unsigned int));
   exit(0);
 }
 */

 
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
 /*
  * This is broken, I think.
 if(index >= size){
   fprintf(stderr,"Error: bit offset %d exceeds length %d of bitstring in rand[]\n",n,size*sizeof(unsigned int));
   exit(0);
 }
 */

 
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
   fprintf(stderr,"Error: bit offset %d exceeds length %d of bitstring in rand[]\n",n,8*size*sizeof(unsigned int));
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

