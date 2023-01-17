#include <stdio.h> 
#include <assert.h> 
#include <math.h> 

#include "rng.h"  // in libvk2
#include "ran_discrete.h"  // in libvk2

#include "prb.h" 
#include "funcs.h" 

static double* prb_choice_probs_new(
    const prb* p, int i, double rel_temp, double step_func_par, gsl_rng* rng,
    double* delta_cost_by_col, double max_exp);

//============================================================================
int prb_make_choice_j(
    const prb* p, int i, double rel_temp, double step_func_par, 
    gsl_rng* rng, double max_exp)
//============================================================================
{
    // only for high enough temperature
    assert( ! prb_row_is_set(p, i) );

    int ncols = p->ncols;
    int debug_flag = 0;

    double* delta_cost_by_col = NULL;
    if(debug_flag){
        delta_cost_by_col = (double*)malloc(ncols * sizeof(double));  
        assert(delta_cost_by_col != NULL);
    };

    assert(p->init_key == PRB_INIT_KEY); // is initialized
    assert(rng != NULL);

    double* prob = prb_choice_probs_new(
        p, i, rel_temp, step_func_par, rng, delta_cost_by_col, max_exp);

    if(debug_flag)
    {
        assert(delta_cost_by_col != NULL);
        int jj;
        for(jj=0; jj<ncols; jj++)
        {
            printf(
                "   --> i=%d   jj= %3d  dcost= %f  p= %f (%e)\n", 
                i, jj, delta_cost_by_col[jj], prob[jj], prob[jj]
            );
        }
    };
     

    //  Choose j (column index according to the probabilities)  
    rdtab rdt = ran_discrete_rdtab_getnew(ncols, prob);
    int j0new = ran_discrete(rng, rdt);

    int count = 0;
    while( ! prb_is_valid_ij(p, i, j0new) )
    {
        fprintf(stderr, " ==> WARNING: a very rare event 1:\n");
        fprintf(stderr, "  -> i= %d\n", i);
        fprintf(stderr, "  -> j0new= %d\n", j0new);
        j0new = ran_discrete(rng, rdt);
       
        count += 1;
        if(count > 10)
        {
            fprintf(stderr, "Exit\n");
            exit(1);
        };
    }
    ran_discrete_rdtab_release(rdt);

    free(prob);

    if(delta_cost_by_col != NULL) {   
        assert(debug_flag);
        free(delta_cost_by_col);
    };
    return j0new;
};


//********************************************
//   Static functions:
//********************************************

static void process_w_for_zero_temp(double* w, int ncols);

static void process_w_for_nonzero_temp(
    double* w, int ncols, double rel_temp, double max_exp);

// todo: a separate branch for rel_temp < 0.01
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
double* prb_choice_probs_new(
    const prb* p, int i, double rel_temp, double step_func_par, gsl_rng* rng,
    double* delta_cost_by_col, double max_exp)  
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
    // 1. The returned value is created by malloc(), use free() to free
    // 2. delta_cost_by_col is optional, if not NULL, should be at least [ncols]

    assert( ! prb_row_is_set(p, i) );
    assert(p->init_key == PRB_INIT_KEY); // is initialized
    assert(rng != NULL);
    assert(rel_temp >= 0.);

    int ncols = p->ncols;

    double* w = (double*)malloc(ncols * sizeof(double));
    assert(w != NULL);


    int v = arr1d_getval(&(p->row_volume), i);
    assert(v > 0); // vk_check

    // Fill w[j] by delta-cost or -1 (if the cell is not valid):
    double s = 0.;
    double count = 0;
    for(int j=0; j<ncols; j++){
        if(prb_is_valid_ij(p, i, j)) 
        {
            int d_penalty = prb_penalty_delta(p, i, j, v);
            int d_var_cost = prb_var_cost_delta(p, i, j, v);
            int d_lh_cost = prb_linehaul_cost_delta(p, i, j, v, step_func_par);

            int d_cost = d_penalty + d_var_cost + d_lh_cost;
            assert(d_cost >= 0);

            w[j] = (double)d_cost;

            s += (double)d_cost;
            count += 1;
        } 
        else 
        {
            w[j] = -1.;  // negative values are in non-valid cells
        }

        if(delta_cost_by_col != NULL)
            delta_cost_by_col[j] = w[j];
    }; 
    assert(count > 0); // at least one cell is valid in the row


    if(rel_temp > 0.)
        process_w_for_nonzero_temp(w, ncols, rel_temp, max_exp);
    else
        process_w_for_zero_temp(w, ncols);

    // Normalize the weights to make them probabilities:
    double sw = 0.;
    for(int j=0; j<ncols; j++)
    {
        sw += w[j];
    };
    for(int j=0; j<ncols; j++)
    {
        w[j] /= sw;
    };


    return w;
};


//  *** Static functions of 2nd level


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void process_w_for_zero_temp(double* w, int ncols)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
    // Find the minimal value
    double wmin;
    int count = 0;
    for(int j=0; j<ncols; j++)
        if(w[j] >= 0.) 
        {
           if(count == 0)
               wmin = w[j];
           else
               if(w[j] < wmin)
                   wmin = w[j];
           count += 1;
        }
    assert(count > 0);

    //  Leave the mininal value(s) as 1.0, replace others by 0.0 
    count = 0;
    for(int j=0; j<ncols; j++)
        if(w[j] >= 0.) 
        {
            if(w[j] == wmin)
                w[j] = 1.0;
            else
                w[j] = 0.0;
        }
        else
        {
            w[j] = 0.0;  // zero prob-weight for non-available options
        }

//    // Normalize w[j]
//    double wsum = 0.;
//    for(j=0; j<ncols; j++)
//        wsum += w[j];
//    for(j=0; j<ncols; j++)
//        w[j] /= wsum;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void process_w_for_nonzero_temp(
    double* w, int ncols, double rel_temp, double max_exp)
//    int debug_flag)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
    // Calculate the mean
    double s = 0.;
    int count = 0;
    for(int j=0; j<ncols; j++)
        if(w[j] >= 0)
        {
            s += w[j];
            count += 1;
        };
    double mean = s / count;

    // Calculate STD
    double s2 = 0.;
    for(int j=0; j<ncols; j++)
    {
        if(w[j] >= 0)
            s2 += pow(abs(w[j] - mean), 2); 
    }
    double std = sqrt(s2 / count);

    double eps = 0.001;
    if(std > eps)   // more than "eps" cent (non-zero difference)
    {
        // normalize to STD
        for(int j=0; j<ncols; j++)
            if(w[j] >= 0) 
                w[j] /= std;

        // find the minimal value 
        int count = 0;
        double wmin;
        for(int j=0; j<ncols; j++)
            if(w[j] >= 0) 
            {
                if(count == 0)
                    wmin = w[j];
                else
                {
                    if(w[j] < wmin)
                        wmin = w[j];
                }
                
                count += 1;
            }
        assert(count > 0);

        // 0. Subtract the minimal value
        // 1. Replace all values > max_exp by 0.0 
        //    (too high "energy" => zero prob) 
        // 2. Replace w[j] <= max_exp by exponents
        //    (to avoid numerical errors in exp())
        for(int j=0; j<ncols; j++)
            if(w[j] >= 0) 
            {
                w[j] = (w[j] - wmin) / rel_temp;
                if(w[j] <= max_exp)
                    w[j] = exp(-w[j]);
                else
                    w[j] = 0.0;   // too high cost to be exponentiated`
            }
            else
                w[j] = 0.0; // invalid options

    }
    else   // zero std  (all probabilities are equal)
    {
        double prob = 1./count;
        for(int j=0; j<ncols; j++)
        {
            if(w[j] >= 0.0)
                w[j] = prob;
            else
                w[j] = 0.0;  // invalid j
        }
    }

}


