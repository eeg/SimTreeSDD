/******************************************************************************
* Copyright 2010 Emma Goldberg
* 
* This file is part of SimTreeSDD.
* 
* SimTreeSDD is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* SimTreeSDD is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with SimTreeSDD.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "keyvalue.h"

struct KeyValue *newKeyValue()
{
	struct KeyValue *p;

	p = (struct KeyValue *)malloc(sizeof(struct KeyValue));
	p->nparms = 0;

	return p;
}

void deleteKeyValue(struct KeyValue *p)
{
	int i;
	
	if(!p) return;
	
	for(i = 0; i < p->nparms; i++)
	{
		free(p->key[i]);
		free(p->value[i]);
	}
	free(p);
}

void KeyValueaddparm(struct KeyValue *p, const char *key, const char *value)
{
	p->key[p->nparms]   = strdup(key);
	p->value[p->nparms] = strdup(value);
	p->nparms++;
}

struct KeyValue *loadKeyValue(const char *filename)
{
	struct KeyValue *p;
	FILE *in;
	int i;
	int eq;
	char str[1000], K[200], V[200];

	if(strcmp(filename, "-") == 0) in = stdin;
	else in = fopen(filename, "r");

	if(!in)
	{
		fprintf(stderr, "loadKeyValue: no file: %s\n", filename);
		return 0;
	}

	p = newKeyValue();

	while(1)
	{
		fgets(str, 999, in);
		if(feof(in)) break;
		if(str[0] == 0) continue;
		eq = 0;
		for(i = 0; str[i] != 0; i++)
		{
			if(str[i] == '#') str[i] = 0;
			else if(str[i] == '=' && eq == 0) 
			{
				str[i] = ' ';
				eq = 1;
			}
		}
		if(sscanf(str, "%s %s\n", K, V) != 2) continue;
		if(isdigit(K[0])) continue;
		KeyValueaddparm(p, K, V);
	}

	if(strcmp(filename, "-") != 0) fclose(in);
	
	return p;
}

/* return -1 if not in the list of keys */
int KeyValuekeyindex(const struct KeyValue *p, const char *key)
{
	int i;

	for(i = 0; i < p->nparms; i++)
		if(strcmp(p->key[i], key) == 0) return i;
	
	return -1;
}

int getKeyValueint(const struct KeyValue *p, const char *key)
{
	int i = KeyValuekeyindex(p, key);

	if(i < 0) return KV_INTERR;
	return atoi(p->value[i]);
}

double getKeyValuedouble(const struct KeyValue *p, const char *key)
{
	int i = KeyValuekeyindex(p, key);

	if(i < 0) return KV_FLOATERR;
	return atof(p->value[i]);
}
 
const char *getKeyValuestring(const struct KeyValue *p, const char *key)
{
	int i = KeyValuekeyindex(p, key);

	if(i < 0) return 0;
	return p->value[i];
}

void printKeyValue(const struct KeyValue *p)
{
	int i;

	for(i = 0; i < p->nparms; i++)
		printf("KV: %s = %s\n", p->key[i], p->value[i]);
}
