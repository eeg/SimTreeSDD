#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "build.h"
#include "build_2regions.h"
#include "input_sim.h"
#include "output.h"
#include "randist.h"


extern int node_counter;			// keep track of number of nodes in the tree

/***
 * NOTE about trait values:
 *   for a newly created node, trait is the location in which it arose (1 or 2)
 *   while waiting along a branch, trait can change due to dispersal or 
 *     extinction (becoming 0, 1, or 2)
 *   for a tip, trait is the location in which it is now (0, 1, or 2)
 ***/

/******************************************************************************
 * this does the hard work in building the tree
 *****************************************************************************/
void BirthDeath2Regions(TreeNode *root, TreeNode *here, int where, int direction, TreeParams *parameters)
{
	double wait_t;
	TreeNode *temp;
	int todo;

	if (node_counter > TOO_BIG)
	{
		fprintf(stderr, "more than %d nodes ... aborting\n", TOO_BIG);
		return;
	}

	if (direction == 2)
	{
		if (here != root) 
		{
			// see if this node should be kept, and then keep building the tree
			BackUp2Regions(root, here, parameters);
		}

		/* Now you're done, so exit the BirthDeath2Regions recursion. */
	}

	else 	// direction = 0 or 1
	{
		/*****
		 * Allow dispersal and local extinctions to happen.
		 * Exit the function when global extinction or speciation occurs, or
		 *    when the end time is reached.
		 * The return value tells what event happened:
		 *       0: global extinction (species is no longer in either region)
		 *       1: speciation in region 1
		 *       2: speciation in region 2
		 *       3: reached end_t
		 *****/
		wait_t = 0;
		todo = Wait2RegionEvent(&where, here->time, &wait_t, parameters);

		/*****
		 * If global extinction occurred along that branch, 
		 *    enter another recursive layer, continuing from the same node.
		 *****/
		if (todo == 0)
			BirthDeath2Regions(root, here, here->trait, ++direction, parameters);

		/*****
		 * If speciation occurred along that branch, add the new node and 
		 *    continue to the left from it. 
		 *****/
		else if (todo == 1 || todo == 2)
		{
			temp = NewNode(here, (here->time)+wait_t);
			temp->trait = todo;
			node_counter++;
			/*****
			 * NOTE: that is why BirthDeath2Regions needs a location argument: 
			 *       the parent/left species may be in both locations, 
			 *       though the daughter/right species is in only one)
			 *****/

			if (direction == 0)           // if going left
			{
				here->left = temp;
				here = here->left;
			}
			else                          // if going right
			{
				here->right = temp;
				here = here->right;
			}
			
			BirthDeath2Regions(root, here, where, 0, parameters);
		}

		/* if there was no birth or death before reaching the stopping time */
		else if (todo == 3)
		{
			temp = NewNode(here, parameters->end_t);
			temp->trait = where;
			node_counter++;

			if (direction == 0)
				here->left = temp;
			else
				here->right = temp;
//printf("reached end_t, tip loc=%d\n", temp->trait);

			BirthDeath2Regions(root, here, here->trait, ++direction, parameters);
		}
	}
}



/******************************************************************************
 * When you are at node "here" and direction=2, you need decide whether "here"
 *    is a real node (with two descendents), and then back up to the 
 *    previous/ancestral node.
 *****************************************************************************/
void BackUp2Regions(TreeNode *root, TreeNode *here, TreeParams *parameters)
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
	BirthDeath2Regions(root, here, here->trait, from_right+1, parameters);
}


/******************************************************************************
 * Wait2RegionEvent allows dispersal and local extinctions to happen, while 
 *    waiting for global extinction or speciation to occur, or the end time 
 *    to be reached.
 * Its return value tells what event happened:
 *       0: global extinction (species is no longer in either region)
 *       1: speciation in region 1
 *       2: speciation in region 2
 *       3: reached end_t
 *****************************************************************************/
int Wait2RegionEvent(int *where, double now, double *wait_t, TreeParams *parameters)
{
	double which;
	double s1 = parameters->birth[0];
	double s2 = parameters->birth[1];
	double x1 = parameters->death[0];
	double x2 = parameters->death[1];
	double d1 = parameters->transition[0];
	double d2 = parameters->transition[1];
	double wait_rate[3] = {s1+s2+x1+x2, s1+x1+d1, s2+x2+d2};

	for (;;)
	{
		*wait_t += exponential_distribution(wait_rate[*where]);

		if (*wait_t + now > parameters->end_t)
			return 3;
		
		else if (*where == 0)			// if it's at both places
		{
			which = uniform_distribution(0, s1+s2+x1+x2);
			if (which < s1)			// speciation in region 1
				return 1;
			else if (which < s1+s2)		// speciation in region 2
				return 2;
			else if (which < s1+s2+x1)	// extinction in region 1...
				*where = 2;			// ...and continue waiting
			else 					// extinction in region 2...
				*where = 1;			// ...and continue waiting
		}

		else if (*where == 1)			// if it's only in region 1
		{
			which = uniform_distribution(0, x1+s1+d1);
			if (which < x1)			// extinction
				return 0;
			else if (which < x1+s1)		// speciation
				return 1;
			else *where = 0;			// continue waiting
		}

		else if (*where == 2)			// if it's only in region 2
		{
			which = uniform_distribution(0, x2+s2+d2);
			if (which < x2)			// extinction
				return 0;
			else if (which < x2+s2)		// speciation
				return 2;
			else *where = 0;			// continue waiting
		}
	}
}



/******************************************************************************
 * start with a single lineage (not a node)
 * see what happens to it, and if it speciates, build the tree
 *****************************************************************************/
void BuildTree2Regions(TreeNode *root, TreeParams *parameters)
{
	double wait_t = 0;
	int todo;
	int where = root->trait;
	TreeNode *temp;

	todo = Wait2RegionEvent(&where, root->time, &wait_t, parameters);

	/* if global extinction occurred, don't do anything */
	if (todo == 0)
	{}

	/* if speciation occurred along that branch, move the root there and continue */
	else if (todo == 1 || todo == 2)
	{
		root->time += wait_t;
		root->trait = todo;
		// note that "where" is the location of the parent lineage
		BirthDeath2Regions(root, root, where, 0, parameters);
	}

	/* if there was no birth or death before reaching the stopping time */
	else if (todo == 3)
	{
		temp = NewNode(root, parameters->end_t);
		temp->trait = where;
		node_counter++;

		root->left = temp;
	}
}
