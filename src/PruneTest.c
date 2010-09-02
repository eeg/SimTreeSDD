/******************************************************************************
 * try to prune a tree by trait
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "build.h"
#include "copy.h"
#include "extra.h"
#include "label.h"
#include "nodes.h"
#include "output.h"
#include "randist.h"
#include "read_tree.h"
#include "report.h"


void PruneTree(TreeNode *p, int where);

int main(int argc, char *argv[])
{
	int tip_counter[3];				// will count how many tips are in each location
	TreeNode *treeRoot, *prune1, *prune2;


	// check the program call
	if (argc < 2)
	{
		fprintf(stderr, "\nspecify a tree .ttn file(s)\n\n");
 		return -1;
	}

	// read in the tree
	treeRoot = ReadTree(argv[1]);	// (this assigns intNodeNum)

	// record the number of tips in each region
	tip_counter[0] = 0;
	tip_counter[1] = 0;
	tip_counter[2] = 0;
	CountTipStates(treeRoot, tip_counter);
	printf("num tips: %d  %d  %d\n\n", tip_counter[0], tip_counter[1], tip_counter[2]);

	/* prune the tree */

	if (tip_counter[1] > 1)
	{
		prune1 = NewNode(NULL, treeRoot->time);
		CopyNode(treeRoot, prune1);
		CopyTree(treeRoot, prune1);
		PruneTree(prune1, 1);
		prune1 = MoveRoot(prune1);
		AssignBranchLengths(prune1);
		WriteNewickFile(prune1, "prune1");
		FreeTree(prune1);
	}
	if (tip_counter[2] > 1)
	{
		prune2 = NewNode(NULL, treeRoot->time);
		CopyNode(treeRoot, prune2);
		CopyTree(treeRoot, prune2);
		PruneTree(prune2, 2);
		prune2 = MoveRoot(prune2);
		AssignBranchLengths(prune2);
		WriteNewickFile(prune2, "prune2");
		FreeTree(prune2);
	}

	FreeTree(treeRoot);

	return 0;
}


void PruneTree(TreeNode *p, int where)
{
	int from_right;

	if (p != NULL)
	{
		if ( (p->left==NULL && p->right==NULL) )	// at a tip
		{
			// if this tip should be cut off
			if (p->trait != where)
			{
				/* free the tip and adjust its ancestor's pointer */
				if (p->anc->left == p)
				{
					p = FreeNode(p);
					p->left = NULL;
				}
				else
				{
					p = FreeNode(p);
					p->right= NULL;
				}
				// p is now the tip's ancestor

				/* if the tip didn't go straight to the root, collapse the spare node */
				if (p->anc != NULL)
				{
					// see if you will be dropping back from the left or the right
					if (p->anc->right == p)
						from_right = 1;
					else
						from_right = 0;

					// now collapse p
					if (p->right == NULL)
					{
						p->index = UNDEF;	// mark this node as collapsed
						if (from_right == 0)
							p->anc->left = p->left;
						else
							p->anc->right = p->left;
						p->left->anc = p->anc;
						p = FreeNode(p);
					}
					else if (p->left == NULL)
					{
						if (from_right == 0)
							p->anc->left = p->right;
						else
							p->anc->right = p->right;
						p->right->anc = p->anc;
						p = FreeNode(p);
					}
				}

				/* if the tip did go to the root, don't worry about collapsing the root */
				else
				{
					if (p->right != NULL)
						PruneTree(p->right, where);
					// else if falling back to the root from the right, you're done
				}
			}
		}
		else
		{
			// continue from here, if this node hasn't been collapsed
			if (p->index != UNDEF)
				PruneTree(p->left, where);
			if (p->index != UNDEF)
				PruneTree(p->right, where);
		}
	}
}
