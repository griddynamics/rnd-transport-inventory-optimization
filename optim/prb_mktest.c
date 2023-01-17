#include <stdio.h>
#include <assert.h>

#include "prb.h"
#include "funcs.h"

#define STEP_FUNC_PAR 1.0


prb  prb_mktest_empty(int ndc, int ncarriers, int nroutes, int nrows)
{
    int ndays = 7; 
    int ncols = ndc * ncarriers * nroutes;

    int verbose = 1;
    int routes_with_hubs = 3; // each carrier has this number of hubs

    prb p = prb_mknew(nrows, ncols, ndays, ndc, ncarriers, verbose);
    printf(" ==> nrows= %d\n", nrows);
    printf(" ==> ncols= %d\n", ncols);

    int i,j,k;

    for(i=0; i<nrows; i++){
        arr1d_setval(&(p.row_dday),  i,  day(i % p.ndays, p.ndays));
    };

    int col_index = 0;
    for(i=0; i<ndc; i++) 
        for(j=0; j<ncarriers; j++) 
            for(k=0; k<nroutes; k++) 
            {
                int dc_i = i;
                int carrier_j = j;
                arr1d_setval(&(p.dc), col_index, dc_i);
                arr1d_setval(&(p.carrier), col_index, carrier_j);

                if(k < routes_with_hubs){
                    int hub_idx = j * routes_with_hubs + k; 
                    arr1d_setval(&(p.hub), col_index, hub_idx);
                };

                arr1d_setval(&(p.duration), col_index, col_index % 3 + 1);

                // set linehaul parameters
                if(col_index % 2 == 1){
                    arr1d_setval(&(p.linehaul_flag), col_index, 1);

                    for(int d=0; d<p.ndays; d++){
                        int v = 1000 + (d % 3) * 300;
                        int c =  (100 + (d % 4) * 20) * 1000;
                        arr2d_setval(&(p.volume_per_truck), col_index, d, v);
                        arr2d_setval(&(p.cost_per_truck), col_index, d, c);
                    };
                };

                col_index += 1;
            };

    // fill var_cost
    int c = 0;
    for(i=0; i<nrows; i++)
        for(j=0; j<ncols; j++)
        {
            if( c % 5 == 0)
            {
                arr2d_setval(&(p.var_cost), i, j, UNDEF_INT);
            }
            else
            {
                arr2d_setval(&(p.var_cost), i, j, (i*ncols+j) % 10 + 3);
            }

            c += 1;
        };

    // fill all volume values 
    for(i=0; i<p.nrows; i++)
    {
        // set the volume for the given row
        int volume_i = 20 + (i % 17) * 10;
        arr1d_setval(&(p.row_volume), i, volume_i);
    };

    return p;
};



void prb_fill_test(prb* p)
{
    int i;

    for(i=0; i<p->nrows; i++)
    {
        // set an arbitrary cell in the row
        int j0 = i % p->ncols;

        if(prb_is_valid_ij(p, i, j0))
        { 
            prb_set_row(p, i, j0); 
        }
        else
        {
           // find any another "j"
           int found = 0;
           int j;
           for(j=0; j<p->ncols; j++){
               if(prb_is_valid_ij(p, i, j)){
                   prb_set_row(p, i, j); 
                   found = 1;
                   break; 
               };
           };
           found=found;
           assert(found);
        };
    };
};


