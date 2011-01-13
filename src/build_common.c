/******************************************************************************
* Copyright 2010 Emma Goldberg
* 
* This file is part of SimTreeSDD.
* 
* SimTreeSDD is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* SimTreeSDD is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with SimTreeSDD.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


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
