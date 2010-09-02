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
void ShowTreeDiscard(TreeParams *parameters);

void WriteNewickFile(TreeNode *p, char *prefix);
void WriteNewickTree(TreeNode *p, FILE *fp);
void WriteNexusFile(TreeNode *p, char *prefix, int n_tips, int states[]);
void WriteNexusTree(TreeNode *p, FILE *fp);
void WriteBMSTraitFile(TreeNode *p, char *prefix, int n_tips, int states[]);
void WriteTTNFile(TreeNode *p, char *prefix);


#endif
