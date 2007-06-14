/*
 *========================================================================
 * $Id: OPERM.c 2 2000-12-31 06:00:44Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 *  Project template.
 *========================================================================
 */

#include "operm.h"


/*
 * main() is just a shell for routines that parse the command line,
 * do all the requested work, and then exit cleanly.  main() is pretty
 * much all there is in OPERM.c
 */
int main(int argc, char *argv[]) 
{

 /*
  * Parse command line and set global variables
  */
 parsecl(argc,argv);

 /*
  * Note that most of my OPERMs (except the terminally simple/stupid) 
  * have three phases after parsecl():
  *
  * Startup: Allocate memory, initialize all derivative variables from
  * command line values.  
 operm_startup();
  */

 /*
  * Work: Do all the work.  In a complicated OPERM, project_work would
  * itself be a shell for a lot of other modular routines.
  */
 operm_work();

 /* 
  * Quit:  Done. Clean up (if necessary) and exit.
 operm_quit();
  */

}
