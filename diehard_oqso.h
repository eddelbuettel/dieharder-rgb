/*
 * Hopefully this is a valid default initialization of the template test.
 */

static Dtest temp = {
  "Diehard OQSO Test",
  "\n\
#==================================================================\n\
#   Diehard Overlapping Quadruples Sparce Occupancy (OQSO) Test\n\
#\n\
#  Similar, to OPSO except that it considers 4-letter \n\
#  words from an alphabet of 32 letters, each letter determined  \n\
#  by a designated string of 5 consecutive bits from the test    \n\
#  file, elements of which are assumed 32-bit random integers.   \n\
#  The mean number of missing words in a sequence of 2^21 four-  \n\
#  letter words,  (2^21+3 \"keystrokes\"), is again 141909, with   \n\
#  sigma = 295.  The mean is based on theory; sigma comes from   \n\
#  extensive simulation.                                         \n\
# \n\
#  Note 2^21 = 2097152, tsamples cannot be varied.\n\
#==================================================================\n",
  100,
  2097152
};

static Dtest *dtest = &temp;
