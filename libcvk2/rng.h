#ifndef __rng__h 
#define  __rng__h 1

// -lgsl -lgslcblas -lrt

#include <gsl/gsl_rng.h>

gsl_rng* rng_create(unsigned long int seed);  // 0 - randomize
void rng_delete(gsl_rng* rng);

// gsl_rng_set(rng, unsigned long int  seed);

#endif 

