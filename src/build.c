#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "build.h"
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
	else
	{
/* WRONG! changed 13 Sept 07
		if (uniform_distribution(0,1) < parameters->transition[0]/(parameters->transition[0]+parameters->transition[1]))
*/
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
 * this does all the hard work in building the birth-death tree
 *****************************************************************************/
void BirthDeath(TreeNode *root, TreeNode *here, int direction, TreeParams *parameters)
{
	double t, which;
	double lambda, mu;
	TreeNode *temp;
	int trait_now;


	if (node_counter > TOO_BIG)
	{
//		fprintf(stderr, "more than %d nodes ... aborting\n", TOO_BIG);
		return;
	}

	/*	direction = 0: need to go left
	 *	direction = 1: have gone left, need to go right
	 *	direction = 2: have gone both left and right, need to back up
	 */

	if (direction == 2)			// if you've already gone left and right
	{
		if (here != root) 
			// see if this node should be kept, and then keep building the tree
			BackUp(root, here, parameters);

		/* Now you're done, so exit the BirthDeath recursion. */
	}

	else 	// direction = 0 or 1: you haven't already gone both left and right
	{
		/* keep track of character changes until the branch ends (from birth or death) */
		trait_now = GetNextBD(here, parameters, &t);

		/* if there was no birth or death before reaching the stopping time */
		if ((t + here->time) >= parameters->end_t)
		{
			temp = NewNode(here, parameters->end_t);
			temp->trait = trait_now;
			node_counter++;

			if (direction == 0)					// if going left
				here->left = temp;
			else 							// if going right
				here->right = temp;

			BirthDeath(root, here, ++direction, parameters);
		}

		/* if there was a birth or death before reaching the stopping time */
		else
		{
			lambda = parameters->birth[trait_now];
			mu = parameters->death[trait_now];
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
				BirthDeath(root, here, 0, parameters);
			}
	
			else							// extinction along that branch, so no new node is created
				// enter another recursive layer, continuing from the same node
				BirthDeath(root, here, ++direction, parameters);
		}
	}
}


/******************************************************************************
 * wait for the next birth or death, while keeping track of character changes
 *****************************************************************************/
double GetNextBD(TreeNode *p, TreeParams *parameters, double *t)
{
	double t1, t2;
	double lambda, mu, trans;
	int trait_now = p->trait;

	*t = 0;
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
			// if you've reached the stopping time, stop without doing anything else
			if ((*t + p->time) >= parameters->end_t)
				break;
			// otherwise, the trait changes
			trait_now = (trait_now+1)%2;
		}
		else			// if there is a birth or death first
		{
			*t += t2;
			break;
		}
	}
	return trait_now;
}


/******************************************************************************
 * When you are at node "here" and direction=2, you need decide whether "here"
 *    is a real node (with two descendents), and then back up to the 
 *    previous/ancestral node.
 *****************************************************************************/
void BackUp(TreeNode *root, TreeNode *here, TreeParams *parameters)
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
	BirthDeath(root, here, from_right+1, parameters);
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
