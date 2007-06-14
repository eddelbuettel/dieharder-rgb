/*
 *========================================================================
 * $Id: OPERM_work.c 14 2001-09-12 16:09:15Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 *  Project work template.
 *========================================================================
 */

#include "OPERM.h"

void OPERM_work()
{

 char c;

 printf("Do you want me to write Hello World?\n");
 c = getchar();
 if(c == 'y'){
   if(verbose){
     fprintf(STDERR,"About to write \"Hello, World!\" to STDOUT\n");
   }
   fprintf(STDOUT,"Hello, World!\n");
 }

}
