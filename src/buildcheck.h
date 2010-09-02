/*******************************************************************************
 * these functions are modified from those in build.c to allow checking the
 *   state of lineages at a particular time while building the tree
 ******************************************************************************/

#ifndef __BUILDCHECK_H__
#define __BUILDCHECK_H__ 


#include "input_sim.h"
#include "nodes.h"

extern int node_counter;			// defined in build.c

// note: other necessary functions are in build.h (GetNextBD)

// these versions allow exiting when there are enough tips
void BirthDeathCount(TreeNode *root, TreeNode *here, int direction, TreeParams *parameters, int *tips_so_far);
void BackUpCount(TreeNode *root, TreeNode *here, TreeParams *parameters, int *tips_so_far);

// these versions allow checking trait values at a particular time and exiting when there are enough tips
void BirthDeathCheck(TreeNode *root, TreeNode *here, int direction, TreeParams *parameters, BranchNode *checknode, int *tips_so_far);
void BackUpCheck(TreeNode *root, TreeNode *here, TreeParams *parameters, BranchNode *checknode, int *tips_so_far);
double GetNextBDCheck(TreeNode *p, TreeParams *parameters, double *t, BranchNode *checknode, int direction);

// this version allows backing up to the base of the tree when it is not a branching point
void BackUpFinal(TreeNode *here);


#endif
