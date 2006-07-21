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
#==================================================================",
  100,
  10000000
};

static Dtest *dtest = &temp;


