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
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_sort.h>
#include "tensor.h"

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
#define K       1024

 typedef enum {
   D_QUIET,
   D_ALL,
   D_SEED,
   D_DIEHARD_RUNS,
   D_DIEHARD_BDAY,
   D_DIEHARD_2DSPHERE,
   D_DIEHARD_3DSPHERE,
   D_RGB_PERSIST,
   D_RGB_BITDIST,
   D_STS_MONOBIT,
   D_STS_RUNS,
   D_BITS,
   D_SAMPLE,
   D_CHISQ,
   D_KSTEST,
   D_BTEST,
   D_XTEST,
   N_DEBUG
 } Debug;

 typedef enum {
   QUIET,
   MEASURE,
   N_VERBOSE
 } Verbose;

 
 /* Diehard Tests (by number) */
 typedef enum {
   DIEHARD_NONE,    /* Controls verbosity */
   DIEHARD_ALL,     /* Controls verbosity */
   DIEHARD_RUNS,
   DIEHARD_BDAY,
   DIEHARD_2DSPHERE,
   DIEHARD_3DSPHERE,
   N_DIEHARD_TESTS
 } Diehard_Tests;

 /* RGB Tests (by number) */
 typedef enum {
   RGB_NONE,    /* Controls verbosity */
   RGB_ALL,     /* Controls verbosity */
   RGB_PERSIST,
   RGB_BITDIST,
   N_RGB_TESTS
 } Rgb_Tests;

 typedef enum {
   STS_NONE,    /* Controls verbosity */
   STS_ALL,     /* Controls verbosity */
   STS_MONOBIT,
   STS_RUNS,
   N_STS_TESTS
 } Sts_Tests;

 /*
  * Add your own test types here!  Start by replacing dummy here and
  * in the subroutine prototypes below.
  */
 typedef enum {
   USER_NONE,    /* Controls verbosity */
   USER_ALL,     /* Controls verbosity */
   USER_DUMMY,
   N_USER_TESTS
 } User_Tests;


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
 void help();
 double binomial(unsigned int n, unsigned int k, double p);
 double chisq_eval(double *x,double *y,double *sigma, unsigned int n);
 double chisq_poisson(uint *observed,double lambda,int kmax,uint nsamp);
 double chisq_binomial(double *observed,double prob,uint kmax,uint nsamp);
 void dumpbits(unsigned int *data, unsigned int nbits);
 double sample(void *testfunc());
 double kstest(double *pvalue,int count);
 double kstest_kuiper(double *pvalue,int count);
 double q_ks(double x);
 double q_ks_kuiper(double x);
 /*
  *========================================================================
  * Test Prototypes.  Follow exactly this prototype format to add new
  * tests "within" the tool (using its standard p-analysis setup).
  *========================================================================
  */

 /* diehard "runs" test */
 double diehard_runs();
 void diehard_runs_test();
 void help_diehard_runs();
 /* diehard "birthdays" test */
 double diehard_birthdays();
 void diehard_birthdays_test();
 void help_diehard_birthdays();
 /* diehard "minimum distance" test, renamed */
 double diehard_2dsphere();
 void diehard_2dsphere_test();
 void help_diehard_2dsphere();
 /* diehard "3D spheres" test */
 double diehard_3dsphere();
 void diehard_3dsphere_test();
 void help_diehard_3dsphere();

 /* rgb "bit persistence test" is an exception! */
 double rgb_persist();
 void help_rgb_persist();
 /* rgb "bit distribution" master test */
 double rgb_bitdist();
 void rgb_bitdist_test();
 void help_rgb_bitdist();

 /* STS "monobit" test */
 double sts_monobit();
 void sts_monobit_test();
 void help_sts_monobit();
 /* STS "runs" test */
 double sts_runs();
 void sts_runs_test();
 void help_sts_runs();

 /* User "dummy" test.  Replace with your own test(s) */
 double user_dummy();
 void user_dummy_test();
 void help_user_dummy();

 /*
  *========================================================================
  *                           Global Variables
  *
  * The primary control variables, in alphabetical order, with comments.
  *========================================================================
  */
 int all;               /* Flag to do all tests on selected generator */
 int bits;              /* bitstring size (in bits) */
 int diehard;           /* Diehard test number */
 char filename[K];      /* Input file name */
 int generator;         /* GSL generator id number to be tested */
 int help_flag;              /* Help flag */
 int iterations;	/* For timing loop, set iterations to be timed */
 int list;              /* List all tests flag */
 int List;              /* List all generators flag */
 int ntuple;            /* n-tuple size for n-tuple tests */
 int psamples;          /* Number of test runs in final KS test */
 int quiet;             /* quiet flag -- surpresses full output report */
 int rgb;               /* rgb test number */
 int sts;               /* sts test number */
 uint Seed;             /* user selected seed.  Surpresses reseeding per sample.*/
 uint tsamples;         /* Generally should be "a lot". */
 int user;              /* user defined test number */
 int verbose;           /* Default is not to be verbose. */
 double x_user;         /* General purpose command line inputs for use */
 double y_user;         /* in any test. */
 double z_user;
  
 /*
  *========================================================================
  *
  * A few more needed variables.
  *
  *      ks_pvalue is a vector of p-values obtained by samples, kspi is
  *      its index.
  *
  *      tv_start and tv_stop are used to record timings.
  *
  *      dummy and idiot are there to fool the compiler into not optimizing
  *      empty loops out of existence so we can time one accurately.
  *
  *      fp is a file pointer for input or output purposes.
  *
  *========================================================================
  */
#define KS_SAMPLES_PER_TEST_MAX 256
 double *ks_pvalue;
 unsigned int kspi;
 struct timeval tv_start,tv_stop;
 int dummy,idiot;
 FILE *fp;


 /*
  * rng global vectors and variables for setup and tests.
  */
 const gsl_rng_type **types;    /* where all the rng types go */
 gsl_rng *rng;               /* global gsl random number generator */
 unsigned int *rand_int;        /* vector of "random" ints */
 unsigned int seed;             /* rng seed of run (?) */
 unsigned int random_max;       /* maximum rng returned by generator */
 unsigned int rmax;             /* scratch space for random_max manipulation */
 unsigned int rmax_bits;        /* Number of valid bits in rng */
 unsigned int rmax_mask;        /* Mask for valid section of uint */
 double *rand_uniform;          /* vector of "random" uniform deviates */
 int num_gsl_rngs,num_my_rngs,num_rngs;  /* number of rng's */


 /*
  * Looks like we'll need certain structs in order to be able to really
  * streamline and objectify the process of test->results->chisq->p-value
  * (and possibly even -> p-p-value, since p-values themselves should have
  * a distribution and we might do better running 30 samples of 1000 instead
  * of one sample of 30,000).
  */
 typedef struct {
   unsigned int npts;
   double p;
   double *x;
   double *y;
   double *sigma;
   double chisq;
   double pvalue;
   char testname[128];
   char rngname[128];
 } Ntest;

 typedef struct {
   unsigned int bins;
   double p;
   double *x;
   double *y;
   double *sigma;
   double chisq;
   double pvalue;
   char testname[128];
   char rngname[128];
 } Btest;

 typedef struct {
   unsigned int npts;
   double p;
   double x;
   double y;
   double sigma;
   double pvalue;
   char testname[128];
   char rngname[128];
 } Xtest;
 
