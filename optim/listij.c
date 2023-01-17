#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mmem.h"
#include "listij.h"

//****************************************************
//    List item:
//    (we use malloc/free intentionally, not mm* functions
//    because mmgetref() makes no sense and potentiall dangerous)
//****************************************************

struct struct_listitem; //???

struct struct_listitem
{
    struct struct_listitem* next_item;
    int i;
    int j;
} ;
typedef struct struct_listitem* listitem;

// Main listitem functions:
listitem listitem_getnew(int i, int j);
listitem listitem_release(listitem item); // returns the next "listitem" 
void listitem_append_to(listitem item_prev, listitem item);
void listitem_process(listitem item, void (*f)(int i, int j));

//=======================================
listitem listitem_getnew(int i, int j)
//=======================================
{
    listitem item = (listitem)malloc(sizeof(struct struct_listitem));
    assert(item != NULL);

    item->i = i;
    item->j = j;
    item->next_item = NULL;

    return item;
}

//========================================
listitem listitem_release(listitem item)
//========================================
{
    assert(item != NULL); //???
    listitem next = item->next_item;
    free(item);
    return next;
}

//========================================
void listitem_append_to(listitem item_prev, listitem item)
//========================================
{
    assert(item_prev != NULL);
    assert(item_prev->next_item == NULL);  // we append to the last item
    assert(item != NULL);
    assert(item->next_item == NULL);

    item_prev->next_item = item;
}

//===============================================
void listitem_process(listitem item, void (*f)(int i, int j))
//===============================================
{
    assert(item != NULL);
    assert(f != NULL);

    (*f)(item->i, item->j);
}

//****************************************************
//    Listij:
//****************************************************

struct struct_listij
{
    int nitems; // the items counter
    listitem first_item;
    listitem last_item;
};

//=======================
listij listij_getnew()
//=======================
{
    listij retv = (listij)mmgetnew(sizeof(struct struct_listij));
    assert(retv != NULL);

    retv->nitems = 0;
    retv->first_item = NULL;
    retv->last_item = NULL;

    return retv;
};

//=====================================
int listij_nitems(const listij list)
//=====================================
{
    assert(list != NULL);
    return list->nitems;
}


//================================================
void listij_add_item(listij list, int i, int j)
//================================================
{
    assert(list != NULL);
    assert(list->last_item == NULL || list->last_item->next_item == NULL);

    listitem new_item = listitem_getnew(i, j);

    if(list->nitems != 0)  // the list is not empty
    {
        assert(list->first_item != NULL);
        assert(list->last_item != NULL);
        listitem_append_to(list->last_item, new_item);
    }
    else  // an empty list, 
    {
        list->first_item = new_item;
        list->last_item = new_item;
    } 

    list->nitems += 1;
    list->last_item = new_item;
    assert(list->last_item->next_item == NULL); // the item is the last one
};


//================================
void listij_process(
    listij list, 
    void (*func)(int i, int j, void* par, int item_idx), 
    void* par) 
//================================
{
    assert(list != NULL);
    listitem item = list->first_item;
    int item_idx = 0;
    while(item != NULL)
    {
        (*func)(item->i, item->j, par, item_idx);
        item = item->next_item;
        item_idx += 1;
    };

}

//===================================
void listij_clear(listij list)
//===================================
{
    assert(list != NULL);

    // Free all items:
    while(list->first_item != NULL)
    {
        assert(list->nitems > 0);
        assert(list->last_item != NULL);

        listitem next_item = listitem_release(list->first_item);
        list->first_item = next_item;
        list->nitems -= 1;
    }

    assert(list->first_item == NULL);
    assert(list->nitems == 0);

    list->last_item = NULL;
}

//==================================
void listij_release(listij list)
//==================================
{
    assert(list != NULL);

    if(mmrefcount(list) == 1)  // the last reference
        listij_clear(list);

    mmrelease(list);

};


void listij_print(const listij list, const char* text)
{
    assert(list != NULL);
    printf(" ==> listij: %s   nitems= %d\n", text, list->nitems);

    int count = 0;
    listitem item = list->first_item;
    while(item != NULL)
    {
        printf("  -> item[%d]   i= %d  j=%d\n", count, item->i, item->j);
        item = item->next_item;
        count += 1;
    };
};


static void func_print(int i, int j, void* par, int item_idx)
{
    const char* text = (const char*) par;
    printf("  -> item:   i= %d  j=%d  [idx= %d] -> %s\n", i, j, item_idx, text);
};

void listij_print2(const listij list, const char* text) // via listij_process
{
    assert(list != NULL);
    printf(" ==> print2: listij: %s   nitems= %d\n", text, list->nitems);

    listij_process(list, func_print, (void*)text);
};


//*************************************************************************
//            Testing functions
//*************************************************************************
//
//
////==========================
//void test__listij(int n)
////==========================
//{
//    listij list1 = listij_getnew();
//
//    listij_print(list1, "After getnew:");
//
//    for(int i=0; i<n; i++)
//    {
//        listij_add_item(list1, i+1, (i+1)*10);         
//        listij_print(list1, "After add_item:");
//        printf("\n");
//        char buff[128];
//        snprintf(buff, 127, "After add item %d", i+1);
//        listij_print2(list1, buff); // via listij_process
//        printf("\n");
//    }
//
//    listij list2 = (listij)mmgetref(list1);
//    listij_release(list1);
//    list1 = NULL;
//
//    listij_print(list2, "list2=list1 after release list1:");
//
//    listij_clear(list2);
//    listij_print(list2, "list2 after clear:");
//
//    listij_release(list2);
//};
//
//int main()
//{
//    int n = 5;
//    test__listij(n);
//};
