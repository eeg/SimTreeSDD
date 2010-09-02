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
