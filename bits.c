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
 * This should be the only tool we use to access bit substrings
 * from now on.  Note that we write our own bitstring access tools
 * instead of using ldap's LBER (Basic Encoding Rules) library call
 * to both retain control and because it would introduce a slightly
 * exotic dependency in the resulting application.
 *
 * bstring is a pointer to the uint string to be parsed.  It is a uint
 * pointer to make it easy to pass arbitrary strings which will generally
 * be e.g. unsigned ints in rand_rate but might be other data types
 * in other applications (might as well make this semi-portable while I'm
 * writing it at all).  bslen is the length of bitstring in uints.  blen is
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
unsigned int get_bit_ntuple(unsigned int *bitstring,
         unsigned int bslen,unsigned int blen,unsigned int boffset)
{

 unsigned int b,rlen;
 int ioffset;
 unsigned int result,carry,nmask;

 /*
  * Some tests for failure or out of bounds.  8*blen has to be less than
  * sizeof(uint).
  */
 if(blen > 8*sizeof(uint)) blen = 8*sizeof(uint);
 /*
  * Now that blen is sane, we can make a mask for the eventual return
  * value of length blen.
  */
 nmask = 1;
 for(b=0;b<blen-1;b++) {
   nmask = nmask <<1;
   nmask++;
 }
 
 if(verbose == D_BITS || verbose == D_ALL){
   printf("bslen = %u, blen = %u, boffset = %u\n",bslen,blen,boffset);
   printf("nmask =                   ");
   dumpbits(&nmask,32);
 }

 /*
  * This is the index of bitstring[] containing the first bit to
  * be returned, for example if boffset is 30, rmax_bits is 24,
  * and bslen (the length of the uint bitstring) is 4 (indices
  * run from 0-3) then ioffset is 4 - 1 -1 = 2 and the 30th bit
  * from the RIGHT is to be found in bitstring[2]. Put this uint
  * into result to work on further.
  */
 ioffset = bslen - (uint) boffset/rmax_bits - 1;
 result = bitstring[ioffset];
 if(verbose == D_BITS || verbose == D_ALL){
   printf("bitstring[%d] = %u\n",ioffset,result);
   printf("Initial result =          ");
   dumpbits(&result,32);
 }
 /*
  * Now, WHICH bit in result is the boffset relative to ITS
  * rightmost bit?  We have to find the modulus boffset%rmax_bits.
  * For example 30%24 = 6, so in the continuing example it would
  * be bit 6 in result.
  */
 boffset = boffset%rmax_bits;
 if(verbose == D_BITS || verbose == D_ALL){
   printf("Shifting to bit offset %u\n",boffset);
 }

 /*
  * Now for the easy part.  We shift right boffset bits.
  */
 for(b=0;b<boffset;b++) result = result >> 1;
 if(verbose == D_BITS || verbose == D_ALL){
   printf("Shifted result =          ");
   dumpbits(&result,32);
 }

 /*
  * rlen is the cumulated length of result.  Initially, it is set to
  * rmax_bits - boffset, the number of bits result can now contribute from
  * boffset.  We now have to loop, adding bits from uints up the list
  * (cyclic) until we get enough to return blen.  Note that we have to
  * loop because (for example) blen = 32, rmax_bits = 16, boffset = 30
  * would start in bitstring[2] and get 2 bits (30 and 31), get all 16 bits
  * from bitstring[1], and still need 12 bits of bitstring[0] to return.
  */
 rlen = rmax_bits - boffset;
 if(verbose == D_BITS || verbose == D_ALL){
   printf("Cumulated %u signifcant bits\n",rlen);
 }

 while(blen > rlen){
   /*
    * If we get here, we have to use either bitstring[ioffset-1] or
    * bitstring[bslen-1], shifted and filled into result starting
    * at the correct slot.  Put this into carry to work on.
    */
   ioffset--;
   if(ioffset < 0) ioffset = bslen-1;
   carry = bitstring[ioffset];
   if(verbose == D_BITS || verbose == D_ALL){
     printf("bitstring[%d] = %u\n",ioffset,carry);
     printf("Next carry =              ");
     dumpbits(&carry,32);
   }

   /*
    * This is tricky!  Shift carry left until the first bit lines
    * up with the first empty bit in result, which we'll hope is
    * the current rlen bit.
    */
   for(b=0;b<rlen;b++){
     carry = carry << 1;
   }
   if(verbose == D_BITS || verbose == D_ALL){
     printf("Shifted carry =           ");
     dumpbits(&carry,32);
   }

   /*
    * Now we simply add result and carry AND increment rlen by
    * rmax_bit (since this is the exact number of bits it adds
    * to rlen).
    */
   result += carry;
   rlen += rmax_bits;
   if(verbose == D_BITS || verbose == D_ALL){
     printf("Cumulated %u signifcant bits\n",rlen);
     printf("result+carry =            ");
     dumpbits(&result,32);
   }
 }

 result = result & nmask;
   if(verbose == D_BITS || verbose == D_ALL){
     printf("Returning Result =        ");
     dumpbits(&result,32);
     printf("==========================================================\n");
   }
 return(result);

}

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

