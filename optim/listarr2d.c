#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mmem.h"
#include "listij.h"
#include "listarr2d.h"

struct struct_larr2d
{
    int size1;
    int size2;
    listij* data; 
};

//=============================================
larr2d larr2d_getnew(int size1, int size2)
//=============================================
{
    larr2d obj = (larr2d)mmgetnew(sizeof(struct struct_larr2d));
    assert(obj != NULL);

    obj->data = (listij*)malloc(sizeof(listij) * size1 * size2);
    assert(obj->data != NULL);
    obj->size1 = size1;
    obj->size2 = size2;
 
    // Fill the array by empty listij objects
    for(int k=0; k<size1; k++)
        for(int l=0; l<size2; l++)
        {
            int kl =  k * obj->size2 + l;
            obj->data[kl] = listij_getnew();
        };


    return obj;
};


//===============================
void larr2d_release(larr2d la)
//===============================
{
    assert(la != NULL);

    if(mmrefcount(la) == 1)   // release all lists and free(la->data)
    {
        // Release all listij objects:
        for(int k=0; k<la->size1; k++)
            for(int l=0; l<la->size2; l++)
            {
                int kl =  k * la->size2 + l;
                listij_release(la->data[kl]);
            };

        // free the "data" field
        free(la->data);
    };
    mmrelease(la);
}


//=============================================
int larr2d_nitems_kl(const larr2d la, int k, int l)
//=============================================
{
    assert(k>=0 && k<la->size1);
    assert(l>=0 && l<la->size2);

    int kl =  k * la->size2 + l;
    return listij_nitems(la->data[kl]);
};


//==========================================================
void larr2d_add_kl(larr2d la, int k, int l,  int i, int j)
//==========================================================
{
    assert(k>=0 && k<la->size1);
    assert(l>=0 && l<la->size2);

    int kl =  k * la->size2 + l;
    listij_add_item(la->data[kl], i, j);
};



//==========================================================================
void larr2d_process_kl(
    larr2d la, int k, int l, 
    void (*f)(int i, int j, void* par, int item_idx), void* par)
//==========================================================================
{
    assert(la != NULL);
    assert(k>=0 && k<la->size1);
    assert(l>=0 && l<la->size2);

    int kl =  k * la->size2 + l;
    listij list_kl = la->data[kl];
    assert(list_kl != NULL);
    listij_process(list_kl, f, par);
};

//======================================================================
void larr2d_process(
    larr2d la, 
    void (*f)(int i, int j, void* par, int item_idx), 
    void* par)
//======================================================================
{
    assert(la != NULL);

    // Fill the array by empty listij objects
    for(int k=0; k<la->size1; k++)
        for(int l=0; l<la->size2; l++)
        {
            int kl =  k * la->size2 + l;
            listij list_kl = la->data[kl];
            assert(list_kl != NULL);
            listij_process(list_kl, f, par);
        };

};


//*************************************************************************
//     Test code
//*************************************************************************

//void func_print(int i, int j, void* par, int item_idx)
//{
//    char* text = (char*)par;
//    printf("   i,j= %d %d  [idx= %d]  => %s\n", i, j, item_idx, text);
//};
//
//
////========================================
//void test__larr2d(int size1, int size2)
////========================================
//{
//
//    larr2d la = larr2d_getnew(size1, size2);
//
//    printf("\n");
//    for(int k=0; k<size1; k++)
//        for(int l=0; l<size2; l++)
//        {
//            int nitems_kl = larr2d_nitems_kl(la, k, l);
//            printf(
//                "    ==> cell k,l= %d %d  nitems= %d (After init)\n",
//                k, l, nitems_kl);
//            larr2d_process_kl(la, k, l, func_print, (void*)"");
//        };
//
//    printf("\n");
//
//    int nrows = size1 * 3;
//    int ncols = size2 * 2;
//    for(int i=0; i<nrows; i++)
//        for(int j=0; j<ncols; j++)
//        {
//            int k = i % size1;
//            int l = j % size2;
//            larr2d_add_kl(la, k, l,  i, j);
//        }
//
//    printf("\n");
//    for(int k=0; k<size1; k++)
//        for(int l=0; l<size2; l++)
//        {
//            int nitems_kl = larr2d_nitems_kl(la, k, l);
//            printf(
//                "    ==> cell k,l= %d %d  nitems= %d\n",
//                k, l, nitems_kl);
//            larr2d_process_kl(la, k, l, func_print, (void*)" xxx");
//        };
//
//    printf("\n");
//
//    larr2d_release(la);
//};
//
//int main()
//{
//    int size1 = 4;
//    int size2 = 2;
//    test__larr2d(size1, size2);
//}
