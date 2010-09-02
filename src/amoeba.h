/*******************************************************************************
 * uses the downhill simplex algorithm to find parameters which minimize the 
 *   function funk
 * this code is from Numerical Recipes, but is modified to pass the root of the
 *   tree around (rather than make it global)
 ******************************************************************************/
#ifndef __AMOEBA_H__
#define __AMOEBA_H__


#include "nodes.h"

// fewer parameters than this can be used (value stored in TreeInfo structure)
#define MAX_PARAM 6
#define MAX_VERT  (MAX_PARAM+1)

double amoeba (double p[][MAX_PARAM], double y[], double (*funk)(TreeInfo *, double []), TreeInfo *tree); 


#endif
