/*
 * Hopefully this is a valid default initialization of the template test.
 */

static Dtest temp = {
  "Diehard Parking Lot Test",
  "\n\
#==================================================================\n\
#             Diehard Parking Lot Test (modified).\n\
# This tests the distribution of attempts to randomly park a\n\
# square car of length 1 on a 100x100 parking lot without\n\
# crashing.  We plot n (number of attempts) versus k (number of\n\
# attempts that didn't \"crash\" because the car squares \n\
# overlapped and compare to the expected result from a perfectly\n\
# random set of parking coordinates.  This is, alas, not really\n\
# known on theoretical grounds so instead we compare to n=12,000\n\
# where k should average 3523 with sigma 21.9 and is very close\n\
# to normally distributed.  Thus (k-3523)/21.9 is a standard\n\
# normal variable, which converted to a uniform p-value, provides\n\
# input to a KS test with a default 100 samples.\n\
#==================================================================\n",
  100,
  0
};

static Dtest *dtest = &temp;
