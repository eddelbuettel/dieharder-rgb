/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include "copyright.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_sf.h>

/*
 *========================================================================
 * Useful defines
 *========================================================================
 */

#define STDIN	stdin
#define STDOUT	stdout
#define STDERR	stderr
#define YES	1
#define NO	0
#define PI 3.141592653589793238462643

 typedef enum {
   D_QUIET,
   D_ALL,
   D_SEED,
   N_DEBUG
 } Debug;

 typedef enum {
   QUIET,
   MEASURE,
   N_VERBOSE
 } Verbose;

 typedef enum {
   LIST_RNGS,
   LIST_RAND,
   BOGORATE,
   STS_MONOBIT,
   RGB_BINOMIAL,
   N_TEST
 } Tests;


 /*
  *========================================================================
  * Subroutine Prototypes
  *========================================================================
  */
 unsigned long int random_seed();
 void start_timing();
 void stop_timing();
 double delta_timing();
 void measure_rate();
 void Usage();
 double binomial(unsigned int n, unsigned int k, double p);
 double chisq_eval(double *x,double *y,double *sigma, unsigned int n);

 /*
  *========================================================================
  * Global Variables:
  *      samples is the number of samples to be taken
  *      iterations is the number of times the core loop is run (inserted 
  *        so one can minimize errors due to the gettimeofday call itself).
  *      size is the length of the vector being evaluated.
  *      bits is the size of the bitstring being tested (where relevant)
  *      verbose controls the output -- "normal" is verbose=1.
  *      tv_start and tv_stop are used to record timings.
  *      dummy and idiot are there to fool the compiler into not optimizing
  *        empty loops out of existence so we can time one accurately.
  *      filename holds the name(s) of I/O file(s).
  *      fp is a file pointer for input or output purposes.
  *========================================================================
  */
 int samples,iterations,size,bits;
 int quiet,verbose;
 int testnum,randnum;
 struct timeval tv_start,tv_stop;
 int dummy,idiot;
 char filename[128];
 FILE *fp;

 /*
  * rng global vectors and variables for setup and tests.
  */
 const gsl_rng_type **types;    /* where all the rng types go */
 gsl_rng *random;               /* global gsl random number generator */
 unsigned int *rand_int;        /* vector of "random" ints */
 unsigned int random_max,seed;  /* parameter and seed of run */
 double *rand_uniform;          /* vector of "random" uniform deviates */
 int num_gsl_rngs,num_my_rngs,num_rngs;  /* number of rng's */

