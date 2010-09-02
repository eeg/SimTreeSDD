/*****
 * don't use this file!
 * these are functions which were useful earlier but aren't used now
 * if I want to use one of these functions, put it in the appropriate other file
 ****/

// report
void CountTips(TreeNode *p, int *counter);
void CountNodes(TreeNode *p, int *counter);
void CountTipTraits(TreeNode *p, int counter[2]);
void CountNodeTraits(TreeNode *p, int counter[2]);
void AddBranchLengths(TreeNode *p, double lengths[2][2]);
void PassDownTree (TreeNode *p);
void PassUpTree (TreeNode *p);
void GetDownPassSequence (TreeNode *p, int *i);

// output
void PrintNode(TreeNode *here);
void OutputTreeToFile(TreeNode *p, FILE *fp);

// label
void AssignBranchLengthsBackwards(TreeNode *p);

// random
long int geometric_distribution(double p);
double   RandomNumber (long int *seed);

// build
void SetUpTree (int numTaxa);

// likelihood (these should probably be used in a program separate from the simulator)
void CondLikeDn(TreeNode *p);
void CondLikeUp(TreeNode *p, TreeNode *r);
void CalcPostProb(TreeNode *p, double alpha, double beta);
void UnMarkAllNodes(TreeNode *p);
/* notes from Huelsenbeck's use of these functions:
   	before building the tree:
		treeRoot->cl[0] = treeRoot->cl[1] = 0.0;
		treeRoot->cl[treeRoot->trait] = 1.0;
	after building the tree:
		AssignTransitionProbs(treeRoot, transition_values[0], transition_values[1]);
		CalcPostProb(treeRoot, transition_values[0], transition_values[1]);
*/



TreeNode *nodes, **downPassSequence;

unsigned long int factorial(int n);

void AssignTransitionProbs(TreeNode *p, double alpha, double beta);
