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
	// note: segfault here sometimes? use uniform_distribution(0,1-TINY)?
	double temp = uniform_distribution(0,1);
	return log(1-temp) * (-1/lambda);
}
