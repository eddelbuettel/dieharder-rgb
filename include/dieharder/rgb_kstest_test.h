/*
 * rgb_kstest_test test header
 */

/*
 * function prototype
 */
int rgb_kstest_test(Test **test,int irun);

static Dtest rgb_kstest_test_dtest = {
  "RGB Kolmogorov-Smirnov Test Test",
  "rgb_kstest_test",
  "#\n\
#            The Kolmogorov-Smirnov Test Test\n\
#\n\
#\n\
# This test generates a vector of tsamples uniform deviates from the\n\
# selected rng, then applies an Anderson-Darling or Kuiper KS test to\n\
# it to directly test for uniformity.  The AD version has been symmetrized\n\
# to correct for weak left bias for small sample sets; Kuiper is already\n\
# ring-symmetric on the interval.  The AD code corresponds roughly to\n\
# what is in R (thanks to a correction sent in by David Bauer).\n\
# As always, the test is run pvalues times and the (same) KS test is then\n\
# used to generate a final test pvalue, but the real purpose of this test\n\
# is to test ADKS and KKS, not to test rngs.  There is reason to believe\n\
# that they are are returning high-biased final pvalues for the default\n\
# 100 test pvalues in a standard test run, and this test will allow this\n\
# hypothesis to be directly tested.  It may prove useful to detect\n\
# extremely weak rngs as well (for large tsamples) but I somewhat doubt\n\
# it.\n\
#\n",
  100,
  100,
  1,
  rgb_kstest_test,
  0
};

