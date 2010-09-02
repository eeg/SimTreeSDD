#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplex.h"
#include "extra.h"
#include "input_like.h"
#include "label.h"
#include "likelihood.h"
#include "nodes.h"
#include "output.h"
#include "output_like.h"
#include "read_tree.h"
#include "report.h"
#include "reconstruct.h"

/* #defines from elsewhere:
 * LIKEPLOTSIZE     output_like.h
 * MAX_NODES        likelihood.h
 * MAX_PARAM        simplex.h
 * MAX_VERT         simplex.h
 */


int main(int argc, char *argv[])
{
 	struct KeyValue *kv;
	LikeParams *parameters;
	double (*likefunc)(TreeInfo *, double[]) = NULL;

	FILE *fp_like = 0;		// initialize to avoid compiler warnings
	FILE *fp_ml = 0;
	char message[1000];

	double rates[6];
	double loglike;
	double total_loglike[LIKEPLOTSIZE][LIKEPLOTSIZE];
	int i, j, t;

	TreeNode *treeRoot;
	TreeInfo *treeInfo;
	double times[MAX_NODES];

	// for DownhillSimplex
	int n_vert;						// n_param is treeInfo->n_param
	double simplex[MAX_VERT][MAX_PARAM];
	double y[MAX_VERT];
	int p, vertex, niter;


	// check the program call
	if (argc < 3)
	{
		fprintf(stderr, "\n"
				"usage: specify file with parameter values, and one or more tree .ttn files\n"
				"\t(see params_TreeLike.dat)\n\n");
 		return -1;
	}

	// load parameter file into parameter structure
	kv = loadKeyValue(argv[1]);
	parameters = NewLikeParams();
	AcquireLikeParams(kv, parameters);
	deleteKeyValue(kv);

	// set the verbosity
	if (parameters->verbosity <= 0)
		verbosity = 0;
	else if (parameters->verbosity >= 2)
		verbosity = 2;
	else
		verbosity = 1;

	treeInfo = NewTreeInfo();

	// choose the appropriate model
	if (parameters->like_func == 0)
	{
		likefunc = &GetPrunedNegLogL;
		treeInfo->n_param = 2;
		sprintf(message, "param1 = 0->1 transition rate, param2 = 1->0 transition rate");
		PrintMessage(message, 1);
	}
	else if (parameters->like_func == 1)
	{
		likefunc = &GetNeeNegLogL;
		treeInfo->n_param = 2;
		sprintf(message, "param1 = speciation rate, param2 = extinction rate");
		PrintMessage(message, 1);
	}
	// TODO: GetSDDLogL
	else
		return -1;
	n_vert = treeInfo->n_param + 1;

	// open some files if they will be used
	if (parameters->use_fixed_param > 0)
		fp_like = OpenFixedLikeFile(parameters);
	if (parameters->maximize > 0)
		fp_ml = OpenMLFile(parameters);

	// initialize the array that will hold the total likelihood surface
	for (i=0; i<LIKEPLOTSIZE; i++)
		for (j=0; j<LIKEPLOTSIZE; j++)
			total_loglike[i][j] = 0;

	// loop over all given trees
	for (t=2; t<argc; t++)
	{
		// make a labelled tree from the input file
		treeRoot = ReadTree(argv[t]);
		treeInfo->root = treeRoot;
		treeInfo->name = argv[t];
		treeInfo->n_tips = (intNodeNum+1)/2;

		if (parameters->like_func == 1)
		{
			// get the node times
			for (i=0; i<MAX_NODES; i++)
				times[i] = UNDEF;	// arbitrary large positive number
			GetNodeTimes(treeRoot, times);

			/* sort the node times: 
			 *   0 becomes the root, 
			 *   1 to (intNodeNum-3)/2 are internal nodes
			 *   (intNodeNum-1)/2 to intNodeNum-1 are the tips
			 */
			qsort(times, intNodeNum, sizeof(double), compare_doubles);

			// put stuff in the TreeInfo structure
			treeInfo->node_times = times;
			treeInfo->start_t = 0;
			treeInfo->end_t = times[intNodeNum-1];
		}

		/*** compute the likelihood for these particular rate values ***/
		if (parameters->use_fixed_param > 0)
		{
			loglike = -likefunc(treeInfo, parameters->param);
			ReportFixedLike(fp_like, loglike, treeInfo); 

			if (parameters->like_func == 0)
				if (parameters->reconstruct > 0)
					ReconstructAncestors(treeInfo, parameters->param, 1);
		}

		/*** compute likelihood values in a grid of parameter space ***/
		if (parameters->plot_like > 0)
		{
			// make a plot file for this tree, and add its contribution to total_loglike
			MakeLikePlot(treeInfo, total_loglike, likefunc);
		}

		/*** find maximum likelihood parameter estimates ***/
		if (parameters->maximize > 0)
		{
			// define the initial parameter values and simplex
			for (vertex = 0; vertex < n_vert; vertex++)
				for (p = 0; p < treeInfo->n_param; p++)
					simplex[vertex][p] = parameters->param[p];
			for (p = 0; p < treeInfo->n_param; p++)
				simplex[p+1][p] = simplex[p][p] + 1;

			// get the initial function values
			for (vertex = 0; vertex < n_vert; vertex++) 
			{
				for (p = 0; p < treeInfo->n_param; p++)
					rates[p] = simplex[vertex][p];
				y[vertex] = likefunc(treeInfo, rates);
			}

			// minimize the negative log-likelihood
			niter = DownhillSimplex(simplex, y, likefunc, treeInfo);

			// report the results
			for (p = 0; p < treeInfo->n_param; p++)
				rates[p] = simplex[0][p];
			loglike = -likefunc(treeInfo, rates);
	 		ReportML(fp_ml, loglike, treeInfo, niter, rates);

			// get the ancestral states
			if (parameters->like_func == 0)
				if (parameters->reconstruct > 0)
					ReconstructAncestors(treeInfo, rates, 0);
		}
	}

	if (parameters->plot_like > 0)
		WriteTotalLikeFile(total_loglike, parameters);

	if (fp_like)
		fclose(fp_like);
	if (fp_ml)
		fclose(fp_ml);

	FreeTreeInfo(treeInfo);
	FreeLikeParams(parameters);
		

	return 0;
}
