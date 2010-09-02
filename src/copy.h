/*******************************************************************************
 * these functions are for making a copy of a tree
 ******************************************************************************/

#ifndef __COPY_H__
#define __COPY_H__ 


#include "nodes.h"

TreeNode *CopyTree(TreeNode *oldroot);
void CopyTreeGuts(TreeNode *p, TreeNode *q);
void CopyNode(TreeNode *old, TreeNode *new);


#endif
