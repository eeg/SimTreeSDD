#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "build_common.h"
#include "input_sim.h"
#include "output.h"
#include "randist.h"

int node_counter;			// keep track of number of nodes in the tree


int AssignRootState(TreeNode *root, TreeParams *parameters)
{
	int report;

	if (parameters->root_state == 0)
	{
		root->trait = 0;
		report = 0;
	}
	else if (parameters->root_state == 1)
	{
		root->trait = 1;
		report = 1;
	}
	else if (parameters->root_state == 2)
	{
		root->trait = 2;
		report = 2;
	}
	// TODO: allow stationary distribution for 2regions
	else
	{
		if (uniform_distribution(0,1) < parameters->transition[1]/(parameters->transition[0]+parameters->transition[1]))
		{
			root->trait = 0;
			report = 2;
		}
		else
		{
			root->trait = 1;
			report = 3;
		}
	}

	return report;
}


/******************************************************************************
 * make sure the root has two descendants, not just one
 *****************************************************************************/
TreeNode *MoveRoot(TreeNode *root)
{
	TreeNode *temp;
	int report;

	if (root->left != NULL && root->right == NULL)
	{
		temp = root->left;
		free(root);
		node_counter--;
		root = temp;
		root->anc = NULL;
		report = 1;
	}
	else if (root->left == NULL && root->right != NULL)
	{
		temp = root->right;
		free(root);
		node_counter--;
		root = temp;
		root->anc = NULL;
		report = 1;
	}
	else
		report = 0;
	ShowRootMove(report);

	return root;
}
