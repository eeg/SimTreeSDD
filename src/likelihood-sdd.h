/*******************************************************************************
 * these functions are for calculating the likelihood of a tree under
 *    state-dependent diversification
 ******************************************************************************/
#ifndef __LIKELIHOODSDD_H__
#define __LIKELIHOODSDD_H__ 


#include "input_sim.h"
#include "nodes.h"

/*******************************************************************************
 * this structure allows SimTree1 to return just the relevant information
 *    rather than a whole tree
 ******************************************************************************/
typedef struct st1i
{
	int n_tips;
	int s2;
} SimTree1Info;

SimTree1Info *NewSimTree1Info();
void FreeSimTree1Info(SimTree1Info *p);

double GetSDDLogL(TreeInfo *tree, double rates[], int model);
void GetSurvProbs(TreeInfo *tree, TreeParams *parameters);
void GetOneTipProb(BranchNode *b, TreeParams *parameters, int s1, int nonextinct);
void GetSegProb(BranchNode *b, TreeParams *parameters, int s1);
//TreeNode *SimTree1(TreeParams *parameters, BranchNode *bnode, int start_trait, int check);
void SimTree1(TreeParams *parameters, BranchNode *bnode, int start_trait, int check, SimTree1Info *siminfo);
void BreakTree(TreeNode *p, BranchNode *branches[]);


#endif
