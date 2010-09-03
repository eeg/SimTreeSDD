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

/*******************************************************************************
 * these functions are for reporting stuff about a tree
 ******************************************************************************/
#ifndef __REPORT_H__
#define __REPORT_H__ 


#include "nodes.h"

void GetTipStates(TreeNode *p, int states[]);
void GetNodeTimes(TreeNode *p, double times[]);
void GetNodeDepth(TreeNode *p, int depths[]);
void GetBranchLengths(TreeNode *p, double lengths[]);
void CountTipStates(TreeNode *p, int tip_counter[]);
void CountTips(TreeNode *p, int *counter);


#endif
