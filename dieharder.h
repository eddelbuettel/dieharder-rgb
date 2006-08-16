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
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "parse.h"
#include "Vtest.h"
#include "Xtest.h"

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
#define PI      3.141592653589793238462643
#define K       1024
#define PAGE    4096
#define M       1048576
#define M_2     2097152
/*
 * For reasons unknown and unknowable, free() doesn't null the pointer
 * it frees (possibly because it is called by value!)  Nor does it return
 * a success value.  In fact, it is just a leak or memory corruption waiting
 * to happen.  Sigh.
 *
 * nullfree frees and sets the pointer it freed back to NULL.  This let's
 * one e.g. if(a) nullfree(a) to safely free a IFF it is actually a pointer,
 * and let's one test on a in other ways to avoid leaking memory.
 */
#define nullfree(a) {free(a);a = 0;}

 typedef enum {
   D_QUIET,
   D_ALL,
   D_DIEHARD_BDAY,
   D_DIEHARD_OPERM5,
   D_DIEHARD_RANK_32x32,
   D_DIEHARD_RANK_6x8,
   D_DIEHARD_BITSTREAM,
   D_DIEHARD_OPSO,
   D_DIEHARD_OQSO,
   D_DIEHARD_DNA,
   D_DIEHARD_COUNT_1S_STREAM,
   D_DIEHARD_COUNT_1S_BYTE,
   D_DIEHARD_PARKING_LOT,
   D_DIEHARD_2DSPHERE,
   D_DIEHARD_3DSPHERE,
   D_DIEHARD_SQUEEZE,
   D_DIEHARD_SUMS,
   D_DIEHARD_RUNS,
   D_DIEHARD_CRAPS,
   D_MARSAGLIA_TSANG_GCD,
   D_MARSAGLIA_TSANG_GORILLA,
   D_RGB_TIMING,
   D_RGB_PERSIST,
   D_RGB_BITDIST,
   D_RGB_LMN,
   D_STS_MONOBIT,
   D_STS_RUNS,
   D_STS_BLOCK,
   D_USER_TEMPLATE,
   D_SEED,
   D_STARTUP,
   D_BITS,
   D_SAMPLE,
   D_CHISQ,
   D_KSTEST,
   D_BTEST,
   D_XTEST,
   D_BRANK,
   D_FILE_INPUT,
   D_FILE_INPUT_RAW,
   N_DEBUG
 } Debug;

 typedef enum {
   QUIET,
   MEASURE,
   N_VERBOSE
 } Verbose;

 
 /* Diehard Tests (by number) */
 typedef enum {
   DIEHARD_NONE,
   DIEHARD_BDAY,
   DIEHARD_OPERM5,
   DIEHARD_RANK_32x32,
   DIEHARD_RANK_6x8,
   DIEHARD_BITSTREAM,
   DIEHARD_OPSO,
   DIEHARD_OQSO,
   DIEHARD_DNA,
   DIEHARD_COUNT_1S_STREAM,
   DIEHARD_COUNT_1S_BYTE,
   DIEHARD_PARKING_LOT,
   DIEHARD_2DSPHERE,
   DIEHARD_3DSPHERE,
   DIEHARD_SQUEEZE,
   DIEHARD_SUMS,
   DIEHARD_RUNS,
   DIEHARD_CRAPS,
   MARSAGLIA_TSANG_GCD,
   MARSAGLIA_TSANG_GORILLA,
   N_DIEHARD_TESTS
 } Diehard_Tests;

 /* RGB Tests (by number) */
 typedef enum {
   RGB_NONE,
   RGB_TIMING,
   RGB_PERSIST,
   RGB_BITDIST,
   RGB_LMN,
   N_RGB_TESTS
 } Rgb_Tests;

 typedef enum {
   STS_NONE,
   STS_MONOBIT,
   STS_RUNS,
   STS_BLOCK,
   N_STS_TESTS
 } Sts_Tests;

 /*
  * Add your own test types here!  Use/rename/copy template here and in
  * the subroutine prototypes below.  Note also the D_USER_TEMPLATE in
  * the Debug enum, in case you want to add controllable I/O to help
  * you debug with the -v flag.
  */
 typedef enum {
   USER_NONE,
   USER_TEMPLATE,
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
  Cruft
 char **allocate_fields(size_t maxfields,size_t maxfieldlength);
 int parse(char *inbuffer,char **outfields,int maxfields,int maxfieldlength);
 int split(char *inbuffer,char **outfields,char *delim,
               int maxfields,int maxfieldlength);
 */
 /*
  *========================================================================
  * Test Prototypes.  Follow exactly this prototype format to add new
  * tests "within" the tool (using its standard p-analysis setup).
  *========================================================================
  */

 /* 1: diehard "birthdays" test */
 double diehard_birthdays();
 void diehard_birthdays_test();
 void help_diehard_birthdays();

 /* 2: diehard "operm5" test */
 double diehard_operm5();
 void diehard_operm5_test();
 void help_diehard_operm5();

 /* 3: diehard "binary rank" test 32x32*/
 double diehard_rank_32x32();
 void diehard_rank_32x32_test();
 void help_diehard_rank_32x32();

 /* 4: diehard "binary rank" test 6x8*/
 double diehard_rank_6x8();
 void diehard_rank_6x8_test();
 void help_diehard_rank_6x8();

 /* 5: diehard "bitstream" test */
 double diehard_bitstream();
 void diehard_bitstream_test();
 void help_diehard_bitstream();

 /* 6: diehard "opso" test */
 double diehard_opso();
 void diehard_opso_test();
 void help_diehard_opso();

 /* 7: diehard "oqso" test */
 double diehard_oqso();
 void diehard_oqso_test();
 void help_diehard_oqso();

 /* 8: diehard "dna" test */
 double diehard_dna();
 void diehard_dna_test();
 void help_diehard_dna();

 /* 9: diehard "count_1s_stream" test */
 double diehard_count_1s_stream();
 void diehard_count_1s_stream_test();
 void help_diehard_count_1s_stream();

 /* 10: diehard "count_1s_byte" test */
 double diehard_count_1s_byte();
 void diehard_count_1s_byte_test();
 void help_diehard_count_1s_byte();

 /* 11: diehard "parking lot" test */
 double diehard_parking_lot();
 void diehard_parking_lot_test();
 void help_diehard_parking_lot();

 /* 12: diehard "minimum distance" test, renamed */
 double diehard_2dsphere();
 void diehard_2dsphere_test();
 void help_diehard_2dsphere();

 /* 13: diehard "3D spheres" test */
 double diehard_3dsphere();
 void diehard_3dsphere_test();
 void help_diehard_3dsphere();

 /* 14: diehard "squeeze" test */
 double diehard_squeeze();
 void diehard_squeeze_test();
 void help_diehard_squeeze();

 /* 15: diehard "sums" test */
 double diehard_sums();
 void diehard_sums_test();
 void help_diehard_sums();

 /* 16: diehard "runs" test */
 double diehard_runs();
 void diehard_runs_test();
 void help_diehard_runs();

 /* 17: diehard "craps" test */
 double diehard_craps();
 void diehard_craps_test();
 void help_diehard_craps();

 /* 18: Marsaglia and Tsang GCD test */
 double marsaglia_tsang_gcd();
 void marsaglia_tsang_gcd_test();
 void help_marsaglia_tsang_gcd();

 /* 19: Marsaglia and Tsang Gorilla test */
 double marsaglia_tsang_gorilla();
 void marsaglia_tsang_gorilla_test();
 void help_marsaglia_tsang_gorilla();

 /* rgb "timing test"  is an exception!  No psamples.*/
 double rgb_timing();
 void help_rgb_timing();
 /* rgb "bit persistence test" is an exception!  Also no psamples. */
 double rgb_persist();
 void help_rgb_persist();
 /* rgb "bit distribution" master test */
 double rgb_bitdist();
 void rgb_bitdist_test();
 void help_rgb_bitdist();
 /* rgb "lmn" master master test */
 double rgb_lmn();
 void rgb_lmn_test();
 void help_rgb_lmn();

 /* STS "monobit" test */
 double sts_monobit();
 void sts_monobit_test();
 void help_sts_monobit();
 /* STS "runs" test */
 double sts_runs();
 void sts_runs_test();
 void help_sts_runs();
 /* STS "block" test 
 double sts_block();
 void sts_block_test();
 void help_sts_block(); */


 /* User/template test.  Replace/copy/rename with your own test(s) */
 double user_template();
 void user_template_test();
 void help_user_template();

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
 int binary;            /* Flag to output rands in binary (with -o -f) */
 int bits;              /* bitstring size (in bits) */
 int diehard;           /* Diehard test number */
 int generator;         /* GSL generator id number to be tested */
 int help_flag;         /* Help flag */
 int hist_flag;         /* Histogram display flag */
 int iterations;	/* For timing loop, set iterations to be timed */
 int list;              /* List all tests flag */
 int List;              /* List all generators flag */
 int ntuple;            /* n-tuple size for n-tuple tests */
 int num_randoms;		/* the number of randoms stored into memory and usable */
 int output;		/* equals 1 if you output to file, otherwise 0. */
 int overlap;           /* equals 1 if you really want to use diehard overlap */
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
 /* We need two of these to do diehard_craps.  Sigh. */
 double *ks_pvalue,*ks_pvalue2;
 unsigned int kspi;
 struct timeval tv_start,tv_stop;
 int dummy,idiot;
 FILE *fp;
#define MAXFIELDNUMBER 8
 char **fields;

 /*
  * Global variables and prototypes associated with file_input and
  * file_input_raw.
  */
 uint file_input_get_rewind_cnt(gsl_rng *rng);
 uint file_input_get_rtot(gsl_rng *rng);
 void file_input_set_rtot(gsl_rng *rng,uint value);

 char filename[K];      /* Input file name */
 int fromfile;		/* set true if file is used for rands */
 int filenumbits;	/* number of bits per integer */
 uint filecount;	/* number of rands in file */
 char filetype;         /* file type */
/*
 * This struct contains the data maintained on the operation of
 * the file_input rng, and can be accessed via rng->state->whatever
 *
 *  fp is the file pointer
 *  flen is the number of rands in the file (filecount)
 *  rptr is a count of rands returned since last rewind
 *  rtot is a count of rands returned since the file was opened
 *  rewind_cnt is a count of how many times the file was rewound
 *     since its last open.
 */
typedef struct
  {
    FILE *fp;
    uint flen;
    uint rptr;
    uint rtot;
    uint rewind_cnt;
  } file_input_state_t;

 

 /*
  * rng global vectors and variables for setup and tests.
  */
 const gsl_rng_type **types;    /* where all the rng types go */
 gsl_rng *rng;               /* global gsl random number generator */
 unsigned int *rand_int;        /* vector of "random" uints */
 unsigned int **rand_mtx;       /* matrix of "random" uints */
 double *rand_dbl;              /* vector of "random" uniform deviates */

 /*
  * All required for GSL Singular Value Decomposition (to obtain
  * the rank of the random matrix for diehard rank tests).
  */
 gsl_matrix *A,*V;
 gsl_vector *S,*svdwork;

 unsigned int seed;             /* rng seed of run (?) */
 unsigned int random_max;       /* maximum rng returned by generator */
 unsigned int rmax;             /* scratch space for random_max manipulation */
 unsigned int rmax_bits;        /* Number of valid bits in rng */
 unsigned int rmax_mask;        /* Mask for valid section of uint */
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
 } Ntest;

 typedef struct {
   unsigned int bins;
   double p;
   double *x;
   double *y;
   double *sigma;
   double chisq;
   double pvalue;
 } Btest;

 /*
  * OK, I've got a TON of boilerplate code that would be greatly
  * simplified if I could create a generic test struct and make e.g.
  * the toplevel test wrapper for MOST tests a single call plus at most
  * a single set of boilerplate for customized test description.
  * This struct will contain INDIVIDUAL pointers for all requisite
  * standard data structures or control parameters, e.g. tsamples,
  * psamples, etc.
  */
 typedef struct {
   /* The name of the test */
   char *name;

   /* pointer to a test description */
   char *description;

   /* Standard test default */
   uint psamples_std;

   /* Standard test default */
   uint tsamples_std;

 } Dtest;

 void test_header(Dtest *dtest);
 void test_footer(Dtest *dtest, double pvalue, double *pvalues, char *desc);

