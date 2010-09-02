#include <stdio.h>
#include "nodes.h"
#include "copy.h"




/******************************************************************************
 * this is mostly a wrapper around CopyTreeGuts, which does the hard work
 * call this as:
 *   TreeNode *newroot;
 *   newroot = CopyTree(oldroot);
 *****************************************************************************/
TreeNode *CopyTree(TreeNode *oldroot)
{
	TreeNode *newroot;

	newroot = NewNode(NULL, oldroot->time);
	CopyNode(oldroot, newroot);
	CopyTreeGuts(oldroot, newroot);

	return newroot;
}


/******************************************************************************
 * copy one tree (p) to a new one (q)
 * (likely call with CopyTreeGuts(oldRoot, newRoot);)
 *****************************************************************************/
void CopyTreeGuts(TreeNode *p, TreeNode *q)
{
	if (p != NULL)
	{
		if (p->left == NULL)
			q->left = NULL;
		else
		{
			q->left = NewNode(q, (p->left)->time);
			CopyNode(p->left, q->left);
		}
		CopyTreeGuts(p->left, q->left);

		if (p->right == NULL)
			q->right = NULL;
		else
		{
			q->right = NewNode(q, (p->right)->time);
			CopyNode(p->right, q->right);
		}
		CopyTreeGuts(p->right, q->right);
	}
}


void CopyNode(TreeNode *old, TreeNode *new)
{
	new->length = old->length;
	new->index = old->index;
	new->trait = old->trait;
	// NOTE: cl[] is not copied
}
