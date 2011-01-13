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
 * these functions label things on an existing tree
 *   branch lengths
 *   tip/node times
 *   character states
 ******************************************************************************/

#ifndef __LABEL_H__
#define __LABEL_H__ 


#include "nodes.h"

extern int intNodeNum;

void AssignBranchLengths(TreeNode *p);
void AssignNodeTimes(TreeNode *p);
void LabelInteriorNodes(TreeNode *p);
void LabelTips(TreeNode *p);


#endif
