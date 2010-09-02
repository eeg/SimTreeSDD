/*******************************************************************************
 * MCMC for state-dependent diversification
 * (up to 6 rates are allowed)
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "build.h"
#include "buildcheck.h"
#include "extra.h"
#include "input_like.h"
#include "input_mcmc.h"
#include "input_sim.h"
#include "keyvalue.h"
#include "label.h"
#include "likelihood-sdd.h"
#include "nodes.h"
#include "output.h"
#include "output_like.h"
#include "randist.h"
#include "read_tree.h"
#include "report.h"

int main(int argc, char *argv[])
{
	TreeNode *treeRoot, *p;
	TreeInfo *treeInfo;
	TreeParams *parameters;
	BranchNode *branches[(int) TOO_BIG/2];
 	struct KeyValue *kv;

	double rates[6], rates_new[6];
	double eta[6];
	double tune[6];
	double acceptances[6];
	int n, r, i, n_iter, model;

	double prior_ratio, likelihood_ratio, proposal_ratio, accept_ratio;
	double loglike, loglike_new;

	FILE *fp;
	char filename[100];


	srandom(time(0));

	// check the program call
	if (argc < 3)
	{
		fprintf(stderr, "\nERROR: need to specify a parameter file and a tree .ttn file(s)\n\n");
 		return -1;
	}

	treeInfo = NewTreeInfo();
	parameters = NewTreeParams();

	// get input parameters
	kv = loadKeyValue(argv[1]);
	n = AcquireMCMCParams(kv, &model, &n_iter, eta, tune);
	deleteKeyValue(kv);
	if (n < 0)
	{
		return -1;
	}
	// TODO: verbosity setting
	// TODO: option to continue from previous .mcmc file

	// get the number of parameters
	if (model==0)
		treeInfo->n_param = 3;
	else if (model==7)
		treeInfo->n_param = 6;
	else if (model==1 || model==2 || model==3)
		treeInfo->n_param = 4;
	else if (model==4 || model==5 || model==6)
		treeInfo->n_param = 5;

	// open the output file
	ChangeFileSuffix(argv[2], ".ttn", ".mcmc", filename);
	fp = fopen(filename, "w");

	// read in the tree
	treeRoot = ReadTree(argv[2]);	// (this assigns intNodeNum)

	// fill in TreeInfo stuff
	treeInfo->root = treeRoot;
	treeInfo->name = argv[2];
	treeInfo->n_tips = (intNodeNum+1)/2;
	treeInfo->branches = branches;
	treeInfo->start_t = 0;
	p = treeRoot;
	while (p->left != NULL)
		p = p->left;
	treeInfo->end_t = p->time;

	// break the tree into branches, and put the base of each into the branches[] array
	BreakTree(treeRoot, branches);
	
	// sort the array of branch bases, to assure that the two root branches are first
	qsort(branches, treeInfo->n_tips, sizeof(BranchNode *), compare_branchtimes);

	/* now start the MCMC */

	// propose initial values for the rates, drawn from an exponential prior
/*
	rates[0] = 0.50;	// (cheat for now)
	rates[1] = 0.51;
	rates[2] = 0.30;
	rates[3] = 0.31;
	rates[4] = 0.20;
	rates[5] = 0.21;
*/
	for (r=0; r<treeInfo->n_param; r++)
	{
		rates[r] = exponential_distribution(eta[r]);
		rates_new[r] = rates[r];
		acceptances[r] = 0;
	}

	loglike = GetSDDLogL(treeInfo, rates, model);

	for (n=0; n<n_iter; n++)	// run the MCMC for n_iter iterations
	{
		for (r=0; r<treeInfo->n_param; r++)	// loop over the rates
		{
			// propose a new value for the rate
			rates_new[r] = rates[r] * exp( tune[r]*(uniform_distribution(0,1)-0.5) );

			// TODO: try out a lot of interior node states
			/* do this n_tips times:
			 *    pick a node number at random
			 *    go to that node number
			 *    swap the state at that node
			 *    compute likelihood for that proposed change
			 *    accept or reject the change
			 */

			// get the ratios
			prior_ratio = exp(-rates_new[r]*eta[r]) / exp(-rates[r]*eta[r]);
			proposal_ratio = rates_new[r] / rates[r];
			loglike_new = GetSDDLogL(treeInfo, rates_new, model);
			likelihood_ratio = exp(loglike_new - loglike);

			// determine whether or not to accept the new rate
			accept_ratio = prior_ratio * likelihood_ratio * proposal_ratio;
			if ( (accept_ratio > 1) || (uniform_distribution(0,1) < accept_ratio) )
			{
				acceptances[r]++;
				rates[r] = rates_new[r];
				loglike = loglike_new;
			}
			else
			{
				rates_new[r] = rates[r];
			}

			// print the results
			printf("%d  ", n);
			for (i=0; i<treeInfo->n_param; i++)
				printf("%f  ", rates[i]);
			printf("%f\n", loglike);
		}

		// record the results
		for (i=0; i<treeInfo->n_param; i++)
			fprintf(fp, "%f\t", rates[i]);
		fprintf(fp, "%f\n", loglike);
	}

	fprintf(fp, "#acceptances: ");
	for (i=0; i<treeInfo->n_param; i++)
		fprintf(fp, "%f  ", acceptances[i]/n_iter);
	fprintf(fp, "\n");

	printf("acceptances: ");
	for (i=0; i<treeInfo->n_param; i++)
		printf("%f  ", acceptances[i]/n_iter);
	printf("\n");

	fclose(fp);
	FreeTreeInfo(treeInfo);
	FreeTreeParams(parameters);

	return 0;
}
/*
	propose initial values for the six rates (drawn from prior?); "old" values
	propose initial values for internal node states (how? prior = 50/50?); "old" values

	get tree likelihood; "old" value
	start the MCMC
		loop over rates (fixed number of iterations)
			propose a new value for this rate (e.g. r-new = r-old * exp[tune*(uniform-0.5)])
			get prior ratio
			get likelihood ratio (need to simulate)
			get Hastings ratio (uniform? new/old?)
			get acceptance ratio = prior ratio * likelihood ratio * Hastings ratio
			accept or reject: adjust "old" likelihood and rate values
			write results (iteration number, likelihood, rate values) to a file

			loop over all interior nodes (maybe more than once?)
				propose to change the node state
				get prior ratio ( = 1 for 50/50)
				get likelihood ratio (get "new" likelihood from "old" by just accounting for changed node)
				get Hastings ratio (how?)
				get acceptance ratio = prior ratio * likelihood ratio * Hastings ratio
				accept or reject: adjust "old" likelihood and node values
*/
