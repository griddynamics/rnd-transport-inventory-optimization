#include <stdio.h> 
#include <assert.h>
#include <math.h>  // pow(),

#include "prb_run.h"
#include "rng.h"
#include "prb.h"
//#include "funcs.h"

static void iter_one_pass(
    prb* p, gsl_rng* rng, double rel_temp, 
    double stepfunc_par, double max_exp);


//=====================================================
prb prb_generate(
    long seed, int ndc, int ncarriers, int nroutes, int nrows, int ndays,
    double max_constraints_to_total, 
    double cost_per_truck_mult, int linehaul_flag)
//=====================================================
{
    gsl_rng* rng = rng_create(seed); 

    printf("  -> prb_generate: seed= %ld\n", seed);

    prb pp = prb_mktest_empty_random(
        ndc, ncarriers, nroutes, nrows, ndays, rng, 
        max_constraints_to_total, cost_per_truck_mult, linehaul_flag);

    prb_print_cost(&pp, " => P00: After prb_mktest_empty_random()");

    prb_fill_test_random(&pp, rng);
    printf("\n");
    prb_print_cost(&pp, " => P01: After prb_fill_test_random");

    rng_delete(rng); 

    // All multipliers are 1 on default
    pp.var_cost_mult = 1;  //
    pp.linehaul_cost_mult = 1; //
    pp.penalty_mult_dc_sday = 1;
    pp.penalty_mult_hub_dday = 1;
    pp.penalty_mult_carrier_dc_sday = 1;

    return pp;
};

//=====================================================
void prb_set_var_cost_mult(prb* p, int mult)
//=====================================================
{
    assert(prb_is_initialized(p));
    p->var_cost_mult = mult;
};

//=====================================================
void prb_set_linehaul_cost_mult(prb* p, int mult)
//=====================================================
{
    assert(prb_is_initialized(p));
    p->linehaul_cost_mult = mult;
};

//=====================================================
void prb_set_penalty_mult_dc_sday(prb* p, int mult)
//=====================================================
{
    assert(prb_is_initialized(p));
    p->penalty_mult_dc_sday = mult;
};

//=====================================================
void prb_set_penalty_mult_carrier_dc_sday(prb* p, int mult)
//=====================================================
{
    assert(prb_is_initialized(p));
    p->penalty_mult_carrier_dc_sday = mult;
};





//===========================================================
void run_session(
    prb* p, const schedule sch, int nstep, int nrep, 
    const char* session_name, gsl_rng* rng, double max_exp)
//===========================================================
{
    assert(nstep >= 2);
    assert(nrep >= 1);

    double delta_rel_temp = sch.rel_temp.v1 - sch.rel_temp.v0;
    double delta_penalty = sch.penalty.v1 - sch.penalty.v0;
    double delta_stepfunc_par = sch.stepfunc_par.v1 - sch.stepfunc_par.v0;

    int count = 0;
    for(int i=0; i<nstep; i++)
    {
        double x = (double)i / (nstep - 1);  // changes from 0.0 to 1.0:w

        double rel_temp_i = 
            sch.rel_temp.v0 + delta_rel_temp * pow(x, sch.rel_temp.power);

        int penalty_i =  
            (int)ceil(
                sch.penalty.v0 + delta_penalty * pow(x, sch.penalty.power)
            );

        double stepfunc_par_i = 
            sch.stepfunc_par.v0 
            + delta_stepfunc_par * pow(x, sch.stepfunc_par.power);

        prb_set_penalty(p, (int)round(penalty_i));

        for(int j=0; j<nrep; j++)
        {
            // if count == 0, just print the statistics, no optmization
            if(count > 0)  
                iter_one_pass(p, rng, rel_temp_i, stepfunc_par_i, max_exp);
            count += 1;


            char buff[128];  // the prefix
            snprintf(buff, 127, "%s%d.%d:  ", session_name, i+1, j+1);
            //
            printf("\n");
            printf("    ====> Step: %s\n", buff);
            printf("          -> rel_temp= %f\n", rel_temp_i);
            printf("          -> penalty_i= %d (each cell)\n", penalty_i);
            printf("          -> stepfunc_par= %f\n", stepfunc_par_i);

            char buff_csv[128];  // the prefix
            snprintf(buff_csv, 127, "%s-CSV1  ", session_name);

            prb_print_cost(p, buff);
            prb_print_cost_csv(p, buff_csv, i, j);
            prb_print_linehaul_stats(p, buff); 
            prb_print_real2max_stats(p, buff);

            prb_print_cost_csv(p, buff, i, j);
 
        }; // for(int j=0; j<nrep; j++)

    }; // for(int i=0; i<nstep; i++)
};


//--------------------------------------------------------------
void iter_one_pass(
    prb* p, gsl_rng* rng, double rel_temp, double stepfunc_par, double max_exp)
//--------------------------------------------------------------
{
    fflush(stdout);
    assert(prb_is_initialized(p));
  
    int nrows = p->nrows;

    //   Allocate, fill, and shuffle an array of indexes: 0,...(nrows-1):
    int* row_idx = (int*)malloc(sizeof(int) * nrows);
    assert(row_idx != NULL);
    //
    for(int i=0; i<nrows; i++)
        row_idx[i] = i;
    //
    gsl_ran_shuffle (rng, (void*)row_idx, nrows, sizeof(int));

    for(int ii=0; ii<nrows; ii++)
    {
        int i = row_idx[ii];  // select a row randomly 

        // If the row is set, clear it (a precondition for prb_make_choice_j)
        if(prb_row_is_set(p, i))
            prb_clear_row(p, i);

        int j0 = prb_make_choice_j(p, i, rel_temp, stepfunc_par, rng, max_exp);
        prb_set_row(p, i, j0);
    }; // for(int ii=0; ii<nrows; ii++)

    free(row_idx); 
    return;
};

