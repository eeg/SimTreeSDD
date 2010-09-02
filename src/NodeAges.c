/*******************************************************************************
 * read in a two-region tree
 * for tips in each endemic state (1 and 2) or each region (0+1, 0+2), get
 *   1. the average tip length (distance from tip to previous node)
 *   2. the average node distance (number of nodes from tip to root)
 *
 * then prune the tree to get endemic subtrees
 * for each subtree, get
 *   3. the average tip length (distance from tip to previous node)
 *   4. the average time of all nodes
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "extra.h"
#include "label.h"
#include "nodes.h"
#include "output.h"
#include "read_tree.h"
#include "report.h"
#include "randist.h"
#include "prune.h"
#include "copy.h"
#include "build.h"


void GetNodeAges1(TreeNode *p, double total_tip_length[], int total_node_depth[]);
void GetNodeAges2(TreeNode *p, double *total_tip_length, double *total_node_time);


int main(int argc, char *argv[])
{
	FILE *fp1;//, *fp2;

	TreeNode *treeRoot;//, *prune;

	int r, t;
	int tip_counter[3];
	double total_tip_lengths[3];
	int total_node_depth[3];
//	double total_tip_length, total_node_time;


	// check the program call
	if (argc < 2)
	{
		fprintf(stderr, "\nERROR: need to specify a tree .ttn file(s)\n\n");
 		return -1;
	}

	fp1 = fopen("node_ages_unpruned.dat", "w");
	fprintf(fp1, "# state0 num, state0 tip length, state0 node depth, state1 ..., state2...\n");
/*
	fp2 = fopen("node_ages_pruned.dat", "w");
	fprintf(fp2, "# state0 tip length, state0 node time, state1 ..., state2...\n");
*/

	// loop over all given trees
	for (t=1; t<argc; t++)
	{
		/* make a labelled tree from the input file */
		treeRoot = ReadTree(argv[t]);

		// initialize stuff
		for (r=0; r<3; r++)
		{
			tip_counter[r] = 0;
			total_tip_lengths[r] = 0;
			total_node_depth[r] = 0;
		}

		CountTipStates(treeRoot, tip_counter);

		/* get stuff from complete tree */

		GetNodeAges1(treeRoot, total_tip_lengths, total_node_depth);

		for (r=0; r<3; r++)
		{
			fprintf(fp1, "%d\t", tip_counter[r]);
			if (tip_counter[r] > 0)
			{
				fprintf(fp1, "%f\t", total_tip_lengths[r]/tip_counter[r]);
				fprintf(fp1, "%f\t", (double)total_node_depth[r]/tip_counter[r]);
			}
			else
//				fprintf(fp1, "NA\tNA\t");
				fprintf(fp1, "0\t0\t");
		}
		fprintf(fp1, "\n");

		/* prune trees and get stuff from them */
/*
		for (r=0; r<3; r++)			// for endemics at each region
		{
			if (tip_counter[r] > 1)
			{
				prune = CopyTree(treeRoot);
				PruneTree(prune, r);
				prune = MoveRoot(prune);
				AssignBranchLengths(prune);

				total_tip_length = 0;
				total_node_time = 0;
				GetNodeAges2(treeRoot, &total_tip_length, &total_node_time);

				fprintf(fp2, "%f\t%f\t", total_tip_length/tip_counter[r], total_node_time/tip_counter[r]);
			}
			else
				fprintf(fp2, "NA\tNA\t");
		}
		fprintf(fp2, "\n");
*/
	}

	fclose(fp1);
//	fclose(fp2);

	return 0;
}


// used with unpruned tree
void GetNodeAges1(TreeNode *p, double total_tip_lengths[], int total_node_depth[])
{
	int trait;

	if (p != NULL)
	{
		GetNodeAges1(p->left,  total_tip_lengths, total_node_depth);
		GetNodeAges1(p->right, total_tip_lengths, total_node_depth);
		if (p->left == NULL && p->right == NULL)
		{
			trait = p->trait;
			total_tip_lengths[trait] += p->length;
			while (p->anc != NULL)
			{
				total_node_depth[trait]++;
				p = p->anc;
			}
		}
	}
}


// used with pruned trees
void GetNodeAges2(TreeNode *p, double *total_tip_length, double *total_node_time)
{
	if (p != NULL)
	{
		GetNodeAges2(p->left,  total_tip_length, total_node_time);
		GetNodeAges2(p->right, total_tip_length, total_node_time);
		if (p->left == NULL && p->right == NULL)
			*total_tip_length += p->length;
		else
			*total_node_time += p->time;
	}
}
