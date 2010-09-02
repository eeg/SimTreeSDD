/*******************************************************************************
 * these functions allow information to be given to the simulator
 *   parameter values
 *   file names and locations
 *   verbosity setting
 ******************************************************************************/

#ifndef __INPUT_H__
#define __INPUT_H__ 

#include "keyvalue.h"

typedef struct
{
	double birth[2];			// speciation rates
	double death[2];			// extinction rates
	double transition[2];		// transition rates {0->1, 1->0}
	double end_t;				// time for which to run the tree
	int trait_type;			// binary character (0) or region (1)
	char file_prefix[128];		// prefix to give output files
	int verbosity;				// how much stuff to print to screen (0, 1, 2)
	int root_state;			// character state of the root species
	int min_tips;				// number of tips a tree must have to be counted
	int min_two_states;			// whether to discard a tree with only one state at tips
	int num_trees;				// number of trees to simulate
	int num_start;				// number at which to start labeling output files
	int write_newick;			// output files to create
	int write_nexus;
	int write_bmstrait;
	int write_ttn;
} TreeParams;

TreeParams *NewTreeParams();
void FreeTreeParams(TreeParams *params);
int AcquireTreeParams(struct KeyValue *kv, TreeParams *parameters);


#endif
