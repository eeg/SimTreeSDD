/*******************************************************************************
 * these functions are for reading in a tree from a .ttn file
 *    .ttn = tree-tips-nodes
 *    file format written by my simulation program (or by hand)
 *    contains Newick tree string and list of character states
 ******************************************************************************/
#ifndef __READTREE_H__
#define __READTREE_H__ 


#include "label.h"
#include "nodes.h"
#include "output.h"

TreeNode *ReadTree(char *filename);
void ReadTreeString(TreeNode *root, char *tree_string);
void PutTipStates(TreeNode *p, int traits[]);
void PutInteriorNodeStates(TreeNode *p, int traits[]);


#endif
