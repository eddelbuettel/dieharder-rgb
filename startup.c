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

#include "cpu_rate.h"

void cpu_rate_startup()
{

 int i,j,k;
 const gsl_rng_type *T;
 unsigned int random_max,seed;

 gsl_rng_env_setup();
 T = gsl_rng_default;
 random = gsl_rng_alloc (T);
 random_max = gsl_rng_max(random);
 if ((fp = fopen("/dev/random","r")) == NULL) {
   if(verbose == 10) printf("Cannot open /dev/random, setting seed to 0\n");
   seed = 0;
 } else {
   fread(&seed,sizeof(seed),1,fp);
   if(verbose == 10) printf("Got seed %u from /dev/random\n",seed);
   /* leave this open if we're going to benchmark /dev/random itself */
   if(testnum != 10) fclose(fp);
 }

 gsl_rng_set(random,seed);

 if(verbose == 10){
   printf("generator type: %s\n", gsl_rng_name(random));
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
