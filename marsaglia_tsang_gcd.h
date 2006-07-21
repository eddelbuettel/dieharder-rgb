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
# number of times each value for k in the range 3 to 35 (with counts\n\
# greater than or less than this range lumped in with the endpoints).\n\
# The other table is the frequency of occurrence of each GCD w.\n\
# k should be distributed (approximately) binomially -- a chisq test\n\
# is performed on a scaled table provided by Marsaglia and Tsang that\n\
# is a slight improvement on this.  w is distributed like\n\
# (6/pi^2) 1/w^2 (no table is needed) and a chisq test converts\n\
# this as well to a p-value.  In the dieharder implementation p_k and\n\
# p_w are accumulated separately psamples (100) times and a final\n\
# KS test is performed to generate overall p-values for both aspects\n\
# of the test.\n\
#==================================================================\n",
  100,
  10000000
};

static Dtest *dtest = &temp;

/*
 * This table is the result of "extensive simulation" with RNGs believed
 * to be "the best of show" -- good ones according to the GSL and the
 * other dieharder tests.  Eventually it will be based on roughly
 * 10^12 samples each from mt19937_1999, rndlx2, gsfr4, and taus2
 * (presuming that all of them produce mean probabilities that
 * are both normally distributed and overlapping for each number).
 *
 * This SHOULD result in all four of these generators passing
 * easily for the more common 10^7 steps per run and 100 runs
 * (10^9 total samples) which begs the question, maybe, dunno.
 */
double kprob[41] = {
 0.00000000, 0.00000000, 0.00000006, 0.00000053, 0.00000303, 0.00001449,
 0.00005897, 0.00020648, 0.00062801, 0.00167934, 0.00400101, 0.00851778,
 0.01634962, 0.02843971, 0.04493003, 0.06475867, 0.08533291, 0.10300242,
 0.11405233, 0.11604088, 0.10854099, 0.09338050, 0.07390149, 0.05379036,
 0.03603043, 0.02215252, 0.01251099, 0.00648336, 0.00306850, 0.00132797,
 0.00052381, 0.00018806, 0.00006098, 0.00001774, 0.00000462, 0.00000113,
 0.00000022, 0.00000005, 0.00000000, 0.00000000, 0.00000000,};
       
