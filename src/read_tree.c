#include <stdio.h>
#include <stdlib.h>
#include "read_tree.h"

extern int intNodeNum;	// defined in label.c

/*******************************************************************************
 * .ttn file format is
 *      (...tree string...);
 *      tip1<TAB>state1		[optional, but required when giving node states, too]
 *      tip2<TAB>state2
 *      ...
 *      nodeN<TAB>stateN	[optional]
 *      ... (nodes ordered by pass-up sequence)
 * right now, there is no error checking, so make sure the file format correct!
 ******************************************************************************/
TreeNode *ReadTree(char *filename)
{
	TreeNode *root;
	// TODO: connect these with TOO_BIG in build.h
	 char tree_string[3000000];
	int states[100000];
	FILE *fp;
	int n_states, n_tips, n_nodes;
	

	// TODO: check if file exists and has the right form
	fp = fopen(filename, "r");
	
	// get the Newick tree string
	fscanf(fp, "%s\n", tree_string);

	// get the tip (and node) states
	n_states = 0;
	while ( !feof(fp) )
	{
		fscanf(fp, "%*s\t%d\n", &states[n_states++]);
	}
	fclose(fp);

	// read in the tree
	root = NewNode(NULL, 0);
	ReadTreeString(root, tree_string);

	// label the tree
	intNodeNum = 0;
	LabelTips(root);
	n_tips = intNodeNum;
	LabelInteriorNodes(root);
	n_nodes = intNodeNum;
	AssignNodeTimes(root);

	// assign the tip (and node) states
	if (n_states >= n_tips)
	{
		PutTipStates(root, states);
		if (n_states == n_nodes)
			PutInteriorNodeStates(root, states);
	}

	return root;
}


/*******************************************************************************
 * apply the state values in states[] to the tips of the tree
 ******************************************************************************/
void PutTipStates(TreeNode *p, int states[])
{
	if (p != NULL)
	{
		PutTipStates(p->left, states);
		PutTipStates(p->right, states);
		if (p->left == NULL && p->right == NULL)
		{
			p->trait = states[p->index];
		}
	}
}


/*******************************************************************************
 * apply the state values in states[] to the interior nodes of the tree
 ******************************************************************************/
void PutInteriorNodeStates(TreeNode *p, int states[])
{
	if (p != NULL)
	{
		PutInteriorNodeStates(p->left, states);
		PutInteriorNodeStates(p->right, states);
		if (p->left == NULL && p->right == NULL)
		{
		}
		else
		{
			p->trait = states[p->index];
		}
	}
}


/*******************************************************************************
 * this function is based on code from blackrim.org, but it ignores node labels
 *   http://blackrim.org/ink/article/5/read-newick (Stephen Andrew Smith)
 ******************************************************************************/
void ReadTreeString(TreeNode *root, char *tree_string)
{
	int start = 1;
	int keepGoing = 1;
	int index = 0;
	char nextChar = tree_string[index];

	TreeNode *curNode = root;
	TreeNode *newNode;

	while (keepGoing == 1)
	{

		if (nextChar == '(')
		{
			if (start == 1)
				start = 0;
			else
			{
				newNode = NewNode(curNode, 0);
				// consider using num_descendents instead...
				if (curNode->left == NULL)
					curNode->left = newNode;
				else if (curNode->right == NULL)
					curNode->right = newNode;
				else
					fprintf(stderr, "ERROR in reading Newick tree file\n");
				curNode = newNode;
			}
		}

		else if (nextChar == ',')
			curNode = curNode->anc;

		else if (nextChar == ')')
		{			
			curNode = curNode->anc;
			index++;
			nextChar = tree_string[index];
			if (nextChar!=',' && nextChar!=')' && nextChar!='(' && nextChar!= ';' && nextChar !=':')
			{
				char name[100] = "b";
				int goingName = 1;
				int namePlacement = 0;
				while (goingName == 1)
				{
					name[namePlacement] = nextChar;
					namePlacement++;
					index++;
					nextChar = tree_string[index];
					if(nextChar==',' || nextChar==')' || nextChar==':' || nextChar=='(' || nextChar== ';')
					{
						goingName = 0;
						break;
					}
				}
			}
			index--;
			nextChar = tree_string[index];			
		}

		else if (nextChar == ';')
		{
			keepGoing = 0;
			break;
		}

		else if (nextChar == ' ') { }

		else if (nextChar == ':')
		{			
			index++;
			nextChar = tree_string[index];
			char len[100] = {};
			int Tlen = 0; 
			int goingName = 1;
			while (goingName==1)
			{
				len[Tlen] = nextChar;
				index++;
				nextChar = tree_string[index];
				Tlen++;
				if (nextChar==',' || nextChar==')' || nextChar==':' || nextChar==';')
				{
					goingName = 0;
					break;
				}
			}
			double y = strtod(len, NULL);
			curNode->length = y;
			index--;
			nextChar = tree_string[index];
		}

		else
		{
			newNode = NewNode(curNode, 0);
			// consider using num_descendents instead...
			if (curNode->left == NULL)
				curNode->left = newNode;
			else if (curNode->right == NULL)
				curNode->right = newNode;
			else
				fprintf(stderr, "ERROR in reading Newick tree file\n");
			curNode = newNode;
			int goingName = 1;
			while(goingName == 1)
			{
				index++;
				nextChar = tree_string[index];
				if(nextChar==',' || nextChar==')' || nextChar==':')
				{
					goingName = 0;
					break;
				}
			}
			index--;
			nextChar = tree_string[index];
		}
		index++;
		nextChar = tree_string[index];
	}
}
