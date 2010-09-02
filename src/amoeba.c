#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "amoeba.h"
#include "nodes.h"

/* the degree of similarity among the vertices required to define convergence to a solution
   (I've been using 1.0e-16 or 1.0e-12 or 1.0e-08 */
const double ftol = 1.0e-16;

// if convergence is not reached by this many iterations, give up (100000 or 50000)
const int max_iter = 50000;


static void swap(double *a, double *b)
{
	double temp;
	
	temp = *a;
	*a = *b;
	*b = temp;
}

/* gets the sum of the parameters at each vertex, which is used for stretching out new vertices */
static void get_psum (double p[][MAX_PARAM], double psum[], int n_param)
{
	int n_vert = n_param+1;
	int i, j;
	double sum;

	for (j=0; j<n_param; j++) {
		for (sum=0.0,i=0; i<n_vert; i++)
			sum += p[i][j];
		psum[j] = sum;
	}
}

static double amotry (double p[][MAX_PARAM], double y[], double psum[], 
	double (*funk)(TreeInfo *, double []), TreeInfo *tree, int ihi, double fac)
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
		ptry[j] = psum[j]*fac1 - p[ihi][j]*fac2;

	// avoid negative parameter values
	flag = 0;
	for (j=0; j<n_param; j++) 
		if (ptry[j] < 0)
			flag = 1;
	if (flag == 1)
		ytry = 10e50;		
	else
		ytry = (*funk)(tree, ptry);

/*
	// my penalties
	for (j=0; j<n_param; j++) 
	{
		// negative parameter values are bad
		if (ptry[j] < 0)
			ytry += (-ptry[j])*100000;
		// ridiculously large parameter values are also bad
		else if (ptry[j] > 50)
			ytry += ptry[j]*10;
	}
*/

	// if it's a better vertex, replace the worst vertex with it 
	if (ytry < y[ihi]) {
		y[ihi] = ytry;
		for (j=0; j<n_param; j++) {
			psum[j] += ptry[j] - p[ihi][j];
			p[ihi][j] = ptry[j];
		}
	}

	return ytry;
}

double amoeba (double p[][MAX_PARAM], double y[], double (*funk)(TreeInfo *, double []), TreeInfo *tree)
{
	int n_param = tree->n_param;
	int n_vert = n_param+1;
	int i, j;
	int ihi, inhi, ilo;		// vertex numbers for worst, next-worst, and best vertices
	int niter = 0;			// will contain the number of iterations required
	double rtol;			// will contain how un-similar the vertices are
	double psum[n_vert];	// see get_psum()
	double ysave, ytry;
//	FILE *fp;				// used if max_iter is exceeded

	get_psum(p, psum, n_param);

	// find the worst, next-worst, and best vertices
	for (;;) {
		ilo = 0;
		ihi = y[0]>y[1] ? (inhi=1,0) : (inhi=0,1);
		for (i=0; i<n_vert; i++) {
			if (y[i] <= y[ilo])
				ilo = i;
			if (y[i] > y[ihi]) {
				inhi = ihi;
				ihi = i;
			}
		else if (y[i] > y[inhi] && i != ihi) 
			inhi = i;
		}

		// if the vertices are similar enough, you're done
		rtol=2.0*fabs(y[ihi]-y[ilo])/(fabs(y[ihi])+fabs(y[ilo]));
		if (rtol < ftol) {
			swap(&y[0],&y[ilo]);
			for (i=0;i<n_param;i++) 
				swap(&p[1][i],&p[ilo][i]);
			break;
		}
	
		// if it's been too many iterations, give up and write the best estimate to a file
		if (niter >= max_iter) {

/*
			fp = fopen("bs_maxiter1.dat", "a");
			fprintf(fp, "rtol = %e: ", rtol);
			for (j = 0; j < n_param; j++) 
				fprintf(fp, "%f\t", p[ilo][j]);
			fprintf(fp, "\n");
			fclose(fp);
*/

			fprintf(stderr, "%s: maximum number of iterations exceeded\n", tree->name);
//			exit(1);
			return niter;
		}
	
		niter += 2;
	
		// try out new vertices
		ytry = amotry(p, y, psum, funk, tree, ihi, -1.0);
		if (ytry <= y[ilo])
			ytry = amotry(p, y, psum, funk, tree, ihi, 2.0);
		else if (ytry >= y[inhi]) {
			ysave = y[ihi];
			ytry = amotry(p, y, psum, funk, tree, ihi, 0.5);
			if (ytry >= ysave) {
				for (i=0; i<n_vert; i++) {
					if (i != ilo) {
						for (j=0; j<n_param; j++)
							p[i][j] = psum[j] = 0.5*(p[i][j]+p[ilo][j]);
						y[i] = (*funk)(tree, psum);
					}
				}
				niter += n_param;
				get_psum(p, psum, n_param);
			}
		}
		else niter--;
//printf("%e\t%e\t%f\t%f\t%f\t%f\t%f\t%f\n", rtol, y[ilo], p[ilo][0], p[ilo][1], p[ilo][2], p[ilo][3], p[ilo][4], p[ilo][5]);
	}

	return niter;
}
/* (C) Copr. 1986-92 Numerical Recipes Software . */
