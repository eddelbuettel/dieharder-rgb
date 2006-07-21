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
 0.00000000, 0.00000000, 0.00000006, 0.00000052, 0.00000291, 0.00001442,
 0.00005915, 0.00020651, 0.00062856, 0.00167769, 0.00399680, 0.00851849,
 0.01635983, 0.02842709, 0.04492956, 0.06476814, 0.08532849, 0.10300806,
 0.11406801, 0.11605583, 0.10852275, 0.09336438, 0.07389176, 0.05379594,
 0.03601755, 0.02216985, 0.01251636, 0.00647881, 0.00306680, 0.00132967,
 0.00052408, 0.00018744, 0.00006074, 0.00001786, 0.00000460, 0.00000105,
 0.00000021, 0.00000004, 0.00000001, 0.00000000, 0.00000000};
 */


/*
 * 1000000000 passes through gfsr4 produce this table
double kprob[41] = {
 0.00000000, 0.00000001, 0.00000006, 0.00000049, 0.00000289, 0.00001432,
 0.00005907, 0.00020618, 0.00062861, 0.00167990, 0.00399645, 0.00851895,
 0.01635088, 0.02843226, 0.04493818, 0.06477078, 0.08533041, 0.10299858,
 0.11407716, 0.11603810, 0.10852250, 0.09337407, 0.07389772, 0.05379910,
 0.03600950, 0.02215874, 0.01251623, 0.00648233, 0.00307104, 0.00132856,
 0.00052460, 0.00018776, 0.00006084, 0.00001774, 0.00000462, 0.00000108,
 0.00000023, 0.00000004, 0.00000001, 0.00000000, 0.00000000};
 */
       
/*
 * 1000000000 passes through ranlxd2 produce this table
double kprob[41] = {
 0.00000000, 0.00000001, 0.00000006, 0.00000046, 0.00000292, 0.00001407,
 0.00005894, 0.00020589, 0.00062840, 0.00168119, 0.00399783, 0.00851083,
 0.01635648, 0.02842381, 0.04492509, 0.06474807, 0.08534645, 0.10301025,
 0.11406899, 0.11604431, 0.10852716, 0.09337502, 0.07390388, 0.05379728,
 0.03603149, 0.02215876, 0.01250717, 0.00647815, 0.00307101, 0.00132934,
 0.00052377, 0.00018804, 0.00006100, 0.00001783, 0.00000465, 0.00000113,
 0.00000023, 0.00000003, 0.00000001, 0.00000000, 0.00000000};
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
