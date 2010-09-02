#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "build.h"
#include "buildcheck.h"
#include "extra.h"
#include "input_sim.h"
#include "randist.h"

//extern int node_counter;		// defined in birth.c, mentioned in buildcheck.h

/*******************************************************************************
 * this is identical to BirthDeath in build.c except that it exits when there
 *   is more than one tip
 ******************************************************************************/
void BirthDeathCount(TreeNode *root, TreeNode *here, int direction, TreeParams *parameters, int *tips_so_far)
{
	double t, which;
	double lambda, mu;
	TreeNode *temp;
	int trait_now;

	if (node_counter > TOO_BIG)
	{
		fprintf(stderr, "more than %d nodes ... aborting\n", TOO_BIG);
		return;
	}

	if (*tips_so_far > 1)
		return;

	/*	direction = 0: need to go left
	 *	direction = 1: have gone left, need to go right
	 *	direction = 2: have gone both left and right, need to back up
	 */

	if (direction == 2)			// if you've already gone left and right
	{
		if (here != root) 
			// see if this node should be kept, and then keep building the tree
			BackUpCount(root, here, parameters, tips_so_far);

		/* Now you're done, so exit the BirthDeathCount recursion. */
	}

	else 	// direction = 0 or 1: you haven't already gone both left and right
	{
		/* keep track of character changes until the branch ends (from birth or death) */
		trait_now = GetNextBD(here, parameters, &t);

		lambda = parameters->birth[trait_now];
		mu = parameters->death[trait_now];

		/* if there was no birth or death before reaching the stopping time */
		if ((t + here->time) >= parameters->end_t)
		{
			temp = NewNode(here, parameters->end_t);
			temp->trait = trait_now;
			node_counter++;
			(*tips_so_far)++;

			if (direction == 0)					// if going left
				here->left = temp;
			else 							// if going right
				here->right = temp;

			BirthDeathCount(root, here, ++direction, parameters, tips_so_far);
		}

		/* if there was a birth or death before reaching the stopping time */
		else
		{
			which = uniform_distribution(0, lambda+mu);
	
			if (which < lambda)				// speciation, so a new node is created
			{
				temp = NewNode(here, (here->time)+t);
				temp->trait = trait_now;
				node_counter++;

				if (direction == 0)			// if going left
				{
					here->left = temp;
					here = here->left;
				}
				else						// if going right
				{
					here->right = temp;
					here = here->right;
				}
	
				// enter another recursive layer, building the tree to the left from the new node
				BirthDeathCount(root, here, 0, parameters, tips_so_far);
			}
	
			else							// extinction along that branch, so no new node is created
				// enter another recursive layer, continuing from the same node
				BirthDeathCount(root, here, ++direction, parameters, tips_so_far);
		}
	}
}


/*******************************************************************************
 * this is identical to BackUp in build.c except that it passes tips_so_far to
 *   allow BirthDeathCount to exit when there is more than one tip
 ******************************************************************************/
void BackUpCount(TreeNode *root, TreeNode *here, TreeParams *parameters, int *tips_so_far)
{
	int from_right;

	// see if you will be dropping back from the left or the right
	if ((here->anc)->right == here)
		from_right = 1;
	else
		from_right = 0;
	
	// option 1: if there are no descendents, remove the node and back up
	if (here->left == NULL && here->right == NULL)
	{
		here = FreeNode(here);
		node_counter--;
		if (from_right == 0)
			here->left = NULL;
		else
			here->right = NULL;
	}

	// option 2: if there is only one descendent, collapse the node and back up
	else if (here->left != NULL && here->right == NULL)
	{
		if (from_right == 0)
			here->anc->left = here->left;
		else
			here->anc->right = here->left;
		here->left->anc = here->anc;

		here = FreeNode(here);
		node_counter--;
	}
	else if (here->left == NULL && here->right != NULL)
	{
		if (from_right == 0)
			here->anc->left = here->right;
		else
			here->anc->right = here->right;
		here->right->anc = here->anc;

		here = FreeNode(here);
		node_counter--;
	}

	// option 3: if there are two descendents, keep the node and back up
	else
		here = here->anc;

	// now continue building the tree
	BirthDeathCount(root, here, from_right+1, parameters, tips_so_far);
}


/*******************************************************************************
 * start with BirthDeath() from build.c
 * modify it to check the state at a particular time (at checknode)
 *   the state at checknode's time is recorded in the node just before then
 *   (since there need not be a node at this time in the simulated tree)
 * note that the way this is called in Get2Like, root is actually simroot->left
 ******************************************************************************/
void BirthDeathCheck(TreeNode *root, TreeNode *here, int direction, TreeParams *parameters, BranchNode *checknode, int *tips_so_far)
{
	double t, which;
	double lambda, mu;
	TreeNode *temp;
	int trait_now;

	if (node_counter > TOO_BIG)
	{
		fprintf(stderr, "more than %d nodes ... aborting\n", TOO_BIG);
		return;
	}

	if (*tips_so_far > 1)
		return;

	/*	direction = 0: need to go left
	 *	direction = 1: have gone left, need to go right
	 *	direction = 2: have gone both left and right, need to back up
	 */

	if (direction == 2)			// if you've already gone left and right
	{
		if (here != root) 
			// see if this node should be kept, and then keep building the tree
			BackUpCheck(root, here, parameters, checknode, tips_so_far);
		else
			// back up to simroot and don't keep building the tree
			BackUpFinal(here);

		/* Now you're done, so exit the BirthDeathCheck recursion. */
	}

	else 	// direction = 0 or 1: you haven't already gone both left and right
	{
		/* keep track of character changes until the branch ends (from birth or death) */
		trait_now = GetNextBDCheck(here, parameters, &t, checknode, direction);

		lambda = parameters->birth[trait_now];
		mu = parameters->death[trait_now];

		/* if there was no birth or death before reaching the stopping time */
		if ((t + here->time) >= parameters->end_t)
		{
			temp = NewNode(here, parameters->end_t);
			temp->trait = trait_now;
			temp->cl[0] = UNDEF;
			temp->cl[1] = UNDEF;
			node_counter++;
			(*tips_so_far)++;

			if (direction == 0)					// if going left
				here->left = temp;
			else 							// if going right
				here->right = temp;

			BirthDeathCheck(root, here, ++direction, parameters, checknode, tips_so_far);
		}

		/* if there was a birth or death before reaching the stopping time */
		else
		{
			which = uniform_distribution(0, lambda+mu);
	
			// speciation, so a new node is created
			if (which < lambda)
			{
				temp = NewNode(here, (here->time)+t);
				temp->trait = trait_now;
				temp->cl[0] = UNDEF;
				temp->cl[1] = UNDEF;
				node_counter++;

				if (direction == 0)			// if going left
				{
					here->left = temp;
					here = here->left;
				}
				else						// if going right
				{
					here->right = temp;
					here = here->right;
				}
	
				// enter another recursive layer, building the tree to the left from the new node
				BirthDeathCheck(root, here, 0, parameters, checknode, tips_so_far);
			}
	
			// extinction along that branch, so no new node is created
			else
				// enter another recursive layer, continuing from the same node
				BirthDeathCheck(root, here, ++direction, parameters, checknode, tips_so_far);
		}
	}
}


/******************************************************************************
 * When you are at node "here" and direction=2, you need decide whether "here"
 *    is a real node (with two descendents), and then back up to the 
 *    previous/ancestral node.
 *****************************************************************************/
void BackUpCheck(TreeNode *root, TreeNode *here, TreeParams *parameters, BranchNode *checknode, int *tips_so_far)
{
	int from_right;

	// see if you will be dropping back from the left or the right
	if ((here->anc)->right == here)
		from_right = 1;
	else
		from_right = 0;
	
	// option 1: if there are no descendents, remove the node and back up
	if (here->left == NULL && here->right == NULL)
	{
		here = FreeNode(here);
		node_counter--;

		// remove any record of the checked state (don't bother seeing if this node had it recorded)
		if (from_right == 0)
		{
			here->left = NULL;
			here->cl[0] = UNDEF;
		}
		else
		{
			here->right = NULL;
			here->cl[1] = UNDEF;
		}
	}

	// option 2: if there is only one descendent, collapse the node and back up
	else if ( (here->left != NULL && here->right == NULL) || (here->left == NULL && here->right != NULL) )
	{
		// link around the node to be lost
		if (here->left != NULL && here->right == NULL)
		{
			if (from_right == 0)
				here->anc->left = here->left;
			else
				here->anc->right = here->left;
			here->left->anc = here->anc;
		}
		else
		{
			if (from_right == 0)
				here->anc->left = here->right;
			else
				here->anc->right = here->right;
			here->right->anc = here->anc;
		}

		// copy any checkstate info from the node to be lost to the previous node
		if (from_right == 0)
		{
			if (here->cl[0] != UNDEF)
				here->anc->cl[0] = here->cl[0];
			else if (here->cl[1] != UNDEF)
				here->anc->cl[0] = here->cl[1];
		}
		else
		{
			if (here->cl[0] != UNDEF)
				here->anc->cl[1] = here->cl[0];
			else if (here->cl[1] != UNDEF)
				here->anc->cl[1] = here->cl[1];
		}

		// lose the node that got collapsed
		here = FreeNode(here);
		node_counter--;
	}

	// option 3: if there are two descendents, keep the node and back up
	else
		here = here->anc;
		// don't worry about the checkstate info since there is more than one tip
		// TODO: could abort tree-building now, since there is definitely more than one tip

	// now continue building the tree
	BirthDeathCheck(root, here, from_right+1, parameters, checknode, tips_so_far);
}


/******************************************************************************
 * wait for the next birth or death, while keeping track of character changes
 *****************************************************************************/
double GetNextBDCheck(TreeNode *p, TreeParams *parameters, double *t, BranchNode *checknode, int direction)
{
	double t1, t2;
	double lambda, mu, trans;
	int trait_now = p->trait;
	int pasttime;

	*t = 0;	// this will become equal to the time to the next birth/death event

	if (p->time < checknode->time)
		pasttime = 0;
	else
		pasttime = 1;

	for (;;)
	{
		lambda = parameters->birth[trait_now];
		mu = parameters->death[trait_now];
		trans = parameters->transition[trait_now];

		t1 = exponential_distribution(trans);		// time to next character change
		t2 = exponential_distribution(lambda+mu);	// time to next birth or death

		if (t1 < t2)	// if the character changes before the next birth or death
		{
			*t += t1;

			// see if you've just passed the time you're checking
			if (pasttime == 0 && *t+p->time >= checknode->time)
			{
				p->cl[direction] = trait_now;
				pasttime = 1;
			}

			// if you've reached the stopping time, stop without doing anything else
			if ((*t + p->time) >= parameters->end_t)
				break;

			// otherwise, the trait changes
			trait_now = (trait_now+1)%2;
		}
		else			// if there is a birth or death first
		{
			*t += t2;

			// see if you've just passed the time you're checking
			if (pasttime == 0 && *t+p->time >= checknode->time)
			{
				p->cl[direction] = trait_now;
				pasttime = 1;
			}
			break;
		}
	}
	return trait_now;
}


/*******************************************************************************
 * this is a trimmed version of BackUp[Check/Count] for falling back to simroot
 * it assumes from_right = 0, and it does not call the BirthDeath recursion
 ******************************************************************************/
void BackUpFinal(TreeNode *here)
{
	// note: from_right is always 0, since backing up to simroot
	
	// option 1: if there are no descendents, remove the node and back up
	if (here->left == NULL && here->right == NULL)
	{
		here = FreeNode(here);
		node_counter--;

		here->left = NULL;
		// remove any record of the checked state (don't bother seeing if this node had it recorded)
		here->cl[0] = UNDEF;
	}

	// option 2: if there is only one descendent lineage, collapse the node and back up
	else if ( (here->left != NULL && here->right == NULL) || (here->left == NULL && here->right != NULL) )
	{
		// link around the node to be lost
		if (here->left != NULL && here->right == NULL)
		{
			here->anc->left = here->left;
			here->left->anc = here->anc;
		}
		else
		{
			here->anc->left = here->right;
			here->right->anc = here->anc;
		}

		// copy the checkstate info from the node to be lost to the previous node
		if (here->cl[0] != UNDEF)
			here->anc->cl[0] = here->cl[0];
		else if (here->cl[1] != UNDEF)
			here->anc->cl[0] = here->cl[1];

		// lose the node that got collapsed
		here = FreeNode(here);
		node_counter--;
	}

	// option 3: if there are two descendents, keep the node and back up
	// (don't bother with the cl's because there is more than one tip)
	else
		here = here->anc;
}
