/*******************************************************************************
 * estimate the transition rate by counting nodes (the hacked method we're 
 *   using on Josh's SI data) and by using branch length info (to compare)
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "extra.h"
#include "label.h"
#include "nodes.h"
#include "read_tree.h"
#include "report.h"


void GetSISCChanges(TreeNode *p, int *nodes_si, int *nodes_sisc, int *branches_sisc);
void GetStateBranchLengths(TreeNode *p, double *sum0, double *sum1);
void ZeroNodes(TreeNode *p);
void TraceSI(TreeNode *p);


int main(int argc, char *argv[])
{
	FILE *fp;

	TreeNode *treeRoot, *p;

	int t, i;
	int depths[25000];	// should instead use TOO_BIG in build.h
	double depth_sum, avg_depth;
	double length_depth, length_true, length_si;
	int nodes_si, nodes_sisc, branches_sisc;
	int n_tips;
	double tree_age;
	double lensum_sc, lensum_si;	// sum of branch lengths coming from SC or SI nodes
	double beta1, beta2, beta3, beta4;


	// check the program call
	if (argc < 2)
	{
		fprintf(stderr, "\nERROR: need to specify a tree .ttn file(s)\n\n");
 		return -1;
	}

	fp = fopen("beta_estimates.dat", "w");
	fprintf(fp, "# tips, tree age, SI nodes, SI->SC nodes, SI->SC branches, depth length, true length, SI length, node beta, branch beta, SI branch beta, trace beta\n");

	// loop over all given trees
	for (t=1; t<argc; t++)
	{
		/* make a labelled tree from the input file */
		treeRoot = ReadTree(argv[t]);
		n_tips = (intNodeNum+1)/2;

		/* get the elapsed time = tree age */
		p = treeRoot;
		do
		{
			p = p->left;
		} while (p->left != NULL);
		tree_age = p->time;

		/* get the average node depth and branch (segment) length */
		for (i=0; i<n_tips; i++)
			depths[i] = 0;
		GetNodeDepth(treeRoot, depths);
		depth_sum = 0;
		for (i=0; i<n_tips; i++)
			depth_sum += depths[i];
		avg_depth = depth_sum/n_tips;
		length_depth = tree_age/avg_depth;

		/* get the number of SI nodes and the number of nodes and branches with SI->SC changes */
		nodes_si = 0;
		nodes_sisc = 0;
		branches_sisc = 0;
		GetSISCChanges(treeRoot, &nodes_si, &nodes_sisc, &branches_sisc);

		/* get the true average branch lengths for each state */
		lensum_sc = 0;
		lensum_si = 0;
		GetStateBranchLengths(treeRoot, &lensum_sc, &lensum_si);
		length_true = (lensum_sc+lensum_si) / (2*n_tips-2);
		length_si = lensum_si / (2*nodes_si);

		/* compute beta in various ways */
		beta1 = ( (double)nodes_sisc / nodes_si) / length_depth;
		beta2 = ( (double)branches_sisc / (2*nodes_si)) / length_depth;
		beta3 = ( (double)branches_sisc / (2*nodes_si)) / length_si;

		/* record the results so far */
		fprintf(fp, "%d\t%f\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f", n_tips, tree_age, nodes_si, nodes_sisc, branches_sisc, length_depth, length_true, length_si, beta1, beta2, beta3);

		/*** now that we're done with the real tree, mess with the character states
		 * When reconstructing SI on a real-life tree, SI nodes/branches that 
		 * left no SI tips will not be known to be SI.  Emulate that issue here.
		 ***/

		// set all internal nodes to 0
		ZeroNodes(treeRoot);
		// assign SI nodes by tracing back from SI tips
		TraceSI(treeRoot);
		// re-get SI node and branch info
		nodes_si = 0;
		nodes_sisc = 0;
		branches_sisc = 0;
		GetSISCChanges(treeRoot, &nodes_si, &nodes_sisc, &branches_sisc);
		// compute beta (the one we'll use) for this tree
		beta4 = ( (double)branches_sisc / (2*nodes_si)) / length_depth;

		/* record the final result */
		fprintf(fp, "\t%f\n", beta4);

		FreeTree(treeRoot);
	}

	fclose(fp);

	return 0;
}



/*******************************************************************************
 * get the number of SI nodes and the number of nodes and branches with SI->SC changes
 ******************************************************************************/
void GetSISCChanges(TreeNode *p, int *nodes_si, int *nodes_sisc, int *branches_sisc)
{
	if (p != NULL)
	{
		GetSISCChanges(p->left, nodes_si, nodes_sisc, branches_sisc);
		GetSISCChanges(p->right, nodes_si, nodes_sisc, branches_sisc);

		// if not at a tip
		if (p->left != NULL || p->right != NULL)
		{
			if (p->trait == 1)					// if it's an SI node
			{
				(*nodes_si)++;
				if (p->left->trait==0 || p->right->trait==0)	// if it's an SI->SC node
				{
					(*nodes_sisc)++;
					if (p->left->trait == 0)		// if the left branch is SI->SC
						(*branches_sisc)++;
					if (p->right->trait == 0)	// if the right branch is SI->SC
						(*branches_sisc)++;
				}
			}
		}
	}
}


/*******************************************************************************
 * get the sum of the branch lengths coming from nodes of each state
 ******************************************************************************/
void GetStateBranchLengths(TreeNode *p, double *sum0, double *sum1)
{
	if (p != NULL)
	{
		GetStateBranchLengths(p->left, sum0, sum1);
		GetStateBranchLengths(p->right, sum0, sum1);
		if (p->anc != NULL)
		{
			if (p->anc->trait == 0)
				(*sum0) += p->length;
			else if (p->anc->trait == 1)
				(*sum1) += p->length;
		}
	}
}


/*******************************************************************************
 * set all internal node states to 0
 ******************************************************************************/
void ZeroNodes(TreeNode *p)
{
	if (p != NULL)
	{
		ZeroNodes(p->left);
		ZeroNodes(p->right);

		// if not at a tip
		if (p->left != NULL || p->right != NULL)
			p->trait = 0;
	}
}


/*******************************************************************************
 * trace SI from tips to root
 ******************************************************************************/
void TraceSI(TreeNode *p)
{
	if (p != NULL)
	{
		TraceSI(p->left);
		TraceSI(p->right);
		
		// if at a tip
		if (p->left == NULL && p->right == NULL)
		{
			if (p->trait == 1)
			{
				while (p->anc != NULL)	// trace SI back in tree
				{
					p->trait = 1;
					p = p->anc;
				}
			}
		}
	}
}
