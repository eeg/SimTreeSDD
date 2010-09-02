#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "build.h"
#include "buildcheck.h"
#include "extra.h"
#include "input_like.h"
#include "input_sim.h"
#include "likelihood-sdd.h"
#include "nodes.h"
#include "randist.h"
#include "report.h"


/*******************************************************************************
 * SimTree1Info functions
 ******************************************************************************/

SimTree1Info *NewSimTree1Info()
{
	SimTree1Info *p;
	p = (SimTree1Info *) malloc(sizeof(SimTree1Info));
	return p;
}

void FreeSimTree1Info(SimTree1Info *p)
{
	free(p);
}


/*******************************************************************************
 * put the rates in the right parts of the parameters structure
 ******************************************************************************/
void AssignMCMCModel(int model, double rates[], TreeParams *parameters)
{
	if (model == 0)		// bdt
	{
		parameters->birth[0] = rates[0];
		parameters->birth[1] = rates[0];
		parameters->death[0] = rates[1];
		parameters->death[1] = rates[1];
		parameters->transition[0] = rates[2];
		parameters->transition[1] = rates[2];
	}
	else if (model == 7)	// b2d2t2
	{
		parameters->birth[0] = rates[0];
		parameters->birth[1] = rates[1];
		parameters->death[0] = rates[2];
		parameters->death[1] = rates[3];
		parameters->transition[0] = rates[4];
		parameters->transition[1] = rates[5];
	}
	else if (model == 1)	// b2dt
	{
		parameters->birth[0] = rates[0];
		parameters->birth[1] = rates[1];
		parameters->death[0] = rates[2];
		parameters->death[1] = rates[2];
		parameters->transition[0] = rates[3];
		parameters->transition[1] = rates[3];
	}
	else if (model == 2)	// bd2t
	{
		parameters->birth[0] = rates[0];
		parameters->birth[1] = rates[0];
		parameters->death[0] = rates[1];
		parameters->death[1] = rates[2];
		parameters->transition[0] = rates[3];
		parameters->transition[1] = rates[3];
	}
	else if (model == 3)	// bdt2
	{
		parameters->birth[0] = rates[0];
		parameters->birth[1] = rates[0];
		parameters->death[0] = rates[1];
		parameters->death[1] = rates[1];
		parameters->transition[0] = rates[2];
		parameters->transition[1] = rates[3];
	}
	else if (model == 4)	// b2d2t
	{
		parameters->birth[0] = rates[0];
		parameters->birth[1] = rates[1];
		parameters->death[0] = rates[2];
		parameters->death[1] = rates[3];
		parameters->transition[0] = rates[4];
		parameters->transition[1] = rates[4];
	}
	else if (model == 4)	// b2dt2
	{
		parameters->birth[0] = rates[0];
		parameters->birth[1] = rates[1];
		parameters->death[0] = rates[2];
		parameters->death[1] = rates[2];
		parameters->transition[0] = rates[3];
		parameters->transition[1] = rates[4];
	}
	else if (model == 5)	// bd2t2
	{
		parameters->birth[0] = rates[0];
		parameters->birth[1] = rates[0];
		parameters->death[0] = rates[1];
		parameters->death[1] = rates[2];
		parameters->transition[0] = rates[3];
		parameters->transition[1] = rates[4];
	}
}


/*******************************************************************************
 * compute the likelihood of the tree
 ******************************************************************************/
double GetSDDLogL(TreeInfo *tree, double rates[], int model)
{
	TreeParams *parameters;
	BranchNode *b;
	int n;
	double loglike;

	parameters = NewTreeParams();

	// put the rates in the right parts of the parameters structure
	AssignMCMCModel(model, rates, parameters);

//printf("%f %f %f %f %f %f\n", rates[0], rates[1], rates[2], rates[3], rates[4], rates[5]);
//printf("%f %f %f %f %f %f\n", parameters->birth[0], parameters->birth[1], parameters->death[0], parameters->death[1], parameters->transition[0], parameters->transition[1]);

	/* run the simulator to get the segment survival probabilities (segprob[this][prev] in each BranchNode)
	 * (note: these only need to be recomputed when the rates change, not when only the node states change)
	 * TODO, maybe: if file is given with function call, use that instead */
	GetSurvProbs(tree, parameters);

	/*** compute the likelihood ***/

	loglike = 0;

	/* 1. for the non-root branches */

	for (n=2; n<tree->n_tips; n++)
	{
		// put b at the base node of this branch
		b = tree->branches[n];

		loglike += log( parameters->birth[b->trait] );				// L( A | b_{k_j} )
		loglike += log( b->segprob[b->trait][0] );					// L( B | v_{j,0} )

		b = b->next;			// move to the next node on the broken branch
		while (b != NULL)
		{
			loglike += log( b->segprob[b->trait][b->prev->trait] );	// L( v_{j,i} | v_{j,i-1}, B )
			b = b->next;
		}
		// NOTE: for two-region case, don't forget to include L( v_{j,0} | A, b_{k_j} ), which is just 1 here
	}


	/* 2. for one of the root branches */

	b = tree->branches[1];
	loglike += log( b->segprob[b->trait][0] );						// L( B | v_{1,0}, C )

	b = b->next;
	while (b != NULL)
	{
		loglike += log( b->segprob[b->trait][b->prev->trait] );		// L( v_{1,i} | v_{1,i-1}, B )
		b = b->next;
	}


	/* 3. for the other root branch */

	b = tree->branches[0];
	loglike += log( b->segprob[b->trait][0] );						// L( B | v_{0,0}, C )
	loglike += log( parameters->birth[b->trait] / (parameters->birth[0]+parameters->birth[1]) );	// L( v_{0,0} )

	b = b->next;
	while (b != NULL)
	{
		loglike += log( b->segprob[b->trait][b->prev->trait] );		// L( v_{0,i} | v_{0,i-1}, B )
		b = b->next;
	}

	FreeTreeParams(parameters);

	return loglike;
}


/*******************************************************************************
 * compute survival probabilities by simulation
 ******************************************************************************/
void GetSurvProbs(TreeInfo *tree, TreeParams *parameters)
{
 	int n, s1;
	BranchNode *b;

	for (n=0; n<tree->n_tips; n++)	// for each of the n_tips branches
	{
		// put b at the base node of this branch
		b = tree->branches[n];

		parameters->end_t = tree->end_t; // ? (was tree->end_t - b->time)

		/*** 1. get the one-tip probabilities (for the base nodes)
		 *      L( B | v_{j,0} ) for j > 1 OR L( B | v_{j,0}, C ) for j = 0, 1
		 *      store this in j -> i=0 -> segprob[state of this node, i=0][0]
		 ***/

		for (s1=0; s1<2; s1++)
		{
			if (n < 2)
				GetOneTipProb(b, parameters, s1, 1);	// condition the root branches on non-extinction
			else
				GetOneTipProb(b, parameters, s1, 0);	// don't condition the other branches on non-extinction
			// b->segprob[s1][0] is now assigned
//printf("n = %d: s1 = %d, T = %f, one tip prob = %f\n", n, s1, parameters->end_t - b->time, b->segprob[s1][0]);
		}

		/*** 2. get the segment probabilities (for all the other nodes/tips)
		 *      L( v_{j,i} | v_{j,i-1}, B ) for all branches j and for nodes i > 0 on each branch
		 *      store this in j -> i -> segprob[state of this node, i][state of prev node, i-1]
		 ***/
		b = b->next;			// move to the next node on the broken branch
		while (b != NULL)
		{
			for (s1=0; s1<2; s1++)	// loop over the character states of the earlier node
			{
				GetSegProb(b, parameters, s1);
//printf("n = %d: s1 = %d, t = %f, segprob(s2=0) = %f, segprob(s2=1) = %f\n", n, s1, b->time, b->segprob[s1][0], b->segprob[s1][1]);
			}
			b = b->next;
		}

		// TODO: maybe write output to a file
	}
}


/*******************************************************************************
 * get the probability of leaving exactly one tip, possibly conditioning on 
 *   non-extinction
 ******************************************************************************/
void GetOneTipProb(BranchNode *b, TreeParams *parameters, int s1, int nonextinct)
{
	SimTree1Info *siminfo;
	int n;
	int target_n;
	double p, cv;
	double cv_tol = 0.05;

	double onetip_counter = 0;
	double nonextinct_counter = 0;
	double anyrun_counter = 0;

	int max_counter;
	int max_runs = 1000000;

	siminfo = NewSimTree1Info();

	/* simulate the first block of trees */
	max_counter = 0;
	for (n=0; n<1000; )
	{
		SimTree1(parameters, b, s1, 0, siminfo);

		if (siminfo->n_tips > 0)
		{
			nonextinct_counter++;
			n++;
		}
		if (siminfo->n_tips == 1)
			onetip_counter++;
		anyrun_counter++;

		max_counter++;
		if (max_counter > max_runs)
		{
//			printf("exceeded max_runs in GetOneTipProb\n"); // TODO: remove this
			break;
		}
	}

	/* compute the probability and coefficient of variation */

	if (nonextinct == 0)	// don't condition on non-extinction
		p = onetip_counter/anyrun_counter;
	else					// condition on non-extinction
		p = onetip_counter/nonextinct_counter;
	cv = sqrt( (1-p)/(n*p) );

	// if the accuracy is good enough, stop
	if (cv < cv_tol)
	{
		b->segprob[s1][0] = p;
	}

	/* otherwise, simulate more trees */
	else
	{
		if (p > 0)
			target_n = (1-p) / (cv_tol*cv_tol*p);
		else
			target_n = 10000;

		max_counter = 0;
		for ( ; n<target_n; )	// continue counting from the n used so far 
		{
			SimTree1(parameters, b, s1, 0, siminfo);

			if (siminfo->n_tips > 0)
			{
				nonextinct_counter++;
				n++;
			}
			if (siminfo->n_tips == 1)
				onetip_counter++;
			anyrun_counter++;

			max_counter++;
			if (max_counter > max_runs)
			{
//				printf("exceeded max_runs in GetOneTipProb\n"); // TODO: remove this
				break;
			}
		}
		
		// turn the counter into a probability and assign it
		if (onetip_counter > 0)
		{
			if (nonextinct == 0)
				b->segprob[s1][0] = onetip_counter/anyrun_counter;
			else
				b->segprob[s1][0] = onetip_counter/nonextinct_counter;
		}
		else		// to avoid taking log(0)
			b->segprob[s1][0] = TINY;

		p = b->segprob[s1][0];
		cv = sqrt( (1-p)/(n*p) );
	}

	FreeSimTree1Info(siminfo);
}


/*******************************************************************************
 * get the probability that this node is in its state (s2), given that the 
 *   previous node was in its state (s1) and that there is exactly one tip
 ******************************************************************************/
void GetSegProb(BranchNode *b, TreeParams *parameters, int s1)
{
	SimTree1Info *siminfo;
	int n, s2;
	int target_n;
	double p, cv;
	double cv_tol = 0.05;

	double success_counter[2] = {0,0};
	double run_counter = 0;

	int max_counter;
	int max_runs = 1000000;

	siminfo = NewSimTree1Info();

	// simulate the first block of trees 
	max_counter = 0;
	for (n=0; n<1000; )
	{
		SimTree1(parameters, b->prev, s1, 1, siminfo);
		if (siminfo->n_tips > 0)
		{
			n++;
		}
		if (siminfo->n_tips == 1)
		{
			run_counter++;
			success_counter[siminfo->s2]++;
		}

		max_counter++;
		if (max_counter > max_runs)
		{
//			printf("exceeded max_runs in GetSegProb\n"); // TODO: remove this
			break;
		}
	}

	// compute the (smaller) probability and (larger) coefficient of variation
	for (s2=0; s2<2; s2++)
	{
		p = success_counter[s2]/run_counter;
		if (p > 0.5)
			p = 1-p;
		cv = sqrt( (1-p)/(n*p) );
	}

	// if the accuracy is good enough, stop
	if (cv < cv_tol)
	{
		for (s2=0; s2<2; s2++)
			b->segprob[s1][s2] = success_counter[s2]/run_counter;
	}

	// otherwise, simulate more trees
	else
	{
		if (p > 0)
			target_n = (1-p) / (cv_tol*cv_tol*p);
		else
			target_n = 10000;

		max_counter = 0;
		for ( ; n<target_n; )	// continue counting from the n used so far 
		{
			SimTree1(parameters, b->prev, s1, 1, siminfo);
			if (siminfo->n_tips > 0)
			{
				n++;
			}
			if (siminfo->n_tips == 1)
			{
				success_counter[siminfo->s2]++;
				run_counter++;
			}

			max_counter++;
			if (max_counter > max_runs)
			{
//				printf("exceeded max_runs in GetSegProb\n"); // TODO: remove this
				break;
			}
		}

		// turn the counters into probabilities and assign them
		for (s2=0; s2<2; s2++)
		{
			if (success_counter[s2] > 0)
				b->segprob[s1][s2] = success_counter[s2]/run_counter;
			else		// to avoid taking log(0)
				b->segprob[s1][s2] = TINY;
		}
	}

	FreeSimTree1Info(siminfo);

	// TODO: maybe write output to a file
}


/*******************************************************************************
 * simulate a tree (using BirthDeath), but start with a single lineage rather
 *   than a node (=> two lineages)
 * optionally, call a version of BirthDeath that will record the state of the
 *   tree it's building at a particular time
 ******************************************************************************/
void SimTree1(TreeParams *parameters, BranchNode *bnode, int start_trait, int check, SimTree1Info *siminfo)
{
	TreeNode *simroot, *temp;
	int trait;
	double t;
	double lambda, mu, which;
	int tips_so_far;

	// create the tree root
	simroot = NewNode(NULL, 0);
	simroot->trait = start_trait;
	simroot->time = bnode->time;

	// follow the lineage until it branches or ends
	trait = GetNextBDCheck(simroot, parameters, &t, bnode->next, 0);
	t += simroot->time;

	// if there was no birth or death before the end time, stop
	if (t >= parameters->end_t)
	{
		temp = NewNode(simroot, parameters->end_t);
		temp->trait = trait;
		simroot->left = temp;
		simroot->cl[0] = trait;

		siminfo->n_tips = 1;
		siminfo->s2 = (int)simroot->cl[0];
	}

	// if there was a birth or death, find out which
	else
	{
		lambda = parameters->birth[trait];
		mu = parameters->death[trait];
		which = uniform_distribution(0, lambda+mu);

		// if it was a birth, don't move the root, but make a node and build the tree
		if (which < lambda)
		{
			temp = NewNode(simroot, t);
			temp->trait = trait;
			temp->cl[0] = UNDEF;
			temp->cl[1] = UNDEF;
			simroot->left = temp;
			simroot->cl[1] = UNDEF;

			node_counter = 1;
			tips_so_far = 0;

			if (check == 0)
				BirthDeathCount(temp, temp, 0, parameters, &tips_so_far);
			else
				BirthDeathCheck(temp, temp, 0, parameters, bnode->next, &tips_so_far);

			siminfo->n_tips = tips_so_far;
			if (siminfo->n_tips == 1)
			{
				// walk the "tree" to find s2
				temp = simroot;
				do
				{
					if (temp->left != NULL)
					{
						temp = temp->left;
						which = 0;
					}
					else if (temp->right != NULL)
					{
						temp = temp->right;
						which = 1;
					}
					else
					{
						// temp is a tip, so bnode->next = checknode must also be a tip
						break;
					}
				} while (temp->time < bnode->next->time);
				siminfo->s2 = (int)temp->anc->cl[(int)which];
			}

		}
		// if it was a death
		else
			siminfo->n_tips = 0;
	}

	FreeTree(simroot);
}


/*******************************************************************************
 * break a tree into branches, and put the base of each branch into an array
 ******************************************************************************/
void BreakTree(TreeNode *p, BranchNode *branches[])
{
	static int mark = 0;
	static int n = 0;
	static BranchNode *current;

	// keep going if you're not yet at a tip
	if (p->left != NULL || p->right != NULL)
	{
		// if you need the base of a new branch, take it here
		if (mark == 0)
		{
			branches[n] = NewBranchNode();
			current = branches[n];
			current->time = p->time;
			current->trait = p->trait;
			current->prev = NULL;
			mark = 1;
		}
		// otherwise, add this node to the current branch
		else
		{
			current->next = NewBranchNode();
			current->next->prev = current;
			current = current->next;
			current->time = p->time;
			current->trait = p->trait;
		}

		// then, move up the tree
		BreakTree(p->left, branches);

		// if you need the base of a new branch, take it here
		if (mark == 0)
		{
			branches[n] = NewBranchNode();
			current = branches[n];
			current->time = p->time;
			current->trait = p->trait;
			current->prev = NULL;
			mark = 1;
		}
		// otherwise, add this node to the current branch
		else
		{
			current->next = NewBranchNode();
			current->next->prev = current;
			current = current->next;
			current->time = p->time;
			current->trait = p->trait;
		}

		// then, move up the tree
		BreakTree(p->right, branches);
	}

	// if you're at a tip, that is the end of the branch
	else
	{
		current->next = NewBranchNode();
		current->next->prev = current;
		current = current->next;
		current->time = p->time;
		current->trait = p->trait;
		current->next = NULL;
		n++;
		mark = 0;
	}
}
