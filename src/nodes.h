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
	double cl[2];			// conditional likelihoods
	// in buildcheck, cl[direction] holds the trait value at the target time
	// when adding stuff here, consider adding it to CopyNode (copy.c)
} TreeNode;

TreeNode *NewNode(TreeNode *ancestor, double t);
TreeNode *FreeNode(TreeNode *here);
void FreeTree(TreeNode *p);


/*******************************************************************************
 * manage information on broken branches (only used by Get2Like.c so far)
 *    define what's in a branch node
 *    create a node
 *    destroy a node
 *    destroy a whole branch
 ******************************************************************************/
typedef struct bnode
{
	double time;			// time of an internal node
	int trait;			// state of the node
	struct bnode *next;		// the next node on the broken branch
	struct bnode *prev;		// the previous node on the broken branch

	// change the dimensions if there are more than two states
	double segprob[2][2];	// segment probabilities
} BranchNode;

BranchNode *NewBranchNode();
void FreeBranchNode(BranchNode *p);
void FreeBranch(BranchNode *p);


/*******************************************************************************
 * information about a tree (a collection of nodes)
 ******************************************************************************/
typedef struct
{
	TreeNode *root;
	double *node_times;		// can point to an array containing node times
	BranchNode **branches;	// ditto for an array of broken branches
	double start_t;		// time of root
	double end_t;			// time of tips
	int n_tips;			// # of tips (# of internal nodes = n_tips-1)
	int n_param;
	char *name;
} TreeInfo;

TreeInfo *NewTreeInfo();
void FreeTreeInfo(TreeInfo *tree);


#endif
