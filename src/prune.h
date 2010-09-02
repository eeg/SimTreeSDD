/*******************************************************************************
 * these functions are for pruning a tree by tip trait value
 ******************************************************************************/

#ifndef __PRUNE_H__
#define __PRUNE_H__ 


#include "nodes.h"

void PruneTree(TreeNode *p, int where);
void PruneTreeNeg(TreeNode *p, int where);

#endif
