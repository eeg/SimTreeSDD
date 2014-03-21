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
 * these functions write out results of the simulations
 *   status information as simulations are proceeding
 *   files containing results, for use with analysis programs
 * some functions might potentially be used in likelihood analysis reports...
 ******************************************************************************/

#ifndef __OUTPUT_H__
#define __OUTPUT_H__ 


#include <string.h>
#include "input_sim.h"
#include "nodes.h"

extern int verbosity;

void PrintMessage(char *message, int verb_level);

void ShowTree(TreeNode *p);
void ShowTreeParentheses(TreeNode *p);
void ShowTreeIndent(TreeNode *p, int indent);
void ShowParamValues(TreeParams *parameters);
void ShowRootState(int report);
void ShowRootMove(int report);
void ShowTreeDiscard(TreeParams *parameters, int reason);

void WriteNewickFile(TreeNode *p, char *prefix);
void WriteNewickTree(TreeNode *p, FILE *fp);
void WriteNexusFile(TreeNode *p, char *prefix, int n_tips, int states[], TreeParams *parameters);
void WriteNexusTree(TreeNode *p, FILE *fp);
void WriteBMSTraitFile(TreeNode *p, char *prefix, int n_tips, int states[]);
void WriteTTNFile(TreeNode *p, char *prefix);
void WriteAgeFile(TreeNode *p, char *prefix);


#endif
