#include <stdio.h> 
#include <assert.h> 

#include "prb.h" 
#include "funcs.h" 

static int calc_penalty(
    int v_real, int v_min, int v_max, int penalty_per_volume);

static int total_penalty2d(
        const arr2d* aa_min, 
        const arr2d* aa_max, 
        const arr2d* aa_real, 
        const arr2d* aa_penalty);

static int total_penalty3d(
        const arr3d* aaa_min, 
        const arr3d* aaa_max, 
        const arr3d* aaa_real, 
        const arr3d* aaa_penalty);


int prb_penalty_delta(const prb* p, int i, int j, int delta_volume) 
{
    assert(p->init_key == PRB_INIT_KEY); // is initialized
    assert(i >= 0  &&  i < p->nrows);
    assert(j >= 0  &&  j < p->ncols);

    // Get DC index:
    int dc_j = arr1d_getval(&(p->dc), j);
    assert(dc_j >= 0);  // is defined

    // Get dday and sday for given "i":
    int dday = arr1d_getval(&(p->row_dday), i); // the delivery day
    assert(dday >=0 && dday < p->ndays);
    int duration = arr1d_getval(&(p->duration), j);
    int sday = day(dday - duration, p->ndays);

    int penalty0 = 0;
    int penalty1 = 0;

    //  Penalty for DC+sday
    int v_real = arr2d_getval(&(p->c_real.dc_sday), dc_j, sday);
    int penalty_per_volume = arr2d_getval(&(p->c_penalty.dc_sday), dc_j, sday);
    int v_min = arr2d_getval(&(p->c_min.dc_sday), dc_j, sday);
    int v_max = arr2d_getval(&(p->c_max.dc_sday), dc_j, sday);
    //
    penalty0 += 
        calc_penalty(v_real, v_min, v_max, penalty_per_volume)
        * p->penalty_mult_dc_sday;
    penalty1 += calc_penalty(
        v_real + delta_volume, v_min, v_max, penalty_per_volume)
        * p->penalty_mult_dc_sday;

    // Penalty for hub_dday:
    int hub_j = arr1d_getval(&(p->hub), j);
    if(hub_j >= 0){   // if a hub exists for the j-th lane
        v_real = arr2d_getval(&(p->c_real.hub_dday), hub_j, dday);
        penalty_per_volume=arr2d_getval(&(p->c_penalty.hub_dday), hub_j, dday);
        v_min = arr2d_getval(&(p->c_min.hub_dday), hub_j, dday);
        v_max = arr2d_getval(&(p->c_max.hub_dday), hub_j, dday);
        //
        penalty0 += 
            calc_penalty(v_real, v_min, v_max, penalty_per_volume)
            * p->penalty_mult_hub_dday;
        penalty1 += calc_penalty(
            v_real + delta_volume, v_min, v_max, penalty_per_volume)
            * p->penalty_mult_hub_dday;
        }
    

    // Penalty for carrier+DC+sday:
    int carrier_j = arr1d_getval(&(p->carrier), j); 
    assert(carrier_j >= 0);
    v_real = arr3d_getval(&(p->c_real.carrier_dc_sday), carrier_j, dc_j, sday);
    penalty_per_volume = arr3d_getval(
        &(p->c_penalty.carrier_dc_sday), carrier_j, dc_j, sday);
    v_min = arr3d_getval(&(p->c_min.carrier_dc_sday), carrier_j, dc_j, sday);
    v_max = arr3d_getval(&(p->c_max.carrier_dc_sday), carrier_j, dc_j, sday);
    //
    penalty0 += calc_penalty(
        v_real, v_min, v_max, penalty_per_volume)
        * p->penalty_mult_carrier_dc_sday;
    //
    penalty1 += calc_penalty(
        v_real + delta_volume, v_min, v_max, penalty_per_volume)
        * p->penalty_mult_carrier_dc_sday;

    return penalty1 - penalty0;
};

long int prb_penalty_total(const prb* p){
    long total_penalty = 0;
    assert(p->init_key == PRB_INIT_KEY);

    // Add penalty for DC+sday:
    total_penalty += total_penalty2d(
        &(p->c_min.dc_sday), 
        &(p->c_max.dc_sday), 
        &(p->c_real.dc_sday), 
        &(p->c_penalty.dc_sday) 
    ) * p->penalty_mult_dc_sday;

    // Add penalty for hub+dday:
    total_penalty += total_penalty2d(
        &(p->c_min.hub_dday), 
        &(p->c_max.hub_dday), 
        &(p->c_real.hub_dday), 
        &(p->c_penalty.hub_dday) 
    ) * p->penalty_mult_hub_dday;

    // Add penalty for carrier+DC+sday:
    total_penalty += total_penalty3d(
        &(p->c_min.carrier_dc_sday), 
        &(p->c_max.carrier_dc_sday), 
        &(p->c_real.carrier_dc_sday), 
        &(p->c_penalty.carrier_dc_sday) 
    ) * p->penalty_mult_carrier_dc_sday;

    return total_penalty;
};

//
//  Static functions
//


int calc_penalty(int v_real, int v_min, int v_max, int penalty_per_volume){
    assert(v_real >= 0);
    int retv = 0;

    if(v_max >= 0 && v_real > v_max){
        retv += penalty_per_volume * (v_real - v_max);
    }
    else {
        if(v_min >= 0 && v_real < v_min){
            retv += penalty_per_volume * (v_min - v_real);
        }
    };
    
    return retv;
};


int total_penalty2d(
        const arr2d* aa_min, 
        const arr2d* aa_max, 
        const arr2d* aa_real, 
        const arr2d* aa_penalty) {

    int size1 = aa_min->size1;
    int size2 = aa_min->size2;
    int i,j;

    int  total_penalty = 0;
    for(i=0; i<size1; i++)
        for(j=0; j<size2; j++) {
            int v_min = arr2d_getval(aa_min, i, j);
            int v_max = arr2d_getval(aa_max, i, j);
            int v_real = arr2d_getval(aa_real, i, j);
            int penalty_per_volume = arr2d_getval(aa_penalty, i, j);
            //
            total_penalty += calc_penalty(
                v_real, v_min, v_max, penalty_per_volume);
        };

    return total_penalty;
};

int total_penalty3d(
        const arr3d* aaa_min, 
        const arr3d* aaa_max, 
        const arr3d* aaa_real, 
        const arr3d* aaa_penalty) {

    int size1 = aaa_min->size1;
    int size2 = aaa_min->size2;
    int size3 = aaa_min->size3;
    int i,j,k;

    int total_penalty = 0;
    for(i=0; i<size1; i++)
        for(j=0; j<size2; j++) 
            for(k=0; k<size3; k++) {
                int v_min = arr3d_getval(aaa_min, i, j, k);
                int v_max = arr3d_getval(aaa_max, i, j, k);
                int v_real = arr3d_getval(aaa_real, i, j, k);
                int penalty_per_volume = arr3d_getval(aaa_penalty, i, j, k);
                //
                total_penalty += calc_penalty(
                    v_real, v_min, v_max, penalty_per_volume);
            };

    return total_penalty;
};


