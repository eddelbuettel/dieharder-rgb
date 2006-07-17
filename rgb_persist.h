/*
 * Hopefully this is a valid default initialization of the template test.
 */

static Dtest rgb_persist_data = {
  "RGB Bit Persistence Test",
  "\n\
#==================================================================\n\
#                   RGB Bit Persistence Test\n\
# This test generates 256 sequential samples of an random unsigned\n\
# integer from the given rng.  Successive integers are logically\n\
# processed to extract a mask with 1's whereever bits do not\n\
# change.  Since bits will NOT change when filling e.g. unsigned\n\
# ints with 16 bit ints, this mask logically &'d with the maximum\n\
# random number returned by the rng.  All the remaining 1's in the\n\
# resulting mask are therefore significant -- they represent bits\n\
# that never change over the length of the test.  These bits are\n\
# very likely the reason that certain rng's fail the monobit\n\
# test -- extra persistent e.g. 1's or 0's inevitably bias the\n\
# total bitcount.  In many cases the particular bits repeated\n\
# appear to depend on the seed.  If the -i flag is given, the\n\
# entire test is repeated with the rng reseeded to generate a mask\n\
# and the extracted mask cumulated to show all the possible bit\n\
# positions that might be repeated for different seeds.\n\
#==================================================================\n",
  1,
  256
};

static Dtest *dtest = &rgb_persist_data;
