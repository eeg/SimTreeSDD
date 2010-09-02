/*******************************************************************************
 * these functions don't seem to belong with anything else...
 ******************************************************************************/

#ifndef __EXTRA_H__
#define __EXTRA_H__ 

#define UNDEF 999999

void ChangeFileSuffix(char inname[], char oldend[], char newend[], char outname[]);

int compare_doubles(const void *a, const void *b);
int compare_branchtimes(const void *a, const void *b);


#endif
