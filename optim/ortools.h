#ifndef ORTOOLS__H
#define ORTOOLS__H

#include "prb.h" 
#include "arrays.h" 

void ort_mk_header(FILE* f, const char* indent);

void ort_mk_binvars(FILE* f, const prb* p, const char* indent);

void ort_mk_constr_rowsum(FILE* f, const prb* p, const char* indent);

void ort_mk_constr_objective(FILE* f, const prb* p, const char* indent);

void ort_mk_constr_dc_sday(FILE* f, const prb* p, const char* indent);
void ort_mk_constr_carrier_dc_sday(FILE* f, const prb* p, const char* indent);

void ort_solution(FILE* f, const prb* p);

#endif  // #ifndef ORTOOLS__H
