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
#include <gsl/gsl_rng.h>

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
   LOUD,
   HELPGEN,
   TST_RNG,
   N_VERBOSE,
 } Verbose;


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


 /*
  *========================================================================
  * Global Variables:
  *      samples is the number of samples to be taken
  *      iterations is the number of times the core loop is run (inserted 
  *        so one can minimize errors due to the gettimeofday call itself).
  *      random_flag controls the insertion of usleep(1), probably cruft
  *        as it makes little visible difference EXCEPT in the empty
  *        timing loop (where it is essential and not controlled by
  *        random_flag!).
  *      cache_flag is an experimental control.
  *      xtest is an input variable used to pass the d[i] (and hence
  *        division) variable from the command line.
  *      size is the length of the vector being evaluated.
  *      verbose controls the output -- "normal" is verbose=1.
  *      tv_start and tv_stop are used to record timings.
  *
  *      floattest selects a straight bogomflops run
  *      doubletest selects a double precision bogomflops run (bogomdops?)
  *========================================================================
  */
 int samples,iterations,stride,random_flag,cache_flag,size;
 int quiet,verbose;
 int floattest,doubletest,transtest;
 int testnum,hbtestnum;
 struct timeval tv_start,tv_stop;

 /*
  * Global vectors and constants for tests.
  */
 double xtest;			/* input variable */
 int dummy,idiot;		/* To fool compiler into executing empty cases */
 int *rand_int;		        /* vector of "random" ints */
 const gsl_rng_type **types;
 unsigned int random_max,seed;
 gsl_rng *random;  /* global gsl random number generator */
 int num_gsl_rngs;  /* number of GSL rng's found in current library */
 FILE *fp;         /* pointer to /dev/random, for that benchmark or rng seed */
 char homebrews[10][64];  /* names of homebrew random number generators */
 int num_homebrews;       /* and their number */
