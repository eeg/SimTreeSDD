#include <stdio.h>
#include <stdlib.h>
#include "output.h"


/*******************************************************************************
* the value of this is set in the main program, but it's only used in funcs here
* 2 = print lots, 1 = print some, 0 = print nothing
*******************************************************************************/
int verbosity;


/*******************************************************************************
 * print a message if the verbosity setting calls for it
 * (this is to avoid using the verbosity parameter in files other than this one
*******************************************************************************/
void PrintMessage(char *message, int verb_level)
{
	if (verbosity > verb_level)
		printf("%s\n", message);
}


// this just puts a verbosity wrapper around the real ShowTree functions
void ShowTree(TreeNode *p)
{
	if (verbosity == 2)
	{
		printf("\nall tree nodes and tips:\n");
		ShowTreeIndent(p, 2);
		printf("\n");
	}

	if (verbosity > 0)
	{
		printf("tips of tree:\n");
		ShowTreeParentheses(p);
		printf("\n");
	}
}

/*******************************************************************************
 * show tree in parentheses format
 * (Huelsenbeck's function WriteTree)
*******************************************************************************/
void ShowTreeParentheses(TreeNode *p)
{
	if (p->left == NULL && p->right == NULL)
	{
		printf ("%d", p->index);
	}
	else
	{
		putchar ('(');
		ShowTreeParentheses(p->left);
		putchar (',');
		ShowTreeParentheses(p->right);
		putchar (')');
	}
}


/*******************************************************************************
 * show tree in indented format
 * (Huelsenbeck's function ShowTree)
*******************************************************************************/
int Dex(TreeNode *p);
void ShowTreeIndent(TreeNode *p, int indent)
{
	if (p != NULL)
	{
		printf("%*cN %d (l=%d r=%d a=%d t=%3.2f len=%3.2f tr=%d)\n", indent, ' ', 
		  Dex(p), Dex(p->left), Dex(p->right), Dex(p->anc), p->time, p->length, p->trait);
		ShowTreeIndent(p->left, indent + 2);
		ShowTreeIndent(p->right, indent + 2);
	}
}

// returns the index of p, if p exists, and -1 otherwise; for use with ShowTreeIndent
int Dex(TreeNode *p)
{
	return (p == NULL) ? -1 : p->index;
}


/*******************************************************************************
 * print parameter values
*******************************************************************************/
void ShowParamValues(TreeParams *parameters)
{
	// for a binary character
	if (parameters->trait_type == 0)
	{
		if (verbosity == 2)
		{
			printf(	"   speciation rate, state 0:  %f\n"
					"   speciation rate, state 1:  %f\n"
					"   extinction rate, state 0:  %f\n"
					"   extinction rate, state 1:  %f\n"
					"   transition rate, 0 -> 1 :  %f\n"
					"   transition rate, 1 -> 0 :  %f\n"
					"   time to grow tree       :  %f\n",
				parameters->birth[0], parameters->birth[1], parameters->death[0], parameters->death[1], parameters->transition[0], parameters->transition[1], parameters->end_t);
		}

		else if (verbosity == 1)
		{
			printf( "   birth, death, transition rates; end_t:\n"
					"   %f %f %f %f %f %f %f\n", 
					parameters->birth[0], parameters->birth[1], parameters->death[0], parameters->death[1], parameters->transition[0], parameters->transition[1], parameters->end_t);
		}
	}

	// for a geographic character
	else
	{
		if (verbosity == 2)
		{
			printf(	"   speciation rate, region A :  %f\n"
					"   speciation rate, region B :  %f\n"
					"   allopatric speciation rate:  %f\n"
					"   extinction rate, region A :  %f\n"
					"   extinction rate, region B :  %f\n"
					"   dispersal rate, A -> B    :  %f\n"
					"   dispersal rate, B -> A    :  %f\n"
					"   time to grow tree         :  %f\n",
				parameters->birth[0], parameters->birth[1], parameters->birth[2], parameters->death[0], parameters->death[1], parameters->transition[0], parameters->transition[1], parameters->end_t);
		}

		else if (verbosity == 1)
		{
			printf( "   birth, death, dispersal; end_t:\n"
					"   %f %f %f, %f %f, %f %f, %f\n", 
					parameters->birth[0], parameters->birth[1], parameters->birth[2], parameters->death[0], parameters->death[1], parameters->transition[0], parameters->transition[1], parameters->end_t);
		}
	}
}


/*******************************************************************************
* print the root's character state
*******************************************************************************/
void ShowRootState(int report)
{
	if (verbosity == 2)
	{
		if (report == 0 || report == 1)
			printf("root character state fixed to ");
		else
			printf("root state drawn to be ");

		if (report == 0 || report == 2)
			printf("0\n");
		else
			printf("1\n");
	}
}


/*******************************************************************************
* print a note if the root was moved
*******************************************************************************/
void ShowRootMove(int report)
{
	if (verbosity == 2)
		if (report > 0)
			printf("one branch from the root went extinct, so the root was moved\n");
}


/*******************************************************************************
* print a note if the tree was discarded for having too few tips
*******************************************************************************/
void ShowTreeDiscard(TreeParams *parameters, int reason)
{
	if (verbosity == 2)
	{
		if (reason == 0)
			printf("not enough tips (min_tips = %d) -- discarding this tree\n", parameters->min_tips);
		else if (reason == 1)
			printf("all tips in same state -- discarding this tree\n");
	}
}


/*******************************************************************************
 * create a file containing the tree in Newick format
 * nodes and tips are labeled by number and with their character state
*******************************************************************************/
void WriteNewickFile(TreeNode *p, char *prefix)
{
	FILE *fp;
	char filename[128];

	strcpy(filename, prefix);
	strcat(filename, ".tre");

	fp = fopen(filename, "w");

	if (fp != NULL)
	{
		// NOTE: recently added ( and ) here
		fprintf(fp, "(");
		WriteNewickTree(p, fp);
		fprintf(fp, ");\n");
/*
		WriteNewickTree(p, fp);
		fprintf(fp, ";\n");
*/

		fclose(fp);

		if (verbosity > 0)
			printf("created file %s\n", filename);
	}
	else
		printf("ERROR: can't open %s for writing\n", filename);
}

// for use with WriteNewickFile
void WriteNewickTree(TreeNode *p, FILE *fp)
{
	if (p->left == NULL && p->right == NULL)
		fprintf(fp, "%d_tip%d:%lf", p->trait, p->index, p->length);
	else
	{
		fprintf(fp, "(");
		WriteNewickTree(p->left, fp);
		fprintf(fp, ",");
		WriteNewickTree(p->right, fp);
		fprintf(fp, ")%d_n%d:%lf", p->trait, p->index, p->length);
	}
}


/*******************************************************************************
 * write a Nexus file, containing character and tree information
*******************************************************************************/
void WriteNexusFile(TreeNode *p, char *prefix, int n_tips, int states[], TreeParams *parameters)
{
	FILE *fp;
	char filename[128];
	int i;

	strcpy(filename, prefix);
	strcat(filename, ".nex");

	fp = fopen(filename, "w");

	if (fp != NULL)
	{
		fprintf (fp, "#NEXUS\n\nBEGIN TAXA;\n\tDIMENSIONS NTAX = %d;\n\tTAXLABELS", n_tips);
		for (i=0; i<n_tips; i++)
			fprintf(fp, " %d_tip%d", states[i], i);
		fprintf(fp, ";\nEND;\n\n");

		if (parameters->trait_type == 0)
			fprintf(fp, "BEGIN CHARACTERS;\n\tDIMENSIONS NCHAR = 1;\n\tFORMAT SYMBOLS = \"0 1\";\n\tMATRIX");
		else
			fprintf(fp, "BEGIN CHARACTERS;\n\tDIMENSIONS NCHAR = 1;\n\tFORMAT SYMBOLS = \"0 1 2\";\n\tMATRIX");

		for (i=0; i<n_tips; i++)
			fprintf(fp, "\n\t\t%d_tip%d %d", states[i], i, states[i]);
		fprintf(fp, ";\nEND;\n\n");

		fprintf(fp,"BEGIN TREES;\n\tTRANSLATE\n");
		for (i=0; i<n_tips-1; i++)
			fprintf(fp, "\t\t%d %d_tip%d,\n", i, states[i], i);
		fprintf(fp, "\t\t%d %d_tip%d;\n", n_tips-1, states[n_tips-1], n_tips-1);
		fprintf(fp, "\tTREE simtree = ");
		WriteNexusTree(p, fp);
		fprintf(fp, ";\nEND;\n");

		fclose(fp);

		if (verbosity > 0)
			printf("created file %s\n", filename);
	}
	else
		printf("ERROR: can't open %s for writing\n", filename);
}

// for use with WriteNexusFile and WriteTTNFile
void WriteNexusTree(TreeNode *p, FILE *fp)
{
	if (p->left == NULL && p->right == NULL)
		fprintf(fp, "%d:%lf", p->index, p->length);
	else
	{
		fprintf(fp, "(");
		WriteNexusTree(p->left, fp);
		fprintf(fp, ",");
		WriteNexusTree(p->right, fp);
		fprintf(fp, "):%lf", p->length);
	}
}


/*******************************************************************************
 * writes the tip trait data to a file for use by BayesMultiState
*******************************************************************************/
void WriteBMSTraitFile(TreeNode *p, char *prefix, int n_tips, int states[])
{
	FILE *fp;
	char filename[100];
	int i;

	strcpy(filename, prefix);
	strcat(filename, ".bmstrait");

	fp = fopen(filename, "w");

	if (fp != NULL)
	{
		for (i=0; i<n_tips; i++)
			fprintf(fp, "%d_tip%d\t%d\n", states[i], i, states[i]);

		fclose(fp);

		if (verbosity > 0)
			printf("created file %s\n", filename);
	}
	else
		printf("ERROR: can't open %s for writing\n", filename);
}


/*******************************************************************************
 * writes the tree string and list of trait values for use by my read_newick
*******************************************************************************/
void WriteTTNTips(TreeNode *p, FILE *fp);
void WriteTTNNodes(TreeNode *p, FILE *fp);
void WriteTTNFile(TreeNode *p, char *prefix)
{
	FILE *fp;
	char filename[100];

	strcpy(filename, prefix);
	strcat(filename, ".ttn");

	fp = fopen(filename, "w");

	if (fp != NULL)
	{
		WriteNexusTree(p, fp);
		fprintf(fp, ";\n");

		WriteTTNTips(p, fp);
		WriteTTNNodes(p, fp);

		fclose(fp);

		if (verbosity > 0)
			printf("created file %s\n", filename);
	}
	else
		printf("ERROR: can't open %s for writing\n", filename);
}

// for use with WriteTTNFile
void WriteTTNTips(TreeNode *p, FILE *fp)
{
	if (p != NULL)
	{
		WriteTTNTips(p->left, fp);
		WriteTTNTips(p->right, fp);
		if (p->left == NULL && p->right == NULL)
		{
			fprintf(fp, "%d\t%d\n", p->index, p->trait);
		}
	}
}

// for use with WriteTTNFile
void WriteTTNNodes(TreeNode *p, FILE *fp)
{
	if (p != NULL)
	{
		WriteTTNNodes(p->left, fp);
		WriteTTNNodes(p->right, fp);
		if (p->left == NULL && p->right == NULL)
		{
		}
		else
		{
			if (p->ptrait != -1)
			{
				fprintf(fp, "%d\t%d\n", p->index, p->ptrait);
			}
			else
			{
				fprintf(fp, "%d\t%d\n", p->index, p->trait);
			}
		}
	}
}

// FIXME: use ptrait for node labels in .tre output, too
