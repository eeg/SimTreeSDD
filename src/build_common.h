/*******************************************************************************
 * for simulating either kind of tree (binary state or two regions)
 ******************************************************************************/

#ifndef __BUILD_COMMON_H__
#define __BUILD_COMMON_H__ 


#include "input_sim.h"
#include "nodes.h"

#define TOO_BIG 50000			// abort if there are more nodes than this
extern int node_counter;			// defined in build_common.c

int AssignRootState(TreeNode *root, TreeParams *parameters);
TreeNode *MoveRoot(TreeNode *root);


#endif
