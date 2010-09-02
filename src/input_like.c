#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input_like.h"
#include "keyvalue.h"


LikeParams *NewLikeParams()
{
	LikeParams *p;
	p = (LikeParams *) malloc(sizeof(LikeParams));

	if (p == NULL)
	{
		fprintf(stderr, "NewLikeParams malloc failed\n");
		exit(1);
	}

	return p;
}


void FreeLikeParams(LikeParams *params)
{
	free(params);
}


int AcquireLikeParams(struct KeyValue *kv, LikeParams *parameters)
{
	const char *str;

	str = getKeyValuestring(kv, "like_func");
	if ( strcmp(str, "prune") == 0 )
		parameters->like_func = 0;
	else if ( strcmp(str, "bd") == 0 )
		parameters->like_func = 1;
	else
	{
		fprintf(stderr, "\n   error: need to specify valid like_func (prune or bd)\n\n");
		return -1;
	}

	parameters->param[0] = getKeyValuedouble(kv, "param1");
	if (parameters->param[0] == KV_FLOATERR)
		parameters->param[0] = 0.1;
	else if (parameters->param[0] == 0)
		parameters->param[0] = TINY;

	parameters->param[1] = getKeyValuedouble(kv, "param2");
	if (parameters->param[1] == KV_FLOATERR)
		parameters->param[1] = 0.1;
	else if (parameters->param[1] == 0)
		parameters->param[1] = TINY;

	parameters->verbosity = getKeyValueint(kv, "verbosity");
	if (parameters->verbosity != 0 && parameters->verbosity != 1)
		parameters->verbosity = 2;

	parameters->maximize = getKeyValueint(kv, "maximize");
	if (parameters->maximize != 0)
		parameters->maximize = 1;

	parameters->reconstruct = getKeyValueint(kv, "reconstruct");
	if (parameters->reconstruct != 0)
		parameters->reconstruct = 1;

	parameters->use_fixed_param = getKeyValueint(kv, "use_fixed_param");
	if (parameters->use_fixed_param <= 0)
		parameters->use_fixed_param = 0;

	parameters->plot_like = getKeyValueint(kv, "plot_like");
	if (parameters->plot_like <= 0)
		parameters->plot_like = 0;


	return 0;		// return -1 elsewhere if there's an error
}
