#ifndef FUNCS__H
#define FUNCS__H

int day(int iday, int ndays);

//double step_func_default(double x, double par);
//double step_func_pseudo(double x, double power);
double step_func_universal(double x, double par);

// compute indices of sorted values
void sort_indices(const int* v, int* idx, int n);

// Calculates quantiles for non-negative values only 
//    (the negatives are judged as undefined)
// Returns also the fraction of the non-negative values
double calc_quantiles_nonneg(
    const int* a, int n, const double* q, int nq, double* quantiles);

long data_sum(const int* data, int n, double* undef_frac);

#endif // FUNCS__H
