#include <stdio.h>
#include <assert.h>

#include <gsl/gsl_sort.h>
#include <gsl/gsl_statistics_double.h>


void calc_quantiles(double* data, int n, const double* q, int nq, double* quantiles)
{
    // "quantiles" - at least double[nq]
    assert(n >= 1);
    assert(nq >= 1);

    gsl_sort (data, 1, n);

    for(int i=0; i<nq; i++)
        quantiles[i] = gsl_stats_quantile_from_sorted_data (data, 1, n, q[i]);

    return;
}
