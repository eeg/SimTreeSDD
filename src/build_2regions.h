/*******************************************************************************
 * these functions are for creating a simulated birth-death-dispersal tree
 ******************************************************************************/

#ifndef __BUILDREGION_H__
#define __BUILDREGION_H__ 


#include "input_sim.h"
#include "nodes.h"

extern int node_counter;			// defined in build.c

void BirthDeath2Regions(TreeNode *root, TreeNode *here, int where, int direction, TreeParams *parameters);
void BackUp2Regions(TreeNode *root, TreeNode *here, TreeParams *parameters);
int Wait2RegionEvent(int *where, double now, double *wait_t, TreeParams *parameters);
void BuildTree2Regions(TreeNode *root, TreeParams *parameters);


#endif
