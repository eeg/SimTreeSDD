/******************************************************************************
 * Copyright 2010 Emma Goldberg
 * 
 * This file is part of SimTreeSDD.
 * 
 * SimTreeSDD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SimTreeSDD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SimTreeSDD.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef __NODES_H__
#define __NODES_H__ 


/*******************************************************************************
 * management of the tree nodes themselves
 *   define what's in a node
 *   create a node
 *   destroy a node
 *   free all nodes in a tree
 ******************************************************************************/
typedef struct tnode
{
	double time, length;
	struct tnode *left, *right, *anc;
	int index, trait;
	double atime;   // for absolute ages
	int ptrait;     // for 2regions and allopatric speciation
} TreeNode;

TreeNode *NewNode(TreeNode *ancestor, double t);
TreeNode *FreeNode(TreeNode *here);
void FreeTree(TreeNode *p);


/*******************************************************************************
 * information about a tree (a collection of nodes)
 ******************************************************************************/
typedef struct
{
	TreeNode *root;
	double *node_times;		// can point to an array containing node times
	double start_t;		// time of root
	double end_t;			// time of tips
	int n_tips;			// # of tips (# of internal nodes = n_tips-1)
	int n_param;
	char *name;
} TreeInfo;

TreeInfo *NewTreeInfo();
void FreeTreeInfo(TreeInfo *tree);


#endif
