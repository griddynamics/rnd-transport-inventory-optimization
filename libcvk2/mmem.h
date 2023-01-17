#ifndef __mmem__h 
#define  __mmem__h 1

#include <stdlib.h>   // uint4
#include "gendefs.h"  

// major:
void* mmgetnew(uint4 size);
void* mmgetnew2(uint4 size, void (*release_fields)(void* ptr));
void* mmgetref(const void* mptr);
void mmrelease(void* mptr);
uint4 mmrefcount(const void* mptr);

// housekeeping:

void mmrelease_(void** mptr);  // sets *mtpr to NULL

void* mmgetnew_copy(const void* mptr, uint4 size);
char* mmgetnew_str(const char* str); // mm-variant of strdup

// debugging: print malloc-ed pointers to file (or stderr)
//#define __MM_DEBUG

void mmdebug_open_file(const char* path);
void mmdebug_close_file();
void mmdebug_mark(const char* mark);
void mmdebug_clear();

#endif 
