/*
 * Hopefully this is a valid default initialization of the template test.
 */

static Dtest temp = {
  "Diehard 3d Sphere (Minimum Distance) Test",
  "\n\
#==================================================================\n\
#          Diehard 3d Sphere (Minimum Distance) Test\n\
# Generate 4000 points in a 1000^3 integer cube.  Determine the\n\
# the shortest nearest neighbor distance R.  This should generate\n\
# p = 1.0 - exp(-R^3/30).  Repeat for lots of samples, apply a\n\
# KS test to see if p is uniform.\n\
#==================================================================\n\,
  100,
  0
};

static Dtest *dtest = &temp;
#define POINTS 4000
#define DIM 3

typedef struct {
  double x[DIM];
} C3;
 
