#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "build.h"
#include "buildcheck.h"
#include "extra.h"
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
	BranchNode *branches[(int) TOO_BIG/2];		// will contain the base of each broken branch
	double rates[6];
	int n;
	double loglike;
	
	srandom(time(0));

	// check the program call
	if (argc < 2)
	{
		fprintf(stderr, "call with a .ttn file\n");
		return -1;
	}


 	/*** 1. define parameter values; TODO: get these from user input ***/

	// specify the rates (these will be loaded into a parameter structure later, but this form is good for calling the likelihood function if using amoeba); TODO: not using amoeba, so put these directly into parameters
	/*
	rates[0] = 0.20;	// birth[0]
	rates[1] = 0.15;	// birth[1]
	rates[2] = 0.08;	// death[0]
	rates[3] = 0.05;	// death[1]
	rates[4] = 0.07;	// transition[0] (alpha)
	rates[5] = 0.10;	// transition[1] (beta)
	*/
	rates[0] = 0.50;	// birth
	rates[1] = 0.30;	// death
	rates[2] = 0.00;	// transition


	/*** 2. read in, label, and break the tree ***/

	treeRoot = ReadTree(argv[1]);	// (this assigns intNodeNum)

	// fill in TreeInfo stuff
	treeInfo = NewTreeInfo();
	treeInfo->n_param = 6;
	treeInfo->root = treeRoot;
	treeInfo->name = argv[1];
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

	//PrintBranches(branches, treeInfo->n_tips);

	/*** 3. compute the likelihood ***/
	loglike = GetSDDLogL(treeInfo, rates);

	printf("logL = %f\n", loglike);


	// clean up dynamically allocated memory
	for (n=0; n<treeInfo->n_tips; n++)
		FreeBranch(branches[n]);
	FreeTreeInfo(treeInfo);
	FreeTree(treeRoot);

	return 0;
}
