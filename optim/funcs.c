#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>  // pow

#include "quantile.h"  // libcvk

int day(int iday, int ndays){
    int d = iday % ndays;
    if(d<0) d += ndays;
    return d;
}

//double step_func_default(double x, double par){
//    assert(x > 0.0 && x < 1.0);
//    par = par; // a trick to avoid warnings
//    return 1.0;
//};
//
//double step_func_pseudo(double x, double power){
//    assert(x > 0.0 && x < 1.0);
//    return pow(x, power);
//};
//
double step_func_universal(double x, double par){
    assert(x > 0.0 && x < 1.0);
    if(par > 0.001) 
        return pow(x, par);
    else
        return 1.0;
};

////  the variant with "double"
//typedef struct {
//    double val;
//    int idx;
//} validx;
//
//static int validx_compar(const void* v1_, const void* v2_)
//{
//    validx* v1 = (validx*)v1_;
//    validx* v2 = (validx*)v2_;
//
//    if(v1->val < v2->val)
//        return -1;
//
//    if(v1->val > v2->val)
//        return 1;
//
//    if(v1->val == v2->val)
//        return 0;
//};
//
//void sort_indices(const double* v, int* idx, int n)
//{
//    validx* val_idx = (validx*)malloc(sizeof(validx) * n);
//    assert(val_idx != NULL);
//
//    int i;
//    for(i=0; i<n; i++)
//    {
//        val_idx[i].val = v[i];
//        val_idx[i].idx = i;
//    }
//
//    qsort((void*)val_idx, n, sizeof(validx), *validx_compar);
//
//    for(i=0; i<n; i++)
//        idx[i] = val_idx[i].idx;
//    free(val_idx);
//};
//


//  the variant with "int"
typedef struct {
    int val;
    int idx;
} validx;

static int validx_compar(const void* v1_, const void* v2_)
{
    validx* v1 = (validx*)v1_;
    validx* v2 = (validx*)v2_;

    if(v1->val < v2->val)
        return -1;

    if(v1->val > v2->val)
        return 1;

    return 0;
};

void sort_indices(const int* v, int* idx, int n)
{
    validx* val_idx = (validx*)malloc(sizeof(validx) * n);
    assert(val_idx != NULL);

    int i;
    for(i=0; i<n; i++)
    {
        val_idx[i].val = v[i];
        val_idx[i].idx = i;
    }

    qsort((void*)val_idx, n, sizeof(validx), *validx_compar);

    for(i=0; i<n; i++)
        idx[i] = val_idx[i].idx;
    free(val_idx);
};


// Calculates quantiles for non-negative values only 
//    (the negatives are judged as undefined)
// Returns also the fraction of the non-negative values
//
double calc_quantiles_nonneg(
    const int* a, int n, const double* q, int nq, double* quantiles)
{

    double* data = (double*)malloc(n * sizeof(double));
    assert(data != NULL);

    int count = 0;
    for(int i=0; i<n; i++)
    {
        if(a[i] >= 0) {
            data[count] = (double)a[i];
            count += 1; 
        }; 
    };

    if(count > 0)
        calc_quantiles(data, count, q, nq, quantiles);

    free(data);
    return (double)count / n;
};

long data_sum(const int* data, int n, double* undef_frac)
{
    long s = 0;
    long undef_count = 0;
    for(int i=0; i<n; i++)
        if(data[i] >= 0)
            s += data[i];
        else
            undef_count += 1;

    if(undef_frac != NULL)
        *undef_frac = (double)undef_count / n;

    return s;
};




//******************* test code *******************

//int main()
//{
//    int n = 7;
//    int v[7];
//    int idx[7];
//
//    v[0] = 3;
//    v[1] = 4;
//    v[2] = 5;
//    v[3] = 7;
//    v[4] = 6;
//    v[5] = 1;
//    v[6] = 2;
//
//    printf(" Orig: ");
//    for(int i=0; i<n; i++)
//        printf("    %d", v[i]);
//    printf("\n");
//
//    for(int i=0; i<n; i++)
//        idx[i] = i;
//    printf("  Idx: ");
//    for(int i=0; i<n; i++)
//        printf("    %d", idx[i]);
//    printf("\n");
//
//    sort_indices(v, idx, n);
//
//    printf("  ====> Afer sort:\n");
//
//    printf(" Orig: ");
//    for(int i=0; i<n; i++)
//        printf("    %d", v[i]);
//    printf("\n");
//
//    printf("  Idx: ");
//    for(int i=0; i<n; i++)
//        printf("    %d", idx[i]);
//    printf("\n");
//
//};
//
//
