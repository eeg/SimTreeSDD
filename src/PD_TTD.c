/*******************************************************************************
 * For the phylogenetic diversity over space project, started at Silwood June 2009.
 *
 * Read in a two-region tree.
 * Get the number of species, phylogenetic diversity (PD), and total taxonomic 
 *   distinctiveness (TTD, using branch lengths) for the whole tree, ignoring 
 *   character states.
 *
 * Then prune the tree to get subtrees for each region (states 0+1 and 0+2)
 * For each subtree, also get N, PD, and TTD.
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


void SumUniqueBranches(TreeNode *p, double *total_branch_length);   // for PD
void CladeInfo(TreeNode *p, double clade_info[][3]);                // for TTD
void SingleTipLength(TreeNode *p, int region, double *length);      // for PD


int main(int argc, char *argv[])
{
	FILE *fp;

	TreeNode *treeRoot, *prune;

	int r, t, i;
	int SR[3];
	double PD[3];
	double TTD[3];
	double branch_sum;
	int tip_counter[3];
	int n_tips;
	double tip_time;

	// check the program call
	if (argc < 2)
	{
		fprintf(stderr, "\nERROR: need to specify a tree .ttn file(s)\n\n");
 		return -1;
	}

	fp = fopen("pd-ttd.dat", "a");
	fprintf(fp, "tree\tSR\tSR.A\tSR.B\tPD\tPD.A\tPD.B\tTTD\tTTD.A\tTTD.B\n");

	// loop over all given trees
	for (t=1; t<argc; t++)
	{
		printf("working on %s\n", argv[t]);
		/* make a labelled tree from the input file */
		treeRoot = ReadTree(argv[t]);
		double clade_info[intNodeNum][3];

		for (r=0; r<3; r++)
		{
			tip_counter[r] = 0;
		}

		CountTipStates(treeRoot, tip_counter);
		SR[0] = tip_counter[0] + tip_counter[1] + tip_counter[2];
		SR[1] = tip_counter[0] + tip_counter[1];
		SR[2] = tip_counter[0] + tip_counter[2];
		n_tips = SR[0];

		/* get stuff from complete tree */

		branch_sum = 0;
		SumUniqueBranches(treeRoot, &branch_sum);
		PD[0] = branch_sum;

		/*--------------------------------------------------
		 * in clade_info:
		 *    0: number of tips in the clade
		 *    1: number of tip-to-tip combos in the clade
		 *    2: time of the node (root is zero)
		*--------------------------------------------------*/
		branch_sum = 0;
		CladeInfo(treeRoot, clade_info);
		tip_time = clade_info[0][2];
		for (i=n_tips; i<n_tips*2-1; i++)     // for each internal node
		{
			branch_sum += clade_info[i][1] * (tip_time - clade_info[i][2]) * 2;
		}
		TTD[0] = branch_sum / (SR[0] - 1);

		/* get stuff from the two pruned trees */

		if (SR[1] > 1 && SR[2] > 1)
		{
			for (r=1; r<3; r++)
			{
				if (SR[r] == 1)
				{
					SingleTipLength(treeRoot, r, &PD[r]);
					TTD[r] = 0;
				}
				else
				{
					// prune and clean up the tree
					prune = CopyTree(treeRoot);
					PruneTreeNeg(prune, (r%2)+1);   // get rid of other-region endemics
					prune = MoveRoot(prune);
					AssignBranchLengths(prune);
					intNodeNum = 0;
					LabelTips(prune);
					LabelInteriorNodes(prune);

					branch_sum = 0;
					SumUniqueBranches(prune, &branch_sum);
					PD[r] = branch_sum;

					branch_sum = 0;
					CladeInfo(prune, clade_info);
					n_tips = SR[r];
					for (i=n_tips; i<n_tips*2-1; i++)     // for each internal node
						branch_sum += clade_info[i][1] * (tip_time - clade_info[i][2]) * 2;
					TTD[r] = branch_sum / (SR[r] - 1);

					FreeTree(prune);
				}
			}
		}
		else
		{
			for (r=1; r<3; r++)
			{
				PD[r] = -1;
				TTD[r] = -1;
			}
		}
		/*--------------------------------------------------
		* else if (SR[1] == 0)
		* {
		* 	PD[1] = 0;
		* 	TTD[1] = 0;
		* 	PD[2] = PD[0];
		* 	TTD[2] = TTD[0];
		* }
		* else if (SR[2] == 0)
		* {
		* printf("hi\n");
		* 	PD[2] = 0;
		* 	TTD[2] = 0;
		* 	PD[1] = PD[0];
		* 	TTD[1] = TTD[0];
		* }
		* else
		* {
		* 	printf("ERROR: check small SR logic\n");
		* 	return -1;
		* }
		*--------------------------------------------------*/

		FreeTree(treeRoot);

		/* write out the results */
		fprintf(fp, "%s\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\n", argv[t], SR[0], SR[1], SR[2], PD[0], PD[1], PD[2], TTD[0], TTD[1], TTD[2]);
	}

	fclose(fp);

	return 0;
}

/*
 * when only a single tip is in a region (endemic or not), just look up its tip 
 *   length rather than pruning the tree
 */
void SingleTipLength(TreeNode *p, int region, double *length)
{
	SingleTipLength(p->left, region, length);
	SingleTipLength(p->right, region, length);
	if (p->left == NULL && p->right == NULL)
	{
		if (p->trait == 0 || p->trait == region)
		{
			*length = p->length;
		}
	}
}

/*
 * sum the branch lengths on the whole tree
 */
void SumUniqueBranches(TreeNode *p, double *total_branch_length)
{
	if (p != NULL)
	{
	 	*total_branch_length += p->length;
	 	SumUniqueBranches(p->left, total_branch_length);
	 	SumUniqueBranches(p->right, total_branch_length);
	}
}



/*
* 0: number of tips in the clade
* 1: number of tip-to-tip combos in the clade
* 2: time of the node (root is zero)
*/
void CladeInfo(TreeNode *p, double clade_info[][3])
{
	int i = p->index;
	int d1, d2;

 	if (p->left == NULL && p->right == NULL)
	{
		clade_info[i][0] = 1;
		clade_info[i][1] = 1;
		clade_info[i][2] = p->time;
	}
	else
	{
		CladeInfo(p->left, clade_info);
		CladeInfo(p->right, clade_info);

		d1 = p->left->index;
		d2 = p->right->index;
		clade_info[i][0] = clade_info[d1][0] + clade_info[d2][0];
		clade_info[i][1] = clade_info[d1][0] * clade_info[d2][0];
		clade_info[i][2] = p->time;
	}
	//printf("%d: %f  %f  %f\n", i, clade_info[i][0], clade_info[i][1], clade_info[i][2]);
}
/*--------------------------------------------------
* 0: positive if this clade contains the focal tip; zero if not
* 1: number of tips in the clade
* 2: number of tip-to-tip combos in the clade
* 3: time of the node (root is zero)
* void CladeInfo(TreeNode *p, double clade_info[][4], int tip)
* {
* 	int i = p->index;
* 	int d1, d2;
* 
*  	if (p->left == NULL && p->right == NULL)
* 	{
* 		if (i == tip)
* 			clade_info[i][0] = 1;
* 		else
* 			clade_info[i][0] = 0;
* 		clade_info[i][1] = 1;
* 		clade_info[i][2] = 1;
* 		clade_info[i][3] = p->time;
* 	}
* 	else
* 	{
* 		CladeInfo(p->left, clade_info, tip);
* 		CladeInfo(p->right, clade_info, tip);
* 
* 		d1 = p->left->index;
* 		d2 = p->right->index;
* 		clade_info[i][0] = clade_info[d1][0] + clade_info[d2][0];
* 		clade_info[i][1] = clade_info[d1][1] + clade_info[d2][1];
* 		clade_info[i][2] = clade_info[d1][1] * clade_info[d2][1];
* 		clade_info[i][3] = p->time;
* 	}
* 	printf("%d  (%d): %f  %f  %f  %f\n", tip, i, clade_info[i][0], clade_info[i][1], clade_info[i][2], clade_info[i][3]);
* }
*--------------------------------------------------*/

/*--------------------------------------------------
* void SumTipDistances(TreeNode *p, int tip, double tip_time, double *total_branch_length)
* {
*  	if (p->left != NULL && p->right != NULL)
* 	{
* 		SumPairwiseBranches(p->left, tip, tip_time, total_branch_length);
* 		SumPairwiseBranches(p->right, tip, tip_time, total_branch_length);
* 		LookBothWays(p, tip, tip_time, total_branch_length);
* 	}
* }
* 
* void LookBothWays(TreeNode *p, int tip, double tip_time, double *total_branch_length)
* {
* 	int n_left = 0;
* 	int n_right = 0;
* 	int found = 0;
* 	double path = 0;
* 	double combos = 0;
* 
* 	FindTip(p->left, tip, &n_left, &found);
* 	FindTip(p->right, tip, &n_left, &found);
* 
* 	if (found == 1)   // if the focal tip is in this clade
* 		path = 2 * (tip_time - p->time);
* 	else
* 		*total_branch_length += 2 * tip_time;
* 
* }
*--------------------------------------------------*/


/*
 * sum the branch lengths across all pairs of tips
 */
/*--------------------------------------------------
* void SumPairwiseBranches(TreeNode *p, double *total_branch_length)
* {
* 	if (p->left == NULL && p->right == NULL)
* 	{
* 		TreeNode *q;
* 		double length_so_far;
* 
* 		if (p->anc->left == p)
* 			q = p->anc->right;
* 		else
* 			q = p->anc->left;
* 		
* 		length_so_far = p->length;
* 		printf("%d ... \n", p->index);
* 		SumLengths(q, total_branch_length, &length_so_far);
* 	}
* 	else
* 	{
* 		SumPairwiseBranches(p->left, total_branch_length);
* 		SumPairwiseBranches(p->right, total_branch_length);
* 	}
* }
* 
*--------------------------------------------------*/
/* visit all tips descended from the node and sum the path traversed */
/*--------------------------------------------------
* void SumLengths(TreeNode *p, double *total_branch_length, double *length_so_far)
* {
* 	if (p->left == NULL && p->right == NULL)
* 	{
* 		*total_branch_length += *length_so_far + p->length;
* 		printf("     %d: path = %f, total = %f\n", p->index, *length_so_far + p->length, *total_branch_length);
* 	}
* 	else
* 	{
* 		*length_so_far += p->length;
* 		SumLengths(p->left, total_branch_length, length_so_far);
* 		SumLengths(p->right, total_branch_length, length_so_far);
* 	}
* }
*--------------------------------------------------*/


/*--------------------------------------------------
* void SumPairwiseBranches(TreeNode *p, double *total_branch_length)
* {
* 	if (p->left == NULL && p->right == NULL)
* 	{
*  		double length_so_far = p->length;
* 		//VisitOthers(p, total_branch_length, &length_so_far);
* 		printf("from tip %d ... \n", p->index);
* 		//VisitOthers(p, p->anc, total_branch_length, &length_so_far);
* 		VisitOtherTips(p->anc, 1, total_branch_length, &length_so_far);
* 	}
* 	else
* 	{
* 		SumPairwiseBranches(p->left, total_branch_length);
* 		SumPairwiseBranches(p->right, total_branch_length);
* 	}
* }
* 
* void VisitOtherTips(TreeNode *here, int direction, double *total_branch_length, double *length_so_far)
* {
*  	printf("     at node/tip %d ... \n", here->index);
* 
* 	if (here->left == NULL && here->right == NULL)
* 	{
* 		*total_branch_length += *length_so_far;
* 	}
* 
* 	else
* 	{
* 		if (direction == 0)
* 		{
* 			VisitOtherTips(here->left, 0, total_branch_length, length_so_far);
* 			VisitOtherTips(here->right, 0, total_branch_length, length_so_far);
* 		}
* 		else if (direction == 1)
* 		{
* 			VisitOtherTips(here->right, 0, total_branch_length, length_so_far);
* 		}	
* 		else if (direction == 2)
* 		{
* 			if (here->anc->left == here)
* 				VisitOtherTips(here->anc, 1, total_branch_length, length_so_far);
* 			else // (here->anc->right == here)
* 				VisitOtherTips(here->anc, 2, total_branch_length, length_so_far);
* 		}	
* 	}
* }
*--------------------------------------------------*/

/*
 * if right hasn't been done, go right
 * else if there is an anc, go back
 * else if it's a tip, stop and fall back
 */
/*--------------------------------------------------
* void VisitOthers(TreeNode *prev, TreeNode *p, double *total_branch_length, double *length_so_far)
* {
* 	printf("     at node/tip %d ... \n", p->index);
* 	if (p->left == NULL && p->right == NULL)
* 	{
* 		*total_branch_length += *length_so_far + p->length;
* 	}
* 	else if (p->left == prev)
* 	{
* 		VisitOthers(p, p->right, total_branch_length, length_so_far);
* 	}
* 	else
* 	{
* 		*length_so_far += p->length;
* 		VisitOthers(p, p->left, total_branch_length, length_so_far);
* 		VisitOthers(p, p->right, total_branch_length, length_so_far);
* 	}
* 
* 	if (p->anc != NULL)
* 	{
* 		*length_so_far += p->length;
* 		VisitOthers(p, p->anc, total_branch_length, length_so_far);
* 	}
* }
*--------------------------------------------------*/

/* fall back, then visit other descendents */
/*--------------------------------------------------
* void VisitOthers(TreeNode *p, double *total_branch_length, double *length_so_far)
* {
* 	TreeNode *q;
* 
* 	if (p->anc->left == p)
* 		q = p->anc->right;
* 	else
* 		q = p->anc->left;
* 	*length_so_far += p->length;
* 
* 	if (q->left == NULL && q->right == NULL)
* 	{
*  		*total_branch_length += *length_so_far + q->length;
* 	}
* 	VisitOthers(p->anc, total_branch_length, length_so_far);
* }
*--------------------------------------------------*/

/*--------------------------------------------------
* input node = p
* fall back from p = q
* visit all other descendents q
* 	add to total when a tip is visited
*--------------------------------------------------*/
