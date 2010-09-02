/*******************************************************************************
 * these functions are for reconstructing ancestral states on a tree
 ******************************************************************************/
#ifndef __RECONSTRUCT_H__
#define __RECONSTRUCT_H__ 


#include "nodes.h"

void ReconstructAncestors(TreeInfo *tree, double rates[], int fixed);
void GetAncStates(TreeNode *p, double rates[], FILE *fp);


#endif
