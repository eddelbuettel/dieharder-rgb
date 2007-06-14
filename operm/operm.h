/*
 *========================================================================
 * $Id: OPERM.h 2 2000-12-31 06:00:44Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include "copyright.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_sf.h>

/*
 *========================================================================
 * Useful defines
 *========================================================================
 */

#define STDIN	stdin
#define STDOUT	stdout
#define STDERR	stderr
#define YES	1
#define NO	0
#define PI 3.141592653589793238462643

 /*
  *========================================================================
  * Subroutine Prototypes
  *========================================================================
  */
 void Usage();


 /*
  *========================================================================
  * Global Variables
  *
  * Set the default values and override from the command line in parsecl().
  *========================================================================
  */
 int verbose;

