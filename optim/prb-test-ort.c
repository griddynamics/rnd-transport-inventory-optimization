#include <stdio.h>
#include <assert.h>
#include <math.h>  // pow(),

#include "prb_run.h"

#include "rng.h"
#include "prb.h"
#include "funcs.h"
#include "ortools.h"

//#define NO_CONSTRAINTS

int main()
{
    double max_exp = 30.;
    int linehaul_flag = 0;  // if not set, no linehaul

    long seed0 = 1111 + 1;  // For generation of the dataset only 
    long seed = 12342+6;  // For optimization

    printf("FILE: prb-test-ort.c\n");
    printf("  -> seed0= %ld [for dataset generation]\n", seed0);
    printf("  -> seed= %ld [for optimization]\n", seed);
    printf("\n");

    gsl_rng* rng = rng_create(seed); // this rng is only for optimization


    int ndc = 5;
    int ncarriers = 2;
    int nroutes = 1;
    int nrows = 100;
    int ndays = 7;

    /*  // OR-Tools fail for these data  dim=3000: 
        // RecursionError: maximum recursion depth exceeded during compilation
        // dim = 2000 is OK
    int ndc = 5;
    int ncarriers = 4;
    int nroutes = 1;
    int nrows = 150;
    int ndays = 7;
    */

    double max_constraints_to_total = 2.0;  // 2.6->both fail  2.7->both OK
    double cost_per_truck_mult = 0.5;

    printf(" ====> Model structure parameters:\n");
    printf("    -> ndc= %d\n", ndc);
    printf("    -> ncarriers= %d\n", ncarriers);
    printf("    -> nroutes= %d\n", nroutes);
    printf("    -> nrows = %d\n", nrows);
    printf("    -> ndays= %d\n", ndays);
    printf(
        "        -> cost_per_truck_mult = %f\n", cost_per_truck_mult);
    printf(
        "        -> max_constraints_to_total= %f\n", max_constraints_to_total);
    printf("\n");

    prb pp = prb_generate(
        seed0, ndc, ncarriers, nroutes, nrows, ndays,  
        max_constraints_to_total, cost_per_truck_mult, linehaul_flag);
    prb* p = &pp;


//    //
//    //  Experiment with a greedy solution
//    //
//
//    // prb_set_var_cost_mult(p, 1);
//    // prb_set_linehaul_cost_mult(p, 1);
//
//    prb_set_penalty_mult_dc_sday(p, 0);
//    prb_set_penalty_mult_carrier_dc_sday(p, 0);
//
//    char* session_name_z = "Z";  // constraints programming
//    int nstep_z = 2;
//    schedule sch_z = {
//        {0.0, 0.0, 1},  // rel_temp
//        {1, 1, 1.0},    // penalty (set here)
//        {1.0, 1.0,  1.0}  // stepfunc_par
//    };
//    run_session(p, sch_z, nstep_z, 1, session_name_z, rng, max_exp);
//    prb_set_penalty_mult_dc_sday(p, 1);
//    prb_set_penalty_mult_carrier_dc_sday(p, 1);
//
////    // 
////    //  A session which starts from the greedy solution from the prev-session
////    //
////    int nstep_zz = 10;
////    char* session_name_zz = "ZZ";
////    schedule sch_zz = {
////        {0.0, 0.0, 1},  // rel_temp
////        {0, 1000, 1.0},    // penalty (set here)
////        {1.0, 1.0,  1.0}  // stepfunc_par
////    };
////    //
////    run_session(p, sch_zz, nstep_zz, 1, session_name_zz, rng, max_exp);
//
//    return 0;
//
////#ifdef NO_CONSTRAINTS
////    prb_set_constraints_off(p);
////    printf(" ==> WARNING: all constraints off\n");
////#endif
//

    //
    //   Generate a py-file for ortools:
    //

    char* filename = "ortools.prg.py";
    FILE* file = fopen(filename, "w");
    assert(file != NULL);
    //
    const char* indent = "    ";
    ort_mk_header(file, "");
    ort_mk_binvars(file, p, indent);
    ort_mk_constr_rowsum(file, p, indent);
    ort_mk_constr_dc_sday(file, p, indent);
    ort_mk_constr_carrier_dc_sday(file, p, indent);
    ort_mk_constr_objective(file, p, indent);
    ort_solution(file, p);
    //
    fclose(file);
    printf(" ==> File created: \"%s\"\n", filename);


    int nstep = 10;
    int nrep = 1;
    schedule sch_a = {
        {1., 0.00, 0.25},  // rel_temp
        {10, 10000, 1.0},    // penalty (set here)
        {1.0, 0.0001, 1.0}  // stepfunc_par
    };
//    sch=sch; // a trick for debugging


    char* session_name = "A";  // constraints programming

    prb_set_var_cost_mult(p, 0);
    prb_set_linehaul_cost_mult(p, 0);
    //
    run_session(p, sch_a, nstep, nrep, session_name, rng, max_exp);

    int nstep_b = 20;
    session_name = "B";
    schedule sch_b = {
        {0.2, 0.00, 1},  // rel_temp
        {1, 1000, 2.0},    // penalty (set here)
        {1.0, 0.0001, 1.0}  // stepfunc_par
    };
    prb_set_var_cost_mult(p, 1);
    prb_set_linehaul_cost_mult(p, 1);
    //
    run_session(p, sch_b, nstep_b, nrep, session_name, rng, max_exp);

    int nstep_c = 20;
    session_name = "C";
    schedule sch_c = {
        {0.05, 0.00, 1},  // rel_temp
        {0, 1000, 2.0},    // penalty (set here)
        {1.0, 0.0001, 1.0}  // stepfunc_par
    };
    //
    run_session(p, sch_c, nstep_c, nrep, session_name, rng, max_exp);

//    int nstep3 = 10;
//    const char* session_name3 = "C";
//    schedule sch3 = {
//        {1, 0.00, 0.25},  // rel_temp
//        {0., 1000, 1},    // penalty (set here)
//        {1.0, 0.0001, 1.0}  // stepfunc_par
//    };
//    //
//    run_session(p, sch3, nstep3, nrep, session_name3, rng, max_exp);


    prb_memclear(p);
    rng_delete(rng); 
    printf(" ==> File created: \"%s\"\n", filename);
};

