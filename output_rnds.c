/*
 *
 * See copyright in copyright.h and the accompanying file COPYING
 * See also accompanying file STS.COPYING
 *
 */

/*
 *========================================================================
 * Output a list of random numbers of the selected type into a file.
 * Write a nice informative header, as well, to indicate several things
 * about the list (type, generator, count etc.)
 *========================================================================
 */

#include "dieharder.h"

double output_rnds()
{

 int i,j;
 unsigned int rand_uint;
 FILE *fp;

 seed = random_seed();
 gsl_rng_set(rng,seed);

 
 if ((fp = fopen(filename,"w")) == NULL) {
   fprintf(stderr,"Error: Cannot open %s, exiting.\n",filename);
   exit(0);
 }

 fprintf(fp,"type: d\ncount: %i\nnumbit: 32\n",tsamples);
 for(i=0;i<tsamples;i++){
   rand_uint = gsl_rng_get(rng);
   fprintf(fp,"%10u\n",rand_uint);
 }

 fclose(fp);

}

output_cruft()
{

 int num_randoms,i;

 if(!filename){
   printf(stderr,"Error: set to output but no file specified for printing.\n");
   fprintf(stderr,"Please use the -f <filename> option to create output file.\n");
   exit(0);
 }

 /* now prints num_randoms ints from rng to filename */
 if((fp = fopen(filename,"w")) == NULL) {
   fprintf(stderr,"Error: Cannot open %s, exiting.\n", filename);
   exit(0);
 }

 fprintf(fp,"type: d\ncount: %i\nnumbit: 32\n",num_randoms);
 for(i = 0; i < num_randoms; i++){
   fprintf(fp,"%lu\n",gsl_rng_get(rng));
 }

 fprintf(stdout,"File %s generated, contains %u rands.\n",filename,num_randoms);
 exit(0);

}
