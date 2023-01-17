#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h> // NOTE: if missed, bizarre errors are possible

#include "mmem.h"
#include "ran_discrete.h"

//-------------------
struct struct_rdtab
//-------------------
{
   int n;
   double* tab;       // table with ascending values (cumulative)  eps, ..., 1.
};

//===============================================
void print_rdtab(rdtab rdt, const char* prefix)
//===============================================
{
    int n = rdt->n;
    for(int i=0; i<n; i++)
    {
        printf("%srdt[%d] of %d = %f\n", prefix, i, n, rdt->tab[i]);
    }
};


//==========================================================
rdtab ran_discrete_rdtab_getnew(int n, const double* weights)
//==========================================================
{
   assert(weights != NULL);
   assert(n > 1);

   rdtab rdt = (rdtab) mmgetnew(sizeof(struct struct_rdtab));
   assert(rdt != NULL);

   rdt->n = n;

   rdt->tab = (double*) mmgetnew(sizeof(double) * n); 
   assert(rdt->tab != NULL);

   int i;

   double sum_weight = 0.;

   for(i=0; i<n; i++)
       sum_weight += weights[i]; 

   double* tab = rdt->tab; // working var

   for(i=0; i<n; i++)
      tab[i] = weights[i] / sum_weight;
    
      // cumulative:
   for(i=1; i<n; i++)
      tab[i] = tab[i-1] + tab[i];

   return rdt;
};

//============================================
void ran_discrete_rdtab_release(rdtab rdt)
//============================================
{
   assert(rdt != NULL);

   if(mmrefcount(rdt) == 1)
      mmrelease(rdt->tab);

   mmrelease(rdt);
};


//==================================
int ran_discrete(gsl_rng* rng, const rdtab rdt)
//==================================
{
   assert(rdt != NULL);

   int i0 = 0;           // first idx
   int i1 = rdt->n - 1;  // last idx 

   double* tab = rdt->tab;
   double u = gsl_ran_flat(rng, 0., 1.);  // uniform [0, ,,, ,1] 
   
   int i01;

   while(1)
   {
      i01 = (i0 + i1) / 2;

      if(u <= tab[i01])
      {
         if(i01 == i0) break;
         i1 = i01;
      }
      else // (u > tab[i01])
      {
         i01 += 1;
         if(i01 == i1) break;
         i0 = i01;
      };
   };

   assert(i01 >= 0);
   assert(i01 < rdt->n);

   return i01;
};


