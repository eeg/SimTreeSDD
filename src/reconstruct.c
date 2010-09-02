#include <stdio.h>
#include <math.h>
#include "extra.h"
#include "likelihood.h"
#include "nodes.h"
#include "output.h"
#include "reconstruct.h"

void ReconstructAncestors(TreeInfo *tree, double rates[], int fixed)
{
	char filename[100];
	FILE *fp;
	char message[1000];
	double like[2];

	if (fixed == 1)
		ChangeFileSuffix(tree->name, ".ttn", "-fixed.anc", filename);
	else
		ChangeFileSuffix(tree->name, ".ttn", ".anc", filename);
	fp = fopen(filename, "w");

	fprintf(fp, "# rates used are %e and %e\n", rates[0], rates[1]);
	fprintf(fp, "# node index | node time | true trait | prob(0) | prob(1)\n");

	// make sure the tree has its normal conditional likelihoods assigned
	GetPrunedNegLogL(tree, rates);

	// get and write the results for all nodes but the root
	GetAncStates(tree->root, rates, fp);
	
	// take care of the root
	tree->root->cl[0] = GetCL0(tree->root, rates);
	tree->root->cl[1] = GetCL1(tree->root, rates);

	/*** FIXME: which of these methods should be used?
	 * this one seems like it should be right, weighting by the stationary distribution
	like[0] = rates[1]*(tree->root->cl[0]) / (rates[0]+rates[1]);
	like[1] = rates[0]*(tree->root->cl[1]) / (rates[0]+rates[1]);
	 * but this one is what agrees (perfectly) with Mesquite
	like[0] = tree->root->cl[0];
	like[1] = tree->root->cl[1];
	 ***/
	like[0] = rates[1]*(tree->root->cl[0]) / (rates[0]+rates[1]);
	like[1] = rates[0]*(tree->root->cl[1]) / (rates[0]+rates[1]);

	fprintf(fp, "%d\t%f\t%d\t%f\t%f\n", tree->root->index, tree->root->time, tree->root->trait, like[0]/(like[0]+like[1]), like[1]/(like[0]+like[1]) );

	fclose(fp);

	sprintf(message, "created file %s", filename);
	PrintMessage(message, 1);
}

void GetAncStates(TreeNode *p, double rates[], FILE *fp)
{
	TreeNode *q;
	int i;
	double like[2];

	if (p != NULL)
	{
		GetAncStates(p->left, rates, fp);
		GetAncStates(p->right, rates, fp);

		// for interior nodes, but not tips or the root
		if ( (p->left != NULL || p->right != NULL) && p->anc != NULL)
		{
			// for each character state the node can take
			for (i=0; i<2; i++)
			{
				// fix the node
				if (i == 0)
				{
					p->cl[0] = GetCL0(p, rates);
					p->cl[1] = 0;
				}
				else
				{
					p->cl[0] = 0;
					p->cl[1] = GetCL1(p, rates);
				}

				// walk back until reaching the root
				q = p;
				do
				{
					q = q->anc;
					q->cl[0] = GetCL0(q, rates);
					q->cl[1] = GetCL1(q, rates);
				} while (q->anc != NULL);

				// compute the likelihood for this tree
				/*** FIXME: which of these methods should be used?
				 * this one seems like it should be right, weighting by the stationary distribution
				like[i] = (rates[1]*(q->cl[0]) + rates[0]*(q->cl[1])) / (rates[0]+rates[1]);
				 * but this one is what agrees (perfectly) with Mesquite
				like[i] = q->cl[0] + q->cl[1];
				 ***/
				like[i] = (rates[1]*(q->cl[0]) + rates[0]*(q->cl[1])) / (rates[0]+rates[1]);
			}

			// give p its real cl back
			p->cl[1] = GetCL1(p, rates);
			p->cl[0] = GetCL0(p, rates);

			// record the state probabilities for this node
			fprintf(fp, "%d\t%f\t%d\t%f\t%f\n", p->index, p->time, p->trait, like[0]/(like[0]+like[1]), like[1]/(like[0]+like[1]) );
		}
	}
}
