#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "simplex.h"
#include "nodes.h"

/* the degree of similarity among the vertices required to define convergence to a solution
   (I've been using 1.0e-16 or 1.0e-12 or 1.0e-08 */
const double ftol = 1.0e-10;

// if convergence is not reached by this many iterations, give up (100000 or 50000)
const int max_iter = 75000;


static void swap(double *a, double *b)
{
	double temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

/* gets the sum of the function values at each vertex, which is used for stretching out new vertices */
static void get_simsum (double simplex[][MAX_PARAM], double simsum[], int n_param)
{
	int n_vert = n_param+1;
	int i, j;
	double sum;

	for (j=0; j<n_param; j++)
	{
		sum = 0;
		for (i=0; i<n_vert; i++)
			sum += simplex[i][j];
		simsum[j] = sum;
	}
}

static double try_vertex(double simplex[][MAX_PARAM], double y[], double simsum[], double (*func)(TreeInfo *, double []), TreeInfo *tree, int ihi, double fac)
{
	int n_param = tree->n_param;
	int n_vert = n_param+1;
	int j;
	double fac1, fac2;		// stretching factors
	double ptry[n_vert];	// trial value of a potential new vertex
	double ytry;			// trial value of the function at a potential new vertex
	int flag;				// wave this if a parameter value is negative

	
	fac1=(1.0-fac)/n_param;	// set stretching factors
	fac2=fac1-fac;

	// try out a new vertex
	for (j=0; j<n_param; j++) 
		ptry[j] = simsum[j]*fac1 - simplex[ihi][j]*fac2;

	// avoid negative parameter values
	flag = 0;
	for (j=0; j<n_param; j++) 
		if (ptry[j] < 0)
			flag = 1;
	if (flag == 1)
		ytry = 10e50;		
	else
		ytry = (*func)(tree, ptry);

	// if it's a better vertex, replace the worst vertex with it 
	if (ytry < y[ihi])
	{
		y[ihi] = ytry;
		for(j=0; j<n_param; j++)
		{
			simsum[j] += ptry[j] - simplex[ihi][j];
			simplex[ihi][j] = ptry[j];
		}
	}

	return ytry;
}

double DownhillSimplex(double simplex[][MAX_PARAM], double y[], double (*func)(TreeInfo *, double []), TreeInfo *tree)
{
	int n_param = tree->n_param;
	int n_vert = n_param+1;
	int i, j;
	int ihi, inhi, ilo;		// vertex numbers for worst, next-worst, and best vertices
	int niter = 0;			// will contain the number of iterations required
	double rtol;			// will contain how un-similar the vertices are
	double simsum[n_vert];	// see get_simsum()
	double ysave, ytry;

	get_simsum(simplex, simsum, n_param);

	// find the worst, next-worst, and best vertices
	for (;;)
	{
		ilo = 0;
		ihi = y[0]>y[1] ? (inhi=1,0) : (inhi=0,1);
		for (i=0; i<n_vert; i++)
		{
			if (y[i] <= y[ilo])
				ilo = i;
			if (y[i] > y[ihi])
			{
				inhi = ihi;
				ihi = i;
			}
		else if (y[i] > y[inhi] && i != ihi) 
			inhi = i;
		}

		// if the vertices are similar enough, you're done
		rtol = 2.0 * fabs(y[ihi]-y[ilo]) / (fabs(y[ihi])+fabs(y[ilo]));
		if (rtol < ftol)
		{
			swap(&y[0], &y[ilo]);
			for (i=0; i<n_param; i++) 
				swap(&simplex[1][i], &simplex[ilo][i]);
			break;
		}
	
		// if it's been too many iterations, give up
		if (niter >= max_iter)
		{
			fprintf(stderr, "%s: maximum number of iterations exceeded\n", tree->name);
			return niter;
		}
	
		niter += 2;
	
		// try out new vertices
		ytry = try_vertex(simplex, y, simsum, func, tree, ihi, -1.0);
		if (ytry <= y[ilo])
			ytry = try_vertex(simplex, y, simsum, func, tree, ihi, 2.0);
		else if (ytry >= y[inhi]) {
			ysave = y[ihi];
			ytry = try_vertex(simplex, y, simsum, func, tree, ihi, 0.5);
			if (ytry >= ysave) {
				for (i=0; i<n_vert; i++)
				{
					if (i != ilo)
					{
						for (j=0; j<n_param; j++)
							simplex[i][j] = simsum[j] = 0.5*(simplex[i][j]+simplex[ilo][j]);
						y[i] = (*func)(tree, simsum);
					}
				}
				niter += n_param;
				get_simsum(simplex, simsum, n_param);
			}
		}
		else niter--;
	}

	return niter;
}
