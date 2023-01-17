#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "prb.h"
#include "funcs.h"

#define STEP_FUNC_PAR 1.0


prb  prb_mktest_empty_random(
    int ndc, int ncarriers, int nroutes, int nrows, int ndays, gsl_rng* rng,
    double max_constraints_to_total, double cost_per_truck_mult,
    int linehaul_flag)
{
    // 
//    double max_constraints_to_total = 2.;
    double min_constr = 0.5;
    double max_constr = 1.5;

    // The fractions:
    double frac_linehaul_cols = 0.80;   // the fraction of the columns 
                                        //      with the "linehaul" mode
    double frac_routes_with_hub = 0.9; // the fraction of the routes with a hub 
    //  double frac_varcost_undef = 0.25;    // invalid i,j
    double frac_varcost_undef = 0.;    // invalid i,j

    // The duration of delivery in days:
    int duration_min = 1;
    int duration_max = 4;

    int volume_per_truck_min = 500;
    int volume_per_truck_max = 1000;
    

    // The volume in a row:
    int volume_min = 20;
    int volume_max = 200;

    // The var.cost per box:
    int varcost_min = 5;
    int varcost_max = 15;

    int ncols = ndc * ncarriers * nroutes;

    int verbose = 1;
    int routes_with_hubs = 3; // each carrier has this number of hubs

    // the range for auxiliary costs
    double vmin = 0.5;
    double vmax = 3.0; 

    prb p = prb_mknew(nrows, ncols, ndays, ndc, ncarriers, verbose);
    printf(" ==> nrows= %d\n", nrows);
    printf(" ==> ncols= %d\n", ncols);
    printf(
        " ==> dimensionality= %d  (%f mln)\n\n", 
        ncols * nrows, (double)(ncols * nrows) / 1000000);

    int i,j,k;

    for(i=0; i<nrows; i++){
        arr1d_setval(&(p.row_dday),  i,  day(i % p.ndays, p.ndays));
    };

    // We wish to make uneven distribution of the costs over
    //    DC, hub, carrier (to modulate the varcost)
    

    // DC:
    double* avg_cost_dc = (double*)malloc(sizeof(double) * ndc);
    assert(avg_cost_dc != NULL);
    for(i=0; i<ndc; i++)
        avg_cost_dc[i] = gsl_ran_flat(rng, vmin, vmax);

    // carrier;
    double* avg_cost_carrier = (double*)malloc(sizeof(double) * ncarriers);
    assert(avg_cost_carrier != NULL);
    for(i=0; i<ncarriers; i++)
        avg_cost_carrier[i] = gsl_ran_flat(rng, vmin, vmax);

//    // route;
//    double* avg_cost_route = (double*)malloc(sizeof(double) * nroutes);
//    assert(avg_cost_route != NULL);
//    for(i=0; i<nroutes; i++)
//        avg_cost_route[i] = gsl_ran_flat(rng, vmin, vmax);

    int col_index = 0;
    for(i=0; i<ndc; i++) 
        for(j=0; j<ncarriers; j++) 
            for(k=0; k<nroutes; k++) 
            {
                int dc_i = i;
                int carrier_j = j;
                arr1d_setval(&(p.dc), col_index, dc_i);
                arr1d_setval(&(p.carrier), col_index, carrier_j);

                if(gsl_ran_bernoulli (rng, frac_routes_with_hub))
                {
                    // Assign a hub to the column 
                    //     (with the given probability):
                    int hub_idx = j * routes_with_hubs + k;  
                                        // the k-th hub of the j-th carrier 
                    arr1d_setval(&(p.hub), col_index, hub_idx);
                };

                int duration = gsl_rng_uniform_int(
                    rng, duration_max - duration_min) + duration_min;
                arr1d_setval(&(p.duration), col_index, duration);

                // Set linehaul parameters
                if(linehaul_flag)
                {
                    if(gsl_ran_bernoulli (rng, frac_linehaul_cols))
                    { 
                        arr1d_setval(&(p.linehaul_flag), col_index, 1);

                        for(int d=0; d<p.ndays; d++)
                        {
                            // Truck volume:
                            int v = gsl_rng_uniform_int(
                                rng, 
                                volume_per_truck_max - volume_per_truck_min
                            ) + volume_per_truck_min;
                            
                            // We use varcost * cost_per_truck_mult as the 
                            //     average cost of a box for a full truck
                            int varcost = gsl_rng_uniform_int(
                                rng, varcost_max - varcost_min) + varcost_min;

                            // cost_per_truck:
                            int c = (int)(cost_per_truck_mult * varcost) * v;

                            arr2d_setval(
                                &(p.volume_per_truck), col_index, d, v);

                            arr2d_setval(&(p.cost_per_truck), col_index, d, c);

                        }; // for(int d=0; d<p.ndays; d++)

                    }; // if(gsl_ran_bernoulli (rng, frac_linehaul_cols))
                }
                else
                    arr1d_setval(&(p.linehaul_flag), col_index, 0);

                col_index += 1;
            }; // for(i,j,k ...

    // fill var_cost
    int c = 0;
    for(i=0; i<nrows; i++)
        for(j=0; j<ncols; j++)
        {
            if(gsl_ran_bernoulli(rng, frac_varcost_undef))
            {
                arr2d_setval(&(p.var_cost), i, j, UNDEF_INT);
            }
            else
            {
                int varcost_ij = gsl_rng_uniform_int(
                    rng, varcost_max - varcost_min) + varcost_min;


                // Multiply varcost_ij accoring to DC and carrier 
                //    (for given j-th column)
                int idx_dc = arr1d_getval(&(p.dc), j); 
                int idx_carrier = arr1d_getval(&(p.carrier), j); 
                double mult_ij = 
                    avg_cost_dc[idx_dc] + avg_cost_carrier[idx_carrier];
                //
                varcost_ij = (int)(round(mult_ij * varcost_ij));

                // We use on avarage 1/3 of var-cost for the columns 
                //    with linehaul (to make the linehaul mode competitive):
                if(arr1d_getval(&(p.linehaul_flag), j))
                    varcost_ij /= 3;

                arr2d_setval(&(p.var_cost), i, j, varcost_ij);
            }

            c += 1;
        };


    // fill all volume values 
    for(i=0; i<p.nrows; i++)
    {   // Set the volume for the given row:
        int volume_i = gsl_rng_uniform_int(
            rng, volume_max - volume_min) + volume_min;
        arr1d_setval(&(p.row_volume), i, volume_i);
    };

    //
    // Set max constraints
    //

    double total_volume = (double)arr1d_sum(&(p.row_volume), NULL);
    double total_max_constraints = total_volume * max_constraints_to_total;

    // Set max constraints for dc_sday
    arr2d* aa = &(p.c_max.dc_sday);
    double avg = total_max_constraints / arr2d_datasize(aa);
    for(i=0; i<aa->size1; i++) 
        for(j=0; j<aa->size2; j++) 
        {
           double v_ij = avg * gsl_ran_flat(rng, min_constr, max_constr);
           arr2d_setval(aa, i, j, (int)round(v_ij));
        };


    // Set max constraints for hub_dday 
    aa = &(p.c_max.hub_dday);
    avg = total_max_constraints / arr2d_datasize(aa);
    for(i=0; i<aa->size1; i++) 
        for(j=0; j<aa->size2; j++) 
        {
           double v_ij = avg * gsl_ran_flat(rng, min_constr, max_constr);
           arr2d_setval(aa, i, j, (int)round(v_ij));
        };

    // Set max constraints for carrier_dc_sday: 
    arr3d* aaa = &(p.c_max.carrier_dc_sday);
    avg = total_max_constraints / arr3d_datasize(aaa);
    for(i=0; i<aaa->size1; i++) 
        for(j=0; j<aaa->size2; j++) 
            for(k=0; k<aaa->size3; k++) 
            {
               double v_ijk = avg * gsl_ran_flat(rng, min_constr, max_constr);
               arr3d_setval(aaa, i, j, k, (int)round(v_ijk));
            };


    //
    //  free the locally allocate memory
    //
    free(avg_cost_dc);
    free(avg_cost_carrier);

    return p;
};

void prb_fill_test_random(prb* p, gsl_rng* rng)
{
    int i;

    for(i=0; i<p->nrows; i++)
    {
        // set an arbitrary cell in the row
        int j0 = -2;
        int count = 0;
        do 
        {
            j0 = gsl_rng_uniform_int(rng, p->ncols);
            count += 1;
            assert(count < 100); // a very improbable case
        }   while( ! prb_is_valid_ij(p, i, j0));
        prb_set_row(p, i, j0); 
        assert(j0 >= 0);
    };
};


static int set_max_constraints_from_real(
    int* vmax,  // the maximal constraints
    const int* vreal,  // the real values
    int n,   // the size of vmax[] and real[]
    double set_frac,   // the fraction of ->max values to be set in each table
    double set_mult,   // multiply the real value *set_mult to get the max.
    int use_most_popular,   //if true, then most populated cells are set first
    gsl_rng* rng)
{
    int nset = (int)(set_frac * n);

    int* idx2set = (int*)malloc(sizeof(int) * nset);
    assert(idx2set != NULL);

    int* idx = (int*)malloc(sizeof(int) * n);
    assert(idx != NULL);
    for(int i=0; i<n; i++) 
        idx[i] = i;

    if(use_most_popular)   // sort the indices in idx[] according to the values
    {
        // sort_indices(vmax, idx, n);
        sort_indices(vreal, idx, n);
        //  now idx[] contains the indices sorted in the order of the values
    }
    else  // shuffle the indices in idx[]
    {
        gsl_ran_shuffle (rng, (void*)idx, n, sizeof(int));
    }

    //  Copy "nset" highest indices from idx[] into idx2set[]:
    for(int i=0; i<nset; i++)
        idx2set[i] = idx[n - 1 - i] ;

    // set new values for nset elements in vmax[]
    for(int i=0; i<nset; i++)
    {
        int ii = idx2set[i];
        if(vreal[ii] > 0)
            vmax[ii] = (int)(vreal[ii] * set_mult);
    }

    free(idx2set);
    free(idx);

    return nset;
};


void prb_set_max_constraints_from_real(
    prb* p, 
    double set_frac,   // the fraction of ->max values to be set in each table
    double set_mult,   // multiply the real value *set_mult to get the max.
    int use_most_popular,   //if true, then most populated cells are set first
    gsl_rng* rng)
{
    assert(prb_is_initialized(p));
/*
typedef struct {
   arr2d dc_sday; // [ndc * ndays] 
   arr2d hub_dday; // max: [ncols*ndays]: <0 => no hub cap for the route 
   arr3d carrier_dc_sday; // [ncarriers * ndc * ndays]; ground constraints
} constraints;
*/
    //  for DC+ShipmentDay:
    int* vmax = arr2d_data(&(p->c_max.dc_sday));
    int* vreal = arr2d_data(&(p->c_real.dc_sday));
    int n = arr2d_datasize(&(p->c_max.dc_sday));
    //
    set_max_constraints_from_real(
        vmax, vreal, n, set_frac, set_mult, use_most_popular, rng);

    //  for Hub+DeliveryDay:
    vmax = arr2d_data(&(p->c_max.hub_dday));
    vreal = arr2d_data(&(p->c_real.hub_dday));
    n = arr2d_datasize(&(p->c_max.hub_dday));
    //
    set_max_constraints_from_real(
        vmax, vreal, n, set_frac, set_mult, use_most_popular, rng);

    //  for Carrier+DC+ShipmentDay:
    vmax = arr3d_data(&(p->c_max.carrier_dc_sday));
    vreal = arr3d_data(&(p->c_real.carrier_dc_sday));
    n = arr3d_datasize(&(p->c_max.carrier_dc_sday));
    //
    set_max_constraints_from_real(
        vmax, vreal, n, set_frac, set_mult, use_most_popular, rng);
};

