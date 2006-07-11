/*
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is the Diehard Count a Stream of 1's test, rewritten from the
 * description in tests.txt on George Marsaglia's diehard site.
 *
 * Rewriting means that I can standardize the interface to
 * gsl-encapsulated routines more easily.  It also makes this
 * my own code.  Finally, since the C versions Marsaglia provides
 * are the result of f2c running on Fortran sources, they are really
 * ugly code and the rewrite should be much more manageable.
 *
 * Here is the test description from diehard_tests.txt:
 *
 *     This is the COUNT-THE-1's TEST on a stream of bytes.      ::
 * Consider the file under test as a stream of bytes (four per   ::
 * 32 bit integer).  Each byte can contain from 0 to 8 1's,      ::
 * with probabilities 1,8,28,56,70,56,28,8,1 over 256.  Now let  ::
 * the stream of bytes provide a string of overlapping  5-letter ::
 * words, each "letter" taking values A,B,C,D,E. The letters are ::
 * determined by the number of 1's in a byte::  0,1,or 2 yield A,::
 * 3 yields B, 4 yields C, 5 yields D and 6,7 or 8 yield E. Thus ::
 * we have a monkey at a typewriter hitting five keys with vari- ::
 * ous probabilities (37,56,70,56,37 over 256).  There are 5^5   ::
 * possible 5-letter words, and from a string of 256,000 (over-  ::
 * lapping) 5-letter words, counts are made on the frequencies   ::
 * for each word.   The quadratic form in the weak inverse of    ::
 * the covariance matrix of the cell counts provides a chisquare ::
 * test::  Q5-Q4, the difference of the naive Pearson sums of    ::
 * (OBS-EXP)^2/EXP on counts for 5- and 4-letter cell counts.    ::
 *========================================================================
 */


#include "dieharder.h"

double diehard_count_1s_stream()
{

 double *pvalue,pks;
 uint tempsamples;

 /*
  * This is the merest shell to set any test-specific variables, call
  * the main test routine (which fills one or more slots in ks_pvalue[]
  * and increments kspi accordingly), and run a Kuiper Kolmogorov-Smirnov
  * test on the vector of pvalues produced and turn it into a single,
  * cumulative p-value (pks) for the entire test.  If the test parameters
  * are set properly, this will USUALLY yield an unambiguous signal of
  * failure.
  */

 /*
  * If this test is run by itself, we can ignore tsamples.  If it is
  * part of a "standard run", we have to use specific values.  Either
  * way, we have to adjust the sizes of e.g. the list of integers to
  * be generated and sampled, and (re)allocate memory accordingly.
  * Then at the bottom, we have to put it all back.
  */
 if(all == YES){
   tempsamples = tsamples;
   tsamples = 2560000;  /* Standard value from diehard */
 }

 if(!quiet){
   help_diehard_count_1s_stream();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of rands required is around 3x10^8 for 100 samples.\n");
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_count_1s_stream_test);

 /*
  * Display histogram of ks p-values (optional)
  */
 if(hist_flag){
   histogram(ks_pvalue,psamples,0.0,1.0,10,"p-values");
 }
 printf("# p = %8.6f for diehard_count_1s_stream test (mean) from Kuiper Kolmogorov-Smirnov\n",pks);
 printf("#     test on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for diehard_count_1s_stream.\n",gsl_rng_name(rng));
 }

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = tempsamples;
 }

 return(pks);

}

/*
 * This table was generated using the following code fragment.
 {
   char table[256];
   table[i] = 0;
   for(j=0;j<8*sizeof(uint);j++){
     table[i] += get_int_bit(i,j);
   }
   switch(table[i]){
     case 0:
     case 1:
     case 2:
       table[i] = 0;
       break;
     case 3:
       table[i] = 1;
       break;
     case 4:
       table[i] = 2;
       break;
     case 5:
       table[i] = 3;
       break;
     case 6:
     case 7:
     case 8:
       table[i] = 4;
       break;
     default:
       fprintf(stderr,"Hahahahah\n");
       exit(0);
       break;
   }
 }
 */

char b5[] = {
0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 2,
0, 0, 0, 1, 0, 1, 1, 2, 0, 1, 1, 2, 1, 2, 2, 3,
0, 0, 0, 1, 0, 1, 1, 2, 0, 1, 1, 2, 1, 2, 2, 3,
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
0, 0, 0, 1, 0, 1, 1, 2, 0, 1, 1, 2, 1, 2, 2, 3,
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 4,
0, 0, 0, 1, 0, 1, 1, 2, 0, 1, 1, 2, 1, 2, 2, 3,
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 4,
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 4,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 4,
2, 3, 3, 4, 3, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4};

/*
 * The following are needed to generate the test statistic.
 */

const double mu=2500, std=70.7106781;
/*
 * Vector of probabilities for each integer.
 * 37.0/256.0,56.0/256.0,70.0/256.0,56.0/256.0,37.0/256.0
 */
const double p[]={
0.144531250,
0.218750000,
0.273437500,
0.218750000,
0.144531250};

#define LSHIFT5(old,new) (old*5 + new)

void diehard_count_1s_stream_test()
{

 uint i,j,k,index5,index4,letter,t;
 uint boffset;
 uint count5[3125],count4[625];
 Btest btest4,btest5;
 Xtest ptest;

 /*
  * Count a Stream of 1's is a very complex way of generating a statistic.
  * We take a random stream, and turn it bytewise (overlapping or not)
  * into a 4 and/or 5 digit base 5 integer (in the ranges 0-624 and 0-3124
  * respectively) via the bytewise mapping in b5[] above, derived from the
  * prescription of Marsaglia in diehard.  Increment a vector for 4 and 5
  * digit numbers separately that counts the number of times that 4, 5
  * digit integer has occurred in the random stream.  Compare these
  * vectors to their expected values, generated from the probabilities of
  * the occurrence of each base 5 integer in the byte map.  Compute chisq
  * for each of these vectors -- the "degrees of freedom" of the stream
  * thus mapped..  The difference between chisq(5) and chisq(4)should be
  * 5^4 - 5^3 = 2500 with stddev sqrt(5000), use this in an Xtest to
  * generate the final trial statistic.
  */

 /*
  * I'm leaving this in so the chronically bored can validate that
  * the table exists and is correctly loaded and addressable.
  */
 if(verbose == -1){
   for(i=0;i<256;i++){
     printf("%u, ",b5[i]);
     /* dumpbits(&i,8); */
     if((i+1)%16 == 0){
       printf("\n");
     }
   }
   exit(0);
 }

 /*
  * This is basically a pair of parallel btests, with a final test
  * statistic generated from their difference (somehow).  We therefore
  * create two btests, one for four digit base 5 integers and one for
  * five digit base 5 integers, and generate their expected values for
  * tsamples trials.
  */

 ptest.y = 2500.0;
 ptest.sigma = sqrt(5000.0);

 Btest_create(&btest4,625,"diehard_count_the_1s",gsl_rng_name(rng));
 for(i=0;i<625;i++){
   j = i;
   btest4.y[i] = tsamples;
   btest4.x[i] = 0.0;
   /*
    * Digitize base 5, compute expected value for THIS integer i.
    */
   /* printf("%u:  ",j); */
   for(k=0;k<4;k++){
     /*
      * Take the least significant "letter" of j in range 0-4
      */
     letter = j%5;
     /*
      * multiply by the probability of getting this letter
      */
     btest4.y[i] *= p[letter];
     /*
      * Right shift j to get next digit.
      */
     /* printf("%1u",letter); */
     j /= 5;
   }
   /* printf(" = %f\n",btest4.y[i]); */
   btest4.sigma[i] = 0.0;
 }

 Btest_create(&btest5,3125,"diehard_count_the_1s",gsl_rng_name(rng));
 for(i=0;i<3125;i++){
   j = i;
   btest5.y[i] = tsamples;
   btest5.x[i] = 0.0;
   /*
    * Digitize base 5, compute expected value for THIS integer i.
    */
   for(k=0;k<5;k++){
     /*
      * Take the least significant "letter" of j in range 0-4
      */
     letter = j%5;
     /*
      * multiply by the probability of getting this letter
      */
     btest5.y[i] *= p[letter];
     /*
      * Right shift j to get next digit.
      */
     j /= 5;
   }
   btest5.sigma[i] = 0.0;
 }

 /*
  * Preload index with the four bytes of the first rand if overlapping
  * only.
  */
 if(overlap){
   i = gsl_rng_get(rng);
   if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
     dumpbits(&i,32);
   }
   /* 1st byte */
   j = get_bit_ntuple(&i,1,8,0);
   index5 = b5[j];
   if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
     printf("b5[%u] = %u, index5 = %u\n",j,b5[j],index5);
     dumpbits(&j,8);
   }
   /* 2nd byte */
   j = get_bit_ntuple(&i,1,8,8);
   index5 = LSHIFT5(index5,b5[j]);
   if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
     printf("b5[%u] = %u, index5 = %u\n",j,b5[j],index5);
     dumpbits(&j,8);
   }
   /* 3rd byte */
   j = get_bit_ntuple(&i,1,8,16);
   index5 = LSHIFT5(index5,b5[j]);
   if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
     printf("b5[%u] = %u, index5 = %u\n",j,b5[j],index5);
     dumpbits(&j,8);
   }
   /* 4th byte */
   j = get_bit_ntuple(&i,1,8,24);
   index5 = LSHIFT5(index5,b5[j]);
   if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
     printf("b5[%u] = %u, index5 = %u\n",j,b5[j],index5);
     dumpbits(&j,8);
   }
 }

 boffset = 0;
 for(t=0;t<tsamples;t++){

   /*
    * OK, we now have to do a simple modulus decision as to whether or not
    * a new random uint is required AND to track the byte offset.  We also
    * have to determine whether or not to use overlapping bytes.  I
    * actually think that we may need to turn index5 into a subroutine
    * where each successive call returns the next morphed index5,
    * overlapping or not.
    */
   if(overlap){
     /*
      * Use overlapping bytes to generate the next index5 according to
      * the diehard prescription (designed to work with a very small
      * input file of rands).
      */
     if(boffset%32 == 0){
       /*
        * We need a new rand to get our next byte.
        */
       boffset = 0;
       i = gsl_rng_get(rng);
       if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
         dumpbits(&i,32);
       }
     }
     /*
      * get next byte from the last rand we generated.
      */
     j = get_bit_ntuple(&i,1,8,boffset);
     index5 = LSHIFT5(index5,b5[j]);
     /*
      * I THINK that this basically throws away the sixth digit in the
      * left-shifted base 5 value, keeping the value of the 5-digit base 5
      * number in the range 0 to 5^5-1 or 0 to 3124 decimal.
      */
     index5 = index5%3125;
     if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
       printf("b5[%u] = %u, index5 = %u\n",j,b5[j],index5);
       dumpbits(&j,8);
     }
     boffset+=8;
   } else {
     /*
      * Get the next five bytes and make an index5 out of them, no
      * overlap.
      */
     for(k=0;k<5;k++){
       if(boffset%32 == 0){
         /*
	  * We need a new rand to get our next byte.
	  */
         boffset = 0;
         i = gsl_rng_get(rng);
         if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
           dumpbits(&i,32);
         }
       }
       /*
        * get next byte from the last rand we generated.
        */
       j = get_bit_ntuple(&i,1,8,boffset);
       index5 = LSHIFT5(index5,b5[j]);
       if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
         printf("b5[%u] = %u, index5 = %u\n",j,b5[j],index5);
         dumpbits(&j,8);
       }
       boffset+=8;
     }
   }
   /*
    * We use modulus to throw away the sixth digit in the left-shifted
    * base 5 index value, keeping the value of the 5-digit base 5 number
    * in the range 0 to 5^5-1 or 0 to 3124 decimal.  We repeat for the
    * four digit index.  At this point we increment the counts for index4
    * and index5.  Tres simple, no?
    */
   index5 = index5%3125;
   index4 = index5%625;
   btest4.x[index4]++;
   btest5.x[index5]++;
 }
 /*
  * OK, all that is left now is to figure out the statistic.
  */
 if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
   for(i = 0;i<625;i++){
     printf("%u:  %f    %f\n",i,btest4.y[i],btest4.x[i]);
   }
   for(i = 0;i<3125;i++){
     printf("%u:  %f    %f\n",i,btest5.y[i],btest5.x[i]);
   }
 }
 Btest_eval(&btest4);
 Btest_eval(&btest5);
 if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
   printf("btest4.chisq = %f   btest5.chisq = %f\n",btest4.chisq,btest5.chisq);
 }
 ptest.x = btest5.chisq - btest4.chisq;
 Xtest_eval(&ptest);

 ks_pvalue[kspi] = ptest.pvalue;
 if(verbose == D_DIEHARD_COUNT_1S_STREAM || verbose == D_ALL){
   printf("# diehard_count_1s_stream_freq(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;

}

void help_diehard_count_1s_stream()
{

 printf("\n\
#==================================================================\n\
#            Diehard \"count_1s_stream\" test (modified).\n\
# Consider the file under test as a stream of bytes (four per   \n\
# 32 bit integer).  Each byte can contain from 0 to 8 1's,      \n\
# with probabilities 1,8,28,56,70,56,28,8,1 over 256.  Now let  \n\
# the stream of bytes provide a string of overlapping  5-letter \n\
# words, each \"letter\" taking values A,B,C,D,E. The letters are \n\
# determined by the number of 1's in a byte::  0,1,or 2 yield A,\n\
# 3 yields B, 4 yields C, 5 yields D and 6,7 or 8 yield E. Thus \n\
# we have a monkey at a typewriter hitting five keys with vari- \n\
# ous probabilities (37,56,70,56,37 over 256).  There are 5^5   \n\
# possible 5-letter words, and from a string of 256,000 (over-  \n\
# lapping) 5-letter words, counts are made on the frequencies   \n\
# for each word.   The quadratic form in the weak inverse of    \n\
# the covariance matrix of the cell counts provides a chisquare \n\
# test::  Q5-Q4, the difference of the naive Pearson sums of    \n\
# (OBS-EXP)^2/EXP on counts for 5- and 4-letter cell counts.    \n\
#==================================================================\n");

}
