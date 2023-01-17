#include <stdio.h> 
#include <assert.h> 
#include <math.h> 

#include "prb.h" 
#include "funcs.h" 

#define QVALUES {0.01, 0.1, 0.25, 0.5, 0.75, 0.90, 0.99}
#define NQ 7 

static int calc_linehaul_cost(
        int volume, int truck_cost, int truck_volume, 
        double step_func_par); 


int prb_linehaul_cost_delta(
        const prb* p, int i, int j, int delta_volume, double step_func_par)
{
    assert(p->init_key == PRB_INIT_KEY); // is initialized
    assert(i >= 0  &&  i < p->nrows);
    assert(j >= 0  &&  j < p->ncols);

    int dday = arr1d_getval(&(p->row_dday), i); // the delivery day
    assert(dday >= 0  &&  dday < p->ndays);

    if(arr1d_getval(&(p->linehaul_flag), j) == 0) 
        return 0;

    int truck_cost = arr2d_getval(&(p->cost_per_truck), j, dday);
    assert(truck_cost > 0);

    int truck_volume = arr2d_getval(&(p->volume_per_truck), j, dday);
    assert(truck_volume > 0);

    int volume_ij = arr2d_getval(&(p->linehaul_volume), j, dday);

    int cost0 = calc_linehaul_cost(
        volume_ij, truck_cost, truck_volume, step_func_par); 
    int cost1 = calc_linehaul_cost(
        volume_ij+delta_volume, truck_cost, truck_volume, step_func_par); 

    return (cost1 - cost0) * p->linehaul_cost_mult;
};


long prb_total_linehaul_cost(const prb* p, double step_func_par)
{
    assert(p->init_key == PRB_INIT_KEY); // is initialized

    // int nrows = p->nrows;
    int ncols = p->ncols;
    int ndays = p->ndays;

    long total = 0;

    int j;
    int k;
    for(j=0; j<ncols; j++) {
        if(arr1d_getval(&(p->linehaul_flag), j) == 0)
            // this lane is not a linehaul one
            continue;
        for(k=0; k<ndays; k++)
        {
            double volume = arr2d_getval(&(p->linehaul_volume), j, k); 
            double truck_cost = arr2d_getval(&(p->cost_per_truck), j, k); 
            double truck_volume = arr2d_getval(&(p->volume_per_truck), j, k); 

            assert(volume >= 0);
            assert(truck_cost > 0);
            assert(truck_volume > 0); // vk_check

            if(volume == 0) continue;

            total += calc_linehaul_cost(
                volume, truck_cost, truck_volume, step_func_par);
        };
    };

    return total * p->linehaul_cost_mult;
};


//
//  Static functions
//

int calc_linehaul_cost(
        int volume, int truck_cost, int truck_volume, 
        double step_func_par) {

    int retv = 0;

    int n_trucks_min = volume / truck_volume; 
    retv += n_trucks_min * truck_cost;  // the cost full trucks (the min.)

    // Do we have some boxes remaining?
    int boxes_remain = volume % truck_volume; 
    if(boxes_remain > 0){  // we need one more truck
        // Yes, we should take the remaining boxes into account:
        double truck_frac = (double)boxes_remain / truck_volume;
        double frac_cost = step_func_universal(truck_frac, step_func_par);
        retv += (int)(round(frac_cost * truck_cost));
    };

    return retv;
};

static void print_arr2d_stats(
    const arr2d* aa, const char* title, const char* prefix)
{
    //  Report statistics for linehaul_volume
    double q[NQ] = QVALUES;
    double quantiles[NQ];

    double frac_defined = arr2d_calc_quantiles(aa, q, NQ, quantiles);
    long sum = arr2d_sum(aa, NULL);
    printf("%s  => %s\n", prefix, title);
    printf(
        "%sn= %d (=%d x %d)  sum= %ld  frac_defined= %f\n", 
        prefix, aa->size1 * aa->size2, aa->size1, aa->size2, sum, frac_defined);

 
    const char* format = "FORMAT NOT SET";
    if(quantiles[NQ/2] < 10)
        format = "%.4f  ";
    else
        format = "%.2f  ";
    //    
    printf("%s     QQ: ", prefix);
    for(int l=0; l<NQ; l++)
        printf(format, quantiles[l]);
    printf("\n");
};


void prb_print_linehaul_stats(const prb* p, const char* prefix) 
{
    assert(prb_is_initialized(p));
    int ncols = p->ncols;
    printf("%s\n", prefix);
    printf("%s===> Linehaul: ---\n", prefix);

    // Count the number of columns with the linehaul_flag set:
    int col_count = 0;
    for(int j=0; j<ncols; j++)
        if( arr1d_getval(&(p->linehaul_flag), j) )
            col_count += 1;

    if(col_count == 0)
    {
        printf("%s  no linehaul columns\n", prefix);
        return;
    }

    //  Report (the number of columns with the linehaul_flag set):
    printf(
        "%sncols= %d  ncols_linehaul= %d  (%.1f%%)\n", 
        prefix, ncols, col_count, (double)col_count/ncols * 100);

    //
    // Report on linehaul_volume 
    //

    // Prepare arrays with only "col_count" rows 
    //    (to use proper stats functions for arr2d)
    arr2d linehaul_volume =  arr2d_mknew(col_count, p->ndays, UNDEF_INT); 
    arr2d full_trucks =  arr2d_mknew(col_count, p->ndays, UNDEF_INT); 
    arr2d frac_remain =  arr2d_mknew(col_count, p->ndays, UNDEF_INT); 
            // frac_remain is in 0.0001 fraction units 

    int jj = 0;
    for(int j=0; j<ncols; j++)
    {
        if( arr1d_getval(&(p->linehaul_flag), j) )
        {
            for(int d=0; d<p->ndays; d++)
                {
                    int volume = arr2d_getval(&(p->linehaul_volume), j, d);
                    arr2d_setval(&linehaul_volume, jj, d, volume);

                    int truck_volume = 
                        arr2d_getval(&(p->volume_per_truck), j, d);
                    
                    int nfull_trucks = volume / truck_volume;
                    arr2d_setval(&full_trucks, jj, d, nfull_trucks);

                    int boxes_remain = volume % truck_volume;
                    if(nfull_trucks == 0  &&  boxes_remain == 0)
                        // We do not count such cases 
                        continue;

                    // the fraction remained:
                    double fraction = (double) boxes_remain / truck_volume; 
                    if(boxes_remain > 0)  
                        // we count only non-fully-loaded trucks here
                        // warning: we record remain-fractions in 0.0001 units
                        arr2d_setval(
                            &frac_remain, jj, d, (int)(round(fraction * 10000))
                        );
                };
            jj += 1;

        }; // if( arr1d_getval(&(p->linehaul_flag), j) )

    }; // for(int j=0; j<ncols; j++)

    print_arr2d_stats(&linehaul_volume, " Linehaul_volume:", prefix);
    print_arr2d_stats(&full_trucks, " Full_trucks", prefix);
    print_arr2d_stats(&frac_remain, " Remain(>0) in 10^-4", prefix);

    // print remain values with CSV2 prefix
    int count = 0;
    for(int i=0; i<frac_remain.size1; i++)
        for(int j=0; j<frac_remain.size2; j++)
        {
            int frac_ij = arr2d_getval(&frac_remain, i, j);
            printf("%s-CSV2 %d,%4f\n", prefix, count, (double)frac_ij * 0.0001);
            count += 1;
        };

    arr2d_memclear(&linehaul_volume);
    arr2d_memclear(&full_trucks);
    arr2d_memclear(&frac_remain);

};




