#include <stdio.h>
#include "nodes.h"


/*******************************************************************************
 * put the character states of the tips into an array
 ******************************************************************************/
void GetTipStates(TreeNode *p, int states[])
{
	if (p->left == NULL && p->right == NULL)
		states[p->index] = p->trait;
	else
	{
		GetTipStates(p->left, states);
		GetTipStates(p->right, states);
	}
}


/*******************************************************************************
 * put the node (internal and tips) times into an array
 ******************************************************************************/
void GetNodeTimes(TreeNode *p, double times[])
{
	if (p != NULL)
	{
		GetNodeTimes(p->left, times);
		GetNodeTimes(p->right, times);
		times[p->index] = p->time;
	}
}


/*******************************************************************************
 * put the node depths for each tip into an array
 * (node depth = number of nodes from tip to root)
 ******************************************************************************/
void GetNodeDepth(TreeNode *p, int depths[])
{
	int index;
	if (p != NULL)
	{
		GetNodeDepth(p->left,  depths);
		GetNodeDepth(p->right, depths);
		if (p->left == NULL && p->right == NULL)
		{
			index = p->index;
			while (p->anc != NULL)
			{
				depths[index]++;
				p = p->anc;
			}
		}
	}
}

/*******************************************************************************
 * put the branch lengths (internal and tips) into an array
 * only use the first n_tips*2-2 entries in lengths[]
 * (remove/ignore the root value = the next entry)
 ******************************************************************************/
void GetBranchLengths(TreeNode *p, double lengths[])
{
	if (p != NULL)
	{
		GetBranchLengths(p->left, lengths);
		GetBranchLengths(p->right, lengths);
		lengths[p->index] = p->length;
	}
}


/*******************************************************************************
 * count the number of tips in each character state
 * (note that for a "tree" consisting of only a root, the root will be counted
 *    as a tip here, so make sure to avoid that case)
 ******************************************************************************/
void CountTipStates(TreeNode *p, int tip_counter[])
{
	if (p != NULL)
	{
		if (p->left == NULL && p->right == NULL)
			tip_counter[p->trait]++;
		else
		{
			CountTipStates(p->left, tip_counter);
			CountTipStates(p->right, tip_counter);
		}
	}
}


/*******************************************************************************
 * count the number of tips
 * (note that for a "tree" consisting of only a root, the root will be counted
 *    as a tip here, so make sure to avoid that case)
 * (and don't forget to set the counter to 0 before calling CountTips)
 ******************************************************************************/
void CountTips(TreeNode *p, int *counter)
{
	if (p != NULL)
	{
		if (p->left == NULL && p->right == NULL)
			(*counter)++;
		else
		{
			CountTips(p->left, counter);
			CountTips(p->right, counter);
		}
	}
}
