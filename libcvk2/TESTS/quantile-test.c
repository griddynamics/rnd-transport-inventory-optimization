#include <stdio.h>

#include <gsl/gsl_randist.h>

#include "rng.h"
#include "quantile.h" 

#define SIZE 1000


int main()
{
   
   int seed = 10123;
   gsl_rng* rng_ptr = rng_create(seed); 

   double data[SIZE];

   for(int i=0; i<SIZE; i++)
       data[i] = (double)i / SIZE;

   gsl_ran_shuffle (rng_ptr, (void*)data, SIZE, sizeof(double));

   double quantiles[5];
   double q[5] = {0.01, 0.25, 0.5, 0.75, 0.99};
   int nq = 5;
   calc_quantiles(data, SIZE, q, nq, quantiles);

   for(int i=0; i<5; i++)
   {
       printf(" q=  %f  qv=  %f\n", q[i], quantiles[i]);
   }

   rng_delete(rng_ptr);
}

