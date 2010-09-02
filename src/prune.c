#include <stdio.h>
#include "nodes.h"
#include "extra.h"
#include "prune.h"


/*******************************************************************************
 * this prunes a tree (root at p), removing tips (and collapsing corresponding
 *   interior nodes) that do NOT have trait = where
 ******************************************************************************/

void PruneTree(TreeNode *p, int where)
{
	int from_right;

	if (p != NULL)
	{
		if ( (p->left==NULL && p->right==NULL) )	// at a tip
		{
			// if this tip should be cut off
			if (p->trait != where)
			{
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
				else
				{
					if (p->right != NULL)
						PruneTree(p->right, where);
					// else if falling back to the root from the right, you're done
				}
			}
		}
		else
		{
			// continue from here, if this node hasn't been collapsed
			if (p->index != UNDEF)
				PruneTree(p->left, where);
			if (p->index != UNDEF)
				PruneTree(p->right, where);
		}
	}
}


/*******************************************************************************
 * this prunes a tree (root at p), removing tips (and collapsing corresponding
 *   interior nodes) that DO have trait = where
 ******************************************************************************/

void PruneTreeNeg(TreeNode *p, int where)
{
	int from_right;

	if (p != NULL)
	{
		if ( (p->left==NULL && p->right==NULL) )	// at a tip
		{
			// if this tip should be cut off
			if (p->trait == where)
			{
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
				else
				{
					if (p->right != NULL)
						PruneTreeNeg(p->right, where);
					// else if falling back to the root from the right, you're done
				}
			}
		}
		else
		{
			// continue from here, if this node hasn't been collapsed
			if (p->index != UNDEF)
				PruneTreeNeg(p->left, where);
			if (p->index != UNDEF)
				PruneTreeNeg(p->right, where);
		}
	}
}
