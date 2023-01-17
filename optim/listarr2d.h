//  2D and 3D arrays of lists of (i,j) 
#ifndef _LISTARR2D_ 
#define _LISTARR2D_

//  2D variant:
typedef struct struct_larr2d* larr2d;

larr2d larr2d_getnew(int size1, int size2);
void larr2d_release(larr2d la);

int larr2d_nitems_kl(const larr2d la, int k, int l);

void larr2d_add_kl(larr2d la, int k, int l,  int i, int j);

void larr2d_process_kl(
    larr2d la, int k, int l, 
    void (*f)(int i, int j, void* par, int item_idx), void* par);

void larr2d_process(
    larr2d la, 
    void (*f)(int i, int j, void* par, int item_idx), 
    void* par);

#endif // #ifndef _LISTARR2D_
