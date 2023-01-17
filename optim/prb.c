#include <stdio.h> 
#include <assert.h> 
#include <math.h> // pow 

#include "prb.h"
#include "funcs.h"

#define INIT_CONSTR_MIN -1  // UNDEF_INT
#define INIT_CONSTR_MAX -1  // UNDEF_INT
#define INIT_CONSTR_PENALTY 0  // (in cents)

static constraints constraints_mknew(
        int ndc, int ncarriers, int ncols, int ndays, 
        int init_value, int init_value_hub_dday);

static void constraints_memclear(constraints* c);

static void prb_set_or_clear_row(prb* p, int i, int j);


prb prb_mknew(
        int nrows, int ncols, int ndays, 
        int ndc, int ncarriers, int verbose){

    prb r;  // the struct to be returned
    r.init_key = PRB_INIT_KEY;
    r.nrows = nrows;
    r.ncols = ncols;
    r.ndays = ndays;
    r.ndc = ndc;
    r.ncarriers = ncarriers;

    // Init. row-related arrays:
    r.row_dday = arr1d_mknew(nrows, UNDEF_INT);
    r.row_zipcode = arr1d_mknew(nrows, UNDEF_INT);
    //    init. empty solution: no boxes (volumes) added
    r.row_volume = arr1d_mknew(nrows, 0);
    r.row_colindex = arr1d_mknew(nrows, UNDEF_INT); // the column index 

    // Init. column-related arrays:
    r.dc = arr1d_mknew(ncols, UNDEF_INT);
    r.carrier = arr1d_mknew(ncols, UNDEF_INT);
    r.hub = arr1d_mknew(ncols, UNDEF_INT);
    r.duration = arr1d_mknew(ncols, UNDEF_INT);


    // Init. linehaul parameters (UNDEF_INT means "no linehaul" for given col.)
    r.linehaul_flag = arr1d_mknew(r.ncols, 0);  // default = "Direct"
    r.cost_per_truck = arr2d_mknew(r.ncols, r.ndays, UNDEF_INT);
    r.volume_per_truck = arr2d_mknew(r.ncols, r.ndays, UNDEF_INT);

    // Init. linehaul volume counters:
    r.linehaul_volume = arr2d_mknew(ncols, ndays, 0);

    // Init. volume counters for constraints
    r.c_min = constraints_mknew(
        ndc, ncarriers, ncols, ndays, INIT_CONSTR_MIN, -1);
    r.c_max = constraints_mknew(
        ndc, ncarriers, ncols, ndays, INIT_CONSTR_MAX, -1);
    r.c_real = constraints_mknew(
        ndc, ncarriers, ncols, ndays, 0, 0);
    r.c_penalty = constraints_mknew(
        ndc, ncarriers, ncols, ndays, INIT_CONSTR_PENALTY, 0);

    // Init. the variable cost
    r.var_cost = arr2d_mknew(nrows, ncols, UNDEF_INT); // const

    return r;
};

void prb_memclear(prb* p)
{
    assert(prb_is_initialized(p));

    arr1d_memclear(&(p->row_dday));
    arr1d_memclear(&(p->row_zipcode));
    arr1d_memclear(&(p->row_volume));
    arr1d_memclear(&(p->row_colindex));

    arr1d_memclear(&(p->dc));
    arr1d_memclear(&(p->carrier));
    arr1d_memclear(&(p->hub));
    arr1d_memclear(&(p->duration));
    arr1d_memclear(&(p->linehaul_flag));
    //
    arr2d_memclear(&(p->cost_per_truck));
    arr2d_memclear(&(p->volume_per_truck));

    arr2d_memclear(&(p->linehaul_volume));

    constraints_memclear(&(p->c_min));
    constraints_memclear(&(p->c_max));
    constraints_memclear(&(p->c_real));
    constraints_memclear(&(p->c_penalty));

    arr2d_memclear(&(p->var_cost));

};

void prb_set_penalty(prb* p, int value)
{
    //static void constraints_memclear(constraints* c);
    
    constraints* c = &(p->c_penalty);
    constraints_memclear(c);
    *c = constraints_mknew(
        p->ndc, p->ncarriers, p->ncols, p->ndays, value, 0);
};

int prb_is_initialized(const prb* p)
{
    return p->init_key == PRB_INIT_KEY;
}


////void prb_print_stats(const prb* p, const char* prefix)
//void prb_print_stats(const prb* p, const char* prefix)
//{
//    // Linehaul stats
//
//
//};
//

static void prb_print_cost_(const prb* p, const char* prefix, int i, int j)
{
    long penalty = prb_penalty_total(p);
    long var_cost = prb_total_var_cost(p);
    long linehaul_cost = prb_total_linehaul_cost(p, 1.0);
    long linehaul_cost_true = prb_total_linehaul_cost(p, 0.);

    long total_true_cost = var_cost + linehaul_cost_true;

    double div = 1000. * 1000 * 100; // 100 mln cents = 1mln doll
    if(i < 0)
    {
        printf("%s ===> Costs: ---\n", prefix);
        printf(
            "%spenalty= %ld   (%f mln.usd)\n", 
            prefix, penalty, (double)penalty/div);
        printf(
            "%svar_cost= %ld  (%f mln.usd)\n", 
            prefix, var_cost, (double)var_cost/div);
        printf(
            "%slinehaul_cost= %ld  (%f mln.usd)\n", 
            prefix, linehaul_cost, (double)linehaul_cost/div);
        printf(
            "%slinehaul_cost_true= %ld  (%f mln.usd)\n", 
            prefix, linehaul_cost_true, (double)linehaul_cost_true/div);

        printf(
            "%s -> Total_true_cost (var+lh)= %ld  (%f mln.)\n", 
            prefix, total_true_cost, (double)total_true_cost/div);
        }
    else
    {
        printf("%s%d,%d,%ld,%ld\n", prefix, i, j, total_true_cost, penalty);
    };
}

void prb_print_cost(const prb* p, const char* prefix)
{
    prb_print_cost_(p, prefix, -1, -1);

}

void prb_print_cost_csv(const prb* p, const char* prefix, int i, int j)
{
    prb_print_cost_(p, prefix, i, j);
}


void prb_print_real2max_stats(const prb* p, const char* prefix)
{
    assert(prb_is_initialized(p));
    const constraints* c1 = &(p->c_real);
    const constraints* c2 = &(p->c_max);

    const arr2d* aa1 = NULL;
    const arr2d* aa2 = NULL;
    const arr3d* aaa1 = NULL;
    const arr3d* aaa2 = NULL;

    // DC + ShipmentDay:
    aa1 = &(c1->dc_sday);
    aa2 = &(c2->dc_sday);
    printf("%s\n", prefix);
    printf("%s==> ratio real/max for DC_SDAY: ---\n", prefix);
    arr2d_print_ratio_stats(aa1, aa2, prefix);

// todo:
// Hub load calculation does not work properly:
//    // Hub + DeliveryDay: 
//    aa1 = &(c1->hub_dday);
//    aa2 = &(c2->hub_dday);
//    printf("%s\n", prefix);
//    printf("%s==> ratio real/max for HUB_DDAY: ---\n", prefix);
//    arr2d_print_ratio_stats(aa1, aa2, prefix);

    // Carrier + DC + ShipmentDay: 
    aaa1 = &(c1->carrier_dc_sday);
    aaa2 = &(c2->carrier_dc_sday);
    printf("%s\n", prefix);
    printf("%s==> ratio real/max for CARRIER_DC_SDAY: ---\n", prefix);
    arr3d_print_ratio_stats(aaa1, aaa2, prefix);

};


int prb_is_valid_ij(const prb* p, int i, int j){
    assert(p->init_key == PRB_INIT_KEY); // is initialized
    assert(i >= 0  &&  i < p->nrows);
    assert(j >= 0  &&  j < p->ncols);

    int is_valid_ij = arr2d_getval(&(p->var_cost), i, j) >= 0; 
    return is_valid_ij;
};

int prb_var_cost_delta(const prb* p, int i, int j, int delta_volume) 
{
    assert(p->init_key == PRB_INIT_KEY); // is initialized
    assert(i >= 0  &&  i < p->nrows);
    assert(j >= 0  &&  j < p->ncols);

    // Calculate delta_var_cost:
    int var_cost_per_box = arr2d_getval(&(p->var_cost), i, j);
    assert(var_cost_per_box >= 0);  // the (i,j) is allowed

    return delta_volume * var_cost_per_box * p->var_cost_mult; 
};


int prb_row_is_set(const prb* p, int i)
{
    assert(p->init_key == PRB_INIT_KEY); // is initialized
    assert(i >= 0  &&  i < p->nrows);
    int j = arr1d_getval(&(p->row_colindex), i);
    return j >= 0;
};

void prb_set_row(prb* p, int i, int j)
{
    assert(p->init_key == PRB_INIT_KEY); // is initialized
    assert(j >= 0);
    assert( ! prb_row_is_set(p, i) ); // ensure the row is not set
    prb_set_or_clear_row(p, i, j);
};

void prb_clear_row(prb* p, int i) 
{
    assert(p->init_key == PRB_INIT_KEY); // is initialized
    assert(prb_row_is_set(p, i)); // ensure the row is set
    prb_set_or_clear_row(p, i, UNDEF_INT);
}

int prb_get_volume(const prb* p, int i)
{
    assert(p->init_key == PRB_INIT_KEY); // is initialized
    return arr1d_getval(&(p->row_volume), i); 
};

void prb_change_volume(prb* p, int i, int new_volume)
{
    assert(p->init_key == PRB_INIT_KEY); // is initialized

    // If the row is set, clear it:
    int row_is_set = 0;
    int j0 = arr1d_getval(&(p->row_colindex), i);
    if(prb_row_is_set(p, i)) {
        row_is_set = 1;  // remember that the row was set before the prb_clear_row() call
        prb_clear_row(p, i); // remove the current volume (of this row) first
    }

    // Change the volume for this row
    arr1d_setval(&(p->row_volume), i, new_volume);

    // If the row was initially set, then set it
    if(row_is_set) {
        assert(j0 >= 0);
        prb_set_row(p, i, j0);
    };
}


//long prb_total_var_cost(const prb* p, double step_func_par){
long prb_total_var_cost(const prb* p)
{
    assert(p->init_key == PRB_INIT_KEY); // is initialized

    int nrows = p->nrows;
    //int ncols = p->ncols;
    //int ndays = p->ndays;
    int i;

    // Calculate total var-cost:
    long total = 0;
    for(i=0; i<nrows; i++)
    {    
        int j = arr1d_getval(&(p->row_colindex), i);
        if(j < 0){   // the i-th row is not set yet (no volume to deliver)
            continue; // the row is not set yet
        };
        assert(j < p->ncols);

        int volume_i = arr1d_getval(&(p->row_volume), i);
        if(volume_i > 0){
            int varcost_ij = arr2d_getval(&(p->var_cost), i, j);
            assert(varcost_ij >= 0);  // the i,j is available
            total += (long)volume_i * varcost_ij;
        };
    };

    return total * p->var_cost_mult;
};

void prb_clear_all_rows(prb* p)
{
    assert(prb_is_initialized(p));

    int nrows = p->nrows;
    for(int i=0; i<nrows; i++)
    {
        if(prb_row_is_set(p, i))
            prb_clear_row(p, i);
    };
};


//
//    Static functions
//

constraints constraints_mknew(
        int ndc, int ncarriers, int ncols, int ndays, 
        int init_value, int init_value_hub_dday)
{

    constraints c;
    c.dc_sday = arr2d_mknew(ndc, ndays, init_value);
    // vk_check:
    c.hub_dday = arr2d_mknew(ncols, ndays, init_value_hub_dday);

    c.carrier_dc_sday = arr3d_mknew(ncarriers, ndc, ndays, init_value);
    return c;
};

void constraints_memclear(constraints* c)
{
    arr2d_memclear(&(c->dc_sday));
    arr2d_memclear(&(c->hub_dday));
    arr3d_memclear(&(c->carrier_dc_sday));
};

void prb_set_or_clear_row(prb* p, int i, int j) {
    assert(p->init_key == PRB_INIT_KEY); // is initialized
    assert(i >= 0  &&  i < p->nrows);

    // Get dday and sday for given "i":
    int dday = arr1d_getval(&(p->row_dday), i); // the delivery day
    assert(dday >=0 && dday < p->ndays);


    double delta_volume = arr1d_getval(&(p->row_volume), i);
    assert(delta_volume > 0);


    int j0 = -2;
    if(j >= 0) 
    {   // we set the i,j cell (should not be set in this row)  
        assert( prb_is_valid_ij(p, i, j) );
        arr1d_setval(&(p->row_colindex), i, j);
        j0 = j;
    }
    else  // j < 0 
    {   // we clear the cell (should be set in this row)
        delta_volume = -delta_volume;
        j0 = arr1d_getval(&(p->row_colindex), i);
        arr1d_setval(&(p->row_colindex), i, UNDEF_INT);
    };
    assert(j0 != -2);

    //
    //  Increment (decrement) the counters
    //

    int duration = arr1d_getval(&(p->duration), j0);
    int sday = day(dday - duration, p->ndays);

    // Add to linehaul volumne counters
    if( arr1d_getval(&(p->linehaul_flag), j0) )
        arr2d_addval(&(p->linehaul_volume), j0, dday, delta_volume);

    // Increment DC_SDAY counters:
    int dc_j = arr1d_getval(&(p->dc), j0);
    assert(dc_j >= 0);  // is defined
    arr2d_addval(&(p->c_real.dc_sday), dc_j, sday, delta_volume);

    // Increment HUB_DDAY counters:
    int hub_j = arr1d_getval(&(p->hub), j0);
    if(hub_j >= 0){   // if a hub exists for the j-th lane
        arr2d_addval(&(p->c_real.hub_dday), hub_j, dday, delta_volume);
    }

    // Increment CARRIER_DC_SDAY counters:
    int carrier_j = arr1d_getval(&(p->carrier), j0); 
    assert(carrier_j >= 0);  // is defined
    arr3d_addval(
        &(p->c_real.carrier_dc_sday), carrier_j, dc_j, sday, delta_volume);
};

void prb_print_stats_constraints(
    const prb* p, ConstraintType ct, const char* prefix)
{
    const constraints* c = NULL; 
    const char* suffix = ""; 
    switch(ct)
    {
        case CONSTR_MIN:
            c = &(p->c_min);
            suffix = "min:";
            break;

        case CONSTR_MAX:
            c = &(p->c_max);
            suffix = "max:";
            break;

        case CONSTR_REAL:
            c = &(p->c_real);
            suffix = "real:";
            break;

        case CONSTR_PENALTY:
            c = &(p->c_penalty);
            suffix = "penalty:";
            break;

        default:
            assert(0);
            break;
    };

    const arr2d* aa = NULL;
    const arr3d* aaa = NULL;
    const char* suffix2 = NULL;

    char txt[128];

    suffix2 = "dc_sday";
    aa = &(c->dc_sday);
    printf("\n");
    //
    snprintf(txt, 127, "%s%s%s: ", prefix, suffix, suffix2);
    arr2d_print_stats(aa, txt);

    printf("\n");

    suffix2 = "hub_dday";
    aa = &(c->hub_dday);
    printf("\n");
    //
    snprintf(txt, 127, "%s%s%s: ", prefix, suffix, suffix2);
    arr2d_print_stats(aa, txt);

    suffix2 = "carrier_dc_sday";
    aaa = &(c->carrier_dc_sday);
    printf("\n");
    //
    snprintf(txt, 127, "%s%s%s: ", prefix, suffix, suffix2);
    arr3d_print_stats(aaa, txt);

}


static void set_constraints_off(constraints* c) 
{
    // arr2d_setall(&(c->dc_sday), -1);
    arr2d_setall(&(c->hub_dday), -1);
    arr3d_setall(&(c->carrier_dc_sday), -1);
};

void prb_set_constraints_off(prb* p)
{
    set_constraints_off(&(p->c_min));
    set_constraints_off(&(p->c_max));
};


void save2csv_dc_sday(const prb* p, const char* csv_path)
{
    FILE* file = fopen(csv_path, "w");
    assert(file != NULL);

    const arr2d* aa_min = &(p->c_min.dc_sday);
    const arr2d* aa_max = &(p->c_max.dc_sday);
    const arr2d* aa_real = &(p->c_real.dc_sday);
    const arr2d* aa_penalty = &(p->c_penalty.dc_sday);

    // Print the CSC-file header
    fprintf(
        file, "\"i\",\"j\",\"c_min\",\"c_max\",\"c_real\",\"c_penalty\"\n" );

    for(int i=0; i<aa_min->size1; i++)
        for(int j=0; j<aa_min->size2; j++)
        {
            int c_min_ij = arr2d_getval(aa_min, i, j);
            int c_max_ij = arr2d_getval(aa_max, i, j);
            int c_real_ij = arr2d_getval(aa_real, i, j);
            int c_penalty_ij = arr2d_getval(aa_penalty, i, j);

            fprintf(
                file, 
                "%d,%d,%d,%d,%d,%d\n", 
                i, j, c_min_ij, c_max_ij, c_real_ij, c_penalty_ij);
        };

    fclose(file);
};

void save2csv_carrier_dc_sday(const prb* p, const char* csv_path)
{
    FILE* file = fopen(csv_path, "w");
    assert(file != NULL);

    const arr3d* aa_min = &(p->c_min.carrier_dc_sday);
    const arr3d* aa_max = &(p->c_max.carrier_dc_sday);
    const arr3d* aa_real = &(p->c_real.carrier_dc_sday);
    const arr3d* aa_penalty = &(p->c_penalty.carrier_dc_sday);

    // Print the CSC-file header
    fprintf(
        file, 
        "\"i\",\"j\",\"k\",\"c_min\",\"c_max\",\"c_real\",\"c_penalty\"\n" );

    for(int i=0; i<aa_min->size1; i++)
        for(int j=0; j<aa_min->size2; j++)
            for(int k=0; k<aa_min->size3; k++)
        {
            int c_min_ij = arr3d_getval(aa_min, i, j, k);
            int c_max_ij = arr3d_getval(aa_max, i, j, k);
            int c_real_ij = arr3d_getval(aa_real, i, j, k);
            int c_penalty_ij = arr3d_getval(aa_penalty, i, j, k);

            fprintf(
                file, 
                "%d,%d,%d,%d,%d,%d,%d\n", 
                i, j, k, c_min_ij, c_max_ij, c_real_ij, c_penalty_ij);
        };

    fclose(file);
};


//// Calculate the linehaul cost for given i, j:
//long prb_linehaul_cost(
//        const prb* p, int i, int j, int delta_volume, double step_func_par){ 
//    //
//    assert(p->init_key == PRB_INIT_KEY); // is initialized
//    assert(i >= 0  &&  i < p->nrows);
//    assert(j >= 0  &&  j < p->ncols);
//
//    long retv = 0;
//
//    int dday = arr1d_getval(&(p->row_dday), i); // the delivery day
//    assert(dday >= 0  &&  dday < p->ndays);
//    int cost_per_truck = arr2d_getval(&(p->cost_per_truck), j, dday);
//
//    if ( cost_per_truck > 0){
//        int volume_per_truck = arr2d_getval(&(p->volume_per_truck), j, dday);
//        assert(volume_per_truck > 0);
//
//        int curr_volume = arr2d_getval(&(p->linehaul_volume), j, dday);
//        curr_volume += delta_volume;
//
//        // Calculate the total cost of the trucks necessary:
//        int n_trucks_min = curr_volume / volume_per_truck; 
//        retv += n_trucks_min * cost_per_truck; 
//
//        int boxes_remain = curr_volume % volume_per_truck; 
//        if(boxes_remain > 0){  // we need one more truck
//            double truck_frac = (double)boxes_remain / volume_per_truck;
//            double frac_cost = step_func_universal(truck_frac, step_func_par);
//            retv += (int)(round(frac_cost * cost_per_truck));
//        }
//
//    } // if ( cost_per_truck > 0){
//
//    return retv;
//};
//
