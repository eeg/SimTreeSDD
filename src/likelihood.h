/*******************************************************************************
 * these functions are for calculating the likelihood of a tree
 ******************************************************************************/
#ifndef __LIKELIHOOD_H__
#define __LIKELIHOOD_H__ 


#include "nodes.h"

#define MAX_NODES 10000

double GetPrunedNegLogL(TreeInfo *tree, double rates[]);
void PruningCL(TreeNode *p, double rates[]);

double GetCL0(TreeNode *p, double rates[]);
double GetCL1(TreeNode *p, double rates[]);
double TransitionProb(int from, int to, double time, double rates[]);

double GetNeeNegLogL(TreeInfo *tree, double rates[]);


#endif
