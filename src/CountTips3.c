/*******************************************************************************
 * read in a three-state tree
 * get and record the number of tips in each state
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "extra.h"
#include "label.h"
#include "nodes.h"
#include "read_tree.h"
#include "report.h"


int main(int argc, char *argv[])
{
	FILE *fp;

	TreeNode *treeRoot;

	int r, t;
	int tip_counter[3];


	// check the program call
	if (argc < 2)
	{
		fprintf(stderr, "\nERROR: need to specify a tree .ttn file(s)\n\n");
 		return -1;
	}

	fp = fopen("tip_counts.dat", "w");
	fprintf(fp, "tree file: # state0 num, # state1 num, # state2 num\n");

	
	// loop over all given trees
	for (t=1; t<argc; t++)
	{
		/* make a labelled tree from the input file */
		treeRoot = ReadTree(argv[t]);

		// initialize tip counter
		for (r=0; r<3; r++)
			tip_counter[r] = 0;

		CountTipStates(treeRoot, tip_counter);
		FreeTree(treeRoot);

		fprintf(fp, "%s:\t%d\t%d\t%d\n", argv[t], tip_counter[0], tip_counter[1], tip_counter[2]);
	}

	fclose(fp);

	return 0;
}
