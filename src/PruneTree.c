/******************************************************************************
 * prune trees by character state
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
	int r, t;
	int tip_counter[3];				// will count how many tips are in each location
	TreeNode *treeRoot, *prune;
	char outfile[100];
	char outtemp[10];


	// check the program call
	if (argc < 2)
	{
		fprintf(stderr, "\nERROR: need to specify a tree .ttn file(s)\n\n");
 		return -1;
	}

	/***
	 * for each tree in the program call
	 *   read in the tree
	 *   prune the tree to endemics at each region
	 *   write resulting .ttn files
	 ***/

	for (t=1; t<argc; t++)
	{
		// read in the tree
		treeRoot = ReadTree(argv[t]);	// (this assigns intNodeNum)

		// get the number of tips in each region
		tip_counter[0] = 0;
		tip_counter[1] = 0;
		tip_counter[2] = 0;
		CountTipStates(treeRoot, tip_counter);
		printf("  num tips: %d  %d  %d\n", tip_counter[0], tip_counter[1], tip_counter[2]);

		for (r=1; r<=2; r++)			// for endemics at each region
		{
			// prune the tree
			if (tip_counter[r] > 1)
			{
				prune = CopyTree(treeRoot);
				PruneTree(prune, r);
				prune = MoveRoot(prune);
				AssignBranchLengths(prune);

				sprintf(outtemp, ".%d", r);
				ChangeFileSuffix(argv[t], ".ttn", outtemp, outfile);
				WriteTTNFile(prune, outfile);
				FreeTree(prune);
			}
		}

		FreeTree(treeRoot);
	}

	return 0;
}
