#ifndef PRB__H
#define PRB__H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h> // NOTE: if missed, bizarre errors are possible

#include "arrays.h"

#define PRB_INIT_KEY 123456

typedef unsigned char byte;
typedef double (*step_func)(double x, double par);

typedef struct {
   arr2d dc_sday; // [ndc * ndays] 
   arr2d hub_dday; // max: [ncols*ndays]: <0 => no hub cap for the route 
   arr3d carrier_dc_sday; // [ncarriers * ndc * ndays]; ground constraints
} constraints;

typedef struct {
    int init_key;  // 
    int nrows;
    int ncols;
    int ndays; 
    int ndc; 
    int ncarriers; 


    // Arrays of size [nrows]: for rows:  dday + zipcode
    arr1d row_dday;  // [nrows]
    arr1d row_zipcode;  // [nrows]   // NOT USED, only malloc-free

    // Current state (solution)  [nrows]:
    arr1d row_volume;    // the volume to be delivered for this row 
    arr1d row_colindex;  // the column index selected; initially UNDEF_INT

    // Arrays of size [ncols]: for delivery lanes:
    arr1d dc;
    arr1d carrier;
    arr1d hub;             // the index of the hub used or UNDEF_INT
    arr1d duration;        // duration of delivery [days]

    // linehaul parameters:  
    arr1d linehaul_flag;    // [ncols]
    arr2d cost_per_truck;  // [ncols][ndays] linehaul cost [cents]or UNDEF_INT
    arr2d volume_per_truck;  // [ncols][ndays] 

    constraints c_min;  // minimal volumes: init by UNDEF_INT 
    constraints c_max;  // maximal volumes: init by UNDEF_INT 
    constraints c_real;  // real volumes: init by 0
    constraints c_penalty;  // penalty per box out-of-interval [cents] 

    arr2d linehaul_volume; // ncols * ndays
    arr2d var_cost;    // nrows * ncols  price in cents


    // Cost multipliers (use 1 value for true cost)
    int var_cost_mult;  //
    int linehaul_cost_mult;

    // Penalty multipliers (use 1 for specified penalty)
    int penalty_mult_dc_sday;
    int penalty_mult_hub_dday;
    int penalty_mult_carrier_dc_sday;


} prb;

typedef struct {
    long var_cost;
    long linehaul_cost;
} cost;

prb prb_mknew(
        int nrows, int ncols, int ndays, 
        int ndc, int ncarriers, int verbose);
void prb_memclear(prb* p);  // free all the alloced pointers within the structs

void prb_set_penalty(prb* p, int value); // set penalty=value in all 3 tables


int prb_is_initialized(const prb* p);
//void prb_print_stats(const prb* p, const char* prefix);
void prb_print_cost(const prb* p, const char* prefix);
void prb_print_cost_csv(const prb* p, const char* prefix, int i, int j);

void prb_print_real2max_stats(const prb* p, const char* prefix);

int prb_is_valid_ij(const prb* p, int i, int j); // this i,j is allowed  


// Compute the delta of the cost-component in response to delta_volume:
//    (be sure that there is at most one non-zero volume on a row)
int prb_penalty_delta(const prb* p, int i, int j, int delta_volume);
int prb_var_cost_delta(const prb* p, int i, int j, int delta_volume);
int prb_linehaul_cost_delta(
        const prb* p, int i, int j, int delta_volume, double step_func_par);

// The major functions for loops:
int prb_row_is_set(const prb* p, int i);
void prb_set_row(prb* p, int i, int j); 
void prb_clear_row(prb* p, int i); 
int prb_get_volume(const prb* p, int i);
void prb_change_volume(prb* p, int i, int new_volume);

// Functions for testing mainly (slow):
long int prb_penalty_total(const prb* p);
long prb_total_var_cost(const prb* p);
long prb_total_linehaul_cost(const prb* p, double step_func_par);
    // if step_func_par == 0.0, then the true step function is calculated

// Returns the index of the column for "i-th" row:
//     rel_temp is the relative temperature
int prb_make_choice_j(
    const prb* p, int i, double rel_temp, double step_func_par, 
    gsl_rng* rng, double max_exp);

// These 2 funcions set/fill in a regular manner:
prb  prb_mktest_empty(int ndc, int ncarriers, int nroutes, int nrows);
void prb_fill_test(prb* p); // an arbitrary fill of columns

// These 2 funcions set/fill in a random manner:
prb  prb_mktest_empty_random(
    int ndc, int ncarriers, int nroutes, int nrows, int ndays, gsl_rng* rng,
    double max_constraints_to_total, double cost_per_truck_mult,
    int linehaul_flag);

void prb_fill_test_random(prb* p, gsl_rng* rng);

//void prb_set_constraints_from_real(
//    prb* p, 
//    double set_frac,   // the fraction of ->max values to be set in each table
//    int use_most_popular);   //if true, then most populated cells are set first
//

void prb_clear_all_rows(prb* p); // just clear all rows

void prb_print_linehaul_stats(const prb* p, const char* prefix); 

// Set constraints->max as = ->real value * set_mult for "set_frac"
//     of values (most popular or randomly chosen)
void prb_set_max_constraints_from_real(
    prb* p, 
    double set_frac,   // the fraction of ->max values to be set in each table
    double set_mult,   // multiply the real value *set_mult to get the max.
    int use_most_popular,   //if true, then most populated cells are set first
    gsl_rng* rng);

typedef enum 
    {CONSTR_MIN, CONSTR_MAX, CONSTR_REAL, CONSTR_PENALTY} ConstraintType ;
//
void prb_print_stats_constraints(
    const prb* p, ConstraintType ct, const char* prefix);

void prb_print_real2max_stats(const prb* p, const char* prefix);

void prb_set_constraints_off(prb* p);   // for debugging


void save2csv_dc_sday(const prb* p, const char* csv_path);
void save2csv_carrier_dc_sday(const prb* p, const char* csv_path);


#endif      // #ifndef PRB__H
