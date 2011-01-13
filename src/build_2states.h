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
 * for building a binary state tree
 ******************************************************************************/

#ifndef __BUILDSTATES_H__
#define __BUILDSTATES_H__ 


#include "input_sim.h"
#include "nodes.h"

#define TOO_BIG 50000			// abort if there are more nodes than this
extern int node_counter;			// defined in build_common.c

void BirthDeath2States(TreeNode *root, TreeNode *here, int direction, TreeParams *parameters);
double GetNextBD(TreeNode *p, TreeParams *parameters, double *t);
void BackUp2States(TreeNode *root, TreeNode *p, TreeParams *parameters);


#endif
