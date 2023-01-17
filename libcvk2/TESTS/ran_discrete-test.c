#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h> // NOTE: if missed, bizarre errors are possible
#include <gsl/gsl_math.h> // only for test   // vk_remove

#include "mmem.h"

#include "ran_discrete.h"

     // slower version (no binary division):
static double* vk_ran_discrete_table_getnew(int n, const double* weights);
static int vk_ran_discrete(gsl_rng* rng,  int n, double* table);

#define MAX_n 1000

double __weights[MAX_n];

//======================
void test_cmp_slow(int n, int nrep, int seed)
//======================
{
  assert(n > 1);
  assert(n <= MAX_n);
  assert(nrep > 0);

     // fill weights
  int i;
  for(i=0; i<n; i++)
     __weights[i] = i+1;

      // allocate table for slow method (vk_...)
  double* vk_tab = vk_ran_discrete_table_getnew(n, __weights);

      // allocate table for binary-division method 
  rdtab rdt = ran_discrete_rdtab_getnew(n, __weights);

      // allocate / set two gsl_rng-objects:
  gsl_rng* rng = gsl_rng_alloc(gsl_rng_mt19937); // 
  assert(rng != NULL);
  gsl_rng_set (rng, seed);

  gsl_rng* rng2 = gsl_rng_alloc(gsl_rng_mt19937); // 
  assert(rng2 != NULL);
  gsl_rng_set (rng2, seed);

  int count_diff = 0;
  for(i=0; i<nrep; i++)
  {
     int r1 = vk_ran_discrete(rng,  n, vk_tab);
     int r2 = ran_discrete(rng2, rdt);
     if(r1 != r2)
     {
       printf(" error: r1!= r2: r1= %d r2=%d \n", r1, r2);
       count_diff += 1;
     };
  };

  if(count_diff == 0)
    printf("  TEST PASSED: nrep = %d\n", nrep);
  else
    printf("  ERRORS: nrep = %d\n", nrep);


  gsl_rng_free(rng); 
  gsl_rng_free(rng2); 
  ran_discrete_rdtab_release(rdt);
  mmrelease(vk_tab);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
double* vk_ran_discrete_table_getnew(int n, const double* weights)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
     double* rdt = (double*) mmgetnew(sizeof(double) * n);
     assert(rdt != NULL);

     double sum_weight = 0.;

     int i;
     for(i=0; i<n; i++)
         sum_weight += weights[i]; 
     for(i=0; i<n; i++)
        rdt[i] = weights[i] / sum_weight;
      
        // cumulative:
     for(i=1; i<n; i++)
        rdt[i] = rdt[i-1] + rdt[i];

     return rdt;
};

//+++++++++++++++++++++++++++++++++++++++++++
int vk_ran_discrete(gsl_rng* rng,  int n, double* table)
//+++++++++++++++++++++++++++++++++++++++++++
{
   assert(n > 1);
   assert(table != NULL);

   double u = gsl_ran_flat(rng, 0., 1.); 

   int i;
   for(i=0; i<n; i++)
   {
      if(u <= table [i])
          return i;
   };
  
   assert(0); // we should not be here
   return -1;
};




#include <mcheck.h>    

int main(int argc, char* argv[])
{
  setenv("MALLOC_TRACE", "mtrace.log", 1); // no export required with this 
  mtrace();  

  {
      double x = 7.0;
      double x2 = gsl_log1p(x); // vk_remove
      x2 = x2;
  };
  // code is here
  int n = 99;
  int nrep = 2000000;
  int seed = 1001;
  test_cmp_slow(n, nrep, seed);


  muntrace();  
  return 0;
}


