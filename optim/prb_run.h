#ifndef PRB_RUN__H
#define PRB_RUN__H

#include "prb.h"

typedef struct {
    double v0;
    double v1;
    double power;
} schedule_item;

typedef struct {
    schedule_item rel_temp;
    schedule_item penalty;
    schedule_item stepfunc_par;
} schedule;


prb prb_generate(
    long seed, int ndc, int ncarriers, int nroutes, int nrows, int ndays,
    double max_constraints_to_total, 
    double cost_per_truck_mult, int linehaul_flag);

void run_session(
    prb* p, const schedule sch, int nstep, int nrep, 
    const char* session_name, gsl_rng* rng, double max_exp);

// For experimentation (typically  mult= 0|1)
void prb_set_var_cost_mult(prb* p, int mult);
void prb_set_linehaul_cost_mult(prb* p, int mult);
//
void prb_set_penalty_mult_dc_sday(prb* p, int mult);
void prb_set_penalty_mult_carrier_dc_sday(prb* p, int mult);

#endif // #define PRB_RUN__H
