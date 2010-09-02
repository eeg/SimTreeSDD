#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "build.h"
#include "build_2regions.h"
#include "input_sim.h"
#include "label.h"
#include "nodes.h"
#include "output.h"
#include "randist.h"
#include "report.h"


int main(int argc, char *argv[])
{
 	struct KeyValue *kv;
	TreeParams *parameters;
	char k[100], v[100];

	int report;

	TreeNode *treeRoot;
	char temp_prefix[100];

 	int i, j, run_counter;
	int n_tips;
	int tip_states[(int) TOO_BIG/2];

	
	srandom(time(0));


/*******************************************************************************
 * 1. user input of parameter values
 ******************************************************************************/

	// check the program call
 	if (argc < 2)
 	{
		fprintf(stderr, "\n"
				"usage: specify file with parameter values, and optionally values on command line\n"
				"\t(see params_SimTreeSDD.dat)\n\n");
 		return -1;
 	}

	// load specified file containing parameter values
	kv = loadKeyValue(argv[1]);

	// overwrite parameter values with those specified on the command line
	if (argc > 2) for(i = 2; i < argc; i++)
	{
		if(argv[i][0] == '=') fprintf(stderr,
			"Warning -- option begins with =\n");
		for(j = 0; argv[i][j] != 0; j++) if(argv[i][j] == '=')
			argv[i][j] = ' ';
		if(sscanf(argv[i], "%s %s", k, v) != 2) continue;
		
		j = KeyValuekeyindex(kv, k);
		if (j < 0)
			KeyValueaddparm(kv, k, v);
		else
		{
		 	free(kv->value[j]);
		 	kv->value[j] = strdup(v);
		}
	}

	// load the values into a parameter structure
	parameters = NewTreeParams();
 	if (AcquireTreeParams(kv, parameters) == -1) 
	{
		fprintf(stderr, "unable to proceed -- not all required parameters specified\n");
		return -1;
	}

	// set the verbosity
	if (parameters->verbosity <= 0)
		verbosity = 0;
	else if (parameters->verbosity >= 2)
		verbosity = 2;
	else
		verbosity = 1;

	// report parameter values to be used, so the user can check
	ShowParamValues(parameters);


/*******************************************************************************
 * 2. making the tree
 ******************************************************************************/
	for (run_counter=0; run_counter < parameters->num_trees; )
	{
		// create the tree root
		treeRoot = NewNode(NULL, 0);
		treeRoot->index = 0;

		// assign character state to tree root
		report = AssignRootState(treeRoot, parameters);
		ShowRootState(report);

		// build the tree through a birth-death process
		node_counter = 1;		// the root already exists	

		if (parameters->trait_type == 0)
			/***
			 * starting with a single node, build the tree 
			 *    through a birth-death-transition process
			 ***/
			BirthDeath(treeRoot, treeRoot, 0, parameters);
		// TODO: option for starting with a single lineage for binary character?
		else
			/***
			 * starting with a single lineage (not a node), build the tree 
			 *    through a birth-death-dispersal process
			 ***/
			BuildTree2Regions(treeRoot, parameters);
		// TODO: option for starting with a node for two regions?

		if (node_counter > TOO_BIG)
		{
			fprintf(stderr, "run %d: more than %d nodes -- aborting this run and writing no output\n", run_counter+parameters->num_start, TOO_BIG);
			run_counter++;
		}

		else
		{
			// move the root if only one branch comes from there
			treeRoot = MoveRoot(treeRoot);

			// label the tree
			intNodeNum = 0;
			LabelTips(treeRoot);
			LabelInteriorNodes(treeRoot);
			AssignBranchLengths(treeRoot);


/*******************************************************************************
 * 3. reporting the results
 ******************************************************************************/

			// get the number of tips
			if (intNodeNum < 3)
				n_tips = intNodeNum - 1;
			else
				n_tips = (intNodeNum+1)/2;

			// only record the results if there are at least min_tips tips
			if (n_tips >= parameters->min_tips)
			{
				// show the tree on the screen
				ShowTree(treeRoot);

				// make an array containing the tip character states
				GetTipStates(treeRoot, tip_states);

				// adjust file_prefix if there will be more than one tree
				if (parameters->num_trees > 1)
					sprintf(temp_prefix, "%s-%d", parameters->file_prefix, run_counter+parameters->num_start);
				else
					strcpy(temp_prefix, parameters->file_prefix);
				
				// write the requested output files
				if (parameters->write_newick > 0)
					WriteNewickFile(treeRoot, temp_prefix);
				if (parameters->write_nexus > 0)
					WriteNexusFile(treeRoot, temp_prefix, n_tips, tip_states);
				if (parameters->write_bmstrait > 0)
					WriteBMSTraitFile(treeRoot, temp_prefix, n_tips, tip_states);
				if (parameters->write_ttn > 0)
					WriteTTNFile(treeRoot, temp_prefix);

				run_counter++;
			}
			else
			{
				ShowTreeDiscard(parameters);
				// could also record in a file how many trees were discarded
			}
		}

		FreeTree(treeRoot);
	}

	FreeTreeParams(parameters);

	return 0;
}
