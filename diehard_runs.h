/*
 * Hopefully this is a valid default initialization of the Diehard
 * Craps test.
 */

Dtest diehard_runs_data = {
  "Diehard(er) Runs Test",
  "\n\
#==================================================================\n\
#                Diehard \"runs\" test (modified).\n\
# This tests the distribution of increasing and decreasing runs\n\
# of integers.  If called with reasonable parameters e.g. -s 100\n\
# or greater and -n 100000 or greater, it will compute a vector\n\
# of p-values for up and down and verify that the proportion\n\
# of these values less than 0.01 is consistent with a uniform\n\
# distribution.\n\
#==================================================================\n",
  100,     /* Default psamples */
  100000   /* Default tsamples */
};

/*
 * The following are the definitions and parameters for runs, based on
 * Journal of Applied Statistics v30, Algorithm AS 157, 1981:
 *    The Runs-Up and Runs-Down Tests, by R. G. T. Grafton.
 * (and before that Knuth's The Art of Programming v. 2).
 */

#define RUN_MAX 6

/*
 * a_ij
 */
static double a[6][6] = {
 { 4529.4,   9044.9,  13568.0,   18091.0,   22615.0,   27892.0},
 { 9044.9,  18097.0,  27139.0,   36187.0,   45234.0,   55789.0},
 {13568.0,  27139.0,  40721.0,   54281.0,   67852.0,   83685.0},
 {18091.0,  36187.0,  54281.0,   72414.0,   90470.0,  111580.0},
 {22615.0,  45234.0,  67852.0,   90470.0,  113262.0,  139476.0},
 {27892.0,  55789.0,  83685.0,  111580.0,  139476.0,  172860.0}
};

/*
 * b_i
 */
static double b[6] = {
 1.0/6.0,
 5.0/24.0,
 11.0/120.0,
 19.0/720.0,
 29.0/5040.0,
 1.0/840.0,};

static Dtest *dtest = &diehard_runs_data;
