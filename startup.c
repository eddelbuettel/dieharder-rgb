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

#include "rand_rate.h"

void cpu_rate_startup()
{


  

 int i,imax,j,k;
 const gsl_rng_type **types;
 unsigned int random_max,seed;

 /*
  * List the available, built in gsl generators
  */
 types = gsl_rng_types_setup ();
 printf("Available built-in gsl-linked generators:\n");
 i = 0;
 while(types[i] != NULL){
   printf("%d: %s\n", i, types[i]->name);
   i++;
 }
 num_gsl_rngs = i+1;
 /*
  * List any homemade or locally linked non-gsl generators.
  * There will ALWAYS be /dev/random -- many of the rest that
  * we might want to link are already wrapped in the gsl -- but we'll
  * still need to add more, I'm sure.  Especially vector generators,
  * since I'm betting one can double the speed of the fastest generator
  * here by simply filling a vector all at once, and not calling the
  * generator in a subroutine wrapper...
  */
 printf("Available private generators:\n");
 printf("%d: /dev/random (always used to reseed selected generator)\n", i);
 i++;

 /*
  * The following code is all likely to go away.  It just validates
  * that gsl_rng's "work" and so forth.
  */
 random = gsl_rng_alloc (types[12]);
 random_max = gsl_rng_max(random);
 seed = random_seed();
 gsl_rng_set(random,seed);

 if(verbose == 10){
   printf("generator type: %s\n", gsl_rng_name(random));
   printf("seed value: %u\n", seed);
   printf("max value = %u\n", random_max);
   printf("first value = %u\n", gsl_rng_get(random));
   printf("second value = %u\n", gsl_rng_get(random));
   printf("third value = %u\n", gsl_rng_get(random));
 }


 /*
  * Do some initialization and setup here, according to the values
  * of testnum (and the other parameters)
  */
 idiot = 1.0;	/* Give the idiot a value */
 /*
  * This is a double indexing vector.  It will have either
  * sequential entries or shuffled entries, and be used to
  * permit direct comparison of random vs sequential memory
  * speed tests.  We abstract the index resolution directly into
  * the timing loop time, which is subtracted in any event.
  *
  * I think that we're going to include this in ALL the vector
  * benchmarks, but subject to a shuffle flag, instead of using
  * independent test numbers.
  */
 index = (int *) malloc((size_t) (size*sizeof(int)));
 switch(testnum){
   case 0:
     dummy = idiot;	/* This is the "nearly empty loop" */
     break;
   case 1:	/* bogomflops */
     /* Allocate the vector to be tested with */
     d = (double *) malloc((size_t) (size*sizeof(double)));
     /* xtest is set from the command line, default PI */
     ad = xtest;
     bd = 1.0 + xtest;
     /* Initialize the vector */
    for(i=0;i<size;i++){
      d[i] = xtest;
     }
     break;
   case 2:	/* copy (stream) */
     /* Allocate the vectors to be tested with */
     d = (double *) malloc((size_t) (size*sizeof(double)));
     a = (double *) malloc((size_t) (size*sizeof(double)));
     /* xtest is set from the command line, default PI */
     /* Initialize the vector */
    for(i=0;i<size;i++){
      a[i] = xtest;
     }
     break;
   case 3:	/* scale (stream) */
     /* Allocate the vectors to be tested with */
     d = (double *) malloc((size_t) (size*sizeof(double)));
     /* Initialize the vector */
    for(i=0;i<size;i++){
      d[i] = 0.0;
     }
     break;
   case 4:	/* add (stream) */
     /* Allocate the vectors to be tested with */
     d = (double *) malloc((size_t) (size*sizeof(double)));
     a = (double *) malloc((size_t) (size*sizeof(double)));
     b = (double *) malloc((size_t) (size*sizeof(double)));
     /* xtest is set from the command line, default PI */
     /* Initialize the vectors */
    for(i=0;i<size;i++){
      a[i] = xtest;
      b[i] = xtest;
     }
     break;
   case 5:	/* triad (stream) */
     /* Allocate the vectors to be tested with */
     d = (double *) malloc((size_t) (size*sizeof(double)));
     a = (double *) malloc((size_t) (size*sizeof(double)));
     b = (double *) malloc((size_t) (size*sizeof(double)));
     /* xtest is set from the command line, default PI */
     /* Initialize the vectors */
    for(i=0;i<size;i++){
      a[i] = xtest;
      b[i] = 0.0;
     }
     break;
   case 6:	/* bogomegatrids, no divide */
     /* Allocate the vector to be tested with */
     d = (double *) malloc((size_t) (size*sizeof(double)));
     /* xtest is set from the command line, default PI */
     ad = 1.0;
     bd = xtest;
     cd = xtest;
     /* Initialize the vector */
    for(i=0;i<size;i++){
      d[i] = xtest;
     }
     break;
   case 7:	/* memtest */
     /* Allocate the vector to be tested with */
     ai = (unsigned int *) malloc((size_t) (size*sizeof(unsigned int)));
     /* Initialize the vector */
     for(i=0;i<size;i++){
      ai[i] = i;
     }
     break;
   case 8:	/* memtest, random */
     /* Allocate the vector to be tested with */
     ai = (unsigned int *) malloc((size_t) (size*sizeof(unsigned int)));
     /* Initialize the vector */
     for(i=0;i<size;i++){
       ai[i] = i;
     }
     if(verbose == 10){
       printf("#==================================================================\n");
       for(i=0;i<size;i++) printf("# Unshuffled: ai[%d] = %u\n",i,ai[i]);
     }
     gsl_ran_shuffle(random, ai, size, sizeof (unsigned int));
     if(verbose == 10){
       printf("#==================================================================\n");
       for(i=0;i<size;i++) printf("# Shuffled: ai[%d] = %u\n",i,ai[i]);
       printf("#==================================================================\n");
     }
     break;
   case 9:	/* savage, requires no initialization */
     break;
   default:
     break;
 }

}
