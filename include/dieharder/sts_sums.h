/*
 * rgb_bitdist test header.
 */

/*
 * function prototype
 */
void sts_serial(Test **test,int irun);

static Dtest sts_serial_dtest = {
  "STS Serial Test (Generalized)",
  "\n\
#========================================================================\n\
#                         STS Serial Test\n\
# Accumulates the frequencies of n-tuples of bits drawn from a source\n\
# of random integers with and/or without overlap.  In the case of no\n\
# overlap the expected distribution of n-bit patterns is multinomial with\n\
# p = 2^(-n) e.g. the four 2-bit patterns 00 01 10 11 should occur with\n\
# equal probability.  The target distribution is thus a simple chisq with\n\
# 2^n - 1 degrees of freedom, one lost due to the constraint that:\n\
#\n\
#         p_00 + p_01 + p_01 + p_11 = 1\n\
#\n\
# With overlap the test is more difficult to construct.  For example,\n\
# given a bit string such as 0110100111000110 without overlap, it becomes\n\
# 01|10|10|01|11|00|01|10 and we count 1 00, 3 01s, 3 10s, and 1 11.
# WITH overlap we get all of these patterns as well as (with cyclic wrap):
# 0|11|01|00|11|10|00|11|0 and we count 4 00s, 4 01s, 4 10s, and 3 11s.\n\
# There is considerable covariance in the bit frequencies and a simple\n\
# chisq test no longer suffices.  It is not clear that testing a\n\
# generator with overlap in any way increases the sensitivity of the\n\
# test, nor does it necessarily save a significant amount of computer\n\
# time to achieve a given level of sensitivity.\n\
#\n\
# Nevertheless we include overlap using the STS method for converting\n\
# the raw frequencies of different length bitstrings into chisq tests.\n\
# This is closely akin to the dieharder OPERM5 tests and similar tests\n\
# described in e.g. Wegenkittl, except that those tests require the\n\
# computation of weak inverses of the covariance matrices and get\n\
# REALLY complicated.\n\
#\n",
  100,     /* Default psamples */
  100000,  /* Default tsamples */
  1        /* We magically make all the bit tests return a single histogram */
};

