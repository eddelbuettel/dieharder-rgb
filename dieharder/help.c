/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include "dieharder.h"

void help()
{

 int i;

 dieharder_test_types();

 /*
  * help for EACH test must be defined in the test.c package, even if
  * it is null or says "sorry, no help available".  help should be
  * a brief test description all pretty with each line beginning with
  * "#" so it is easy to filter out of standard test results output.
  */
 if(all==YES){
   for(i=0;i<900;i++){
     if(dh_test_types[i]){
       fprintf(stdout,"%s",dh_test_types[i]->description);
     }
   }
   Exit(0);
 }

 printf("OK, what is dtest_num = %u\n",dtest_num);

 if(dh_test_types[dtest_num]){
   fprintf(stdout,"%s",dh_test_types[dtest_num]->description);
   Exit(0);
 } else {
   Usage();
   Exit(0);
 }

}

void Usage()
{

 dh_header();

 fprintf(stdout, "\n\
Usage:\n\
\n\
  dieharder [-a] [-d dieharder test number] [-f filename]\n\
           [-D output flag [-D output flag] ... ] [-F] [-c separator] \n\
           [-g generator number or -1] [-h] [-l] \n\
           [-n ntuple] [-p number of p samples] [-o filename] \n\
           [-s seed strategy] [-S random number seed]\n\
	   [-t number of test samples] [-v verbose flag]\n\
	   [-x xvalue] [-y yvalue] [-z zvalue] \n\
\n\
  -a Runs all the tests with standard/default options to create a report\n\
  -d test number -  selects specific diehard test.\n\
  -f filename - two generators permit either formatted ASCII numbers\n\
     or raw binary numbers to be read in from a file for testing.\n\
     Note well: many tests with default parameters require a lot of rands!\n\
     To see a sample of the (required) header for ASCII formatted input, run\n\
\n\
         dieharder -o -f example.input -t 10\n\
\n\
     and then examine the contents of example.input.\n\
     Raw binary input reads 32 bit increments of the specified data stream.\n\
     stdin_input_raw accepts a pipe from a raw binary stream.\n\
  -D output flag permits fields to be selected for inclusion in dieharder\n\
     output.  Each flag can be entered as a binary number that turns\n\
     on a specific output field or header or by flag name; flags are\n\
     aggregated.  To see all currently known flags enter:\n\
  -F which lists all known flags by name and number.\n\
  -c table separator - where separator is e.g. ',' (CSV) or ' ' (whitespace).\n\
  -g generator number - selects a specific generator for testing.  Using\n\
     -1 causes all known generators to be printed out to the display.\n\
  -h prints context-sensitive help -- usually Usage (this message) or a\n\
     test synopsis if entered as e.g. dieharder -D 3 -h.\n\
  -l list all known tests.\n\
  -n ntuple - set ntuple length for tests on short bit strings that permit\n\
     the length to be varied (e.g. rgb bitdist).\n\
  -o filename - output -t count random numbers to file.\n\
  -p count - sets the number of p-value samples per test (default 100).\n\
  -S seed - where seed is a uint.  Overrides the default random seed\n\
     selection.  Ignored for file or stdin input.\n\
  -s strategy - if strategy is the (default) 0, dieharder reseeds (or\n\
     rewinds) once at the beginning when the random number generator is\n\
     selected and then never again.  If strategy is nonzero, the generator\n\
     is reseeded or rewound at the beginning of EACH TEST.  If -S seed was\n\
     specified, or a file is used, this means every test is applied to the\n\
     same sequence (which is useful for validation and testing of dieharder,\n\
     but not a good way to test rngs).  Otherwise a new random seed is\n\
     selected for each test.\n\
  -t count - sets the number of random entities used in each test, where\n\
     possible.  Be warned -- some tests will take a long time with the\n\
     default value of 10000.  Read the test synopses for suggested settings\n\
     for -t or use -a first.  Many tests will ignore -t as they require\n\
     a very specific number of samples to be used in generating their\n\
     statistic.\n\
  -v verbose flag -- controls the verbosity of the output for debugging\n\
     only.  Probably of little use to non-developers, and developers can\n\
     read the enum(s) in dieharder.h and the test sources to see which\n\
     flag values turn on output on which routines.  1 is 'all' and will\n\
     result in a highly detailed trace of program activity.\n\
\n\
  NOTE WELL:  The assessment(s) for the rngs may, in fact, be completely\n\
  incorrect or misleading.  In particular, 'Weak' pvalues should occur\n\
  one test in a hundred, and 'Failed' pvalues should occur one test in\n\
  a thousand -- that's what p MEANS.  Use them at your Own Risk!  Be Warned!\n\
\n",QUOTEME(VERSION));

 exit(0);

}

/* Cruft
 * This is a standard way of putting out a test description
void help_test(Dtest *dtest)
{

 printf("%s",dtest->description);
 
}
 */
