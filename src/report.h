/*******************************************************************************
 * these functions are for reporting stuff about a tree
 ******************************************************************************/
#ifndef __REPORT_H__
#define __REPORT_H__ 


#include "nodes.h"

void GetTipStates(TreeNode *p, int states[]);
void GetNodeTimes(TreeNode *p, double times[]);
void CountTipStates(TreeNode *p, int tip_counter[]);
void CountTips(TreeNode *p, int *counter);


#endif
