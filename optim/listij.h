#ifndef LISTIJ__H 
#define LISTIJ__H 

typedef struct struct_listij* listij;

listij listij_getnew();
void listij_clear(listij list);  // release all items
void listij_release(listij list);

int listij_nitems(const listij list);
void listij_add_item(listij list, int i, int j);

void listij_process(
    listij list, 
    void (*func)(int i, int j, void* par, int item_idx), 
    void* par); 

void listij_print(const listij list, const char* text); // for debugging

#endif // #ifndef LISTIJ__H 
