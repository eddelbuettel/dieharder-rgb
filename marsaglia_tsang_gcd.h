/*
 * Hopefully this is a valid default initialization of the template test.
 */

static Dtest temp = {
  "Marsaglia and Tsang GCD Test",
  "#==================================================================\n\
#                     Marsaglia and Tsang GCD Test\n\
#\n\
# 10^7 tsamples (default) of uint rands u, v are generated and two\n\
# statistics are generated: their greatest common divisor (GCD) (w)\n\
# and the number of steps of Euclid's Method required to find it\n\
# (k).  Two tables of frequencies are thus generated -- one for the\n\
# number of times each value for k in the range 0 to 41 (with counts\n\
# greater than this range lumped in with the endpoints).\n\
# The other table is the frequency of occurrence of each GCD w.\n\
# k is be distributed approximately binomially, but this is useless for\n\
# the purposes of performing a stringent test.  Instead four \"good\"\n\
# RNGs (gfsr4,mt19937_1999,rndlxs2,taus2) were used to construct a\n\
# simulated table of high precision probabilities for k (a process that\n\
# obviously begs the question as to whether or not THESE generators\n\
# are \"good\" wrt the test).  At any rate, they produce very similar tables\n\
# and pass the test with each other's tables (and are otherwise very\n\
# different RNGs).  The table of probabilities for the gcd distribution is\n\
# generated dynamically per test (it is easy to compute).  Chisq tests\n\
# on both of these binned distributions yield two p-values per test,\n\
# and 100 (default) p-values of each are accumulated and subjected to\n\
# final KS tests and displayed in a histogram.\n\
#==================================================================\n",
  100,
  10000000
};

static Dtest *dtest = &temp;

/*
 * This table is the result of "extensive simulation" with RNGs believed
 * to be "the best of show" -- good ones according to the GSL and the
 * other dieharder tests.  Eventually it will be based on roughly
 * 10^12 samples each from mt19937_1999, ranlxd2, gfsr4, and taus2
 * (presuming that all of them produce mean probabilities that
 * are both normally distributed and overlapping for each number).
 *
 * This SHOULD result in all four of these generators passing
 * easily for the more common 10^7 steps per run and 100 runs
 * (10^9 total samples) which begs the question, maybe, dunno.
 */

/*
 * 1000000000 passes through mt19937_1999 produce this table
double kprob[41] = {
 0.00000000e+00, 9.00000000e-09, 6.10000000e-08, 4.64000000e-07, 2.97000000e-06, 1.44620000e-05,
 5.89260000e-05, 2.06560000e-04, 6.29030000e-04, 1.67947700e-03, 3.99509000e-03, 8.51673000e-03,
 1.63520170e-02, 2.84309990e-02, 4.49361310e-02, 6.47695460e-02, 8.53523910e-02, 1.03000804e-01,
 1.14058597e-01, 1.16057281e-01, 1.08529716e-01, 9.33552020e-02, 7.38878410e-02, 5.38052490e-02,
 3.60252240e-02, 2.21585000e-02, 1.25080210e-02, 6.47410800e-03, 3.07120300e-03, 1.32867000e-03,
 5.22293000e-04, 1.88175000e-04, 6.06610000e-05, 1.75190000e-05, 4.72500000e-06, 1.09300000e-06,
 2.21000000e-07, 2.90000000e-08, 3.00000000e-09, 2.00000000e-09, 0.00000000e+00 };
*/


/*
 * 1000000000 passes through gfsr4 produce this table
double kprob[41] = {
 0.00000000e+00, 2.00000000e-09, 5.60000000e-08, 4.88000000e-07, 2.93600000e-06, 1.41980000e-05,
 5.87710000e-05, 2.06463000e-04, 6.28227000e-04, 1.67945800e-03, 3.99414700e-03, 8.51530300e-03,
 1.63522620e-02, 2.84365590e-02, 4.49312500e-02, 6.47630060e-02, 8.53330790e-02, 1.02999632e-01,
 1.14062682e-01, 1.16051639e-01, 1.08537040e-01, 9.33631650e-02, 7.38971760e-02, 5.37974130e-02,
 3.60227920e-02, 2.21610450e-02, 1.25156100e-02, 6.47749100e-03, 3.07318400e-03, 1.32887900e-03,
 5.23531000e-04, 1.87855000e-04, 6.08240000e-05, 1.77430000e-05, 4.71300000e-06, 1.10000000e-06,
 2.40000000e-07, 3.60000000e-08, 5.00000000e-09, 0.00000000e+00, 0.00000000e+00 };
*/
       
/*
 * 1000000000 passes through ranlxd2 produce this table
double kprob[41] = {
 0.00000000e+00, 1.00000000e-09, 5.70000000e-08, 4.87000000e-07, 2.96000000e-06, 1.44010000e-05,
 5.93860000e-05, 2.06081000e-04, 6.27411000e-04, 1.68188600e-03, 3.99561800e-03, 8.51950500e-03,
 1.63582570e-02, 2.84329560e-02, 4.49402540e-02, 6.47584160e-02, 8.53356540e-02, 1.02996361e-01,
 1.14069062e-01, 1.16032478e-01, 1.08535610e-01, 9.33725140e-02, 7.38923460e-02, 5.37952690e-02,
 3.60179830e-02, 2.21606850e-02, 1.25166100e-02, 6.47897800e-03, 3.07285400e-03, 1.32957500e-03,
 5.24012000e-04, 1.88015000e-04, 6.05240000e-05, 1.78120000e-05, 4.61000000e-06, 1.09400000e-06,
 2.26000000e-07, 4.60000000e-08, 6.00000000e-09, 0.00000000e+00, 0.00000000e+00 };
 */
       
/*
 * 1000000000 passes through taus2 produce this table
 */
double kprob[41] = {
 0.00000000e+00, 1.00000000e-08, 5.90000000e-08, 4.69000000e-07, 2.89800000e-06, 1.43030000e-05,
 5.90690000e-05, 2.06573000e-04, 6.27223000e-04, 1.67981400e-03, 3.99532600e-03, 8.51784800e-03,
 1.63539980e-02, 2.84268470e-02, 4.49374280e-02, 6.47643620e-02, 8.53356470e-02, 1.03006610e-01,
 1.14052510e-01, 1.16046884e-01, 1.08547672e-01, 9.33837210e-02, 7.38873400e-02, 5.37986890e-02,
 3.60130290e-02, 2.21577570e-02, 1.25135350e-02, 6.47871400e-03, 3.06750200e-03, 1.32797700e-03,
 5.24629000e-04, 1.86837000e-04, 6.07010000e-05, 1.79740000e-05, 4.64900000e-06, 1.14000000e-06,
 2.17000000e-07, 2.90000000e-08, 9.00000000e-09, 0.00000000e+00, 1.00000000e-09 };
