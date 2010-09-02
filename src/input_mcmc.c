#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input_sim.h"
#include "keyvalue.h"


int AcquireMCMCParams(struct KeyValue *kv, int *model, int *n_iter, double eta[], double tune[])
{
	const char *str;

	str = getKeyValuestring(kv, "model");
	if ( strcmp(str, "b2d2t2") == 0 )
		*model = 7;
	else if ( strcmp(str, "bdt") == 0 )
		*model = 0;
	else if ( strcmp(str, "b2dt") == 0 )
		*model = 1;
	else if ( strcmp(str, "bd2t") == 0 )
		*model = 2;
	else if ( strcmp(str, "bdt2") == 0 )
		*model = 3;
	else if ( strcmp(str, "b2d2t") == 0 )
		*model = 4;
	else if ( strcmp(str, "b2dt2") == 0 )
		*model = 5;
	else if ( strcmp(str, "bd2t2") == 0 )
		*model = 6;
	else
	{
		fprintf(stderr, "\n   error: invalid model specified\n\n");
		return -1;
	}

	*n_iter = getKeyValueint(kv, "n_iter");
	if (*n_iter == KV_INTERR || *n_iter < 0)
		*n_iter = 1000;

	// for the priors
	eta[0] = getKeyValuedouble(kv, "prior1");
	if (eta[0] == KV_FLOATERR || eta[0] < 0)
		eta[0] = 1;
	eta[1] = getKeyValuedouble(kv, "prior2");
	if (eta[1] == KV_FLOATERR || eta[1] < 0)
		eta[1] = 1;
	eta[2] = getKeyValuedouble(kv, "prior3");
	if (eta[2] == KV_FLOATERR || eta[2] < 0)
		eta[2] = 1;
	eta[3] = getKeyValuedouble(kv, "prior4");
	if (eta[3] == KV_FLOATERR || eta[3] < 0)
		eta[3] = 1;
	eta[4] = getKeyValuedouble(kv, "prior5");
	if (eta[4] == KV_FLOATERR || eta[4] < 0)
		eta[4] = 1;
	eta[5] = getKeyValuedouble(kv, "prior6");
	if (eta[5] == KV_FLOATERR || eta[5] < 0)
		eta[5] = 1;

	// tuning parameters
	tune[0] = getKeyValuedouble(kv, "tune1");
	if (tune[0] == KV_FLOATERR || tune[0] < 0)
		tune[0] = 1;
	tune[1] = getKeyValuedouble(kv, "tune2");
	if (tune[1] == KV_FLOATERR || tune[1] < 0)
		tune[1] = 1;
	tune[2] = getKeyValuedouble(kv, "tune3");
	if (tune[2] == KV_FLOATERR || tune[2] < 0)
		tune[2] = 1;
	tune[3] = getKeyValuedouble(kv, "tune4");
	if (tune[3] == KV_FLOATERR || tune[3] < 0)
		tune[3] = 1;
	tune[4] = getKeyValuedouble(kv, "tune5");
	if (tune[4] == KV_FLOATERR || tune[4] < 0)
		tune[4] = 1;
	tune[5] = getKeyValuedouble(kv, "tune6");
	if (tune[5] == KV_FLOATERR || tune[5] < 0)
		tune[5] = 1;

	return 0;
}
