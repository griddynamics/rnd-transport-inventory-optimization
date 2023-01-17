#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mmem.h"
#include "listij.h"
#include "listarr3d.h"

struct struct_larr3d
{
    int size1;
    int size2;
    int size3;
    listij* data; 
};


//=============================================
larr3d larr3d_getnew(int size1, int size2, int size3)
//=============================================
{
    larr3d obj = (larr3d)mmgetnew(sizeof(struct struct_larr3d));
    assert(obj != NULL);

    obj->data = (listij*)malloc(sizeof(listij) * size1 * size2 * size3);
    assert(obj->data != NULL);
    obj->size1 = size1;
    obj->size2 = size2;
    obj->size3 = size3;
 
    // Fill the array by empty listij objects
    for(int k=0; k<size1; k++)
        for(int l=0; l<size2; l++)
            for(int m=0; m<size3; m++)
            {
                int klm =  k * obj->size2 * obj->size3 + l * obj->size3 + m;
                obj->data[klm] = listij_getnew();
            };


    return obj;
};


//===============================
void larr3d_release(larr3d la)
//===============================
{
    assert(la != NULL);

    if(mmrefcount(la) == 1)   // release all lists and free(la->data)
    {
        // Release all listij objects:
        for(int k=0; k<la->size1; k++)
            for(int l=0; l<la->size2; l++)
                for(int m=0; m<la->size3; m++)
                {
                    int klm = k * la->size2 * la->size3 + l * la->size3 + m;
                    listij_release(la->data[klm]);
                };

        // free the "data" field
        free(la->data);
    };
    mmrelease(la);
}


//=============================================
int larr3d_nitems_klm(const larr3d la, int k, int l, int m)
//=============================================
{
    assert(k>=0 && k<la->size1);
    assert(l>=0 && l<la->size2);
    assert(m>=0 && m<la->size3);

    int klm =  k * la->size2 * la->size3 + l * la->size3 + m;

    return listij_nitems(la->data[klm]);
};


//==========================================================
void larr3d_add_klm(larr3d la, int k, int l,  int m, int i, int j)
//==========================================================
{
    assert(k>=0 && k<la->size1);
    assert(l>=0 && l<la->size2);
    assert(m>=0 && m<la->size3);

    int klm =  k * la->size2 * la->size3 + l * la->size3 + m;
    listij_add_item(la->data[klm], i, j);
};



//==========================================================================
void larr3d_process_klm(
    larr3d la, 
    int k, int l, int m, 
    void (*f)(int i, int j, void* par, int item_idx), 
    void* par)
//==========================================================================
{
    assert(la != NULL);
    assert(k>=0 && k<la->size1);
    assert(l>=0 && l<la->size2);
    assert(m>=0 && m<la->size3);

    int klm =  k * la->size2 * la->size3 + l * la->size3 + m;

    listij list_klm = la->data[klm];
    assert(list_klm != NULL);
    listij_process(list_klm, f, par);
};

//======================================================================
void larr3d_process(
    larr3d la, 
    void (*f)(int i, int j, void* par, int item_idx), 
    void* par)
//======================================================================
{
    assert(la != NULL);

    // Fill the array by empty listij objects
    for(int k=0; k<la->size1; k++)
        for(int l=0; l<la->size2; l++)
            for(int m=0; m<la->size3; m++)
            {
                int klm =  k * la->size2 * la->size3 + l * la->size3 + m;
                listij list_klm = la->data[klm];
                assert(list_klm != NULL);
                listij_process(list_klm, f, par);
            };

};


//*************************************************************************
//     Test code
//*************************************************************************

//void func_print(int i, int j, void* par)
//{
//    char* text = (char*)par;
//    printf("   i,j= %d %d   => %s\n", i, j, text);
//};
//
//
////========================================
//void test__larr3d(int size1, int size2, int size3)
////========================================
//{
//
//    larr3d la = larr3d_getnew(size1, size2, size3);
//
//    printf("\n");
//    for(int k=0; k<size1; k++)
//        for(int l=0; l<size2; l++)
//            for(int m=0; m<size3; m++)
//            {
//                int nitems_klm = larr3d_nitems_klm(la, k, l, m);
//                printf(
//                    "    ==> cell k,l,m= %d %d %d nitems= %d (After init)\n",
//                    k, l, m, nitems_klm);
//                larr3d_process_klm(la, k, l, m, func_print, (void*)"");
//            };
//
//    printf("\n");
//
//    int nrows = size1 * size2;
//    int ncols = size2 * size3;
//    for(int i=0; i<nrows; i++)
//        for(int j=0; j<ncols; j++)
//        {
//            int k = i % size1;
//            int l = j % size2;
//            int m = j % size3;
//            larr3d_add_klm(la, k, l, m,  i, j);
//        }
//
//    printf("\n");
//    for(int k=0; k<size1; k++)
//        for(int l=0; l<size2; l++)
//            for(int m=0; m<size3; m++)
//            {
//                int nitems_klm = larr3d_nitems_klm(la, k, l, m);
//                printf(
//                    "    ==> cell k,l,m= %d %d %d  nitems= %d\n",
//                    k, l, m, nitems_klm);
//                larr3d_process_klm(la, k, l, m, func_print, (void*)"After");
//            };
//
//    printf("\n");
//
//    larr3d_release(la);
//};
//
//int main()
//{
//    int size1 = 4;
//    int size2 = 2;
//    int size3 = 3;
//    test__larr3d(size1, size2, size3);
//}

