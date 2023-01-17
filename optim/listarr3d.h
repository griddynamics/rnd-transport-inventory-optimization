//  3D array of lists of (i,j) 
#ifndef _LISTARR3D_ 
#define _LISTARR3D_

typedef struct struct_larr3d* larr3d;

larr3d larr3d_getnew(int size1, int size2, int size3);
void larr3d_release(larr3d la);

int larr3d_nitems_klm(const larr3d la, int k, int l, int m);

void larr3d_add_klm(larr3d la, int k, int l, int m, int i, int j);

void larr3d_process_klm(
    larr3d la, 
    int k, int l, int m, 
    void (*f)(int i, int j, void* par, int item_idx), 
    void* par);

void larr3d_process(
    larr3d la, 
    void (*f)(int i, int j, void* par, int item_idx), 
    void* par);

#endif // #ifndef _LISTIJARR_ 
