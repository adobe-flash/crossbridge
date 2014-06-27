/* For copyright information, see olden_v1.0/COPYRIGHT */

/*
 * CODE.H: define various global things for CODE.C.
 * Copyright (c) 1991, Joshua E. Barnes, Honolulu, HI.
 * 	    It's free because it's yours.
 */


/* Former global variables.  convert to #defines   */

/* #define nbody 256       	 number of bodies in system               */
#define outfile ""		/* file name for snapshot output            */
#define infile ""		/* file name for snapshot input             */
#define dtime 0.0125		/* timestep for leapfrog integrator         */
#define tstop 2.0		/* time to stop calculation                 */
#define dtout 0.25		/* time between data outputs                */
#define headline "Hack code: Plummer model"
                		/* message describing calculation           */

extern int nbody;




           

