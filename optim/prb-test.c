#include <stdio.h>
#include <assert.h>

#include "prb.h"
#include "funcs.h"

#define STEP_FUNC_PAR 1.0

void print_prb_stats(const prb* p, const char* prefix)
{

    long int penalty_total = prb_penalty_total(p);
    printf("%s penalty_total= %ld\n", prefix, penalty_total);
    
    long var_cost_true = prb_total_var_cost(p, 0.);
    long linehaul_cost_true = prb_total_linehaul_cost(p, 0.);
    printf(
        "%s cost_true=  %ld  %ld\n", 
        prefix, var_cost_true, linehaul_cost_true);

    long var_cost_pseudo = prb_total_var_cost(p, STEP_FUNC_PAR);
    long linehaul_cost_pseudo = prb_total_linehaul_cost(p, STEP_FUNC_PAR);
    printf(
        "%s cost_pseudo=  %ld  %ld\n", 
        prefix, var_cost_pseudo, linehaul_cost_pseudo);

    return;
};


int main(){
    int ndc = 12;
    int ncarriers = 10;
    int nroutes = 5;
    int nrows = 200000;


    prb p = prb_mktest_empty(ndc, ncarriers, nroutes, nrows);
    printf(" ==> p.nrows= %d\n", p.nrows);
    printf(" ==> p.ncols = %d\n", p.ncols);

    long integral_penalty = 0;
    long integral_cost_true_var = 0;
    long integral_cost_true_linehaul = 0;
    long integral_cost_pseudo_var = 0;
    long integral_cost_pseudo_linehaul = 0;

    int i;
    int kk;
    for(kk=0; kk<2; kk++)
        for(i=0; i<p.nrows; i++){
            int j = i % p.ncols;

            if(!prb_is_valid_ij(&p, i, j))
            {
                int found = 0;
                for(int jj=0; jj<p.ncols; jj++)
                {
                    if(prb_is_valid_ij(&p, i, jj)){
                        j = jj;
                        found = 1;
                    }                     
                 
                };
                assert(found);
            };

            int volume_i = arr1d_getval(&(p.row_volume), i);
            if(kk == 1)
                volume_i = -volume_i;

            printf("\n");
            printf(
                " ==> kk=%d i= %d  j=  %d   volume= %d\n", kk, i, j, volume_i);
            print_prb_stats(&p, " -> P0: ");

            // Warning: these calls of prb_cost_delta() shuld precede
            //    the call of prb_add_volume() because the state for
            //    delta calculation shuld be taken before prb_add_volume() call
            integral_penalty += prb_penalty_delta(&p, i, j, volume_i);
            //
            integral_cost_true_var += prb_var_cost_delta(
                &p, i, j, volume_i); 
            integral_cost_true_linehaul += prb_linehaul_cost_delta(
                &p, i, j, volume_i, 0.);
            //
            integral_cost_pseudo_var += prb_var_cost_delta(
                &p, i, j, volume_i); 
            integral_cost_pseudo_linehaul += prb_linehaul_cost_delta(
                &p, i, j, volume_i, STEP_FUNC_PAR);

            if(kk == 0)  // the first pass: we set the row 
                prb_set_row(&p, i, j); 
            else  // the second pass: we clear the row
            {
                assert(kk == 1);
                prb_clear_row(&p, i); 
            };

            printf(" ----\n");
            print_prb_stats(&p, " -> P1: ");

            printf("\n");
            printf(" -> integral_penalty= %ld\n", integral_penalty);
            printf(
                " -> integral_cost_true= %ld   %ld\n", 
                integral_cost_true_var, integral_cost_true_linehaul);
            printf(
                " -> integral_cost_pseudo= %ld   %ld\n", 
                integral_cost_pseudo_var, integral_cost_pseudo_linehaul);
        };

};
