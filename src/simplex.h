/*******************************************************************************
 * uses the downhill simplex algorithm to find parameters which minimize the 
 *   function func; the algorithm is described in Numerical Recipes
 * this version includes a check to avoid negative parameter values
 * it also passes around a TreeInfo structure
 ******************************************************************************/
#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__


#include "nodes.h"

// fewer parameters than this can be used (value stored in TreeInfo structure)
#define MAX_PARAM 6
#define MAX_VERT  (MAX_PARAM+1)

double DownhillSimplex(double p[][MAX_PARAM], double y[], double (*func)(TreeInfo *, double []), TreeInfo *tree); 


#endif
