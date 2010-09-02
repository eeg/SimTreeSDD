/*******************************************************************************
 * for building a binary state tree
 ******************************************************************************/

#ifndef __BUILDSTATES_H__
#define __BUILDSTATES_H__ 


#include "input_sim.h"
#include "nodes.h"

#define TOO_BIG 50000			// abort if there are more nodes than this
extern int node_counter;			// defined in build_common.c

void BirthDeath2States(TreeNode *root, TreeNode *here, int direction, TreeParams *parameters);
double GetNextBD(TreeNode *p, TreeParams *parameters, double *t);
void BackUp2States(TreeNode *root, TreeNode *p, TreeParams *parameters);


#endif
