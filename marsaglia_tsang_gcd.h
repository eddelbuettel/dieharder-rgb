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

double kprob[41] = {
 0.00000000, 0.00000000, 0.00000003, 0.00000025, 0.00000174, 0.00000967,
 0.00004369, 0.00016548, 0.00053596, 0.00150710, 0.00372331, 0.00815829,
 0.01597666, 0.02814035, 0.04481325, 0.06480685, 0.08542249, 0.10294506,
 0.11372349, 0.11541169, 0.10779156, 0.09278761, 0.07370018, 0.05406323,
 0.03664863, 0.02296648, 0.01330651, 0.00712714, 0.00352767, 0.00161256,
 0.00068017, 0.00026442, 0.00009460, 0.00003109, 0.00000937, 0.00000258,
 0.00000065, 0.00000015, 0.00000003, 0.00000001, 0.00000000};
       
