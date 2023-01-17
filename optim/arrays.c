// vk: works with tests for 1, 2, 3D arrays:
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "quantile.h" // in ../libcvk
#include "arrays.h"
#include "funcs.h"

static void print_data_stats(
    const int* data, int n, const char* prefix);
static void print_ratio_stats(
    const int* v1, const int* v2, int n, const char* prefix);


arr1d arr1d_mknew(int size, int init_value){
    assert(size > 0);
    arr1d a;

    a.data = (int*)malloc(size * sizeof(int)); 
    assert(a.data != NULL);
    a.size = size;

    int i;
    for(i=0; i<size; i++){
        a.data[i] = init_value;
    }

    return a;
}

void arr1d_memclear(arr1d* a) 
{
    assert(a->data != NULL);
    free(a->data);
    a->data = NULL;
}

int arr1d_getval(const arr1d* a, int i){
    // vk_remove
    if( ! (i>=0 && i<a->size) )
    {
        printf(" assert: i= %d  a->size= %d\n", i, a->size); 
        int aa = 0;
        int bb = 1/aa;
        bb = bb;
    };

    assert(i>=0 && i<a->size);
    return a->data[i];
};

void arr1d_setval(arr1d* a, int i, int val){
    assert(i>=0 && i<a->size);
    a->data[i] = val;
    return;
};

void arr1d_setall(arr1d* a, int val){
    for(int i; i<a->size; i++)
        a->data[i] = val;
}

void arr1d_addval(arr1d* a, int i, int val_to_add){
    assert(i>=0 && i<a->size);
    assert(a->data[i] >= 0);  
    a->data[i] += val_to_add;
    return;
};

long arr1d_sum(const arr1d* a, double* undef_frac)
{
    int* data = a->data;
    int n = a->size;
    return data_sum(data, n, undef_frac);
};


double arr1d_calc_quantiles(const arr1d* a, const double* q, int nq, double* quantiles)
{
    int n = a->size;
    return calc_quantiles_nonneg(a->data, n, q, nq, quantiles);
};

int* arr1d_data(const arr1d* a)
{
    return a->data;
}

int arr1d_datasize(const arr1d* a)
{
    return a->size;
}

arr2d arr2d_mknew(int size1, int size2, int init_value){
    assert(size1 > 0);
    assert(size2 > 0);

    arr2d aa;
    aa.data = (int*)malloc(size1 * size2 * sizeof(int)); 
    assert(aa.data != NULL);
    aa.size1 = size1;
    aa.size2 = size2;

    int i, j;
    for(i=0; i<size1; i++)
        for(j=0; j<size2; j++){
            int ij = i * size2 + j;
            aa.data[ij] = init_value;
        }
        
    return aa;
}

void arr2d_memclear(arr2d* a) 
{
    assert(a->data != NULL);
    free(a->data);
    a->data = NULL;
}


int arr2d_getval(const arr2d* aa, int i, int j){
    assert(i>=0 && i<aa->size1);
    assert(j>=0 && j<aa->size2);
    int ij = i * aa->size2 + j;
    return aa->data[ij];
};

void arr2d_setval(arr2d* aa, int i, int j, int val){
    assert(i>=0 && i<aa->size1);
    assert(j>=0 && j<aa->size2);

    int ij = i * aa->size2 + j;
    aa->data[ij] = val;
};

void arr2d_setall(arr2d* aa, int val)
{
    for(int i=0; i<aa->size1; i++)
        for(int j=0; j<aa->size2; j++)
        {
            int ij = i * aa->size2 + j;
            aa->data[ij] = val;
        }
}

void arr2d_addval(arr2d* aa, int i, int j, int val_to_add){
    assert(i>=0 && i<aa->size1);
    assert(j>=0 && j<aa->size2);

    int ij = i * aa->size2 + j;
    assert(aa->data[ij] >= 0);  
    aa->data[ij] += val_to_add;
};

long arr2d_sum(const arr2d* aa, double* undef_frac)
{
    int* data = aa->data;
    int n = aa->size1 * aa->size2;
    return data_sum(data, n, undef_frac);
};


double arr2d_calc_quantiles(const arr2d* aa, const double* q, int nq, double* quantiles)
{
    int n = aa->size1 * aa->size2;
    return calc_quantiles_nonneg(aa->data, n, q, nq, quantiles);
};

int* arr2d_data(const arr2d* a)
{
    return a->data;
}

int arr2d_datasize(const arr2d* a)
{
    return a->size1 * a->size2;
}

void arr2d_print_stats(const arr2d* aa, const char* prefix)
{
    printf("%s -> arr2d: %d x %d\n", prefix, aa->size1, aa->size2);

    int* data = arr2d_data(aa);
    int n = arr2d_datasize(aa);
    print_data_stats(data, n, "   ");
};

void arr2d_print_ratio_stats(
    const arr2d* aa1, const arr2d* aa2, const char* prefix)
{
    assert(aa1->size1 == aa2->size1);
    assert(aa1->size2 == aa2->size2);

    printf(
        "%sarr2d: %d x %d (= %d) \n", 
        prefix, aa1->size1, aa1->size2, aa1->size1 * aa1->size2);

    int* data1 = arr2d_data(aa1);
    int* data2 = arr2d_data(aa2);
    int n = arr2d_datasize(aa1);
    print_ratio_stats(data1, data2, n, prefix);
};




arr3d arr3d_mknew(int size1, int size2, int size3, int init_value){
    assert(size1 > 0);
    assert(size2 > 0);
    assert(size3 > 0);

    arr3d aaa;
    aaa.data = (int*)malloc(size1 * size2 * size3 * sizeof(int)); 
    assert(aaa.data != NULL);
    aaa.size1 = size1;
    aaa.size2 = size2;
    aaa.size3 = size3;

    int i, j, k;
    for(i=0; i<size1; i++)
        for(j=0; j<size2; j++)
            for(k=0; k<size3; k++) {
                int ijk = i * size2 * size3 + j * size3 + k;
                aaa.data[ijk] = init_value;
            }
        
    return aaa;
}

void arr3d_memclear(arr3d* a) 
{
    assert(a->data != NULL);
    free(a->data);
    a->data = NULL;
}


int arr3d_getval(const arr3d* aaa, int i, int j, int k){
    assert(i>=0 && i<aaa->size1);
    assert(j>=0 && j<aaa->size2);
    assert(k>=0 && k<aaa->size3);

    int ijk = i * aaa->size2 * aaa->size3 + j * aaa->size3 + k;
    return aaa->data[ijk];
};

void arr3d_setval(const arr3d* aaa, int i, int j, int k, int val){
    assert(i>=0 && i<aaa->size1);
    assert(j>=0 && j<aaa->size2);
    assert(k>=0 && k<aaa->size3);

    int ijk = i * aaa->size2 * aaa->size3 + j * aaa->size3 + k;
    aaa->data[ijk] = val;
};

void arr3d_setall(arr3d* aaa, int val)
{
    for(int i=0; i<aaa->size1; i++)
        for(int j=0; j<aaa->size2; j++)
            for(int k=0; k<aaa->size3; k++)
            {
                int ijk = i * aaa->size2 * aaa->size3 + j * aaa->size3 + k;
                aaa->data[ijk] = val;
            }
}


void arr3d_addval(const arr3d* aaa, int i, int j, int k, int val_to_add){
    assert(i>=0 && i<aaa->size1);
    assert(j>=0 && j<aaa->size2);
    assert(k>=0 && k<aaa->size3);

    int ijk = i * aaa->size2 * aaa->size3 + j * aaa->size3 + k;
    assert(aaa->data[ijk] >= 0);
    aaa->data[ijk] += val_to_add;
};



long arr3d_sum(const arr3d* aaa, double* undef_frac)
{
    int* data = aaa->data;
    int n = aaa->size1 * aaa->size2 * aaa->size3;
    return data_sum(data, n, undef_frac);
};



double arr3d_calc_quantiles(
    const arr3d* aaa, const double* q, int nq, double* quantiles)
{
    int n = aaa->size1 * aaa->size2 * aaa->size3;
    return calc_quantiles_nonneg(aaa->data, n, q, nq, quantiles);
};

int* arr3d_data(const arr3d* a)
{
    return a->data;
}

int arr3d_datasize(const arr3d* a)
{
    return a->size1 * a->size2 * a->size3;
}

void arr3d_print_stats(const arr3d* aaa, const char* prefix)
{
    printf(
        "%s -> arr3d: %d x %d x %d\n", 
        prefix, aaa->size1, aaa->size2, aaa->size3);

    int* data = arr3d_data(aaa);
    int n = arr3d_datasize(aaa);
    print_data_stats(data, n, "   ");
};

void arr3d_print_ratio_stats(
    const arr3d* aaa1, const arr3d* aaa2, const char* prefix)
{
    assert(aaa1->size1 == aaa2->size1);
    assert(aaa1->size2 == aaa2->size2);
    assert(aaa1->size3 == aaa2->size3);

    printf(
        "%sarr3d: %d x %d x %d (= %d) \n", 
        prefix, aaa1->size1, aaa1->size2, aaa1->size3, 
        aaa1->size1 * aaa1->size2 * aaa1->size3);

    int* data1 = arr3d_data(aaa1);
    int* data2 = arr3d_data(aaa2);
    int n = arr3d_datasize(aaa1);
    print_ratio_stats(data1, data2, n, prefix);
};


    
//*******************************************************************
//       Static functions
//*******************************************************************

void print_data_stats(const int* data, int n, const char* prefix)
{
    int nq = 7;
    double qq[7] = {0.01, 0.1, 0.25, 0.5, 0.75, 0.90, 0.99};
    double quantiles[7];

    double frac_defined = calc_quantiles_nonneg(data, n, qq, nq, quantiles);
    printf("%s -> datasize= %d\n", prefix, n); 
    printf("%s -> frac_defined= %.5f\n", prefix, frac_defined); 

    if(frac_defined > 1./n * 0.1)  // at least one element is defined
    {    
        long sum = data_sum(data, n, NULL);
        double avg = sum / (frac_defined * n);
        printf("%s -> sum= %ld\n", prefix, sum); 
        printf("%s -> avg= %.5f\n", prefix, avg); 
        
        printf("%s -> Quantiles: ", prefix);
        for(int i=0; i<nq; i++)
            printf("%.1f  ", quantiles[i]);
        printf("\n");
    }
}

void print_ratio_stats(const int* v1, const int* v2, int n, const char* prefix)
{
    int nq = 7;
    double qq[7] = {0.01, 0.1, 0.25, 0.5, 0.75, 0.90, 0.99};
    double quantiles[7];

    double* ratio = (double*)malloc(sizeof(double) * n); 
    assert(ratio != NULL);

    // Calculate and fill the ratio values
    int count = 0;   // count only defined ratio values
    for(int i=0; i<n; i++)
    {
        int vi1 = v1[i];
        int vi2 = v2[i];
        if(vi1 >= 0 && vi2 > 0)   // both values are defind
        {
            ratio[count] = (double)vi1 / vi2;
            count += 1;
        };

    }; // for(int i=0; i<n; i++)


    double frac_defined = (double) count / n;
    printf("%sdatasize= %d\n", prefix, n); 
    printf("%sfrac_defined= %.5f\n", prefix, frac_defined); 


    if(frac_defined > 1./n * 0.1)  // at least one element is defined
    {    
        // Calculate/print the average ratio, min, max: (do we need it?)
        double sum = 0.;
        double min = ratio[0];
        double max = ratio[0];
        for(int i=0; i<count; i++)
        {
            sum += ratio[i];
            if(ratio[i] < min)
                min = ratio[i];
            if(ratio[i] > max)
                max = ratio[i];
        }

        double avg = sum / count;
        printf("%savg_ratio= %.5f\n", prefix, avg); 
        printf("%smin= %f\n", prefix, min); 
        printf("%smax= %f\n", prefix, max); 
        
        // Calculate/print quantiles of the ratio:
        calc_quantiles(ratio, count, qq, nq, quantiles);
        const char* format = "FORMAT NOT SET";
        if(quantiles[nq/2] < 10)
            format = "%.4f  ";
        else
            format = "%.2f  ";

        printf("%s    QQ: ", prefix);
        for(int i=0; i<nq; i++)
            printf(format, quantiles[i]);
        printf("\n");
    }

    free(ratio);
}

