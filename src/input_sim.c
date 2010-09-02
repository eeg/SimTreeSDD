#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input_sim.h"
#include "keyvalue.h"


TreeParams *NewTreeParams()
{
	TreeParams *p;
	p = (TreeParams *) malloc(sizeof(TreeParams));

	if (p == NULL)
	{
		fprintf(stderr, "NewTreeParams malloc failed\n");
		exit(1);
	}

	return p;
}


void FreeTreeParams(TreeParams *params)
{
	free(params);
}


int AcquireTreeParams(struct KeyValue *kv, TreeParams *parameters)
{
	const char *str;

	str = getKeyValuestring(kv, "trait_type");
	if (str)
	{
		if (strcmp(str, "character") == 0)
			parameters->trait_type = 0;
		else if (strcmp(str, "region") == 0)
			parameters->trait_type = 1;
		else
		{
			fprintf(stderr, "trait_type = %s is invalid (use character or region instead)\n", str);
			return -1;
		}
	}
	else
	{
		fprintf(stderr, "need to specify trait_type (character or region)\n");
		return -1;
	}

	parameters->birth[0] = getKeyValuedouble(kv, "birth0");
	if (parameters->birth[0] == KV_FLOATERR)
	{
		fprintf(stderr, "need to specify value of birth0\n");
		return -1;
	}

	parameters->birth[1] = getKeyValuedouble(kv, "birth1");
	if (parameters->birth[1] == KV_FLOATERR)
	{
		fprintf(stderr, "need to specify value of birth1\n");
		return -1;
	}

	parameters->death[0] = getKeyValuedouble(kv, "death0");
	if (parameters->death[0] == KV_FLOATERR)
	{
		fprintf(stderr, "need to specify value of death0\n");
		return -1;
	}

	parameters->death[1] = getKeyValuedouble(kv, "death1");
	if (parameters->death[1] == KV_FLOATERR)
	{
		fprintf(stderr, "need to specify value of death1\n");
		return -1;
	}

	parameters->transition[0] = getKeyValuedouble(kv, "alpha");
	if (parameters->transition[0] == KV_FLOATERR)
	{
		fprintf(stderr, "need to specify value of alpha\n");
		return -1;
	}

	parameters->transition[1] = getKeyValuedouble(kv, "beta");
	if (parameters->transition[1] == KV_FLOATERR)
	{
		fprintf(stderr, "need to specify value of beta\n");
		return -1;
	}

	parameters->end_t = getKeyValuedouble(kv, "end_t");
	if (parameters->end_t == KV_FLOATERR)
	{
		fprintf(stderr, "need to specify value of end_t\n");
		return -1;
	}

	// use default file prefix if none is specified
	str = getKeyValuestring(kv, "file_prefix");
	if (!str)
		strcpy(parameters->file_prefix, "run");
	else
		strcpy(parameters->file_prefix, str);

	// default is high verbosity
	parameters->verbosity = getKeyValueint(kv, "verbosity");
	if (parameters->verbosity == KV_INTERR)
		parameters->verbosity = 2;
	
	// if (parameters->root_state== KV_INTERR), state will be drawn from stationary distribution
	parameters->root_state= getKeyValueint(kv, "root_state");
	
	// default is one tree
	parameters->num_trees = getKeyValueint(kv, "num_trees");
	if (parameters->num_trees == KV_INTERR)
		parameters->num_trees = 1;
	
	// default is start labeling at 1
	parameters->num_start = getKeyValueint(kv, "num_start");
	if (parameters->num_start == KV_INTERR)
		parameters->num_start = 1;
	
	// default is 0 tips required (all simulation runs count)
	parameters->min_tips = getKeyValueint(kv, "min_tips");
	if (parameters->min_tips == KV_INTERR)
		parameters->min_tips = 0;

	// default is to create newick file
	parameters->write_newick = getKeyValueint(kv, "write_newick");
	if (parameters->write_newick > 0 || parameters->write_newick == KV_INTERR)
		parameters->write_newick = 1;
	else
		parameters->write_newick = 0;
	
	// default is to create nexus file
	parameters->write_nexus = getKeyValueint(kv, "write_nexus");
	if (parameters->write_nexus > 0 || parameters->write_nexus == KV_INTERR)
		parameters->write_nexus = 1;
	else
		parameters->write_nexus = 0;
	
	// default is not to create bmstrait file
	parameters->write_bmstrait = getKeyValueint(kv, "write_bmstrait");
	if (parameters->write_bmstrait <= 0 || parameters->write_bmstrait == KV_INTERR)
		parameters->write_bmstrait = 0;
	else
		parameters->write_bmstrait = 1;
	
	// default is not to create ttn file
	parameters->write_ttn = getKeyValueint(kv, "write_ttn");
	if (parameters->write_ttn <= 0 || parameters->write_ttn == KV_INTERR)
		parameters->write_ttn = 0;
	else
		parameters->write_ttn = 1;


	return 0;
}
