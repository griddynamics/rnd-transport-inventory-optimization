#ifndef __ran_discrete__h 
#define  __ran_discrete__h 

// it is a replacement of gsl_ran_discrete()
//   because gsl_ran_discrete_preproc() seems to be very slow 
//   (if the distribution changes frequently, then causes performance loss)

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h> // NOTE: if missed, bizarre errors are possible

typedef struct struct_rdtab* rdtab;

void print_rdtab(rdtab rdt, const char* prefix);

rdtab ran_discrete_rdtab_getnew(int n, const double* weights);
void ran_discrete_rdtab_release(rdtab rdt);

int ran_discrete(gsl_rng* rng, const rdtab rdt);




#endif

