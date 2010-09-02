/*****
 * don't use this file!
 * if I want to use one of these functions, put it in the appropriate other file
 ****/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "tree_funcs.h"


/****************** my functions *******************/


// useful when debugging
void PrintNode(TreeNode *here)
{
	int print_left, print_right, print_anc;

	if (here->left == NULL)		print_left = -1;
	else						print_left = 0;
	if (here->right == NULL)	print_right = -1;
	else						print_right = 0;
	if (here->anc == NULL)		print_anc = -1;
	else						print_anc = 0;
	printf("left = %d\tright = %d\tanc = %d\ttime = %2.2f\tindex = %d\n", print_left, print_right, print_anc, here->time, here->index);
}


void AssignBranchLengthsBackwards(TreeNode *p)
{
	if (p != NULL)
	{
		AssignBranchLengthsBackwards(p->left);
		AssignBranchLengthsBackwards(p->right);
		if (p->anc != NULL)
			p->length = -(p->time - p->anc->time);
		else
			p->length = 0;
	}
}


// this function added by JH 6/23/05
void AssignTransitionProbs(TreeNode *p, double alpha, double beta)
{
	double e, sum;
	if (p != NULL)
	{
		AssignTransitionProbs(p->left,  alpha, beta);
		AssignTransitionProbs(p->right, alpha, beta);
		if (p->anc != NULL)
		{
			sum = alpha + beta;
			e = exp(-(sum) * p->length);
			p->ti[0][0] = beta/sum  + (alpha/sum) * e;
			p->ti[0][1] = alpha/sum - (alpha/sum) * e;
			p->ti[1][0] = beta/sum  - (beta/sum)  * e;
			/* changed by EEG 2Aug06 
			p->ti[1][1] = alpha/sum + (alpha/sum) * e;	// this seems wrong
			*/
			p->ti[1][1] = alpha/sum + (beta/sum) * e;
		}
	}
}


// this function added by JH 6/23/05
void CalcPostProb(TreeNode *p, double alpha, double beta)
{
	int	i, j;
	double	sumL, sumR, sumA, freq[2];
	TreeNode *r;
	
	if (p != NULL)
	{
		CalcPostProb(p->left, alpha, beta);
		CalcPostProb(p->right, alpha, beta);
		if (p->left != NULL && p->right != NULL)
		{
			printf ("calculating posterior probability for node %d\n", p->index);
			UnMarkAllNodes(treeRoot);
			if (p->anc != NULL)
			{
				r = p->anc;
				r->marked = 1;
				while (r->anc != NULL)
				{
					r = r->anc;
					r->marked = 1;
				}
			}
			CondLikeDn(treeRoot);
			CondLikeUp(treeRoot, p);

			if (p->anc == NULL)
			{
				for (i=0; i<2; i++)
				{
					sumL = sumR = 0.0;
					for (j=0; j<2; j++)
					{
						sumL += p->left->ti[i][j] * p->left->cl[j];
						sumR += p->right->ti[i][j] * p->right->cl[j];
					}
					p->cl[i] = sumL * sumR;
				}
			}
			else
			{
				for (i=0; i<2; i++)
				{
					sumL = sumR = sumA = 0.0;
					for (j=0; j<2; j++)
					{
						sumL += p->left->ti[i][j] * p->left->cl[j];
						sumR += p->right->ti[i][j] * p->right->cl[j];
						sumA += p->ti[i][j] * p->anc->cl[j];
					}
					p->cl[i] = sumL * sumR * sumA;
				}
			}

			freq[0] = beta/(alpha+beta);
			freq[1] = alpha/(alpha+beta);
			p->pp[0] = p->cl[0] * freq[0] / (p->cl[0] * freq[0] + p->cl[1] * freq[1]);
			p->pp[1] = p->cl[1] * freq[1] / (p->cl[0] * freq[0] + p->cl[1] * freq[1]);
		}
	}
}


// this function added by JH 6/23/05
void CondLikeDn(TreeNode *p)
{

	int	i, j;
	double sumL, sumR;
	if (p != NULL)
	{
		CondLikeDn(p->left);
		CondLikeDn(p->right);
		if (p->left != NULL && p->right != NULL && p->marked == 0)
		{
			for (i=0; i<2; i++)
			{
				sumL = sumR = 0.0;
				for (j=0; j<2; j++)
				{
					sumL += p->left->ti[i][j]  * p->left->cl[j];
					sumR += p->right->ti[i][j] * p->right->cl[j];
				}
				p->cl[i] = sumL * sumR;
			}
		}
	}
}


// this function added by JH 6/23/05
void CondLikeUp(TreeNode *p, TreeNode *r)
{

	int	i, j;
	double sumU, sumD;
	TreeNode *u, *d;
	
	if (p != NULL)
	{
		if (p->left != NULL && p->right != NULL && p->marked == 1)
		{
			u = NULL;
			if ((p->left->marked == 1 || p->left == r) && p->right->marked == 0)
				u = p->right;
			else if (p->left->marked == 0 && (p->right->marked == 1 || p->right == r))
				u = p->left;
			else if (p->left->marked == 0 && p->right->marked == 0 && p->anc != NULL && p->left != r && p->right != r)
			{
				printf ("ERROR: Couldn't find unmarked node above node %d\n", p->index);
				exit(0);
			}
			if (u == NULL && p->anc == NULL)

			{
				printf ("ERROR: u is null\n");
				exit(0);
			}
			if (p->anc == NULL)
			{
				for (i=0; i<2; i++)
				{
					sumU = 0.0;
					for (j=0; j<2; j++)
						sumU += u->ti[i][j] * u->cl[j];
					p->cl[i] = sumU;
				}
			}
			else
			{
				d = p->anc;
				for (i=0; i<2; i++)
				{
					sumU = sumD = 0.0;
					for (j=0; j<2; j++)
					{
						sumU += u->ti[i][j] * u->cl[j];
						sumD += p->ti[i][j] * d->cl[j];
					}
					p->cl[i] = sumU * sumD;
				}
			}

		}
		CondLikeUp(p->left, r);
		CondLikeUp(p->right, r);
	}
}


// this function added by JH 6/23/05
void UnMarkAllNodes(TreeNode *p)
{
    if (p != NULL)
    {
        UnMarkAllNodes(p->left);
        UnMarkAllNodes(p->right);
        p->marked = 0;
    }
}


/* now in report.c
// count the number of tips
// call with CountTips(treeRoot, &num_tips);
void CountTips(TreeNode *p, int *counter)
{
	if (p->left == NULL && p->right == NULL)
		(*counter)++;
	else
	{
		CountTips(p->left, counter);
		CountTips(p->right, counter);
	}
}
*/

/* duh note: num_nodes = num_tips - 2 */

// count the number of interior nodes (excluding the root)
/* call with CountNodes(treeRoot, &num_nodes); */
void CountNodes(TreeNode *p, int *counter)
{
	if (p != NULL)
	{
		if (p->left != NULL && p->right != NULL && p->anc != NULL)
			(*counter)++;
		CountNodes(p->left, counter);
		CountNodes(p->right, counter);
	}
}


// count the numbers of tips with trait = 0 and 1
void CountTipTraits(TreeNode *p, int counter[2])
{
	if (p != NULL)
	{
		if (p->left == NULL && p->right == NULL)
			counter[p->trait]++;
		CountTipTraits(p->left, counter);
		CountTipTraits(p->right, counter);
	}
/* this also works and is a bit more efficient
	if (p->left == NULL && p->right == NULL)
		counter[p->trait]++;
	else
	{
		CountTipTraits(p->left, counter);
		CountTipTraits(p->right, counter);
	}
*/
}


// count the numbers of nodes with trait = 0 and 1 (excluding the root)
void CountNodeTraits(TreeNode *p, int counter[2])
{
	if (p != NULL)
	{
		if (p->left != NULL && p->right != NULL && p->anc != NULL)
			counter[p->trait]++;
		CountNodeTraits(p->left, counter);
		CountNodeTraits(p->right, counter);
	}
}


// compute the total (use to get average) branch lengths for trait = 0 and 1
void AddBranchLengths(TreeNode *p, double lengths[2][2])
{
	if (p != NULL)
	{
		if (p->anc != NULL)								// avoid the root
		{
			if (p->left == NULL && p->right == NULL)	// for tips
				lengths[p->trait][0] += p->length;
			else										// for interior nodes
				lengths[p->trait][1] += p->length;
		}
		AddBranchLengths(p->left, lengths);
		AddBranchLengths(p->right, lengths);
	}
}


/*-------| OutputTreeToFile |---------------------------------------------
|   Output tree information to a file.
(added by Boris ~1/13/06)
*/
void OutputTreeToFile(TreeNode *p, FILE *fp)
{
	if (p != NULL)
	{
		fprintf(fp, "%d: %3.2f, %3.2f, %d, %lf, %lf, %lf, %lf, %e, %e, %lf, %lf,\n", p->index, p->time, p->length, p->trait, p->ti[0][0], p->ti[0][1], p->ti[1][0], p->ti[1][1], p->cl[0], p->cl[1], p->pp[0], p->pp[1]);

		OutputTreeToFile(p->left, fp);
		OutputTreeToFile(p->right, fp);
	}
}



long int geometric_distribution(double p)
{
	return (long int) (exponential_distribution(-log(1-p)));
}

unsigned long int factorial(int n)
{
	int i;
	int ans = 1;

	if (n > 12)
	{
		fprintf(stderr, "ERROR: %d! is too large\n", n);
		exit(-1);
	}
			
	if (n == 0)
		return 1;
	else
	{
		for (i=n; i>0; i--)
			ans *= i;
		return ans;
	}
}



/***************** Huelsenbeck's functions ***********************/


void GetDownPassSequence(TreeNode *p, int *i)
{

	if (p != NULL)
	{
		GetDownPassSequence (p->left,  i);
		GetDownPassSequence (p->right, i);
		downPassSequence[(*i)++] = p;
	}

}


// make a pectinate tree
void SetUpTree(int numTaxa)
{
	int		i;
	TreeNode	*p, *q;

	if (numTaxa <= 2)
	{
		printf("Too few species\n");
		exit(-1);
	}

	for (i=0; i<2*numTaxa; i++)
	{
		nodes[i].anc   = NULL;
		nodes[i].left  = NULL;
		nodes[i].right = NULL;
	}

	p = nodes + 0;
	p->index = 0;
	treeRoot = p;
	q = nodes + 1;
	p->left = q;
	q->anc = p;
	p = q;

	q = nodes + 2;
	q->index = 2;
	p->left = q;
	q->anc = p;

	q = nodes + 3;
	q->index = 3;
	p->right = q;
	q->anc = p;
	p = p->left;

	for (i=4; i<2*numTaxa; i++)
	{
		if (p->left == NULL)
		{
			q = nodes + i;
			q->index = i;
			p->left = q;
			q->anc = p;
		}
		else
		{
			q = nodes + i;
			q->index = i;
			p->right = q;
			q->anc = p;
			p = p->left;
		}
	}
}


// print each node exactly once, starting in the upper left
void PassDownTree(TreeNode *p)
{
	if (p != NULL)
	{
		PassDownTree(p->left);
		PassDownTree(p->right);
		printf("pd %d\n", p->index);
	}
}


// print each node exactly once, starting at the root
void PassUpTree(TreeNode *p)
{
	if (p != NULL)
	{
		printf ("pu %d\n", p->index);
		PassUpTree (p->left);
		PassUpTree (p->right);
	}
}


/*-------| RandomNumber |------------------------------------------------
|   This pseudorandom number generator is described in:
|
|   Park, S. K. and K. W. Miller.  1988.  Random number generators: good
|      ones are hard to find.  Communications of the ACM, 31(10):1192-1201.
*/
double RandomNumber (long int *seed)
{
	long int	lo, hi, test;

	hi = (*seed) / 127773;
	lo = (*seed) % 127773;

	test = 16807 * lo - 2836 * hi;
	if (test > 0.0)
		*seed = test;
	else
		*seed = test + 2147483647;

	return (double)(*seed) / (double)2147483647;
}
