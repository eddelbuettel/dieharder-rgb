/*
 * Hopefully this is a valid default initialization of the template test.
 */

static Dtest temp = {
  "Example Dieharder Test",
  "\n\
#==================================================================\n\
#                   Example Dieharder Test\n\
#\n\
#                     Lagged Sum Test\n\
# This package contains many very lovely tests.  Very few of them,\n\
# however, test for lagged correlations -- the possibility that\n\
# the random number generator has a bitlevel correlation after\n\
# some period.  Diehard tests, for example, COULD NOT test for this\n\
# sort of thing with only a few million rands to sample from.\n\
# The template test is therefore very simple.  It takes a user-\n\
# specified lag (-x lag) and adds up uniform deviates sampled with\n\
# that lag.  The mean of tsamples samples summed should be\n\
# 0.5*tsamples.  The standard deviation should be sqrt(tsamples/12).\n\
# The experimental values of the sum are thus converted into a\n\
# p-value (using the erf()) and a ks-test applied to psamples of them.\n\
#==================================================================\n",
  100,
  100000
};

static Dtest *dtest = &temp;
