/******************************************************************************
* Copyright 2009 Emma Goldberg
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
	double birth[3];			// speciation rates
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
