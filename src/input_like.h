/*******************************************************************************
 * these functions allow information to be given to the likelihood program
 *   parameter values
 *   file names and locations
 *   verbosity setting
 ******************************************************************************/

#ifndef __INPUTLIKE_H__
#define __INPUTLIKE_H__ 

#define TINY 1E-16

#include "keyvalue.h"

typedef struct
{
	/*** likelihood function    param1      param2
	 *   prune (0)              0 -> 1      1 -> 0
	 *   bd (1)                 speciation  extinction
	 ***/
	int like_func;
	double param[6];			// so far, only two are used...
	int maximize;				// whether to maximize the likelihood
	int reconstruct;			// whether to reconstruct ancestral states (if like_func = prune)
	int use_fixed_param;		// whether to get the likelihood and anc. states for given parameter values
	int plot_like;				// whether to make files for plotting the likelihood surface
	int verbosity;				// how much stuff to print to the screen
} LikeParams;

LikeParams *NewLikeParams();
void FreeLikeParams(LikeParams *params);
int AcquireLikeParams(struct KeyValue *kv, LikeParams *parameters);


#endif
