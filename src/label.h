/*******************************************************************************
 * these functions label things on an existing tree
 *   branch lengths
 *   tip/node times
 *   character states
 ******************************************************************************/

#ifndef __LABEL_H__
#define __LABEL_H__ 


#include "nodes.h"

extern int intNodeNum;

void AssignBranchLengths(TreeNode *p);
void AssignNodeTimes(TreeNode *p);
void LabelInteriorNodes(TreeNode *p);
void LabelTips(TreeNode *p);


#endif
