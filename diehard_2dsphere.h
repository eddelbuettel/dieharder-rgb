/*
 * Hopefully this is a valid default initialization of the template test.
 */

static Dtest temp = {
  "Diehard Minimum Distance Test",
  "\n\
#==================================================================\n\
#         Diehard Minimum Distance Test (modified).\n\
# Generate 8000 points in a 10000^2 square.  Determine the\n\
# the shortest nearest neighbor distance R.  This should generate\n\
# p = 1.0 - exp(-R^2/0.995).  Repeat for lots of samples, apply a\n\
# KS test to see if p is uniform.\n\
#\n\
# The number of samples is fixed -- tsamples is ignored.\n\
#==================================================================\n",
  100,
  100000
};

static Dtest *dtest = &temp;

#define POINTS 8000
#define DIM 2

typedef struct {
  double x[DIM];
} C3;
 
