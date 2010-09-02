/*******************************************************************************
 * these functions write out results of the likelihood analysis
 ******************************************************************************/

#ifndef __OUTPUTLIKE_H__
#define __OUTPUTLIKE_H__ 


#include <string.h>
#include "input_like.h"

#define LIKEPLOTSIZE 100

FILE *OpenFixedLikeFile(LikeParams *parameters);
void ReportFixedLike(FILE *fp, double loglike, TreeInfo *tree);

FILE *OpenMLFile(LikeParams *parameters);
void ReportML(FILE *fp, double loglike, TreeInfo *tree, int niter, double rates[]);

void MakeLikePlot(TreeInfo *treeInfo, double total_loglike[][LIKEPLOTSIZE], double (*likefunc)(TreeInfo *, double []));
void WriteTotalLikeFile(double total_loglike[][LIKEPLOTSIZE], LikeParams *parameters);

void PrintBranches(BranchNode *branches[], int n_branches);

#endif
