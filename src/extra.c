#include <stdio.h>
#include <string.h>

#include "extra.h"
#include "nodes.h"

void ChangeFileSuffix(char inname[], char oldend[], char newend[], char outname[])
{
	char *cutme;

	strcpy(outname, inname);
	cutme = strstr(outname, oldend);
	if (cutme != NULL)
		*cutme = '\0';
	strcat(outname, newend);
}


/*******************************************************************************
 * comparison functions for qsort
 ******************************************************************************/
int compare_doubles(const void *a, const void *b)
{
	if      (*(double*)a < *(double*)b)	return -1;
	else if (*(double*)a > *(double*)b)	return  1;
	else								return  0;
}

int compare_branchtimes(const void *a, const void *b)
{
	const BranchNode * va = *((const BranchNode **) a);
	const BranchNode * vb = *((const BranchNode **) b); 

	if      (va->time < vb->time)	return -1;
	else if (va->time > vb->time)	return 1;
	else			             	return 0;
}
