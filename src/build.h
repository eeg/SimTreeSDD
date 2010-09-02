/*******************************************************************************
 * these functions are for creating a simulated birth-death-transition tree
 ******************************************************************************/

#ifndef __BUILD_H__
#define __BUILD_H__ 


#include "input_sim.h"
#include "nodes.h"

#define TOO_BIG 50000			// abort if there are more nodes than this
extern int node_counter;			// defined in build.c

int AssignRootState(TreeNode *root, TreeParams *parameters);
void BirthDeath(TreeNode *root, TreeNode *here, int direction, TreeParams *parameters);
double GetNextBD(TreeNode *p, TreeParams *parameters, double *t);
void BackUp(TreeNode *root, TreeNode *p, TreeParams *parameters);
TreeNode *MoveRoot(TreeNode *root);


#endif
