/******************************************************************************
 * prune a tree down to a specified number or fraction of tips
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "build.h"
#include "extra.h"
#include "label.h"
#include "nodes.h"
#include "output.h"
#include "randist.h"
#include "read_tree.h"
#include "report.h"


void FillTipArray(TreeNode *p, TreeNode *tips[]);
void RemoveTip(TreeNode *p);


int main(int argc, char *argv[])
{
	int t, i;
	TreeNode *treeRoot;
	TreeNode *tips[TOO_BIG];
	int rm_tips[TOO_BIG];
	int n_tips, n_choose, n_choose_temp;
	char outfile[100];


	// check the program call
	if (argc < 2)
	{
		fprintf(stderr, "\nERROR: need to specify a tree .ttn file(s)\n\n");
 		return -1;
	}

	srandom(time(0));

	/***
	 * for each tree in the program call
	 *   read in and label the tree
	 *   prune the tree to the desired size
	 *   write resulting .ttn file
	 ***/

	for (t=1; t<argc; t++)
	{
		// read in the tree
		treeRoot = ReadTree(argv[t]);	// (this assigns intNodeNum)
		n_tips = (intNodeNum+1)/2;

		// make an array of the tips
		FillTipArray(treeRoot, tips);

		// decide how many tips to remove
		//n_choose = (int) n_tips * (1 - 314./2696.);
		//n_choose = (int) n_tips*(1-0.0777);	// keep 7.8%
		//n_choose = (int) n_tips*(1-0.1141);	// keep 11.4%
		n_choose = (int) n_tips*(1-0.1339);	// keep 308/2300
		//n_choose = n_tips - 5;		// keep 5

		// choose tips to remove (Knuth's algorithm)
		n_choose_temp = n_choose;
		for (i=0; i<n_tips && n_choose_temp>0; i++)
		{
			if (uniform_distribution(0, n_tips-i) < n_choose_temp)
			{
				rm_tips[n_choose-n_choose_temp] = i;
				n_choose_temp--;
			}
		}

		for (i=0; i<n_choose; i++)
		{
//			printf("removing tip %d\n", rm_tips[i]);
			RemoveTip(tips[rm_tips[i]]);
		}
		treeRoot = MoveRoot(treeRoot);
		AssignBranchLengths(treeRoot);

		/*--------------------------------------------------
		* // add prefix
		* strcpy(outfile, "sm-");
		* strcat(outfile, argv[t]);
		* ChangeFileSuffix(outfile, ".ttn", "", outfile);
		*--------------------------------------------------*/
		// or add suffix
		ChangeFileSuffix(argv[t], ".ttn", "-sm", outfile);

		WriteTTNFile(treeRoot, outfile);

		FreeTree(treeRoot);
	}

	return 0;
}


/* fill an array of pointers to all the tree's tips */
void FillTipArray(TreeNode *p, TreeNode *tips[])
{
	if (p->left == NULL && p->right == NULL)
		tips[p->index] = p;
	else
	{
		FillTipArray(p->left, tips);
		FillTipArray(p->right, tips);
	}
}


/* remove the tip p */
void RemoveTip(TreeNode *p)
{
	int from_right;

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
}
