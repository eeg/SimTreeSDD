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
#include <stdlib.h>
#include "label.h"


int intNodeNum;


/*******************************************************************************
 * assign values for branch lengths (distance from node to its ancestor)
 * this assumes that node creation times are already known (e.g. from simulator)
*******************************************************************************/
void AssignBranchLengths(TreeNode *p)
{
	if (p != NULL)
	{
		AssignBranchLengths(p->left);
		AssignBranchLengths(p->right);
		if (p->anc != NULL)
			p->length = p->time - p->anc->time;
		else
			p->length = 0;
	}
}


/*******************************************************************************
 * assign values for node creation times (root at time 0)
 * this assumes that branch lengths are already known (e.g. from tree read in)
*******************************************************************************/
void AssignNodeTimes(TreeNode *p)
{
	if (p != NULL)
	{
		if (p->anc != NULL)
			p->time = p->anc->time + p->length;
		else
			p->time = 0;
		AssignNodeTimes(p->left);
		AssignNodeTimes(p->right);
	}
}


void LabelTips(TreeNode *p)
{
	if (p != NULL)
	{
		LabelTips (p->left);
		LabelTips (p->right);
		if (p->left == NULL && p->right == NULL)
		{
			p->index = intNodeNum++;
		}
	}
}


void LabelInteriorNodes(TreeNode *p)
{
	if (p != NULL)
	{
		LabelInteriorNodes (p->left);
		LabelInteriorNodes (p->right);
		if (p->left == NULL && p->right == NULL)
		{
		}
		else
		{
			p->index = intNodeNum++;
		}
	}
}
