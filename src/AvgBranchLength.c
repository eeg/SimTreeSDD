/*******************************************************************************
 * read in a tree
 * compute the average length of branches (tip and internal) on the tree
 * compute the average node depth of all tips
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

	int t, i;
	double lengths[50000];	// should instead use TOO_BIG in build.h
	double length_sum;
	int depths[50000];	// should instead use TOO_BIG in build.h
	double depth_sum;
	int n_tips;


	// check the program call
	if (argc < 2)
	{
		fprintf(stderr, "\nERROR: need to specify a tree .ttn file(s)\n\n");
 		return -1;
	}

	fp = fopen("avg_branch_lens.dat", "w");
	fprintf(fp, "# tips, avg branch length, avg node depth\n");

	// loop over all given trees
	for (t=1; t<argc; t++)
	{
		/* make a labelled tree from the input file */
		treeRoot = ReadTree(argv[t]);
		n_tips = (intNodeNum+1)/2;

		
		/* get the branch lengths */
		GetBranchLengths(treeRoot, lengths);
		// (don't bother initializing because only using the right number of entries)
		length_sum = 0;
		for (i=0; i<n_tips*2-2; i++)
			length_sum += lengths[i];

		/* get the node depths */
		for (i=0; i<n_tips; i++)
			depths[i] = 0;
		GetNodeDepth(treeRoot, depths);
		depth_sum = 0;
		for (i=0; i<n_tips; i++)
			depth_sum += depths[i];

		FreeTree(treeRoot);

		/* record the results */
		fprintf(fp, "%d\t%f\t%f\n", n_tips, length_sum/n_tips, depth_sum/n_tips);
	}

	fclose(fp);

	return 0;
}
