#include <stdio.h>
#include <stdlib.h>
#include <math.h>			// for isfinite(); requires -std=gnu99 (or -std=c99 ?) compiler flag
#include "output.h"
#include "output_like.h"
#include "extra.h"


FILE *OpenFixedLikeFile(LikeParams *parameters)
{
	FILE *fp;
	char *filename = "fixed_results.dat";
	char message[1000];

	fp = fopen(filename, "w");

	if (fp != NULL)
	{
		sprintf(message, "created file %s", filename);
		PrintMessage(message, 1);
		fprintf(fp, "param1 = %e, param2 = %e\n", parameters->param[0], parameters->param[1]);
		fprintf(fp, "tree\tn_tips\tlog(likelihood)\n");
	}
	else
		fprintf(stderr, "can't open file %s for writing\n", filename);

	sprintf(message, "param1 = %e, param2 = %e", parameters->param[0], parameters->param[1]);
	PrintMessage(message, 0);

	return fp;
}


void ReportFixedLike(FILE *fp, double loglike, TreeInfo *tree)
{
	char message[1000];

	if (fp)
//		fprintf(fp, "%s: log(likelihood) = %e\n", tree->name, loglike);
		fprintf(fp, "%s\t\t%d\t\t%e\n", tree->name, tree->n_tips, loglike);
	sprintf(message, "%s: log(likelihood) = %e for given param1 and param2", tree->name, loglike);
	PrintMessage(message, 0);
}


FILE *OpenMLFile(LikeParams *parameters)
{
	FILE *fp;
	char *filename = "ML_results.dat";
	char message[1000];

	fp = fopen(filename, "w");

	if (fp != NULL)
	{
		sprintf(message, "created file %s", filename);
		PrintMessage(message, 1);
		fprintf(fp, "tree\tn_tips\tparam1\tparam2\tlog(likelihood)\n");
	}
	else
		fprintf(stderr, "can't open file %s for writing\n", filename);

	return fp;
}


void ReportML(FILE *fp, double loglike, TreeInfo *tree, int niter, double rates[])
{
	char message[1000];

	if (fp)
		fprintf(fp, "%s\t\t%d\t\t%e\t\t%e\t\t%e\n", tree->name, tree->n_tips, rates[0], rates[1], loglike);
	sprintf(message, "%s: ML parameter values (%d iterations, logL = %e) are...\n   param1: %e       param2: %e", tree->name, niter, loglike, rates[0], rates[1]);
	PrintMessage(message, 0);
}


void MakeLikePlot(TreeInfo *tree, double total_loglike[][LIKEPLOTSIZE], double (*likefunc)(TreeInfo *, double []))
{
	FILE *fp;
	char filename[100];
	int i, j;
	double rates[2];
	double loglike;
	char message[1000];

	ChangeFileSuffix(tree->name, ".ttn", ".plot", filename);
	fp = fopen(filename, "w");

	if (fp != NULL)
	{
		for (i=0; i<LIKEPLOTSIZE; i++)
		{
			rates[0] = i*0.02 + 0.0001;
			for (j=0; j<LIKEPLOTSIZE; j++)
			{
				rates[1] = j*0.02 + 0.0001;
				loglike = -likefunc(tree, rates);
				// FIXME: really need to do something in GetPrunedNegLogL instead
				if (isfinite(loglike) == 0)
					loglike = -1e+10;
				total_loglike[i][j] += loglike;
				fprintf(fp, "%f ", loglike);
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
	else
		fprintf(stderr, "can't open file %s for writing\n", filename);
	sprintf(message, "%s: created file %s", tree->name, filename);
	PrintMessage(message, 1);
}


void WriteTotalLikeFile(double total_loglike[][LIKEPLOTSIZE], LikeParams *parameters)
{
	FILE *fp;
	char *filename = "total_loglike.dat";
	int i, j;
	char message[1000];

	fp = fopen(filename, "w");
	for (i=0; i<LIKEPLOTSIZE; i++)
	{
		for (j=0; j<LIKEPLOTSIZE; j++)
			fprintf(fp, "%e ", total_loglike[i][j]);
		fprintf(fp, "\n");
	}
	fclose(fp);

	sprintf(message, "created file %s", filename);
	PrintMessage(message, 1);
}


void PrintBranches(BranchNode *branches[], int n_branches)
{
	int n;
	BranchNode *b;

	for (n=0; n<n_branches; n++)
	{
		printf("n = %d:   ", n);

		b = branches[n];
		while (b != NULL)
		{
			printf("%f, %d   ", b->time, b->trait);
			b = b->next;
		}
		printf("\n");
	}
}
