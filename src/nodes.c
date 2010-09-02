#include <stdio.h>
#include <stdlib.h>
#include "nodes.h"


/*******************************************************************************
 * tree nodes
 ******************************************************************************/

// create a new tree node
TreeNode *NewNode(TreeNode *ancestor, double t)
{
	TreeNode *p;
	p = (TreeNode *) malloc(sizeof(TreeNode));

	if (p == NULL)
	{
		fprintf(stderr, "NewNode malloc failed\n");
		exit(1);
	}
	
	p->time = t;
	p->anc = ancestor;
	p->left = NULL;	// will be assigned if there are descendents
	p->right = NULL;	// will be assigned if there are descendents
	p->index = -1;		// will assign this later
	p->trait = -1;		// will assign this later
	p->ptrait = -1;	// will assign this is allopatric speciation happens

	return p;
}


// free a node and return its ancestor
TreeNode *FreeNode(TreeNode *here)
{
	TreeNode *p;

	p = here->anc;
	free(here);
	return p;
}


// free all the nodes of a tree
void FreeTree(TreeNode *p)
{
	if (p != NULL)
	{
		FreeTree(p->left);
		FreeTree(p->right);
		free(p);
	}
}


/*******************************************************************************
 * tree information
 ******************************************************************************/
// create a new tree-information structure
TreeInfo *NewTreeInfo()
{
	TreeInfo *tree;
	tree = (TreeInfo *) malloc(sizeof(TreeInfo));

	if (tree == NULL)
	{
		fprintf(stderr, "NewLikeParams malloc failed\n");
		exit(1);
	}

	return tree;

}

// free a tree-information structure (but not the tree itself)
void FreeTreeInfo(TreeInfo *tree)
{
	free(tree);
}
