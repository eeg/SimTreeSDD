#include <stdio.h>
#include "nodes.h"
#include "copy.h"

/******************************************************************************
 * copy one tree (p) to a new one (q)
 * (likely call with CopyTree(oldRoot, newRoot);)
 *****************************************************************************/

void CopyTree(TreeNode *p, TreeNode *q)
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
		CopyTree(p->left, q->left);

		if (p->right == NULL)
			q->right = NULL;
		else
		{
			q->right = NewNode(q, (p->right)->time);
			CopyNode(p->right, q->right);
		}
		CopyTree(p->right, q->right);
	}
}

void CopyNode(TreeNode *old, TreeNode *new)
{
	new->length = old->length;
	new->index = old->index;
	new->trait = old->trait;
	// NOTE: cl[] is not copied
}
