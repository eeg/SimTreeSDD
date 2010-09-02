/*******************************************************************************
 * these functions allow information to be given to the simulator
 *   parameter values
 *   file names and locations
 *   verbosity setting
 ******************************************************************************/

#ifndef __INPUTMCMC_H__
#define __INPUTMCMC_H__ 

#include "keyvalue.h"

int AcquireMCMCParams(struct KeyValue *kv, int *model, int *n_iter, double eta[], double tune[]);

#endif
