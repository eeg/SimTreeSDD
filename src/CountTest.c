#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "build.h"
#include "buildcheck.h"
#include "extra.h"
#include "input_sim.h"
#include "keyvalue.h"
#include "label.h"
#include "likelihood-sdd.h"
#include "nodes.h"
#include "output.h"
#include "randist.h"
#include "report.h"

TreeNode *TempSimTree1(TreeParams *parameters, BranchNode *bnode, int start_trait, int check, SimTree1Info *siminfo);
void TempCountTips(TreeNode *p, int *counter, double end_t);

int main(int argc, char *argv[])
{
	TreeParams *parameters;
	TreeNode *treeRoot;
	//int tips_so_far;
	SimTree1Info *siminfo;
	BranchNode *checknode;
	int i, nonextinct, anyrun;
	double onetip;

	double seed = time(0);
//	double seed = 1166631327;
	
	srandom(seed);
//	printf("seed = %f\n", seed);

	parameters = NewTreeParams();
	siminfo = NewSimTree1Info();
	checknode = NewBranchNode();

	parameters->birth[0] = 0.5;
	parameters->birth[1] = 0.5;
	parameters->death[0] = 0.3;
	parameters->death[1] = 0.3;
	parameters->transition[0] = 0;
	parameters->transition[1] = 0;
	parameters->end_t = 2.896;
	checknode->time = 1;

	nonextinct = 0;
	anyrun = 10000;
	onetip = 0;
	for (i=0; i<anyrun; i++)
	{
		treeRoot = TempSimTree1(parameters, checknode, 0, 0, siminfo);

//		ShowTreeIndent(treeRoot, 2);

/*
		ntips = 0;
		if (treeRoot->left!=NULL)
		{
			TempCountTips(treeRoot, &ntips, parameters->end_t);
			if (ntips == 1)
				onetip++;
			if (ntips > 0)
				nonextinct++;
		}
*/
		if (siminfo->n_tips == 1)
			onetip++;
		if (siminfo->n_tips > 0)
			nonextinct++;

		FreeTree(treeRoot);
	}

	printf("B|C = %f, B = %f\n", onetip/nonextinct, onetip/anyrun);

	FreeTreeParams(parameters);
	FreeSimTree1Info(siminfo);
	FreeBranchNode(checknode);

	return 0;
}

/*******************************************************************************
 * taken from likelihood-sdd.c/SimTree1
 ******************************************************************************/
TreeNode *TempSimTree1(TreeParams *parameters, BranchNode *bnode, int start_trait, int check, SimTree1Info *siminfo)
{
	TreeNode *simroot, *temp;
	int trait;
	double t;
	double lambda, mu, which;
	int tips_so_far, ntips;

	// create the tree root
	simroot = NewNode(NULL, 0);
	simroot->trait = start_trait;
	simroot->time = 0;

	// follow the lineage until it branches or ends
	trait = GetNextBDCheck(simroot, parameters, &t, bnode, 0);
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

//			BirthDeath(temp, temp, 0, parameters);
//			BirthDeathCount(temp, temp, 0, parameters, &tips_so_far);
			BirthDeathCheck(temp, temp, 0, parameters, bnode, &tips_so_far);

			siminfo->n_tips = tips_so_far;
			siminfo->s2 = (int)simroot->cl[0];
		}
		// if it was a death
		else
			siminfo->n_tips = 0;
	}

/*
	ntips = 0;
	if (simroot->left!=NULL)
	{
		TempCountTips(simroot, &ntips, parameters->end_t);
		siminfo->n_tips = ntips;
	}
	else
		siminfo->n_tips = 0;
	siminfo->s2 = (int)simroot->cl[0];
*/

	return(simroot);
}


/*******************************************************************************
 * taken a modified from report.c/CountTips
 ******************************************************************************/
void TempCountTips(TreeNode *p, int *counter, double end_t)
{
	if (p != NULL)
	{
		if (p->time == end_t)
			(*counter)++;
		else
		{
			TempCountTips(p->left, counter, end_t);
			TempCountTips(p->right, counter, end_t);
		}
	}
}
