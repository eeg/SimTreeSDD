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
