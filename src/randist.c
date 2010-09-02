#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "randist.h"


double uniform_distribution(double min, double max)
{
	return random()/((double)RAND_MAX) * (max-min) + min;
}


double exponential_distribution(double lambda)
{
	double temp = uniform_distribution(0,1);
// FIXME: segfault here sometimes -- use uniform_distribution(0,1-TINY) ?
	return log(1-temp) * (-1/lambda);
}
