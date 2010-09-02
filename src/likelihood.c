#include <stdio.h>
#include <math.h>
#include "likelihood.h"
#include "nodes.h"


/*******************************************************************************
 * use the pruning algorithm and stationary frequencies to compute the 
 *   likelihood of a tree
 * this includes transition rates but assumes character don't affect speciation
 *   and extinction rates
 ******************************************************************************/
double GetPrunedNegLogL(TreeInfo *tree, double rates[])
{
	double like;

	// use the pruning algorithm to assign conditional likelihoods to each node
	PruningCL(tree->root, rates);

	// compute likelihood at root
	/*** FIXME: which of these methods should be used?
	 * this one seems like it should be right, weighting by the stationary distribution
	like = (rates[1]*(tree->root->cl[0]) + rates[0]*(tree->root->cl[1])) / (rates[0]+rates[1]);
	 * but this one is what agrees (perfectly) with Mesquite
	like = tree->root->cl[0] + tree->root->cl[1];
	 ***/
	like = (rates[1]*(tree->root->cl[0]) + rates[0]*(tree->root->cl[1])) / (rates[0]+rates[1]);

	return -log(like);
}

/*******************************************************************************
 * use the pruning algorithm to assign conditional likelihoods to each node
 * (used by GetPrunedNegLogLike)
 ******************************************************************************/
void PruningCL(TreeNode *p, double rates[])
{

	if (p != NULL)
	{
		PruningCL(p->left, rates);
		PruningCL(p->right, rates);

		if (p->left == NULL && p->right == NULL)	// for tips
		{
			p->cl[p->trait] = 1;
			// the other cl was already initialized to 0
		}
		else										// for interior nodes
		{
			p->cl[0] = GetCL0(p, rates);
			p->cl[1] = GetCL1(p, rates);
		}
	}
}


/*******************************************************************************
 * get the conditional likelihood for state 0
 ******************************************************************************/
double GetCL0(TreeNode *p, double rates[])
{
	return ( TransitionProb(0,0,p->left->length,rates)*p->left->cl[0] + TransitionProb(0,1,p->left->length,rates)*p->left->cl[1] ) * ( TransitionProb(0,0,p->right->length,rates)*p->right->cl[0] + TransitionProb(0,1,p->right->length,rates)*p->right->cl[1] );
}

/*******************************************************************************
 * get the conditional likelihood for state 1
 ******************************************************************************/
double GetCL1(TreeNode *p, double rates[])
{
	return ( TransitionProb(1,0,p->left->length,rates)*p->left->cl[0] + TransitionProb(1,1,p->left->length,rates)*p->left->cl[1] ) * ( TransitionProb(1,0,p->right->length,rates)*p->right->cl[0] + TransitionProb(1,1,p->right->length,rates)*p->right->cl[1] );
}

/*******************************************************************************
 * from transition rates, calculate transition probabilities
 ******************************************************************************/
double TransitionProb(int from, int to, double time, double rates[])
{
	// if ever using more than two character states, need to re-think this!
	return ( rates[(to+1)%2] + pow(-1, from-to)*(rates[from] * exp(-(rates[0]+rates[1])*time)) ) / (rates[0]+rates[1]);
}


/*******************************************************************************
 * use Nee et al.'s broken branch method to compute the likelihood of a tree
 * this includes speciation and extinction rates but ignores character states
 ******************************************************************************/
double GetNeeNegLogL(TreeInfo *tree, double rates[])
{
	double Ptimes[MAX_NODES];		// array to hold survival probabilities
	double Qtimes[MAX_NODES];		// array to hold survival probabilities
	double loglike;
	int i;

	/* compute survival probabilities using Kendall's formulas
	 *   Ptimes[i] = prob(lineage at t_i leaves >0 descendents at end_t)
	 *   Qtimes[i] = prob(lineage at t_i leaves exactly 1 descendents at end_t)
	 */
	if (rates[0] == rates[1])
	{
		for (i=0; i<tree->n_tips-1; i++)
		{
			Ptimes[i] = 1 / (1 + rates[0]*(tree->end_t-tree->node_times[i]));
			Qtimes[i] = Ptimes[i];
		}
	}
	else
	{
		for (i=0; i<tree->n_tips-1; i++)
		{
			Ptimes[i] = (rates[0]-rates[1]) / ( rates[0] - rates[1]*exp(-(rates[0]-rates[1])*(tree->end_t-tree->node_times[i])) );
			Qtimes[i] = 1 - ( rates[0] - rates[0]*exp(-(rates[0]-rates[1])*(tree->end_t-tree->node_times[i])) ) / ( rates[0] - rates[1]*exp(-(rates[0]-rates[1])*(tree->end_t-tree->node_times[i])) );
		}
	}

	/* sum up the log-likelihood */

	loglike = 0;
	for (i=1; i<tree->n_tips-1; i++)
	{
		// node contributions (excluding root)
		loglike += log( (i+1)*rates[0]*Ptimes[i] );

		// broken branch (one daughter inherits age) contributions, excluding root branches
		loglike += log(Qtimes[i]);
	}

	// contribution of the two root branches
	loglike += 2*log(Qtimes[0]);


	return -loglike;
}
