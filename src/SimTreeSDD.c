/******************************************************************************
* Copyright 2010 Emma Goldberg
* 
* This file is part of SimTreeSDD.
* 
* SimTreeSDD is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* SimTreeSDD is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with SimTreeSDD.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "build_common.h"
#include "build_2states.h"
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
	int tip_counter[3];
	int countrun;

	
	srandom(time(0));


/******************************************************************************
 * 1. user input of parameter values
 ******************************************************************************/

	// check the program call
 	if (argc < 2)
 	{
		fprintf(stderr, "\n"
				"usage: specify file with parameter values, and optionally values on command line\n"
				"\t(e.g. %s sim-params.dat birth0=0.5)\n\n", argv[0]);
//				"\t(see params_SimTreeSDD.dat)\n\n");
 		return -1;
 	}
	// load specified file containing parameter values
	kv = loadKeyValue(argv[1]);

	// overwrite parameter values with those specified on the command line
	if (argc > 2) for(i = 2; i < argc; i++)
	{
		if(argv[i][0] == '=') fprintf(stderr,
			"Warning -- option begins with = (be sure to use option=value, without spaces)\n");
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
			BirthDeath2States(treeRoot, treeRoot, 0, parameters);
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
			fprintf(stderr, "run %d: more than %d nodes -- aborting this run and "
					"writing no output\n", run_counter+parameters->num_start, TOO_BIG);
			// run_counter++;
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
				// make an array containing the tip character states
				GetTipStates(treeRoot, tip_states);

				// check whether more than one character state is represented, if required
				countrun = 1;
				if (parameters->min_two_states == 1)
				{
					if (parameters->trait_type == 0)	// binary character
					{
						for (i=0; i<2; i++)
							tip_counter[i] = 0;
						CountTipStates(treeRoot, tip_counter);
						if (tip_counter[0]==0 || tip_counter[1]==0)
							countrun = 0;
					}
					else							// geographic character
					{
						for (i=0; i<3; i++)
							tip_counter[i] = 0;
						CountTipStates(treeRoot, tip_counter);
						if ( (tip_counter[0]==0 && tip_counter[1]==0) 
								|| (tip_counter[0]==0 && tip_counter[2]==0) 
								|| (tip_counter[1]==0 && tip_counter[2]==0) )
							countrun = 0;
					}
				}

				if (countrun > 0)
				{
					// show the tree on the screen
					ShowTree(treeRoot);

					// adjust file_prefix if there will be more than one tree
					if (parameters->num_trees > 1)
						sprintf(temp_prefix, "%s-%d", parameters->file_prefix, 
								run_counter+parameters->num_start);
					else
						strcpy(temp_prefix, parameters->file_prefix);
					
					// write the requested output files
					if (parameters->write_newick > 0)
						WriteNewickFile(treeRoot, temp_prefix);
					if (parameters->write_nexus > 0)
						WriteNexusFile(treeRoot, temp_prefix, n_tips, tip_states, parameters);
					if (parameters->write_bmstrait > 0)
						WriteBMSTraitFile(treeRoot, temp_prefix, n_tips, tip_states);
					if (parameters->write_ttn > 0)
						WriteTTNFile(treeRoot, temp_prefix);

					run_counter++;
				}
				else
				{
					ShowTreeDiscard(parameters, 1);
					ShowTree(treeRoot);
				}
			}
			else
			{
				ShowTreeDiscard(parameters, 0);
				// could also record in a file how many trees were discarded
			}
		}

		FreeTree(treeRoot);
	}

	FreeTreeParams(parameters);

	return 0;
}
