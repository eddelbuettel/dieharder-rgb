/*
 * Hopefully this is a valid default initialization of the template test.
 */

static Dtest temp = {
  "RGB Timing Test",
  "\n\
#==================================================================\n\
#                      RGB Timing Test\n\
#\n\
# This test times the selected random number generator only.  It is\n\
# generally run at the beginning of a run of -a(ll) the tests to provide\n\
# some measure of the relative time taken up generating random numbers\n\
# for the various generators and tests.\n\
#==================================================================\n",
  1,
  1
};

static Dtest *dtest = &temp;
