#ifndef ARRAYS__H
#define ARRAYS__H

#define UNDEF_INT -1

typedef struct {
    int size;
    int* data;
} arr1d;

typedef struct {
    int size1;  // e.g. rows
    int size2;  // e.g. columns 
    int* data;
} arr2d;

typedef struct {
    int size1;  // e.g. rows
    int size2;  // e.g. columns 
    int size3;  // e.g. days 
    int* data;
} arr3d;


arr1d arr1d_mknew(int size, int init_value);
void arr1d_memclear(arr1d* a);
int arr1d_getval(const arr1d* a, int i);
void arr1d_setval(arr1d* a, int i, int val);
void arr1d_setall(arr1d* a, int val);
void arr1d_addval(arr1d* a, int i, int val_to_add);
long arr1d_sum(const arr1d* a, double* undef_frac);
double arr1d_calc_quantiles(const arr1d* a, const double* q, int nq, double* quantiles);
int* arr1d_data(const arr1d* a);
int arr1d_datasize(const arr1d* a);
int* arr1d_data(const arr1d* a);
int arr1d_datasize(const arr1d* a);


arr2d arr2d_mknew(int size1, int size2, int init_value);
void arr2d_memclear(arr2d* a);
int arr2d_getval(const arr2d* aa, int i, int j);
void arr2d_setval(arr2d* aa, int i, int j, int val);
void arr2d_setall(arr2d* aa, int val);
void arr2d_addval(arr2d* aa, int i, int j, int val_to_add);
long arr2d_sum(const arr2d* aa, double* undef_frac);
double arr2d_calc_quantiles(const arr2d* aa, const double* q, int nq, double* quantiles);
int* arr2d_data(const arr2d* a);
int arr2d_datasize(const arr2d* a);
void arr2d_print_stats(const arr2d* aa, const char* prefix);
void arr2d_print_ratio_stats(
    const arr2d* aa1, const arr2d* aa2, const char* prefix);


arr3d arr3d_mknew(int size1, int size2, int size3, int init_value);
void arr3d_memclear(arr3d* a); 
int arr3d_getval(const arr3d* aaa, int i, int j, int k);
void arr3d_setval(const arr3d* aaa, int i, int j, int k, int val);
void arr3d_setall(arr3d* aaa, int val);
void arr3d_addval(const arr3d* aaa, int i, int j, int k, int val_to_add);
long arr3d_sum(const arr3d* aaa, double* undef_frac);
double arr3d_calc_quantiles(const arr3d* aa, const double* q, int nq, double* quantiles);
int* arr3d_data(const arr3d* a);
int arr3d_datasize(const arr3d* a);
void arr3d_print_stats(const arr3d* aa, const char* prefix);
void arr3d_print_ratio_stats(
    const arr3d* aaa1, const arr3d* aaa2, const char* prefix);


#endif // ARRAYS__H
