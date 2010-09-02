/******************************************************************************
 * prune trees by character state to make a composite tree
 *           -> prune1
 * composite
 *           -> prune2
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
#include "prune.h"
#include "randist.h"
#include "read_tree.h"
#include "report.h"


int main(int argc, char *argv[])
{
	int t;
	int tip_counter[3];				// will count how many tips are in each location
	TreeNode *treeRoot, *prune1, *prune2, *composite;
	char outfile[100];
	char outtemp[10];


	// check the program call
	if (argc < 2)
	{
		fprintf(stderr, "\nERROR: need to specify a tree .ttn file(s)\n\n");
 		return -1;
	}

	for (t=1; t<argc; t++)
	{
		// read in the tree
		treeRoot = ReadTree(argv[t]);	// (this assigns intNodeNum)

		// get the number of tips in each region
		tip_counter[0] = 0;
		tip_counter[1] = 0;
		tip_counter[2] = 0;
		CountTipStates(treeRoot, tip_counter);

		if (tip_counter[1] > 1 && tip_counter[2] > 1)
		{
/* why doesn't this work?  with TreeNode *prune[2]
			for (r=1; r<=2; r++)
			{
				prune[r] = CopyTree(treeRoot);
				PruneTree(prune[r], r);
			}
			composite->left = prune[1];
			composite->right = prune[2];
*/

			prune1 = CopyTree(treeRoot);
			PruneTree(prune1, 1);
			prune2 = CopyTree(treeRoot);
			PruneTree(prune2, 2);

			composite = NewNode(NULL, 0);
			composite->time = 0;
			composite->trait = -1;
			if (prune1->left != NULL && prune1->right != NULL)
			{
				composite->left = prune1;
				prune1->anc = composite;
			}
			else if (prune1->left == NULL)
			{
				composite->left = prune1->right;
				prune1->right->anc = composite;
			}
			else
			{
				composite->left = prune1->left;
				prune1->left->anc = composite;
			}
			if (prune2->left != NULL && prune2->right != NULL)
			{
				composite->right = prune2;
				prune1->anc = composite;
			}
			else if (prune2->left == NULL)
			{
				composite->right = prune2->right;
				prune2->right->anc = composite;
			}
			else
			{
				composite->right = prune2->left;
				prune2->left->anc = composite;
			}
			intNodeNum = 0;
			LabelTips(composite);
			LabelInteriorNodes(composite);
			AssignBranchLengths(composite);

			sprintf(outtemp, ".C");
			ChangeFileSuffix(argv[t], ".ttn", outtemp, outfile);
			WriteTTNFile(composite, outfile);

			FreeTree(composite);
		}

		FreeTree(treeRoot);
	}

	return 0;
}
